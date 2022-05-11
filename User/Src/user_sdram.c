#include "user_common.h"

#ifdef ENABLE_SDRAM

extern SDRAM_HandleTypeDef hsdram1;

SDRAM_PARAM sdram_param __attribute__((at(LCD_FRAME_BUF_ADDR)));

#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) //SDRAM开始地址

//SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((u16)0x0200)

#if 0
//SDRAM底层驱动，引脚配置，时钟使能
//此函数会被HAL_SDRAM_Init()调用
//hsdram:SDRAM句柄
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_FMC_CLK_ENABLE();                 //使能FMC时钟
  __HAL_RCC_GPIOC_CLK_ENABLE();               //使能GPIOC时钟
  __HAL_RCC_GPIOD_CLK_ENABLE();               //使能GPIOD时钟
  __HAL_RCC_GPIOE_CLK_ENABLE();               //使能GPIOE时钟
  __HAL_RCC_GPIOF_CLK_ENABLE();               //使能GPIOF时钟
  __HAL_RCC_GPIOG_CLK_ENABLE();               //使能GPIOG时钟
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //推挽复用
  GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
  GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC
  HAL_GPIO_Init(GPIOC, &GPIO_Initure);         //初始化PC0,2,3
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &GPIO_Initure);    //初始化PD0,1,8,9,10,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);    //初始化PE0,1,7,8,9,10,11,12,13,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);    //初始化PF0,1,2,3,4,5,11,12,13,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOG, &GPIO_Initure);     //初始化PG0,1,2,4,5,8,15
}
#endif

//向SDRAM发送命令
//bankx:0,向BANK5上面的SDRAM发送指令
//      1,向BANK6上面的SDRAM发送指令
//cmd:指令(0,正常模式/1,时钟配置使能/2,预充电所有存储区/3,自动刷新/4,加载模式寄存器/5,自刷新/6,掉电)
//refresh:自刷新次数
//regval:模式寄存器的定义
//返回值:0,正常;1,失败.
u8 user_fmc_sdram_send_cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval)
{
  u32 target_bank = 0;
  FMC_SDRAM_CommandTypeDef Command;

  if (bankx == 0) target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
  else if (bankx == 1) target_bank = FMC_SDRAM_CMD_TARGET_BANK2;

  Command.CommandMode = cmd;              //命令
  Command.CommandTarget = target_bank;    //目标SDRAM存储区域
  Command.AutoRefreshNumber = refresh;    //自刷新次数
  Command.ModeRegisterDefinition = regval; //要写入模式寄存器的值

  if (HAL_SDRAM_SendCommand(&hsdram1, &Command, 0X1000) == HAL_OK) //向SDRAM发送命令
  {
    return 0;
  }
  else return 1;
}

//在指定地址(WriteAddr+Bank5_SDRAM_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void user_fmc_sdram_write_buffer(u8 *pBuffer, u32 WriteAddr, u32 n)
{
  for (; n != 0; n--)
  {
    *(vu8 *)(Bank5_SDRAM_ADDR + WriteAddr) = *pBuffer;
    WriteAddr++;
    pBuffer++;
  }
}

//在指定地址((WriteAddr+Bank5_SDRAM_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void user_fmc_sdram_read_buffer(u8 *pBuffer, u32 ReadAddr, u32 n)
{
  for (; n != 0; n--)
  {
    *pBuffer++ = *(vu8 *)(Bank5_SDRAM_ADDR + ReadAddr);
    ReadAddr++;
  }
}

//发送SDRAM初始化序列
static void _user_fmc_sdram_initialization_sequence(SDRAM_HandleTypeDef *hsdram)
{
  u32 temp = 0;
  //SDRAM控制器初始化完成以后还需要按照如下顺序初始化SDRAM
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0); //时钟配置使能
  user_delay_us(500);                                  //至少延时200us
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);    //对所有存储区预充电
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0); //设置自刷新次数
  //配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
  //bit3为指定突发访问的类型，bit4~bit6为CAS值，bit7和bit8为运行模式
  //bit9为指定的写突发模式，bit10和bit11位保留位
  temp = (u32)SDRAM_MODEREG_BURST_LENGTH_1          | //设置突发长度:1(可以是1/2/4/8)
         SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   | //设置突发类型:连续(可以是连续/交错)
         SDRAM_MODEREG_CAS_LATENCY_3           | //设置CAS值:3(可以是2/3)
         SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //设置操作模式:0,标准模式
         SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //设置突发写模式:1,单点访问
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp); //设置SDRAM的模式寄存器
  //刷新频率计数器(以SDCLK频率计数),计算方法:
  //COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
  //我们使用的SDRAM刷新周期为64ms,SDCLK=180/2=90Mhz,行数为8192(2^13).
  //所以,COUNT=64*1000*90/8192-20=683
  HAL_SDRAM_ProgramRefreshRate(&hsdram1, 761);
}

void user_fmc_sdram_init(void)
{
#if 0
  FMC_SDRAM_TimingTypeDef SDRAM_Timing;
  hsdram1.Instance = FMC_SDRAM_DEVICE;                           //SDRAM在BANK5,6
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;                         //SDRAM接在BANK5上
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;   //列数量
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;        //行数量
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;     //数据宽度为16位
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4; //一共4个BANK
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;             //CAS为3
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE; //失能写保护
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;         //SDRAM时钟为HCLK/2=180M/2=90M=11.1ns
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;              //使能突发
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;          //读通道延时
  SDRAM_Timing.LoadToActiveDelay = 2;                                 //加载模式寄存器到激活时间的延迟为2个时钟周期
  SDRAM_Timing.ExitSelfRefreshDelay = 8;                              //退出自刷新延迟为8个时钟周期
  SDRAM_Timing.SelfRefreshTime = 6;                                   //自刷新时间为6个时钟周期
  SDRAM_Timing.RowCycleDelay = 6;                                     //行循环延迟为6个时钟周期
  SDRAM_Timing.WriteRecoveryTime = 2;                                 //恢复延迟为2个时钟周期
  SDRAM_Timing.RPDelay = 2;                                           //行预充电延迟为2个时钟周期
  SDRAM_Timing.RCDDelay = 2;                                          //行到列延迟为2个时钟周期
  HAL_SDRAM_Init(&hsdram1, &SDRAM_Timing);
#endif
  _user_fmc_sdram_initialization_sequence(&hsdram1);//发送SDRAM初始化序列
}

#endif // ENABLE_SDRAM





