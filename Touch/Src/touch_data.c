#include "user_common.h"
#ifdef ENABLE_TOUCH

#define TouchDataFile (_T("1:/TouchData.bin"))
#define TouchDataFile43 (_T("1:/TouchData43.bin"))
#define TOUCH_DATA_FILE USERFile
void touch_data_read(uint32_t *pBufferRead, uint32_t NumToRead)
{
  uint32_t i;
  uint8_t DataBuf[24] = {0};
  TCHAR fileName[20];
  memset(fileName, 0, sizeof(fileName));

  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    utils_str_memmove(fileName, (TCHAR *)TouchDataFile);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    utils_str_memmove(fileName, (TCHAR *)TouchDataFile43);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    utils_str_memmove(fileName, (TCHAR *)TouchDataFile);
  }

  taskENTER_CRITICAL();

  if (f_open(&TOUCH_DATA_FILE, fileName, FA_READ) == FR_OK)
  {
    (void)f_read(&TOUCH_DATA_FILE, DataBuf, NumToRead * 4, (uint32_t *)NULL); //读取24字节
    (void)f_close(&TOUCH_DATA_FILE);
  }

  taskEXIT_CRITICAL();

  for (i = 0; i < NumToRead; ++i)
  {
    pBufferRead[i] = (uint32_t)DataBuf[i * 4];
    pBufferRead[i] += (((uint32_t)DataBuf[(i * 4) + 1]) << 8);
    pBufferRead[i] += (((uint32_t)DataBuf[(i * 4) + 2]) << 16);
    pBufferRead[i] += (((uint32_t)DataBuf[(i * 4) + 3]) << 24);
  }
}

void touch_data_write(const uint32_t *pBufferWrite, const uint32_t NumToWrite)
{
  uint32_t i;
  uint8_t DataBuf[24] = {0};
  TCHAR fileName[20];
  memset(fileName, 0, sizeof(fileName));

  if (ccm_param.t_sys.lcd_type == LCD_TYPE_35_480320_SIZE)
  {
    utils_str_memmove(fileName, (TCHAR *)TouchDataFile);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_43_480272_SIZE)
  {
    utils_str_memmove(fileName, (TCHAR *)TouchDataFile43);
  }
  else if (ccm_param.t_sys.lcd_type == LCD_TYPE_7_1024600_SIZE)
  {
    utils_str_memmove(fileName, (TCHAR *)TouchDataFile);
  }

  for (i = 0; i < NumToWrite; ++i)
  {
    DataBuf[i * 4] = (uint8_t)pBufferWrite[i];
    DataBuf[(i * 4) + 1] = (uint8_t)(pBufferWrite[i] >> 8);
    DataBuf[(i * 4) + 2] = (uint8_t)(pBufferWrite[i] >> 16);
    DataBuf[(i * 4) + 3] = (uint8_t)(pBufferWrite[i] >> 24);
  }

  taskENTER_CRITICAL();
  f_unlink(fileName);

  if (f_open(&TOUCH_DATA_FILE, fileName, FA_CREATE_NEW | FA_WRITE) == FR_OK)
  {
    (void)f_write(&TOUCH_DATA_FILE, DataBuf, NumToWrite * 4, (uint32_t *)NULL); //写入24字节
    (void)f_close(&TOUCH_DATA_FILE);
  }

  taskEXIT_CRITICAL();
}

#endif













