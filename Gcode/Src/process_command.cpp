#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#include "../../Grbl/Inc/temperature_pid_temp.h"
#ifndef ENABLE_GUI_LVGL
#include "../../Feature/Inc/Alter.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================
//=============================public variables=============================
//===========================================================================
extern void process_m109(void);


float homing_feedrate[MAX_NUM_AXIS] = HOMING_FEEDRATE;
volatile uint8_t gcode_cmd_type;
int feedmultiply = 100; //100->1 200->2
int extrudemultiply = 100; //100->1 200->2

extern osSemaphoreId ReadUdiskSemHandle;

//===========================================================================
//=============================private variables=============================
//===========================================================================

uint32_t SDPos = 0;
/*static*/
float add_homeing[XYZ_NUM_AXIS] = {0.0f};



static void plan_st_synchronize(void)
{
  for (int i = 0; i < XYZ_NUM_AXIS; i++)
  {
    // 获取st pos值会异常，限定值范围，避免异常
    if (ccm_param.t_model.xyz_move_max_pos[i] < sg_grbl::st_get_position_length(i))
    {
      ccm_param.grbl_current_position[i] = ccm_param.t_model.xyz_move_max_pos[i];
      ccm_param.grbl_destination[i] = ccm_param.t_model.xyz_move_max_pos[i];
    }
    else if (0 > sg_grbl::st_get_position_length(i))
    {
      ccm_param.grbl_current_position[i] = 0;
      ccm_param.grbl_destination[i] = 0;
    }
    else
    {
      ccm_param.grbl_current_position[i] = sg_grbl::st_get_position_length(i);
      ccm_param.grbl_destination[i] = sg_grbl::st_get_position_length(i);
    }
  }

  sg_grbl::planner_set_position(ccm_param.grbl_current_position);
}

void stepper_quick_stop(void)
{
  // 清空运动队列
  sg_grbl::st_quick_stop();
  // 清空指令队列
  user_clear_queue_str();

  // 二次确认运动队列
  if (sg_grbl::planner_moves_planned() > 1)
    sg_grbl::st_quick_stop();

  // 延时让其他任务执行
  OS_DELAY(100);

  if (BED_LEVEL_PRESSURE_SENSOR != ccm_param.t_sys_data_current.enable_bed_level)
  {
    plan_st_synchronize();
  }
}

volatile float feedrate = 1500.0, next_feedrate;

static unsigned long max_inactive_time = 0;
unsigned long previous_xTaskGetTickCount_cmd = 0;
unsigned long stepper_inactive_time = DEFAULT_STEPPER_DEACTIVE_TIME * 1000L;
uint8_t tmp_extruder;


void process_buffer_line_basic(volatile float *position, const float feed_rate, const int feed_multiply,
                               const int extruder_multiply, const unsigned long sd_pos, bool is_sync_z)
{
  ccm_param.running_status.target_bed_temp = sg_grbl::temperature_get_bed_target();
  ccm_param.running_status.target_extruder0_temp = sg_grbl::temperature_get_extruder_target(0);
  ccm_param.running_status.target_extruder1_temp = sg_grbl::temperature_get_extruder_target(1);
  ccm_param.running_status.fan_speed = feature_control_get_fan_speed();
  ccm_param.running_status.active_extruder = gcode::active_extruder;
  ccm_param.running_status.feed_rate = feed_rate;
  ccm_param.running_status.feed_multiply = feed_multiply;
  ccm_param.running_status.extruder_multiply = extruder_multiply;
  ccm_param.running_status.sd_position = sd_pos;
  ccm_param.running_status.is_serial = 0;
  ccm_param.running_status.z_real_change_value = 0.0f;
  ccm_param.running_status.layer_count = gcode::layer_count;
  ccm_param.running_status.current_layer = gcode::current_layer;
  ccm_param.running_status.print_time_save = ccm_param.t_gui.printed_time_sec;//(long)feature_print_control::print_time();
  ccm_param.running_status.enable_color_buf = ccm_param.t_sys.enable_color_buf;

  for (int i = 0; i < MAX_NUM_AXIS ; i++)
  {
    ccm_param.running_status.axis_position[i] = position[i];
  }

  if (is_sync_z)
  {
    ccm_param.running_status.axis_position[Z2_AXIS] = position[Z_AXIS];
  }

  sg_grbl::planner_buffer_line((planner_running_status_t *)&ccm_param.running_status, is_sync_z);
}

