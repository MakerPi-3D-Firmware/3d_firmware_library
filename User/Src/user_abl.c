#include "user_common.h"
#ifdef ENABLE_ABL

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim10;

void user_abl_deinit(void)
{
}

void user_abl_init(void)
{
  if (!ccm_param.t_sys.is_abl_init) return;

  if (F300TP == ccm_param.t_sys_data_current.model_id)
  {
    ccm_param.t_sys.enable_cavity_temp = false; //
    HAL_GPIO_DeInit(GD32_TEMP_CAVITY_GPIO_Port, GD32_TEMP_CAVITY_Pin);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GD32_TEMP_CAVITY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GD32_TEMP_CAVITY_GPIO_Port, &GPIO_InitStruct);
  }
  else if (K5 == ccm_param.t_sys_data_current.model_id)
  {
    if (1 == ccm_param.t_sys_data_current.IsMechanismLevel) // 黑屏固件，机械调平
    {
      ccm_param.t_sys.enable_cavity_temp = false; //
      HAL_GPIO_DeInit(GD32_TEMP_E_GPIO_Port, GD32_TEMP_E_Pin);
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      /* GPIO Ports Clock Enable */
      __HAL_RCC_GPIOB_CLK_ENABLE();
      GPIO_InitStruct.Pin = GD32_TEMP_E_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      HAL_GPIO_Init(GD32_TEMP_E_GPIO_Port, &GPIO_InitStruct);
    }
  }
}

bool user_abl_z_check(void)
{
  if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
  {
    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
    {
      return user_motor_axis_endstop_read_min(Z_AXIS);
    }
    else if (K600 == ccm_param.t_sys_data_current.model_id || F400TP == ccm_param.t_sys_data_current.model_id)
    {
      return user_motor_axis_endstop_read_max(Z_AXIS);
    }
    else if (F300TP == ccm_param.t_sys_data_current.model_id)
    {
      USER_GPIO_GET(TEMP_CAVITY, GPIO_PIN_RESET);
    }
    else if (K5 == ccm_param.t_sys_data_current.model_id)
    {
      if (1 == ccm_param.t_sys_data_current.IsMechanismLevel) // 黑屏固件，机械调平
      {
        USER_GPIO_GET(TEMP_E, GPIO_PIN_RESET);
      }
    }
  }

  return false;
}

#endif



