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

  static void laser_set_xyz_dest(const int axis, const float value)
  {
    bool is_relative_mode = (bool)(ccm_param.planner_settings.axis_relative_modes[axis] || ccm_param.planner_settings.relative_mode);
    ccm_param.grbl_destination[axis] = value + (float)is_relative_mode * sg_grbl::plan_get_current_save_xyz(axis);

    if (axis == Z_AXIS)
    {
      ccm_param.grbl_destination[Z2_AXIS] = ccm_param.grbl_destination[axis];
    }
  }

  void get_coordinates_laser(const int axis, const float value)
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      if (axis == X_AXIS)
      {
        if (1 == gcode::active_extruder)
        {
          laser_set_xyz_dest(X2_AXIS, value);

          if (print_status.is_printing)
          {
            ccm_param.grbl_destination[X2_AXIS] += flash_param_t.laser_extruder_1_bed_offset[0];
          }
        }
        else if (0 == gcode::active_extruder)
        {
          if (!print_status.is_printing)
          {
            laser_set_xyz_dest(X_AXIS, value);
          }
        }
      }
      else if (axis == X2_AXIS || axis == Z2_AXIS || axis == E_AXIS || axis == B_AXIS) // 跳过
      {
      }
      else if (axis == Y_AXIS)
      {
        laser_set_xyz_dest(Y_AXIS, value);

        if (print_status.is_printing)
        {
          ccm_param.grbl_destination[Y_AXIS] += flash_param_t.laser_ext0_home_pos_adding[1];
        }
      }
      else
      {
        if ((parseGcodeBufHandle.codeSeen('D') && parseGcodeBufHandle.codeValueLong() == 1))
          laser_set_xyz_dest(axis, value); // Z
      }
    }
  }

  void clamp_to_software_endstops_laser(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface)
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      if (1 == gcode::active_extruder)
      {
        // 避免撞击
        if (target[X2_AXIS] < flash_param_t.laser_extruder_1_bed_offset[0])
          IsPopWarningInterface = ERR_X_MIN_LIMIT;
      }

      if (IsPopWarningInterface != -1)
      {
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



}

