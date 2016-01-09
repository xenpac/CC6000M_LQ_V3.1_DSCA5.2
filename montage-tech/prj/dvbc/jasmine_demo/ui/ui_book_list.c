/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#include "ui_common.h"

#include "ui_book_list.h"
#include "ui_timer.h"
/* others */
enum control_id
{
  IDC_INVALID = 0,
  IDC_BLIST_LCONT,
  IDC_BLIST_HEAD,
  IDC_BLIST_TITLE1,
  IDC_BLIST_TITLE2,
  IDC_BLIST_TITLE3,
  IDC_BLIST_TITLE4,
  IDC_BLIST_TITLE5,
  IDC_BLIST_TITLE6,
  IDC_BLIST_LIST,
  IDC_BLIST_SBAR,
};

enum book_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
};


static list_xstyle_t list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,//RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

static list_xstyle_t list_field_rstyle_left =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
  RSI_PBACK,
  RSI_LIST_FIELD_LEFT_HL,
};

static list_xstyle_t list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};


static list_xstyle_t list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t list_field_attr[BLIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_ICON | STL_CENTER | STL_VCENTER,
    60, 0, 0, &list_field_rstyle_left,  &list_field_fstyle},
  { LISTFIELD_TYPE_STRID | STL_CENTER | STL_VCENTER,
    60, 60, 0, &list_field_rstyle,  &list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    240, 120, 0, &list_field_rstyle,  &list_field_fstyle},
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    80, 360, 0, &list_field_rstyle,  &list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    120, 440, 0, &list_field_rstyle,  &list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    120, 560, 0, &list_field_rstyle,  &list_field_fstyle},
};
#if defined(CUSTOMER_JIESAI_WUNING) || defined(CUSTOMER_JIZHONG_ANXIN)
static comm_help_data_t2 book_list_help_data = //help bar data
{
  4, 280, {40, 250, 40, 250},
  {
    HELP_RSC_BMP   | IM_EXIT,
    HELP_RSC_STRID | IDS_EXIT,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_BOOK,
  },
};

#else
static comm_help_data_t2 book_list_help_data = //help bar data
{
  4, 280, {40, 250, 40, 250},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_RECORD,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_BOOK,
  },
};
#endif
comm_dlg_data_t book_exit_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_ASK | DLG_STR_MODE_STATIC,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_MSG_ASK_FOR_SAV,
  0,
};

//static BOOL g_is_modified;
static book_pg_t g_booknode[MAX_BOOK_PG];
static u8 g_bookCount = 0;

RET_CODE book_list_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 book_list_keymap(u16 key);
RET_CODE book_list_proc(control_t * p_list, u16 msg, u32 para1, u32 para2);

