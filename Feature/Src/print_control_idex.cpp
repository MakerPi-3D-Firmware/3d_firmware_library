#include "user_common_cpp.h"
#ifdef HAS_PRINT_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} //extern "C" {
#endif

namespace feature_print_control
{

  bool idex_sys_is_extruder(void)
  {
    return flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL && ccm_param.t_sys.is_idex_extruder;
  }

  void idex_sys_set_extruder(bool is_idex)
  {
    ccm_param.t_sys.is_idex_extruder = is_idex;
  }

  static bool idex_print_type_equal(unsigned char print_type0, unsigned char print_type1)
  {
    if (idex_sys_is_extruder())
    {
      return print_type0 == print_type1;
    }
    else
    {
      return false;
    }
  }

  bool idex_sys_is_normal(void)
  {
    return idex_print_type_equal(ccm_param.t_sys.print_type_idex, IDEX_PRINT_TYPE_NORMAL);
  }

  bool idex_sys_is_copy(void)
  {
    return idex_print_type_equal(ccm_param.t_sys.print_type_idex, IDEX_PRINT_TYPE_COPY);
  }

  bool idex_sys_is_mirror(void)
  {
    return idex_print_type_equal(ccm_param.t_sys.print_type_idex, IDEX_PRINT_TYPE_MIRROR);
  }

  bool idex_sys_is_copy_or_mirror(void)
  {
    return idex_sys_is_copy() || idex_sys_is_mirror();
  }

  void idex_sys_set_normal(void)
  {
    ccm_param.t_sys.print_type_idex = IDEX_PRINT_TYPE_NORMAL;
  }

  void idex_sys_set_copy(void)
  {
    ccm_param.t_sys.print_type_idex = IDEX_PRINT_TYPE_COPY;
  }

  void idex_sys_set_mirror(void)
  {
    ccm_param.t_sys.print_type_idex = IDEX_PRINT_TYPE_MIRROR;
  }

  void idex_sys_copy_flash_print_type(void)
  {
    ccm_param.t_sys.print_type_idex = flash_param_t.idex_print_type;
  }

  bool idex_flash_is_normal(void)
  {
    return idex_print_type_equal(flash_param_t.idex_print_type, IDEX_PRINT_TYPE_NORMAL);
  }

  bool idex_flash_is_copy(void)
  {
    return idex_print_type_equal(flash_param_t.idex_print_type, IDEX_PRINT_TYPE_COPY);
  }

  bool idex_flash_is_mirror(void)
  {
    return idex_print_type_equal(flash_param_t.idex_print_type, IDEX_PRINT_TYPE_MIRROR);
  }

  bool idex_flash_is_copy_or_mirror(void)
  {
    return idex_flash_is_copy() || idex_flash_is_mirror();
  }

  void idex_flash_copy_sys_print_type(void)
  {
    flash_param_t.idex_print_type = ccm_param.t_sys.print_type_idex;
  }



}

#endif // HAS_PRINT_CONTROL





