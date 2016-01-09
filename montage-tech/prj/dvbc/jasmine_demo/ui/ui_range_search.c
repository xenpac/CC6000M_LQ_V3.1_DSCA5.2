/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_range_search.h"
#include "ui_play_api.h"
#include "ui_mainmenu.h"
enum control_id
{
  IDC_SPECIFY_TP_FRM = 1,
  IDC_SIGNAL_INFO_FRM,
};

enum specify_tp_frm_id
{
  IDC_SPECIFY_TP_FREQ_START = 1,
  IDC_SPECIFY_TP_FREQ_END,
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

u32 g_DVBCFullFreqTable_Chi[] =
{
  115000,
  123000,
  131000,
  139000,
  147000,
  155000,
  163000,
  171000,
  179000,
  187000,
  195000,
  203000,
  211000,
  219000,
  227000,
  235000,
  243000,
  251000,
  259000,
  267000,
  275000,
  283000,
  291000,
  299000,
  307000,
  315000,
  323000,
  331000,
  339000,
  347000,
  355000,
  363000,
  371000,
  379000,
  387000,
  395000,
  403000,
  411000,
  419000,
  427000,
  435000,
  443000,
  451000,
  459000,
  467000,
  474000,
  482000,
  490000,
  498000,
  506000,
  514000,
  522000,
  530000,
  538000,
  546000,
  554000,
  562000,
  570000,
  578000,
  586000,
  594000,
  602000,
  610000,
  618000,
  626000,
  634000,
  642000,
  650000,
  658000,
  666000,
  674000,
  682000,
  690000,
  698000,
  706000,
  714000,
  722000,
  730000,
  738000,
  746000,
  754000,
  762000,
  770000,
  778000,
  786000,
  794000,
  802000,
  810000,
  818000,
  826000,
  834000,
  842000,
  850000,
  858000,
};

u32 g_DVBCFullFreqTable_Ind[] =
{    
  50000,
  58000,
  66000,
  74000,
  82000,
  90000,
  98000,
  106000,
  114000,
  122000,
  130000,
  138000,
  146000,
  154000,
  162000,
  170000,
  178000,
  186000,
  194000,
  202000,
  210000,
  218000,
  226000,
  234000,
  242000,
  250000,
  258000,
  266000,
  274000,
  282000,
  290000,
  298000,
  306000,
  314000,
  322000,
  330000,
  338000,
  346000,
  354000,
  362000,
  370000,
  378000,
  386000,
  394000,
  402000,
  410000,
  418000,
  426000,
  434000,
  442000,
  450000,
  458000,
  466000,
  474000,
  482000,
  490000,
  498000,
  506000,
  514000,
  522000,
  530000,
  538000,
  546000,
  554000,
  562000,
  570000,
  578000,
  586000,
  594000,
  602000,
  610000,
  618000,
  626000,
  634000,
  642000,
  650000,
  658000,
  666000,
  674000,
  682000,
  690000,
  698000,
  706000,
  714000,
  722000,
  730000,
  738000,
  746000,
  754000,
  762000,
  770000,
  778000,
  786000,
  794000,
  802000,
  810000,
  818000,
  826000,
  834000,
  842000,
  850000,
  858000,
};

u32 g_DVBCFullFreqTable_Ind_hxd[] =
{    
  50000,
  57000,
  64000,
  121000,
  128000,
  135000,
  142000,
  149000,
  156000,
  163000,
  170000,
  177000,
  184000,
  191000,
  198000,
  205000,
  212000,
  219000,
  226000,
  233000,
  240000,
  247000,
  254000,
  261000,
  268000,
  275000,
  282000,
  289000,
  296000,
  306000,
  314000,
  322000,
  330000,
  338000,
  346000,
  354000,
  362000,
  370000,
  378000,
  386000,
  394000,
  402000,
  410000,
  418000,
  426000,
  434000,
  442000,
  450000,
  458000,
  466000,
  474000,
  482000,
  490000,
  498000,
  506000,
  514000,
  522000,
  530000,
  538000,
  546000,
  554000,
  562000,
  570000,
  578000,
  586000,
  594000,
  602000,
  610000,
  618000,
  626000,
  634000,
  642000,
  650000,
  658000,
  666000,
  674000,
  682000,
  690000,
  698000,
  706000,
  714000,
  722000,
  730000,
  738000,
  746000,
  754000,
  762000,
  770000,
  778000,
  786000,
  794000,
  802000,
  810000,
  818000,
  826000,
  834000,
  842000,
  850000,
  858000,
};

u32 g_DVBCFullFreqTable_Ind_jzmex[] =
{    
  57000,
  63000,
  69000,
  79000,
  85000,
  123000,
  129000,
  135000,
  141000,
  147000,
  153000,
  159000,
  165000,
  171000,
  177000,
  183000,
  189000,
  195000,
  201000,
  207000,
  213000,
  219000,
  225000,
  231000,
  237000,
  243000,
  249000,
  255000,
  261000,
  267000,
  273000,
  279000,
  285000,
  291000,
  297000,  
  303000,
  309000,
  315000,
  321000,
  327000,
  333000,
  339000,
  345000,
  351000,
  357000,
  363000,
  369000,
  375000,
  381000,
  387000,
  393000,
  399000,  
  405000,
  411000,
  417000,
  423000,
  429000,
  435000,
  441000,
  447000,
  453000,
  459000,
  465000,
  471000,
  477000,
  483000,
  489000,
  495000,
  501000,
  507000,
  513000,
  519000,
  525000,
  531000,
  537000,
  543000,
  549000,
  555000,
  561000,
  567000,
  573000,
  579000,
  585000,
  591000,
  597000,
  603000,
  609000,
  615000,
  621000,
  627000,
  633000,
  639000,
  645000,
  651000,
  657000,
  663000,
  669000,
  675000,
  681000,
  687000,
  693000,
  699000,
  705000,
  711000,
  717000,
  723000,
  729000,
  735000,
  741000,
  747000,
  753000,
  759000,
  765000,
  771000,
  777000,
  783000,
  789000,
  795000,
  801000,
  807000,
  813000,
  819000,
  825000,
  831000,
  837000,
  843000,
  849000,
  855000,
  861000,
};

static u8 full_tp_count = 0;
u32 *p_FreqTable = NULL;

static comm_help_data_t2 range_search_help_data[] = //help bar data
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

u16 range_search_cont_keymap(u16 key);

RET_CODE range_search_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

u16 range_search_select_keymap(u16 key);

RET_CODE range_search_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 range_search_tp_frm_keymap(u16 key);
RET_CODE range_search_tp_frm_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE range_search_num_edit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE range_search_qam_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static u16 range_search_pwdlg_keymap(u16 key);
RET_CODE range_search_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

static void range_search_check_signal(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  dvbs_tp_node_t tp = {0};
  control_t *p_ctrl;
  u32 freq;
  u32 sym;
  u8 demod;
  ui_scan_param_t scan_param;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;

  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_FREQ_START);
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
  u32 sym;
  u8 demod;
  ui_scan_param_t scan_param;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
  u32 freq_start, freq_end;
  u8 i;
  
  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_FREQ_START);
  freq_start= ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_FREQ_END);
  freq_end= ui_comm_numedit_get_num(p_ctrl);
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_SPECIFY_TP_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);
  
  
  scan_param.tp.freq = freq_start;
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
  scan_param.nit_type = NIT_SCAN_ONCE;
  if(freq_start < freq_end)
  {
    ui_scan_param_init();
    for(i =0; i< full_tp_count; i++)
    {
      if(p_FreqTable[i] == freq_start)
        break;
    }
    if(i == full_tp_count)
    {
      scan_param.tp.freq = freq_start;
      ui_scan_param_add_tp(&scan_param.tp);
    }
    for(i =0; i< full_tp_count; i++)
    {
      if( p_FreqTable[i] >= freq_start && p_FreqTable[i] <= freq_end)
      {
        scan_param.tp.freq = p_FreqTable[i];
        ui_scan_param_add_tp(&scan_param.tp);
      }
    }
    ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_RANGE, 0);
  }
  else if(freq_start == freq_end)
  {
    ui_scan_param_init();
    
    scan_param.tp.freq = freq_start;
    ui_scan_param_add_tp(&scan_param.tp);
    ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_RANGE, 0);
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_FREQUENCY_RANGE_ERROR, NULL, 2000);
  }

  return SUCCESS;
}

