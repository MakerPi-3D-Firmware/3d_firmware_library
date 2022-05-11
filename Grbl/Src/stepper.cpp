#include "user_common_cpp.h"
#include "../../Grbl/Inc/grbl_acc_dec.h"

namespace sg_grbl
{

  // Variables used by The Stepper Driver Interrupt
  static block_t *st_current_block = NULL;                                /*!<  A pointer to the block currently being traced */
  static volatile bool save_axis_min_endstop[XYZ_NUM_AXIS] = {false};                /*!< 保存最小限位状态 */
  static volatile bool save_axis_max_endstop[XYZ_NUM_AXIS] = {false};                /*!< 保存最大限位状态 */
  static volatile bool check_endstops = false;                            /*!< 限位检测开关 */
  static volatile signed char count_direction[MAX_NUM_AXIS] = {0};        /*!< 计数方向*/
  static volatile float count_position[MAX_NUM_AXIS] = {0};                /*!< 记录当前位置 */
  static volatile float endstops_trigsteps[XYZ_NUM_AXIS] = {0};                       /*!< 记录限位位置 */
  static volatile long counter[MAX_NUM_AXIS] = {0};                       /*!< Counter variables for the bresenham line tracer */
  static volatile unsigned long step_events_completed = 0;                /*!< The number of step events executed in the current block */
  static volatile char step_loops = 0;
  static volatile bool check_endstop_z_hit_max = false;                   /*!< 检测z限位撞击最大限位状态 */
  static volatile bool check_endstop_z_hit_min = false;                   /*!< 检测z限位撞击最小限位状态 */
  static volatile bool endstop_z_hit_min = true;
  static volatile bool endstop_z_hit_max = true;

#if defined(S_CURVE_ACCELERATION)
  int32_t __attribute__((used)) bezier_A __asm__("bezier_A");    // A coefficient in Bézier speed curve with alias for assembler
  int32_t __attribute__((used)) bezier_B __asm__("bezier_B");    // B coefficient in Bézier speed curve with alias for assembler
  int32_t __attribute__((used)) bezier_C __asm__("bezier_C");    // C coefficient in Bézier speed curve with alias for assembler
  uint32_t __attribute__((used)) bezier_F __asm__("bezier_F");   // F coefficient in Bézier speed curve with alias for assembler
  uint32_t __attribute__((used)) bezier_AV __asm__("bezier_AV"); // AV coefficient in Bézier speed curve with alias for assembler
  bool bezier_2nd_half;    // =false If Bézier curve has been initialized or not
#endif
  //===========================================================================
  //=============================functions private ============================
  //===========================================================================

  // Called when the current block is no longer needed. Discards the block and makes the memory
  // availible for new blocks.
  static inline void plan_discard_current_block()
  {
    if (ccm_param.block_buffer_head != ccm_param.block_buffer_tail)
    {
      ccm_param.block_buffer_tail = (ccm_param.block_buffer_tail + 1) & (BLOCK_BUFFER_SIZE - 1);
    }
  }

  // Gets the current block. Returns NULL if buffer empty
  static inline block_t *plan_get_current_block()
  {
    if (ccm_param.block_buffer_head == ccm_param.block_buffer_tail)
    {
      return (NULL);
    }

    block_t *block = (block_t *)&ccm_param.block_buffer[ccm_param.block_buffer_tail];
    block->busy = true;
    return (block);
  }

#ifdef USE_PULSE_DELAY
#if 0
  static inline void stepper_delay_us(unsigned short time)
  {
    unsigned short i = 0;

    while (time--)
    {
      i = 10; //自己定义

      while (i--)
      {
        __NOP();
      }
    }
  }
#endif
#endif

