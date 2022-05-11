#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t tmp_extruder;
extern volatile uint8_t gcode_cmd_type;

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  void m200_process(void)
  {
    //float area = 0.0f;
    float radius = 0.0f;

    if (parseGcodeBufHandle.codeSeen('D'))
    {
      radius = (float)parseGcodeBufHandle.codeValue() * 0.5f;

      if (radius == 0)
      {
        //area = 1;
      }
      else
      {
        //area = PI * pow(radius, 2);
      }
    }
    else
    {
      //reserved for setting filament diameter via UFID or filament measuring device
      return;
    }

    tmp_extruder = gcode::active_extruder;

    if (parseGcodeBufHandle.codeSeen('T'))
    {
      tmp_extruder = (unsigned char)parseGcodeBufHandle.codeValue();

      if (tmp_extruder >= EXTRUDERS)
      {
        //            SERIAL_ECHO_START;
        //            SERIAL_ECHO(MSG_M200_INVALID_EXTRUDER);
        return;
      }
    }

    //volumetric_multiplier[tmp_extruder] = 1 / area;
  }

  void m201_process(void)
  {
    for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (parseGcodeBufHandle.codeSeen(axis_codes[i]))
      {
        ccm_param.planner_settings.max_acceleration_mm_per_s2[i] = (unsigned long)parseGcodeBufHandle.codeValue();
      }
    }

    // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
    sg_grbl::reset_acceleration_rates();
  }

  void m203_process(void)
  {
    for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (parseGcodeBufHandle.codeSeen(axis_codes[i])) ccm_param.planner_settings.max_feedrate_mm_s[i] = parseGcodeBufHandle.codeValue();
    }
  }

  void m204_process(void)
  {
    bool is_m204_print_all_info = true;

    if (parseGcodeBufHandle.codeSeen('S'))
    {
      ccm_param.planner_settings.acceleration = parseGcodeBufHandle.codeValue() ;
      is_m204_print_all_info = false;
    }

    if (parseGcodeBufHandle.codeSeen('T'))
    {
      ccm_param.planner_settings.retract_acceleration = parseGcodeBufHandle.codeValue() ;
      is_m204_print_all_info = false;
    }

    if (is_m204_print_all_info)
    {
      if (gcode_cmd_type == GCODE_TYPE_USBD)
      {
        USER_SEND_USBD_CMD_BUF("Acceleration: T%.2f S%.2f\r\n", ccm_param.planner_settings.acceleration, ccm_param.planner_settings.retract_acceleration);
      }
      else
      {
        USER_EchoLogStr("Acceleration: T%.2f S%.2f\r\n", ccm_param.planner_settings.acceleration, ccm_param.planner_settings.retract_acceleration);
      }
    }
  }

  void m205_process(void)
  {
    if (parseGcodeBufHandle.codeSeen('S')) ccm_param.planner_settings.min_feedrate_mm_s = parseGcodeBufHandle.codeValue();

    if (parseGcodeBufHandle.codeSeen('T')) ccm_param.planner_settings.min_travel_feedrate_mm_s = parseGcodeBufHandle.codeValue();

    if (parseGcodeBufHandle.codeSeen('B')) ccm_param.planner_settings.min_segment_time_us = (unsigned long)parseGcodeBufHandle.codeValue() ;

    if (parseGcodeBufHandle.codeSeen('X')) ccm_param.planner_settings.max_xy_jerk = parseGcodeBufHandle.codeValue() ;

    if (parseGcodeBufHandle.codeSeen('Z')) ccm_param.planner_settings.max_z_jerk = parseGcodeBufHandle.codeValue() ;

    if (parseGcodeBufHandle.codeSeen('E'))
    {
      ccm_param.planner_settings.max_e_jerk = parseGcodeBufHandle.codeValue() ;
      ccm_param.planner_settings.max_b_jerk = ccm_param.planner_settings.max_e_jerk;
    }
  }



}

