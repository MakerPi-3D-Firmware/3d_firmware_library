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
    z = flash_param_t.idex_ext1_ext0_offset[2];   //???????????????  0-200   0-2.0
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

  // ????????????????????????
  if (!ccm_param.t_sys.is_gcode_g28_done)
    gcode_cmd_g28();

  // ??????xyz???????????????
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
  case BackZeroValue:  //??????
    gui_homing();
    break;

  case DisableStepValue:  //????????????
    USER_SEND_INTERNAL_CMD_BUF("M84 X Y E B");         // ??????XYEB
    ccm_param.t_sys.is_gcode_g28_done = false;
    break;

  case PreHeatPLAValue:  //??????PLA
    GUI_SET_TARGET_TEMP(true, true, ccm_param.t_sys.pla_preheat_ext_temp, ccm_param.t_sys.pla_preheat_ext_temp, ccm_param.t_sys.pla_preheat_bed_temp);
    break;

  case CoolDownValue:  //??????
    GUI_SET_TARGET_TEMP(true, true, 0, 0, 0);
    break;

  case FeedFilamentValue:  //??????
    feature_filament::control_start_load();
    break;

  case StopFeedFilamentValue:  //????????????
    feature_filament::control_cancel_process();
    break;

  case BackFilamentValue:  //??????
    feature_filament::control_start_unload();
    break;

  case StopBackFilamentValue:  //????????????
    //      USER_EchoLogStr("DEBUG: StopBackFilament ok!\r\n");
    feature_filament::control_cancel_process();
    break;

  case OpenSDCardValue:  //??????SD???
    USER_EchoLog("OpenSDCard ok!");
    GUIOpenSDCard();
    break;

  case OpenDirValue:  //????????????
    USER_EchoLog("OpenDir ok!");
    GUIOpenSDDir();
    break;

  case BackLastDirValue:  //??????????????????
    USER_DbgLog("BackLastDir ok!");
    GUIBackSDLastDir();
    break;

  case NextPageValue:  //?????????
    USER_DbgLog("NextPage ok!");
    GUINextPage();
    break;

  case LastPageValue:  //?????????
    USER_DbgLog("LastPage ok!");
    GUILastPage();
    break;

  case FilePrintValue:  //????????????????????????
    USER_DbgLog("FilePrint ok!");
    feature_print_control::file_print_start();
    break;

  case PausePrintValue:  //????????????
    USER_DbgLog("PausePrint ok!");
    ccm_param.t_sys.print_pause_flag = true;
    feature_print_control::pause_start();
    break;

  case ResumePrintValue:  //????????????
    USER_DbgLog("ResumePrint ok!");
    ccm_param.t_sys.print_pause_flag = false;
    feature_print_control::resume_start();
    break;

  case StopPrintValue:  //????????????
    USER_DbgLog("StopPrint ok!");
    ccm_param.t_sys.print_flag = false;
    ccm_param.t_sys.print_pause_flag = false;
    GUI_SET_TARGET_TEMP(true, true, 0, 0, 0);
    feature_print_control::stop_start();
    break;

  case OpenBeep:  //???????????????
    USER_DbgLog("OpenBeep ok!");

    if (ccm_param.t_sys.sound_alarm)
      user_buzzer_set_alarm_status(true);

    break;

  case CloseBeep:  //???????????????
    USER_DbgLog("CloseBeep ok!");
    user_buzzer_set_alarm_status(false);
    break;

  case SysErrValue:  //????????????
    break;

  case PrintSetValue_M14:  //M14?????? ????????????
    USER_SEND_INTERNAL_CMD_BUF("M104 T0 S%d", ccm_param.t_gui_semp.TargetNozzleTemp);
    USER_SEND_INTERNAL_CMD_BUF("M106 S%d", ccm_param.t_gui_semp.FanSpeed);
    SetFeedMultiply(ccm_param.t_gui_semp.PrintSpeed); //int PrintSpeed :100->1 200->2 ???????????????
    break;

  case PreHeatABSValue:  //??????ABS
    GUI_SET_TARGET_TEMP(true, true, ccm_param.t_sys.abs_preheat_ext_temp, ccm_param.t_sys.abs_preheat_ext_temp, ccm_param.t_sys.abs_preheat_bed_temp);
    break;

  case MoveXYZValue:  //?????????????????????
    gui_move_xyz((float (&)[XYZ_NUM_AXIS])ccm_param.t_gui.move_xyz_pos);
    //    GUI_MoveXYZ(ccm_param.t_gui.move_x_pos,ccm_param.t_gui.move_y_pos,ccm_param.t_gui.move_z_pos);
    gcode_cmd_gui_wait_done();
    break;

  case ConfirmChangeFilamentValue:  //??????????????????
    feature_filament::mid_chg_start();
    break;

  case ConfirmLoadFilamentValue:  //???????????????????????????
    feature_filament::mid_chg_confirm_load();
    break;

  case ConfirmChangeModelValue:  //????????????
    USER_EchoLogStr("ConfirmChangeModelValue ok!\r\n");//??????????????????????????????2017.7.6
    SaveSelectedModel();
    break;

  case ConfirmChangePictureValue:  //
    SaveSelectedPicture();
    break;

  case ConfirmChangeFunctionValue:  //????????????
    //      USER_EchoLogStr("ConfirmChangeFunctionValue ok!\r\n");//??????????????????????????????2017.7.6
    SaveSelectedFunction();
    break;

  case PrintSetValue_NotM14_Left:  //???M14?????? ??????????????? ????????????
    sg_grbl::temperature_set_extruder_target(ccm_param.t_gui_semp.TargetNozzleTemp, 0);

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      sg_grbl::temperature_set_extruder_target(ccm_param.t_gui_semp.TargetNozzleTemp1, 1);
    }

    SetFeedMultiply(ccm_param.t_gui_semp.PrintSpeed); //int PrintSpeed :100->1 200->2 ???????????????
    break;

  case PrintSetValue_NotM14_Right:  //???M14?????? ??????????????? ????????????
    sg_grbl::temperature_set_bed_target(ccm_param.t_gui_semp.TargetHotbedTemp);
    feature_control_set_fan_speed(ccm_param.t_gui_semp.FanSpeed);
    break;

  case ConfirmPowerOffRecover:  //?????????????????????
    user_buzzer_set_alarm_status(false);
    feature_pow_rec_ready_to_print();
    break;

  case CancelPowerOffRecover:  //??????????????????
    if (0 != ccm_param.t_sys_data_current.enable_powerOff_recovery)
    {
      user_buzzer_set_alarm_status(false);
      flash_poweroff_data_reset();
      feature_pow_data_delete_file_from_sd();
      feature_pow_data_reset_flag();
    }

    break;

  case CalculateZMaxPos:  //????????????
    if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
    {
      feature_pow_cal_z_start();
    }

    break;

  case MatCheckCalibrateValue:  //????????????????????????
    break;

  case PauseToResumeNozzleTemp: //??????????????????????????????????????????????????????????????????
    break;
