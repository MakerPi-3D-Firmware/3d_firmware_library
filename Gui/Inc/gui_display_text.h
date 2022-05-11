#ifndef GUI_DISPLAY_TEXT_H
#define GUI_DISPLAY_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#define TEXT_BUF_4_INDEX_PRINT_PROGRESS 0

#define TEXT_BUF_4_INDEX_LASER 0

#define TEXT_BUF_4_INDEX_X 0
#define TEXT_BUF_4_INDEX_Y 1
#define TEXT_BUF_4_INDEX_Z 2

#define TEXT_BUF_9_INDEX_PRINT_TIME 0
#define TEXT_BUF_9_INDEX_PRINT_SPEED 1
#define TEXT_BUF_9_INDEX_PRINT_NOZ_TEMP 2
#define TEXT_BUF_9_INDEX_PRINT_NOZ1_TEMP 3
#define TEXT_BUF_9_INDEX_PRINT_BED_TEMP 4
#define TEXT_BUF_9_INDEX_PRINT_CAVITY_TEMP 5

#define TEXT_BUF_9_INDEX_X 0
#define TEXT_BUF_9_INDEX_Y 1
#define TEXT_BUF_9_INDEX_Z 2

#define TEXT_BUF_9_INDEX_BED_LEFT 0
#define TEXT_BUF_9_INDEX_BED_RIGHT 1

#define TEXT_BUF_4_INDEX_PRINT_NOZ_TEMP 0
#define TEXT_BUF_4_INDEX_BED_TEMP 1
#define TEXT_BUF_4_INDEX_FAN_SPEED 2
#define TEXT_BUF_4_INDEX_PRINT_SPEED 3

#define TEXT_BUF_4_INDEX_CAVITY_TEMP 0
#define TEXT_BUF_4_INDEX_CAVITY_ON 1

#define TEXT_BUF_4_INDEX_DUAL_NOZ_TEMP 0
#define TEXT_BUF_4_INDEX_DUAL_NOZ1_TEMP 1
#define TEXT_BUF_4_INDEX_DUAL_BED_TEMP 2

void gui_disp_text_noz0_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_noz0_temp_show(void);
void gui_disp_text_noz1_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_noz1_temp_show(void);
void gui_disp_text_bed_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_bed_temp_show(void);
void gui_disp_text_time_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_time_show(int time_sec);
void gui_disp_text_print_progress_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_print_progress_show(void);
void gui_disp_text_cavity_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_cavity_temp_show(void);
void gui_disp_text_buf_show_m(char *text_buf, int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s, bool is_middle);
void gui_disp_text_buf_show(char *text_buf, int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_set_text_buf_color(uint16_t chg_text_color);
void gui_disp_text_noz0_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_speed_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);
void gui_disp_text_z_height_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s);

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gui
{
  extern void set_text_range_4_buf(int index, int x, int y);
  extern void display_text_range_4_buf(int index, char *fmt, ...);
  extern void set_text_range_9_buf(int index, int x, int y);
  extern void display_text_range_9_buf(int index, char *fmt, ...);
  extern void set_disp_text_color(uint16_t chg_text_color);
  extern void display_text_buf(int x, int y, char *text);
  extern void disp_text(const unsigned char *pstr, int x, int y, int size, uint16_t colour);
}

#endif // GUI_DISPLAY_TEXT_H

