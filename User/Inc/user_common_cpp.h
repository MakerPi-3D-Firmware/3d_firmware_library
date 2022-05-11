#ifndef USER_COMMON_CPP_H
#define USER_COMMON_CPP_H

#include "./user_common.h"

#include "../../Model/Inc/model.h"

#include "../../Feature/Inc/control_interface.h"
#include "../../Feature/Inc/sysconfig.h"
#ifndef ENABLE_GUI_LVGL
  #include "../../Feature/Inc/bed_level_mechanical.h"
  #include "../../Feature/Inc/dual_e0_e1_coor_cal.h"
#endif
#include "../../Feature/Inc/gcode_cmd.h"

#ifdef ENABLE_FLASH
  #include "../../Feature/Inc/flash_config.h"
  #include "../../Feature/Inc/flash_poweroff_data.h"
#endif

#ifdef HAS_FILAMENT
  #include "../../Feature/Inc/filament.h"
  #include "../../Feature/Inc/filament_mid_chg.h"
  #include "../../Feature/Inc/filament_control.h"
#endif

#ifdef HAS_PRINT_CONTROL
  #include "../../Feature/Inc/print_control.h"
#endif

#ifdef HAS_FAN_CONTROL
  #include "../../Feature/Inc/control_fan.h"
#endif

#include "../../Grbl/Inc/Configuration.h"
#include "../../Grbl/Inc/ConfigurationStore.h"
#include "../../Grbl/Inc/temperature.h"
#include "../../Grbl/Inc/planner.h"
#include "../../Grbl/Inc/stepper.h"
#include "../../Grbl/Inc/vector_3.h"

#include "../../Gcode/Inc/gcode.h"
#include "../../Gcode/Inc/process_command.h"
#include "../../Gcode/Inc/process_m_code.h"

#ifndef ENABLE_GUI_LVGL
  #include "../../Gui/Inc/gui_common.h"
  #include "../../Gui/Inc/gui_common_functions.h"
  #include "../../Gui/Inc/gui_common_languages.h"
  #include "../../Gui/Inc/gui_button.h"
  #include "../../Gui/Inc/gui_display_pic.h"
  #include "../../Gui/Inc/gui_display_file_name.h"
  #include "../../Gui/Inc/gui_touch.h"
  #include "../../Gui/Inc/gui_display_text.h"
  #include "../../Gui/Inc/gui_display_error.h"
#else
  #include "custom.h"
#endif

#ifdef HAS_BOARD_TEST
  #include "../../Feature/Inc/board_test.h"
#endif
#endif // USER_COMMON_CPP_H



















