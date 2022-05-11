#include "user_common.h"

#ifdef HAS_LED_CONTROL

void user_led_control_init(void)
{
  if (F300TP == ccm_param.t_sys_data_current.model_id)
  {
    user_led_control_lighting_status(true);
#ifdef ENABLE_GUI_LVGL
    ccm_param.t_sys.is_lighting_on = 1;
#endif
  }
}

// Board led
void user_led_board_toggle(void)
{
  if (mcu_id == MCU_GD32F450IIH6)
  {
    HAL_GPIO_TogglePin(GD32_LIGHT_GPIO_Port, GD32_LIGHT_Pin);
  }
}

void user_led_caution_toggle(void)
{
  //None
}

void user_led_control_caution_status(bool isOn)
{
  //None
}

void user_led_control_lighting_status(bool isOn)
{
  user_pin_light_bar_ctrl(isOn);
}

#endif // HAS_LED_CONTROL












