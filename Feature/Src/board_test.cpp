#include "user_common_cpp.h"

#ifdef HAS_BOARD_TEST

#include "boardtest.h"
#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t ModelSelect;
extern uint8_t M140_HeatingHotendFinish();

#define NoSelectModel 255

void board_test_display_function(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  if (boardTest.guiInterface())
    return ;
}

void board_test_model_select(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  boardTest.modelSelect();
}

void board_test_cal_heat_time_gui(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  if (boardTest.calHeatTimeGui())
    return ;
}

void board_test_cal_touch_count(void)
{
  if (boardTest.ERRTouchCount())
    return ;
}
void board_test_pressure(void)
{
  boardTest.PressureTest();
  return ;
}

#ifdef __cplusplus
} //extern "C" {
#endif

BoardTest::BoardTest()
{
  isStepTest = 0;
  isFanTest = 0;
  isHeatTest = 0;
  memset(nozzleHeatTimeStr, 0, sizeof(nozzleHeatTimeStr));
  memset(bed50HeatTimeStr, 0, sizeof(bed50HeatTimeStr));
  memset(bed70HeatTimeStr, 0, sizeof(bed70HeatTimeStr));
  memset(bed115HeatTimeStr, 0, sizeof(bed115HeatTimeStr));
  clockTime = 0;
  heatStatus = 0;
  processOn = 1;
}

bool BoardTest::guiInterface(void)
{
  if (gui::is_refresh())
  {
    display_picture(78);
    displayInit();
    displayText();
    ccm_param.t_sys_data_current.have_set_machine = 1;
  }

  if (touchCheck())
    return 1;

  if (gui::is_refresh_rtc())
  {
    displayText();
  }

  return 1;
}

void BoardTest::displayInit(void)
{
  gui_disp_text_noz0_temp_set_range(137, 35, 137, 35, 137, 35);
  gui_disp_text_bed_temp_set_range(345, 35, 345, 35, 345, 35);
}

void BoardTest::displayText(void)
{
  gui_disp_text_noz0_temp_show();

  if (ccm_param.t_sys.enable_hot_bed)
  {
    gui_disp_text_bed_temp_show();
  }
}
menufunc_t lcdtest_lastdisplay;//用于测试触摸计数时记住上一个界面
bool BoardTest::touchCheck(void)
{
  if (gui_touch_xy(352, 111, 460, 294))
  {
    if (user_usb_host_is_mount())
    {
      user_os_respond_gui_send_sem(OpenSDCardValue);
      gui::set_current_display(filescanF);
      return 1;
    }
    else
    {
      gui::set_current_display(NoUdiskF);
      return 1;
    }
  }

  //电机测试
  if (gui_touch_xy(240, 207, 320, 294))
  {
    user_os_respond_gui_send_sem(StepTestValue);
    /*
    //20170825一键测试
    ccm_param.t_gui_semp.GUISempValue = FanTestValue;
    GUISendSempToSYS();
    ccm_param.t_gui_semp.GUISempValue = HeatTestValue;
    GUISendSempToSYS();
    */
    return 1;
  }

  //风扇测试
  if (gui_touch_xy(20, 210, 101, 295))
  {
    user_os_respond_gui_send_sem(FanTestValue);
    return 1;
  }

  //加热测试
  if (gui_touch_xy(130, 207, 215, 294))
  {
    user_os_respond_gui_send_sem(HeatTestValue);
    return 1;
  }

  //最大行程
  if (gui_touch_xy(18, 100, 102, 186))
  {
    user_os_respond_gui_send_sem(RunMaxPos);
    return 1;
  }

  //加熱計時
  if (gui_touch_xy(130, 100, 211, 186))
  {
    user_os_respond_gui_send_sem(CalHeatTime);
    gui::set_current_display(board_test_cal_heat_time_gui);
    return 1;
  }

  //TFTLCD屏误触发计数测试，卢工2017.4.20
  if (gui_touch_xy(240, 100, 320, 186))
  {
    lcdtest_lastdisplay = gui::current_display;
    gui::set_current_display(board_test_cal_touch_count);
    return 1;
  }

  return 0;
}

