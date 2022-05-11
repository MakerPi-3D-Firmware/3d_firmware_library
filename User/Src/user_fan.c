#include "user_common.h"

#ifdef HAS_FAN_CONTROL

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim10;

void user_fan_control_init(void)
{
  if (mcu_id == MCU_GD32F450IIH6)
  {
    // Extruder fan
    {
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //B Extruder fan
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); //E Extruder fan
      htim2.Instance->CCR1 = 0;
      htim2.Instance->CCR2 = 0;
    }

    // Board fan
    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
        P3_Pro == ccm_param.t_sys_data_current.model_id ||
        F300TP == ccm_param.t_sys_data_current.model_id ||
        K600 == ccm_param.t_sys_data_current.model_id ||
        F400TP == ccm_param.t_sys_data_current.model_id ||
        Drug == ccm_param.t_sys_data_current.model_id)
    {
      htim10.Instance->CCR1 = 0;
      HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
    }

    // EB motor fan
    USER_GPIO_WRITE(GD32, FAN_EB_MOTOR, GPIO_PIN_RESET);
    // Nozzle heat block fan
    USER_GPIO_WRITE(GD32, FAN_FILTER, GPIO_PIN_RESET);
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
  }
}

//E Extruder fan set
void user_fan_control_e_pwm(int pwm_value)
{
  unsigned short pwn_tmp = pwm_value;

  if (pwm_value > 255)
    pwn_tmp = 255;

  if (mcu_id == MCU_GD32F450IIH6)
  {
    pwn_tmp = pwn_tmp * 2120 / 255;
    htim2.Instance->CCR2 = pwn_tmp;
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    pwn_tmp = pwn_tmp * 1000 / 255;
    htim3.Instance->CCR3 = pwn_tmp;
  }
}

//B Extruder fan set
void user_fan_control_b_pwm(int pwm_value)
{
  unsigned short pwn_tmp = pwm_value;

  if (pwm_value > 255)
    pwn_tmp = 255;

  if (mcu_id == MCU_GD32F450IIH6)
  {
    pwn_tmp = (pwn_tmp) * 2120 / 255;
    htim2.Instance->CCR1 = pwn_tmp;
  }
}

//EB motor fan set
void user_fan_control_eb_motor(bool isOn)
{
  if (mcu_id == MCU_GD32F450IIH6)
  {
    USER_GPIO_WRITE(GD32, FAN_EB_MOTOR, (isOn ? GPIO_PIN_SET : GPIO_PIN_RESET));
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
  }
}

// Nozzle heat block fan set
void user_fan_control_nozzle_heat_block(bool isOn)
{
  if (mcu_id == MCU_GD32F450IIH6)
  {
    USER_GPIO_WRITE(GD32, FAN_FILTER, (isOn ? GPIO_PIN_SET : GPIO_PIN_RESET));
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
  }
}

void user_fan_control_board_cool(bool isOn)
{
  HAL_TIM_PWM_Stop(&htim10, TIM_CHANNEL_1);
  htim10.Instance->CCR1 = (isOn ? 2120 * 9 / 10 : 0);
  HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
}

void user_fan_disable_all(void)
{
  user_fan_control_e_pwm(0);
  user_fan_control_b_pwm(0);
  user_fan_control_nozzle_heat_block(false);
  user_fan_control_board_cool(false);
}

#endif // HAS_FILAMENT