void process_buffer_line(volatile float *position, const float feed_rate, const int feed_multiply,
                         const int extruder_multiply, const unsigned long sd_pos)
{
  process_buffer_line_basic(position, feed_rate, feed_multiply,
                            extruder_multiply, sd_pos, true);
}

void process_buffer_line_normal(volatile float *position, const float feed_rate)
{
  process_buffer_line_basic(position, feed_rate, 100, 100, 0, true);
}

uint8_t M140_HeatingHotendFinish();

void manage_synchronize(void);

uint8_t M140_HeatingHotendFinish()
{
  if (sg_grbl::temperature_is_extruder_heating(tmp_extruder))
    return 0 ;
  else
    return 1 ;
}

int GetFeedRate(void)
{
  return (int)feedrate;
}

void SetFeedRate(float Value)
{
  feedrate = Value;
}

int GetFeedMultiply(void)
{
  return feedmultiply;
}

void SetFeedMultiply(int FeedMultiplyValue)
{
  feedmultiply = FeedMultiplyValue;
}

float GetCurrentPosition(uint8_t AXIS)
{
  if (AXIS < ccm_param.t_sys.axis_num)
  {
    return ccm_param.grbl_current_position[AXIS];
  }
  else
    return 0;
}

void SetCurrentPosition(uint8_t AXIS, float AxisPos)
{
  if (AXIS < ccm_param.t_sys.axis_num)
  {
    ccm_param.grbl_current_position[AXIS] = AxisPos;
  }
}

void kill()
{
  taskENTER_CRITICAL();
  sg_grbl::temperature_disable_heater();

  for (int i = 0; i < MAX_NUM_AXIS; i++)
  {
    user_motor_axis_enable(i, false);
  }

  taskEXIT_CRITICAL();
  USER_ErrLog("Printer halted. kill() called!");

  for (;;)
  {
    /* Intentionally left empty */
  } // Wait for reset
}

void manage_inactivity(void)
{
  if ((xTaskGetTickCount() - previous_xTaskGetTickCount_cmd) >  max_inactive_time)
  {
    if (max_inactive_time)
    {
      kill();
    }
  }

  if (stepper_inactive_time)
  {
    if ((xTaskGetTickCount() - previous_xTaskGetTickCount_cmd) >  stepper_inactive_time)
    {
      if (!sg_grbl::planner_blocks_queued())
      {
        user_motor_axis_enable(X_AXIS, false);

        if (feature_print_control::idex_sys_is_extruder())
          user_motor_axis_enable(X2_AXIS, false);

        user_motor_axis_enable(Y_AXIS, false);
        //        if(ccm_param.t_sys_data_current.pic_id != 2)//非日本机器才松开z轴
        //          disable_z();//Z? -yanghai 20140605
        user_motor_axis_enable(E_AXIS, false);
        user_motor_axis_enable(B_AXIS, false);
      }
    }
  }

  static uint32_t stepper_timeout = xTaskGetTickCount() + 500;

  if (xTaskGetTickCount() >  stepper_timeout)
  {
    // 以下机型Z轴一直使能
    if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
    {
      user_motor_axis_enable(Z_AXIS, true);
      user_motor_axis_enable(Z2_AXIS, true);
    }
    else if (K600 == ccm_param.t_sys_data_current.model_id || F400TP == ccm_param.t_sys_data_current.model_id)
    {
      user_motor_axis_enable(Z_AXIS, true);
    }
    else if (F300TP == ccm_param.t_sys_data_current.model_id)
    {
      user_motor_axis_enable(Z_AXIS, true);
    }

    stepper_timeout = xTaskGetTickCount() + 500;
  }
}

