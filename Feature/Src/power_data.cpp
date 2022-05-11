#include "user_common_cpp.h"

#ifdef HAS_POWER_RECOVERY

#ifdef __cplusplus
extern "C" {
#endif

void feature_pow_data_init(void)
{
  if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
  {
    flash_poweroff_data_read();

    if (0U != ccm_param.flash_poweroff_recovery.is_serial) // 串口标志位开，断电标志关闭
    {
      ccm_param.flash_poweroff_recovery.flag = 0U;
    }

    memmove((planner_running_status_t *)&ccm_param.t_power_off,
            (planner_running_status_t *)&ccm_param.flash_poweroff_recovery, sizeof(planner_running_status_t));
    utils_str_memmove(ccm_param.power_path_file_name, flash_param_t.pathFileName);
    utils_str_memmove((TCHAR *)ccm_param.t_gui.print_name, flash_param_t.fileName);
    ccm_param.t_sys.enable_color_buf = ccm_param.flash_poweroff_recovery.enable_color_buf;
    ccm_param.t_sys.print_time_save = ccm_param.flash_poweroff_recovery.print_time_save;

    if ((uint8_t)(flash_param_t.pathFileName[0] - '0') == 0)
    {
      ccm_param.t_sys.is_power_off_file_from_user = false;
    }
    else
    {
      ccm_param.t_sys.is_power_off_file_from_user = true;
    }
  }
}

// 重置断电标志位
void feature_pow_data_reset_flag(void)
{
  if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
  {
    taskENTER_CRITICAL();
    memset((planner_running_status_t *)&ccm_param.t_power_off, 0, sizeof(ccm_param.t_power_off));
    taskEXIT_CRITICAL();
  }
}

//断电续打取消删除SD卡中的文件
void feature_pow_data_delete_file_from_sd(void)
{
  if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
  {
    if (ccm_param.t_sys.is_power_off_file_from_user)   //断电续打文件在SD卡中，删除文件
    {
      taskENTER_CRITICAL();
      (void)f_unlink(ccm_param.power_path_file_name);
      taskEXIT_CRITICAL();
    }
  }
}

//断电续打取消删除SD卡中的文件
void feature_pow_data_save_file_path_name(TCHAR *path_file_name, TCHAR *file_name)
{
  if ((0U != ccm_param.t_sys_data_current.enable_powerOff_recovery)) // 断电开启，非断电状态
  {
    /*确定选择的打印文件后，将文件名和路径名保存到flash，以便断电续打时调用*/
    utils_str_memmove(flash_param_t.pathFileName, path_file_name);
    utils_str_memmove(flash_param_t.fileName, file_name);
    feature_print_control::idex_flash_copy_sys_print_type();
    flash_param_t.mix_print_type = ccm_param.t_sys.print_type_mix;
    flash_param_t.flag = 1;
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_POWER_RECOVERY













