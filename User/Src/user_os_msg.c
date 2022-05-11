#include "user_common.h"

#ifdef ENABLE_FREERTOS

extern osMessageQId GcodeCommandHandle;
extern osSemaphoreId GUISendSemHandle;
extern osSemaphoreId GUIWaitSemHandle;

void user_os_respond_gui_send_sem(const int sempValue)
{
  ccm_param.t_gui_semp.index_v = sempValue;
  (void)osSemaphoreRelease(GUISendSemHandle);
  (void)osSemaphoreWait(GUIWaitSemHandle, osWaitForever);
  osDelay(100);
}

#ifdef ENABLE_OS_POOL
  osPoolDef(m_pool_t, 32, os_message_t);
  osPoolId m_os_pool_id;
#else
  #define CMD_BUF_SIZE 64
  #define MAX_CMD_BUF_SIZE (MAX_CMD_SIZE+GCODE_BUF_OFFSET_GCODE_STR)
  static char os_put_msg[CMD_BUF_SIZE][MAX_CMD_BUF_SIZE];
  volatile uint8_t os_put_msg_head = 0;

#endif

void user_os_init(void)
{
#ifdef ENABLE_OS_POOL
  m_os_pool_id = osPoolCreate(osPool(m_pool_t));  //创建内存池
#else
  memset(os_put_msg, 0, sizeof(os_put_msg));
  memset(ccm_param.os_gcode_buf, 0, sizeof(ccm_param.os_gcode_buf));
  os_put_msg_head = 0;
#endif
}

static void user_set_cmd_type(uint8_t cmd_type, uint8_t cmd_length)
{
  os_put_msg_head = (os_put_msg_head + 1) % CMD_BUF_SIZE;
  memset(os_put_msg[os_put_msg_head], 0, sizeof(char) * (MAX_CMD_BUF_SIZE));
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_TYPE] = cmd_type;
  os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_LENGTH] = cmd_length;
}

static void user_send_cmd_only(uint8_t cmd_type, uint8_t cmd_length, const char *cmd_buf, uint32_t timeout)
{
  user_set_cmd_type(cmd_type, cmd_length);
  memcpy(&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_STR], cmd_buf, cmd_length);
  osMessagePut(GcodeCommandHandle, (uint32_t)os_put_msg[os_put_msg_head], timeout);
}

void user_send_uart_cmd(const char *msg, const unsigned int msg_length)
{
#ifdef ENABLE_OS_POOL
  os_message_t *message = (os_message_t *)osPoolAlloc(m_os_pool_id);
  message->cmd_type = cmd_str_type;
  message->cmd_length = strlen(msg);
  message->file_pos = 0;
  message->layer_count = 0;
  message->current_layer = 0;
  memcpy(&message->cmd_str[0], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)message, 0);
#else
  user_send_cmd_only(GCODE_TYPE_UART, msg_length, msg, 0);
#endif
}

void user_send_usbd_cmd(const char *msg, const unsigned int msg_length)
{
#ifdef ENABLE_OS_POOL
  os_message_t *message = (os_message_t *)osPoolAlloc(m_os_pool_id);
  message->cmd_type = cmd_str_type;
  message->cmd_length = strlen(msg);
  message->file_pos = 0;
  message->layer_count = 0;
  message->current_layer = 0;
  memcpy(&message->cmd_str[0], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)message, 0);
#else
  user_send_cmd_only(GCODE_TYPE_USBD, msg_length, msg, 0);
#endif
}

void user_send_internal_cmd(const char *msg)
{
#ifdef ENABLE_OS_POOL
  printf("1::%s\n", msg);
  os_message_t *message = (os_message_t *)osPoolAlloc(m_os_pool_id);
  printf("2::%s\n", msg);
  message->cmd_type = GCODE_TYPE_INTERNAL;
  message->cmd_length = strlen(msg);
  message->file_pos = 0;
  message->layer_count = 0;
  message->current_layer = 0;
  memcpy(&message->cmd_str[0], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)message, 0);
#else
  user_send_cmd_only(GCODE_TYPE_INTERNAL, strlen(msg), msg, 0);
#endif
}

void user_send_file_cmd(const char *msg, uint32_t file_pos, long layer_count, long current_layer)
{
#ifdef ENABLE_OS_POOL
  os_message_t *message = (os_message_t *)osPoolAlloc(m_os_pool_id);
  message->cmd_type = GCODE_TYPE_FILE;
  message->cmd_length = strlen(msg);
  message->file_pos = file_pos;
  message->layer_count = layer_count;
  message->current_layer = current_layer;
  memcpy(&message->cmd_str[0], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)message, 0);
