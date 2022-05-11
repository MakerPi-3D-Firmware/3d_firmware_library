#ifndef GUI_TOUCH_H
#define GUI_TOUCH_H

#ifdef __cplusplus
extern "C" {
#endif

struct _touch
{
  volatile  int touch_flag;
  volatile  int down_flag;
  volatile  int up_flag;
  volatile  int pixel_x;
  volatile  int pixel_y;
  volatile  int touch_x;
  volatile  int touch_y;
};

void gui_touch_scan(void);
int gui_touch_xy(int touchx, int touchy, int rangex, int rangey);
int gui_touch_xy_no_beep(int touchx, int touchy, int rangex, int rangey);
int gui_touch_xy_is_up(void);
int gui_touch_xy_is_down(int touchx, int touchy, int rangex, int rangey);

bool gui_touch_43_480272_xy(int touchx, int touchy, int rangex, int rangey);
bool gui_touch_35_480320_xy(int touchx, int touchy, int rangex, int rangey);
bool gui_touch_7_1024600_xy(int touchx, int touchy, int rangex, int rangey);

bool gui_touch_43_480272_xy_is_down(int touchx, int touchy, int rangex, int rangey);
bool gui_touch_35_480320_xy_is_down(int touchx, int touchy, int rangex, int rangey);
bool gui_touch_7_1024600_xy_is_down(int touchx, int touchy, int rangex, int rangey);

bool gui_touch_43_480272_xy_is_range(int touchx, int touchy, int rangex, int rangey);
bool gui_touch_35_480320_xy_is_range(int touchx, int touchy, int rangex, int rangey);
bool gui_touch_7_1024600_xy_is_range(int touchx, int touchy, int rangex, int rangey);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // GUI_TOUCH_H

