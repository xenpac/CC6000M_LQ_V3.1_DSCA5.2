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
  BLOCK_AV_BUFFER = 0,
  BLOCK_SUB_BUFFER = 1,
  BLOCK_OSD1_BUFFER = 2,
  BLOCK_OSD0_BUFFER = 3,
  BLOCK_EPG_BUFFER = 4,
  BLOCK_REC_BUFFER = 5,
  BLOCK_PLAY_BUFFER = 6,
  BLOCK_UI_RESOURCE_BUFFER = 7,
  BLOCK_GUI_VSURF_BUFFER = 8,
  BLOCK_GUI_ANIM_BUFFER = 9,  
  BLOCK_MAX_ID,
} block_id_t;

#define STATIC_BLOCK_NUM    BLOCK_MAX_ID

/*!
   Memory configuration
  */

#define AV_BUFFER_SIZE  VIDEO_FW_CFG_SIZE
#define AV_BUFFER_ADDR  (VIDEO_FW_CFG_ADDR & 0x0FFFFFFF)

#define SUB_BUFFER_SIZE (0 * KBYTES)//1280*(720 + 4)*4
#define SUB_BUFFER_ADDR (((SYSTEM_MEMORY_END & 0x0FFFFFFF) - SUB_BUFFER_SIZE)  | 0xa0000000)

#define SUB_VSCALER_BUF_SIZE (0 * KBYTES)//1280*(720 + 4)*4
#define SUB_VSCALER_BUF_ADDR ((SUB_BUFFER_ADDR - SUB_VSCALER_BUF_SIZE) | 0xa0000000)

#define OSD0_BUFFER_SIZE (0 * KBYTES)//1280*720*4
#define OSD0_BUFFER_ADDR ((SUB_VSCALER_BUF_ADDR - OSD0_BUFFER_SIZE)  | 0xa0000000)

#define OSD0_VSCALER_BUF_SIZE (0 * KBYTES)//1280*720*4
#define OSD0_VSCALER_BUF_ADDR ((OSD0_BUFFER_ADDR - OSD0_VSCALER_BUF_SIZE) | 0xa0000000)

#define OSD1_BUFFER_SIZE (0 * KBYTES)//(1280*720*4)
#define OSD1_BUFFER_ADDR ((OSD0_VSCALER_BUF_ADDR - OSD1_BUFFER_SIZE) | 0xa0000000)

#define OSD1_VSCALER_BUF_SIZE (0 * KBYTES)//1280*720*4
#define OSD1_VSCALER_BUF_ADDR ((OSD1_BUFFER_ADDR - OSD1_VSCALER_BUF_SIZE) | 0xa0000000)

#define GUI_VSURF_BUFFER_SIZE 0//(2300 * KBYTES)
#define GUI_VSURF_BUFFER_ADDR ((OSD1_VSCALER_BUF_ADDR - GUI_VSURF_BUFFER_SIZE)  & (~0xa0000000))

#define GUI_ANIM_BUFFER_SIZE (0 * KBYTES)
#define GUI_ANIM_BUFFER_ADDR (GUI_VSURF_BUFFER_ADDR - GUI_ANIM_BUFFER_SIZE)

#define GUI_RESOURCE_BUFFER_SIZE (0 * KBYTES) //(16000 * KBYTES) //48K for customer)
#define GUI_RESOURCE_BUFFER_ADDR (GUI_ANIM_BUFFER_ADDR - GUI_RESOURCE_BUFFER_SIZE)

#define PLAY_BUFFER_SIZE (VIDEO_SHIFT_SIZE_WITH_FILEPALY) // 512K BYTES //dmx limit 256K (112K use by fast revend trick mode)
#define PLAY_BUFFER_ADDR (VIDEO_FILE_PLAY_ADDR & 0x0FFFFFFF)

#define GUI_PARTITION_SIZE        (0 * KBYTES)//(2*2000 * KBYTES)

#define SYS_PARTITION_SIZE        (0x1800000) // 4M

#define SYS_PARTITION_ATOM        64

/*!
  multiplex size
  */
#define MUSIC_MODULE_USED_SIZE (4 * MBYTES)  

#define MUSIC_REGION_USED_SIZE (4 * MBYTES)

#define PICTURE_MODULE_USED_SIZE (28 * MBYTES)

#define PICTURE_REGION_USED_SIZE (4 * MBYTES)

#define OSD0_VSCLAER_SHARE_VIDEO_SIZE (0x34c000)//(0x546000) //same with OSD0_VSCALER_BUFFER_SIZE 1280*1080*4,±ØÐë4K¶ÔÆë

#define TS_PLAY_BUFFER_LEN  (490 * 188) //to ts sequence

/*!
  Set memory config mode
  \param[in] cfg parameter for memory confi
  */
void mem_cfg(mem_cfg_t cfg);
#endif

