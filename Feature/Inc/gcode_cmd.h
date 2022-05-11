#ifndef GCODE_CMD_H
#define GCODE_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

void gcode_cmd_g28(void);
void gcode_cmd_z_down_to_bottom(void);
void gcode_cmd_z_down(char code, float z_value);
void gcode_cmd_t0(bool is_process);
void gcode_cmd_t1(bool is_process);
void gcode_cmd_coord_transform(bool is_on);
void gcode_cmd_gui_wait_done(void);
void gcode_cmd_endstop_check(bool is_on);
void gcode_cmd_eb_absolute_mode(void);
void gcode_cmd_eb_relative_mode(void);
void gcode_cmd_all_axis_relative_mode(bool is_on);
void gcode_cmd_eb_reset(void);
void gcode_cmd_xy_move_to_home(void);
void gcode_cmd_z_down_60mm_xy_to_home(void);

void gcode_cmd_load_pre_do(void);
void gcode_cmd_load_process(void);

void gcode_cmd_unload_pre_do(void);
void gcode_cmd_unload_process(void);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // GCODE_CMD_H

