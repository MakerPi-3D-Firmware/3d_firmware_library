#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

void model_p3_pro_flash_config_init(void);
void model_k600_flash_config_init(void);
void model_f400tp_flash_config_init(void);
void model_f300tp_flash_config_init(void);
void model_p3_pro_mini_flash_config_init(void);
void model_m30_flash_config_init(void);
void model_drug_flash_config_init(void);
void model_k5_flash_config_init(void);

void model_flash_config_init(void)
{
  if (P3_Pro == ccm_param.t_sys_data_current.model_id)
  {
    model_p3_pro_flash_config_init();
  }
  else if (K600 == ccm_param.t_sys_data_current.model_id)
  {
    model_k600_flash_config_init();
  }
  else if (F400TP == ccm_param.t_sys_data_current.model_id)
  {
    model_f400tp_flash_config_init();
  }
  else if (F300TP == ccm_param.t_sys_data_current.model_id)
  {
    model_f300tp_flash_config_init();
  }
  else if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id)
  {
    model_p3_pro_mini_flash_config_init();
  }
  else if (M30 == ccm_param.t_sys_data_current.model_id)
  {
    model_m30_flash_config_init();
  }
  else if (Drug == ccm_param.t_sys_data_current.model_id)
  {
    model_drug_flash_config_init();
  }
  else if (K5 == ccm_param.t_sys_data_current.model_id)
  {
    model_k5_flash_config_init();
  }
}

void model_p3_pro_grbl_config_init(void);
void model_p3_pro_mini_grbl_config_init(void);
void model_k600_grbl_config_init(void);
void model_f400tp_grbl_config_init(void);
void model_f300tp_grbl_config_init(void);
void model_m30_grbl_config_init(void);
void model_drug_grbl_config_init(void);
void model_k5_grbl_config_init(void);

void model_grbl_config_init(void)
{
  if (P3_Pro == ccm_param.t_sys_data_current.model_id)
  {
    model_p3_pro_grbl_config_init();
  }
  else if (K600 == ccm_param.t_sys_data_current.model_id)
  {
    model_k600_grbl_config_init();
  }
  else if (F400TP == ccm_param.t_sys_data_current.model_id)
  {
    model_f400tp_grbl_config_init();
  }
  else if (F300TP == ccm_param.t_sys_data_current.model_id)
  {
    model_f300tp_grbl_config_init();
  }
  else if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id)
  {
    model_p3_pro_mini_grbl_config_init();
  }
  else if (M30 == ccm_param.t_sys_data_current.model_id)
  {
    model_m30_grbl_config_init();
  }
  else if (Drug == ccm_param.t_sys_data_current.model_id)
  {
    model_drug_grbl_config_init();
  }
  else if (K5 == ccm_param.t_sys_data_current.model_id)
  {
    model_k5_grbl_config_init();
  }
}

void model_p3_pro_init(void);
void model_p3_pro_mini_init(void);
void model_f300tp_init(void);
void model_k600_init(void);
void model_f400tp_init(void);
void model_m30_init(void);
void model_drug_init(void);
void model_k5_init(void);

void model_init(void)
{
  if (P3_Pro == ccm_param.t_sys_data_current.model_id)
  {
    model_p3_pro_init();
  }
  else if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id)
  {
    model_p3_pro_mini_init();
  }
  else if (F300TP == ccm_param.t_sys_data_current.model_id)
  {
    model_f300tp_init();
  }
  else if (K600 == ccm_param.t_sys_data_current.model_id)
  {
    model_k600_init();
  }
  else if (F400TP == ccm_param.t_sys_data_current.model_id)
  {
    model_f400tp_init();
  }
  else if (M30 == ccm_param.t_sys_data_current.model_id)
  {
    model_m30_init();
  }
  else if (Drug == ccm_param.t_sys_data_current.model_id)
  {
    model_drug_init();
  }
  else if (K5 == ccm_param.t_sys_data_current.model_id)
  {
    model_k5_init();
  }
}

static void machine_model_param_init(void);         ///< 机器型号参数初始化
static void machine_model_size_init(void);          ///< 机器型号尺寸初始化
static void machine_model_home_pos_init(void);      ///< 机器型号归零位置初始化
static void machine_model_sys_init(void);

