#include "user_common.h"

#ifdef ENABLE_SD

static void _user_mount_sd(void)
{
  FRESULT res = f_mount(&SDFatFS, (const char *)SDPath, 0);

  if (res == FR_OK)
  {
    USER_EchoLog("SDCard f_mount ok!");
  }
  else
  {
    USER_EchoLog("SDCard f_mount not ok! Error code:%d\n", res);
  }
}

void user_sd_init(void)
{
  while (BSP_SD_Init() != MSD_OK)
    osDelay(10);

  _user_mount_sd();
}

#endif



