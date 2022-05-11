#ifndef FILAMENT_CONTROL_H
#define FILAMENT_CONTROL_H

#ifdef HAS_FILAMENT

namespace feature_filament
{

  extern void control_init(void);
  extern void control_cancel_process(void);            ///< 取消进退丝
  extern void control_process(void);                   ///< 执行进退丝入口
  extern void control_start_load(void);                ///< 开始进丝
  extern void control_start_unload(void);              ///< 开始退丝
  extern void control_reset_status(void);

}
#endif
#endif // FILAMENT_CONTROL_H












