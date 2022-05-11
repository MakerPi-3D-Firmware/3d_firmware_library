#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif

#define EXTRUDER_1_Z_MOVE_F400TP 0.0f
#define EXTRUDER_1_Z_MOVE_P2_PRO 0.0f
#define EXTRUDER_1_Z_MOVE_P3_PRO 0.0f

extern float feedrate;
extern uint8_t tmp_extruder;


#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  volatile uint8_t active_extruder = 0;

  static void t0_process(void);
  static void t1_process(void);

  extern void home_axis_process(int axis, bool is_sync_z);

  static void t0_process(void)
  {
    if (feature_print_control::idex_sys_is_extruder() && ccm_param.t_sys.is_gcode_g28_done)
    {
      ccm_param.grbl_destination[X_AXIS] = 0;
      ccm_param.grbl_current_position[X_AXIS] = ccm_param.grbl_destination[X_AXIS];
      sg_grbl::planner_set_position(ccm_param.grbl_current_position);
    }

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      sg_grbl::st_synchronize();

      if (feature_print_control::idex_sys_is_extruder())
        active_extruder = 0;

      osDelay(1000);
      feedrate = homing_feedrate[Z_AXIS];
      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      sg_grbl::st_synchronize();

      for (int i = 0; i < XYZ_NUM_AXIS; i++)
      {
        ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
      }
    }
  }


  static void t1_process(void)
  {
    if (feature_print_control::idex_sys_is_extruder() && ccm_param.t_sys.is_gcode_g28_done)
    {
      ccm_param.grbl_destination[X_AXIS] = ccm_param.t_model.xyz_max_pos[X_AXIS];
      ccm_param.grbl_current_position[X_AXIS] = ccm_param.grbl_destination[X_AXIS];
      sg_grbl::planner_set_position(ccm_param.grbl_current_position);
    }

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      feedrate = homing_feedrate[Z_AXIS];
      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      sg_grbl::st_synchronize();

      for (int i = 0; i < XYZ_NUM_AXIS; i++)
      {
        ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
      }

      if (feature_print_control::idex_sys_is_extruder())
        active_extruder = 1;

      osDelay(1000);
    }
  }

  static void idex_x_move_to_home(int axis, int change_axis)
  {
    sg_grbl::st_synchronize();

    for (int i = 0; i < MAX_NUM_AXIS; i++)
    {
      ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
    }

    // Not in home position
    if (!user_is_float_data_equ(ccm_param.grbl_destination[axis], ccm_param.t_model.xyz_home_pos[axis]))
    {
      float axis_dest_value = ccm_param.grbl_destination[axis];
      float axis_y_dest_value = ccm_param.grbl_destination[Y_AXIS];
      // 喷嘴远离平台，避免调平导致刮平台
      ccm_param.grbl_destination[Z_AXIS] = ccm_param.grbl_destination[Z_AXIS] + 5;
      feedrate = homing_feedrate[Z_AXIS];
      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      sg_grbl::st_synchronize();
      // X轴回零
      sg_grbl::st_enable_endstops(true) ;
      ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis] - ccm_param.t_model.xyz_home_dir[axis] * 10.0f;
      //      ccm_param.grbl_destination[Y_AXIS] = ccm_param.t_model.xyz_home_pos[Y_AXIS] - ccm_param.t_model.xyz_home_dir[Y_AXIS] * 10.0f;
      feedrate = homing_feedrate[axis];
      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      sg_grbl::st_synchronize();
      sg_grbl::st_enable_endstops(false);
      //      // X轴回零
      //      sg_grbl::st_enable_endstops(true) ;
      //      ccm_param.grbl_destination[axis] = ccm_param.t_model.xyz_home_pos[axis] - ccm_param.t_model.xyz_home_dir[axis] * 10.0f;
      //      feedrate = homing_feedrate[axis];
      //      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      //      sg_grbl::st_synchronize();
      //      sg_grbl::st_enable_endstops(false);
      //      // Y轴回零
      //      sg_grbl::st_enable_endstops(true) ;
      //      ccm_param.grbl_destination[Y_AXIS] = ccm_param.t_model.xyz_home_pos[Y_AXIS] - ccm_param.t_model.xyz_home_dir[Y_AXIS] * 10.0f;
      //      feedrate = homing_feedrate[Y_AXIS];
      //      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      //      sg_grbl::st_synchronize();
      //      sg_grbl::st_enable_endstops(false);
      //
      ccm_param.grbl_destination[change_axis] = axis_dest_value;
      ccm_param.grbl_destination[Y_AXIS] = axis_y_dest_value;

      if (feature_print_control::idex_sys_is_normal() && 1 == active_extruder && change_axis == X2_AXIS) // idex结构
      {
        sg_grbl::plan_compensation_destination_idex_basic(1, X2_AXIS, 0.0f);
        sg_grbl::plan_compensation_destination_idex_basic(1, Y_AXIS, 0.0f);
      }

      feedrate = homing_feedrate[change_axis];
      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      sg_grbl::st_synchronize();
      // 喷嘴返回Z位置
      ccm_param.grbl_destination[Z_AXIS] = ccm_param.grbl_destination[Z_AXIS] - 5;

      if (feature_print_control::idex_sys_is_normal() && 1 == active_extruder && change_axis == X2_AXIS) // idex结构
      {
        sg_grbl::plan_compensation_destination_idex_basic(1, Z_AXIS, 0.0f);
      }

      feedrate = homing_feedrate[Z_AXIS];
      process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
      sg_grbl::st_synchronize();
    }
  }

  bool t_process(uint8_t switch_extruder, bool is_process_t)
  {
    if (switch_extruder == active_extruder) return false;

    active_extruder = switch_extruder;

    if (feature_print_control::idex_sys_is_copy_or_mirror()) // idex复制模式、镜像模式不切换喷嘴id
    {
      return false;
    }

    if (is_process_t)
    {
      if (feature_print_control::idex_sys_is_normal()) // idex结构
      {
        if (0 == switch_extruder)
        {
          idex_x_move_to_home(X2_AXIS, X_AXIS);
        }
        else if (1 == switch_extruder)
        {
          idex_x_move_to_home(X_AXIS, X2_AXIS);
        }

        for (int i = 0; i < XYZ_NUM_AXIS; i++)
        {
          ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
        }
      }
      else
      {
        if (0 == switch_extruder)
        {
          sg_grbl::st_enable_endstops(true);
          home_axis_process(X_AXIS, true);
          sg_grbl::st_enable_endstops(false);
          t0_process();
        }
        else if (1 == switch_extruder)
        {
          ccm_param.grbl_destination[X_AXIS] = 0;
          feedrate = homing_feedrate[X_AXIS];
          process_buffer_line_normal(ccm_param.grbl_destination, feedrate / 60);
          sg_grbl::st_synchronize();
          t1_process();
        }
      }
    }

    return true;
  }


  void t_process(void)
  {
    bool is_process_t = true;
    tmp_extruder = (unsigned char)parseGcodeBufHandle.codeValue();

    // S-1 只变更active_extruder
    if (parseGcodeBufHandle.codeSeen('S') && parseGcodeBufHandle.codeValue() == -1)
    {
      is_process_t = false;
    }

    switch (tmp_extruder)
    {
    case 0:
    case 1:
      if (!t_process(tmp_extruder, is_process_t)) break;

      break;

    default:
      break;
    }
  }

}

