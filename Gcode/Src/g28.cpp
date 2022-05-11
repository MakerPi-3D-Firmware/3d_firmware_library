#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif

extern float add_homeing[XYZ_NUM_AXIS];
extern unsigned long previous_xTaskGetTickCount_cmd;

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  static volatile bool home_all_axis = false;
  static volatile bool home_hit_z_endstop = false;
  static volatile bool home_code_seen_xyz[XYZ_NUM_AXIS] = {false};
  static volatile float home_code_value_xyz[XYZ_NUM_AXIS] = {0.0f};
  static volatile bool level_is_rise_z = false; //判断调平模式是否抬升Z轴

  static void g28_set_curr_position(void)
  {
    for (int i = 0; i < MAX_NUM_AXIS; i++)
    {
      ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
    }

    sg_grbl::planner_set_position(ccm_param.grbl_current_position);
    sg_grbl::st_synchronize();
  }

  // Z激光限位，先上升Z，移动xy，再Z归零
  static void laser_z_rise_first(void)
  {
    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      if (feature_print_control::idex_sys_is_extruder() ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_SINGLE ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_LASER ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
      {
        if (!level_is_rise_z)
        {
          g28_set_curr_position();
          sg_grbl::planner_set_position(ccm_param.grbl_current_position);
          sg_grbl::st_synchronize();
          //          if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
          sg_grbl::st_enable_endstops(false);
          //          else
          //            sg_grbl::st_enable_endstops(true);
          ccm_param.grbl_destination[(int)Z_AXIS] = ccm_param.grbl_current_position[Z_AXIS] + 5;

          if (ccm_param.grbl_destination[Z_AXIS] > ccm_param.t_model.xyz_max_pos[Z_AXIS] + add_homeing[Z_AXIS])
          {
            ccm_param.grbl_destination[Z_AXIS] = ccm_param.t_model.xyz_max_pos[Z_AXIS] + add_homeing[Z_AXIS];
          }

          process_buffer_line_normal(ccm_param.grbl_destination, homing_feedrate[Z_AXIS] / 60);
          sg_grbl::st_synchronize();
          sg_grbl::st_enable_endstops(true);
          ccm_param.grbl_destination[(int)Z_AXIS] = ccm_param.grbl_current_position[Z_AXIS];
          sg_grbl::planner_set_position(ccm_param.grbl_current_position);
          sg_grbl::st_synchronize();
          level_is_rise_z = true;
        }
      }
    }
  }

  static void home_quick(void)
  {
#ifdef QUICK_HOME
    float quit_home_feedrate = homing_feedrate[X_AXIS];

    if ((home_all_axis) || (parseGcodeBufHandle.codeSeen(axis_codes[X_AXIS]) && parseGcodeBufHandle.codeSeen(axis_codes[Y_AXIS]))) //first diagonal move
    {
      // 设置当前点XY为零点
      ccm_param.grbl_destination[(int)X_AXIS] = ccm_param.t_model.xyz_home_pos[X_AXIS] + add_homeing[X_AXIS];
      ccm_param.grbl_destination[(int)Y_AXIS] = ccm_param.t_model.xyz_home_pos[Y_AXIS] + add_homeing[Y_AXIS];
      g28_set_curr_position();
      laser_z_rise_first();
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(true);
      // xy再次往负方向移动，直至撞击限位
      ccm_param.grbl_destination[(int)X_AXIS] = 1.5f * ccm_param.t_model.xyz_max_length[(int)X_AXIS] * ccm_param.t_model.xyz_home_dir[(int)X_AXIS];
      ccm_param.grbl_destination[(int)Y_AXIS] = 1.5f * ccm_param.t_model.xyz_max_length[(int)Y_AXIS] * ccm_param.t_model.xyz_home_dir[(int)Y_AXIS];
      quit_home_feedrate = homing_feedrate[X_AXIS];

      if (homing_feedrate[Y_AXIS] < quit_home_feedrate)
      {
        quit_home_feedrate = homing_feedrate[Y_AXIS];
      }

      process_buffer_line_normal(ccm_param.grbl_destination, quit_home_feedrate / 60);
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(false);
      ccm_param.grbl_current_position[X_AXIS] = ccm_param.t_model.xyz_home_pos[X_AXIS] + add_homeing[X_AXIS]; // axis_is_at_home
      ccm_param.grbl_current_position[Y_AXIS] = ccm_param.t_model.xyz_home_pos[Y_AXIS] + add_homeing[Y_AXIS]; // axis_is_at_home
      sg_grbl::planner_set_position(ccm_param.grbl_current_position);
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(true);
      ccm_param.grbl_destination[X_AXIS] = ccm_param.grbl_current_position[X_AXIS];
      ccm_param.grbl_destination[Y_AXIS] = ccm_param.grbl_current_position[Y_AXIS];
      process_buffer_line_normal(ccm_param.grbl_destination, quit_home_feedrate / 60);
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(false);

      for (int i = 0; i < XYZ_NUM_AXIS; i++)
      {
        ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
      }

      sg_grbl::planner_set_position(ccm_param.grbl_current_position);
      sg_grbl::st_synchronize();
    }

#endif
  }

  void g28_get_home_pos_adding(const int dir, float &x, float &y, float &z)
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      if (feature_print_control::idex_sys_is_extruder())
      {
        x = ccm_param.t_model.xyz_home_pos[X_AXIS] + add_homeing[X_AXIS] -
            dir * flash_param_t.idex_ext0_home_pos_adding[0]; // axis_is_at_home
        y = ccm_param.t_model.xyz_home_pos[Y_AXIS] + add_homeing[Y_AXIS] -
            dir * flash_param_t.idex_ext0_home_pos_adding[1]; // axis_is_at_home
        z = ccm_param.t_model.xyz_home_pos[Z_AXIS] + add_homeing[Z_AXIS] -
            dir * flash_param_t.idex_ext0_home_pos_adding[2]; // axis_is_at_home
      }
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      x = ccm_param.t_model.xyz_home_pos[X_AXIS] + add_homeing[X_AXIS] -
          dir * flash_param_t.mix_ext0_home_pos_adding[0]; // axis_is_at_home
      y = ccm_param.t_model.xyz_home_pos[Y_AXIS] + add_homeing[Y_AXIS] -
          dir * flash_param_t.mix_ext0_home_pos_adding[1]; // axis_is_at_home
      z = ccm_param.t_model.xyz_home_pos[Z_AXIS] + add_homeing[Z_AXIS] -
          dir * flash_param_t.mix_ext0_home_pos_adding[2]; // axis_is_at_home
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      //      x = ccm_param.t_model.xyz_home_pos[X_AXIS] + add_homeing[X_AXIS] -
      //          dir * flash_param_t.laser_ext0_home_pos_adding[0]; // axis_is_at_home
      y = ccm_param.t_model.xyz_home_pos[Y_AXIS] + add_homeing[Y_AXIS] -
          dir * flash_param_t.laser_ext0_home_pos_adding[1]; // axis_is_at_home
      //      z = ccm_param.t_model.xyz_home_pos[Z_AXIS] + add_homeing[Z_AXIS] -
      //          dir * flash_param_t.laser_ext0_home_pos_adding[2]; // axis_is_at_home
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
    {
      x = ccm_param.t_model.xyz_home_pos[X_AXIS] + add_homeing[X_AXIS] -
          dir * flash_param_t.laser_ext0_home_pos_adding[0]; // axis_is_at_home
      y = ccm_param.t_model.xyz_home_pos[Y_AXIS] + add_homeing[Y_AXIS] -
          dir * flash_param_t.laser_ext0_home_pos_adding[1]; // axis_is_at_home
      z = ccm_param.t_model.xyz_home_pos[Z_AXIS] + add_homeing[Z_AXIS] -
          dir * flash_param_t.laser_ext0_home_pos_adding[2]; // axis_is_at_home
    }
    else
    {
      x = 0.0f;
      y = 0.0f;
      z = 0.0f;
    }
  }

  void home_axis_move_to(const int axis, bool is_sync_z)
  {
    const int axis_home_dir = ccm_param.t_model.xyz_home_dir[axis];
    const float axis_feedrate = homing_feedrate[axis];
    ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis];
    ccm_param.grbl_current_position[axis] = ccm_param.t_model.xyz_home_pos[axis];
    sg_grbl::planner_set_position_basic(ccm_param.grbl_current_position, is_sync_z);
    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(true);
    ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis] + 1.5f * ccm_param.t_model.xyz_max_length[axis] * axis_home_dir;
    process_buffer_line_basic(ccm_param.grbl_destination, axis_feedrate / 60, 100, 100, 0, is_sync_z);
    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(false);
    ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis];
    ccm_param.grbl_current_position[axis] = ccm_param.t_model.xyz_home_pos[axis];
    sg_grbl::planner_set_position_basic(ccm_param.grbl_current_position, is_sync_z);
    sg_grbl::st_synchronize();
  }

  void home_axis_move_to(const int axis)
  {
    home_axis_move_to(axis, true);
  }

  void home_axis_process_basic(int axis, bool is_sync_z, float feedrate_factor, bool is_retract)
  {
    const int axis_home_dir = ccm_param.t_model.xyz_home_dir[axis];
    const float axis_feedrate = homing_feedrate[axis] * feedrate_factor;

    if (axis == X_AXIS)
    {
      if (feature_print_control::idex_sys_is_extruder() ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      {
        home_axis_move_to(X2_AXIS);
        g28_set_curr_position();
      }
    }
    else if (axis == X2_AXIS)
    {
      if (feature_print_control::idex_sys_is_extruder() ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      {
        home_axis_move_to(X_AXIS);
        g28_set_curr_position();
      }
    }

    ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis];
    ccm_param.grbl_current_position[axis] = ccm_param.t_model.xyz_home_pos[axis];
    sg_grbl::planner_set_position_basic(ccm_param.grbl_current_position, is_sync_z);
    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(true);
    ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis] + 1.5f * ccm_param.t_model.xyz_max_length[axis] * axis_home_dir;
    process_buffer_line_basic(ccm_param.grbl_destination, axis_feedrate / 60, 100, 100, 0, is_sync_z);
    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(false);
    ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis];
    ccm_param.grbl_current_position[axis] = ccm_param.t_model.xyz_home_pos[axis];
    sg_grbl::planner_set_position_basic(ccm_param.grbl_current_position, is_sync_z);
    sg_grbl::st_synchronize();

    if (is_retract)
    {
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(true);
      ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis] - ccm_param.t_model.xyz_home_retract_mm[axis] * axis_home_dir;
      process_buffer_line_basic(ccm_param.grbl_destination, axis_feedrate / 60, 100, 100, 0, is_sync_z);
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(true);
      ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis] + 2.0f * ccm_param.t_model.xyz_home_retract_mm[axis] * axis_home_dir;
      process_buffer_line_basic(ccm_param.grbl_destination, axis_feedrate / 2 / 60, 100, 100, 0, is_sync_z);
      sg_grbl::st_synchronize();
    }

    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(false);
    ccm_param.grbl_current_position[axis] = ccm_param.t_model.xyz_home_pos[axis] + add_homeing[axis]; // axis_is_at_home
    ccm_param.grbl_destination[axis] = ccm_param.grbl_current_position[axis];
  }

  void home_axis_process(int axis, bool is_sync_z)
  {
    home_axis_process_basic(axis, is_sync_z, 1.0f, true);
  }

  static void home_axis_sync_z(void)
  {
    // Avoid triggering single Z to zero when the nozzle (E0 and E1) Z is not limited,
    // resulting in an increase in the height difference of nozzle (E0 and E1) Z
    if (!sg_grbl::st_axis_xyz_read_min(Z_AXIS) && !sg_grbl::st_axis_xyz_read_min(Z2_AXIS))
    {
      home_axis_process(Z_AXIS, true);
    }

    if (!sg_grbl::st_axis_xyz_read_min(Z_AXIS) || !sg_grbl::st_axis_xyz_read_min(Z2_AXIS))
    {
      osDelay(500);
      g28_set_curr_position();
      home_axis_process_basic(Z_AXIS, false, 0.5f, false);
      osDelay(500);
      g28_set_curr_position();
      home_axis_process_basic(Z2_AXIS, false, 0.5f, false);
    }
  }

  static void home_axis_z(void)
  {
    if (ccm_param.t_sys.is_dual_z_axis)
    {
      if (!home_all_axis)
      {
        if (!(parseGcodeBufHandle.codeSeen(axis_codes[X_AXIS])))
        {
          g28_set_curr_position();
          home_axis_process(X_AXIS, true);
        }

        if (!(parseGcodeBufHandle.codeSeen(axis_codes[Y_AXIS])))
        {
          g28_set_curr_position();
          home_axis_process(Y_AXIS, true);
        }
      }

      g28_set_curr_position();
      home_axis_process(Z_AXIS, true);
      osDelay(500);
      home_axis_sync_z();
    }
    else
    {
      g28_set_curr_position();
      home_axis_process(Z_AXIS, false);
      osDelay(500);
    }
  }

  static void home_axis(void)
  {
    if ((home_all_axis) || (parseGcodeBufHandle.codeSeen(axis_codes[X_AXIS])))
    {
      g28_set_curr_position();
      home_axis_process(X_AXIS, true);

      if (feature_print_control::idex_sys_is_extruder() ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_LASER ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_MIX) // idex 双头
      {
        g28_set_curr_position();
        home_axis_process(X2_AXIS, true);
      }
    }

    if ((home_all_axis) || (parseGcodeBufHandle.codeSeen(axis_codes[Y_AXIS])))
    {
      g28_set_curr_position();
      home_axis_process(Y_AXIS, true);
    }

    if ((home_all_axis) || (parseGcodeBufHandle.codeSeen(axis_codes[Z_AXIS])))
    {
#if (Z_HOME_DIR == 1)
      //TODO
#elif (Z_HOME_DIR == -1)                      // If homing towards BED do Z last
      home_axis_z();
#endif
    }
  }

  static void is_at_z_max(void)
  {
    // 上电第一次归零确认z最大限位状态
    if (sg_grbl::st_get_z_max_endstops_status())
    {
      home_hit_z_endstop = true;
      ccm_param.grbl_destination[(int)Z_AXIS] = 10;//ccm_param.t_model.xyz_home_pos[Z_AXIS] + add_homeing[Z_AXIS];
      g28_set_curr_position();
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(true);
      // xy再次往负方向移动，直至撞击限位
      ccm_param.grbl_destination[(int)Z_AXIS] = 0;
      process_buffer_line_normal(ccm_param.grbl_destination, homing_feedrate[Z_AXIS] / 60);
      sg_grbl::st_synchronize();
    }
  }

  static void home_get_axis_status(void)
  {
    for (int i = 0; i < XYZ_NUM_AXIS; i++)
    {
      home_code_seen_xyz[i] = parseGcodeBufHandle.codeSeen(axis_codes[i]);

      if (home_code_seen_xyz[i])
      {
        if (parseGcodeBufHandle.codeValueLong() != 0)
        {
          home_code_value_xyz[i] = parseGcodeBufHandle.codeValue();
        }
        else
        {
          home_code_value_xyz[i] = (int)ccm_param.t_model.xyz_home_pos[i];
        }

        if (flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG  || flash_param_t.extruder_type != EXTRUDER_TYPE_SINGLE)
        {
          if (X_AXIS == i)
          {
            home_code_value_xyz[X2_AXIS] = (int)ccm_param.t_model.xyz_home_pos[X2_AXIS];
          }
        }
      }
      else
      {
        home_code_value_xyz[i] = (int)ccm_param.t_model.xyz_home_pos[i];
      }
    }

    home_all_axis = !(home_code_seen_xyz[X_AXIS] || home_code_seen_xyz[Y_AXIS] || home_code_seen_xyz[Z_AXIS]);

    if (flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG  || flash_param_t.extruder_type != EXTRUDER_TYPE_SINGLE)
    {
      if (home_all_axis)
      {
        home_code_value_xyz[X2_AXIS] = (int)ccm_param.t_model.xyz_home_pos[X2_AXIS];
      }
    }

    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
    }
  }

  static void home_set_curr_position(void)
  {
    //    // 设置xyz归零坐标
    //    float x, y, z;
    //    g28_get_home_pos_adding(-1, x, y, z);
    if (home_all_axis || home_code_seen_xyz[X_AXIS])
    {
      ccm_param.grbl_current_position[X_AXIS] = home_code_value_xyz[X_AXIS] + add_homeing[X_AXIS];
      ccm_param.grbl_destination[X_AXIS] = home_code_value_xyz[X_AXIS] + add_homeing[X_AXIS];

      if (flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG  || flash_param_t.extruder_type != EXTRUDER_TYPE_SINGLE)
      {
        ccm_param.grbl_current_position[X2_AXIS] = home_code_value_xyz[X2_AXIS] + add_homeing[X2_AXIS];
        ccm_param.grbl_destination[X2_AXIS] = home_code_value_xyz[X2_AXIS] + add_homeing[X2_AXIS];
      }
    }

    if (home_all_axis || home_code_seen_xyz[Y_AXIS])
    {
      ccm_param.grbl_current_position[Y_AXIS] = home_code_value_xyz[Y_AXIS] + add_homeing[Y_AXIS];
      ccm_param.grbl_destination[Y_AXIS] = home_code_value_xyz[Y_AXIS] + add_homeing[Y_AXIS];
    }

    if (home_all_axis || home_code_seen_xyz[Z_AXIS])
    {
      if ((flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG || flash_param_t.extruder_type == EXTRUDER_TYPE_SINGLE) && home_all_axis)
      {
        ccm_param.grbl_current_position[Z_AXIS] = ccm_param.t_model.xyz_home_pos[Z_AXIS];
        ccm_param.grbl_destination[Z_AXIS] = ccm_param.t_model.xyz_home_pos[Z_AXIS];
      }
      else
      {
        ccm_param.grbl_current_position[Z_AXIS] = home_code_value_xyz[Z_AXIS] + add_homeing[Z_AXIS];
        ccm_param.grbl_destination[Z_AXIS] = home_code_value_xyz[Z_AXIS] + add_homeing[Z_AXIS];
        //      ccm_param.grbl_current_position[Z_AXIS] -= z;
        //      ccm_param.grbl_destination[Z_AXIS] -= z;
      }
    }

    sg_grbl::planner_set_position(ccm_param.grbl_current_position);
    sg_grbl::st_synchronize();
  }

  void g28_bed_level_set_curr_offset(void)
  {
    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      if (home_all_axis || home_code_seen_xyz[Z_AXIS])
      {
        // 设置xyz归零坐标
        float x, y, z;
        g28_get_home_pos_adding(-1, x, y, z);
        ccm_param.grbl_current_position[Z_AXIS] += z;
        ccm_param.grbl_destination[Z_AXIS] += z;
        process_buffer_line_normal(ccm_param.grbl_current_position, homing_feedrate[Z_AXIS] / 4 / 60);
        sg_grbl::st_synchronize();
        ccm_param.grbl_current_position[Z_AXIS] = home_code_value_xyz[Z_AXIS] + add_homeing[Z_AXIS];
        ccm_param.grbl_destination[Z_AXIS] = home_code_value_xyz[Z_AXIS] + add_homeing[Z_AXIS];
        sg_grbl::planner_set_position(ccm_param.grbl_current_position);
        sg_grbl::st_synchronize();
      }
    }
  }

  void g28_process(void)
  {
    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      level_is_rise_z = false; // 喷头抬升
      sg_grbl::plan_set_process_auto_bed_level_status(false);
    }

    ccm_param.t_sys.is_gcode_g28_done = false;
    previous_xTaskGetTickCount_cmd = xTaskGetTickCount();
    home_get_axis_status();

    if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
    {
      is_at_z_max(); // 判断是否在z最大位置
    }

    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(true);
