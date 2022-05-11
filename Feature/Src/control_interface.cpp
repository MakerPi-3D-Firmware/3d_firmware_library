#include "user_common_cpp.h"

#ifdef HAS_DOOR_CONTROL
  #include "../../Feature/Inc/control_door.h"
#endif

#ifdef HAS_LED_CONTROL
  #include "../../Feature/Inc/control_led.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void feature_control(void)
{
#ifdef HAS_DOOR_CONTROL
  feature_control_door();
  osDelay(50);
#endif
#ifdef HAS_LED_CONTROL
  feature_control_led();
  osDelay(50);
#endif
#ifdef HAS_FAN_CONTROL
  feature_control_fan();
  osDelay(50);
#endif
}

#ifdef __cplusplus
} //extern "C" {
#endif
















