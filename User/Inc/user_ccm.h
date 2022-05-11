#ifndef USER_CCM_H
#define USER_CCM_H

#include "../../Grbl/Inc/planner_block_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


#define PIC_BUF_SIZE          (1024*38) // 12k大小
#define MAX_CMD_SIZE          96        // 定义最大接收字节数 96
#define POWER_OFF_BUF_SIZE    512       //  数据长度不能超过512，否则溢出,以字符串形式存储，一次性写入到文件11
#define BUFSIZE               1   /*!< 定义环形数组大小为 256 */
#ifndef ENABLE_GUI_LVGL
#define TEXT_BUF_24_12_9_SIZE (24*12*9)
#define TEXT_BUF_24_12_4_SIZE (24*12*4)
#else
#define TEXT_BUF_24_12_9_SIZE (2)
#define TEXT_BUF_24_12_4_SIZE (2)
#endif
#define FILE_READ_SIZE  (6*1024)

#define MEDIUM_FILE_SIZE 8
#define MEDIUM_FILE_NAME_SIZE 100

#define CAL_Z_MAX_POS_OFFSET 20.0F                 // 校準Z最大位置時，校準值與原來MAX Z的偏移量

typedef struct
{
  uint32_t max_acceleration_mm_per_s2[MAX_NUM_AXIS], // (mm/s^2) M201 XYZE
           min_segment_time_us;                // (µs) M205 B
  uint32_t axis_steps_per_sqr_second[MAX_NUM_AXIS];
  long drop_segments;
  float axis_steps_per_mm[MAX_NUM_AXIS];    // (steps) M92 XYZE - Steps per millimeter
  float max_feedrate_mm_s[MAX_NUM_AXIS];    // (mm/s) M203 XYZE - Max speeds
  float acceleration,                       // (mm/s^2) M204 S - Normal acceleration. DEFAULT ACCELERATION for all printing moves.
        retract_acceleration,               // (mm/s^2) M204 R - Retract acceleration. Filament pull-back and push-forward while standing still in the other axes
        travel_acceleration;                // (mm/s^2) M204 T - Travel acceleration. DEFAULT ACCELERATION for all NON printing moves.
  float min_feedrate_mm_s,                  // (mm/s) M205 S - Minimum linear feedrate
        min_travel_feedrate_mm_s;           // (mm/s) M205 T - Minimum travel feedrate
  float max_xy_jerk,                        // (mm/s) M205 X - speed than can be stopped at once, if i understand correctly.
        max_z_jerk,                         // (mm/s) M205 Z - speed than can be stopped at once, if i understand correctly.
        max_e_jerk,                         // (mm/s) M205 E - speed than can be stopped at once, if i understand correctly.
        max_b_jerk;                         // (mm/s) M205 E - speed than can be stopped at once, if i understand correctly.
  bool axis_relative_modes[MAX_NUM_AXIS];   // (false/true) M82/M83
  bool relative_mode;                       // (false/true) G90/G91 - Determines Absolute or Relative Coordinates
} planner_settings_t;

typedef struct
{
  char boot_ver_str[20];                   // boot版本
  char app2_ver_str[20];                   // app2版本
  char build_date_str[20];                 // 制作时间
  char model_str[30];                           // 机型字符串
  char function_str[35];                        // 功能字符串
  char version_str[30];                         // 版本字符串
  TCHAR print_name[_MAX_LFN];
  TCHAR dir_name[_MAX_LFN];  //选中的目录名
  int32_t printed_time_sec;                         // 已打印的时间
  int32_t used_total_material;                      // 耗材总长度
  long machine_run_time;                            // 机器运行时间
  int16_t nozzle_temp[2];                           // 喷嘴温度
  int16_t target_nozzle_temp[2];                    // 喷嘴目标温度
  float move_xyz_pos[XYZ_NUM_AXIS];               // 移动XYZ轴
  uint8_t is_refresh_rtc;                           // 是否GUI刷新
  int16_t hot_bed_temp;                             // 热床温度
  int16_t target_hot_bed_temp;                      // 热床目标温度
  uint16_t print_speed_value;                       // 打印速度
  uint16_t fan_speed_value;                         // 风扇速度
  uint16_t cavity_temp;                             //
  uint16_t target_cavity_temp;                      //
  uint16_t target_cavity_temp_on;                   //
  uint8_t print_percent;                            // 打印进度-百分数
  uint32_t printfile_size;                          // 打印文件总大小（字节）
  uint32_t file_size;                               // 打印文件剩余大小（字节）
  uint16_t cura_speed;                              // 获取到的cura软件上的速度，M117命令传送
} T_GUI;

