
#if defined(USE_BOOT) || defined(USE_APP1)
#include "..\Inc\iap_param.h"
// 功能文件名
const char *unpack_function_file_name[FUNCTION_FILE_SIZE] =
{
  (char *)"Bootloader.bin",
  (char *)"APP1.bin",
  (char *)"APP2.bin",
};

// 数据文件名
const char *unpack_data_file_name[DATA_FILE_SIZE] =
{
  (char *)"GBK24.FON",
  (char *)"TouchData.bin",
  (char *)"TouchData43.bin",
  (char *)"PowerOffData.txt"
};

// 数据文件名
const char *unpack_BMP35_file_name[BMP35_FILE_SIZE] =
{
  (char *)"C_Picture.bin",
  (char *)"J_Picture.bin",
  (char *)"EN_Picture.bin",
  (char *)"KOR_Picture.bin",
  (char *)"RUS_Picture.bin",
  (char *)"C_T_Picture.bin",
  (char *)"L_Picture.bin"
};

// 数据文件名
const char *unpack_BMP43_file_name[BMP43_FILE_SIZE] =
{
  (char *)"C_Picture_43.bin",
  (char *)"EN_Picture_43.bin",
  (char *)"C_T_Picture_43.bin",
  (char *)"L_Picture.bin"
};

// 数据文件名
const char *unpack_BMP70_file_name[BMP70_FILE_SIZE] =
{
  (char *)"C_Picture_70.bin",
};

// 配置文件名
const char *unpack_config_file_name[CONFIG_FILE_SIZE] =
{
  (char *)"SysConfig.txt"
};

MaseHeader header;                  // 文件头结构定义
FilesMsg   fileMsg;                 // 文件信息结构

unsigned char isUpdateBootloaderVersion = 0;
#endif


