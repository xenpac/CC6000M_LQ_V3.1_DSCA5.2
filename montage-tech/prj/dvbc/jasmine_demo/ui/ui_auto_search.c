/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_auto_search.h"
#include "ui_play_api.h"
#include "ui_mainmenu.h"
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
  IDC_SPECIFY_START_SEARCH,
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

static comm_help_data_t2 auto_search_help_data[] = //help bar data
{
   {
    2, 100, {80, 600},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_SCAN,
    },
  },
  {
    2, 100, {80, 600},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_SCAN,
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
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_STA,
    },
  },  
};

u16 auto_search_cont_keymap(u16 key);

RET_CODE auto_search_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

u16 auto_search_select_keymap(u16 key);

RET_CODE auto_search_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 auto_search_tp_frm_keymap(u16 key);
RET_CODE auto_search_tp_frm_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE auto_search_num_edit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE auto_search_qam_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static u16 auto_search_pwdlg_keymap(u16 key);
RET_CODE auto_search_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

static void auto_search_check_signal(control_t *cont, u16 msg, u32 para1, u32 para2)
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

  tp.nim_modulate = nim_modul;

  ui_set_transpond(&tp);

}

static RET_CODE start_auto_scan(control_t *cont, u16 msg, u32 para1, u32 para2)
{
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

  scan_param.tp.freq = freq;
  scan_param.tp.sym = sym;
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
  scan_param.tp.nim_modulate = nim_modul;
#ifdef AISET_BOUQUET_SUPPORT
  scan_param.nit_type = NIT_SCAN_ALL_TP;
#else
#ifdef QUICK_SEARCH_SUPPORT
  scan_param.nit_type = NIT_SCAN_AND_STD_OTR;
#else
  scan_param.nit_type = NIT_SCAN_ONCE;
#endif
#endif
  //ui_init_scan();
  //ui_scan_param_init();
  //ui_scan_param_add_tp(&tp);
  //ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_ALL_TP);

  // start scan
  //ui_start_scan();
    if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_AUTO, (u32)&scan_param);
    else if(g_customer.customer_id == CUSTOMER_JIZHONGMEX)
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_FULL, 0);
    else
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, (u32)&scan_param);

  return SUCCESS;
}

