#include "user_common_cpp.h"
#ifdef HAS_POWER_RECOVERY

static volatile uint8_t _feature_pow_rec_status = 0;
extern volatile float mix_eb_value_save;
extern volatile int mix_current_layer_save;

#ifdef __cplusplus
extern "C" {
#endif

static volatile bool _feature_pow_rec_is_start = false;                       /*!< 是否为断电恢复操作 */
static volatile bool _feature_pow_rec_is_finish = false;                 /*!< 是否为断电恢复操作 */

void feature_pow_rec_ready_to_print(void)
{
  if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG || flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      return;

    _feature_pow_rec_is_start = 1;
    ccm_param.t_sys.is_power_off_rec_flag = true; //标志为断电状态
  }
}

static void _feature_pow_rec_eb_set(void)
{
  gcode_cmd_coord_transform(false);    // 关闭坐标转换

  if (feature_print_control::idex_sys_is_extruder())
  {
    if (feature_print_control::idex_flash_is_copy_or_mirror())
    {
      gcode_cmd_eb_reset();          // 设置eb位置
      USER_SEND_INTERNAL_CMD_BUF("G1 F150 E8 B8 D1");      // eb各自运动8mm
      USER_SEND_INTERNAL_CMD_BUF("G92 E%f B%f", ccm_param.t_power_off.axis_real_position[E_AXIS], ccm_param.t_power_off.axis_real_position[B_AXIS]); // 设置EB坐标
    }
    else if (feature_print_control::idex_flash_is_normal())
    {
      gcode_cmd_eb_reset();          // 设置eb位置

      if (ccm_param.t_sys_data_current.enable_color_mixing)
      {
        USER_SEND_INTERNAL_CMD_BUF("G1 F150 E8 B8 D1");      // eb各自运动8mm
      }
      else
      {
        USER_SEND_INTERNAL_CMD_BUF("G1 F150 E16 B0 D1");      // eb各自运动8mm
      }

      USER_SEND_INTERNAL_CMD_BUF("G92 %c%f %c%f", axis_codes[E_AXIS], ccm_param.t_power_off.axis_real_position[E_AXIS],
                                 axis_codes[B_AXIS], ccm_param.t_power_off.axis_real_position[B_AXIS]); // 设置EB坐标
    }
  }
  else
  {
    gcode_cmd_eb_reset();          // 设置eb位置

    if (ccm_param.t_sys_data_current.enable_color_mixing)
    {
      USER_SEND_INTERNAL_CMD_BUF("G1 F150 E8 B8 D1");      // eb各自运动8mm
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("G1 F150 E16 B0 D1");      // eb各自运动8mm
    }

    USER_SEND_INTERNAL_CMD_BUF("G92 %c%f %c%f", axis_codes[E_AXIS], ccm_param.t_power_off.axis_real_position[E_AXIS],
                               axis_codes[B_AXIS], ccm_param.t_power_off.axis_real_position[B_AXIS]); // 设置EB坐标
  }

  gcode_cmd_coord_transform(true);            // 开启坐标转换
}

