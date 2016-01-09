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
#include "cat.h"
#include "class_factory.h"
#include "mdl.h"
#include "dsmcc.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"

#include "ap_framework.h"
#include "ap_ota.h"

#include "db_dvbs.h"
#include "uio.h"
#include "sys_status.h"

#include "uio.h"
#include "sys_cfg.h"


static void ota_crc_init()
{
  RET_CODE ret;

   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret);
}

static u32 ota_crc32_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
}

static void ota_init_tdi(void *p_tdi)
{
  s32 ret;
  char misc_info[100];
  misc_options_t *misc;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)misc_info);
  MT_ASSERT(ret != ERR_FAILURE);
  misc = (misc_options_t *)misc_info;
  
  OS_PRINTF("oui = 0x%x\n", misc->ota_tdi.oui);
  OS_PRINTF("hw_mod_id = 0x%x\n", misc->ota_tdi.hw_mod_id);
  OS_PRINTF("hw_version = 0x%x\n", misc->ota_tdi.hw_version);
  OS_PRINTF("manufacture_id = 0x%x\n", misc->ota_tdi.manufacture_id);
  OS_PRINTF("sw_mod_id = 0x%x\n", misc->ota_tdi.sw_mod_id);

  memcpy((ota_tdi_t *)p_tdi, (u8 *)&misc->ota_tdi, sizeof(ota_tdi_t));
}

void ota_read_otai(ota_info_t *p_otai)
{
  memcpy((u8 *)p_otai, (u8 *)sys_status_get_ota_info(), sizeof(ota_info_t));
}

void ota_write_otai(ota_info_t *p_otai)
{
  sys_status_set_ota_info(p_otai);
  sys_status_save();
}

static void ota_cfg(ota_cfg_t *p_ota_cfg, BOOL is_upgrade_all, BOOL is_zipped) 
{
    return;
}

static BOOL ota_burn_file_group(u8 *buf, u32 start, u32 size, void *notify_progress)
{
    return TRUE;
}

ota_policy_t *construct_ota_policy(void)
{
  ota_policy_t *p_ota_impl = mtos_malloc(sizeof(ota_policy_t));
  MT_ASSERT(p_ota_impl != NULL);

  p_ota_impl->ota_crc_init = ota_crc_init;
  p_ota_impl->ota_crc32_generate = ota_crc32_generate;
  p_ota_impl->ota_init_tdi = ota_init_tdi;
  p_ota_impl->ota_read_otai = ota_read_otai;
  p_ota_impl->ota_write_otai = ota_write_otai;
  p_ota_impl->ota_cfg = ota_cfg;
  p_ota_impl->ota_burn_file_group = ota_burn_file_group;
  p_ota_impl->p_data = NULL;
  
  return p_ota_impl;
}

void destruct_ota_policy(ota_policy_t *p_ota_impl)
{
  //Free private data
  mtos_free(p_ota_impl->p_data);

  //Free implement policy
  mtos_free(p_ota_impl);
}
