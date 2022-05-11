#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

void model_drug_flash_config_init(void)
{
  if (flash_param_t.update_param_flag != FLASH_UPDATE_FLAG)
  {
  }
}

void model_drug_grbl_config_init(void)
{
  const float buf2[] = DEFAULT_MAX_FEEDRATE_DRUG;
  const long buf3[] = DEFAULT_MAX_ACCELERATION_DRUG;
  const float buf4[] = DEFAULT_AXIS_STEPS_PER_UNIT_DRUG;

  for (short i = 0; i < ccm_param.t_sys.axis_num; i++)
  {
    ccm_param.planner_settings.axis_steps_per_mm[i] = buf4[i] * ccm_param.t_sys.step;
    ccm_param.planner_settings.max_feedrate_mm_s[i] = buf2[i];
    ccm_param.planner_settings.max_acceleration_mm_per_s2[i] = buf3[i];
  }

  ccm_param.planner_settings.max_xy_jerk = 5.0f;
  ccm_param.planner_settings.max_z_jerk = 0.2f;
  ccm_param.planner_settings.max_e_jerk = 2.5f;
  ccm_param.planner_settings.max_b_jerk = 2.5f;
}

void model_drug_init(void)
{
  ccm_param.t_model.enable_invert_dir[X_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[X2_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[Y_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[Z_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[Z2_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[E_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[B_AXIS] = false;
  ccm_param.t_model.xyz_home_dir[Z_AXIS] = 1;
  ccm_param.t_sys_data_current.enable_color_mixing = true;
  ccm_param.t_sys.enable_z_max_limit = true;
  ccm_param.t_sys.medium_page_file_num = 4;
  sg_grbl::temperature_set_heater_maxtemp(0, 325);
  sg_grbl::temperature_set_heater_maxtemp(1, 325);
  {
    flash_param_t.extruder_type = EXTRUDER_TYPE_DRUG;
    flash_config_set_flag_save();
  }
  homing_feedrate[Z_AXIS] = 10.0f * 60.0f;
  homing_feedrate[Z2_AXIS] = 10.0f * 60.0f;
  // Turn off the drug printhead
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}


#ifdef __cplusplus
} //extern "C" {
#endif

