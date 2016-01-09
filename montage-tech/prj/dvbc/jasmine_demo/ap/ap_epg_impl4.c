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
#include "sys_cfg.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"

#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"

#include "eit.h"
#include "epg_data4.h"
#include "epg_util.h"

#include "ap_framework.h"
#include "ap_epg4.h"

/*!
  Total partition size of EPG:
  Configurable with different project
  */
#define MAX_NET_NODE_NUM          (2)
#define MAX_TS_NODE_NUM           (2)
#define MAX_SVC_NODE_NUM          (5)
#define MAX_EVT_NODE_NUM          (1000)
//#define MEM_DEBUG
#define MAX_EXT_BUF_NUM 3
#define PTI_BUF_SIZE 64 * 1024

static void impl_mem_cfg(void *p_data)
{
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  u8 *p_block_addr = NULL;
  u32 block_size = 0;
  u32 size;
  u8 buf_cnt = 0;
  epg_db_policy_t epg_db_policy = {0};
  filter_ext_buf_t *p_filter_ext_buf = NULL;
  p_block_addr = 
          (u8*)mem_mgr_require_block(p_impl_data->block_id,  SYS_MODULE_EPG);
  block_size = mem_mgr_get_block_size(p_impl_data->block_id);
  memset(p_block_addr, 0, block_size);
  size=(u32)p_block_addr;
  p_filter_ext_buf = (filter_ext_buf_t *)(p_impl_data->p_ext_buf_list);
  OS_PRINTF("AP EPG start %x\n", (u32)p_block_addr);
  OS_PRINTF("block size %x\n", block_size);

  MT_ASSERT(p_block_addr != NULL);
  mem_mgr_release_block(p_impl_data->block_id);
  
  //Add PTI buffer into FIFO
 for(buf_cnt = 0; buf_cnt < p_impl_data->ext_buf_num; buf_cnt ++)
  {    
    p_filter_ext_buf[buf_cnt].p_buf = p_block_addr;
    p_filter_ext_buf[buf_cnt].p_next = NULL;
    p_filter_ext_buf[buf_cnt].size = PTI_BUF_SIZE;    
    p_block_addr += PTI_BUF_SIZE;
  }

  //init epg db policy

  epg_db_policy.evt_info_priority = EPG_EVT_INFO_ALL;
  epg_db_policy.evt_max_day = EPG_EVT_8_DAYS;
  epg_db_policy.p_mem_addr = (void*)p_block_addr;
#ifdef MEM_DEBUG
  epg_db_policy.evt_priority = EPG_EVT_OF_CUR_SVCS_ONLY;
  epg_db_policy.mem_size = 400000;
#else  
  epg_db_policy.evt_priority = EPG_EVT_OF_CUR_SVCS_ONLY;
  epg_db_policy.mem_size = block_size-((u32)p_block_addr-size);
#endif
  
  //Allocate memory for EPG database  
  epg_data_mem_alloc(class_get_handle_by_id(EPG_CLASS_ID), &epg_db_policy,
                     MAX_NET_NODE_NUM,MAX_TS_NODE_NUM,
					 MAX_SVC_NODE_NUM,MAX_EVT_NODE_NUM);

}

static void impl_mem_release(void *p_data)
{
  u8 buf_cnt = 0;
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  filter_ext_buf_t *p_filter_ext_buf = NULL;
  //Clear PTI buffer 
  p_filter_ext_buf = (filter_ext_buf_t *)(p_impl_data->p_ext_buf_list);
  for(buf_cnt = 0; buf_cnt < p_impl_data->ext_buf_num; buf_cnt ++)
  {
    p_filter_ext_buf[buf_cnt].p_buf = NULL;
    p_filter_ext_buf[buf_cnt].p_next = NULL;
    p_filter_ext_buf[buf_cnt].size = 0;
  }
}

/*!
  Build basic structure for EPG data
  */
static void epg_impl_init(void *p_data, u8 max_pti_num)
{
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  MT_ASSERT(p_data != NULL);
   
  p_impl_data->ext_buf_num = MAX_EXT_BUF_NUM;
  p_impl_data->p_ext_buf_list = 
              mtos_malloc(sizeof(filter_ext_buf_t) * p_impl_data->ext_buf_num);
  MT_ASSERT(p_impl_data->p_ext_buf_list != NULL);
  memset(p_impl_data->p_ext_buf_list, 0, sizeof(filter_ext_buf_t) * p_impl_data->ext_buf_num);
}

static BOOL epg_impl_start(void *p_data, u8 block_id)
{
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  p_impl_data->block_id = block_id;
  //Memory reconfig
  impl_mem_cfg(p_data);
  //epg engine config
  p_impl_data->pf_freed_interval = 3000;
  p_impl_data->pf_requested_interval = 300;
  p_impl_data->new_section_num = 1;
  return TRUE;
}

static void epg_impl_stop(void *p_data)
{
  //Release memory partition
  epg_impl_data_t *p_impl_data = (epg_impl_data_t*)p_data;
  MT_ASSERT(p_impl_data != NULL);
  
  //epg_data_deinit(class_get_handle_by_id(EPG_CLASS_ID));
  impl_mem_release(p_data);
}


epg_policy_t *construct_epg_policy4(void)
{
  epg_policy_t    *p_epg_impl = mtos_malloc(sizeof(epg_policy_t));
  epg_impl_data_t *p_imp_data = NULL;
  
  MT_ASSERT(p_epg_impl != NULL);
  memset(p_epg_impl, 0, sizeof(epg_policy_t));

  p_epg_impl->epg_impl_start = epg_impl_start;
  p_epg_impl->epg_impl_stop  = epg_impl_stop;
  p_epg_impl->epg_impl_init  = epg_impl_init;

  p_epg_impl->p_data = mtos_malloc(sizeof(epg_impl_data_t));
  MT_ASSERT(p_epg_impl->p_data != NULL);
  
  memset(p_epg_impl->p_data, 0, sizeof(epg_impl_data_t));
  p_imp_data = (epg_impl_data_t*)p_epg_impl->p_data;
  
  
  return p_epg_impl;
}