RET_CODE open_range_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[RANGE_SEARCH_ITEM_CNT], *p_specify_tp_frm, *p_searching_frm;
  control_t *p_bar;
  u8 i = 0;
  u16 y;
  dvbc_lock_t tp_main, tp_begin, tp_end;
   
  u16 stxt[RANGE_SEARCH_ITEM_CNT] =
  { IDS_FREQUENCY_START, IDS_FREQUENCY_END, IDS_SYMBOL, IDS_CONSTELLATION};

  //get main tp
  sys_status_get_main_tp1(&tp_main);

  //get range tp begin
  sys_status_get_range_begin_tp(&tp_begin);
  
  //get range tp end
  sys_status_get_range_end_tp(&tp_end);

  //stop monitor service
  {
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
  }

  switch(g_customer.country)
  {
  case COUNTRY_INDIA:
    p_FreqTable = g_DVBCFullFreqTable_Ind;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Ind)/sizeof(s32);
    break;
    
  case COUNTRY_CHINA:
    p_FreqTable = g_DVBCFullFreqTable_Chi;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Chi)/sizeof(s32);
    break;
    
  default:
    p_FreqTable = g_DVBCFullFreqTable_Chi;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Chi)/sizeof(s32);
    break;
  }
  
  if(CUSTOMER_HUAXIDA == CUSTOMER_ID)
  {
    p_FreqTable = g_DVBCFullFreqTable_Ind_hxd;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Ind_hxd)/sizeof(s32);
  }
  
   if(CUSTOMER_JIZHONGMEX == CUSTOMER_ID)
  {
    p_FreqTable = g_DVBCFullFreqTable_Ind_jzmex;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Ind_jzmex)/sizeof(s32);
  }
  p_cont = ui_comm_root_create(ROOT_ID_RANGE_SEARCH,
                               0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,
                               IM_TITLEICON_TV,
                               IDS_RANGE_SEARCH);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, range_search_cont_keymap);
  ctrl_set_proc(p_cont, range_search_cont_proc);
  
  //specify tp frm
  p_specify_tp_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SPECIFY_TP_FRM,
                            RANGE_SEARCH_SPECIFY_TP_FRM_X, RANGE_SEARCH_SPECIFY_TP_FRM_Y,
                            RANGE_SEARCH_SPECIFY_TP_FRM_W, RANGE_SEARCH_SPECIFY_TP_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_specify_tp_frm, RSI_RANGE_SEARCH_FRM, RSI_RANGE_SEARCH_FRM, RSI_RANGE_SEARCH_FRM);
  ctrl_set_keymap(p_specify_tp_frm, range_search_tp_frm_keymap);
  ctrl_set_proc(p_specify_tp_frm, range_search_tp_frm_proc);

  memset(p_ctrl, 0, 4 * RANGE_SEARCH_ITEM_CNT);
  y = RANGE_SEARCH_ITEM_Y;
  for(i = 0; i < RANGE_SEARCH_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
      case 2:
        p_ctrl[i] = ui_comm_numedit_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ_START + i),
                                             RANGE_SEARCH_ITEM_X, y,
                                             RANGE_SEARCH_ITEM_LW,
                                             RANGE_SEARCH_ITEM_RW);
        ui_comm_ctrl_set_proc(p_ctrl[i], range_search_num_edit_proc);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_numedit_set_postfix(p_ctrl[i], (i==2) ? IDS_UNIT_SYMB: IDS_UNIT_FREQ);

        if(i==0)
        {
          if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
            ui_comm_numedit_set_param(p_ctrl[0], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 57000, 861000, 6, 0);
          else
            ui_comm_numedit_set_param(p_ctrl[0], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 45000, 862000, 6, 0);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[0], tp_begin.tp_freq);
        }
        else if(i==1)
        {
          if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
            ui_comm_numedit_set_param(p_ctrl[1], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 57000, 861000, 6, 0);
          else
            ui_comm_numedit_set_param(p_ctrl[1], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 45000, 862000, 6, 0);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[1], tp_end.tp_freq);
        }
        else
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 0, 9999, 4, 0);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[i], tp_main.tp_sym);

        }
        break;
        
      case 3:
        p_ctrl[i] = ui_comm_select_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ_START + i),
                                            RANGE_SEARCH_ITEM_X, y,
                                            RANGE_SEARCH_ITEM_LW,
                                            RANGE_SEARCH_ITEM_RW);
        ui_comm_ctrl_set_proc(p_ctrl[i], range_search_qam_proc);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);

         if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE,
                                     CBOX_WORKMODE_STATIC, 2,
                                     CBOX_ITEM_STRTYPE_STRID,
                                     NULL);
          ui_comm_select_set_content(p_ctrl[i], 0, IDS_QAM64);
          ui_comm_select_set_content(p_ctrl[i], 1, IDS_QAM256);
          if(tp_main.nim_modulate == 8)
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
          ui_comm_select_set_focus(p_ctrl[i], tp_main.nim_modulate);
        }
        break;
       
      case 4:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_SPECIFY_TP_FREQ_START + i),
                           RANGE_SEARCH_ITEM_X, y,
                           RANGE_SEARCH_ITEM_LW + RANGE_SEARCH_ITEM_RW, RANGE_SEARCH_ITEM_H,
                           p_specify_tp_frm, 0);
        ctrl_set_keymap(p_ctrl[i], range_search_select_keymap);
        ctrl_set_proc(p_ctrl[i], range_search_select_proc);
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
                        (u8)((i - 1 + RANGE_SEARCH_ITEM_CNT) % RANGE_SEARCH_ITEM_CNT + 1), /* up */
                        0, /* right */
                        (u8)((i + 1) % RANGE_SEARCH_ITEM_CNT + 1)); /* down */

    y += RANGE_SEARCH_ITEM_H +  RANGE_SEARCH_ITEM_V_GAP;
  }
  
  //searching info frm
  p_searching_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SIGNAL_INFO_FRM,
                            RANGE_SEARCH_SIG_INFO_FRM_X, RANGE_SEARCH_SIG_INFO_FRM_Y,
                            RANGE_SEARCH_SIG_INFO_FRM_W, RANGE_SEARCH_SIG_INFO_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_searching_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE); 
  // pbar
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_STRENGTH_BAR,
                               RANGE_SEARCH_SIG_STRENGTH_PBAR_X,
                               RANGE_SEARCH_SIG_STRENGTH_PBAR_Y,
                               RANGE_SEARCH_SIG_STRENGTH_PBAR_W,
                               RANGE_SEARCH_SIG_STRENGTH_PBAR_H,
                               RANGE_SEARCH_SIG_STRENGTH_NAME_X, 
                               RANGE_SEARCH_SIG_STRENGTH_NAME_Y, 
                               RANGE_SEARCH_SIG_STRENGTH_NAME_W, 
                               RANGE_SEARCH_SIG_STRENGTH_NAME_H,
                               RANGE_SEARCH_SIG_STRENGTH_PERCENT_X,
                               RANGE_SEARCH_SIG_STRENGTH_PERCENT_Y,
                               RANGE_SEARCH_SIG_STRENGTH_PERCENT_W,
                               RANGE_SEARCH_SIG_STRENGTH_PERCENT_H);
  
  ui_comm_bar_set_param(p_bar, IDS_RFLEVEL, 0, 100, 100);

  ui_comm_bar_set_style(p_bar,
                        RSI_RANGE_SEARCH_PBAR_BG, RSI_RANGE_SEARCH_PBAR_MID,
                        RSI_IGNORE, FSI_WHITE,
                        RSI_PBACK, FSI_WHITE);
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dBuv");

  // signal BER pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_BER_BAR,
                               RANGE_SEARCH_SIG_BER_PBAR_X,
                               RANGE_SEARCH_SIG_BER_PBAR_Y,
                               RANGE_SEARCH_SIG_BER_PBAR_W,
                               RANGE_SEARCH_SIG_BER_PBAR_H,
                               RANGE_SEARCH_SIG_BER_NAME_X, 
                               RANGE_SEARCH_SIG_BER_NAME_Y, 
                               RANGE_SEARCH_SIG_BER_NAME_W, 
                               RANGE_SEARCH_SIG_BER_NAME_H,
                               RANGE_SEARCH_SIG_BER_PERCENT_X,
                               RANGE_SEARCH_SIG_BER_PERCENT_Y,
                               RANGE_SEARCH_SIG_BER_PERCENT_W,
                               RANGE_SEARCH_SIG_BER_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_BER, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_RANGE_SEARCH_PBAR_BG, RSI_RANGE_SEARCH_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
  
  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"E-6");

  // signal SNR pbar  
  p_bar = ui_comm_bar_create(p_searching_frm, IDC_SEARCH_SIG_SNR_BAR,
                               RANGE_SEARCH_SIG_SNR_PBAR_X,
                               RANGE_SEARCH_SIG_SNR_PBAR_Y,
                               RANGE_SEARCH_SIG_SNR_PBAR_W,
                               RANGE_SEARCH_SIG_SNR_PBAR_H,
                               RANGE_SEARCH_SIG_SNR_NAME_X, 
                               RANGE_SEARCH_SIG_SNR_NAME_Y, 
                               RANGE_SEARCH_SIG_SNR_NAME_W, 
                               RANGE_SEARCH_SIG_SNR_NAME_H,
                               RANGE_SEARCH_SIG_SNR_PERCENT_X,
                               RANGE_SEARCH_SIG_SNR_PERCENT_Y,
                               RANGE_SEARCH_SIG_SNR_PERCENT_W,
                               RANGE_SEARCH_SIG_SNR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_CN, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_RANGE_SEARCH_PBAR_BG, RSI_RANGE_SEARCH_PBAR_MID,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);

  ui_comm_tp_bar_update(p_bar, 0, TRUE, (u8*)"dB");

  ui_comm_help_create2(&range_search_help_data[0], p_cont,FALSE);
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  ui_enable_signal_monitor(TRUE);
  range_search_check_signal(p_specify_tp_frm, 0, 0, 0);

  return SUCCESS;
}

