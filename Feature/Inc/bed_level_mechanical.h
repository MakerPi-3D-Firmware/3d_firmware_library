#ifndef BED_LEVEL_MECHANICAL_H
#define BED_LEVEL_MECHANICAL_H

#ifdef __cplusplus
extern "C" {
#endif

void bed_level_mechanical_before_warm(void);
void bed_level_mechanical_adjust_start(void);
void bed_level_mechanical_adjust_process(void);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // BED_LEVEL_MECHANICAL_H

