/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ota.h"
#include "ui_ota_search.h"

enum control_id
{
  IDC_OTA_INVALID = 0,
  IDC_OTA_FREQ,
  IDC_OTA_SYM,
  IDC_OTA_DEMOD,
  IDC_OTA_PID,
  IDC_OTA_START,
  IDC_SIGNAL_INFO_FRM,
};

enum signal_info_frm_id
{
  IDC_SEARCH_SIG_STRENGTH_BAR = 1,
  IDC_SEARCH_SIG_BER_BAR,
  IDC_SEARCH_SIG_SNR_BAR,
  IDC_SEARCH_SIG_STRENGTH_NAME,
  IDC_SEARCH_SIG_STRENGTH_PBAR,
  IDC_SEARCH_SIG_STRENGTH_PERCENT,
  IDC_SEARCH_SIG_BER_NAME,
  IDC_SEARCH_SIG_BER_PBAR,
  IDC_SEARCH_SIG_BER_PERCENT,
  IDC_SEARCH_SIG_SNR_NAME,
  IDC_SEARCH_SIG_SNR_PBAR,
  IDC_SEARCH_SIG_SNR_PERCENT,
};

static comm_help_data_t2 ota_help_data[] = //help bar data
{
   {
    2, 100, {80, 600},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_FRE,
    },
  },
  {
    2, 100, {80, 600},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_SYM,
    },
  },
   {
    2, 100, {80, 600},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_CONST,
    },
  },
  {
    2, 100, {80, 600},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_PID,
    },
  },  
  {
    2, 100, {80, 600},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_STA,
    },
  },
};


static u16 ota_cont_keymap(u16 key);
static RET_CODE ota_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE ota_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE ota_nbox_proc(control_t *p_nbox, u16 msg, u32 para1, u32 para2);
static RET_CODE ota_qam_proc(control_t *p_nbox, u16 msg, u32 para1, u32 para2);

static void _ui_ota_v_reset_transponder(control_t *p_cont)
{
  control_t *p_freq, *p_sym, *p_dem;
  dvbs_tp_node_t tp_node = {0};  
  u8 demod;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;

  p_freq = ctrl_get_child_by_id(p_cont, IDC_OTA_FREQ);
  p_sym = ctrl_get_child_by_id(p_cont, IDC_OTA_SYM);
  p_dem = ctrl_get_child_by_id(p_cont, IDC_OTA_DEMOD);

  tp_node.freq = ui_comm_numedit_get_num(p_freq);
  tp_node.sym = ui_comm_numedit_get_num(p_sym);
  demod = (u8)ui_comm_select_get_focus(p_dem);
  if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
  {
    switch(demod)
    {
      case 0:
        nim_modul =NIM_MODULA_QAM64;
        break;

      case 1:
        nim_modul = NIM_MODULA_QAM256;      
        break;
        
      default:
        nim_modul = NIM_MODULA_QAM256;
        break;
    }
  }
  else
  {
    switch(demod)
    {
      case 0:
        nim_modul =NIM_MODULA_AUTO;
        break;

      case 1:
        nim_modul = NIM_MODULA_BPSK;      
        break;

      case 2:
        nim_modul = NIM_MODULA_QPSK;      
        break;

      case 3:
        nim_modul = NIM_MODULA_8PSK;      
        break;

      case 4:
        nim_modul = NIM_MODULA_QAM16;
        break;
        
      case 5:
        nim_modul = NIM_MODULA_QAM32;
        break;

      case 6:
        nim_modul = NIM_MODULA_QAM64;      
        break;

      case 7:
        nim_modul = NIM_MODULA_QAM128;
        break;

      case 8:
        nim_modul = NIM_MODULA_QAM256;
        break;
        
      default:
        nim_modul = NIM_MODULA_QAM64;
        break;
    }
  }
  tp_node.nim_modulate = nim_modul;

  ui_set_transpond(&tp_node);

  return;
}

