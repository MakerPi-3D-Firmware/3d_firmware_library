#include "user_common_cpp.h"

namespace gcode
{
  extern void g29_init(void);

  static void grbl_init(void)
  {
    sg_grbl::config_reset_default();
    model_grbl_config_init();
    sg_grbl::st_init();
    sg_grbl::planner_init();
    sg_grbl::temperature_init();
  }

  void init(void)
  {
    grbl_init();

    for (int8_t i = 0; i < MAX_NUM_AXIS; i++)
    {
      if (i < XYZ_NUM_AXIS)
      {
        ccm_param.grbl_destination[i] = ccm_param.t_model.xyz_home_pos[i];
      }
      else
      {
        ccm_param.grbl_destination[i] = 0.0f;
      }

      ccm_param.grbl_current_position[i] = ccm_param.grbl_destination[i];
    }

    g29_init(); // Bed level init

    // K5黑屏固件，一开始不初始化abl调平模块，等更换模块后再初始化
    if (K5 == ccm_param.t_sys_data_current.model_id)
    {
      if (1 == ccm_param.t_sys_data_current.IsMechanismLevel)
      {
        ccm_param.t_sys.is_abl_init = false;
      }
    }
    else
    {
      ccm_param.t_sys.is_abl_init = true;
      user_abl_init();
    }
  }


}





