#include "user_common_cpp.h"

#ifdef ENABLE_FLASH

#ifdef __cplusplus
extern "C" {
#endif

// 0x080ff800~0x081000000  2kb
#define FLASH_PARAM_START_ADDR ((uint32_t)0x080ff800)
#define FLASH_PARAM_END_ADDR   ((uint32_t)0x080fffff)
#define FLASH_PARAM_PAGE_SIZE  ((uint16_t)0xF00)


FLASH_PARAM_T flash_param_t;

extern sg_grbl::matrix_3x3 plan_bed_level_matrix;

void flash_config_set_flag_save(void)
{
  taskENTER_CRITICAL();
  flash_param_t.flag = 1;
  taskEXIT_CRITICAL();
  osDelay(100);
}

void flash_config_save(void)
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
    FLASH_If_EraseSectors(FLASH_PARAM_START_ADDR, FLASH_PARAM_END_ADDR);
    address = FLASH_PARAM_START_ADDR;
    pd = (uint32_t *)(&flash_param_t);

    for (i = 0; i < sizeof(flash_param_t) / 4; i++)
    {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *pd);
      address += 4;
      pd ++;
    }

    isRight = 1;
    address = FLASH_PARAM_START_ADDR;
    pd = (uint32_t *)(&flash_param_t);

    for (i = 0; i < sizeof(flash_param_t) / 4; i++)
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

static void _flash_config_data_init_idex(void)
{
  for (int i = 0; i < 3; i++)
  {
    if (isnan(flash_param_t.idex_ext0_home_pos_adding[i]))
    {
      flash_param_t.idex_ext0_home_pos_adding[i] = 0.0f;
    }

    if (i < 2)
    {
      if (isnan(flash_param_t.idex_extruder_0_bed_offset[i]))
      {
        flash_param_t.idex_extruder_0_bed_offset[i] = 0.0f;
      }

      if (isnan(flash_param_t.idex_extruder_1_bed_offset[i]))
      {
        flash_param_t.idex_extruder_1_bed_offset[i] = 0.0f;
      }
    }
  }
}

static void _flash_config_data_init_mix(void)
{
  for (int i = 0; i < 3; i++)
  {
    if (isnan(flash_param_t.mix_ext0_home_pos_adding[i]))
    {
      flash_param_t.mix_ext0_home_pos_adding[i] = 0.0f;
    }

    if (i < 2)
    {
      if (isnan(flash_param_t.mix_extruder_0_bed_offset[i]))
      {
        flash_param_t.mix_extruder_0_bed_offset[i] = 0.0f;
      }
    }
  }
}

static void _flash_config_data_init_laser(void)
{
  for (int i = 0; i < 3; i++)
  {
    if (isnan(flash_param_t.laser_ext0_home_pos_adding[i]))
    {
      flash_param_t.laser_ext0_home_pos_adding[i] = 0.0f;
    }

    if (i < 2)
    {
      if (isnan(flash_param_t.laser_extruder_0_bed_offset[i]))
      {
        flash_param_t.laser_extruder_0_bed_offset[i] = 0.0f;
      }

      if (isnan(flash_param_t.laser_extruder_1_bed_offset[i]))
      {
        flash_param_t.laser_extruder_1_bed_offset[i] = 0.0f;
      }
    }
  }

  if (isnan(flash_param_t.laser_focus_z_height))
  {
    flash_param_t.laser_focus_z_height = 42.0f;
  }
}

static void _flash_config_data_init_bed_level(void)
{
  plan_bed_level_matrix.set_to_identity();

  if (flash_param_t.is_process_bedlevel != 1 && flash_param_t.is_process_bedlevel != 2) // 没执行调平操作，重置调平数据
  {
    flash_param_t.matrix_back_right.set_to_identity();
    flash_param_t.matrix_front_left.set_to_identity();
    flash_param_t.matrix_left_back.set_to_identity();
    flash_param_t.matrix_right_front.set_to_identity();
    plan_bed_level_matrix.set_to_identity();
  }
}

static void _flash_config_data_init(void)
{
  _flash_config_data_init_bed_level();
  _flash_config_data_init_idex();
  _flash_config_data_init_mix();
  _flash_config_data_init_laser();
  model_flash_config_init();
}

static void _flash_config_old_data_migration(void)
{
  if (flash_param_t.update_param_flag != FLASH_UPDATE_FLAG)
  {
    flash_param_t.update_param_flag = FLASH_UPDATE_FLAG;
    flash_param_t.key_sound = 1;
    flash_param_t.alarm_sound = 1;
    flash_param_t.is_buzzer_chg = 1;
    flash_param_t.z_offset_value = ccm_param.t_sys_data_current.z_offset_value;
    flash_param_t.pid_output_factor = ccm_param.t_sys_data_current.pid_output_factor;
    flash_param_t.poweroff_rec_z_max_value = ccm_param.t_sys_data_current.poweroff_rec_z_max_value;
    flash_config_save();
  }
}

void flash_config_read(void)
{
  uint32_t address;
  uint32_t *pd;
  uint16_t i;
  address = FLASH_PARAM_START_ADDR;
  pd = (uint32_t *)(&flash_param_t);

  for (i = 0; i < sizeof(flash_param_t) / 4; i++)
  {
    *pd = *((volatile uint32_t *) address);
    address += 4;
    pd ++;
  }

  USER_EchoLog("flash_param_t.version = %d", flash_param_t.version);

  if (flash_param_t.version == FLASH_PARAM_VERSION)
  {
    flash_param_t.flag = 0;
    _flash_config_data_init();
    _flash_config_old_data_migration();
  }
  else
  {
    memset((char *)&flash_param_t, 0, sizeof(flash_param_t));
    flash_param_t.version = FLASH_PARAM_VERSION;
    _flash_config_old_data_migration();
  }

  ccm_param.t_sys.sound_key = flash_param_t.key_sound;
  ccm_param.t_sys.sound_alarm = flash_param_t.alarm_sound;
  ccm_param.t_sys_data_current.z_offset_value = flash_param_t.z_offset_value;
  ccm_param.t_sys_data_current.pid_output_factor = flash_param_t.pid_output_factor;
  ccm_param.t_sys_data_current.poweroff_rec_z_max_value = flash_param_t.poweroff_rec_z_max_value;
}

#ifdef __cplusplus
}
#endif

#endif // ENABLE_FLASH
















