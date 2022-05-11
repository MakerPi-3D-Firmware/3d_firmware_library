#include "user_common_cpp.h"
#include "UdiskControl.h"
#ifdef __cplusplus
extern "C" {
#endif

extern osSemaphoreId GUISendSemHandle;
extern osSemaphoreId GUIWaitSemHandle;

#define GUI_SET_TARGET_TEMP(IS_EXT, IS_BED, EXT0_TEMP, EXT1_TEMP, BED_TEMP) \
  { \
    if(IS_EXT) \
    { \
      USER_SEND_INTERNAL_CMD_BUF("M104 T0 S%d", EXT0_TEMP); \
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL) \
      { \
        USER_SEND_INTERNAL_CMD_BUF("M104 T1 S%d", EXT1_TEMP); \
      } \
    } \
    if(IS_BED) \
    { \
      if (ccm_param.t_sys.enable_hot_bed) \
      { \
        USER_SEND_INTERNAL_CMD_BUF("M140 S%d", BED_TEMP); \
      } \
    } \
    USER_SEND_INTERNAL_CMD_BUF("M107"); \
  }

void gui_home_pos_calibration(void)
{
  float x, y, z;

  if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
  {
    x = flash_param_t.idex_ext0_home_pos_adding[0];
    y = flash_param_t.idex_ext0_home_pos_adding[1];
    z = flash_param_t.idex_ext0_home_pos_adding[2];
  }
  else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
  {
    x = flash_param_t.mix_ext0_home_pos_adding[0];
    y = flash_param_t.mix_ext0_home_pos_adding[1];
    z = flash_param_t.mix_ext0_home_pos_adding[2];
  }
  else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
  {
    x = flash_param_t.laser_ext0_home_pos_adding[0];
    y = flash_param_t.laser_ext0_home_pos_adding[1];
    z = flash_param_t.laser_ext0_home_pos_adding[2];
  }

  //  z = 5.0f;
  gcode_cmd_t0(false);
  USER_SEND_INTERNAL_CMD_BUF("G28 S-1");
  USER_SEND_INTERNAL_CMD_BUF("G1 F3600 X%f Y%f Z10", x, y);
  USER_SEND_INTERNAL_CMD_BUF("G1 F300 X%f Y%f Z%f", x, y, z);
  gcode_cmd_gui_wait_done();
}

void gui_home_e1_pos_calibration(void)
{
  float x, y, z;

  if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
  {
    x = flash_param_t.idex_extruder_1_bed_offset[1] + flash_param_t.idex_ext1_ext0_offset[0];
    y = flash_param_t.idex_ext0_home_pos_adding[1] + flash_param_t.idex_ext1_ext0_offset[1];
    z = flash_param_t.idex_ext1_ext0_offset[2];   //转换成小数  0-200   0-2.0
  }

  //  z = 5.0f;
  USER_SEND_INTERNAL_CMD_BUF("G28 S-1");
  gcode_cmd_t1(false);
  //  USER_SEND_INTERNAL_CMD_BUF("G1 F3600 X%f Y%f Z10", flash_param_t.idex_extruder_1_bed_offset[1], flash_param_t.idex_ext0_home_pos_adding[1]);
  USER_SEND_INTERNAL_CMD_BUF("G1 F1800 X%f Y%f Z10", x, y);
  USER_SEND_INTERNAL_CMD_BUF("G1 F300 X%f Y%f Z%f", x, y, z);
  gcode_cmd_gui_wait_done();
}

void gui_ext_bed_pos_set(void)
{
  ccm_param.t_model.xyz_home_pos[X2_AXIS] = ccm_param.t_model.xyz_move_max_pos[X2_AXIS];
  gcode_cmd_t0(false);
  gcode_cmd_g28();
  USER_SEND_INTERNAL_CMD_BUF("G1 F300 Z10");
  gcode_cmd_gui_wait_done();
}

void gui_bed_level_platform_out(void)
{
  USER_SEND_INTERNAL_CMD_BUF("G28 X Y");

  if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
  {
    USER_SEND_INTERNAL_CMD_BUF("G1 F3600 Y%f", ccm_param.t_model.xyz_max_pos[Y_AXIS]);
  }

  gcode_cmd_gui_wait_done();
}

void gui_e0_to_datum_pos(void)
{
  gcode_cmd_g28();
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F600 Z10");
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F1800 X%.2f Y%.2f",
                             flash_param_t.idex_extruder_0_bed_offset[0] + flash_param_t.idex_ext1_ext0_datum_pos[0],
                             flash_param_t.idex_ext1_ext0_datum_pos[1]);
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F600 Z%.2f",
                             flash_param_t.idex_ext1_ext0_datum_pos[2]);
  gcode_cmd_gui_wait_done();
}

