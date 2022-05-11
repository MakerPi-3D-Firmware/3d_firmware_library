#include "user_common_cpp.h"
#ifdef HAS_LED_CONTROL
#ifdef __cplusplus
extern "C" {
#endif

//主板灯
static void _feature_control_led_board(void)
{
  static unsigned long led_last_time = 0UL;

  if (led_last_time < xTaskGetTickCount())
  {
    user_led_board_toggle();
    led_last_time = xTaskGetTickCount() + 500U; // 500ms闪烁一次
  }
}

//警示灯闪烁
static void feature_control_led_caution_twinkle(uint32_t delay_time)
{
  if (ccm_param.t_sys.enable_warning_light)
  {
    static unsigned long CautionLightTimeControl = 0UL;

    if (CautionLightTimeControl < xTaskGetTickCount())
    {
      user_led_caution_toggle();
      CautionLightTimeControl = xTaskGetTickCount() + delay_time;
    }
  }
}

//照明灯条控制
static void _feature_control_led_lighting(void)
{
  static uint8_t LEDLight_status = 0U;
  static unsigned long LEDLight_timeoutToStatus = 0UL;

  if (sg_grbl::planner_moves_planned() || feature_print_control::print_is_heating()) // 移动或加热，灯条开启
  {
    if (LEDLight_status == 0)
    {
      user_led_control_lighting_status(true);
      ccm_param.t_sys.is_lighting_on = 1;
      LEDLight_status = 1U;
    }

    LEDLight_timeoutToStatus = xTaskGetTickCount() + (60UL * 1000UL); //1 minute timeout
  }
  else
  {
    if ((LEDLight_status == 1U) && (LEDLight_timeoutToStatus < xTaskGetTickCount()))
    {
      user_led_control_lighting_status(false);
      ccm_param.t_sys.is_lighting_on = 0;
      LEDLight_status = 0U;
    }
  }
}

//LED控制入口
void feature_control_led(void)
{
  _feature_control_led_board(); // 核心板LED一直开启

  if (ccm_param.t_sys.enable_led_light) // 有LED灯条功能，开启LED灯条照明
  {
    if (ccm_param.t_sys.is_lighting_auto_ctr) //F300TP手动控制
    {
      _feature_control_led_lighting();
    }
  }

  if (ccm_param.t_sys.enable_warning_light)
  {
    if (ccm_param.t_sys.is_door_open && print_status.is_printing) //打印中门未关闭
    {
      feature_control_led_caution_twinkle(100U);//0.1S闪烁
    }
    else if ((int)sg_grbl::temperature_get_extruder_current(0) > 60 ||
             print_status.is_printing) //温度大于60度 或 打印前的加热阶段
    {
      if (ccm_param.t_sys.is_door_open)
      {
        feature_control_led_caution_twinkle(100U);//0.1S闪烁
      }
      else
      {
        feature_control_led_caution_twinkle(500U);//0.5S闪烁
      }
    }
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_LED_CONTROL