typedef struct
{
  unsigned char have_set_machine;          // 是否已經設置機器
  unsigned char model_id;                  // 要改变为哪种机型
  unsigned char pic_id;                    // 要改变为哪种图片 中文图片或日文图片
  unsigned char enable_powerOff_recovery;  // 是否有斷電功能
  float poweroff_rec_z_max_value;          // 斷電z最大高度
  unsigned char enable_color_mixing;       // 是否有混色功能
  unsigned char enable_material_check;     // 是否有断料检测功能
  float material_chk_vol_value;            //
  unsigned char enable_block_detect;       // 是否開啓堵料檢測
  float bed_level_z_at_left_front;         //
  float bed_level_z_at_right_front;        //
  float bed_level_z_at_left_back;          //
  float bed_level_z_at_right_back;         //
  float bed_level_z_at_middle;             //
  float pid_output_factor;                 // 加热系数
  unsigned char enable_bed_level;          // 是否开启自动调平
  unsigned char enable_soft_filament;      // 是否使用软料
  unsigned char enable_LOGO_interface;     // 是否开启开机LOGO界面
  unsigned char logo_id;                   // logo图片的id号
  unsigned char custom_model_id;           // 定制机型id
  unsigned char buzzer_value;              // 按键声、报警声开关
  float z_offset_value;                    // Z零点偏移
  unsigned char enable_v5_extruder;//25
  unsigned char enable_cavity_temp;//26
  unsigned char enable_type_of_thermistor;//27
  unsigned char enable_high_temp;//28
  unsigned char ui_number;//29
  unsigned char is_2GT;//30
  unsigned char IsMechanismLevel;//31
  unsigned char IsLaser;//32
  unsigned char IsLaserMode;//33
  unsigned char lcd_type;
  unsigned char cf35;
  unsigned char cf36;
  unsigned char cf37;
  unsigned char cf38;
  unsigned char cf39;
  unsigned char cf40;
} T_SYS_DATA;

