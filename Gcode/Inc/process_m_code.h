#ifndef PROCESS_M_CODE_H
#define PROCESS_M_CODE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//  void process_m114(void);
//  void process_m92(void);
//  void process_m221(void);
extern void process_m_code(void);                 ///< M代码执行入口
extern bool isM190HeatingComplete(void);          ///< 是否热床加热完成
extern void resetM190HeatingComplete(void);       ///< 重置热床加热完成状态

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCESS_M_CODE_H


