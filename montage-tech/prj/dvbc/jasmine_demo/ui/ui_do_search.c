/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_do_search.h"
#include "customer_config.h"
#include "bat.h"
#include "user_parse_table.h"
#ifdef ENABLE_CA
#include "config_cas.h"
#include "ui_ca_public.h"
#include "cas_manager.h"
#endif
#include "sys_app.h"
#ifdef STARTUP_NIT_VERSION_CHECK
#include "ap_nit.h"
#endif

#ifdef LCN_SWITCH_DS_JHC
#include "nit_logic_num_parse.h"
#endif

enum local_msg
{
  MSG_STOP_SCAN = MSG_LOCAL_BEGIN + 150,
  MSG_CLOSE_ALL,
};

enum control_id
{
  IDC_PROG_INFO_FRM = 1,
  IDC_DO_SEARCH_TOP_LINE,
  IDC_DO_SEARCH_BOTTOM_LINE,
  IDC_TP_INFO_FRM,
  IDC_PBAR,
};

enum prog_sub_control_id
{
  IDC_NUM_TV = 1,
  IDC_NUM_RADIO,
  IDC_LIST_TV,
  IDC_LIST_RADIO,
};

enum tp_sub_control_id
{
  IDC_TP_LIST = 1,
};

#define PROG_NAME_STRLEN    DB_DVBS_MAX_NAME_LENGTH
#define LCN_NO_STRLEN      4 /* 999 */

static u16 prog_name_str[DO_SEARCH_LIST_CNT][DO_SEARCH_LIST_PAGE][
  PROG_NAME_STRLEN + 1];
static u16 *prog_name_str_addr[DO_SEARCH_LIST_CNT][DO_SEARCH_LIST_PAGE];
static u16 prog_lcn_num[DO_SEARCH_LIST_CNT][DO_SEARCH_LIST_PAGE][LCN_NO_STRLEN + 1];
static u16 *prog_lcn_num_addr[DO_SEARCH_LIST_CNT][DO_SEARCH_LIST_PAGE];
static u16 prog_curn[DO_SEARCH_LIST_CNT];
static u16 first_id[DO_SEARCH_LIST_CNT];
//static u8 first_pg_flag;


#define TP_NO_STRLEN      4 /* 999 */
#define TP_INFO_STRLEN    64

static u16 tp_no_str[DO_SEARCH_TP_LIST_PAGE][TP_NO_STRLEN + 1];
static u16 tp_info_str[DO_SEARCH_TP_LIST_PAGE][TP_INFO_STRLEN + 1];
static u16 *tp_no_str_addr[DO_SEARCH_TP_LIST_PAGE];
static u16 *tp_info_str_addr[DO_SEARCH_TP_LIST_PAGE];
static u16 tp_curn;

static BOOL is_db_empty = FALSE;
static BOOL is_stop = TRUE;
static BOOL is_ask_for_cancel = FALSE;
static BOOL is_finished = TRUE;
//static u16 g_curn_tp_view_id;

static u8 g_ui_type = 0;

u16 do_search_cont_keymap(u16 key);

RET_CODE do_search_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

extern void categories_mem_reset(void);
extern BOOL have_logic_number(void);
extern void add_dvbc_all_tp_zhinling_by(void);
extern void add_dvbc_all_tp_jzmex(void);

static void do_search_reset_pg_and_bouquet(void)
{
  BOOL is_scart = FALSE;
  BOOL is_db_full = FALSE;
  //set default use common buffer as ext buffer
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID, BLOCK_REC_BUFFER);
  sys_status_get_status(BS_IS_SCART_OUT, &is_scart);
  sys_status_get_status(BS_DB_FULL, &is_db_full);
  sys_status_set_categories_count(0);
  categories_mem_reset();
  if(is_scart)
 {
 	sys_status_set_status(BS_IS_SCART_OUT, TRUE);
 }
  if(is_db_full == TRUE)
  {
    sys_status_set_status(BS_DB_FULL, FALSE);
  }
  sys_status_check_group();
  sys_status_save();
  
  //clear history 
  ui_clear_play_history();

  //delete all book imformation
  book_delete_all_node();
 }

static void ui_delete_pg_data(void)
{
       /**clear db date***/
    ui_dbase_delete_all_pg();
    //set default
    db_dvbs_restore_to_factory(PRESET_BLOCK_ID, 0);
    //sys_status_load();
    sys_group_reset();
    sys_status_check_group();
    sys_status_save();
}

void ui_pre_nit_search_set()
{
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  ui_delete_pg_data(); /****delete all pg****/
  sys_status_reset_channel_and_nit_version();/***make sure next start up can auto scan*****/
  manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
}

static void do_cancel(void)
{
  if(!is_stop)
  {
    OS_PRINTF("DO_SEARCH: cancel scan!\n");

    OS_PRINTF("*************************press ok : %d\n", mtos_ticks_get());
    is_stop = TRUE;
    ui_stop_scan();
  }
}


static void undo_cancel(void)
{
  OS_PRINTF("DO_SEARCH: resume scan!\n");
  ui_resume_scan();
}


