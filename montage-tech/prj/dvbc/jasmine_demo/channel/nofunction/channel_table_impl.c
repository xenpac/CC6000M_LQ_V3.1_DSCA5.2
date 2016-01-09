/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "sys_regs_jazz.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lib_util.h"
#include "lib_rect.h"
#include "hal_gpio.h"
#include "hal_misc.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "common.h"
#include "drv_dev.h"
#include "nim.h"
#include "uio.h"
#include "hal_watchdog.h"
#include "hal_misc.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "nit.h"
#include "cat.h"
#include "data_manager.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "db_dvbs.h"
#include "mt_time.h"

#include "ss_ctrl.h"
#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_signal_monitor.h"

#include "ap_ota.h"
#include "sys_status.h"
#include "hal_base.h"
#include "smc_op.h"
#include "lpower.h"
#include "config_customer.h"
#include "config_prj.h"
#include "db_dvbs.h"
#include "ap_table_parse.h"
#include "customer_def.h"

void request_channel_sec(dvb_section_t *p_sec, u32 para1, u32 para2)
{
}
void parse_channel_sec(handle_t handle, dvb_section_t *p_sec)
{
}
channel_table_parse_policy_t *construct_channel_table_parse_policy(void)
{
  channel_table_parse_policy_t *p_table_parse_policy = mtos_malloc(sizeof(channel_table_parse_policy_t));
  MT_ASSERT(p_table_parse_policy != NULL);
  memset(p_table_parse_policy, 0, sizeof(channel_table_parse_policy_t));
  p_table_parse_policy->channel_bit = 0;
  p_table_parse_policy->pid = 0;
  p_table_parse_policy->table_id = 0;
  p_table_parse_policy->p_channel_init = NULL;
  p_table_parse_policy->p_channel_process= NULL;
  p_table_parse_policy->p_channel_release = NULL;
  return p_table_parse_policy;
}


void destruct_channel_table_parse_policy(channel_table_parse_policy_t *p_table_parse_policy)
{
  mtos_free(p_table_parse_policy);
}

