/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifndef __MEM_CFG_H__
#define __MEM_CFG_H__


//warriors

typedef enum
{
  MEMCFG_T_NORMAL,
}mem_cfg_t;

/*! Video buffer classification.*/
typedef enum
{
  BLOCK_STP_BUFFER = 0,
} block_id_t;

#define STATIC_BLOCK_NUM    1

/*!
   Memory configuration
  */
#define SYS_MEMORY_END     0x3000000  //48M

#define SUB_BUFFER_SIZE (0 * KBYTES)//1280*(720 + 4)*4
#define SUB_BUFFER_ADDR (((SYSTEM_MEMORY_END & 0x0FFFFFFF) - SUB_BUFFER_SIZE)  | 0xa0000000)

#define SUB_VSCALER_BUF_SIZE (0 * KBYTES)//1280*(720 + 4)*4
#define SUB_VSCALER_BUF_ADDR ((SUB_BUFFER_ADDR - SUB_VSCALER_BUF_SIZE) | 0xa0000000)

#define OSD0_BUFFER_SIZE 0//(2300 * KBYTES)
#define OSD0_BUFFER_ADDR ((SUB_VSCALER_BUF_ADDR - OSD0_BUFFER_SIZE)  | 0xa0000000)

#define OSD0_VSCALER_BUF_SIZE (0 * KBYTES)//1280*720*4
#define OSD0_VSCALER_BUF_ADDR ((OSD0_BUFFER_ADDR - OSD0_VSCALER_BUF_SIZE) | 0xa0000000)

#define OSD1_BUFFER_SIZE (3620 * KBYTES)//(1280*720*4)
#define OSD1_BUFFER_ADDR ((OSD0_VSCALER_BUF_ADDR - OSD1_BUFFER_SIZE) | 0xa0000000)

#define OSD1_VSCALER_BUF_SIZE (3620 * KBYTES)//1280*720*4
#define OSD1_VSCALER_BUF_ADDR ((OSD1_BUFFER_ADDR - OSD1_VSCALER_BUF_SIZE) | 0xa0000000)

#define STP_32BIT_BUFFER_SIZE (0 * MBYTES)
#define STP_32BIT_BUFFER_ADDR (OSD1_VSCALER_BUF_ADDR - STP_32BIT_BUFFER_SIZE)

#define GUI_PARTITION_SIZE   (250 * KBYTES)

#define SYS_PARTITION_SIZE      (0x1800000)// (24 * MBYTES)//(7 * MBYTES+512*KBYTES)//(20 * 1024 * KBYTES)
#define SYS_PARTITION_ATOM           64

#define DMA_PARTITION_SIZE       (3 * 512 * KBYTES)
#define DMA_PARTITION_ATOM      64

/*!
  Set memory config mode
  \param[in] cfg parameter for memory confi
  */
void mem_cfg(mem_cfg_t cfg);
#endif

