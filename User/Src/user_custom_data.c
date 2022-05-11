#include "user_common.h"

#ifdef __cplusplus
extern "C" {
#endif

const char axis_codes[MAX_NUM_AXIS] = {'X', 'I', 'Y', 'Z', 'J', 'E', 'B'};

//機型尺寸表
const short model_id_table[]  =
{
  //{x_max_pos,y_max_pos,z_max_pos}
  M14,       //M14
  M2030,     //M2030
  M2041,     //M2041
  M2048,     //M2048
  M3145,     //M3145
  M4141,     //M4141
  M4040,     //M4040
  M4141S,    //M4141S
  AMP410W,   //AMP410W
  M14R03,    //M14R03
  M2030HY,   //M2030HY
  M14S,      //M14S
  M3145S,    //M3145S
  M15,       //M15
  M3036,     //M3036
  M4141S_NEW,//M4141S_NEW M41S改版
  M41G,      //M41G
  M3145T,    //M3145T
  M3145K,    //M3145T
  K5,         //K5
  F400TP,    //F400TP
  F1000TP,   //F1000TP
  P2_Pro,    //P2_Pro
  P3_Pro,    //P3_Pro
  Drug,      //Drug
  F300TP,    //F300TP
  P3_Pro_Mini,     //P2_Pro
  K600,    //K600
  M30
};

//機型尺寸表
const float model_size_table[][XYZ_NUM_AXIS]  =
{
  //{x_max_pos, x2_max_pos, y_max_pos, z_max_pos, z2_max_pos}
  {148, 148, 148, 158, 158}, //M14
  {208, 208, 208, 308, 308}, //M2030
  {208, 208, 208, 418, 418}, //M2041
  {208, 208, 208, 488, 488}, //M2048
  {318, 318, 318, 458, 458}, //M3145
  {418, 418, 418, 418, 418}, //M4141
  {408, 408, 408, 408, 408}, //M4040
  {418, 418, 418, 418, 418}, //M4141S
  {418, 418, 418, 418, 418}, //AMP410W
  {148, 148, 148, 158, 158}, //M14R03
  {208, 208, 208, 308, 308}, //M2030HY
  {148, 148, 148, 148, 148}, //M14S
  {318, 318, 318, 458, 458}, //M3145S
  {153, 153, 153, 168, 168}, //M15
  {308, 308, 308, 368, 368}, //M3036
  {418, 418, 418, 418, 418}, //M4141S-NEW
  {418, 418, 418, 418, 418}, //M41G
  {318, 318, 318, 458, 458}, //M3145T
  {318, 318, 318, 458, 458}, //M3145K
  {208, 208, 208, 300, 300}, //K5
  {600, 600, 600, 800, 800}, //F400TP
  {1000, 1000, 1000, 1000, 1000}, //F1000TP
  {260, 260, 260, 260, 260}, //P2 Pro
  {410, 410, 300, 360, 360}, //P3 Pro
  {360, 360, 300, 105, 360}, //Drug
  {330, 330, 400, 600, 600}, //F300TP
  {329, 329, 220, 250, 250},  //P2 Pro NEW
  {677.5, 677.5, 675, 800, 800}, //K600
  {318, 318, 318, 458, 458}, //M30
};

// 機型名稱表
const char *model_name_table[] =
{
  //{x_max_pos,y_max_pos,z_max_pos}
  (char *)"M14",     //M14
  (char *)"M2030",   //M2030
  (char *)"M2041",   //M2041
  (char *)"M2048",   //M2048
  (char *)"M3145",   //M3145
  (char *)"M4141",   //M4141
  (char *)"M4040TP", //M4040
  (char *)"M4141S",  //M4141S
  (char *)"AMP410W", //AMP410W
  (char *)"M14R03",  //M14R03
  (char *)"M2030HY", //M2030HY
  (char *)"M14S",    //M14S
  (char *)"M3145S",  //M3145S
  (char *)"M15",     //M15
  (char *)"M3036",   //M3036
  (char *)"M4141S_N",//M4141S_NEW
  (char *)"M41G",    //M41G
  (char *)"M3145T",  //M3145T
  (char *)"M3145K",  //M3145K
  (char *)"K5",      //K5
  (char *)"F400TP",  //F400TP
  (char *)"F1000TP", //F1000TP
  (char *)"P2 Pro",  //P2 Pro
  (char *)"P3 Pro",  //P3 Pro
  (char *)"Drug",    //Drug
  (char *)"F300TP",  //F300TP
  (char *)"P2 Pro",   //P2 Pro NEW
  (char *)"K600",   //K600
  (char *)"M30",  //M3145K
};
#ifdef __cplusplus
} // extern "C" {
#endif


