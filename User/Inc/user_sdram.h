#ifndef _USER_SDRAM_H
#define _USER_SDRAM_H

#ifdef ENABLE_SDRAM

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint8_t _control_add_alpha_70x40_map[3 * 70 * 40];
  uint8_t _control_subtract_alpha_70x40_map[3 * 70 * 40];
  uint8_t _home_bed_alpha_135x135_map[3 * 135 * 135];
  uint8_t _home_control_alpha_250x250_map[3 * 250 * 250];
  uint8_t _home_ext_alpha_135x135_map[3 * 135 * 135];
  uint8_t _home_print_alpha_250x250_map[3 * 250 * 250];
  uint8_t _home_speed_alpha_135x135_map[3 * 135 * 135];
  uint8_t _print_bed_schedule_alpha_150x150_map[3 * 150 * 150];
  uint8_t _print_division_schedule_alpha_4x45_map[3 * 4 * 45];
  uint8_t _print_ext_schedule_alpha_150x150_map[3 * 150 * 150];
  uint8_t _print_filament_schedule_alpha_45x45_map[3 * 45 * 45];
  uint8_t _print_image_schedule_alpha_400x460_map[3 * 400 * 460];
  uint8_t _print_progress_schedule_alpha_45x45_map[3 * 45 * 45];
  uint8_t _print_speep_schedule_alpha_150x150_map[3 * 150 * 150];
  uint8_t _print_time_schedule_alpha_45x45_map[3 * 45 * 45];
  uint8_t _print_z_schedule_alpha_45x45_map[3 * 45 * 45] ;
  uint8_t _topbar_back_alpha_40x70_map[3 * 40 * 70];
  uint8_t _topbar_setup_alpha_70x70_map[3 * 70 * 70];
  uint8_t _PrintOnlineTrans_animimgprint_online_t_1_p_map[3 * 730 * 312];
  uint8_t _PrintOnlineTrans_animimgprint_online_t_2_p_map[3 * 730 * 312];
  uint8_t _PrintOnlineTrans_animimgprint_online_t_3_p_map[3 * 730 * 312];
  uint8_t _PrintOnlineTrans_animimgprint_online_t_4_p_map[3 * 730 * 312];
  uint8_t _PrintOnlineTrans_animimgprint_online_t_5_p_map[3 * 730 * 312];
  uint8_t _PrintOnlineTrans_animimgprint_online_t_6_p_map[3 * 730 * 312];
  uint8_t _glyph_dsc_xbf_p_map[3689914];
} LVGL_IMG_MAP_T;

typedef struct
{
  //根据不同的颜色格式,定义帧缓存数组
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
  u32 ltdc_lcd_framebuf[1280][800]; //定义最大屏分辨率时,LCD所需的帧缓存数组大小
#else
  u16 ltdc_lcd_framebuf[1280][800] ; //定义最大屏分辨率时,LCD所需的帧缓存数组大小
#endif
#if defined(USE_APP1)|| defined(USE_APP2)
  unsigned char blktbl[4096];
  unsigned short int lutbuf[4096 * 2];
#endif
#define TRANS_FILE_BUF_SIZE (1024*64+1)  //64K
  volatile uint8_t trans_file_bufs[TRANS_FILE_BUF_SIZE];
#ifdef ENABLE_GUI_LVGL
  lv_color_t color_buf[LV_HOR_RES_MAX * LV_VER_RES_MAX];
  lv_color_t color_buf2[LV_HOR_RES_MAX * LV_VER_RES_MAX];
  LVGL_IMG_MAP_T lvgl_img_map;
  char lvgl_buff[128];
#endif
  unsigned short int text_range_buf_24_12_9[TEXT_BUF_24_12_9_SIZE];    /*!< */
  unsigned short int TextRangeBuf_24_12_9[6][TEXT_BUF_24_12_9_SIZE];    /*!< */
  unsigned short int TextRangeBuf_24_12_4[4][TEXT_BUF_24_12_4_SIZE];
} SDRAM_PARAM;

extern SDRAM_PARAM sdram_param;

extern void user_fmc_sdram_init(void);
extern u8 user_fmc_sdram_send_cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval);
extern void user_fmc_sdram_write_buffer(u8 *pBuffer, u32 WriteAddr, u32 n);
extern void user_fmc_sdram_read_buffer(u8 *pBuffer, u32 ReadAddr, u32 n);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // ENABLE_SDRAM

#endif //_USER_SDRAM_H

