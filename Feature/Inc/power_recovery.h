#ifndef POWER_RECOVERY_H
#define POWER_RECOVERY_H

#ifdef HAS_POWER_RECOVERY

#ifdef __cplusplus
extern "C" {
#endif

extern void feature_pow_rec_ready_to_print(void);
extern void feature_pow_rec_process(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // POWER_RECOVERY_H

