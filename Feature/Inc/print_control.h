#ifndef PRINT_CONTROL_H
#define PRINT_CONTROL_H

#ifdef HAS_PRINT_CONTROL

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  volatile bool is_printing;                 /*!< 是否正在打印 */
  volatile bool is_pause_printing;           /*!< 是否正在暂停打印 */
  volatile bool is_stop_printing;            /*!< 是否正在停止打印 */
  volatile bool is_resume_printing;          /*!< 是否正在恢复止打印 */
  volatile bool is_finish_print;             /*!< 是否完成了打印 */
  volatile bool is_poweroff_recovery_print;  /*!< 是否为断电恢复操作 */
  volatile bool is_print_medium_user_file;            /*!< 是否打印sd卡文件 */
  volatile bool is_pause_to_cool_down;       /*!< 是否暂停温度降低 */
  volatile bool is_mid_chg_filament;         /*!< 是否中途换料 */
} t_print_status;

extern t_print_status print_status;

namespace feature_print_control
{
  extern int print_is_heating(void);
  extern bool print_not_from_medium_user(void);

  extern unsigned int print_time(void);
  extern void pause_start(void);
  extern void pause_process(void);
  extern void resume_start(void);
  extern void resume_process(void);
  extern void stop_start(void);
  extern void stop_process(void);

  extern bool file_open(void);
  extern void file_close(void);
  extern void file_get_gcode_buf(void);
  extern int file_get_percent(void);
  extern void file_form_medium_user(void);
  extern void file_reset_medium(void);

  extern void power_rec_print_start(void);
  extern void file_print_start(void);
  extern void set_file_print_finish(void);
  extern void set_file_print_start(void);
  extern void check_idex_normal_mode(void);
  extern int get_pause_extruder_target_temp(void);

  extern bool idex_sys_is_copy_or_mirror(void);
  extern bool idex_sys_is_normal(void);
  extern bool idex_sys_is_extruder(void);
  extern bool idex_sys_is_normal(void);
  extern bool idex_sys_is_copy(void);
  extern bool idex_sys_is_mirror(void);
  extern void idex_sys_set_extruder(bool is_idex);
  extern void idex_sys_set_normal(void);
  extern void idex_sys_set_copy(void);
  extern void idex_sys_set_mirror(void);
  extern void idex_sys_copy_flash_print_type(void);
  // Use for poweroff recovery
  extern bool idex_flash_is_normal(void);
  extern bool idex_flash_is_copy(void);
  extern bool idex_flash_is_mirror(void);
  extern bool idex_flash_is_copy_or_mirror(void);
  extern void idex_flash_copy_sys_print_type(void);

  extern void pc_process(void);
  extern void pc_init(void);
}

#ifdef __cplusplus
} //extern "C"
#endif

#endif // HAS_PRINT_CONTROL

#endif // PRINT_CONTROL_H
















