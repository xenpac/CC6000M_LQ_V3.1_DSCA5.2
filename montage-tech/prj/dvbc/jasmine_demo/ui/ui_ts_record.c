/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_ts_record.h"

enum control_id
{
  IDC_SPECIFY_TP_FRM = 1,
  IDC_SIGNAL_INFO_FRM,
};

enum specify_tp_frm_id
{
  IDC_SPECIFY_TP_FREQ = 1,
  IDC_SPECIFY_TP_SYM,
  IDC_SPECIFY_TP_DEMOD,
  IDC_SPECIFY_START_RECORD,
};

enum signal_info_frm_id
{
  IDC_TS_RECORD_SIG_STRENGTH_BAR = 1,
  IDC_TS_RECORD_SIG_BER_BAR,
  IDC_TS_RECORD_SIG_SNR_BAR,
  IDC_TS_RECORD_SIG_STRENGTH_NAME,
  IDC_TS_RECORD_SIG_STRENGTH_PBAR,
  IDC_TS_RECORD_SIG_STRENGTH_PERCENT,
  IDC_TS_RECORD_SIG_BER_NAME,
  IDC_TS_RECORD_SIG_BER_PBAR,
  IDC_TS_RECORD_SIG_BER_PERCENT,
  IDC_TS_RECORD_SIG_SNR_NAME,
  IDC_TS_RECORD_SIG_SNR_PBAR,
  IDC_TS_RECORD_SIG_SNR_PERCENT,
};

static comm_help_data_t2 ts_record_help_data[] = //help bar data
{
   {
    2, 100, {40, 400},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_FRE,
    },
  },
  {
    2, 100, {40, 400},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_SYM,
    },
  },
   {
    2, 100, {40, 400},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_CONST,
    },
  },
  {
    2, 100, {40, 400},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_RECORD,
    },
  },
};

static u32 g_cur_total_time = 0;
static u32 g_cur_rec_time = 0;
static utc_time_t total_rec_time = {0,0,0,2,0,0,0};
static u32 g_uTotal_rec_time = 0;

u16 ts_record_cont_keymap(u16 key);

RET_CODE ts_record_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

u16 ts_record_time_keymap(u16 key);

static RET_CODE ts_record_time_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);

u16 ts_record_tp_frm_keymap(u16 key);
RET_CODE ts_record_tp_frm_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE ts_record_num_edit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE ts_record_qam_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void ts_record_check_signal(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  dvbs_tp_node_t tp = {0};
  control_t *p_ctrl;
  u32 freq;
  u32 sym;
  u8 demod;
  ui_scan_param_t scan_param;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;

  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_FREQ);
  freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);
  
  tp.freq = freq;
  tp.sym = sym;
  
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

  tp.nim_modulate = nim_modul;

  ui_set_transpond(&tp);

}

