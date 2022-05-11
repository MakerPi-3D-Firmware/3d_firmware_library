#ifndef LCD_COMMON_H
#define LCD_COMMON_H
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

#if defined(ENABLE_LTDC)
#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向
#elif defined(ENABLE_FSMC)
#define DFT_SCAN_DIR  R2L_D2U
#endif

#if defined(ENABLE_FSMC)
#ifdef HAL_SRAM_MODULE_ENABLED
extern SRAM_HandleTypeDef hsram1;
extern void LCD_WR_REG(__IO uint16_t regval);
extern void LCD_WR_DATA(__IO uint16_t data);
extern uint16_t LCD_RD_DATA(void);
extern void LCD_WriteRAM_Prepare(void);
extern void LCD_WriteReg(__IO uint16_t LCD_Reg, __IO uint16_t LCD_RegValue);
extern uint16_t LCD_ReadReg(__IO uint16_t LCD_Reg);
extern uint16_t LCD_RD_REG(void);
extern u16 LCD_BGR2RGB(u16 c);
extern void opt_delay(u8 i);
#endif
#endif

#ifdef __cplusplus
} //extern "C"
#endif
#endif // LCD_COMMON_H

