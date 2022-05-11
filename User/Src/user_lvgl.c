#include "user_common.h"

#ifdef ENABLE_GUI_LVGL

#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

//nand 4G 0X100000000
//sdram 256M 0X10000000

// 图片包文件头结构
typedef struct Pic_SMaseFileHeader
{
  uint32_t  uFileFlag;          // 包文件头标记: 'MASE'
  uint32_t  uFileCount;         // 包内文件个数
  uint32_t  uFileListOfs;       // 文件列表偏移
  uint32_t  uMaxFileCount;      // 最大子文件个数
  uint32_t  uFileSize;          // 包文件的大小
} Pic_MaseHeader;

//包内文件信息结构
typedef struct Pic_SFilesMessage
{
  uint32_t  uFileOfs;          // 本文件在包内的偏移
  uint32_t  uFileSize;         // 本文件的大小
  char  szFileName[60];   // 本文件的文件名，包含路径
} Pic_FilesMsg;

enum LVGL_IMG_INDEX
{
  control_add_alpha_70x40 = 1,
  control_subtract_alpha_70x40 = 2,
  home_bed_alpha_135x135 = 3,
  home_control_alpha_250x250 = 4,
  home_ext_alpha_135x135 = 5,
  home_print_alpha_250x250 = 6,
  home_speed_alpha_135x135 = 7,
  print_bed_schedule_alpha_150x150 = 8,
  print_division_schedule_alpha_4x45 = 9,
  print_ext_schedule_alpha_150x150 = 10,
  print_filament_schedule_alpha_45x45 = 11,
  print_image_schedule_alpha_400x460 = 12,
  print_progress_schedule_alpha_45x45 = 13,
  print_speep_schedule_alpha_150x150 = 14,
  print_time_schedule_alpha_45x45 = 15,
  print_z_schedule_alpha_45x45 = 16,
  topbar_back_alpha_40x70 = 17,
  topbar_setup_alpha_70x70 = 18,
  PrintOnlineTrans_animimgprint_online_t_1_p = 19,
  PrintOnlineTrans_animimgprint_online_t_2_p = 20,
  PrintOnlineTrans_animimgprint_online_t_3_p = 21,
  PrintOnlineTrans_animimgprint_online_t_4_p = 22,
  PrintOnlineTrans_animimgprint_online_t_5_p = 23,
  PrintOnlineTrans_animimgprint_online_t_6_p = 24,
  glyph_dsc_xbf_p = 25
};

lv_ui guider_ui;
Pic_FilesMsg   lvgl_Pic_fileMsg;

