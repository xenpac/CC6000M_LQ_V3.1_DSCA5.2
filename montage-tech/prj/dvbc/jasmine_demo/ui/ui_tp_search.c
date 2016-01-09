/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_tp_search.h"

enum tp_search_ctrl_id
{
  IDC_TITLE = 1,
  IDC_MODE,
  IDC_FTA_ONLY,
  IDC_CHANNEL,
  IDC_NETWORK,
  IDC_VPID,
  IDC_APID,
  IDC_PPID,
  IDC_BTN_YES,
  IDC_BTN_NO,
};

static u16 g_curn_tp_id;
static u16 g_curn_tp_view_id;
static u16 tp_total_select = 0;

u16 tp_search_btn_keymap(u16 key);

RET_CODE tp_search_btn_proc(control_t *p_btn, u16 msg, 
                            u32 para1, u32 para2);

u16 tp_search_cont_keymap(u16 key);
RET_CODE tp_search_cont_proc(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2);


RET_CODE tp_search_select_proc(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2);

RET_CODE tp_search_num_proc(control_t *p_ctrl, u16 msg,
                                      u32 para1, u32 para2);

static void tp_search_set_mode(control_t *p_cont, BOOL is_normal,
                               BOOL is_paint)
{
  control_t *p_normal, *p_pid, *p_mode, *p_yes, *p_no;
  u8 i;
 
    //set attr
    for (i = 0; i < (TP_SEARCH_ITEM_NUM); i++)
    {
      p_normal = ctrl_get_child_by_id(p_cont, (u8)(IDC_FTA_ONLY + i));
      p_pid = ctrl_get_child_by_id(p_cont, (u8)(IDC_VPID + i));

      ctrl_set_sts(p_normal, is_normal ? OBJ_STS_SHOW : OBJ_STS_HIDE);
      if (is_paint)
      {
        ctrl_paint_ctrl(p_normal, TRUE);
      }
      ctrl_set_sts(p_pid, is_normal ? OBJ_STS_HIDE : OBJ_STS_SHOW);
      if (is_paint)
      {
        ctrl_paint_ctrl(p_pid, TRUE);
      }
    }
	p_yes = ctrl_get_child_by_id(p_cont, IDC_BTN_YES);
	p_no = ctrl_get_child_by_id(p_cont, IDC_BTN_NO);
     if(tp_total_select <= 1)
    {
	    //set relation ship of p_mode
	    p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
	    ctrl_set_related_id(p_mode,
	                        0, IDC_BTN_NO,
	                        0, is_normal ? IDC_FTA_ONLY : IDC_VPID);
	    
	    ctrl_set_related_id(p_yes,
	                        IDC_BTN_NO,
	                        is_normal ? IDC_NETWORK : IDC_PPID,
	                        IDC_BTN_NO,
	                        IDC_MODE);

		    ctrl_set_related_id(p_no,
		                        IDC_BTN_YES,
		                        is_normal ? IDC_NETWORK : IDC_PPID,
		                        IDC_BTN_YES,
		                        IDC_MODE);
     	}
	else
	{
		ctrl_set_related_id(p_yes,
										IDC_BTN_NO, IDC_NETWORK, IDC_BTN_NO, IDC_FTA_ONLY);
		ctrl_set_related_id( p_no,
										IDC_BTN_YES, IDC_NETWORK, IDC_BTN_YES, IDC_FTA_ONLY);
	}
			
}


