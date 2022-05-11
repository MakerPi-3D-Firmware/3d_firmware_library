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

  bool is_code_seen[MAX_NUM_AXIS] = {false};
  float drug_e_value_pre = 0.0f;

  static void drug_set_xyz_dest(const int axis, const float value)
  {
    bool is_relative_mode = (bool)(ccm_param.planner_settings.axis_relative_modes[axis] || ccm_param.planner_settings.relative_mode);
    ccm_param.grbl_destination[axis] = value + (float)is_relative_mode * sg_grbl::plan_get_current_save_xyz(axis);

    if (axis == Z_AXIS)
    {
      ccm_param.grbl_destination[Z2_AXIS] = ccm_param.grbl_destination[axis];
    }
  }

  void get_coordinates_drug(const int axis, const float value)
  {
    is_code_seen[axis] = true;

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
    {
      if (axis == X_AXIS)
      {
        drug_set_xyz_dest(X_AXIS, value);

        if (print_status.is_printing)
        {
          ccm_param.grbl_destination[X_AXIS] += flash_param_t.laser_extruder_1_bed_offset[0];
        }
      }
      else if (axis == X2_AXIS || axis == Z2_AXIS || axis == B_AXIS) // 跳过
      {
      }
      else if (axis == E_AXIS) // 跳过
      {
        sg_grbl::st_synchronize();

        if (is_code_seen[X_AXIS] || is_code_seen[Y_AXIS] || is_code_seen[Z_AXIS])
        {
          HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
        }
        else
        {
          if (drug_e_value_pre > value)
          {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
          }
          else
          {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
          }
        }

        drug_e_value_pre = value;
      }
      else if (axis == Y_AXIS)
      {
        drug_set_xyz_dest(Y_AXIS, value);

        if (print_status.is_printing)
        {
          ccm_param.grbl_destination[Y_AXIS] += flash_param_t.laser_ext0_home_pos_adding[1];
        }
      }
      else
      {
        drug_set_xyz_dest(axis, value); // Z
      }
    }
  }

  void clamp_to_software_endstops_drug(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface)
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
    {
      if (IsPopWarningInterface != -1)
      {
        target[X_AXIS] = ccm_param.grbl_current_position[X_AXIS];
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

