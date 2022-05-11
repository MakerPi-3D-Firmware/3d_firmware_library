#ifndef USER_FTL_H
#define USER_FTL_H

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//NAND FLASH FTL层算法代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/15
//版本：V1.3
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//升级说明
//V1.1 20160124
//修改FTL_CopyAndWriteToBlock和FTL_WriteSectors函数,提高非0XFF时的写入速度.
//V1.2 20160520
//1,修改FTL_ReadSectors,增加ECC出错判断,检测坏块处理,并增加多块连读,提高速度
//2,新增FTL_BlockCompare和FTL_SearchBadBlock函数,用于搜寻坏块
//3,修改FTL_Format坏块检测方式,增加FTL_USE_BAD_BLOCK_SEARCH宏
//V1.3 20160530
//修改当1bit ECC错误出现时，读取2次，来确认1bit 错误，以防错误的修改数据
//////////////////////////////////////////////////////////////////////////////////

//坏块搜索控制
//如果设置为1,将在FTL_Format的时候,搜寻坏块,耗时久(512M,3分钟以上),且会导致RGB屏乱闪
#define FTL_USE_BAD_BLOCK_SEARCH    0   //定义是否使用坏块搜索



u8 user_ftl_init(void);
void user_ftl_bad_block_mark(u32 blocknum);
u8 user_ftl_check_bad_block(u32 blocknum);
u8 user_ftl_used_block_mark(u32 blocknum);
u32 user_ftl_find_unused_block(u32 sblock, u8 flag);
u32 user_ftl_find_same_plane_unused_block(u32 sblock);
u8 user_ftl_copy_and_write_to_block(u32 Source_PageNum, u16 ColNum, u8 *pBuffer, u32 NumByteToWrite);
u16 user_ftl_lbn_to_pbn(u32 LBNNum);
u8 user_ftl_write_sectors(u8 *pBuffer, u32 SectorNo, u16 SectorSize, u32 SectorCount);
u8 user_ftl_read_sectors(u8 *pBuffer, u32 SectorNo, u16 SectorSize, u32 SectorCount);
u8 user_ftl_create_lut(u8 mode);
u8 user_ftl_block_compare(u32 blockx, u32 cmpval);
u32 user_ftl_search_bad_block(void);
u8 user_ftl_format(void);
#endif //USER_FTL_H