static void init_static_data(void)
{
  u16 i, j, *p_str;

  for(i = 0; i < DO_SEARCH_LIST_CNT; i++)
  {
    for(j = 0; j < DO_SEARCH_LIST_PAGE; j++)
    {
      p_str = prog_name_str[i][j];
      prog_name_str_addr[i][j] = p_str;
      p_str[0] = '\0';

      p_str = prog_lcn_num[i][j];
      prog_lcn_num_addr[i][j] = p_str;
      p_str[0] = '\0';
    }
  }
  memset(prog_curn, 0, sizeof(prog_curn));

  for(i = 0; i < DO_SEARCH_TP_LIST_PAGE; i++)
  {
    p_str = tp_no_str[i];
    tp_no_str_addr[i] = p_str;
    p_str[0] = '\0';

    p_str = tp_info_str[i];
    tp_info_str_addr[i] = p_str;
    p_str[0] = '\0';
  }
  tp_curn = 0;

  for (i = 0; i < DO_SEARCH_LIST_CNT; i++)
  {
    first_id[i] = INVALIDID;
  }

  //first_pg_flag = 0;
}


static void init_prog_list_content(control_t *list)
{
  u8 i, list_idx = ctrl_get_ctrl_id(list) == IDC_LIST_TV ? 0 : 1;
  u8 asc_str[5] = {0};
  u16 uni_str[5] = {0};
  
  for(i = 0; i < DO_SEARCH_LIST_PAGE; i++)
  {
    if(have_logic_number())
    {
      list_set_field_content_by_unistr(list, i, 0, prog_lcn_num_addr[list_idx][i]);
    }
    else
    {
      if(prog_curn[list_idx] <= DO_SEARCH_LIST_PAGE)
      {
        if(i < prog_curn[list_idx])
        {
          sprintf((char*)asc_str, "%.4d", i + 1);
          str_asc2uni(asc_str, uni_str);
          list_set_field_content_by_unistr(list, i, 0, uni_str);
        }
        else
        {
          list_set_field_content_by_unistr(list, i, 0, (u16*)" ");
        }
      }
      else
      {
        sprintf((char*)asc_str, "%.4d", prog_curn[list_idx] - (DO_SEARCH_LIST_PAGE - 1) + i);
        str_asc2uni(asc_str, uni_str);
        list_set_field_content_by_unistr(list, i, 0, uni_str);
      }
    }
    list_set_field_content_by_extstr(list, i, 1, prog_name_str_addr[list_idx][i]);
  }
}


static void init_tp_list_content(control_t *list)
{
  u8 i;

  for(i = 0; i < DO_SEARCH_TP_LIST_PAGE; i++)
  {
    list_set_field_content_by_extstr(list, i, 0, tp_no_str_addr[i]);
    list_set_field_content_by_extstr(list, i, 1, tp_info_str_addr[i]);
  }
}


static void add_pro_info_to_data(u8 list, u16 *p_str, dvbs_prog_node_t *p_pg)
{
  u16 i, curn = prog_curn[list] % DO_SEARCH_LIST_PAGE;
  u8 ascstr[LCN_NO_STRLEN + 1];
  prog_name_str[list][curn][0] = '\0';
  uni_strcat(prog_name_str[list][curn], p_str, PROG_NAME_STRLEN);
  if(have_logic_number())
  {
	  sprintf((char *)ascstr, "%.4d", p_pg->logical_num);
	  str_asc2uni(ascstr, prog_lcn_num[list][curn]);
	  prog_lcn_num_addr[list][curn] = prog_lcn_num[list][curn];
 }
  prog_name_str_addr[list][curn] = prog_name_str[list][curn];
  prog_curn[list]++;
  if(prog_curn[list] >= DO_SEARCH_LIST_PAGE)
  {
    for(i = 0; i < DO_SEARCH_LIST_PAGE; i++)
    {
      prog_name_str_addr[list][i] =
      prog_name_str[list][(prog_curn[list] + i + 1) % DO_SEARCH_LIST_PAGE];
      if(have_logic_number())
         prog_lcn_num_addr[list][i] = prog_lcn_num[list][(prog_curn[list] + i + 1) % DO_SEARCH_LIST_PAGE];
    }
  }

}

static void add_info_to_data(u16 *p_str)
{
  u8 ascstr[TP_NO_STRLEN + 1];
  u16 i, curn = tp_curn % DO_SEARCH_TP_LIST_PAGE;

  sprintf((char *)ascstr, "%.4d", tp_curn + 1);
  str_asc2uni(ascstr, tp_no_str[curn]);
  tp_no_str_addr[curn] = tp_no_str[curn];

  tp_info_str[curn][0] = '\0';
  uni_strcat(tp_info_str[curn], p_str, TP_INFO_STRLEN);
  tp_info_str_addr[curn] = tp_info_str[curn];

  if(tp_curn >= DO_SEARCH_TP_LIST_PAGE)
  {
    for(i = 0; i < DO_SEARCH_TP_LIST_PAGE; i++)
    {
      tp_no_str_addr[i] =
        tp_no_str[(tp_curn + i + 1) % DO_SEARCH_TP_LIST_PAGE];
      tp_info_str_addr[i] =
        tp_info_str[(tp_curn + i + 1) % DO_SEARCH_TP_LIST_PAGE];
    }
  }

  tp_curn++;
}