static BOOL create_search_param(void)
{
  control_t *p_ctrl;
  dvbs_tp_node_t tp;
  u8 type, is_normal_scan;
  u8 is_free_only, chan_type, enable_nit;
  u16 tp_total = 0, i = 0, tp_id = 0;
  u16 j = 0;
  
  // release common block for scan and restart after finish scan
  ui_epg_stop();
  // init
  ui_scan_param_init();

  if(tp_total_select <= 1)
{
  // get preset
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_TP_SEARCH, IDC_MODE);
  is_normal_scan = (u8)ui_comm_select_get_focus(p_ctrl);
}
  tp_total = db_dvbs_get_count(g_curn_tp_view_id);
    for(i = 0; i < tp_total; i++)
    {
      if(db_dvbs_get_mark_status(g_curn_tp_view_id, i, DB_DVBS_SEL_FLAG, 0))
      {
         j ++ ;
        tp_id = db_dvbs_get_id_by_view_pos(g_curn_tp_view_id, i);
        MT_ASSERT(tp_id != INVALIDID);
        
        if(db_dvbs_get_tp_by_id(tp_id, &tp) != DB_DVBS_OK)
        {
          MT_ASSERT(0);
        }

        if(!ui_scan_param_add_tp(&tp))
        {
          // space is full
          MT_ASSERT(0);
          break;
        }        
      }
    }

  if(j < 1)    
  {
    if (db_dvbs_get_tp_by_id(g_curn_tp_id, &tp) != DB_DVBS_OK)
    {
      MT_ASSERT(0);
      return FALSE;
    }
    if (!ui_scan_param_add_tp(&tp))
    {
      MT_ASSERT(0);
      return FALSE;
    }
  }

 if(j > 1)
  {
    type = USC_PRESET_SCAN;
  }
  else
  {
    // add rid
     if(is_normal_scan)
    {
      type = USC_TP_SCAN;
    }
    else
    {
      // add rid
      u16 vid, aid, pcrid;

      type = USC_PID_SCAN;

      p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_TP_SEARCH, IDC_VPID);
      vid = (u16)ui_comm_numedit_get_num(p_ctrl);

      p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_TP_SEARCH, IDC_APID);
      aid = (u16)ui_comm_numedit_get_num(p_ctrl);

      p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_TP_SEARCH, IDC_PPID);
      pcrid = (u16)ui_comm_numedit_get_num(p_ctrl);

      ui_scan_param_set_pid(vid, aid, pcrid);
     }
  }

  // other setting
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_TP_SEARCH, IDC_FTA_ONLY);
  is_free_only = (u8)ui_comm_select_get_focus(p_ctrl);

  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_TP_SEARCH, IDC_NETWORK);
  enable_nit = (u8)ui_comm_select_get_focus(p_ctrl);

  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_TP_SEARCH, IDC_CHANNEL);
  chan_type = (u8)ui_comm_select_get_focus(p_ctrl);

  // init scan param
  ui_scan_param_set_type(type, chan_type, is_free_only, (nit_scan_type_t)enable_nit);

  return TRUE;
}




RET_CODE open_tp_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_mode, *p_ctrl[TP_SEARCH_ITEM_NUM][2];
  control_t *p_btn[TP_SEARCH_BTN_CNT], *p_title;
  u16 i,tp_total = 0;
  u16 x, y;
  u8 j;
  u16 stxt[TP_SEARCH_ITEM_NUM][2] =
  {
    { IDS_FTA_ONLY,       IDS_V_PID         },
    { IDS_SCAN_CHANNEL,   IDS_A_PID         },
    { IDS_NETWORK_SEARCH, IDS_PCR_PID       }
  };

  u16 btn_txt[TP_SEARCH_BTN_CNT] =
  { IDS_SEARCH, IDS_EXIT };
  u8 opt_cnt[TP_SEARCH_ITEM_NUM] = { 2, 3, 2 };
  u16 opt_data[TP_SEARCH_ITEM_NUM][4] =
  {
    { IDS_NO,           IDS_YES             },
    { IDS_TV_AND_RADIO, IDS_TV, IDS_RADIO   },
    { IDS_NO,           IDS_YES             }
  };
  scan_param_t scan_param;

  g_curn_tp_id = (u16)para1;
  g_curn_tp_view_id = (u16)para2;

  tp_total = db_dvbs_get_count(g_curn_tp_view_id);
  //db_dvbs_get_tp_by_id(g_curn_tp_id, &tp);
  tp_total = db_dvbs_get_count(g_curn_tp_view_id);

  tp_total_select = 0;

  for(i = 0; i < tp_total; i++)
   {
     if(db_dvbs_get_mark_status(g_curn_tp_view_id, i, DB_DVBS_SEL_FLAG, 0))
      {
       tp_total_select ++ ;
      }
  }
  sys_status_get_scan_param(&scan_param);
  //stop monitor service
  {
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
  }

  scan_param.tp_scan_mode = 1;
  
