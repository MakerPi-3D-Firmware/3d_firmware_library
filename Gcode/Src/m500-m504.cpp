
#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#include "../../Grbl/Inc/temperature_pid_temp.h"
#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint8_t gcode_cmd_type;
extern float add_homeing[XYZ_NUM_AXIS];

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{

  void m500_process(void)
  {
    memmove((planner_settings_t *)&flash_param_t.settings.planner_settings, (planner_settings_t *)&ccm_param.planner_settings, sizeof(planner_settings_t));
    memmove((float *)&flash_param_t.settings.home_offset, (float *)&add_homeing, sizeof(float)*XYZ_NUM_AXIS);
    flash_param_t.settings.temp_p = sg_grbl::temp_pid_extruder_get_kp();
    flash_param_t.settings.temp_i = sg_grbl::temp_pid_extruder_get_ki();
    flash_param_t.settings.temp_d = sg_grbl::temp_pid_extruder_get_kd();
    flash_param_t.settings.settings_version = FLASH_SETTINGS_VERSION;
    flash_param_t.flag = 1;

    if (gcode_cmd_type == GCODE_TYPE_USBD)
    {
      USER_SEND_USBD_CMD_BUF("echo: Hardcoded Default Settings Save, Version = V%d\r\n", FLASH_SETTINGS_VERSION);
    }
    else
    {
      USER_EchoLogStr("echo: Hardcoded Default Settings Save, Version = V%d\r\n", FLASH_SETTINGS_VERSION);
    }
  }

  void m501_process(void)
  {
    if (flash_param_t.settings.settings_version == FLASH_SETTINGS_VERSION)
    {
      if (gcode_cmd_type == GCODE_TYPE_USBD)
      {
        USER_SEND_USBD_CMD_BUF("echo: Flash version mismatch(EEPROM = V%d Marlin = V%d)\r\n", flash_param_t.settings.settings_version, FLASH_SETTINGS_VERSION);
        USER_SEND_USBD_CMD_BUF("echo: Hardcoded Default Settings Loaded\r\n");
      }
      else
      {
        USER_EchoLogStr("echo: Flash version mismatch(EEPROM = V%d Marlin = V%d)\r\n", flash_param_t.settings.settings_version, FLASH_SETTINGS_VERSION);
        USER_EchoLogStr("echo: Hardcoded Default Settings Loaded\r\n");
      }

      memmove((planner_settings_t *)&ccm_param.planner_settings, (planner_settings_t *)&flash_param_t.settings.planner_settings, sizeof(planner_settings_t));
      memmove((float *)&add_homeing, (float *)&flash_param_t.settings.home_offset, sizeof(float)*XYZ_NUM_AXIS);
      sg_grbl::temp_pid_extruder_set_kp(flash_param_t.settings.temp_p);
      sg_grbl::temp_pid_extruder_set_ki(flash_param_t.settings.temp_i);
      sg_grbl::temp_pid_extruder_set_kd(flash_param_t.settings.temp_d);
    }
    else
    {
      if (gcode_cmd_type == GCODE_TYPE_USBD)
      {
        USER_SEND_USBD_CMD_BUF("echo: Flash version mismatch(EEPROM = V%d Marlin = V%d)\r\n", flash_param_t.settings.settings_version, FLASH_SETTINGS_VERSION);
        USER_SEND_USBD_CMD_BUF("echo: Hardcoded Default Settings not Loaded\r\n");
      }
      else
      {
        USER_EchoLogStr("echo: Flash version mismatch(EEPROM = V%d Marlin = V%d)\r\n", flash_param_t.settings.settings_version, FLASH_SETTINGS_VERSION);
        USER_EchoLogStr("echo: Hardcoded Default Settings not Loaded\r\n");
      }
    }
  }

  void m502_process(void)
  {
    gcode::init();

    for (int i = 0; i < XYZ_NUM_AXIS; i++)
    {
      add_homeing[i] = 0.0f;
    }

    if (gcode_cmd_type == GCODE_TYPE_USBD)
    {
      USER_SEND_USBD_CMD_BUF("echo:Hardcoded Default Settings Reset\r\n");
    }
    else
    {
      USER_EchoLogStr("echo:Hardcoded Default Settings Reset\r\n");
    }
  }

  void m503_process(void)
  {
    if (gcode_cmd_type == GCODE_TYPE_USBD)
    {
      //      USER_SEND_USBD_CMD_BUF("echo:  G21    ; Units in mm (mm)\r\n");
      //      USER_SEND_USBD_CMD_BUF("echo:  M200 S0 D1.75\r\n");
      USER_SEND_USBD_CMD_BUF("echo:; Steps per unit:\r\n");
      USER_SEND_USBD_CMD_BUF("echo:  M92 X%.2f I%.2f Y%.2f Z%.2f J%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.axis_steps_per_mm[X_AXIS], \
                             (float)ccm_param.planner_settings.axis_steps_per_mm[X2_AXIS], \
                             (float)ccm_param.planner_settings.axis_steps_per_mm[Y_AXIS], \
                             (float)ccm_param.planner_settings.axis_steps_per_mm[Z_AXIS], \
                             (float)ccm_param.planner_settings.axis_steps_per_mm[Z2_AXIS], \
                             (float)ccm_param.planner_settings.axis_steps_per_mm[E_AXIS], \
                             (float)ccm_param.planner_settings.axis_steps_per_mm[B_AXIS]);
      USER_SEND_USBD_CMD_BUF("echo:; Maximum feedrates (units/s):\r\n");
      USER_SEND_USBD_CMD_BUF("echo:  M203 X%.2f I%.2f Y%.2f Z%.2f J%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.max_feedrate_mm_s[X_AXIS], \
                             (float)ccm_param.planner_settings.max_feedrate_mm_s[X2_AXIS], \
                             (float)ccm_param.planner_settings.max_feedrate_mm_s[Y_AXIS], \
                             (float)ccm_param.planner_settings.max_feedrate_mm_s[Z_AXIS], \
                             (float)ccm_param.planner_settings.max_feedrate_mm_s[Z2_AXIS], \
                             (float)ccm_param.planner_settings.max_feedrate_mm_s[E_AXIS], \
                             (float)ccm_param.planner_settings.max_feedrate_mm_s[B_AXIS]);
      USER_SEND_USBD_CMD_BUF("echo:; Maximum Acceleration (units/s2):\r\n");
      USER_SEND_USBD_CMD_BUF("echo:  M201 X%.2f I%.2f Y%.2f Z%.2f J%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[X_AXIS], \
                             (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[X2_AXIS], \
                             (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[Y_AXIS], \
                             (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[Z_AXIS], \
                             (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[Z2_AXIS], \
                             (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[E_AXIS], \
                             (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[B_AXIS]);
      USER_SEND_USBD_CMD_BUF("echo:; Acceleration (units/s2): T<print_accel> S<retract_accel>\r\n");
      USER_SEND_USBD_CMD_BUF("echo:  M204 T%.2f S%.2f\r\n", ccm_param.planner_settings.acceleration, ccm_param.planner_settings.retract_acceleration);
      USER_SEND_USBD_CMD_BUF("echo:; Advanced: B<min_segment_time_us> S<min_feedrate> T<min_travel_feedrate> ");
      USER_SEND_USBD_CMD_BUF("X<max_x_jerk> Y<max_y_jerk> Z<max_z_jerk> E<max_e_jerk>\r\n");
      USER_SEND_USBD_CMD_BUF("echo:  M205 B%.2f S%.2f T%.2f X%.2f Y%.2f Z%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.min_segment_time_us, \
                             (float)ccm_param.planner_settings.min_feedrate_mm_s, \
                             (float)ccm_param.planner_settings.min_travel_feedrate_mm_s, \
                             (float)ccm_param.planner_settings.max_xy_jerk, \
                             (float)ccm_param.planner_settings.max_xy_jerk, \
                             (float)ccm_param.planner_settings.max_z_jerk, \
                             (float)ccm_param.planner_settings.max_e_jerk, \
                             (float)ccm_param.planner_settings.max_b_jerk);
      USER_SEND_USBD_CMD_BUF("echo:; Home offset:\r\n");
      USER_SEND_USBD_CMD_BUF("echo:  M206 X%.2f Y%.2f Z%.2f\r\n", add_homeing[X_AXIS], add_homeing[Y_AXIS], add_homeing[Z_AXIS]);
      //      USER_SEND_USBD_CMD_BUF("echo:; Auto Bed Leveling:\r\n");
      //      USER_SEND_USBD_CMD_BUF("echo:  M420 S0 Z0.00\r\n");
      USER_SEND_USBD_CMD_BUF("echo:; PID settings:\r\n");
      USER_SEND_USBD_CMD_BUF("echo:  M301 P%.2f I%.2f D%.2f\r\n", sg_grbl::temp_pid_extruder_get_kp(), sg_grbl::temp_pid_extruder_get_ki(), sg_grbl::temp_pid_extruder_get_kd());
      //      USER_SEND_USBD_CMD_BUF("echo:; Power-Loss Recovery:\r\n");
      //      USER_SEND_USBD_CMD_BUF("echo:  M413 S1\r\n");
      //      USER_SEND_USBD_CMD_BUF("echo:; Z-Probe Offset (mm):\r\n");
      //      USER_SEND_USBD_CMD_BUF("echo:  M851 X0.00 Y0.00 Z0.00\r\n");
    }
    else
    {
      //      USER_EchoLogStr("echo:  G21    ; Units in mm (mm)\r\n");
      //      USER_EchoLogStr("echo:  M200 S0 D1.75\r\n");
      USER_EchoLogStr("echo:; Steps per unit:\r\n");
      USER_EchoLogStr("echo:  M92 X%.2f I%.2f Y%.2f Z%.2f J%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.axis_steps_per_mm[X_AXIS], \
                      (float)ccm_param.planner_settings.axis_steps_per_mm[X2_AXIS], \
                      (float)ccm_param.planner_settings.axis_steps_per_mm[Y_AXIS], \
                      (float)ccm_param.planner_settings.axis_steps_per_mm[Z_AXIS], \
                      (float)ccm_param.planner_settings.axis_steps_per_mm[Z2_AXIS], \
                      (float)ccm_param.planner_settings.axis_steps_per_mm[E_AXIS], \
                      (float)ccm_param.planner_settings.axis_steps_per_mm[B_AXIS]);
      USER_EchoLogStr("echo:; Maximum feedrates (units/s):\r\n");
      USER_EchoLogStr("echo:  M203 X%.2f I%.2f Y%.2f Z%.2f J%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.max_feedrate_mm_s[X_AXIS], \
                      (float)ccm_param.planner_settings.max_feedrate_mm_s[X2_AXIS], \
                      (float)ccm_param.planner_settings.max_feedrate_mm_s[Y_AXIS], \
                      (float)ccm_param.planner_settings.max_feedrate_mm_s[Z_AXIS], \
                      (float)ccm_param.planner_settings.max_feedrate_mm_s[Z2_AXIS], \
                      (float)ccm_param.planner_settings.max_feedrate_mm_s[E_AXIS], \
                      (float)ccm_param.planner_settings.max_feedrate_mm_s[B_AXIS]);
      USER_EchoLogStr("echo:; Maximum Acceleration (units/s2):\r\n");
      USER_EchoLogStr("echo:  M201 X%.2f I%.2f Y%.2f Z%.2f J%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[X_AXIS], \
                      (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[X2_AXIS], \
                      (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[Y_AXIS], \
                      (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[Z_AXIS], \
                      (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[Z2_AXIS], \
                      (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[E_AXIS], \
                      (float)ccm_param.planner_settings.max_acceleration_mm_per_s2[B_AXIS]);
      USER_EchoLogStr("echo:; Acceleration (units/s2): T<print_accel> S<retract_accel>\r\n");
      USER_EchoLogStr("echo:  M204 T%.2f S%.2f\r\n", ccm_param.planner_settings.acceleration, ccm_param.planner_settings.retract_acceleration);
      USER_EchoLogStr("echo:; Advanced: B<min_segment_time_us> S<min_feedrate> T<min_travel_feedrate> ");
      USER_EchoLogStr("X<max_x_jerk> Y<max_y_jerk> Z<max_z_jerk> E<max_e_jerk>\r\n");
      USER_EchoLogStr("echo:  M205 B%.2f S%.2f T%.2f X%.2f Y%.2f Z%.2f E%.2f B%.2f\r\n", (float)ccm_param.planner_settings.min_segment_time_us, \
                      (float)ccm_param.planner_settings.min_feedrate_mm_s, \
                      (float)ccm_param.planner_settings.min_travel_feedrate_mm_s, \
                      (float)ccm_param.planner_settings.max_xy_jerk, \
                      (float)ccm_param.planner_settings.max_xy_jerk, \
                      (float)ccm_param.planner_settings.max_z_jerk, \
                      (float)ccm_param.planner_settings.max_e_jerk, \
                      (float)ccm_param.planner_settings.max_b_jerk);
      USER_EchoLogStr("echo:; Home offset:\r\n");
      USER_EchoLogStr("echo:  M206 X%.2f Y%.2f Z%.2f\r\n", add_homeing[X_AXIS], add_homeing[Y_AXIS], add_homeing[Z_AXIS]);
      //      USER_EchoLogStr("echo:; Auto Bed Leveling:\r\n");
      //      USER_EchoLogStr("echo:  M420 S0 Z0.00\r\n");
      USER_EchoLogStr("echo:; PID settings:\r\n");
      USER_EchoLogStr("echo:  M301 P%.2f I%.2f D%.2f\r\n", sg_grbl::temp_pid_extruder_get_kp(), sg_grbl::temp_pid_extruder_get_ki(), sg_grbl::temp_pid_extruder_get_kd());
      //      USER_EchoLogStr("echo:; Power-Loss Recovery:\r\n");
      //      USER_EchoLogStr("echo:  M413 S1\r\n");
      //      USER_EchoLogStr("echo:; Z-Probe Offset (mm):\r\n");
      //      USER_EchoLogStr("echo:  M851 X0.00 Y0.00 Z0.00\r\n");
    }
  }
}






