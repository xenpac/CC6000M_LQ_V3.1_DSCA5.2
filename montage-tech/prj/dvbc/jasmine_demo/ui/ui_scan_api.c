/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

// param package
struct
{
  u8 type;
  BOOL is_paused;
  scan_input_param_t *p_param;
}g_scan_param = {0, FALSE, NULL};
s32 g_DVBCFfreqTablezhilingby[][2] =
 {
    {227000,6875}, {235000,6875}, {243000,6875}, {251000,6875}, {259000,6875}, 
    {267000,6875}, {275000,6875}, {283000,6875}, {291000,6875}, {299000,6875},
    {307000,6875}, {315000,6875}, {323000,6875}, {331000,6875}, {339000,6875},
    {347000,6875}, {355000,6875}, {363000,6875},{371000,6875},{379000,6875},
    {387000,6875}, {395000,6875},
};
u32 g_DVBCFullFreqTablejzmex[] =
{
  57000,63000,69000,79000,85000,123000,129000,135000,141000,147000,153000,159000,165000,171000,177000,183000,
  189000,195000,201000,207000,213000,219000,225000,231000,237000,243000,249000,255000,261000,267000,273000,279000,
  285000,291000,297000,303000,309000,315000,321000,327000,333000,339000,345000,351000,357000,363000,369000,375000,
  381000,387000,393000,399000,405000,411000,417000,423000,429000,435000,441000,447000,453000,459000,465000,471000,
  477000,483000,489000,495000,501000,507000,513000,519000,525000,531000,537000,543000,549000,555000,561000,567000,
  573000,579000,585000,591000,597000,603000,609000,615000,621000,627000,633000,639000,645000,651000,657000,663000,
  669000,675000,681000,687000,693000,699000,705000,711000,717000,723000,729000,735000,741000,747000,753000,759000,
  765000,771000,777000,783000,789000,795000,801000,807000,813000,819000,825000,831000,837000,843000,849000,855000,
  861000,
};
u16 ui_scan_evtmap(u32 event);

#define DVBC_TP_ALL_COUNT_ZHILING_BY (sizeof(g_DVBCFfreqTablezhilingby) / (sizeof(s32) * 2))

void ui_scan_param_init(void)
{
#if 1
  // use common block
  ap_scan_set_attach_block(TRUE, BLOCK_AV_BUFFER);

  g_scan_param.p_param = ap_scan_param_buffer();
  MT_ASSERT(g_scan_param.p_param != NULL);

  memset(g_scan_param.p_param, 0, sizeof(scan_input_param_t));
#endif
}

extern void set_prompt_flag_of_nit_change(BOOL b);
void ui_scan_param_set_type(u8 type, u8 chan_type, u8 free_only, nit_scan_type_t nit_type)
{
#if 0
  scan_input_param_t *p_param = g_scan_param.p_param;
  MT_ASSERT(p_param != NULL);
  
  g_scan_param.type = type;
  
  p_param->scan_data.nit_scan_type = nit_type;
  p_param->chan_type = chan_type;
  p_param->is_free_only = free_only;
#endif

  scan_input_param_t *p_param = g_scan_param.p_param;
  
  MT_ASSERT(p_param != NULL);
  
  g_scan_param.type = type;

  #ifdef CHANGHONG_LOWCOST
  p_param->scan_data.bat_scan_type = BAT_SCAN_WITHOUT;
  #else
  p_param->scan_data.bat_scan_type = BAT_SCAN_ALL_TP;
  #endif
  p_param->scan_data.scramble_scan_origin = SCAN_SCRAMBLE_FROM_PMT;
  p_param->scan_data.nit_scan_type = nit_type;
  p_param->chan_type = (chan_type_t)chan_type;
  p_param->is_free_only = free_only;
  //p_param->scan_data.scramble_scan_origin = scram_org;

  if(nit_type != NIT_SCAN_WITHOUT)
  {
    set_prompt_flag_of_nit_change(FALSE);
  }
}


u8 ui_scan_param_get_type(void)
{
  return g_scan_param.type;
}

nit_scan_type_t ui_scan_param_nit_type(void)
{
  return g_scan_param.p_param->scan_data.nit_scan_type;
}

u16 ui_scan_param_get_sat_num(void)
{
  return g_scan_param.p_param->scan_data.total_sat;
}
void add_dvbc_all_tp_zhinling_by(void)
{
  u8 i = 0;
  dvbs_tp_node_t tp = {0};
  for (i = 0; i < DVBC_TP_ALL_COUNT_ZHILING_BY; i++)
  {
    tp.freq = g_DVBCFfreqTablezhilingby[i][0];
    tp.sym = g_DVBCFfreqTablezhilingby[i][1];
    tp.nim_modulate = NIM_MODULA_QAM64;
    ui_scan_param_add_tp(&tp);
  }
}

void add_dvbc_all_tp_jzmex(void)
{
   u8 i = 0;
  dvbs_tp_node_t tp = {0};
  dvbc_lock_t dvbc_tp = {0};

  sys_status_get_main_tp1(&dvbc_tp);
  for (i = 0; i < sizeof(g_DVBCFullFreqTablejzmex)/sizeof(u32); i++)
  {
    tp.freq = g_DVBCFullFreqTablejzmex[i];
    tp.sym = dvbc_tp.tp_sym;
    tp.nim_modulate = dvbc_tp.nim_modulate;
    ui_scan_param_add_tp(&tp);
  }
}

