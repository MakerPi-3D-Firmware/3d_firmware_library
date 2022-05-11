#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

void gcode_cmd_g28(void)
{
  USER_SEND_INTERNAL_CMD_BUF("G28");
}

// M2003 S1:开启坐标转换  M2003 S0:关闭坐标转换
void gcode_cmd_coord_transform(bool is_on)
{
  USER_SEND_INTERNAL_CMD_BUF((char *)"M2003 S%d", (int)(is_on ? 1 : 0));
}

// M121:开启限位检测  M120:关闭限位检测
void gcode_cmd_endstop_check(bool is_on)
{
  USER_SEND_INTERNAL_CMD_BUF("%s", (char *)(is_on ? "M121" : "M120"));
}

void gcode_cmd_eb_absolute_mode(void)
{
  USER_SEND_INTERNAL_CMD_BUF("M82");
}

void gcode_cmd_eb_relative_mode(void)
{
  USER_SEND_INTERNAL_CMD_BUF("M83");
}

// G91:开启相对模式  G90:关闭相对模式
void gcode_cmd_all_axis_relative_mode(bool is_on)
{
  USER_SEND_INTERNAL_CMD_BUF("%s", (char *)(is_on ? "G91" : "G90"));
}

void gcode_cmd_z_down_to_bottom(void)
{
  gcode_cmd_endstop_check(true);
  gcode_cmd_coord_transform(false);
  USER_SEND_INTERNAL_CMD_BUF("G1 Z999 D1");          // 平台下降命令
  gcode_cmd_coord_transform(true);
  gcode_cmd_endstop_check(false);
}

void gcode_cmd_z_down(char code, float z_value)
{
  gcode_cmd_all_axis_relative_mode(true);
  gcode_cmd_endstop_check(true);
  gcode_cmd_coord_transform(false);
  USER_SEND_INTERNAL_CMD_BUF("G1 F600 X+0 Y+0 Z%c%f", (char)code, (float)z_value);
  gcode_cmd_coord_transform(true);
  gcode_cmd_endstop_check(false);
  gcode_cmd_all_axis_relative_mode(false);
}

void gcode_cmd_t0(bool is_process)
{
  if (is_process)
  {
    USER_SEND_INTERNAL_CMD_BUF("T0");
  }
  else
  {
    USER_SEND_INTERNAL_CMD_BUF("T0 S-1");
  }
}

void gcode_cmd_t1(bool is_process)
{
  if (is_process)
  {
    USER_SEND_INTERNAL_CMD_BUF("T1");
  }
  else
  {
    USER_SEND_INTERNAL_CMD_BUF("T1 S-1");
  }
}

void gcode_cmd_gui_wait_done(void)
{
  USER_SEND_INTERNAL_CMD_BUF("M2004 S1");
}

void gcode_cmd_eb_reset(void)
{
  USER_SEND_INTERNAL_CMD_BUF("G92 E0 B0");
}

static void _gcode_cmd_axis_move_to_home(const char axis_code, const int axis_index,
    const float home_speed)
{
  float home_pos_value = ccm_param.t_model.xyz_home_pos[axis_index];
  int home_dir = ccm_param.t_model.xyz_home_dir[axis_index];
  USER_SEND_INTERNAL_CMD_BUF("G92 %c%d", axis_code, (int)(home_pos_value - home_dir * 999));
  USER_SEND_INTERNAL_CMD_BUF("G1 F%d %c%d D1", (int)home_speed * 60, axis_code, (int)home_pos_value);
  USER_SEND_INTERNAL_CMD_BUF("G92 %c%d", axis_code, (int)home_pos_value);
}