RET_CODE open_auto_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[AUTO_SEARCH_ITEM_CNT], *p_specify_tp_frm, *p_searching_frm;
  control_t *p_bar;
  u8 i = 0;
  u16 y;
  
  u16 stxt[AUTO_SEARCH_ITEM_CNT] =
  { IDS_FREQUENCY, IDS_SYMBOL, IDS_CONSTELLATION};
  dvbc_lock_t tp = {0};
  memset(p_ctrl, 0, sizeof(control_t *) * AUTO_SEARCH_ITEM_CNT);
  sys_status_get_main_tp1(&tp);

  #ifdef QUICK_SEARCH_SUPPORT
  ui_table_monitor_stop_sdt_otr(FALSE, FALSE);
  #endif
  //stop monitor service
  {
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
  }
  p_cont = ui_comm_root_create(ROOT_ID_AUTO_SEARCH,
                               0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,
                               IM_TITLEICON_TV,
                               IDS_AUTO_SEARCH);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, auto_search_cont_keymap);
  ctrl_set_proc(p_cont, auto_search_cont_proc);
  
  //specify tp frm
  p_specify_tp_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SPECIFY_TP_FRM,
                            AUTO_SEARCH_SPECIFY_TP_FRM_X, AUTO_SEARCH_SPECIFY_TP_FRM_Y,
                            AUTO_SEARCH_SPECIFY_TP_FRM_W, AUTO_SEARCH_SPECIFY_TP_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_specify_tp_frm, RSI_AUTO_SEARCH_FRM, RSI_AUTO_SEARCH_FRM, RSI_AUTO_SEARCH_FRM);
  ctrl_set_keymap(p_specify_tp_frm, auto_search_tp_frm_keymap);
  ctrl_set_proc(p_specify_tp_frm, auto_search_tp_frm_proc);
  
  y = AUTO_SEARCH_ITEM_Y;
  for(i = 0; i < AUTO_SEARCH_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
        p_ctrl[i] = ui_comm_numedit_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ + i),
                                             AUTO_SEARCH_ITEM_X, y,
                                             AUTO_SEARCH_ITEM_LW,
                                             AUTO_SEARCH_ITEM_RW);
        ui_comm_ctrl_set_proc(p_ctrl[i], auto_search_num_edit_proc);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_numedit_set_postfix(p_ctrl[i], (i==1) ? IDS_UNIT_SYMB: IDS_UNIT_FREQ);

        if(i==0)
        {
          if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
            ui_comm_numedit_set_param(p_ctrl[0], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 57000, 861000, 6, 0);
          else
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
                                            AUTO_SEARCH_ITEM_X, y,
                                            AUTO_SEARCH_ITEM_LW,
                                            AUTO_SEARCH_ITEM_RW);
        ui_comm_ctrl_set_proc(p_ctrl[i], auto_search_qam_proc);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE,
                                   CBOX_WORKMODE_STATIC, 9,
                                   CBOX_ITEM_STRTYPE_STRID,
                                   NULL);
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

      case 3:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_SPECIFY_TP_FREQ + i),
                           AUTO_SEARCH_ITEM_X, y,
                           AUTO_SEARCH_ITEM_LW + AUTO_SEARCH_ITEM_RW, AUTO_SEARCH_ITEM_H,
                           p_specify_tp_frm, 0);
        ctrl_set_keymap(p_ctrl[i], auto_search_select_keymap);
        ctrl_set_proc(p_ctrl[i], auto_search_select_proc);
        ctrl_set_rstyle(p_ctrl[i],
                        RSI_COMM_CONT_SH,
                        RSI_COMM_CONT_HL,
                        RSI_COMM_CONT_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_font_style(p_ctrl[i],
                            FSI_COMM_TXT_SH,
                            FSI_COMM_TXT_HL,
                            RSI_COMM_TXT_GRAY);
        text_set_align_type(p_ctrl[i], STL_LEFT| STL_VCENTER);
        text_set_content_by_strid(p_ctrl[i], IDS_START_SEARCH);
        text_set_offset(p_ctrl[i], COMM_CTRL_OX, 0);
        break;
        
      default:
        MT_ASSERT(0);
        break;
    }

    ctrl_set_related_id(p_ctrl[i],
                        0, /* left */
                        (u8)((i - 1 + AUTO_SEARCH_ITEM_CNT) % AUTO_SEARCH_ITEM_CNT + 1), /* up */
                        0, /* right */
                        (u8)((i + 1) % AUTO_SEARCH_ITEM_CNT + 1)); /* down */

    y += AUTO_SEARCH_ITEM_H +  AUTO_SEARCH_ITEM_V_GAP;
  }
  
  //searching info frm
  p_searching_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SIGNAL_INFO_FRM,
                            AUTO_SEARCH_SIG_INFO_FRM_X, AUTO_SEARCH_SIG_INFO_FRM_Y,
                            AUTO_SEARCH_SIG_INFO_FRM_W, AUTO_SEARCH_SIG_INFO_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_searching_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE); 
  // pbar
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_STRENGTH_BAR,
                               AUTO_SEARCH_SIG_STRENGTH_PBAR_X,
                               AUTO_SEARCH_SIG_STRENGTH_PBAR_Y,
                               AUTO_SEARCH_SIG_STRENGTH_PBAR_W,
                               AUTO_SEARCH_SIG_STRENGTH_PBAR_H,
                               AUTO_SEARCH_SIG_STRENGTH_NAME_X, 
                               AUTO_SEARCH_SIG_STRENGTH_NAME_Y, 
                               AUTO_SEARCH_SIG_STRENGTH_NAME_W, 
                               AUTO_SEARCH_SIG_STRENGTH_NAME_H,
                               AUTO_SEARCH_SIG_STRENGTH_PERCENT_X,
                               AUTO_SEARCH_SIG_STRENGTH_PERCENT_Y,
                               AUTO_SEARCH_SIG_STRENGTH_PERCENT_W,
                               AUTO_SEARCH_SIG_STRENGTH_PERCENT_H);
  
  ui_comm_bar_set_param(p_bar, IDS_RFLEVEL, 0, 100, 100);

  ui_comm_bar_set_style(p_bar,
                        RSI_AUTO_SEARCH_PBAR_BG, RSI_AUTO_SEARCH_PBAR_MID,
                        RSI_IGNORE, FSI_WHITE,
                        RSI_PBACK, FSI_WHITE);
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dBuv");

  // signal BER pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_BER_BAR,
                               AUTO_SEARCH_SIG_BER_PBAR_X,
                               AUTO_SEARCH_SIG_BER_PBAR_Y,
                               AUTO_SEARCH_SIG_BER_PBAR_W,
                               AUTO_SEARCH_SIG_BER_PBAR_H,
                               AUTO_SEARCH_SIG_BER_NAME_X, 
                               AUTO_SEARCH_SIG_BER_NAME_Y, 
                               AUTO_SEARCH_SIG_BER_NAME_W, 
                               AUTO_SEARCH_SIG_BER_NAME_H,
                               AUTO_SEARCH_SIG_BER_PERCENT_X,
                               AUTO_SEARCH_SIG_BER_PERCENT_Y,
                               AUTO_SEARCH_SIG_BER_PERCENT_W,
                               AUTO_SEARCH_SIG_BER_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_BER, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_AUTO_SEARCH_PBAR_BG, RSI_AUTO_SEARCH_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
  
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"E-6");

  // signal SNR pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_SNR_BAR,
                               AUTO_SEARCH_SIG_SNR_PBAR_X,
                               AUTO_SEARCH_SIG_SNR_PBAR_Y,
                               AUTO_SEARCH_SIG_SNR_PBAR_W,
                               AUTO_SEARCH_SIG_SNR_PBAR_H,
                               AUTO_SEARCH_SIG_SNR_NAME_X, 
                               AUTO_SEARCH_SIG_SNR_NAME_Y, 
                               AUTO_SEARCH_SIG_SNR_NAME_W, 
                               AUTO_SEARCH_SIG_SNR_NAME_H,
                               AUTO_SEARCH_SIG_SNR_PERCENT_X,
                               AUTO_SEARCH_SIG_SNR_PERCENT_Y,
                               AUTO_SEARCH_SIG_SNR_PERCENT_W,
                               AUTO_SEARCH_SIG_SNR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_CN, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_AUTO_SEARCH_PBAR_BG, RSI_AUTO_SEARCH_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);

  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dB");

  ui_comm_help_create2(&auto_search_help_data[0], p_cont,FALSE);
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  ui_enable_signal_monitor(TRUE);
  auto_search_check_signal(p_specify_tp_frm, 0, 0, 0);

  return SUCCESS;
}