void BoardTest::modelSelect(void)
{
  if (gui::is_refresh())
  {
    display_picture(31);

    switch (ModelSelect)
    {
    case M14:
      LCD_Fill(89 + 5, 41 + 5, 89 + 5 + 20, 41 + 5 + 12, (u16)testcolor);
      break;

    case M2030:
      LCD_Fill(202 + 5, 41 + 5, 202 + 5 + 20, 41 + 5 + 12, (u16)testcolor);
      break;

    case M2041:
      LCD_Fill(315 + 5, 41 + 5, 315 + 5 + 20, 41 + 5 + 12, (u16)testcolor);
      break;

    case M2048:
      LCD_Fill(424 + 5, 41 + 5, 424 + 5 + 20, 41 + 5 + 12, (u16)testcolor);
      break;

    case M3145:
      LCD_Fill(91 + 5, 119 + 5, 91 + 5 + 20, 119 + 5 + 12, (u16)testcolor);
      break;

    case M4141:
      LCD_Fill(202 + 5, 119 + 5, 202 + 5 + 20, 119 + 5 + 12, (u16)testcolor);
      break;

    case M4040:
      LCD_Fill(315 + 5, 119 + 5, 315 + 5 + 20, 119 + 5 + 12, (u16)testcolor);
      break;

    case M4141S:
      LCD_Fill(424 + 5, 119 + 5, 424 + 5 + 20, 119 + 5 + 12, (u16)testcolor);
      break;

    case AMP410W:
      LCD_Fill(91 + 5, 197 + 5, 91 + 5 + 20, 197 + 5 + 12, (u16)testcolor);
      break;

    case M14R03:
      LCD_Fill(202 + 5, 197 + 5, 202 + 5 + 20, 197 + 5 + 12, (u16)testcolor);
      break;

    case M2030HY:
      LCD_Fill(315 + 5, 197 + 5, 315 + 5 + 20, 197 + 5 + 12, (u16)testcolor);
      break;

    case M14S:
      LCD_Fill(424 + 5, 197 + 5, 424 + 5 + 20, 197 + 5 + 12, (u16)testcolor);
      break;

    default:
      break;
    }
  }

  if (gui_touch_xy(24, 40, 115, 109))
  {
    ModelSelect = M14;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(136, 40, 229, 109))
  {
    ModelSelect = M2030;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(252, 40, 342, 109))
  {
    ModelSelect = M2041;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(362, 40, 452, 109))
  {
    ModelSelect = M2048;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(25, 119, 117, 189))
  {
    ModelSelect = M3145;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(136, 119, 229, 189))
  {
    ModelSelect = M4141;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(252, 119, 342, 189))
  {
    ModelSelect = M4040;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(362, 119, 452, 189))
  {
    ModelSelect = M4141S;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(25, 197, 117, 265))
  {
    ModelSelect = AMP410W;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(136, 197, 229, 265))
  {
    ModelSelect = M14R03;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(252, 197, 342, 265))
  {
    ModelSelect = M2030HY;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(362, 197, 452, 265))
  {
    ModelSelect = M14S;
    gui::set_current_display(board_test_model_select);
    return ;
  }

  if (gui_touch_xy(0, 271, 240, 320))
  {
    if (ModelSelect != NoSelectModel)
    {
      ccm_param.t_sys_data_current.model_id = ModelSelect;
      // 机器型号初始化
      machine_model_init();
    }

    if (ModelSelect == NoSelectModel)
      gui::set_current_display(MachineSetting);
    else
    {
      gui::set_current_display(board_test_display_function);
    }

    ModelSelect = NoSelectModel;
    return ;
  }

  if (gui_touch_xy(240, 271, 420, 320))
  {
    ModelSelect = NoSelectModel;
    gui::set_current_display(MachineSetting);
    return ;
  }
}

void BoardTest::toggleStepStatus(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  if (0 == isStepTest)
  {
    isStepTest = 1;
  }
  else
    isStepTest = 0;
}

void BoardTest::toggleFanStatus(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  if (0 == isFanTest)
    isFanTest = 1;
  else
    isFanTest = 0;
}

void BoardTest::toggleHeatStatus(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  if (0 == isHeatTest)
    isHeatTest = 1;
  else
    isHeatTest = 0;
}

