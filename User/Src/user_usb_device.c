#include "user_common.h"
#ifdef ENABLE_GUI_LVGL
  #include "custom.h"
#endif
#ifdef ENABLE_USB_DEVICE

//  #define TRANS_FILE_BUF_SIZE (65536)  //64K
//  volatile uint8_t trans_file_bufs[TRANS_FILE_BUF_SIZE]   __attribute__((at(0XC02F3000)));
#define trans_file_bufs sdram_param.trans_file_bufs

extern osSemaphoreId ReceiveUartCmdHandle; //信号量
extern USBD_HandleTypeDef hUsbDeviceHS;
extern uint8_t UserRxBufferHS[APP_RX_DATA_SIZE];
extern uint32_t byteswritten;
extern FIL MyFile;
extern unsigned long TransFileTimeOut;

volatile uint32_t total_write_byte = 0;
volatile TransFileStatus transFileStatus;
volatile TransFilePrintStatus transFilePrintStatus;

static long usbd_cmd_N, usbd_cmd_LastN;     //wifi
static char *usbd_cmd_strchr_pointer;
///////////////上传文件--中断回调函数usbd_cdc_DataOut 中的处理///////////////////


static void _user_usb_device_reset_variable(void) //变量重置
{
  transFileStatus.IsStartTrans = 0;
  transFileStatus.FileSize = 0;
  transFileStatus.ReceivedFileSize = 0;
  transFileStatus.ReceivedDataSizeInBuf = 0;
}

static void _user_usb_device_receive_m28(const char *Buf) //M28 test.gcode S1024
{
  char *StrHeadPos;
  char *StrEndPos;
  //获取文件名
  StrHeadPos = (char *)&Buf[4];
  StrEndPos = strchr(StrHeadPos, ' ');
  *StrEndPos = 0;
#ifdef ENABLE_GUI_LVGL
  //  (void)utils_str_memmove((TCHAR *)transFileStatus.FileNameStr, USERPath);
  //  utils_str_strcat((TCHAR *)transFileStatus.FileNameStr, (TCHAR *)L"/temp.gcode");
  uint8_t path_name[20];
  int path_name_length = 0;
  custom_unicode_to_utf8(USERPath, 0, path_name, &path_name_length);
  path_name[path_name_length] = 0;
  utils_str_snprintf_path((TCHAR *)transFileStatus.FileNameStr, path_name, (uint8_t *)StrHeadPos); //获取文件名，并添加上SD卡路径  char FileNameStr[40]//长度不能超
#else
  (void)snprintf((char *)transFileStatus.FileNameStr, sizeof(transFileStatus.FileNameStr), "1:/%s", StrHeadPos); //获取文件名，并添加上SD卡路径  char FileNameStr[40]//长度不能超
#endif
  transFileStatus.IsTaskCritical = 1;
  //获取文件大小
  StrHeadPos = StrEndPos + 2; //指向文件大小数值的开头
  StrEndPos = strchr(StrHeadPos, ' ');
  *StrEndPos = 0;
  total_write_byte = 0;
  transFileStatus.FileSize = (unsigned long)strtol(StrHeadPos, NULL, 10); //获取文件大小  unsigned long FileSize//大小不能超 32位
  USER_EchoLog("Virtual serial port ==>> Tran file size:%ld!\n", transFileStatus.FileSize);
  // 初始化接收数组
  transFileStatus.ReceivedDataSizeInBuf = 0;
  memset((void *)trans_file_bufs, 0, sizeof(trans_file_bufs));
  transFileStatus.IsStartTrans = 1; //开始传输
}

bool _user_usb_device_receive_cmd(const char *Buf, const uint32_t Len)
{
  bool is_process_cmd = true;
  _user_usb_device_reset_variable();

  if (strstr((char *)&Buf[0], "M35")) transFileStatus.IsM35Order = 1; //命令格式：M35
  else if (strstr((char *)&Buf[0], "G28")) transFileStatus.IsG28Order = 1;//G28归零命令
  else if (strstr((char *)&Buf[0], "M25")) transFileStatus.IsM25Order = 1;//M25暂停打印
  else if (strstr((char *)&Buf[0], "M24")) transFileStatus.IsM24Order = 1;//M24继续打印
  else if (strstr((char *)&Buf[0], "M33")) transFileStatus.IsM33Order = 1;//M33停止打印
  else if (strstr((char *)&Buf[0], "M700")) transFileStatus.IsM700Order = 1;//M700获取USB联机是否已完成打印
  else if (strstr((char *)&Buf[0], "M105")) transFileStatus.IsM105Order = 1; //M105命令
  else if (strstr((char *)&Buf[0], "M34"))
  {
    (void)strcpy((char *)transFileStatus.ModelStr, (char *)&Buf[4]); //命令格式：M34 M14  //M14为机型名称
    transFileStatus.IsM34Order = 1; //M34命令
  }
  else if (strstr((char *)&Buf[0], "M28")) //没有传输文件内容时，检测是否接收到M28命令
  {
    _user_usb_device_receive_m28(Buf);//命令格式：M28 test.gcode S1024 M14
    transFileStatus.IsM28Order = 1; //是M28命令
  }
  else if (strstr((char *)&Buf[0], "G1")) //G1移动命令
  {
    (void)strcpy((char *)transFileStatus.G1OrderBuf, (char *)Buf);
    transFileStatus.IsG1Order = 1; //命令格式：G1 F150 X100 Y10 Z15----就是GCODE命令
  }
  else
  {
    is_process_cmd = false;
  }

  transFileStatus.IsCmd = 1; //缓冲区数据清零
  (void)osSemaphoreRelease(ReceiveUartCmdHandle);  //跳转到任务处理
  return is_process_cmd;
}

