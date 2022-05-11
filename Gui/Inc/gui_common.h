#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*menufunc_t)(void);
#define DEFAULT_TEXT_COLOR RED //文字显示颜色

extern u16 testcolor;

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gui
{
  extern volatile menufunc_t current_display;
  extern volatile int rtc_flag ;

  extern void need_refresh(void);
  extern bool is_refresh(void);
  extern bool is_refresh_rtc(void);
  extern void set_current_display(const volatile menufunc_t display);

  extern void set_text_range_4_buf(int index, int x, int y);
  extern void display_text_range_4_buf(int index, char *fmt, ...);
  extern void set_text_range_9_buf(int index, int x, int y);
  extern void display_text_range_9_buf(int index, char *fmt, ...);
  extern void set_disp_text_color(uint16_t chg_text_color);
  extern void display_text_buf(int x, int y, char *text);
  void disp_text(const unsigned char *pstr, int x, int y, int size, uint16_t colour);
}

#endif // GUI_COMMON_H

