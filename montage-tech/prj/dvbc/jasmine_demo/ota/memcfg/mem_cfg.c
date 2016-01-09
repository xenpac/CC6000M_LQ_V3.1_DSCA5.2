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
//#include "mtos_printk.h"
#include "mtos_mem.h"

//drv
#include "common.h"

#include "mem_manager.h"

#include "drv_dev.h"
#include "vdec.h"

#include "mem_cfg.h"
 
extern u32 get_mem_addr(void);

static const partition_block_configs_t block_cfg_normal[STATIC_BLOCK_NUM] =
{
  {BLOCK_STP_BUFFER, STP_32BIT_BUFFER_ADDR, STP_32BIT_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED}
};

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
    curn_blk_cfg[i].addr += base;
  }

  ret = mem_mgr_config_blocks(curn_blk_cfg, STATIC_BLOCK_NUM);
  MT_ASSERT(ret != FALSE);
}




