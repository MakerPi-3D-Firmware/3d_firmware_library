#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FILAMENT_CHANGE_IN 0
#define FILAMENT_CHANGE_OUT 1

#define FILAMENTCHANGE_FIRSTRETRACT -50  //第二步退料未接收到'E'或'B'命令时用
#define FILAMENTCHANGE_FINALRETRACT -80  //第三步退料未接收到'L'命令时用
#define FILAMENTCHANGE_PREPARE_RETRACT -80
#define EB_DIFF_MM   (int)(159.7-116.7) //消除EB电机高度差
//#define FILAMENTCHANGE_LENGTH_MM 150
#define FIRST_LOAD_MM 80
#define FINAL_LOAD_MM 155 //返回打印前，最后一次改变出丝量

static bool IsMidWayChangeMaterial = false;

int IsMidWayChangeMat(void)
{
  return (IsMidWayChangeMaterial ? 1 : 0);
}

void setMidWayChangeMat(bool value)
{
  IsMidWayChangeMaterial = value;
}

static int IsConfirmLoadFilament = 0;
int GUI_ClickedLoadFilament(void)
{
  return IsConfirmLoadFilament;
}

void setConfirmLoadFilament(int _IsConfirmLoadFilament)
{
  IsConfirmLoadFilament = _IsConfirmLoadFilament;
}