#if 1//#ifndef SPT_SUPPORT
  p_cont = fw_create_mainwin(ROOT_ID_TP_SEARCH,
                                  TP_SEARCH_CONT_X, TP_SEARCH_CONT_Y,
                                  TP_SEARCH_CONT_W, TP_SEARCH_CONT_H,
                                  ROOT_ID_INVALID, 0, 
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_TP_SEARCH_FRM, RSI_TP_SEARCH_FRM,
                  RSI_TP_SEARCH_FRM);
  ctrl_set_keymap(p_cont, tp_search_cont_keymap);
  ctrl_set_proc(p_cont, tp_search_cont_proc);

  y = TP_SEARCH_ITEM_VGAP;
  //create title
  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE, TP_SEARCH_ITEM_X,
                             y, (TP_SEARCH_ITEM_LW + TP_SEARCH_ITEM_RW),
                             TP_SEARCH_ITEM_H, p_cont,
                             0);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_TP_SEARCH);

  y += (TP_SEARCH_ITEM_H + TP_SEARCH_ITEM_VGAP);

  //create scan mode item.
  if(tp_total_select > 1)
  {
    y -= 20;
  }
  p_mode = ui_comm_select_create(p_cont, (u8)(IDC_MODE),
                                   TP_SEARCH_ITEM_X, y, TP_SEARCH_ITEM_LW,
                                   TP_SEARCH_ITEM_RW);

  ui_comm_select_set_static_txt(p_mode, IDS_SCAN_MODE);
  ui_comm_ctrl_set_proc(p_mode, tp_search_select_proc);

  ui_comm_select_set_param(p_mode, TRUE,
                           CBOX_WORKMODE_STATIC, 2,
                           CBOX_ITEM_STRTYPE_STRID, NULL);
  ui_comm_select_set_content(p_mode, 0, IDS_PID);
  ui_comm_select_set_content(p_mode, 1, IDS_NORMAL);
  if(tp_total_select > 1)
  {
    ctrl_set_sts(p_mode,OBJ_STS_HIDE);
  }
  //ui_comm_ctrl_set_cont_rstyle(p_mode, RSI_ITEM_6_GRAY,
  //                               RSI_ITEM_5_HL, RSI_ITEM_6_GRAY);
  //ui_comm_ctrl_set_ctrl_rstyle(p_mode, RSI_ITEM_6_GRAY,
  //                               RSI_COMM_SELECT_HL, RSI_ITEM_6_GRAY);

  y += (TP_SEARCH_ITEM_H + TP_SEARCH_ITEM_VGAP);

  //other items
  for (i = 0; i < TP_SEARCH_ITEM_NUM; i++)
  {
    p_ctrl[i][0] = ui_comm_select_create(p_cont, (u8)(IDC_FTA_ONLY + i),
                                           TP_SEARCH_ITEM_X, y,
                                           TP_SEARCH_ITEM_LW,
                                           TP_SEARCH_ITEM_RW);

    ui_comm_select_set_static_txt(p_ctrl[i][0], stxt[i][0]);
    ui_comm_ctrl_set_proc(p_ctrl[i][0], tp_search_select_proc);
    ui_comm_select_set_param(p_ctrl[i][0], TRUE,
                               CBOX_WORKMODE_STATIC, opt_cnt[i],
                               CBOX_ITEM_STRTYPE_STRID, NULL);
    for (j = 0; j < opt_cnt[i]; j++)
    {
      ui_comm_select_set_content(p_ctrl[i][0], j, opt_data[i][j]);
    }
    //ui_comm_ctrl_set_cont_rstyle(p_ctrl[i][0], RSI_ITEM_6_GRAY,
    //                               RSI_ITEM_5_HL, RSI_ITEM_6_GRAY);
    //ui_comm_ctrl_set_ctrl_rstyle(p_ctrl[i][0], RSI_ITEM_6_GRAY,
    //                               RSI_COMM_SELECT_HL, RSI_ITEM_6_GRAY);


    p_ctrl[i][1] = ui_comm_numedit_create(p_cont, (u8)(IDC_VPID + i),
                                            TP_SEARCH_ITEM_X, y,
                                            TP_SEARCH_ITEM_LW,
                                            TP_SEARCH_ITEM_RW);

    ui_comm_numedit_set_static_txt(p_ctrl[i][1], stxt[i][1]);
    ui_comm_ctrl_set_proc(p_ctrl[i][1], tp_search_num_proc);
    ui_comm_numedit_set_param(p_ctrl[i][1], NBOX_NUMTYPE_DEC,
                                TP_SEARCH_MIN, TP_SEARCH_MAX,
                                TP_SEARCH_BLEN, (u8)(TP_SEARCH_BLEN - 1));
    ui_comm_numedit_set_decimal_places(p_ctrl[i][1],
                                         TP_SEARCH_PLACE);
    //ui_comm_ctrl_set_cont_rstyle(p_ctrl[i][1], RSI_ITEM_6_GRAY,
    //                               RSI_ITEM_5_HL, RSI_ITEM_6_GRAY);
    //ui_comm_ctrl_set_ctrl_rstyle(p_ctrl[i][1], RSI_ITEM_6_GRAY,
    //                               RSI_COMM_NUMEDIT_HL, RSI_ITEM_6_GRAY);

    y += (TP_SEARCH_ITEM_H + TP_SEARCH_ITEM_VGAP);
  }

  x = TP_SEARCH_BTN_X;
  for (i = 0; i < TP_SEARCH_BTN_CNT; i++)
  {
    p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_BTN_YES + i),
                                x, TP_SEARCH_BTN_Y, TP_SEARCH_BTN_W,
                                TP_SEARCH_BTN_H,
                                p_cont,
                                0);
    ctrl_set_rstyle(p_btn[i],
                    RSI_TP_SEARCH_BTN_SH,
                    RSI_TP_SEARCH_BTN_HL,
                    RSI_TP_SEARCH_BTN_SH);
    ctrl_set_keymap(p_btn[i], tp_search_btn_keymap);
    ctrl_set_proc(p_btn[i], tp_search_btn_proc);
    text_set_font_style(p_btn[i],
                        FSI_WHITE, FSI_BLACK, FSI_WHITE);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn[i], btn_txt[i]);

    x += TP_SEARCH_BTN_HGAP + TP_SEARCH_BTN_W;
  }

  ctrl_set_related_id(p_ctrl[0][0],
                      0, (tp_total_select > 1)? IDC_BTN_YES : IDC_MODE, 0, IDC_CHANNEL);     //fta
  ctrl_set_related_id(p_ctrl[1][0],
                      0, IDC_FTA_ONLY, 0, IDC_NETWORK); //channel
  ctrl_set_related_id(p_ctrl[2][0],
                      0, IDC_CHANNEL, 0, IDC_BTN_YES);  //network

  ctrl_set_related_id(p_ctrl[0][1],
                      0, IDC_MODE, 0, IDC_APID);        //v-pid
  ctrl_set_related_id(p_ctrl[1][1],
                      0, IDC_VPID, 0, IDC_PPID);        //a-pid
  ctrl_set_related_id(p_ctrl[2][1],
                      0, IDC_APID, 0, IDC_BTN_YES);     //p-pid
                      
  //set p_ctrl attr, and related id of p_yes and p_mode
  tp_search_set_mode(p_cont, TRUE, FALSE);