static RET_CODE book_list_update(control_t* ctrl, u16 start, u16 size, 
                                      u32 context)
{
  u16 i, pos;
  u16 cnt = list_get_count(ctrl);
  dvbs_prog_node_t pg;
  u8 asc_str[64];
  u16 uni_str[128];
  utc_time_t endtime = {0};
  utc_time_t start_time = {0};
  utc_time_t curn_time = {0};
  u8 weekday = 0;
  s8 time_cmp_result = 0;
  u16 str[] = 
    {
      IDS_MON,
      IDS_TUS,
      IDS_WED,
      IDS_THU,
      IDS_FRI,
      IDS_SAT,
      IDS_SUN,
    };

  time_get(&curn_time, FALSE);
  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      start_time = g_booknode[pos].start_time;
      time_cmp_result = time_cmp(&start_time, &curn_time, FALSE);

      //icon
      if((g_booknode[pos].book_mode != BOOK_TMR_OFF)
        &&(g_booknode[pos].pgid != 0) && (time_cmp_result >= 0))
      {
        list_set_field_content_by_icon(ctrl, pos, 0, IM_EPG_BOOK);
      }
      else
      {
        list_set_field_content_by_icon(ctrl, pos, 0, 0);
      }
      
      //record
      if(g_booknode[pos].record_enable)
      {
        list_set_field_content_by_strid(ctrl, pos, 1, IDS_YES);       
      }
      else
      {
        list_set_field_content_by_strid(ctrl, pos, 1, IDS_NO);       
      }
      
      //date time
      memcpy(&endtime, &(g_booknode[pos].start_time), sizeof(utc_time_t));
      time_add(&endtime, &(g_booknode[pos].drt_time));
      //timer type
      sprintf((char *)asc_str,"%.4d/%.2d/%.2d %.2d:%.2d-%.2d:%.2d",
                    g_booknode[pos].start_time.year,
                    g_booknode[pos].start_time.month,
                    g_booknode[pos].start_time.day,
                    g_booknode[pos].start_time.hour, 
                    g_booknode[pos].start_time.minute, 
                    endtime.hour, 
                    endtime.minute
                    );
      list_set_field_content_by_ascstr(ctrl, pos, 2, asc_str);

      //weekday
      weekday = date_to_weekday(&(g_booknode[pos].start_time));
      list_set_field_content_by_strid(ctrl, pos, 3, str[weekday]);
      
      //service name
      //if((g_booknode[pos].book_mode)
        //&& (g_booknode[pos].svc_type != SVC_TYPE_NVOD_REFRENCE))
      //{
        db_dvbs_get_pg_by_id(g_booknode[pos].pgid, &pg);
        ui_dbase_get_full_prog_name(&pg, uni_str, 31);
        list_set_field_content_by_unistr(ctrl, pos, 4, uni_str);
      //}    
      //else
    //  {
      //  list_set_field_content_by_ascstr(ctrl, pos, 3, (u8 *)" "); 
      //}
      
      //event name
      list_set_field_content_by_unistr(ctrl, pos, 5, g_booknode[pos].event_name);   
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_book_list_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  u16 index = list_get_focus_pos(p_list);
  
  if(index >= g_bookCount)
  {
    return ERR_FAILURE;
  }
  
  if(g_booknode[index].book_mode != BOOK_TMR_OFF)
  {
    g_booknode[index].book_mode = BOOK_TMR_OFF;
    list_set_field_content_by_icon(p_list, index, 0, 0);
  }
  else
  {
    g_booknode[index].book_mode = BOOK_TMR_ONCE;
    list_set_field_content_by_icon(p_list, index, 0, IM_EPG_BOOK);
  }

  list_draw_field_ext(p_list, index, 0, TRUE);
  return SUCCESS;

}

static RET_CODE on_book_list_record(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  u16 index = list_get_focus_pos(p_list);
  
  if(index >= g_bookCount)
  {
    return ERR_FAILURE;
  }
  
  if(g_booknode[index].record_enable)
  {
    g_booknode[index].record_enable = FALSE;
    list_set_field_content_by_strid(p_list, index, 1, IDS_NO);
  }
  else
  {
    g_booknode[index].record_enable = TRUE;
    list_set_field_content_by_strid(p_list, index, 1, IDS_YES);
  }

  list_draw_item_ext(p_list, index, TRUE);
  return SUCCESS;

}


static void book_do_save_all()
{
  book_pg_t temp_node = {0};
  u16 i = 0, index = 0;
  
  for (i = 0; i < g_bookCount; i++)
  {
    if ((g_booknode[i].pgid != 0))
    {
      memcpy(&temp_node, &(g_booknode[i]), sizeof(book_pg_t));
      temp_node.book_mode = BOOK_TMR_ONCE;
      index = book_get_match_node(&temp_node);
      if(g_booknode[i].book_mode == BOOK_TMR_OFF)
      {
        book_delete_node((u8)index);
      }
      else
      {
        sys_status_get_book_node((u8)index, &temp_node);
        if(g_booknode[i].record_enable != temp_node.record_enable)
        book_edit_node((u8)index, &g_booknode[i]);
      }
    }
  }
}

static int do_book_node_sort(const void* node1,const void* node2)
{
  s8 result;

  result = time_cmp(&((book_pg_t *)node1)->start_time, &((book_pg_t *)node2)->start_time, FALSE);

  return result;
}
static u8 book_get_all_booked_node(void)
{
  u8 index = 0;
  u8 i = 0;
  book_pg_t temp_node;

  memset(g_booknode, 0, sizeof(book_pg_t)*MAX_BOOK_PG);
  //book_sort_by_start_time();
  
  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    book_get_book_node(i, &temp_node);
    if ((temp_node.pgid != 0) && (temp_node.book_mode != BOOK_TMR_OFF))
    {
      memcpy(&(g_booknode[index]), &temp_node, sizeof(book_pg_t));
      index++;
    }
  }
  // sort book node by time
  qsort(g_booknode, index, sizeof(book_pg_t), do_book_node_sort);

  g_bookCount = index;

  return g_bookCount;
}

