#include "user_common.h"

#ifdef ENABLE_UART1

extern UART_HandleTypeDef huart1;

#ifdef ENABLE_UART1_DMA

#define RECEIVE_LENGTH 96
#define USART_DMA_SENDING 0//发送未完成
#define USART_DMA_SENDOVER 1//发送完成
typedef struct
{
  volatile unsigned char receive_flag: 1; //空闲接收标记
    volatile unsigned char dmaSend_flag: 1; //发送完成标记
    unsigned short rx_len;//接收长度
    unsigned char usartDMA_rxBuf[RECEIVE_LENGTH];//DMA接收缓存
  } USER_USART_RECEIVE_T;

  //===========================================================================
  //=============================private variables ============================
  //===========================================================================
  USER_USART_RECEIVE_T user_uart_receive;
  static long gcode_N, gcode_LastN;     //wifi
  static char *strchr_pointer;
  //===========================================================================
  //=============================public  function =============================
  //===========================================================================

  //开启串口中断
  void user_uart1_dma_start(void)
{
  __HAL_UART_CLEAR_OREFLAG(&huart1);
  user_uart_receive.receive_flag = 0;
  user_uart_receive.dmaSend_flag = USART_DMA_SENDOVER;
  HAL_UART_Receive_DMA(&huart1, user_uart_receive.usartDMA_rxBuf, RECEIVE_LENGTH);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

//DMA发送函数
void user_uart1_dma_send_data(uint8_t *pdata, uint16_t Length)
{
  while (user_uart_receive.dmaSend_flag == USART_DMA_SENDING);

  user_uart_receive.dmaSend_flag = USART_DMA_SENDING;
  HAL_UART_Transmit_DMA(&huart1, pdata, Length);
}

static void _user_uart1_flush_serial_request_resend()
{
  USER_EchoLogStr("Resend: ");
  USER_EchoLogStr("%ld\n", gcode_LastN + 1);
  USER_EchoLogStr("ok\n");
}

static void _user_uart1_receive_ncode(unsigned char *data)
{
  strchr_pointer = strchr((char *)data, 'N');
  gcode_N = (strtol((const char *)strchr_pointer, NULL, 10));

  if (gcode_N != gcode_LastN + 1 && (strstr((char *)data, (char *)"M110") == NULL))
  {
    USER_EchoLogStr("Error:");
    USER_EchoLogStr("Line Number is not Last Line Number+1, Last Line: ");
    USER_EchoLogStr("%ld", gcode_LastN);
    _user_uart1_flush_serial_request_resend();
    return;
  }

  if (strchr((const char *)data, '*') != NULL)
  {
    uint8_t checksum = 0;
    uint8_t count = 0;

    while (data[count] != '*')
    {
      checksum = checksum ^ data[count++];
    }

    strchr_pointer = strchr((char *)data, '*');

    if ((uint8_t)(strtod((const char *)strchr_pointer, NULL)) != checksum)
    {
      USER_EchoLogStr("Error:");
      USER_EchoLogStr("checksum mismatch, Last Line: ");
      USER_EchoLogStr(" checksum: %d\n\r", checksum);
      count = 0;
      USER_EchoLogStr(" '");

      while (data[count] != '*')
      {
        USER_EchoLogStr("%c", data[count++]);
      }

      USER_EchoLogStr(" '\n\r ");
      checksum = 0;
      count = 0;

      while (data[count] != '*')
      {
        USER_EchoLogStr("ccm_param.command_buffer:%d;", data[count]);
        checksum = checksum ^ data[count++];
        USER_EchoLogStr(" checksum:%d \n\r", checksum);
      }

      /// USER_EchoLogStr("\n\r ");
      USER_EchoLogStr("%ld", gcode_LastN);
      _user_uart1_flush_serial_request_resend();
      return;
    }

    //if no errors, continue parsing
  }
  else
  {
    USER_EchoLogStr("Error:");
    USER_EchoLogStr("No Checksum with line number, Last Line: ");
    USER_EchoLogStr("%ld", gcode_LastN);
    _user_uart1_flush_serial_request_resend();
    return;
  }

  gcode_LastN = gcode_N;
  //if no errors, continue parsing
}

__weak void user_uart1_dma_receive_process(unsigned char *data, unsigned short length)
{
  if (length > 0)
  {
    USER_EchoLogStr("Rec:%s;len:%d\n", data, length);

    if (strchr((const char *)data, 'N') != NULL)
    {
      _user_uart1_receive_ncode(data);
    }
    else  // if we don't receive 'N' but still see '*'
    {
      if ((strchr((char *)data, '*') != NULL))
      {
        USER_EchoLogStr("Error:");
        USER_EchoLogStr("No Line Number with checksum, Last Line: ");
        USER_EchoLogStr("%ld", gcode_LastN);
        return;
      }
    }

    user_send_uart_cmd((char *)data, length);
  }
}

//串口接收空闲中断
void user_uart1_dma_receive_idle(void)
{
  uint32_t temp;

  if ((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET))
  {
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    HAL_UART_DMAStop(&huart1);
    temp = huart1.hdmarx->Instance->NDTR;
    user_uart_receive.rx_len =  RECEIVE_LENGTH - temp;
    user_uart_receive.receive_flag = 1;
    HAL_UART_Receive_DMA(&huart1, user_uart_receive.usartDMA_rxBuf, RECEIVE_LENGTH);
  }

  // 打印过程中，接收到gcode指令后，返回ok申请下一条
  // 直到当前指令条数为BUFSIZE-1，暂停请求
  if (user_uart_receive.receive_flag) //如果产生了空闲中断
  {
    char *p;
    p = strtok((char *)user_uart_receive.usartDMA_rxBuf, "\r\n");

    while (p)
    {
      printf("%s\n", p);
      user_uart1_dma_receive_process((unsigned char *)p, strlen(p));
      p = strtok(NULL, "\r\n");
    }

    memset((char *)user_uart_receive.usartDMA_rxBuf, 0, sizeof(user_uart_receive.usartDMA_rxBuf));
    //    user_uart1_dma_receive_process(user_uart_receive.usartDMA_rxBuf, user_uart_receive.rx_len);
    user_uart_receive.receive_flag = 0;
  }
}

//DMA发送完成中断回调函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  __HAL_DMA_DISABLE(huart->hdmatx);
  user_uart_receive.dmaSend_flag = USART_DMA_SENDOVER;
}
#endif // ENABLE_UART1_DMA

//===========================================================================
//===========加入以下代码,支持printf函数,而不需要选择use MicroLIB============
//===========================================================================

#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
  int handle;

};
/* FILE is typedef’ d in stdio.h. */
FILE __stdout;
void _sys_exit(int x)
{
  x = x;
}
void _ttywrch(int ch)
{
  ch = ch;
}
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
#if !defined(ENABLE_UART1_DMA)

  while ((USART1->SR & 0x40) == 0); //循环发送，直到发送完毕

  USART1->DR = (uint8_t)ch; //发送数据
#else
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
#endif
  return ch;
}
#endif // ENABLE_UART1



