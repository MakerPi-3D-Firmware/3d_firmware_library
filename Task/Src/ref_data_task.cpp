#include "user_common_cpp.h"
#include "USBFileTransfer.h"
#ifdef __cplusplus
extern "C" {
#endif

void gui_refresh_param_info(void)
{
  static unsigned long NextRefresh = 0;
  static unsigned long machine_timeout = 0;

  if (NextRefresh < xTaskGetTickCount())
  {
    ccm_param.t_gui.is_refresh_rtc = 1;
    NextRefresh = xTaskGetTickCount() + 500;
  }

  if (machine_timeout < xTaskGetTickCount())
  {
    machine_timeout = xTaskGetTickCount() + 500;
    // RefMachRunTime
    ccm_param.t_gui.machine_run_time = xTaskGetTickCount() / 1000;
    ccm_param.t_gui.printed_time_sec = feature_print_control::print_time();
    ccm_param.t_gui.print_percent = feature_print_control::file_get_percent();
  }

  // UpdateGUITemp
  ccm_param.t_gui.nozzle_temp[0] = (int)sg_grbl::temperature_get_extruder_current(0);
  ccm_param.t_gui.nozzle_temp[1] = (int)sg_grbl::temperature_get_extruder_current(1);
  ccm_param.t_gui.target_nozzle_temp[0] = (int)sg_grbl::temperature_get_extruder_target(0);
  ccm_param.t_gui.target_nozzle_temp[1] = (int)sg_grbl::temperature_get_extruder_target(1);

  if (ccm_param.t_sys.enable_hot_bed)
  {
    ccm_param.t_gui.hot_bed_temp = (int)sg_grbl::temperature_get_bed_current();
    ccm_param.t_gui.target_hot_bed_temp = (int)sg_grbl::temperature_get_bed_target();
  }

  ccm_param.t_gui.target_cavity_temp = (int) sg_grbl::temperature_get_cavity_target();
  ccm_param.t_gui.cavity_temp = (int)sg_grbl::temperature_get_cavity_current();
  // UpdateGUIPrintSpeed
  ccm_param.t_gui.print_speed_value = GetFeedMultiply();
  // UpdateGUIFanSpeed
  ccm_param.t_gui.fan_speed_value = feature_control_get_fan_speed();
}

/*
 * 当温度超过最大温度时就发生警报，因温度最大只能是HEATER_0_MAXTEMP-25
 * 已经注释的代码不稳定，经常乱报警，所以方案不可行
 * 2017/8/22
*/
void temperature_error_pop(void)
{
  if (DETECT_PCB_FAULSE == sg_grbl::temperature_get_error_status())
  {
#ifndef ENABLE_GUI_LVGL
    display_picture(95);//警告界面
#endif
    //      gui_send_sem_open_beep();
    user_os_respond_gui_send_sem(OpenBeep);
  }
  else if (MaxTempBedError == sg_grbl::temperature_get_error_status())
  {
#ifndef ENABLE_GUI_LVGL
    gui_disp_error_info_show(ERR_MAX_TEMP_BED);
#else
    custom_disp_error_info_show(ERR_MAX_TEMP_BED);
#endif
  }
  else if (MinTempError == sg_grbl::temperature_get_error_status())
  {
    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      {
#ifndef ENABLE_GUI_LVGL
        err_info_is_show = ERR_INFO_NOT_SHOW;
#else
        custom_err_info_is_show = ERR_INFO_NOT_SHOW;
#endif
        return;
      }
    }
    else if (Drug == ccm_param.t_sys_data_current.model_id)
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
      {
#ifndef ENABLE_GUI_LVGL
        err_info_is_show = ERR_INFO_NOT_SHOW;
#else
        custom_err_info_is_show = ERR_INFO_NOT_SHOW;
#endif
        return;
      }
    }

#ifndef ENABLE_GUI_LVGL
    gui_disp_error_info_show(ERR_MIN_TEMP);
#else
    custom_disp_error_info_show(ERR_MIN_TEMP);
#endif
  }
  else if (MaxTempError == sg_grbl::temperature_get_error_status())
  {
#ifndef ENABLE_GUI_LVGL
    gui_disp_error_info_show(ERR_MAX_TEMP);
#else
    custom_disp_error_info_show(ERR_MAX_TEMP);
#endif
  }
  else if (HeatFailError == sg_grbl::temperature_get_error_status())
  {
#ifndef ENABLE_GUI_LVGL
    gui_disp_error_info_show(ERR_HEAT_FAIL);
#else
    custom_disp_error_info_show(ERR_HEAT_FAIL);
#endif
  }
  else if (ThermistorFallsOffError == sg_grbl::temperature_get_error_status())
  {
#ifndef ENABLE_GUI_LVGL
    gui_disp_error_info_show(ERR_THERMISTOR_FALLS);
#else
    custom_disp_error_info_show(ERR_THERMISTOR_FALLS);
#endif
  }
}

