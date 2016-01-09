/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_terrestrial_manual_search.h"
#include "ui_signal_api.h"
#include "sys_status.h"


enum control_id
{
  IDC_MANUAL_SEARCH_INVALID = 0,
  IDC_MANUAL_SEARCH_FREQUENCY,
  IDC_MANUAL_SEARCH_MANUAL_SEARCH,
  IDC_HELP_CONT,
};

enum manual_search_local_msg
{
  MSG_SEARCH = MSG_LOCAL_BEGIN + 135,
  MSG_CHECK_FREQUENCY_CHANGE,
};

u16 manual_search_keymap(u16 key);
RET_CODE manual_search_proc(control_t *p_btn, u16 msg, u32 para1, u32 para2);
RET_CODE manual_search_frequency_change_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static BOOL _ui_manual_search_c_init(void)
{
  u32 scan_freq = 0;
  dvbs_tp_node_t tp = {0};
  sat_node_t sat_node = {0};
  control_t *p_freq = NULL;
  BOOL lcn_enable = FALSE;
  
  //t2_search_info search_info = {0};

  /* todo: need update country */
  //sys_status_get_country_set(&search_info);
  sys_status_get_status(BS_LCN_RECEIVED, &lcn_enable);
  p_freq = ui_comm_root_get_ctrl(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, IDC_MANUAL_SEARCH_FREQUENCY);
  scan_freq = (u32)ui_comm_numedit_get_num(p_freq);
  
  ui_scan_param_init();

  tp.freq = scan_freq;
  tp.sym = DTMB_HN_SYMBOL_DEFAULT;
  tp.sat_id = sat_node.id;
  tp.nim_type = NIM_DTMB;
  
  ui_scan_param_add_tp(&tp);
  if(TRUE == lcn_enable)
  {
    ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_ALL_TP);
  }
  else
  {
    ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
  }

  return TRUE;
}

extern void ui_set_terrestrial_transpond(dvbs_tp_node_t *p_tp);
static BOOL _ui_manual_search_c_lock_tp(u32 frequency)
{
  dvbs_tp_node_t tp = {0};
  sat_node_t sat_node = {0};

  tp.freq = frequency;
  tp.sym = DTMB_HN_SYMBOL_DEFAULT;
  tp.sat_id = sat_node.id;
  tp.nim_type = NIM_DTMB;
  ui_set_terrestrial_transpond(&tp);

  return TRUE;
}


static RET_CODE _ui_manual_search_v_manual_search(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(_ui_manual_search_c_init())
  {
    fw_tmr_destroy(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, MSG_CHECK_FREQUENCY_CHANGE);

    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_MANUAL, 0);
  }
  
  return SUCCESS;
}


static RET_CODE _ui_manual_search_v_input_frequency(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 freq;
  osd_set_t osd_set = {0};
  control_t *p_number = NULL;
  
  nbox_class_proc(p_ctrl, msg, para1, para2);
  p_number = ui_comm_root_get_ctrl(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, IDC_MANUAL_SEARCH_FREQUENCY);
  freq = (u32)ui_comm_numedit_get_num(p_number);
  sys_status_get_osd_set(&osd_set);

  if(fw_tmr_reset(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, MSG_CHECK_FREQUENCY_CHANGE, osd_set.timeout * 1000) != SUCCESS)
  {
    fw_tmr_create(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, MSG_CHECK_FREQUENCY_CHANGE, osd_set.timeout * 1000, FALSE);
  }
  
  return SUCCESS;
}


static RET_CODE _ui_manual_search_v_update_signal(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 freq;
  control_t *p_number = NULL;
  
  p_number = ui_comm_root_get_ctrl(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, IDC_MANUAL_SEARCH_FREQUENCY);
  freq = (u32)ui_comm_numedit_get_num(p_number);
  _ui_manual_search_c_lock_tp(freq);
    
  return SUCCESS;
}


static RET_CODE _ui_manual_search_v_auto_update_signal(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  _ui_manual_search_v_update_signal(p_ctrl, msg, para1, para2);
  fw_tmr_destroy(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, MSG_CHECK_FREQUENCY_CHANGE);

  return SUCCESS;
}


