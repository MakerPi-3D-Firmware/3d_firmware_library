#include "user_common_cpp.h"

#ifdef HAS_FILAMENT

#include "../../Feature/Inc/filament_check.h"

namespace feature_filament
{

  void f_init(void)
  {
    check_init();
    control_init();
  }

  void f_process(void)
  {
    if (!print_status.is_printing)
    {
      // Load and unload filament entry
      control_process();
    }
    else
    {
      // Midway refueling status update
      mid_chg_refresh_status();
    }

    // No material detection entry
    check();
  }
}

#endif // HAS_FILAMENT_SENSOR

