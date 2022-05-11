#include "user_common_cpp.h"
#ifndef ENABLE_GUI_LVGL

namespace gui
{

#define GUI_BUTTON_TIMEOUT 600
#define GUI_BUTTON_FIRST_TIMEOUT 600

  volatile uint8_t button_key_value = 0;
  static uint8_t button_slow_update_data = 0;
  static uint8_t button_fast_update_data = 0;
  static unsigned long button_timeout = 0;
  static unsigned long button_first_timeout = 0;

  static void button_long_press(uint8_t no_key_value)
  {
    if (gui_touch_xy_is_up())
    {
      button_key_value = no_key_value;
      (void)osDelay(200);
    }
    else if (button_timeout < xTaskGetTickCount())
    {
      button_timeout = GUI_BUTTON_TIMEOUT + xTaskGetTickCount();
      button_slow_update_data = 0;
      button_fast_update_data = 1;
    }
  }

  static void button_short_press(void)
  {
    button_slow_update_data = 1;
    button_fast_update_data = 0;
  }

  void button_scan_key_value(uint8_t no_key_value, uint8_t key_value)
  {
    if (no_key_value == button_key_value)
    {
      button_timeout = GUI_BUTTON_TIMEOUT + xTaskGetTickCount();
      button_first_timeout = GUI_BUTTON_FIRST_TIMEOUT + xTaskGetTickCount();
      button_key_value = key_value;
    }
    else
    {
      if (button_first_timeout < xTaskGetTickCount())
      {
        button_long_press(no_key_value);
      }
      else if (gui_touch_xy_is_up())
      {
        button_short_press();
      }
    }
  }

  bool button_is_update_data(void)
  {
    bool result = ((1 == button_fast_update_data) || (1 == button_slow_update_data));
    return result;
  }

  void button_reset_update_data(void)
  {
    button_fast_update_data = 0;
    button_slow_update_data = 0;
  }

  float button_get_update_data(uint8_t no_key_value, float fast_value, float slow_value)
  {
    float result = 0.0f;

    if (1 == button_fast_update_data)
    {
      result = fast_value;
    }
    else
    {
      result = slow_value;
      button_key_value = no_key_value;
    }

    return result;
  }
}

#endif