RET_CODE open_terrestrial_manual_search(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_ctrl[_MANUAL_SEARCH_ITEM_CNT] = {NULL};
  t2_search_info search_info = {0};
  u8 i = 0, country_id = 0, channel_min = 0, channel_max =0;
  u32 frequency = 0;
  dvbc_lock_t tp = {0};
  u16 y, bandwidth = 0, stxt[_MANUAL_SEARCH_ITEM_CNT] =
  {
    IDS_FREQUENCY, IDS_MANUAL_SEARCH
  };

  comm_help_data_t2 help_data[] =
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
        HELP_RSC_BMP   | IM_OK,
        HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_STA,
      },
    },
  };

  country_id = (u8)para2;
  /* todo: need update country */
  sys_status_get_country_set(&search_info);
  /* check receive id == saved infomation? */
  if(country_id != search_info.country_style)
  {
    return ERR_FAILURE;
  }
  
  channel_min = dvbt_get_min_rf_ch_num(search_info.country_style);
  channel_max = dvbt_get_max_rf_ch_num(search_info.country_style);
  dvbt_get_tp_setting(channel_min, search_info.country_style, &bandwidth, &frequency);
  sys_status_get_main_tp1(&tp);
  frequency = tp.tp_freq;
  _ui_manual_search_c_lock_tp(frequency);
  
  p_cont = ui_comm_root_create_with_signbar(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH,
                                            0, 
                                            COMM_BG_X, COMM_BG_Y,
                                            COMM_BG_W, COMM_BG_H, 
                                            0, IDS_MANUAL_SEARCH, TRUE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, manual_search_keymap);
  ctrl_set_proc(p_cont, manual_search_proc);

  y = _MANUAL_SEARCH_ITEM_Y;
  for(i = 0; i < _MANUAL_SEARCH_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_MANUAL_SEARCH_FREQUENCY + i),
                                           _MANUAL_SEARCH_ITEM_X, y, 
                                           _MANUAL_SEARCH_ITEM_LW, _MANUAL_SEARCH_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], manual_search_frequency_change_proc);
        ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC, SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, _MANUAL_SEARCH_FREQ_BIT, 0);
        ui_comm_numedit_set_num(p_ctrl[i], frequency);
        break;

      case 1:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MANUAL_SEARCH_FREQUENCY + i),
                                    _MANUAL_SEARCH_ITEM_X, y, 
                                    (_MANUAL_SEARCH_ITEM_LW + _MANUAL_SEARCH_ITEM_RW), _MANUAL_SEARCH_ITEM_H, p_cont, 0);
        ctrl_set_rstyle(p_ctrl[i], _RSI_MANUAL_SEARCH_BTN_SH, _RSI_MANUAL_SEARCH_BTN_HL, _RSI_MANUAL_SEARCH_BTN_SH);
        text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
        text_set_offset(p_ctrl[i], COMM_CTRL_OX, 0);
        text_set_content_by_strid(p_ctrl[i], stxt[i]);
        break;
        
      default:
        break;
     }
    ctrl_set_related_id(p_ctrl[i],
                        0,                                                                    /* left */
                        (u8)((i - 1 + _MANUAL_SEARCH_ITEM_CNT) % _MANUAL_SEARCH_ITEM_CNT + 1),  /* up */
                        0,                                                                    /* right */
                        (u8)((i + 1) % _MANUAL_SEARCH_ITEM_CNT + 1));                          /* down */

    y += _MANUAL_SEARCH_ITEM_H + _MANUAL_SEARCH_ITEM_V_GAP;
  }
  
  #ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
  #endif
  
{
  control_t *p_help_cont = NULL;
  p_help_cont = ctrl_create_ctrl(CTRL_CONT, IDC_HELP_CONT, _MANUAL_SEARCH_ITEM_X + 20, SCREEN_HEIGHT - 60, 
                        SCREEN_WIDTH - _MANUAL_SEARCH_ITEM_X * 2, _MANUAL_SEARCH_ITEM_H, p_cont, 0);
  if(p_help_cont)
  {
    ui_comm_help_create2(&help_data[0], p_help_cont,FALSE);
  }
}

  /* default focus on first option */
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


BEGIN_KEYMAP(manual_search_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(manual_search_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(manual_search_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, _ui_manual_search_v_manual_search)
  ON_COMMAND(MSG_CHECK_FREQUENCY_CHANGE, _ui_manual_search_v_auto_update_signal)
END_MSGPROC(manual_search_proc, ui_comm_root_proc)

BEGIN_MSGPROC(manual_search_frequency_change_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_CHANGED, _ui_manual_search_v_update_signal)
  ON_COMMAND(MSG_NUMBER, _ui_manual_search_v_input_frequency)
END_MSGPROC(manual_search_frequency_change_proc, ui_comm_num_proc)

