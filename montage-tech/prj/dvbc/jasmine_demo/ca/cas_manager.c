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
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_task.h"

// driver
#include "lib_util.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_irq_jazz.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"


#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "spi.h"
#include "driver.h"
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
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
// ap headers
#include "ap_framework.h"
#include "sys_cfg.h"
#include "db_dvbs.h"
#include "ss_ctrl.h"

#include "ui_common.h"
#include "sys_status.h"

//#include "customer_def.h"
#include "customer_config.h"
#ifdef ENABLE_CA
#include "config_cas.h"
#endif
#include "cas_manager.h"

static cas_manager_policy_t *cas_manager_handle = NULL;


BOOL g_is_timeshift_play = FALSE;

void cas_manager_policy_init(void)
{
    cas_manager_handle = construct_cas_manager_policy();
}

cas_manager_policy_t * get_cas_manager_policy_handle(void)
{
  return cas_manager_handle;
}

void cas_manager_config_init(void)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();  
    if((NULL != handle) && (NULL != handle->cas_config_init))
    {	
       handle->cas_config_init();  
    }
}

u8 cas_manager_get_ca_id(void)
{
    cas_manager_policy_t *handle = NULL;
    u8 ca_id = 0;
    handle = get_cas_manager_policy_handle();
    if(NULL != handle)
    {
       ca_id = (u8)handle->ap_cas_id;
    }
    return ca_id;
}

void cas_manager_set_sid(u16 pg_id)
{
    cas_manager_policy_t *handle = NULL;
    //g_is_PVR_play = FALSE;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (NULL != handle->cas_set_sid))
    {
       handle->cas_set_sid(pg_id);
    }
}

void cas_manager_dvb_monitor_start(u16 pg_id)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (NULL != handle->cas_dvb_monitor_start))
    {
       handle->cas_dvb_monitor_start(pg_id);
    }
}

void cas_manager_stop_ca_play(void)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (NULL != handle->cas_play_stop))
    {
       handle->cas_play_stop();
    }
    //g_is_PVR_play = FALSE;
}

void cas_manager_get_ca_card_info(void)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (NULL != handle->cas_request_card_info))
    {
       handle->cas_request_card_info();
    }
}

RET_CODE cas_manager_ca_area_limit_check_start(void)
{
  RET_CODE ret = ERR_FAILURE;
  cas_manager_policy_t *handle = NULL;
  handle = get_cas_manager_policy_handle();
  if((NULL != handle) && (NULL != handle->cas_area_limit_check_start))
  {
     handle->cas_area_limit_check_start();
     return SUCCESS;
  }
  return ret;
}
void cas_manager_ca_auto_receve_email_start(void)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (NULL != handle->cas_auto_receive_email_start))
    {
       handle->cas_auto_receive_email_start();
    }
}
RET_CODE cas_manage_process_menu(ca_input_menu_type type,control_t *p_ctrl,u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_process_menu))
    {
       ret = handle->cas_manage_process_menu(type,p_ctrl,para1,para2);
    }
    return ret;
}

RET_CODE cas_manage_proc_on_normal_cas(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_process_msg))
      {
          return handle->cas_manage_process_msg(p_ctrl,msg,para1,para2);
      }
    else
    {
         return cont_class_proc(p_ctrl,msg,para1,para2);
    }
}

u16 cas_manage_keymap_on_normal_cas(u16 key)
{
    u16 msg = 0;
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_process_keymap))
      {
          return handle->cas_manage_process_keymap(key);
      }
    else
    {
         return msg;
    }
}

void cas_manage_finger_repaint()
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_finger_repaint))
    {
       handle->cas_manage_finger_repaint();
    }
}

void cas_manage_reroll_redraw_finger(u8 from_id,u8 to_id)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_reroll_redraw_finger))
    {
       handle->cas_manage_reroll_redraw_finger(from_id, to_id);
    }
}

void cas_manager_start_pvr_play(u16 index)
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manager_start_pvr_play))
    {
       handle->cas_manager_start_pvr_play(index);
    }
}

RET_CODE cas_manager_get_current_program_ecm_emm_info(u16 *ts_emm_id, u16 *audio_ecm_pid, u16 *video_ecm_pid)
{
    RET_CODE ret = ERR_FAILURE;
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manager_get_current_program_ecm_emm_info))
    {
       ret = handle->cas_manager_get_current_program_ecm_emm_info(ts_emm_id, audio_ecm_pid, video_ecm_pid);
    }

    return ret;
}

RET_CODE cas_manager_get_current_program_ecm_emm(u16 *p_ecm_pid, u16 *p_emm_id)
{
    RET_CODE ret = ERR_FAILURE;
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manager_get_current_program_ecm_emm))
    {
       ret = handle->cas_manager_get_current_program_ecm_emm(p_ecm_pid, p_emm_id);
    }

    return ret;
}

extern BOOL g_is_PVR_play;

BOOL cas_manager_is_pvr_play(void)
{
    return g_is_PVR_play;
}

BOOL cas_manager_is_pvr_forbid()
{
  cas_manager_policy_t *handle = NULL;
  handle = get_cas_manager_policy_handle();
  if((NULL != handle) && (handle->cas_manage_is_pvr_forbid))
  {
    return handle->cas_manage_is_pvr_forbid();
  }
  else
  {
    return FALSE;
  }
}

void cas_manager_set_timeshift_play(BOOL is_timeshift)
{
  g_is_timeshift_play = is_timeshift;
}

void cas_manager_set_pvr_forbid(BOOL pvr_flag)
{
  cas_manager_policy_t *handle = NULL;
  handle = get_cas_manager_policy_handle();
  if((NULL != handle) && (handle->cas_manage_set_pvr_forbid))
  {
    handle->cas_manage_set_pvr_forbid(pvr_flag);
  }
}

void cas_manage_get_discramble_key(u8 *p_key)
{
  cas_manager_policy_t *handle = NULL;
  handle = get_cas_manager_policy_handle();
  if((NULL != handle) && (handle->cas_manage_get_discramble_key))
  {
    handle->cas_manage_get_discramble_key(p_key);
  }
}

void cas_manager_reroll_after_close_all_menu(void)
{
  cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_reroll_after_close_all_menu))
    {
       handle->cas_manage_reroll_after_close_all_menu();
    }
}

void cas_manage_super_osd_repaint()
{
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_super_osd_repaint))
    {
       handle->cas_manage_super_osd_repaint();
    }
}

BOOL cas_manage_ecm_update(u8 *p_pmt_data, cas_desc_t *p_ecm_info, u16 *p_max)
{
    cas_manager_policy_t *handle = NULL;
    BOOL ret = FALSE;
    
    handle = get_cas_manager_policy_handle();
    if((NULL != handle) && (handle->cas_manage_ecm_update))
    {
       ret = handle->cas_manage_ecm_update(p_pmt_data, p_ecm_info, p_max);
    }
    return ret;
}

