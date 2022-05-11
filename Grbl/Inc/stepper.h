
#ifndef stepper_h
#define stepper_h

#include <stdbool.h>
#include <stdint.h>

#define USE_PULSE_DELAY
#define PULSE_DELAY_US 10

namespace sg_grbl
{

  void st_init(void);                                                 ///< Initialize and start the stepper motor subsystem
  void st_enable_endstops(bool check);                                ///< Enable/disable endstop checking
  void st_set_position(const float *_current_position);                ///< Set current position in steps
  void st_set_position_axis(const float value, const int axis);
  void st_synchronize(void);                                          ///< Block until all buffered steps are executed
  void st_quick_stop(void);                                           ///< 电机快速停止
  long st_get_position_steps(int axis);                               ///< Get current position in steps
  float st_get_endstops_len(int axis);                                ///< 获取限位位置
  float st_get_position_length(int axis);                             ///< 获取当前实际位置
  bool st_get_z_max_endstops_status(void);                            ///< 获取Z最大限位状态
  bool st_check_endstop_z_hit_max(void);                              ///< 是否撞击Z限位最大，只检测一次，函数调用完置状态为false
  bool st_check_endstop_z_hit_min(void);                              ///< 是否撞击Z限位最小，只检测一次，函数调用完置状态为false
  bool st_check_queue_is_empty(void);                                 ///< 检测队列是否为空
  bool st_axis_xyz_read_min(const int axis);

  // The stepper subsystem goes to sleep when it runs out of things to execute. Call this
  // to notify the subsystem that it is time to go to work.
  void st_wake_up(void);

  // "The Stepper Driver Interrupt" - This timer interrupt is the workhorse.
  // It pops blocks from the block_buffer and executes them by pulsing the stepper pins appropriately.
  void st_process(void);

#if defined(S_CURVE_ACCELERATION)
  extern int32_t bezier_A,     // A coefficient in Bézier speed curve
  bezier_B,     // B coefficient in Bézier speed curve
  bezier_C;     // C coefficient in Bézier speed curve
  extern uint32_t bezier_F,    // F coefficient in Bézier speed curve
  bezier_AV;   // AV coefficient in Bézier speed curve
  extern bool bezier_2nd_half; // If Bézier curve has been initialized or not

  extern void _calc_bezier_curve_coeffs(const int32_t v0, const int32_t v1, const uint32_t av);
  extern int32_t _eval_bezier_curve(const uint32_t curr_step);
#endif

}
#endif




