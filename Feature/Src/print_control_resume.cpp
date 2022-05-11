#include "user_common_cpp.h"
#ifdef HAS_PRINT_CONTROL
#include "USBFileTransfer.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifdef ENABLE_GUI_LVGL
extern bool is_wait_resume_temp;
#endif

namespace feature_print_control
{
  extern volatile int temp_hotend0_to_resume;                   /*!< 恢复打印喷嘴温度 */
  extern volatile int temp_hotend1_to_resume;                   /*!< 恢复打印喷嘴温度 */
  extern volatile int temp_bed_to_resume;                       /*!< 恢复打印热床温度 */
  extern volatile float print_real_pos[MAX_NUM_AXIS];           /*!< 恢复打印XYZ位置 */
  extern volatile float print_pause_real_pos[MAX_NUM_AXIS];     /*!< 恢复打印XYZ位置 */
  extern volatile float print_curr_pos[MAX_NUM_AXIS];           /*!< 恢复打印XYZ位置 */
  extern volatile uint8_t pause_active_extruder_save;
  extern volatile int pause_print_status;               /*!< 暂停打印状态 */
  extern volatile bool is_heating_status;           /*!< 是否处于打印加热状态 */
  extern volatile int pause_feed_rate_save;

  int get_pause_extruder_target_temp(void)
  {
    return temp_hotend0_to_resume;
  }

  bool is_pause_to_resume_temp = false;     /*!< 是否暂停恢复温度 */

  static void _resume_eb(void)
  {
    if (feature_print_control::idex_sys_is_extruder())
    {
      if (feature_print_control::idex_sys_is_normal())
      {
        USER_SEND_INTERNAL_CMD_BUF("G92 X%f Y%f Z%f", print_pause_real_pos[pause_active_extruder_save == 0 ? X_AXIS : X2_AXIS], print_pause_real_pos[Y_AXIS], print_pause_real_pos[Z_AXIS]);
      }
      else if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        USER_SEND_INTERNAL_CMD_BUF("G92 X%f I%f Y%f Z%f", print_pause_real_pos[X_AXIS], print_pause_real_pos[X2_AXIS], print_pause_real_pos[Y_AXIS], print_pause_real_pos[Z_AXIS]);
      }
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("G92 X%f Y%f Z%f", print_pause_real_pos[pause_active_extruder_save == 0 ? X_AXIS : X2_AXIS], print_pause_real_pos[Y_AXIS], print_pause_real_pos[Z_AXIS]);
    }

    gcode_cmd_eb_reset();          // 设置eb位置

    if (ccm_param.t_sys_data_current.enable_color_mixing)
    {
      USER_SEND_INTERNAL_CMD_BUF("G1 F150 E8 B8 D1");      // eb各自运动8mm
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("G1 F150 E16 B0 D1");      // eb各自运动8mm
    }

