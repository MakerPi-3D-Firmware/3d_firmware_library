#ifndef FLASH_INTERFACE_H
#define FLASH_INTERFACE_H

#include "power_cal_z_height.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void flash_config_read(void);
extern void flash_config_set_flag_save(void);

extern void flash_poweroff_data_read(void);
extern void flash_poweroff_data_reset(void);

extern void flash_interface_process(void);

extern void flash_interface_save_buzzer(void);
extern void flash_interface_save_cal_zmax_pos(float value);

#ifdef __cplusplus
} //extern "C"
#endif


#endif // FLASH_INTERFACE_H













