#include "user_common.h"

#ifdef ENABLE_TOUCH


//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//触摸屏驱动（支持ADS7843/7846/UH7843/7846/XPT2046/TSC2046/OTT2001A等） 代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20140721
//修正MDK在-O2优化时,触摸屏数据无法读取的bug.在TP_Write_Byte函数添加一个延时,解决问题.
//////////////////////////////////////////////////////////////////////////////////



extern void display_picture(int PictureName);

static void tp_pin_init(void)
{
  if (GT9147_Init() == 1)
  {
    tp_dev.scan = GT9147_Scan;
    ccm_param.t_sys.is_touch_capacitive = true;
  }
  else if (FT5206_Init() == 1)
  {
    ccm_param.t_sys.is_touch_capacitive = true;
    tp_dev.scan = FT5206_Scan;  //扫描函数指向GT9147触摸屏扫描
    tp_dev.touchtype |= 0X80;   //电容屏
    tp_dev.touchtype |= lcddev.dir & 0X01; //横屏还是竖屏
  }
  else
  {
    ccm_param.t_sys.is_touch_capacitive = false;
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    /*
    PH7     ------> T_PEN
    PG3     ------> T_MISO
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    /*
    PI8     ------> T_CS
    PI3     ------> T_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
    /*
    PH6     ------> T_SCK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  }
}

_m_tp_dev tp_dev =
{
  TP_Init,
  TP_Scan,
  TP_Adjust,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};
//默认为touchtype=0的数据.
u8 CMD_RDX = 0X90;
u8 CMD_RDY = 0XD0;

//SPI写数据
//向触摸屏IC写入1byte数据
//num:要写入的数据
void TP_Write_Byte(u8 num)
{
  u8 count;

  for (count = 0; count < 8; count++)
  {
    if (num & 0x80)
    {
      user_pin_tp_mosi_ctrl(true);
    }
    else
    {
      user_pin_tp_mosi_ctrl(false);
    }

    num <<= 1;
    user_pin_tp_sck_ctrl(false);
    user_delay_us(1);
    user_pin_tp_sck_ctrl(true);  //上升沿有效
  }
}
//SPI读数据
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据
u16 TP_Read_AD(u8 CMD)
{
  u8 count = 0;
  u16 Num = 0;
  user_pin_tp_sck_ctrl(false);    //先拉低时钟
  user_pin_tp_mosi_ctrl(false);   //拉低数据线
  user_pin_tp_cs_ctrl(false);     //选中触摸屏IC
  TP_Write_Byte(CMD);//发送命令字
  user_delay_us(6);//ADS7846的转换时间最长为6us
  user_pin_tp_sck_ctrl(false);
  user_delay_us(1);
  user_pin_tp_sck_ctrl(true);    //给1个时钟，清除BUSY
  user_delay_us(1);
  user_pin_tp_sck_ctrl(false);

  for (count = 0; count < 16; count++) //读出16位数据,只有高12位有效
  {
    Num <<= 1;
    user_pin_tp_sck_ctrl(false);  //下降沿有效
    user_delay_us(1);
    user_pin_tp_sck_ctrl(true);

    if (user_pin_tp_miso_read())Num++;
  }

  Num >>= 4;  //只有高12位有效.
  user_pin_tp_cs_ctrl(true);   //释放片选
  return (Num);
}
//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5  //读取次数
#define LOST_VAL 1      //丢弃值
u16 TP_Read_XOY(u8 xy)
{
  u16 i, j;
  u16 buf[READ_TIMES];
  u16 sum;
  u16 temp;

  for (i = 0; i < READ_TIMES; i++)buf[i] = TP_Read_AD(xy);

  for (i = 0; i < (READ_TIMES - 1); i++) //排序
  {
    for (j = i + 1; j < READ_TIMES; j++)
    {
      if (buf[i] > buf[j]) //升序排列
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  sum = 0;

  for (i = LOST_VAL; i < (READ_TIMES - LOST_VAL); ++i)sum += buf[i];

  temp = sum / (READ_TIMES - (2 * LOST_VAL));
  return temp;
}
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
u8 TP_Read_XY(u16 *x, u16 *y)
{
  u16 xtemp, ytemp;
  xtemp = TP_Read_XOY(CMD_RDX);
  ytemp = TP_Read_XOY(CMD_RDY);
  //if(xtemp<100||ytemp<100)return 0;//读数失败
  *x = xtemp;
  *y = ytemp;
  return 1;//读数成功
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围
u8 TP_Read_XY2(u16 *x, u16 *y)
{
  u16 x1, y1;
  u16 x2, y2;
  u8 flag;
  flag = TP_Read_XY(&x1, &y1);

  if (flag == 0)
  {
    return (0);
  }

  flag = TP_Read_XY(&x2, &y2);

  if (flag == 0)
  {
    return (0);
  }

  if ((((x2 <= x1) && (x1 < (x2 + ERR_RANGE))) || ((x1 <= x2) && (x2 < (x1 + ERR_RANGE)))) //前后两次采样在+-50内
      && (((y2 <= y1) && (y1 < (y2 + ERR_RANGE))) || ((y1 <= y2) && (y2 < (y1 + ERR_RANGE)))))
  {
    *x = (x1 + x2) / 2;
    *y = (y1 + y2) / 2;
    return 1;
  }
  else
  {
    return 0;
  }
}
//////////////////////////////////////////////////////////////////////////////////
//与LCD部分有关的函数
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(u16 x, u16 y, u16 color)
{
  POINT_COLOR = color;
  LCD_DrawLine(x - 12, y, x + 13, y); //横线
  LCD_DrawLine(x, y - 12, x, y + 13); //竖线
  LCD_DrawPoint(x + 1, y + 1);
  LCD_DrawPoint(x - 1, y + 1);
  LCD_DrawPoint(x + 1, y - 1);
  LCD_DrawPoint(x - 1, y - 1);
  LCD_Draw_Circle(x, y, 6); //画中心圈
}
//画一个大点(2*2的点)
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(u16 x, u16 y, u16 color)
{
  POINT_COLOR = color;
  LCD_DrawPoint(x, y); //中心点
  LCD_DrawPoint(x + 1, y);
  LCD_DrawPoint(x, y + 1);
  LCD_DrawPoint(x + 1, y + 1);
}
//////////////////////////////////////////////////////////////////////////////////

#ifdef LCD_DEBUG //卢工调试LCD要求
  extern uint32_t Lcd_Count_touch;
#endif
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 TP_Scan(u8 tp)
{
  taskENTER_CRITICAL();

  if (user_pin_tp_pen_read()) //有按键按下
  {
    if (tp)
    {
      (void)TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]); //读取物理坐标
    }

    if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0])) //读取屏幕坐标
    {
      tp_dev.x[1] = (unsigned short)((tp_dev.xfac * tp_dev.x[0]) + tp_dev.xoff); //将结果转换为屏幕坐标
      tp_dev.y[1] = (unsigned short)((tp_dev.yfac * tp_dev.y[0]) + tp_dev.yoff);
    }

    if ((tp_dev.sta & TP_PRES_DOWN) == 0) //之前没有被按下
    {
      tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES; //按键按下

      if (ccm_param.t_sys.is_touch_capacitive && (lcddev.id == 0X7018 || lcddev.id == 0X7016))
      {
        tp_dev.x[4] = tp_dev.x[0]; //记录第一次按下时的坐标
        tp_dev.y[4] = tp_dev.y[0];
      }
    }

#ifdef LCD_DEBUG
    //    if(Lcd_Count_touch[0]>=65535) Lcd_Count_touch[1]+=1;
    Lcd_Count_touch++;
#endif
  }
  else
  {
    if (tp_dev.sta & TP_PRES_DOWN) //之前是被按下的
    {
      tp_dev.sta &= ~(1 << 7); //标记按键松开
    }
    else //之前就没有被按下
    {
      tp_dev.x[4] = 0;
      tp_dev.y[4] = 0;

      if (ccm_param.t_sys.is_touch_capacitive && (lcddev.id == 0X7018 || lcddev.id == 0X7016))
      {
        tp_dev.x[0] = 0xffff;
        tp_dev.y[0] = 0xffff;
      }
    }
  }

  taskEXIT_CRITICAL();
  return tp_dev.sta & TP_PRES_DOWN; //返回当前的触屏状态
}
//////////////////////////////////////////////////////////////////////////
//保存校准参数
void TP_Save_Adjdata(void)
{
  float iyfac, ixfac;
  u16 ioa = 0X0A;
  u32 buffer[4];
  touch_data_read(buffer, 4);

  /////////////////////////////////////////修改负数
  if (tp_dev.xfac < 0)
  {
    ixfac = -tp_dev.xfac;
    ioa |= (1 << 8);      //x轴负数标志
  }
  else
  {
    ixfac = tp_dev.xfac;
    ioa &= (~(1 << 8));   //
  }

  buffer[0] = (u32)(ixfac * 100000000);

  /////////////////////
  if (tp_dev.yfac < 0)
  {
    iyfac = -tp_dev.yfac;
    ioa |= (1 << 9);      //y轴负数标志
  }
  else
  {
    iyfac = tp_dev.yfac;
    ioa &= (~(1 << 9));   //
  }

  buffer[1] = (u32)(iyfac * 100000000);
  //////////////////////////////////////////////////////////////////
  buffer[2] = (int)(tp_dev.xoff & 0xffff) | ((int)tp_dev.yoff << 16);
  buffer[3] = (int)tp_dev.touchtype | ((int)ioa << 16);
  touch_data_write(buffer, 4);
}
//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
u8 TP_Get_Adjdata(void)
{
  int tempfac;
  u32 buffer[4];
  touch_data_read(buffer, 4);
  tempfac = buffer[3] >> 16;

  if ((tempfac & 0xff) == 0X0A) //触摸屏已经校准过了
  {
    ///////////////////////////////////////////////////////修改负数
    if (tempfac & (1 << 8))
    {
      tp_dev.xfac = -(float)buffer[0] / 100000000; //得到x校准参数
    }
    else
    {
      tp_dev.xfac = (float)buffer[0] / 100000000; //得到x校准参数
    }

    //////////////////////////////////////////
    if (tempfac & (1 << 9))
    {
      tp_dev.yfac = -(float)buffer[1] / 100000000; //得到y校准参数
    }
    else
    {
      tp_dev.yfac = (float)buffer[1] / 100000000; //得到y校准参数
    }

    ///////////////////////////////////////////////////////////////////
    tp_dev.xoff = (short)(buffer[2] & 0xffff);
    tp_dev.yoff = (short)(buffer[2] >> 16);

    if (ccm_param.t_sys.is_touch_capacitive) // 电容屏返回
      return 1;

    tp_dev.touchtype = (u8)(buffer[3] & 0xff);

    if (tp_dev.touchtype) //X,Y方向与屏幕相反
    {
      CMD_RDX = 0XD0;
      CMD_RDY = 0X90;

      if ((lcddev.id != 0X4342) && (lcddev.id != 0X7016) && (lcddev.id != 0X7018))
      {
        tp_dev.touchtype = 0;
        CMD_RDX = 0X90;
        CMD_RDY = 0XD0;
        return 0;
      }
    }
    else            //X,Y方向与屏幕相同
    {
      CMD_RDX = 0X90;
      CMD_RDY = 0XD0;

      if ((lcddev.id == 0X4342) ||
          (F1000TP == ccm_param.t_sys_data_current.model_id) ||
          (F400TP == ccm_param.t_sys_data_current.model_id) ||
          (K600 == ccm_param.t_sys_data_current.model_id) ||
          (F300TP == ccm_param.t_sys_data_current.model_id))
      {
        tp_dev.touchtype = 1;
        CMD_RDX = 0XD0;
        CMD_RDY = 0X90;
        return 0;
      }
    }

    return 1;
  }

  return 0;
}

//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{
  u16 pos_temp[4][2];//坐标缓存值
  u8  cnt;
  u16 d1, d2;
  //u32 tem1,tem2;
  int tem1, tem2;
  float fac;
  u16 outtime = 0;
  cnt = 0;
  POINT_COLOR = BLUE;
  BACK_COLOR = WHITE;
  POINT_COLOR = RED; //红色
  POINT_COLOR = BLACK;
  //////////////////////////////////////////////////////////////////////////////
  display_picture(15);
  user_delay_ms(1000);
  TP_Drow_Touch_Point(50, 50, RED); //画点1
  tp_dev.sta = 0; //消除触发信号
  tp_dev.xfac = 0; //xfac用来标记是否校准过,所以校准之前必须清掉!以免错误

  while (1) //如果连续10分钟没有按下,则自动退出
  {
    (void)tp_dev.scan(1);//扫描物理坐标

    if ((tp_dev.sta & 0xc0) == TP_CATH_PRES) //按键按下了一次(此时按键松开了.)
    {
      outtime = 0;
      tp_dev.sta &= ~(1 << 6); //标记按键已经被处理过了.
      //////////////////////////////////////////////////LCD_Buzz
      ////////////////////////////////////////////////////

      if (tp_dev.x[0] != 0 && tp_dev.x[0] != 4095) //触摸数据异常，重新获取
      {
        user_buzzer_buzz(100);
        pos_temp[cnt][0] = tp_dev.x[0];
        pos_temp[cnt][1] = tp_dev.y[0];
        cnt++;
      }

      switch (cnt)
      {
      case 1:
        TP_Drow_Touch_Point(50, 50, 0x1992);      //清除点1
        TP_Drow_Touch_Point(lcddev.width - 50, 50, RED); //画点2
        break;

      case 2:
        TP_Drow_Touch_Point(lcddev.width - 50, 50, 0x1992); //清除点2
        TP_Drow_Touch_Point(50, lcddev.height - 50, RED); //画点3
        break;

      case 3:
        TP_Drow_Touch_Point(50, lcddev.height - 50, 0x1992);  //清除点3
        TP_Drow_Touch_Point(lcddev.width - 50, lcddev.height - 50, RED); //画点4
        break;

      case 4:  //全部四个点已经得到
        //对边相等
        tem1 = abs(pos_temp[0][0] - pos_temp[1][0]); //x1-x2
        tem2 = abs(pos_temp[0][1] - pos_temp[1][1]); //y1-y2
        tem1 *= tem1;
        tem2 *= tem2;
        d1 = (unsigned short)sqrtf((float)(tem1 + tem2)); //得到1,2的距离
        tem1 = abs(pos_temp[2][0] - pos_temp[3][0]); //x3-x4
        tem2 = abs(pos_temp[2][1] - pos_temp[3][1]); //y3-y4
        tem1 *= tem1;
        tem2 *= tem2;
        d2 = (unsigned short)sqrtf((float)(tem1 + tem2)); //得到3,4的距离
        fac = (float)d1 / d2;

        //if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
        if ((fac < 0.85F) || (fac > 1.15F) || (d1 == 0) || (d2 == 0)) //不合格
        {
          cnt = 0;
          TP_Drow_Touch_Point(lcddev.width - 50, lcddev.height - 50, 0x1992); //清除点4
          ///////////////////////////////////////////////////////////////////////////////////////////
          display_picture(16);
          user_delay_ms(250);
          display_picture(15);
          TP_Drow_Touch_Point(50, 50, RED); //画点1
          continue;
        }

        tem1 = abs(pos_temp[0][0] - pos_temp[2][0]); //x1-x3
        tem2 = abs(pos_temp[0][1] - pos_temp[2][1]); //y1-y3
        tem1 *= tem1;
        tem2 *= tem2;
        d1 = (unsigned short)sqrtf((float)(tem1 + tem2)); //得到1,3的距离
        tem1 = abs(pos_temp[1][0] - pos_temp[3][0]); //x2-x4
        tem2 = abs(pos_temp[1][1] - pos_temp[3][1]); //y2-y4
        tem1 *= tem1;
        tem2 *= tem2;
        d2 = (unsigned short)sqrtf((float)(tem1 + tem2)); //得到2,4的距离
        fac = (float)d1 / d2;

        //if(fac<0.95||fac>1.05)//不合格
        if ((fac < 0.85F) || (fac > 1.15F)) //不合格
        {
          cnt = 0;
          TP_Drow_Touch_Point(lcddev.width - 50, lcddev.height - 50, 0x1992); //清除点4
          //////////////////////////////////////////////////////////////////////////
          display_picture(16);
          user_delay_ms(1000);
          display_picture(15);
          TP_Drow_Touch_Point(50, 50, RED); //画点1
          continue;
        }//正确了

        //对角线相等
        tem1 = abs(pos_temp[1][0] - pos_temp[2][0]); //x1-x3
        tem2 = abs(pos_temp[1][1] - pos_temp[2][1]); //y1-y3
        tem1 *= tem1;
        tem2 *= tem2;
        d1 = (unsigned short)sqrtf((float)(tem1 + tem2)); //得到1,4的距离
        tem1 = abs(pos_temp[0][0] - pos_temp[3][0]); //x2-x4
        tem2 = abs(pos_temp[0][1] - pos_temp[3][1]); //y2-y4
        tem1 *= tem1;
        tem2 *= tem2;
        d2 = (unsigned short)sqrtf((float)(tem1 + tem2)); //得到2,3的距离
        fac = (float)d1 / d2;

        //if(fac<0.95||fac>1.05)//不合格
        if ((fac < 0.85F) || (fac > 1.15F)) //不合格
        {
          cnt = 0;
          TP_Drow_Touch_Point(lcddev.width - 50, lcddev.height - 50, 0x1992); //清除点4
          ////////////////////////////////////////////////////////////////////////////
          display_picture(16);
          user_delay_ms(1000);
          display_picture(15);
          TP_Drow_Touch_Point(50, 50, RED); //画点1
          continue;
        }//正确了

        //计算结果
        tp_dev.xfac = (float)(lcddev.width - 100) / (pos_temp[1][0] - pos_temp[0][0]); //得到xfac
        tp_dev.xoff = (short)(lcddev.width - (tp_dev.xfac * (pos_temp[1][0] + pos_temp[0][0]))) / 2; //得到xoff
        tp_dev.yfac = (float)(lcddev.height - 100) / (pos_temp[2][1] - pos_temp[0][1]); //得到yfac
        tp_dev.yoff = (short)(lcddev.height - (tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1]))) / 2; //得到yoff

        if ((abs((int)tp_dev.xfac) > 2) || (abs((int)tp_dev.yfac) > 2)) //触屏和预设的相反了.
        {
          cnt = 0;
          TP_Drow_Touch_Point(lcddev.width - 50, lcddev.height - 50, 0x1992); //清除点4
          display_picture(16);
          user_delay_ms(1000);
          display_picture(15);
          TP_Drow_Touch_Point(50, 50, RED); //画点1
          continue;
        }

        POINT_COLOR = BLUE;
        ////////////////////////////////////////////////////////////////////////
        display_picture(17);
        user_delay_ms(1000);
        TP_Save_Adjdata();
        return;//校正完成

      default:
        break;
      }
    }

    user_delay_ms(10);
    outtime++;

    if (outtime > 60000) //十分钟后自动退出
    {
      (void)TP_Get_Adjdata();
      break;
    }
  }
}
//触摸屏初始化
//返回值:0,没有进行校准
//       1,进行过校准
u8 TP_Init(void)
{
  tp_pin_init();

  if (ccm_param.t_sys.is_touch_capacitive)
    return 1;

  (void)TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]); //第一次读取初始化

  if (TP_Get_Adjdata())
  {
    return 0;//已经校准
  }
  else         //未校准?
  {
    user_pin_lcd_backlight_ctrl(true);
    //LCD_LED=1; //点亮背光
    TP_Adjust();    //屏幕校准
    TP_Save_Adjdata();
  }

  (void)TP_Get_Adjdata();
  return 1;
}

#endif



