#ifndef USER_USB_DEVICE_H
#define USER_USB_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_USB_DEVICE

typedef struct
{
  uint8_t IsM35Order;  //是否是M35获取状态命令
  uint8_t IsM34Order;  //是否是M34机型确认命令
  uint8_t IsM28Order;  //是否是M28开始传输命令
  uint8_t IsM105Order;  //是否是M105获取温度数据
  uint8_t IsG1Order;  //是否是G1控制电机命令
  uint8_t IsG28Order; //G28命令
  uint8_t IsM25Order; //M25暂停打印命令
  uint8_t IsM24Order; //M24继续打印命令
  uint8_t IsM33Order; //M33停止打印命令
  uint8_t IsM700Order; //M700获取USB打印是否完成
  uint8_t IsStartTrans; //是否开始传输
  uint8_t IsEndTrans; //是否传输完毕
  uint8_t IsCmd; //是否是指令
  uint8_t IsTaskCritical; //是否进入临界区保存文件
  char G1OrderBuf[64]; //G1控制电机命令
  char ModelStr[20]; //机型
  TCHAR FileNameStr[40]; //文件名
  volatile unsigned long FileSize; //文件大小
  volatile unsigned long ReceivedFileSize; //已接收的文件大小
  volatile unsigned int ReceivedDataSizeInBuf; //在32K的缓冲区中已存储的字节数
} TransFileStatus;


typedef struct
{
  bool isFilamentCheck;  //是否断料
} TransFilePrintStatus;

extern void user_usb_device_receive(uint8_t *Buf, uint32_t *Len);
extern void user_usb_device_transmit(const char *Buf);
extern void user_usb_device_write_file_buf(void);
extern void user_usb_device_receive_prepare_file(void);
extern void user_usb_device_receive_prepare_cmd(void);
extern void user_usb_device_trans_file_err(void);
extern bool user_usb_device_is_receive_process(unsigned char *data, unsigned short length);
extern volatile TransFileStatus transFileStatus;
extern volatile TransFilePrintStatus transFilePrintStatus;
extern volatile uint32_t total_write_byte;

#define USER_SEND_USBD_CMD_BUF(...) \
  { \
    char usbd_cmd_buf[96] = {0}; \
    (void)memset(usbd_cmd_buf, 0, sizeof(usbd_cmd_buf)); \
    (void)snprintf(usbd_cmd_buf, sizeof(usbd_cmd_buf), __VA_ARGS__); \
    user_usb_device_transmit(usbd_cmd_buf); \
  }

#endif //ENABLE_USB_DEVICE

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_USB_DEVICE_H