void BoardTest::stepTest(void)
{
  static unsigned long steptest_last_time = 0;

  if (steptest_last_time < xTaskGetTickCount())
  {
    if (1 == isStepTest)
    {
      if (sg_grbl::planner_moves_planned() == 0)
      {
        // 全部正转反转
        user_send_internal_cmd((char *)"G92 X0 Y75 Z0 E75 B75");
        user_send_internal_cmd((char *)"G1 F2400 X75 Y0 Z15 E0 B0");
        user_send_internal_cmd((char *)"G1 F2400 X0 Y75 Z0 E75 B75");
        // X轴正转反转
        user_send_internal_cmd((char *)"G92 X0");
        user_send_internal_cmd((char *)"G1 F2400 X50");
        user_send_internal_cmd((char *)"G1 F2400 X0");
        // Y轴正转反转
        user_send_internal_cmd((char *)"G92 Y50");
        user_send_internal_cmd((char *)"G1 F2400 Y0");
        user_send_internal_cmd((char *)"G1 F2400 Y50");
        // Z轴正转反转
        user_send_internal_cmd((char *)"G92 Z0");
        user_send_internal_cmd((char *)"G1 F2400 Z10");
        user_send_internal_cmd((char *)"G1 F2400 Z0");
        // EB轴正转反转
        user_send_internal_cmd((char *)"G92 E50 B50");
        user_send_internal_cmd((char *)"G1 F2400 E0 B0");
        user_send_internal_cmd((char *)"G1 F2400 E50 B50");
      }
    }
    else
    {
      if (sg_grbl::planner_moves_planned() == 0)
      {
        user_send_internal_cmd((char *)"M84 X Y Z E B");
      }
    }

    steptest_last_time = xTaskGetTickCount() + 500;
  }
}

void BoardTest::fanTest(void)
{
  static int fantest = 0;
  static unsigned long fantest_last_time = 0;

  if (fantest_last_time < xTaskGetTickCount())
  {
    if (1 == isFanTest)
    {
      if (fantest)
      {
        user_fan_control_eb_motor(true);
        user_send_internal_cmd((char *)"M106 S255");
        (void)OS_DELAY(500);
        fantest = 0;
      }
      else
      {
        user_fan_control_eb_motor(false);
        user_send_internal_cmd((char *)"M106 S0");
        fantest = 1;
      }
    }
    else
    {
      user_fan_control_eb_motor(false);
      user_send_internal_cmd((char *)"M106 S0");
    }

    fantest_last_time = xTaskGetTickCount() + 500;
  }
}

void BoardTest::heatTest(void)
{
  //  static int heatest=0;
  static unsigned long heattest_last_time = 0;

  if (heattest_last_time < xTaskGetTickCount())
  {
    if (1 == isHeatTest)
    {
      user_send_internal_cmd((char *)"M140 S100");
      user_send_internal_cmd((char *)"M104 S220");
    }
    else
    {
      user_send_internal_cmd((char *)"M140 S0");
      user_send_internal_cmd((char *)"M104 S0");
    }

    heattest_last_time = xTaskGetTickCount() + 500;
  }
}

void BoardTest::process(void)
{
  if (processOn)
  {
    heatTest();
    fanTest();
    stepTest();
    OS_DELAY(50);
  }
}

void BoardTest::runMaxPos(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  static char gcodeG1XmaxCommandBuf[30] = {0};
  static char gcodeG1YmaxCommandBuf[30] = {0};
  static char gcodeG1ZmaxCommandBuf[30] = {0};
  gcode_cmd_all_axis_relative_mode(false);
  gcode_cmd_eb_absolute_mode();
  user_send_internal_cmd((char *)"G28 X0 Y0");            // XY归零命令
  user_send_internal_cmd((char *)"G28 Z0");               // Z归零命令
  user_send_internal_cmd((char *)"G92 X0 Y0 Z0");         // 设置XYZ当前位置
  memset(gcodeG1ZmaxCommandBuf, 0, sizeof(gcodeG1ZmaxCommandBuf));
  (void)snprintf(gcodeG1ZmaxCommandBuf, sizeof(gcodeG1ZmaxCommandBuf), "G1 Z%f", ccm_param.t_model.xyz_max_pos[2]);
  user_send_internal_cmd((char *)gcodeG1ZmaxCommandBuf);   // Z下降最大
  memset(gcodeG1XmaxCommandBuf, 0, sizeof(gcodeG1XmaxCommandBuf));
  (void)snprintf(gcodeG1XmaxCommandBuf, sizeof(gcodeG1XmaxCommandBuf), "G1 X%f", ccm_param.t_model.xyz_max_pos[0]);
  user_send_internal_cmd((char *)gcodeG1XmaxCommandBuf);   // X移动最大
  memset(gcodeG1YmaxCommandBuf, 0, sizeof(gcodeG1YmaxCommandBuf));
  (void)snprintf(gcodeG1YmaxCommandBuf, sizeof(gcodeG1YmaxCommandBuf), "G1 Y%f", ccm_param.t_model.xyz_max_pos[1]);
  user_send_internal_cmd((char *)gcodeG1YmaxCommandBuf);  // Y移动最大
  user_send_internal_cmd((char *)"G1 X0");                // X归零命令
  user_send_internal_cmd((char *)"G1 Y0");                // Y归零命令
  user_send_internal_cmd((char *)"G1 Z0");                // Z归零命令
}

