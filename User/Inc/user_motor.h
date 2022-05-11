#ifndef USER_MOTOR_H
#define USER_MOTOR_H

// coarse Endstop Settings
#define ENDSTOPPULLUPS // Comment this out (using // at the start of the line) to disable the endstop pullup resistors
#ifdef ENDSTOPPULLUPS
  #define ENDSTOPPULLUP_XMAX
  #define ENDSTOPPULLUP_X2MAX
  #define ENDSTOPPULLUP_YMAX
  #define ENDSTOPPULLUP_ZMAX
  #define ENDSTOPPULLUP_XMIN
  #define ENDSTOPPULLUP_X2MIN
  #define ENDSTOPPULLUP_YMIN
  #define ENDSTOPPULLUP_ZMIN
#endif

// The pullups are needed if you directly connect a mechanical endswitch between the signal and ground pins.
#define X_MIN_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define X2_MIN_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define Y_MIN_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define Z_MIN_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define Z2_MIN_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define X_MAX_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define X2_MAX_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define Y_MAX_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define Z_MAX_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.
#define Z2_MAX_ENDSTOP_INVERTING 1 // set to true to invert the logic of the endstop.

// For Inverting Stepper Enable Pins (Active Low) use 0, Non Inverting (Active High) use 1
#define X_ENABLE_ON 0
#define X2_ENABLE_ON 0
#define Y_ENABLE_ON 0
#define Z_ENABLE_ON 0
#define Z2_ENABLE_ON 0
#define E_ENABLE_ON 0
#define B_ENABLE_ON 0 // For all extruders

#define X_ENABLE_OFF 1
#define X2_ENABLE_OFF 1
#define Y_ENABLE_OFF 1
#define Z_ENABLE_OFF 1
#define Z2_ENABLE_OFF 1
#define E_ENABLE_OFF 1
#define B_ENABLE_OFF 1 // For all extruders

// Disables axis when it's not being used.
#define DISABLE_X false
#define DISABLE_X2 false
#define DISABLE_Y false
#define DISABLE_Z false
#define DISABLE_Z2 false
#define DISABLE_E false
#define DISABLE_B false// For all extruders
#define DISABLE_INACTIVE_EXTRUDER true //disable only inactive extruders and keep active extruder enabled

//By default pololu step drivers require an active high signal. However, some high power drivers require an active low signal as step.
#define INVERT_X_STEP_PIN false
#define INVERT_X2_STEP_PIN false
#define INVERT_Y_STEP_PIN false
#define INVERT_Z_STEP_PIN false
#define INVERT_Z2_STEP_PIN false
#define INVERT_E_STEP_PIN false
#define INVERT_B_STEP_PIN false//COLOR_MIXING

bool user_motor_axis_endstop_read_min(int axis);
bool user_motor_axis_endstop_read_max(int axis);
void user_motor_axis_enable(int axis, bool isEnable);
void user_motor_axis_write_step(int axis, bool value);
void user_motor_axis_write_dir(int axis, bool isInvert);
void user_motor_pin_init(void);
void user_motor_timer_start(void);
void user_motor_timer_stop(void);
void user_motor_timer_set_period(uint32_t period_value);
void user_motor_delay_us(volatile unsigned short time);

#endif //USER_MOTOR_H