#define lvgl_img_map sdram_param.lvgl_img_map
#define USER_LVGL_GET_IMG_BUF(IMG_INDEX) \
  { \
    f_lseek(&NandFile, (IMG_INDEX - 1)*sizeof(Pic_FilesMsg) + sizeof(Pic_MaseHeader)); \
    f_read(&NandFile, &lvgl_Pic_fileMsg, sizeof(Pic_FilesMsg), NULL); \
    if (lvgl_Pic_fileMsg.uFileOfs) \
    { \
      f_lseek(&NandFile, lvgl_Pic_fileMsg.uFileOfs); \
    } \
    file_size = lvgl_Pic_fileMsg.uFileSize; \
    read_count = 0; \
    while(read_count < file_size) { \
      if(read_count + 512 < file_size) \
      { \
        f_read(&NandFile, &lvgl_img_map._##IMG_INDEX##_map[read_count], 512, &read_count_tmp); \
      } else \
      { \
        f_read(&NandFile, &lvgl_img_map._##IMG_INDEX##_map[read_count], file_size - read_count , &read_count_tmp); \
      } \
      if(read_count + read_count_tmp < file_size && read_count_tmp != 512) \
      { \
        (void)f_lseek(&NandFile, lvgl_Pic_fileMsg.uFileOfs + read_count); \
        continue; \
      } \
      read_count += read_count_tmp; \
    } \
  }

lv_img_dsc_t _control_add_alpha_70x40;
lv_img_dsc_t _control_subtract_alpha_70x40;
lv_img_dsc_t _home_bed_alpha_135x135;
lv_img_dsc_t _home_control_alpha_250x250;
lv_img_dsc_t _home_ext_alpha_135x135;
lv_img_dsc_t _home_print_alpha_250x250;
lv_img_dsc_t _home_speed_alpha_135x135;
lv_img_dsc_t _print_bed_schedule_alpha_150x150;
lv_img_dsc_t _print_division_schedule_alpha_4x45;
lv_img_dsc_t _print_ext_schedule_alpha_150x150;
lv_img_dsc_t _print_filament_schedule_alpha_45x45;
lv_img_dsc_t _print_image_schedule_alpha_400x460;
lv_img_dsc_t _print_progress_schedule_alpha_45x45;
lv_img_dsc_t _print_speep_schedule_alpha_150x150;
lv_img_dsc_t _print_time_schedule_alpha_45x45;
lv_img_dsc_t _print_z_schedule_alpha_45x45;
lv_img_dsc_t _topbar_back_alpha_40x70;
lv_img_dsc_t _topbar_setup_alpha_70x70;
lv_img_dsc_t PrintOnlineTrans_animimgprint_online_t_1;
lv_img_dsc_t PrintOnlineTrans_animimgprint_online_t_2;
lv_img_dsc_t PrintOnlineTrans_animimgprint_online_t_3;
lv_img_dsc_t PrintOnlineTrans_animimgprint_online_t_4;
lv_img_dsc_t PrintOnlineTrans_animimgprint_online_t_5;
lv_img_dsc_t PrintOnlineTrans_animimgprint_online_t_6;

static void _user_lvgl_load_image(void)
{
  FIL NandFile;
  uint32_t file_size;
  uint32_t read_count;
  uint32_t read_count_tmp;
  taskENTER_CRITICAL();
  memset((void *)&lvgl_img_map, 0xff, sizeof(lvgl_img_map));

  if (f_open(&NandFile, _T("1:/lvgl_file.bin"), FA_OPEN_EXISTING | FA_READ) == FR_OK)
  {
    USER_LVGL_GET_IMG_BUF(control_add_alpha_70x40);
    USER_LVGL_GET_IMG_BUF(control_subtract_alpha_70x40);
    USER_LVGL_GET_IMG_BUF(home_bed_alpha_135x135);
    USER_LVGL_GET_IMG_BUF(home_control_alpha_250x250);
    USER_LVGL_GET_IMG_BUF(home_ext_alpha_135x135);
    USER_LVGL_GET_IMG_BUF(home_print_alpha_250x250);
    USER_LVGL_GET_IMG_BUF(home_speed_alpha_135x135);
    USER_LVGL_GET_IMG_BUF(print_bed_schedule_alpha_150x150);
    USER_LVGL_GET_IMG_BUF(print_division_schedule_alpha_4x45);
    USER_LVGL_GET_IMG_BUF(print_ext_schedule_alpha_150x150);
    USER_LVGL_GET_IMG_BUF(print_filament_schedule_alpha_45x45);
    USER_LVGL_GET_IMG_BUF(print_image_schedule_alpha_400x460);
    USER_LVGL_GET_IMG_BUF(print_progress_schedule_alpha_45x45);
    USER_LVGL_GET_IMG_BUF(print_speep_schedule_alpha_150x150);
    USER_LVGL_GET_IMG_BUF(print_time_schedule_alpha_45x45);
    USER_LVGL_GET_IMG_BUF(print_z_schedule_alpha_45x45);
    USER_LVGL_GET_IMG_BUF(topbar_back_alpha_40x70);
    USER_LVGL_GET_IMG_BUF(topbar_setup_alpha_70x70);
    USER_LVGL_GET_IMG_BUF(PrintOnlineTrans_animimgprint_online_t_1_p);
    USER_LVGL_GET_IMG_BUF(PrintOnlineTrans_animimgprint_online_t_2_p);
    USER_LVGL_GET_IMG_BUF(PrintOnlineTrans_animimgprint_online_t_3_p);
    USER_LVGL_GET_IMG_BUF(PrintOnlineTrans_animimgprint_online_t_4_p);
    USER_LVGL_GET_IMG_BUF(PrintOnlineTrans_animimgprint_online_t_5_p);
    USER_LVGL_GET_IMG_BUF(PrintOnlineTrans_animimgprint_online_t_6_p);
    USER_LVGL_GET_IMG_BUF(glyph_dsc_xbf_p);
    f_close(&NandFile);
  }

  taskEXIT_CRITICAL();
}

static void _user_lvgl_set_img_dsc(lv_img_dsc_t *img_dsc, uint32_t w, uint32_t h, uint8_t *map)
{
  img_dsc->header.always_zero = 0;
  img_dsc->header.w = w;
  img_dsc->header.h = h;
  img_dsc->data_size = w * h * LV_COLOR_SIZE / 8;
  img_dsc->header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
  img_dsc->data = map;
}

static void _user_lvgl_init_img_dsc(void)
{
  _user_lvgl_set_img_dsc(&_control_add_alpha_70x40, 70, 40, lvgl_img_map._control_add_alpha_70x40_map);
  _user_lvgl_set_img_dsc(&_control_subtract_alpha_70x40, 70, 40, lvgl_img_map._control_subtract_alpha_70x40_map);
  _user_lvgl_set_img_dsc(&_home_bed_alpha_135x135, 135, 135, lvgl_img_map._home_bed_alpha_135x135_map);
  _user_lvgl_set_img_dsc(&_home_control_alpha_250x250, 250, 250, lvgl_img_map._home_control_alpha_250x250_map);
  _user_lvgl_set_img_dsc(&_home_ext_alpha_135x135, 135, 135, lvgl_img_map._home_ext_alpha_135x135_map);
  _user_lvgl_set_img_dsc(&_home_print_alpha_250x250, 250, 250, lvgl_img_map._home_print_alpha_250x250_map);
  _user_lvgl_set_img_dsc(&_home_speed_alpha_135x135, 135, 135, lvgl_img_map._home_speed_alpha_135x135_map);
  _user_lvgl_set_img_dsc(&_print_bed_schedule_alpha_150x150, 150, 150, lvgl_img_map._print_bed_schedule_alpha_150x150_map);
  _user_lvgl_set_img_dsc(&_print_division_schedule_alpha_4x45, 4, 45, lvgl_img_map._print_division_schedule_alpha_4x45_map);
  _user_lvgl_set_img_dsc(&_print_ext_schedule_alpha_150x150, 150, 150, lvgl_img_map._print_ext_schedule_alpha_150x150_map);
  _user_lvgl_set_img_dsc(&_print_filament_schedule_alpha_45x45, 45, 45, lvgl_img_map._print_filament_schedule_alpha_45x45_map);
  _user_lvgl_set_img_dsc(&_print_image_schedule_alpha_400x460, 400, 460, lvgl_img_map._print_image_schedule_alpha_400x460_map);
  _user_lvgl_set_img_dsc(&_print_progress_schedule_alpha_45x45, 45, 45, lvgl_img_map._print_progress_schedule_alpha_45x45_map);
  _user_lvgl_set_img_dsc(&_print_speep_schedule_alpha_150x150, 150, 150, lvgl_img_map._print_speep_schedule_alpha_150x150_map);
  _user_lvgl_set_img_dsc(&_print_time_schedule_alpha_45x45, 45, 45, lvgl_img_map._print_time_schedule_alpha_45x45_map);
  _user_lvgl_set_img_dsc(&_print_z_schedule_alpha_45x45, 45, 45, lvgl_img_map._print_z_schedule_alpha_45x45_map);
  _user_lvgl_set_img_dsc(&_topbar_back_alpha_40x70, 40, 70, lvgl_img_map._topbar_back_alpha_40x70_map);
  _user_lvgl_set_img_dsc(&_topbar_setup_alpha_70x70, 70, 70, lvgl_img_map._topbar_setup_alpha_70x70_map);
  _user_lvgl_set_img_dsc(&PrintOnlineTrans_animimgprint_online_t_1, 730, 312, lvgl_img_map._PrintOnlineTrans_animimgprint_online_t_1_p_map);
  _user_lvgl_set_img_dsc(&PrintOnlineTrans_animimgprint_online_t_2, 730, 312, lvgl_img_map._PrintOnlineTrans_animimgprint_online_t_2_p_map);
  _user_lvgl_set_img_dsc(&PrintOnlineTrans_animimgprint_online_t_3, 730, 312, lvgl_img_map._PrintOnlineTrans_animimgprint_online_t_3_p_map);
  _user_lvgl_set_img_dsc(&PrintOnlineTrans_animimgprint_online_t_4, 730, 312, lvgl_img_map._PrintOnlineTrans_animimgprint_online_t_4_p_map);
  _user_lvgl_set_img_dsc(&PrintOnlineTrans_animimgprint_online_t_5, 730, 312, lvgl_img_map._PrintOnlineTrans_animimgprint_online_t_5_p_map);
  _user_lvgl_set_img_dsc(&PrintOnlineTrans_animimgprint_online_t_6, 730, 312, lvgl_img_map._PrintOnlineTrans_animimgprint_online_t_6_p_map);
}

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
typedef struct
{
  uint16_t min;
  uint16_t max;
  uint8_t  bpp;
  uint8_t  reserved[3];
} x_header_t;
typedef struct
{
  uint32_t pos;
} x_table_t;
typedef struct
{
  uint8_t adv_w;
  uint8_t box_w;
  uint8_t box_h;
  int8_t  ofs_x;
  int8_t  ofs_y;
  uint8_t r;
} glyph_dsc_t;

static const uint8_t *__user_font_get_bitmap(const lv_font_t *font, uint32_t unicode_letter)
{
  x_header_t *p_hd = (x_header_t *)&lvgl_img_map._glyph_dsc_xbf_p_map[0];

  if (unicode_letter > p_hd->max || unicode_letter < p_hd->min)
  {
    return NULL;
  }

  uint32_t unicode_offset = unicode_letter - p_hd->min;
  uint32_t *p_pos = (uint32_t *)&lvgl_img_map._glyph_dsc_xbf_p_map[ sizeof(x_header_t) ];

  if (p_pos[unicode_offset] != 0)
  {
    return &lvgl_img_map._glyph_dsc_xbf_p_map[ p_pos[unicode_offset] + sizeof(glyph_dsc_t) ];
  }

  return NULL;
}


static bool __user_font_get_glyph_dsc(const lv_font_t *font, lv_font_glyph_dsc_t *dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
  x_header_t *p_hd = (x_header_t *)&lvgl_img_map._glyph_dsc_xbf_p_map[0];

  if (unicode_letter > p_hd->max || unicode_letter < p_hd->min)
  {
    return false;
  }

  uint32_t unicode_offset = unicode_letter - p_hd->min;
  uint32_t *p_pos = (uint32_t *)&lvgl_img_map._glyph_dsc_xbf_p_map[ sizeof(x_header_t) ];

  if (p_pos[unicode_offset] != 0)
  {
    glyph_dsc_t *gdsc = (glyph_dsc_t *)&lvgl_img_map._glyph_dsc_xbf_p_map[ p_pos[unicode_offset] ];
    dsc_out->adv_w = gdsc->adv_w;
    dsc_out->box_h = gdsc->box_h;
    dsc_out->box_w = gdsc->box_w;
    dsc_out->ofs_x = gdsc->ofs_x;
    dsc_out->ofs_y = gdsc->ofs_y;
    dsc_out->bpp   = p_hd->bpp;
    return true;
  }

  return false;
}

//Noto Sans SC,,-1
//字模高度：26
//XBF字体,内部大数组
lv_font_t lv_customer_font_NotoSansSC_Regular_18 =
{
  .get_glyph_bitmap = __user_font_get_bitmap,
  .get_glyph_dsc = __user_font_get_glyph_dsc,
  .line_height = 26,
  .base_line = 0,
};

void user_lvgl_init(void)
{
  lv_init(); //lvgl 系统初始化
  lv_port_disp_init(); //lvgl 显示接口初始化,放在 lv_init()的后面
  lv_port_indev_init(); //lvgl 输入接口初始化,放在 lv_init()的后面
  _user_lvgl_load_image();
  _user_lvgl_init_img_dsc();
  setup_ui(&guider_ui);
  events_init(&guider_ui);
  custom_init(&guider_ui);
}

void user_lvgl_task(void)
{
  osDelay(50);
  lv_task_handler();
  osDelay(50);
}
#endif



