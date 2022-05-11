#include "user_common.h"

void user_motor_delay_us(volatile unsigned short time)
{
  volatile unsigned short i = 0;

  while (time--)
  {
    i = 10; //自己定义

    while (i--) __NOP();
  }
}

const static bool endstop_min_inverting[XYZ_NUM_AXIS] = {X_MIN_ENDSTOP_INVERTING, X2_MIN_ENDSTOP_INVERTING,
                                                         Y_MIN_ENDSTOP_INVERTING, Z_MIN_ENDSTOP_INVERTING,
                                                         Z2_MIN_ENDSTOP_INVERTING
                                                        };
const static bool endstop_max_inverting[XYZ_NUM_AXIS] = {X_MAX_ENDSTOP_INVERTING, X2_MAX_ENDSTOP_INVERTING,
                                                         Y_MAX_ENDSTOP_INVERTING, Z_MAX_ENDSTOP_INVERTING,
                                                         Z2_MAX_ENDSTOP_INVERTING
                                                        };

const static bool axis_enable[MAX_NUM_AXIS] = {X_ENABLE_ON, X2_ENABLE_ON, Y_ENABLE_ON, Z_ENABLE_ON,
                                               Z2_ENABLE_ON, E_ENABLE_ON, B_ENABLE_ON
                                              };
const static bool axis_disable[MAX_NUM_AXIS] = {X_ENABLE_OFF, X2_ENABLE_OFF, Y_ENABLE_OFF, Z_ENABLE_OFF,
                                                Z2_ENABLE_OFF, E_ENABLE_OFF, B_ENABLE_OFF
                                               };


const static bool invert_axis_step_pin[MAX_NUM_AXIS] = {INVERT_X_STEP_PIN, INVERT_X2_STEP_PIN,
                                                        INVERT_Y_STEP_PIN, INVERT_Z_STEP_PIN,
                                                        INVERT_Z2_STEP_PIN, INVERT_E_STEP_PIN,
                                                        INVERT_B_STEP_PIN
                                                       };

bool user_motor_axis_endstop_read_min(int axis)
{
  if (axis < XYZ_NUM_AXIS)
  {
    USER_ENDSTOP_READ(MOTOR_LIMIT_MIN_PIN, axis, endstop_min_inverting[axis]);
  }

  return false;
}

bool user_motor_axis_endstop_read_max(int axis)
{
  if (axis < XYZ_NUM_AXIS)
  {
    USER_ENDSTOP_READ(MOTOR_LIMIT_MAX_PIN, axis, endstop_max_inverting[axis]);
  }

  return false;
}

void user_motor_axis_enable(int axis, bool isEnable)
{
  bool value = (isEnable ? axis_enable[axis] : axis_disable[axis]);

  if (!ccm_param.t_sys.is_dual_z_axis && Z2_AXIS == axis) return;

  USER_MOTOR_WRITE(MOTOR_EN_PIN, axis, value);
}

void user_motor_axis_write_step(int axis, bool value)
{
  USER_MOTOR_WRITE(MOTOR_STEP_PIN, axis, value);
}

void user_motor_axis_write_dir(int axis, bool isInvert)
{
  GPIO_PinState value = (GPIO_PinState)(isInvert ? ccm_param.t_model.enable_invert_dir[axis] :
                                        !ccm_param.t_model.enable_invert_dir[axis]);
  USER_MOTOR_WRITE(MOTOR_DIR_PIN, axis, value);
}

void user_motor_pin_init(void)
{
  //endstops and pullups
  for (int i = 0; i < XYZ_NUM_AXIS; i++)
  {
    USER_MOTOR_WRITE(MOTOR_LIMIT_MIN_PIN, i, GPIO_PIN_SET);
  }

  //Initialize Step Pins
  for (int i = 0; i < MAX_NUM_AXIS; i++)
  {
    user_motor_axis_enable(i, true);
    user_motor_axis_write_step(i, invert_axis_step_pin[i]);
    user_motor_axis_enable(i, false);
  }
}

void user_motor_timer_start(void)
{
  extern TIM_HandleTypeDef htim4;
  HAL_TIM_Base_Start_IT(&htim4);
}

void user_motor_timer_stop(void)
{
  extern TIM_HandleTypeDef htim4;
  HAL_TIM_Base_Stop_IT(&htim4);
}

void user_motor_timer_set_period(uint32_t period_value)
{
  extern TIM_HandleTypeDef htim4;
  htim4.Instance->ARR = period_value;
  htim4.Init.Period = period_value;
}



