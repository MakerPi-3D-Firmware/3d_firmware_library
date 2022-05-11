#ifndef FILAMENT_H
#define FILAMENT_H

#ifdef HAS_FILAMENT

#ifdef __cplusplus
extern "C" {
#endif
namespace feature_filament
{
  extern void f_init(void);
  extern void f_process(void);
}
#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // FILAMENT_H

