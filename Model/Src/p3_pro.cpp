#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

void model_p3_pro_flash_config_init(void)
{
  if (flash_param_t.update_param_flag != FLASH_UPDATE_FLAG)
  {
    //idex
    flash_param_t.idex_extruder_0_bed_offset[0] = 50;
    flash_param_t.idex_extruder_0_bed_offset[1] = 350;
    flash_param_t.idex_extruder_1_bed_offset[0] = 50;
    flash_param_t.idex_extruder_1_bed_offset[1] = 350;
    flash_param_t.idex_ext0_home_pos_adding[0] = 50;
    flash_param_t.idex_ext0_home_pos_adding[1] = 0;
    flash_param_t.idex_ext1_ext0_datum_pos[0] = 150.0f;
    flash_param_t.idex_ext1_ext0_datum_pos[1] = 10.0f;
    flash_param_t.idex_ext1_ext0_datum_pos[2] = 0.0f;
    //mix
    flash_param_t.mix_extruder_0_bed_offset[0] = 55;
    flash_param_t.mix_extruder_0_bed_offset[1] = 352;
    flash_param_t.mix_ext0_home_pos_adding[0] = 60;
    flash_param_t.mix_ext0_home_pos_adding[1] = 60;
    flash_param_t.mix_ext0_home_pos_adding[0] = 65;
    flash_param_t.mix_ext0_home_pos_adding[1] = 5;
    //laser
    flash_param_t.laser_extruder_0_bed_offset[0] = 50;
    flash_param_t.laser_extruder_0_bed_offset[1] = 350;
    flash_param_t.laser_extruder_1_bed_offset[0] = 60;
    flash_param_t.laser_extruder_1_bed_offset[1] = 352;
    flash_param_t.laser_ext0_home_pos_adding[0] = 0;
    flash_param_t.laser_ext0_home_pos_adding[1] = 15;
    flash_param_t.laser_focus_z_height = 25.0f;
  }
}

void model_p3_pro_grbl_config_init(void)
{
  const float buf2[] = DEFAULT_MAX_FEEDRATE_P3_PRO;
  const long buf3[] = DEFAULT_MAX_ACCELERATION_P3_PRO;
  const float buf4[] = DEFAULT_AXIS_STEPS_PER_UNIT_P3_PRO;

  for (short i = 0; i < ccm_param.t_sys.axis_num; i++)
  {
    ccm_param.planner_settings.axis_steps_per_mm[i] = buf4[i] * ccm_param.t_sys.step;
    ccm_param.planner_settings.max_feedrate_mm_s[i] = buf2[i];
    ccm_param.planner_settings.max_acceleration_mm_per_s2[i] = buf3[i];
  }

  ccm_param.planner_settings.max_xy_jerk = 10.0f;
  ccm_param.planner_settings.max_z_jerk = 0.2f;
  ccm_param.planner_settings.max_e_jerk = 5.0f;
  ccm_param.planner_settings.max_b_jerk = 5.0f;
  ccm_param.planner_settings.acceleration = 2000;
  ccm_param.planner_settings.retract_acceleration = 2000;
}

void model_p3_pro_init(void)
{
  ccm_param.t_model.enable_invert_dir[X_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[X2_AXIS] = true;
  ccm_param.t_model.enable_invert_dir[Y_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[Z_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[Z2_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[E_AXIS] = false;
  ccm_param.t_model.enable_invert_dir[B_AXIS] = false;
  ccm_param.t_sys_data_current.enable_color_mixing = true;
  ccm_param.t_sys.enable_z_max_limit = false;
  ccm_param.t_sys.is_power_off_z_not_move = true;
  ccm_param.t_sys.medium_page_file_num = 4;
  ccm_param.t_sys.is_dual_z_axis = true;
  sg_grbl::temperature_set_heater_maxtemp(0, 325);
  sg_grbl::temperature_set_heater_maxtemp(1, 325);

  if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
  {
    feature_print_control::idex_sys_set_extruder(true);
    gcode::active_extruder = 0;
    ccm_param.t_model.enable_invert_dir[E_AXIS] = false;
    ccm_param.t_model.enable_invert_dir[B_AXIS] = true;
    ccm_param.planner_settings.axis_steps_per_mm[E_AXIS] = EB_AXIS_STEPS_PER_UNIT_P3_PRO_IDEX;
    ccm_param.planner_settings.axis_steps_per_mm[B_AXIS] = EB_AXIS_STEPS_PER_UNIT_P3_PRO_IDEX;
    ccm_param.planner_settings.axis_steps_per_sqr_second[E_AXIS] = (unsigned long)(ccm_param.planner_settings.max_acceleration_mm_per_s2[E_AXIS] * ccm_param.planner_settings.axis_steps_per_mm[E_AXIS]);
    ccm_param.planner_settings.axis_steps_per_sqr_second[B_AXIS] = (unsigned long)(ccm_param.planner_settings.max_acceleration_mm_per_s2[B_AXIS] * ccm_param.planner_settings.axis_steps_per_mm[B_AXIS]);
  }
  else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
  {
    gcode::active_extruder = 0;
    ccm_param.t_model.enable_invert_dir[E_AXIS] = true;
    ccm_param.t_model.enable_invert_dir[B_AXIS] = false;
    ccm_param.planner_settings.axis_steps_per_mm[E_AXIS] = EB_AXIS_STEPS_PER_UNIT_P3_PRO_MIX;
    ccm_param.planner_settings.axis_steps_per_mm[B_AXIS] = EB_AXIS_STEPS_PER_UNIT_P3_PRO_MIX;
    ccm_param.planner_settings.axis_steps_per_sqr_second[E_AXIS] = (unsigned long)(ccm_param.planner_settings.max_acceleration_mm_per_s2[E_AXIS] * ccm_param.planner_settings.axis_steps_per_mm[E_AXIS]);
    ccm_param.planner_settings.axis_steps_per_sqr_second[B_AXIS] = (unsigned long)(ccm_param.planner_settings.max_acceleration_mm_per_s2[B_AXIS] * ccm_param.planner_settings.axis_steps_per_mm[B_AXIS]);
  }
  else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
  {
    gcode::active_extruder = 1;// 激光头使用喷头2
  }
  else
  {
    flash_param_t.extruder_type = EXTRUDER_TYPE_DUAL;
    feature_print_control::idex_sys_set_extruder(true);
    ccm_param.t_model.enable_invert_dir[E_AXIS] = true;
    flash_config_set_flag_save();
  }
}


#ifdef __cplusplus
} //extern "C" {
#endif

