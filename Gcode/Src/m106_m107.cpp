#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  void m106_process(void)
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
    {
      sg_grbl::st_synchronize();//等待上一条消息执行完
    }

    int fanSpeed = 0;

    if (parseGcodeBufHandle.codeSeen('S'))
    {
      int fanspeed_codevalue = (int)parseGcodeBufHandle.codeValue();
      fanSpeed = constrain(fanspeed_codevalue, 0, 255);
    }
    else
    {
      fanSpeed = 255;
    }

    feature_control_set_fan_speed(fanSpeed);
  }

  void m107_process(void)
  {
    feature_control_set_fan_speed(0);
  }
}