#else
  user_set_cmd_type(GCODE_TYPE_FILE, strlen(msg));
  uint32_t *file_pos_tmp = (uint32_t *)&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_FILE_POS];
  *file_pos_tmp = file_pos;
  long *layer_count_tmp = (long *)&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_LAYER_COUNT];
  *layer_count_tmp = layer_count;
  long *current_layer_tmp = (long *)&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_CURRENT_LAYER];
  *current_layer_tmp = current_layer;
  memcpy(&os_put_msg[os_put_msg_head][GCODE_BUF_OFFSET_GCODE_STR], msg, strlen(msg));
  osMessagePut(GcodeCommandHandle, (uint32_t)os_put_msg[os_put_msg_head], 0);
#endif
}

void user_clear_queue_str(void)
{
  osEvent evt = osMessageGet(GcodeCommandHandle, 0);

  while (evt.status == osEventMessage)
  {
    evt = osMessageGet(GcodeCommandHandle, 0);
  }
}

#ifdef PRINT_TASK_LIST

#ifdef configUSE_TRACE_FACILITY
  #undef configUSE_TRACE_FACILITY
  #define configUSE_TRACE_FACILITY 1
#endif

#ifdef configUSE_STATS_FORMATTING_FUNCTIONS
  #undef configUSE_STATS_FORMATTING_FUNCTIONS
  #define configUSE_STATS_FORMATTING_FUNCTIONS 1
#endif

uint8_t pcWriteBuffer[500];
void _user_os_print_task_list(void)
{
  vTaskList((char *)&pcWriteBuffer);
  USER_EchoLogStr("task_name   task_state  priority   stack  tasK_num\r\n");
  USER_EchoLogStr("%s\r\n", pcWriteBuffer);
  //  vTaskDelete(NULL);
}

#endif



#ifdef PRINT_TASK_HIGH_WATER_MARK

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
  #undef INCLUDE_uxTaskGetStackHighWaterMark
  #define INCLUDE_uxTaskGetStackHighWaterMark 1
#endif

extern osThreadId InitTaskHandle;
extern osThreadId RefDataTaskHandle;
extern osThreadId ReadUdiskTaskHandle;
extern osThreadId PrintTaskHandle;
extern osThreadId GUITaskHandle;
extern osThreadId RespondGUITaskHandle;
extern USBH_HandleTypeDef hUsbHostFS;

void _user_os_print_task_high_water_mark(void)
{
  int uxHighWaterMark = uxTaskGetStackHighWaterMark(InitTaskHandle);
  USER_EchoLogStr("\r\nInitTaskHandle remaining space: %d\r\n", uxHighWaterMark);
  uxHighWaterMark = uxTaskGetStackHighWaterMark(RefDataTaskHandle);
  USER_EchoLogStr("\r\nRefDataTaskHandle remaining space: %d\r\n", uxHighWaterMark);
  uxHighWaterMark = uxTaskGetStackHighWaterMark(ReadUdiskTaskHandle);
  USER_EchoLogStr("\r\nReadUdiskTaskHandle remaining space: %d\r\n", uxHighWaterMark);
  uxHighWaterMark = uxTaskGetStackHighWaterMark(PrintTaskHandle);
  USER_EchoLogStr("\r\nPrintTaskHandle remaining space: %d\r\n", uxHighWaterMark);
  uxHighWaterMark = uxTaskGetStackHighWaterMark(GUITaskHandle);
  USER_EchoLogStr("\r\nGUITaskHandle remaining space: %d\r\n", uxHighWaterMark);
  uxHighWaterMark = uxTaskGetStackHighWaterMark(RespondGUITaskHandle);
  USER_EchoLogStr("\r\nRespondGUITaskHandle remaining space: %d\r\n", uxHighWaterMark);
  uxHighWaterMark = uxTaskGetStackHighWaterMark(hUsbHostFS.thread);
  USER_EchoLogStr("\r\nUSBH_Thread remaining space: %d\r\n", uxHighWaterMark);
}

#endif

void user_os_print_task_info(void)
{
  static unsigned long RefreshGuiTimeOut = 0;

  if (RefreshGuiTimeOut < xTaskGetTickCount())
  {
#ifdef PRINT_TASK_LIST
    _user_os_print_task_list();
#endif
#ifdef PRINT_TASK_HIGH_WATER_MARK
    _user_os_print_task_high_water_mark();
#endif
    RefreshGuiTimeOut = xTaskGetTickCount() + 5000;
  }
}


#endif //ENABLE_FREERTOS

