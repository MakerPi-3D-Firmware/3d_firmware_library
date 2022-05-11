#ifndef USER_LED_H
#define USER_LED_H

#ifdef HAS_LED_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

extern void user_led_control_init(void);
extern void user_led_board_toggle(void);
extern void user_led_caution_toggle(void);
extern void user_led_control_lighting_status(bool isOn);
extern void user_led_control_caution_status(bool isOn);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // USER_LED_H

