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
#include "cas_ware.h"
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
#include "cas_ware.h"
#include "Data_manager.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
// ap headers
#include "ap_framework.h"
#include "ap_cas.h"
#include "sys_cfg.h"
#include "db_dvbs.h"
#include "ss_ctrl.h"

#include "ui_common.h"
#include "sys_status.h"

//#include "customer_def.h"
#include "customer_config.h"
#include "config_cas.h"

#include "ui_mainmenu.h"
//#include "ui_sub_menu.h"
#include "ui_menu_manager.h"

#include "ui_conditional_accept.h"
#include "ui_conditional_accept_info.h"
#include "ui_conditional_accept_pin.h"
#include "ui_conditional_accept_level.h"
#include "ui_conditional_accept_worktime.h"
#include "ui_conditional_accept_antiauth.h"
#include "ui_conditional_accept_update.h"
#include "ui_ca_ippv_ppt_dlg.h"
#include "ui_ca_entitle_info.h"
#include "ui_email_mess.h"
#include "ui_new_mail.h"

#include "ui_signal.h"
#include "ui_ca_public.h"
#include "cas_manager.h"
#include "ui_ca_public.h"
#include "ui_ca_prompt.h"
#include "ui_ca_card_info.h"


static cas_sid_t g_ca_sid;

static void ca_do_cmd(u32 cmd_id, u32 para1)
{
   ui_ca_do_cmd(cmd_id,para1,0);
}
void desai_52_ca_set_sid(u16 pg_id)
{
    cas_sid_t ca_sid;
    play_param_t g_pb_info;
    memset(&g_pb_info, 0, sizeof(play_param_t));   
    memset(&ca_sid, 0, sizeof(cas_sid_t));   
    load_play_paramter_by_pgid(&g_pb_info, pg_id);
    ca_sid.num = 1;
    ca_sid.pgid = pg_id;
    ca_sid.pg_sid_list[0] = g_pb_info.pg_info.s_id;
    ca_sid.tp_freq = g_pb_info.inner.dvbs_lock_info.tp_rcv.freq;
    ca_sid.a_pid = g_pb_info.pg_info.a_pid;
    ca_sid.v_pid = g_pb_info.pg_info.v_pid;
    ca_sid.pmt_pid = g_pb_info.pg_info.pmt_pid;
    ca_sid.ca_sys_id = 0x1FFF;
    
    load_desc_paramter_by_pgid(&ca_sid, pg_id);  
    memcpy(&g_ca_sid, &ca_sid, sizeof(cas_sid_t));
    ca_do_cmd(CAS_CMD_PLAY_INFO_SET,(u32)&g_ca_sid);
}

void desai_52_ca_dvb_monitor_start(u16 pg_id)
{
    play_param_t g_pb_info;
    static m_svc_cmd_p_t param = {0}; 
	
    memset(&g_pb_info, 0, sizeof(play_param_t));   
    load_play_paramter_by_pgid(&g_pb_info, pg_id);
    param.s_id = g_pb_info.pg_info.s_id;
    param.pmt_pid = g_pb_info.pg_info.pmt_pid;
   dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_RESET_CMD, &param);
}

void desai_52_ca_stop_play(void)
{
  //update_ca_message(IDS_E00);
  ca_do_cmd(CAS_CMD_STOP,0);
}

void desai_52_ca_reset_data(void)
{
  ca_do_cmd(CAS_CMD_FACTORY_SET,0);
}

void desai_52_ca_area_limit_check_start(void)
{
  on_ca_zone_check_start(CAS_CMD_ZONE_CHECK,0, 0);
}

void desai_52_ca_get_card_info(void)
{
  ca_do_cmd(CAS_CMD_CARD_INFO_GET,0);
}

void desai_52_ca_auto_receive_email_start(void)
{
   ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0);
}

void desai_52_ca_redraw_after_close_all_menu()
{
  redraw_finger();
}

RET_CODE desai_52_ca_manage_process_menu(ca_input_menu_type type,control_t *p_ctrl, u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;
    switch(type)
    {
      case SUB_MENU_CONDITIONAL:
      ret = manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT, 0, 0);
      break;
      case MAIN_MENU_EMAIL:
      ret = manage_open_menu(ROOT_ID_EMAIL_MESS, 0, 0);
      break;
      case CA_ROLL_ON_DESKTOP:
      ret = open_ca_rolling_menu(para1,para2);
      break;
      case CA_CLOSE_EMAIL:
        if(ui_is_new_mail())
        {
          close_new_mail();
        }
        break;
      case CA_OPEN_EMAIL:
        if(ui_is_new_mail())
        {
          open_ui_new_mail(0, 0);
        }
        break;
      case CA_CLOSE_FINGER_PRINT:
      if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
      {
          fw_tmr_destroy(ROOT_ID_FINGER_PRINT, MSG_CANCEL);
          fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
      }
      break;
      #if 0
      case CA_CLOSE_IPP:
      if(fw_find_root_by_id(ROOT_ID_CA_IPP) != NULL)
      {
        manage_close_menu(ROOT_ID_CA_IPP, 0, 0);
      }
      break;
      #endif
      default:
      break;
    }
    return ret;
}