extern unsigned long stepper_inactive_time;
extern volatile float mix_eb_value_save;
extern volatile int mix_current_layer_save;
#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  volatile uint8_t m600_status = 0;


  static volatile unsigned char mid_way_chg_mat_print_status = 0; /*!< 中途换料，打印状态：0为没打印状态，1打印状态，2为暂停状态 */
  static volatile float m600_current_real_position[MAX_NUM_AXIS] = {0.0f};
  static volatile float m600_current_position[MAX_NUM_AXIS] = {0.0f};
  static volatile float m600_target[MAX_NUM_AXIS] = {0.0f};
  static volatile float m600_lastpos[MAX_NUM_AXIS] = {0.0f};
  static volatile uint8_t active_extruder_bak = 0;

  static void _m600_save_pos(void)
  {
    active_extruder_bak = gcode::active_extruder;

    for (int8_t i = 0; i < MAX_NUM_AXIS; i++)
    {
      // 执行中途换料，需要执行暂停操作
      if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
      {
        if (i < XYZ_NUM_AXIS)
        {
          m600_target[i] = sg_grbl::plan_get_xyz_real(i);
          m600_lastpos[i] = m600_target[i];
          m600_current_real_position[i] = m600_target[i];
          m600_current_position[i] = ccm_param.grbl_current_position[i];
        }
        else
        {
          m600_target[i] = ccm_param.grbl_current_position[i];
          m600_lastpos[i] = m600_target[i];
          m600_current_real_position[i] = m600_target[i];
          m600_current_position[i] = m600_target[i];
        }
      }
      else
      {
        m600_target[i] = ccm_param.grbl_current_position[i];
        m600_lastpos[i] = m600_target[i];
        m600_current_real_position[i] = m600_target[i];
        m600_current_position[i] = m600_target[i];
      }
    }
  }

  static void _m600_z_away_from_platform(void)
  {
    float m600_z_feedrate = homing_feedrate[Z_AXIS];
    sg_grbl::planner_set_position(m600_current_real_position);
    m600_target[Z_AXIS] += 60.0f;

    // Maximum travel limit
    if (m600_target[Z_AXIS] > ccm_param.t_model.xyz_max_pos[Z_AXIS])
      m600_target[Z_AXIS] = ccm_param.t_model.xyz_max_pos[Z_AXIS];

    // Travel over 350 limit travel speed
    if (ccm_param.t_model.xyz_max_pos[Z_AXIS] > 350)
      m600_z_feedrate = 350.0f;

    process_buffer_line_normal(m600_target, m600_z_feedrate / 60.0f);
    sg_grbl::st_synchronize();// wait for finish step buffer
  }

  static void _m600_z_add_before_xy_home(void)
  {
#ifdef FILAMENTCHANGE_ZADD
    target[Z_AXIS] += FILAMENTCHANGE_ZADD ;
#endif

    if (m600_target[Z_AXIS] >= (float)ccm_param.t_model.xyz_max_pos[Z_AXIS])
      m600_target[Z_AXIS] = (float)ccm_param.t_model.xyz_max_pos[Z_AXIS];

    process_buffer_line_normal(m600_target, homing_feedrate[Z_AXIS] / 60);
  }

  static void _m600_home_axis(int axis, const float feedrate)
  {
    sg_grbl::st_enable_endstops(true);
    sg_grbl::planner_set_position(m600_target);
    m600_target[(int)axis] = 1.5f \
                             * ccm_param.t_model.xyz_max_length[(int)axis] \
                             * ccm_param.t_model.xyz_home_dir[(int)axis];
    process_buffer_line_normal(m600_target, feedrate / 60);
    sg_grbl::st_synchronize();
    sg_grbl::st_enable_endstops(false);
  }

  static void _m600_xy_home(void)
  {
    float m600_xy_feedrate = homing_feedrate[X_AXIS];

    if (homing_feedrate[Y_AXIS] < m600_xy_feedrate)
      m600_xy_feedrate = homing_feedrate[Y_AXIS];

    // Home XY
    sg_grbl::planner_set_position(m600_target);
    _m600_home_axis(X_AXIS, m600_xy_feedrate);

    if (feature_print_control::idex_sys_is_extruder())
    {
      _m600_home_axis(X2_AXIS, m600_xy_feedrate);
    }

    _m600_home_axis(Y_AXIS, m600_xy_feedrate);
    {
      // Set XY position
      m600_target[X_AXIS] = ccm_param.t_model.xyz_home_pos[X_AXIS] ;

      if (feature_print_control::idex_sys_is_extruder())
      {
        m600_target[X2_AXIS] = ccm_param.t_model.xyz_home_pos[X2_AXIS] ;
      }

      m600_target[Y_AXIS] = ccm_param.t_model.xyz_home_pos[Y_AXIS] ;
      sg_grbl::planner_set_position(m600_target);
      process_buffer_line_normal(m600_target, m600_xy_feedrate / 60);
    }
    sg_grbl::st_synchronize();
  }

  static void _m600_filament_set_e_value(const float value, const int axis)
  {
    m600_target[axis] = value;
  }

  static void _m600_filament_set_first(const int type, const char code, const int axis)
  {
    if (parseGcodeBufHandle.codeSeen(code))
    {
      _m600_filament_set_e_value(parseGcodeBufHandle.codeValue(), axis);
    }
    else
    {
      if (type == FILAMENT_CHANGE_OUT)
      {
#ifdef FILAMENTCHANGE_FIRSTRETRACT

        if ((K5 == ccm_param.t_sys_data_current.model_id))
        {
          _m600_filament_set_e_value(FILAMENTCHANGE_FIRSTRETRACT / 2, axis);
        }
        else if (P3_Pro == ccm_param.t_sys_data_current.model_id ||
                 P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
                 K600 == ccm_param.t_sys_data_current.model_id)
        {
          if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
          {
            _m600_filament_set_e_value(FILAMENTCHANGE_FIRSTRETRACT - 50, axis);
          }
          else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
          {
            _m600_filament_set_e_value(-50, axis);
          }
        }
        else
        {
          _m600_filament_set_e_value(FILAMENTCHANGE_FIRSTRETRACT, axis);
        }

#endif
      }
      else if (type == FILAMENT_CHANGE_IN)
      {
        if ((K5 == ccm_param.t_sys_data_current.model_id))
        {
          _m600_filament_set_e_value(FIRST_LOAD_MM / 2, axis);
        }
        else if (P3_Pro == ccm_param.t_sys_data_current.model_id ||
                 P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
                 K600 == ccm_param.t_sys_data_current.model_id)
        {
          if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
          {
            _m600_filament_set_e_value(FIRST_LOAD_MM, axis);
          }
          else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
          {
            _m600_filament_set_e_value(50.0f, axis);
          }
        }
        else
        {
          _m600_filament_set_e_value(FIRST_LOAD_MM, axis);
        }
      }
    }
  }

  static void _m600_filament_set_final(const int type, const int axis)
  {
    if (parseGcodeBufHandle.codeSeen('L'))
    {
      _m600_filament_set_e_value(parseGcodeBufHandle.codeValue(), axis);
    }
    else
    {
      if (type == FILAMENT_CHANGE_OUT)
      {
#ifdef FILAMENTCHANGE_FIRSTRETRACT

        if ((K5 == ccm_param.t_sys_data_current.model_id))
        {
          _m600_filament_set_e_value(FILAMENTCHANGE_FINALRETRACT / 2 - 10.0f, axis);
        }
        else if (P3_Pro == ccm_param.t_sys_data_current.model_id ||
                 P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
                 K600 == ccm_param.t_sys_data_current.model_id)
        {
          if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
          {
            _m600_filament_set_e_value(FILAMENTCHANGE_FINALRETRACT - 30.0f, axis);
          }
          else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
          {
            _m600_filament_set_e_value(-60.0f, axis);
          }
        }
        else
        {
          _m600_filament_set_e_value(FILAMENTCHANGE_FINALRETRACT - 10.0f, axis);
        }

#endif
      }
      else if (type == FILAMENT_CHANGE_IN)
      {
        if ((K5 == ccm_param.t_sys_data_current.model_id))
        {
          _m600_filament_set_e_value(FINAL_LOAD_MM / 2, axis);
        }
        else if (P3_Pro == ccm_param.t_sys_data_current.model_id ||
                 P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
                 K600 == ccm_param.t_sys_data_current.model_id)
        {
          if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
          {
            _m600_filament_set_e_value(FINAL_LOAD_MM - 85.0f, axis);
          }
          else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
          {
            _m600_filament_set_e_value(60.0f, axis);
          }
          else
          {
          }
        }
        else
        {
          _m600_filament_set_e_value(FINAL_LOAD_MM, axis);
        }
      }
    }
  }

  static void _m600_fast(const int type, const float e_feedrate, const float b_feedrate)
  {
    float feedrate = e_feedrate;
    m600_target[E_AXIS] = 0.0f;
    m600_target[B_AXIS] = 0.0f;
    sg_grbl::planner_set_axis_position(m600_target[E_AXIS], E_AXIS);
    sg_grbl::planner_set_axis_position(m600_target[B_AXIS], B_AXIS);

    if (feature_print_control::idex_sys_is_extruder())
    {
      if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        _m600_filament_set_first(type, 'E', E_AXIS);
        _m600_filament_set_first(type, 'B', B_AXIS);
      }
      else if (feature_print_control::idex_sys_is_normal())
      {
        if (gcode::active_extruder == 0)
        {
          _m600_filament_set_first(type, 'E', E_AXIS);
        }
        else
        {
          _m600_filament_set_first(type, 'B', B_AXIS);
        }
      }
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      feedrate = e_feedrate;
      _m600_filament_set_first(type, 'E', E_AXIS);
      _m600_filament_set_first(type, 'B', B_AXIS);
    }
    else
    {
      _m600_filament_set_first(type, 'E', E_AXIS);

      if (ccm_param.t_sys_data_current.enable_color_mixing)
      {
        feedrate = b_feedrate;
        _m600_filament_set_first(type, 'B', B_AXIS);
      }
    }

    process_buffer_line_normal(m600_target, feedrate / 60);
    sg_grbl::st_synchronize();
  }

  static void _m600_slow(const int type, const float e_feedrate, const float b_feedrate)
  {
    float feedrate = e_feedrate;
    m600_target[E_AXIS] = 0.0f;
    m600_target[B_AXIS] = 0.0f;
    sg_grbl::planner_set_axis_position(m600_target[E_AXIS], E_AXIS);
    sg_grbl::planner_set_axis_position(m600_target[B_AXIS], B_AXIS);

    if (feature_print_control::idex_sys_is_extruder())
    {
      if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        _m600_filament_set_final(type, E_AXIS);
        _m600_filament_set_final(type, B_AXIS);
      }
      else if (feature_print_control::idex_sys_is_normal())
      {
        if (gcode::active_extruder == 0)
        {
          _m600_filament_set_final(type, E_AXIS);
        }
        else
        {
          _m600_filament_set_final(type, B_AXIS);
        }
      }
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      feedrate = e_feedrate;
      _m600_filament_set_final(type, E_AXIS);
      _m600_filament_set_final(type, B_AXIS);
    }
    else
    {
      _m600_filament_set_final(type, E_AXIS);

      if (ccm_param.t_sys_data_current.enable_color_mixing)
      {
        feedrate = b_feedrate;
        _m600_filament_set_final(type, B_AXIS);
      }
    }

    process_buffer_line_normal(m600_target, feedrate / 60);
    sg_grbl::st_synchronize();
  }

  static void _m600_adjust_eb_diff(const int type, const float eb_feedrate)
  {
    if (ccm_param.t_sys_data_current.enable_color_mixing)
    {
      if (FILAMENT_CHANGE_IN == type)
      {
        if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
        {
          m600_target[E_AXIS] += (-1) * FILAMENTCHANGE_PREPARE_RETRACT;
          m600_target[B_AXIS] += (-1) * FILAMENTCHANGE_PREPARE_RETRACT + 45;
          process_buffer_line_normal(m600_target, eb_feedrate / 60);
          sg_grbl::st_synchronize();
        }
      }
    }
  }

  static void _m600_xy_home_and_unload(void)
  {
    _m600_z_add_before_xy_home();
    _m600_xy_home();

    //第一步 先进料3cm，让料融化
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      m600_target[E_AXIS] += 30;
      m600_target[B_AXIS] += 30;
      process_buffer_line_normal(m600_target, 250 / 60);
      sg_grbl::st_synchronize();
    }

    //第二步(1) 退料 消除E\B 电机高度差异
    _m600_adjust_eb_diff(FILAMENT_CHANGE_OUT, 8400);

    if (P3_Pro == ccm_param.t_sys_data_current.model_id ||
        P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
        K600 == ccm_param.t_sys_data_current.model_id)
    {
      //第二步(2)，退料 FILAMENTCHANGE_FIRSTRETRACT
      _m600_fast(FILAMENT_CHANGE_OUT, 6000, 6000);
      //第三步，退料 FILAMENTCHANGE_FINALRETRACT
      _m600_slow(FILAMENT_CHANGE_OUT, 600, 600);
    }
    else
    {
      //第二步(2)，退料 FILAMENTCHANGE_FIRSTRETRACT
      _m600_fast(FILAMENT_CHANGE_OUT, 8400, 6000);
      //第三步，退料 FILAMENTCHANGE_FINALRETRACT
      _m600_slow(FILAMENT_CHANGE_OUT, 300, 300);
    }
  }

  static void _m600_beep_notice(void)
  {
    static uint8_t LCDBuzzCount = 5;

    while (!GUI_ClickedLoadFilament())
    {
      if (LCDBuzzCount > 0)
      {
        user_buzzer_buzz(100);
        LCDBuzzCount--;
      }

      (void)OS_DELAY(100);
    }

    LCDBuzzCount = 5;
  }

  static void _m600_move_to_lastpos_xy(float xy_feedrate)
  {
    if (feature_print_control::idex_sys_is_extruder())
    {
      if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        m600_target[X_AXIS] = m600_lastpos[X_AXIS];
        m600_target[X2_AXIS] = m600_lastpos[X2_AXIS];
      }
      else
      {
        if (gcode::active_extruder == 1)
        {
          m600_target[X2_AXIS] = m600_lastpos[X2_AXIS];
        }
        else if (gcode::active_extruder == 0)
        {
          m600_target[X_AXIS] = m600_lastpos[X_AXIS];
        }
      }
    }
    else
    {
      m600_target[X_AXIS] = m600_lastpos[X_AXIS];
    }

    m600_target[Y_AXIS] = m600_lastpos[Y_AXIS];
    process_buffer_line_normal(m600_target, xy_feedrate / 60.0f);
    sg_grbl::st_synchronize();
  }

  static void _m600_move_to_lastpos_z(float z_feedrate)
  {
    // 平台下降到最低位置，限位开关关闭，平台上升，限位开关开启，避免平台上不去
    sg_grbl::st_enable_endstops(false);
    m600_target[Z_AXIS] = m600_lastpos[Z_AXIS];
    process_buffer_line_normal(m600_target, z_feedrate / 60.0f);
    sg_grbl::st_synchronize();
  }

  static void _m600_compensate_eb(void)
  {
    ccm_param.grbl_destination[E_AXIS] = m600_lastpos[E_AXIS];
    ccm_param.grbl_destination[B_AXIS] = m600_lastpos[B_AXIS];

    if (feature_print_control::idex_sys_is_extruder())
    {
      ccm_param.grbl_destination[E_AXIS] -= 3; //改善中途换料断层现象
      ccm_param.grbl_destination[B_AXIS] -= 3; //改善中途换料断层现象
      ccm_param.grbl_current_position[E_AXIS] = ccm_param.grbl_destination[E_AXIS];
      ccm_param.grbl_current_position[B_AXIS] = ccm_param.grbl_destination[B_AXIS];
      sg_grbl::planner_set_axis_position(ccm_param.grbl_current_position[E_AXIS], E_AXIS);
      sg_grbl::planner_set_axis_position(ccm_param.grbl_current_position[B_AXIS], B_AXIS);
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      mix_eb_value_save = m600_lastpos[E_AXIS] + m600_lastpos[B_AXIS];
      mix_current_layer_save = -1;
      ccm_param.grbl_destination[E_AXIS] -= 3.0f; //改善中途换料断层现象
      ccm_param.grbl_destination[B_AXIS] -= 3.0f; //改善中途换料断层现象
      ccm_param.grbl_current_position[E_AXIS] = ccm_param.grbl_destination[E_AXIS];
      ccm_param.grbl_current_position[B_AXIS] = ccm_param.grbl_destination[B_AXIS];
      sg_grbl::planner_set_axis_position(ccm_param.grbl_current_position[E_AXIS], E_AXIS);
      sg_grbl::planner_set_axis_position(ccm_param.grbl_current_position[B_AXIS], B_AXIS);
    }
    else
    {
      ccm_param.grbl_destination[E_AXIS] -= 3; //改善中途换料断层现象
      ccm_param.grbl_current_position[E_AXIS] = ccm_param.grbl_destination[E_AXIS];
      ccm_param.grbl_current_position[B_AXIS] = ccm_param.grbl_destination[B_AXIS];
      sg_grbl::planner_set_axis_position(ccm_param.grbl_current_position[E_AXIS], E_AXIS);
      sg_grbl::planner_set_axis_position(ccm_param.grbl_current_position[B_AXIS], B_AXIS);
    }
  }

  static void _m600_return_normal(void)
  {
    //第一步：进料 消除E\B 电机高度差异
    _m600_adjust_eb_diff(FILAMENT_CHANGE_IN, 300);

    if (P3_Pro == ccm_param.t_sys_data_current.model_id ||
        P3_Pro_Mini == ccm_param.t_sys_data_current.model_id ||
        K600 == ccm_param.t_sys_data_current.model_id)
    {
      //第二步：快速进料 使料到达加热管
      _m600_fast(FILAMENT_CHANGE_IN, 1200, 1200);
      //第三步：慢速进料 FILAMENTCHANGE_FIRSTRETRACT
      _m600_slow(FILAMENT_CHANGE_IN, 600, 600);
    }
    else
    {
      _m600_fast(FILAMENT_CHANGE_IN, 300, 330);
      _m600_slow(FILAMENT_CHANGE_IN, 140, 140);
    }

    //第四步：还原xy
    _m600_move_to_lastpos_xy(2400);
    // 第五步：还原Z
    _m600_move_to_lastpos_z(300);
    // 补偿eb值
    _m600_compensate_eb();
  }

  static void _m600_process(void)
  {
    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      sg_grbl::plan_set_process_auto_bed_level_status(false);
    }

    stepper_inactive_time = 10 * 60  * 1000; //电机解锁时间设置10分钟，避免电机解锁导致xy偏移
    _m600_save_pos();
    sg_grbl::st_enable_endstops(true);
    //Z轴向下移动60
    m600_status = M600_STATUS_Z_AWAY_PLATFORM;
    _m600_z_away_from_platform();
    m600_status = M600_STATUS_XY_HOME_UNLOAD;
    _m600_xy_home_and_unload();
    m600_status = M600_STATUS_BEEP_NOTICE;
    _m600_beep_notice();
    _m600_xy_home();//返回零点，防止换料时移动挤出头
    m600_status = M600_STATUS_RESTORE_POS;
    _m600_return_normal();
    // finish
    m600_status = M600_STATUS_FINISH;

    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      sg_grbl::plan_set_process_auto_bed_level_status(true);
    }

    sg_grbl::planner_set_position(m600_current_position);
    gcode::active_extruder = active_extruder_bak;
    IsMidWayChangeMaterial = false; // 重置中途换料状态
    IsConfirmLoadFilament = 0;  // 重置中途换料确认进丝按钮
  }

  void m600_process(void)
  {
    //正常下的中途换料
    sg_grbl::st_synchronize();
    _m600_process();
    sg_grbl::st_synchronize();

    if (print_status.is_mid_chg_filament)
    {
      print_status.is_mid_chg_filament = false;
    }
  }
}






