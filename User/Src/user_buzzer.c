#include "user_common.h"

#ifdef HAS_BUZZER_CONTROL

extern TIM_HandleTypeDef htim11;

static volatile bool is_alarm_on = false;
static volatile bool buzzer_control_status = false;

void user_buzzer_init(void)
{
  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
}

void user_buzzer_control(const bool is_on)
{
  if (buzzer_control_status == is_on) return;

  buzzer_control_status = is_on;
  taskENTER_CRITICAL();
  unsigned int compare = is_on ? 1000 : 0;
  HAL_TIM_PWM_Stop(&htim11, TIM_CHANNEL_1);
  htim11.Instance->ARR  = compare;
  htim11.Instance->CCR1 = compare * 2 / 5;
  HAL_TIM_GenerateEvent(&htim11, TIM_EventSource_Update);
  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
  taskEXIT_CRITICAL();
}

void user_buzzer_beep(unsigned short time)
{
  static unsigned long timeout_beeper = 0;
  static bool b = false;

  if (0 == time)
  {
    user_buzzer_control(false);
    return;
  }

  if (timeout_beeper < xTaskGetTickCount())
  {
    timeout_beeper = xTaskGetTickCount() + time;
    b = !b;

    if (b) user_buzzer_control(true);
    else user_buzzer_control(false);
  }
}

void user_buzzer_buzz(unsigned short msticks)
{
  user_buzzer_control(true);
  (void)osDelay(msticks);
  user_buzzer_control(false);
  (void)osDelay(msticks);
}

void user_buzzer_set_alarm_status(const bool is_on)
{
  is_alarm_on = is_on;
}

void user_buzzer_beep_alarm(void)
{
  static bool IsCloseBeep = true;

  if (is_alarm_on)
  {
    user_buzzer_beep(500);
    IsCloseBeep = false;
  }
  else
  {
    if (!IsCloseBeep)
    {
      user_buzzer_beep(0);
      IsCloseBeep = true;
    }
  }
}

#endif



