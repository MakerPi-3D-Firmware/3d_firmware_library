#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

void model_m30_flash_config_init(void)
{
  if (flash_param_t.update_param_flag != FLASH_UPDATE_FLAG)
  {
  }
}

void model_m30_grbl_config_init(void)
{
  const float buf2[] = DEFAULT_MAX_FEEDRATE_M30;
  const long buf3[] = DEFAULT_MAX_ACCELERATION_M30;
  const float buf4[] = DEFAULT_AXIS_STEPS_PER_UNIT_M30;

  for (short i = 0; i < ccm_param.t_sys.axis_num; i++)
  {
    ccm_param.planner_settings.axis_steps_per_mm[i] = buf4[i] * ccm_param.t_sys.step;
    ccm_param.planner_settings.max_feedrate_mm_s[i] = buf2[i];
    ccm_param.planner_settings.max_acceleration_mm_per_s2[i] = buf3[i];
  }

  ccm_param.planner_settings.max_xy_jerk = 10.0f;
  ccm_param.planner_settings.max_z_jerk = 0.2f;
  ccm_param.planner_settings.max_e_jerk = 2.5f;
  ccm_param.planner_settings.max_b_jerk = 2.5f;
  ccm_param.planner_settings.acceleration = 2000;
  ccm_param.planner_settings.retract_acceleration = 2000;
}

void model_m30_init(void)
{
  ccm_param.t_model.enable_invert_dir[X_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[X2_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[Y_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[Z_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[Z2_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[E_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[B_AXIS] = false;
  ccm_param.t_sys.enable_check_door_open = false;
  ccm_param.t_sys.enable_led_light = true;   //有LED灯照明功能
  ccm_param.t_sys_data_current.enable_color_mixing = false;
  ccm_param.t_sys.enable_z_max_limit = true;
  ccm_param.t_sys.is_power_off_z_not_move = false;
  ccm_param.t_sys.medium_page_file_num = 7;
  flash_param_t.extruder_type = EXTRUDER_TYPE_SINGLE;
  feature_print_control::idex_sys_set_extruder(false);
  flash_config_set_flag_save();
  sg_grbl::temperature_set_heater_maxtemp(0, 300);
  homing_feedrate[Z_AXIS] = 8.0f * 60.0f;
}

#ifdef __cplusplus
} //extern "C" {
#endif

