#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

namespace sg_grbl
{

#ifdef __cplusplus
  extern "C" {
#endif

  void reset_acceleration_rates(void);
  void config_reset_default(void);

#ifdef __cplusplus
} // extern "C" {
#endif

}

#endif//CONFIG_STORE_H











