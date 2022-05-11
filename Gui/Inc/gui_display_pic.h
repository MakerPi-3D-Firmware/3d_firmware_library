#ifndef GUI_DISPLAY_PIC_H
#define GUI_DISPLAY_PIC_H

#ifdef __cplusplus
extern "C" {
#endif

void display_picture(int PictureName);
void DisplayLogoPicture(int PictureName);
uint32_t Draw_progressBar_new(uint32_t Printfilesize, uint32_t Filesize, int x, int y, int x_max, int y_max);
uint32_t Draw_progressBar(uint32_t Printfilesize, uint32_t Filesize);
void gui_disp_pic_logo(void);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // GUI_DISPLAY_PIC_H

