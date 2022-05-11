#include "user_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  void g92_abl_init(void);
  void g29_abl_process(void);

  volatile bool g29_complete_flag = false;
  volatile bool g29_open_laser_check_flag = false;

  void g29_init(void)
  {
    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      g92_abl_init();
    }
  }

  void g29_process(void)
  {
    g29_complete_flag = false;

    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      g29_open_laser_check_flag = true;
      g29_abl_process();
      g29_open_laser_check_flag = false;
    }

    g29_complete_flag = true;
  }
}











