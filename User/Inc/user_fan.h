#ifndef USER_FAN_H
#define USER_FAN_H

#ifdef HAS_FAN_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

extern void user_fan_control_init(void);
extern void user_fan_control_e_pwm(int pwm_value);
extern void user_fan_control_b_pwm(int pwm_value);
extern void user_fan_control_eb_motor(bool isOn);
extern void user_fan_control_nozzle_heat_block(bool isOn);
extern void user_fan_control_board_cool(bool isOn);
extern void user_fan_disable_all(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // USER_FAN_H