RET_CODE open_ts_record(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[TS_RECORD_ITEM_CNT], *p_specify_tp_frm, *p_searching_frm;
  control_t *p_bar;
  u8 i = 0, j = 0;
  u16 y;
  u8 separator[TBOX_MAX_ITEM_NUM] = {'-', '-', ' ', ':', ':', ' '};
  
  u16 stxt[TS_RECORD_ITEM_CNT] =
  { IDS_FREQUENCY, IDS_SYMBOL, IDS_CONSTELLATION, IDS_START_RECORD};
  dvbc_lock_t tp = {0};
  sys_status_get_main_tp1(&tp);

   //stop monitor service
  {
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
  }
   
  p_cont = ui_comm_root_create(ROOT_ID_TS_RECORD,
                               0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,
                               IM_TITLEICON_TV,
                               IDS_TS_RECORD);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ts_record_cont_keymap);
  ctrl_set_proc(p_cont, ts_record_cont_proc);
  
  //specify tp frm
  p_specify_tp_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SPECIFY_TP_FRM,
                            TS_RECORD_SPECIFY_TP_FRM_X, TS_RECORD_SPECIFY_TP_FRM_Y,
                            TS_RECORD_SPECIFY_TP_FRM_W, TS_RECORD_SPECIFY_TP_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_specify_tp_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_specify_tp_frm, ts_record_tp_frm_keymap);
  ctrl_set_proc(p_specify_tp_frm, ts_record_tp_frm_proc);
  
  memset(p_ctrl, 0, 4 * TS_RECORD_ITEM_CNT);
  y = TS_RECORD_ITEM_Y;
  for(i = 0; i < TS_RECORD_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
        p_ctrl[i] = ui_comm_numedit_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ + i),
                                             TS_RECORD_ITEM_X, y,
                                             TS_RECORD_ITEM_LW,
                                             TS_RECORD_ITEM_RW);
        ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_num_edit_proc);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_numedit_set_postfix(p_ctrl[i], (i==1) ? IDS_UNIT_SYMB: IDS_UNIT_FREQ);

        if(i==0)
        {
          ui_comm_numedit_set_param(p_ctrl[0], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 45000, 862000, 6, 0);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[0], tp.tp_freq);
        }
        else
        {
          ui_comm_numedit_set_param(p_ctrl[1], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 0, 9999, 4, 0);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[1], tp.tp_sym);
        }
        break;
        
      case 2:
        p_ctrl[i] = ui_comm_select_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ + i),
                                            TS_RECORD_ITEM_X, y,
                                            TS_RECORD_ITEM_LW,
                                            TS_RECORD_ITEM_RW);
        ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_qam_proc);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
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
        break;
        
      case 3:
        //duration
        p_ctrl[i] = ui_comm_timedit_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ + i),
                                        TS_RECORD_ITEM_X, y, TS_RECORD_ITEM_LW, TS_RECORD_ITEM_RW);
        ui_comm_ctrl_set_keymap(p_ctrl[i], ts_record_time_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_time_proc);
        ui_comm_timedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_timedit_set_time(p_ctrl[i], &total_rec_time);
        ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN | TBOX_SECOND, TBOX_SEPARATOR_TYPE_UNICODE, 18);
        for(j = 0; j < TBOX_MAX_ITEM_NUM; j++)
        {
          ui_comm_timedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, separator[j]);
        }
        break;
        
      default:
        MT_ASSERT(0);
        break;
    }

    ctrl_set_related_id(p_ctrl[i],
                        0, /* left */
                        (u8)((i - 1 + TS_RECORD_ITEM_CNT) % TS_RECORD_ITEM_CNT + 1), /* up */
                        0, /* right */
                        (u8)((i + 1) % TS_RECORD_ITEM_CNT + 1)); /* down */

    y += TS_RECORD_ITEM_H +  TS_RECORD_ITEM_V_GAP;
  }
  
  //searching info frm
  p_searching_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SIGNAL_INFO_FRM,
                            TS_RECORD_SIG_INFO_FRM_X, TS_RECORD_SIG_INFO_FRM_Y,
                            TS_RECORD_SIG_INFO_FRM_W, TS_RECORD_SIG_INFO_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_searching_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE); 
  // pbar
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_TS_RECORD_SIG_STRENGTH_BAR,
                               TS_RECORD_SIG_STRENGTH_PBAR_X,
                               TS_RECORD_SIG_STRENGTH_PBAR_Y,
                               TS_RECORD_SIG_STRENGTH_PBAR_W,
                               TS_RECORD_SIG_STRENGTH_PBAR_H,
                               TS_RECORD_SIG_STRENGTH_NAME_X, 
                               TS_RECORD_SIG_STRENGTH_NAME_Y, 
                               TS_RECORD_SIG_STRENGTH_NAME_W, 
                               TS_RECORD_SIG_STRENGTH_NAME_H,
                               TS_RECORD_SIG_STRENGTH_PERCENT_X,
                               TS_RECORD_SIG_STRENGTH_PERCENT_Y,
                               TS_RECORD_SIG_STRENGTH_PERCENT_W,
                               TS_RECORD_SIG_STRENGTH_PERCENT_H);
  
  ui_comm_bar_set_param(p_bar, IDS_CN, 0, 100, 100);

  ui_comm_bar_set_style(p_bar,
                        RSI_TS_RECORD_PBAR_BG, RSI_TS_RECORD_PBAR_MID,
                        RSI_IGNORE, FSI_WHITE,
                        RSI_PBACK, FSI_WHITE);
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dBuv");

  // signal BER pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_TS_RECORD_SIG_BER_BAR,
                               TS_RECORD_SIG_BER_PBAR_X,
                               TS_RECORD_SIG_BER_PBAR_Y,
                               TS_RECORD_SIG_BER_PBAR_W,
                               TS_RECORD_SIG_BER_PBAR_H,
                               TS_RECORD_SIG_BER_NAME_X, 
                               TS_RECORD_SIG_BER_NAME_Y, 
                               TS_RECORD_SIG_BER_NAME_W, 
                               TS_RECORD_SIG_BER_NAME_H,
                               TS_RECORD_SIG_BER_PERCENT_X,
                               TS_RECORD_SIG_BER_PERCENT_Y,
                               TS_RECORD_SIG_BER_PERCENT_W,
                               TS_RECORD_SIG_BER_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_BER, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_TS_RECORD_PBAR_BG, RSI_TS_RECORD_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
  
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"E-6");

  // signal SNR pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_TS_RECORD_SIG_SNR_BAR,
                               TS_RECORD_SIG_SNR_PBAR_X,
                               TS_RECORD_SIG_SNR_PBAR_Y,
                               TS_RECORD_SIG_SNR_PBAR_W,
                               TS_RECORD_SIG_SNR_PBAR_H,
                               TS_RECORD_SIG_SNR_NAME_X, 
                               TS_RECORD_SIG_SNR_NAME_Y, 
                               TS_RECORD_SIG_SNR_NAME_W, 
                               TS_RECORD_SIG_SNR_NAME_H,
                               TS_RECORD_SIG_SNR_PERCENT_X,
                               TS_RECORD_SIG_SNR_PERCENT_Y,
                               TS_RECORD_SIG_SNR_PERCENT_W,
                               TS_RECORD_SIG_SNR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_RFLEVEL, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_TS_RECORD_PBAR_BG, RSI_TS_RECORD_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);

  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dB");

  ui_comm_help_create2(&ts_record_help_data[0], p_cont,FALSE);
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  ui_enable_signal_monitor(TRUE);
  ts_record_check_signal(p_specify_tp_frm, 0, 0, 0);

  return SUCCESS;
}


