#include "user_common.h"

#ifdef ENABLE_CCM

//IRAM2共64K，起始地址0x10000000  IRAM1共128K，起始地址0x20000000
ccm_param_t ccm_param  __attribute__((at(0X10000000)));
void user_ccm_param_init(void)
{
  memset((T_SYS_DATA *)&ccm_param.t_sys_data_current, 0, sizeof(T_SYS_DATA));
  memset((T_SYS *)&ccm_param.t_sys, 0, sizeof(T_SYS));
  memset((T_MODEL *)&ccm_param.t_model, 0, sizeof(T_MODEL));
  memset((block_t *)&ccm_param.block_buffer[0], 0, sizeof(block_t) * BLOCK_BUFFER_SIZE);
  ccm_param.block_buffer_head = 0U;
  ccm_param.block_buffer_tail = 0U;
  memset((planner_running_status_t *)&ccm_param.t_power_off, 0, sizeof(planner_running_status_t));
  memset((planner_running_status_t *)&ccm_param.flash_poweroff_recovery, 0, sizeof(planner_running_status_t));
  memset((planner_running_status_t *)&ccm_param.runningStatus[0], 0, sizeof(planner_running_status_t) * BLOCK_BUFFER_SIZE);
  memset((planner_running_status_t *)&ccm_param.running_status, 0, sizeof(planner_running_status_t));
  memset((T_GUI *)&ccm_param.t_gui, 0, sizeof(T_GUI));
  memset((T_GUI_SEMP *)&ccm_param.t_gui_semp, 0, sizeof(T_GUI_SEMP));
  memset((char *)ccm_param.command_buffer, 0, sizeof(char)*MAX_CMD_SIZE);
  memset(ccm_param.file_read_buf, 0, sizeof(ccm_param.file_read_buf));
  ccm_param.file_read_buf_index = FILE_READ_SIZE;
  memset(ccm_param.os_gcode_buf, 0, sizeof(ccm_param.os_gcode_buf));
  ccm_param.t_sys.lcd_type = LCD_TYPE_35_480320_SIZE;
  ccm_param.t_sys.print_type_idex = IDEX_PRINT_TYPE_NORMAL;
  ccm_param.t_sys.print_type_mix = MIX_PRINT_TYPE_GRADIENT_COLOR;
  ccm_param.t_sys.is_gui_warning_start = true;
  ccm_param.t_sys.medium_page_file_num = MEDIUM_FILE_SIZE;
  ccm_param.t_gui_semp.index_v = -1;
}

#endif



