RET_CODE open_book_list(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_sbar,*p_list_cont, *p_ctrl;
  u16 i;
  list_field_attr_t *p_attr = list_field_attr;
  u8 book_count=0;
  
  /* pre-create */
  book_count = book_get_all_booked_node();
#if 1//#ifndef SPT_SUPPORT
#ifdef CUSTOMER_HCI
  p_cont =
    ui_comm_root_create(ROOT_ID_BOOK_LIST, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
                               COMM_BG_H, IM_TITLEICON_TV, IDS_BOOK_MANAGE_HK);
#else
  p_cont =
    ui_comm_root_create(ROOT_ID_BOOK_LIST, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
                               COMM_BG_H, IM_TITLEICON_TV, IDS_BOOK_MANAGE);
#endif

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, book_list_cont_proc);
  
  p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_BLIST_LCONT, BLIST_LCONT_X,
                     BLIST_LCONT_Y, BLIST_LCONT_W,
                     BLIST_LCONT_H, p_cont,
                     0);
  ctrl_set_rstyle(p_list_cont, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  
  //head
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_BLIST_HEAD,
                           BLIST_LIST_HEAD_X, BLIST_LIST_HEAD_Y, BLIST_LIST_HEAD_W,BLIST_LIST_HEAD_H, p_list_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  
  //title cont: item1
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_BLIST_TITLE1,
                           20, BLIST_LIST_HEAD_Y, 60,BLIST_LIST_HEAD_H, p_list_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type(p_ctrl , STL_LEFT|STL_VCENTER);
  text_set_font_style(p_ctrl , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl , IDS_BOOK);

  //title cont: item2
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_BLIST_TITLE2,
                           80, BLIST_LIST_HEAD_Y, 100,BLIST_LIST_HEAD_H, p_list_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type( p_ctrl , STL_LEFT|STL_VCENTER);
  text_set_font_style( p_ctrl , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type( p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid( p_ctrl , IDS_RECORD);  

  //title cont: item3
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_BLIST_TITLE3,
                           210, BLIST_LIST_HEAD_Y, 100,BLIST_LIST_HEAD_H, p_list_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type( p_ctrl , STL_LEFT|STL_VCENTER);
  text_set_font_style( p_ctrl , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type( p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid( p_ctrl , IDS_DATE_TIME);

  //title cont: item4
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_BLIST_TITLE4,
                           370, BLIST_LIST_HEAD_Y, 70,BLIST_LIST_HEAD_H, p_list_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type(p_ctrl , STL_LEFT|STL_VCENTER);
  text_set_font_style(p_ctrl , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_WEEK);

  //title cont: item5
  p_ctrl= ctrl_create_ctrl(CTRL_TEXT, IDC_BLIST_TITLE5,
                           440, BLIST_LIST_HEAD_Y, 100,BLIST_LIST_HEAD_H, p_list_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type(p_ctrl , STL_LEFT|STL_VCENTER);
  text_set_font_style(p_ctrl , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl , IDS_CHANNEL);

  //title cont: item6
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_BLIST_TITLE6,
                           560, BLIST_LIST_HEAD_Y, 100,BLIST_LIST_HEAD_H, p_list_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_EMAIL_RECT, RSI_EMAIL_RECT, RSI_EMAIL_RECT);
  text_set_align_type( p_ctrl , STL_LEFT|STL_VCENTER);
  text_set_font_style(p_ctrl , FSI_WHITE,FSI_WHITE,FSI_WHITE);
  text_set_content_type( p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid( p_ctrl , IDS_PROGRAM);
  //create program list
  p_list =
    ctrl_create_ctrl(CTRL_LIST, IDC_BLIST_LIST,
                     BLIST_LIST_X, BLIST_LIST_Y,
                     BLIST_LIST_W, BLIST_LIST_H,
                     p_list_cont, 0);
  ctrl_set_rstyle(p_list, RSI_BLIST_LIST,
                  RSI_BLIST_LIST, RSI_BLIST_LIST);
  ctrl_set_keymap(p_list, book_list_keymap);
  ctrl_set_proc(p_list, book_list_proc);
  ctrl_set_mrect(p_list,
    BLIST_MID_X, BLIST_MID_Y, (BLIST_MID_X + BLIST_MID_W), (BLIST_MID_Y + BLIST_MID_H));
  list_set_item_interval(p_list, BLIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &list_item_rstyle);
  list_set_count(p_list, book_count, BLIST_PAGE);
  list_set_field_count(p_list, BLIST_FIELD_NUM, BLIST_PAGE);
  list_set_update(p_list, book_list_update, 0);

  for (i = 0; i < BLIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, p_attr->attr,
                        p_attr->width, p_attr->left, p_attr->top);
    list_set_field_rect_style(p_list, (u8)i, p_attr->rstyle);
    list_set_field_font_style(p_list, (u8)i, p_attr->fstyle);

    p_attr++;
  }

  //create scroll bar
  p_sbar = ctrl_create_ctrl(
    CTRL_SBAR, IDC_BLIST_SBAR,
    BLIST_BAR_X,
    BLIST_BAR_Y,
    BLIST_BAR_W, BLIST_BAR_H,
    p_list_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_BLIST_SBAR_BG,
                  RSI_BLIST_SBAR_BG, RSI_BLIST_SBAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_BLIST_SBAR_MID,
                     RSI_BLIST_SBAR_MID, RSI_BLIST_SBAR_MID);
  /*ctrl_set_mrect(p_sbar, 0, BLIST_SBAR_VERTEX_GAP,
                    BLIST_SBAR_WIDTH,
                    BLIST_SBAR_HEIGHT - BLIST_SBAR_VERTEX_GAP);*/
  list_set_scrollbar(p_list, p_sbar);

#else
#endif
  list_set_focus_pos(p_list, 0);
  book_list_update(p_list, list_get_valid_pos(p_list), BLIST_PAGE, 0);

  ui_comm_help_create2(&book_list_help_data, p_cont,FALSE);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_book_list_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u8 i = 0;
  BOOL bModify = FALSE;
  book_pg_t temp_node = {0};
  u16 index = 0;
  dlg_ret_t dlg_ret;
  
  for(i=0; i<g_bookCount; i++)
  {
    memcpy(&temp_node, &(g_booknode[i]), sizeof(book_pg_t));
    temp_node.book_mode = BOOK_TMR_ONCE;
    index = book_get_match_node(&temp_node);
    sys_status_get_book_node((u8)index, &temp_node );
    if((g_booknode[i].pgid != 0) && 
      ((g_booknode[i].book_mode == BOOK_TMR_OFF) || ((g_booknode[i].record_enable != temp_node.record_enable))))
    {
      bModify = TRUE;
    }
  }

  if(bModify)
  {
    dlg_ret = ui_comm_dlg_open(&book_exit_data);

    if(dlg_ret == DLG_RET_YES)
    {
      book_do_save_all();
      if(fw_find_root_by_id(ROOT_ID_EPG) != NULL)
      {
      	fw_notify_root(fw_find_root_by_id(ROOT_ID_EPG), NOTIFY_T_MSG, FALSE, MSG_BOOK_UPDATE, 0, 0);
      }
    }
    else
    {
      //book_undo_save_all();
    }
  }
  return ERR_NOFEATURE;
}

BEGIN_MSGPROC(book_list_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_book_list_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_book_list_exit)
END_MSGPROC(book_list_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(book_list_keymap, NULL)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_GREEN, MSG_SELECT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  #ifdef CUSTOMER_YINHE_TR
   ON_EVENT(V_KEY_BLUE, MSG_SELECT)
  #endif
END_KEYMAP(book_list_keymap, NULL)

BEGIN_MSGPROC(book_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_book_list_select)
  ON_COMMAND(MSG_RED, on_book_list_record)
END_MSGPROC(book_list_proc, list_class_proc)


