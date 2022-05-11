/**
******************************************************************************
* @file    USB_Host/FWupgrade_Standalone/Src/iap_menu.c
* @author  MCD Application Team
* @version V1.3.4
* @date    06-May-2016
* @brief   COMMAND IAP Execute Application
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright ?2016 STMicroelectronics International N.V.
* All rights reserved.</center></h2>
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted, provided that the following conditions are met:
*
* 1. Redistribution of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. Neither the name of STMicroelectronics nor the names of other
*    contributors to this software may be used to endorse or promote products
*    derived from this software without specific written permission.
* 4. This software, including modifications and/or derivative works of this
*    software, must execute solely and exclusively on microcontroller or
*    microprocessor devices manufactured by or for STMicroelectronics.
* 5. Redistribution and use of this software other than as permitted under
*    this license is void and will automatically terminate your rights under
*    this license.
*
* THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
* RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
* SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "../Inc/iap_menu.h"
#include "../Inc/iap_config.h"
#include "../Inc/iap_param.h"
#include "user_common.h"

#if defined(USE_BOOT) || defined(USE_APP1)

#ifdef USE_BOOT
  #include "unpack.h"
#endif

#ifdef USE_APP1
  #include "lcd.h"
#endif

#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define BOOT_START_ADDRESS         ((uint32_t)0x08000000)
#define BOOT_END_ADDRESS           ((uint32_t)0x08007FFF)
#define BOOT_FLASH_SIZE            ((uint32_t)0x08008000)
#define APP1_START_ADDRESS         ((uint32_t)0x08008400)
#define APP1_END_ADDRESS           ((uint32_t)0x0801FFFF)
#define APP1_FLASH_SIZE            ((uint32_t)0x00017c00)
#define APP2_START_ADDRESS         ((uint32_t)0x08020000)
#define APP2_END_ADDRESS           ((uint32_t)0x080DFFFF)
#define APP2_FLASH_SIZE            ((uint32_t)0x000C0000)

#ifdef USE_APP1
  #define BOOT_FILE_PATH_NAME        (_T("1:/Bootloader.bin"))
  #define APP2_FILE_PATH_NAME        (_T("1:/APP2.bin"))

  extern uint8_t unpack_file_interface(void);
  extern void unpack_file_delete_function_file(void);
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t UploadCondition = 0x00;
DIR dir;
FILINFO fno;
#ifdef USE_APP1
  uint8_t sd_mount_one = 1;
#endif

extern ApplicationTypeDef Appli_state;
extern USBH_HandleTypeDef hUsbHostFS;

/* Private function prototypes -----------------------------------------------*/
static void USBH_USR_BufferSizeControl(void);

/* Private functions ---------------------------------------------------------*/
extern uint8_t USH_User_App(void);



static void NVIC_DeInit(void)
{
  uint32_t index = 0;
  NVIC->ICER[0] = 0xFFFFFFFF;
  NVIC->ICER[1] = 0x000007FF;
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICPR[1] = 0x000007FF;

  for (index = 0; index < 0x0B; index++)
  {
    NVIC->IP[index] = 0x00000000;
  }
}

static void Jump_To_App(__IO uint32_t app_address)
{
  #ifdef USE_BOOT
  USER_EchoLog("Bootloader ==>> jump to app %x\n", app_address);
  #endif
  #ifdef USE_APP1
  USER_EchoLog("APP1 ==>> jump to app %x\n", app_address);
  #endif
  HAL_Delay(1);
  pFunction Jump_To_Application;
  uint32_t JumpAddress;

  if (((*(__IO uint32_t *)app_address) &  0x2FFE0000) == 0x20000000)
  {
    NVIC_DisableIRQ(OTG_FS_IRQn);
    NVIC_DisableIRQ(USART1_IRQn);
    #ifdef USE_APP1
    NVIC_DisableIRQ(FSMC_IRQn);
    NVIC_DisableIRQ(SDIO_IRQn);
    NVIC_DisableIRQ(DMA2_Stream3_IRQn);
    NVIC_DisableIRQ(DMA2_Stream6_IRQn);
    #endif
    NVIC_DeInit();
    HAL_SuspendTick();
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t *)(app_address + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *) app_address);
    Jump_To_Application();
  }

  //  __set_FAULTMASK(1);               // 关闭中断
  /* Software reset */
  NVIC_SystemReset();
}

static bool udisk_mount_once(void)
{
  static uint8_t udisk_mount_one = 1;

  if (udisk_mount_one == 1)
  {
    if (!user_mount_Udisk())
    {
      /* FatFs initialization fails */
      FatFs_Fail_Handler();
      return false;
    }

    udisk_mount_one = 0;
  }

  return true;
}