void _user_usb_device_receive_file(uint8_t *Buf, uint32_t *Len)
{
  if (transFileStatus.ReceivedDataSizeInBuf == 0)
  {
    memmove((char *)&trans_file_bufs[transFileStatus.ReceivedDataSizeInBuf], Buf, *Len);
  }

  transFileStatus.ReceivedDataSizeInBuf += *Len;

  if (transFileStatus.ReceivedFileSize + transFileStatus.ReceivedDataSizeInBuf == transFileStatus.FileSize) //文件是否接收完成
  {
    USER_EchoLog("Virtual serial port ==>> Rec file sizes: %ld, file sizes: %ld\n", transFileStatus.ReceivedFileSize + transFileStatus.ReceivedDataSizeInBuf, transFileStatus.FileSize);
    _user_usb_device_reset_variable(); //变量重置
    transFileStatus.IsEndTrans = 1;
    (void)osSemaphoreRelease(ReceiveUartCmdHandle);  //跳转到任务处理
  }
  else if (transFileStatus.ReceivedDataSizeInBuf >= TRANS_FILE_BUF_SIZE - CDC_DATA_FS_MAX_PACKET_SIZE)  //缓冲区满
  {
    transFileStatus.ReceivedFileSize += transFileStatus.ReceivedDataSizeInBuf;
#if 1
    USER_EchoLog("Virtual serial port ==>> ReceivedDataSizeInBuf: %d\n",
                 transFileStatus.ReceivedDataSizeInBuf);
    USER_EchoLog("Virtual serial port ==>> Rec file sizes: %ld, file sizes: %ld\n",
                 transFileStatus.ReceivedFileSize + transFileStatus.ReceivedDataSizeInBuf,
                 transFileStatus.FileSize);
#endif
    (void)osSemaphoreRelease(ReceiveUartCmdHandle);  //跳转到任务处理
  }
  else //缓冲区没有满，接收下一个包
  {
    USBD_CDC_SetRxBuffer(&hUsbDeviceHS, (uint8_t *)&trans_file_bufs[transFileStatus.ReceivedDataSizeInBuf]);
    USBD_CDC_ReceivePacket(&hUsbDeviceHS);
  }
}

void user_usb_device_receive(uint8_t *Buf, uint32_t *Len)
{
  if (1 == transFileStatus.IsStartTrans) //是否已开始传输文件内容
  {
    _user_usb_device_receive_file(Buf, Len);
  }
  else
  {
    USER_EchoLog("Virtual serial port ==>> Receive: %s\n", (char *)Buf);

    if (!_user_usb_device_receive_cmd((char *)Buf, *Len))
    {
      user_send_usbd_cmd((char *)Buf, *Len);
    }
  }
}

void user_usb_device_transmit(const char *Buf)
{
  CDC_Transmit_HS((uint8_t *)Buf, strlen(Buf));
  USER_EchoLog("Virtual serial port ==>> Transmit: %s\n", Buf);
}

static void _user_usb_device_flush_serial_request_resend()
{
  USER_SEND_USBD_CMD_BUF("Resend: ");
  USER_SEND_USBD_CMD_BUF("%ld\n\r", usbd_cmd_LastN + 1);
  USER_SEND_USBD_CMD_BUF("ok\n\r");
}

