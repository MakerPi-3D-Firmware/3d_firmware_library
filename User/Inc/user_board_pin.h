#ifndef USER_BOARD_PIN_H
#define USER_BOARD_PIN_H

/* Includes ------------------------------------------------------------------*/

#define MOTOR_STEP_PIN  0
#define MOTOR_DIR_PIN   1
#define MOTOR_EN_PIN    2
#define MOTOR_LIMIT_MIN_PIN 3
#define MOTOR_LIMIT_MAX_PIN 4

#define HEAT_PIN_E_INDEX 0
#define HEAT_PIN_BED_INDEX 1
#define HEAT_PIN_CAVITY_INDEX 2
#define HEAT_PIN_B_INDEX 3

typedef struct
{
  uint16_t Pin;
  GPIO_TypeDef *Port;
} Motor_Pin_TypeDer;

typedef struct
{
  uint16_t Pin;
  GPIO_TypeDef *Port;
} Heat_Pin_TypeDer;

typedef struct
{
  uint16_t Pin;
  uint32_t Channel;
  GPIO_TypeDef *Port;
} ADC_Pin_TypeDer;


extern volatile Heat_Pin_TypeDer GD32_Heat_Pin[4];
extern volatile Heat_Pin_TypeDer STM32_Heat_Pin[4];

extern volatile ADC_Pin_TypeDer GD32_ADC_Pin[4];
extern volatile ADC_Pin_TypeDer STM32_ADC_Pin[4];
extern volatile Motor_Pin_TypeDer GD32_Motor_Pin[5][MAX_NUM_AXIS];
extern volatile Motor_Pin_TypeDer STM32_Motor_Pin[5][MAX_NUM_AXIS];

#define USER_GPIO_READ(MCU, TYPE)   HAL_GPIO_ReadPin(MCU##_##TYPE##_GPIO_Port, MCU##_##TYPE##_Pin)
#define USER_GPIO_WRITE(MCU, TYPE, value)   HAL_GPIO_WritePin(MCU##_##TYPE##_GPIO_Port, MCU##_##TYPE##_Pin, value)
#define USER_GPIO_CTRL(TYPE, isOn) \
  { \
    volatile GPIO_PinState value = isOn ? GPIO_PIN_SET : GPIO_PIN_RESET; \
    if (mcu_id == MCU_GD32F450IIH6) \
    { \
      USER_GPIO_WRITE(GD32, TYPE, value); \
    } \
    else if (mcu_id == MCU_STM32F429IGT6) \
    { \
      USER_GPIO_WRITE(STM32, TYPE, value); \
    } \
  }

#define USER_GPIO_GET_VALUE(TYPE) \
  { \
    volatile GPIO_PinState result = GPIO_PIN_RESET; \
    if (mcu_id == MCU_GD32F450IIH6) \
    { \
      result = USER_GPIO_READ(GD32, TYPE); \
    } \
    else if (mcu_id == MCU_STM32F429IGT6) \
    { \
      result = USER_GPIO_READ(STM32, TYPE); \
    } \
    return (result); \
  }

#define USER_GPIO_GET(TYPE, VALUE) \
  { \
    volatile GPIO_PinState result = GPIO_PIN_RESET; \
    if (mcu_id == MCU_GD32F450IIH6) \
    { \
      result = USER_GPIO_READ(GD32, TYPE); \
    } \
    else if (mcu_id == MCU_STM32F429IGT6) \
    { \
      result = USER_GPIO_READ(STM32, TYPE); \
    } \
    return (result == VALUE); \
  }