bool BoardTest::calHeatTimeGuiTouchCheck(void)
{
  return 0;
}

bool BoardTest::calHeatTimeGui(void)
{
  int second = 0, minute = 0, hour = 0;
  static uint8_t HEATING_COMPLETE = 0 ;
  second = (xTaskGetTickCount() / 1000 - clockTime);
  hour = second / 3600;
  minute = (second - hour * 3600) / 60;
  second = (second - hour * 3600) % 60;

  if (sg_grbl::temperature_get_bed_current() >= 50 && heatStatus == 1)
  {
    snprintf(bed50HeatTimeStr, sizeof(bed50HeatTimeStr), "Bed50 Time  = %3d:%02d:%02d", (int)hour, (int)minute, (int)second); // 最多从源串中拷贝n－1个字符到目标串中，然后再在后面加一个0
    gui_disp_text_buf_show((char *)bed50HeatTimeStr, 60, 178, 60, 178, 60, 178);
    heatStatus = 2;
  }

  if (sg_grbl::temperature_get_bed_current() >= 70 && heatStatus == 2)
  {
    snprintf(bed70HeatTimeStr, sizeof(bed70HeatTimeStr), "Bed70 Time  = %3d:%02d:%02d", (int)hour, (int)minute, (int)second); // 最多从源串中拷贝n－1个字符到目标串中，然后再在后面加一个0
    gui_disp_text_buf_show((char *)bed70HeatTimeStr, 60, 220, 60, 220, 60, 220);
    heatStatus = 3;
  }

  if (isM190HeatingComplete() && heatStatus == 3)
  {
    snprintf(bed115HeatTimeStr, sizeof(bed115HeatTimeStr), "Bed115 Time = %3d:%02d:%02d", (int)hour, (int)minute, (int)second); // 最多从源串中拷贝n－1个字符到目标串中，然后再在后面加一个0
    gui_disp_text_buf_show((char *)bed115HeatTimeStr, 60, 266, 60, 266, 60, 266);
    heatStatus = 4;
  }

  if ((M140_HeatingHotendFinish() == 1) && (HEATING_COMPLETE == 0))
  {
    HEATING_COMPLETE = 1 ;
    second = (xTaskGetTickCount() / 1000 - clockTime);
    hour = second / 3600;
    minute = (second - hour * 3600) / 60;
    second = (second - hour * 3600) % 60;
    snprintf(nozzleHeatTimeStr, sizeof(nozzleHeatTimeStr), "Nozzle Time = %3d:%02d:%02d", (int)hour, (int)minute, (int)second); // 最多从源串中拷贝n－1个字符到目标串中，然后再在后面加一个0
    gui_disp_text_buf_show((char *)nozzleHeatTimeStr, 60, 130, 60, 130, 60, 130);
  }

  if (gui::is_refresh())
  {
    display_picture(79);
    displayInit();
    displayText();
    gui_disp_text_time_set_range(62 + 12 * 14, 86, 62 + 12 * 14, 86, 62 + 12 * 14, 86);
  }

  if (calHeatTimeGuiTouchCheck())
    return 1;

  if (gui::is_refresh_rtc())
  {
    displayText();
    gui_disp_text_buf_show((char *)"Clock Time  = ", 62, 86, 62, 86, 62, 86);
    gui_disp_text_time_show((xTaskGetTickCount() / 1000 - clockTime));
  }

  if ((heatStatus == 4) && (HEATING_COMPLETE == 1))
    user_buzzer_beep(500);

  return 1;
}

