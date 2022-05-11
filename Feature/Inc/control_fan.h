#ifndef CONTROL_FAN_H
#define CONTROL_FAN_H

#ifdef HAS_FAN_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

extern int feature_control_get_fan_speed(void);
extern void feature_control_set_fan_speed(int value);
extern void feature_control_fan(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // CONTROL_FAN_H

