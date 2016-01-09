/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std headers
#include "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"

// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "charsto.h"

#include "uio.h"
#include "smc_op.h"
#include "lib_util.h"
#include "lpower.h"

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"
#include "cas_ware.h"

// ap headers
#include "ap_framework.h"
#include "ap_cas.h"
#include "sys_cfg.h"
#include "hal_misc.h"
#include "customer_config.h"

//#define WRITE_DATA_TO_MEM

extern void cas_manager_config_init(void);
extern BOOL cas_manage_ecm_update(u8 *p_pmt_data, cas_desc_t *p_ecm_info, u16 *p_max);

static void init_ca_module(cas_module_id_t cam_id)
{
  OS_PRINTF("[CA]===>%s, cam_id:%d\n", __FUNCTION__, cam_id);

  if (cam_id == CAS_UNKNOWN)
  {
    cas_module_init(CAS_ID_ONLY1);
  }
  else
  {
    cas_module_init(cam_id);
  }
}

static void def_test_ca_performance(cas_step_t step, u32 ticks)
{
}

static u32 get_msgq_timeout(class_handle_t handle)
{
 	return 30;
}

static void config_ca_data_mem(u8 **p_addr, u32 *p_size)
{
  *p_size = 256 * KBYTES;
  *p_addr = (u8 *)mtos_malloc(*p_size);
  MT_ASSERT(p_addr != NULL);
}

cas_policy_t* construct_cas_policy(void)
{
  cas_policy_t *p_policy = mtos_malloc(sizeof(cas_policy_t));

  MT_ASSERT(p_policy != NULL);

  memset(p_policy, 0, sizeof(cas_policy_t));

  p_policy->on_init = cas_manager_config_init;
  p_policy->test_ca_performance = def_test_ca_performance;
  p_policy->init_ca_module = init_ca_module;
  p_policy->get_msgq_timeout = get_msgq_timeout;
  p_policy->update_ecm_info = cas_manage_ecm_update;
  if(CAS_ID == CONFIG_CAS_ID_DMT)
  {
      p_policy->config_ca_data_mem = config_ca_data_mem;
  }
  OS_PRINTF("[CA]===>%s\n", __FUNCTION__);

  return p_policy;
}