static void add_prog_to_list(control_t *cont, dvbs_prog_node_t *p_pg_info)
{
  control_t *frm, *icon, *list;
  dvbs_prog_node_t pg;
  u8 idx = 0;
  u8 icon_num[10];
  u16 content[PROG_NAME_STRLEN + 1];

  OS_PRINTF("ADD PG -> name[%s]\n", p_pg_info->name);

/*
   if (db_dvbs_get_pg_by_id(rid, &pg) != DB_DVBS_OK)
   {
    OS_PRINTF("GET PG failed\n", rid);
    return;
   }
  */
  memcpy(&pg, p_pg_info, sizeof(dvbs_prog_node_t));

#ifdef QUICK_SEARCH_SUPPORT
  if(pg.service_type ==  SVC_TYPE_TV)
    {
      idx = 0;
    }
  else 
    if(pg.service_type ==  SVC_TYPE_RADIO)
    {
      idx = 1;
    }
#else
    idx = pg.video_pid != 0 ? 0 /* tv */ : 1 /* radio */;
#endif

  ui_dbase_get_full_prog_name(&pg, content, PROG_NAME_STRLEN);
  add_pro_info_to_data(idx, content,&pg);

  frm = ctrl_get_child_by_id(cont, IDC_PROG_INFO_FRM);
  icon = ctrl_get_child_by_id(frm, IDC_NUM_TV + idx);

  sprintf((char *)icon_num, "[%d]", prog_curn[idx]);
  text_set_content_by_ascstr(icon, icon_num);
  ctrl_paint_ctrl(icon, TRUE);

  list = ctrl_get_child_by_id(frm, IDC_LIST_TV + idx);
  init_prog_list_content(list);
  ctrl_paint_ctrl(list, TRUE);
  OS_PRINTF("END ADD PG -> name[%s]\n", p_pg_info->name);

  //if(first_pg_flag == 0)
  //{
  //  first_id[idx] = pg.id;
  //  first_pg_flag = 1;
  //}

  if(first_id[idx] == INVALIDID)
  {
    first_id[idx] = (u16)pg.id;
  }
}


static void add_tp_to_list(control_t *cont, dvbs_tp_node_t *p_tp_info)
{
  control_t *frm, *list;
  dvbs_tp_node_t tp;
  u16 str_id=IDS_QAM64;
  u8 asc_buf[TP_INFO_STRLEN + 1];
  u16 uni_buf[TP_INFO_STRLEN + 1];
  u8 ascstr[32];
  u16 unistr[64];
  u8 scan_type = ui_scan_param_get_type();

  OS_PRINTF("ADD TP -> freq[%d]\n", p_tp_info->freq);
  memcpy(&tp, p_tp_info, sizeof(dvbs_tp_node_t));

  asc_buf[0] = '\0';
  uni_buf[0] = '\0';

  //--ui_dbase_name2str(curn_sat_node.name, uni_buf, TP_INFO_STRLEN);
  //uni_strncpy(uni_buf, curn_sat_node.name, TP_INFO_STRLEN);
  //len = (u16)uni_strlen(uni_buf);
  if(scan_type == USC_DTMB_SCAN)
  {
    sprintf(asc_buf, " %.5d %.5d...", (int)tp.freq, (int)tp.sym);
  }
  else
  {
    switch(tp.nim_modulate)
    {
      case 0:
        str_id = IDS_AUTO;
        break;

      case 1:
        str_id =IDS_QPSK;
        break;

      case 2:
        str_id = IDS_BPSK;
        break;

      case 3:
        str_id = IDS_8PSK;
        break;

      case 4:
        str_id = IDS_QAM16;
        break;

      case 5:
        str_id = IDS_QAM32;
        break;

      case 6:
        str_id = IDS_QAM64;
        break;

      case 7:
        str_id = IDS_QAM128;
        break;

      case 8:
        str_id = IDS_QAM256;
        break;
      default:
        str_id = IDS_QAM64;
        break;
    }
  }
  gui_get_string(str_id, unistr, 64);
  str_uni2asc((u8 *)ascstr,(u16 *)unistr);
  sprintf((char *)asc_buf, "%.6d %s %.4d...", (int)tp.freq,
         (char *)ascstr, (int)tp.sym);
  str_nasc2uni(asc_buf, uni_buf, TP_INFO_STRLEN);

  add_info_to_data(uni_buf);

  frm = ctrl_get_child_by_id(cont, IDC_TP_INFO_FRM);
  list = ctrl_get_child_by_id(frm, IDC_TP_LIST);
  init_tp_list_content(list);
  ctrl_paint_ctrl(list, TRUE);
  OS_PRINTF("END ADD TP ->freq[%d]\n", p_tp_info->freq);
}


