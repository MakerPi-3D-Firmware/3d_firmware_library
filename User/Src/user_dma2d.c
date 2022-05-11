#include "user_common.h"

#ifdef ENABLE_DMA2D

extern DMA2D_HandleTypeDef hdma2d;

#ifdef ENABLE_GUI_LVGL
static void user_dma2d_callback(DMA2D_HandleTypeDef *hdma2d)
{
  lv_port_disp_ready();
}
#endif

void user_dma2d_init(void)
{
  HAL_DMA2D_DeInit(&hdma2d);
  hdma2d.Instance = DMA2D;
#ifdef ENABLE_GUI_LVGL
  hdma2d.XferCpltCallback = user_dma2d_callback;
#endif
  hdma2d.Init.Mode = DMA2D_R2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hdma2d.Init.OutputOffset = 0;

  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
}

/*
 ******************************************************************************************************
 *    函 数 名: user_dma2d_fill
 *    功能说明: DMA2D颜色填充功能
 *    形    参: color_p        颜色数据源地址
 *              p_dst          颜色数据目的地址
 *              x_size         色块X轴大小，即每行像素数
 *              y_size         色块Y轴大小，即行数
 *              offset_line    前景层图像的行偏移
 *              color_index    色块颜色值
 *              pixel_format   目标区颜色格式
 *    返 回 值: 无
 ******************************************************************************************************
 */
static void user_dma2d_fill_basic(void *color_p,
                                  void *p_dst,
                                  uint32_t mode,
                                  uint32_t x_size,
                                  uint32_t y_size,
                                  uint32_t offset_line,
                                  uint32_t color_index,
                                  uint32_t pixel_format)
{
  //使能DM2D时钟
  __HAL_RCC_DMA2D_CLK_ENABLE();
  //先停止DMA2D
  DMA2D->CR &= ~(DMA2D_CR_START);
  /* DMA2D采用寄存器到存储器模式, 这种模式用不到前景层和背景层 */
  DMA2D->CR = mode;
#ifdef ENABLE_GUI_LVGL  // 源地址
  DMA2D->FGMAR   = (uint32_t)(uint16_t *)(color_p);
#else

  // 源地址
  if (DMA2D_M2M == mode)
  {
    DMA2D->FGMAR   = color_index;
  }
  else if (DMA2D_R2M == mode)
  {
    DMA2D->FGMAR   = (uint32_t)(uint16_t *)(color_p);
  }

#endif
  // 目标地址
  DMA2D->OMAR    = (uint32_t) * (&p_dst);
  // 输入偏移
  DMA2D->FGOR    = 0;
  // 输出偏移
  DMA2D->OOR     = offset_line;
  // 设置填充使用的颜色
  DMA2D->OCOLR   = color_index;
  /* 前景层和输出区域都采用的RGB565颜色格式 */
  DMA2D->FGPFCCR = pixel_format;
  DMA2D->OPFCCR  = pixel_format;
  // 多少行
  DMA2D->NLR     = (uint32_t)(x_size << 16) | (uint16_t)y_size;
}

//LTDC延时
void user_dma2d_delay(vu32 i)
{
  while (i > 0)i--;
}

void user_dma2d_fill_ltdc(void *color_p,
                          void *p_dst,
                          uint32_t x_size,
                          uint32_t y_size,
                          uint32_t offset_line,
                          uint32_t color_index,
                          uint32_t pixel_format,
                          uint32_t cr_mode)
{
  u32 timeout = 0;
#ifdef ENABLE_GUI_LVGL
  //寄存器到存储器模式
  u32 CR = DMA2D_R2M;
  user_dma2d_fill_basic(color_p, p_dst, CR, x_size, y_size, offset_line, color_index, pixel_format);
#else
  user_dma2d_fill_basic(color_p, p_dst, cr_mode, x_size, y_size, offset_line, color_index, pixel_format);
#endif
  //启动DMA2D
  DMA2D->CR |= DMA2D_CR_START;

  //等待传输完成
  while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)
  {
    timeout++;

    //超时退出
    if (timeout > 0X1FFFFF)break;
  }

  if (mcu_id == MCU_GD32F450IIH6)
  {
    user_dma2d_delay(0x10);
  }

  //清除传输完成标志
  DMA2D->IFCR |= DMA2D_FLAG_TC;
}


#ifdef ENABLE_GUI_LVGL
void user_dma2d_fill_lvgl(void *color_p,
                          void *p_dst,
                          uint32_t x_size,
                          uint32_t y_size,
                          uint32_t offset_line,
                          uint32_t color_index,
                          uint32_t pixel_format)
{
  // 模式 TC 中断使能
  u32 CR = 0x00000000UL | (1 << 9);
  user_dma2d_fill_basic(color_p, p_dst, CR, x_size, y_size, offset_line, color_index, pixel_format);
  // 开启中断
  DMA2D->CR |= DMA2D_IT_TC | DMA2D_IT_TE | DMA2D_IT_CE;
  /* 启动传输 */
  DMA2D->CR   |= DMA2D_CR_START;
  //  /* 等待DMA2D传输完成 */
  //  while (DMA2D->CR & DMA2D_CR_START) {}
}
#endif

#endif


