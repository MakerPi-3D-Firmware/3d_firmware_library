#ifndef LCD_NT35310_H
#define LCD_NT35310_H

#ifdef __cplusplus
extern "C" {
#endif
extern void NT35310_Fsmc_ReInit(void);
extern uint16_t NT35310_Get_IC_ID(void);
extern void NT35310_Lcd_Color_Fill(uint16_t Xcmd, uint16_t Ycmd, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, const uint16_t *color);
extern void NT35310_Lcd_Scan_Dir(uint16_t width, uint16_t height, uint16_t Xcmd, uint16_t Ycmd, uint8_t dir);
#ifdef __cplusplus
} //extern "C"
#endif

#endif // LCD_NT35310_H


