#include "user_common.h"

volatile uint32_t mcu_id = MCU_NOT_SUPPORT;

static uint32_t mcu_cpu_id = 0U;
static uint16_t mcu_rev_id = 0U;
static uint16_t mcu_dev_id = 0U;

/*定义STM32 MCU的类型*/
typedef enum
{
  MCU_STM32F0,
  MCU_STM32F1,
  MCU_STM32F2,
  MCU_STM32F3,
  MCU_STM32F4,
  MCU_STM32F7,
  MCU_STM32L0,
  MCU_STM32L1,
  MCU_STM32L4,
  MCU_STM32H7,
  MCU_GD32F450,
} MCUTypedef;


static const uint32_t unique_id_addr[] = {0x1FFFF7AC,  /*STM32F0唯一ID起始地址*/
                                          0x1FFFF7E8,  /*STM32F1唯一ID起始地址*/
                                          0x1FFF7A10,  /*STM32F2唯一ID起始地址*/
                                          0x1FFFF7AC,  /*STM32F3唯一ID起始地址*/
                                          0x1FFF7A10,  /*STM32F4唯一ID起始地址*/
                                          0x1FF0F420,  /*STM32F7唯一ID起始地址*/
                                          0x1FF80050,  /*STM32L0唯一ID起始地址*/
                                          0x1FF80050,  /*STM32L1唯一ID起始地址*/
                                          0x1FFF7590,  /*STM32L4唯一ID起始地址*/
                                          0x1FF0F420,  /*STM32H7唯一ID起始地址*/
                                          0x1FFF7A10   /*GD32F4XX唯一ID起始地址*/
                                         };


static const uint32_t flash_addr[] = {0x1FFFF7CC,               /*STM32F0 FLASH 起始地址*/
                                      0x1FFFF7E0,               /*STM32F1 FLASH 起始地址*/
                                      0x1FFF7A22,               /*STM32F2 FLASH 起始地址*/
                                      0x1FFFF7CC,               /*STM32F3 FLASH 起始地址*/
                                      0x1FFF7A22,               /*STM32F4 FLASH 起始地址*/
                                      0x1FF0F442,               /*STM32F7 FLASH 起始地址*/
                                      0x1FF8007C,               /*STM32L0 FLASH 起始地址*/
                                      0x1FF8004C, //0x1FF800CC  /*STM32L1 FLASH 起始地址*/
                                      0x1FFF75E0,               /*STM32L4 FLASH 起始地址*/
                                      0x1FF0F442,               /*STM32H7 FLASH 起始地址*/
                                      0x1FFF7A20                /*GD32F4XX FLASH 起始地址*/
                                     };

/*获取MCU的唯一ID*/
static void get_mcu_id(uint32_t *id, MCUTypedef type)
{
  if (id != NULL)
  {
    id[0] = *(volatile uint32_t *)(unique_id_addr[type]);
    id[1] = *(volatile uint32_t *)(unique_id_addr[type] + 4);
    id[2] = *(volatile uint32_t *)(unique_id_addr[type] + 8);
  }
}


static uint16_t get_jtag_id()
{
  if (*(volatile uint8_t *)(0xE00FFFE8) & 0x08)
  {
    return ((*(volatile uint8_t *)(0xE00FFFD0) & 0x0F) << 8) |
           ((*(volatile uint8_t *)(0xE00FFFE4) & 0xFF) >> 3) |
           ((*(volatile uint8_t *)(0xE00FFFE8) & 0x07) << 5) + 1 ;
  }

  return 0;
}


