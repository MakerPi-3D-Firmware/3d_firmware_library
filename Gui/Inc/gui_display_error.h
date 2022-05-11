#ifndef GUI_DISPLAY_ERROR_H
#define GUI_DISPLAY_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

enum GUI
{
  ERR_MIN_TEMP = 0,
  ERR_MAX_TEMP = 1,
  ERR_MAX_TEMP_BED = 2,
  ERR_HEAT_FAIL = 3,
  ERR_X_MIN_LIMIT = 4,
  ERR_Y_MIN_LIMIT = 5,
  ERR_Z_MIN_LIMIT = 6,
  ERR_X_MAX_LIMIT = 7,
  ERR_Y_MAX_LIMIT = 8,
  ERR_Z_MAX_LIMIT = 9,
  ERR_IWDG_RESET = 10,
  ERR_FATFS = 11,
  ERR_THERMISTOR_FALLS = 12
};

#define ERR_INFO_NOT_SHOW 0
#define ERR_INFO_SHOW 1

extern uint8_t err_info_is_show;
void gui_disp_error_info_control(void);
void gui_disp_error_info_show(int gui_err_index);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // GUI_DISPLAY_ERROR_H

