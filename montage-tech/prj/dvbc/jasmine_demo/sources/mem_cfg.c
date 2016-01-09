/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "sys_define.h"
#include "sys_types.h"
#include "sys_cfg.h"

#include "string.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"

//drv
#include "common.h"

#include "mem_manager.h"

#include "drv_dev.h"
#include "vdec.h"
#include "hal_misc.h"
#include "sys_app.h"

#include "mem_cfg.h"
extern u32 get_mem_addr(void);

/************************************  sys cfg .h ****************************************/
u32 g_video_fw_cfg_size = 0;
u32 g_video_fw_cfg_addr = 0;
u32 g_vid_di_cfg_size = 0;
u32 g_vid_di_cfg_addr = 0;
u32 g_vbi_buf_size = 0;
u32 g_vbi_buf_addr = 0;
u32 g_audio_fw_cfg_size = 0;
u32 g_audio_fw_cfg_addr = 0;
u32 g_ap_av_share_mem_size = 0;
u32 g_ap_av_share_mem_addr = 0;
u32 g_vid_sd_wr_back_size = 0;
u32 g_vid_sd_wr_back_addr = 0;
u32 g_epg_buffer_size = 0;
u32 g_epg_buffer_addr = 0;
u32 g_rec_buffer_size = 0;
u32 g_rec_buffer_addr = 0;
u32 g_play_buffer_size = 0;  // for PVR PLAY
u32 g_play_buffer_add = 0;  // for PVR PLAY
u32 g_video_file_play_addr = 0;                       //fileplay addr   and  pvr play addr
u32 g_video_file_play_size = 0;                       //for fileplay size = PVR rec size + PVR play size

/************************************  sys cfg .h ****************************************/


/***********************************  mem cfg .h *****************************************/
u32 g_av_buffer_size = 0;
u32 g_av_buffer_addr = 0;

u32 g_sub_buffer_size = 0;
u32 g_sub_buffer_addr = 0;
u32 g_sub_vscaler_buffer_size = 0;
u32 g_sub_vscaler_buffer_addr = 0;

u32 g_osd0_buffer_size = 0;
u32 g_osd0_buffer_addr = 0;
u32 g_osd0_vscaler_buffer_size = 0;
u32 g_osd0_vscaler_buffer_addr = 0;

u32 g_osd1_buffer_size = 0;
u32 g_osd1_buffer_addr = 0;
u32 g_osd1_vscaler_buffer_size = 0;
u32 g_osd1_vscaler_buffer_addr = 0;

u32 g_gui_resource_buffer_size = 0;
u32 g_gui_resource_buffer_addr = 0;
u32 g_gui_partition_size = 0;
u32 g_gui_partition_addr = 0;

/***********************************  mem cfg .h *****************************************/


