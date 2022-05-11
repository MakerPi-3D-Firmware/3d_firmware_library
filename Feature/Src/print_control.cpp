#include "user_common_cpp.h"
#ifdef HAS_PRINT_CONTROL
#include "USBFileTransfer.h"
#ifdef __cplusplus
extern "C" {
#endif
t_print_status print_status;
namespace feature_print_control
{
  static volatile uint32_t print_start_time = 0;              /*!< 打印时间计数 */
  static volatile bool is_check_idex_normal_mode = false;     /*!< 检测idex正常模式下打印单色、双色 */

  static void set_check_idex_normal_mode(void)
  {
    if (feature_print_control::idex_sys_is_normal())
    {
      is_check_idex_normal_mode = true;
    }
  }

  void check_idex_normal_mode(void)
  {
    if (is_check_idex_normal_mode && print_status.is_printing && feature_print_control::idex_sys_is_normal())
    {
      if (ccm_param.layer_count > 0)
      {
        if ((int)sg_grbl::temperature_get_extruder_target(gcode::active_extruder) == 0 &&
            (int)sg_grbl::temperature_get_extruder_target((gcode::active_extruder + 1) % EXTRUDERS) == 0)
        {
          return;
        }

        if (sg_grbl::temperature_get_extruder_target(gcode::active_extruder) > 0.0f &&
            sg_grbl::temperature_get_extruder_target((gcode::active_extruder + 1) % EXTRUDERS) < 80.0f)
        {
          ccm_param.t_sys.print_type_idex_single_mode = true;
        }
        else
        {
          ccm_param.t_sys.print_type_idex_single_mode = false;
        }

        is_check_idex_normal_mode = false;
      }
      else
      {
        ccm_param.t_sys.print_type_idex_single_mode = true;
      }
    }
  }

  static void print_status_reset(void)
  {
    print_status.is_printing = false;
    print_status.is_pause_printing = false;
    print_status.is_stop_printing = false;
    print_status.is_resume_printing = false;
    print_status.is_finish_print = false;
    print_status.is_mid_chg_filament = false;
    print_status.is_poweroff_recovery_print = false;
    print_status.is_print_medium_user_file = false;
    print_status.is_pause_to_cool_down = false;
  }

  bool print_not_from_medium_user(void)
  {
    return (!print_status.is_print_medium_user_file) && (!ccm_param.t_sys.is_power_off_file_from_user);
  }

  //是否在加热
  int print_is_heating(void)
  {
    if (ccm_param.t_sys.enable_hot_bed)
    {
      if ((int)sg_grbl::temperature_get_bed_target())
      {
        return 1;
      }
    }

    if ((int)sg_grbl::temperature_get_extruder_target(0))
    {
      return 1;
    }

    return 0;
  }

  //开始打印
  static void print_start(void)
  {
    (void)memset(ccm_param.file_read_buf, 0, sizeof(ccm_param.file_read_buf)); //clear buffer
    ccm_param.t_gui.file_size = 0;
    ccm_param.t_gui.printfile_size = 0;
    ccm_param.t_sys.is_filament_print_not_have = false;
    // 重置联机打印状态
    SetIsUSBPrintFinished(false);
    SetIsUSBPrintStop(false);
    SetIsUSBPrintPause(false);
    set_check_idex_normal_mode();

    if (feature_print_control::file_open())
    {
      // 文件打开成功，发送指令M2000设置打印状态
      print_start_time = xTaskGetTickCount();
      feature_print_control::idex_sys_copy_flash_print_type();
      ccm_param.t_sys.print_type_mix = flash_param_t.mix_print_type;
      stepper_quick_stop(); // 电机快速停止

      if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
      {
        ccm_param.layer_count = ccm_param.t_power_off.layer_count;
        ccm_param.current_layer = ccm_param.t_power_off.current_layer;
      }
      else
      {
        ccm_param.layer_count = 0;
        ccm_param.current_layer = 0;
      }

      user_send_file_cmd((char *)"M2000", 0, ccm_param.layer_count, ccm_param.current_layer);
    }

    if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
    {
      print_status.is_poweroff_recovery_print = false; // 完成恢复操作，重置断电恢复操作标志
    }

    ccm_param.t_sys.enable_color_buf = false;
  }

  //开始打印
  void power_rec_print_start(void)
  {
    print_status_reset();
    print_status.is_poweroff_recovery_print = true;
    flash_poweroff_data_reset();
    print_start();
  }


  void file_print_start(void)
  {
    print_status_reset();         // 重置打印变量
    flash_config_set_flag_save(); // 保存文件名
    flash_poweroff_data_reset();  // 重置断电flash

    if (!ccm_param.t_sys_data_current.enable_powerOff_recovery)
    {
      sg_grbl::temperature_set_extruder_target(0, 0);
      sg_grbl::temperature_set_extruder_target(0, 1);
      sg_grbl::temperature_set_bed_target(0);
    }

    print_start();
    ccm_param.t_sys.is_gcode_m109_done = false;  //打印时重置为还没有加热完成
  }

  void set_file_print_finish(void)
  {
    print_status_reset();
    print_status.is_finish_print = true;   // 设置打印完成标志为true，用于gui页面跳转
  }

  void set_file_print_start(void)
  {
    print_status_reset();
    print_status.is_printing = true;   // 设置打印完成标志为true，用于gui页面跳转
  }

  unsigned int print_time(void)
  {
    return (unsigned int)((xTaskGetTickCount() / 1000 - print_start_time / 1000) + ccm_param.t_sys.print_time_save);
  }

  void pc_init(void)
  {
    print_status_reset();
  }

  void pc_process(void)
  {
    if (!print_status.is_printing)
    {
      osDelay(50);
      resume_process();
    }

    osDelay(50);
    // 停止打印乱跑，过滤乱跑指令
    stop_process();
    osDelay(50);
    pause_process();
    osDelay(50);                        // 延时以让低优先级的任务执行
    check_idex_normal_mode();
  }
}


#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_PRINT_CONTROL





