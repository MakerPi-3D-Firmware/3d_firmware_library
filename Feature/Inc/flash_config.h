#ifndef FLASH_CONFIG_H
#define FLASH_CONFIG_H

#include "user_common.h"
#include "../../Grbl/Inc/vector_3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_PARAM_VERSION ((uint32_t)20210318) //版本号
#define FLASH_UPDATE_FLAG ((uint32_t)20220423) //版本号

#define IDEX_EXT_BED_MIN_INDEX 0
#define IDEX_EXT_BED_MAX_INDEX 1

#define LEVEL_POS_COUNT 5
#define LEVEL_POS_INDEX_MIDDLE 0
#define LEVEL_POS_INDEX_LEFT_FRONT 1
#define LEVEL_POS_INDEX_LEFT_BACK 2
#define LEVEL_POS_INDEX_RIGHT_BACK 3
#define LEVEL_POS_INDEX_RIGHT_FRONT 4

#define FLASH_SETTINGS_VERSION ((uint32_t)1)

typedef struct
{
  planner_settings_t planner_settings;
  float home_offset[XYZ_NUM_AXIS];
  float temp_p;
  float temp_i;
  float temp_d;
  uint32_t settings_version;
} SETTINGS_T;

typedef struct
{
  volatile uint8_t flag;                         // 写flash标准位
  volatile uint8_t is_process_bedlevel;
  volatile uint8_t extruder_type;                // 喷嘴类型
  volatile uint8_t key_sound;
  volatile uint8_t alarm_sound;
  volatile uint8_t is_buzzer_chg;
  volatile uint8_t para8[26];
  volatile uint16_t para16[32];
  volatile uint32_t version;
  volatile uint32_t update_param_flag;
  volatile uint32_t para32[30];
  volatile float paraF0[23];
  volatile float z_offset_value;
  volatile float pid_output_factor;
  volatile float poweroff_rec_z_max_value;
  volatile float paraF[6];
  sg_grbl::matrix_3x3 matrix_front_left;
  sg_grbl::matrix_3x3 matrix_left_back;
  sg_grbl::matrix_3x3 matrix_back_right;
  sg_grbl::matrix_3x3 matrix_right_front;
  volatile float idex_ext1_ext0_offset[3];       // Idex喷头2偏移喷头1数值
  volatile float idex_extruder_0_bed_offset[2];  // Idex喷头1热床位置偏移量
  volatile float idex_extruder_1_bed_offset[2];  // Idex喷头2热床位置偏移量
  volatile float mix_extruder_0_bed_offset[2];   // 混色喷头热床位置偏移量
  volatile float laser_extruder_0_bed_offset[2]; // 激光头热床位置偏移量
  volatile float idex_ext0_home_pos_adding[3];   // Idex喷头1归零点偏移，主要用于红外检测Z归零
  volatile float mix_ext0_home_pos_adding[3];    // 混色喷头1归零点偏移，主要用于红外检测Z归零
  volatile float laser_ext0_home_pos_adding[3];  // 激光喷头1归零点偏移，主要用于红外检测Z归零
  volatile float laser_extruder_1_bed_offset[2]; // 激光头热床位置偏移量
  volatile float level_pos_z[LEVEL_POS_COUNT];
  volatile float idex_ext1_ext0_datum_pos[3];    //E0、E1基准点
  volatile float laser_focus_z_height;
  volatile float paraF1;
  TCHAR pathFileName[100];                            /*!< 文件路径名 */
  TCHAR fileName[100];                                /*!< 文件名 */
  volatile unsigned char idex_print_type;                     /*!< 打印类型 */
  volatile unsigned char mix_print_type;
  volatile float paraF2[128];
  SETTINGS_T settings;
} FLASH_PARAM_T;

extern FLASH_PARAM_T flash_param_t;

extern void flash_config_save(void);


#ifdef __cplusplus
} //extern "C"
#endif


#endif // FLASH_CONFIG_H

