#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

extern osMessageQId GcodeCommandHandle;

void print_task_loop(void)
{
  osEvent evt = osMessageGet(GcodeCommandHandle, osWaitForever); //从消息对列中读取消息

  if (evt.status == osEventMessage)
  {
#ifdef ENABLE_OS_POOL
    os_message_t *message = (os_message_t *)evt.value.p;
    printf("%d::%s\n", message->cmd_length, message->cmd_str);

    if (message->cmd_type == GCODE_TYPE_FILE)
    {
      process_commands(message->cmd_type, message->file_pos, (char *)message->cmd_str, message->layer_count, message->current_layer); //处理gcode命令
    }
    else
    {
      process_commands(message->cmd_type, 0, (char *)message->cmd_str, 0, 0); //处理gcode命令
    }

    osPoolFree(m_os_pool_id, message);
#else
    char *msg = (char *)evt.value.p;
    int buf_len = msg[GCODE_BUF_OFFSET_GCODE_LENGTH];
    //printf("%d::%d::%s\n", msg[GCODE_BUF_OFFSET_GCODE_LENGTH], 0, &msg[GCODE_BUF_OFFSET_GCODE_STR]);

    if (msg[GCODE_BUF_OFFSET_TYPE] == GCODE_TYPE_FILE)
    {
      uint32_t *file_pos = (uint32_t *) &msg[GCODE_BUF_OFFSET_FILE_POS];
      //printf("%d::%d::%s\n", msg[GCODE_BUF_OFFSET_GCODE_LENGTH], *file_pos, &msg[GCODE_BUF_OFFSET_GCODE_STR]);
      long *layer_count = (long *) &msg[GCODE_BUF_OFFSET_LAYER_COUNT];
      long *current_layer = (long *) &msg[GCODE_BUF_OFFSET_CURRENT_LAYER];
      process_commands(msg[GCODE_BUF_OFFSET_TYPE], *file_pos, (char *)&msg[GCODE_BUF_OFFSET_GCODE_STR], *layer_count, *current_layer); //处理gcode命令
    }
    else if (msg[GCODE_BUF_OFFSET_TYPE] == GCODE_TYPE_USBD)
    {
      if (user_usb_device_is_receive_process((unsigned char *)&msg[GCODE_BUF_OFFSET_GCODE_STR], buf_len))
      {
        process_commands(msg[GCODE_BUF_OFFSET_TYPE], 0, (char *)&msg[GCODE_BUF_OFFSET_GCODE_STR], 0, 0); //处理gcode命令
      }
    }
    else
    {
      process_commands(msg[GCODE_BUF_OFFSET_TYPE], 0, (char *)&msg[GCODE_BUF_OFFSET_GCODE_STR], 0, 0); //处理gcode命令
    }

#endif
    (void)osDelay(10); //延时以让低优先级的任务执行
  }
}

#ifdef __cplusplus
} //extern "C"
#endif








