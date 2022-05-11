#ifndef UTILS_H
#define UTILS_H

void utils_str_snprintf_path(TCHAR *path_dest, const uint8_t *src1, const uint8_t *src2);
void utils_str_memmove(TCHAR *dest, TCHAR *src);
void utils_str_strcat(TCHAR *dest, const TCHAR *src);
bool utils_str_strstr(TCHAR *str1, const TCHAR *src2);

#endif // UTILS_H

