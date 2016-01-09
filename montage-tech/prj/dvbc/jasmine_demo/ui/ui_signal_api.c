/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"

static BOOL g_is_lock = TRUE;
static struct signal_data g_sign_data = {0, 0, 0, 0, TRUE};
static dvbc_lock_info_t g_tuner_param = {0};
static dvbt_lock_info_t g_dvbt_param = {0};
BOOL is_boot_up = FALSE;
u16 ui_signal_evtmap(u32 event);

void ui_init_signal(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_SIGNAL_MONITOR;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  g_tuner_param.tp_freq = 4125;
  g_tuner_param.tp_sym = 1000;
  g_tuner_param.nim_modulate = NIM_MODULA_AUTO;

  fw_register_ap_evtmap(APP_SIGNAL_MONITOR, ui_signal_evtmap);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_PRO_INFO);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MAINMENU);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MANUAL_SEARCH);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_AUTO_SEARCH);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_RANGE_SEARCH);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_OTA);
}


void ui_enable_signal_monitor(BOOL is_enable)
{
  cmd_t cmd = {0};

  cmd.id = SIG_MON_MONITOR_PERF;
  cmd.data1 = (u32)is_enable;

  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);
}


void ui_release_signal(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_SIGNAL_MONITOR;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_AUTO_SEARCH);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MANUAL_SEARCH);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MAINMENU);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_BACKGROUND);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_PRO_INFO);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_RANGE_SEARCH);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_OTA);
  fw_unregister_ap_evtmap(APP_SIGNAL_MONITOR);
}

void ui_set_transpond(dvbs_tp_node_t *p_tp)
{
  cmd_t cmd = {0};

  trans_lock_tp_info(&g_tuner_param, p_tp);

  cmd.id = SIG_MON_LOCK_TP;
  cmd.data1 = (u32)(&g_tuner_param);
  cmd.data2 = SYS_DVBC;

  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);

  UI_PRINTF("----------------------------------\n");
  UI_PRINTF("- set transpond id = %d\n", p_tp->id);
  UI_PRINTF("---freq = %d\n", p_tp->freq);
  UI_PRINTF("---symbol = %d\n", p_tp->sym);
  UI_PRINTF("----------------------------------\n");
}


//T2
void ui_set_terrestrial_transpond(dvbs_tp_node_t *p_tp)
{
  cmd_t cmd = {0};
  u32  sys_mode = 0;

  sys_mode =  (TUNER0 << 16) | SYS_DVBT2;

  g_dvbt_param.tp_freq = p_tp->freq;
  g_dvbt_param.band_width = p_tp->sym;
  g_dvbt_param.nim_type = p_tp->nim_type;

  cmd.id = SIG_MON_LOCK_TP;
  cmd.data1 = (u32)(&g_dvbt_param);
  cmd.data2 = (u32)sys_mode;

  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);

  UI_PRINTF("--------------ui_set_terrestrial_transpond--------------------\n");
  UI_PRINTF("---nim type = %d\n", g_dvbt_param.nim_type);
  UI_PRINTF("---freq = %d\n", p_tp->freq);
  UI_PRINTF("---symbol = %d\n", p_tp->sym);
  UI_PRINTF("----------------------------------\n");

}

BOOL ui_signal_is_lock(void)
{
  return g_is_lock;
}


void ui_signal_set_lock(BOOL is_lock)
{
  g_is_lock = is_lock;
}

static u8 berc_to_ber(double ber_c)
{
  u8 ber = 0;
  u8 conv_ber[32] = {0};

  memset(conv_ber, 0, sizeof(conv_ber)/sizeof(u8));
  sprintf(conv_ber,"%lf",ber_c);
  //OS_PRINTF("%s\n",conv_ber);

  if(ber_c * 1000000 >= 10000)
  {
    ber = 100;
  }
  else if(ber_c * 1000000 > 5000)
  {
    ber = 90;
  }
  else if(ber_c * 1000000 > 1000)
  {
    ber = 80;
  }
  else if(ber_c * 1000000 > 500)
  {
    ber = 70;
  }
  else if(ber_c * 1000000 > 100)
  {
    ber = 60;
  }
  else if(ber_c * 1000000 > 50)
  {
    ber = 50;
  }
  else
  {
    ber = ber_c * 1000000;
  }

  //OS_PRINTF("%d\n",ber);

  return ber;
}

