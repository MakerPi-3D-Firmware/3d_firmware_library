#ifndef TEMPERATURE_PID_OUTPUT_FACTOR_H
#define TEMPERATURE_PID_OUTPUT_FACTOR_H
#include <stdint.h>

namespace sg_grbl
{

#ifdef __cplusplus
  extern "C" {
#endif

  void temp_accumulated_temp_per_second(const float &pid_input, const int16_t &extruder);
  void temp_reset_pid_output_factor_param(const int16_t &extruder);
  bool temp_cal_pid_output_factor(float &factor, const int16_t &extruder);
  float temp_get_pid_output_max_factor(const int16_t &extruder);
  void temp_init_pid_output_factor_param(void);

#ifdef __cplusplus
} // extern "C" {
#endif
}
#endif // TEMPERATURE_PID_OUTPUT_FACTOR_H