static void _feature_pow_rec_z_set_p3_pro(void)
{
  gcode_cmd_coord_transform(false); // 关闭坐标转换
  USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.t_power_off.axis_real_position[Z_AXIS]); // 移动Z
  USER_SEND_INTERNAL_CMD_BUF("G1 F%f R%u",
                             ccm_param.t_power_off.feed_rate > 2400 ?
                             2400 :
                             ccm_param.t_power_off.feed_rate, ccm_param.t_power_off.sd_position); // 设置移动速度，断电文件位置
  gcode_cmd_coord_transform(true);    // 开启坐标转换

  if (feature_print_control::idex_sys_is_extruder())
  {
    if (feature_print_control::idex_flash_is_copy_or_mirror())
    {
      gcode_cmd_t0(false);
      USER_SEND_INTERNAL_CMD_BUF("G92 X%f I%f Y%f Z%f D1",
                                 ccm_param.t_power_off.axis_position[X_AXIS],
                                 ccm_param.t_power_off.axis_position[X2_AXIS],
                                 ccm_param.t_power_off.axis_position[Y_AXIS],
                                 ccm_param.t_power_off.axis_position[Z_AXIS]);
      USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f I%f Y%f Z%f D1",
                                 ccm_param.t_power_off.axis_position[X_AXIS],
                                 ccm_param.t_power_off.axis_position[X2_AXIS],
                                 ccm_param.t_power_off.axis_position[Y_AXIS],
                                 ccm_param.t_power_off.axis_position[Z_AXIS]);
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("T%d S-1", ccm_param.t_power_off.active_extruder); // 设置当前喷嘴id
      USER_SEND_INTERNAL_CMD_BUF("G92 X%f Y%f Z%f D1",
                                 ccm_param.t_power_off.axis_position[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                                 ccm_param.t_power_off.axis_position[Y_AXIS],
                                 ccm_param.t_power_off.axis_position[Z_AXIS]);
      USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f Y%f Z%f D1",
                                 ccm_param.t_power_off.axis_position[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                                 ccm_param.t_power_off.axis_position[Y_AXIS],
                                 ccm_param.t_power_off.axis_position[Z_AXIS]);
    }
  }
  else
  {
    USER_SEND_INTERNAL_CMD_BUF("T%d S-1", ccm_param.t_power_off.active_extruder); // 设置当前喷嘴id
    USER_SEND_INTERNAL_CMD_BUF("G92 X%f Y%f Z%f D1",
                               ccm_param.t_power_off.axis_position[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                               ccm_param.t_power_off.axis_position[Y_AXIS],
                               ccm_param.t_power_off.axis_position[Z_AXIS]);
    USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f Y%f Z%f D1",
                               ccm_param.t_power_off.axis_position[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                               ccm_param.t_power_off.axis_position[Y_AXIS],
                               ccm_param.t_power_off.axis_position[Z_AXIS]);
  }
}

static void _feature_pow_rec_z_set(void)
{
  if (!ccm_param.t_sys.enable_z_max_limit && ccm_param.t_sys.is_power_off_z_not_move)
  {
    _feature_pow_rec_z_set_p3_pro();
  }
  else if (F300TP == ccm_param.t_sys_data_current.model_id || M30 == ccm_param.t_sys_data_current.model_id)
  {
    gcode_cmd_coord_transform(false); // 关闭坐标转换
    USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.t_power_off.axis_real_position[Z_AXIS]); // 移动Z
    USER_SEND_INTERNAL_CMD_BUF("G1 F%f R%u",
                               ccm_param.t_power_off.feed_rate > 2400 ?
                               2400 :
                               ccm_param.t_power_off.feed_rate, ccm_param.t_power_off.sd_position); // 设置移动速度，断电文件位置
    gcode_cmd_coord_transform(true);    // 开启坐标转换
  }
  else if (K5 == ccm_param.t_sys_data_current.model_id)
  {
    gcode_cmd_coord_transform(false); // 关闭坐标转换
    USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.t_power_off.axis_real_position[Z_AXIS]); // 移动Z
    USER_SEND_INTERNAL_CMD_BUF("G1 F%f R%u",
                               ccm_param.t_power_off.feed_rate > 2400 ?
                               2400 :
                               ccm_param.t_power_off.feed_rate, ccm_param.t_power_off.sd_position); // 设置移动速度，断电文件位置
    gcode_cmd_coord_transform(true);    // 开启坐标转换
  }
}

