#include "user_common_cpp.h"
#ifndef ENABLE_GUI_LVGL

#ifdef __cplusplus
extern "C" {
#endif

void PowerOntoRunOnce(void) //上电执行一次
{
  static uint8_t OnlyRunOnce = 1;

  if (OnlyRunOnce)
  {
    OnlyRunOnce = 0;
    (void)OS_DELAY(100);//需要延时一点时间，否则SD卡不知为什么会读写异常
    (void)tp_dev.init();//触摸初始化
    /**开机LOGO界面************************************************************/
    gui_disp_pic_logo();
    /**LOGO界面结束************************************************************/
    gui::set_current_display(maindisplayF);
    user_pin_lcd_backlight_ctrl(true);
    osDelay(100);
#ifdef HAS_BOARD_TEST
    //品质测试用的固件20170825
    ccm_param.t_sys.enable_board_test = true;
    gui::set_current_display(board_test_display_function);
#endif
  }
}

void transfileF(void)
{
  if (gui::is_refresh())
  {
    display_picture(25);
  }
}

void TransFileInterfaceControl(void)
{
  static int transflag = 1;
  static int transprintflag = 1;
  static int transpause = 0;

  if (ccm_param.t_sys.is_vsp_trans_file) //是否显示传输界面
  {
    if (transflag)
    {
      transflag = 0;
      transpause = 1;
      transprintflag = 1;
      gui::set_current_display(transfileF);
    }
  }
  else if (print_status.is_print_medium_user_file) //是否打印上传的文件
  {
    if (transprintflag)
    {
      transpause = 0;
      transflag = 1;
      transprintflag = 0;
      user_os_respond_gui_send_sem(FilePrintValue);
      ccm_param.t_sys.print_pause_flag = false;
      ccm_param.t_sys.print_flag = true;
      gui::set_current_display(maindisplayF);
    }
  }
  else  //显示主界面
  {
    if (transpause)
    {
      transpause = 0;
      transprintflag = 0;
      transflag = 1;
      ccm_param.t_sys.print_pause_flag = false;
      ccm_param.t_sys.print_flag = false;
      gui::set_current_display(maindisplayF);
    }
  }
}

void TouchAndRtcControl(void)
{
  if (ccm_param.t_gui.is_refresh_rtc)    //根据公共api变量实时刷新rtc信号
  {
    ccm_param.t_gui.is_refresh_rtc = 0;
    gui::rtc_flag = 1;
    gui::current_display(); //进入模块函数处理信号
    gui::rtc_flag = 0;
  }
}

void gui_task_loop(void)
{
  osDelay(50);
  PowerOntoRunOnce(); //上电后界面显示首先要执行的操作
  TouchAndRtcControl(); //触摸反应和界面数据刷新
  gui_touch_scan(); //触摸扫描函数，处理扫描数据
  gui_disp_error_info_control(); //错误界面显示控制
  TransFileInterfaceControl(); //传输界面控制
  DoorOpenWarningInfo_NotPrinting(); //M14R03,M14S 门打开高温提示
}



#ifdef __cplusplus
} //extern "C" {
#endif
#endif