//lint -e732
BOOL ui_scan_param_add_tp(dvbs_tp_node_t *p_tp_info)
{
  scan_preset_data_t *p_scan_buf = &g_scan_param.p_param->scan_data;
  u16 cur_tp_num = p_scan_buf->total_tp;
  scan_tp_info_i_t *p_buf_tp = &p_scan_buf->tp_list[cur_tp_num];

  MT_ASSERT(p_scan_buf != NULL);
  if(p_scan_buf->total_tp >= MAX_TP_NUM_SUPPORTED)
  {
    return FALSE;
  } 

  trans_tp_info(&p_buf_tp->tp_info, p_tp_info);
  //Set tp info
  p_buf_tp->id = p_tp_info->id;
  p_buf_tp->sat_id = p_tp_info->sat_id;
#ifdef DTMB_PROJECT
  p_buf_tp->tp_info.nim_type = p_tp_info->nim_type;
#endif

  //Update current satellite information
  p_scan_buf->total_tp++;
  return TRUE;
}
//lint +e732

void ui_scan_param_set_pid(u16 v_pid, u16 a_pid, u16 pcr_pid)
{
  scan_input_param_t *p_param = g_scan_param.p_param;
  MT_ASSERT(p_param != NULL);

  p_param->pid_scan_enable = TRUE;
  
  p_param->pid_parm.video_pid = v_pid;
  p_param->pid_parm.audio_pid = a_pid;
  p_param->pid_parm.pcr_pid = pcr_pid;
}


void ui_init_scan(void)
{
#if 1
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_SCAN;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_SCAN, ui_scan_evtmap);
  fw_register_ap_msghost(APP_SCAN, ROOT_ID_DO_SEARCH);
#endif
}


void ui_release_scan(void)
{
#if 1
 cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_SCAN;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_msghost(APP_SCAN, ROOT_ID_DO_SEARCH);
  fw_unregister_ap_evtmap(APP_SCAN);
#endif
}

BOOL ui_start_scan(void)
{
#if 1
  cmd_t cmd = {SCAN_CMD_START_SCAN};
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);  
  switch(g_scan_param.type)
  {
    case USC_BLIND_SCAN:
      g_scan_param.p_param->scan_mode = BLIND_SCAN;
      break;
    case USC_PID_SCAN:
    case USC_TP_SCAN:
    case USC_PRESET_SCAN:
      g_scan_param.p_param->scan_mode = TP_SCAN;
      break;
    case USC_DVBC_FULL_SCAN:
      g_scan_param.p_param->scan_mode = DVBC_SCAN;
      break;
    case USC_DVBC_MANUAL_SCAN:
      g_scan_param.p_param->scan_mode = DVBC_SCAN;
      break;
    case USC_DTMB_SCAN:
      g_scan_param.p_param->scan_mode = DTMB_SCAN;
      nc_set_lock_mode(nc_handle, TUNER0, SYS_DTMB);
      break;
    default:
      MT_ASSERT(0);
      return FALSE;
  }
#ifndef DTMB_PROJECT
  g_scan_param.p_param->scan_mode = DVBC_SCAN;
#endif  
  cmd.data1 = (u32)ap_scan_param_apply();
  cmd.data2 = 1;
  ap_frm_do_command(APP_SCAN, &cmd);

  g_scan_param.is_paused = FALSE;
#endif
 
  return TRUE;
}

void ui_stop_scan(void)
{
#if 1
 cmd_t cmd = {SCAN_CMD_CANCEL_SCAN};

  if(g_scan_param.is_paused)
  {
    ui_resume_scan();
  }

  ap_frm_do_command(APP_SCAN, &cmd);
#endif
}


void ui_pause_scan(void)
{
#if 1
 cmd_t cmd;

  if(g_scan_param.is_paused)
  {
    OS_PRINTF("ui_pause_scan: already paused, ERROR!\n");
    return;
  }

  cmd.id = SCAN_CMD_PAUSE_SCAN;
  cmd.data1 = 0;
  cmd.data2 = 0;  
  ap_frm_do_command(APP_SCAN, &cmd);

  cmd.id = AP_FRM_CMD_PAUSE_APP;
  cmd.data1 = APP_SCAN;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  g_scan_param.is_paused = TRUE;
#endif
}


void ui_resume_scan(void)
{
#if 1
 cmd_t cmd;

  if(!g_scan_param.is_paused)
  {
    OS_PRINTF("ui_resume_scan: not paused, ERROR!\n");
    return;
  }

  cmd.id = AP_FRM_CMD_RESUME_APP;
  cmd.data1 = APP_SCAN;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  cmd.id = SCAN_CMD_RESUME_SCAN;
  cmd.data1 = 0;
  cmd.data2 = 0;  
  ap_frm_do_command(APP_SCAN, &cmd);

  g_scan_param.is_paused = FALSE;
#endif
}

BEGIN_AP_EVTMAP(ui_scan_evtmap)
  CONVERT_EVENT(SCAN_EVT_PG_FOUND,  MSG_SCAN_PG_FOUND)
  CONVERT_EVENT(SCAN_EVT_TP_FOUND,  MSG_SCAN_TP_FOUND)
  CONVERT_EVENT(SCAN_EVT_PROGRESS,  MSG_SCAN_PROGRESS)
  CONVERT_EVENT(SCAN_EVT_SAT_SWITCH, MSG_SCAN_SAT_FOUND)
  CONVERT_EVENT(SCAN_EVT_NO_MEMORY,   MSG_SCAN_DB_FULL)
  CONVERT_EVENT(SCAN_EVT_FINISHED,  MSG_SCAN_FINISHED)
  CONVERT_EVENT(SCAN_EVT_NIT_FOUND,  MSG_SCAN_NIT_FOUND)
END_AP_EVTMAP(ui_scan_evtmap)