    USER_SEND_INTERNAL_CMD_BUF("G92 E%f B%f", print_real_pos[E_AXIS], print_real_pos[B_AXIS]); // 设置EB坐标
  }

  static void _resume_xyz_mix(void)
  {
    gcode_cmd_t0(false);
    USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%0.4f Y%0.4f D1", print_real_pos[X_AXIS], print_real_pos[Y_AXIS]); //XY回到原来位置
    USER_SEND_INTERNAL_CMD_BUF("G1 F%d Z%0.4f D1", (int)homing_feedrate[Z_AXIS], print_real_pos[Z_AXIS]); //z回到原来位置
  }

  static void _resume_xyz_other(void)
  {
    if (ccm_param.t_model.xyz_move_max_pos[X_AXIS] < print_real_pos[X_AXIS] || ccm_param.t_model.xyz_move_max_pos[Y_AXIS] < print_real_pos[Y_AXIS])
    {
      USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%0.4f Y%0.4f D1", print_real_pos[X_AXIS], print_real_pos[Y_AXIS]); //XY回到原来位置
      USER_SEND_INTERNAL_CMD_BUF("G1 F%d Z%0.4f D1", (int)homing_feedrate[Z_AXIS], print_real_pos[Z_AXIS]); //z回到原来位置
    }
    else
    {
      USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%0.4f Y%0.4f D1", print_real_pos[X_AXIS], print_real_pos[Y_AXIS]); //XY回到原来位置
      USER_SEND_INTERNAL_CMD_BUF("G1 F%d Z%0.4f D1", (int)homing_feedrate[Z_AXIS], print_real_pos[Z_AXIS]); //z回到原来位置
    }
  }

  static void _resume_xyz(void)
  {
    if (feature_print_control::idex_sys_is_extruder())
    {
      if (feature_print_control::idex_sys_is_normal())
      {
        USER_SEND_INTERNAL_CMD_BUF("T%d S-1", pause_active_extruder_save); // 关闭坐标转换
        USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%0.4f Y%0.4f D1", print_real_pos[pause_active_extruder_save == 0 ? X_AXIS : X2_AXIS], print_real_pos[Y_AXIS]); //XY回到原来位置
        USER_SEND_INTERNAL_CMD_BUF("G1 F%d Z%0.4f D1", (int)homing_feedrate[Z_AXIS], print_real_pos[Z_AXIS]); //z回到原来位置
      }
      else if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        gcode_cmd_t0(false);
        USER_SEND_INTERNAL_CMD_BUF("G1 F2400 X%0.4f I%0.4f Y%0.4f D1", print_real_pos[X_AXIS], print_real_pos[X2_AXIS], print_real_pos[Y_AXIS]); //XY回到原来位置
        USER_SEND_INTERNAL_CMD_BUF("G1 F%d Z%0.4f D1", (int)homing_feedrate[Z_AXIS], print_real_pos[Z_AXIS]); //z回到原来位置
      }
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      _resume_xyz_mix();
    }
    else
    {
      _resume_xyz_other();
    }
  }


  bool resumeBackToPrintPos(void)
  {
    static int resume_bak_pos_status = 0;

    if (feature_print_control::idex_sys_is_extruder())
    {
      feature_print_control::idex_sys_copy_flash_print_type();
    }

    if (!is_heating_status)
    {
      if (0 == resume_bak_pos_status  && 0 == sg_grbl::planner_moves_planned())
      {
        gcode_cmd_coord_transform(false); // 关闭坐标转换
        _resume_eb();
        gcode_cmd_coord_transform(true);    // 开启坐标转换
        resume_bak_pos_status = 1;
        return false;
      }
      else if ((1 == resume_bak_pos_status) && (0 == sg_grbl::planner_moves_planned()))
      {
        gcode_cmd_coord_transform(false); // 关闭坐标转换
        _resume_xyz();
        gcode_cmd_coord_transform(true);    // 开启坐标转换
        USER_SEND_INTERNAL_CMD_BUF("G92 X%f Y%f Z%f D1", print_curr_pos[pause_active_extruder_save == 0 ? X_AXIS : X2_AXIS], print_curr_pos[Y_AXIS], print_curr_pos[Z_AXIS]); //XY回到原来位置
        USER_SEND_INTERNAL_CMD_BUF("G92 E%f B%f D1", print_curr_pos[E_AXIS], print_curr_pos[B_AXIS]); //XY回到原来位置
        USER_SEND_INTERNAL_CMD_BUF("G1 F%d", pause_feed_rate_save); //z回到原来位置
        USER_SEND_INTERNAL_CMD_BUF("M400"); //z回到原来位置
        resume_bak_pos_status = 2;
        return false;
      }
      else if ((2 == resume_bak_pos_status) && (0 == sg_grbl::planner_moves_planned()))
      {
        resume_bak_pos_status = 0; // 重置变量，避免第二次恢复打印异常
        return true;
      }

      return false;
    }
    else
    {
      return true;
    }
  }

  void pauseToResumeTemp(void)
  {
    if (!is_pause_to_resume_temp)
    {
      ccm_param.t_gui.target_nozzle_temp[0] = temp_hotend0_to_resume;
      ccm_param.t_gui.target_nozzle_temp[1] = temp_hotend1_to_resume;
      ccm_param.t_gui.target_hot_bed_temp = temp_bed_to_resume;
      sg_grbl::temperature_set_extruder_target(temp_hotend0_to_resume, 0);
      sg_grbl::temperature_set_extruder_target(temp_hotend1_to_resume, 1);
      sg_grbl::temperature_set_bed_target(temp_bed_to_resume);
      is_pause_to_resume_temp = true;
    }

    osDelay(50);
  }

  bool isResumeTempDone(void)
  {
    pauseToResumeTemp();
    int degh = (int)sg_grbl::temperature_get_extruder_current(gcode::active_extruder);
    int deghT = (int)sg_grbl::temperature_get_extruder_target(gcode::active_extruder);

    if (degh < deghT) return false;

    if (ccm_param.t_sys.enable_hot_bed)
    {
      //    int degb=degBed();
      //    int degbT=degTargetBed();
      //      if((degh<deghT) || (degb<degbT)) return;//暂停后返回打印，热床不等待，2017/3/14
    }
    else
    {
      if (degh < deghT) return false;
    }

    return true;
  }


  void processResumeFinish(void)
  {
    pause_print_status = 0;
    is_heating_status = false;
    is_pause_to_resume_temp = false;
    SetIsUSBPrintPause(false);
    user_send_file_cmd((char *)"M400", 0, ccm_param.layer_count, ccm_param.current_layer);
    user_send_file_cmd((char *)"M2001", 0, ccm_param.layer_count, ccm_param.current_layer);
    user_send_file_cmd((char *)"M2004 S1", 0, ccm_param.layer_count, ccm_param.current_layer);
  }


  void resume_process(void)
  {
    bool is_rec_pos_success = false;

    if (!print_status.is_resume_printing) return;

    switch (pause_print_status)
    {
    case 0:
      return;

    case 1:
    case 2:
    case 3:
      break;

    case 4:
    case 5:
    case 6:
      is_rec_pos_success = resumeBackToPrintPos();

      if (!is_rec_pos_success) return;

      break;

    case 7:
      if (!isResumeTempDone())
      {
#ifdef ENABLE_GUI_LVGL
        is_wait_resume_temp = true;
#endif
        return;
      }

#ifdef ENABLE_GUI_LVGL
      is_wait_resume_temp = false;
#endif
      is_rec_pos_success = resumeBackToPrintPos();

      if (!is_rec_pos_success) return;

      break;
    } // end switch

    if (is_rec_pos_success)
      processResumeFinish();
  }

  //继续打印
  void resume_start(void)
  {
    flash_poweroff_data_reset();
    print_status.is_resume_printing = true;
  }

}

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_PRINT_CONTROL





