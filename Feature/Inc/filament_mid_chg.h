#ifndef FILAMENT_MID_CHG_H
#define FILAMENT_MID_CHG_H

#ifdef HAS_FILAMENT

#ifdef __cplusplus
extern "C" {
#endif
namespace feature_filament
{
  extern void mid_chg_confirm_load(void);
  extern void mid_chg_refresh_status(void);
  extern void mid_chg_start(void);
}
#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // FILAMENT_MID_CHG_H