static bool _user_usb_device_receive_ncode(unsigned char *data)
{
  usbd_cmd_strchr_pointer = strchr((char *)data, 'N');
  usbd_cmd_N = (strtol((const char *)usbd_cmd_strchr_pointer, NULL, 10));

  if (usbd_cmd_N != usbd_cmd_LastN + 1 && (strstr((char *)data, (char *)"M110") == NULL))
  {
    USER_SEND_USBD_CMD_BUF("Error:");
    USER_SEND_USBD_CMD_BUF("Line Number is not Last Line Number+1, Last Line: ");
    USER_SEND_USBD_CMD_BUF("%ld\n\r", usbd_cmd_LastN);
    _user_usb_device_flush_serial_request_resend();
    return false;
  }

  if (strchr((const char *)data, '*') != NULL)
  {
    uint8_t checksum = 0;
    uint8_t count = 0;

    while (data[count] != '*')
    {
      checksum = checksum ^ data[count++];
    }

    usbd_cmd_strchr_pointer = strchr((char *)data, '*');

    if ((uint8_t)(strtod((const char *)usbd_cmd_strchr_pointer, NULL)) != checksum)
    {
      USER_SEND_USBD_CMD_BUF("Error:");
      USER_SEND_USBD_CMD_BUF("checksum mismatch, Last Line: ");
      USER_SEND_USBD_CMD_BUF(" checksum: %d\n\r", checksum);
      count = 0;
      USER_SEND_USBD_CMD_BUF(" '");

      while (data[count] != '*')
      {
        USER_SEND_USBD_CMD_BUF("%c", data[count++]);
      }

      USER_SEND_USBD_CMD_BUF(" '\n\r ");
      checksum = 0;
      count = 0;

      while (data[count] != '*')
      {
        USER_SEND_USBD_CMD_BUF("ccm_param.command_buffer:%d;", data[count]);
        checksum = checksum ^ data[count++];
        USER_SEND_USBD_CMD_BUF(" checksum:%d \n\r", checksum);
      }

      /// USER_EchoLogStr("\n\r ");
      USER_SEND_USBD_CMD_BUF("%ld\n\r", usbd_cmd_LastN);
      _user_usb_device_flush_serial_request_resend();
      return false;
    }

    //if no errors, continue parsing
  }
  else
  {
    USER_SEND_USBD_CMD_BUF("Error:");
    USER_SEND_USBD_CMD_BUF("No Checksum with line number, Last Line: ");
    USER_SEND_USBD_CMD_BUF("%ld\n\r", usbd_cmd_LastN);
    _user_usb_device_flush_serial_request_resend();
    return false;
  }

  usbd_cmd_LastN = usbd_cmd_N;
  //if no errors, continue parsing
  return true;
}

bool user_usb_device_is_receive_process(unsigned char *data, unsigned short length)
{
  if (length > 0)
  {
    //    USER_SEND_USBD_CMD_BUF("Rec:%s;len:%d\n", data, length);
    if (strchr((const char *)data, 'N') != NULL)
    {
      return _user_usb_device_receive_ncode(data);
    }
    else  // if we don't receive 'N' but still see '*'
    {
      if ((strchr((char *)data, '*') != NULL))
      {
        USER_SEND_USBD_CMD_BUF("Error:");
        USER_SEND_USBD_CMD_BUF("No Line Number with checksum, Last Line: ");
        USER_SEND_USBD_CMD_BUF("%ld\n\r", usbd_cmd_LastN);
        return false;
      }
    }
  }
  else
  {
    return false;
  }

  return true;
}

void user_usb_device_write_file_buf(void)
{
  if (transFileStatus.IsTaskCritical)
  {
    taskENTER_CRITICAL();
  }

  if (transFileStatus.ReceivedDataSizeInBuf > 0)
  {
    if (f_open(&MyFile, (TCHAR *)transFileStatus.FileNameStr,  FA_WRITE) == FR_OK)
    {
      f_lseek(&MyFile, total_write_byte);
      f_write(&MyFile, (char *)trans_file_bufs, transFileStatus.ReceivedDataSizeInBuf, (unsigned int *)&byteswritten);
      f_close(&MyFile); //关闭文件
      total_write_byte += byteswritten;
      transFileStatus.ReceivedDataSizeInBuf = 0; //缓冲区数据清零
      memset((void *)trans_file_bufs, 0, sizeof(trans_file_bufs));
    }
  }

  if (transFileStatus.IsTaskCritical)
  {
    taskEXIT_CRITICAL();
  }
}

void user_usb_device_receive_prepare_file(void)
{
  transFileStatus.ReceivedDataSizeInBuf = 0; //缓冲区数据清零
  memset((void *)trans_file_bufs, 0, sizeof(trans_file_bufs));
  USBD_CDC_SetRxBuffer(&hUsbDeviceHS, (uint8_t *)&trans_file_bufs[transFileStatus.ReceivedDataSizeInBuf]);
  USBD_CDC_ReceivePacket(&hUsbDeviceHS); //重新启动接收
}

void user_usb_device_receive_prepare_cmd(void)
{
  memset((void *)UserRxBufferHS, 0, sizeof(UserRxBufferHS));
  USBD_CDC_SetRxBuffer(&hUsbDeviceHS, (uint8_t *)&UserRxBufferHS[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceHS); //重新启动接收
}

void user_usb_device_trans_file_err(void)
{
  if (ccm_param.t_sys.is_vsp_trans_file) //正在传输文件
  {
    if (TransFileTimeOut < xTaskGetTickCount()) //若传输中途超时，则主动中断此次传输
    {
      USER_EchoLog("Virtual serial port ==>> Tran file timeout! Packet loss:%ld\n", transFileStatus.FileSize - total_write_byte);
      ccm_param.t_sys.is_vsp_trans_file = false; //GUI标志位重置
      _user_usb_device_reset_variable(); //变量重置

      if (transFileStatus.IsTaskCritical)
      {
        taskENTER_CRITICAL();
      }

      (void)f_unlink((TCHAR *)transFileStatus.FileNameStr); //删除文件

      if (transFileStatus.IsTaskCritical)
      {
        taskEXIT_CRITICAL();
      }
    }
  }
}

#endif // ENABLE_USB_HOST


