#ifndef USER_COMMON_H
#define USER_COMMON_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <arm_math.h>
#include <stdarg.h>
#include <ctype.h>
#include <wchar.h>

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "./user_config.h"
#include "./user_board.h"
#include "./user_mcu.h"
#include "./user_util.h"
#include "./user_debug.h"
#include "./user_sys.h"
#include "./user_custom_data.h"
#include "./user_board_pin.h"
#include "./user_motor.h"
#include "./user_timer.h"
#include "../../Feature/Inc/irq_interface.h"
#include "../../Feature/Inc/utils.h"

#ifdef __cplusplus
} //extern "C" {
#endif


#ifdef ENABLE_UART1
  #include "user_uart1.h"
#endif

#ifdef ENABLE_IWDG
  #include "user_iwdg.h"
#endif

#ifdef ENABLE_SD
  #include "user_sd.h"
#endif

#ifdef ENABLE_DELAY
  #include "user_delay.h"
#endif

#ifdef ENABLE_FREERTOS
  #include "./user_os_msg.h"
  #include "./user_task.h"
#endif

#ifdef ENABLE_CCM
  #include "./user_ccm.h"
#endif

#ifdef ENABLE_FLASH
  #include "../../Iap/Inc/flash_if.h"
  #include "../../Feature/Inc/flash_interface.h"
#endif

#ifdef ENABLE_IAP
  #include "../../Iap/Inc/iap_menu.h"
#endif

#ifdef ENABLE_USB_HOST
  #include "usb_host.h"
  #include "./user_usb_host.h"
#endif

#ifdef ENABLE_USB_DEVICE
  #include "usbd_cdc_if.h"
  #include "./user_usb_device.h"
#endif

#ifdef ENABLE_TOUCH
  #include "../../Touch/Inc/ft5206.h"
  #include "../../Touch/Inc/touch.h"
  #include "../../Touch/Inc/touch_data.h"
  #include "../../Touch/Inc/ctiic.h"
  #include "../../Touch/Inc/gt9147.h"
#endif

#ifdef ENABLE_DMA2D
  #include "./user_dma2d.h"
#endif

#ifdef ENABLE_LCD
  #if defined(ENABLE_FSMC)
    #include "../../Lcd/Inc/lcd_common.h"
    #include "../../Lcd/Inc/lcd_nt35310.h"
    #include "../../Lcd/Inc/lcd_ili9488.h"
    #include "../../Lcd/Inc/LCD_SSD1963.h"
  #elif defined(ENABLE_LTDC)
    #include "./user_ltdc.h"
  #endif
  #include "../../Lcd/Inc/lcd.h"
  #include "../../Lcd/Inc/font.h"
  #include "../../Lcd/Inc/icons.h"
#endif

#ifdef ENABLE_GUI_LVGL
  #include "lvgl/lvgl.h"
  #include "lv_port_indev.h"
  #include "lv_port_disp.h"
  #include "user_lvgl.h"
  #include "../../Feature/Inc/lvgl_interface.h"
#endif

#ifdef ENABLE_SDRAM
  #include "./user_sdram.h"
#endif

#ifdef HAS_FAN_CONTROL
  #include "./user_fan.h"
#endif

#ifdef ENABLE_NAND
  #include "./user_nand.h"
  #include "./user_ftl.h"
#endif

#ifdef HAS_BUZZER_CONTROL
  #include "./user_buzzer.h"
#endif

#ifdef HAS_LED_CONTROL
  #include "./user_led.h"
#endif

#ifdef HAS_POWER_RECOVERY
  #include "../../Feature/Inc/power_recovery.h"
  #include "../../Feature/Inc/power_data.h"
#endif

#ifdef ENABLE_ABL
  #include "./user_abl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void user_init0(void);
extern void user_init(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_COMMON_H

