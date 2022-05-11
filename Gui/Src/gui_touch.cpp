#include "user_common_cpp.h"
#ifdef __cplusplus
extern "C" {
#endif

struct _touch touch = {0};
static bool gui_touch_is_beep = false;

#define TICKCOUNT  100

static bool gui_touch_xy_is_touch(int touchx, int touchy, int rangex, int rangey)
{
  return touch.touch_flag && \
         touch.pixel_x > touchx && touch.pixel_x < rangex && \
         touch.pixel_y > touchy && touch.pixel_y < rangey;
}

int gui_touch_xy_common(int touchx, int touchy, int rangex, int rangey)
{
  if (gui_touch_xy_is_touch(touchx, touchy, rangex, rangey))
  {
    if (ccm_param.t_sys.sound_key)              //如果设置了按键响声
    {
      if (gui_touch_is_beep)
        user_buzzer_buzz(TICKCOUNT);     //响铃函数
    }

    osDelay(50);
    return 1;
  }
  else
  {
    return 0;
  }
}

int gui_touch_xy_is_down(int touchx, int touchy, int rangex, int rangey)
{
  gui_touch_is_beep = true;
  return gui_touch_xy_common(touchx, touchy, rangex, rangey);
}

int gui_touch_xy_is_up(void)  //add by zouxb
{
  if (touch.up_flag)
  {
    touch.touch_flag = 0;
    return 1;
  }
  else
  {
    return 0;
  }
}

int gui_touch_xy_no_beep(int touchx, int touchy, int rangex, int rangey)
{
  if (touch.up_flag)
  {
    gui_touch_is_beep = false;
    return gui_touch_xy_common(touchx, touchy, rangex, rangey);
  }
  else
  {
    return 0;
  }
}

int gui_touch_xy(int touchx, int touchy, int rangex, int rangey)
{
  if (touch.up_flag)
  {
    gui_touch_is_beep = true;
    return gui_touch_xy_common(touchx, touchy, rangex, rangey);
  }
  else
  {
    return 0;
  }
}

void gui_touch_scan(void)        //触摸扫描函数
{
  (void)tp_dev.scan(0);

  if (tp_dev.sta & (0x80))   //有触摸设置相关参数
  {
    touch.touch_flag = 1;
    touch.down_flag = 1;
    touch.up_flag = 0;

    if (ccm_param.t_sys.is_touch_capacitive && (lcddev.id == 0X7016 || lcddev.id == 0X7018)) // 7寸电容触摸
    {
      touch.pixel_x = tp_dev.x[0];
      touch.pixel_y = tp_dev.y[0];
    }
    else
    {
      touch.pixel_x = tp_dev.x[1];
      touch.pixel_y = tp_dev.y[1];
    }
  }
  else                       //没触摸设置相关参数
  {
    touch.up_flag = 1;
    touch.down_flag = 0;
  }

  if (touch.touch_flag)      //根据触摸信号和rtc信号进入界面显示
  {
    gui::current_display(); //进入模块函数处理信号

    if (touch.up_flag) //有触摸信号，进入函数处理
    {
      touch.touch_flag = 0;            //关闭相关的tch信号
      touch.up_flag = 0;               //关闭相关的tch信号
    }
  }
}

bool gui_touch_43_480272_xy(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE && gui_touch_xy(touchx, touchy, rangex, rangey);
}

bool gui_touch_35_480320_xy(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE && gui_touch_xy(touchx, touchy, rangex, rangey);
}

bool gui_touch_7_1024600_xy(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE && gui_touch_xy(touchx, touchy, rangex, rangey);
}

bool gui_touch_43_480272_xy_is_down(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE && gui_touch_xy_is_down(touchx, touchy, rangex, rangey);
}

bool gui_touch_35_480320_xy_is_down(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE && gui_touch_xy_is_down(touchx, touchy, rangex, rangey);
}

bool gui_touch_7_1024600_xy_is_down(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE && gui_touch_xy_is_down(touchx, touchy, rangex, rangey);
}

bool gui_touch_43_480272_xy_is_range(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE && gui_touch_xy_no_beep(touchx, touchy, touchx + rangex, touchy + rangey);
}

bool gui_touch_35_480320_xy_is_range(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE && gui_touch_xy_no_beep(touchx, touchy, touchx + rangex, touchy + rangey);
}

bool gui_touch_7_1024600_xy_is_range(int touchx, int touchy, int rangex, int rangey)
{
  return ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE && gui_touch_xy_no_beep(touchx, touchy, touchx + rangex, touchy + rangey);
}

#ifdef __cplusplus
} //extern "C" {
#endif

