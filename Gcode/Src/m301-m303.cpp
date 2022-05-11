
#include "user_common_cpp.h"
#include "../../Grbl/Inc/temperature_pid_temp.h"
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


  void m303_process(void)
  {
    float temp = 150.0;
    int16_t e = 0;
    int16_t c = 5;

    if (parseGcodeBufHandle.codeSeen('E')) e = (short)parseGcodeBufHandle.codeValue();

    if (e < EXTRUDERS && e >= 0)
    {
      if (parseGcodeBufHandle.codeSeen('S')) temp = parseGcodeBufHandle.codeValue();

      if (parseGcodeBufHandle.codeSeen('C')) c = (short)parseGcodeBufHandle.codeValue();

      sg_grbl::pid_autotune(temp, e, c);
    }
  }

  void m302_process(void)
  {
#ifdef PREVENT_DANGEROUS_EXTRUDE
    float temp = 0.0;

    if (parseGcodeBufHandle.codeSeen('S'))
    {
      temp = parseGcodeBufHandle.codeValue();
      ccm_param.t_sys.extrude_min_temp = temp;
    }

    //  else //输出当前最小温度
#endif
  }


  void m301_process(void)
  {
    bool is_m301_print_info = true;
#ifdef PIDTEMP

    if (parseGcodeBufHandle.codeSeen('P'))
    {
      sg_grbl::temp_pid_extruder_set_kp(parseGcodeBufHandle.codeValue());
      is_m301_print_info = false;
    }

    if (parseGcodeBufHandle.codeSeen('I'))
    {
      sg_grbl::temp_pid_extruder_set_ki(parseGcodeBufHandle.codeValue());
      is_m301_print_info = false;
    }

    if (parseGcodeBufHandle.codeSeen('D'))
    {
      sg_grbl::temp_pid_extruder_set_kd(parseGcodeBufHandle.codeValue());
      is_m301_print_info = false;
    }

#ifdef PID_ADD_EXTRUSION_RATE

    if (parseGcodeBufHandle.codeSeen('C')) sg_grbl::temp_pid_extruder_set_kc(parseGcodeBufHandle.codeValue());

#endif
    sg_grbl::temp_pid_extruder_update();

    if (is_m301_print_info)
    {
      if (gcode_cmd_type == GCODE_TYPE_USBD)
      {
        USER_SEND_USBD_CMD_BUF("echo: p:%.2f i:%.2f d:%.2f", sg_grbl::temp_pid_extruder_get_kp(), sg_grbl::temp_pid_extruder_get_ki(), sg_grbl::temp_pid_extruder_get_kd());
      }
      else
      {
        USER_EchoLogStr("echo: p:%.2f i:%.2f d:%.2f", sg_grbl::temp_pid_extruder_get_kp(), sg_grbl::temp_pid_extruder_get_ki(), sg_grbl::temp_pid_extruder_get_kd());
      }
    }

#endif // #ifdef PIDTEMP
  }

}