static void trans_ota_tp_info(dvbc_lock_info_t *p_tp_lock, dvbs_tp_node_t *p_tp)
{
  p_tp_lock->tp_sym = p_tp->sym;
  p_tp_lock->tp_freq = p_tp->freq;
  p_tp_lock->nim_modulate = (nim_modulation_t)p_tp->nim_modulate;
}

static void trans_ota_tp_info_dvbt(dvbt_lock_info_t *p_tp_lock, dvbs_tp_node_t *p_tp)
{
  p_tp_lock->band_width = p_tp->sym;
  p_tp_lock->tp_freq = p_tp->freq;
  p_tp_lock->nim_type = NIM_DVBT;//(nim_modulation_t)p_tp->nim_modulate;
}


static RET_CODE _ui_ota_v_select_start(control_t *p_text, u16 msg, u32 para1, u32 para2)
{
  control_t *p_pid, *p_cont, *p_sym, *p_freq, *p_dem;
  ota_info_t upgrade;
  dvbs_tp_node_t tp = {0};
  u8 demod;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
  
  p_cont = ctrl_get_parent(ctrl_get_parent(p_text));
  p_freq = ctrl_get_child_by_id(p_cont, IDC_OTA_FREQ);
  p_sym = ctrl_get_child_by_id(p_cont, IDC_OTA_SYM);
  p_pid = ctrl_get_child_by_id(p_cont, IDC_OTA_PID);
  p_dem = ctrl_get_child_by_id(p_cont, IDC_OTA_DEMOD);

  memcpy((u8 *)&upgrade, (u8 *)sys_status_get_ota_info(), sizeof(ota_info_t));
  upgrade.ota_tri = OTA_TRI_NONE;
  upgrade.download_data_pid = (u16)ui_comm_numedit_get_num(p_pid);
  tp.freq= ui_comm_numedit_get_num(p_freq);
  tp.sym = ui_comm_numedit_get_num(p_sym);
  demod = (u8)ui_comm_select_get_focus(p_dem);
  if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
  {
    switch(demod)
    {
      case 0:
        nim_modul =NIM_MODULA_QAM64;
        break;

      case 1:
        nim_modul = NIM_MODULA_QAM256;      
        break;
        
      default:
        nim_modul = NIM_MODULA_QAM256;
        break;
    }
  }
  else
  {
    switch(demod)
    {
      case 0:
        nim_modul = NIM_MODULA_AUTO;
        break;
        
      case 1:
        nim_modul = NIM_MODULA_QPSK;
        break;

      case 2:
        nim_modul = NIM_MODULA_BPSK;      
        break;

      case 3:
        nim_modul = NIM_MODULA_8PSK;      
        break;

      case 4:
        nim_modul = NIM_MODULA_QAM16;
        break;
        
      case 5:
        nim_modul = NIM_MODULA_QAM32;
        break;

      case 6:
        nim_modul = NIM_MODULA_QAM64;      
        break;

      case 7:
        nim_modul = NIM_MODULA_QAM128;
        break;

      case 8:
        nim_modul = NIM_MODULA_QAM256;
        break;
      default:
        nim_modul = NIM_MODULA_QAM64;
        break;
    }
  }
  tp.nim_modulate = nim_modul;
  if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
  {
    upgrade.sys_mode = SYS_DTMB;
    trans_ota_tp_info_dvbt(&(upgrade.lockt), &tp);
  }
  else
  {
    upgrade.sys_mode = SYS_DVBC;
    trans_ota_tp_info(&(upgrade.lockc), &tp);
  }
  
  OS_PRINTF("%s(Line: %d): (upgrade)ota_tri = %d, download_data_pid = %d.\n", __FUNCTION__, __LINE__, upgrade.ota_tri, upgrade.download_data_pid);
  OS_PRINTF("%s(Line: %d): (tp)freq = %d, polarity = %d, sym = %d.\n", __FUNCTION__, __LINE__, tp.freq, tp.polarity, tp.sym);

  return manage_open_menu(ROOT_ID_OTA_SEARCH, 0, (u32)&upgrade);
}

