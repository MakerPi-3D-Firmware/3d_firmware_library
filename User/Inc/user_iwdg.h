#ifndef USER_IWDG_H
#define USER_IWDG_H

#ifdef ENABLE_IWDG

#ifdef __cplusplus
extern "C" {
#endif

void user_iwdg_show_err_info(void);
void user_iwdg_init(void);         ///< 看门狗初始化
void user_iwdg_refresh(void);      ///< 3s喂一次狗，5s没有喂狗会复位重启

#ifdef __cplusplus
} // extern "C" {
#endif

#endif

#endif //USER_IWDG_H

