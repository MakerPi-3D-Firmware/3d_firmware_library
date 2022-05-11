#ifndef LVGL_INTERFACE_H
#define LVGL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

float lvgl_get_bed_curr_temp(void);
float lvgl_get_bed_target_temp(void);
float lvgl_get_ext_curr_temp(int index);
float lvgl_get_ext_target_temp(int index);

void lvgl_set_ext_target_temp(float value, int index);
void lvgl_set_bed_target_temp(float value);

int32_t lvgl_get_print_filament_length(void);
float lvgl_get_print_platform_height(void);

void lvgl_set_print_speed(int value);
int lvgl_get_print_speed(void);

uint32_t lvgl_get_print_time_sec(void);

int lvgl_get_print_percent(void);

void lvgl_disable_heater(void);

bool lvgl_is_pause_to_cool_down(void);
bool lvgl_is_finish_print(void);
void lvgl_change_filament(void);
bool lvgl_is_print_sd_file(void);
void lvgl_ref_data(void);

void lvgl_open_beep(uint32_t timeout);
void lvgl_close_beep(bool is_now);
void lvgl_set_fan(bool is_on);
int lvgl_get_fan(void);


uint8_t lvgl_get_m600_status(void);
#ifdef __cplusplus
} //extern "C"
#endif


#endif // LVGL_INTERFACE_H













