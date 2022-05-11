#include <wchar.h>
#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

void utils_str_snprintf_path(TCHAR *path_dest, const uint8_t *src1, const uint8_t *src2)
{
#if _LFN_UNICODE
  int length = strlen((char *)src1) + strlen((char *)src2);
  (void)swprintf((wchar_t *)path_dest, length, _T("%s%s"), src1, src2);
  path_dest[length + 1] = 0;
#else
  int length = strlen((char *)src1) + strlen((char *)src2);
  (void)snprintf((char *)path_dest, strlen((char *)src1) + strlen((char *)src2), "%s%s", src1, src2);
  path_dest[length + 1] = 0;
#endif
}

void utils_str_memmove(TCHAR *dest, const TCHAR *src)
{
#if _LFN_UNICODE
  int length =  wcslen((wchar_t *)src);
  wmemmove((wchar_t *)dest, (wchar_t *)src, length);
  dest[length] = 0;
#else
  int length =  strlen((char *)src);
  memmove((char *)dest, (char *)src, length);
  dest[length] = 0;
#endif
}

void utils_str_strcat(TCHAR *dest, const TCHAR *src)
{
#if _LFN_UNICODE
  wcscat((wchar_t *)dest, (wchar_t *)src);
#else
  strcat((char *)dest, (char *)src);
#endif
}

bool utils_str_strstr(TCHAR *str1, const TCHAR *src2)
{
#if _LFN_UNICODE
  return wcsstr((wchar_t *)str1, (wchar_t *)src2) != NULL;
#else
  return strstr((char *)str1, (char *)src2) != NULL;
#endif
}


#ifdef __cplusplus
} //extern "C" {
#endif