static RET_CODE _ui_ota_v_download_pid_outrange(control_t *p_nbox, u16 msg, u32 para1, u32 para2)
{
	ui_comm_cfmdlg_open(NULL, IDS_MSG_OUT_OF_RANGE, NULL, 0);
  
  return SUCCESS;
}

static RET_CODE _ui_ota_v_frequency_symbolrate_pid_exit(control_t *p_nbox, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u32 border;

  if(nbox_is_on_edit(p_nbox))
  {
    if(nbox_is_outrange(p_nbox, &border))
    {
      ret = nbox_class_proc(p_nbox, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}

static RET_CODE _ui_ota_v_frequency_symbolrate_change(control_t *p_nbox, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont;

	p_cont = ctrl_get_parent(ctrl_get_parent(p_nbox));
  _ui_ota_v_reset_transponder(p_cont);

	return SUCCESS;
}

static RET_CODE _ui_ota_v_demod_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont;
  u16 f_index;
  f_index = cbox_static_get_focus(p_ctrl);
  switch(msg)
  {
	case MSG_INCREASE:
		if(f_index == 8)
		{
			cbox_static_set_focus(p_ctrl,4);
		       ctrl_paint_ctrl(p_ctrl, FALSE);
		}
		else
		{
			 cbox_class_proc(p_ctrl, msg, para1, para2);
		}
		break;
	case MSG_DECREASE:
		if(f_index == 4)
		{
			cbox_static_set_focus(p_ctrl,8);
		       ctrl_paint_ctrl(p_ctrl, FALSE);
		}
		else
		{
			cbox_class_proc(p_ctrl, msg, para1, para2);
		}
		break;
	default:
		break;
  }
   p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
  _ui_ota_v_reset_transponder(p_cont);

	return SUCCESS;
}

static RET_CODE _ui_ota_v_frequency_symbolrate_pid_focus_left(control_t *p_nbox, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = SUCCESS;
	u32 num, min, max;

	if(nbox_is_on_edit(p_nbox))
	{
		ret = nbox_class_proc(p_nbox, msg, para1, para2);
	}
	else
	{
		num = nbox_get_num(p_nbox);
		nbox_get_range(p_nbox, (s32 *)&min, (s32 *)&max);

		if(num == min)
		{
			num = max;
		}
		else
		{
			num--;
		}
		
		nbox_set_num_by_dec(p_nbox, num);
		ctrl_paint_ctrl(p_nbox, TRUE);
	}
	
	return ret;
}

static RET_CODE _ui_ota_v_frequency_symbolrate_pid_focus_right(control_t *p_nbox, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = SUCCESS;
	u32 num, min, max;

	if(nbox_is_on_edit(p_nbox))
	{
		ret = nbox_class_proc(p_nbox, msg, para1, para2);
	}
	else
	{
		num = nbox_get_num(p_nbox);
		nbox_get_range(p_nbox, (s32 *)&min, (s32 *)&max);

		if(num == max)
		{
		  num = min;
		}
		else
		{
		  num++;
		}
		
		nbox_set_num_by_dec(p_nbox, num);
		ctrl_paint_ctrl(p_nbox, TRUE);
	}
	
	return ret;
}


static RET_CODE _ui_ota_v_frequency_symbolrate_pid_focus_up(control_t *p_nbox, u16 msg, u32 para1, u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_nbox != NULL);
  if((p_nbox->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_nbox, &border);
    nbox_exit_edit(p_nbox);
    if(is_out)
    {
      return SUCCESS;
    }
  }

  return ERR_NOFEATURE;
}

static RET_CODE _ui_ota_v_frequency_symbolrate_pid_focus_down(control_t *p_nbox, u16 msg, u32 para1, u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_nbox != NULL);
  if((p_nbox->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_nbox, &border);
    nbox_exit_edit(p_nbox);
    if(is_out)
    {
      return SUCCESS;
    }
  }
  
  return ERR_NOFEATURE;
}

