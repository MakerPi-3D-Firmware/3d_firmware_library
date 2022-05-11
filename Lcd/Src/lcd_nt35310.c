#include "user_common.h"

#ifdef ENABLE_LCD

#include "../Inc/lcd_nt35310.h"
#include "../Inc/lcd_common.h"

void NT35310_Lcd_Scan_Dir(uint16_t width, uint16_t height, uint16_t Xcmd, uint16_t Ycmd, uint8_t dir)
{
  uint16_t regval = 0;
  uint16_t dirreg = 0;
  uint16_t temp;

  switch (dir)
  {
  case L2R_U2D://从左到右,从上到下
    regval |= (0 << 7) | (0 << 6) | (0 << 5);
    break;

  case L2R_D2U://从左到右,从下到上
    regval |= (1 << 7) | (0 << 6) | (0 << 5);
    break;

  case R2L_U2D://从右到左,从上到下
    regval |= (0 << 7) | (1 << 6) | (0 << 5);
    break;

  case R2L_D2U://从右到左,从下到上
    regval |= (1 << 7) | (1 << 6) | (0 << 5);
    break;

  case U2D_L2R://从上到下,从左到右
    regval |= (0 << 7) | (0 << 6) | (1 << 5);
    break;

  case U2D_R2L://从上到下,从右到左
    regval |= (0 << 7) | (1 << 6) | (1 << 5);
    break;

  case D2U_L2R://从下到上,从左到右
    regval |= (1 << 7) | (0 << 6) | (1 << 5);
    break;

  case D2U_R2L://从下到上,从右到左
    regval |= (1 << 7) | (1 << 6) | (1 << 5);
    break;
  }

  dirreg = 0X36;
  LCD_WriteReg(dirreg, regval);

  if (regval & 0X20)
  {
    if (width < height) //交换X,Y
    {
      temp = width;
      width = height;
      height = temp;
    }
  }
  else
  {
    if (width > height) //交换X,Y
    {
      temp = width;
      width = height;
      height = temp;
    }
  }

  LCD_WR_REG(Xcmd);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  //LCD_WR_DATA((lcddev.width-1)>>8);
  //LCD_WR_DATA((lcddev.width-1)&0XFF);
  LCD_WR_DATA(width >> 8);
  LCD_WR_DATA(width & 0XFF);
  LCD_WR_REG(Ycmd);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  //    LCD_WR_DATA((lcddev.height-1)>>8);
  //    LCD_WR_DATA((lcddev.height-1)&0XFF);
  LCD_WR_DATA(height >> 8);
  LCD_WR_DATA(height & 0XFF);
}

void NT35310_Lcd_Color_Fill(uint16_t Xcmd, uint16_t Ycmd, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, const uint16_t *color)
{
  uint16_t height, width;
  uint16_t i, j;
  width = (ex - sx) + 1; //得到填充的宽度
  height = (ey - sy) + 1; //高度

  for (i = 0; i < height; i++)
  {
    LCD_SetCursor(sx, sy + i); //设置光标位置
    LCD_WriteRAM_Prepare();     //开始写入GRAM

    for (j = 0; j < width; j++)
    {
      LCD_WR_DATA(color[(i * width) + j]); //写入数据
    }
  }
}

uint16_t NT35310_Get_IC_ID(void)
{
  uint16_t ic_id = 0;
  LCD_WR_REG(0XD4);
  (void)LCD_RD_DATA();    //dummy read
  (void)LCD_RD_DATA();    //读回0X01
  ic_id = LCD_RD_DATA();  //读回0X53
  ic_id <<= 8;
  ic_id |= LCD_RD_DATA(); //这里读回0X10
  return ic_id;
}

void NT35310_Fsmc_ReInit(void)
{
  //寄存器清零
  //bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。
  //这里我们使用NE1 ，也就对应BTCR[0],[1]。     
  hsram1.Instance->BTCR[0] = 0X00000000;
  hsram1.Instance->BTCR[1] = 0X00000000;
  hsram1.Extended->BWTR[0] = 0X00000000;
  //操作BCR寄存器 使用异步模式
  hsram1.Instance->BTCR[0] |= 1 << 12; //存储器写使能
  hsram1.Instance->BTCR[0] |= 1 << 14; //读写使用不同的时序
  hsram1.Instance->BTCR[0] |= 1 << 4;
  //存储器数据宽度为16bit      
  //操作BTR寄存器
  //读时序控制寄存器      
  hsram1.Instance->BTCR[1] |= 0 << 28; //模式A        
  hsram1.Instance->BTCR[1] |= 5 << 0;
  //地址建立时间（ADDSET）为5个HCLK 1/168M=6ns*5=30ns    
  //因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
  hsram1.Instance->BTCR[1] |= 0XF << 8;
  //数据保存时间为16个HCLK    
  //写时序控制寄存器  
  hsram1.Extended->BWTR[0] |= 0 << 28;
  //模式A        
  hsram1.Extended->BWTR[0] |= 5 << 0; //地址建立时间（ADDSET）为5个HCLK =30ns
  //4个HCLK（HCLK=72M）因为液晶驱动IC的写信号脉宽，最少也得50ns。72M/4=24M=55ns    
  hsram1.Extended->BWTR[0] |= 8 << 8;
  //数据保存时间为9个HCLK=54ns
  //使能BANK1,区域4
  hsram1.Instance->BTCR[0] |= 1 << 0; //使能BANK1，区域1
  //  //重新配置写时序控制寄存器的时序
  //  hsram1.Extended->BWTR[6]&=~(0XF<<0); //地址建立时间（ADDSET）清零
  //  hsram1.Extended->BWTR[6]&=~(0XF<<8); //数据保存时间清零
  //  hsram1.Extended->BWTR[6]|=3<<0;      //地址建立时间（ADDSET）为3个HCLK =18ns
  //  hsram1.Extended->BWTR[6]|=2<<8;      //数据保存时间为6ns*3个HCLK=18ns
}

#endif