static void _feature_pow_rec_heating(void)
{
  if (ccm_param.t_sys.enable_hot_bed)
  {
    if (ccm_param.t_power_off.target_bed_temp > 50)
    {
      USER_SEND_INTERNAL_CMD_BUF("M190 S50");
      USER_SEND_INTERNAL_CMD_BUF("M140 S%d", (int)ccm_param.t_power_off.target_bed_temp);
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("M190 S%d", (int)ccm_param.t_power_off.target_bed_temp);
    }
  }

  USER_SEND_INTERNAL_CMD_BUF("M104 T0 S%d", (int)ccm_param.t_power_off.target_extruder0_temp);
  USER_SEND_INTERNAL_CMD_BUF("M104 T1 S%d", (int)ccm_param.t_power_off.target_extruder1_temp);
  USER_SEND_INTERNAL_CMD_BUF("M109 T0 S%d", (int)ccm_param.t_power_off.target_extruder0_temp);
  USER_SEND_INTERNAL_CMD_BUF("M109 T1 S%d", (int)ccm_param.t_power_off.target_extruder1_temp);
}

static void _feature_pow_rec_xy_set(void)
{
  gcode_cmd_coord_transform(false);

  if (feature_print_control::idex_sys_is_extruder())
  {
    if (feature_print_control::idex_flash_is_copy_or_mirror())
    {
      gcode_cmd_t0(false);
      USER_SEND_INTERNAL_CMD_BUF("G92 X%f I%f Y%f D1",
                                 ccm_param.t_model.xyz_home_pos[X_AXIS],
                                 ccm_param.t_model.xyz_home_pos[X2_AXIS],
                                 ccm_param.t_model.xyz_home_pos[Y_AXIS]);
      USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f I%f Y%f D1",
                                 ccm_param.t_power_off.axis_real_position[X_AXIS],
                                 ccm_param.t_power_off.axis_real_position[X2_AXIS],
                                 ccm_param.t_power_off.axis_real_position[Y_AXIS]);
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("T%d S-1", ccm_param.t_power_off.active_extruder);
      USER_SEND_INTERNAL_CMD_BUF("G92 X%f Y%f D1",
                                 ccm_param.t_model.xyz_home_pos[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                                 ccm_param.t_model.xyz_home_pos[Y_AXIS]);
      USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f Y%f D1",
                                 ccm_param.t_power_off.axis_real_position[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                                 ccm_param.t_power_off.axis_real_position[Y_AXIS]);
    }
  }
  else
  {
    USER_SEND_INTERNAL_CMD_BUF("T%d S-1", ccm_param.t_power_off.active_extruder);
    USER_SEND_INTERNAL_CMD_BUF("G92 X%f Y%f D1",
                               ccm_param.t_model.xyz_home_pos[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                               ccm_param.t_model.xyz_home_pos[Y_AXIS]);
    USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%f Y%f D1",
                               ccm_param.t_power_off.axis_real_position[ccm_param.t_power_off.active_extruder == 0 ? X_AXIS : X2_AXIS],
                               ccm_param.t_power_off.axis_real_position[Y_AXIS]);
  }

  gcode_cmd_coord_transform(true);
}