void ui_signal_check(u32 para1, u32 para2)
{
  menu_attr_t *p_attr;
  control_t *p_root;

  u8 curn_mode;
  u32 tp_freq = 0;
  u8 focus_id = 0;
  static u32 ticks_time = 0;
  static u8 unlock_count = 0;
  sig_mon_info_t *signal = (sig_mon_info_t *)para1;
  
  //OS_PRINTF("g_is_lock:%d, unlock_count:%d\n", g_is_lock, unlock_count);
  if(CUSTOMER_ID == CUSTOMER_JIULIAN)
  {
    //for R836 tuner
    if(!g_is_lock)
    {
      if(unlock_count == 1)
      {
        ticks_time = mtos_ticks_get();
      }
      
      if(unlock_count != 1)
      {
        unlock_count++;
      }
      
      //reset tp if time > 5s
      if(unlock_count == 1 && (mtos_ticks_get() - ticks_time) > 5 * 1000/10)
      {
        OS_PRINTF("ui_reset_cur_tp\n");
        unlock_count = 0;
        ui_reset_cur_tp();
      }
    }
    else
    {
      unlock_count = 0;
    }
  }
  if(signal == NULL)
  	return;
  // check tp 
  focus_id = fw_get_focus_id();
  if(focus_id == ROOT_ID_PROG_BAR ||
  	focus_id == ROOT_ID_AUTO_SEARCH ||
  	focus_id == ROOT_ID_MANUAL_SEARCH ||
  	focus_id == ROOT_ID_RANGE_SEARCH ||
  	!is_boot_up)
  {
    //fix bug 69113
     if(ui_is_playing())
     {
         tp_freq = ui_get_playing_tp();  
         OS_PRINTF("playing pro tp is %d\n",tp_freq);
     } 
     else  
     {  
         tp_freq = g_tuner_param.tp_freq;
         OS_PRINTF("installation is %d\n",tp_freq);	  
     }
     if(tp_freq != signal->tp_freq) 
     { 
  	  OS_PRINTF("not monitor tp %d\n",signal->tp_freq); 
	   return;  
     }
  }
  g_sign_data.ber = berc_to_ber(signal->ber_c);
  g_sign_data.ber_c = signal->ber_c;
  g_sign_data.intensity = signal->strength;
  g_sign_data.quality = signal->snr;
  if((((para2 >> 8) & 0XFF) == 1) || ((para2 & 0X00FF) == 1))
  {
    g_sign_data.lock = TRUE;    
  }
  else
  {
    g_sign_data.lock = FALSE;        
  }

  //OS_PRINTF("intensity %d, quality %d\n", g_sign_data.intensity, g_sign_data.quality);
  p_attr = manage_get_curn_menu_attr();
  p_root = fw_find_root_by_id(p_attr->root_id);

  curn_mode = sys_status_get_curn_prog_mode();

  //OS_PRINTF("signal check g_sign_data.lock[%d], g_is_lock[%d]\n", g_sign_data.lock, g_is_lock);
  if(g_sign_data.lock != g_is_lock)
  {
    g_is_lock = g_sign_data.lock;

    //close screen when no signal in TV mode
    if((!g_is_lock)
      && (curn_mode == CURN_MODE_TV)
      && ui_is_playing())
    {
      void *p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
      
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, FALSE);
    }

    if(p_attr->root_id == ROOT_ID_PVR_REC_BAR)
    {
      //OS_PRINTF("pause or resume record\n");
      //pause or resume record
      if(!g_is_lock)
      {
        ui_recorder_pause();
      }
      else
      {
        ui_recorder_resume();
      }
    }

    update_signal();

    if(p_attr->signal_msg == SM_LOCK)
    {
      if(p_root != NULL)
      {
        ctrl_process_msg(p_root,
                         (u16)(g_is_lock ? MSG_SIGNAL_LOCK : MSG_SIGNAL_UNLOCK),
                         0, 0);
      }
    }
  }

  if(p_attr->signal_msg == SM_BAR)
  {
    if(p_root != NULL)
    {
      ctrl_process_msg(p_root, MSG_SIGNAL_UPDATE,
                       (u32) & g_sign_data, 0);
    }
  }
}

void ui_reset_cur_tp(void)
{
  u16 curn_group;
  u16 prog_id;
  u8 curn_mode;
  u32 context;
  dvbs_prog_node_t pg_node = {0};
  dvbs_tp_node_t p_node;
  
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  if(curn_mode != CURN_MODE_NONE)
  {
    sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);
    if (db_dvbs_get_pg_by_id(prog_id, &pg_node) == DB_DVBS_OK)
    {
      if (db_dvbs_get_tp_by_id((u16)(pg_node.tp_id), &p_node) == DB_DVBS_OK)
      {
         ui_set_transpond(&p_node);
      }
    }
  }
}

BEGIN_AP_EVTMAP(ui_signal_evtmap)
CONVERT_EVENT(SIG_MON_SIGNAL_INFO, MSG_SIGNAL_CHECK)
END_AP_EVTMAP(ui_signal_evtmap)
