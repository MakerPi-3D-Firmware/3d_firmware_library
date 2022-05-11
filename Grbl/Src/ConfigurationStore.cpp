#include "user_common_cpp.h"

namespace sg_grbl
{

  // Calculate the steps/s^2 acceleration rates, based on the mm/s^s
  void reset_acceleration_rates()
  {
    for (int8_t i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      ccm_param.planner_settings.axis_steps_per_sqr_second[i] =
        (unsigned long)(ccm_param.planner_settings.max_acceleration_mm_per_s2[i] *
                        ccm_param.planner_settings.axis_steps_per_mm[i]);
    }
  }

  void config_reset_default(void)
  {
    for (short i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (ccm_param.t_sys_data_current.enable_color_mixing)
      {
        float tmp1[MAX_NUM_AXIS] = DEFAULT_AXIS_STEPS_PER_UNIT_X;
        float tmp2[MAX_NUM_AXIS] = DEFAULT_MAX_FEEDRATE_X;
        unsigned long tmp3[MAX_NUM_AXIS] = DEFAULT_MAX_ACCELERATION_X;
        ccm_param.planner_settings.axis_steps_per_mm[i] = tmp1[i] * ccm_param.t_sys.step;
        ccm_param.planner_settings.max_feedrate_mm_s[i] = tmp2[i];
        ccm_param.planner_settings.max_acceleration_mm_per_s2[i] = tmp3[i];
      }
      else
      {
        float tmp1[MAX_NUM_AXIS] = DEFAULT_AXIS_STEPS_PER_UNIT;
        float tmp2[MAX_NUM_AXIS] = DEFAULT_MAX_FEEDRATE;
        unsigned long tmp3[MAX_NUM_AXIS] = DEFAULT_MAX_ACCELERATION;
        ccm_param.planner_settings.axis_steps_per_mm[i] = tmp1[i] * ccm_param.t_sys.step;
        ccm_param.planner_settings.max_feedrate_mm_s[i] = tmp2[i];
        ccm_param.planner_settings.max_acceleration_mm_per_s2[i] = tmp3[i];
      }
    }

    bool buf[] = AXIS_RELATIVE_MODES_X;
    (void)memcpy((void *)ccm_param.planner_settings.axis_relative_modes, buf, sizeof(buf) / sizeof(bool));
    ccm_param.planner_settings.relative_mode = false;
    // steps per sq second need to be updated to agree with the units per sq second
    reset_acceleration_rates();
    ccm_param.planner_settings.acceleration = DEFAULT_ACCELERATION;
    ccm_param.planner_settings.retract_acceleration = DEFAULT_RETRACT_ACCELERATION;
    ccm_param.planner_settings.min_feedrate_mm_s = DEFAULT_MINIMUMFEEDRATE;
    ccm_param.planner_settings.min_segment_time_us = DEFAULT_MINSEGMENTTIME;
    ccm_param.planner_settings.min_travel_feedrate_mm_s = DEFAULT_MINTRAVELFEEDRATE;
    ccm_param.planner_settings.max_xy_jerk = DEFAULT_XYJERK;
    ccm_param.planner_settings.max_z_jerk = DEFAULT_ZJERK;
    ccm_param.planner_settings.max_e_jerk = DEFAULT_EJERK;
    ccm_param.planner_settings.drop_segments = DROP_SEGMENTS;

    if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
    {
      ccm_param.planner_settings.max_b_jerk = DEFAULT_BJERK;
    }
  }

}













