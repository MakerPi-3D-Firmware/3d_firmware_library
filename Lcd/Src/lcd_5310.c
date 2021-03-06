#include "user_common.h"

#ifdef ENABLE_LCD

#if defined(ENABLE_FSMC)

#include "../Inc/lcd_common.h"

#ifdef HAL_SRAM_MODULE_ENABLED

void LCD_Init_5310(void)
{
  LCD_WR_REG(0xED);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0xFE);
  LCD_WR_REG(0xEE);
  LCD_WR_DATA(0xDE);
  LCD_WR_DATA(0x21);
  LCD_WR_REG(0xF1);
  LCD_WR_DATA(0x01);
  LCD_WR_REG(0xDF);
  LCD_WR_DATA(0x10);
  //VCOMvoltage//
  LCD_WR_REG(0xC4);
  LCD_WR_DATA(0x8F);    //5f
  LCD_WR_REG(0xC6);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xE2);
  LCD_WR_DATA(0xE2);
  LCD_WR_DATA(0xE2);
  LCD_WR_REG(0xBF);
  LCD_WR_DATA(0xAA);
  LCD_WR_REG(0xB0);
  LCD_WR_DATA(0x0D);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x0D);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x11);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x19);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x21);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x2D);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x3D);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x5D);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x5D);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB1);
  LCD_WR_DATA(0x80);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x8B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x96);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB2);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB3);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB4);
  LCD_WR_DATA(0x8B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x96);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA1);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB5);
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB6);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB7);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x3F);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x5E);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x8C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xAC);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xDC);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x70);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x90);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xEB);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xDC);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xB8);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xBA);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC1);
  LCD_WR_DATA(0x20);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x54);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xFF);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC2);
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC3);
  LCD_WR_DATA(0x3C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x3A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x39);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x37);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x3C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x36);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x32);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x2F);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x29);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x26);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x3C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x36);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x32);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x2F);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x2C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x29);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x26);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x24);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC4);
  LCD_WR_DATA(0x62);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x84);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF0);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x18);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA4);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x18);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x50);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x0C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x17);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x95);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xE6);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC5);
  LCD_WR_DATA(0x32);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x65);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x76);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x88);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC6);
  LCD_WR_DATA(0x20);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x17);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC7);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC8);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xC9);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE0);
  LCD_WR_DATA(0x16);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x21);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x36);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x46);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x52);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x7A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x8B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA8);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xB9);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC4);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xCA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD2);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD9);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xE0);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE1);
  LCD_WR_DATA(0x16);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x36);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x45);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x52);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x64);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x7A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x8B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA8);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xB9);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC4);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xCA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD2);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD8);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xE0);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE2);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x1B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x4F);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x61);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x79);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x88);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x97);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA6);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xB7);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC2);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC7);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD6);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xDD);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE3);
  LCD_WR_DATA(0x05);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x1C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x33);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x50);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x62);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x78);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x88);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x97);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA6);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xB7);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC2);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC7);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD1);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD5);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xDD);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE4);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x2A);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x3C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x4B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x5D);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x74);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x84);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x93);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA2);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xB3);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xBE);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC4);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xCD);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD3);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xDD);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE5);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x02);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x29);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x3C);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x4B);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x5D);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x74);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x84);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x93);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xA2);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xB3);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xBE);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xC4);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xCD);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xD3);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xDC);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE6);
  LCD_WR_DATA(0x11);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x34);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x56);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x76);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x77);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x66);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x88);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xBB);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x66);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x55);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x55);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x45);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x43);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE7);
  LCD_WR_DATA(0x32);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x55);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x76);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x66);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x67);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x67);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x87);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xBB);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x77);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x56);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x33);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x45);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE8);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x87);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x88);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x77);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x66);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x88);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xAA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0xBB);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x99);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x66);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x55);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x55);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x44);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x55);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xE9);
  LCD_WR_DATA(0xAA);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0x00);
  LCD_WR_DATA(0xAA);
  LCD_WR_REG(0xCF);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xF0);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x50);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xF3);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0xF9);
  LCD_WR_DATA(0x06);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x29);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0x3A);
  LCD_WR_DATA(0x55);  //66
  LCD_WR_REG(0x11);
  user_delay_ms(100);
  LCD_WR_REG(0x29);
  LCD_WR_REG(0x35);
  LCD_WR_DATA(0x00);
  LCD_WR_REG(0x51);
  LCD_WR_DATA(0xFF);
  LCD_WR_REG(0x53);
  LCD_WR_DATA(0x2C);
  LCD_WR_REG(0x55);
  LCD_WR_DATA(0x82);
  LCD_WR_REG(0x2c);
}

#endif
#endif
#endif