static RET_CODE on_range_search_pwdlg_correct(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();

  return open_range_search(0, 0);
}

static RET_CODE on_range_search_pwdlg_exit(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  
  return SUCCESS;
}

RET_CODE preopen_range_search(u32 para1, u32 para2)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y,
    IDS_MSG_INPUT_PASSWORD,
    range_search_pwdlg_keymap,
    range_search_pwdlg_proc,
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
    open_range_search(0, 0);
  return SUCCESS;
}

static RET_CODE on_range_search_cont_focus_change_down(control_t *p_ctrl, u16 msg,
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
  ui_comm_help_create2(&range_search_help_data[0], p_parent,FALSE);
  else 
  ui_comm_help_create2(&range_search_help_data[ctrl_id], p_parent,FALSE);
  
  ctrl_paint_ctrl(p_help, TRUE);
   
  return ERR_NOFEATURE;
}

static RET_CODE on_range_search_cont_focus_change_up(control_t *p_ctrl, u16 msg,
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
  ui_comm_help_create2(&range_search_help_data[4], p_parent,FALSE);
  else
  ui_comm_help_create2(&range_search_help_data[ctrl_id-2], p_parent,FALSE);
  
  ctrl_paint_ctrl(p_help, TRUE);

  return ERR_NOFEATURE;
}