#define USER_MOTOR_WRITE(PIN_TYPE, AXIS, value) \
  { \
    if (mcu_id == MCU_GD32F450IIH6) \
    { \
      if (GD32_Motor_Pin[PIN_TYPE][AXIS].Port != NULL && GD32_Motor_Pin[PIN_TYPE][AXIS].Port != NULL) \
      { \
        HAL_GPIO_WritePin(GD32_Motor_Pin[PIN_TYPE][AXIS].Port, GD32_Motor_Pin[PIN_TYPE][AXIS].Pin, (GPIO_PinState)value); \
      } \
    } \
    else if (mcu_id == MCU_STM32F429IGT6) \
    { \
      if (STM32_Motor_Pin[PIN_TYPE][AXIS].Port != NULL && STM32_Motor_Pin[PIN_TYPE][AXIS].Port != NULL) \
      { \
        HAL_GPIO_WritePin(STM32_Motor_Pin[PIN_TYPE][AXIS].Port, STM32_Motor_Pin[PIN_TYPE][AXIS].Pin, (GPIO_PinState)value); \
      } \
    } \
  }

#define USER_ENDSTOP_READ(PIN_TYPE, AIXS, status) \
  { \
    volatile GPIO_PinState result = GPIO_PIN_SET; \
    if (mcu_id == MCU_GD32F450IIH6) \
    { \
      if (GD32_Motor_Pin[PIN_TYPE][AIXS].Port != NULL && GD32_Motor_Pin[PIN_TYPE][AIXS].Port != NULL) \
      { \
        result = HAL_GPIO_ReadPin(GD32_Motor_Pin[PIN_TYPE][AIXS].Port, GD32_Motor_Pin[PIN_TYPE][AIXS].Pin); \
      } \
      return (result != status); \
    } \
    else if (mcu_id == MCU_STM32F429IGT6) \
    { \
      if (STM32_Motor_Pin[PIN_TYPE][AIXS].Port != NULL && STM32_Motor_Pin[PIN_TYPE][AIXS].Port != NULL) \
      { \
        result = HAL_GPIO_ReadPin(STM32_Motor_Pin[PIN_TYPE][AIXS].Port, STM32_Motor_Pin[PIN_TYPE][AIXS].Pin); \
      } \
      return (result != status); \
    } \
  }

#define USER_HEAT_WRITE(PIN_TYPE, value) \
  { \
    if (mcu_id == MCU_GD32F450IIH6) \
    { \
      HAL_GPIO_WritePin(GD32_Heat_Pin[PIN_TYPE].Port, GD32_Heat_Pin[PIN_TYPE].Pin, (GPIO_PinState)value); \
    } \
    else if (mcu_id == MCU_STM32F429IGT6) \
    { \
      HAL_GPIO_WritePin(STM32_Heat_Pin[PIN_TYPE].Port, STM32_Heat_Pin[PIN_TYPE].Pin, (GPIO_PinState)value); \
    } \
  }

__inline static void user_pin_light_bar_ctrl(bool isOn) USER_GPIO_CTRL(LIGHT_BAR, isOn)
__inline static void user_pin_lcd_backlight_ctrl(bool isOn) USER_GPIO_CTRL(LCD_BACKLIGHT, isOn)
__inline static void user_pin_tp_sck_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_SCK, isOn)
__inline static void user_pin_tp_cs_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_CS, isOn)
__inline static void user_pin_tp_mosi_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_MOSI, isOn)
__inline static void user_pin_tp_pen_ctrl(bool isOn) USER_GPIO_CTRL(TOUCH_PEN, isOn)

__inline static bool user_pin_tp_pen_read(void) USER_GPIO_GET(TOUCH_PEN, GPIO_PIN_RESET)
__inline static bool user_pin_tp_miso_read(void) USER_GPIO_GET(TOUCH_MISO, GPIO_PIN_SET)
__inline static bool user_pin_sig_mat_e0_read(void) USER_GPIO_GET(SIG_MAT_E0, GPIO_PIN_SET)
__inline static bool user_pin_sig_mat_e1_read(void) USER_GPIO_GET(SIG_MAT_E1, GPIO_PIN_SET)
__inline static bool user_pin_sig_door_read(void) USER_GPIO_GET(SIG_DOOR, GPIO_PIN_SET)

#endif /* USER_BOARD_PIN_H */

