/**
  ******************************************************************************
  * @file    USB_Host/FWupgrade_Standalone/Src/command.c
  * @author  MCD Application Team
  * @version V1.4.4
  * @date    06-May-2016
  * @brief   This file provides all the IAP command functions.
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
#include "usb_host.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t TmpProgramCounter = 0x00;
static uint32_t TmpReadSize = 0x00;
static uint32_t RamAddress = 0x00;
static __IO uint32_t LastPGAddress = APPLICATION_ADDRESS;
static uint8_t RAM_Buf[IAP_BUFFER_SIZE] = {0x00};

static FIL MyFileR;  /* File object for download operation */
extern DIR dir;
extern FILINFO fno;

/* Private function prototypes -----------------------------------------------*/
static void COMMAND_ProgramFlashMemory(uint32_t Address, uint32_t _LastPageAddress);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  IAP Write Flash memory.
  * @param  None
  * @retval None
  */
void COMMAND_Download(TCHAR *_PathName, uint32_t _Address, uint32_t _LastPageAddress, uint32_t _PageSize)
{
  /* Open the binary file to be downloaded */
  if (f_open(&MyFileR, _PathName, FA_READ) == FR_OK)
  {
    if (MyFileR.obj.objsize > _PageSize)
    {
      Fail_Handler();
    }
    else
    {
      /* Erase FLASH sectors to download image */
      if (FLASH_If_EraseSectors(_Address, _LastPageAddress) != 0x00)
      {
        Erase_Fail_Handler();
      }

      /* Program flash memory */
      COMMAND_ProgramFlashMemory(_Address, _LastPageAddress);
      /* Close file */
      f_close(&MyFileR);
    }
  }
  else
  {
    Fail_Handler();
  }
}

/**
  * @brief  Programs the internal Flash memory.
  * @param  None
  * @retval None
  */
static void COMMAND_ProgramFlashMemory(uint32_t Address, uint32_t _LastPageAddress)
{
  uint32_t programcounter = 0x00;
  uint8_t readflag = TRUE;
  uint16_t bytesread;
  /* RAM Address Initialization */
  RamAddress = (uint32_t) &RAM_Buf;
  /* Erase address init */
  LastPGAddress = Address;

  /* While file still contain data */
  while (readflag == TRUE)
  {
    /* Read maximum 512 Kbyte from the selected file */
    f_read(&MyFileR, RAM_Buf, IAP_BUFFER_SIZE, (void *)&bytesread);
    /* Temp variable */
    TmpReadSize = bytesread;

    /* The read data < "BUFFER_SIZE" Kbyte */
    if (TmpReadSize < IAP_BUFFER_SIZE)
    {
      readflag = FALSE;
    }

    /* Program flash memory */
    for (programcounter = TmpReadSize; programcounter != 0; programcounter -= 4)
    {
      TmpProgramCounter = programcounter;

      /* Write word into flash memory */
      if (FLASH_If_Write((LastPGAddress - TmpProgramCounter + TmpReadSize), \
                         * (uint32_t *)(RamAddress - programcounter + TmpReadSize), _LastPageAddress) != 0x00)
      {
        /* Flash programming error */
        Fail_Handler();
      }
    }

    /* Update last programmed address value */
    LastPGAddress = LastPGAddress + TmpReadSize;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