static void _gcode_cmd_xy_move_to_home(int xy_slow_speed)
{
  float home_pos_x_value = ccm_param.t_model.xyz_home_pos[X_AXIS];
  float home_pos_y_value = ccm_param.t_model.xyz_home_pos[Y_AXIS];
  int home_x_dir = ccm_param.t_model.xyz_home_dir[X_AXIS];
  int home_y_dir = ccm_param.t_model.xyz_home_dir[Y_AXIS];
  gcode_cmd_coord_transform(false);    // 关闭坐标转换
  gcode_cmd_t0(false);
  // XY快速归零
  USER_SEND_INTERNAL_CMD_BUF("G92 X%d Y%d", (int)(home_pos_x_value - home_x_dir * 999),
                             (int)(home_pos_y_value - home_y_dir * 999));
  USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%d Y%d D1", (int)home_pos_x_value, (int)home_pos_y_value);

  if ((feature_print_control::idex_sys_is_extruder()) ||
      flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
      flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
  {
    // X单轴归零
    gcode_cmd_t0(false);
    _gcode_cmd_axis_move_to_home(axis_codes[X_AXIS], X_AXIS, xy_slow_speed);
    // X2单轴归零
    gcode_cmd_t1(false);
    _gcode_cmd_axis_move_to_home(axis_codes[X_AXIS], X2_AXIS, xy_slow_speed);
    gcode_cmd_t0(false);
  }
  else
  {
    // X单轴归零
    _gcode_cmd_axis_move_to_home(axis_codes[X_AXIS], X_AXIS, xy_slow_speed);
  }

  // Y单轴归零
  _gcode_cmd_axis_move_to_home(axis_codes[Y_AXIS], Y_AXIS, xy_slow_speed);
  gcode_cmd_coord_transform(true);   // 开启坐标转换
}


void gcode_cmd_xy_move_to_home(void)
{
  if (feature_print_control::idex_sys_is_extruder())
  {
    gcode_cmd_t0(false);
  }

  gcode_cmd_endstop_check(true);   // 开启限位检测
  _gcode_cmd_xy_move_to_home(20);  // 快速归零
  _gcode_cmd_xy_move_to_home(5);   // 慢速归零
  USER_SEND_INTERNAL_CMD_BUF("G92 X%.2f Y%.2f", ccm_param.t_model.xyz_home_pos[X_AXIS],
                             ccm_param.t_model.xyz_home_pos[Y_AXIS]);
  gcode_cmd_endstop_check(false);  // 关闭限位检测
}

void gcode_cmd_z_down_60mm_xy_to_home(void)
{
  if (!ccm_param.t_sys_data_current.IsLaser)
  {
    gcode_cmd_z_down('+', 60.0f);
  }

  gcode_cmd_xy_move_to_home();
}

void gcode_cmd_load_pre_do(void)
{
  // 消除EB高度差
  if ((P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id))
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      gcode_cmd_eb_reset();
      USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F1200 E40 B60");
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F1200 E40 B40");
    }
  }
}

void gcode_cmd_load_process(void)
{
  gcode_cmd_eb_reset();
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F300 E15 B15");
}

static void gcode_cmd_unload_pre_do_p3_pro(void)
{
  gcode_cmd_eb_reset();
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F200 E30 B30");

  if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
  {
    gcode_cmd_eb_reset();
    USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F3600 E-70 B-70");
    gcode_cmd_eb_reset();
    USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F2400 E-70 B-70");
  }
  else if (feature_print_control::idex_sys_is_extruder())
  {
    gcode_cmd_eb_reset();
    USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F8400 E-15 B-15");
    gcode_cmd_eb_reset();
    USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F2500 E-70 B-70");
  }
}

void gcode_cmd_unload_pre_do(void)
{
  if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
  {
    gcode_cmd_unload_pre_do_p3_pro();
  }
  else
  {
    gcode_cmd_eb_reset();
    USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F200 E30 B30");

    if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
    {
      if (K5 == ccm_param.t_sys_data_current.model_id)
      {
        USER_SEND_INTERNAL_CMD_BUF("G1 F300 E50 B50");
        gcode_cmd_eb_reset();
        USER_SEND_INTERNAL_CMD_BUF("G1 F1800 E-40 B-30");
        gcode_cmd_eb_reset();
        USER_SEND_INTERNAL_CMD_BUF("G1 F800 E-10 B-10");
        gcode_cmd_eb_reset();
      }
      else
      {
        gcode_cmd_eb_reset();
        USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F8400 E-15 B-5");
        gcode_cmd_eb_reset();
        USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F2500 E-65 B-70");
      }
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("G1 F200 E50 B50");
    }
  }
}

void gcode_cmd_unload_process(void)
{
  USER_SEND_INTERNAL_CMD_BUF((char *)"G1 F500 E-10 B-10");
}

#ifdef __cplusplus
} //extern "C" {
#endif

