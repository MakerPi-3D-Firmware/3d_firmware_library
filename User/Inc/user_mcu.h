#ifndef USER_MCU_H
#define USER_MCU_H

#define MCU_NOT_SUPPORT 0
#define MCU_STM32F429IGT6 1
#define MCU_STM32F103RCT6 2
#define MCU_STM32F407VGT6 3
#define MCU_GD32F450IIH6 4
#define MCU_GD32F303RET6 5
#define MCU_GD32F303RCT6 6

extern volatile uint32_t mcu_id;

extern void user_get_mcu_id(void);
extern void user_print_mcu_info(void);

#endif // USER_MCU_H