static RET_CODE on_ts_record_cont_focus_change_down(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child, *p_parent;
  u16 ctrl_id;
  control_t *p_help;
  
  p_child = ctrl_get_active_ctrl(p_ctrl);
  ctrl_id = ctrl_get_ctrl_id(p_child);
  p_parent = ctrl_get_parent(p_ctrl);
  p_help = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT);
  if(ctrl_id == 5) 
  ui_comm_help_create2(&ts_record_help_data[0], p_parent,FALSE);
  else 
  ui_comm_help_create2(&ts_record_help_data[ctrl_id], p_parent,FALSE);
  
  ctrl_paint_ctrl(p_help, TRUE);
   
  return ERR_NOFEATURE;
}

static RET_CODE on_ts_record_cont_focus_change_up(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child, *p_parent;
  u16 ctrl_id;
  control_t *p_help;
  
  p_child = ctrl_get_active_ctrl(p_ctrl);
  ctrl_id = ctrl_get_ctrl_id(p_child);
  p_parent = ctrl_get_parent(p_ctrl);
  p_help = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT);

  if(ctrl_id == 1)
  ui_comm_help_create2(&ts_record_help_data[4], p_parent,FALSE);
  else
  ui_comm_help_create2(&ts_record_help_data[ctrl_id-2], p_parent,FALSE);
  
  ctrl_paint_ctrl(p_help, TRUE);

  return ERR_NOFEATURE;
}

static RET_CODE on_ts_record_signal_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_sig_info_frm, *p_strength, *p_snr, *p_ber;
  struct signal_data *data = (struct signal_data *)(para1);

  p_sig_info_frm = ctrl_get_child_by_id(p_cont, IDC_SIGNAL_INFO_FRM);

  p_strength = ctrl_get_child_by_id(p_sig_info_frm, IDC_TS_RECORD_SIG_STRENGTH_BAR);
  ui_comm_tp_bar_update(p_strength, data->intensity, TRUE, (u8*)"dBuv");
  p_ber = ctrl_get_child_by_id(p_sig_info_frm, IDC_TS_RECORD_SIG_BER_BAR);
  ui_comm_tp_bar_update(p_ber, data->ber, TRUE, (u8*)"E-6");
  p_snr = ctrl_get_child_by_id(p_sig_info_frm, IDC_TS_RECORD_SIG_SNR_BAR);
  ui_comm_tp_bar_update(p_snr, data->quality, TRUE, (u8*)"dB");
  
  ctrl_paint_ctrl(p_strength, TRUE);
  ctrl_paint_ctrl(p_ber, TRUE);
  ctrl_paint_ctrl(p_snr, TRUE);

  return SUCCESS;
}

