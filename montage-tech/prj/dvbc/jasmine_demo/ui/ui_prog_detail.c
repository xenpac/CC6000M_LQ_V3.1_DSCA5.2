/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"

#include "ui_prog_detail.h"

#include "epg_util.h"

#include "ui_signal.h"
#include "ui_epg_api.h"

static event_node_t *p_evt_node;
static u16 *p_ext_detail = NULL;
static u16 pgid;

enum local_msg
{
  MSG_CLOSE= MSG_LOCAL_BEGIN + 300,
};

enum prog_detail_ctrl_id
{
  IDC_PDETAIL_TTL = 1,
  IDC_PDETAIL_DTL,
  IDC_PDETAIL_SBAR,
  IDC_PDETAIL_HELP,
};

enum prog_detail_title_ctrl_id
{
  IDC_PDETAIL_SNAME = 1,
  IDC_PDETAIL_ENAME,
  IDC_PDETAIL_ETIME,
  IDC_PDETAIL_ELANG,
  IDC_PDETAIL_ETYPE,
};

u16 prog_detail_cont_keymap(u16 key);
RET_CODE prog_detail_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 prog_detail_keymap(u16 key);

extern BOOL have_logic_number(void);

static RET_CODE on_prog_detail_close(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  mtos_free(p_ext_detail);
  manage_close_menu(ROOT_ID_PROG_DETAIL, 0, 0) ;
  return   manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
}

static RET_CODE on_prog_detail_shift_pg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  on_prog_detail_close(p_cont, msg, para1, para2);
  switch(msg)
  {
    case MSG_FOCUS_UP:
      manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_UP, 0);
      break;
    case MSG_FOCUS_DOWN:
      manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_DOWN, 0);
      break;
    case MSG_PAGE_UP:
      manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_PAGE_UP, 0);
      break;
    case MSG_PAGE_DOWN:
      manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_PAGE_DOWN, 0);
      break;
    default:
      break;
  }
  return SUCCESS;
}

