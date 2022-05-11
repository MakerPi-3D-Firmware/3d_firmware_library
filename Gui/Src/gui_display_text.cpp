#include "user_common_cpp.h"

#ifndef ENABLE_GUI_LVGL

#ifdef __cplusplus
extern "C" {
#endif

#define TEXT_RANGE_4_BUF(INDEX) sdram_param.TextRangeBuf_24_12_4[INDEX]
#define TEXT_RANGE_9_BUF(INDEX) sdram_param.TextRangeBuf_24_12_9[INDEX]
#define DEFAULT_TEXT_SIZE 24

struct _textrange
{
  int x;
  int y;
  int rangex;
  int rangey;
};

static void gui_disp_text_buf9_set_range(int index, int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    gui::set_text_range_9_buf(index, x_35_s, y_35_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    gui::set_text_range_9_buf(index, x_43_s, y_43_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    gui::set_text_range_9_buf(index, x_70_s, y_70_s);
  }
}

void gui_disp_text_noz0_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  gui_disp_text_buf9_set_range(TEXT_BUF_9_INDEX_PRINT_NOZ_TEMP, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s);
}

void gui_disp_text_noz0_temp_show(void)
{
  gui::display_text_range_9_buf(TEXT_BUF_9_INDEX_PRINT_NOZ_TEMP, (char *)"%3d/%3d",
                                (int)ccm_param.t_gui.nozzle_temp[0],
                                (int)ccm_param.t_gui.target_nozzle_temp[0]);
}

void gui_disp_text_noz1_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  gui_disp_text_buf9_set_range(TEXT_BUF_9_INDEX_PRINT_NOZ1_TEMP, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s);
}

void gui_disp_text_noz1_temp_show(void)
{
  gui::display_text_range_9_buf(TEXT_BUF_9_INDEX_PRINT_NOZ1_TEMP, (char *)"%3d/%3d",
                                (int)ccm_param.t_gui.nozzle_temp[1],
                                (int)ccm_param.t_gui.target_nozzle_temp[1]);
}

void gui_disp_text_bed_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  gui_disp_text_buf9_set_range(TEXT_BUF_9_INDEX_PRINT_BED_TEMP, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s);
}

void gui_disp_text_bed_temp_show(void)
{
  gui::display_text_range_9_buf(TEXT_BUF_9_INDEX_PRINT_BED_TEMP, (char *)"%3d/%3d",
                                (int)ccm_param.t_gui.hot_bed_temp,
                                (int)ccm_param.t_gui.target_hot_bed_temp);
}

void gui_disp_text_time_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  gui_disp_text_buf9_set_range(TEXT_BUF_9_INDEX_PRINT_TIME, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s);
}

void gui_disp_text_time_show(int time_sec)
{
  int second, minute, hour;
  second = time_sec;
  hour = second / 3600;
  minute = (second - hour * 3600) / 60;
  second = (second - hour * 3600) % 60;
  gui::display_text_range_9_buf(TEXT_BUF_9_INDEX_PRINT_TIME, (char *)"%3d:%2d:%2d", hour, minute, second);
}

void gui_disp_text_print_progress_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_PROGRESS, x_35_s, y_35_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_PROGRESS, x_43_s, y_43_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_PROGRESS, x_70_s, y_70_s);
  }
}

void gui_disp_text_noz0_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_NOZ_TEMP, x_35_s, y_35_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_NOZ_TEMP, x_43_s, y_43_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_NOZ_TEMP, x_70_s, y_70_s);
  }
}

void gui_disp_text_speed_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_SPEED, x_35_s, y_35_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_SPEED, x_43_s, y_43_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_SPEED, x_70_s, y_70_s);
  }
}

void gui_disp_text_z_height_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_Z, x_35_s, y_35_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_Z, x_43_s, y_43_s);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    gui::set_text_range_4_buf(TEXT_BUF_4_INDEX_Z, x_70_s, y_70_s);
  }
}


void gui_disp_text_print_progress_show(void)
{
  gui::display_text_range_4_buf(TEXT_BUF_4_INDEX_PRINT_PROGRESS, (char *)"%2d%%", (int)ccm_param.t_gui.print_percent);
}

void gui_disp_text_cavity_temp_set_range(int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  gui_disp_text_buf9_set_range(TEXT_BUF_9_INDEX_PRINT_CAVITY_TEMP, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s);
}

void gui_disp_text_cavity_temp_show(void)
{
  gui::display_text_range_9_buf(TEXT_BUF_9_INDEX_PRINT_CAVITY_TEMP, (char *)"%3d/%3d",
                                (int)ccm_param.t_gui.cavity_temp,
                                (int)ccm_param.t_gui.target_cavity_temp);
}

