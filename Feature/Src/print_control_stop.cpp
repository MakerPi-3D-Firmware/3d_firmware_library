#include "user_common_cpp.h"
#ifdef HAS_PRINT_CONTROL

#include "USBFileTransfer.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif

static bool is_process_stop_print = false;       /*!< 是否执行停止打印 */

bool IsProcessStopPrint(void)
{
  return is_process_stop_print;
}

namespace feature_print_control
{



  void stop_start(void)
  {
    print_status.is_stop_printing = true;
    file_close();
    sg_grbl::temperature_disable_heater();
    flash_poweroff_data_reset();

    if (ccm_param.t_sys.is_print_e1_offset_flag)
    {
      ccm_param.t_sys.is_print_e1_offset_flag = false;
    }

    SetIsUSBPrintStop(true);
  }

  void stop_prepare(void)
  {
    if (print_status.is_stop_printing) // 串口停止打印或正常停止打印
    {
      stepper_quick_stop(); // 电机快速停止

      if (0 == sg_grbl::planner_moves_planned())                    // 运动队列为空，执行以下操作
      {
        print_status.is_stop_printing = false;                   // 设置停止打印状态为false
        print_status.is_printing = false;
        is_process_stop_print = true;             // 执行停止打印标志置为true

        if (feature_print_control::idex_sys_is_extruder())
        {
          feature_print_control::idex_sys_set_normal();
        }

        return;
      }
    }
  }

  void stop_process(void)
  {
    stop_prepare();                                                                   // 停止打印准备操作
    osDelay(100); // 延时让其他任务执行

    if (is_process_stop_print && 0 == sg_grbl::planner_moves_planned())
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
      {
        user_send_internal_cmd((char *)"M107");
      }

      if (feature_print_control::idex_sys_is_extruder())
      {
        gcode_cmd_t0(false);
      }

      if (!ccm_param.t_sys.print_is_home)
      {
        gcode_cmd_g28();
        ccm_param.t_sys.print_is_home = false;
      }

      gcode_cmd_z_down_60mm_xy_to_home();

      if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      {
        gcode_cmd_t1(false);
      }

      osDelay(100); // 延时让其他任务执行
      USER_SEND_INTERNAL_CMD_BUF("M84 X Y E B");         // 解锁XYEB
      USER_SEND_INTERNAL_CMD_BUF("M104 T0 S0");
      USER_SEND_INTERNAL_CMD_BUF("M104 T1 S0");
      USER_SEND_INTERNAL_CMD_BUF("M140 S0");
      resetCmdBuf();                                                                 // 重置指令数组
      ccm_param.t_gui.cura_speed = 0;                                                          // 读取gcode文件获取数值，停止打印则清0
      is_process_stop_print = false;                                                    // 执行停止打印标志置为false
      osDelay(100); // 延时让其他任务执行
      gcode_cmd_gui_wait_done();
    }
  }


}
#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_PRINT_CONTROL





