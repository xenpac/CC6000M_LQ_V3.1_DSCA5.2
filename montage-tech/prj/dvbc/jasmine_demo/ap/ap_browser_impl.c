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
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"

#include "mtos_msg.h"
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
#include "monitor_service.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"

#include "Data_manager.h"
// ap headers
#include "ap_framework.h"

#include "sys_cfg.h"
#include "db_dvbs.h"
//#include "config_customer.h"
//#include"config_prj.h"
#ifndef WIN32
#include "browser_adapter.h"
#endif
#include "ap_browser.h"
//#define WRITE_DATA_TO_MEM
#if 0
static void cas_performance(cas_step_t step, u32 ticks)
{
  UI_PRINTF("ap cas cur step %d, tick%d\n", step, ticks);
}

#endif
#if 0
#ifdef JAZZ_BROWSER_FUNCTION
static RET_CODE browser_get_freq(u16 net_id,u16 ts_id,u16 service_id,u32 *freq)
{

   u8 view_id;
   u16 prog_pos = 0, prog_id = 0,pg_cnt = 0,tp_id = 0;
   dvbs_prog_node_t pg;
   dvbs_tp_node_t tp;
	OS_PRINTF("net_id :%d,ts_id:%d, service_id: %d\n",net_id,ts_id,service_id);
    view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
    pg_cnt = db_dvbs_get_count(view_id);
    for(;prog_pos <pg_cnt;prog_pos++)
    {
      prog_id = db_dvbs_get_id_by_view_pos(view_id, prog_pos);
      db_dvbs_get_pg_by_id(prog_id, &pg);
      if(pg.orig_net_id == net_id && pg.ts_id == ts_id &&
         pg.s_id == service_id)
        {
          tp_id =(u16) pg.tp_id;
          break;
        }
     }
   // MT_ASSERT(prog_pos < pg_cnt);
    db_dvbs_get_tp_by_id(tp_id, &tp);
	*freq = tp.freq;
    return SUCCESS;
}

#endif
#endif
extern RET_CODE kvdb_ipanel_attach(browser_module_cfg_t *p_cfg);
void on_browser_init(void)
{

  browser_module_cfg_t browser_module_cfg = {0};
 // browser_module_cfg.browser_get_freq= browser_get_freq;
  browser_module_cfg.task_prio = BROWSER_APP_TASK_START;
 browser_module_cfg.task_end_pro = BROWSER_APP_TASK_END;
  kvdb_ipanel_attach(&browser_module_cfg);
  browser_module_init(AP_BROWSER_ID);
}
browser_policy_t* construct_browser_policy(void)
{
  browser_policy_t *p_policy = mtos_malloc(sizeof(browser_policy_t));
  MT_ASSERT(p_policy != NULL);
  memset(p_policy, 0, sizeof(browser_policy_t));

  p_policy->on_init = on_browser_init;
  p_policy->test_browser_performance = NULL;
  return p_policy;
}