RET_CODE open_ota(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[OTA_ITEM_CNT],*p_searching_frm, *p_bar;
  u8 i;
  dvbc_lock_t tp = {0};
  u16 y, stxt [OTA_ITEM_CNT] = {IDS_FREQUECY, IDS_SYMBOL,IDS_CONSTELLATION,IDS_DOWNLOAD_PID, 
                                IDS_START};

  sys_status_get_upgrade_tp(&tp);
  memset(p_ctrl, 0, sizeof(control_t *) * OTA_ITEM_CNT);
  //stop monitor service
  {
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
  }

  p_cont = ui_comm_root_create(ROOT_ID_OTA, 0, 
                                            COMM_BG_X, COMM_BG_Y,
                                            COMM_BG_W, COMM_BG_H, 
                                            IM_TITLEICON_TV, IDS_UPGRADE_BY_SAT);
  if(NULL == p_cont)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ota_cont_keymap);
  ctrl_set_proc(p_cont, ota_cont_proc);

  y = OTA_ITEM_Y;
  for(i = 0; i < OTA_ITEM_CNT; i++)
  {
    switch (i)
    {        
      case 2:
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_OTA_FREQ + i),OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_qam_proc);
        if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE,
                                     CBOX_WORKMODE_STATIC, 2,
                                     CBOX_ITEM_STRTYPE_STRID,
                                     NULL);
          ui_comm_select_set_content(p_ctrl[i], 0, IDS_QAM64);
          ui_comm_select_set_content(p_ctrl[i], 1, IDS_QAM256);
          if(tp.nim_modulate == 8)
            ui_comm_select_set_focus(p_ctrl[i], 1);
          else
            ui_comm_select_set_focus(p_ctrl[i], 0);
        }
        else
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE,
                                     CBOX_WORKMODE_STATIC, 9,
                                     CBOX_ITEM_STRTYPE_STRID,
                                     NULL);
          ui_comm_select_set_content(p_ctrl[i], 0, IDS_AUTO);
          ui_comm_select_set_content(p_ctrl[i], 1, IDS_BPSK);
          ui_comm_select_set_content(p_ctrl[i], 2, IDS_QPSK);
          ui_comm_select_set_content(p_ctrl[i], 3, IDS_8PSK);
          ui_comm_select_set_content(p_ctrl[i], 4, IDS_QAM16);
          ui_comm_select_set_content(p_ctrl[i], 5, IDS_QAM32);
          ui_comm_select_set_content(p_ctrl[i], 6, IDS_QAM64);
          ui_comm_select_set_content(p_ctrl[i], 7, IDS_QAM128);
          ui_comm_select_set_content(p_ctrl[i], 8, IDS_QAM256);
          ui_comm_select_set_focus(p_ctrl[i], tp.nim_modulate);
        }
        break;
      case 0:/* freq */
      case 1:/* sym */
      case 3:/* pid */
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_OTA_FREQ + i), OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_nbox_proc);
        if(0 == i)
        {
          if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
            ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 57000, 861000, 6, 0);
          else
            ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 45000, 862000, 6, 0);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
          ui_comm_numedit_set_num(p_ctrl[i], tp.tp_freq);
        }
        else if(1 == i)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 0, 9999, 4, 0);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_SYMB);
          ui_comm_numedit_set_num(p_ctrl[i], tp.tp_sym);
        }
        else
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 0, 9999, 4, 0);
          ui_comm_numedit_set_num(p_ctrl[i], 7000);
        }
        break;
        
      case 4:
        p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_OTA_FREQ + i), OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_text_proc);
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    ctrl_set_related_id(p_ctrl[i],
                        0,                                                  /* left */
                        (u8)((i - 1 + OTA_ITEM_CNT) % OTA_ITEM_CNT + 1),    /* up */
                        0,                                                  /* right */
                        (u8)((i + 1) % OTA_ITEM_CNT + 1));                  /* down */

    y += OTA_ITEM_H + OTA_ITEM_V_GAP;
  }

  //searching info frm
  p_searching_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SIGNAL_INFO_FRM,
                            OTA_SIG_INFO_FRM_X, OTA_SIG_INFO_FRM_Y,
                            OTA_SIG_INFO_FRM_W, OTA_SIG_INFO_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_searching_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE); 
  // pbar
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_STRENGTH_BAR,
                               OTA_SIG_STRENGTH_PBAR_X,
                               OTA_SIG_STRENGTH_PBAR_Y,
                               OTA_SIG_STRENGTH_PBAR_W,
                               OTA_SIG_STRENGTH_PBAR_H,
                               OTA_SIG_STRENGTH_NAME_X, 
                               OTA_SIG_STRENGTH_NAME_Y, 
                               OTA_SIG_STRENGTH_NAME_W, 
                               OTA_SIG_STRENGTH_NAME_H,
                               OTA_SIG_STRENGTH_PERCENT_X,
                               OTA_SIG_STRENGTH_PERCENT_Y,
                               OTA_SIG_STRENGTH_PERCENT_W,
                               OTA_SIG_STRENGTH_PERCENT_H);
  
  ui_comm_bar_set_param(p_bar, IDS_RFLEVEL, 0, 100, 100);

  ui_comm_bar_set_style(p_bar,
                        RSI_OTA_PBAR_BG, RSI_OTA_PBAR_MID,
                        RSI_IGNORE, FSI_WHITE,
                        RSI_PBACK, FSI_WHITE);
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dBuv");

  // signal BER pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_BER_BAR,
                               OTA_SIG_BER_PBAR_X,
                               OTA_SIG_BER_PBAR_Y,
                               OTA_SIG_BER_PBAR_W,
                               OTA_SIG_BER_PBAR_H,
                               OTA_SIG_BER_NAME_X, 
                               OTA_SIG_BER_NAME_Y, 
                               OTA_SIG_BER_NAME_W, 
                               OTA_SIG_BER_NAME_H,
                               OTA_SIG_BER_PERCENT_X,
                               OTA_SIG_BER_PERCENT_Y,
                               OTA_SIG_BER_PERCENT_W,
                               OTA_SIG_BER_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_BER, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_OTA_PBAR_BG, RSI_OTA_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
  
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"E-6");

  // signal SNR pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_SNR_BAR,
                               OTA_SIG_SNR_PBAR_X,
                               OTA_SIG_SNR_PBAR_Y,
                               OTA_SIG_SNR_PBAR_W,
                               OTA_SIG_SNR_PBAR_H,
                               OTA_SIG_SNR_NAME_X, 
                               OTA_SIG_SNR_NAME_Y, 
                               OTA_SIG_SNR_NAME_W, 
                               OTA_SIG_SNR_NAME_H,
                               OTA_SIG_SNR_PERCENT_X,
                               OTA_SIG_SNR_PERCENT_Y,
                               OTA_SIG_SNR_PERCENT_W,
                               OTA_SIG_SNR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_CN, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_OTA_PBAR_BG, RSI_OTA_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);

  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dB");
  
  ui_comm_help_create2(&ota_help_data[0], p_cont,FALSE);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_enable_signal_monitor(TRUE);
  _ui_ota_v_reset_transponder(p_cont);
  return SUCCESS;
}

