#include "user_common_cpp.h"
#ifdef ENABLE_FLASH

#ifdef __cplusplus
extern "C" {
#endif

void flash_interface_process(void)
{
  static uint32_t flash_save_timeout = 0;

  if (flash_save_timeout < xTaskGetTickCount())
  {
    flash_save_timeout = xTaskGetTickCount() + 1000;

    if (flash_param_t.flag == 1)
    {
      taskENTER_CRITICAL();
      flash_param_t.flag = 0;
      flash_config_save();
      taskEXIT_CRITICAL();
    }
    else if (flash_poweroff_data_is_reset)
    {
      taskENTER_CRITICAL();
      flash_poweroff_data_is_reset = false;
      flash_poweroff_data_erase();
      taskEXIT_CRITICAL();
    }
  }
}

void flash_interface_save_buzzer(void)
{
  taskENTER_CRITICAL();
  flash_param_t.key_sound = ccm_param.t_sys.sound_key;
  flash_param_t.alarm_sound = ccm_param.t_sys.sound_alarm;
  flash_param_t.is_buzzer_chg = 1;
  flash_param_t.flag = 1;
  taskEXIT_CRITICAL();
}

void flash_interface_save_cal_zmax_pos(float value)
{
  if (ccm_param.t_sys_data_current.enable_powerOff_recovery && ccm_param.t_sys.enable_z_max_limit)
  {
    ccm_param.t_model.xyz_max_pos[Z_AXIS] = value;
    ccm_param.t_model.xyz_move_max_pos[Z_AXIS] = value;
    ccm_param.t_sys_data_current.poweroff_rec_z_max_value = value;
    flash_param_t.poweroff_rec_z_max_value = value;
    flash_config_set_flag_save();
  }
}

#ifdef __cplusplus
}
#endif

#endif