static void update_progress(control_t *cont, u16 progress)
{
  control_t *bar = ctrl_get_child_by_id(cont, IDC_PBAR);

  if(ctrl_get_sts(bar) != OBJ_STS_HIDE)
  {
    OS_PRINTF("UPDATE PROGRESS -> %d\n", progress);
    ui_comm_bar_update(bar, progress, TRUE);
    ui_comm_bar_paint(bar, TRUE);
  }
}

static void get_finish_str(u16 *str, u16 max_len)
{
  u16 uni_str[10], len;

  if(prog_curn[0] > 0
    || prog_curn[1] > 0)
  {
    len = 0, str[0] = '\0';
    gui_get_string(IDS_MSG_SEARCH_IS_END, str, max_len);

    convert_i_to_dec_str(uni_str, prog_curn[0]);
    uni_strcat(str, uni_str, max_len);

    len = (u16)uni_strlen(str);
    gui_get_string(IDS_MSG_N_TV, &str[len], (u16)(max_len - len));

    convert_i_to_dec_str(uni_str, prog_curn[1]);
    uni_strcat(str, uni_str, max_len);

    len = (u16)uni_strlen(str);
    gui_get_string(IDS_MSG_N_RADIO, &str[len], (u16)(max_len - len));
  }
  else
  {
    gui_get_string(IDS_MSG_NO_PROG_FOUND, str, max_len);
  }
}


static void process_finish(void)
{
  comm_dlg_data_t dlg_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    0,
    3000,
  };

  u16 content[64 + 1];

  get_finish_str(content, 64);
  dlg_data.content = (u32)content;

  is_stop = TRUE;
  is_finished = TRUE;

  // already popup a dlg for ask, force close it
  if(is_ask_for_cancel)
  {
    // resume scan
    ui_resume_scan();
  }

  ui_comm_dlg_open(&dlg_data);
}


static void do_search_pre_open(void)
{
  OS_PRINTF("do scan pre open\n");
  //to disable book remind.
  ui_set_book_flag(FALSE);
  OS_PRINTF("set book flag\n");

#if ENABLE_TTX_SUBTITLE
  ui_enable_subt(FALSE);
#endif

  // disable pb
  ui_enable_playback(FALSE);

//disable time
ui_time_release();

 // ui_set_front_panel_by_str("----");
  if((g_customer.customer_id == CUSTOMER_FANTONG) ||
     (g_customer.customer_id == CUSTOMER_BOYUAN)  ||
     (g_customer.customer_id == CUSTOMER_JIULIAN) ||
     (g_customer.customer_id == CUSTOMER_JINGHUICHENG_DIVI) ||
     (g_customer.customer_id == CUSTOMER_TAIHUI_DIVI))
  {
    ui_set_front_panel_by_str("SCAN");
  }
  else
  {
    ui_set_front_panel_by_str("SCH-");
  }

 OS_PRINTF("1111111111111111set front panel\n");
//  video_layer_display(FALSE);

  ui_epg_stop();
  /* stop epg */
  init_static_data();
  OS_PRINTF("init static data\n");

  /* set callback func */
//  dvbs_install_notification(scan_do_notify, 0);

  /* set flag */
  is_stop = is_finished = FALSE;

  if(sys_status_get_curn_group_curn_prog_id() == INVALIDID)
  {
    is_db_empty = TRUE;
  }
  else
  {
    is_db_empty = FALSE;
  }

//  ui_simon_stop();
//  ui_simon_release();

  ui_init_scan();
  OS_PRINTF("init scan\n");

}


static void do_search_post_close(void)
{
  comm_dlg_data_t saving_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    IDS_MSG_SAVING,
    0,
  };

  if((prog_curn[0] > 0)
    || (prog_curn[1] > 0))
  {
    ui_comm_dlg_open(&saving_data);
  }

  /* set callback func */
  OS_PRINTF("do_search_post_close: reset the desktop notify func\n");

  //to enable book remind.
  ui_set_book_flag(TRUE);

  /* fix bug 18050 */
  ui_signal_set_lock(TRUE);

  /* delete the local view */

  /* chk, if need sort on autoscan mode */

  /* recheck group info*/
  ui_dbase_reset_last_prog(is_db_empty);

  ui_set_front_panel_by_str("----");

  /* restart epg in ui_play_channel */
  ui_release_scan();

  // restart epg after finish scan
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  // disable pb
  ui_enable_playback(TRUE);

  ui_time_init();

#if ENABLE_TTX_SUBTITLE
  ui_enable_subt(TRUE);
#endif
#ifdef CAS_CONFIG_DS_5_0
  if(osd_roll_stop_msg_unsend())
  {
    ui_ca_do_cmd(CAS_CMD_OSD_ROLL_OVER, 0, 0);
  }
#endif
  is_stop = is_finished = TRUE;

  if((prog_curn[0] > 0)
    || (prog_curn[1] > 0))
  {
    ui_comm_dlg_close();
  }
}


BOOL do_search_is_finish(void)
{
  return is_finished;
}

BOOL do_search_is_stop(void)
{
  return is_stop;
}

