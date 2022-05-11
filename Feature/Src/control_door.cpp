#include "user_common_cpp.h"
#ifdef HAS_DOOR_CONTROL
#ifdef __cplusplus
extern "C" {
#endif

//警示灯控制
static void _feature_control_door_caution_light(void)
{
  static uint8_t LastStatus = 0U;
  static bool doorOpenBeep = false;//标志门检打开了蜂鸣器

  if (ccm_param.t_sys.is_door_open && print_status.is_printing) //打印中门未关闭
  {
    LastStatus = 1U;
    ccm_param.t_sys.is_door_open_disp_info = false;
  }
  else if (print_status.is_printing && ccm_param.t_sys.is_gcode_m109_done) //加热完成并开始打印
  {
    LastStatus = 2U;
    ccm_param.t_sys.is_door_open_disp_info = false;
  }
  else if ((int)sg_grbl::temperature_get_extruder_current(0) > 60 || print_status.is_printing) //温度大于60度 或 打印前的加热阶段
  {
    if (ccm_param.t_sys.is_door_open)
    {
      ccm_param.t_sys.is_door_open_beep = true;
      doorOpenBeep = true;
    }
    else
    {
      if (doorOpenBeep)
      {
        ccm_param.t_sys.is_door_open_beep = false;
        doorOpenBeep = false;
      }
    }

    if (((int)sg_grbl::temperature_get_extruder_current(0) > 60) && !print_status.is_printing && ccm_param.t_sys.is_door_open) //待机、预热、进丝、退丝 时 温度大于60度且门打开显示提示信息
    {
      ccm_param.t_sys.is_door_open_disp_info = true;
    }
    else
    {
      ccm_param.t_sys.is_door_open_disp_info = false;
    }

    LastStatus = 3U;
  }
  else //待机且温度小于60度
  {
    if ((LastStatus == 3) && ccm_param.t_sys.is_door_open)
    {
      ccm_param.t_sys.is_door_open_beep = false;
    }

    LastStatus = 4U;
    ccm_param.t_sys.is_door_open_disp_info = false;
  }
}

// 门检测
static void _feature_control_door_check_open(void)
{
  if (ccm_param.t_sys.enable_check_door_open) // 有门检测功能，开启门检测
  {
    if (user_pin_sig_door_read())
    {
      //door open
      ccm_param.t_sys.is_door_open = true;
    }
    else
    {
      //door close
      ccm_param.t_sys.is_door_open = false;
    }
  }
}

void feature_control_door(void)
{
  if ((ccm_param.t_sys.enable_check_door_open))
  {
    _feature_control_door_caution_light();
    _feature_control_door_check_open();
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_DOOR_CONTROL














