#include "user_common_cpp.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DISPLAY_TEXT_LENGTH 20

static char gui_disp_file_name[_MAX_LFN];
static char gui_file_path_name[_MAX_LFN];
static char gui_file_name[_MAX_LFN];

static void gui_file_reset_print_name(const char *print_name)
{
  memset(gui_file_name, 0, _MAX_LFN);
  strcpy(gui_file_name, print_name);

  if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    if (strlen(gui_file_name) > MAX_DISPLAY_TEXT_LENGTH)
    {
      gui_file_name[MAX_DISPLAY_TEXT_LENGTH] = 0;
      gui_file_name[MAX_DISPLAY_TEXT_LENGTH - 1] = '.';
      gui_file_name[MAX_DISPLAY_TEXT_LENGTH - 2] = '.';
      gui_file_name[MAX_DISPLAY_TEXT_LENGTH - 3] = '.';
    }
  }
}

static void gui_disp_file_name_common(void)
{
  gui_file_reset_print_name((char *)ccm_param.t_gui.print_name);
  (void)snprintf(gui_disp_file_name, _MAX_LFN, "%s", gui_file_name);
}

static void gui_get_print_path_name(void)
{
  memset(gui_file_path_name, 0, _MAX_LFN);

  if (ccm_param.t_sys.is_medium_root_dir)
    (void)snprintf(gui_file_path_name, _MAX_LFN, "%s", ccm_param.t_sys.medium_current_path);
  else
    (void)snprintf(gui_file_path_name, _MAX_LFN, "%s/", ccm_param.t_sys.medium_current_path);

  (void)strcat(gui_file_path_name, (char *)ccm_param.t_gui.print_name);
  (void)snprintf(gui_disp_file_name, _MAX_LFN, "%s", gui_file_path_name);
  /*确定选择的打印文件后，将文件名和路径名保存到flash，以便断电续打时调用*/
  feature_pow_data_save_file_path_name((char *)gui_disp_file_name, (char *)ccm_param.t_gui.print_name);
}

void gui_disp_file_name_confirm(void)
{
  gui_get_print_path_name();
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 85, 0, 85, 0, 205, true);
}

void gui_disp_file_name_confirm_idex(void)
{
  gui_get_print_path_name();
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 48, 0, 48, 0, 98, true);
}

void gui_disp_file_name_pause(void)
{
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 85, 0, 85, 0, 205, true);
}

void gui_disp_file_name_resume(void)
{
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 85, 0, 85, 0, 205, true);
}

void gui_disp_file_name_stop(void)
{
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 85, 0, 85, 0, 205, true);
}

void gui_disp_file_name_power_rec(void)
{
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 85, 0, 85, 0, 205, true);
}

void gui_disp_file_name_chg_filament(void)
{
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 85, 0, 85, 0, 205, true);
}

void gui_disp_file_name_finish(void)
{
  gui_disp_file_name_common();
  gui_disp_text_buf_show_m((char *)gui_disp_file_name, 0, 78, 0, 78, 0, 205, true);
}

void gui_disp_file_name_list(const char *print_name, int index)
{
  gui_file_reset_print_name((char *)print_name);
  (void)snprintf(gui_disp_file_name, _MAX_LFN, "%s", gui_file_name);

  if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
    gui::disp_text((uint8_t *)gui_disp_file_name, 180 + 24, 184 + index * (24 + 18), 24, (u16)testcolor);
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
    gui::disp_text((uint8_t *)gui_disp_file_name, 84 + 24, 84 + index * (24 + 8), 24, (u16)testcolor);
  else
    gui::disp_text((uint8_t *)gui_disp_file_name, 55 + 24, 97 + index * (24 + 16), 24, (u16)testcolor);
}

#ifdef __cplusplus
} //extern "C" {
#endif

