#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif
sg_grbl::vector_3 level_points[LEVEL_POS_COUNT];
#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  static const float z_feedrate = homing_feedrate[Z_AXIS] ;
  static const float xy_feedrate = (homing_feedrate[X_AXIS] + homing_feedrate[Y_AXIS]) / 2.0f ;

  // y=kx+b
  static volatile float line_k_slope = 1.0f;
  static volatile float line_b_offset = 1.0f;

  static void g29_abl_set_curr_position(void)
  {
    for (int i = 0; i < MAX_NUM_AXIS; i++)
    {
      ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
    }

    sg_grbl::planner_set_position(ccm_param.grbl_current_position);
  }

  static void g29_abl_xy_homing(void)
  {
    sg_grbl::st_synchronize();
    // Z up 10mm
    ccm_param.grbl_destination[Z_AXIS] = 0;
    g29_abl_set_curr_position();
    // Endstop stop
    sg_grbl::st_enable_endstops(false);
    ccm_param.grbl_destination[Z_AXIS] += 20;
    process_buffer_line_normal(ccm_param.grbl_destination, z_feedrate / 60);
    sg_grbl::st_synchronize();
    // Endstop start
    sg_grbl::st_enable_endstops(true);
    // X home
    g29_abl_set_curr_position();
    ccm_param.grbl_destination[X_AXIS] = ccm_param.t_model.xyz_home_dir[X_AXIS] * ccm_param.t_model.xyz_home_pos[X_AXIS] * 1.5f;
    process_buffer_line_normal(ccm_param.grbl_destination, xy_feedrate / 60);
    sg_grbl::st_synchronize();
    // Endstop stop
    sg_grbl::st_enable_endstops(false);
    ccm_param.grbl_destination[X_AXIS] = ccm_param.t_model.xyz_home_pos[X_AXIS];
    // Y home
    // Endstop start
    sg_grbl::st_enable_endstops(true);
    g29_abl_set_curr_position();
    ccm_param.grbl_destination[Y_AXIS] = ccm_param.t_model.xyz_home_dir[Y_AXIS] * ccm_param.t_model.xyz_home_pos[Y_AXIS] * 1.5f;
    process_buffer_line_normal(ccm_param.grbl_destination, xy_feedrate / 60);
    sg_grbl::st_synchronize();
    // Endstop stop
    sg_grbl::st_enable_endstops(false);
    ccm_param.grbl_destination[Y_AXIS] = ccm_param.t_model.xyz_home_pos[Y_AXIS];
    g29_abl_set_curr_position();
    // Reset the z value to avoid scraping the platform
    ccm_param.grbl_destination[Z_AXIS] = 0;
    g29_abl_set_curr_position();
  }

  static void g29_abl_get_z_endstop_value(float z_feedrate_factor)
  {
    // Z move to hit platform
    g29_abl_set_curr_position();
    // Clean z hit min endstop status
    sg_grbl::st_check_endstop_z_hit_min();
    // Endstop start
    sg_grbl::st_enable_endstops(true);
    ccm_param.grbl_destination[Z_AXIS] = -999.0f;
    process_buffer_line_normal(ccm_param.grbl_destination, z_feedrate * z_feedrate_factor / 60);
    sg_grbl::st_synchronize();
    // Endstop stop
    sg_grbl::st_enable_endstops(false);

    while (!sg_grbl::st_check_endstop_z_hit_min())
    {
      osDelay(50);
    }

    // Set z endstop value
    ccm_param.grbl_destination[Z_AXIS] = sg_grbl::st_get_endstops_len(Z_AXIS);
    g29_abl_set_curr_position();
  }

  static void g29_abl_get_level_pos_z(sg_grbl::vector_3 &pos)
  {
    sg_grbl::st_synchronize();
    // Endstop stop
    sg_grbl::st_enable_endstops(false);
    // Z move to 10mm
    ccm_param.grbl_destination[Z_AXIS] = 10;
    process_buffer_line_normal(ccm_param.grbl_destination, z_feedrate / 60);
    sg_grbl::st_synchronize();
    // XY move to pos(x, y)
    g29_abl_set_curr_position();
    ccm_param.grbl_destination[X_AXIS] = pos.x;
    ccm_param.grbl_destination[Y_AXIS] = pos.y;
    process_buffer_line_normal(ccm_param.grbl_destination, xy_feedrate / 60);
    sg_grbl::st_synchronize();
    g29_abl_get_z_endstop_value(1.5f);
    // Z move to +5mm
    g29_abl_set_curr_position();
    ccm_param.grbl_destination[Z_AXIS] += 2;
    process_buffer_line_normal(ccm_param.grbl_destination, z_feedrate / 2 / 60);
    sg_grbl::st_synchronize();
    g29_abl_get_z_endstop_value(0.25f);
    pos.z = ccm_param.grbl_destination[Z_AXIS];
  }

  // Relative center point height
  static void g92_abl_get_z_relative_center(int index)
  {
    g29_abl_get_level_pos_z(level_points[LEVEL_POS_INDEX_MIDDLE]); // Get the Z height from the middle position
    level_points[LEVEL_POS_INDEX_MIDDLE].z = 0.0f;
    ccm_param.grbl_destination[Z_AXIS] = 0;
    g29_abl_set_curr_position();
    g29_abl_get_level_pos_z(level_points[index]);
  }

  void g92_abl_level_pos_z_process(void)
  {
    g29_abl_xy_homing();
    g92_abl_get_z_relative_center(LEVEL_POS_INDEX_LEFT_FRONT); // Get the Z height from the left_front position
    g92_abl_get_z_relative_center(LEVEL_POS_INDEX_LEFT_BACK); // Get the Z height from the left_back position
    g92_abl_get_z_relative_center(LEVEL_POS_INDEX_RIGHT_BACK); // Get the Z height from the right_back position
    g92_abl_get_z_relative_center(LEVEL_POS_INDEX_RIGHT_FRONT); // Get the Z height from the right_front position
  }

  static void g29_abl_set_level_pos(const float bed_offset[2], const float home_offset[3], const float x_offset[2], const float y_offset[2])
  {
    level_points[LEVEL_POS_INDEX_LEFT_FRONT].x = bed_offset[0] + x_offset[0];
    level_points[LEVEL_POS_INDEX_LEFT_BACK].x = bed_offset[0] + x_offset[0];
    level_points[LEVEL_POS_INDEX_RIGHT_BACK].x = bed_offset[1] + x_offset[1];
    level_points[LEVEL_POS_INDEX_RIGHT_FRONT].x = bed_offset[1] + x_offset[1];
    level_points[LEVEL_POS_INDEX_MIDDLE].x = bed_offset[0] + fabs(bed_offset[1] - bed_offset[0]) / 2;
    float y_offset_tmp = home_offset[1];

    if (P3_Pro == ccm_param.t_sys_data_current.model_id || F400TP == ccm_param.t_sys_data_current.model_id)
    {
      if (y_offset_tmp > 20) y_offset_tmp = 20;
    }

    level_points[LEVEL_POS_INDEX_LEFT_FRONT].y = y_offset_tmp + y_offset[0];
    level_points[LEVEL_POS_INDEX_RIGHT_FRONT].y = y_offset_tmp + 10 + y_offset[0];
    level_points[LEVEL_POS_INDEX_LEFT_BACK].y = y_offset_tmp + ccm_param.t_model.xyz_max_pos[Y_AXIS] + y_offset[1];
    level_points[LEVEL_POS_INDEX_RIGHT_BACK].y = y_offset_tmp + ccm_param.t_model.xyz_max_pos[Y_AXIS] + y_offset[1];

    if (P3_Pro == ccm_param.t_sys_data_current.model_id || F400TP == ccm_param.t_sys_data_current.model_id)
    {
      level_points[LEVEL_POS_INDEX_MIDDLE].y = (y_offset_tmp + ccm_param.t_model.xyz_max_pos[Y_AXIS] / 2.0f) + 25;
    }
    else if (F300TP == ccm_param.t_sys_data_current.model_id)
    {
      level_points[LEVEL_POS_INDEX_MIDDLE].y = (y_offset_tmp + ccm_param.t_model.xyz_max_pos[Y_AXIS] / 2.0f) - 60;
    }
    else
    {
      level_points[LEVEL_POS_INDEX_MIDDLE].y = (y_offset_tmp + ccm_param.t_model.xyz_max_pos[Y_AXIS] / 2.0f);
    }
  }


  static void g29_abl_reset_level_pos(void)
  {
    if (P3_Pro == ccm_param.t_sys_data_current.model_id)
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
      {
        float x_offsets[2] = {0.0f, -30.0f};
        float y_offsets[2] = {20.0f, -20.0f};
        g29_abl_set_level_pos((float *)flash_param_t.idex_extruder_0_bed_offset, (float *)flash_param_t.idex_ext0_home_pos_adding, x_offsets, y_offsets);
      }
      else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
      {
        float x_offsets[2] = {-10.0f, -20.0f};
        float y_offsets[2] = {20.0f, -20.0f};
        g29_abl_set_level_pos((float *)flash_param_t.mix_extruder_0_bed_offset, (float *)flash_param_t.mix_ext0_home_pos_adding, x_offsets, y_offsets);
      }
      else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      {
        float x_offsets[2] = {-10.0f, -20.0f};
        float y_offsets[2] = {20.0f, -20.0f};
        g29_abl_set_level_pos((float *)flash_param_t.laser_extruder_0_bed_offset, (float *)flash_param_t.laser_ext0_home_pos_adding, x_offsets, y_offsets);
      }
    }

    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id)
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
      {
        float x_offsets[2] = {-10.0f, -20.0f};
        float y_offsets[2] = {20.0f, 0.0f};
        g29_abl_set_level_pos((float *)flash_param_t.idex_extruder_0_bed_offset, (float *)flash_param_t.idex_ext0_home_pos_adding, x_offsets, y_offsets);
      }
      else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
      {
        float x_offsets[2] = {-10.0f, -30.0f};
        float y_offsets[2] = {0.0f, -20.0f};
        g29_abl_set_level_pos((float *)flash_param_t.mix_extruder_0_bed_offset, (float *)flash_param_t.mix_ext0_home_pos_adding, x_offsets, y_offsets);
      }
      else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      {
        float x_offsets[2] = {-10.0f, -30.0f};
        float y_offsets[2] = {0.0f, -20.0f};
        g29_abl_set_level_pos((float *)flash_param_t.laser_extruder_0_bed_offset, (float *)flash_param_t.laser_ext0_home_pos_adding, x_offsets, y_offsets);
      }
    }
    else if (K600 == ccm_param.t_sys_data_current.model_id || F400TP == ccm_param.t_sys_data_current.model_id)
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
      {
        float x_offsets[2] = {+20.0f, -15.0f};
        float y_offsets[2] = {40.0f, -110.0f};
        g29_abl_set_level_pos((float *)flash_param_t.idex_extruder_0_bed_offset, (float *)flash_param_t.idex_ext0_home_pos_adding, x_offsets, y_offsets);
      }
    }
    else if (F300TP == ccm_param.t_sys_data_current.model_id)
    {
      {
        float x_offsets[2] = {+30.0f, +10.0f};
        float y_offsets[2] = {0.0f, -140.0f};
        g29_abl_set_level_pos((float *)flash_param_t.idex_extruder_0_bed_offset, (float *)flash_param_t.idex_ext0_home_pos_adding, x_offsets, y_offsets);
      }
    }
    else if (K5 == ccm_param.t_sys_data_current.model_id)
    {
      level_points[LEVEL_POS_INDEX_LEFT_FRONT].x += 20;
      level_points[LEVEL_POS_INDEX_LEFT_FRONT].y += 20;
      level_points[LEVEL_POS_INDEX_LEFT_BACK].x += 20;
      level_points[LEVEL_POS_INDEX_LEFT_BACK].y -= 30;
      level_points[LEVEL_POS_INDEX_RIGHT_BACK].x -= 40;
      level_points[LEVEL_POS_INDEX_RIGHT_BACK].y -= 30;
      level_points[LEVEL_POS_INDEX_RIGHT_FRONT].x -= 40;
      level_points[LEVEL_POS_INDEX_RIGHT_FRONT].y += 20;
    }
  }

  void g92_abl_init(void)
  {
    float x_min = 0.0f;
    float y_min = 0.0f;
    float x_max = ccm_param.t_model.xyz_max_pos[X_AXIS];
    float y_max = ccm_param.t_model.xyz_max_pos[Y_AXIS];
    float x_mid = ccm_param.t_model.xyz_max_pos[X_AXIS] / 2.0f;
    float y_mid = ccm_param.t_model.xyz_max_pos[Y_AXIS] / 2.0f;
    level_points[LEVEL_POS_INDEX_MIDDLE].x = x_mid;
    level_points[LEVEL_POS_INDEX_MIDDLE].y = y_mid;
    level_points[LEVEL_POS_INDEX_LEFT_FRONT].x = x_min;
    level_points[LEVEL_POS_INDEX_LEFT_FRONT].y = y_min;
    level_points[LEVEL_POS_INDEX_LEFT_BACK].x = x_min;
    level_points[LEVEL_POS_INDEX_LEFT_BACK].y = y_max;
    level_points[LEVEL_POS_INDEX_RIGHT_BACK].x = x_max;
    level_points[LEVEL_POS_INDEX_RIGHT_BACK].y = y_max;
    level_points[LEVEL_POS_INDEX_RIGHT_FRONT].x = x_max;
    level_points[LEVEL_POS_INDEX_RIGHT_FRONT].y = y_min;
    g29_abl_reset_level_pos();

    // Get level pos z height in flash
    for (int i = 0; i < LEVEL_POS_COUNT; i++)
    {
      level_points[i].z = flash_param_t.level_pos_z[i];
    }

    sg_grbl::plan_init_offset1();
    // Get the slope of point left_back and point right_front
    line_k_slope = (level_points[LEVEL_POS_INDEX_LEFT_BACK].y - level_points[LEVEL_POS_INDEX_RIGHT_FRONT].y) /
                   (level_points[LEVEL_POS_INDEX_LEFT_BACK].x - level_points[LEVEL_POS_INDEX_RIGHT_FRONT].x);
    line_b_offset = level_points[LEVEL_POS_INDEX_LEFT_BACK].y - level_points[LEVEL_POS_INDEX_LEFT_BACK].x * line_k_slope;
  }

  int g29_abl_get_matrix_index(sg_grbl::vector_3 point)
  {
    static int pre_result = 0;
    int result = 0;
    float value = point.x * line_k_slope + line_b_offset - point.y;

    if (value < 0.0f) // Point above line
    {
      result = 1;
    }
    else if (value > 0.0f)  // Point below line
    {
      result = 0;
    }
    else // Point on line
    {
      result = pre_result;
    }

    pre_result = result;
    return result;
  }

  void g29_abl_process(void)
  {
    sg_grbl::st_synchronize();
    plan_bed_level_matrix.set_to_identity();
    sg_grbl::plan_set_process_auto_bed_level_status(false);
    g92_abl_level_pos_z_process();
    taskENTER_CRITICAL();

    for (int i = 0; i < LEVEL_POS_COUNT; i++)
    {
      flash_param_t.level_pos_z[i] = level_points[i].z;
    }

    // mid-left matrix
    sg_grbl::vector_3 planeNormal = sg_grbl::vector_3::get_plane_normal(
                                      level_points[LEVEL_POS_INDEX_LEFT_FRONT],
                                      level_points[LEVEL_POS_INDEX_RIGHT_FRONT],
                                      level_points[LEVEL_POS_INDEX_LEFT_BACK]);
    flash_param_t.matrix_front_left = sg_grbl::matrix_3x3::create_look_at(planeNormal);
    // mid-right matrix
    planeNormal = sg_grbl::vector_3::get_plane_normal(
                    level_points[LEVEL_POS_INDEX_LEFT_BACK],
                    level_points[LEVEL_POS_INDEX_LEFT_FRONT],
                    level_points[LEVEL_POS_INDEX_RIGHT_BACK]).get_normal();
    flash_param_t.matrix_left_back = sg_grbl::matrix_3x3::create_look_at(planeNormal);
    // mid-front matrix
    planeNormal = sg_grbl::vector_3::get_plane_normal(
                    level_points[LEVEL_POS_INDEX_RIGHT_BACK],
                    level_points[LEVEL_POS_INDEX_LEFT_BACK],
                    level_points[LEVEL_POS_INDEX_RIGHT_FRONT]).get_normal();
    flash_param_t.matrix_back_right = sg_grbl::matrix_3x3::create_look_at(planeNormal);
    // mid-back matrix
    planeNormal = sg_grbl::vector_3::get_plane_normal(
                    level_points[LEVEL_POS_INDEX_RIGHT_FRONT],
                    level_points[LEVEL_POS_INDEX_RIGHT_BACK],
                    level_points[LEVEL_POS_INDEX_LEFT_FRONT]).get_normal();
    flash_param_t.matrix_right_front = sg_grbl::matrix_3x3::create_look_at(planeNormal);
    sg_grbl::plan_init_offset1();
    flash_param_t.is_process_bedlevel = 1;
    taskEXIT_CRITICAL();
    flash_config_set_flag_save();
    sg_grbl::plan_set_process_auto_bed_level_status(true);
  }

}











