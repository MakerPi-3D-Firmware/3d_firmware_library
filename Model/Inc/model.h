#ifndef MODEL_H
#define MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

void model_flash_config_init(void);
void model_grbl_config_init(void);
void model_init(void);
void machine_model_init(void);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // MODEL_H

