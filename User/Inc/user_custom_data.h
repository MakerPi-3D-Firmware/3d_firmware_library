#ifndef USER_CUSTOM_DATA_H
#define USER_CUSTOM_DATA_H

#define F_CPU 16000000UL

#define HIGH 0x1
#define LOW  0x0

#ifndef min
  #define min(a,b) ((a)<(b)?(a):(b))
#endif // min

#ifndef max
  #define max(a,b) ((a)>(b)?(a):(b))
#endif // max

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define MAX_NUM_AXIS 7
#define XYZ_NUM_AXIS 5
enum AxisEnum {X_AXIS = 0, X2_AXIS = 1, Y_AXIS  = 2, Z_AXIS = 3, Z2_AXIS = 4, E_AXIS = 5, B_AXIS = 6};

extern const char axis_codes[MAX_NUM_AXIS];

//===========================================================================
//=============================Buffers           ============================
//===========================================================================

// The number of linear motions that can be in the plan at any give time.
// THE BLOCK_BUFFER_SIZE NEEDS TO BE A POWER OF 2, i.g. 8,16,32 because shifts and ors are used to do the ring-buffering.
#define BLOCK_BUFFER_SIZE 32 // maximize block buffer

#define  FORCE_INLINE  __attribute__((always_inline)) inline

#define MILLIS() xTaskGetTickCount()
#define TASK_ENTER_CRITICAL()  taskENTER_CRITICAL();
#define TASK_EXIT_CRITICAL()  taskEXIT_CRITICAL();
#define OS_DELAY(TICK_VALUE) osDelay(TICK_VALUE);

// 機型表
extern const short model_id_table[];
extern const float   model_size_table[][XYZ_NUM_AXIS];
extern const char *model_name_table[];


// 機型ID
enum ModelId
{
  M14 = 0,
  M2030 = 1,
  M2041 = 2,
  M2048 = 3,
  M3145 = 4,
  M4141 = 5,
  M4040 = 6, // 日本专用
  M4141S = 7,
  AMP410W = 8,
  M14R03 = 9,
  M2030HY = 10,
  M14S = 11,
  M3145S = 12,
  M15 = 13,
  M3036 = 14,
  M4141S_NEW = 15, //M4141S_NEW M41S改版
  M41G = 16,
  M3145T = 17,
  M3145K = 18,
  K5 = 19,
  F400TP = 20,
  F1000TP = 21,
  P2_Pro = 22,
  P3_Pro = 23,
  Drug = 24,
  F300TP = 25,
  P3_Pro_Mini = 26,
  K600 = 27,
  M30 = 28
};

#define LCD_TYPE_35_480320_SIZE 0 // 3.5寸 480*320
#define LCD_TYPE_43_480272_SIZE 1 // 4.3寸 480*272
#define LCD_TYPE_7_1024600_SIZE 2 // 7寸 1024*600

#define BED_LEVEL_PRESSURE_SENSOR 3 //红外激光头

#define EXTRUDER_TYPE_NONE 0
#define EXTRUDER_TYPE_SINGLE 1
#define EXTRUDER_TYPE_MIX 2
#define EXTRUDER_TYPE_DUAL 3
#define EXTRUDER_TYPE_LASER 4
#define EXTRUDER_TYPE_DRUG 5

#define IDEX_PRINT_TYPE_NORMAL 0
#define IDEX_PRINT_TYPE_COPY 1
#define IDEX_PRINT_TYPE_MIRROR 2

#define MIX_PRINT_TYPE_GRADIENT_COLOR 0
#define MIX_PRINT_TYPE_FIX_PROPORTION 1
#define MIX_PRINT_TYPE_RANDOM 2
///////////////////////////////////// SysConfig Start///////////////////////////////////////////

extern char sys_data[512];                      // sysconfig數據緩存
extern uint32_t sys_data_size;                  // sysconfig數據緩存大小

///////////////////////////////////// SysConfig End///////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 默认配置
#define DEFAULT_MODEL               0 // 默认配置//0:M14 1:M2030 2:M2041 3:M2048 4:M3145 5:M4141 6:M4040 7:M4141S 8:AMP410W 9:M14R03 10:M2030HY 11:M14S 12:M3145S    //4141S机型的X轴方向不一样
#define DEFAULT_COLORMIXING         0 // 1:打开混色功能 0:关闭混色功能
#define DEFAULT_POWEROFFRECOVERY    0 // 1:打开断电续打功能 0:关闭断电续打功能
#define DEFAULT_MATCHECK            0 // 1:打开断料检测功能 0：关闭断料检测功能
#define DEFAULT_STEP                1 // 1: 电机16细分 2： 电机32细分
#define DEFAULT_NUMAXIS             4 // 4: 基础版4个电机 5：混色版5个电机

// GUI圖片ID
#define PICTURE_IS_CHINESE  1
#define PICTURE_IS_JAPANESE 2
#define PICTURE_IS_ENGLISH  3
#define PICTURE_IS_KOREA    4
#define PICTURE_IS_RUSSIA   5
#define PICTURE_IS_CHINESE_TRADITIONAL   6

// 4.3寸屏UI
#define LCD_43_UI_TYPE_NONE 0
#define LCD_43_UI_TYPE_BLUE 1
#define LCD_43_UI_TYPE_BLACK 2

extern void sys_write_info(const TCHAR *filePath);
extern void sys_read_info(const TCHAR *filePath);

#endif // USER_CUSTOM_DATA_H

