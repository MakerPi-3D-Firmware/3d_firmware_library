#include "user_common_cpp.h"
#ifndef ENABLE_GUI_LVGL
#include "../../Feature/Inc/Alter.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MECHANICAL_ADJUST_STATUS_NONE 0
#define MECHANICAL_ADJUST_STATUS_HOME 1
#define MECHANICAL_ADJUST_STATUS_Z_DOWN 2
#define MECHANICAL_ADJUST_STATUS_SWITCH_MODULE 3
#define MECHANICAL_ADJUST_STATUS_SWITCH_MODULE_DONE 4
#define MECHANICAL_ADJUST_STATUS_LEVEL 5
#define MECHANICAL_ADJUST_STATUS_LEVEL_DONE 6

typedef struct
{
  uint8_t is_adjust_start;                          /*!< 启动红外调平校准 */
  uint8_t adjust_status;                            /*!< 红外调平校准状态 */
} bed_level_mechanical_t;

static volatile bed_level_mechanical_t bed_level_mechanical;

void bed_level_mechanical_unload_filament(void)
{
  gui::set_current_display(unloadfilament0F);
  user_os_respond_gui_send_sem(BackFilamentValue);
}

void bed_level_mechanical_before_warm(void)
{
  static float tmp_poweroff_rec_z_max_value = 0.0f;

  if (gui::is_refresh())
  {
    display_picture(116);
    tmp_poweroff_rec_z_max_value = ccm_param.t_sys_data_current.poweroff_rec_z_max_value;
    ccm_param.t_sys_data_current.poweroff_rec_z_max_value = ccm_param.t_model.z_max_pos_origin - 20.0f; // 调平归零设置最大高度为默认机型高度减去20mm，避免刮喷嘴
  }

  if (gui_touch_xy(40, 180, 200, 290))
  {
    // K5黑屏固件，初始化abl模块
    if (K5 == ccm_param.t_sys_data_current.model_id)
    {
      if (1 == ccm_param.t_sys_data_current.IsMechanismLevel)
      {
        ccm_param.t_sys.is_abl_init = true;
        user_abl_init();
      }
    }

    bed_level_mechanical.is_adjust_start = 1;
    Zaxis_RunOnce = false;
  }
  else if (gui_touch_xy(270, 180, 430, 290))
  {
    Gui_Mode = MODE_FILUM_LEVEL;
    waiting_for_homing(bed_level_mechanical_unload_filament);
    return ;
  }
  else if (gui_touch_xy(30, 0, 180, 80))
  {
    ccm_param.t_sys_data_current.poweroff_rec_z_max_value = tmp_poweroff_rec_z_max_value;
    ccm_param.t_sys.is_gcode_g28_done = false;
    gui::set_current_display(settingF);
  }
}

// 开始校准Z平台
void bed_level_mechanical_adjust_start(void)
{
  err_info_is_show = ERR_INFO_NOT_SHOW;
  gui::set_current_display(bed_level_mechanical_before_warm);
}

void bed_level_mechanical_init(void)
{
  bed_level_mechanical.is_adjust_start = 0;
  bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_NONE;
}

void bed_level_mechanical_z_move(void)
{
  if (gui::is_refresh())
  {
    display_picture(164);
  }
}

void bed_level_mechanical_switch_module(void)
{
  if (gui::is_refresh())
  {
    display_picture(117);
  }

  if (gui_touch_xy(150, 160, 340, 320))
  {
    gui::set_current_display(bed_level_mechanical_z_move);
    bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_SWITCH_MODULE_DONE;
    gcode_cmd_z_down('-', 50.0f);
  }
}

void bed_level_mechanical_bed_level_done(void)
{
  bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_LEVEL_DONE;
}

// 调平完成
void bed_level_mechanical_adjust_finish(void)
{
  static unsigned long BeepWaringTime = 0;

  if (gui::is_refresh())
  {
    display_picture(118);
    user_os_respond_gui_send_sem(OpenBeep);
    BeepWaringTime = xTaskGetTickCount() + 5000; //鸣叫5秒
  }

  if (BeepWaringTime < xTaskGetTickCount()) //时间到关闭鸣叫
  {
    user_os_respond_gui_send_sem(CloseBeep);
  }
}

void bed_level_mechanical_adjust_process(void)
{
  if (1 == bed_level_mechanical.is_adjust_start)
  {
    if (MECHANICAL_ADJUST_STATUS_NONE == bed_level_mechanical.adjust_status) // home
    {
      bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_HOME;
      ccm_param.t_sys.is_gcode_g28_done = false;
      waiting_for_homing(bed_level_mechanical_z_move);
    }
    else if (MECHANICAL_ADJUST_STATUS_HOME == bed_level_mechanical.adjust_status &&
             ccm_param.t_sys.is_gcode_g28_done) // Z down
    {
      bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_Z_DOWN;
      sg_grbl::temperature_disable_heater();
      user_fan_disable_all();
      gcode_cmd_z_down('+', 50.0f);
    }
    else if (MECHANICAL_ADJUST_STATUS_Z_DOWN == bed_level_mechanical.adjust_status &&
             0 == sg_grbl::planner_moves_planned()) // swich level module
    {
      bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_SWITCH_MODULE;
      gui::set_current_display(bed_level_mechanical_switch_module);
    }
    else if (MECHANICAL_ADJUST_STATUS_SWITCH_MODULE == bed_level_mechanical.adjust_status)
    {
    }
    else if (MECHANICAL_ADJUST_STATUS_SWITCH_MODULE_DONE == bed_level_mechanical.adjust_status &&
             0 == sg_grbl::planner_moves_planned()) // leveling
    {
      bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_LEVEL;
      waiting_for_bed_leveling(bed_level_mechanical_bed_level_done);
    }
    else if (MECHANICAL_ADJUST_STATUS_LEVEL == bed_level_mechanical.adjust_status)
    {
    }
    else if (MECHANICAL_ADJUST_STATUS_LEVEL_DONE == bed_level_mechanical.adjust_status) // leveling done
    {
      bed_level_mechanical.is_adjust_start = 0;
      bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_NONE;
      gcode_cmd_z_down('+', 50.0f);
      gui::set_current_display(bed_level_mechanical_adjust_finish);
    }
  }
  else
  {
    bed_level_mechanical.adjust_status = MECHANICAL_ADJUST_STATUS_NONE;
  }
}


#ifdef __cplusplus
} //extern "C" {
#endif
#endif