static void write_bin_to_flash(TCHAR *_PathName, uint32_t _Address, uint32_t _LastPageAddress, uint32_t _PageSize)
{
  USER_EchoLog("write flash:%s\n", _PathName);
  HAL_FLASH_Unlock();
  /* 在对 flash 连续写的过程，ECC 纠错检查会开启。会触发ECCD错误，（在 __HAL_FLASH_GET_FLAG(FLASH_FLAG_ECCD) 时返回错误），需要清除FLASH标志位*/
  /* 清除所有挂起标志位 */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
                         FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
  COMMAND_Download(_PathName, _Address, _LastPageAddress, _PageSize);
  HAL_FLASH_Lock();
}

#ifdef USE_APP1
static void go_to_app(void)
{
  APP_LCD_ShowString((uint8_t *)"go to app", 0);
  LCD_Clear(BLACK);
  LCD_ShowString(150, 140, 210, 24, 24, (uint8_t *)"Welcome...");
  Jump_To_App(APP2_START_ADDRESS);  // 进入APP1，PC指针 = APPLICATION_ADDRESS + 4
}

static bool sd_mount_once(void)
{
  //if(!SDCard_Mount())
  //  {
  //    /* FatFs initialization fails */
  //    FatFs_Fail_Handler();
  //    return false;
  //  }
  FIL Rfile;

  if (f_open(&Rfile, "1:/APP2.bin", FA_READ) == FR_OK || f_open(&Rfile, "1:/APP2.BIN", FA_READ) == FR_OK)
  {
    (void)f_close(&Rfile);
    LCD_Clear(BLACK);
    APP_LCD_ShowString((uint8_t *)"Start write flash", 0);
    APP_LCD_ShowString((uint8_t *)"...", 1);
    HAL_FLASH_Unlock();
    COMMAND_Download(APP2_FILE_PATH_NAME, (uint32_t)APP2_START_ADDRESS, (uint32_t)APP2_END_ADDRESS, (uint32_t)APP2_FLASH_SIZE);
    HAL_FLASH_Lock();
    f_unlink("1:/APP2.bin");
    go_to_app();
  }

  return true;
}
#endif //#ifdef USE_APP1

static void write_flash_memory(void)
{
  #if defined(USE_BOOT)
  USER_EchoLog("Bootloader ==>> flash app1\n");
  /* Writes Flash memory */
  write_bin_to_flash(BOOTLOADER_APP1_FILE_PATH_NAME, (uint32_t)APP1_START_ADDRESS, (uint32_t)APP1_END_ADDRESS, (uint32_t)APP1_FLASH_SIZE);
  f_unlink(BOOTLOADER_APP1_FILE_PATH_NAME);
  Jump_To_App(APP1_START_ADDRESS);  // 进入APP1，PC指针 = APPLICATION_ADDRESS + 4
  #elif defined(USE_APP1)
  USER_EchoLog("APP1 ==>> flash app2\n");
  APP_LCD_ShowString((uint8_t *)"Start write flash", 0);
  APP_LCD_ShowString((uint8_t *)"...", 1);
  write_bin_to_flash(APP2_FILE_PATH_NAME, (uint32_t)APP2_START_ADDRESS, (uint32_t)APP2_END_ADDRESS, (uint32_t)APP2_FLASH_SIZE);

  if (isUpdateBootloaderVersion)
  {
    APP_LCD_ShowString((uint8_t *)"Start write boot flash", 0);
    APP_LCD_ShowString((uint8_t *)"...", 1);
    write_bin_to_flash(BOOT_FILE_PATH_NAME, (uint32_t)BOOT_START_ADDRESS, (uint32_t)BOOT_END_ADDRESS, (uint32_t)BOOT_FLASH_SIZE);
  }

  printf("Start write flash finish\n");
  unpack_file_delete_function_file();
  go_to_app();
  #endif
}

/**
  * @brief  Demo application for IAP through USB mass storage.
  * @param  None
  * @retval None
  */
