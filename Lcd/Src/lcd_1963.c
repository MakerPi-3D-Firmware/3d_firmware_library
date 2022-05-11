#include "user_common.h"

#ifdef ENABLE_LCD

#if defined(ENABLE_FSMC)

#include "../Inc/lcd_common.h"

#ifdef HAL_SRAM_MODULE_ENABLED

//LCD分辨率设置
#define SSD_HOR_RESOLUTION    800   //LCD水平分辨率
#define SSD_VER_RESOLUTION    480   //LCD垂直分辨率
//LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH   1   //水平脉宽
#define SSD_HOR_BACK_PORCH    46    //水平前廊
#define SSD_HOR_FRONT_PORCH   210   //水平后廊

#define SSD_VER_PULSE_WIDTH   1   //垂直脉宽
#define SSD_VER_BACK_PORCH    23    //垂直前廊
#define SSD_VER_FRONT_PORCH   22    //垂直前廊
//如下几个参数，自动计算
#define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

//SSD1963 背光设置
//pwm:背光等级,0~100.越大越亮.
static void LCD_SSD_BackLightSet(u8 pwm)
{
  LCD_WR_REG(0xBE); //配置PWM输出
  LCD_WR_DATA(0x05);  //1设置PWM频率
  LCD_WR_DATA(pwm * 2.55); //2设置PWM占空比
  LCD_WR_DATA(0x01);  //3设置C
  LCD_WR_DATA(0xFF);  //4设置D
  LCD_WR_DATA(0x00);  //5设置E
  LCD_WR_DATA(0x00);  //6设置F
}

void LCD_Init_1963(void)
{
  LCD_WR_REG(0xE2);   //Set PLL with OSC = 10MHz (hardware),  Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
  LCD_WR_DATA(0x1D);    //参数1
  LCD_WR_DATA(0x02);    //参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz
  LCD_WR_DATA(0x04);    //参数3 Validate M and N values
  user_delay_us(100);
  LCD_WR_REG(0xE0);   // Start PLL command
  LCD_WR_DATA(0x01);    // enable PLL
  user_delay_ms(10);
  LCD_WR_REG(0xE0);   // Start PLL command again
  LCD_WR_DATA(0x03);    // now, use PLL output as system clock
  user_delay_ms(12);
  LCD_WR_REG(0x01);   //软复位
  user_delay_ms(10);
  LCD_WR_REG(0xE6);   //设置像素频率,33Mhz
  LCD_WR_DATA(0x2F);
  LCD_WR_DATA(0xFF);
  LCD_WR_DATA(0xFF);
  LCD_WR_REG(0xB0);   //设置LCD模式
  LCD_WR_DATA(0x20);    //24位模式
  LCD_WR_DATA(0x00);    //TFT 模式
  LCD_WR_DATA((SSD_HOR_RESOLUTION - 1) >> 8); //设置LCD水平像素
  LCD_WR_DATA(SSD_HOR_RESOLUTION - 1);
  LCD_WR_DATA((SSD_VER_RESOLUTION - 1) >> 8); //设置LCD垂直像素
  LCD_WR_DATA(SSD_VER_RESOLUTION - 1);
  LCD_WR_DATA(0x00);    //RGB序列
  LCD_WR_REG(0xB4);   //Set horizontal period
  LCD_WR_DATA((SSD_HT - 1) >> 8);
  LCD_WR_DATA(SSD_HT - 1);
  LCD_WR_DATA(SSD_HPS >> 8);
  LCD_WR_DATA(SSD_HPS);
  LCD_WR_DATA(SSD_HOR_PULSE_WIDTH - 1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB6);   //Set vertical period
  LCD_WR_DATA((SSD_VT - 1) >> 8);
  LCD_WR_DATA(SSD_VT - 1);
  LCD_WR_DATA(SSD_VPS >> 8);
  LCD_WR_DATA(SSD_VPS);
  LCD_WR_DATA(SSD_VER_FRONT_PORCH - 1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xF0); //设置SSD1963与CPU接口为16bit
  LCD_WR_DATA(0x03);  //16-bit(565 format) data for 16bpp
  LCD_WR_REG(0x29); //开启显示
  //设置PWM输出  背光通过占空比可调
  LCD_WR_REG(0xD0); //设置自动白平衡DBC
  LCD_WR_DATA(0x00);  //disable
  LCD_WR_REG(0xBE); //配置PWM输出
  LCD_WR_DATA(0x05);  //1设置PWM频率
  LCD_WR_DATA(0xFE);  //2设置PWM占空比
  LCD_WR_DATA(0x01);  //3设置C
  LCD_WR_DATA(0x00);  //4设置D
  LCD_WR_DATA(0x00);  //5设置E
  LCD_WR_DATA(0x00);  //6设置F
  LCD_WR_REG(0xB8); //设置GPIO配置
  LCD_WR_DATA(0x03);  //2个IO口设置成输出
  LCD_WR_DATA(0x01);  //GPIO使用正常的IO功能
  LCD_WR_REG(0xBA);
  LCD_WR_DATA(0X01);  //GPIO[1:0]=01,控制LCD方向
  LCD_SSD_BackLightSet(100);//背光设置为最亮
}

#endif
#endif
#endif