static RET_CODE on_auto_search_pwdlg_correct(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();

  return open_auto_search(0, 0);
}

static RET_CODE on_auto_search_pwdlg_exit(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  
  return SUCCESS;
}

RET_CODE preopen_auto_search(u32 para1, u32 para2)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y,
    IDS_MSG_INPUT_PASSWORD,
    auto_search_pwdlg_keymap,
    auto_search_pwdlg_proc,
    PWDLG_T_COMMON
  };
  BOOL is_lock;
  BOOL is_prompt = FALSE;
  
  if (para1 == ROOT_ID_MAINMENU)
  {
    pwdlg_data.left = PWD_DLG_FOR_CHK_X;
    pwdlg_data.top = PWD_DLG_FOR_CHK_Y;
  }

  if((CUSTOMER_AISAT == CUSTOMER_ID) && (COUNTRY_CHINA == CUSTOMER_COUNTRY))
  {   
    sys_status_get_status(BS_MENU_LOCK, &is_lock);
    if(is_lock)
    {
      is_prompt = TRUE;
    } 
  }
  
  if(is_prompt)
    ui_comm_pwdlg_open(&pwdlg_data);
  else
    open_auto_search(0, 0);
  return SUCCESS;
}

static RET_CODE on_auto_search_cont_focus_change_down(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child, *p_parent;
  control_t *p_help;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);
  ctrl_id = (u8)ctrl_get_ctrl_id(p_child);
  p_parent = ctrl_get_parent(p_ctrl);
  p_help = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT);
  if(ctrl_id == 4) 
  ui_comm_help_create2(&auto_search_help_data[0], p_parent,FALSE);
  else 
  ui_comm_help_create2(&auto_search_help_data[ctrl_id], p_parent,FALSE);
  
  ctrl_paint_ctrl(p_help, TRUE);
   
  return ERR_NOFEATURE;
}

