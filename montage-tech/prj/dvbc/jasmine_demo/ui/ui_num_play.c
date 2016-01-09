/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_num_play.h"

enum control_id
{
  IDC_NUM_FRM = 1,
  IDC_ICON_NUM_INDEX0,
  IDC_ICON_NUM_INDEX1,
  IDC_ICON_NUM_INDEX2,
  IDC_ICON_NUM_INDEX3,
};

static u8 num_icon_idc[] = 
{
	IDC_ICON_NUM_INDEX0,
	IDC_ICON_NUM_INDEX1,
	IDC_ICON_NUM_INDEX2,
	IDC_ICON_NUM_INDEX3,
};

static comm_dlg_data_t dlg_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  COMM_DLG_X, COMM_DLG_Y,
  COMM_DLG_W, COMM_DLG_H,
  IDS_MSG_INVALID_NUMBER,
  4000,
};

static u8 g_num_play_bit = 0;
static u16 g_cur_num = 0;
static u16 g_prog_cnt = 0;
BOOL is_fullsrc = FALSE;
static u16 num_icon_count = sizeof(num_icon_idc)/sizeof(u8);
extern BOOL have_logic_number();

static u16 find_prog_in_cur_group(u16 logic_num)
{
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 count = db_dvbs_get_count(view_id);
  u16 i, pg_id, pg_pos = INVALIDPOS;
  u8 org_mode = 0;
  u16 view_type = 0;
  dvbs_prog_node_t prog = {0};

  for(i=0; i<count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);

    if(logic_num == prog.logical_num)
    {
      pg_pos = i;
      break;
    }
  }

  if( (pg_pos == INVALIDPOS)
      && (sys_status_get_curn_group() != 0)/*Group ALL*/)
  {
    //find in ALL
    ui_cache_view();

    org_mode = sys_status_get_curn_prog_mode();
    switch (org_mode)
    {
      case CURN_MODE_TV:
        view_type = DB_DVBS_ALL_TV;
        break;
      case CURN_MODE_RADIO:
        view_type = DB_DVBS_ALL_RADIO;
        break;
      default:
        view_type = DB_DVBS_ALL_TV;
        break;
    }

    view_id = ui_dbase_create_view(view_type, 0, NULL);
    count = db_dvbs_get_count(view_id);   
    ui_dbase_set_pg_view_id(view_id);
    sys_status_set_curn_group(0);

    for(i=0; i<count; i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      db_dvbs_get_pg_by_id(pg_id, &prog);

      if(logic_num == prog.logical_num)
      {
        pg_pos = i;
        break;
      }
    }

    if(pg_pos == INVALIDPOS)
    {
      ui_restore_view();
    }
    else
    {
      ui_cache_view();
    }
  }
  
  return pg_pos;
}

static void jump_to_prog(control_t *p_cont)
{
  u16 prog_id;
  u16 prog_pos = 0;
  /* jump to prog */
  #ifdef LCN_SWITCH_DS_JHC
  {
    prog_pos = find_prog_in_cur_group(g_cur_num);
	  if(prog_pos == INVALIDPOS)
	  {
	    if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
	    { 
	      return;
	    }
	    
	    ui_comm_dlg_open(&dlg_data);
	    ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
	    return;
	  }
  }
  #else
  if(have_logic_number())
  {
	  /* jump to prog */
	  prog_pos = find_prog_in_cur_group(g_cur_num);
	  if(prog_pos == INVALIDPOS)
	  {
	    if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
	    { 
	      return;
	    }
	    if(CUSTOMER_ID != CUSTOMER_AISAT_DEMO)
	    	ui_comm_dlg_open(&dlg_data);
	    else	
	     	ui_comm_showdlg_open(NULL, IDS_MSG_INVALID_NUMBER, NULL, 2000);
	    ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
	    return;
	  }
  }
  else
  {
  	if (g_cur_num > g_prog_cnt || g_cur_num == 0)
  	{
    	    ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
    	    ui_comm_dlg_open(&dlg_data);
           return;
  	}
  }
  #endif
  if(is_fullsrc)
  {
    #ifdef LCN_SWITCH_DS_JHC
    prog_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), prog_pos);
    #else
    if(have_logic_number())
    {
      prog_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), prog_pos);
    }
    else
    {
      prog_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), g_cur_num - 1 /* base on 1*/);
    }
    #endif
    ui_play_prog(prog_id, FALSE);
  }
  else
  {
    if(have_logic_number())
    {
      fw_notify_parent(ROOT_ID_NUM_PLAY, NOTIFY_T_MSG,
        FALSE, MSG_NUM_SELECT, prog_pos, 0);
    }
    else
    {
       fw_notify_parent(ROOT_ID_NUM_PLAY, NOTIFY_T_MSG,
       FALSE, MSG_NUM_SELECT, (g_cur_num - 1), 0);
    }
  }

  /* close */
  ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
  return;
}


static BOOL input_num(control_t *parent, u8 num, BOOL is_update)
{
  control_t *p_ctrl, *p_subctrl;
  u16 temp = 0;
  int i =0;
  u16 number_icon_rid[] =
  {
    IM_NUMBER_0, IM_NUMBER_1, IM_NUMBER_2, IM_NUMBER_3,
    IM_NUMBER_4, IM_NUMBER_5, IM_NUMBER_6, IM_NUMBER_7,
    IM_NUMBER_8, IM_NUMBER_9,
  };

  p_ctrl = ctrl_get_child_by_id(parent, IDC_NUM_FRM);

  if (g_num_play_bit < NUM_PLAY_CNT)
  {
    g_cur_num = (u16)(g_cur_num * 10 + num);
    g_num_play_bit++;
  }
  else
  {
    g_cur_num = num;
    g_num_play_bit = 1;
  }

  // set number
  temp = g_cur_num;
  for (i = num_icon_count - 1; i >= 0; i--)
  {	
    p_subctrl = ctrl_get_child_by_id(p_ctrl, num_icon_idc[i]);
    bmap_set_content_by_id(p_subctrl, number_icon_rid[temp%10]);
	  temp /= 10;
  }

  if (is_update)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  if( g_num_play_bit == NUM_PLAY_CNT )
  {
    fw_tmr_destroy(ROOT_ID_NUM_PLAY, MSG_SELECT);
    ctrl_process_msg(parent, MSG_SELECT, 0, 0);
  }

  return TRUE;
}


