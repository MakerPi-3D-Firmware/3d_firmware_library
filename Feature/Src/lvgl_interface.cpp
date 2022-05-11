#include "user_common_cpp.h"
#ifdef ENABLE_GUI_LVGL

#ifdef __cplusplus
extern "C" {
#endif

static uint32_t open_beep_timeout = 0;
static bool is_open_beep = false;

void lvgl_open_beep(uint32_t timeout)
{
  user_os_respond_gui_send_sem(OpenBeep);
  open_beep_timeout = xTaskGetTickCount() + timeout;
  is_open_beep = true;
}

void lvgl_close_beep(bool is_now)
{
  if ((is_open_beep && open_beep_timeout < xTaskGetTickCount()) || is_now)
  {
    user_os_respond_gui_send_sem(CloseBeep);
    is_open_beep = false;
  }
}

float lvgl_get_bed_curr_temp(void)
{
  return sg_grbl::temperature_get_bed_current();
}

float lvgl_get_bed_target_temp(void)
{
  return sg_grbl::temperature_get_bed_target();
}

float lvgl_get_ext_curr_temp(int index)
{
  return sg_grbl::temperature_get_extruder_current(index);
}

float lvgl_get_ext_target_temp(int index)
{
  return sg_grbl::temperature_get_extruder_target(index);
}

void lvgl_set_ext_target_temp(float value, int index)
{
  sg_grbl::temperature_set_extruder_target(value, index);
}

void lvgl_set_bed_target_temp(float value)
{
  sg_grbl::temperature_set_bed_target(value);
}

int32_t lvgl_get_print_filament_length(void)
{
  return ccm_param.t_gui.used_total_material + ccm_param.grbl_destination[E_AXIS] + ccm_param.grbl_destination[B_AXIS];
}

float lvgl_get_print_platform_height(void)
{
  return sg_grbl::st_get_position_length(Z_AXIS);
}

uint32_t lvgl_get_print_time_sec(void)
{
  return ccm_param.t_gui.printed_time_sec;
}

int lvgl_get_print_percent(void)
{
  return ccm_param.t_gui.print_percent;
}

void lvgl_set_print_speed(int value)
{
  SetFeedMultiply(value);
}

int lvgl_get_print_speed(void)
{
  return GetFeedMultiply();
}

void lvgl_disable_heater(void)
{
  sg_grbl::temperature_disable_heater();
}

bool lvgl_is_pause_to_cool_down(void)
{
  return print_status.is_pause_to_cool_down;
}

bool lvgl_is_finish_print(void)
{
  return print_status.is_finish_print;
}

bool lvgl_is_print_sd_file(void)
{
  return print_status.is_print_medium_user_file;
}

void lvgl_change_filament(void)
{
  if (print_status.is_pause_to_cool_down) //暂停打印的时候降低了温度
  {
    user_os_respond_gui_send_sem(PauseToResumeNozzleTemp);
  }
  else
  {
    user_os_respond_gui_send_sem(ConfirmChangeFilamentValue);
  }
}

void lvgl_ref_data(void)
{
  custom_preload_listen();
}

void lvgl_set_fan(bool is_on)
{
  taskENTER_CRITICAL();
  feature_control_set_fan_speed(is_on ? 255 : 0);
  taskEXIT_CRITICAL();
}

int lvgl_get_fan(void)
{
  return feature_control_get_fan_speed();
}

uint8_t lvgl_get_m600_status(void)
{
  return gcode::m600_status;
}

#ifdef __cplusplus
}
#endif

#endif // ENABLE_GUI_LVGL