static void do_search_get_main_tp(ui_scan_param_t *scan_param)
{
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
  dvbc_lock_t tp = {0};
  sys_status_get_main_tp1(&tp);
#ifdef QUICK_SEARCH_SUPPORT
scan_param->nit_type = NIT_SCAN_AND_STD_OTR;
#else
  scan_param->nit_type = NIT_SCAN_ONCE;
#endif
  scan_param->tp.freq = tp.tp_freq;
  scan_param->tp.sym = tp.tp_sym;

  switch(tp.nim_modulate)
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

  scan_param->tp.nim_modulate = nim_modul;
}

#if 1
static BOOL ui_auto_dtmb_search_parameter_init(void)
{
  dvbs_tp_node_t tp[FULL_SCAN_CNT];
  sat_node_t sat_node = {0};
  u16 i = 0;
  
  memset(tp, 0, sizeof(dvbs_tp_node_t) * FULL_SCAN_CNT);
  tp[0].freq = SEARCH_FREQ_MIN;

  for(i = 1; i < FULL_SCAN_CNT; i++)
  {
    tp[i].freq = tp[0].freq + (i * 8000);
  }
  
  ui_scan_param_init();

  for(i = 0; i < FULL_SCAN_CNT; i++)
  {
    if((tp[i].freq >= SEARCH_FREQ_MIN) && (tp[i].freq <= SEARCH_FREQ_MAX))
    {
      tp[i].sym = DTMB_HN_SYMBOL_DEFAULT;
      tp[i].sat_id = sat_node.id;
      tp[i].nim_type = NIM_DVBT;
      ui_scan_param_add_tp(&tp[i]);
    }
  }
  ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
  
  return TRUE;
}
#else
static BOOL ui_auto_dtmb_search_parameter_init()
{  
  dvbs_tp_node_t tp = {0};
  scan_param_t scan_param = {0};
  dvbc_lock_t main_tp = {0};
  sat_node_t sat_node = {0};

  /* init, use video buffer */
  ui_scan_param_init();
  /* add preset tp */
  sys_status_get_scan_param(&scan_param);
  sys_status_get_main_tp1(&main_tp);
  main_tp.tp_freq = 602000;
  
  if(main_tp.tp_freq < SEARCH_FREQ_MIN)
    main_tp.tp_freq = SEARCH_FREQ_MIN;
  if(main_tp.tp_freq > SEARCH_FREQ_MAX)
    main_tp.tp_freq = SEARCH_FREQ_MAX;
  tp.freq = main_tp.tp_freq;
  tp.sym = DTMB_HN_SYMBOL_DEFAULT;
  tp.nim_type = NIM_DVBT;
  tp.sat_id = sat_node.id;

  ui_scan_param_add_tp(&tp);
  
  /* init scan param */
  ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);

  return TRUE;
}
#endif

RET_CODE open_do_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_bar;
  ui_scan_param_t scan_param;
  control_t *p_ctrl, *p_subctrl;
  list_xstyle_t rstyle =
  {
    RSI_PBACK, RSI_PBACK,
    RSI_PBACK, RSI_PBACK,
    RSI_PBACK
  };
  u16 title_txt[DO_SEARCH_TITLE_CNT] = {IDS_TV, IDS_RADIO};
  list_xstyle_t fstyle =
  {
    FSI_DO_SEARCH_LIST_ITEM, FSI_DO_SEARCH_LIST_ITEM,
    FSI_DO_SEARCH_LIST_ITEM, FSI_DO_SEARCH_LIST_ITEM,
    FSI_DO_SEARCH_LIST_ITEM
  };
  u16 i;
  u16 title_strid = RSC_INVALID_ID;
  dvbs_tp_node_t tp;
  BOOL lcn_flag = FALSE;