typedef struct
{
  bool sound_key;
  bool sound_alarm;
  bool lcd_ssd1963_43_480_272;
  bool enable_cavity_temp;            /*!< 是否开启腔体温度开关 */
  bool enable_color_buf;
  bool enable_warning_light;          /*!< 是否有警示灯 */
  bool enable_led_light;              /*!< 是否有LED照明 */
  bool enable_abs;                    /*!< 是否能打印ABS */
  bool enable_hot_bed;                /*!< 是否开启热床 */
  bool enable_board_test;               // 是否開啓大板測試
  bool enable_check_door_open;          // 是否有门检测
  bool enable_z_max_limit;             // 是否有Z轴下限位开关
  bool is_power_off_z_not_move;
  bool is_power_off_prepare_done; //是否完成了断电续打的准备
  bool is_power_off_file_from_user;                      // 断电续打文件是否在SD卡中
  bool is_power_off_rec_flag;                         // 是否已经断电
  bool is_power_off_rec_data_chg;                         // 是否已经断电
  bool is_bed_level_down_to_zero;     /*!< 平台是否下归零 */
  bool is_abl_init;                   /*!< 是否初始化abl */
  bool is_touch_capacitive;
  bool is_filament_heat_done;         /*!< 是否完成加热-进丝、退丝 */
  bool is_filament_control_done;      /*!< 是否完成进丝、退丝 */
  bool is_filament_print_not_have;    /*!< 在打印的时候是否没料了 */
  bool is_gcode_g28_done;             /*!< 是否归零完成 */
  bool is_gcode_m109_done;            /*!< 是否加热完成 */
  bool is_gcode_m190_done;            /*!< 是否加热完成 */
  bool is_z_max_pos_cal_finish;       /*!< 是否完成了Z轴的行程测量 */
  bool is_z_max_pos_cal_home;
  bool is_idex_extruder;              /*!< 是否为idex结构 */
  bool is_gui_warning_show;  //错误警告
  bool is_gui_warning_start;
  bool is_vsp_trans_file;  //是否在上传文件
  bool is_vsp_control_stop;
  bool is_vsp_control_resume;
  bool is_vsp_control_pause;
  bool is_door_open; //M14R03,M14S检测到门是否打开
  bool is_door_open_disp_info; //是否显示门打开的提示信息
  bool is_door_open_beep; //是否打开蜂鸣器
  bool is_medium_root_dir;  //当前目录是否是根目录
  bool is_medium_have_next_page;  //是否有下一页
  bool is_medium_have_file[MEDIUM_FILE_SIZE];//是否有需要显示的文件名
  bool is_medium_dir[MEDIUM_FILE_SIZE];  //当前GUI显示的文件是否是目录文件
  bool is_print_e1_offset_flag;                         // 是否已经断电
  bool is_dual_z_axis;
  bool is_lighting_on;
  bool is_lighting_auto_ctr;
  bool is_gui_waiting_done;
  bool print_type_idex_single_mode;   /*!< 是否为idex结构 */
  bool print_flag;
  bool print_pause_flag;
  bool print_is_home;
  unsigned char axis_num;             /*!< 機型軸數 */
  unsigned char step;                 /*!< 機型細分數 1:16 2:32; */
  unsigned char lcd_type;             /*!< lcd类型 */
  unsigned char print_type_idex;      /*!< Idex print type===>>0:normal 1:copy 2:mirror */
  unsigned char print_type_mix;       /*!< mix打印类型 */
  unsigned char medium_current_page;  //当前GUI要显示的页面-当前目录下的文件分成多个GUI页面
  unsigned char medium_page_file_num;
  TCHAR medium_current_path[MEDIUM_FILE_NAME_SIZE];                  //当前目录的路径
  TCHAR medium_disp_file_name[MEDIUM_FILE_SIZE][MEDIUM_FILE_NAME_SIZE];  //当前GUI要显示的文件名
  unsigned int print_time_save;
  int gui_warning_select;
  int filament_load_unload_temp;
  int pla_preheat_ext_temp;
  int pla_preheat_bed_temp;
  int abs_preheat_ext_temp;
  int abs_preheat_bed_temp;
  float extrude_min_temp;
} T_SYS;

typedef struct
{
  int index_v;  //信号量值
  int TargetNozzleTemp; //喷嘴目标温度
  int TargetNozzleTemp1; //喷嘴目标温度
  int TargetHotbedTemp;  //热床目标温度
  int PrintSpeed;  //打印速度
  int FanSpeed;  //风扇速度
  int TargetCavityTemp; //喷嘴目标温度
  int TargetCavityOnTemp; //喷嘴目标温度
  volatile int x_move_value;
  volatile uint8_t active_extruder;
} T_GUI_SEMP;

// 运行状态
typedef struct
{
  volatile float axis_position[MAX_NUM_AXIS];
  volatile float axis_real_position[MAX_NUM_AXIS];
  volatile float z_real_change_value;
  volatile float target_bed_temp;                                   /*!< 热床目标温度 */
  volatile float target_extruder0_temp;                             /*!< 喷嘴目标温度 */
  volatile float target_extruder1_temp;                             /*!< 喷嘴目标温度 */
  volatile float feed_rate;                                         /*!< 进料速度 */
  volatile uint32_t sd_position;                                    /*!< 文件位置 */
  volatile uint32_t print_time_save;                                /*!< 打印时间 */
  volatile int32_t extruder_multiply;                               /*!< 移动速度百分比 */
  volatile int32_t feed_multiply;                                   /*!< 进料速度百分比 */
  volatile int32_t fan_speed;                                       /*!< 风扇速度 */
  volatile int32_t layer_count;                                     /*!< 打印文件层数 */
  volatile int32_t current_layer;                                   /*!< 当前层数 */
  volatile uint8_t enable_color_buf;                                /*!< 是否混色 */
  volatile uint8_t active_extruder;                                 /*!< 当前活动喷头 */
  volatile uint8_t is_serial;                                       /*!< 串口标志位 */
  volatile uint8_t flag;                                            /*!< 断电标志位 */
} planner_running_status_t;

