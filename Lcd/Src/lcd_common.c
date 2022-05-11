#include "user_common.h"

#ifdef ENABLE_LCD

#if defined(ENABLE_FSMC)

#include "../Inc/lcd_common.h"
#include "../Inc/lcd.h"

#ifdef HAL_SRAM_MODULE_ENABLED

//////////////////////////////////////////////////////////////////////////////////
//-----------------MCU屏 LCD端口定义----------------
//LCD背光 PB5
#define LCD_LED     PBout(5)

//LCD地址结构体
typedef struct
{
  vu16 LCD_REG;
  vu16 LCD_RAM;
} LCD_TypeDef;

//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A6作为数据命令区分线
//注意设置时STM32内部会右移一位对其! 111 1110=0X7E
#define LCD_BASE    ((u32)(0x60000000 | 0x007ffffe))
#define LCD         ((LCD_TypeDef *) LCD_BASE)
#define LCD_WR_Reg  (LCD->LCD_REG)
#define LCD_WR_Data (LCD->LCD_RAM)
//////////////////////////////////////////////////////////////////////////////////

/**
 * @brief LCD_WR_REG 写寄存器函数
 * @param regval 寄存器值
 */
void LCD_WR_REG(__IO u16 regval)
{
  regval = regval;  //使用-O2优化的时候,必须插入的延时
  HAL_SRAM_Write_16b(&hsram1, (u32 *)&LCD_WR_Reg, (u16 *)&regval, 1);
}

/**
 * @brief LCD_RD_REG 读寄存器函数
 * @param regval 寄存器值
 */
u16 LCD_RD_REG(void)
{
  __IO u16 regval;
  HAL_SRAM_Read_16b(&hsram1, (u32 *)&LCD_WR_Reg, (u16 *)&regval, 1);
  return regval;
}

/**
 * @brief LCD_WR_DATA 写LCD数据
 * @param data 要写入的值
 */
void LCD_WR_DATA(__IO u16 data)
{
  data = data;    //使用-O2优化的时候,必须插入的延时
  HAL_SRAM_Write_16b(&hsram1, (u32 *)&LCD_WR_Data, (u16 *)&data, 1);
}

/**
 * @brief LCD_RD_DATA 读LCD数据
 * @return 读到的值
 */
u16 LCD_RD_DATA(void)
{
  __IO u16 ram; //防止被优化
  HAL_SRAM_Read_16b(&hsram1, (u32 *)&LCD_WR_Data, (u16 *)&ram, 1);
  return ram;
}

/**
 * @brief LCD_WriteReg 写寄存器
 * @param LCD_Reg 寄存器地址
 * @param LCD_RegValue 要写入的数据
 */
void LCD_WriteReg(__IO u16 LCD_Reg, __IO u16 LCD_RegValue)
{
  LCD_WR_REG(LCD_Reg);
  LCD_WR_DATA(LCD_RegValue);
}

/**
 * @brief LCD_ReadReg 读寄存器
 * @param LCD_Reg 寄存器地址
 * @return 读到的数据
 */
u16 LCD_ReadReg(__IO u16 LCD_Reg)
{
  LCD_WR_REG(LCD_Reg);  //写入要读的寄存器序号
  HAL_Delay(1);//delay_us(5);
  return LCD_RD_DATA(); //返回读到的值
}

/**
 * @brief LCD_WriteRAM_Prepare 开始写GRAM
 */
void LCD_WriteRAM_Prepare(void)
{
  LCD_WR_REG(lcddev.wramcmd);
}

/**
 * @brief LCD_WriteRAM LCD写GRAM
 * @param RGB_Code 颜色值
 */
void LCD_WriteRAM(u16 RGB_Code)
{
  LCD_WR_DATA(RGB_Code);//写十六位GRAM
}

/**
 * @brief LCD_BGR2RGB 从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
 * 通过该函数转换
 * @param c GBR格式的颜色值
 * @return RGB格式的颜色值
 */
u16 LCD_BGR2RGB(u16 c)
{
  u16  r, g, b, rgb;
  b = (c >> 0) & 0x1f;
  g = (c >> 5) & 0x3f;
  r = (c >> 11) & 0x1f;
  rgb = (u16)((b << 11) + (g << 5) + (r << 0));
  return (rgb);
}

/**
 * @brief opt_delay 当mdk -O1时间优化时需要设置
 * @param i 延时i
 */
void opt_delay(u8 i)
{
  while (i--);
}

#endif
#endif
#endif

