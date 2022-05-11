#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif
extern sg_grbl::vector_3 level_points[LEVEL_POS_COUNT];
sg_grbl::matrix_3x3 plan_bed_level_matrix;   ///< 热床偏移矩阵
#ifdef __cplusplus
} //extern "C" {
#endif

namespace sg_grbl
{
  volatile bool is_process_auto_bed_leveling = false;             ///< 执行平台校准开关

  static matrix_3x3 matrix;           ///< 当前矩阵
  static int matrix_index;            ///< 当前矩阵id
  static int pre_matrix_index = 0;    ///< 前矩阵id
  static int zero_matrix_index = 0;   ///< 零点矩阵id
  static vector_3 offset;
  static vector_3 offset1;

  static void _get_matrix(vector_3 vector)
  {
    // 获取矩阵
    matrix.set_to_identity();
    matrix_index = gcode::g29_abl_get_matrix_index(vector);

    if (matrix_index == 0)
    {
      matrix = flash_param_t.matrix_front_left;
    }
    else if (matrix_index == 1)
    {
      matrix = flash_param_t.matrix_back_right;
    }
  }

  void plan_set_process_auto_bed_level_status(bool status)
  {
    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      is_process_auto_bed_leveling = status;

      if (!is_process_auto_bed_leveling)
      {
        _get_matrix(vector_3(0.0f, 0.0f, 0.0f));
        zero_matrix_index = matrix_index;
        pre_matrix_index = matrix_index;
        offset = vector_3(0.0f, 0.0f, 0.0f);

        if (K5 == ccm_param.t_sys_data_current.model_id && 1 == ccm_param.t_sys_data_current.IsMechanismLevel)
        {
        }
        else
        {
          ccm_param.t_sys.is_gcode_g28_done = false;
        }
      }
    }
  }

  void plan_init_offset1(void)
  {
    vector_3 vector0 = level_points[LEVEL_POS_INDEX_RIGHT_FRONT];
    vector_3 vector1 = level_points[LEVEL_POS_INDEX_RIGHT_FRONT];
    vector0.apply_rotation(matrix_3x3::create_inverse(flash_param_t.matrix_front_left));
    vector1.apply_rotation(matrix_3x3::create_inverse(flash_param_t.matrix_back_right));
    vector_3 vector2 = level_points[LEVEL_POS_INDEX_LEFT_BACK];
    vector_3 vector3 = level_points[LEVEL_POS_INDEX_LEFT_BACK];
    vector2.apply_rotation(matrix_3x3::create_inverse(flash_param_t.matrix_front_left));
    vector3.apply_rotation(matrix_3x3::create_inverse(flash_param_t.matrix_back_right));
    offset1 = (vector1 - vector0) + (vector3 - vector2);
    offset1.x = offset1.x / 2.0f;
    offset1.y = offset1.y / 2.0f;
    offset1.z = offset1.z / 2.0f;
    offset1.x = ((float)((int)((offset1.x + 0.0005f) * 1000))) / 1000.0f;
    offset1.y = ((float)((int)((offset1.y + 0.0005f) * 1000))) / 1000.0f;
    offset1.z = ((float)((int)((offset1.z + 0.0005f) * 1000))) / 1000.0f;
  }

  static void plan_apply_rotation_xyz(float &x, float &y, float &z)
  {
    if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    {
      vector_3 vector = vector_3(x, y, z);
      _get_matrix(vector);

      if (matrix_index != pre_matrix_index)
      {
        if (zero_matrix_index == matrix_index)
        {
          offset = vector_3(0.0f, 0.0f, 0.0f);
        }
        else
        {
          if (matrix_index == 1)
          {
            offset = offset1;
          }
          else
          {
            offset = vector_3(0.0f, 0.0f, 0.0f);
          }
        }
      }

      vector = vector + offset;
      vector.apply_rotation(matrix);
      x = vector.x;
      y = vector.y;
      z = vector.z;
      pre_matrix_index = matrix_index;
    }
  }

  // 保存当前位置
  static void _get_xyz_save_current(float (&plan_buffer_position)[MAX_NUM_AXIS])
  {
    for (int i = 0; i < XYZ_NUM_AXIS; i++)
    {
      ccm_param.t_planner_level.current_save_xyz[i] = plan_buffer_position[i];
    }
  }

  // 保存真实移动坐标
  static void _get_xyz_save_real(float (&plan_buffer_position)[MAX_NUM_AXIS])
  {
    for (int i = 0; i < XYZ_NUM_AXIS; i++)
    {
      ccm_param.t_planner_level.current_xyz_real[i] = plan_buffer_position[i];
    }
  }

  static void _get_xy_compensate_max(float (&plan_buffer_position)[MAX_NUM_AXIS], int axis)
  {
    // 转换坐标如果大于最大值，不移动，保存当前该值，以便下一次补偿
    float max_value = ccm_param.t_model.xyz_max_pos[axis];

    if (axis == X2_AXIS)
    {
      max_value = ccm_param.t_model.xyz_home_pos[axis];
    }

    if (plan_buffer_position[axis] > max_value)
    {
      ccm_param.t_planner_level.current_xyz_compensate[axis] = plan_buffer_position[axis] - max_value;
      plan_buffer_position[axis] = max_value;
    }
    else // 确定是否需要补偿，清除补偿值
    {
      plan_buffer_position[axis] += ccm_param.t_planner_level.current_xyz_compensate[axis];
      ccm_param.t_planner_level.current_xyz_compensate[axis] = 0.0f;
    }
  }

  static void _get_xy_compensate_min(float (&plan_buffer_position)[MAX_NUM_AXIS], int axis)
  {
    // 转换坐标如果小于最小值，不移动，保存当前该值，以便下一次补偿
    if (plan_buffer_position[axis] < ccm_param.t_model.xyz_min_pos[axis]) // 转换坐标X如果为负数，不移动，保存当前该值，以便下一次补偿
    {
      ccm_param.t_planner_level.current_xyz_compensate[axis] = ccm_param.t_model.xyz_min_pos[axis] - plan_buffer_position[axis];
      plan_buffer_position[axis] = ccm_param.t_model.xyz_min_pos[axis];
    }
    else // 确定是否需要补偿，清除补偿值
    {
      if (ccm_param.t_planner_level.current_xyz_compensate[axis] > 0.001f)
      {
        plan_buffer_position[axis] += ccm_param.t_planner_level.current_xyz_compensate[axis];
        ccm_param.t_planner_level.current_xyz_compensate[axis] = 0.0f;
      }
    }
  }

  // 获取xy补偿值
  static void _get_xy_compensate(float (&plan_buffer_position)[MAX_NUM_AXIS])
  {
    if (feature_print_control::idex_sys_is_extruder() && 1 == gcode::active_extruder)
    {
      plan_apply_rotation_xyz(plan_buffer_position[X2_AXIS], plan_buffer_position[Y_AXIS], plan_buffer_position[Z_AXIS]);
      _get_xy_compensate_max(plan_buffer_position, X2_AXIS);
    }
    else
    {
      plan_apply_rotation_xyz(plan_buffer_position[X_AXIS], plan_buffer_position[Y_AXIS], plan_buffer_position[Z_AXIS]);
      _get_xy_compensate_min(plan_buffer_position, X_AXIS);
    }

    _get_xy_compensate_min(plan_buffer_position, Y_AXIS);
  }

  void plan_compensation_destination_idex_basic(const int plus_or_minus, const int axis, const float x_offset)
  {
    if (is_process_auto_bed_leveling)
    {
      if (X_AXIS == axis)
      {
        ccm_param.grbl_destination[axis] = ccm_param.grbl_destination[axis];// + plus_or_minus * (flash_param_t.idex_ext1_ext0_offset[0] + x_offset);
      }
      else if (X2_AXIS == axis)
      {
        ccm_param.grbl_destination[axis] = ccm_param.grbl_destination[axis] + plus_or_minus * (flash_param_t.idex_ext1_ext0_offset[0] + x_offset);
      }
      else if (Y_AXIS == axis)
      {
        ccm_param.grbl_destination[axis] = ccm_param.grbl_destination[axis] + plus_or_minus * flash_param_t.idex_ext1_ext0_offset[1];
      }
      else if (Z_AXIS == axis)
      {
        ccm_param.grbl_destination[axis] = ccm_param.grbl_destination[axis] + plus_or_minus * (flash_param_t.idex_ext1_ext0_offset[2] - flash_param_t.idex_ext0_home_pos_adding[2]);
      }
    }
  }

  void plan_buffer_line_get_xyz(float (&plan_buffer_position)[MAX_NUM_AXIS])
  {
    // 保存当前位置
    _get_xyz_save_current(plan_buffer_position);

    if (is_process_auto_bed_leveling)
    {
      if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
      {
        if (is_process_auto_bed_leveling)
        {
          _get_xy_compensate(plan_buffer_position);
        }
        else
        {
          // 调平模式下，不转换坐标，重置xyz补偿值
          for (int i = 0; i < XYZ_NUM_AXIS; i++)
          {
            ccm_param.t_planner_level.current_xyz_compensate[i] = 0.0f;
          }
        }
      }
    }

    // 保存真实移动坐标
    _get_xyz_save_real(plan_buffer_position);
  }

  void plan_set_position_get_xyz(const volatile float *_current_position, float (&position_xyz)[MAX_NUM_AXIS], bool is_sync_z)
  {
    float auto_bed_level_position[MAX_NUM_AXIS] = {0.0f};

    for (int i = 0; i < XYZ_NUM_AXIS; i++)
    {
      auto_bed_level_position[i] = _current_position[i];
    }

    plan_buffer_line_get_xyz(auto_bed_level_position);

    for (int i = 0; i < XYZ_NUM_AXIS; i++)
    {
      if (X2_AXIS == i)
      {
        if (feature_print_control::idex_sys_is_extruder() && 1 == gcode::active_extruder)
        {
          position_xyz[i] = (auto_bed_level_position[i] * ccm_param.planner_settings.axis_steps_per_mm[i]);
          position_xyz[i] = lroundf(position_xyz[i]);
        }
      }
      else
      {
        position_xyz[i] = (auto_bed_level_position[i] * ccm_param.planner_settings.axis_steps_per_mm[i]);
        position_xyz[i] = lround(position_xyz[i]);
      }
    }

    if (is_sync_z)
    {
      position_xyz[Z2_AXIS] = position_xyz[Z_AXIS];
    }
  }

  float plan_get_current_save_xyz(int axis)
  {
    if (is_process_auto_bed_leveling)
    {
      return ccm_param.t_planner_level.current_save_xyz[axis];
    }
    else
    {
      return ccm_param.t_planner_level.current_xyz_real[axis];
    }
  }

  float plan_get_xyz_real(int axis)
  {
    return ccm_param.t_planner_level.current_xyz_real[axis];
  }
}


