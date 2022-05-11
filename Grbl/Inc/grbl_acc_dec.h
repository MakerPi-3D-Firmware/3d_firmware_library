#ifndef GRBL_ACC_DEC_H
#define GRBL_ACC_DEC_H

namespace sg_grbl
{

#ifdef __cplusplus
  extern "C" {
#endif

#include "planner_block_buffer.h"

#ifdef __cplusplus
} // extern "C" {
#endif

class Trapezoid
{
public:
  Trapezoid();
  void calculate_for_block(block_t *block, float entry_factor, float exit_factor);
  void generator_reset(block_t *st_current_block, volatile char &step_loops);
  void calculare_timer(block_t *st_current_block, volatile unsigned long step_events_completed, volatile char &step_loops);
  /**
   * Calculate the maximum allowable speed at this point, in order
   * to reach 'target_velocity' using 'acceleration' within a given
   * 'distance'.
   */
  float max_allowable_speed(float accel, float target_velocity, float distance);
private:
  /**
   * Calculate the distance (not time) it takes to accelerate
   * from initial_rate to target_rate using the given acceleration:
   */
  inline float estimate_acceleration_distance(float initial_rate, float target_rate, float accel);
  /**
   * Return the point at which you must start braking (at the rate of -'acceleration') if
   * you start at 'initial_rate', accelerate (until reaching the point), and want to end at
   * 'final_rate' after traveling 'distance'.
   *
   * This is used to compute the intersection point between acceleration and deceleration
   * in cases where the "trapezoid" has no plateau (i.e., never reaches maximum speed)
   */
  inline float intersection_distance(float initial_rate, float final_rate, float accel, float distance);
private:
  long acceleration_time;
  long deceleration_time;
  unsigned short acc_step_rate; // needed for deccelaration start point
  unsigned short OCR1A_nominal;
  char step_loops_nominal;
};
extern Trapezoid trapezoid;

#ifdef __cplusplus
extern "C" {
#endif

  inline void trapezoid_or_s_curve_generator_reset(block_t *st_current_block, volatile char &step_loops)
  {
    trapezoid.generator_reset(st_current_block, step_loops);
  }

  inline void trapezoid_or_s_curve_calculare_timer(block_t *st_current_block, volatile unsigned long step_events_completed, volatile char &step_loops)
  {
    trapezoid.calculare_timer(st_current_block, step_events_completed, step_loops);
  }

  inline float trapezoid_or_s_curve_max_allowable_speed(block_t *block, float accel, float target_velocity, float distance)
  {
    return trapezoid.max_allowable_speed(accel, target_velocity, distance);
  }

  inline void trapezoid_or_s_curve_calculate_for_block(block_t *block, float entry_factor, float exit_factor)
  {
    trapezoid.calculate_for_block(block, entry_factor, exit_factor);
  }


#ifdef __cplusplus
} // extern "C" {
#endif

}
#endif // GRBL_ACC_DEC_H








