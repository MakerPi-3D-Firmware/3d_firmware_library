#ifndef __USER_LVGL_H_
#define __USER_LVGL_H_

#ifdef ENABLE_GUI_LVGL

#ifdef __cplusplus
extern "C" {
#endif

extern void user_lvgl_init(void);
extern void user_lvgl_task(void);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif

#endif /* __USER_LVGL_H_ */