/*
  //stop monitor service
  {
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
  }
*/
  #ifdef ENABLE_CA
    if(CAS_ID  == CONFIG_CAS_ID_UNITEND)
    {
      cas_manager_stop_ca_play();
    }
  #endif

  g_ui_type = (u8)para1;
  switch(g_ui_type)
  {
    case SCAN_TYPE_RANGE:
      title_strid = IDS_RANGE_SEARCH;
      break;
    case SCAN_TYPE_FULL:
	title_strid = IDS_RANGE_SEARCH;
      ui_scan_param_init();

      if(CUSTOMER_ZHUMUDIAN_BY == CUSTOMER_ID) 
      {
          add_dvbc_all_tp_zhinling_by();
      }
      if(CUSTOMER_JIZHONGMEX == CUSTOMER_ID) 
      {
          add_dvbc_all_tp_jzmex();
      }
      
      ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);

      break;
    case SCAN_TYPE_MANUAL:
      title_strid = IDS_MANUAL_SEARCH;
      memcpy((void *)&scan_param, (void *)para2, sizeof(ui_scan_param_t));
      if(scan_param.nit_type != NIT_SCAN_WITHOUT)
      {
        do_search_reset_pg_and_bouquet();
      }
      ui_scan_param_init();
      memset(&tp, 0, sizeof(dvbs_tp_node_t));
      ui_scan_param_add_tp(&scan_param.tp);
      ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, FALSE, scan_param.nit_type);
      break;
    case SCAN_TYPE_AUTO:
      do_search_reset_pg_and_bouquet();
      
      #ifdef STARTUP_NIT_VERSION_CHECK
        sys_status_reset_channel_and_nit_version();
      #endif
      
      title_strid = IDS_AUTO_SEARCH;
      if((void *)para2 != NULL)
      {
        memcpy((void *)&scan_param, (void *)para2, sizeof(ui_scan_param_t));
      }
      else
      {
        do_search_get_main_tp(&scan_param);
      }
      ui_scan_param_init();
      memset(&tp, 0, sizeof(dvbs_tp_node_t));
      ui_scan_param_add_tp(&scan_param.tp);
      ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, FALSE, scan_param.nit_type);
      break;
    case SCAN_TYPE_DTMB_AUTO:
      title_strid = IDS_AUTO_SEARCH;

      do_search_reset_pg_and_bouquet();
      ui_auto_dtmb_search_parameter_init();
      break;
    case SCAN_TYPE_DTMB_MANUAL:
      title_strid = IDS_MANUAL_SEARCH;
      break;
    case SCAN_TYPE_DTMB_FULL:
      title_strid = IDS_FULL_SCAN;
      break;
    default:
      title_strid = IDS_TP_SEARCH;
      memcpy((void *)&scan_param, (void *)para2, sizeof(ui_scan_param_t));
      ui_scan_param_init();
      memset(&tp, 0, sizeof(dvbs_tp_node_t));
      ui_scan_param_add_tp(&scan_param.tp);
      ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_ONCE);
      break;
  }

  //g_curn_tp_view_id = (u16)para2;

  p_cont = ui_comm_root_create(ROOT_ID_DO_SEARCH, 0, 
                                                    COMM_BG_X, COMM_BG_Y,
                                                    COMM_BG_W, COMM_BG_H, RSC_INVALID_ID, title_strid);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, do_search_cont_keymap);
  ctrl_set_proc(p_cont, do_search_cont_proc);

  // before open
  do_search_pre_open();
  // prog part
  p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_PROG_INFO_FRM,
                            DO_SEARCH_FRM_X , DO_SEARCH_FRM_Y,
                            DO_SEARCH_FRM_W, DO_SEARCH_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  for(i = 0; i < DO_SEARCH_TITLE_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT,
                                 0,
                                 (u16)(DO_SEARCH_TITLE_BG_X + i *
                                 (DO_SEARCH_TITLE_AND_TITLE_HGAP + DO_SEARCH_TITLE_BG_W)),
                                 DO_SEARCH_TITLE_BG_Y,
                                 DO_SEARCH_TITLE_BG_W,
                                 DO_SEARCH_TITLE_BG_H,
                                 p_ctrl,
                                 0);
     ctrl_set_rstyle(p_subctrl, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
    text_set_align_type(p_subctrl, STL_CENTER | STL_CENTER);
    text_set_font_style(p_subctrl, FSI_DO_SEARCH_LIST_ITEM,
                        FSI_DO_SEARCH_LIST_ITEM, FSI_DO_SEARCH_LIST_ITEM);
    text_set_content_by_strid(p_subctrl, title_txt[i]);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT,
                                 (u8)(IDC_NUM_TV + i),
                                 (u16)(260 + DO_SEARCH_TITLE_BG_X + i *
                                 (DO_SEARCH_TITLE_AND_TITLE_HGAP + DO_SEARCH_TITLE_BG_W)),
                                 DO_SEARCH_TITLE_BG_Y,
                                 DO_SEARCH_TITLE_NUM_W,
                                 DO_SEARCH_TITLE_NUM_H,
                                 p_ctrl,
                                 0);
    ctrl_set_rstyle(p_subctrl, RSI_LINE_03_MID, RSI_LINE_03_MID, RSI_LINE_03_MID);
    text_set_align_type(p_subctrl, STL_CENTER | STL_CENTER);
    text_set_font_style(p_subctrl, FSI_DO_SEARCH_LIST_ITEM,
                        FSI_DO_SEARCH_LIST_ITEM, FSI_DO_SEARCH_LIST_ITEM);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  }

  for(i = 0; i < DO_SEARCH_LIST_CNT; i++)
  {
    p_list = ctrl_create_ctrl(CTRL_LIST, (u8)(IDC_LIST_TV + i),
                              (u16)(DO_SEARCH_LIST_X + i *
                              (DO_SEARCH_TITLE_AND_TITLE_HGAP + DO_SEARCH_LIST_W)),
                              DO_SEARCH_LIST_Y, DO_SEARCH_LIST_W, DO_SEARCH_LIST_H,
                              p_ctrl, 0);
    ctrl_set_rstyle(p_list,
                    RSI_COMMON_RECT1,
                    RSI_COMMON_RECT1,
                    RSI_COMMON_RECT1);

    list_set_count(p_list,
                   DO_SEARCH_LIST_PAGE,
                   DO_SEARCH_LIST_PAGE);
    ctrl_set_mrect(p_list, 0, 0, DO_SEARCH_LIST_W, DO_SEARCH_LIST_H);
    list_set_item_interval(p_list, DO_SEARCH_LIST_ITEM_V_GAP);
    list_set_item_rstyle(p_list, &rstyle);

    list_set_field_count(p_list, DO_SEARCH_LIST_FIELD_CNT,
                         DO_SEARCH_LIST_PAGE);
    list_set_field_attr(p_list, 0,
                        STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_UNISTR,
                        130, 30, 0);
    list_set_field_attr(p_list, 1,
                        STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                        DO_SEARCH_LIST_W - 160, 160, 0);
    list_set_field_font_style(p_list, 0, &fstyle);
    list_set_field_font_style(p_list, 1, &fstyle);
  }

  // tp part
  p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_TP_INFO_FRM,
                            DO_SEARCH_TP_LIST_FRM_X, DO_SEARCH_TP_LIST_FRM_Y,
                            DO_SEARCH_TP_LIST_FRM_W, DO_SEARCH_TP_LIST_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_TP_LIST,
                            DO_SEARCH_TP_LIST_X, DO_SEARCH_TP_LIST_Y,
                            DO_SEARCH_TP_LIST_W, DO_SEARCH_TP_LIST_H,
                            p_ctrl, 0);

  ctrl_set_rstyle(p_list,
                  RSI_DO_SEARCH_LIST,
                  RSI_DO_SEARCH_LIST,
                  RSI_DO_SEARCH_LIST);
  list_set_count(p_list,
                 DO_SEARCH_TP_LIST_PAGE,
                 DO_SEARCH_TP_LIST_PAGE);
  ctrl_set_mrect(p_list, 0, 0, DO_SEARCH_TP_LIST_W, DO_SEARCH_TP_LIST_H);
  list_set_item_interval(p_list, DO_SEARCH_TP_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &rstyle);

  list_set_field_count(p_list, DO_SEARCH_TP_LIST_FIELD_CNT,
                       DO_SEARCH_TP_LIST_PAGE);
  list_set_field_attr(p_list, 0,
                      STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                      DO_SEARCH_TP_LIST_NO_W, 0, 0);
  list_set_field_font_style(p_list, 0, &fstyle);
  list_set_field_attr(p_list, 1,
                      STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                      DO_SEARCH_TP_LIST_INFO_W, DO_SEARCH_TP_LIST_NO_W, 0);
  list_set_field_font_style(p_list, 1, &fstyle);
  init_tp_list_content(p_list);

  // pbar
  p_bar = ui_comm_bar_create(p_cont, IDC_PBAR,
                             DO_SEARCH_PBAR_X, DO_SEARCH_PBAR_Y,
                             DO_SEARCH_PBAR_W, DO_SEARCH_PBAR_H,
                             0, 0, 0, 0,
                             DO_SEARCH_PBAR_PERCENT_X,
                             DO_SEARCH_PBAR_PERCENT_Y,
                             DO_SEARCH_PBAR_PERCENT_W,
                             DO_SEARCH_PBAR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, RSC_INVALID_ID, 0, 100, 100);

  ui_comm_bar_set_style(p_bar,
                        RSI_DO_SEARCH_PBAR_BG, RSI_DO_SEARCH_PBAR_MID,
                        RSI_IGNORE, FSI_WHITE,
                        RSI_PBACK, FSI_WHITE);
  ui_comm_bar_update(p_bar, 0, TRUE);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
 if(CUSTOMER_ID == CUSTOMER_AISAT || CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
 {
    sys_status_get_status(BS_LCN_RECEIVED, &lcn_flag);
    if(lcn_flag != TRUE)
    {
      sys_status_set_status(BS_LCN_RECEIVED, TRUE);
      sys_status_save();
    }
 }
  // start scan
  ui_start_scan();
  return SUCCESS;
}


