#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gui
{
  extern volatile uint8_t button_key_value;
  extern void button_scan_key_value(uint8_t no_key_value, uint8_t key_value);
  extern bool button_is_update_data(void);
  extern float button_get_update_data(uint8_t no_key_value, float fast_value, float slow_value);
  extern void button_reset_update_data(void);
}

#endif // GUI_BUTTON_H

