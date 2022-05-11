#ifndef LCD_ILI9488_H
#define LCD_ILI9488_H
#ifdef __cplusplus
extern "C" {
#endif
extern void ILI9488_Lcd_Init(void);
extern void ILI9488_Fsmc_ReInit(void);
extern uint16_t ILI9488_Get_IC_ID(void);
extern void ILI9488_Lcd_Color_Fill(uint16_t Xcmd, uint16_t Ycmd, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, const uint16_t *color);
extern void ILI9488_Lcd_Scan_Dir(uint16_t width, uint16_t height, uint16_t Xcmd, uint16_t Ycmd, uint8_t dir);
#ifdef __cplusplus
} //extern "C"
#endif

#endif // LCD_ILI9488_H