void gui_e1_to_datum_pos(void)
{
  gcode_cmd_t1(true);
  gcode_cmd_g28();
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F600 Z10");
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F1800 X%.2f Y%.2f",
                             flash_param_t.idex_extruder_1_bed_offset[0] + flash_param_t.idex_ext1_ext0_datum_pos[0] + flash_param_t.idex_ext1_ext0_offset[0],
                             flash_param_t.idex_ext1_ext0_datum_pos[1] + flash_param_t.idex_ext1_ext0_offset[1]);
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F600 Z%.2f",
                             flash_param_t.idex_ext1_ext0_datum_pos[2] + flash_param_t.idex_ext1_ext0_offset[2]);
  gcode_cmd_gui_wait_done();
}

void gui_laser_to_focus_pos(void)
{
  if (ccm_param.t_sys_data_current.model_id == P3_Pro && flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
  {
    if (!ccm_param.t_sys.is_gcode_g28_done)
      gcode_cmd_g28();

    gcode_cmd_t1(true);
    USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F600 Z%.1f", flash_param_t.laser_focus_z_height);
    USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F2400 X%d Y100", (int)flash_param_t.laser_extruder_1_bed_offset[0] + 150);
    gcode_cmd_gui_wait_done();
  }
}

void gui_homing(void)
{
  ccm_param.t_sys.is_gcode_g28_done = false;

  if (feature_print_control::idex_sys_is_extruder() ||
      flash_param_t.extruder_type == EXTRUDER_TYPE_LASER ||
      flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
      flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
  {
    gcode_cmd_t0(false);
    gcode_cmd_g28();
  }
  else
  {
    gcode_cmd_g28();
  }

  gcode_cmd_gui_wait_done();

  for (int i = 0; i < 3; i ++)
    ccm_param.t_gui.move_xyz_pos[i] = ccm_param.t_model.xyz_home_pos[i];
}

void gui_leveling(void)
{
  gcode::g29_complete_flag = false;

  if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
  {
    if (!ccm_param.t_sys.is_gcode_g28_done)
      gcode_cmd_g28();

    user_send_internal_cmd((char *)"G29");
    user_send_internal_cmd((char *)"M84 X Y");
  }

  gcode_cmd_gui_wait_done();
}

void gui_move_xyz(float (&xyz_mm)[XYZ_NUM_AXIS])
{
  if (feature_print_control::idex_sys_is_extruder() ||
      flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
      flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
  {
    gcode_cmd_t0(false);
  }

  if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
  {
    gcode_cmd_t1(false);
  }

  // 判断是否已经归零
  if (!ccm_param.t_sys.is_gcode_g28_done)
    gcode_cmd_g28();

  // 限制xyz最小最大值
  for (int i = 0; i < XYZ_NUM_AXIS; i++)
  {
    if (xyz_mm[i] < ccm_param.t_model.xyz_min_pos[i])
      xyz_mm[i] = (int)ccm_param.t_model.xyz_min_pos[i];

    if (xyz_mm[i] > ccm_param.t_model.xyz_max_pos[i])
      xyz_mm[i] = (int)ccm_param.t_model.xyz_max_pos[i];
  }

  USER_SEND_INTERNAL_CMD_BUF("G1 F1500 X%f Y%f Z%f", (float)xyz_mm[X_AXIS], (float)xyz_mm[Y_AXIS], (float)xyz_mm[Z_AXIS]);
}


void ScanGUISemStatus(void)
{
  switch (ccm_param.t_gui_semp.index_v)
  {
  case BackZeroValue:  //回零
    gui_homing();
    break;

  case DisableStepValue:  //解锁电机
    USER_SEND_INTERNAL_CMD_BUF("M84 X Y E B");         // 解锁XYEB
    ccm_param.t_sys.is_gcode_g28_done = false;
    break;

  case PreHeatPLAValue:  //预热PLA
    GUI_SET_TARGET_TEMP(true, true, ccm_param.t_sys.pla_preheat_ext_temp, ccm_param.t_sys.pla_preheat_ext_temp, ccm_param.t_sys.pla_preheat_bed_temp);
    break;

  case CoolDownValue:  //冷切
    GUI_SET_TARGET_TEMP(true, true, 0, 0, 0);
    break;

  case FeedFilamentValue:  //进料
    feature_filament::control_start_load();
    break;

  case StopFeedFilamentValue:  //退出进料
    feature_filament::control_cancel_process();
    break;

  case BackFilamentValue:  //退料
    feature_filament::control_start_unload();
    break;

  case StopBackFilamentValue:  //退出退料
    //      USER_EchoLogStr("DEBUG: StopBackFilament ok!\r\n");
    feature_filament::control_cancel_process();
    break;

  case OpenSDCardValue:  //打开SD卡
    USER_EchoLog("OpenSDCard ok!");
    GUIOpenSDCard();
    break;

  case OpenDirValue:  //打开目录
    USER_EchoLog("OpenDir ok!");
    GUIOpenSDDir();
    break;

  case BackLastDirValue:  //返回上层目录
    USER_DbgLog("BackLastDir ok!");
    GUIBackSDLastDir();
    break;

  case NextPageValue:  //下一页
    USER_DbgLog("NextPage ok!");
    GUINextPage();
    break;

  case LastPageValue:  //上一页
    USER_DbgLog("LastPage ok!");
    GUILastPage();
    break;

  case FilePrintValue:  //选中文件确定打印
    USER_DbgLog("FilePrint ok!");
    feature_print_control::file_print_start();
    break;

  case PausePrintValue:  //暂停打印
    USER_DbgLog("PausePrint ok!");
    ccm_param.t_sys.print_pause_flag = true;
    feature_print_control::pause_start();
    break;

  case ResumePrintValue:  //继续打印
    USER_DbgLog("ResumePrint ok!");
    ccm_param.t_sys.print_pause_flag = false;
    feature_print_control::resume_start();
    break;

  case StopPrintValue:  //停止打印
    USER_DbgLog("StopPrint ok!");
    ccm_param.t_sys.print_flag = false;
    ccm_param.t_sys.print_pause_flag = false;
    GUI_SET_TARGET_TEMP(true, true, 0, 0, 0);
    feature_print_control::stop_start();
    break;

  case OpenBeep:  //打开蜂鸣器
    USER_DbgLog("OpenBeep ok!");

    if (ccm_param.t_sys.sound_alarm)
      user_buzzer_set_alarm_status(true);

    break;

  case CloseBeep:  //关闭蜂鸣器
    USER_DbgLog("CloseBeep ok!");
    user_buzzer_set_alarm_status(false);
    break;

  case SysErrValue:  //系统错误
    break;

  case PrintSetValue_M14:  //M14机型 打印设置
    USER_SEND_INTERNAL_CMD_BUF("M104 T0 S%d", ccm_param.t_gui_semp.TargetNozzleTemp);
    USER_SEND_INTERNAL_CMD_BUF("M106 S%d", ccm_param.t_gui_semp.FanSpeed);
    SetFeedMultiply(ccm_param.t_gui_semp.PrintSpeed); //int PrintSpeed :100->1 200->2 写打印速度
    break;

  case PreHeatABSValue:  //预热ABS
    GUI_SET_TARGET_TEMP(true, true, ccm_param.t_sys.abs_preheat_ext_temp, ccm_param.t_sys.abs_preheat_ext_temp, ccm_param.t_sys.abs_preheat_bed_temp);
    break;

  case MoveXYZValue:  //移动光轴确定键
    gui_move_xyz((float (&)[XYZ_NUM_AXIS])ccm_param.t_gui.move_xyz_pos);
    //    GUI_MoveXYZ(ccm_param.t_gui.move_x_pos,ccm_param.t_gui.move_y_pos,ccm_param.t_gui.move_z_pos);
    gcode_cmd_gui_wait_done();
    break;

  case ConfirmChangeFilamentValue:  //确认中途换料
    feature_filament::mid_chg_start();
    break;

  case ConfirmLoadFilamentValue:  //中途换料中确认进料
    feature_filament::mid_chg_confirm_load();
    break;

  case ConfirmChangeModelValue:  //更改机型
    USER_EchoLogStr("ConfirmChangeModelValue ok!\r\n");//串口上传信息到上位机2017.7.6
    SaveSelectedModel();
    break;

  case ConfirmChangePictureValue:  //
    SaveSelectedPicture();
    break;

  case ConfirmChangeFunctionValue:  //更改功能
    //      USER_EchoLogStr("ConfirmChangeFunctionValue ok!\r\n");//串口上传信息到上位机2017.7.6
    SaveSelectedFunction();
    break;

  case PrintSetValue_NotM14_Left:  //非M14机型 点击左上方 打印设置
    sg_grbl::temperature_set_extruder_target(ccm_param.t_gui_semp.TargetNozzleTemp, 0);

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      sg_grbl::temperature_set_extruder_target(ccm_param.t_gui_semp.TargetNozzleTemp1, 1);
    }

    SetFeedMultiply(ccm_param.t_gui_semp.PrintSpeed); //int PrintSpeed :100->1 200->2 写打印速度
    break;

  case PrintSetValue_NotM14_Right:  //非M14机型 点击右上方 打印设置
    sg_grbl::temperature_set_bed_target(ccm_param.t_gui_semp.TargetHotbedTemp);
    feature_control_set_fan_speed(ccm_param.t_gui_semp.FanSpeed);
    break;

  case ConfirmPowerOffRecover:  //断电续打确认键
    user_buzzer_set_alarm_status(false);
    feature_pow_rec_ready_to_print();
    break;

  case CancelPowerOffRecover:  //断电续打取消
    if (0 != ccm_param.t_sys_data_current.enable_powerOff_recovery)
    {
      user_buzzer_set_alarm_status(false);
      flash_poweroff_data_reset();
      feature_pow_data_delete_file_from_sd();
      feature_pow_data_reset_flag();
    }

    break;

  case CalculateZMaxPos:  //测量行程
    if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
    {
      feature_pow_cal_z_start();
    }

    break;

  case MatCheckCalibrateValue:  //断料检测模块校准
    break;

  case PauseToResumeNozzleTemp: //断料状态下去换料，先把暂停打印降低的温度恢复
    break;
#ifdef HAS_BOARD_TEST

  case StepTestValue:  //电机测试
    boardTest.toggleStepStatus();
    break;

  case FanTestValue:  //风扇测试
    boardTest.toggleFanStatus();
    break;

  case HeatTestValue:  //加热测试
    boardTest.toggleHeatStatus();
    break;

  case RunMaxPos:  //加热测试
    boardTest.runMaxPos();
    break;

  case CalHeatTime:  //加热計時
    boardTest.calHeatTime();
    break;
#endif

  case ConfirmChangelogoValue:  //确定logo设置
    SaveSelectedlogo();
    break;

  case PreHeatBedValue:
    GUI_SET_TARGET_TEMP(false, true, 0, 0, 100);
    break;

  case PrintSetValue_Cavity:
    if (ccm_param.t_gui.target_cavity_temp_on != ccm_param.t_gui_semp.TargetCavityOnTemp) //防止二次设置
      ccm_param.t_gui.target_cavity_temp_on = ccm_param.t_gui_semp.TargetCavityOnTemp;

    if (ccm_param.t_gui.target_cavity_temp_on)
    {
      if (ccm_param.t_gui.target_cavity_temp != ccm_param.t_gui_semp.TargetCavityTemp) //防止二次设置
      {
        ccm_param.t_gui.target_cavity_temp = ccm_param.t_gui_semp.TargetCavityTemp;
        // 设置温度
        sg_grbl::temperature_set_cavity_target(ccm_param.t_gui.target_cavity_temp);
      }
    }
    else
    {
      if (ccm_param.t_gui.target_cavity_temp != 0) //防止二次设置
      {
        ccm_param.t_gui.target_cavity_temp = 0;
        sg_grbl::temperature_set_cavity_target(ccm_param.t_gui.target_cavity_temp);
      }
    }

    break;

  // 校准平台接口
  case StartCalBedLevel:
    gui_leveling();
    break;

  case ResetEBValue:
    gcode_cmd_eb_reset();
    break;

  case SwitchLaser:
    gcode_cmd_t1(false);
    gcode_cmd_eb_reset();
    break;

  case LaserMoveZUp:
    break;

  case IdexMoveX:
    char buf[30];

    if (ccm_param.t_gui_semp.active_extruder == 0)
    {
      gcode_cmd_t0(false);
      (void)snprintf(buf, sizeof(buf), "G1 F1200 X%d D1", ccm_param.t_gui_semp.x_move_value);
      user_send_internal_cmd((char *)buf);
    }
    else if (ccm_param.t_gui_semp.active_extruder == 1)
    {
      gcode_cmd_t1(false);
      (void)snprintf(buf, sizeof(buf), "G1 F1200 X%d D1", ccm_param.t_gui_semp.x_move_value);
      user_send_internal_cmd((char *)buf);
      gcode_cmd_t0(false);
    }

    break;

  case HomePosCalibration:
    gui_home_pos_calibration();
    break;

  case ExtBedPosSet:
    gui_ext_bed_pos_set();
    break;

  case BedLevelStart:
    gui_bed_level_platform_out();
    break;

  case BedLevelEnd:
    gui_bed_level_platform_out();
    break;

  case E0ToDatumPos:
    gui_e0_to_datum_pos();
    break;

  case E1ToDatumPos:
    gui_e1_to_datum_pos();
    break;

  case LaserFocusPos:
    gui_laser_to_focus_pos();
    break;

  case HomeE1PosCalibration:
    gui_home_e1_pos_calibration();
    break;

  default:
    USER_DbgLog("GUISempValue is not effective Value");
  }

  (void)osSemaphoreRelease(GUIWaitSemHandle);
}


void respond_gui_task_loop(void)
{
  if (osSemaphoreWait(GUISendSemHandle, osWaitForever) == osOK) //GUI信号量处理
  {
    (void)OS_DELAY(50);
    ScanGUISemStatus();
    (void)OS_DELAY(50);
  }
}


#ifdef __cplusplus
} // extern "C" {
#endif



