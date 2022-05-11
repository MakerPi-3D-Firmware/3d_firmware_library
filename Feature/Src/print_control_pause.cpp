#include "user_common_cpp.h"
#ifdef HAS_PRINT_CONTROL
#include "USBFileTransfer.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace feature_print_control
{
  volatile int temp_hotend0_to_resume = 0;                /*!< 恢复打印喷嘴温度 */
  volatile int temp_hotend1_to_resume = 0;                /*!< 恢复打印喷嘴温度 */
  volatile int temp_bed_to_resume = 0;                    /*!< 恢复打印热床温度 */
  volatile float print_real_pos[MAX_NUM_AXIS];            /*!< 恢复打印XYZ位置 */
  volatile float print_pause_real_pos[MAX_NUM_AXIS];      /*!< 恢复打印XYZ位置 */
  volatile float print_curr_pos[MAX_NUM_AXIS];            /*!< 恢复打印XYZ位置 */
  volatile int pause_print_status = 0;               /*!< 暂停打印状态 */
  volatile bool is_heating_status = false;           /*!< 是否处于打印加热状态 */
  volatile TickType_t xTimeToCoolDown = 0;           /*!< 冷却超时时间 */
  volatile uint8_t pause_active_extruder_save = 0;
  volatile int pause_feed_rate_save = 0;

  void _pause_to_cool_down(float coolDownFactor)
  {
    float hotendTemp = (sg_grbl::temperature_get_extruder_target(gcode::active_extruder) * coolDownFactor); // float
    float bedTemp = (sg_grbl::temperature_get_bed_target() * coolDownFactor); // float

    if (ccm_param.t_gui.target_nozzle_temp[gcode::active_extruder] != hotendTemp) //防止二次设置,M601命令为了与android屏匹配，执行换料前先加热；20170930
    {
      sg_grbl::temperature_set_extruder_target(hotendTemp, 0);
      ccm_param.t_gui.target_nozzle_temp[gcode::active_extruder] = hotendTemp;
    }

    if ((ccm_param.t_sys.enable_hot_bed) && (ccm_param.t_gui.target_hot_bed_temp != bedTemp))
    {
      sg_grbl::temperature_set_bed_target(bedTemp);
      ccm_param.t_gui.target_hot_bed_temp = bedTemp;
    }

    print_status.is_pause_to_cool_down = true; // 断料状态下，换料要用到，即降温了则要先加温才能换料
  }

  // 执行暂停打印操作，平台下降60mm，xy归零
  void _pause_prepare(void)
  {
    if (!is_heating_status) // 未开始打印，不用执行暂停操作
    {
      pause_feed_rate_save = GetFeedRate();
      sg_grbl::st_synchronize();

      for (int i = 0; i < MAX_NUM_AXIS; i++)
      {
        if (i < XYZ_NUM_AXIS)
        {
          if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
          {
            print_real_pos[i] = sg_grbl::plan_get_xyz_real(i); //获取位置
          }
          else
          {
            print_real_pos[i] = GetCurrentPosition(i); //获取位置
          }
        }
        else
        {
          print_real_pos[i] = GetCurrentPosition(i); //获取位置
        }

        print_curr_pos[i] = GetCurrentPosition(i);
      }

      gcode_cmd_z_down_60mm_xy_to_home();
    }
  }

  void pause_process(void)
  {
    if (!print_status.is_pause_printing) return;

    if (print_status.is_pause_printing && (0 == pause_print_status) && (0 == sg_grbl::planner_moves_planned()))
    {
      pause_print_status = 1;
    }

    // 判断当前是否已经加热完成，加热完成跳到下一步
    // 未加热，设置is_heating_status为true，等待加热完成
    if (1 == pause_print_status && (0 == sg_grbl::planner_moves_planned()))
    {
      if (ccm_param.t_sys.is_gcode_m109_done) // 已加热，下一步
      {
        is_heating_status = false; // 处于加热状态
      }
      else
      {
        is_heating_status = true; // 处于加热状态
      }

      pause_print_status = 2;
    }
    else if (2 == pause_print_status && (0 == sg_grbl::planner_moves_planned()))   // 判断当前运动队列是否执行完
    {
      if (is_heating_status)
      {
        print_status.is_printing = false; // 设置打印状态为false
        SetIsUSBPrintPause(true);
        pause_print_status = 3;
        return;
      }

      if (0 == sg_grbl::planner_moves_planned()) // 运动队列为空，下一步
      {
        print_status.is_printing = false; // 设置打印状态为false
        SetIsUSBPrintPause(true);
        pause_print_status = 3;
      }
    }
    else if (3 == pause_print_status && (0 == sg_grbl::planner_moves_planned())) // 执行暂停打印操作
    {
      pause_active_extruder_save = gcode::active_extruder;
      _pause_prepare();
      pause_print_status = 4;
    }
    else if ((4 == pause_print_status) && (sg_grbl::planner_moves_planned() == 0))
    {
      if (ccm_param.t_sys_data_current.enable_powerOff_recovery) // 断电开启，非断电状态
      {
        flash_poweroff_data_pause_save();
      }

      pause_print_status = 5;
      gcode_cmd_gui_wait_done();
      gcode_cmd_coord_transform(false);
      gcode_cmd_t0(false);
      USER_SEND_INTERNAL_CMD_BUF("G92 X%d Y%d", (int)ccm_param.t_model.xyz_home_pos[X_AXIS], (int)ccm_param.t_model.xyz_home_pos[Y_AXIS]);

      if (feature_print_control::idex_sys_is_normal())
      {
        gcode_cmd_t1(false);
        USER_SEND_INTERNAL_CMD_BUF("G92 X%d Y%d", (int)ccm_param.t_model.xyz_home_pos[X2_AXIS], (int)ccm_param.t_model.xyz_home_pos[Y_AXIS]);
        USER_SEND_INTERNAL_CMD_BUF("T%d S-1", pause_active_extruder_save);
      }
      else if (feature_print_control::idex_sys_is_copy_or_mirror())
      {
        gcode_cmd_t0(false);
        USER_SEND_INTERNAL_CMD_BUF("G92 X%f I%f Y%f", ccm_param.t_model.xyz_home_pos[X_AXIS], ccm_param.t_model.xyz_home_pos[X2_AXIS], ccm_param.t_model.xyz_home_pos[Y_AXIS]);
      }

      gcode_cmd_coord_transform(true);
    }
    else if (5 == pause_print_status && (sg_grbl::planner_moves_planned() == 0)) // 设置暂停打印冷却超时时间
    {
      for (int i = 0; i < MAX_NUM_AXIS; i++)
      {
        if (i == X_AXIS || i == X2_AXIS || i == Y_AXIS)
        {
          print_pause_real_pos[i] = ccm_param.t_model.xyz_home_pos[i];
        }
        else if (i == Z_AXIS || i == Z2_AXIS)
        {
          if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
          {
            print_pause_real_pos[i] = sg_grbl::plan_get_xyz_real(i); //获取位置
          }
          else
          {
            print_pause_real_pos[i] = GetCurrentPosition(i); //获取位置
          }
        }
        else
        {
          print_pause_real_pos[i] = GetCurrentPosition(i); //获取位置
        }
      }

      temp_hotend0_to_resume = (int)sg_grbl::temperature_get_extruder_target(0);
      temp_hotend1_to_resume = (int)sg_grbl::temperature_get_extruder_target(1);
      temp_bed_to_resume = (int)sg_grbl::temperature_get_bed_target();
      xTimeToCoolDown = xTaskGetTickCount() + 1000 * 60 * 3;
      pause_print_status = 6;
    }
    else if (6 == pause_print_status) // 暂停超时，执行温度下降操作
    {
      if (xTaskGetTickCount() > xTimeToCoolDown)
      {
        if (!ccm_param.t_sys.is_filament_print_not_have)
          _pause_to_cool_down(0.5f);

        pause_print_status = 7;
      }
    }
    else if (7 == pause_print_status) // 暂停打印结束
      print_status.is_pause_printing = false;
  }

  void pause_start(void)
  {
    print_status.is_pause_printing = true;
    pause_print_status = 0;
    print_status.is_pause_to_cool_down = false;
  }

}
#ifdef __cplusplus
} //extern "C" {
#endif
#endif // HAS_PRINT_CONTROL