static RET_CODE on_pg_found(control_t *cont, u16 msg,
                            u32 para1, u32 para2)
{
  OS_PRINTF("PROCESS -> add prog\n");
  add_prog_to_list(cont, (dvbs_prog_node_t *)para1);
  return SUCCESS;
}


static RET_CODE on_tp_found(control_t *cont, u16 msg,
                            u32 para1, u32 para2)
{
  OS_PRINTF("PROCESS -> add/change tp\n");
  add_tp_to_list(cont, (dvbs_tp_node_t *)para1);
  return SUCCESS;
}

static RET_CODE on_nit_found(control_t *cont, u16 msg,
                                   u32 para1, u32 para2)
{
  #ifdef STARTUP_NIT_VERSION_CHECK
   extern void set_channel_init_nit_ver(u8 nit_version);
   set_channel_init_nit_ver((u8)para2);
  #else
    u32 old_nit_ver = 0;

    sys_status_get_nit_version(&old_nit_ver);
    if(old_nit_ver != para2)
    {
      sys_status_set_nit_version(para2);
    }
  #endif
  return SUCCESS;
}

static RET_CODE on_update_progress(control_t *cont, u16 msg,
                                   u32 para1, u32 para2)
{
  OS_PRINTF("PROCESS -> update progress\n");
  update_progress(cont, (u16)para1);
  return SUCCESS;
}