static RET_CODE on_ota_signal_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_sig_info_frm, *p_strength, *p_snr, *p_ber;
  struct signal_data *data = (struct signal_data *)(para1);

  p_sig_info_frm = ctrl_get_child_by_id(p_cont, IDC_SIGNAL_INFO_FRM);

  p_strength = ctrl_get_child_by_id(p_sig_info_frm, IDC_SEARCH_SIG_STRENGTH_BAR);
  ui_comm_tp_bar_update(p_strength, data->intensity, TRUE, (u8*)"dBuv");
  p_ber = ctrl_get_child_by_id(p_sig_info_frm, IDC_SEARCH_SIG_BER_BAR);
  ui_comm_tp_bar_update(p_ber, data->ber, TRUE, (u8*)"E-6");
  p_snr = ctrl_get_child_by_id(p_sig_info_frm, IDC_SEARCH_SIG_SNR_BAR);
  ui_comm_tp_bar_update(p_snr, data->quality, TRUE, (u8*)"dB");
  
  ctrl_paint_ctrl(p_strength, TRUE);
  ctrl_paint_ctrl(p_ber, TRUE);
  ctrl_paint_ctrl(p_snr, TRUE);

  return SUCCESS;
}

static RET_CODE on_ota_focus_up(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;
  control_t *p_help;
  
  p_child = ctrl_get_active_ctrl(p_cont);
  ctrl_id = (u8)ctrl_get_ctrl_id(p_child);
  p_help = ctrl_get_child_by_id(p_cont, IDC_COMM_HELP_CONT);
  if(ctrl_id == 1)
  ui_comm_help_create2(&ota_help_data[4], p_cont,FALSE);
  else
  ui_comm_help_create2(&ota_help_data[ctrl_id-2], p_cont,FALSE);
  
  ctrl_paint_ctrl(p_help, TRUE);
  
  return ERR_NOFEATURE;
}

