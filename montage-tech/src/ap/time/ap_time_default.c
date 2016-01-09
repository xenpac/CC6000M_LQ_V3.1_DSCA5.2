/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "lib_util.h"
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
#include "mosaic.h"
#include "nit.h"
#include "pmt.h"

#include "data_base.h"
#include "data_manager.h"

#include "ap_framework.h"
#include "ap_time.h"
#include "mt_time.h"
#include "mtos_misc.h"

#include "ap_time_default.h"


/*!
  Ap Time impl private data
  */
typedef struct impl_data_tag
{
  /*!
    old time, for time check.
    */
  utc_time_t old_time;
}time_impl_data_t;


static void ap_time_tdt_found(u32 para1, u32 para2)
{
  utc_time_t time;
  memcpy(&time, ((utc_time_t *)para1), sizeof(utc_time_t));
  time_set(&time);
}


static BOOL ap_check_time_update(u32 para1, u32 para2)
{
  time_impl_data_t *p_time_impl_data = (time_impl_data_t *)para1;

  utc_time_t new_time = {0};

  time_get(&new_time, TRUE);

  if(time_cmp(&(p_time_impl_data->old_time), &new_time, TRUE) != 0)
  {
    p_time_impl_data->old_time = new_time;
    return TRUE;
  }

  return FALSE;
}

static void ap_time_lnb_check(u32 para1, u32 para2)
{
#if 0
  time_impl_data_t *p_time_impl_data = (time_impl_data_t *)para1;
  nim_device_t *dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);  
  event_t evt = {TIME_EVT_LNB_SHORT};
  u32 param = NIM_LNB_SC_NO_PROTING;
  u32 cur_tick = mtos_ticks_get();

  if((p_time_impl_data->lnb_check == TRUE) && 
    ((cur_tick - p_time_impl_data->last_tick) >= 50))
  {
//    OS_PRINTF("check lnb\n");
    dev_io_ctrl(dev, NIM_IOCTRL_CHECK_LNB_SC_PROT, (u32)&param);

    p_time_impl_data->last_tick = cur_tick;

    if(param == NIM_LNB_SC_PROTING)
    {
      ap_frm_send_evt_to_ui(APP_TIME, &evt);      
    }
  }
#endif
}

static void ap_time_lnb_check_enable(u32 para1, u32 para2)
{
#if 0
  time_impl_data_t *p_time_impl_data = (time_impl_data_t *)para1;

  OS_PRINTF("enable check lnb[%d]\n", para2);
  p_time_impl_data->lnb_check = (BOOL)para2;
#endif
}

time_policy_t *construct_time_policy_defautl(void)
{
  time_policy_t *p_time_impl = mtos_malloc(sizeof(time_policy_t));

  memset(p_time_impl, 0, sizeof(time_policy_t));
  p_time_impl->on_time_tdt_found  = ap_time_tdt_found;
  p_time_impl->on_time_update_check = ap_check_time_update;
  p_time_impl->on_time_lnb_check = ap_time_lnb_check;
  p_time_impl->on_time_lnb_check_enable = ap_time_lnb_check_enable;
  p_time_impl->p_data = mtos_malloc(sizeof(time_impl_data_t));
  memset(p_time_impl->p_data, 0, sizeof(time_impl_data_t));
  
  return p_time_impl;
}
