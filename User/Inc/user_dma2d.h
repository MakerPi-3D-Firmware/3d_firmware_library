#ifndef _USER_DMA2D_H
#define _USER_DMA2D_H

#ifdef ENABLE_DMA2D

#ifdef __cplusplus
extern "C" {
#endif

void user_dma2d_init(void);
void user_dma2d_fill_ltdc(void *color_p,
                          void *p_dst,
                          uint32_t x_size,
                          uint32_t y_size,
                          uint32_t offset_line,
                          uint32_t color_index,
                          uint32_t pixel_format,
                          uint32_t cr_mode);

#ifdef ENABLE_GUI_LVGL
void user_dma2d_fill_lvgl(void *color_p,
                          void *p_dst,
                          uint32_t x_size,
                          uint32_t y_size,
                          uint32_t offset_line,
                          uint32_t color_index,
                          uint32_t pixel_format);
#endif

#ifdef __cplusplus
} //extern "C" {
#endif

#endif

#endif
