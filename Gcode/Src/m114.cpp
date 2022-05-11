#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  void m114_process(void)
  {
    float e = sg_grbl::st_get_position_length(E_AXIS);
    float b = 0.0f;

    if (ccm_param.t_sys_data_current.enable_color_mixing)
    {
      b = sg_grbl::st_get_position_length(B_AXIS);
    }

    USER_EchoLogStr("ok X:%.2f X2:%.2f Y:%.2f Z:%.2f Z2:%.2f ", sg_grbl::st_get_position_length(X_AXIS), sg_grbl::st_get_position_length(X2_AXIS), \
                    sg_grbl::st_get_position_length(Y_AXIS), sg_grbl::st_get_position_length(Z_AXIS), sg_grbl::st_get_position_length(Z2_AXIS));
    USER_EchoLogStr("E:%.2f B:%.2f ", e, b);
    USER_EchoLogStr("\r\n");
  }
}






