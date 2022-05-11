#include "user_common.h"

#if defined(ENABLE_FSMC)

#ifdef HAL_SRAM_MODULE_ENABLED

#if 0

extern SRAM_HandleTypeDef hsram1;    //SRAM句柄(用于控制LCD)

//SRAM底层驱动，时钟使能，引脚分配
//此函数会被HAL_SRAM_Init()调用
//hsram:SRAM句柄
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_FMC_CLK_ENABLE();       //使能FMC时钟
  __HAL_RCC_GPIOD_CLK_ENABLE();     //使能GPIOD时钟
  __HAL_RCC_GPIOE_CLK_ENABLE();     //使能GPIOE时钟
  //初始化PD0,1,4,5,7,8,9,10,13,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | \
                     GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_Initure.Mode = GPIO_MODE_AF_PP;  //复用
  GPIO_Initure.Pull = GPIO_PULLUP;    //上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH; //高速
  GPIO_Initure.Alternate = GPIO_AF12_FMC; //复用为FMC
  HAL_GPIO_Init(GPIOD, &GPIO_Initure);
  //初始化PE7,8,9,10,11,12,13,14,15
  GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                     GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);
}

void user_fsmc_init(void)
{
  GPIO_InitTypeDef GPIO_Initure;
  FMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
  FMC_NORSRAM_TimingTypeDef FSMC_WriteTim;
  __HAL_RCC_GPIOB_CLK_ENABLE();     //开启GPIOB时钟
  GPIO_Initure.Pin = GPIO_PIN_5;          //PB5,背光控制
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出
  GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
  HAL_GPIO_Init(GPIOB, &GPIO_Initure);
  hsram1.Instance = FMC_NORSRAM_DEVICE;      //BANK1
  hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  hsram1.Init.NSBank = FMC_NORSRAM_BANK1;   //使用NE1
  hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE; //不复用数据线
  hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;            //SRAM
  hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16; //16位数据宽度
  hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE; //是否使能突发访问,仅对同步突发存储器有效,此处未用到
  hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW; //等待信号的极性,仅在突发模式访问下有用
  hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS; //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
  hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;  //存储器写使能
  hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;         //等待使能位,此处未用到
  hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;      //读写使用不同的时序
  hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE; //是否使能同步传输模式下的等待信号,此处未用到
  hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;         //禁止突发写
  hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
  //FSMC读时序控制寄存器
  FSMC_ReadWriteTim.AddressSetupTime = 0x0F;      //地址建立时间(ADDSET)为15个HCLK 1/180M*15=5.5ns*15=82.5ns
  FSMC_ReadWriteTim.AddressHoldTime = 0x00;
  FSMC_ReadWriteTim.DataSetupTime = 0x46;         //数据保存时间(DATAST)为70个HCLK  =5.5*70=385ns
  FSMC_ReadWriteTim.AccessMode = FMC_ACCESS_MODE_A; //模式A
  //FSMC写时序控制寄存器
  FSMC_WriteTim.AddressSetupTime = 0x0F;          //地址建立时间(ADDSET)为15个HCLK=82.5ns
  FSMC_WriteTim.AddressHoldTime = 0x00;
  FSMC_WriteTim.DataSetupTime = 0x0F;             //数据保存时间(DATAST)为5.5ns*15个HCLK=82.5ns
  FSMC_WriteTim.AccessMode = FMC_ACCESS_MODE_A;   //模式A
  HAL_SRAM_Init(&hsram1, &FSMC_ReadWriteTim, &FSMC_WriteTim);
}
#endif // #if 0
#endif // #ifdef HAL_SRAM_MODULE_ENABLED
#endif // #if defined(ENABLE_FSMC)

