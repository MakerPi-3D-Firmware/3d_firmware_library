#ifndef FLASH_POWEROFF_DATA_H
#define FLASH_POWEROFF_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

extern volatile bool flash_poweroff_data_is_reset;

extern void flash_poweroff_data_erase(void);
extern void flash_poweroff_data_save(void);
extern void flash_poweroff_data_pause_set(void);
extern void flash_poweroff_data_pause_save(void);

#ifdef __cplusplus
} //extern "C"
#endif


#endif // FLASH_POWEROFF_DATA_H













