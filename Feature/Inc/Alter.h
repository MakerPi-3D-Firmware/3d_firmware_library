#ifndef ALTER_H
#define ALTER_H
#include "stdint.h"
#include "stdbool.h"

#define TIME_PER   1000

#define MM_PER_ARC_SEGMENT 1
#define N_ARC_CORRECTION 25
#define LASER_MODE 1

#ifdef __cplusplus
extern "C" {
#endif
  typedef enum
  {
    MODE_MIAN = 0,
    MODE_SET,
    MODE_PREPARE,
    MODE_FILUM,
    MODE_FILUM_LEVEL,
    MODE_SET_LAN,
  }
  MODEValueTypeDef;

  extern MODEValueTypeDef Gui_Mode;
  extern char TempMinErr_cnt;
  extern bool Zaxis_RunOnce;
  extern uint8_t TextDisplayX;

  char Check_TempMinErr(void);
  void While_Pro(void);

#if LASER_MODE
  void Laser_change_warn(void);
  void Laser_nonsupport(void);
  void Laser_up(void);
  void Laser_change_sure(void);
#endif


#ifdef __cplusplus
} // extern "C" {
#endif


#endif

