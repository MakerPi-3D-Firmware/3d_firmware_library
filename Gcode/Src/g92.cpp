#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif

extern float add_homeing[XYZ_NUM_AXIS];

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  static void g92_set_axis_value(const int axis)
  {
    ccm_param.grbl_current_position[axis] = parseGcodeBufHandle.codeValue();
    ccm_param.grbl_destination[axis] = parseGcodeBufHandle.codeValue();

    if (axis == Z_AXIS)
    {
      ccm_param.grbl_current_position[Z2_AXIS] = parseGcodeBufHandle.codeValue();
      ccm_param.grbl_destination[Z2_AXIS] = parseGcodeBufHandle.codeValue();
    }
    else if (axis == E_AXIS || axis == B_AXIS)
    {
      ccm_param.t_gui.used_total_material += ccm_param.grbl_destination[B_AXIS] - parseGcodeBufHandle.codeValue();

      if (ccm_param.t_gui.used_total_material < 0.0f)
      {
        ccm_param.t_gui.used_total_material = 0.0f;
      }
    }
  }

  static void g92_set_axis_value_idex_normal(const int axis)
  {
    if (axis < XYZ_NUM_AXIS)
    {
      ccm_param.grbl_current_position[axis] = parseGcodeBufHandle.codeValue() + add_homeing[axis];
      ccm_param.grbl_destination[axis] = parseGcodeBufHandle.codeValue() + add_homeing[axis];
    }
    else
    {
      ccm_param.grbl_current_position[axis] = parseGcodeBufHandle.codeValue();
      ccm_param.grbl_destination[axis] = parseGcodeBufHandle.codeValue();
    }
  }

  static void g92_process_idex_normal(const int axis)
  {
    if (axis == (int)E_AXIS)
    {
      if (1 == gcode::active_extruder)
      {
        g92_set_axis_value_idex_normal(B_AXIS);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[B_AXIS], B_AXIS);
      }
      else if (0 == gcode::active_extruder)
      {
        g92_set_axis_value_idex_normal(axis);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
      }
    }
    else  if (axis == (int)B_AXIS)
    {
      if (!print_status.is_printing)//
      {
        g92_set_axis_value_idex_normal(axis);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
      }
    }
    else if (axis == (int)X_AXIS)
    {
      if (1 == gcode::active_extruder)
      {
        g92_set_axis_value_idex_normal(X2_AXIS);
      }
      else if (0 == gcode::active_extruder)
      {
        g92_set_axis_value_idex_normal(axis);
      }
    }
    else if (axis == (int)Z_AXIS)
    {
      g92_set_axis_value_idex_normal(Z_AXIS);
      g92_set_axis_value_idex_normal(Z2_AXIS);
    }
    else if (axis == (int)Y_AXIS)
    {
      g92_set_axis_value_idex_normal(Y_AXIS);
    }
  }

  static void g92_process_idex_copy_mirror(const int axis)
  {
    if (axis == (int)E_AXIS)
    {
      if (0 == gcode::active_extruder)
      {
        g92_set_axis_value(axis);
        g92_set_axis_value(B_AXIS);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[B_AXIS], B_AXIS);
      }
    }
    else if (axis == (int)X_AXIS)
    {
      if (0 == gcode::active_extruder)
      {
        g92_set_axis_value(X_AXIS);
      }
      else if (1 == gcode::active_extruder)
      {
        g92_set_axis_value(X2_AXIS);
      }
    }
    else if (axis == (int)X2_AXIS)
    {
      if (0 == gcode::active_extruder)
      {
        g92_set_axis_value(axis);
      }
    }
    else if (axis == (int)Z_AXIS)
    {
      g92_set_axis_value(Z_AXIS);
    }
    else
    {
      ccm_param.grbl_current_position[axis] = parseGcodeBufHandle.codeValue() + add_homeing[axis];
    }
  }


  static void g92_process_idex(const int axis)
  {
    if (feature_print_control::idex_sys_is_normal())
    {
      g92_process_idex_normal(axis);
    }
    else if (feature_print_control::idex_sys_is_copy_or_mirror())
    {
      g92_process_idex_copy_mirror(axis);
    }
  }


  static void g92_process_mix(const int axis)
  {
    if (axis == (int)E_AXIS)
    {
      if (print_status.is_printing)
      {
        if (!print_status.is_mid_chg_filament)
        {
          g92_set_axis_value(axis);
          g92_set_axis_value(B_AXIS);
          sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
          sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[B_AXIS], B_AXIS);
        }
        else
        {
          g92_set_axis_value(axis); //load and unload filament
          sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
        }
      }
      else
      {
        g92_set_axis_value(axis); //load and unload filament
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
      }
    }
    else if (axis == (int)B_AXIS)
    {
      if (!print_status.is_printing)
      {
        g92_set_axis_value(axis); //load and unload filament
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
      }
      else
      {
        if (print_status.is_mid_chg_filament)
        {
          g92_set_axis_value(axis); //load and unload filament
          sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
        }
      }
    }
    else if (axis == (int)Z_AXIS)
    {
      g92_set_axis_value(Z_AXIS);
    }
    else if (axis == (int)X_AXIS)
    {
      if (0 == gcode::active_extruder)
      {
        g92_set_axis_value(X_AXIS);
      }
      else if (1 == gcode::active_extruder)
      {
        g92_set_axis_value(X2_AXIS);
      }
    }
    else
    {
      ccm_param.grbl_current_position[axis] = parseGcodeBufHandle.codeValue() + add_homeing[axis];
      ccm_param.grbl_destination[axis] = ccm_param.grbl_current_position[axis];
    }
  }

  static void g92_process_laser(const int axis)
  {
    if (axis == (int)E_AXIS)
    {
      if (print_status.is_printing)
      {
        g92_set_axis_value(axis);
        g92_set_axis_value(B_AXIS);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[B_AXIS], B_AXIS);
      }
      else
      {
        g92_set_axis_value(axis); //load and unload filament
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
      }
    }
    else if (axis == (int)B_AXIS)
    {
      if (!print_status.is_printing)
      {
        g92_set_axis_value(axis); //load and unload filament
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
      }
    }
    else if (axis == (int)Z_AXIS)
    {
      g92_set_axis_value(Z_AXIS);
    }
    else if (axis == (int)X_AXIS)
    {
      if (0 == gcode::active_extruder)
      {
        g92_set_axis_value(X_AXIS);
      }
      else if (1 == gcode::active_extruder)
      {
        g92_set_axis_value(X2_AXIS);
      }
    }
    else
    {
      ccm_param.grbl_current_position[axis] = parseGcodeBufHandle.codeValue() + add_homeing[axis];
      ccm_param.grbl_destination[axis] = ccm_param.grbl_current_position[axis];
    }
  }

  static void g92_process_other(const int axis)
  {
    if (axis == (int)E_AXIS)
    {
      if (1 == gcode::active_extruder)
      {
        g92_set_axis_value(B_AXIS);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[B_AXIS], B_AXIS);
      }
      else if (0 == gcode::active_extruder)
      {
        g92_set_axis_value(axis);
        sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
      }
    }
    else if (axis == (int)B_AXIS)
    {
      g92_set_axis_value(axis);
      sg_grbl::planner_set_axis_position(ccm_param.grbl_destination[axis], axis);
    }
    else if (axis == (int)Z_AXIS)
    {
      g92_set_axis_value(Z_AXIS);
    }
    else if (axis == (int)Z2_AXIS || axis == (int)X2_AXIS)
    {
    }
    else
    {
      ccm_param.grbl_current_position[axis] = parseGcodeBufHandle.codeValue() + add_homeing[axis];
      ccm_param.grbl_destination[axis] = ccm_param.grbl_current_position[axis];
    }
  }

  void g92_process(void)
  {
    if (!parseGcodeBufHandle.codeSeen(axis_codes[E_AXIS]) && !parseGcodeBufHandle.codeSeen(axis_codes[B_AXIS]))
    {
      while (sg_grbl::planner_moves_planned() > 0)
        OS_DELAY(50);
    }

    bool is_seen_xyz = false;

    for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (parseGcodeBufHandle.codeSeen(axis_codes[i]))
      {
        if (feature_print_control::idex_sys_is_extruder())
        {
          g92_process_idex(i);
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
        {
          g92_process_mix(i);
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
        {
          g92_process_laser(i);
        }
        else
        {
          g92_process_other(i);
        }

        if (i < XYZ_NUM_AXIS)
        {
          is_seen_xyz = true;
        }
      }
    }

    if (is_seen_xyz)
    {
      sg_grbl::planner_set_position(ccm_param.grbl_current_position);
    }
  }


}