static void _feature_pow_rec_prepare(void)
{
  // 先加热到150度，再移动喷嘴防止喷嘴与打印模具粘在一起。
  if (ccm_param.t_power_off.target_extruder0_temp >= 20)
    USER_SEND_INTERNAL_CMD_BUF("M104 T0 S%d", (int)ccm_param.t_power_off.target_extruder0_temp - 20);

  if (ccm_param.t_power_off.target_extruder1_temp >= 20)
    USER_SEND_INTERNAL_CMD_BUF("M104 T1 S%d", (int)ccm_param.t_power_off.target_extruder1_temp - 20);

  if (ccm_param.t_power_off.target_extruder0_temp >= 20)
    USER_SEND_INTERNAL_CMD_BUF("M109 T0 S%d", (int)ccm_param.t_power_off.target_extruder0_temp - 20);

  if (ccm_param.t_power_off.target_extruder1_temp >= 20)
    USER_SEND_INTERNAL_CMD_BUF("M109 T1 S%d", (int)ccm_param.t_power_off.target_extruder1_temp - 20);

  // P3_Pro断电Z不动
  if (!ccm_param.t_sys.enable_z_max_limit && ccm_param.t_sys.is_power_off_z_not_move)
  {
    // 设置当前Z位置
    gcode_cmd_coord_transform(false);            // 关闭坐标转换

    if (user_is_float_data_equ(ccm_param.t_power_off.z_real_change_value, 0.0f))
    {
      USER_SEND_INTERNAL_CMD_BUF("G92 Z%f", ccm_param.t_power_off.axis_real_position[Z_AXIS]);
      USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.t_power_off.axis_real_position[Z_AXIS] + 10.0f);      // Z增加60mm
      flash_poweroff_data_reset();
      ccm_param.t_sys.is_power_off_rec_data_chg = true;
      ccm_param.flash_poweroff_recovery.z_real_change_value = ccm_param.t_power_off.axis_real_position[Z_AXIS] + 10.0f;
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("G92 Z%f", ccm_param.t_power_off.z_real_change_value);
    }

    gcode_cmd_coord_transform(true);            // 开启坐标转换
    gcode_cmd_all_axis_relative_mode(false);                 // 关闭相对模式
    gcode_cmd_eb_absolute_mode();
    // XY回原点
    gcode_cmd_xy_move_to_home();
  }
  else if (F300TP == ccm_param.t_sys_data_current.model_id || M30 == ccm_param.t_sys_data_current.model_id)
  {
    // 设置当前Z位置
    gcode_cmd_coord_transform(false);            // 关闭坐标转换
    USER_SEND_INTERNAL_CMD_BUF("G92 Z0");
    USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z999");
    USER_SEND_INTERNAL_CMD_BUF("M400");
    USER_SEND_INTERNAL_CMD_BUF("G92 Z%f",  flash_param_t.poweroff_rec_z_max_value);
    USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.t_power_off.axis_real_position[Z_AXIS] + 20); // 移动Z
    gcode_cmd_coord_transform(true);            // 开启坐标转换
    gcode_cmd_all_axis_relative_mode(false);                 // 关闭相对模式
    gcode_cmd_eb_absolute_mode();
    // XY回原点
    gcode_cmd_xy_move_to_home();
  }
  else if (K5 == ccm_param.t_sys_data_current.model_id)
  {
    // 设置当前Z位置
    gcode_cmd_coord_transform(false);            // 关闭坐标转换
    USER_SEND_INTERNAL_CMD_BUF("G92 Z0");
    USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z999");
    USER_SEND_INTERNAL_CMD_BUF("M400");
    USER_SEND_INTERNAL_CMD_BUF("G92 Z%f",  flash_param_t.poweroff_rec_z_max_value);
    USER_SEND_INTERNAL_CMD_BUF("G1 F600 Z%f", ccm_param.t_power_off.axis_real_position[Z_AXIS] + 20); // 移动Z
    gcode_cmd_coord_transform(true);            // 开启坐标转换
    gcode_cmd_all_axis_relative_mode(false);                 // 关闭相对模式
    gcode_cmd_eb_absolute_mode();
    // XY回原点
    gcode_cmd_xy_move_to_home();
  }
}