void BoardTest::calHeatTime(void)
{
  if (!ccm_param.t_sys.enable_board_test)
    return;

  processOn = 0;
  ccm_param.t_sys.is_gcode_m109_done = false;
  resetM190HeatingComplete();

  if (!ccm_param.t_sys.enable_hot_bed)
  {
    heatStatus = 4;
    snprintf(bed50HeatTimeStr, sizeof(bed50HeatTimeStr), "Bed50 Time  =   0: 0: 0");
    snprintf(bed70HeatTimeStr, sizeof(bed70HeatTimeStr), "Bed75 Time  =   0: 0: 0");
    snprintf(bed115HeatTimeStr, sizeof(bed115HeatTimeStr), "Bed115 Time =   0: 0: 0");
  }
  else
    heatStatus = 1;

  clockTime = xTaskGetTickCount() / 1000;
  user_send_internal_cmd((char *)"M104 S220");    // 加热喷嘴，不等待
  user_send_internal_cmd((char *)"M190 S115");  // 加热热床，等待
  user_send_internal_cmd((char *)"M109 S220");    // 防止加热热床比喷嘴快，导致喷嘴无加热
  user_send_internal_cmd((char *)"M104 S0");      // 喷嘴温度置零
  user_send_internal_cmd((char *)"M140 S0");      // 热床温度置零
}
/*
 *卢工屏幕触摸计数测试2017421
*/
uint32_t Lcd_Count_touch = 0;//在检测按键触摸处使用
uint8_t BoardTest::ERRTouchCount(void)
{
  int second = 0;
  char buffer[32];

  if (gui::is_refresh())
  {
    display_picture(94);//94
  }

  second = (xTaskGetTickCount() / 1000 - clockTime);

  if (gui::is_refresh_rtc()) //按一次或500ms更新一次数据
  {
    gui_disp_set_text_buf_color(RED);
    gui_disp_text_buf_show((char *)"Clock Time  = ", 62, 86, 62, 86, 62, 86);
    snprintf(buffer, sizeof(buffer), "%3d:%02d:%02d", second / 3600, second % 3600 / 60, second % 3600 % 60);
    gui_disp_text_buf_show((char *)buffer, 62 + 12 * 14, 86, 62 + 12 * 14, 86, 62 + 12 * 14, 86);
    snprintf(buffer, sizeof(buffer), "Coutn_touch[0]=%d", (Lcd_Count_touch % (256 * 256))); //Lcd_Count_touch&0x0000ffff
    gui_disp_text_buf_show((char *)buffer, 60, 178, 60, 178, 60, 178);
    snprintf(buffer, sizeof(buffer), "Coutn_touch[1]=%d", Lcd_Count_touch / (256 * 256));
    gui_disp_text_buf_show((char *)buffer, 60, 266, 60, 266, 60, 266);
  }

  if (gui_touch_xy(0, 0, 150, 65))     //返回按钮
  {
    if (ccm_param.t_sys.enable_board_test) Lcd_Count_touch++;

    if (lcdtest_lastdisplay != board_test_display_function)
      ccm_param.t_sys.enable_board_test = false;

    gui::set_current_display(lcdtest_lastdisplay);
  }

  if (gui_touch_xy(0, 0, 480, 320))
  {
    if (ccm_param.t_sys.enable_board_test) //从测试按键进来的才执行这个计数，且按一次计数一次，一直按着不会计数。为了防止测试界面进来不计数
      Lcd_Count_touch++;                   //非打印状态主界面和打印状态主界面中的隐藏按键，进来的不执行该计数2017422
  }

  if (gui_touch_xy(400, 0, 480, 65))     //清零按钮
    Lcd_Count_touch = 0;

  return 1;
}
#define Debug_PresureSensor
#ifdef Debug_PresureSensor
void BoardTest::PressureTest(void)
{
  char buffer[32];
  static uint8_t Loop = 0, count = 0;

  if (gui::is_refresh())
  {
    display_picture(94);//94
  }

  if (gui::is_refresh_rtc()) //按一次或500ms更新一次数据
  {
    snprintf(buffer, sizeof(buffer), "Clock Time  = ");
    gui::disp_text((uint8_t *)buffer, 62, 86, 24, (u16)testcolor);
    count++;
    count &= 0x03;
  }

  if (gui_touch_xy(0, 0, 150, 65))     //返回按钮
  {
    gui::set_current_display(lcdtest_lastdisplay);
  }

  if (gui_touch_xy(0, 70, 150, 320))
  {
    Loop = 1;
  }

  if (gui_touch_xy(150, 0, 300, 320))
  {
    Loop = 0;
  }

  if (Loop)
  {
    if (count == 0)
    {
      user_send_internal_cmd((char *)"G1 Z0");
    }
    else if (count == 2)
    {
      user_send_internal_cmd((char *)"G1 Z30");
    }
  }

  static uint8_t Zposition = 0;

  if (gui_touch_xy(300, 0, 480, 150))
  {
    if (Zposition < 255)
      Zposition++;

    sprintf(buffer, "G1 Z%d", Zposition);
    user_send_internal_cmd(buffer);
  }
  else if (gui_touch_xy(300, 170, 480, 320))
  {
    if (Zposition > 0)
      Zposition--;

    sprintf(buffer, "G1 Z%d", Zposition);
    user_send_internal_cmd(buffer);
  }
}
#endif

BoardTest boardTest;

#endif