void machine_model_init(void)
{
  machine_model_param_init();
  machine_model_size_init();
  machine_model_home_pos_init();
  machine_model_sys_init();
  model_init();
}

static void machine_model_sys_init(void)
{
  // 3d打印基本参数
  ccm_param.t_sys.step = DEFAULT_STEP;
  ccm_param.t_sys.axis_num = MAX_NUM_AXIS;
  ccm_param.t_sys.enable_check_door_open = false;
  ccm_param.t_sys.enable_z_max_limit = false;
  ccm_param.t_sys.enable_board_test = false;
  ccm_param.t_sys.enable_abs = true;
  ccm_param.t_sys.enable_hot_bed = true;
  ccm_param.t_sys.enable_warning_light = false;
  ccm_param.t_sys.enable_led_light = false;
  ccm_param.t_sys.is_lighting_on = false;
  ccm_param.t_sys.is_lighting_auto_ctr = true;
  ccm_param.t_sys.filament_load_unload_temp = 230;
  ccm_param.t_sys.pla_preheat_ext_temp = 180;
  ccm_param.t_sys.pla_preheat_bed_temp = 70;
  ccm_param.t_sys.abs_preheat_ext_temp = 230;
  ccm_param.t_sys.abs_preheat_bed_temp = 100;
  ccm_param.t_sys.extrude_min_temp = EXTRUDE_MINTEMP;

  if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
    ccm_param.t_sys.axis_num = MAX_NUM_AXIS; // 混色机型为5轴

  if (1 == ccm_param.t_sys_data_current.enable_powerOff_recovery)
    ccm_param.t_sys.enable_z_max_limit = true; //断电续打必须有下限位开关

  if (1 == ccm_param.t_sys_data_current.have_set_machine)
    ccm_param.t_sys.enable_board_test = false; // 已经设置好机型，关闭电路板测试功能
}

