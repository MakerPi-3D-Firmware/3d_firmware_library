#include "user_common_cpp.h"

#ifdef HAS_FILAMENT
namespace feature_filament
{

  static uint8_t startLoadFlag;               /*!< 开始进丝标志位 */
  static uint8_t startUnloadFlag;             /*!< 开始退丝标志位 */
  static uint8_t timeOutFlag;                 /*!< 进退丝超时标志位 */
  static unsigned long timeOutTickCount;      /*!< 进退丝超时计数 */
  static bool is_process_load_unload_done = false;
  bool Lowtemp_load = false;
  // 重置进退丝状态
  void control_reset_status(void)
  {
    // 设置目标温度为0
    sg_grbl::temperature_set_extruder_target((float)0, 0);
    ccm_param.t_gui.target_nozzle_temp[0] = 0;

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      sg_grbl::temperature_set_extruder_target((float)0, 1);
      ccm_param.t_gui.target_nozzle_temp[1] = 0;
    }

    // 重置进退丝状态变量
    startLoadFlag = 0;
    startUnloadFlag = 0;
    timeOutFlag = 0;
    timeOutTickCount = 0;
    is_process_load_unload_done = false;
  }

  static void _control_exit(bool isCancel)
  {
    // 退出进丝或退丝操作
    user_send_internal_cmd("G1 F2400"); // 设置速度为7200mm/min
    gcode_cmd_eb_absolute_mode();// 关闭绝对模式
    gcode_cmd_eb_reset(); // 重置E、B坐标值为0
    // 重置进退丝状态
    control_reset_status();
  }

  static void _control_prepare_home_and_z_down_50mm(void)
  {
    // Make sure that the XY axis is home
    if (!ccm_param.t_sys.is_gcode_g28_done)
    {
      gcode_cmd_g28();
      gcode_cmd_z_down('+', 50.0f);
    }
    else
    {
      gcode_cmd_z_down('+', 50.0f - sg_grbl::st_get_position_length(Z_AXIS));
      gcode_cmd_xy_move_to_home();
    }

    gcode_cmd_eb_relative_mode();
  }

  // 准备进退丝操作
  static void _control_prepare(void)
  {
    ccm_param.t_sys.is_filament_control_done = false;
    ccm_param.t_sys.is_filament_heat_done = false;
    sg_grbl::temperature_set_extruder_target((float)ccm_param.t_sys.filament_load_unload_temp, 0);
    ccm_param.t_gui.target_nozzle_temp[0] = ccm_param.t_sys.filament_load_unload_temp;
    gcode::active_extruder = 0;

    // 喷嘴预热230度
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      sg_grbl::temperature_set_extruder_target((float)ccm_param.t_sys.filament_load_unload_temp, 1);
      ccm_param.t_gui.target_nozzle_temp[1] = ccm_param.t_sys.filament_load_unload_temp;
    }

    _control_prepare_home_and_z_down_50mm();
  }


  static bool _control_process_check_noz_temp_done(void)
  {
    bool result = (int)sg_grbl::temperature_get_extruder_current(0) >= (ccm_param.t_gui.target_nozzle_temp[0] - 3);

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      result = result && (int)sg_grbl::temperature_get_extruder_current(1) >= (ccm_param.t_gui.target_nozzle_temp[1] - 3);
    }

    return result;
  }

  static bool _control_process_check_above_noz_min_temp(void)
  {
    bool result = (int)sg_grbl::temperature_get_extruder_current(0) >= ccm_param.t_sys.extrude_min_temp;

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
    {
      result = result && (int)sg_grbl::temperature_get_extruder_current(1) >= ccm_param.t_sys.extrude_min_temp;
    }

    return result;
  }

  static void _load_set_timeout(void)
  {
    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
      {
        timeOutTickCount = xTaskGetTickCount() + 45 * 1000UL; //120s
      }
      else
      {
        timeOutTickCount = xTaskGetTickCount() + 28 * 1000UL; //120s
      }
    }
    else if (K5 == ccm_param.t_sys_data_current.model_id)
    {
      if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
      {
        timeOutTickCount = xTaskGetTickCount() + (60 * 1000UL); //120s
      }
      else
      {
        timeOutTickCount = xTaskGetTickCount() + (90 * 1000UL); //120s
      }
    }
    else
    {
      timeOutTickCount = xTaskGetTickCount() + 120 * 1000UL; //120s
    }
  }

  static void _load_pre_do_and_set_timeout(void)
  {
    if (!timeOutFlag)
    {
      _load_set_timeout();
      gcode_cmd_load_pre_do();
      timeOutFlag = 1;
    }
  }

  // 执行进丝操作
  static void _control_process_load(void)
  {
    if (_control_process_check_noz_temp_done())   //最后几度要等待挺长时间的，不再等待
    {
      _load_pre_do_and_set_timeout();
    }

    if (timeOutFlag && !is_process_load_unload_done)
    {
      if (_control_process_check_above_noz_min_temp())
      {
        if (sg_grbl::planner_moves_planned() > 1)   //只有1个有效block的时候，继续发送进丝命令
        {
          return;
        }
        else
        {
          gcode_cmd_load_process();
        }
      }
    }
  }

  static void _unload_set_timeout(void)
  {
    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
    {
      if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
      {
        timeOutTickCount = xTaskGetTickCount() + 30 * 1000UL; //60s
      }
      else
      {
        timeOutTickCount = xTaskGetTickCount() + 25 * 1000UL; //60s
      }
    }
    else
    {
      timeOutTickCount = xTaskGetTickCount() + 60 * 1000UL; //60s
    }
  }

  static void _unload_pre_do_and_set_timeout(void)
  {
    if (!timeOutFlag)
    {
      _unload_set_timeout();
      gcode_cmd_unload_pre_do();
      timeOutFlag = 1;
    }
  }

  // 执行退丝操作
  static void _control_process_unload(void)
  {
    if (_control_process_check_noz_temp_done())   //最后几度要等待挺长时间的，不再等待
    {
      _unload_pre_do_and_set_timeout();
    }

    if (timeOutFlag && !is_process_load_unload_done)
    {
      if (_control_process_check_above_noz_min_temp())
      {
        if (sg_grbl::planner_moves_planned() > 1)
        {
          return;
        }
        else     //只有1个有效block的时候，继续发送退丝命令
        {
          gcode_cmd_unload_process();
        }
      }
    }
  }

  void control_init()
  {
    startLoadFlag = 0;
    startUnloadFlag = 0;
    timeOutFlag = 0;
    timeOutTickCount = 0;
  }

  // 开始进丝
  void control_start_load(void)
  {
    if (!startLoadFlag) // 避免重复触发
    {
      _control_prepare();
    }

    startLoadFlag = 1;
  }

  // 开始退丝
  void control_start_unload(void)
  {
    if (!startUnloadFlag) // 避免重复触发
    {
      _control_prepare();
    }

    startUnloadFlag = 1;
  }

  // 执行进退丝入口
  void control_process(void)
  {
    if ((!startLoadFlag) && (!startUnloadFlag)) // 进退丝标志位都为false，退出
    {
      return;
    }

    if (startLoadFlag)
    {
      _control_process_load();
    }

    if (startUnloadFlag)
    {
      _control_process_unload();
    }

    //进丝成功或退丝成功
    if (timeOutFlag)
    {
      // 判断进丝或退丝加热是否完成
      ccm_param.t_sys.is_filament_heat_done = true;

      if (xTaskGetTickCount() > timeOutTickCount)
      {
        is_process_load_unload_done = true;
      }

      if (is_process_load_unload_done && sg_grbl::planner_moves_planned() == 0)
      {
        // 退出进退丝操作
        _control_exit(false);
        // 判断进丝或退丝是否完成
        ccm_param.t_sys.is_filament_control_done = true;
      }
    }

    osDelay(50);
  }

  void control_cancel_process(void)
  {
    stepper_quick_stop(); // 电机快速停止
    _control_exit(true);
  }

}


#endif