void manage_synchronize()
{
  (void)OS_DELAY(50);  //延时以让低优先级的任务执行
  user_iwdg_refresh();
  flash_interface_process();
  (void)OS_DELAY(50);  //延时以让低优先级的任务执行
  sg_grbl::temperature_manage_heater(feature_control_get_fan_speed());   // 加热控制
  manage_inactivity();           // 打印相关控制
  temperature_error_pop();       // 检测是否电路板有问题，导致喷嘴一直加热
}

void UpdateNoPrintStatus(void)
{
  (void)OS_DELAY(50);  //延时以让低优先级的任务执行
  user_usb_host_update_status();
  (void)OS_DELAY(50);  //延时以让低优先级的任务执行
  user_usb_device_trans_file_err(); //传输文件是否错误，比如传输中突然拔线，采用的是超时检测

#ifndef ENABLE_GUI_LVGL
  if (1 == ccm_param.t_sys_data_current.IsMechanismLevel)
    bed_level_mechanical_adjust_process();
#endif
}

void my_printf_test(void)
{
  static unsigned long RefreshGuiTimeOut = 0;
  //  static unsigned count = 0;

  if (mcu_id == MCU_GD32F450IIH6)
  {
    if (RefreshGuiTimeOut < xTaskGetTickCount())
    {
      //      USER_DbgLog("count=%d\n", count++);
      RefreshGuiTimeOut = xTaskGetTickCount() + 1000;
    }
  }
}

void control_function_process(void)
{
  if (!print_status.is_printing)    // 串口打印和U盘打印情况下不执行下面函数
  {
#ifdef HAS_BOARD_TEST

    if (ccm_param.t_sys.enable_board_test)   // 测试固件入口
    {
      boardTest.process();
    }

#endif
  }

  (void)OS_DELAY(50);                      // 延时以让低优先级的任务执行
  user_iwdg_refresh();
  feature_pow_rec_process();           // 断电恢复入口
  (void)OS_DELAY(50);                        // 延时以让低优先级的任务执行
  feature_control(); // 电机风扇控制
  (void)OS_DELAY(50);                        // 延时以让低优先级的任务执行
  user_buzzer_beep_alarm();
}

void ref_data_task_loop(void)
{
  user_iwdg_refresh();
  (void)OS_DELAY(50);                        // 延时以让低优先级的任务执行
  manage_synchronize();
  (void)OS_DELAY(50);                        // 延时以让低优先级的任务执行

  if (!print_status.is_printing) // 串口打印和U盘打印情况下不执行下面函数
    UpdateNoPrintStatus();                  // 更新GUI相关数据

  feature_print_control::pc_process();
  feature_filament::f_process();
  //    if(!serialPrintStatus())                  // 串口打印没打开时，更新界面gui数据
  gui_refresh_param_info();
  (void)OS_DELAY(50);                        // 延时以让低优先级的任务执行
  control_function_process();
  (void)OS_DELAY(50);                        // 延时以让低优先级的任务执行
  my_printf_test();
  user_os_print_task_info();
#ifdef ENABLE_GUI_LVGL
  lvgl_ref_data();
#endif
}

#ifdef __cplusplus
} // extern "C" {
#endif







