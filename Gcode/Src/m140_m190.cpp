#include "user_common_cpp.h"

#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif


extern unsigned long previous_xTaskGetTickCount_cmd;
extern void manage_synchronize(void);

bool isM190HeatingComplete(void)
{
  return ccm_param.t_sys.is_gcode_m190_done;
}

void resetM190HeatingComplete(void)
{
  ccm_param.t_sys.is_gcode_m190_done = false;
}


#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  static bool bed_is_cool_down_not_wait = true;              /*!< 冷却不等待 */
  static bool bed_target_direction;                          /*!<  */
  static bool bed_cancel_heatup = false;                     /*!< 是否取消加热 */
  static unsigned long bed_respond_temp_status_peroid = 0;
  static unsigned long bed_respond_temp_status_time_count = 0;

  void m140_process(void)
  {
    if (parseGcodeBufHandle.codeSeen('S'))
    {
      sg_grbl::temperature_set_bed_target(parseGcodeBufHandle.codeValue());
    }
  }

  void m190_process(void)
  {
    ccm_param.t_sys.is_gcode_m190_done = false;
    {
      if (parseGcodeBufHandle.codeSeen('S'))
      {
        sg_grbl::temperature_set_bed_target(parseGcodeBufHandle.codeValue());
        bed_is_cool_down_not_wait = true;
      }
      else if (parseGcodeBufHandle.codeSeen('R'))
      {
        sg_grbl::temperature_set_bed_target(parseGcodeBufHandle.codeValue());
        bed_is_cool_down_not_wait = false;
      }

      bed_cancel_heatup = false;
      bed_target_direction = sg_grbl::temperature_is_bed_heating(); // true if heating, false if cooling

      while ((bed_target_direction) && (!bed_cancel_heatup) ? (sg_grbl::temperature_is_bed_heating()) : (sg_grbl::temperature_is_bed_cooling() && (!bed_is_cool_down_not_wait)))
      {
        // 秒数累加
        if (bed_respond_temp_status_peroid < MILLIS())
        {
          bed_respond_temp_status_peroid = MILLIS() + 1000;
          bed_respond_temp_status_time_count++;
        }

        if (2 < bed_respond_temp_status_time_count)
        {
          bed_respond_temp_status_time_count = 0;
        }

        manage_synchronize();
        (void)OS_DELAY(100);
      }

      previous_xTaskGetTickCount_cmd = xTaskGetTickCount();
    }
    ccm_param.t_sys.is_gcode_m190_done = true;
  }


}