void gui_disp_text_buf_show_m(char *text_buf, int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s, bool is_middle)
{
  int x_tmp;

  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    x_tmp = x_35_s;

    if (is_middle)
    {
      x_tmp = (480 - (strlen(text_buf) - 1) * 12) / 2;
    }

    gui::display_text_buf(x_tmp, y_35_s, (char *)text_buf);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    x_tmp = x_43_s;

    if (is_middle)
    {
      x_tmp = (480 - (strlen(text_buf) - 1) * 12) / 2;
    }

    gui::display_text_buf(x_tmp, y_43_s, (char *)text_buf);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    x_tmp = x_70_s;

    if (is_middle)
    {
      x_tmp = (1024 - (strlen(text_buf) - 1) * 12) / 2;
    }

    gui::display_text_buf(x_tmp, y_70_s, (char *)text_buf);
  }
}

void gui_disp_text_buf_show(char *text_buf, int x_35_s, int y_35_s, int x_43_s, int y_43_s, int x_70_s, int y_70_s)
{
  gui_disp_text_buf_show_m(text_buf, x_35_s, y_35_s, x_43_s, y_43_s, x_70_s, y_70_s, false);
}

void gui_disp_set_text_buf_color(uint16_t chg_text_color)
{
  gui::set_disp_text_color(chg_text_color);
}

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gui
{
  struct _textrange  text_range_4[4];
  struct _textrange  text_range_9[6];
  volatile uint16_t text_color = RED;
  FIL hanziziku;

  //复制一份读出的像素数据
  static void copy_text_disp_range_info(struct _textrange textrange, const u16 *psource, u16 *p)
  {
    int i, ii;

    for (i = 0; i < textrange.rangey; i++)
    {
      for (ii = 0; ii < textrange.rangex; ii++)
      {
        *p++ = *psource++;
      }
    }
  }

  //将指定区域内的像素读出
  static  void read_text_disp_range_info(struct _textrange textrange, u16 *p)
  {
    int i, ii;

    for (i = 0; i < textrange.rangey; i++)
    {
      for (ii = 0; ii < textrange.rangex; ii++)
      {
        *p++ = LCD_ReadPoint((u16)(ii + textrange.x), (u16)(i + textrange.y));
      }
    }
  }

  //将合成了文字信息的像素数据重新写入指定区域
  static void write_text_disp_range_info(struct _textrange textrange, const u16 *p)
  {
    int i, ii;

    for (i = 0; i < textrange.rangey; i++)
    {
      for (ii = 0; ii < textrange.rangex; ii++)
      {
        LCD_Fast_DrawPoint((u16)(textrange.x + ii), (u16)(textrange.y + i), *p++);
      }
    }
  }


  //code 字符指针开始
  //从字库中查找出字模
  //code 字符串的开始地址,GBK码
  //mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小
  //size:字体大小

  static void get_cn_mat(const unsigned char *code, unsigned char *mat, u8 size)
  {
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    unsigned int readbyte;
    u8 csize = (u8)((size / 8 + ((size % 8) ? 1 : 0)) * (size)); //得到字体一个字符对应点阵集所占的字节数
    qh = *code;
    ql = *(++code);

    if (qh < 0x81 || ql < 0x40 || ql == 0xff || qh == 0xff) //非 常用汉字
    {
      for (i = 0; i < csize; i++)*mat++ = 0x00; //填充满格

      return; //结束访问
    }

    if (ql < 0x7f)ql -= 0x40; //注意!
    else ql -= 0x41;

    qh -= 0x81;
    foffset = ((unsigned long)190 * qh + ql) * csize; //得到字库中的字节偏移量

    switch (size)
    {
    case 12:
      break;

    case 16:
      break;

    case 24:
      taskENTER_CRITICAL();
      (void)f_open(&hanziziku, "1:/GBK24.FON", FA_READ);  //打開字庫文件
      (void)f_lseek(&hanziziku, foffset);
      (void)f_read(&hanziziku, mat, csize, &readbyte);
      (void)f_close(&hanziziku);
      taskEXIT_CRITICAL();
      break;

    default:
      break;
    }
  }

  //将单个英文字符点阵信息合成到读出的像素数据中
  static void write_en_char_to_buf(struct _textrange textrange, const unsigned char *pstr, int XHeadPos, u16 *p, int size, u16 colour)
  {
    //参数*pstr字符，参数testrange是显示区域，参数p是数组，参数size是字体大小，colour是字体颜色
    int CharCodeNum, BitData, YPos = 0, XPos = -1;
    unsigned char CharCode;
    unsigned char CharPos = *pstr - ' ';  //偏移量

    if (size == 24) //24*12的字体
    {
      for (CharCodeNum = 0; CharCodeNum < 36; CharCodeNum++) //一个24*12字符对应36个字节数据
      {
        CharCode = asc2_2412[CharPos][CharCodeNum]; //24*12字符码  //从上到下，从左到右，高位在前

        if ((CharCodeNum % 3) == 0) //3个字节为1列
        {
          ++XPos; //下一列
          YPos = 0; //行归零
        }

        for (BitData = 0; BitData < 8; BitData++) //一个字节有8位，每一位都对应一个像素
        {
          if (CharCode & 0x80) //有效像素点写颜色数据
          {
            p[XHeadPos + XPos + YPos * textrange.rangex] = colour; //一个像素是16位的颜色数据
          }

          CharCode <<= 1; //下一个像素
          YPos++; //下一行
        }
      }
    }
  }

  //将单个中文字符点阵信息合成到读出的像素数据中
  static void write_cn_char_to_buf(struct _textrange textrange, const unsigned char *pstr, int XHeadPos, u16 *p, int size, u16 colour)
  {
    //参数*pstr字符，参数testrange是显示区域，参数p是数组，参数size是字体大小，colour是字体颜色
    int CharCodeNum, BitData, YPos = 0, XPos = -1;
    unsigned char CharCode;
    unsigned char dzk[72]; //一个汉字的点阵数据72字节Buf

    if (size == 24)
    {
      get_cn_mat(pstr, dzk, (u8)size); //得到72字节的点阵数据

      for (CharCodeNum = 0; CharCodeNum < 72; CharCodeNum++) //72字节
      {
        CharCode = dzk[CharCodeNum];  //24*24字符码

        if ((CharCodeNum % 3) == 0) //3个字节为一列
        {
          ++XPos; //下一列
          YPos = 0; //行归零
        }

        for (BitData = 0; BitData < 8; BitData++) //一个字节有8位，每一位都对应一个像素
        {
          if (CharCode & 0x80) //有效像素点写颜色数据
          {
            p[XHeadPos + XPos + YPos * textrange.rangex] = colour; //一个像素是16位的颜色数据
          }

          CharCode <<= 1; //下一个像素
          YPos++; //下一行
        }
      }
    }
  }

  //显示英文字符
  static void disp_en_char(const unsigned char *pstr, int XHeadPos, int YHeadPos, int size, u16 colour)
  {
    //参数*pstr是字符，参数XHeadPos、YHeadPos是显示的起始位置，参数size是字体大小，colour是字体颜色
    int CharCodeNum, BitData, YPos = 0, XPos = -1;
    unsigned char CharCode;
    unsigned char CharPos = *pstr - ' ';  //偏移量

    if (size == 24) //24*12字体
    {
      for (CharCodeNum = 0; CharCodeNum < 36; CharCodeNum++) //36个字节
      {
        CharCode = asc2_2412[CharPos][CharCodeNum]; //24*12字符码

        if ((CharCodeNum % 3) == 0) //3个字节为一列
        {
          ++XPos; //下一列
          YPos = 0; //行归零
        }

        for (BitData = 0; BitData < 8; BitData++) //一个字节有8位，每一位都对应一个像素
        {
          if (CharCode & 0x80) //有效像素点写颜色数据
          {
            LCD_Fast_DrawPoint((u16)(XHeadPos + XPos), (u16)(YHeadPos + YPos), colour); //一个像素是16位的颜色数据
          }

          CharCode <<= 1; //下一个像素
          YPos++; //下一行
        }
      }
    }
  }

  //显示中文字符
  static void disp_cn_char(const unsigned char *pstr, int XHeadPos, int YHeadPos, int size, u16 colour)
  {
    //参数*pstr是中文字符，参数XHeadPos、YHeadPos是显示的起始位置，参数size是字体大小，colour是字体颜色
    int CharCodeNum, BitData, YPos = 0, XPos = -1;
    unsigned char CharCode;
    unsigned char dzk[72]; //一个汉字的点阵数据72字节Buf

    if (size == 24)
    {
      get_cn_mat(pstr, dzk, (u8)size); //得到72字节的点阵数据

      for (CharCodeNum = 0; CharCodeNum < 72; CharCodeNum++) //72字节
      {
        CharCode = dzk[CharCodeNum];  //24*24字符码

        if ((CharCodeNum % 3) == 0) //3个字节为一列
        {
          ++XPos; //下一列
          YPos = 0; //行归零
        }

        for (BitData = 0; BitData < 8; BitData++) //一个字节有8位，每一位都对应一个像素
        {
          if (CharCode & 0x80) //有效像素点写颜色数据
          {
            LCD_Fast_DrawPoint((u16)(XHeadPos + XPos), (u16)(YHeadPos + YPos), colour); //一个像素是16位的颜色数据
          }

          CharCode <<= 1; //下一个像素
          YPos++; //下一行
        }
      }
    }
  }


  //在指定的区域写文字
  static void disp_text_in_range(unsigned char *pstr, struct _textrange textrange, u16 *p, int size, u16 colour)
  {
    //参数pstr字符串，参数testrange是显示区域，参数p是数组，参数size是字体大小，colour是字体颜色
    int XPos = 0;

    while (*pstr != 0)
    {
      if (*pstr <= 126) // 英语
      {
        if (XPos <= (textrange.rangex - 12)) //防止溢出  //一个英文字符的宽度为12
        {
          write_en_char_to_buf(textrange, pstr, XPos, p, size, colour); //将英文字符写入数组中
          XPos = XPos + 12; //一个英文字符的宽度为12
          pstr = pstr + 1; //一个字节
        }
      }
      else //汉字
      {
        if (XPos <= (textrange.rangex - 24)) //防止溢出  //一个中文字符的宽度为24
        {
          write_cn_char_to_buf(textrange, pstr, XPos, p, size, colour); //将中文字符写入数组中
          XPos = XPos + 24; //一个中文字符的宽度为24
          pstr = pstr + 2; //两个字节
        }
      }
    }

    write_text_disp_range_info(textrange, p); //将数组中的数据写到lcd中
  }

  //设置文字显示的区域范围
  static void set_text_disp_range(int x, int y, int rangex, int rangey, struct _textrange *p)
  {
    p->x = x;
    p->y = y;
    p->rangex = rangex;
    p->rangey = rangey;
  }

  void set_text_range_4_buf(int index, int x, int y)
  {
    set_text_disp_range(x, y, 12 * 4, DEFAULT_TEXT_SIZE, &text_range_4[index]); //设置显示区域
    read_text_disp_range_info(text_range_4[index], TEXT_RANGE_4_BUF(index)); //从lcd读取像素，保存到数组
  }

  void display_text_range_4_buf(int index, char *fmt, ...)
  {
    char text_buffer[20];
    va_list args;
    va_start(args, fmt);
    _vsnprintf(text_buffer, sizeof(text_buffer), fmt, args);
    va_end(args);
    copy_text_disp_range_info(text_range_4[index], TEXT_RANGE_4_BUF(index), sdram_param.text_range_buf_24_12_9);
    disp_text_in_range((unsigned char *)text_buffer, text_range_4[index], sdram_param.text_range_buf_24_12_9, DEFAULT_TEXT_SIZE, (u16)testcolor);
  }

  void set_disp_text_color(uint16_t chg_text_color)
  {
    text_color = chg_text_color;
  }

  void display_text_buf(int x, int y, char *text)
  {
    gui::disp_text((uint8_t *)text, x, y, DEFAULT_TEXT_SIZE, (u16)text_color); //机型
  }

  void set_text_range_9_buf(int index, int x, int y)
  {
    set_text_disp_range(x, y, 12 * 9, DEFAULT_TEXT_SIZE, &text_range_9[index]); //设置显示区域
    read_text_disp_range_info(text_range_9[index], TEXT_RANGE_9_BUF(index)); //从lcd读取像素，保存到数组
  }

  void display_text_range_9_buf(int index, char *fmt, ...)
  {
    char text_buffer[20];
    va_list args;
    va_start(args, fmt);
    _vsnprintf(text_buffer, sizeof(text_buffer), fmt, args);
    va_end(args);
    copy_text_disp_range_info(text_range_9[index], TEXT_RANGE_9_BUF(index), sdram_param.text_range_buf_24_12_9);
    disp_text_in_range((unsigned char *)text_buffer, text_range_9[index], sdram_param.text_range_buf_24_12_9, DEFAULT_TEXT_SIZE, (u16)testcolor);
  }

  void disp_text(const unsigned char *pstr, int x, int y, int size, u16 colour)
  {
    //参数pstr字符串，参数xy显示区域，参数size字体大小
    while (*pstr != 0)
    {
      if (*pstr <= 126) //英语
      {
        gui::disp_en_char(pstr, x, y, size, colour); //英文字符
        x += size / 2;
        pstr++;
      }
      else // 汉字
      {
        gui::disp_cn_char(pstr, x, y, size, colour); //中文字符
        x += size;
        pstr += 2;
      }
    }
  }
}

#endif


