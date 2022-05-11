#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"

#ifdef __cplusplus
extern "C" {
#endif

bool setTargetedHotend(int16_t code);
extern int extrudemultiply;

#ifdef __cplusplus
} //extern "C" {
#endif

namespace gcode
{
  void m221_process(void)
  {
    if (parseGcodeBufHandle.codeSeen('S'))
    {
      int16_t tmp_code = (int16_t)parseGcodeBufHandle.codeValue();

      if (parseGcodeBufHandle.codeSeen('T'))
      {
        if (setTargetedHotend(221))
        {
          return;
        }

        //extruder_multiply[tmp_extruder] = tmp_code;
      }
      else
      {
        extrudemultiply = tmp_code ;
      }
    }
  }
}






