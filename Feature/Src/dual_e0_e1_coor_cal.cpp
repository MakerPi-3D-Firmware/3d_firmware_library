#include "user_common_cpp.h"
#ifndef ENABLE_GUI_LVGL
#ifdef __cplusplus
extern "C" {
#endif

#define E1_E2_COOR_CAL_FILE  ((char *)"E1_E2_coor_cal.gcode")

void dual_e0_e1_coor_cal_start(void)
{
  char dual_file_name[20];
  FIL dual_file;
  memset(dual_file_name, 0, sizeof(dual_file_name));
  (void)strcpy(dual_file_name, USBHPath);
  (void)strcat(dual_file_name, E1_E2_COOR_CAL_FILE);
  // Default USB flash disk printing double head coordinate calibration gcode
  // USB flash drive file does not exist, print from NAND flash file
  FRESULT res = f_open(&dual_file, dual_file_name, FA_READ);

  if (res == FR_OK)
  {
    feature_print_control::file_reset_medium();
    f_close(&dual_file);
  }
  else
  {
    memset(dual_file_name, 0, sizeof(dual_file_name));
    (void)strcpy(dual_file_name, USERPath);
    (void)strcat(dual_file_name, E1_E2_COOR_CAL_FILE);
    feature_print_control::file_form_medium_user();
  }

  strcpy((char *)ccm_param.t_gui.print_name, E1_E2_COOR_CAL_FILE);

  if (feature_print_control::idex_sys_is_extruder())
  {
    feature_print_control::idex_sys_set_normal();
  }

  gcode::active_extruder = 0;
  ccm_param.t_sys.print_flag = true;
  user_os_respond_gui_send_sem(FilePrintValue);
  ccm_param.t_sys.is_print_e1_offset_flag = true;
  gui::set_current_display(maindisplayF);
}

bool dual_e0_e1_coor_cal_file_is_exit(char *file_name)
{
  bool result = false;

  if (0 == strcmp(file_name, "1:/E1_E2_coor_cal.gcode"))
  {
    result = true;
  }

  return result;
}

#ifdef __cplusplus
} //extern "C" {
#endif
#endif