#ifdef HAS_BOARD_TEST

  case StepTestValue:  //????????????
    boardTest.toggleStepStatus();
    break;

  case FanTestValue:  //????????????
    boardTest.toggleFanStatus();
    break;

  case HeatTestValue:  //????????????
    boardTest.toggleHeatStatus();
    break;

  case RunMaxPos:  //????????????
    boardTest.runMaxPos();
    break;

  case CalHeatTime:  //????????????
    boardTest.calHeatTime();
    break;
#endif

  case ConfirmChangelogoValue:  //??????logo??????
    SaveSelectedlogo();
    break;

  case PreHeatBedValue:
    GUI_SET_TARGET_TEMP(false, true, 0, 0, 100);
    break;

  case PrintSetValue_Cavity:
    if (ccm_param.t_gui.target_cavity_temp_on != ccm_param.t_gui_semp.TargetCavityOnTemp) //??????????????????
      ccm_param.t_gui.target_cavity_temp_on = ccm_param.t_gui_semp.TargetCavityOnTemp;

    if (ccm_param.t_gui.target_cavity_temp_on)
    {
      if (ccm_param.t_gui.target_cavity_temp != ccm_param.t_gui_semp.TargetCavityTemp) //??????????????????
      {
        ccm_param.t_gui.target_cavity_temp = ccm_param.t_gui_semp.TargetCavityTemp;
        // ????????????
        sg_grbl::temperature_set_cavity_target(ccm_param.t_gui.target_cavity_temp);
      }
    }
    else
    {
      if (ccm_param.t_gui.target_cavity_temp != 0) //??????????????????
      {
        ccm_param.t_gui.target_cavity_temp = 0;
        sg_grbl::temperature_set_cavity_target(ccm_param.t_gui.target_cavity_temp);
      }
    }

    break;

  // ??????????????????
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
  if (osSemaphoreWait(GUISendSemHandle, osWaitForever) == osOK) //GUI???????????????
  {
    (void)OS_DELAY(50);
    ScanGUISemStatus();
    (void)OS_DELAY(50);
  }
}


#ifdef __cplusplus
} // extern "C" {
#endif



