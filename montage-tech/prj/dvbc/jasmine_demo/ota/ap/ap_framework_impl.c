/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
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
#include "mosaic.h"
#include "pmt.h"
#include "nit.h"
#include "data_manager.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "db_dvbs.h"
#include "mt_time.h"
#include "audio.h"
#include "video.h"

#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_signal_monitor.h"
//#include "ap_satcodx.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"
#include "i2c.h"
#include "hal_base.h"

#include "lib_util.h"
#include "lpower.h"
//#include "standby.h"
static void ap_proc(void)
{
#ifndef WIN32
  //This will cause hal_pm_reset() not working in concerto platform.
  //hal_watchdog_feed();
#endif
}


BOOL ap_get_standby(void)
{

    return FALSE;
}

void ap_set_standby(u32 flag)
{

}



void ap_test_uart(void)
{
}


static BOOL ap_consume_ui_event(u32 ui_state, os_msg_t *p_msg)
{
  return FALSE;
}


/*!
   Construct APP. framework policy
  */
ap_frm_policy_t *construct_ap_frm_policy(void)
{
  ap_frm_policy_t *p_policy = mtos_malloc(sizeof(ap_frm_policy_t));
  MT_ASSERT(p_policy != NULL);
  memset(p_policy, 0, sizeof(ap_frm_policy_t));

  p_policy->enter_standby = NULL;
  p_policy->extand_proc = ap_proc;
  p_policy->is_standby = ap_get_standby;
  p_policy->resotre_to_factory = NULL;
  p_policy->set_standby = ap_set_standby;
  p_policy->test_uart = ap_test_uart;
  p_policy->consume_ui_event = ap_consume_ui_event;


  return p_policy;
}
