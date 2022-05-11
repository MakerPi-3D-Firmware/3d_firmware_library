#include "user_common_cpp.h"
#include "UdiskControl.h"
#include "USBFileTransfer.h"
#include "../../Grbl/Inc/temperature_pid_temp.h"
#ifdef __cplusplus
extern "C" {
#endif
extern osSemaphoreId ReceiveUartCmdHandle;
extern float add_homeing[XYZ_NUM_AXIS];

void start_default_task_init(void)
{
  // 机器型号初始化
  machine_model_init();
  gcode::init();

  if (flash_param_t.settings.settings_version == FLASH_SETTINGS_VERSION)
  {
    memmove((planner_settings_t *)&ccm_param.planner_settings, (planner_settings_t *)&flash_param_t.settings.planner_settings, sizeof(planner_settings_t));
    memmove((float *)&add_homeing, (float *)&flash_param_t.settings.home_offset, sizeof(float)*XYZ_NUM_AXIS);
    sg_grbl::temp_pid_extruder_set_kp(flash_param_t.settings.temp_p);
    sg_grbl::temp_pid_extruder_set_ki(flash_param_t.settings.temp_i);
    sg_grbl::temp_pid_extruder_set_kd(flash_param_t.settings.temp_d);
  }

  //删除SD卡里的gcode文件
  DelectSDFiles();
  feature_filament::f_init();
  feature_print_control::pc_init();

  for (int i = 0; i < XYZ_NUM_AXIS; i ++)
  {
    ccm_param.t_gui.move_xyz_pos[i] = ccm_param.t_model.xyz_home_pos[i];
  }
}



void start_default_task_loop(void)
{
  (void)osSemaphoreWait(ReceiveUartCmdHandle, osWaitForever); // 等待信号量
  (void)OS_DELAY(50); // 延时以让低优先级的任务执行
  SaveUSBFile(); // 存储从电脑端发过来的文件
}


#ifdef __cplusplus
} // extern "C" {
#endif

