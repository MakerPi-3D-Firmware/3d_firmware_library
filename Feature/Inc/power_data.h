#ifndef POWER_DATA_H
#define POWER_DATA_H

#ifdef HAS_POWER_RECOVERY

#ifdef __cplusplus
extern "C" {
#endif

extern void feature_pow_data_init(void);
extern void feature_pow_data_reset_flag(void);
extern void feature_pow_data_delete_file_from_sd(void);
extern void feature_pow_data_save_file_path_name(TCHAR *path_file_name, TCHAR *file_name);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // POWER_DATA_H