RET_CODE open_prog_detail(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[PDETAIL_ITEM_NUM];
  control_t *p_title, *p_detail;
  u8 i, j, asc_str[32];
  u16 x, y, w, view_pos;
  u16 uni_str1[32], uni_str2[32];
  utc_time_t startime;
  utc_time_t endtime;
  dvbs_prog_node_t pg;
  ext_desc_t *p_evt_desc = NULL; 
  
  //special code, to close signal first.
  close_signal();
  
  p_evt_node = (event_node_t *)para1;
  pgid = (u16)para2;
  
  p_ext_detail = (u16 *)mtos_malloc(MAX_EXT_TXT_BUF_LEN);
  MT_ASSERT(p_ext_detail != NULL);

  memset(p_ext_detail, 0, MAX_EXT_TXT_BUF_LEN);

  p_cont = fw_create_mainwin(ROOT_ID_PROG_DETAIL,
   PDETAIL_CONT_X,PDETAIL_CONT_Y,PDETAIL_CONT_W,PDETAIL_CONT_H,
    ROOT_ID_PROG_BAR, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    OS_PRINTF("ui: open_epg_detail %d\n", __LINE__);
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_PDETAIL_FRM, RSI_PDETAIL_FRM, RSI_PDETAIL_FRM);
  ctrl_set_keymap(p_cont, prog_detail_cont_keymap);
  ctrl_set_proc(p_cont, prog_detail_cont_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_PDETAIL_TTL,
   0,0,PDETAIL_CONT_W,PDETAIL_CONT_H,// 0, 0, PDETAIL_CONT_W+500, PDETAIL_CONT_H-130,
   						p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_PDETAIL_TTL, RSI_PDETAIL_TTL, RSI_PDETAIL_TTL);
  text_set_font_style(p_title, FSI_PROG_DETAIL_TXT, FSI_PROG_DETAIL_TXT, FSI_PROG_DETAIL_TXT);
  
  x = PDETAIL_ITEM_X;
  y = PDETAIL_ITEM_Y;
  w = PDETAIL_ITEM_W;
  for (i = 0; i < PDETAIL_ITEM_NUM; i++)
  {
    p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_PDETAIL_SNAME + i), 
      x, y, w+100, PDETAIL_ITEM_H, p_title, 0);
    text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_ctrl[i], FSI_PROG_DETAIL_TXT, FSI_PROG_DETAIL_TXT, FSI_PROG_DETAIL_TXT);
        
   switch (i)
    {
      case 0://sevice name
        db_dvbs_get_pg_by_id(pgid, &pg);
        view_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), pgid);
        if(have_logic_number())
          sprintf((char *)asc_str, "%.4d", pg.logical_num);
        else
          sprintf((char *)asc_str, "%.4d", view_pos + 1);

        //sprintf(asc_str, "%.4d  ", view_pos + 1);
        str_nasc2uni(asc_str, uni_str1, 31);
        ui_dbase_get_full_prog_name(&pg, uni_str2, 31);
        uni_strcat(uni_str1, uni_str2, 31);
        text_set_content_by_unistr(p_ctrl[i], uni_str1);
        
        y += PDETAIL_ITEM_H + PDETAIL_V_GAP;        
        break;
      case 1://event name
        text_set_content_by_unistr(p_ctrl[i], p_evt_node->event_name); 
        
        y += PDETAIL_ITEM_H + PDETAIL_V_GAP;
        w = PDETAIL_ITEM_LW;
        break;
      case 2://event time
        time_to_local(&p_evt_node->start_time, &startime);

        //endtime.value = startime.value;
        memcpy(&endtime, &startime, sizeof(utc_time_t));
        time_add(&endtime, &(p_evt_node->drt_time));

        sprintf((char *)asc_str, "%.2d:%.2d-%.2d:%.2d", 
          startime.hour, startime.minute, endtime.hour, endtime.minute);      
        text_set_content_by_ascstr(p_ctrl[i], asc_str); 
        
        x = PDETAIL_ITEM_X + PDETAIL_ITEM_LW;
        w = PDETAIL_ITEM_RW;
        break;
      case 3://event language code
        text_set_content_by_unistr(p_ctrl[i], p_evt_node->lang_code);

        x = PDETAIL_ITEM_X;
        w = PDETAIL_ITEM_W; 
        y += PDETAIL_ITEM_H + PDETAIL_V_GAP;  
        break;
      case 4://event type
        //tbd
        text_set_content_by_ascstr(p_ctrl[i], epg_get_type(p_evt_node->cont_level));
        y += PDETAIL_ITEM_H + PDETAIL_V_GAP;
        break;
      default:
        MT_ASSERT(0);
        break;
    }

  }
  p_detail = ctrl_create_ctrl(CTRL_TEXT, IDC_PDETAIL_DTL,
   PDETAIL_DETAIL_X + 5,PDETAIL_DETAIL_Y,PDETAIL_DETAIL_W - 5,PDETAIL_DETAIL_H,
    p_cont, 0);
  ctrl_set_keymap(p_detail, prog_detail_keymap);
  ctrl_set_rstyle(p_detail, RSI_PDETAIL_DTL, RSI_PDETAIL_DTL, RSI_PDETAIL_DTL);
  text_set_font_style(p_detail, FSI_PROG_DETAIL_TXT, FSI_PROG_DETAIL_TXT, FSI_PROG_DETAIL_TXT);
  text_set_content_type(p_detail, TEXT_STRTYPE_EXTSTR);
  text_set_align_type(p_detail, STL_LEFT | STL_TOP);
  text_set_offset(p_detail, 0, 0);
  ctrl_set_mrect(p_detail, PDETAIL_DETAIL_MIDX + 5, PDETAIL_DETAIL_MIDY,
  880,152 );
  text_enable_page(p_detail, TRUE);
  text_set_line_gap(p_detail, 6);

  //short txt
  str_asc2uni((u8 *)"\n", uni_str1);
  str_asc2uni((u8 *)"...", uni_str2);

  if(p_evt_node->p_sht_text != NULL)
  {
    UI_PRINTF("short txt\n");
    uni_strncpy(p_ext_detail, p_evt_node->p_sht_text, PDETAIL_MAX_LENTH);
    uni_strcat(p_ext_detail, uni_str1, PDETAIL_MAX_LENTH);
  }

  //extent txt
  for(i = 0; i < p_evt_node->count_ext_desc; i++)
  {
    p_evt_desc = &p_evt_node->p_ext_desc_list[i];  
  	for(j = 0; j < p_evt_desc->item_num; j++)
  	{
      uni_strcat(p_ext_detail, p_evt_desc->item[j].item_desc, MAX_ITEM_CHAR_LENGTH);
      uni_strcat(p_ext_detail, uni_str2, PDETAIL_MAX_LENTH);        
      uni_strcat(p_ext_detail, p_evt_desc->item[j].item_char, MAX_ITEM_CHAR_LENGTH);
      uni_strcat(p_ext_detail, uni_str1, PDETAIL_MAX_LENTH);
  	}

  
  UI_PRINTF("ui: open_epg_detail %d\n", __LINE__);
  //extent txt
    if(p_evt_desc->p_ext_text != NULL)
  {
    UI_PRINTF("extent txt\n");
      uni_strcat(p_ext_detail, p_evt_desc->p_ext_text, p_evt_desc->ext_text_len);
  }
  }
  text_set_content_by_extstr(p_detail, p_ext_detail);

  ctrl_default_proc(p_detail, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(prog_detail_cont_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_CLOSE)
  ON_EVENT(V_KEY_CANCEL, MSG_CLOSE)
  ON_EVENT(V_KEY_EXIT, MSG_CLOSE)
  ON_EVENT(V_KEY_BACK, MSG_CLOSE)
  ON_EVENT(V_KEY_MENU, MSG_CLOSE)
  ON_EVENT(V_KEY_INFO, MSG_CLOSE)
  ON_EVENT(V_KEY_EPG, MSG_CLOSE)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(prog_detail_cont_keymap, NULL)

BEGIN_MSGPROC(prog_detail_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CLOSE, on_prog_detail_close)
  ON_COMMAND(MSG_FOCUS_UP, on_prog_detail_shift_pg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_prog_detail_shift_pg)
  ON_COMMAND(MSG_PAGE_UP, on_prog_detail_shift_pg)
  ON_COMMAND(MSG_PAGE_DOWN, on_prog_detail_shift_pg)
END_MSGPROC(prog_detail_cont_proc, cont_class_proc)

BEGIN_KEYMAP(prog_detail_keymap, NULL)
  //ON_EVENT(V_KEY_UP, MSG_INCREASE)
  //ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
END_KEYMAP(prog_detail_keymap, NULL)
