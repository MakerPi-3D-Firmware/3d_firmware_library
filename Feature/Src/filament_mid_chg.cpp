#include "user_common_cpp.h"

#ifdef HAS_FILAMENT

#ifdef __cplusplus
extern "C" {
#endif

void setConfirmLoadFilament(int _IsConfirmLoadFilament);
void setMidWayChangeMat(bool value);
#ifndef ENABLE_GUI_LVGL
void set_refresh_once(void);
#endif

#ifdef __cplusplus
} //extern "C" {
#endif
namespace feature_filament
{

#define NotStartChangeFilament 0
#define StartChangeFilament 1
#define ChangeFilamentFinish 5
#define HeatNotComplete 0
#define HeatComplete 1

  static int IsStartChangeFilament = NotStartChangeFilament;


  void mid_chg_confirm_load(void)
  {
    setConfirmLoadFilament(1);
  }

  void mid_chg_start(void)
  {
    taskENTER_CRITICAL();
    print_status.is_mid_chg_filament = true;
    taskEXIT_CRITICAL();
    osDelay(200);
#ifndef ENABLE_GUI_LVGL
    set_refresh_once();
#endif
    gcode::m600_status = 0;
    IsStartChangeFilament = StartChangeFilament;
    // 打开中途换料标志位
    setMidWayChangeMat(true);

    if (ccm_param.t_sys.is_filament_print_not_have) //断料续打中的换料发送M601
    {
      user_send_internal_cmd((char *)"M601");   //换料
    }
    else
    {
      user_send_internal_cmd((char *)"M600");
    }
  }

  void mid_chg_refresh_status(void)
  {
    if (StartChangeFilament == IsStartChangeFilament)
    {
      if (M600_STATUS_FINISH == gcode::m600_status)
      {
        IsStartChangeFilament = NotStartChangeFilament;
        gcode::m600_status = 0;
        setConfirmLoadFilament(0);
      }
    }
  }

}



#endif // HAS_FILAMENT

