#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long previous_xTaskGetTickCount_cmd;
extern float feedrate;
extern int feedmultiply;
extern int extrudemultiply;
extern uint32_t SDPos;

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  volatile int32_t layer_count;
  volatile int32_t current_layer;

  void prepare_move(void)
  {
    //    if (BED_LEVEL_PRESSURE_SENSOR != ccm_param.t_sys_data_current.enable_bed_level)
    {
      // Parameter D1, no software limit
      if (!(parseGcodeBufHandle.codeSeen('D') && parseGcodeBufHandle.codeValueLong() == 1))
      {
        if ((K5 != ccm_param.t_sys_data_current.model_id) && M30 != ccm_param.t_sys_data_current.model_id)
        {
          clamp_to_software_endstops(ccm_param.grbl_destination);
        }
      }
    }
    previous_xTaskGetTickCount_cmd = xTaskGetTickCount();
    // Do not use feedmultiply for E or Z only moves
    //    if (feature_print_control::idex_sys_is_extruder())
    {
      if (1 == gcode::active_extruder)
      {
        if ((ccm_param.grbl_current_position[X2_AXIS] == ccm_param.grbl_destination [X2_AXIS])
            && (ccm_param.grbl_current_position[Y_AXIS] == ccm_param.grbl_destination [Y_AXIS]))
        {
          process_buffer_line(ccm_param.grbl_destination, feedrate / 60, feedmultiply, extrudemultiply, SDPos);
        }
        else
        {
          //plan_buffer_line(destination, (feedrate*feedmultiply/60)/100.0f, gcode::active_extruder,SDPos,100);
          // 现在XY速度不小与20mm/s
          float feed_rate_tmp = (feedrate * feedmultiply / 60) / 100.0f;
          process_buffer_line(ccm_param.grbl_destination, feed_rate_tmp, feedmultiply, extrudemultiply, SDPos);
        }
      }
      else if (0 == gcode::active_extruder)
      {
        if ((ccm_param.grbl_current_position[X_AXIS] == ccm_param.grbl_destination [X_AXIS])
            && (ccm_param.grbl_current_position[Y_AXIS] == ccm_param.grbl_destination [Y_AXIS]))
        {
          process_buffer_line(ccm_param.grbl_destination, feedrate / 60, feedmultiply, extrudemultiply, SDPos);
        }
        else
        {
          //plan_buffer_line(destination, (feedrate*feedmultiply/60)/100.0f, gcode::active_extruder,SDPos,100);
          // 现在XY速度不小与20mm/s
          float feed_rate_tmp = (feedrate * feedmultiply / 60) / 100.0f;
          process_buffer_line(ccm_param.grbl_destination, feed_rate_tmp, feedmultiply, extrudemultiply, SDPos);
        }
      }
    }

    for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
    }
  }

  void g1_process(void)
  {
    get_coordinates(); // For X Y Z E F

    if (parseGcodeBufHandle.codeSeen('R')) //下一条Gcode的文件位置
    {
      SDPos = (unsigned int)parseGcodeBufHandle.codeValueLong();
    }

    prepare_move();
  }


}