void user_get_mcu_id(void)
{
  mcu_cpu_id = SCB->CPUID;
  mcu_rev_id = DBGMCU->IDCODE >> 16;
  mcu_dev_id = DBGMCU->IDCODE & 0x0FFF;

  if (get_jtag_id() == 0x07A3) //GD32
  {
    if (mcu_cpu_id == 0x410FC241 && mcu_rev_id == 0x1608 && mcu_dev_id == 0x0419) //GD32F450IIH6
    {
      mcu_id = MCU_GD32F450IIH6;
    }
    else if (mcu_cpu_id == 0x410FC241 && mcu_rev_id == 0x2104 && mcu_dev_id == 0x0414) //GD32F303RET6
    {
      mcu_id = MCU_GD32F303RET6;
    }
    else if (mcu_cpu_id == 0x410FC241 && mcu_rev_id == 0x1701 && mcu_dev_id == 0x0414) //GD32F303RCT6
    {
      mcu_id = MCU_GD32F303RCT6;
    }
    else
    {
      mcu_id = MCU_NOT_SUPPORT;
    }
  }
  else if (get_jtag_id() == 0x0041) //STM32
  {
    if (mcu_cpu_id == 0x410FC241 && mcu_rev_id == 0x2003 && mcu_dev_id == 0x0419) //STM32F429IGT6
    {
      mcu_id = MCU_STM32F429IGT6;
    }
    else if (mcu_cpu_id == 0x411FC231 && mcu_rev_id == 0x1003 && mcu_dev_id == 0x0414) //STM32F103RCT6
    {
      mcu_id = MCU_STM32F103RCT6;
    }
    else if (mcu_cpu_id == 0x410FC241 && mcu_rev_id == 0x1007 && mcu_dev_id == 0x0413) //STM32F407VGT6
    {
      mcu_id = MCU_STM32F407VGT6;
    }
    else
    {
      mcu_id = MCU_NOT_SUPPORT;
    }
  }
  else
  {
    mcu_id = MCU_NOT_SUPPORT;
  }
}

void user_print_mcu_info(void)
{
  MCUTypedef curr_mcu = MCU_GD32F450;
  uint32_t mcu_id_buf[3];

  if (mcu_id == MCU_GD32F450IIH6)
  {
    curr_mcu = MCU_GD32F450;
    USER_EchoLog("Current MCU is GD32F450IIH6");
  }
  else if (mcu_id == MCU_STM32F429IGT6)
  {
    curr_mcu = MCU_STM32F4;
    USER_EchoLog("Current MCU is STM32F429IGT6");
  }
  else if (mcu_id == MCU_STM32F407VGT6)
  {
    curr_mcu = MCU_STM32F4;
    USER_EchoLog("Current MCU is STM32F407VGT6");
  }

  if (get_jtag_id() == 0x07A3) //GD32
  {
    uint16_t Sram_Size = *(volatile uint16_t *)(flash_addr[curr_mcu]);
    uint16_t Flash_Size = *(volatile uint16_t *)(flash_addr[curr_mcu] + 2U);
    get_mcu_id(&mcu_id_buf[0], curr_mcu);
    USER_EchoLog("Cpu ID: %X; Dev ID: %X; REV ID: %X;", mcu_cpu_id, mcu_dev_id, mcu_rev_id);
    USER_EchoLog("Unique ID: %X %X %X", mcu_id_buf[2], mcu_id_buf[1], mcu_id_buf[0]);
    USER_EchoLog("Flash_Size: %dKB, Sram Size: %dKB", Flash_Size, Sram_Size);
    USER_EchoLog("Buid Date: %s, %s", __DATE__, __TIME__);
  }
  else if (get_jtag_id() == 0x0041)  //STM32
  {
    uint16_t Flash_Size = *(volatile uint16_t *)(flash_addr[curr_mcu]);
    get_mcu_id(&mcu_id_buf[0], curr_mcu);
    USER_EchoLog("Cpu ID: %X; Dev ID: %X; REV ID: %X;", mcu_cpu_id, mcu_dev_id, mcu_rev_id);
    USER_EchoLog("Unique ID: %X %X %X", mcu_id_buf[2], mcu_id_buf[1], mcu_id_buf[0]);
    USER_EchoLog("Flash_Size: %dKB", Flash_Size);
    USER_EchoLog("Buid Date: %s, %s", __DATE__, __TIME__);
  }
}