  static inline void set_steps(void)
  {
    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      counter[i] += st_current_block->steps_axis[i];

      if (counter[i] > 0)
      {
        if (i < XYZ_NUM_AXIS)
        {
          if (K600 == ccm_param.t_sys_data_current.model_id)
          {
            user_delay_us(10);
          }
          else if (F400TP == ccm_param.t_sys_data_current.model_id)
          {
            user_delay_us(20);
          }
          else if (Drug == ccm_param.t_sys_data_current.model_id)
          {
            user_delay_us(10);
          }
          else if (ccm_param.t_sys_data_current.model_id == F300TP)
          {
            user_delay_us(2);
          }
        }

        user_motor_axis_write_step(i, HIGH);
        //      if (true)
        //      {
        //        for (int delay = 200; delay > 0; delay--);
        //        counter[i] -= st_current_block->step_event_count;
        //        count_position[i] += count_direction[i];
        //        user_motor_axis_write_step(i, LOW);
        //      }
        //      else
        {
#ifndef USE_PULSE_DELAY
          counter[i] -= st_current_block->step_event_count;
          count_position[i] += count_direction[i];
          user_motor_axis_write_step(i, LOW);
#else

          if (i < XYZ_NUM_AXIS)
          {
            if (K600 == ccm_param.t_sys_data_current.model_id)
            {
              user_delay_us(10);
            }
            else if (F400TP == ccm_param.t_sys_data_current.model_id)
            {
              user_delay_us(20);
            }
            else if (Drug == ccm_param.t_sys_data_current.model_id)
            {
              user_delay_us(10);
            }
            else if (ccm_param.t_sys_data_current.model_id == F300TP)
            {
              user_delay_us(2);
            }
            else
            {
              user_delay_us(2);
            }
          }

          counter[i] -= st_current_block->step_event_count;
          count_position[i] += count_direction[i];
          user_motor_axis_write_step(i, LOW);

          if (i < XYZ_NUM_AXIS)
          {
            if (K600 == ccm_param.t_sys_data_current.model_id)
            {
              user_delay_us(10);
            }
            else if (F400TP == ccm_param.t_sys_data_current.model_id)
            {
              user_delay_us(20);
            }
            else if (Drug == ccm_param.t_sys_data_current.model_id)
            {
              user_delay_us(10);
            }
            else if (ccm_param.t_sys_data_current.model_id == F300TP)
            {
              user_delay_us(2);
            }
            else
            {
              user_delay_us(2);
            }
          }

#endif
        }
      }
    }
  }

  static inline void set_dirs(void)
  {
    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      if ((st_current_block->direction_bits & (1 << i)) != 0)
      {
        // -direction
        user_motor_axis_write_dir(i, true);
        count_direction[i] = -1;
      }
      else
      {
        // +direction
        user_motor_axis_write_dir(i, false);
        count_direction[i] = 1;
      }

      if (counter[i] > 0 && i < XYZ_NUM_AXIS)
      {
        if (K600 == ccm_param.t_sys_data_current.model_id)
        {
          user_delay_us(30);
        }
        else if (F400TP == ccm_param.t_sys_data_current.model_id)
        {
          user_delay_us(80);
        }
        else if (Drug == ccm_param.t_sys_data_current.model_id)
        {
          user_delay_us(30);
        }
        else if (ccm_param.t_sys_data_current.model_id == F300TP)
        {
          user_delay_us(8);
        }
        else
        {
          user_delay_us(4);
        }
      }
    }
  }

  static inline void set_dir_check_endstop_xyz(int axis)
  {
    if ((st_current_block->direction_bits & (1 << axis)) != 0)
    {
      // -direction
      bool min_endstop = st_axis_xyz_read_min(axis);

      //      if (ccm_param.t_model.xyz_home_dir[axis] == 1)
      //        min_endstop = false;

      if (min_endstop && save_axis_min_endstop[axis] && (st_current_block->steps_axis[axis] > 0))
      {
        endstops_trigsteps[axis] = count_position[axis];
        step_events_completed = (unsigned long)st_current_block->step_event_count;

        if (Z_AXIS == axis)
        {
          check_endstop_z_hit_min = true;
        }
      }

      save_axis_min_endstop[axis] = min_endstop;
    }
    else
    {
      if (!ccm_param.t_sys.enable_z_max_limit && (axis == Z_AXIS || axis == Z2_AXIS)) return;

      // +direction
      bool max_endstop = user_motor_axis_endstop_read_max(axis);

      if (K5 == ccm_param.t_sys_data_current.model_id && axis == X_AXIS)
      {
        max_endstop = user_motor_axis_endstop_read_min(axis);
      }

      //      if (ccm_param.t_model.xyz_home_dir[axis] == -1)
      //        max_endstop = false;

      if (max_endstop && save_axis_max_endstop[axis] && (st_current_block->steps_axis[axis] > 0))
      {
        endstops_trigsteps[axis] = count_position[axis];
        step_events_completed = st_current_block->step_event_count;

        if (Z_AXIS == axis)
        {
          check_endstop_z_hit_max = true;
        }
      }

      save_axis_max_endstop[axis] = max_endstop;
    }
  }

  static inline void set_dir_check_endstop(void)
  {
    if (check_endstops)
    {
      set_dir_check_endstop_xyz(X_AXIS);
      set_dir_check_endstop_xyz(Y_AXIS);
      set_dir_check_endstop_xyz(Z_AXIS);

      if ((feature_print_control::idex_sys_is_extruder()) ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
          flash_param_t.extruder_type == EXTRUDER_TYPE_LASER) // idex 双头
      {
        set_dir_check_endstop_xyz(X2_AXIS);
        set_dir_check_endstop_xyz(Z2_AXIS);
      }
    }
  }

  //===========================================================================
  //=============================functions public  ============================
  //===========================================================================

  void st_process(void)
  {
    // If there is no current block, attempt to pop one from the buffer
    if (st_current_block == NULL)
    {
      // Anything in the buffer?
      st_current_block = plan_get_current_block();
      block_t *next_block = (block_t *)&ccm_param.block_buffer[(ccm_param.block_buffer_tail + 1) & (BLOCK_BUFFER_SIZE - 1)];

      if (st_current_block == NULL && next_block == NULL)
      {
        user_motor_axis_enable(E_AXIS, false);
        user_motor_axis_enable(B_AXIS, false);
      }

      if (st_current_block != NULL)
      {
        st_current_block->busy = true;
        trapezoid_or_s_curve_generator_reset(st_current_block, step_loops);

        for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
        {
          counter[i] = -(long)(st_current_block->step_event_count >> 1);
        }

        step_events_completed = 0;
      }
      else
      {
        user_motor_timer_set_period(2000 - 1); // 1kHz.
      }
    }
    else
    {
      // Set directions TO DO This should be done once during init of trapezoid. Endstops -> interrupt
      //out_bits = st_current_block->direction_bits;
      // Set the direction bits (X_AXIS=A_AXIS and Y_AXIS=B_AXIS for COREXY)
      set_dirs();
      // Set direction en check limit switches
      set_dir_check_endstop();

      for (int8_t i = 0; i < step_loops; i++)
      {
        // Take multiple steps per interrupt (For high speed moves)
        set_steps();
        step_events_completed += 1;

        if (step_events_completed >= st_current_block->step_event_count)
        {
          break;
        }
      }

#if defined(S_CURVE_ACCELERATION)
      // Initialize the Bézier speed curve
      _calc_bezier_curve_coeffs(st_current_block->initial_rate, st_current_block->cruise_rate, st_current_block->acceleration_time_inverse);
      // We haven't started the 2nd half of the trapezoid
      bezier_2nd_half = false;
#endif
      // Calculare new timer value
      trapezoid_or_s_curve_calculare_timer(st_current_block, step_events_completed, step_loops);

      // If current block is finished, reset pointer
      if (step_events_completed >= st_current_block->step_event_count)
      {
        st_current_block = NULL;
        plan_discard_current_block();
      }
    }
  }

  void st_init(void)
  {
    user_motor_pin_init();
    user_motor_timer_start();
    check_endstops = true; // Start with endstops active. After homing they can be disabled
    // 延时200ms后锁定z电机
    (void)OS_DELAY(200);
    // 先解锁电机
    user_motor_axis_enable(X_AXIS, false);

    if ((feature_print_control::idex_sys_is_extruder()) ||
        flash_param_t.extruder_type == EXTRUDER_TYPE_MIX ||
        flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      user_motor_axis_enable(X2_AXIS, false);
      user_motor_axis_enable(Z2_AXIS, true);
    }

    user_motor_axis_enable(Y_AXIS, false);
    user_motor_axis_enable(E_AXIS, false);
    user_motor_axis_enable(B_AXIS, false);
    // 上电后防止平台往下掉
    user_motor_axis_enable(Z_AXIS, true);
  }

  bool st_check_endstop_z_hit_max(void)
  {
    if (check_endstop_z_hit_max)
    {
      check_endstop_z_hit_max = false;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool st_check_endstop_z_hit_min(void)
  {
    if (check_endstop_z_hit_min)
    {
      check_endstop_z_hit_min = false;
      return true;
    }
    else
    {
      return false;
    }
  }

  void st_enable_endstops(bool check)
  {
    TASK_ENTER_CRITICAL();
    check_endstops = check;
    TASK_EXIT_CRITICAL();
  }

  void st_set_position(const float *_current_position)
  {
    TASK_ENTER_CRITICAL();

    for (int i = 0; i < ccm_param.t_sys.axis_num; i++)
    {
      count_position[i] = _current_position[i];
    }

    TASK_EXIT_CRITICAL();
  }

  void st_set_position_axis(const float value, const int axis)
  {
    TASK_ENTER_CRITICAL();
    count_position[axis] = value;
    TASK_EXIT_CRITICAL();
  }

  float st_get_endstops_len(int axis)
  {
    return (float)endstops_trigsteps[axis] / ccm_param.planner_settings.axis_steps_per_mm[axis];
  }

  float st_get_position_length(int axis)
  {
    return (float)count_position[axis] / ccm_param.planner_settings.axis_steps_per_mm[axis];
  }

  long st_get_position_steps(int axis)
  {
    return count_position[axis];
  }

  void st_synchronize(void)
  {
    while (ccm_param.block_buffer_head != ccm_param.block_buffer_tail)
    {
      (void)OS_DELAY(50);
    }
  }

  bool st_check_queue_is_empty(void)
  {
    return (ccm_param.block_buffer_head == ccm_param.block_buffer_tail);
  }

  void st_wake_up(void)
  {
    user_motor_timer_start();
  }

  void st_quick_stop(void)
  {
    user_motor_timer_stop();

    while (ccm_param.block_buffer_head != ccm_param.block_buffer_tail)
    {
      plan_discard_current_block();
    }

    st_current_block = NULL;
    user_motor_timer_start();
  }

  bool st_get_z_max_endstops_status(void)
  {
    if (!ccm_param.t_sys.enable_z_max_limit)
    {
      return false;
    }
    else
    {
      return user_motor_axis_endstop_read_max(Z_AXIS);
    }
  }

  bool st_axis_xyz_read_min(const int axis)
  {
    if (axis < XYZ_NUM_AXIS)
    {
      if (Z_AXIS == axis)
      {
        if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level && gcode::g29_open_laser_check_flag)
        {
          return user_abl_z_check();
        }
        else
        {
          if (P3_Pro == ccm_param.t_sys_data_current.model_id)
          {
            return user_motor_axis_endstop_read_max(axis);
          }
          else if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id)
          {
            return user_motor_axis_endstop_read_max(axis);
          }
          else if (K600 == ccm_param.t_sys_data_current.model_id)
          {
            return user_motor_axis_endstop_read_min(axis);
          }
          else if (F400TP == ccm_param.t_sys_data_current.model_id)
          {
            return user_motor_axis_endstop_read_min(axis);
          }
          else if (F300TP == ccm_param.t_sys_data_current.model_id)
          {
            return user_motor_axis_endstop_read_min(axis);
          }
          else
          {
            return user_motor_axis_endstop_read_min(axis);
          }
        }
      }
      else
      {
        return user_motor_axis_endstop_read_min(axis);
      }
    }

    return false;
  }

