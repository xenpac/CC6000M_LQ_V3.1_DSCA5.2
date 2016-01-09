/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "pti.h"
#include "video.h"
#include "audio.h"
#include "nim.h"
#include "scart.h"
#include "rf.h"
#include "avsync.h"
#include "osd.h"
#include "drv_misc.h"
#include "sub.h"

#include "lib_util.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "sdt.h"
#include "nit.h"
#include "dvb_svc.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "dvbc_util.h"
//#include "vbi.h"

#include "ap_framework.h"
#include "ap_playback.h"
#include "ap_playback_priv.h"

static void default_play(void *p_data, play_param_t *p_play_param)
{
}

static BOOL default_process_pmt_info(void *p_data, pmt_t *p_pmt, 
              u16 *p_v_pid, u16 *p_pcr_pid, u16 *p_a_pid, u16 *p_a_type)
{
  return FALSE;
}

static BOOL default_process_sdt_info(void *p_data, sdt_t *p_sdt)
{
  return FALSE;
}

static BOOL default_process_nit_info(void *p_data, nit_t *p_nit)
{
  return FALSE;
}
  
pb_policy_t *construct_def_ap_playback(void)
{
  pb_policy_t *p_policy = mtos_malloc(sizeof(pb_policy_t));

  p_policy->on_play = default_play;
  p_policy->process_pmt_info = default_process_pmt_info;
  p_policy->process_sdt_info = default_process_sdt_info;
  p_policy->process_nit_info = default_process_nit_info;
  return p_policy;
}

