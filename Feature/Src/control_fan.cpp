#include "user_common_cpp.h"
#ifdef HAS_FAN_CONTROL

#ifdef __cplusplus
extern "C" {
#endif
static volatile int user_extruder_fan_speed = 0;

// 电机风扇控制
static void feature_control_fan_eb_motor(void)
{
  static bool motor_fan_status = false;
  static unsigned long motor_timeoutToStatus = 0;
  static unsigned long motor_timeToReset = 0;

  if ((sg_grbl::planner_moves_planned() || print_status.is_printing))
  {
    if (motor_fan_status == 0)
    {
      user_fan_control_eb_motor(true);
      user_fan_control_board_cool(true);
      motor_fan_status = 1;
    }

    motor_timeoutToStatus = xTaskGetTickCount() + 1 * 60 * 1000UL; //1 minute

    // add by suzhiwei 20160711
    // 每5min复位3秒，防止风扇停转
    if (xTaskGetTickCount() > motor_timeToReset && xTaskGetTickCount() <= motor_timeToReset + 3000UL)
    {
      user_fan_control_eb_motor(false);
      user_fan_control_board_cool(false);
    }
    else if (xTaskGetTickCount() > motor_timeToReset + 3000UL)     //5sec+ 5min
    {
      motor_timeToReset = xTaskGetTickCount() + 300000UL;
      user_fan_control_eb_motor(true);
      user_fan_control_board_cool(true);
    }
  }
  else
  {
    if (motor_fan_status == 1 && motor_timeoutToStatus < xTaskGetTickCount())
    {
      user_fan_control_eb_motor(false);
      user_fan_control_board_cool(false);
      motor_fan_status = 0;
    }
  }
}

// 喷嘴加热块风扇控制
static void feature_control_fan_nozzle_heat_block(void)
{
  if (sg_grbl::temperature_get_extruder_current(0) >= 50 ||
      (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL &&
       sg_grbl::temperature_get_extruder_current(1) >= 50))
  {
    user_fan_control_nozzle_heat_block(true);
  }
  else
  {
    user_fan_control_nozzle_heat_block(false);
  }
}

int feature_control_get_fan_speed(void)
{
  return user_extruder_fan_speed;
}

void feature_control_set_fan_speed(int value)
{
  if (user_extruder_fan_speed == value) return; //防止重复设置

  user_extruder_fan_speed = value;

  if (mcu_id == MCU_GD32F450IIH6)
  {
    user_fan_control_e_pwm(user_extruder_fan_speed);

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      //激光头使用风扇B-PWM接口，设置数据大于0，全功率设置，避免激光启动不了
      user_fan_control_b_pwm(user_extruder_fan_speed > 0 ? 255 : user_extruder_fan_speed);
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
    {
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      user_fan_control_b_pwm(user_extruder_fan_speed);
    }
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
        P3_Pro == ccm_param.t_sys_data_current.model_id ||
        K600 == ccm_param.t_sys_data_current.model_id ||
        F400TP == ccm_param.t_sys_data_current.model_id ||
        F300TP == ccm_param.t_sys_data_current.model_id)
    {
      user_fan_control_e_pwm(user_extruder_fan_speed);
    }
  }
}

void feature_control_fan(void)
{
  feature_control_fan_eb_motor();
  feature_control_fan_nozzle_heat_block();
}


#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_FAN_CONTROL

