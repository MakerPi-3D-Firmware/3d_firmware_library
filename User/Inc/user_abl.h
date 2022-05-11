#ifndef USER_ABL_H
#define USER_ABL_H

#ifdef ENABLE_ABL

#ifdef __cplusplus
extern "C" {
#endif

void user_abl_deinit(void);
void user_abl_init(void);
bool user_abl_z_check(void);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif

#endif //USER_ABL_H

