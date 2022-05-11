
/* The ring buffer implementation gleaned from the wiring_serial library by David A. Mellis. */

/*
 Reasoning behind the mathematics in this module (in the key of 'Mathematica'):

 s == speed, a == acceleration, t == time, d == distance

 Basic definitions:

 Speed[s_, a_, t_] := s + (a*t)
 Travel[s_, a_, t_] := Integrate[Speed[s, a, t], t]

 Distance to reach a specific speed with a constant acceleration:

 Solve[{Speed[s, a, t] == m, Travel[s, a, t] == d}, d, t]
 d -> (m^2 - s^2)/(2 a) --> estimate_acceleration_distance()

 Speed after a given distance of travel with constant acceleration:

 Solve[{Speed[s, a, t] == m, Travel[s, a, t] == d}, m, t]
 m -> Sqrt[2 a d + s^2]

 DestinationSpeed[s_, a_, d_] := Sqrt[2 a d + s^2]

 When to start braking (di) to reach a specified destionation speed (s2) after accelerating
 from initial speed s1 without ever stopping at a plateau:

 Solve[{DestinationSpeed[s1, a, di] == DestinationSpeed[s2, a, d - di]}, di]
 di -> (2 a d - s1^2 + s2^2)/(4 a) --> intersection_distance()

 IntersectionDistance[s1_, s2_, a_, d_] := (2 a d - s1^2 + s2^2)/(4 a)
 */
#include "user_common_cpp.h"
#include "../../Grbl/Inc/grbl_acc_dec.h"

