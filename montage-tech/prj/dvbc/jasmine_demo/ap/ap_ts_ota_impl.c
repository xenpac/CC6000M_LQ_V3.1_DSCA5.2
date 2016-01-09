/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_devs.h"
#include "sys_cfg.h"

#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "mtos_mem.h"

#include "mem_manager.h"
#include "fcrc.h"
#include "lib_util.h"

#include "drv_dev.h"
#include "charsto.h"
#include "data_manager.h"
#include "nim.h"
#include "pti.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"
#include "ap_framework.h"
#include "ap_ts_ota.h"
#include "sys_cfg.h"

static void ts_ota_crc_init()
{
  RET_CODE ret;
  
   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret); 
}

static u32 ts_ota_crc32_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
}

static void ts_ota_read_otai(void *p_otai)
{
  class_handle_t p_handle = class_get_handle_by_id(SC_CLASS_ID);
  //class_handle_t p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  
  ss_public_t *p_pub = ss_ctrl_get_public(p_handle);
  //u32 b_size;

  //if(p_pub->otai.check_flag != OTAI_CHECK_FLAG)
  //{
    /* has not processed ota, read default otai */
  //  b_size = dm_get_block_size(p_dm_handle, UPGRADE_BLOCK_ID); 
  //  dm_read(p_dm_handle, UPGRADE_BLOCK_ID, 0, 0, b_size, (u8 *)p_otai); 
  //}
  //else
  //{
    memcpy(p_otai, &p_pub->otai, sizeof(ota_info_t));
  //}
}

static void ts_ota_write_otai(ota_info_t *p_otai)
{
  class_handle_t p_handle = NULL;
  ss_public_t *pub = NULL;

  p_handle = class_get_handle_by_id(SC_CLASS_ID);
  pub = ss_ctrl_get_public(p_handle);
  
  /* update the crc */
  //p_otai->check_flag = OTAI_CHECK_FLAG;
  //p_otai->upg_info_crc = ts_ota_crc32_generate(
  //  (u8 *)p_otai, sizeof(ota_info_t) - sizeof(u32));
  
  memcpy(&pub->otai, p_otai, sizeof(ota_info_t));

  ss_ctrl_update_public(p_handle);
}

typedef void (*FUNC_NOTIFY_PROGRESS)(u32 cur_size, u32 full_size);

#define BL_OFFSET 8
static BOOL ts_ota_burn_file(u8 *p_buf, u32 size, void *notify_progress)
{
  struct charsto_device *p_norf_dev;
  u32 bl_size;
  u32 burn_offset = 0;
  u32 burn_size = 0;
  RET_CODE ret = SUCCESS;
  u32 charsto_size = CHARSTO_SIZE;
  u32 i, sec_cnt;
  FUNC_NOTIFY_PROGRESS func;
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  OS_PRINTF("burn file size[0x%x]\n", size);
  /* burn sectors except bootloader sector */
  p_norf_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  if(SUCCESS != charsto_read(p_norf_dev, BL_OFFSET, (u8 *)&bl_size, 4))
  {
    return FALSE;
  }
  if(bl_size % CHARSTO_SECTOR_SIZE != 0)
  {
    bl_size += CHARSTO_SECTOR_SIZE - (bl_size % CHARSTO_SECTOR_SIZE);
  }

  if(size != charsto_size && size != (charsto_size-bl_size))
  {
    return FALSE;
  }
  
  burn_offset = bl_size;
  if(size == charsto_size)
  {
    p_buf += bl_size;
    burn_size = charsto_size - bl_size;
  }
  else
  {
    burn_size = size;
  }

  sec_cnt = burn_size / CHARSTO_SECTOR_SIZE;


  dev_io_ctrl(p_norf_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_norf_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);
  
  OS_PRINTF("burn, start[0x%x], size[0x%x]\n", burn_offset, burn_size);
  func = (FUNC_NOTIFY_PROGRESS)notify_progress;
  for(i=0; i<sec_cnt; i++)
  {
    #ifndef WIN32
    //OS_PRINTF("burn sector[%d]\n", i);	
    charsto_erase(p_norf_dev, burn_offset+i*CHARSTO_SECTOR_SIZE, 1);
    ret = charsto_writeonly(p_norf_dev, burn_offset+i*CHARSTO_SECTOR_SIZE, 
                  p_buf+i*CHARSTO_SECTOR_SIZE, CHARSTO_SECTOR_SIZE);
    #endif
    if(ret != SUCCESS)
    {
      //restore
      dev_io_ctrl(p_norf_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
      return FALSE;
    } 
    func((i+1)*100/sec_cnt, 100);
    #ifdef WIN32
    mtos_task_sleep(500);
    #else
    //mtos_task_sleep(2000);
    #endif
  }

  //restore
  dev_io_ctrl(p_norf_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
  return TRUE;
}

ts_ota_policy_t *construct_ts_ota_policy(void)
{
  ts_ota_policy_t *p_ts_ota_impl = mtos_malloc(sizeof(ts_ota_policy_t));
  MT_ASSERT(p_ts_ota_impl != NULL);

  p_ts_ota_impl->ota_crc_init = ts_ota_crc_init;
  p_ts_ota_impl->ota_crc32_generate = ts_ota_crc32_generate;
  p_ts_ota_impl->read_otai = ts_ota_read_otai;
  p_ts_ota_impl->write_otai = ts_ota_write_otai;
  p_ts_ota_impl->burn_file = ts_ota_burn_file;
  
  p_ts_ota_impl->p_data = NULL;
  
  return p_ts_ota_impl;
}

void destruct_ts_ota_policy(ts_ota_policy_t *p_ts_ota_impl)
{
  //Free private data
  mtos_free(p_ts_ota_impl->p_data);

  //Free implement policy
  mtos_free(p_ts_ota_impl);
}
