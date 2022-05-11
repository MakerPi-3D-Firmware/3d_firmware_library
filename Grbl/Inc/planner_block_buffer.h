#ifndef PLANNER_BLOCK_BUFFER_H
#define PLANNER_BLOCK_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

// This struct is used when buffering the setup for each linear movement "nominal" values are as specified in
// the source g-code and may never actually be reached if acceleration management is active.
typedef struct
{
  // Fields used by the bresenham algorithm for tracing the line
  volatile long steps_axis[MAX_NUM_AXIS];
  volatile unsigned long step_event_count;           // The number of step events required to complete this block
  volatile long accelerate_until;                    // The index of the step event on which to stop acceleration
  volatile long decelerate_after;                    // The index of the step event on which to start decelerating
#if defined(S_CURVE_ACCELERATION)
  uint32_t cruise_rate,                   // The actual cruise rate to use, between end of the acceleration phase and start of deceleration phase
           acceleration_time,             // Acceleration time and deceleration time in STEP timer counts
           deceleration_time,
           acceleration_time_inverse,     // Inverse of acceleration and deceleration periods, expressed as integer. Scale depends on CPU being used
           deceleration_time_inverse;
#else
  volatile long acceleration_rate;                   // The acceleration rate used for acceleration calculation
#endif
  volatile unsigned char direction_bits;             // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)
  volatile unsigned char active_extruder;            // Selects the active extruder

  // Fields used by the motion planner to manage acceleration
  //  float speed_x, speed_y, speed_z, speed_e;        // Nominal mm/sec for each axis
  volatile float nominal_speed;                               // The nominal speed for this block in mm/sec
  volatile float entry_speed;                                 // Entry speed at previous-current junction in mm/sec
  volatile float max_entry_speed;                             // Maximum allowable junction entry speed in mm/sec
  volatile float millimeters;                                 // The total travel of this block in mm
  volatile float acceleration;                                // acceleration mm/sec^2
  volatile unsigned char recalculate_flag;                    // Planner flag to recalculate trapezoids on entry junction
  volatile unsigned char nominal_length_flag;                 // Planner flag for nominal speed always reached
  volatile char busy;

  // Settings for the trapezoid generator
  volatile unsigned long nominal_rate;                        // The nominal step rate for this block in step_events/sec
  volatile unsigned long initial_rate;                        // The jerk-adjusted step rate at start of block
  volatile unsigned long final_rate;                          // The minimal rate at exit
  volatile unsigned long acceleration_st;                     // acceleration steps/sec^2
} block_t;


#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PLANNER_BLOCK_BUFFER_H