static RET_CODE on_ts_record_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  ts_record_check_signal(p_ctrl->p_parent->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_ts_record_change_qam(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
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

  ts_record_check_signal(p_ctrl->p_parent->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_pvr_rec_stopped(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  g_cur_total_time += g_cur_rec_time;
  ui_recorder_up_devinfo();
  g_cur_rec_time = 0;

  return SUCCESS;
}
static RET_CODE on_pvr_rec_mem_not_enough(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if(ui_recorder_isrecording())
  {
    ui_recorder_end();
    ui_recorder_stop();

    MT_ASSERT(ui_recorder_release() == SUCCESS);
  }
  
  return SUCCESS;
}
static RET_CODE on_pvr_rec_write_error(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  if(ui_recorder_isrecording())
  {
    ui_recorder_end();
    ui_recorder_stop();

    MT_ASSERT(ui_recorder_release() == SUCCESS);
  }
  
  return SUCCESS;
}
static RET_CODE on_pvr_rec_no_signal(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}
static RET_CODE on_pvr_rec_updated(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_time = NULL, *p_start = NULL, *p_tp_frm = NULL;
  utc_time_t rec_time = {0};
  u32 cur_rec_time = 0;

  p_tp_frm = ctrl_get_child_by_id(p_ctrl, IDC_SPECIFY_TP_FRM);
  MT_ASSERT(p_tp_frm != NULL);

  
  g_cur_rec_time = para1 / 1000;
  if(g_cur_rec_time + g_cur_total_time >= g_uTotal_rec_time)
  {
     if(ui_recorder_isrecording())
     {
       ui_recorder_end();
       ui_recorder_stop();
       MT_ASSERT(ui_recorder_release() == SUCCESS);
      }
    
     ui_comm_cfmdlg_open(NULL, IDS_RECORD_FINISH, NULL, 2000);

     p_start = ctrl_get_child_by_id(p_tp_frm, IDC_SPECIFY_START_RECORD);
     if(p_start)
     {
       ui_comm_timedit_set_static_txt(p_start, IDS_START);
       ctrl_paint_ctrl(p_start, TRUE);
     }
  }
  else
  {
      p_time = ctrl_get_child_by_id(p_tp_frm, IDC_SPECIFY_START_RECORD);
      if(p_time == NULL)
      {
        return ERR_FAILURE;
      }

      cur_rec_time = para1 / 1000;
      rec_time.hour = (u8)(cur_rec_time / 3600);
      rec_time.minute = (u8)((cur_rec_time % 3600) / 60);
      rec_time.second = (u8)(cur_rec_time % 60);    
      ui_comm_timedit_set_time(p_time, &rec_time);

      ctrl_paint_ctrl(p_time, TRUE);
  }


  
  return SUCCESS;
}


static RET_CODE on_ts_record_exit_edit_sts(control_t *p_text, u16 msg, u32 para1,
                             	   u32 para2)
{
  control_t  *p_time = NULL;

  p_time = ctrl_get_parent(p_text);
  if((p_time) && (ui_comm_timedit_get_is_on_edit(p_time)))
  {
    tbox_exit_edit(p_text);
  }
  else
  {
    ctrl_process_msg(ctrl_get_parent(ctrl_get_parent(p_time)), MSG_EXIT, 0, 0);
  }
 
  return SUCCESS;
}                             	   
//lint -e438 -e830 start record
static RET_CODE on_ts_record_select(control_t *p_text, u16 msg, u32 para1,
                             	   u32 para2)
{

  u8 file_name[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
  u16 filename[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
  u8 asc_tmp[64] = {0};
  u16 uni_tmp[64] = {0};
  utc_time_t gmt_time = {0};
  partition_t *p_partition = NULL;
  hfile_t file = NULL;
  mul_pvr_rec_attr_t rec_attr = {{0}};

  control_t  *p_cont = NULL, *p_freq = NULL, *p_sym = NULL; 
  control_t  *p_time = NULL;

  p_time = ctrl_get_parent(p_text);
  if((p_time) && (ui_comm_timedit_get_is_on_edit(p_time)))
  {
    tbox_exit_edit(p_text);
  }
  
  //if recording stop, else update string.
  if(ui_recorder_isrecording())
  {
     ui_recorder_end();
     ui_recorder_stop();
     MT_ASSERT(ui_recorder_release() == SUCCESS);
      
     ui_comm_timedit_set_static_txt(ctrl_get_parent(p_text), IDS_START);
     ctrl_paint_ctrl(ctrl_get_parent(p_text), TRUE);

     return SUCCESS;
  }
  else
  {
     ui_comm_timedit_set_static_txt(ctrl_get_parent(p_text), IDS_STOP);
     ctrl_paint_ctrl(ctrl_get_parent(p_text), TRUE);
  }

  p_cont = ctrl_get_parent(ctrl_get_parent(p_text));  
  p_freq = ctrl_get_child_by_id(p_cont, IDC_SPECIFY_TP_FREQ);
  p_sym = ctrl_get_child_by_id(p_cont, IDC_SPECIFY_TP_SYM);
  ctrl_get_child_by_id(p_cont, IDC_SPECIFY_TP_DEMOD);

  sprintf((char *)file_name, "%ld-%ld", ui_comm_numedit_get_num(p_freq), ui_comm_numedit_get_num(p_sym));
  
  //check if can record
  OS_PRINTF("@@@@on_ts_record_select, para2 == %d@@@@@\n",para2);
  if(para2 != 0)
  {
    memcpy(&total_rec_time, (u32 *)para1, sizeof(utc_time_t));
  }
  else
  {
     control_t *p_start = NULL;
     memset(&total_rec_time, 0, sizeof(utc_time_t));

     p_start = ctrl_get_child_by_id(p_cont, IDC_SPECIFY_START_RECORD);
     ui_comm_timedit_get_time(p_start, &total_rec_time);
  }
  
  g_cur_rec_time = 0;
  g_cur_total_time = 0;

  memset(&rec_attr, 0, sizeof(mul_pvr_rec_attr_t));
  rec_attr.media_info.v_pid = 0;//(u16)prog.video_pid;
  rec_attr.b_clear_stream = TRUE;
  rec_attr.media_info.extern_num = 0;//2;

  OS_PRINTF("@@@at on_ts_record_select_function, before time_conver g_uTotal_rec_time = %d@@\n",g_uTotal_rec_time);
  OS_PRINTF("@@@@total_rec_time year = %d@@@\n",total_rec_time.year);
  g_uTotal_rec_time = time_conver(&total_rec_time);
  OS_PRINTF("@@@after time_conver g_uTotal_rec_time = %d@@\n",g_uTotal_rec_time);

  OS_PRINTF("\n##debug: 0x%x 0x%x 0x%x!\n",
    rec_attr.media_info.v_pid, rec_attr.media_info.a_pid, rec_attr.media_info.pcr_pid);
  time_get(&gmt_time, TRUE);
  time_to_local(&gmt_time, &(rec_attr.start));
    
  if (file_list_get_partition(&p_partition) > 0)
  {
    memset(rec_attr.file_name, 0, sizeof(u16) * PVR_MAX_FILENAME_LEN);
    
    uni_strcat(rec_attr.file_name, 
      p_partition[sys_status_get_usb_work_partition()].letter, PVR_MAX_FILENAME_LEN);

    str_asc2uni((u8 *)"\\", uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

    
    str_asc2uni((u8 *)PVR_DIR, uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

    str_asc2uni((u8 *)"\\", uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);    
    
    str_asc2uni(file_name, filename);
    uni_strcat(rec_attr.file_name, filename, PVR_MAX_FILENAME_LEN);    
      
    sprintf((char *)asc_tmp, "-%.4d-%.2d-%.2d %.2d-%.2d-%.2d.ts",
        rec_attr.start.year,
        rec_attr.start.month,
        rec_attr.start.day,
        rec_attr.start.hour,
        rec_attr.start.minute,
        rec_attr.start.second);
        
    str_asc2uni(asc_tmp, uni_tmp);
    uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

    ui_recorder_init();
    
    file = vfs_open(rec_attr.file_name, VFS_NEW);
    if(file == NULL)
    {
      memset(rec_attr.file_name, 0, sizeof(u16) * PVR_MAX_FILENAME_LEN);
      uni_strcat(rec_attr.file_name, 
        p_partition[sys_status_get_usb_work_partition()].letter, PVR_MAX_FILENAME_LEN);

      str_asc2uni((u8 *)"\\", uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
    
      str_asc2uni((u8 *)PVR_DIR, uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
      
      str_asc2uni((u8 *)"\\", uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
      
      str_asc2uni((u8 *)"No_Name", uni_tmp);

      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);    
        
      sprintf((char *)asc_tmp, "-%.4d-%.2d-%.2d %.2d-%.2d-%.2d.ts",
          rec_attr.start.year,
          rec_attr.start.month,
          rec_attr.start.day,
          rec_attr.start.hour,
          rec_attr.start.minute,
          rec_attr.start.second);

      str_asc2uni(asc_tmp, uni_tmp);
      uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
    }
    else
    {
      vfs_close(file);
      file = NULL;
    }
    rec_attr.file_name_len = uni_strlen(rec_attr.file_name);
    rec_attr.max_file_size = (u64)p_partition->free_size * (u64)KBYTES;
    OS_PRINTF("*****free size = %lld\n",rec_attr.max_file_size);
  }
  else
  {
    //no available storage found
    ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 0);
    return SUCCESS;
  }

  rec_attr.stream_type = MUL_PVR_STREAM_TYPE_ALL_TS;
  if(ui_recorder_start(&rec_attr) == ERR_FAILURE)
  {
    ui_recorder_end();
    ui_recorder_stop();
    ui_recorder_release();
    return ERR_FAILURE;
  }

  return SUCCESS;
}
//lint +e438 +e830

static RET_CODE on_ts_record_destroy(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  
  if(ui_recorder_isrecording())
  {
    ui_recorder_end();
    ui_recorder_stop();

    MT_ASSERT(ui_recorder_release() == SUCCESS);
  }
 
  return ERR_NOFEATURE;
}

static RET_CODE on_ts_rec_plug_out(control_t *p_ctrl, u16 msg,
							u32 para1, u32 para2)
{
  if(ui_recorder_isrecording())
  {
    ui_recorder_end();
    ui_recorder_stop();

    MT_ASSERT(ui_recorder_release() == SUCCESS);
  }

  manage_close_menu(ROOT_ID_TS_RECORD, 0, 0);
  
  return SUCCESS;
}		


BEGIN_KEYMAP(ts_record_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(ts_record_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ts_record_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_ts_record_destroy)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_ts_record_signal_update)
  ON_COMMAND(MSG_PVR_REC_STOPPED, on_pvr_rec_stopped)
  ON_COMMAND(MSG_PVR_REC_MEM_NOT_ENOUGH, on_pvr_rec_mem_not_enough)
  ON_COMMAND(MSG_WRITE_ERROR, on_pvr_rec_write_error)
  ON_COMMAND(MSG_PVR_REC_NO_SIGNAL, on_pvr_rec_no_signal)
  ON_COMMAND(MSG_PVR_REC_UPDATED, on_pvr_rec_updated)	
  ON_COMMAND(MSG_PLUG_OUT, on_ts_rec_plug_out)  
END_MSGPROC(ts_record_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ts_record_tp_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(ts_record_tp_frm_keymap, NULL)

BEGIN_MSGPROC(ts_record_tp_frm_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP,on_ts_record_cont_focus_change_up) 
  ON_COMMAND(MSG_FOCUS_DOWN,on_ts_record_cont_focus_change_down)  
END_MSGPROC(ts_record_tp_frm_proc, cont_class_proc)

BEGIN_MSGPROC(ts_record_num_edit_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_NUMBER, on_ts_record_change_freq_symbol)
END_MSGPROC(ts_record_num_edit_proc, ui_comm_num_proc)

BEGIN_MSGPROC(ts_record_qam_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_ts_record_change_qam)
  ON_COMMAND(MSG_DECREASE, on_ts_record_change_qam)
END_MSGPROC(ts_record_qam_proc, cbox_class_proc)

BEGIN_KEYMAP(ts_record_time_keymap, ui_comm_tbox_keymap)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ts_record_time_keymap, ui_comm_tbox_keymap)

BEGIN_MSGPROC(ts_record_time_proc, ui_comm_time_proc)
	ON_COMMAND(MSG_SELECT, on_ts_record_select)
	ON_COMMAND(MSG_EXIT, on_ts_record_exit_edit_sts)
END_MSGPROC(ts_record_time_proc, ui_comm_time_proc)