typedef struct
{
  float xyz_max_pos[MAX_NUM_AXIS];                // XYZ最大位置
  float xyz_min_pos[MAX_NUM_AXIS];                // XYZ最小位置
  float xyz_home_pos[MAX_NUM_AXIS];               // XYZ零點位置
  float xyz_max_length[MAX_NUM_AXIS];             // XYZ最大長度
  float xyz_home_retract_mm[MAX_NUM_AXIS];        // XYZ歸零回抽距離mm
  float xyz_move_max_pos[MAX_NUM_AXIS];           // XYZ最大移动位置
  float extrude_maxlength;             // prevent extrusion of very large distances.
  float z_max_pos_origin;              // 保存机型默认Z最大点，用于校准Z高度
  bool enable_invert_dir[MAX_NUM_AXIS];  // 是否反轉軸方向
  signed char xyz_home_dir[MAX_NUM_AXIS];         // XYZ方向
} T_MODEL;

typedef struct
{
  float current_save_xyz[XYZ_NUM_AXIS];         ///< 保存当前XYZ位置，校准前
  float current_xyz_compensate[XYZ_NUM_AXIS];   ///< 保存当前XYZ位置补偿
  float current_xyz_real[XYZ_NUM_AXIS];         ///< 保存当前XYZ真实位置，校准后
} T_PLANNER_LEVEL;

typedef struct
{
  volatile T_SYS_DATA t_sys_data_current;                              /*!<  */
  volatile T_SYS t_sys;                                                /*!<  */
  volatile T_MODEL t_model;                          /*!<  */
  volatile T_PLANNER_LEVEL t_planner_level;
  volatile planner_settings_t planner_settings;
  volatile block_t block_buffer[BLOCK_BUFFER_SIZE];                    /*!< A ring buffer for motion instfructions */
  volatile unsigned char block_buffer_head;                            /*!< Index of the next block to be pushed */
  volatile unsigned char block_buffer_tail;                            /*!< Index of the block to process now */
  TCHAR power_path_file_name[100];                     // 断电续打文件路徑全名
  volatile planner_running_status_t runningStatus[BLOCK_BUFFER_SIZE];  /*!< 运动参数数组 */
  volatile planner_running_status_t running_status;                    /*!< 当前运动参数 */
  volatile planner_running_status_t flash_poweroff_recovery;
  volatile planner_running_status_t t_power_off;
  volatile T_GUI t_gui;                                                /*!<  */
  volatile T_GUI_SEMP t_gui_semp;
  volatile char command_buffer[MAX_CMD_SIZE];                          /*!< 指令数组 */
  char os_gcode_buf[MAX_CMD_SIZE];
  volatile float grbl_current_position[MAX_NUM_AXIS];                  /*!< */
  volatile float grbl_destination[MAX_NUM_AXIS];                       /*!< */
  volatile int32_t layer_count;                                        /*!< */
  volatile int32_t current_layer;                                      /*!< */
  char poweroff_data[POWER_OFF_BUF_SIZE];                              /*!< 斷電續打數據緩存  处于IRAM2区的起始地址7k */
  char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];                               /*!< 环形指令队列 */
  char file_read_buf[FILE_READ_SIZE];                                  /*!< 读取文件数组对象 */
  volatile int file_read_buf_index;                                    /*!<  */
  char PictureFileBuf[PIC_BUF_SIZE];                                   /*!< 处于IRAM2区的起始地址 40k 大小12k */
} ccm_param_t;

extern ccm_param_t ccm_param;
extern void user_ccm_param_init(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif //USER_CCM_H




