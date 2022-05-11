#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

extern osSemaphoreId ReadUdiskSemHandle;

void read_udisk_task_loop(void)
{
  (void)osSemaphoreWait(ReadUdiskSemHandle, osWaitForever); //信号量等待

  if (print_status.is_printing && !print_status.is_pause_printing &&
      !print_status.is_mid_chg_filament) //若打印才去SD卡或U盘中读取数据
  {
    if (ccm_param.t_gui_semp.index_v != StopPrintValue) //防止点击停止打印后，还去读取数据的情况，此种情况是由于RefDataTask_OS高优先级任务中的osDelay(10);引起的，此时是否打印标志位还没更新
    {
      feature_print_control::file_get_gcode_buf(); //从SD卡或U盘中读取数据，当上传文件打印时是从SD卡读取，当触摸显示屏选择文件打印时是从U盘读取
      (void)osDelay(10); //延时以让低优先级的任务执行
    }
  }
}



#ifdef __cplusplus
} // extern "C" {
#endif












