#include "user_common_cpp.h"

#ifdef ENABLE_FLASH

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_POWEROFF_DATA_START_ADDR ((uint32_t)0x080C0000)
#define FLASH_POWEROFF_DATA_END_ADDR   ((uint32_t)0x080c0200)
#define FLASH_POWEROFF_DATA_SIZE  ((uint16_t)0x200)

volatile bool flash_poweroff_data_is_reset = false;

// 暂停打印，先设置当前数据
void flash_poweroff_data_pause_set(void)
{
  taskENTER_CRITICAL();
  memmove((planner_running_status_t *)&ccm_param.flash_poweroff_recovery,
          (planner_running_status_t *)&ccm_param.runningStatus[ccm_param.block_buffer_tail], sizeof(planner_running_status_t));
  ccm_param.flash_poweroff_recovery.flag = 1;
  taskEXIT_CRITICAL();
}

// 暂停打印后，Z高度会有变化
void flash_poweroff_data_pause_save(void)
{
  taskENTER_CRITICAL();
  ccm_param.flash_poweroff_recovery.z_real_change_value = sg_grbl::plan_get_xyz_real(Z_AXIS);
  flash_poweroff_data_save();
  taskEXIT_CRITICAL();
}

/*
  从片内flash中读出断电保存的数据
*/
void flash_poweroff_data_read(void)
{
  uint32_t address;
  uint32_t *pd;
  uint16_t i;
  address = FLASH_POWEROFF_DATA_START_ADDR;
  pd = (uint32_t *)(&ccm_param.flash_poweroff_recovery);

  for (i = 0; i < sizeof(ccm_param.flash_poweroff_recovery) / 4; i++)
  {
    *pd = *((volatile uint32_t *) address);
    address += 4;
    pd ++;
  }
}

/*
  断电时保存数据到flash，只保存，不擦除
  擦拭flash动作在每次开始打印前、停止打印按钮、取消续打按钮，打印完成后，因为断电瞬间来不及做擦除动作
*/
void flash_poweroff_data_save(void)
{
  uint16_t i;
  uint8_t writeTimes;
  uint32_t address;
  uint8_t isRight;
  uint32_t *pd;
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);
  writeTimes = 1;

  while (writeTimes--)
  {
    //    FLASH_If_EraseSectors(FLASH_POWEROFF_DATA_START_ADDR, FLASH_POWEROFF_DATA_END_ADDR);
    address = FLASH_POWEROFF_DATA_START_ADDR;
    pd = (uint32_t *)(&ccm_param.flash_poweroff_recovery);

    for (i = 0; i < sizeof(ccm_param.flash_poweroff_recovery) / 4; i++)
    {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *pd);
      address += 4;
      pd ++;
    }

    isRight = 1;
    address = FLASH_POWEROFF_DATA_START_ADDR;
    pd = (uint32_t *)(&ccm_param.flash_poweroff_recovery);

    for (i = 0; i < sizeof(ccm_param.flash_poweroff_recovery) / 4; i++)
    {
      if ((*(__IO uint32_t *) address) != *pd)
      {
        isRight = 0;
      }

      address += 4;
      pd ++;
    }

    if (isRight)
    {
      break;
    }
  }

  HAL_FLASH_Lock();//上锁写保护
}

void flash_poweroff_data_reset(void)
{
  taskENTER_CRITICAL();
  flash_poweroff_data_is_reset = true;
  taskEXIT_CRITICAL();
}

/*
  擦拭flash 断电续打的数据
*/
void flash_poweroff_data_erase(void)
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);
  FLASH_If_EraseSectors(FLASH_POWEROFF_DATA_START_ADDR, FLASH_POWEROFF_DATA_END_ADDR);
  HAL_FLASH_Lock();
}

#ifdef __cplusplus
}
#endif

#endif



