static void _feature_pow_rec_process_loop(void)
{
  if (_feature_pow_rec_status == 0)   //加热完成和Z轴向下完成
  {
#ifdef ENABLE_GUI_LVGL
    lvgl_custom_param.power_rec_status = CUSTOM_POWER_REC_STATUS_HOME_Z_DOWN;
#endif
    _feature_pow_rec_status = 1;
    ccm_param.t_sys.is_gcode_g28_done = false; //设置为归零
    stepper_quick_stop(); // 电机快速停止
    ccm_param.t_sys.is_gcode_m109_done = false; //设置为未加热
    _feature_pow_rec_prepare(); // 准备操作，预热，xy归零
  }
  else if (_feature_pow_rec_status == 1 && ccm_param.t_sys.is_gcode_m109_done && 0 == sg_grbl::planner_moves_planned())
  {
#ifdef ENABLE_GUI_LVGL
    lvgl_custom_param.power_rec_status = CUSTOM_POWER_REC_STATUS_HEATING;
#endif
    _feature_pow_rec_status = 2;
    ccm_param.t_sys.is_gcode_m109_done = false; //设置为未加热
    _feature_pow_rec_heating(); //设置目标温度，等待
  }
  else if (_feature_pow_rec_status == 2 && ccm_param.t_sys.is_gcode_m109_done)
  {
#ifdef ENABLE_GUI_LVGL
    lvgl_custom_param.power_rec_status = CUSTOM_POWER_REC_STATUS_REC_POS;
#endif
    _feature_pow_rec_status = 3;
    _feature_pow_rec_eb_set(); // 加热完成设置eb值
  }
  else if (_feature_pow_rec_status == 3 && 0 == sg_grbl::planner_moves_planned())     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    _feature_pow_rec_status = 4;
    _feature_pow_rec_xy_set(); // 移动xy
  }
  else if (_feature_pow_rec_status == 4 && 0 == sg_grbl::planner_moves_planned())     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    _feature_pow_rec_status = 5;
    _feature_pow_rec_z_set(); // 移动Z
  }
  else if (_feature_pow_rec_status == 5 && 0 == sg_grbl::planner_moves_planned())     //加热完成和XY轴归零完成和Z轴向下归零完成
  {
    feature_control_set_fan_speed(ccm_param.t_power_off.fan_speed);         //风扇速度

    if (F300TP == ccm_param.t_sys_data_current.model_id || M30 == ccm_param.t_sys_data_current.model_id)
    {
      SetFeedMultiply(100); //打印速度
    }
    else
    {
      SetFeedMultiply(ccm_param.t_power_off.feed_multiply); //打印速度
    }

    gcode::current_layer = ccm_param.t_power_off.current_layer;
    gcode::layer_count = ccm_param.t_power_off.layer_count;
    _feature_pow_rec_status = 0;
    _feature_pow_rec_is_finish = true;
#ifdef ENABLE_GUI_LVGL
    lvgl_custom_param.power_rec_status = CUSTOM_POWER_REC_STATUS_FINISH;
#endif

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      mix_eb_value_save = ccm_param.t_power_off.axis_real_position[E_AXIS] + ccm_param.t_power_off.axis_real_position[B_AXIS];
      mix_current_layer_save = -1;
    }
  }
}

void feature_pow_rec_process(void)
{
  if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
  {
    if (!print_status.is_printing)
    {
      feature_pow_cal_z_process();            // 校准z高度入口
    }

    if (_feature_pow_rec_is_start)
    {
      if (ccm_param.t_power_off.sd_position != 0)
      {
        _feature_pow_rec_process_loop();
      }
      else
      {
        _feature_pow_rec_is_finish = true;
      }

      if (_feature_pow_rec_is_finish)
      {
        ccm_param.t_sys.is_power_off_prepare_done = true; // UI界面更新标志位
        gcode_cmd_coord_transform(true);
        USER_SEND_INTERNAL_CMD_BUF("G21");
        gcode_cmd_all_axis_relative_mode(false);
        gcode_cmd_eb_absolute_mode();
        feature_print_control::power_rec_print_start(); //开始从文件读取内容继续去打印
        _feature_pow_rec_is_finish = false;
        _feature_pow_rec_is_start = 0;
        ccm_param.t_sys.is_power_off_rec_flag = false;
        ccm_param.t_sys.is_power_off_rec_data_chg = false;
        feature_pow_data_reset_flag();
      }
    }
    else
    {
      _feature_pow_rec_status = 0;
      _feature_pow_rec_is_finish = false;
    }
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_POWER_RECOVERY