u16 num_play_cont_keymap(u16 key);

RET_CODE num_play_cont_proc(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2);

RET_CODE open_num_play(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_ctrl;
  u8 i = 0;

  // init
  g_num_play_bit = 0;
  g_cur_num = 0;
  if ((g_prog_cnt = db_dvbs_get_count(ui_dbase_get_pg_view_id())) == 0)
  {
    return ERR_FAILURE;
  }

  is_fullsrc = ui_is_fullscreen_menu(fw_get_focus_id());

  OS_PRINTF("is fullsrc[%d]\n", is_fullsrc);

  if(is_fullsrc)
  {
    if(g_customer.customer_id == CUSTOMER_BOYUAN || g_customer.customer_id == CUSTOMER_YINHE || 
      g_customer.customer_id == CUSTOMER_TAIHUI_DIVI || g_customer.customer_id == CUSTOMER_JINGHUICHENG_DIVI)
    {
      p_cont = fw_create_mainwin(ROOT_ID_NUM_PLAY,
                                      1000, NUM_PLAY_CONT_Y,
                                      NUM_PLAY_CONT_W, NUM_PLAY_CONT_H,
                                      ROOT_ID_INVALID, 0,
                                      OBJ_ATTR_ACTIVE, 0);
    }
	else
    {
      p_cont = fw_create_mainwin(ROOT_ID_NUM_PLAY,
                                      NUM_PLAY_CONT_X, NUM_PLAY_CONT_Y,
                                      NUM_PLAY_CONT_W, NUM_PLAY_CONT_H,
                                      ROOT_ID_INVALID, 0,
                                      OBJ_ATTR_ACTIVE, 0);
    }
	
    if (p_cont == NULL)
    {
      return ERR_FAILURE;
    }
    ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    ctrl_set_keymap(p_cont, num_play_cont_keymap);
    ctrl_set_proc(p_cont, num_play_cont_proc);

    // frm
    p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_NUM_FRM,
                              NUM_PLAY_FRM_X, NUM_PLAY_FRM_Y,
                              NUM_PLAY_FRM_W, NUM_PLAY_FRM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_NUM_PLAY_CONT,
                    RSI_NUM_PLAY_CONT, RSI_NUM_PLAY_CONT);

  }
  else
  {
    p_cont = fw_create_mainwin(ROOT_ID_NUM_PLAY,
                                    NUM_PLAY_CONT_PREVX, NUM_PLAY_CONT_PREVY,
                                    NUM_PLAY_CONT_PREVW, NUM_PLAY_CONT_PREVH,
                                    ROOT_ID_PROG_LIST, 0,
                                    OBJ_ATTR_ACTIVE, 0);
    if (p_cont == NULL)
    {
      return ERR_FAILURE;
    }
    ctrl_set_rstyle(p_cont, RSI_IGNORE,
                    RSI_IGNORE, RSI_IGNORE);
    ctrl_set_keymap(p_cont, num_play_cont_keymap);
    ctrl_set_proc(p_cont, num_play_cont_proc);

    // frm
    p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_NUM_FRM,
                              NUM_PLAY_FRM_PREVX, NUM_PLAY_FRM_PREVY,
                              NUM_PLAY_FRM_PREVW, NUM_PLAY_FRM_PREVH,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_NUM_PLAY_CONT, RSI_NUM_PLAY_CONT, RSI_NUM_PLAY_CONT);
  }
  
  //programe number
  for (i = 0; i < num_icon_count; i++)
  {
      ctrl_create_ctrl(CTRL_BMAP, num_icon_idc[i],
      NUM_PLAY_ICON_X + i * NUM_PLAY_ICON_W, 
      NUM_PLAY_ICON_Y, NUM_PLAY_ICON_W, 
      NUM_PLAY_ICON_H, p_ctrl, 0);
  }
  
  input_num(p_cont, (u8)(para1 - V_KEY_0), FALSE);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  /* create tmr for jump */
  fw_tmr_create(ROOT_ID_NUM_PLAY, MSG_SELECT, 3000, FALSE);

  return SUCCESS;
}


static RET_CODE on_input_num(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u8 num = (u8)(MSG_DATA_MASK & msg);
  fw_tmr_destroy(ROOT_ID_NUM_PLAY, MSG_SELECT);
  fw_tmr_create(ROOT_ID_NUM_PLAY, MSG_SELECT, 3000, FALSE);
  input_num(p_ctrl, num, TRUE);
  return SUCCESS;
}

static RET_CODE on_select(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  jump_to_prog(p_ctrl);

  return SUCCESS;
}


static RET_CODE on_exit_num_play(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  fw_tmr_destroy(ROOT_ID_NUM_PLAY, MSG_SELECT);
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(num_play_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
  ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
  ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
  ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
  ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
  ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
  ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
  ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
  ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
  ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(num_play_cont_keymap, ui_comm_root_keymap)


BEGIN_MSGPROC(num_play_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_NUMBER, on_input_num)
  ON_COMMAND(MSG_SELECT, on_select)
  ON_COMMAND(MSG_EXIT, on_exit_num_play)
END_MSGPROC(num_play_cont_proc, ui_comm_root_proc)