#if (Z_HOME_DIR == 1)                      // If homing away from BED do Z first
    // TODO
#elif (Z_HOME_DIR == -1)
    // TODO
#endif
    home_quick();       // 快速归零

    if (flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG)
    {
      if (!home_hit_z_endstop)
      {
        laser_z_rise_first(); // 激光头归零，未抬高Z50mm，先抬高Z，然后激光头X归零
      }
    }

    home_axis();        // 各轴归零
#ifdef ENDSTOPS_ONLY_FOR_HOMING
    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(false);
#endif
    {
      // 设置xyz归零坐标
      home_set_curr_position();
    }
    previous_xTaskGetTickCount_cmd = xTaskGetTickCount();

    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      sg_grbl::plan_set_process_auto_bed_level_status(true);
    }

    if (!parseGcodeBufHandle.codeSeen('S') && parseGcodeBufHandle.codeValueLong() != -1)
    {
      g28_bed_level_set_curr_offset();
    }

    if (ccm_param.t_sys_data_current.model_id == K5 && ccm_param.t_model.xyz_home_dir[Z_AXIS] == 1)
    {
      if (home_all_axis || home_code_seen_xyz[Z_AXIS])
      {
        if (ccm_param.t_sys_data_current.poweroff_rec_z_max_value > 0.0f)
        {
          ccm_param.grbl_current_position[Z_AXIS] = ccm_param.t_sys_data_current.poweroff_rec_z_max_value;
          ccm_param.grbl_destination[Z_AXIS] = ccm_param.t_sys_data_current.poweroff_rec_z_max_value;
          sg_grbl::planner_set_position(ccm_param.grbl_current_position);
          sg_grbl::st_synchronize();
        }

        ccm_param.grbl_current_position[Z_AXIS] = 0.0f;
        ccm_param.grbl_destination[Z_AXIS] = 0.0f;
        ccm_param.t_gui.move_xyz_pos[Z_AXIS] = 0.0f;
        process_buffer_line_normal(ccm_param.grbl_destination, homing_feedrate[Z_AXIS] / 60);
        sg_grbl::st_synchronize();
      }
    }

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      active_extruder = 1;
      ccm_param.grbl_current_position[Z_AXIS] = flash_param_t.laser_focus_z_height;
      ccm_param.grbl_destination[Z_AXIS] = flash_param_t.laser_focus_z_height;
      process_buffer_line_normal(ccm_param.grbl_destination, homing_feedrate[Z_AXIS] / 60);
      sg_grbl::st_synchronize();
    }

    ccm_param.t_sys.is_gcode_g28_done = true;
  }


}