static const partition_block_configs_t block_cfg_normal[STATIC_BLOCK_NUM] =
{
  {BLOCK_AV_BUFFER, AV_BUFFER_ADDR, AV_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_EPG_BUFFER, EPG_BUFFER_ADDR, EPG_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_REC_BUFFER, REC_BUFFER_ADDR, REC_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_PLAY_BUFFER, PLAY_BUFFER_ADDR, PLAY_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
  {BLOCK_UI_RESOURCE_BUFFER, GUI_RESOURCE_BUFFER_ADDR, GUI_RESOURCE_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
};

void mem_map_cfg()
{
  /********************************* for AV  0xaxxxxxxx **********************************/
  if ((hal_get_chip_rev() >= CHIP_CONCERTO_B0))
  {
    APPLOGA("\n Currect chip version is 0x%08x >= CHIP_CONCERTO_B0 \n",hal_get_chip_rev());
    g_video_fw_cfg_size = VIDEO_FW_CFG_SIZE_B;
  }
  else
  {
    APPLOGA("\n Currect chip version is 0x%08x < CHIP_CONCERTO_B0 \n",hal_get_chip_rev());
    g_video_fw_cfg_size = VIDEO_FW_CFG_SIZE;
  }
  g_video_fw_cfg_addr = AV_STACK_BOT_ADDR - g_video_fw_cfg_size;
  g_vid_di_cfg_size = VID_DI_CFG_SIZE;
  g_vid_di_cfg_addr = g_video_fw_cfg_addr - g_vid_di_cfg_size;
  g_vbi_buf_size = VBI_BUF_SIZE;
  g_vbi_buf_addr = g_vid_di_cfg_addr - g_vbi_buf_size;
  g_audio_fw_cfg_size = AUDIO_FW_CFG_SIZE;
  g_audio_fw_cfg_addr = g_vbi_buf_addr - g_audio_fw_cfg_size;
  g_ap_av_share_mem_size = AP_AV_SHARE_MEM_SIZE;
  g_ap_av_share_mem_addr = g_audio_fw_cfg_addr - g_ap_av_share_mem_size;
  g_vid_sd_wr_back_size = VID_SD_WR_BACK_SIZE;
  g_vid_sd_wr_back_addr = g_ap_av_share_mem_addr - g_vid_sd_wr_back_size;

  
  /********************************* for Fileplay  EPG PVR*******************************************/
  g_epg_buffer_size = EPG_BUFFER_SIZE;
  g_epg_buffer_addr = (g_vid_sd_wr_back_addr - g_epg_buffer_size) & (0xFFFFFFF);
  
  g_rec_buffer_size = REC_BUFFER_SIZE;
#ifdef MIN_AV_64M
  g_rec_buffer_addr = (g_vid_sd_wr_back_addr - g_rec_buffer_size) & (0xFFFFFFF);
#else
  g_rec_buffer_addr = (g_epg_buffer_addr -g_rec_buffer_size) & (0xFFFFFFF);
#endif
  g_play_buffer_size = VIDEO_SHIFT_SIZE_WITH_FILEPALY;  // for PVR PLAY
  g_play_buffer_add = g_rec_buffer_addr - g_play_buffer_size;  // for PVR PLAY
  g_video_file_play_size = g_rec_buffer_size + g_play_buffer_size; //for fileplay size = PVR rec size + PVR play size
#ifdef MIN_AV_64M
  g_video_file_play_addr = (g_vid_sd_wr_back_addr - g_video_file_play_size) & (0x8FFFFFFF); //fileplay addr 
#else
  g_video_file_play_addr = (g_epg_buffer_addr - g_video_file_play_size) | (0x80000000); //fileplay addr 
#endif


  /********************************* for OSD 0xAxxxxxxx *********************************/
  g_sub_buffer_size = SUB_BUFFER_SIZE;
  g_sub_buffer_addr = (g_video_file_play_addr - g_sub_buffer_size) | (0xA0000000);
  g_sub_vscaler_buffer_size = SUB_VSCALER_BUF_SIZE;
  g_sub_vscaler_buffer_addr = (g_sub_buffer_addr -g_sub_vscaler_buffer_size) | (0xA0000000);

  g_osd0_buffer_size = OSD0_BUFFER_SIZE;
  g_osd0_buffer_addr = g_sub_vscaler_buffer_addr - g_osd0_buffer_size;
  g_osd0_vscaler_buffer_size = OSD0_VSCALER_BUF_SIZE;
  g_osd0_vscaler_buffer_addr = g_osd0_buffer_addr - g_osd0_vscaler_buffer_size;

  g_osd1_buffer_size = OSD1_BUFFER_SIZE;
  g_osd1_buffer_addr = g_osd0_vscaler_buffer_addr - g_osd1_buffer_size;
  g_osd1_vscaler_buffer_size = OSD1_VSCALER_BUF_SIZE;
  g_osd1_vscaler_buffer_addr = g_osd1_buffer_addr - g_osd1_vscaler_buffer_size;

  /********************************* for APP *******************************************/
  g_gui_resource_buffer_size = GUI_RESOURCE_BUFFER_SIZE;
  g_gui_resource_buffer_addr = (g_osd1_vscaler_buffer_addr - g_gui_resource_buffer_size) & (0xFFFFFFF);
  g_av_buffer_size = g_video_fw_cfg_size;
  g_av_buffer_addr = (g_video_fw_cfg_addr & 0xFFFFFFF);

}
#if (defined MIN_AV_64M) && (defined DTMB_PROJECT)
void mem_map_cfg_dtmb_64m()
{

    /********************************* for Fileplay  EPG PVR*******************************************/
  g_epg_buffer_size = EPG_BUFFER_SIZE;
  g_epg_buffer_addr = (AV_STACK_BOT_ADDR - g_epg_buffer_size) & (0xFFFFFFF);
  
  g_rec_buffer_size = REC_BUFFER_SIZE;
  g_rec_buffer_addr = (AV_STACK_BOT_ADDR - g_rec_buffer_size) & (0xFFFFFFF);
  
  g_play_buffer_size = VIDEO_SIZE_WITH_FILEPLAY+VIDEO_SHIFT_SIZE_WITH_FILEPALY;  // for PVR PLAY
  g_play_buffer_add = g_rec_buffer_addr - g_play_buffer_size;  // for PVR PLAY

  g_video_file_play_size = g_rec_buffer_size + g_play_buffer_size; //for fileplay size = PVR rec size + PVR play size
  g_video_file_play_addr = ((g_epg_buffer_addr - g_video_file_play_size + g_rec_buffer_size)|0xa0000000) & (0x8FFFFFFF); //fileplay addr 
  OS_PRINTF("g_video_file_play_addr 0x%x VIDEO_FW_CFG_ADDR 0x%x \n", g_video_file_play_addr, VIDEO_FW_CFG_ADDR);
  OS_PRINTF("VIDEO_NO_SHIFT_ADDR 0x%x VIDEO_FILE_PLAY_SIZE 0x%x \n", VIDEO_NO_SHIFT_ADDR, VIDEO_FILE_PLAY_SIZE);
  /********************************* for AV  0xaxxxxxxx **********************************/
  if ((hal_get_chip_rev() >= CHIP_CONCERTO_B0))
  {
    APPLOGA("\n Currect chip version is 0x%08x >= CHIP_CONCERTO_B0 \n",hal_get_chip_rev());
    g_video_fw_cfg_size = VIDEO_FW_CFG_SIZE_B;
  }
  else
  {
    APPLOGA("\n Currect chip version is 0x%08x < CHIP_CONCERTO_B0 \n",hal_get_chip_rev());
    g_video_fw_cfg_size = VIDEO_FW_CFG_SIZE;
  }
  
  g_video_fw_cfg_addr = (g_rec_buffer_addr - VIDEO_SIZE_WITH_FILEPLAY - g_video_fw_cfg_size) | (0xa0000000);
  g_vid_di_cfg_size = VID_DI_CFG_SIZE;
  g_vid_di_cfg_addr = g_video_fw_cfg_addr - g_vid_di_cfg_size;
  g_vbi_buf_size = VBI_BUF_SIZE;
  g_vbi_buf_addr = g_vid_di_cfg_addr - g_vbi_buf_size;
  g_audio_fw_cfg_size = AUDIO_FW_CFG_SIZE;
  g_audio_fw_cfg_addr = g_vbi_buf_addr - g_audio_fw_cfg_size;
  g_ap_av_share_mem_size = AP_AV_SHARE_MEM_SIZE;
  g_ap_av_share_mem_addr = g_audio_fw_cfg_addr - g_ap_av_share_mem_size;
  g_vid_sd_wr_back_size = VID_SD_WR_BACK_SIZE;
  g_vid_sd_wr_back_addr = g_ap_av_share_mem_addr - g_vid_sd_wr_back_size;

  /********************************* for OSD 0xAxxxxxxx *********************************/
  g_sub_buffer_size = SUB_BUFFER_SIZE;
  g_sub_buffer_addr = (g_vid_sd_wr_back_addr - g_sub_buffer_size) | (0xA0000000);
  g_sub_vscaler_buffer_size = SUB_VSCALER_BUF_SIZE;
  g_sub_vscaler_buffer_addr = (g_sub_buffer_addr -g_sub_vscaler_buffer_size) | (0xA0000000);

  g_osd0_buffer_size = OSD0_BUFFER_SIZE;
  g_osd0_buffer_addr = g_sub_vscaler_buffer_addr - g_osd0_buffer_size;
  g_osd0_vscaler_buffer_size = OSD0_VSCALER_BUF_SIZE;
  g_osd0_vscaler_buffer_addr = g_osd0_buffer_addr - g_osd0_vscaler_buffer_size;

  g_osd1_buffer_size = OSD1_BUFFER_SIZE;
  g_osd1_buffer_addr = g_osd0_vscaler_buffer_addr - g_osd1_buffer_size;
  g_osd1_vscaler_buffer_size = OSD1_VSCALER_BUF_SIZE;
  g_osd1_vscaler_buffer_addr = g_osd1_buffer_addr - g_osd1_vscaler_buffer_size;

  /********************************* for APP *******************************************/
  g_gui_resource_buffer_size = GUI_RESOURCE_BUFFER_SIZE;
  g_gui_resource_buffer_addr = (g_osd1_vscaler_buffer_addr - g_gui_resource_buffer_size) & (0xFFFFFFF);
  g_av_buffer_size = g_video_fw_cfg_size;
  g_av_buffer_addr = (g_video_fw_cfg_addr & 0xFFFFFFF);

}
#endif

void mem_cfg(mem_cfg_t cfg)
{
  u16 i = 0;
  u32 base = 0;
  BOOL ret = FALSE;
  partition_block_configs_t curn_blk_cfg[STATIC_BLOCK_NUM] = {{0}};

  switch(cfg)
  {
    case MEMCFG_T_NORMAL:
      memcpy(curn_blk_cfg, block_cfg_normal, sizeof(curn_blk_cfg));
      break;

    default:
      MT_ASSERT(0);
      return;
  }
  // offset to base
  base = get_mem_addr();
  for (i = 0; i < STATIC_BLOCK_NUM; i++)
  {
    switch(curn_blk_cfg[i].block_id)
    {
      case BLOCK_AV_BUFFER:
        {
          curn_blk_cfg[i].addr = g_av_buffer_addr;
          curn_blk_cfg[i].size = g_av_buffer_size;
        }
        break;
      case BLOCK_EPG_BUFFER:
        {
          curn_blk_cfg[i].addr = g_epg_buffer_addr;
          curn_blk_cfg[i].size = g_epg_buffer_size;
        }
        break;
      case BLOCK_REC_BUFFER:
        {
          curn_blk_cfg[i].addr = g_rec_buffer_addr;
          curn_blk_cfg[i].size = g_rec_buffer_size;
        }
        break;
      case BLOCK_PLAY_BUFFER:
        {
          curn_blk_cfg[i].addr = g_play_buffer_add;
          curn_blk_cfg[i].size = g_play_buffer_size;
       }
        break;
      case BLOCK_UI_RESOURCE_BUFFER:
        {
          curn_blk_cfg[i].addr = g_gui_resource_buffer_addr;
          curn_blk_cfg[i].size = g_gui_resource_buffer_size;
      }
        break;
    }
    curn_blk_cfg[i].addr += base;
  }

  ret = mem_mgr_config_blocks(curn_blk_cfg, STATIC_BLOCK_NUM);
  MT_ASSERT(ret != FALSE);
}




