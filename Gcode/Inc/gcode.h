#ifndef GCODE_H
#define GCODE_H

#include "../../Grbl/Inc/vector_3.h"

#ifdef __cplusplus
extern "C" {
#endif
extern sg_grbl::matrix_3x3 plan_bed_level_matrix;

#define M600_STATUS_Z_AWAY_PLATFORM 1
#define M600_STATUS_XY_HOME_UNLOAD 2
#define M600_STATUS_BEEP_NOTICE 3
#define M600_STATUS_RESTORE_POS 4
#define M600_STATUS_FINISH 5

#ifdef __cplusplus
} // extern "C" {
#endif


namespace gcode
{
  extern volatile bool g29_complete_flag;
  extern volatile bool g29_open_laser_check_flag;
  extern volatile uint8_t active_extruder;

  extern volatile int32_t layer_count;
  extern volatile int32_t current_layer;
  extern volatile uint8_t m600_status;

  extern void init(void);

  extern void g1_process(void);
  extern void g2_process(void);
  extern void g3_process(void);
  extern void g28_process(void);
  extern void g28_get_home_pos_adding(const int dir, float &x, float &y, float &z);
  extern void g29_process(void);
  extern int g29_abl_get_matrix_index(sg_grbl::vector_3 point);

  extern void t_process(void);

  extern void g92_process(void);

  extern void m84_process(void);

  extern void m92_process(void);

  extern void m105_process(void);

  extern void m106_process(void);
  extern void m107_process(void);

  extern void m114_process(void);

  extern void m104_process(void);
  extern void m109_process(void);

  extern void m140_process(void);
  extern void m190_process(void);

  extern void m200_process(void);
  extern void m201_process(void);
  extern void m203_process(void);
  extern void m204_process(void);
  extern void m205_process(void);

  extern void m221_process(void);

  extern void m303_process(void);
  extern void m302_process(void);
  extern void m301_process(void);

  extern void m500_process(void);
  extern void m501_process(void);
  extern void m502_process(void);
  extern void m503_process(void);

  extern void m600_process(void);

  extern void get_coordinates_dual_idex(const int axis, const float value);
  extern void get_coordinates_idex_comp_ext1_dest(void);
  extern void clamp_to_software_endstops_dual_idex(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface);

  extern void get_coordinates_laser(const int axis, const float value);
  extern void clamp_to_software_endstops_laser(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface);

  extern void get_coordinates_mix(const int axis, const float value);
  extern void clamp_to_software_endstops_mix(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface);

  extern void get_coordinates_drug(const int axis, const float value);
  extern void clamp_to_software_endstops_drug(volatile float (&target)[MAX_NUM_AXIS], int &IsPopWarningInterface);
}

#endif // GCODE_H