static void clamp_to_software_endstops_basic(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface)
{
  float min_offset = 5.0f;
  float max_offset = 5.0f;

  if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
  {
    min_offset = 20.0f;
    max_offset = 5.0f;
  }

  if (target[X_AXIS] < ccm_param.t_model.xyz_min_pos[X_AXIS] + add_homeing[X_AXIS] - min_offset) IsPopWarningInterface = ERR_X_MIN_LIMIT;

  if (target[Y_AXIS] < ccm_param.t_model.xyz_min_pos[Y_AXIS] + add_homeing[Y_AXIS] - min_offset) IsPopWarningInterface = ERR_Y_MIN_LIMIT;

  if (target[Z_AXIS] < ccm_param.t_model.xyz_min_pos[Z_AXIS] + add_homeing[Z_AXIS] - min_offset) IsPopWarningInterface = ERR_Z_MIN_LIMIT;

  if (target[X_AXIS] > ccm_param.t_model.xyz_max_pos[X_AXIS] + add_homeing[X_AXIS] + max_offset) IsPopWarningInterface = ERR_X_MAX_LIMIT;

  if (target[Y_AXIS] > ccm_param.t_model.xyz_max_pos[Y_AXIS] + add_homeing[Y_AXIS] + max_offset) IsPopWarningInterface = ERR_Y_MAX_LIMIT;

  if (target[Z_AXIS] > ccm_param.t_model.xyz_max_pos[Z_AXIS] + add_homeing[Z_AXIS] + max_offset) IsPopWarningInterface = ERR_Z_MAX_LIMIT;

  if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
  {
    // idex、激光头会使用2号头
    if (feature_print_control::idex_sys_is_extruder() ||
        flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      if (target[X2_AXIS] < ccm_param.t_model.xyz_min_pos[X2_AXIS] + add_homeing[X2_AXIS] - min_offset) IsPopWarningInterface = ERR_X_MIN_LIMIT;

      //      if (target[X2_AXIS] > ccm_param.t_model.xyz_max_pos[X2_AXIS] + add_homeing[X2_AXIS] + max_offset) IsPopWarningInterface = XMaxLimitWarning;

      if (target[Z2_AXIS] < ccm_param.t_model.xyz_min_pos[Z2_AXIS] + add_homeing[Z2_AXIS] - min_offset) IsPopWarningInterface = ERR_Z_MIN_LIMIT;

      if (target[Z2_AXIS] > ccm_param.t_model.xyz_max_pos[Z2_AXIS] + add_homeing[Z2_AXIS] + max_offset) IsPopWarningInterface = ERR_Z_MAX_LIMIT;
    }
  }
}

void clamp_to_software_endstops(volatile float (&target)[MAX_NUM_AXIS])
{
  int IsPopWarningInterface = -1;
  clamp_to_software_endstops_basic(target, IsPopWarningInterface);

  if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
  {
    if (feature_print_control::idex_sys_is_extruder())
    {
      gcode::clamp_to_software_endstops_dual_idex(target, IsPopWarningInterface);
    }
  }
  else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
  {
    gcode::clamp_to_software_endstops_laser(target, IsPopWarningInterface);
  }
  else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
  {
    gcode::clamp_to_software_endstops_drug(target, IsPopWarningInterface);
  }
  else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
  {
    gcode::clamp_to_software_endstops_mix(target, IsPopWarningInterface);
  }
  else
  {
    if (!ccm_param.t_sys.enable_z_max_limit) //没有下限位开关
    {
      if (target[Z_AXIS] > ccm_param.t_model.xyz_max_pos[Z_AXIS] + add_homeing[Z_AXIS])
      {
        // 个别现象，+8是防止校准值小于默认值太多，引起最大行程警报.
        if (target[Z_AXIS] > ccm_param.t_model.xyz_max_pos[Z_AXIS] + add_homeing[Z_AXIS] + 8)
          IsPopWarningInterface = 6;

        target[Z_AXIS] = ccm_param.t_model.xyz_max_pos[Z_AXIS] + add_homeing[Z_AXIS];
      }
    }
    else if (print_status.is_printing) //有下限位开关，在打印的时候才报错
    {
      if (target[Z_AXIS] > ccm_param.t_model.xyz_max_pos[Z_AXIS] + add_homeing[Z_AXIS]) IsPopWarningInterface = ERR_Z_MAX_LIMIT;
    }

    if (IsPopWarningInterface)
    {
      target[X_AXIS] = ccm_param.grbl_current_position[X_AXIS];
      target[Y_AXIS] = ccm_param.grbl_current_position[Y_AXIS];
      target[Z_AXIS] = ccm_param.grbl_current_position[Z_AXIS];
    }

#ifndef ENABLE_GUI_LVGL
    gui_disp_error_info_show(IsPopWarningInterface);
#else
    custom_disp_error_info_show(IsPopWarningInterface);
#endif
  }
}

