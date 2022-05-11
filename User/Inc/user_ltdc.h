#ifndef USER_LTDC_H
#define USER_LTDC_H

#ifdef ENABLE_LTDC

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint16_t ltdc_id;

extern void user_ltdc_read_panel_id(void);


//LCD LTDC重要参数集
typedef struct
{
  u32 pwidth;     //LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
  u32 pheight;    //LCD面板的高度,固定参数,不随显示方向改变
  u16 hsw;      //水平同步宽度
  u16 vsw;      //垂直同步宽度
  u16 hbp;      //水平后廊
  u16 vbp;      //垂直后廊
  u16 hfp;      //水平前廊
  u16 vfp;      //垂直前廊
  u8 activelayer;   //当前层编号:0/1
  u8 dir;       //0,竖屏;1,横屏;
  u16 width;      //LCD宽度
  u16 height;     //LCD高度
  u32 pixsize;    //每个像素所占字节数
} _ltdc_dev;

extern _ltdc_dev lcdltdc;               //管理LCD LTDC参数
//extern LTDC_HandleTypeDef LTDC_Handler;     //LTDC句柄
//extern DMA2D_HandleTypeDef DMA2D_Handler;   //DMA2D句柄

#define LCD_PIXEL_FORMAT_ARGB8888       0X00
#define LCD_PIXEL_FORMAT_RGB888         0X01
#define LCD_PIXEL_FORMAT_RGB565         0X02
#define LCD_PIXEL_FORMAT_ARGB1555       0X03
#define LCD_PIXEL_FORMAT_ARGB4444       0X04
#define LCD_PIXEL_FORMAT_L8             0X05
#define LCD_PIXEL_FORMAT_AL44           0X06
#define LCD_PIXEL_FORMAT_AL88           0X07

///////////////////////////////////////////////////////////////////////
//用户修改配置部分:

//定义颜色像素格式,一般用RGB565
#define LCD_PIXFORMAT       LCD_PIXEL_FORMAT_RGB565
//定义默认背景层颜色
#define LTDC_BACKLAYERCOLOR     0X00000000
//LCD帧缓冲区首地址,这里定义在SDRAM里面.
#define LCD_FRAME_BUF_ADDR      0XC0000000

void user_ltdc_switch(u8 sw);          //LTDC开关
void user_ltdc_layer_switch(u8 layerx, u8 sw); //层开关
void user_ltdc_select_layer(u8 layerx);      //层选择
void user_ltdc_display_dir(u8 dir);        //显示方向控制
void user_ltdc_draw_point(u16 x, u16 y, u32 color); //画点函数
u32 user_ltdc_read_point(u16 x, u16 y);    //读点函数
void user_ltdc_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color);  //矩形单色填充函数
void user_ltdc_color_fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color); //矩形彩色填充函数
void user_ltdc_clear(u32 color);         //清屏函数
u8 user_ltdc_clk_set(u32 pllsain, u32 pllsair, u32 pllsaidivr); //LTDC时钟配置
void user_ltdc_layer_window_config(u8 layerx, u16 sx, u16 sy, u16 width, u16 height); //LTDC层窗口设置
void user_ltdc_layer_parameter_config(u8 layerx, u32 bufaddr, u8 pixformat, u8 alpha, u8 alpha0, u8 bfac1, u8 bfac2, u32 bkcolor); //LTDC基本参数设置
//u16 LTDC_PanelID_Read(void);        //LCD ID读取函数
void user_ltdc_init(void);           //LTDC初始化函数



#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // USER_LTDC_H