static void machine_model_param_init(void)
{
  // 3d打印机型配置信息
  // 反转轴初始化
  ccm_param.t_model.enable_invert_dir[X_AXIS] = INVERT_X_DIR;
  ccm_param.t_model.enable_invert_dir[X2_AXIS] = INVERT_X2_DIR;
  ccm_param.t_model.enable_invert_dir[Y_AXIS] = INVERT_Y_DIR;
  ccm_param.t_model.enable_invert_dir[Z_AXIS] = INVERT_Z_DIR;
  ccm_param.t_model.enable_invert_dir[Z2_AXIS] = INVERT_Z2_DIR;
  ccm_param.t_model.enable_invert_dir[E_AXIS] = INVERT_E0_DIR;
  ccm_param.t_model.enable_invert_dir[B_AXIS] = INVERT_E1_DIR;
  // 最大位置初始化
  ccm_param.t_model.xyz_max_pos[X_AXIS] = X_MAX_POS;
  ccm_param.t_model.xyz_max_pos[X2_AXIS] = X2_MAX_POS;
  ccm_param.t_model.xyz_max_pos[Y_AXIS] = Y_MAX_POS;
  ccm_param.t_model.xyz_max_pos[Z_AXIS] = Z_MAX_POS;
  ccm_param.t_model.xyz_max_pos[Z2_AXIS] = Z2_MAX_POS;
  // 最小位置初始化
  ccm_param.t_model.xyz_min_pos[X_AXIS] = X_MIN_POS;
  ccm_param.t_model.xyz_min_pos[X2_AXIS] = X2_MIN_POS;
  ccm_param.t_model.xyz_min_pos[Y_AXIS] = Y_MIN_POS;
  ccm_param.t_model.xyz_min_pos[Z_AXIS] = Z_MIN_POS;
  ccm_param.t_model.xyz_min_pos[Z2_AXIS] = Z2_MIN_POS;
  // 归零位置初始化
  ccm_param.t_model.xyz_home_pos[X_AXIS] = X_MIN_POS;
  ccm_param.t_model.xyz_home_pos[X2_AXIS] = X2_MAX_POS;
  ccm_param.t_model.xyz_home_pos[Y_AXIS] = Y_MIN_POS;
  ccm_param.t_model.xyz_home_pos[Z_AXIS] = Z_MIN_POS;
  ccm_param.t_model.xyz_home_pos[Z2_AXIS] = Z2_MIN_POS;
  // 最大行程初始化
  ccm_param.t_model.xyz_max_length[X_AXIS] = X_MAX_LENGTH;
  ccm_param.t_model.xyz_max_length[X2_AXIS] = X2_MAX_LENGTH;
  ccm_param.t_model.xyz_max_length[Y_AXIS] = Y_MAX_LENGTH;
  ccm_param.t_model.xyz_max_length[Z_AXIS] = Z_MAX_LENGTH;
  ccm_param.t_model.xyz_max_length[Z2_AXIS] = Z2_MAX_LENGTH;
  // 归零E电机反抽长度
  ccm_param.t_model.xyz_home_retract_mm[X_AXIS] = X_HOME_RETRACT_MM;
  ccm_param.t_model.xyz_home_retract_mm[X2_AXIS] = X2_HOME_RETRACT_MM;
  ccm_param.t_model.xyz_home_retract_mm[Y_AXIS] = Y_HOME_RETRACT_MM;
  ccm_param.t_model.xyz_home_retract_mm[Z_AXIS] = Z_HOME_RETRACT_MM;
  ccm_param.t_model.xyz_home_retract_mm[Z2_AXIS] = Z2_HOME_RETRACT_MM;
  // 归零方向初始化
  ccm_param.t_model.xyz_home_dir[X_AXIS] = X_HOME_DIR;
  ccm_param.t_model.xyz_home_dir[X2_AXIS] = X2_HOME_DIR;
  ccm_param.t_model.xyz_home_dir[Y_AXIS] = Y_HOME_DIR;
  ccm_param.t_model.xyz_home_dir[Z_AXIS] = Z_HOME_DIR;
  ccm_param.t_model.xyz_home_dir[Z2_AXIS] = Z2_HOME_DIR;
  // 最大移动位置初始化
  ccm_param.t_model.xyz_move_max_pos[X_AXIS] = X_MAX_POS;
  ccm_param.t_model.xyz_move_max_pos[X2_AXIS] = X2_MAX_POS;
  ccm_param.t_model.xyz_move_max_pos[Y_AXIS] = Y_MAX_POS;
  ccm_param.t_model.xyz_move_max_pos[Z_AXIS] = Z_MAX_POS;
  ccm_param.t_model.xyz_move_max_pos[Z2_AXIS] = Z2_MAX_POS;
  // Z原始最大位置初始化
  ccm_param.t_model.z_max_pos_origin = Z_MAX_POS;
}

static void machine_model_size_init(void)
{
  //根据model, 查表获取机型尺寸
  for (int i = 0; i < XYZ_NUM_AXIS; i++)
    ccm_param.t_model.xyz_max_pos[i] = (model_size_table[ccm_param.t_sys_data_current.model_id][i]);

  // 保存初始Z最大位置
  ccm_param.t_model.z_max_pos_origin = ccm_param.t_model.xyz_max_pos[Z_AXIS];

  for (int i = 0; i < XYZ_NUM_AXIS; i++)
  {
    ccm_param.t_model.xyz_max_length[i] = ccm_param.t_model.xyz_max_pos[i];
    ccm_param.t_model.xyz_move_max_pos[i] = ccm_param.t_model.xyz_max_pos[i];
  }

  ccm_param.t_model.extrude_maxlength = ccm_param.t_model.xyz_max_length[X_AXIS] + ccm_param.t_model.xyz_max_length[Y_AXIS];
}

static void machine_model_home_pos_init(void)
{
  for (int i = 0; i < XYZ_NUM_AXIS; i++)
  {
    if (ccm_param.t_model.xyz_home_dir[i] == -1)
    {
      ccm_param.t_model.xyz_home_pos[i] = ccm_param.t_model.xyz_min_pos[i];;
    }
    else
    {
      ccm_param.t_model.xyz_home_pos[i] = ccm_param.t_model.xyz_move_max_pos[i];
    }
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

