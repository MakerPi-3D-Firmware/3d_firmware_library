#ifndef _USER_DELAY_H
#define _USER_DELAY_H

#ifdef ENABLE_DELAY

#ifdef __cplusplus
extern "C" {
#endif

void user_delay_init(uint8_t sysclk);
void user_delay_ms(uint16_t nms);
void user_delay_us(uint32_t nus);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif

#endif

