#ifndef GUI_DISPLAY_FILE_NAME_H
#define GUI_DISPLAY_FILE_NAME_H

#ifdef __cplusplus
extern "C" {
#endif

void gui_disp_file_name_confirm(void);
void gui_disp_file_name_confirm_idex(void);
void gui_disp_file_name_pause(void);
void gui_disp_file_name_resume(void);
void gui_disp_file_name_stop(void);
void gui_disp_file_name_power_rec(void);
void gui_disp_file_name_chg_filament(void);
void gui_disp_file_name_finish(void);
void gui_disp_file_name_list(const char *print_name, int index);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // GUI_DISPLAY_FILE_NAME_H

