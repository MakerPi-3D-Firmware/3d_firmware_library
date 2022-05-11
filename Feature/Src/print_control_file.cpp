#include "user_common_cpp.h"
#ifdef HAS_PRINT_CONTROL
#include "../../Grbl/Inc/gcodebufferhandle.h"
#include "USBFileTransfer.h"
#ifdef __cplusplus
extern "C" {
#endif



#define MEDIUM_USER 1                                  /*!< SD卡存储介质 */
#define MEDIUM_USB 2                                    /*!< U盘存储介质 */
static uint8_t medium_id = MEDIUM_USB;                  /*!< 记录打印文件存储介质 */

TCHAR printfilepathname[_MAX_LFN];             /*!< 文件名称 */
FIL printfile;                                /*!< 文件对象 */

namespace feature_print_control
{

  void getFileName(void)
  {
    if (print_status.is_poweroff_recovery_print) // 断电续打恢复打印，设置打印文件名
    {
      if (ccm_param.t_sys.is_power_off_file_from_user)   //断电续打文件在SD卡中
      {
        medium_id = MEDIUM_USER;
        //      IsPowerOffRecoverFileInSD = 1; //打印的是SD卡中的文件
      }

      (void)utils_str_memmove(printfilepathname, ccm_param.power_path_file_name);
    }
    else // 正常打印获取文件名
    {
      if (MEDIUM_USB == medium_id) // U盘文件
      {
        if (ccm_param.t_sys.is_medium_root_dir)
          utils_str_memmove(printfilepathname, (TCHAR *)ccm_param.t_sys.medium_current_path);
        else
        {
          utils_str_memmove(printfilepathname, (TCHAR *)ccm_param.t_sys.medium_current_path);
          utils_str_strcat(printfilepathname, (TCHAR *)_T("/"));
        }

        utils_str_strcat(printfilepathname, (TCHAR *)ccm_param.t_gui.print_name);
      }
      else if (MEDIUM_USER == medium_id) // SD卡文件
      {
        utils_str_memmove(printfilepathname, USERPath);
        utils_str_strcat(printfilepathname, (TCHAR *)ccm_param.t_gui.print_name);
      }
    }
  }

  bool file_open(void)
  {
    bool file_open_status = true;
    ccm_param.file_read_buf_index = FILE_READ_SIZE;
    getFileName();  // 获取要打印文件名

    if (MEDIUM_USER == medium_id)
    {
      taskENTER_CRITICAL();
    }

    if (f_open(&printfile, printfilepathname, FA_READ) == FR_OK)
    {
      ccm_param.t_gui.printfile_size = f_size(&printfile);

      // 断电续打重设文件打印位置
      if (print_status.is_poweroff_recovery_print && (ccm_param.t_power_off.sd_position != 0))
      {
        ccm_param.t_gui.file_size = ccm_param.t_gui.printfile_size - ccm_param.t_power_off.sd_position;
        (void)f_lseek(&printfile, ccm_param.t_power_off.sd_position);
      }
      else
      {
        ccm_param.t_gui.file_size = ccm_param.t_gui.printfile_size;
        ccm_param.t_sys.print_time_save = 0; // 重置打印时间
        ccm_param.t_sys.print_is_home = false;
      }

      resetCmdBuf();
      // 重置gui打印状态
      ccm_param.t_gui.printed_time_sec = 0;
      ccm_param.t_gui.print_percent = 0;
      ccm_param.t_gui.used_total_material = 0;
    }
    else
    {
      file_open_status = false;
    }

    if (MEDIUM_USER == medium_id)
    {
      taskEXIT_CRITICAL();
    }

    return file_open_status;
  }


  void file_close(void)
  {
    ccm_param.t_gui.printfile_size = 0;
    ccm_param.t_gui.file_size = 0;
    ccm_param.file_read_buf_index = FILE_READ_SIZE;

    if (MEDIUM_USER == medium_id)
    {
      print_status.is_print_medium_user_file = false;
      ccm_param.t_sys.is_power_off_file_from_user = false;
      medium_id = MEDIUM_USB;
      taskENTER_CRITICAL();
      (void)f_close(&printfile);
#ifndef ENABLE_GUI_LVGL

      if (!dual_e0_e1_coor_cal_file_is_exit(printfilepathname))
#endif
      {
        (void)f_unlink(printfilepathname); //打印完删除上传的文件
      }

      taskEXIT_CRITICAL();
    }
    else //从U盘读取的时候，不能添加，因为USB的控制是一个单独的任务
    {
      (void)f_close(&printfile);
    }
  }

  bool file_read_data_to_buf(uint32_t read_buf_size)
  {
    unsigned int file_br;    /* File R/W count */
    bool result = false;

    if (ccm_param.t_gui.file_size)
    {
      (void)memset(ccm_param.file_read_buf, 0, sizeof(ccm_param.file_read_buf)); //clear buffer
      uint32_t read_size = 512;
      uint32_t read_size_count = 0;

      for (read_size_count = 0; read_size_count <= read_buf_size; read_size_count += read_size)
      {
        if (MEDIUM_USER == medium_id)
        {
          taskENTER_CRITICAL();
        }

        if (read_size_count > 0 && ccm_param.t_gui.file_size <= read_size_count)
        {
          read_size = ccm_param.t_gui.file_size - (read_size_count - read_size);
        }

        f_lseek(&printfile, ccm_param.t_gui.printfile_size - ccm_param.t_gui.file_size + read_size_count);

        if (f_read(&printfile, &ccm_param.file_read_buf[read_size_count], read_size, &file_br) != FR_OK)
        {
#ifndef ENABLE_GUI_LVGL
          gui_disp_error_info_show(ERR_FATFS);
#else
          custom_disp_error_info_show(ERR_FATFS);
#endif
          result = true;
        }

        if (MEDIUM_USER == medium_id)
        {
          taskEXIT_CRITICAL();
        }

        osDelay(2);
      }

      if (read_size_count > 0)  ccm_param.file_read_buf_index = 0;
    }

    return result;
  }

  void readFinish(void)
  {
    feature_print_control::file_close();                  // 读取结束，关闭文件
    sg_grbl::st_synchronize();         // 执行完环形队列指令再执行下面指令
    set_file_print_finish();
    ccm_param.t_gui.used_total_material += GetCurrentPosition((int)E_AXIS);

    if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
    {
      ccm_param.t_gui.used_total_material += GetCurrentPosition((int)B_AXIS);
    }

    if (feature_print_control::idex_sys_is_extruder())
    {
      feature_print_control::idex_sys_set_normal();
    }
    else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
    {
      ccm_param.t_sys.print_type_mix = MIX_PRINT_TYPE_GRADIENT_COLOR;
    }

    gcode_cmd_eb_reset();
    gcode_cmd_z_down_60mm_xy_to_home();            // 打印完后下降60
    SetIsUSBPrintFinished(true);     // USB联机已完成打印
  }

  //从SD卡读取gcode命令
  void file_get_gcode_buf(void)
  {
    if (print_status.is_printing)
    {
      while (ccm_param.t_gui.file_size)
      {
        if (ccm_param.file_read_buf_index == FILE_READ_SIZE)
        {
          if (file_read_data_to_buf(FILE_READ_SIZE))
          {
            break;    //读取文件失败
          }
        }

        {
          char sd_char = ccm_param.file_read_buf[ccm_param.file_read_buf_index++];
          uint32_t file_position =  ccm_param.t_gui.printfile_size - ccm_param.t_gui.file_size;
          ccm_param.t_gui.file_size--;

          if (GetGcodeFromBuf(sd_char, file_position,
                              (1 == ccm_param.t_sys_data_current.enable_color_mixing &&
                               1 == ccm_param.t_sys_data_current.have_set_machine),
                              ccm_param.t_gui.file_size,
                              (unsigned int &)ccm_param.file_read_buf_index)) // 若是混色版本 且 已经设置好机器，需要解密
          {
            break;
          }
        }
      }

      if (0 == ccm_param.t_gui.file_size)
      {
        readFinish();
      }
    }
  }

  int file_get_percent(void)
  {
    if (ccm_param.t_gui.printfile_size)
    {
      return (int)((ccm_param.t_gui.printfile_size - ccm_param.t_gui.file_size) / ((ccm_param.t_gui.printfile_size + 99) / 100));
    }
    else
    {
      return 0;
    }
  }

  void file_form_medium_user(void)
  {
    medium_id = MEDIUM_USER;
    print_status.is_print_medium_user_file = true;
  }

  void file_reset_medium(void)
  {
    medium_id = MEDIUM_USB;
    print_status.is_print_medium_user_file = false;
  }

}


#ifdef __cplusplus
} //extern "C" {
#endif

#endif // HAS_PRINT_CONTROL





