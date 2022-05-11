#include "../Inc/power_cal_z_height.h"
#include "user_common_cpp.h"
#ifdef __cplusplus
extern "C" {
#endif
static bool IsStartCalculateZMaxPos = false;                      /*!< 是否开启校正Z高度 */
static bool isStopGetZMax = false;                                /*!< 是否停止校准z高度标志位 */

void feature_pow_cal_z_start(void)
{
  isStopGetZMax = false;
  ccm_param.t_sys.is_gcode_g28_done = false; // 设置为未归零
  user_send_internal_cmd((char *)"G28 O1"); // XYZ归零
  gcode_cmd_z_down_to_bottom(); // Z下降到底部
  IsStartCalculateZMaxPos = true; // 设置开始校准Z高度
}

void feature_pow_cal_z_stop(void)
{
  isStopGetZMax = true;
}

void feature_pow_cal_z_process(void)
{
  if (!ccm_param.t_sys_data_current.enable_powerOff_recovery || !ccm_param.t_sys.enable_z_max_limit)
    return;

  if (isStopGetZMax)
  {
    stepper_quick_stop(); // 电机快速停止
    IsStartCalculateZMaxPos = false;
    isStopGetZMax = false;
    osDelay(50);
  }

  if (IsStartCalculateZMaxPos) // 开始校准Z
  {
    if (0 == sg_grbl::planner_moves_planned() && sg_grbl::st_check_endstop_z_hit_max() && ccm_param.t_sys.is_gcode_g28_done) //已碰到限位开关  //先归零，然后向下碰限位开关
    {
      const float ZMaxPosValue = sg_grbl::st_get_endstops_len(static_cast<int>(Z_AXIS));
      flash_interface_save_cal_zmax_pos(ZMaxPosValue);
#ifdef ENABLE_GUI_LVGL
      ccm_param.t_sys.is_z_max_pos_cal_home = true;
#else
      ccm_param.t_sys.is_z_max_pos_cal_finish = true;
#endif
      IsStartCalculateZMaxPos = false;
      gcode_cmd_g28();
      //串口上传信息到上位机2017.7.6
      USER_EchoLogStr("z_max_pos:%.2f\r\n", ZMaxPosValue);
      osDelay(50);
    }
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif



















