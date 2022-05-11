#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long previous_xTaskGetTickCount_cmd;
extern float feedrate;
extern int feedmultiply;
extern int extrudemultiply;
extern uint32_t SDPos;
extern float add_homeing[XYZ_NUM_AXIS];
#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  static void dual_idex_set_xyz_dest(const int axis, const float value)
  {
    bool is_relative_mode = (bool)(ccm_param.planner_settings.axis_relative_modes[axis] || ccm_param.planner_settings.relative_mode);
    ccm_param.grbl_destination[axis] = value + (float)is_relative_mode * sg_grbl::plan_get_current_save_xyz(axis);

    if (axis == Z_AXIS)
    {
      //      if (feature_print_control::idex_sys_is_copy_or_mirror())
      //      {
      //        ccm_param.grbl_destination[Z2_AXIS] = ccm_param.grbl_destination[axis] + (flash_param_t.idex_ext1_ext0_offset[2] - flash_param_t.idex_ext0_home_pos_adding[2]);
      //      }
      //      else
      {
        ccm_param.grbl_destination[Z2_AXIS] = ccm_param.grbl_destination[axis];
      }
    }
  }

  static void dual_idex_set_eb_dest(const int axis, const float value)
  {
    bool is_relative_mode = (bool)(ccm_param.planner_settings.axis_relative_modes[axis] || ccm_param.planner_settings.relative_mode);
    ccm_param.grbl_destination[axis] = value + (float)is_relative_mode * ccm_param.grbl_current_position[axis];
  }

  static void dual_idex_axis_normal(const int axis, const float value)
  {
    if (axis == E_AXIS)
    {
      if (1 == gcode::active_extruder)
      {
        dual_idex_set_eb_dest(B_AXIS, value);
      }
      else if (0 == gcode::active_extruder)
      {
        dual_idex_set_eb_dest(E_AXIS, value);
      }
    }
    else if (axis == X_AXIS)
    {
      if (1 == gcode::active_extruder)
      {
        dual_idex_set_xyz_dest(X2_AXIS, value);

        if (print_status.is_printing)
        {
          ccm_param.grbl_destination[X2_AXIS] += flash_param_t.idex_extruder_0_bed_offset[0];
        }
      }
      else if (0 == gcode::active_extruder)
      {
        dual_idex_set_xyz_dest(X_AXIS, value);

        if (print_status.is_printing)
        {
          ccm_param.grbl_destination[X_AXIS] += flash_param_t.idex_extruder_0_bed_offset[0];
        }
      }
    }
  }

  static void dual_idex_axis_copy(const int axis, const float value)
  {
    if (axis == E_AXIS)
    {
      dual_idex_set_eb_dest(E_AXIS, value);
      dual_idex_set_eb_dest(B_AXIS, value);
    }
    else if (axis == X_AXIS)
    {
      if (print_status.is_printing)
      {
        dual_idex_set_xyz_dest(X_AXIS, value);
        dual_idex_set_xyz_dest(X2_AXIS, value);
        ccm_param.grbl_destination[X_AXIS] += flash_param_t.idex_extruder_0_bed_offset[0];
        ccm_param.grbl_destination[X2_AXIS] += flash_param_t.idex_extruder_0_bed_offset[0] + fabs(flash_param_t.idex_extruder_1_bed_offset[1] - flash_param_t.idex_extruder_1_bed_offset[0]) / 2;
      }
      else
      {
        if (0 == gcode::active_extruder)
        {
          dual_idex_set_xyz_dest(X_AXIS, value);
        }
        else if (1 == gcode::active_extruder)
        {
          dual_idex_set_xyz_dest(X2_AXIS, value);
        }
      }
    }
  }

  static void dual_idex_axis_mirror(const int axis, const float value)
  {
    if (axis == E_AXIS)
    {
      dual_idex_set_eb_dest(E_AXIS, value);
      dual_idex_set_eb_dest(B_AXIS, value);
    }
    else if (axis == X_AXIS)
    {
      if (print_status.is_printing)
      {
        dual_idex_set_xyz_dest(X_AXIS, value);
        dual_idex_set_xyz_dest(X2_AXIS, value);
        ccm_param.grbl_destination[X_AXIS] += flash_param_t.idex_extruder_0_bed_offset[0];
        ccm_param.grbl_destination[X2_AXIS] = flash_param_t.idex_extruder_0_bed_offset[1] - ccm_param.grbl_destination[X2_AXIS];
      }
      else
      {
        if (0 == gcode::active_extruder)
        {
          dual_idex_set_xyz_dest(X_AXIS, value);
        }
        else if (1 == gcode::active_extruder)
        {
          dual_idex_set_xyz_dest(X2_AXIS, value);
        }
      }
    }
  }


  void get_coordinates_dual_idex(const int axis, const float value)
  {
    if (feature_print_control::idex_sys_is_extruder())
    {
      if (axis == E_AXIS || axis == X_AXIS)
      {
        if (feature_print_control::idex_sys_is_normal())
        {
          dual_idex_axis_normal(axis, value);
        }
        else if (feature_print_control::idex_sys_is_copy())
        {
          dual_idex_axis_copy(axis, value);
        }
        else if (feature_print_control::idex_sys_is_mirror())
        {
          dual_idex_axis_mirror(axis, value);
        }
      }
      else if (axis == B_AXIS)
      {
        if (feature_print_control::idex_sys_is_normal())
        {
          dual_idex_set_eb_dest(axis, value); // B
        }
      }
      else if (axis == X2_AXIS)
      {
        if (!print_status.is_printing)
        {
          if (feature_print_control::idex_sys_is_extruder())
          {
            dual_idex_set_xyz_dest(axis, value);
          }
        }
      }
      else if (axis == Z2_AXIS)  // 跳过
      {
      }
      else
      {
        dual_idex_set_xyz_dest(axis, value); // YZ
      }
    }
  }

  void clamp_to_software_endstops_dual_idex(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface)
  {
    if (feature_print_control::idex_sys_is_extruder())
    {
      if (feature_print_control::idex_sys_is_normal())
      {
        if (0 == gcode::active_extruder)
        {
          // 避免撞击喷头2
          if (target[X_AXIS] > flash_param_t.idex_extruder_0_bed_offset[1])
            IsPopWarningInterface = ERR_X_MAX_LIMIT;
        }
        else if (1 == gcode::active_extruder)
        {
          if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
          {
            // 避免撞击喷头1
            if (target[X2_AXIS] < flash_param_t.idex_extruder_1_bed_offset[0])
              IsPopWarningInterface = ERR_X_MIN_LIMIT;
          }
        }
      }
      else if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        if (print_status.is_printing)
        {
          // 避免撞击喷头
          if (target[X_AXIS] > flash_param_t.idex_extruder_0_bed_offset[0] + fabs(flash_param_t.idex_extruder_0_bed_offset[1] - flash_param_t.idex_extruder_0_bed_offset[0]) / 2)
            IsPopWarningInterface = ERR_X_MAX_LIMIT;

          if (target[X2_AXIS] < flash_param_t.idex_extruder_1_bed_offset[0] + fabs(flash_param_t.idex_extruder_1_bed_offset[1] - flash_param_t.idex_extruder_1_bed_offset[0]) / 2)
            IsPopWarningInterface = ERR_X_MIN_LIMIT;
        }
      }

      if (IsPopWarningInterface != -1)
      {
        target[X_AXIS] = ccm_param.grbl_current_position[X_AXIS];
        target[X2_AXIS] = ccm_param.grbl_current_position[X2_AXIS];
        target[Y_AXIS] = ccm_param.grbl_current_position[Y_AXIS];
        target[Z_AXIS] = ccm_param.grbl_current_position[Z_AXIS];
      }

      if (print_status.is_printing)
      {
#ifndef ENABLE_GUI_LVGL
        gui_disp_error_info_show(IsPopWarningInterface);
#else
        custom_disp_error_info_show(IsPopWarningInterface);
#endif
      }
    }
  }

  // IDEX double x structure, compensation XYZ value
  void get_coordinates_idex_comp_ext1_dest(void)
  {
    if (feature_print_control::idex_sys_is_normal() &&
        gcode::active_extruder == 1)
    {
      if (parseGcodeBufHandle.codeSeen('X'))
      {
        sg_grbl::plan_compensation_destination_idex_basic(1, X2_AXIS, 0.0f);
      }

      if (parseGcodeBufHandle.codeSeen('Y'))
      {
        sg_grbl::plan_compensation_destination_idex_basic(1, Y_AXIS, 0.0f);
      }

      if (parseGcodeBufHandle.codeSeen('Z'))
      {
        sg_grbl::plan_compensation_destination_idex_basic(1, Z_AXIS, 0.0f);
      }
    }
  }

}

