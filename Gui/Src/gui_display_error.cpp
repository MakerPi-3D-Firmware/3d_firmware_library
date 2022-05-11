#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint8_t gui_disp_error_select;

#define GUI_DISP_ERR_STR(ERR_TEXT, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s) \
  { \
    if (PICTURE_IS_CHINESE == ccm_param.t_sys_data_current.pic_id) \
    { \
      gui_disp_text_buf_show_m((char *)LCD_##ERR_TEXT##_STR_CN, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s, true);\
    } \
    else if (PICTURE_IS_ENGLISH == ccm_param.t_sys_data_current.pic_id) \
    { \
      gui_disp_text_buf_show_m((char *)LCD_##ERR_TEXT##_STR_EN, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s, true);\
    } \
    else if (PICTURE_IS_CHINESE_TRADITIONAL == ccm_param.t_sys_data_current.pic_id) \
    { \
      gui_disp_text_buf_show_m((char *)LCD_##ERR_TEXT##_STR_TC, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s, true);\
    } \
  }

void gui_disp_error_text_show(void)
{
  switch (gui_disp_error_select)
  {
  case ERR_MIN_TEMP:
    GUI_DISP_ERR_STR(ERR_MIN_TEMP, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_MAX_TEMP:
    GUI_DISP_ERR_STR(ERR_MAX_TEMP, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_MAX_TEMP_BED:
    GUI_DISP_ERR_STR(ERR_MAX_TEMP_BED, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_HEAT_FAIL:
    GUI_DISP_ERR_STR(ERR_HEAT_FAIL, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_X_MIN_LIMIT:
    GUI_DISP_ERR_STR(ERR_X_MIN_LIMIT, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_Y_MIN_LIMIT:
    GUI_DISP_ERR_STR(ERR_Y_MIN_LIMIT, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_Z_MIN_LIMIT:
    GUI_DISP_ERR_STR(ERR_Z_MIN_LIMIT, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_X_MAX_LIMIT:
    GUI_DISP_ERR_STR(ERR_X_MAX_LIMIT, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_Y_MAX_LIMIT:
    GUI_DISP_ERR_STR(ERR_Y_MAX_LIMIT, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_Z_MAX_LIMIT:
    GUI_DISP_ERR_STR(ERR_Z_MAX_LIMIT, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_IWDG_RESET:
    GUI_DISP_ERR_STR(ERR_IWDG_RESET, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_FATFS:
    GUI_DISP_ERR_STR(ERR_FATFS, 0, 138, 0, 112, 0, 234);
    break;

  case ERR_THERMISTOR_FALLS:
    GUI_DISP_ERR_STR(ERR_THERMISTOR_FALLS, 0, 138, 0, 112, 0, 234);
    break;

  default:
    break;
  }
}

uint8_t err_info_is_show = ERR_INFO_SHOW;

static void gui_disp_error_reshow(void)
{
  switch (gui_disp_error_select)
  {
  case ERR_MIN_TEMP:
    err_info_is_show = ERR_INFO_NOT_SHOW;
    break;

  case ERR_MAX_TEMP:
  case ERR_MAX_TEMP_BED:
  case ERR_HEAT_FAIL:
  case ERR_X_MIN_LIMIT:
  case ERR_Y_MIN_LIMIT:
  case ERR_Z_MIN_LIMIT:
  case ERR_X_MAX_LIMIT:
  case ERR_Y_MAX_LIMIT:
  case ERR_Z_MAX_LIMIT:
    err_info_is_show = ERR_INFO_SHOW;
    break;

  case ERR_IWDG_RESET:
    err_info_is_show = ERR_INFO_NOT_SHOW;
    break;

  case ERR_FATFS:
  case ERR_THERMISTOR_FALLS:
    err_info_is_show = ERR_INFO_SHOW;
    break;

  default:
    break;
  }

  ccm_param.t_sys.is_gui_warning_show = false;
}

void gui_disp_error_info_show(int gui_err_index)
{
  if (gui_err_index == -1) return;

  gui_disp_error_select = gui_err_index;

  switch (gui_err_index)
  {
  case ERR_MIN_TEMP:
    break;

  case ERR_MAX_TEMP:
  case ERR_MAX_TEMP_BED:
    if (ERR_INFO_SHOW == err_info_is_show)
    {
      sg_grbl::temperature_disable_heater();
    }

    break;

  case ERR_HEAT_FAIL:
  case ERR_THERMISTOR_FALLS:
    if (ERR_INFO_SHOW == err_info_is_show)
    {
      if (print_status.is_printing) // 加热失败，处于打印状态时，停止打印
      {
        user_os_respond_gui_send_sem(StopPrintValue);
      }
      else
      {
        sg_grbl::temperature_disable_heater();
      }
    }

    break;

  case ERR_X_MIN_LIMIT:
  case ERR_Y_MIN_LIMIT:
  case ERR_Z_MIN_LIMIT:
  case ERR_X_MAX_LIMIT:
  case ERR_Y_MAX_LIMIT:
  case ERR_Z_MAX_LIMIT:
  case ERR_IWDG_RESET:
  case ERR_FATFS:
  default:
    break;
  }

  if (ERR_INFO_SHOW == err_info_is_show)
  {
    ccm_param.t_sys.is_gui_warning_show = true;
  }
}

void gui_disp_error_info(void)
{
  static bool SendCloseBeepOnece;
  static uint32_t beep_timeout;

  if (gui::is_refresh())
  {
    display_picture(24);
    gui_disp_set_text_buf_color(RED);
    gui_disp_error_text_show();
    user_os_respond_gui_send_sem(OpenBeep);
    beep_timeout = xTaskGetTickCount() + 5000;
    SendCloseBeepOnece = TRUE;

    if (ccm_param.t_sys.print_flag) //若显示警告信息的时候正在打印则停止它
    {
      user_os_respond_gui_send_sem(StopPrintValue);

      // 出错关闭激光模块
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
        sg_grbl::temperature_disable_heater();
    }
  }

  if (gui_touch_43_480272_xy(204, 180, 292, 224) ||
      gui_touch_35_480320_xy(196, 210, 300, 260) ||
      gui_touch_7_1024600_xy(436, 396, 620, 490))
  {
    ccm_param.t_sys.is_gui_warning_start = true;
    ccm_param.t_sys.print_pause_flag = false;
    ccm_param.t_sys.print_flag = false;
    user_os_respond_gui_send_sem(CloseBeep);
    gui_disp_error_reshow();
    gui::set_current_display(maindisplayF);
    return ;
  }

  if (SendCloseBeepOnece)
  {
    SendCloseBeepOnece = FALSE;

    if (beep_timeout < xTaskGetTickCount())
    {
      user_os_respond_gui_send_sem(CloseBeep);
    }
  }
}

void user_iwdg_show_err_info(void)
{
  gui_disp_error_info_show(ERR_IWDG_RESET);
}

void gui_disp_error_info_control(void)
{
  if (ccm_param.t_sys.is_gui_warning_show) //是否显示错误界面
  {
    if (ccm_param.t_sys.is_gui_warning_start)
    {
      ccm_param.t_sys.is_gui_warning_start = false;
      gui::set_current_display(gui_disp_error_info);
    }
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

