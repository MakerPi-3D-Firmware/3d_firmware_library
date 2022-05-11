#include "user_common_cpp.h"
#include "../../Grbl/Inc/grbl_acc_dec.h"

namespace sg_grbl
{

#ifdef __cplusplus
  extern "C" {
#endif

#define NOLESS(v,n) do{ if (v < n) v = n; }while(0)
#define NOMORE(v,n) do{ if (v > n) v = n; }while(0)

#define sq(x)  (x*x)

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif // min

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif // max

  // intRes = longIn1 * longIn2 >> 24
  // uses:
  // r26 to store 0
  // r27 to store bits 16-23 of the 48bit result. The top bit is used to round the two byte result.
  // note that the lower two bytes and the upper byte of the 48bit result are not calculated.
  // this can cause the result to be out by one as the lower bytes may cause carries into the upper ones.
  // B0 A0 are bits 24-39 and are the returned value
  // C1 B1 A1 is longIn1
  // D2 C2 B2 A2 is longIn2
  //
#define MultiU24X32toH16(intRes, longIn1, longIn2)  intRes = (uint16_t)((((uint64_t)longIn1 * (uint64_t)longIn2)>> 24)& 0XFFFF);

  //         __________________________
  //        /|                        |\     _________________         ^
  //       / |                        | \   /|               |\        |
  //      /  |                        |  \ / |               | \       s
  //     /   |                        |   |  |               |  \      p
  //    /    |                        |   |  |               |   \     e
  //   +-----+------------------------+---+--+---------------+----+    e
  //   |               BLOCK 1            |      BLOCK 2          |    d
  //
  //                           time ----->
  //
  //  The trapezoid is the shape the speed curve over time. It starts at block->initial_rate, accelerates
  //  first block->accelerate_until step_events_completed, then keeps going at constant speed until
  //  step_events_completed reaches block->decelerate_after after which it decelerates until the trapezoid generator is reset.
  //  The slope of acceleration is calculated with the leib ramp alghorithm.
  static inline unsigned short calc_timer(unsigned short step_rate, volatile char &step_loops)
  {
    unsigned short timer;
    NOMORE(step_rate, MAX_STEP_FREQUENCY);

    if (step_rate > 20000)
    {
      // If steprate > 20kHz >> step 4 times
      step_rate = (step_rate >> 2) & 0x3fff;
      step_loops = 4;
    }
    else if (step_rate > 10000)
    {
      // If steprate > 10kHz >> step 2 times
      step_rate = (step_rate >> 1) & 0x7fff;
      step_loops = 2;
    }
    else
    {
      step_loops = 1;
    }

    //  if (true)
    //  {
    //    timer = (uint16_t)(2000000 / step_rate - 1 + step_loops * 3 * 2);
    //  }
    //  else
    {
#ifdef USE_PULSE_DELAY

      if (K600 == ccm_param.t_sys_data_current.model_id)
      {
        timer = (uint16_t)(2000000 / step_rate - 1 + step_loops * XYZ_NUM_AXIS * 30 + XYZ_NUM_AXIS * 30);
      }
      else if (F400TP == ccm_param.t_sys_data_current.model_id)
      {
        timer = (uint16_t)(2000000 / step_rate - 1 + step_loops * XYZ_NUM_AXIS * 60 + XYZ_NUM_AXIS * 80);
      }
      else if (Drug == ccm_param.t_sys_data_current.model_id)
      {
        timer = (uint16_t)(2000000 / step_rate - 1 + step_loops * XYZ_NUM_AXIS * 30 + XYZ_NUM_AXIS * 30);
      }
      else if (ccm_param.t_sys_data_current.model_id == F300TP)
      {
        timer = (uint16_t)(2000000 / step_rate - 1 + step_loops * XYZ_NUM_AXIS * 6 + XYZ_NUM_AXIS * 8);
      }
      else
      {
        timer = (uint16_t)(2000000 / step_rate - 1 + step_loops * XYZ_NUM_AXIS * 4 + XYZ_NUM_AXIS * 4);
      }

#else
      timer = (uint16_t)(2000000 / step_rate - 1);
#endif
    }

    if (timer < 100) //(20kHz this should never happen)
    {
      timer = 100;
    }

    return timer;
  }

#if defined(S_CURVE_ACCELERATION)

  // All other 32-bit MPUs can easily do inverse using hardware division,
  // so we don't need to reduce precision or to use assembly language at all.
  // This routine, for all other archs, returns 0x100000000 / d ~= 0xFFFFFFFF / d
  static __inline uint32_t get_period_inverse(const uint32_t d)
  {
    return d ? 0xFFFFFFFF / d : 0xFFFFFFFF;
  }

#endif

#ifdef __cplusplus
} // extern "C" {
#endif

Trapezoid::Trapezoid()
{
  acceleration_time = 0;
  deceleration_time = 0;
  acc_step_rate = 0; // needed for deccelaration start point
  OCR1A_nominal = 0;
  step_loops_nominal = 0;
}

#if defined(S_CURVE_ACCELERATION)
/**
 * Calculate the speed reached given initial speed, acceleration and distance
 */
static float final_speed(const float initial_velocity, const float accel, const float distance)
{
  return sqrtf((initial_velocity * initial_velocity) + 2 * accel * distance);
}
#endif

void Trapezoid::calculate_for_block(block_t *block, float entry_factor, float exit_factor)
{
  unsigned long initial_rate = (unsigned long)ceil(block->nominal_rate * entry_factor); // (step/min)
  unsigned long final_rate = (unsigned long)ceil(block->nominal_rate * exit_factor); // (step/min)
  // Limit minimal step rate (Otherwise the timer will overflow.)
  NOLESS(initial_rate, 120);
  NOLESS(final_rate, 120);
#if defined(S_CURVE_ACCELERATION)
  uint32_t cruise_rate = initial_rate;
#endif
  long accel = block->acceleration_st;
  int32_t accelerate_steps = ceil(estimate_acceleration_distance(initial_rate, block->nominal_rate, accel));
  int32_t decelerate_steps = floor(estimate_acceleration_distance(block->nominal_rate, final_rate, -accel));
  // Calculate the size of Plateau of Nominal Rate.
  int32_t plateau_steps = block->step_event_count - accelerate_steps - decelerate_steps;

  // Is the Plateau of Nominal Rate smaller than nothing? That means no cruising, and we will
  // have to use intersection_distance() to calculate when to abort accel and start braking
  // in order to reach the final_rate exactly at the end of this block.
  if (plateau_steps < 0)
  {
    accelerate_steps = ceil(intersection_distance(initial_rate, final_rate, accel, block->step_event_count));
    accelerate_steps = max(accelerate_steps, 0); // Check limits due to numerical round-off
    accelerate_steps = min((uint32_t)accelerate_steps, block->step_event_count);//(We can cast here to unsigned, because the above line ensures that we are above zero)
    plateau_steps = 0;
#if defined(S_CURVE_ACCELERATION)
    // We won't reach the cruising rate. Let's calculate the speed we will reach
    cruise_rate = final_speed(initial_rate, accel, accelerate_steps);
#endif
  }

#if defined(S_CURVE_ACCELERATION)
  else // We have some plateau time, so the cruise rate will be the nominal rate
  {
    cruise_rate = block->nominal_rate;
  }

  // Jerk controlled speed requires to express speed versus time, NOT steps
  uint32_t acceleration_time = ((float)(cruise_rate - initial_rate) / accel) * (2000000),
           deceleration_time = ((float)(cruise_rate - final_rate) / accel) * (2000000),
           // And to offload calculations from the ISR, we also calculate the inverse of those times here
           acceleration_time_inverse = get_period_inverse(acceleration_time),
           deceleration_time_inverse = get_period_inverse(deceleration_time);
#endif
  TASK_ENTER_CRITICAL();

  if (!block->busy)
  {
    // Don't update variables if block is busy.
    block->accelerate_until = accelerate_steps;
    block->decelerate_after = accelerate_steps + plateau_steps;
    block->initial_rate = initial_rate;
#if defined(S_CURVE_ACCELERATION)
    block->acceleration_time = acceleration_time;
    block->deceleration_time = deceleration_time;
    block->acceleration_time_inverse = acceleration_time_inverse;
    block->deceleration_time_inverse = deceleration_time_inverse;
    block->cruise_rate = cruise_rate;
#endif
    block->final_rate = final_rate;
  }

  TASK_EXIT_CRITICAL();
}

/**
 * Calculate the distance (not time) it takes to accelerate
 * from initial_rate to target_rate using the given acceleration:
 */
inline float Trapezoid::estimate_acceleration_distance(float initial_rate, float target_rate, float accel)
{
  if (accel == 0)
  {
    return 0;  // accel was 0, set acceleration distance to 0
  }

  return (sq(target_rate) - sq(initial_rate)) / (accel * 2);
}

/**
* Return the point at which you must start braking (at the rate of -'acceleration') if
* you start at 'initial_rate', accelerate (until reaching the point), and want to end at
* 'final_rate' after traveling 'distance'.
*
* This is used to compute the intersection point between acceleration and deceleration
* in cases where the "trapezoid" has no plateau (i.e., never reaches maximum speed)
*/
inline float Trapezoid::intersection_distance(float initial_rate, float final_rate, float accel, float distance)
{
  if (accel == 0)
  {
    return 0;  // accel was 0, set intersection distance to 0
  }

  return (accel * 2 * distance - sq(initial_rate) + sq(final_rate)) / (accel * 4);
}

// Initializes the trapezoid generator from the current block. Called whenever a new
// block begins.
void Trapezoid::generator_reset(block_t *st_current_block, volatile char &step_loops)
{
  deceleration_time = 0;
  // step_rate to timer interval
  OCR1A_nominal = calc_timer((unsigned short) st_current_block->nominal_rate, step_loops);
  // make a note of the number of step loops required at nominal speed
  step_loops_nominal = step_loops;
  acc_step_rate = (unsigned short)st_current_block->initial_rate;
  acceleration_time = calc_timer(acc_step_rate, step_loops);
  user_motor_timer_set_period((uint32_t)acceleration_time);
}

void Trapezoid::calculare_timer(block_t *st_current_block, volatile unsigned long step_events_completed, volatile char &step_loops)
{
  unsigned short timer;
  unsigned short step_rate;

  if (step_events_completed <= (unsigned long)st_current_block->accelerate_until)
  {
#if defined(S_CURVE_ACCELERATION)
    // Get the next speed to use (Jerk limited!)
    uint32_t acc_step_rate = acceleration_time < st_current_block->acceleration_time
                             ? _eval_bezier_curve(acceleration_time)
                             : st_current_block->cruise_rate;
#else
    MultiU24X32toH16(acc_step_rate, acceleration_time, st_current_block->acceleration_rate);
    acc_step_rate += st_current_block->initial_rate;
    // upper limit
    NOMORE(acc_step_rate, st_current_block->nominal_rate);
#endif
    // step_rate to timer interval
    timer = calc_timer(acc_step_rate, step_loops);
    user_motor_timer_set_period(timer);
    acceleration_time += timer;
  }
  else if (step_events_completed > (unsigned long)st_current_block->decelerate_after)
  {
#if defined(S_CURVE_ACCELERATION)

    // If this is the 1st time we process the 2nd half of the trapezoid...
    if (!bezier_2nd_half)
    {
      // Initialize the Bézier speed curve
      _calc_bezier_curve_coeffs(st_current_block->cruise_rate, st_current_block->final_rate, st_current_block->deceleration_time_inverse);
      bezier_2nd_half = true;
      // The first point starts at cruise rate. Just save evaluation of the Bézier curve
      step_rate = st_current_block->cruise_rate;
    }
    else
    {
      // Calculate the next speed to use
      step_rate = deceleration_time < st_current_block->deceleration_time
                  ? _eval_bezier_curve(deceleration_time)
                  : st_current_block->final_rate;
    }

#else
    MultiU24X32toH16(step_rate, deceleration_time, st_current_block->acceleration_rate);

    if (step_rate <= acc_step_rate)   // Still decelerating?
    {
      step_rate = acc_step_rate - step_rate;
      NOLESS(step_rate, st_current_block->final_rate);
    }
    else
    {
      step_rate = st_current_block->final_rate;
    }

#endif
    // step_rate to timer interval
    timer = calc_timer(step_rate, step_loops);
    user_motor_timer_set_period(timer);
    deceleration_time += timer;
  }
  else
  {
    user_motor_timer_set_period(OCR1A_nominal);
    // ensure we're running at the correct step rate, even if we just came off an acceleration
    step_loops = step_loops_nominal;
  }
}

float Trapezoid::max_allowable_speed(float accel, float target_velocity, float distance)
{
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
  float result = 0.0f;
  float length = sq(target_velocity) - 2 * accel * distance;
  arm_sqrt_f32(length, (float *)&result);
  return result;
#else
  return sqrtf(sq(target_velocity) - 2 * accel * distance);
#endif
}

Trapezoid trapezoid;


}