static RET_CODE on_range_search_signal_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
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

static RET_CODE on_range_search_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  range_search_check_signal(p_ctrl->p_parent->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_range_search_change_qam(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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

  range_search_check_signal(p_ctrl->p_parent->p_parent, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_range_search_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  //ui_reset_cur_tp();
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(range_search_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(range_search_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(range_search_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_range_search_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_range_search_exit)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_range_search_signal_update)
END_MSGPROC(range_search_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(range_search_select_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(range_search_select_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(range_search_select_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_select_msg_start_search)
END_MSGPROC(range_search_select_proc, text_class_proc)

BEGIN_KEYMAP(range_search_tp_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  //ON_EVENT(V_KEY_OK, MSG_START_SCAN)
END_KEYMAP(range_search_tp_frm_keymap, NULL)

BEGIN_MSGPROC(range_search_tp_frm_proc, cont_class_proc)
  //ON_COMMAND(MSG_START_SCAN, start_auto_scan)
  ON_COMMAND(MSG_FOCUS_UP,on_range_search_cont_focus_change_up) 
  ON_COMMAND(MSG_FOCUS_DOWN,on_range_search_cont_focus_change_down)  
END_MSGPROC(range_search_tp_frm_proc, cont_class_proc)

BEGIN_MSGPROC(range_search_num_edit_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_NUMBER, on_range_search_change_freq_symbol)
END_MSGPROC(range_search_num_edit_proc, ui_comm_num_proc)

BEGIN_MSGPROC(range_search_qam_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_range_search_change_qam)
  ON_COMMAND(MSG_DECREASE, on_range_search_change_qam)
END_MSGPROC(range_search_qam_proc, cbox_class_proc)

BEGIN_KEYMAP(range_search_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(range_search_pwdlg_keymap, NULL)

BEGIN_MSGPROC(range_search_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_range_search_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_range_search_pwdlg_exit)
END_MSGPROC(range_search_pwdlg_proc, cont_class_proc)

