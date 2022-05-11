#include "user_common_cpp.h"

#ifndef ENABLE_GUI_LVGL
#include "../../Feature/Inc/Alter.h"
#ifdef __cplusplus
extern "C" {
#endif

//K5屏幕休眠，花屏处理和放宽报警条件
char TempMinErr_cnt;

char Check_TempMinErr(void)
{
  static unsigned long TempMinErr_TimeOut = 0;

  if (ccm_param.t_sys_data_current.model_id != K5)
  {
    return 1;
  }

  ++TempMinErr_TimeOut;

  if (TempMinErr_TimeOut >= 10)
  {
    ++TempMinErr_cnt;
    TempMinErr_TimeOut = 0;
  }

  if (TempMinErr_cnt > 2)
  {
    TempMinErr_cnt = 0;
    return 1;
  }
  else
  {
    return 0;
  }
}

bool Zaxis_RunOnce;
MODEValueTypeDef Gui_Mode = MODE_MIAN;
float Laser_Print_Position = 150.0f;
#if LASER_MODE
//新增激光头功能
extern void SaveLaser(void);
volatile menufunc_t predisplay;
//归零页面
void page_homing(void)
{
  if (gui::is_refresh())
  {
    display_picture(83);
  }

  if (ccm_param.t_sys.is_gcode_g28_done)
  {
    gui::set_current_display(predisplay);
  }
}

//跳转归零页面
void goto_page_homing(void)
{
  if (PICTURE_IS_JAPANESE != ccm_param.t_sys_data_current.pic_id) //日文没归零页面，不做下面操作
  {
    if (!ccm_param.t_sys.is_gcode_g28_done)
    {
      predisplay = gui::current_display;
      gui::set_current_display(page_homing);
    }
  }
}



void Laser_change_warn(void)
{
  if (gui::is_refresh())
  {
    Gui_Mode = MODE_SET;

    if (!ccm_param.t_sys_data_current.IsLaser)
      display_picture(126);
    else
      display_picture(127);
  }

  if (gui_touch_xy(80, 160, 240, 290)) //耗材已拔出
  {
    if (ccm_param.t_sys_data_current.IsLaser)
    {
      //        change_mark = 1;
      ccm_param.t_sys_data_current.IsLaser = 0;
      gcode_cmd_xy_move_to_home();
      ccm_param.t_sys.is_gcode_g28_done = true;
    }
    else
    {
      ccm_param.t_sys_data_current.IsLaser = 1;
      err_info_is_show = ERR_INFO_NOT_SHOW;
      ccm_param.t_sys.is_gcode_g28_done = false;
      gcode_cmd_g28();
    }

    if (2 == ccm_param.t_sys_data_current.enable_v5_extruder)
      feature_control_set_fan_speed(0);

    user_fan_control_eb_motor(false);
    gui::set_current_display(Laser_change_sure);
  }

  if (gui_touch_xy(260, 160, 410, 290)) //耗材未拔出
  {
    if (!ccm_param.t_sys_data_current.IsLaser)
    {
      Gui_Mode = MODE_FILUM;
      gui::set_current_display(unloadfilament0F);
      goto_page_homing();
      user_os_respond_gui_send_sem(BackFilamentValue);
    }
    else
    {
      gui::set_current_display(settingF);
      sg_grbl::temperature_disable_heater();
    }
  }

  if (gui_touch_xy(30, 0, 180, 80)) //耗材未拔出
  {
    gui::set_current_display(settingF);

    if (!ccm_param.t_sys_data_current.IsLaser)
    {
      err_info_is_show = ERR_INFO_SHOW;
    }
    else
    {
      err_info_is_show = ERR_INFO_NOT_SHOW;
    }
  }
}

void Laser_change_sure(void)
{
  if (ccm_param.t_sys.is_gcode_g28_done && sg_grbl::planner_moves_planned() == 0)
  {
    ccm_param.grbl_current_position[X_AXIS] = 100.0f;
    ccm_param.grbl_current_position[Y_AXIS] = 100.0f;
    process_buffer_line_normal(ccm_param.grbl_current_position, homing_feedrate[X_AXIS] / 60);

    if (gui::is_refresh())
    {
      if (ccm_param.t_sys_data_current.IsLaser)
        display_picture(124);
      else
        display_picture(123);
    }

    if (gui_touch_xy(80, 170, 240, 300)) //确认
    {
      gcode_cmd_xy_move_to_home();
      gui::set_current_display(settingF);

      if (!ccm_param.t_sys_data_current.IsLaser)
      {
        err_info_is_show = ERR_INFO_SHOW;
      }
      else
      {
        ccm_param.t_gui.move_xyz_pos[Z_AXIS] = 150;
      }

      SaveLaser();
    }

    if (gui_touch_xy(260, 170, 400, 300)) //取消
    {
      //      change_mark = 0;
      gui::set_current_display(Laser_change_warn);

      if (ccm_param.t_sys_data_current.IsLaser)
      {
        ccm_param.t_sys_data_current.IsLaser = 0;
      }
      else
      {
        ccm_param.t_sys_data_current.IsLaser = 1;
      }
    }
  }
}

void Laser_nonsupport(void)
{
  if (gui::is_refresh())
  {
    display_picture(125);
  }

  if (gui_touch_xy(180, 170, 310, 270))
  {
    switch (Gui_Mode)
    {
    case MODE_SET:
      gui::set_current_display(settingF);
      break;

    case MODE_PREPARE:
      gui::set_current_display(prepareF);
      break;

    default:
      break;
    }
  }
}
#endif




#ifdef __cplusplus
} // extern "C" {
#endif
#endif



