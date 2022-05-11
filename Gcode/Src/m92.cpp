#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"

#ifdef __cplusplus
extern "C" {
#endif
extern volatile uint8_t gcode_cmd_type;
#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  void m92_process(void)
  {
    bool is_m92_print_all_info = true;

    for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (parseGcodeBufHandle.codeSeen(axis_codes[i]))
      {
        is_m92_print_all_info = false;

        if (i == E_AXIS || i == B_AXIS)
        {
          // E
          float value = parseGcodeBufHandle.codeValue();

          if (value < 20.0f)
          {
            float factor = ccm_param.planner_settings.axis_steps_per_mm[i] / value; // increase e constants if M92 E14 is given for netfab.
            ccm_param.planner_settings.max_e_jerk *= factor;
            ccm_param.planner_settings.max_feedrate_mm_s[i] *= factor;
            ccm_param.planner_settings.axis_steps_per_sqr_second[i] *= (unsigned long)factor;
          }

          ccm_param.planner_settings.axis_steps_per_mm[i] = value;
        }
        else
        {
          ccm_param.planner_settings.axis_steps_per_mm[i] = parseGcodeBufHandle.codeValue();
        }
      }
    }

    if (is_m92_print_all_info)
    {
      char m92_print_info_msg[96];
      strcpy(m92_print_info_msg, "echo: M92");

      for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
      {
        sprintf(m92_print_info_msg, "%s %c%.3f", m92_print_info_msg, axis_codes[i], ccm_param.planner_settings.axis_steps_per_mm[i]);
      }

      strcat(m92_print_info_msg, "\r\n");

      if (gcode_cmd_type == GCODE_TYPE_USBD)
      {
        user_usb_device_transmit(m92_print_info_msg);
      }
      else
      {
        USER_EchoLogStr("%s", m92_print_info_msg);
      }
    }
  }
}






