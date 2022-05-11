#include "user_common_cpp.h"
#ifdef __cplusplus
extern "C" {
#endif

#define LOGO_PIC_FILE          ((TCHAR *)_T("1:/L_Picture.bin"))

#define CN_PIC_35_FILE         ((TCHAR *)_T("1:/C_Picture.bin"))
#define CN_PIC_43_BLUE_FILE    ((TCHAR *)_T("1:/C_Picture_43.bin"))
#define CN_PIC_43_BLACK_FILE   ((TCHAR *)_T("1:/C_Picture_43_2.bin"))
#define CN_PIC_70_FILE         ((TCHAR *)_T("1:/C_Picture_70.bin"))
#define EN_PIC_35_FILE         ((TCHAR *)_T("1:/EN_Picture.bin"))
#define EN_PIC_43_BLUE_FILE    ((TCHAR *)_T("1:/EN_Picture_43.bin"))
#define EN_PIC_43_BLACK_FILE   ((TCHAR *)_T("1:/EN_Picture_43_2.bin"))
#define EN_PIC_70_FILE         ((TCHAR *)_T("1:/EN_Picture_70.bin"))
#define CNT_PIC_35_FILE        ((TCHAR *)_T("1:/C_T_Picture.bin"))
#define CNT_PIC_43_BLUE_FILE   ((TCHAR *)_T("1:/C_T_Picture_43.bin"))
#define CNT_PIC_43_BLACK_FILE  ((TCHAR *)_T("1:/C_T_Picture_43_2.bin"))
#define CNT_PIC_70_FILE        ((TCHAR *)_T("1:/C_T_Picture_70.bin"))
#define KOR_PIC_35_FILE        ((TCHAR *)_T("1:/KOR_Picture.bin"))
#define KOR_PIC_43_BLUE_FILE   ((TCHAR *)_T("1:/KOR_Picture_43.bin"))
#define KOR_PIC_43_BLACK_FILE  ((TCHAR *)_T("1:/KOR_Picture_43_2.bin"))
#define KOR_PIC_70_FILE        ((TCHAR *)_T("1:/KOR_Picture_70.bin"))
#define JP_PIC_35_FILE         ((TCHAR *)_T("1:/J_Picture.bin"))
#define JP_PIC_43_BLUE_FILE    ((TCHAR *)_T("1:/J_Picture_43.bin"))
#define JP_PIC_43_BLACK_FILE   ((TCHAR *)_T("1:/J_Picture_43_2.bin"))
#define JP_PIC_70_FILE         ((TCHAR *)_T("1:/J_Picture_70.bin"))
#define RUS_PIC_35_FILE        ((TCHAR *)_T("1:/RUS_Picture.bin"))
#define RUS_PIC_43_BLUE_FILE   ((TCHAR *)_T("1:/RUS_Picture_43.bin"))
#define RUS_PIC_43_BLACK_FILE  ((TCHAR *)_T("1:/RUS_Picture_43_2.bin"))
#define RUS_PIC_70_FILE        ((TCHAR *)_T("1:/RUS_Picture_70.bin"))

//画进度条
#define BarWidth 382  //进度条长度
#define BarHeight 21  //进度条宽度
#define X_BEGIN 75    //进度条x轴起始位置
#define Y_BEGIN 289   //进度条y轴起始位置

// 图片包文件头结构
typedef struct Pic_SMaseFileHeader
{
  uint32_t  uFileFlag;          // 包文件头标记: 'MASE'
  uint32_t  uFileCount;         // 包内文件个数
  uint32_t  uFileListOfs;       // 文件列表偏移
  uint32_t  uMaxFileCount;      // 最大子文件个数
  uint32_t  uFileSize;          // 包文件的大小
} Pic_MaseHeader;

//包内文件信息结构
typedef struct Pic_SFilesMessage
{
  uint32_t  uFileOfs;          // 本文件在包内的偏移
  uint32_t  uFileSize;         // 本文件的大小
  char  szFileName[60];   // 本文件的文件名，包含路径
} Pic_FilesMsg;
Pic_FilesMsg   Pic_fileMsg;      //文件信息结构

enum PIC_TYPE
{
  DISPLAY_PIC = 0, // 界面显示
  LOGO_PIC = 1,   // LOGO显示
};

static FIL PictureFile;

#define X_OFFSET 270      //x轴偏移位置

#define GUI_OPEN_FILE(LANGUAGE_STR) \
  { \
    if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE) \
    { \
      gui_open_res = f_open(&PictureFile, LANGUAGE_STR##_PIC_35_FILE, FA_READ); \
    } \
    else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE) \
    { \
      if (ccm_param.t_sys_data_current.ui_number == LCD_43_UI_TYPE_BLUE) \
      { \
        gui_open_res = f_open(&PictureFile, LANGUAGE_STR##_PIC_43_BLUE_FILE, FA_READ); \
      } \
      else if (ccm_param.t_sys_data_current.ui_number == LCD_43_UI_TYPE_BLACK) \
      { \
        gui_open_res = f_open(&PictureFile, LANGUAGE_STR##_PIC_43_BLACK_FILE, FA_READ); \
      } \
    } \
    else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE) \
    { \
      gui_open_res = f_open(&PictureFile, LANGUAGE_STR##_PIC_70_FILE, FA_READ); \
    } \
  }

static void _open_pic(FRESULT &gui_open_res, int picType, unsigned int &LCDWidth)
{
  if (DISPLAY_PIC == picType)
  {
    if (PICTURE_IS_CHINESE == ccm_param.t_sys_data_current.pic_id) //中文图片
    {
      GUI_OPEN_FILE(CN);
    }
    else if (PICTURE_IS_JAPANESE == ccm_param.t_sys_data_current.pic_id) //日文图片
    {
      GUI_OPEN_FILE(JP);
    }
    else if (PICTURE_IS_ENGLISH == ccm_param.t_sys_data_current.pic_id) //英文图片
    {
      GUI_OPEN_FILE(EN);
    }
    else if (PICTURE_IS_KOREA == ccm_param.t_sys_data_current.pic_id) //韩文图片
    {
      GUI_OPEN_FILE(KOR);
    }
    else if (PICTURE_IS_RUSSIA == ccm_param.t_sys_data_current.pic_id) //俄文图片
    {
      GUI_OPEN_FILE(RUS);
    }
    else if (PICTURE_IS_CHINESE_TRADITIONAL == ccm_param.t_sys_data_current.pic_id) //俄文图片
    {
      GUI_OPEN_FILE(CNT);
    }
  }
  else if (LOGO_PIC == picType)
    gui_open_res = f_open(&PictureFile, LOGO_PIC_FILE, FA_READ);  //打开图片文件
}

static void _displayPicture(int PictureName, int picType, unsigned int y_offset)
{
  unsigned int LCDWidth = 480;
  FRESULT gui_open_res;
  osDelay(50);
  //参数p数组地址，参数jsh备用，参数psize数组大小
  taskENTER_CRITICAL();   //会关闭中断，包括电机中断，打开文件消耗的时间较长，会造成电机停顿感

  if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
    LCDWidth = 1024;

  _open_pic(gui_open_res, picType, LCDWidth);

  if (gui_open_res != FR_OK)
    return;

  unsigned int RCount;
  unsigned int ColorCount = 0;
  uint16_t CurrentColor, CurrentColorNum;
  uint16_t iwidth, iheight;
  uint16_t iprewidth = 0, ipreheight = 0;
  uint32_t PictureSize;
  uint32_t x_offset = 0;
  (void)f_lseek(&PictureFile, (PictureName - 1)*sizeof(Pic_FilesMsg) + sizeof(Pic_MaseHeader));
  (void)f_read(&PictureFile, &Pic_fileMsg, sizeof(Pic_FilesMsg), &RCount);

  if (Pic_fileMsg.uFileOfs)
    (void)f_lseek(&PictureFile, Pic_fileMsg.uFileOfs);

  PictureSize = Pic_fileMsg.uFileSize;
  taskEXIT_CRITICAL();

  while (PictureSize > 0)
  {
    osDelay(1);
    taskENTER_CRITICAL();

    if (PictureSize >= PIC_BUF_SIZE)
    {
      (void)f_read(&PictureFile, ccm_param.PictureFileBuf, PIC_BUF_SIZE, &RCount);
      PictureSize = PictureSize - PIC_BUF_SIZE;
    }
    else
    {
      (void)f_read(&PictureFile, ccm_param.PictureFileBuf, PictureSize, &RCount);
      PictureSize = 0;
    }

    taskEXIT_CRITICAL();

    for (int i = 0; i < RCount; i++)
    {
      CurrentColor = (ccm_param.PictureFileBuf[i] << 8);    //16（565）位颜色的高八位
      ++i;
      CurrentColor |= ccm_param.PictureFileBuf[i];          //16（565）位颜色的低八位
      ++i;
      CurrentColorNum = (ccm_param.PictureFileBuf[i] << 8); //这个颜色的个数的高八位
      ++i;
      CurrentColorNum |= ccm_param.PictureFileBuf[i];     //这个颜色的个数的低八位
      ColorCount += CurrentColorNum;
      iwidth = ColorCount % LCDWidth + x_offset; //压缩bmp时是按顺序一行行（一行Width个像素）压缩bmp的，所以像素个数的对LCDWidth求余就得出每行有几个这样的像素。
      iheight = ColorCount / LCDWidth + y_offset; //按压缩算法规律，对LCDWidth除，即可得出该种颜色的行数
      int fill_count = 2;

      while (fill_count > 0)
      {
        //以下使用lcd_fill显示颜色，根据条件分三种情况:
        if (iheight > ipreheight) //某种像素个数超过一行（480个）
        {
          if (iheight > ipreheight + 1) //某种像素个数超过两行（480*2）
          {
            LCD_Fill_Picture(iprewidth, ipreheight, LCDWidth - 1 + x_offset, ipreheight, CurrentColor); //从上一行接着画点，直到行尾
            LCD_Fill_Picture(0 + x_offset, ipreheight + 1, LCDWidth - 1 + x_offset, iheight - 1, CurrentColor); //画某个像素：长为LCDWidth，宽为（iheight-1）-（ipreheight+1）的矩形
            LCD_Fill_Picture(0 + x_offset, iheight, iwidth, iheight, CurrentColor); //画某个像素最后一行
          }
          else//if(iheight==ipreheight+1)//某个像素个数刚好一行（480个）
          {
            LCD_Fill_Picture(iprewidth, ipreheight, LCDWidth - 1 + x_offset, iheight - 1, CurrentColor); //从上一行接着画点，知道行尾
            LCD_Fill_Picture(0 + x_offset, iheight, iwidth, iheight, CurrentColor);             //从新的一行开头画点,直到该像素在该行结束
          }
        }
        else //if(ipreheight=iheight)//某个像素个数不超过一行
        {
          LCD_Fill_Picture(iprewidth, ipreheight, iwidth, iheight, CurrentColor);
        }

        fill_count--;
      }

      iprewidth = iwidth;
      ipreheight = iheight;
    }
  }

  taskENTER_CRITICAL();
  f_close(&PictureFile);
  taskEXIT_CRITICAL();
}

void display_picture(int PictureName)
{
  _displayPicture(PictureName, DISPLAY_PIC, 0);
}

void DisplayLogoPicture(int PictureName)
{
  _displayPicture(PictureName, LOGO_PIC, 0);
}

uint32_t Draw_progressBar_new(uint32_t Printfilesize, uint32_t Filesize, int x, int y, int x_max, int y_max)
{
  uint32_t PrintPercent;
  static uint32_t Pre_PrintPercent = 1;
  PrintPercent = (uint32_t)((float)(Printfilesize - Filesize) / Printfilesize * (float)x_max);

  if (PrintPercent != Pre_PrintPercent)
  {
    LCD_Fill(x, y, x + PrintPercent, y + y_max, YELLOW);
    Pre_PrintPercent = PrintPercent;
    return 1;
  }

  return 0;
}

uint32_t Draw_progressBar(uint32_t Printfilesize, uint32_t Filesize)
{
  return Draw_progressBar_new(Printfilesize, Filesize, X_BEGIN, Y_BEGIN, BarWidth, BarHeight);
}

void gui_disp_pic_logo(void)
{
  if (ccm_param.t_sys_data_current.logo_id)
  {
    DisplayLogoPicture(ccm_param.t_sys_data_current.logo_id);
    user_pin_lcd_backlight_ctrl(true);
    (void)osDelay(3000);//延时，显示logo图标一段时间
  }
}

#ifdef __cplusplus
} //extern "C" {
#endif