static menu_attr_t desai_52_ca_menu_attr[] =
{
  //  root_id,            play_state,   auto_close, signal_msg,   open
  {ROOT_ID_CONDITIONAL_ACCEPT, PS_PREV, OFF, SM_OFF, open_conditional_accept},
  {ROOT_ID_EMAIL_MESS, PS_KEEP, OFF, SM_OFF, open_email_mess},
  {ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_ca_card_info},
  {ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, PS_KEEP, OFF, SM_OFF, open_conditional_accept_level},
  {ROOT_ID_CONDITIONAL_ACCEPT_PIN, PS_KEEP, OFF, SM_OFF, open_conditional_accept_pin},
  {ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, PS_KEEP, OFF, SM_OFF, open_conditional_accept_worktime},
  {ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH, PS_KEEP, OFF, SM_OFF, open_conditional_accept_antiauth},
  {ROOT_ID_CONDITIONAL_ACCEPT_INFO, PS_KEEP, OFF, SM_OFF, open_conditional_accept_info},
  {ROOT_ID_CA_IPPV_PPT_DLG, PS_KEEP, OFF, SM_BAR, open_ca_ippv_ppt_dlg},
  {ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_ca_entitle_info},
  {ROOT_ID_CA_CARD_UPDATE, PS_KEEP, OFF, SM_OFF, open_ca_card_update_info},
  //{ROOT_ID_CA_IPP, PS_KEEP, OFF, SM_OFF, open_ca_ipp},
  {ROOT_ID_CA_PROMPT, PS_PLAY, OFF, SM_OFF, open_ca_prompt},
  //{ROOT_ID_PROVIDER_INFO, PS_KEEP, OFF, SM_OFF, open_provider_info},
  //{ROOT_ID_CONDITIONAL_ACCEPT_FEED, PS_KEEP, OFF, SM_OFF, open_conditional_accept_feed},
  //{ROOT_ID_ALERT_MESS, PS_KEEP, OFF, SM_OFF, open_alert_mess},
  //{ROOT_ID_CA_NOTIFY, PS_KEEP, OFF, SM_OFF, open_ca_notify},
  //{ROOT_ID_AUTO_FEED, PS_KEEP, OFF, SM_OFF, open_auto_feed},

 };

  #if 0
static preview_attr_t desai_52_preview_attr[] =
{
  {
    ROOT_ID_CONDITIONAL_ACCEPT, 
    {
      (PREV_OFFSET_X + MAINMENU_PREV_X + PREV_LR_W),
      (PREV_OFFSET_Y + MAINMENU_PREV_Y + PREV_TB_H),
      (PREV_OFFSET_X + MAINMENU_PREV_X + MAINMENU_PREV_W - PREV_LR_W),
      (PREV_OFFSET_Y + MAINMENU_PREV_Y + MAINMENU_PREV_H - PREV_TB_H)
    }, 
  },
  {
    ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 
    {
      (SUBMENU_CONT_X + SUBMENU_PREV_X),
      (SUBMENU_CONT_Y + SUBMENU_PREV_Y),
      (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), 
      (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)
    } 
  },
  {
    ROOT_ID_CONDITIONAL_ACCEPT_PIN, 
    {
      (SUBMENU_CONT_X + SUBMENU_PREV_X),
      (SUBMENU_CONT_Y + SUBMENU_PREV_Y),
      (SUBMENU_CONT_X + SUBMENU_PREV_X + SUBMENU_PREV_W), 
      (SUBMENU_CONT_Y + SUBMENU_PREV_Y + SUBMENU_PREV_H)
    } 
  },
 };

  #endif
static u8 ca_fullscreen_root[] =
{
  ROOT_ID_NEW_MAIL,
  ROOT_ID_FINGER_PRINT,
  ROOT_ID_CA_IPPV_PPT_DLG,
};
cas_manager_policy_t *construct_cas_manager_policy(void)
{
  cas_manager_policy_t *p_policy = mtos_malloc(sizeof(cas_manager_policy_t));
  MT_ASSERT(p_policy != NULL);
  memset(p_policy, 0, sizeof(cas_manager_policy_t));
  
  p_policy->ap_cas_id  = CAS_ID_DS;
  p_policy->cas_cfg_id = CONFIG_CAS_ID_DS;
  
  p_policy->cas_area_limit = TRUE;
  #ifndef WIN32
  p_policy->cas_config_init = on_cas_init;
  #else
  p_policy->cas_config_init = NULL;
  #endif
  p_policy->cas_area_limit_check_start = NULL;
  p_policy->cas_set_sid = desai_52_ca_set_sid;
  p_policy->cas_dvb_monitor_start = desai_52_ca_dvb_monitor_start;
  p_policy->cas_auto_receive_email_start = desai_52_ca_auto_receive_email_start;
  p_policy->cas_play_stop = desai_52_ca_stop_play;
  p_policy->cas_sys_ready = NULL;
  p_policy->cas_reset_data = desai_52_ca_reset_data;
  p_policy->cas_request_card_info = desai_52_ca_get_card_info;
  p_policy->cas_ui_desktop_process = NULL;
  p_policy->cas_manage_process_menu = desai_52_ca_manage_process_menu;
  p_policy->cas_manage_process_msg = ui_desktop_proc_cas;
    p_policy->cas_manage_process_keymap = ui_desktop_keymap_cas;
  p_policy->ca_menu_attr =(menu_attr_t *) desai_52_ca_menu_attr;
  p_policy->attr_number  = (sizeof(desai_52_ca_menu_attr) /sizeof(menu_attr_t));
  p_policy->ca_preview_attr = NULL;
  p_policy->preview_attr_number = 0;
  p_policy->ca_fullscreen_root = (u8 *)ca_fullscreen_root;
  p_policy->ca_fullscreen_root_number = (sizeof(ca_fullscreen_root) /sizeof(u8));
  p_policy->cas_manage_finger_repaint = NULL;
  p_policy->cas_manage_reroll_after_close_all_menu = desai_52_ca_redraw_after_close_all_menu;
 
  
  return p_policy;
}

