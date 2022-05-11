#ifndef IAP_CONFIG_H
#define IAP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* This value can be equal to (512 * x) according to RAM size availability with x=[1, 128]
  In this project x is fixed to 64 => 512 * 64 = 32768bytes = 32 Kbytes */
#define IAP_BUFFER_SIZE        ((uint16_t)512*64)

#if defined(USE_BOOT) || defined(USE_APP1)

#define BOOTLOADER_VERSION                    ((char*)"V1.1")                // Bootloader 版本
#define BOOTLOADER_VERSION_FIND_STR           ((char*)"CF0:V1.1")            // Bootloader 版本
#define BOOTLOADER_UNPACK_FILE_PATH_NAME      ((char*)"0:/Update.bin")
#define BOOTLOADER_UNPACK_RE_FILE_PATH_NAME   ((char*)"0:/UpdateFW.bin")
#define BOOTLOADER_APP1_FILE_PATH_NAME        ((char*)"0:/APP1.bin")
#define BOOTLOADER_INFO_FILE_PATH_NAME        ((char*)"0:/Info.txt")
#define BOOTLOADER_UNPACK_RE_FILENAME         ((char*)"UpdateFW.bin")
#define BOOTLOADER_UNPACK_FILENAME            ((char*)"Update.bin")
#define BOOTLOADER_APP1_FILE_NAME             ((char*)"APP1.bin")

#define UNPACK_FILE_PATH_NAME        ((char*)"0:/UpdateFW.bin")
#define UNPACK_RE_FILE_PATH_NAME     ((char*)"0:/Update.bin")
#define INFO_FILE_PATH_NAME          ((char*)"0:/Info.txt")
#define UNPACK_RE_FILENAME           ((char*)"Update.bin")
#define UNPACK_UPDATE_FILENAME       ((char*)"Update_Back.bin")

#endif

#ifdef __cplusplus
} // extern "C" {
#endif

#endif //IAP_CONFIG_H