float feedrate_bak = 0.0f;


void process_commands(const uint8_t gcodeType, const uint32_t file_pos, const char *gcodeStr, const long layer_count, const long current_layer)
{
  static bool is_pause_set_pow_data = false;
  parseGcodeBufHandle.setGcodeBuffer(gcodeStr);
  gcodeCommand.process(gcodeType);

  if (gcodeType == GCODE_TYPE_FILE)
  {
    gcode::layer_count = layer_count;
    gcode::current_layer = current_layer;
    SDPos = file_pos;
    gcodeCommand.set_curr_file_pos(file_pos);
  }

  if (print_status.is_printing  && !print_status.is_stop_printing &&
      !print_status.is_pause_printing && !print_status.is_mid_chg_filament) //若有打印发送信号量从而从SD卡或U盘读取下一条gcode命令
  {
    is_pause_set_pow_data = false;
    (void)osSemaphoreRelease(ReadUdiskSemHandle);
  }
  else if (print_status.is_stop_printing || print_status.is_pause_printing ||
           print_status.is_mid_chg_filament)
  {
    sg_grbl::st_synchronize();

    if (print_status.is_pause_printing && !is_pause_set_pow_data)
    {
      osDelay(200);

      if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
      {
        flash_poweroff_data_pause_set();
      }

      is_pause_set_pow_data = true;
    }
  }
  else
  {
    is_pause_set_pow_data = false;
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

GcodeCommand::GcodeCommand()
{
}

void GcodeCommand::clearToSend(void)
{
  previous_xTaskGetTickCount_cmd = xTaskGetTickCount();

  if (gcode_cmd_type == GCODE_TYPE_INTERNAL) // 内部指令，不返回
  {
    return;
  }
  else if (gcode_cmd_type == GCODE_TYPE_FILE)
  {
    return;
  }
  else if (gcode_cmd_type == GCODE_TYPE_USBD)
  {
    user_usb_device_transmit("ok\n");
  }
  else                                             // 普通指令，返回ok
  {
    if (!parseGcodeBufHandle.codeSeenStr("M105") && !parseGcodeBufHandle.codeSeenStr("M114"))
    {
      USER_EchoLogStr("ok\r\n");
    }
    else
    {
      USER_EchoLogStr("\r\n");
    }
  }
}

void GcodeCommand::process(const uint8_t gcodeType)
{
  gcode_cmd_type = gcodeType;

  //char *starpos = NULL;
  if (parseGcodeBufHandle.firstCodeSeen('G'))
  {
    GcodeCMD();
  }
  else if (parseGcodeBufHandle.firstCodeSeen('M'))
  {
    process_m_code();
  }
  else if (parseGcodeBufHandle.firstCodeSeen('T'))
  {
    gcode::t_process();
  }
  else
  {
    return;
  }

  clearToSend();
}


inline void GcodeCommand::GcodeCMD(void)
{
  switch ((int)parseGcodeBufHandle.codeValue())
  {
  case 0: // G0 -> G1
  case 1: // G1
    gcode::g1_process();
    break;

  case 2: // G1
    gcode::g2_process();
    break;

  case 3: // G1
    gcode::g3_process();
    break;

  case 4: // G4
    if (parseGcodeBufHandle.codeSeen('P'))
    {
      if (parseGcodeBufHandle.codeValue() == 0)
      {
        if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
        {
          sg_grbl::st_synchronize();//等待上一条消息执行完
        }
      }
    }

    break;

  case 28: //G28 Home all Axis one at a time
    gcode::g28_process();

    if (ccm_param.t_sys.print_flag)
      ccm_param.t_sys.print_is_home = true;

    break;

  case 29://G29 Auto Bed Leveling (Marlin)
    gcode::g29_process();
    break;

  case 90: // G90
    ccm_param.planner_settings.relative_mode = false;
    break;

  case 91: // G91
    ccm_param.planner_settings.relative_mode = true;
    break;

  case 92: // G92
    gcode::g92_process();
    break;

  default:
    break;
  }
}

/*************************G28 Command start****************************/
bool is_code_seen[MAX_NUM_AXIS] = {false};
bool is_drug_seen_e = false;
void get_coordinates()
{
  for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
  {
    bool axis_code_seen = parseGcodeBufHandle.codeSeen(axis_codes[i]);
    float axis_code_value = 0.0f;

    if (axis_code_seen) axis_code_value = parseGcodeBufHandle.codeValue();

    bool is_relative_mode = (bool)(ccm_param.planner_settings.axis_relative_modes[i] || ccm_param.planner_settings.relative_mode);
#if LASER_MODE

    if (ccm_param.t_sys_data_current.IsLaser && (Z_AXIS == i) && print_status.is_printing)
      axis_code_seen = false;

#endif
    {
      if (axis_code_seen)
      {
        if (feature_print_control::idex_sys_is_extruder())
        {
          gcode::get_coordinates_dual_idex(i, axis_code_value);
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
        {
          gcode::get_coordinates_laser(i, axis_code_value);
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
        {
          if (is_drug_seen_e == false)
          {
            sg_grbl::st_synchronize();
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
            osDelay(100);
          }

          is_drug_seen_e = true;
          gcode::get_coordinates_drug(i, axis_code_value);
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
        {
          gcode::get_coordinates_mix(i, axis_code_value);
        }
        else
        {
          if (i == E_AXIS)
          {
            if (1 == gcode::active_extruder)
            {
              is_relative_mode = (bool)(ccm_param.planner_settings.axis_relative_modes[B_AXIS] || ccm_param.planner_settings.relative_mode);
              ccm_param.grbl_destination[B_AXIS] = axis_code_value + (float)is_relative_mode * ccm_param.grbl_current_position[B_AXIS];
            }
            else if (0 == gcode::active_extruder)
            {
              ccm_param.grbl_destination[i] = axis_code_value + (float)is_relative_mode * ccm_param.grbl_current_position[i];
            }
          }
          else if (i == B_AXIS)
          {
            ccm_param.grbl_destination[i] = axis_code_value + (float)is_relative_mode * ccm_param.grbl_current_position[i];
          }
          else
          {
            ccm_param.grbl_destination[i] = axis_code_value + (float)is_relative_mode * sg_grbl::plan_get_current_save_xyz(i);
          }
        }

        is_code_seen[i] = true;
      }
      else
      {
        if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG && i == E_AXIS)
        {
          sg_grbl::st_synchronize();
          is_drug_seen_e = false;

          if (is_code_seen[X_AXIS] || is_code_seen[Y_AXIS] || is_code_seen[Z_AXIS])
          {
            {
              HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
            }
          }
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
        {
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
        {
        }
        else if (feature_print_control::idex_sys_is_extruder())
        {
        }
        else
        {
          if (i < XYZ_NUM_AXIS && !ccm_param.t_sys_data_current.IsLaser)
          {
            ccm_param.grbl_destination[i] = sg_grbl::plan_get_current_save_xyz(i); //Are these else lines really needed?
          }
          else
          {
            ccm_param.grbl_destination[i] = ccm_param.grbl_current_position[i]; //Are these else lines really needed?
          }
        }
      }
    }
  }

  // Double head, compensation nozzle 2 coordinates
  if (print_status.is_printing)
    gcode::get_coordinates_idex_comp_ext1_dest();

  if (parseGcodeBufHandle.codeSeen('F'))
  {
    next_feedrate = parseGcodeBufHandle.codeValue();

    if (next_feedrate > 0.0f) feedrate = next_feedrate;
  }

  //当移动Z轴大于当前位置5mm以上，则限制其移动速度为 350； 2017512增加速度350；可以在300~350之间取值，大了会导致下降噪声大 add by john
  if (ccm_param.grbl_destination[Z_AXIS] > ccm_param.grbl_current_position[Z_AXIS] + 5)
  {
    if (ccm_param.t_model.xyz_max_pos[Z_AXIS] > 350) //z轴大于350说明是大机器，小机器不用控制速度201716
      feedrate = 350;
  }

  if (parseGcodeBufHandle.codeSeen('Z') && (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id) && feedrate > 2400)
  {
    feedrate = 1800;
  }

  //  if (parseGcodeBufHandle.codeSeen('P')) //下一条Gcode的文件位置
  //  {
  //    SDPos = (unsigned int)parseGcodeBufHandle.codeValueLong();
  //  }
}

GcodeCommand gcodeCommand;