#if defined(S_CURVE_ACCELERATION)
  /**
   *  This uses a quintic (fifth-degree) Bézier polynomial for the velocity curve, giving
   *  a "linear pop" velocity curve; with pop being the sixth derivative of position:
   *  velocity - 1st, acceleration - 2nd, jerk - 3rd, snap - 4th, crackle - 5th, pop - 6th
   *
   *  The Bézier curve takes the form:
   *
   *  V(t) = P_0 * B_0(t) + P_1 * B_1(t) + P_2 * B_2(t) + P_3 * B_3(t) + P_4 * B_4(t) + P_5 * B_5(t)
   *
   *  Where 0 <= t <= 1, and V(t) is the velocity. P_0 through P_5 are the control points, and B_0(t)
   *  through B_5(t) are the Bernstein basis as follows:
   *
   *        B_0(t) =   (1-t)^5        =   -t^5 +  5t^4 - 10t^3 + 10t^2 -  5t   +   1
   *        B_1(t) =  5(1-t)^4 * t    =   5t^5 - 20t^4 + 30t^3 - 20t^2 +  5t
   *        B_2(t) = 10(1-t)^3 * t^2  = -10t^5 + 30t^4 - 30t^3 + 10t^2
   *        B_3(t) = 10(1-t)^2 * t^3  =  10t^5 - 20t^4 + 10t^3
   *        B_4(t) =  5(1-t)   * t^4  =  -5t^5 +  5t^4
   *        B_5(t) =             t^5  =    t^5
   *                                      ^       ^       ^       ^       ^       ^
   *                                      |       |       |       |       |       |
   *                                      A       B       C       D       E       F
   *
   *  Unfortunately, we cannot use forward-differencing to calculate each position through
   *  the curve, as Marlin uses variable timer periods. So, we require a formula of the form:
   *
   *        V_f(t) = A*t^5 + B*t^4 + C*t^3 + D*t^2 + E*t + F
   *
   *  Looking at the above B_0(t) through B_5(t) expanded forms, if we take the coefficients of t^5
   *  through t of the Bézier form of V(t), we can determine that:
   *
   *        A =    -P_0 +  5*P_1 - 10*P_2 + 10*P_3 -  5*P_4 +  P_5
   *        B =   5*P_0 - 20*P_1 + 30*P_2 - 20*P_3 +  5*P_4
   *        C = -10*P_0 + 30*P_1 - 30*P_2 + 10*P_3
   *        D =  10*P_0 - 20*P_1 + 10*P_2
   *        E = - 5*P_0 +  5*P_1
   *        F =     P_0
   *
   *  Now, since we will (currently) *always* want the initial acceleration and jerk values to be 0,
   *  We set P_i = P_0 = P_1 = P_2 (initial velocity), and P_t = P_3 = P_4 = P_5 (target velocity),
   *  which, after simplification, resolves to:
   *
   *        A = - 6*P_i +  6*P_t =  6*(P_t - P_i)
   *        B =  15*P_i - 15*P_t = 15*(P_i - P_t)
   *        C = -10*P_i + 10*P_t = 10*(P_t - P_i)
   *        D = 0
   *        E = 0
   *        F = P_i
   *
   *  As the t is evaluated in non uniform steps here, there is no other way rather than evaluating
   *  the Bézier curve at each point:
   *
   *        V_f(t) = A*t^5 + B*t^4 + C*t^3 + F          [0 <= t <= 1]
   *
   * Floating point arithmetic execution time cost is prohibitive, so we will transform the math to
   * use fixed point values to be able to evaluate it in realtime. Assuming a maximum of 250000 steps
   * per second (driver pulses should at least be 2μS hi/2μS lo), and allocating 2 bits to avoid
   * overflows on the evaluation of the Bézier curve, means we can use
   *
   *   t: unsigned Q0.32 (0 <= t < 1) |range 0 to 0xFFFFFFFF unsigned
   *   A:   signed Q24.7 ,            |range = +/- 250000 * 6 * 128 = +/- 192000000 = 0x0B71B000 | 28 bits + sign
   *   B:   signed Q24.7 ,            |range = +/- 250000 *15 * 128 = +/- 480000000 = 0x1C9C3800 | 29 bits + sign
   *   C:   signed Q24.7 ,            |range = +/- 250000 *10 * 128 = +/- 320000000 = 0x1312D000 | 29 bits + sign
   *   F:   signed Q24.7 ,            |range = +/- 250000     * 128 =      32000000 = 0x01E84800 | 25 bits + sign
   *
   * The trapezoid generator state contains the following information, that we will use to create and evaluate
   * the Bézier curve:
   *
   *  blk->step_event_count [TS] = The total count of steps for this movement. (=distance)
   *  blk->initial_rate     [VI] = The initial steps per second (=velocity)
   *  blk->final_rate       [VF] = The ending steps per second  (=velocity)
   *  and the count of events completed (step_events_completed) [CS] (=distance until now)
   *
   *  Note the abbreviations we use in the following formulae are between []s
   *
   *  For Any 32bit CPU:
   *
   *    At the start of each trapezoid, calculate the coefficients A,B,C,F and Advance [AV], as follows:
   *
   *      A =  6*128*(VF - VI) =  768*(VF - VI)
   *      B = 15*128*(VI - VF) = 1920*(VI - VF)
   *      C = 10*128*(VF - VI) = 1280*(VF - VI)
   *      F =    128*VI        =  128*VI
   *     AV = (1<<32)/TS      ~= 0xFFFFFFFF / TS (To use ARM UDIV, that is 32 bits) (this is computed at the planner, to offload expensive calculations from the ISR)
   *
   *    And for each point, evaluate the curve with the following sequence:
   *
   *      void lsrs(uint32_t& d, uint32_t s, int cnt) {
   *        d = s >> cnt;
   *      }
   *      void lsls(uint32_t& d, uint32_t s, int cnt) {
   *        d = s << cnt;
   *      }
   *      void lsrs(int32_t& d, uint32_t s, int cnt) {
   *        d = uint32_t(s) >> cnt;
   *      }
   *      void lsls(int32_t& d, uint32_t s, int cnt) {
   *        d = uint32_t(s) << cnt;
   *      }
   *      void umull(uint32_t& rlo, uint32_t& rhi, uint32_t op1, uint32_t op2) {
   *        uint64_t res = uint64_t(op1) * op2;
   *        rlo = uint32_t(res & 0xFFFFFFFF);
   *        rhi = uint32_t((res >> 32) & 0xFFFFFFFF);
   *      }
   *      void smlal(int32_t& rlo, int32_t& rhi, int32_t op1, int32_t op2) {
   *        int64_t mul = int64_t(op1) * op2;
   *        int64_t s = int64_t(uint32_t(rlo) | ((uint64_t(uint32_t(rhi)) << 32U)));
   *        mul += s;
   *        rlo = int32_t(mul & 0xFFFFFFFF);
   *        rhi = int32_t((mul >> 32) & 0xFFFFFFFF);
   *      }
   *      int32_t _eval_bezier_curve_arm(uint32_t curr_step) {
   *        uint32_t flo = 0;
   *        uint32_t fhi = bezier_AV * curr_step;
   *        uint32_t t = fhi;
   *        int32_t alo = bezier_F;
   *        int32_t ahi = 0;
   *        int32_t A = bezier_A;
   *        int32_t B = bezier_B;
   *        int32_t C = bezier_C;
   *
   *        lsrs(ahi, alo, 1);          // a  = F << 31
   *        lsls(alo, alo, 31);         //
   *        umull(flo, fhi, fhi, t);    // f *= t
   *        umull(flo, fhi, fhi, t);    // f>>=32; f*=t
   *        lsrs(flo, fhi, 1);          //
   *        smlal(alo, ahi, flo, C);    // a+=(f>>33)*C
   *        umull(flo, fhi, fhi, t);    // f>>=32; f*=t
   *        lsrs(flo, fhi, 1);          //
   *        smlal(alo, ahi, flo, B);    // a+=(f>>33)*B
   *        umull(flo, fhi, fhi, t);    // f>>=32; f*=t
   *        lsrs(flo, fhi, 1);          // f>>=33;
   *        smlal(alo, ahi, flo, A);    // a+=(f>>33)*A;
   *        lsrs(alo, ahi, 6);          // a>>=38
   *
   *        return alo;
   *      }
   *
   *  This is rewritten in ARM assembly for optimal performance (43 cycles to execute).
   *
   *  For AVR, the precision of coefficients is scaled so the Bézier curve can be evaluated in real-time:
   *  Let's reduce precision as much as possible. After some experimentation we found that:
   *
   *    Assume t and AV with 24 bits is enough
   *       A =  6*(VF - VI)
   *       B = 15*(VI - VF)
   *       C = 10*(VF - VI)
   *       F =     VI
   *      AV = (1<<24)/TS   (this is computed at the planner, to offload expensive calculations from the ISR)
   *
   *    Instead of storing sign for each coefficient, we will store its absolute value,
   *    and flag the sign of the A coefficient, so we can save to store the sign bit.
   *    It always holds that sign(A) = - sign(B) = sign(C)
   *
   *     So, the resulting range of the coefficients are:
   *
   *       t: unsigned (0 <= t < 1) |range 0 to 0xFFFFFF unsigned
   *       A:   signed Q24 , range = 250000 * 6 = 1500000 = 0x16E360 | 21 bits
   *       B:   signed Q24 , range = 250000 *15 = 3750000 = 0x393870 | 22 bits
   *       C:   signed Q24 , range = 250000 *10 = 2500000 = 0x1312D0 | 21 bits
   *       F:   signed Q24 , range = 250000     =  250000 = 0x0ED090 | 20 bits
   *
   *    And for each curve, estimate its coefficients with:
   *
   *      void _calc_bezier_curve_coeffs(int32_t v0, int32_t v1, uint32_t av) {
   *       // Calculate the Bézier coefficients
   *       if (v1 < v0) {
   *         A_negative = true;
   *         bezier_A = 6 * (v0 - v1);
   *         bezier_B = 15 * (v0 - v1);
   *         bezier_C = 10 * (v0 - v1);
   *       }
   *       else {
   *         A_negative = false;
   *         bezier_A = 6 * (v1 - v0);
   *         bezier_B = 15 * (v1 - v0);
   *         bezier_C = 10 * (v1 - v0);
   *       }
   *       bezier_F = v0;
   *      }
   *
   *    And for each point, evaluate the curve with the following sequence:
   *
   *      // unsigned multiplication of 24 bits x 24bits, return upper 16 bits
   *      void umul24x24to16hi(uint16_t& r, uint24_t op1, uint24_t op2) {
   *        r = (uint64_t(op1) * op2) >> 8;
   *      }
   *      // unsigned multiplication of 16 bits x 16bits, return upper 16 bits
   *      void umul16x16to16hi(uint16_t& r, uint16_t op1, uint16_t op2) {
   *        r = (uint32_t(op1) * op2) >> 16;
   *      }
   *      // unsigned multiplication of 16 bits x 24bits, return upper 24 bits
   *      void umul16x24to24hi(uint24_t& r, uint16_t op1, uint24_t op2) {
   *        r = uint24_t((uint64_t(op1) * op2) >> 16);
   *      }
   *
   *      int32_t _eval_bezier_curve(uint32_t curr_step) {
   *        // To save computing, the first step is always the initial speed
   *        if (!curr_step)
   *          return bezier_F;
   *
   *        uint16_t t;
   *        umul24x24to16hi(t, bezier_AV, curr_step);   // t: Range 0 - 1^16 = 16 bits
   *        uint16_t f = t;
   *        umul16x16to16hi(f, f, t);                   // Range 16 bits (unsigned)
   *        umul16x16to16hi(f, f, t);                   // Range 16 bits : f = t^3  (unsigned)
   *        uint24_t acc = bezier_F;                    // Range 20 bits (unsigned)
   *        if (A_negative) {
   *          uint24_t v;
   *          umul16x24to24hi(v, f, bezier_C);          // Range 21bits
   *          acc -= v;
   *          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^4  (unsigned)
   *          umul16x24to24hi(v, f, bezier_B);          // Range 22bits
   *          acc += v;
   *          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^5  (unsigned)
   *          umul16x24to24hi(v, f, bezier_A);          // Range 21bits + 15 = 36bits (plus sign)
   *          acc -= v;
   *        }
   *        else {
   *          uint24_t v;
   *          umul16x24to24hi(v, f, bezier_C);          // Range 21bits
   *          acc += v;
   *          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^4  (unsigned)
   *          umul16x24to24hi(v, f, bezier_B);          // Range 22bits
   *          acc -= v;
   *          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^5  (unsigned)
   *          umul16x24to24hi(v, f, bezier_A);          // Range 21bits + 15 = 36bits (plus sign)
   *          acc += v;
   *        }
   *        return acc;
   *      }
   *    These functions are translated to assembler for optimal performance.
   *    Coefficient calculation takes 70 cycles. Bezier point evaluation takes 150 cycles.
   */



  // For all the other 32bit CPUs
  void _calc_bezier_curve_coeffs(const int32_t v0, const int32_t v1, const uint32_t av)
  {
    // Calculate the Bézier coefficients
    bezier_A =  768 * (v1 - v0);
    bezier_B = 1920 * (v0 - v1);
    bezier_C = 1280 * (v1 - v0);
    bezier_F =  128 * v0;
    bezier_AV = av;
  }

  int32_t _eval_bezier_curve(const uint32_t curr_step)
  {
    //      #if defined(__arm__) || defined(__thumb__)
    //        // For ARM Cortex M3/M4 CPUs, we have the optimized assembler version, that takes 43 cycles to execute
    //        uint32_t flo = 0;
    //        uint32_t fhi = bezier_AV * curr_step;
    //        uint32_t t = fhi;
    //        int32_t alo = bezier_F;
    //        int32_t ahi = 0;
    //        int32_t A = bezier_A;
    //        int32_t B = bezier_B;
    //        int32_t C = bezier_C;
    //         __asm__ __volatile__(
    //          ".syntax unified" "\n\t"              // is to prevent CM0,CM1 non-unified syntax
    //          A("lsrs  %[ahi],%[alo],#1")           // a  = F << 31      1 cycles
    //          A("lsls  %[alo],%[alo],#31")          //                   1 cycles
    //          A("umull %[flo],%[fhi],%[fhi],%[t]")  // f *= t            5 cycles [fhi:flo=64bits]
    //          A("umull %[flo],%[fhi],%[fhi],%[t]")  // f>>=32; f*=t      5 cycles [fhi:flo=64bits]
    //          A("lsrs  %[flo],%[fhi],#1")           //                   1 cycles [31bits]
    //          A("smlal %[alo],%[ahi],%[flo],%[C]")  // a+=(f>>33)*C;     5 cycles
    //          A("umull %[flo],%[fhi],%[fhi],%[t]")  // f>>=32; f*=t      5 cycles [fhi:flo=64bits]
    //          A("lsrs  %[flo],%[fhi],#1")           //                   1 cycles [31bits]
    //          A("smlal %[alo],%[ahi],%[flo],%[B]")  // a+=(f>>33)*B;     5 cycles
    //          A("umull %[flo],%[fhi],%[fhi],%[t]")  // f>>=32; f*=t      5 cycles [fhi:flo=64bits]
    //          A("lsrs  %[flo],%[fhi],#1")           // f>>=33;           1 cycles [31bits]
    //          A("smlal %[alo],%[ahi],%[flo],%[A]")  // a+=(f>>33)*A;     5 cycles
    //          A("lsrs  %[alo],%[ahi],#6")           // a>>=38            1 cycles
    //          : [alo]"+r"( alo ) ,
    //            [flo]"+r"( flo ) ,
    //            [fhi]"+r"( fhi ) ,
    //            [ahi]"+r"( ahi ) ,
    //            [A]"+r"( A ) ,  // <== Note: Even if A, B, C, and t registers are INPUT ONLY
    //            [B]"+r"( B ) ,  //  GCC does bad optimizations on the code if we list them as
    //            [C]"+r"( C ) ,  //  such, breaking this function. So, to avoid that problem,
    //            [t]"+r"( t )    //  we list all registers as input-outputs.
    //          :
    //          : "cc"
    //        );
    //        return alo;
    //      #else
    // For non ARM targets, we provide a fallback implementation. Really doubt it
    // will be useful, unless the processor is fast and 32bit
    uint32_t t = bezier_AV * curr_step;               // t: Range 0 - 1^32 = 32 bits
    uint64_t f = t;
    f *= t;                                           // Range 32*2 = 64 bits (unsigned)
    f >>= 32;                                         // Range 32 bits  (unsigned)
    f *= t;                                           // Range 32*2 = 64 bits  (unsigned)
    f >>= 32;                                         // Range 32 bits : f = t^3  (unsigned)
    int64_t acc = (int64_t) bezier_F << 31;           // Range 63 bits (signed)
    acc += ((uint32_t) f >> 1) * (int64_t) bezier_C;  // Range 29bits + 31 = 60bits (plus sign)
    f *= t;                                           // Range 32*2 = 64 bits
    f >>= 32;                                         // Range 32 bits : f = t^3  (unsigned)
    acc += ((uint32_t) f >> 1) * (int64_t) bezier_B;  // Range 29bits + 31 = 60bits (plus sign)
    f *= t;                                           // Range 32*2 = 64 bits
    f >>= 32;                                         // Range 32 bits : f = t^3  (unsigned)
    acc += ((uint32_t) f >> 1) * (int64_t) bezier_A;  // Range 28bits + 31 = 59bits (plus sign)
    acc >>= (31 + 7);                                 // Range 24bits (plus sign)
    return (int32_t) acc;
    //      #endif
  }
#endif // S_CURVE_ACCELERATION


}









