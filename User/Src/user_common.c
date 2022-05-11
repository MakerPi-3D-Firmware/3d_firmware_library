#include "user_common.h"
#include "main.h"


extern void sysconfig_init(void);

#if defined(USE_APP1) || defined(USE_APP2)
static void user_NVIC_SetVectorTable(uint32_t Offset)
{
  /* Check the parameters */
  assert_param((Offset) < 0x000FFFF);
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | (Offset & (uint32_t)0x1FFFFF80);
#else
  SCB->VTOR = FLASH_BASE | (Offset & (uint32_t)0x1FFFFF80);
#endif
}
#endif

void user_init0(void)
{
#if defined(USE_APP1)
  //重设中断向量表  33k:0x08008400  128k-33k=95K:0x17C00   0x8000000  0x100000
  user_NVIC_SetVectorTable(0x00008400);
  __enable_irq(); // 打开中断
#elif defined(USE_APP2)
  //执行升级程序后所需的操作
  user_NVIC_SetVectorTable(0x00020000);
  __enable_irq(); // 打开中断
#endif
  user_get_mcu_id();
}

#if defined(USE_BOOT)
void user_init_boot(void)
{
  user_print_mcu_info();
  user_board_init();
  USER_EchoLog("Bootloader ==>> start");
}
#endif

#if defined(USE_APP1)
void user_init_app1(void)
{
  user_board_init();
  delay_init(168);
  u8 res = 0;
  USER_EchoLog("APP1 ==>> start");
  SDRAM_Init();
  APP_LCD_StartInit(); //LCD初始化
  APP_LCD_Clear(BLACK);
  HAL_Delay(100);
  LCD_LED = 1;      //点亮背光
  //  user_ftl_init();
  //  exfuns_init();
  res = f_mount(&USERFatFS, "1:", 1);     //挂载NAND FLASH.
  printf("res ..%d\n", res);

  if (res == 0X0D) //NAND FLASH磁盘,FAT文件系统错误,重新格式化NAND FLASH
  {
    BYTE work[_MAX_SS]; /* Work area (larger is better for processing time) */
    res = f_mkfs("1:", 1, 0, work, sizeof(work)); //格式化FLASH,2,盘符;1,不需要引导区,8个扇区为1个簇
    delay_ms(1000);
  }
}
#endif


#if defined(USE_APP2)
void user_init_app2(void)
{
  user_board_init();
  user_ccm_param_init();
  user_os_init();
  user_delay_init(168);
  user_iwdg_init();
#ifdef ENABLE_UART1_DMA
  user_uart1_dma_start();
#endif
  user_fan_control_init();
  user_led_control_init();
  user_buzzer_init();
  user_fmc_sdram_init();
  user_nand_mount();
#ifdef ENABLE_DMA2D
  user_dma2d_init();
#endif
  LCD_Init();//触摸屏初始化
#ifdef ENABLE_GUI_LVGL
  user_lvgl_init();
#endif
  // 加载系统配置文件
  sysconfig_init();
  flash_config_read();
  feature_pow_data_init();
}
#endif

void user_init(void)
{
#if defined(USE_BOOT)
  user_init_boot();
#elif defined(USE_APP1)
  user_init_app1();
#elif defined(USE_APP2)
  user_init_app2();
#endif
}