#else
#endif

  ui_comm_select_set_focus(p_mode, scan_param.tp_scan_mode);
  //0: pid scan, 1: normal scan
  tp_search_set_mode(p_cont, scan_param.tp_scan_mode, FALSE);


  ui_comm_select_set_focus(p_ctrl[0][0], scan_param.fta_only);
  ui_comm_select_set_focus(p_ctrl[1][0], scan_param.scan_channel);
  ui_comm_select_set_focus(p_ctrl[2][0], scan_param.network_search);

  ui_comm_numedit_set_num(p_ctrl[0][1], scan_param.vpid);
  ui_comm_numedit_set_num(p_ctrl[1][1], scan_param.apid);
  ui_comm_numedit_set_num(p_ctrl[2][1], scan_param.ppid);

  ctrl_default_proc(p_btn[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_btn_select(control_t *p_btn, u16 msg, 
                            u32 para1, u32 para2)
{
  RET_CODE ret = ERR_FAILURE;
  if (IDC_BTN_YES == ctrl_get_ctrl_id(p_btn)) //search
  {
    if (create_search_param())
    {
      ret = manage_open_menu(ROOT_ID_DO_SEARCH, 0, g_curn_tp_view_id);
    }
  }
  manage_close_menu(ROOT_ID_TP_SEARCH, 0, 0);
  return SUCCESS;
}


static RET_CODE on_tp_search_exit(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_TP_SEARCH, 0, 0);
  return SUCCESS;
}

static RET_CODE on_select_change_focus(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  control_t *p_parent;
  scan_param_t scan_param;
  u16 focus = 0;
  
  sys_status_get_scan_param(&scan_param);

  cbox_class_proc(p_ctrl, msg, para1, para2);

  p_parent = ctrl_get_parent(p_ctrl);

  focus = cbox_static_get_focus(p_ctrl);
  
  switch (ctrl_get_ctrl_id(p_parent))
  {
    case IDC_MODE:
      tp_search_set_mode(ctrl_get_parent(p_parent), 
        (BOOL)cbox_static_get_focus(p_ctrl), TRUE);
      
      scan_param.tp_scan_mode = (u8)focus;
      break;
    case IDC_FTA_ONLY:
      scan_param.fta_only = (u8)focus;
      break;
    case IDC_CHANNEL:
      scan_param.scan_channel = (u8)focus;
      break;
    case IDC_NETWORK:
      scan_param.network_search = (u8)focus;
      break;
    default:
      ;
  }

  sys_status_set_scan_param(&scan_param);
  sys_status_save();

  return SUCCESS;
}


static RET_CODE on_tp_search_pid_changed(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{

  control_t *p_parent;
  scan_param_t scan_param;
  u16 pid = (u16)para1;
  
  sys_status_get_scan_param(&scan_param);

  p_parent = ctrl_get_parent(p_ctrl);

  
  switch (ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_VPID:
      scan_param.vpid = pid;
      break;
    case IDC_APID:
      scan_param.apid = pid;
      break;
    case IDC_PPID:
      scan_param.ppid = pid;
      break;
    default:
      MT_ASSERT(0);
  }

  sys_status_set_scan_param(&scan_param);
  sys_status_save();

  return SUCCESS;
}



BEGIN_KEYMAP(tp_search_btn_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(tp_search_btn_keymap, NULL)

BEGIN_MSGPROC(tp_search_btn_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_btn_select)
END_MSGPROC(tp_search_btn_proc, text_class_proc)

BEGIN_KEYMAP(tp_search_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(tp_search_cont_keymap, NULL)

BEGIN_MSGPROC(tp_search_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_tp_search_exit)
END_MSGPROC(tp_search_cont_proc, cont_class_proc)

BEGIN_MSGPROC(tp_search_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_select_change_focus)
  ON_COMMAND(MSG_DECREASE, on_select_change_focus)
END_MSGPROC(tp_search_select_proc, cbox_class_proc)


BEGIN_MSGPROC(tp_search_num_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_CHANGED, on_tp_search_pid_changed)
END_MSGPROC(tp_search_num_proc, ui_comm_num_proc)



