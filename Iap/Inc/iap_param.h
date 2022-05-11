#ifndef IAP_PARAM_H
#define IAP_PARAM_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_BOOT) || defined(USE_APP1)

// 资源包文件头结构
typedef struct SMaseFileHeader
{
  unsigned int   uFileFlag;              // 包文件头标记: 'MASE'
  unsigned int   uFileCount;             // 包内文件个数
  unsigned int   uFileListOfs;           // 文件列表偏移
  unsigned int   uMaxFileCount;          // 最大子文件个数
  unsigned int   uFileSize;              // 包文件的大小
  unsigned char  IsRenameUpdateFile;     // 更新完毕后是否重命名更新文件Update.bin为Update_Back.bin
  unsigned char  IsUpdateAPP1;           // 是否更新APP1
  unsigned char  IsUpdateLanguage;       // 是否更新界面语言
  unsigned char  IsUpdateModel;          // 是否更新机型
  unsigned char  IsUpdateFunction;       // 是否更新功能配置----功能：混色功能
  unsigned char  IsUpdateFilesInSDCard;  // 是否更新SD卡中的文件---图片文件、字库文件、触摸数据文件
  unsigned char  IsUpdateLOGOinterface;  // 是否更新logo开关
  unsigned char  IsUpdateCustomModelId;  // 是否更新定制机型id
  unsigned char  IsLcd43;
  unsigned char  IsLcd70;
  unsigned char  Data9;
} MaseHeader;

// 包内文件信息结构
typedef struct SFilesMessage
{
  unsigned int   uFileOfs;               // 本文件在包内的偏移
  unsigned int   uFileSize;              // 本文件的大小
  char      szFileName[60];         // 本文件的路径
} FilesMsg;

#define ESAM (((((('E'<<8)|'S')<<8)|'A')<<8)|'M')
#define FUNCTION_FILE_SIZE 3
#define DATA_FILE_SIZE 4
#define CONFIG_FILE_SIZE 1
#define BMP43_FILE_SIZE 4
#define BMP35_FILE_SIZE 7
#define BMP70_FILE_SIZE 1

extern const char *unpack_function_file_name[FUNCTION_FILE_SIZE];   // 功能文件名
extern const char *unpack_data_file_name[DATA_FILE_SIZE];           // 数据文件名
extern const char *unpack_config_file_name[CONFIG_FILE_SIZE];       // 配置文件名
extern const char *unpack_BMP43_file_name[BMP43_FILE_SIZE];
extern const char *unpack_BMP35_file_name[BMP35_FILE_SIZE];
extern const char *unpack_BMP70_file_name[BMP70_FILE_SIZE];
extern MaseHeader header;                  // 文件头结构定义
extern FilesMsg   fileMsg;                 // 文件信息结构
extern unsigned char  isUpdateBootloaderVersion;

#endif

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // IAP_PARAM_H


