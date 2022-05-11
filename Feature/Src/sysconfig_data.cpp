#include "user_common.h"

#ifdef __cplusplus
extern "C" {
#endif

static FIL sysInfofile;    /* file objects */

// SysConfig
char sys_data[512] ;
uint32_t sys_data_size = 0;                        // sysconfig數據緩存大小

void sys_write_info(const TCHAR *filePath)
{
  UINT NewInfofile_wr;
  taskENTER_CRITICAL();

  if (f_open(&sysInfofile, filePath, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
  {
    (void)f_write(&sysInfofile, sys_data, sys_data_size, &NewInfofile_wr);
    (void)f_close(&sysInfofile);
  }

  taskEXIT_CRITICAL();
}

void sys_read_info(const TCHAR *filePath)
{
  UINT Infofile_br;
  taskENTER_CRITICAL();

  if (f_open(&sysInfofile, filePath, FA_READ) == FR_OK)
  {
    /**********文件打开成功**************/
    (void)f_read(&sysInfofile, sys_data, sizeof(sys_data), &Infofile_br);
    sys_data_size = Infofile_br;
    sys_data[sys_data_size] = 0;
    (void)f_close(&sysInfofile);
  }
  else
  {
    USER_ErrLog("sysconfig open failed!");
  }

  taskEXIT_CRITICAL();
}

#ifdef __cplusplus
} // extern "C" {
#endif