void FW_UPGRADE_Process(void)
{
  static unsigned long disconnect_timeout = 0;
  static bool is_set_disconnect_timeout = true;

  if (is_set_disconnect_timeout)
  {
    disconnect_timeout = HAL_GetTick() + 2000;
    is_set_disconnect_timeout = false;
  }

  #ifdef USE_APP1

  if (sd_mount_one == 1)
  {
    sd_mount_once();
    sd_mount_one = 0;
  }

  #endif

  // USB空载，跳转APP
  if (Appli_state == APPLICATION_IDLE || Appli_state == APPLICATION_DISCONNECT)
  {
    if (disconnect_timeout < HAL_GetTick() || Appli_state == APPLICATION_DISCONNECT)
    {
      #if defined(USE_BOOT)
      USER_EchoLog("Bootloader ==>> usb not connect\n");
      Jump_To_App(APP2_START_ADDRESS);
      #elif defined(USE_APP1)
      USER_EchoLog("APP1 ==>> usb not connect\n");
      Jump_To_App(APP2_START_ADDRESS);
      #endif
      return;
    }
  }

  if (Appli_state != APPLICATION_READY)
    return;

  /* Register the file system object to the FatFs module */
  /* 挂载U盘 */
  if (!udisk_mount_once())
    return;

  if (USBH_MSC_IsReady(&hUsbHostFS))
  {
    /* Control BUFFER_SIZE value */
    USBH_USR_BufferSizeControl();
    #if defined(USE_BOOT)
    USER_EchoLog("Bootloader ==>> unzip firmware\n");

    if (USH_User_App() == (uint8_t) - 1)
    {
      Jump_To_App(APP1_START_ADDRESS);
      //      Fail_Handler();
      return;
    }

    #elif defined(USE_APP1)
    USER_EchoLog("APP1 ==>> unzip firmware\n");

    if (!unpack_file_interface())
    {
      Jump_To_App(APP2_START_ADDRESS);
      Fail_Handler();
      return;
    }

    #endif
    /* Writes Flash memory */
    write_flash_memory();
  }
}
#ifdef USE_APP1
static void LCD_SHOW_WARNING(const char *warningStr)
{
  APP_LCD_Clear(BLACK);
  APP_LCD_ShowString((uint8_t *)warningStr, 0);
  HAL_Delay(3000);
}
#endif
/**
  * @brief  Controls Buffer size value.
  * @param  None
  * @retval None
  */
static void USBH_USR_BufferSizeControl(void)
{
  /* Control BUFFER_SIZE and limit this value to 32Kbyte maximum */
  if ((IAP_BUFFER_SIZE % 4 != 0x00) || (IAP_BUFFER_SIZE / 4 > 8192))
  {
    #ifdef USE_APP1
    LCD_SHOW_WARNING("Controls Buffer Size Fail!");
    #endif
    USER_ErrLog("USBH_USR_BufferSizeControl\n");
    HAL_Delay(1000);
    Jump_To_App(APP2_START_ADDRESS);

    while (1)
    {
    }
  }
}
#ifdef USE_APP1
void APP_LCD_StartInit(void)
{
  LCD_Init();
  HAL_Delay(50);
  POINT_COLOR = (uint16_t)WHITE;
  BACK_COLOR = (uint16_t)BLACK;
}

unsigned int TextPosition_y = 20;
void APP_LCD_ShowString(uint8_t *p, uint8_t isNextLine)
{
  if (isNextLine) TextPosition_y += 30;

  LCD_ShowString(80, TextPosition_y, 320, 24, 24, p);
  USER_DbgLog("%s", p);
}

void APP_LCD_Clear(uint16_t color)
{
  LCD_Clear(color);
  TextPosition_y = 20;
}
#endif

#endif

/**
  * @brief  Handles the program fail.
  * @param  None
  * @retval None
  */
void Fail_Handler(void)
{
  #if defined(USE_BOOT) || defined(USE_APP1)
  #ifdef USE_APP1
  LCD_SHOW_WARNING("Program Fail!");
  #endif
  USER_ErrLog("Fail_Handler\n");
  HAL_Delay(1000);
  Jump_To_App(APP2_START_ADDRESS);
  #endif

  while (1)
  {
  }
}

/**
  * @brief  Handles the Flash Erase fail.
  * @param  None
  * @retval None
  */
void Erase_Fail_Handler(void)
{
  #if defined(USE_BOOT) || defined(USE_APP1)
  #ifdef USE_APP1
  LCD_SHOW_WARNING("Flash Erase Fail!");
  #endif
  USER_ErrLog("Erase_Fail_Handler\n");
  HAL_Delay(1000);
  Jump_To_App(APP2_START_ADDRESS);
  #endif

  while (1)
  {
  }
}

/**
  * @brief  Handles the FatFs fail.
  * @param  None
  * @retval None
  */
void FatFs_Fail_Handler(void)
{
  #if defined(USE_BOOT) || defined(USE_APP1)
  #ifdef USE_APP1
  LCD_SHOW_WARNING("FatFs Fail!");
  #endif
  USER_ErrLog("FatFs_Fail_Handler\n");
  HAL_Delay(1000);
  Jump_To_App(APP2_START_ADDRESS);
  #endif

  while (1)
  {
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
