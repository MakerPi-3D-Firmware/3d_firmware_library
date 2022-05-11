#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

u16 testcolor = 0xf800;     //数值显示的颜色


#ifdef __cplusplus
} //extern "C" {
#endif

namespace gui
{
  volatile menufunc_t current_display;
  volatile int rtc_flag = 0;           //实时rtc刷新信号
  static volatile int refresh_flage = 1;      //界面刷新信号

  bool is_refresh(void)            //检测界面是否需要刷新
  {
    if (refresh_flage)
    {
      refresh_flage = 0;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool is_refresh_rtc(void)
  {
    osDelay(50);

    if (rtc_flag)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  void set_current_display(const volatile menufunc_t display)
  {
    refresh_flage = 1;
    current_display = display;
    osDelay(50);
  }
}