namespace sg_grbl
{

#ifdef __cplusplus
  extern "C" {
#endif

  //===========================================================================
  //=============================public variables ============================
  //===========================================================================

  //===========================================================================
  //=================semi-private variables, used in inline  functions    =====
  //===========================================================================
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
  float square(float x)
  {
    float result = 0.0f;
    arm_power_f32(&x, 2, &result);
    return result;
  }
#else
#define square(x) powf(x, 2)
#endif

  //===========================================================================
  //=============================private variables ============================
  //===========================================================================
  // The current position of the tool in absolute steps
  static float plan_position[MAX_NUM_AXIS];   //rescaled from extern when axis_steps_per_unit are changed by gcode
  static float plan_target[MAX_NUM_AXIS];
  static float previous_speed[MAX_NUM_AXIS]; // Speed of previous path line segment
  static float previous_nominal_speed; // Nominal speed of previous path line segment



  //===========================================================================
  //=============================private function =============================
  //===========================================================================

  // Returns the index of the next block in the ring buffer
  // NOTE: Removed modulo (%) operator, which uses an expensive divide and multiplication.
  static int8_t next_block_index(int8_t block_index)
  {
    block_index++;

    if (block_index == BLOCK_BUFFER_SIZE)
    {
      block_index = 0;
    }

    return (block_index);
  }

  // Returns the index of the previous block in the ring buffer
  static int8_t prev_block_index(int8_t block_index)
  {
    if (block_index == 0)
    {
      block_index = BLOCK_BUFFER_SIZE;
    }

    block_index--;
    return (block_index);
  }

  // Calculates trapezoid parameters so that the entry- and exit-speed is compensated by the provided factors.
  // The kernel called by planner_recalculate() when scanning the plan from last to first entry.
  //void planner_reverse_pass_kernel(block_t *previous, block_t *current, block_t *next)
  static void planner_reverse_pass_kernel(block_t *current, const block_t *next)
  {
    if (!current)
    {
      return;
    }

    if (next)
    {
      // If entry speed is already at the maximum entry speed, no need to recheck. Block is cruising.
      // If not, block in state of acceleration or deceleration. Reset entry speed to maximum and
      // check for maximum allowable speed reductions to ensure maximum possible planned speed.
      if (current->entry_speed != current->max_entry_speed)
      {
        // If nominal length true, max junction speed is guaranteed to be reached. Only compute
        // for max allowable speed if block is decelerating and nominal length is false.
        if ((!current->nominal_length_flag) && (current->max_entry_speed > next->entry_speed))
        {
          float max_allowable_speed_temp = trapezoid_or_s_curve_max_allowable_speed(current, -current->acceleration, next->entry_speed, current->millimeters);
          current->entry_speed = min(current->max_entry_speed, max_allowable_speed_temp);
        }
        else
        {
          current->entry_speed = current->max_entry_speed;
        }

        current->recalculate_flag = true;
      }
    } // Skip last block. Already initialized and set for recalculation.
  }

  // planner_recalculate() needs to go over the current plan twice. Once in reverse and once forward. This
  // implements the reverse pass.
  static void planner_reverse_pass()
  {
    uint8_t block_index = ccm_param.block_buffer_head;
    //Make a local copy of block_buffer_tail, because the interrupt can alter it
    TASK_ENTER_CRITICAL();
    unsigned char tail = ccm_param.block_buffer_tail;
    TASK_EXIT_CRITICAL();

    if ((((ccm_param.block_buffer_head - tail) + BLOCK_BUFFER_SIZE) & (BLOCK_BUFFER_SIZE - 1)) > 3)
    {
      block_index = (ccm_param.block_buffer_head - 3) & (BLOCK_BUFFER_SIZE - 1);
      block_t *block[3] =
      {
        NULL, NULL, NULL
      };

      while (block_index != tail)
      {
        block_index = (uint8_t)prev_block_index((int8_t)block_index);
        block[2] = block[1];
        block[1] = block[0];
        block[0] = (block_t *)&ccm_param.block_buffer[block_index];
        planner_reverse_pass_kernel(block[1], block[2]);
      }
    }
  }

  // The kernel called by planner_recalculate() when scanning the plan from first to last entry.
  //void planner_forward_pass_kernel(block_t *previous, block_t *current, block_t *next)
  static void planner_forward_pass_kernel(const block_t *previous, block_t *current)
  {
    if (!previous)
    {
      return;
    }

    // If the previous block is an acceleration block, but it is not long enough to complete the
    // full speed change within the block, we need to adjust the entry speed accordingly. Entry
    // speeds have already been reset, maximized, and reverse planned by reverse planner.
    // If nominal length is true, max junction speed is guaranteed to be reached. No need to recheck.
    if (!previous->nominal_length_flag)
    {
      if (previous->entry_speed < current->entry_speed)
      {
        float max_allowable_speed_temp = trapezoid_or_s_curve_max_allowable_speed((block_t *)previous, -previous->acceleration, previous->entry_speed, previous->millimeters);
        double entry_speed = min(current->entry_speed, max_allowable_speed_temp);

        // Check for junction speed change
        if (current->entry_speed != entry_speed)
        {
          current->entry_speed = (float)entry_speed;
          current->recalculate_flag = true;
        }
      }
    }
  }

  // planner_recalculate() needs to go over the current plan twice. Once in reverse and once forward. This
  // implements the forward pass.
  static void planner_forward_pass()
  {
    uint8_t block_index = ccm_param.block_buffer_tail;
    block_t *block[3] =
    {
      NULL, NULL, NULL
    };

    while (block_index != ccm_param.block_buffer_head)
    {
      block[0] = block[1];
      block[1] = block[2];
      block[2] = (block_t *)&ccm_param.block_buffer[block_index];
      planner_forward_pass_kernel(block[0], block[1]);
      block_index = (uint8_t)next_block_index((int8_t)block_index);
    }

    planner_forward_pass_kernel(block[1], block[2]);
  }

  // Recalculates the trapezoid speed profiles for all blocks in the plan according to the
  // entry_factor for each junction. Must be called by planner_recalculate() after
  // updating the blocks.
  static void planner_recalculate_trapezoids()
  {
    int8_t block_index = (int8_t)ccm_param.block_buffer_tail;
    block_t *current;
    block_t *next = NULL;

    while (block_index != ccm_param.block_buffer_head)
    {
      current = next;
      next = (block_t *)&ccm_param.block_buffer[block_index];

      if (current)
      {
        // Recalculate if current block entry or exit junction speed has changed.
        if (current->recalculate_flag || next->recalculate_flag)
        {
          // NOTE: Entry and exit factors always > 0 by all previous logic operations.
          trapezoid_or_s_curve_calculate_for_block(current, current->entry_speed / current->nominal_speed,
              next->entry_speed / current->nominal_speed);
          current->recalculate_flag = false; // Reset current only to ensure next trapezoid is computed
        }
      }

      block_index = next_block_index(block_index);
    }

    // Last/newest block in buffer. Exit speed is set with MINIMUM_PLANNER_SPEED. Always recalculated.
    if (next != NULL)
    {
      trapezoid_or_s_curve_calculate_for_block(next, next->entry_speed / next->nominal_speed,
          MINIMUM_PLANNER_SPEED / next->nominal_speed);
      next->recalculate_flag = false;
    }
  }

  // Recalculates the motion plan according to the following algorithm:
  //
  //   1. Go over every block in reverse order and calculate a junction speed reduction (i.e. block_t.entry_factor)
  //      so that:
  //     a. The junction jerk is within the set limit
  //     b. No speed reduction within one block requires faster deceleration than the one, true constant
  //        acceleration.
  //   2. Go over every block in chronological order and dial down junction speed reduction values if
  //     a. The speed increase within one block would require faster accelleration than the one, true
  //        constant acceleration.
  //
  // When these stages are complete all blocks have an entry_factor that will allow all speed changes to
  // be performed using only the one, true constant acceleration, and where no junction jerk is jerkier than
  // the set limit. Finally it will:
  //
  //   3. Recalculate trapezoids for all blocks.
  static void planner_recalculate(void)
  {
    planner_reverse_pass();
    planner_forward_pass();
    planner_recalculate_trapezoids();
  }

#ifdef PREVENT_DANGEROUS_EXTRUDE
  // 保护喷嘴，喷嘴温度小于最小温度限制时，EB轴不运动
  static void plan_prevent_dangerous_extrude(const float target, float &position, const int axis, const float extruder_temp)
  {
    if (target != position)
    {
      if (extruder_temp < ccm_param.t_sys.extrude_min_temp)
      {
        position = target;  //behave as if the move really took place, but ignore E part
      }

#ifdef PREVENT_LENGTHY_EXTRUDE

      if (fabs(target - position) > ccm_param.planner_settings.axis_steps_per_mm[axis]*ccm_param.t_model.extrude_maxlength)
      {
        position = target;  //behave as if the move really took place, but ignore E part
      }

#endif
    }
  }
#endif

  static void plan_init_target(float (&plan_buffer_position)[MAX_NUM_AXIS], const float extruder0_temp, const float extruder1_temp)
  {
    // The target position of the tool in absolute steps
    // Calculate target position in absolute steps
    // this should be done after the wait, because otherwise a M92 code within the gcode disrupts this calculation somehow
    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      plan_target[i] = lroundf(plan_buffer_position[i] * ccm_param.planner_settings.axis_steps_per_mm[i]);
    }

#ifdef PREVENT_DANGEROUS_EXTRUDE

    if (!ccm_param.t_sys.enable_board_test)
    {
      plan_prevent_dangerous_extrude(plan_target[E_AXIS], plan_position[E_AXIS], (int)E_AXIS, extruder0_temp);

      if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
      {
        if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
        {
          plan_prevent_dangerous_extrude(plan_target[B_AXIS], plan_position[B_AXIS], (int)B_AXIS, extruder1_temp);
        }
        else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX)
        {
          plan_prevent_dangerous_extrude(plan_target[B_AXIS], plan_position[B_AXIS], (int)B_AXIS, extruder0_temp);
        }
      }
    }

#endif
  }

  float plan_calc_millimeters_idex_get_x_or_z(float (&delta_mm)[MAX_NUM_AXIS], volatile float &millimeters, int axis, int aixs_2)
  {
    float result = delta_mm[axis];

    if (user_is_float_data_equ(result, 0.0f) && !user_is_float_data_equ(delta_mm[aixs_2], 0.0f))
    {
      result = delta_mm[aixs_2];
    }
    else if (!user_is_float_data_equ(result, 0.0f) && !user_is_float_data_equ(delta_mm[aixs_2], 0.0f) && fabs(result) < fabs(delta_mm[aixs_2]))
    {
      result = delta_mm[aixs_2];
    }

    return result;
  }

  void plan_calc_millimeters_idex(float (&delta_mm)[MAX_NUM_AXIS], volatile float &millimeters)
  {
    float delta_mm_x = plan_calc_millimeters_idex_get_x_or_z(delta_mm, millimeters, X_AXIS, X2_AXIS);
    float delta_mm_z = plan_calc_millimeters_idex_get_x_or_z(delta_mm, millimeters, Z_AXIS, Z2_AXIS);
    float length = square(delta_mm_x) + square(delta_mm[Y_AXIS]) + square(delta_mm_z);
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
    arm_sqrt_f32(length, (float *)&millimeters);
#else
    millimeters = sqrtf(length);
#endif
  }

  // 计算各轴需要移动位移
  static void plan_calc_millimeters(float (&delta_mm)[MAX_NUM_AXIS], block_t *block, volatile float &millimeters, const volatile int &extrudemultiply)
  {
    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      delta_mm[i] = (float)(plan_target[i] - plan_position[i]) / ccm_param.planner_settings.axis_steps_per_mm[i];

      if (E_AXIS == i || B_AXIS == i)
      {
        delta_mm[i] = delta_mm[i] * extrudemultiply / 100.0f;
      }
    }

    // Prepare to set up new block
    bool is_x2_leq_dropsegments = true;

    if ((feature_print_control::idex_sys_is_extruder()) ||
        flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
        flash_param_t.extruder_type == EXTRUDER_TYPE_LASER) // idex 双头
    {
      is_x2_leq_dropsegments = (block->steps_axis[X2_AXIS] <= ccm_param.planner_settings.drop_segments);
    }

    if (block->steps_axis[X_AXIS] <= ccm_param.planner_settings.drop_segments && block->steps_axis[Y_AXIS] <= ccm_param.planner_settings.drop_segments &&
        block->steps_axis[Z_AXIS] <= ccm_param.planner_settings.drop_segments && block->steps_axis[Z2_AXIS] <= ccm_param.planner_settings.drop_segments && is_x2_leq_dropsegments)
    {
      millimeters = fabs(delta_mm[E_AXIS]) + (ccm_param.t_sys_data_current.enable_color_mixing ? fabs(delta_mm[B_AXIS]) : 0);
    }
    else
    {
      if (feature_print_control::idex_sys_is_extruder()) // idex 双头
      {
        if (feature_print_control::idex_sys_is_normal())
        {
          plan_calc_millimeters_idex(delta_mm, millimeters);
        }
        else if (feature_print_control::idex_sys_is_copy_or_mirror())
        {
          if (user_is_float_data_equ(delta_mm[X_AXIS], delta_mm[X2_AXIS])) // 复制、镜像模式开始
          {
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
            float length = square(delta_mm[X_AXIS]) + square(delta_mm[Y_AXIS]) + square(delta_mm[Z_AXIS]);
            arm_sqrt_f32(length, (float *)&millimeters);
#else
            millimeters = sqrtf(square(delta_mm[X_AXIS]) + square(delta_mm[Y_AXIS]) + square(delta_mm[Z_AXIS]));
#endif
          }
          else
          {
            plan_calc_millimeters_idex(delta_mm, millimeters); // 执行归零操作
          }
        }
        else
        {
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
          float length = square(delta_mm[X_AXIS]) + square(delta_mm[Y_AXIS]) + square(delta_mm[Z_AXIS]);
          arm_sqrt_f32(length, (float *)&millimeters);
#else
          millimeters = sqrtf(square(delta_mm[X_AXIS]) + square(delta_mm[Y_AXIS]) + square(delta_mm[Z_AXIS]));
#endif
        }
      }
      else if (flash_param_t.extruder_type == EXTRUDER_TYPE_MIX || flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
      {
        plan_calc_millimeters_idex(delta_mm, millimeters);
      }
      else
      {
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
        float length = square(delta_mm[X_AXIS]) + square(delta_mm[Y_AXIS]) + square(delta_mm[Z_AXIS]);
        arm_sqrt_f32(length, (float *)&millimeters);
#else
        millimeters = sqrtf(square(delta_mm[X_AXIS]) + square(delta_mm[Y_AXIS]) + square(delta_mm[Z_AXIS]));
#endif
      }
    }
  }

  static void plan_calc_direction_bits(volatile unsigned char &direction_bits)
  {
    direction_bits = 0;

    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (plan_target[i] < plan_position[i])
      {
        direction_bits |= (1 << (int)i);
      }
    }
  }

  static float plan_calc_safe_speed(float (&current_speed)[MAX_NUM_AXIS])
  {
    float safe_speed = ccm_param.planner_settings.max_xy_jerk / 2;

    if (fabs(current_speed[Z_AXIS]) > ccm_param.planner_settings.max_z_jerk / 2)
    {
      safe_speed = min(safe_speed, ccm_param.planner_settings.max_z_jerk / 2);
    }

    if (fabs(current_speed[Z2_AXIS]) > ccm_param.planner_settings.max_z_jerk / 2)
    {
      safe_speed = min(safe_speed, ccm_param.planner_settings.max_z_jerk / 2);
    }

    if (fabs(current_speed[E_AXIS]) > ccm_param.planner_settings.max_e_jerk / 2)
    {
      safe_speed = min(safe_speed, ccm_param.planner_settings.max_e_jerk / 2);
    }

    if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
    {
      if (fabs(current_speed[B_AXIS]) > ccm_param.planner_settings.max_b_jerk / 2)
      {
        safe_speed = min(safe_speed, ccm_param.planner_settings.max_b_jerk / 2);
      }
    }

    return safe_speed;
  }

  static void plan_calc_safe_speed(float (&current_speed)[MAX_NUM_AXIS], int moves_queued, float safe_speed, volatile float nominal_speed, volatile float &max_entry_speed)
  {
    float vmax_junction = safe_speed;
    float vmax_junction_factor = 1.0f;

    if ((moves_queued > 1) && (previous_nominal_speed > 0.0001f))
    {
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
      float jerk = 0.0f;
      float length = square((current_speed[X_AXIS] - previous_speed[X_AXIS])) + square((current_speed[Y_AXIS] - previous_speed[Y_AXIS]));
      arm_sqrt_f32(length, (float *)&jerk);
#else
      float jerk = sqrtf(square((current_speed[X_AXIS] - previous_speed[X_AXIS])) + square((current_speed[Y_AXIS] - previous_speed[Y_AXIS])));
#endif
      //    if((fabs(previous_speed[X_AXIS]) > 0.0001) || (fabs(previous_speed[Y_AXIS]) > 0.0001)) {
      vmax_junction = nominal_speed;

      //    }
      if (jerk > ccm_param.planner_settings.max_xy_jerk)
      {
        vmax_junction_factor = (ccm_param.planner_settings.max_xy_jerk / jerk);
      }

      if (feature_print_control::idex_sys_is_extruder()) // idex 双头
      {
#if defined(ENABLE_ARM_MATH) and defined(__USE_C99_MATH)
        float jerk2 = 0.0f;
        float length = square((current_speed[X2_AXIS] - previous_speed[X2_AXIS])) + square((current_speed[Y_AXIS] - previous_speed[Y_AXIS]));
        arm_sqrt_f32(length, (float *)&jerk2);
#else
        float jerk2 = sqrtf(square((current_speed[X2_AXIS] - previous_speed[X2_AXIS])) + square((current_speed[Y_AXIS] - previous_speed[Y_AXIS])));
#endif

        if (jerk2 > ccm_param.planner_settings.max_xy_jerk)
        {
          vmax_junction_factor = min(vmax_junction_factor, (ccm_param.planner_settings.max_xy_jerk / jerk2));
        }
      }

      if (fabs(current_speed[Z_AXIS] - previous_speed[Z_AXIS]) > ccm_param.planner_settings.max_z_jerk)
      {
        double ZSpeedDiffVal = fabs(current_speed[Z_AXIS] - previous_speed[Z_AXIS]);
        vmax_junction_factor = min(vmax_junction_factor, (ccm_param.planner_settings.max_z_jerk / ZSpeedDiffVal));
      }

      if (fabs(current_speed[Z2_AXIS] - previous_speed[Z2_AXIS]) > ccm_param.planner_settings.max_z_jerk)
      {
        double ZSpeedDiffVal = fabs(current_speed[Z2_AXIS] - previous_speed[Z2_AXIS]);
        vmax_junction_factor = min(vmax_junction_factor, (ccm_param.planner_settings.max_z_jerk / ZSpeedDiffVal));
      }

      if (fabs(current_speed[E_AXIS] - previous_speed[E_AXIS]) > ccm_param.planner_settings.max_e_jerk)
      {
        double ESpeedDiffVal = fabs(current_speed[E_AXIS] - previous_speed[E_AXIS]);
        vmax_junction_factor = min(vmax_junction_factor, (ccm_param.planner_settings.max_e_jerk / ESpeedDiffVal));
      }

      if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
      {
        if (fabs(current_speed[B_AXIS] - previous_speed[B_AXIS]) > ccm_param.planner_settings.max_b_jerk)
        {
          double BSpeedDiffVal = fabs(current_speed[B_AXIS] - previous_speed[B_AXIS]);
          vmax_junction_factor = min(vmax_junction_factor, (ccm_param.planner_settings.max_b_jerk / BSpeedDiffVal));
        }
      }

      vmax_junction = min(previous_nominal_speed, vmax_junction * vmax_junction_factor); // Limit speed to max previous speed
    }

    max_entry_speed = vmax_junction;
  }

  static void plan_calc_acceleration(block_t *block)
  {
    // 计算加速度（step/s2）
    // 如果XYZ步进数都为0，加速度取retract_acceleration；不为0时，取加速度最小轴
    // steps_per_mm = step_event_count / millimeters，单位（step/mm）
    float steps_per_mm = block->step_event_count / block->millimeters;
    bool is_x2_step_axis_zero = true;

    if (feature_print_control::idex_sys_is_extruder()) // idex 双头
    {
      is_x2_step_axis_zero = (block->steps_axis[X2_AXIS] == 0);
    }

    if (block->steps_axis[X_AXIS] == 0 && block->steps_axis[Y_AXIS] == 0 && block->steps_axis[Z_AXIS] == 0 && block->steps_axis[Z2_AXIS] == 0 && is_x2_step_axis_zero)
    {
      block->acceleration_st = (unsigned long)ceil(ccm_param.planner_settings.retract_acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
    }
    else
    {
      block->acceleration_st = (unsigned long)ceil(ccm_param.planner_settings.acceleration * steps_per_mm); // convert to: acceleration steps/sec^2

      // Limit acceleration per axis
      for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
      {
        if (((float)block->acceleration_st * (float)block->steps_axis[i] / (float)block->step_event_count) > ccm_param.planner_settings.axis_steps_per_sqr_second[i])
        {
          block->acceleration_st = ccm_param.planner_settings.axis_steps_per_sqr_second[i];
        }
      }
    }

    block->acceleration = block->acceleration_st / steps_per_mm;
#if !defined(S_CURVE_ACCELERATION)
    block->acceleration_rate = (long)((float)block->acceleration_st * (16777216.0f / ((float)F_CPU / 8.0f)));
#endif
  }

  // 计算各轴步进数及总步进数
  // 初始化各轴给进步进数（step），总步进数step_event_count取最大步进数轴
  static void plan_init_steps(block_t *block, const volatile int &extrudemultiply)
  {
    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      block->steps_axis[i] = lroundf(fabsf(plan_target[i] - plan_position[i]));

      if (E_AXIS == i || B_AXIS == i)
      {
        block->steps_axis[i] = block->steps_axis[i] * extrudemultiply / 100.0f;
      }

      if (X_AXIS == i)
      {
        block->step_event_count = block->steps_axis[i];
      }
      else
      {
        block->step_event_count = (unsigned long)max(block->step_event_count, block->steps_axis[i]);
      }
    }
  }

  static void plan_calc_nominal_speed_and_rate(block_t *block, float (&current_speed)[MAX_NUM_AXIS], float (&delta_mm)[MAX_NUM_AXIS], int moves_queued, float feed_rate)
  {
    // Calculate speed in mm/second for each axis. No divide by zero due to previous checks.
    float inverse_millimeters = 1.0f / block->millimeters; // Inverse millimeters to remove multiple divides
    float inverse_second = feed_rate * inverse_millimeters;
    // slow down when de buffer starts to empty, rather than wait at the corner for a buffer refill
#ifdef SLOWDOWN
    //  segment time im micro seconds
    unsigned long segment_time = (unsigned long)lroundf(1000000.0f / inverse_second);

    if ((moves_queued > 1) && (moves_queued < (BLOCK_BUFFER_SIZE * 0.5f)))
    {
      if (segment_time < ccm_param.planner_settings.min_segment_time_us)
      {
        // buffer is draining, add extra time.  The amount of time added increases if the buffer is still emptied more.
        inverse_second = 1000000.0f / (segment_time + (unsigned long)lroundf(2.0f *
                                       (ccm_param.planner_settings.min_segment_time_us - segment_time) / moves_queued));
      }
    }

#endif // END OF SLOW DOWN SECTION
    block->nominal_speed = block->millimeters * inverse_second; // (mm/sec) Always > 0
    block->nominal_rate = (unsigned long)ceil(block->step_event_count * inverse_second); // (step/sec) Always > 0
    // Calculate and limit speed in mm/sec for each axis
    float speed_factor = 1.0f; //factor <=1 do decrease speed

    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      current_speed[i] = delta_mm[i] * inverse_second;

      if (fabs(current_speed[i]) > ccm_param.planner_settings.max_feedrate_mm_s[i])
      {
        double fabs_current_speed = fabs(current_speed[i]);
        speed_factor = min(speed_factor, ccm_param.planner_settings.max_feedrate_mm_s[i] / fabs_current_speed);
      }
    }

    // Correct the speed
    if (speed_factor < 1.0f)
    {
      for (unsigned char i = 0; i < ccm_param.t_sys.axis_num; i++)
      {
        current_speed[i] *= speed_factor;
      }

      block->nominal_speed *= speed_factor;
      block->nominal_rate = (unsigned long)(block->nominal_rate * speed_factor);
    }
  }

  static void plan_init_feed_rate(block_t *block, volatile float &feed_rate)
  {
    if (block->steps_axis[E_AXIS] == 0 || block->steps_axis[B_AXIS] == 0)
    {
      if (feed_rate < ccm_param.planner_settings.min_travel_feedrate_mm_s)
      {
        feed_rate = ccm_param.planner_settings.min_travel_feedrate_mm_s;
      }
    }
    else
    {
      if (feed_rate < ccm_param.planner_settings.min_feedrate_mm_s)
      {
        feed_rate = ccm_param.planner_settings.min_feedrate_mm_s;
      }
    }
  }




  //===========================================================================
  //============================= public function =============================
  //===========================================================================

  void planner_init(void)
  {
    ccm_param.block_buffer_head = 0;
    ccm_param.block_buffer_tail = 0;
    (void)memset(plan_position, 0, sizeof(plan_position)); // clear plan_position
    (void)memset(plan_target, 0, sizeof(plan_target)); // clear plan_position

    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      previous_speed[i] = 0.0f;
    }

    previous_nominal_speed = 0.0f;
  }

  void planner_buffer_line(planner_running_status_t *running_status, bool is_sync_z)
  {
    int next_buffer_head;
    float plan_buffer_position_tmp[MAX_NUM_AXIS] ;

    for (int i = 0; i < MAX_NUM_AXIS; i++)
    {
      plan_buffer_position_tmp[i] = running_status->axis_position[i];
    }

    plan_buffer_line_get_xyz(plan_buffer_position_tmp);

    if (is_sync_z)
    {
      plan_buffer_position_tmp[Z2_AXIS] = plan_buffer_position_tmp[Z_AXIS];
    }

    // Calculate the buffer head after we push this byte
    next_buffer_head = next_block_index((int8_t)ccm_param.block_buffer_head);

    // If the buffer is full: good! That means we are well ahead of the robot.
    // Rest here until there is room in the buffer.
    while (ccm_param.block_buffer_tail == next_buffer_head)
    {
      (void)OS_DELAY(10);
    }

    // 初始化目标位置
    plan_init_target(plan_buffer_position_tmp, running_status->target_extruder0_temp, running_status->target_extruder1_temp);

    // 初始化运行状态值，断电续打
    for (int i = 0; i < MAX_NUM_AXIS; i++)
    {
      running_status->axis_real_position[i] = plan_buffer_position_tmp[i];
    }

    memcpy((char *)&ccm_param.runningStatus[ccm_param.block_buffer_head], running_status, sizeof(planner_running_status_t));
    // Prepare to set up new block
    block_t *block = (block_t *)&ccm_param.block_buffer[ccm_param.block_buffer_head];
    // default non-h-bot planning
    plan_init_steps(block, running_status->extruder_multiply);

    // 如果总步进数脉冲小于5，跳过当前指令
    // Bail if this is a zero-length block
    if ((block->step_event_count <= ccm_param.planner_settings.drop_segments))
    {
      return;
    }

    // Mark block as not busy (Not executed by the stepper interrupt)
    block->busy = false;
    block->active_extruder = running_status->active_extruder;
    // Compute direction bits for this block
    plan_calc_direction_bits(block->direction_bits);

    //enable active axes
    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (block->steps_axis[i] != 0)
      {
        user_motor_axis_enable(i, true);
      }

      if (!ccm_param.t_sys_data_current.enable_color_mixing && E_AXIS == i)
      {
        break;
      }
    }

    plan_init_feed_rate(block, running_status->feed_rate);
    float delta_mm[MAX_NUM_AXIS];
    // 初始化各轴移动位移
    plan_calc_millimeters(delta_mm, block, block->millimeters, running_status->extruder_multiply);
    float current_speed[MAX_NUM_AXIS];
    int moves_queued = ((ccm_param.block_buffer_head - ccm_param.block_buffer_tail) + BLOCK_BUFFER_SIZE) & (BLOCK_BUFFER_SIZE - 1);
    plan_calc_nominal_speed_and_rate(block, current_speed, delta_mm, moves_queued, running_status->feed_rate);
    // Start with a safe speed
    float safe_speed = plan_calc_safe_speed(current_speed);
    safe_speed = min(safe_speed, block->nominal_speed);
    plan_calc_safe_speed(current_speed, moves_queued, safe_speed, block->nominal_speed, block->max_entry_speed);
    plan_calc_acceleration(block);
    // Initialize block entry speed. Compute based on deceleration to user-defined MINIMUM_PLANNER_SPEED.
    double v_allowable = trapezoid_or_s_curve_max_allowable_speed(block, -block->acceleration, MINIMUM_PLANNER_SPEED, block->millimeters);
    block->entry_speed = (float)min(block->max_entry_speed, v_allowable);

    // Initialize planner efficiency flags
    // Set flag if block will always reach maximum junction speed regardless of entry/exit speeds.
    // If a block can de/ac-celerate from nominal speed to zero within the length of the block, then
    // the current block and next block junction speeds are guaranteed to always be at their maximum
    // junction speeds in deceleration and acceleration, respectively. This is due to how the current
    // block nominal speed limits both the current and next maximum junction speeds. Hence, in both
    // the reverse and forward planners, the corresponding block junction speed will always be at the
    // the maximum junction speed and may always be ignored for any speed reduction checks.
    if (block->nominal_speed <= v_allowable)
    {
      block->nominal_length_flag = true;
    }
    else
    {
      block->nominal_length_flag = false;
    }

    block->recalculate_flag = true; // Always calculate trapezoid for new block
    // Update previous path unit_vector and nominal speed
    (void)memcpy(previous_speed, current_speed, sizeof(previous_speed)); // previous_speed[] = current_speed[]
    previous_nominal_speed = block->nominal_speed;
    //calc_adv_rate();
    trapezoid_or_s_curve_calculate_for_block(block, block->entry_speed / block->nominal_speed,
        safe_speed / block->nominal_speed);
    // Move buffer head
    ccm_param.block_buffer_head = (unsigned char)next_buffer_head;
    // Update plan_position
    (void)memcpy(plan_position, plan_target, sizeof(plan_target));
    // 前瞻规划
    planner_recalculate();
    st_wake_up();
  }

  void planner_set_position_basic(const volatile float *_current_position, bool is_sync_z)
  {
    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
      {
        if (i == (int)B_AXIS && _current_position[(int)B_AXIS] == -1)
        {
          continue;
        }
      }

      plan_position[i] = lroundf(_current_position[i] * ccm_param.planner_settings.axis_steps_per_mm[i]);
    }

    plan_set_position_get_xyz(_current_position, plan_position, is_sync_z);
    st_set_position(plan_position);
    previous_nominal_speed = 0.0; // Resets planner junction speeds. Assumes start from rest.

    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      previous_speed[i] = 0.0f;
    }
  }

  void planner_set_position(const volatile float *_current_position)
  {
    planner_set_position_basic(_current_position, true);
  }

  void planner_set_axis_position_basic(const volatile float &value, const int axis, bool is_sync_z)
  {
    plan_position[axis] = lroundf(value * ccm_param.planner_settings.axis_steps_per_mm[axis]);

    if (is_sync_z == true && axis == Z_AXIS)
    {
      plan_position[Z2_AXIS] = lroundf(value * ccm_param.planner_settings.axis_steps_per_mm[Z2_AXIS]);
    }

    st_set_position_axis(plan_position[axis], axis);
  }

  void planner_set_axis_position(const volatile float &value, const int axis)
  {
    planner_set_axis_position_basic(value, axis, true);
  }




  // Returns true if the buffer has a queued block, false otherwise
  bool planner_blocks_queued(void)
  {
    return (ccm_param.block_buffer_head != ccm_param.block_buffer_tail);
  }

  //return the nr of buffered moves
  int planner_moves_planned(void)
  {
    return ((ccm_param.block_buffer_head - ccm_param.block_buffer_tail) + BLOCK_BUFFER_SIZE) & (BLOCK_BUFFER_SIZE - 1);
  }

  //1.队空条件：block_buffer_head==block_buffer_tail
  //2.队满条件：(block_buffer_head+1) %BLOCK_BUFFER_SIZE==block_buffer_tail，其中BLOCK_BUFFER_SIZE为循环队列的最大长度
  //3.计算队列长度：（block_buffer_head-block_buffer_tail+BLOCK_BUFFER_SIZE）%BLOCK_BUFFER_SIZE
  //4.入队：（block_buffer_head+1）%BLOCK_BUFFER_SIZE
  //5.出队：（block_buffer_tail+1）%BLOCK_BUFFER_SIZE
  bool is_planner_moves_planned_full(void)
  {
    return ((ccm_param.block_buffer_head - ccm_param.block_buffer_tail + BLOCK_BUFFER_SIZE) & (BLOCK_BUFFER_SIZE - 1)) < BLOCK_BUFFER_SIZE - 1;
  }
#ifdef __cplusplus
} //extern "C" {
#endif

}



