#ifndef USER_UART1_H
#define USER_UART1_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_UART1
#ifdef ENABLE_UART1_DMA
extern void user_uart1_dma_start(void);
extern void user_uart1_dma_send_data(uint8_t *pdata, uint16_t Length);
extern void user_uart1_dma_receive_idle(void);
extern void user_uart1_dma_receive_process(unsigned char *data, unsigned short length);
#endif
#endif

#ifdef __cplusplus
} //extern "C" {
#endif

#endif //USER_UART1_H

