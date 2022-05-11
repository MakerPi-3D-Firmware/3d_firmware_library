#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"

#ifdef __cplusplus
extern "C" {
#endif

bool setTargetedHotend(int16_t code);

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  void m105_process(void)
  {
    if (setTargetedHotend(105))
      return;

    //  #if defined(HEAT_NOZZLE1_PIN) && TEMP_0_PIN_ON > -1
    //  USER_EchoLogStr("ok T:%.1f /%.1f", temperature_get_extruder_current(0), temperature_get_extruder_target(0));
    //  #elif defined(TEMP_1_PIN) && TEMP_1_PIN_ON > -1

    for (int8_t cur_extruder = 0; cur_extruder < EXTRUDERS; ++cur_extruder)
    {
      USER_EchoLogStr("ok T%d:%.1f /%.1f", cur_extruder, sg_grbl::temperature_get_extruder_current((unsigned char)cur_extruder),
                      sg_grbl::temperature_get_extruder_target((unsigned char)cur_extruder));
    }

    //  #else
    //  USER_ErrLog("Error:No thermistors - no temperature\n");
    //  USER_EchoLogStr(" @:%d B@:%d\n", temperature_get_extruder_heater_power(tmp_extruder), temperature_get_bed_heater_power());
    //  //return;
    //  #endif
    //  #if defined(HEAT_BED_PIN) && TEMP_BED_PIN_ON > -1
    USER_EchoLogStr(" B:%.1f /%.1f", sg_grbl::temperature_get_bed_current(), sg_grbl::temperature_get_bed_target());
    //  #endif //TEMP_BED_PIN
    //腔体温度，先用热床温度代替20170906
#if defined(TEMP_BED_PIN) && TEMP_BED_PIN_ON > -1

    if (ccm_param.t_sys.enable_cavity_temp)
    {
      USER_EchoLogStr(" C:%.1f /%.1f", temperature_get_cavity_current(), temperature_get_cavity_target());
    }

#endif //TEMP_BED_PIN
    USER_EchoLogStr("\r\n");
  }
}






