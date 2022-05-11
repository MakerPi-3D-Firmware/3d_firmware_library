#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

void model_f300tp_flash_config_init(void)
{
  if (flash_param_t.update_param_flag != FLASH_UPDATE_FLAG)
  {
    //idex
    flash_param_t.idex_extruder_0_bed_offset[0] = 0;
    flash_param_t.idex_extruder_0_bed_offset[1] = 300;
    flash_param_t.idex_ext0_home_pos_adding[0] = 0;
    flash_param_t.idex_ext0_home_pos_adding[1] = 0;
  }
}

void model_f300tp_grbl_config_init(void)
{
  const float buf2[] = DEFAULT_MAX_FEEDRATE_F300TP;
  const long buf3[] = DEFAULT_MAX_ACCELERATION_F300TP;
  const float buf4[] = DEFAULT_AXIS_STEPS_PER_UNIT_F300TP;

  for (short i = 0; i < ccm_param.t_sys.axis_num; i++)
  {
    ccm_param.planner_settings.axis_steps_per_mm[i] = buf4[i] * ccm_param.t_sys.step;
    ccm_param.planner_settings.max_feedrate_mm_s[i] = buf2[i];
    ccm_param.planner_settings.max_acceleration_mm_per_s2[i] = buf3[i];
  }

  ccm_param.planner_settings.max_xy_jerk = 20.0f;
  ccm_param.planner_settings.max_z_jerk = 0.4f;
  ccm_param.planner_settings.max_e_jerk = 5.0f;
  ccm_param.planner_settings.max_b_jerk = 5.0f;
  ccm_param.planner_settings.acceleration = 2000;
  ccm_param.planner_settings.retract_acceleration = 2000;
}

void model_f300tp_init(void)
{
  ccm_param.t_model.enable_invert_dir[X_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[X2_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[Y_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[Z_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[Z2_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[E_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[B_AXIS] = true;
  ccm_param.t_sys.enable_check_door_open = true;
  ccm_param.t_sys.enable_led_light = true;   //有LED灯照明功能
  ccm_param.t_sys.is_lighting_auto_ctr = false;
  ccm_param.t_sys_data_current.enable_color_mixing = true;
  ccm_param.t_sys.enable_z_max_limit = true;
  ccm_param.t_sys.medium_page_file_num = 8;
  //
  user_led_control_lighting_status(true);
  ccm_param.t_sys.is_lighting_on = true;
  {
    flash_param_t.extruder_type = EXTRUDER_TYPE_SINGLE;
    flash_param_t.flag = 1;
  }
}


#ifdef __cplusplus
} //extern "C" {
#endif

