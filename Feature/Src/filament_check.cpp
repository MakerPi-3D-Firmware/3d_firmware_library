#include "user_common_cpp.h"

#ifdef HAS_FILAMENT

static uint8_t filament_check_count = 0;
static uint32_t filament_check_timeout = 0;

namespace feature_filament
{

  static void _check_err(void)
  {
    if (filament_check_count < 5)
    {
      filament_check_count++;
    }
  }

  static void _reset_check_count(void)
  {
    filament_check_count = 0;
    transFilePrintStatus.isFilamentCheck = false;
  }

  static void _check_idex_axis(const int _active_extruder, const int _change_extruder)
  {
    if (ccm_param.t_sys.print_type_idex_single_mode) // 非双色模型
    {
      USER_EchoLogStr("feature_filament_check ==>> (idex_single) E%d not filament!\r\n", _active_extruder);

      if ((int)sg_grbl::temperature_get_extruder_target(_active_extruder) == 0)
      {
        USER_EchoLogStr("feature_filament_check ==>> (idex_single) E%d not temp!\r\n", _active_extruder);
        return;
      }

      _reset_check_count();
      ccm_param.t_sys.is_gcode_m109_done = false; //设置为未加热
      USER_SEND_INTERNAL_CMD_BUF("M400"); //等待指令执行完成
      USER_SEND_INTERNAL_CMD_BUF("T%d", _change_extruder); //切换喷头
      USER_SEND_INTERNAL_CMD_BUF("M104 T%d S50", _active_extruder); //降温当前喷头
      USER_SEND_INTERNAL_CMD_BUF("M109 T%d S%d", _change_extruder, (int)sg_grbl::temperature_get_extruder_target(_active_extruder)); //设置切换喷头温度
      USER_SEND_INTERNAL_CMD_BUF("G92 E%f", ccm_param.grbl_destination[_active_extruder == 0 ? E_AXIS : B_AXIS]); //设置切换头E值
      USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.grbl_destination[Z_AXIS] + 10.0f);
      float x = 0.0f;

      if (_active_extruder == 0)
      {
        x =  ccm_param.grbl_destination[X_AXIS] - flash_param_t.idex_extruder_0_bed_offset[0];

        if (x >= ccm_param.t_model.xyz_min_pos[X_AXIS])
        {
          USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f Y%f", x, ccm_param.grbl_destination[Y_AXIS]);
        }
      }
      else if (_active_extruder == 1)
      {
        x =  ccm_param.grbl_destination[X2_AXIS] - (flash_param_t.idex_extruder_0_bed_offset[0] - flash_param_t.idex_extruder_1_bed_offset[0]);

        if (x >= ccm_param.t_model.xyz_min_pos[X2_AXIS])
        {
          USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f Y%f", x, ccm_param.grbl_destination[Y_AXIS]);
        }
      }

      USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.grbl_destination[Z_AXIS]);
    }
    else
    {
      USER_EchoLogStr("feature_filament_check ==>> (idex_dual) E%d not filament!\r\n", _active_extruder);
      _check_err();
    }
  }

  static void _check_idex(void)
  {
    if (user_pin_sig_mat_e0_read() && user_pin_sig_mat_e1_read())
    {
      USER_EchoLogStr("feature_filament_check ==>> (dual) E0 and E1 not filament!\r\n");
      _check_err();
    }
    else
    {
      if (gcode::active_extruder == 0)
      {
        if (user_pin_sig_mat_e0_read())
        {
          _check_idex_axis(0, 1);
        }
        else
        {
          _reset_check_count();
        }
      }
      else if (gcode::active_extruder == 1)
      {
        if (user_pin_sig_mat_e1_read())
        {
          _check_idex_axis(1, 0);
        }
        else
        {
          _reset_check_count();
        }
      }
    }
  }

  static void _check_single(void)
  {
    if (user_pin_sig_mat_e0_read())
    {
      _check_err();
    }
    else
    {
      _reset_check_count();
    }
  }

  void check_init(void)
  {
  }

  void check(void)
  {
    if (ccm_param.t_sys_data_current.enable_material_check) //有断料检测功能
    {
      if (filament_check_timeout < xTaskGetTickCount())
      {
        if (print_status.is_printing && ccm_param.t_sys.is_gcode_m109_done) //打印开始且加热完成后 才去检测是否有料
        {
          if (feature_print_control::idex_sys_is_extruder()) // idex模式
          {
            if (feature_print_control::idex_sys_is_normal()) //正常模式
            {
              _check_idex(); // idex
            }
            else if (feature_print_control::idex_sys_is_copy_or_mirror()) // 克隆、镜像模式
            {
              if (user_pin_sig_mat_e0_read() || user_pin_sig_mat_e1_read()) // 任意头没料，报警
              {
                USER_EchoLogStr("feature_filament_check ==>> (copy\\mirror) E0 or E1 not filament!\r\n");
                _check_err();
              }
            }
          }
          else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG) // 激光模式不检测
          {
            return;
          }
          else
          {
            _check_single(); // 单头、混色头
          }
        }

        if (filament_check_count == 3)
        {
          filament_check_count = 0;
          transFilePrintStatus.isFilamentCheck = true;
          ccm_param.t_sys.is_filament_print_not_have = true; //在打印的时候没料了
          osDelay(100);
#ifndef ENABLE_GUI_LVGL
          waiting_for_pausing(gui_filament_not_have_entry);
#endif
        }

        filament_check_timeout = MILLIS() + 1000;
      }
    }
  }
}

#endif // HAS_FILAMENT_SENSOR