static RET_CODE on_ota_focus_down(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_child;
  u8 ctrl_id;
  control_t *p_help;
  
  p_child = ctrl_get_active_ctrl(p_cont);
  ctrl_id = (u8)ctrl_get_ctrl_id(p_child);
  p_help = ctrl_get_child_by_id(p_cont, IDC_COMM_HELP_CONT);
  if(ctrl_id == 5) 
  ui_comm_help_create2(&ota_help_data[0], p_cont,FALSE);
  else 
  ui_comm_help_create2(&ota_help_data[ctrl_id], p_cont,FALSE);
  
  ctrl_paint_ctrl(p_help, TRUE);
  
  return ERR_NOFEATURE;
}

static RET_CODE on_ota_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont;
  
    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    
    ui_comm_num_proc(p_ctrl, msg, para1, para2);
    _ui_ota_v_reset_transponder(p_cont);
    
  return SUCCESS;
}


BEGIN_KEYMAP(ota_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ota_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ota_cont_proc, ui_comm_root_proc)
   ON_COMMAND(MSG_SIGNAL_UPDATE, on_ota_signal_update)
	ON_COMMAND(MSG_FOCUS_UP, on_ota_focus_up)
	ON_COMMAND(MSG_FOCUS_DOWN, on_ota_focus_down)
END_MSGPROC(ota_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(ota_text_proc, text_class_proc)
	ON_COMMAND(MSG_SELECT, _ui_ota_v_select_start)
END_MSGPROC(ota_text_proc, text_class_proc)

BEGIN_MSGPROC(ota_nbox_proc, nbox_class_proc)
    ON_COMMAND(MSG_NUMBER, on_ota_change_freq_symbol)
	ON_COMMAND(MSG_OUTRANGE, _ui_ota_v_download_pid_outrange)
	ON_COMMAND(MSG_UNSELECT, _ui_ota_v_frequency_symbolrate_pid_exit)
	ON_COMMAND(MSG_CHANGED, _ui_ota_v_frequency_symbolrate_change)
	ON_COMMAND(MSG_FOCUS_LEFT, _ui_ota_v_frequency_symbolrate_pid_focus_left)
	ON_COMMAND(MSG_FOCUS_RIGHT, _ui_ota_v_frequency_symbolrate_pid_focus_right)
	ON_COMMAND(MSG_FOCUS_UP, _ui_ota_v_frequency_symbolrate_pid_focus_up)
	ON_COMMAND(MSG_FOCUS_DOWN, _ui_ota_v_frequency_symbolrate_pid_focus_down)
END_MSGPROC(ota_nbox_proc, nbox_class_proc)

BEGIN_MSGPROC(ota_qam_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, _ui_ota_v_demod_change)
  ON_COMMAND(MSG_DECREASE, _ui_ota_v_demod_change)
END_MSGPROC(ota_qam_proc, cbox_class_proc)