static RET_CODE on_db_is_full(control_t *cont, u16 msg,
                              u32 para1, u32 para2)
{
  OS_PRINTF("PROCESS -> dbase full\n");
  // pause scanning, at first
  OS_PRINTF("DO_SEARCH: pause scan!\n");
  ui_pause_scan();

  ui_comm_cfmdlg_open(NULL, IDS_MSG_SPACE_IS_FULL, do_cancel, 0);
  sys_status_set_status(BS_DB_FULL, TRUE);
  return SUCCESS;
}


static RET_CODE on_finished(control_t *cont, u16 msg,
                            u32 para1, u32 para2)
{
  //u16 i , tp_total;
  if(!is_finished)
  {
    #ifdef STARTUP_NIT_VERSION_CHECK
    	extern u8 get_channel_init_nit_ver(void);

     if(CUSTOMER_ZHUMUDIAN_BY == CUSTOMER_ID) 
      {
            if(( is_stop == FALSE) && (g_ui_type == SCAN_TYPE_FULL))
          {
            sys_status_set_nit_version((u32)get_channel_init_nit_ver());
          }
      }
          if(( is_stop == FALSE) && (g_ui_type == SCAN_TYPE_AUTO))
          {
            sys_status_set_nit_version((u32)get_channel_init_nit_ver());
          }
    #endif
    
    APPLOGA("PROCESS -> finish\n");
    OS_PRINTF("*************************on finished : %d\n", mtos_ticks_get());
    process_finish();
    OS_PRINTF("*************************process finished : %d\n", mtos_ticks_get());
    is_finished = TRUE;

#if 0
    tp_total = db_dvbs_get_count(g_curn_tp_view_id);
    for(i = 0; i < tp_total; i++)
    {
       if(db_dvbs_get_mark_status(g_curn_tp_view_id, i, DB_DVBS_SEL_FLAG, 0))
      {
          db_dvbs_change_mark_status(g_curn_tp_view_id, i, DB_DVBS_SEL_FLAG, 0);
      }
    }
#endif
    ui_close_all_mennus();
    OS_PRINTF("*************************close all menus : %d\n", mtos_ticks_get());
  }
  return SUCCESS;
}


static RET_CODE on_stop_scan(control_t *cont, u16 msg,
                             u32 para1, u32 para2)
{
  if(!is_stop)
  {
    // pause scanning, at first
    OS_PRINTF("DO_SEARCH: pause scan!\n");
    ui_pause_scan();
    is_ask_for_cancel = TRUE;
    // ask for cancel
    ui_comm_ask_for_dodlg_open(NULL, IDS_MSG_ASK_FOR_EXIT_SCAN,
                               do_cancel, undo_cancel, 0);
    is_ask_for_cancel = FALSE;
  }
  return SUCCESS;
}


static RET_CODE on_save_do_search(control_t *cont, u16 msg,
                                  u32 para1, u32 para2)
{
  OS_PRINTF("DO_SEARCH: before MSG_SAVE!\n");
  if(!is_stop)  /* cancel check, dvbs_cancel_scan don't clear it */
  {
    OS_PRINTF("DO_SEARCH: cancel scan!\n");
    ui_stop_scan();
  }

  ui_cache_view();
  create_categories();
  ui_restore_view();

  #ifdef LCN_SWITCH_DS_JHC
  pg_logic_num_update();
  #endif
  
  ui_dbase_pg_sort(DB_DVBS_ALL_PG);

  #ifdef LCN_SWITCH_DS_JHC
  logic_num_add();
  #endif
  
  OS_PRINTF("*************************do_search_post_close 1 : %d\n", mtos_ticks_get());
  do_search_post_close();
  OS_PRINTF("*************************do_search_post_close 2 : %d\n", mtos_ticks_get());
  return SUCCESS;
}


BEGIN_KEYMAP(do_search_cont_keymap, NULL)
ON_EVENT(V_KEY_CANCEL, MSG_STOP_SCAN)
ON_EVENT(V_KEY_EXIT, MSG_STOP_SCAN)
ON_EVENT(V_KEY_BACK, MSG_STOP_SCAN)
ON_EVENT(V_KEY_MENU, MSG_STOP_SCAN)
END_KEYMAP(do_search_cont_keymap, NULL)

BEGIN_MSGPROC(do_search_cont_proc, cont_class_proc)
ON_COMMAND(MSG_SCAN_PG_FOUND, on_pg_found)
ON_COMMAND(MSG_SCAN_TP_FOUND, on_tp_found)
ON_COMMAND(MSG_SCAN_PROGRESS, on_update_progress)
ON_COMMAND(MSG_SCAN_FINISHED, on_finished)
ON_COMMAND(MSG_STOP_SCAN, on_stop_scan)
ON_COMMAND(MSG_SAVE, on_save_do_search)
ON_COMMAND(MSG_SCAN_DB_FULL, on_db_is_full)
ON_COMMAND(MSG_SCAN_NIT_FOUND, on_nit_found)
END_MSGPROC(do_search_cont_proc, cont_class_proc)
