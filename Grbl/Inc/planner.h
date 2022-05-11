
#ifndef planner_h
#define planner_h

#include "user_common_cpp.h"

namespace sg_grbl
{

#ifdef __cplusplus
  extern "C" {
#endif

  extern volatile bool is_process_auto_bed_leveling;

  void planner_init(void);                                                ///< 初始化电机规划系统
  void planner_set_position(const volatile float *_current_position);              ///< G92指令，设置xyzeb坐标
  void planner_set_axis_position(const volatile float &value, const int axis);
  void planner_set_position_basic(const volatile float *_current_position, bool is_sync_z);
  void planner_set_axis_position_basic(const volatile float &value, const int axis, bool is_sync_z);
  void planner_buffer_line(planner_running_status_t *running_status, bool is_sync_z);     ///< 移动指令坐标数据
  bool planner_blocks_queued(void);                                       ///< 返回队列是否有数据
  int planner_moves_planned(void);                                        ///< 返回当前运动队列有多少条数据
  bool is_planner_moves_planned_full(void);

  // 自动调平接口
  void plan_buffer_line_get_xyz(float (&plan_buffer_position)[MAX_NUM_AXIS]);   ///< 获取自动调平变换坐标
  void plan_set_position_get_xyz(const volatile float *_current_position, float (&position_xyz)[MAX_NUM_AXIS], bool is_sync_z);
  void plan_set_process_auto_bed_level_status(bool status);
  float plan_get_current_save_xyz(int axis);
  float plan_get_xyz_real(int axis);
  void plan_init_offset1(void);
  void plan_compensation_destination_idex_basic(const int plus_or_minus, const int axis, const float x_offset);
#ifdef __cplusplus
} //extern "C" {
#endif

}

#endif





