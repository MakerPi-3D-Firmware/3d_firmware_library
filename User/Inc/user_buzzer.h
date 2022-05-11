#ifndef USER_BUZZER_H
#define USER_BUZZER_H

#ifdef HAS_BUZZER_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

extern void user_buzzer_init(void);
extern void user_buzzer_control(const bool is_on);
extern void user_buzzer_beep(unsigned short time);
extern void user_buzzer_buzz(unsigned short msticks);
extern void user_buzzer_set_alarm_status(const bool is_on);
extern void user_buzzer_beep_alarm(void);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif

#endif //USER_BUZZER_H