static RET_CODE on_auto_search_cont_focus_change_up(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child, *p_parent;
  control_t *p_help;
  u8 ctrl_id;
  p_child = ctrl_get_active_ctrl(p_ctrl);
  ctrl_id = (u8)ctrl_get_ctrl_id(p_child);
  p_parent = ctrl_get_parent(p_ctrl);
  p_help = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT);
  if(ctrl_id == 1)
  ui_comm_help_create2(&auto_search_help_data[3], p_parent,FALSE);
  else
  ui_comm_help_create2(&auto_search_help_data[ctrl_id-2], p_parent,FALSE);
  
 ctrl_paint_ctrl(p_help, TRUE);
 
  return ERR_NOFEATURE;
}

static RET_CODE on_auto_search_signal_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
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

static RET_CODE on_select_msg_start_search(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  start_auto_scan(p_ctrl->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_auto_search_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  auto_search_check_signal(p_ctrl->p_parent->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_auto_search_change_qam(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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
 
  auto_search_check_signal(p_ctrl->p_parent->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_auto_search_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  //ui_reset_cur_tp();
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(auto_search_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(auto_search_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(auto_search_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_auto_search_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_auto_search_exit)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_auto_search_signal_update)
END_MSGPROC(auto_search_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(auto_search_select_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(auto_search_select_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(auto_search_select_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_select_msg_start_search)
END_MSGPROC(auto_search_select_proc, text_class_proc)

BEGIN_KEYMAP(auto_search_tp_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_SEARCH, MSG_EXIT_ALL)
  //ON_EVENT(V_KEY_OK, MSG_START_SCAN)
END_KEYMAP(auto_search_tp_frm_keymap, NULL)

BEGIN_MSGPROC(auto_search_tp_frm_proc, cont_class_proc)
  //ON_COMMAND(MSG_START_SCAN, start_auto_scan)
  ON_COMMAND(MSG_FOCUS_UP,on_auto_search_cont_focus_change_up) 
  ON_COMMAND(MSG_FOCUS_DOWN,on_auto_search_cont_focus_change_down)  
END_MSGPROC(auto_search_tp_frm_proc, cont_class_proc)

BEGIN_MSGPROC(auto_search_num_edit_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_NUMBER, on_auto_search_change_freq_symbol)
END_MSGPROC(auto_search_num_edit_proc, ui_comm_num_proc)

BEGIN_MSGPROC(auto_search_qam_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_auto_search_change_qam)
  ON_COMMAND(MSG_DECREASE, on_auto_search_change_qam)
END_MSGPROC(auto_search_qam_proc, cbox_class_proc)

BEGIN_KEYMAP(auto_search_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(auto_search_pwdlg_keymap, NULL)

BEGIN_MSGPROC(auto_search_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_auto_search_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_auto_search_pwdlg_exit)
END_MSGPROC(auto_search_pwdlg_proc, cont_class_proc)

