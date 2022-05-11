#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif



extern uint8_t tmp_extruder;
extern unsigned long previous_xTaskGetTickCount_cmd;
extern void manage_synchronize(void);

bool setTargetedHotend(int16_t code);

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  static bool extruder_is_cool_down_not_wait = true;              /*!< 冷却不等待 */
  static bool extruder_target_direction;                          /*!<  */
  //  static bool extruder_cancel_heatup = false;                     /*!< 是否取消加热 */
  static unsigned long extruder_respond_temp_status_peroid = 0;
  static unsigned long extruder_respond_temp_status_time_count = 0;


  void m104_process(void)
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
    {
      sg_grbl::st_synchronize();//等待上一条消息执行完
    }

    if (setTargetedHotend(104))
      return;

    if (parseGcodeBufHandle.codeSeen('S'))
    {
      if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        if (tmp_extruder == 1) return;

        sg_grbl::temperature_set_extruder_target(parseGcodeBufHandle.codeValue(), 1);
      }

      sg_grbl::temperature_set_extruder_target(parseGcodeBufHandle.codeValue(), tmp_extruder);
    }
  }

  void m109_process(void)
  {
    if (setTargetedHotend(109))
      return;

    ccm_param.t_sys.is_gcode_m109_done = false;

    if (parseGcodeBufHandle.codeSeen('S'))
    {
      if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        if (tmp_extruder == 1) return;

        sg_grbl::temperature_set_extruder_target(parseGcodeBufHandle.codeValue(), 1);
      }

      sg_grbl::temperature_set_extruder_target(parseGcodeBufHandle.codeValue(), tmp_extruder);
      extruder_is_cool_down_not_wait = true;
    }
    else if (parseGcodeBufHandle.codeSeen('R'))
    {
      sg_grbl::temperature_set_extruder_target(parseGcodeBufHandle.codeValue(), tmp_extruder);
      extruder_is_cool_down_not_wait = false;
    }

    //  unsigned long codenum = xTaskGetTickCount();
    /* See if we are heating up or cooling down */
    extruder_target_direction = sg_grbl::temperature_is_extruder_heating(tmp_extruder); // true if heating, false if cooling
    //    extruder_cancel_heatup = false;

    while (extruder_target_direction ? (sg_grbl::temperature_is_extruder_heating(tmp_extruder)) :
           (sg_grbl::temperature_is_extruder_cooling(tmp_extruder) && (!extruder_is_cool_down_not_wait)))
    {
      // 秒数累加
      if (extruder_respond_temp_status_peroid < MILLIS())
      {
        extruder_respond_temp_status_peroid = MILLIS() + 1000;
        extruder_respond_temp_status_time_count++;
      }

      if (2 < extruder_respond_temp_status_time_count)
      {
        extruder_respond_temp_status_time_count = 0;
      }

      manage_synchronize();
      (void)OS_DELAY(100);
    }

    ccm_param.t_sys.is_gcode_m109_done = true;
    previous_xTaskGetTickCount_cmd = xTaskGetTickCount();
  }


}

