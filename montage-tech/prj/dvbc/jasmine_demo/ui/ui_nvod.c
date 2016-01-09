/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_notify.h"

#include "ui_mainmenu.h"
#include "ui_nvod.h"
#include "ui_nvod_api.h"


enum nvod_control_id
{
  IDC_INVALID = 0,
  IDC_TV_WIN,
  IDC_PROG_INFO_CONT,
  IDC_PROG_INFO,
  IDC_REF_SVC_LIST,
  IDC_REF_EVENT_LIST_CONT,
  IDC_REF_EVENT_LIST,
  IDC_REF_EVENT_LIST_SBAR,
  IDC_SHIFT_EVENT_LIST_CONT,
  IDC_SHIFT_EVENT_LIST,
  IDC_SHIFT_EVENT_LIST_SBAR,
  IDC_NVOD_PROGRESS_PBAR,

};

static list_xstyle_t nvod_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
};

static list_xstyle_t nvod_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t nvod_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_xstyle_t nvod_list_field_rstyle_mid =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_LIST_FIELD_MID_HL,
  RSI_IGNORE,
  RSI_LIST_FIELD_MID_HL,
};

static list_field_attr_t ref_event_list_attr[REF_EVENT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    50, 20, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    230, 70, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
};

static list_field_attr_t shift_event_list_attr[SHIFT_EVENT_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    160, 20, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    190, 180, 0, &nvod_list_field_rstyle,  &nvod_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    40, 370, 0, &nvod_list_field_rstyle_mid,  &nvod_list_field_fstyle},
};

static comm_help_data_t2 help_data[] = //help bar data
{
   {
    2, 380, {40, 250},
    {
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_SWITCH_WINDOW,
    },
  },
  {
    4, 160, {40, 250, 40, 250},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_PLAY,
      HELP_RSC_BMP   | IM_CHANGE,
      HELP_RSC_STRID | IDS_SWITCH_WINDOW,
    },
  },
};

static book_pg_t book_node = {0};

static  BOOL is_shift_evt_found = FALSE;

u16 nvod_cont_keymap(u16 key);
RET_CODE nvod_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ref_svc_list_keymap(u16 key);
RET_CODE ref_svc_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 ref_event_keymap(u16 key);
RET_CODE ref_event_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 shift_event_list_keymap(u16 key);
RET_CODE shift_event_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


//u16 nvod_list_keymap(u16 key);
//RET_CODE nvod_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE nvod_play_by_shift_event_index(control_t *p_cont);
static RET_CODE refresh_play_progress(control_t *p_cont, BOOL b_show);


void ui_set_shift_event_list_focus(u16 idx)
{ 
  u16 index;
  control_t *p_cont;
  control_t *p_list_cont = ui_comm_root_get_ctrl(ROOT_ID_NVOD, IDC_SHIFT_EVENT_LIST_CONT);
  control_t *p_list = ctrl_get_child_by_id(p_list_cont , IDC_SHIFT_EVENT_LIST);
  p_cont = ctrl_get_parent(p_list_cont);
  index = list_get_focus_pos(p_list);
  if(index != idx)
  {
    list_set_focus_pos(p_list, idx);
    list_class_proc(p_list, MSG_SELECT, 0, 0);
    refresh_play_progress(p_cont, TRUE);
    ctrl_paint_ctrl(p_list, TRUE);
  }
}

static RET_CODE refresh_refrence_event_info(control_t* p_cont, BOOL is_paint)
{
  u32 i = 0;
  control_t * p_info_cont = ctrl_get_child_by_id(p_cont, IDC_PROG_INFO_CONT);
  control_t* p_info = ctrl_get_child_by_id(p_info_cont, IDC_PROG_INFO);
  u32 ref_event_count = 0;
  control_t *p_ref_event_cont = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST_CONT);
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_ref_event_cont, IDC_REF_EVENT_LIST); 
  //control_t* p_ref_svc_list = ctrl_get_child_by_id(p_cont, IDC_REF_SVC_LIST);
  //u16 ref_svc_index = 0;
  u16 ref_event_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  //nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;

  if(nvod_data_get_ref_svc_cnt(h_nvod) == 0)
  {
    return SUCCESS;
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);

  p_ref_event = nvod_data_get_ref_evt(h_nvod, NULL, &ref_event_count);

  if(p_ref_event != NULL)
  {
    for (i = 0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }

  if(p_ref_event != NULL)
  {
    text_set_content_by_unistr(p_info, p_ref_event->p_sht_txt);
  }
  else
  {
    text_set_content_by_unistr(p_info, (u16*)"");
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_info_cont, TRUE);
  }
  return SUCCESS;
}


static RET_CODE ref_event_list_update(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
  u16 cnt = list_get_count(p_ctrl);
  u8 asc_str[8];

  u32 ref_event_count = 0;
  u32 i = 0;
  //control_t* p_ref_svc_list = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_REF_SVC_LIST);
  //u16 ref_svc_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  //nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event_list = NULL;

  if(nvod_data_get_ref_svc_cnt(h_nvod) == 0)
  {
    return SUCCESS;
  }

  p_ref_event_list = nvod_data_get_ref_evt(h_nvod, NULL, &ref_event_count);

  if(p_ref_event_list != NULL)
  {
    for (i = 0; i<ref_event_count; i++)
    {
      if((p_ref_event_list == NULL) || (i == start))
      {
        break;
      }
      
      p_ref_event_list = nvod_data_get_ref_evt_next(h_nvod, p_ref_event_list);
    }
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char*)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */
      if(p_ref_event_list != NULL)
      {
        list_set_field_content_by_unistr(p_ctrl, (u16)(start + i), 1, p_ref_event_list->event_name);
        
        p_ref_event_list = nvod_data_get_ref_evt_next(h_nvod, p_ref_event_list);
      }
      else
      {
        break;
      }
    }
  }

  return SUCCESS;
}

/*static void shift_event_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 im_value;
  u8 view_id = ui_dbase_get_pg_view_id();

  im_value = ui_dbase_pg_is_fav(view_id, pos) ? IM_LOVE: 0;
  list_set_field_content_by_icon(p_list, pos, 2, im_value);
}*/

static RET_CODE shift_event_list_update(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
  u16 cnt = list_get_count(p_ctrl);
  u8 asc_str[64];
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};
  u32 i = 0;
  u32 shift_event_count = 0;
  u32 ref_event_count = 0;
  control_t *p_cont = ctrl_get_parent(p_ctrl->p_parent);
  control_t *p_ref_event_cont = ctrl_get_child_by_id(p_cont, IDC_REF_EVENT_LIST_CONT);
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_ref_event_cont, IDC_REF_EVENT_LIST); 
  //control_t* p_ref_svc_list = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_REF_SVC_LIST);
  //u16 ref_svc_index = 0;
  u16 ref_event_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  //nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;
  time_shifted_svc_evt_t *p_shift_event = NULL;
  time_shifted_svc_t *p_ts_svc = NULL;
  book_pg_t temp_node = {0};

  if(nvod_data_get_ref_svc_cnt(h_nvod) == 0)
  {
    return SUCCESS;
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);

  p_ref_event = nvod_data_get_ref_evt(h_nvod, NULL, &ref_event_count);

  if(p_ref_event != NULL)
  {
    for (i = 0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }

  if(p_ref_event != NULL)
  {
    p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }

  if(p_shift_event != NULL)
  {
    for (i = 0; i<start; i++)
    {
      if(p_shift_event == NULL)
      {
        break;
      }
      
      p_shift_event = nvod_data_get_shift_evt_next(h_nvod, p_shift_event);
    }
  }
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      if(p_shift_event != NULL)
      {
        memset(asc_str, 0, sizeof(asc_str));
        time_to_local(&(p_shift_event->start_time), &start_time);
        memcpy(&end_time, &start_time, sizeof(utc_time_t));
        time_add(&end_time, &p_shift_event->drt_time);
        sprintf((char*)asc_str, "%02d:%02d-%02d:%02d",start_time.hour,start_time.minute,end_time.hour,end_time.minute);

        memset(&temp_node, 0, sizeof(book_pg_t));
        p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);
        temp_node.pgid = ui_get_timeshift_svc_pgid(p_ts_svc);
        memcpy(&(temp_node.start_time), &start_time, sizeof(utc_time_t));
        memcpy(&(temp_node.drt_time), &(p_shift_event->drt_time), sizeof(utc_time_t));
        temp_node.book_mode = BOOK_TMR_ONCE;
                
        list_set_field_content_by_ascstr(p_ctrl, (u16)(start + i), 0, asc_str);

        if(p_shift_event->evt_status)
        {
          list_set_field_content_by_strid(p_ctrl, (u16)(start + i), 1, IDS_NVOD_IS_PLAYING);
        }
        else
        {
          list_set_field_content_by_strid(p_ctrl, (u16)(start + i), 1, IDS_NVOD_NOT_PLAYING);
        }

        if(book_get_match_node(&temp_node) < MAX_BOOK_PG)
        {
          list_set_field_content_by_icon(p_ctrl, (u16)(start + i), 2, IM_EPG_BOOK);
        }
        else
        {
          list_set_field_content_by_icon(p_ctrl, (u16)(start + i), 2, 0);
        }
        
        p_shift_event = nvod_data_get_shift_evt_next(h_nvod, p_shift_event);
      }
      else
      {
        break;
      }
    }
  }
 
  return SUCCESS;
}


static RET_CODE refresh_shift_event_list(control_t* p_cont, BOOL is_paint)
{
  u32 shift_event_count = 0;
  u32 ref_event_count = 0;
  u16 cur_focus = 0;
  u32 i = 0;
  control_t *p_shift_event_cont = ctrl_get_child_by_id(p_cont,IDC_SHIFT_EVENT_LIST_CONT);
  control_t* p_shift_event_list = ctrl_get_child_by_id(p_shift_event_cont, IDC_SHIFT_EVENT_LIST);
  control_t *p_ref_event_cont = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST_CONT);
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_ref_event_cont, IDC_REF_EVENT_LIST);
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  //nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;
  time_shifted_svc_evt_t *p_shift_event = NULL;
  //u16 ref_svc_index = 0;
  u16 ref_event_index = 0;

  if(nvod_data_get_ref_svc_cnt(h_nvod) == 0)
  {
    return SUCCESS;
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);
  p_ref_event = nvod_data_get_ref_evt(h_nvod, NULL, &ref_event_count);

  if(p_ref_event != NULL)
  {
    for(i=0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }
  
  if(p_ref_event != NULL)
  {
    p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }

  if(p_shift_event == NULL)
  {
    shift_event_count = 0;
  }

  cur_focus = list_get_focus_pos(p_shift_event_list);
  cur_focus = (cur_focus == LIST_INVALID_FOCUS)?0:cur_focus;
  cur_focus = (cur_focus<shift_event_count)?cur_focus:0;
  
  list_set_count(p_shift_event_list, (u16)shift_event_count, NVOD_LIST_SHIFT_PAGE);
  list_set_focus_pos(p_shift_event_list, cur_focus);
  list_select_item(p_shift_event_list, cur_focus);
  shift_event_list_update(p_shift_event_list, list_get_valid_pos(p_shift_event_list), NVOD_LIST_SHIFT_PAGE, 0);

  if((is_paint)&&(ROOT_ID_NVOD == fw_get_focus_id()))
  {
    ctrl_paint_ctrl(p_shift_event_list, TRUE);
  }
  
  return (s32)shift_event_count;
}

static RET_CODE refresh_refrence_event_list(control_t* p_cont, BOOL is_paint)
{
  u32 ref_event_count = 0;
  u16 cur_focus = 0;
  control_t *p_ref_event_cont = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST_CONT);
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_ref_event_cont, IDC_REF_EVENT_LIST);
  //u16 ref_svc_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  //nvod_reference_svc_t *p_ref_svc = NULL;

  if(nvod_data_get_ref_svc_cnt(h_nvod) == 0)
  {
    return SUCCESS;
  }

  nvod_data_get_ref_evt(h_nvod, NULL, &ref_event_count);
  
  cur_focus = list_get_focus_pos(p_ref_event_list);
  cur_focus = (cur_focus == LIST_INVALID_FOCUS)?0:cur_focus;
  cur_focus = (cur_focus<ref_event_count)?cur_focus:0;
  
  list_set_count(p_ref_event_list, (u16)ref_event_count, NVOD_LIST_PAGE);
  list_set_focus_pos(p_ref_event_list, cur_focus);
  list_select_item(p_ref_event_list, cur_focus);
  ref_event_list_update(p_ref_event_list, list_get_valid_pos(p_ref_event_list), NVOD_LIST_PAGE, 0);

  refresh_refrence_event_info(p_cont, is_paint);

  refresh_shift_event_list(p_cont, is_paint);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_ref_event_list, TRUE);
  }
  
  return SUCCESS;
}


RET_CODE open_nvod(u32 para1, u32 para2)
{
  control_t *p_cont, *p_info, *p_bar, *p_info_cont, *p_list_cont, *p_ref_list_sbar, *p_sht_list_sbar, *p_sht_list_cont = NULL;
  //control_t *p_tv_win;
  control_t *p_ref_event_list, *p_shift_event_list;
  u16 unistr[32] = {0};
  u16 i;

  is_shift_evt_found = FALSE;
  ui_nvod_enter();
  ui_set_playing_nvod_reference_svc_evt(NULL);
  ui_set_playing_nvod_time_shifted_svc_evt(NULL);

  //create container
  p_cont = ui_comm_root_create(ROOT_ID_NVOD, 0,
                                NVOD_CONT_X, NVOD_CONT_Y,
                               NVOD_CONT_W, NVOD_CONT_H,
                               IM_TITLEICON_TV,IDS_NVOD_NVOD);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, nvod_cont_keymap);
  ctrl_set_proc(p_cont, nvod_cont_proc);
#if 0
  //TV preview window
  p_tv_win = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_TV_WIN,
                              NVOD_PREV_X, NVOD_PREV_Y,
                              NVOD_PREV_W, NVOD_PREV_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_tv_win, RSI_TV, RSI_TV, RSI_TV);
#endif
  //info
  p_info_cont = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_PROG_INFO_CONT,
                              NVOD_INFO_X, NVOD_INFO_Y,
                              NVOD_INFO_W, NVOD_INFO_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_info_cont, RSI_NVOD_INFO_CONT, RSI_NVOD_INFO_CONT, RSI_NVOD_INFO_CONT);
  
  p_info = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_PROG_INFO,
                              5,  5,
                              NVOD_INFO_W - 5, NVOD_INFO_H - 5,
                              p_info_cont, 0);
  ctrl_set_rstyle(p_info, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_info, 10, 10, NVOD_INFO_W-20, NVOD_INFO_H-20);
  text_set_align_type(p_info, STL_LEFT | STL_TOP);
  text_set_font_style(p_info, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_info, TEXT_STRTYPE_UNICODE);
  
  //reference event list cont
  p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_REF_EVENT_LIST_CONT, REF_EVENT_LIST_CONT_X, REF_EVENT_LIST_CONT_Y,
              REF_EVENT_LIST_CONT_W, REF_EVENT_LIST_CONT_H, p_cont,
              0);
  ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  //reference event list
  p_ref_event_list =ctrl_create_ctrl(CTRL_LIST, IDC_REF_EVENT_LIST, REF_EVENT_LIST_X, REF_EVENT_LIST_Y,
              REF_EVENT_LIST_W, REF_EVENT_LIST_H, p_list_cont,
              0);
  ctrl_set_rstyle(p_ref_event_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_ref_event_list, ref_event_keymap);
  ctrl_set_proc(p_ref_event_list, ref_event_proc);
  ctrl_set_mrect(p_ref_event_list, REF_EVENT_LIST_MIDL, REF_EVENT_LIST_MIDT,
                 REF_EVENT_LIST_MIDL + REF_EVENT_LIST_MIDW, REF_EVENT_LIST_MIDT + REF_EVENT_LIST_MIDH);
  list_set_item_interval(p_ref_event_list, REF_SVC_LIST_VGAP);
  list_set_item_rstyle(p_ref_event_list, &nvod_list_item_rstyle);
  list_enable_select_mode(p_ref_event_list, TRUE);
  list_set_select_mode(p_ref_event_list, LIST_SINGLE_SELECT);
  list_set_count(p_ref_event_list, 1, NVOD_LIST_PAGE);
  list_set_field_count(p_ref_event_list, REF_EVENT_LIST_FIELD, NVOD_LIST_PAGE);
  list_select_item(p_ref_event_list,0);
  list_set_focus_pos(p_ref_event_list,0);
  gui_get_string(IDS_RECEIVING_DATA, unistr, 32);
  list_set_field_content_by_unistr(p_ref_event_list, 0, 1, unistr);
  list_set_update(p_ref_event_list, ref_event_list_update, 0);
 
  for (i = 0; i < REF_EVENT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_ref_event_list, (u8)i, (u32)(ref_event_list_attr[i].attr),
                       (u16)(ref_event_list_attr[i].width),
                       (u16)(ref_event_list_attr[i].left), (u8)(ref_event_list_attr[i].top));
    list_set_field_rect_style(p_ref_event_list, (u8)i, ref_event_list_attr[i].rstyle);
    list_set_field_font_style(p_ref_event_list, (u8)i, ref_event_list_attr[i].fstyle);
  }
  
  //nvod list sbar
  p_ref_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_REF_EVENT_LIST_SBAR, REF_EVENT_LIST_SBARX,
    REF_EVENT_LIST_SBARY, REF_EVENT_LIST_SBARW, REF_EVENT_LIST_SBARH, p_list_cont, 0);
  ctrl_set_rstyle(p_ref_list_sbar, RSI_NVOD_SBAR, RSI_NVOD_SBAR, RSI_NVOD_SBAR);
  sbar_set_autosize_mode(p_ref_list_sbar, 1);
  sbar_set_direction(p_ref_list_sbar, 0);
  sbar_set_mid_rstyle(p_ref_list_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID,
                     RSI_SCROLL_BAR_MID);
  list_set_scrollbar(p_ref_event_list,p_ref_list_sbar);

  //shift event list cont
  p_sht_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_SHIFT_EVENT_LIST_CONT, SHIFT_EVENT_LIST_CONT_X, SHIFT_EVENT_LIST_CONT_Y,
              SHIFT_EVENT_LIST_CONT_W, SHIFT_EVENT_LIST_CONT_H, p_cont,
              0);
  ctrl_set_rstyle(p_sht_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  
  //shift event list
  p_shift_event_list =ctrl_create_ctrl(CTRL_LIST, IDC_SHIFT_EVENT_LIST, SHIFT_EVENT_LIST_X, SHIFT_EVENT_LIST_Y,
                 SHIFT_EVENT_LIST_W, SHIFT_EVENT_LIST_H, p_sht_list_cont,
                 0);
  ctrl_set_rstyle(p_shift_event_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_shift_event_list, shift_event_list_keymap);
  ctrl_set_proc(p_shift_event_list, shift_event_list_proc);
  ctrl_set_mrect(p_shift_event_list, SHIFT_EVENT_LIST_MIDL, SHIFT_EVENT_LIST_MIDT,
                 SHIFT_EVENT_LIST_MIDL + SHIFT_EVENT_LIST_MIDW, SHIFT_EVENT_LIST_MIDT + SHIFT_EVENT_LIST_MIDH);
  list_set_item_interval(p_shift_event_list, REF_SVC_LIST_VGAP);
  list_set_item_rstyle(p_shift_event_list, &nvod_list_item_rstyle);
  list_enable_select_mode(p_shift_event_list, TRUE);
  list_set_select_mode(p_shift_event_list, LIST_SINGLE_SELECT);
  list_set_count(p_shift_event_list, 0, NVOD_LIST_SHIFT_PAGE);
  list_set_field_count(p_shift_event_list, SHIFT_EVENT_LIST_FIELD, NVOD_LIST_SHIFT_PAGE);
  list_set_focus_pos(p_shift_event_list, 0);
  list_select_item(p_shift_event_list, 0);
  list_set_update(p_shift_event_list, shift_event_list_update, 0);
  
  for (i = 0; i < SHIFT_EVENT_LIST_FIELD; i++)
  {
    list_set_field_attr(p_shift_event_list, (u8)i, (u32)(shift_event_list_attr[i].attr),
                      (u16)(shift_event_list_attr[i].width),
                      (u16)(shift_event_list_attr[i].left), (u8)(shift_event_list_attr[i].top));
    list_set_field_rect_style(p_shift_event_list, (u8)i, shift_event_list_attr[i].rstyle);
    list_set_field_font_style(p_shift_event_list, (u8)i, shift_event_list_attr[i].fstyle);
  }

  //nvod shift event list sbar
  p_sht_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_SHIFT_EVENT_LIST_SBAR, SHIFT_EVENT_LIST_SBARX,
    SHIFT_EVENT_LIST_SBARY, SHIFT_EVENT_LIST_SBARW, SHIFT_EVENT_LIST_SBARH, p_sht_list_cont, 0);
  ctrl_set_rstyle(p_sht_list_sbar, RSI_NVOD_SBAR, RSI_NVOD_SBAR, RSI_NVOD_SBAR);
  sbar_set_autosize_mode(p_sht_list_sbar, 1);
  sbar_set_direction(p_sht_list_sbar, 0);
  sbar_set_mid_rstyle(p_sht_list_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID,
                     RSI_SCROLL_BAR_MID);
  list_set_scrollbar(p_shift_event_list, p_sht_list_sbar);
  
  // pbar  
  p_bar = ui_comm_bar_create(p_cont, IDC_NVOD_PROGRESS_PBAR,
                               NVOD_PBAR_X,
                               NVOD_PBAR_Y,
                               NVOD_PBAR_W,
                               NVOD_PBAR_H,
                               NVOD_PBAR_NAME_X, 
                               NVOD_PBAR_NAME_Y, 
                               NVOD_PBAR_NAME_W, 
                               NVOD_PBAR_NAME_H,
                               NVOD_PBAR_PERCENT_X,
                               NVOD_PBAR_PERCENT_Y,
                               NVOD_PBAR_PERCENT_W,
                               NVOD_PBAR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_PROGRESS, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_MID_BLUE,
                          RSI_PBACK, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
  ui_comm_bar_update(p_bar, 100, TRUE);
  ctrl_set_sts(p_bar, OBJ_STS_HIDE);

  //refresh_refrence_event_list(p_cont, FALSE);

  ui_comm_help_create2(&help_data[0], p_cont, FALSE);

  ctrl_default_proc(p_ref_event_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

/*static RET_CODE on_plist_exchange_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 old_focus;
  control_t *p_pg_info_cont;

  old_focus = list_get_focus_pos(p_list);
  ret = list_class_proc(p_list, msg, para1, para2);
  p_pg_info_cont = ctrl_get_child_by_id(p_list->p_parent, IDC_SHIFT_EVENT_LIST);
  ctrl_paint_ctrl(p_pg_info_cont, TRUE);

  return SUCCESS;
}*/

static RET_CODE refresh_play_progress(control_t *p_cont, BOOL b_show)
{
  control_t *p_bar = ctrl_get_child_by_id(p_cont, IDC_NVOD_PROGRESS_PBAR);
  //nvod_reference_svc_evt_t *p_nvod_reference_svc_evt = NULL;
  time_shifted_svc_evt_t *p_ts_svc_evt = NULL;
  utc_time_t cur_time = {0};
  u32 total_time, past_time;
  u16 progress_value = 0;

  //p_nvod_reference_svc_evt = ui_get_playing_nvod_reference_svc_evt();
  p_ts_svc_evt = ui_get_playing_nvod_time_shifted_svc_evt();

  if(b_show)
  {
    if(ctrl_get_sts(p_bar) == OBJ_STS_HIDE)
    {
      ctrl_set_sts(p_bar, OBJ_STS_SHOW);
    }

    time_get(&cur_time,TRUE);

    if(p_ts_svc_evt != NULL)
    {
      past_time = time_dec(&cur_time, &p_ts_svc_evt->start_time);
      total_time = time_conver(&p_ts_svc_evt->drt_time);
    }
    else
    {
      past_time = 0;
      total_time = 0;
    }

    if(total_time>0)
    {
      if(past_time<=total_time)
      {
        progress_value = (u16)(past_time*100/total_time);
      }
      else
      {
        progress_value = 100;
      }
    }
    else
    {
      progress_value = 0;
    }

    ui_comm_bar_update(p_bar, progress_value, TRUE);
    ctrl_paint_ctrl(p_bar, TRUE);
  }
  else
  {
    if(ctrl_get_sts(p_bar) != OBJ_STS_HIDE)
    {
      ctrl_set_sts(p_bar, OBJ_STS_HIDE);
      ctrl_paint_ctrl(p_cont, TRUE);
    }
  }
  
  return SUCCESS;
}
static RET_CODE on_ref_event_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_ref_list_cont, *p_sht_list_cont, *p_next_ctrl = NULL;

  if(!is_shift_evt_found)
  {
      return ERR_FAILURE;
  }
  p_ref_list_cont = ctrl_get_parent(p_list);
  p_cont = ctrl_get_parent(p_ref_list_cont);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
    case MSG_FOCUS_LEFT:
      p_sht_list_cont = ctrl_get_child_by_id(p_cont,IDC_SHIFT_EVENT_LIST_CONT);
      p_next_ctrl = ctrl_get_child_by_id(p_sht_list_cont, IDC_SHIFT_EVENT_LIST);
      break;
      
    //case MSG_FOCUS_LEFT:
    //  p_next_ctrl = ctrl_get_child_by_id(p_cont,IDC_REF_SVC_LIST);
    //  break;

    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      list_class_proc(p_list, MSG_SELECT, para1, para2);
      refresh_refrence_event_list(p_cont, TRUE);
      refresh_shift_event_list(p_cont, TRUE);
      nvod_play_by_shift_event_index(p_cont);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    //ctrl_paint_ctrl(p_list, TRUE);
    //ctrl_paint_ctrl(p_next_ctrl, TRUE);
    ui_comm_help_create2(&help_data[1], p_cont, FALSE);
    ctrl_paint_ctrl(p_cont, FALSE);
    refresh_play_progress(p_cont, TRUE);
  }

  return SUCCESS;
}


static RET_CODE on_shift_event_list_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_ref_list_cont, *p_sht_list_cont, *p_next_ctrl = NULL;
  
  p_sht_list_cont = ctrl_get_parent(p_list);
  p_cont = ctrl_get_parent(p_sht_list_cont);
  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
    case MSG_FOCUS_LEFT:
      p_ref_list_cont = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST_CONT);
      p_next_ctrl = ctrl_get_child_by_id(p_ref_list_cont,IDC_REF_EVENT_LIST);
      break;

    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
    case MSG_PAGE_UP:
    case MSG_PAGE_DOWN:
      list_class_proc(p_list, msg, para1, para2);
      list_class_proc(p_list, MSG_SELECT, para1, para2);
      nvod_play_by_shift_event_index(p_cont);
      refresh_play_progress(p_cont, TRUE);
      break;
      
    default:
      break;
  }

  if(p_next_ctrl != NULL)
  {
    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    //ctrl_paint_ctrl(p_list, TRUE);
    //ctrl_paint_ctrl(p_next_ctrl, TRUE);
    ui_comm_help_create2(&help_data[0], p_cont, FALSE);
    ctrl_paint_ctrl(p_cont, FALSE);
    refresh_play_progress(p_cont, FALSE);
  }

  return SUCCESS;
}

static void nvod_replace_the_conflict_event(void)
{
  book_delete_all_conflict_node(&book_node);
  book_add_node(&book_node);
}

static RET_CODE on_shift_event_list_ok(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont = ctrl_get_parent(p_list->p_parent);
  u32 i = 0;
  u32 shift_event_count = 0;
  u32 ref_event_count = 0;
  u16 shift_event_index = 0;
  control_t *p_ref_event_cont = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST_CONT);
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_ref_event_cont, IDC_REF_EVENT_LIST); 
  //u16 ref_svc_index = 0;
  u16 ref_event_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  //nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;
  time_shifted_svc_evt_t *p_shift_event = NULL;
  time_shifted_svc_t *p_ts_svc = NULL;
  
  if(nvod_data_get_ref_svc_cnt(h_nvod) == 0)
  {
    return SUCCESS;
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);

  p_ref_event = nvod_data_get_ref_evt(h_nvod, NULL, &ref_event_count);

  if(p_ref_event != NULL)
  {
    for (i = 0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }

  if(p_ref_event != NULL)
  {
    p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }

  shift_event_index = list_get_focus_pos(p_list);
  
  if(p_shift_event != NULL)
  {
    for (i = 0; (i<shift_event_count)&&(i<shift_event_index); i++)
    {
      if(p_shift_event == NULL)
      {
        break;
      }
      
      p_shift_event = nvod_data_get_shift_evt_next(h_nvod, p_shift_event);
    }
  }

  if(p_shift_event != NULL)
  {
    if(p_shift_event->evt_status == 1)
    {
      //p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);

      ui_set_playing_nvod_reference_svc_evt(p_ref_event);
      ui_set_playing_nvod_time_shifted_svc_evt(p_shift_event);
      
      //avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
      //ui_play_nvod(ui_get_timeshift_svc_pgid(p_ts_svc));
      ui_nvod_set_shift_event_index((u16)shift_event_index);
      
      return manage_open_menu(ROOT_ID_NVOD_VIDEO, 0, 0);
    }
    else
    {
      p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);
      memset(&book_node, 0, sizeof(book_node));
      book_node.pgid = ui_get_timeshift_svc_pgid(p_ts_svc);
      book_node.svc_type = SVC_TYPE_NVOD_TIMESHIFT_EVENT;
      book_node.book_mode = BOOK_TMR_ONCE;  
      if(p_ref_event != NULL)
      {
        memcpy(book_node.event_name, p_ref_event->event_name, sizeof(u16) * (EVENT_NAME_LENGTH + 1));
      }
      time_to_local(&(p_shift_event->start_time), &(book_node.start_time));
      memcpy((void *)&book_node.drt_time, (void *)&p_shift_event->drt_time, sizeof(utc_time_t)); 
      
      is_shift_evt_found = TRUE;

      switch(check_book_pg(&book_node))
      {
        case BOOK_ERR_DUR_TOO_SHORT:
          ui_comm_cfmdlg_open(NULL, IDS_LESS_ONE_MIN,  NULL, 0); 
          break;

        case BOOK_ERR_PLAYING:
          ui_comm_cfmdlg_open(NULL, IDS_CUR_EVENT_IS_PLAYING,  NULL, 0); 
          break;

        case BOOK_ERR_CONFILICT:
          //ui_comm_cfmdlg_open(NULL, IDS_BOOK_CONFLICT, RSC_INVALID_ID, NULL, 0); 
          //ui_comm_cfmdlg_open(NULL, IDS_BOOK_CONFLICT, RSC_INVALID_ID, NULL, 0); 
          ui_comm_ask_for_dodlg_open(NULL, IDS_CONFLICT_AND_REPLACE,  
                                   nvod_replace_the_conflict_event, NULL, 0);
          if(check_book_pg(&book_node) == BOOK_ERR_SAME)
          {
            list_set_field_content_by_icon(p_list, (u16)shift_event_index, 2, IM_EPG_COLORBUTTON_YELLOW); //JIAGUOTENG todo:
            //list_draw_field_ext(p_list, (u16)shift_event_index, 2, TRUE);
            refresh_shift_event_list(p_cont, TRUE);
            nvod_play_by_shift_event_index(p_cont);
            refresh_play_progress(p_cont, TRUE);
          }
          break;
        case BOOK_ERR_SAME: //cancel book node
          book_delete_node(book_get_match_node(&book_node));
          list_set_field_content_by_icon(p_list, (u16)shift_event_index, 2, 0);
          list_draw_field_ext(p_list, (u16)shift_event_index, 2, TRUE);
          break;

        case BOOK_ERR_FULL:
          ui_comm_cfmdlg_open(NULL, IDS_BOOK_IS_FULL,  NULL, 0);
          break;

        case BOOK_ERR_NO:   //book success
          list_set_field_content_by_icon(p_list, (u16)shift_event_index, 2, IM_EPG_BOOK);//JIAGUOTENG todo:
          list_draw_field_ext(p_list, (u16)shift_event_index, 2, TRUE);
          book_add_node(&book_node);
          break;
        default:
          break;
      }
    }
  }

  return SUCCESS;
}

#if 1
static RET_CODE on_nvod_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  //ui_nvod_release();
  ui_nvod_exit();
  
  return ERR_NOFEATURE;
}
#endif

static RET_CODE on_nvod_back_to_preview(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  disp_sys_t std;
  rect_t orc = 
  {
    (NVOD_CONT_X + NVOD_PREV_X),
    (NVOD_CONT_Y + NVOD_PREV_Y),
    (NVOD_CONT_X + NVOD_PREV_X + NVOD_PREV_W),
    (NVOD_CONT_Y + NVOD_PREV_Y + NVOD_PREV_H)
  };

    std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
   #ifndef WIN32
    switch(std)
    {
      case VID_SYS_1080I:
      case VID_SYS_1080I_50HZ:
      case VID_SYS_1080P:
      case VID_SYS_1080P_24HZ:
      case VID_SYS_1080P_25HZ:
      case VID_SYS_1080P_30HZ:
      case VID_SYS_1080P_50HZ:
        //to scale video
        orc.left = (s16)(orc.left * 1920 / 1280);
        orc.right = (s16)(orc.right * 1920 / 1280);
        orc.top = (s16)(orc.top * 1080 / 720);
        orc.bottom = (s16)(orc.bottom * 1080 / 720);
        break;

      case VID_SYS_NTSC_J:
      case VID_SYS_NTSC_M:
      case VID_SYS_NTSC_443:
      case VID_SYS_PAL_M:
      case VID_SYS_480P:
        orc.left = (s16)(orc.left * 720 / 1280);
        orc.right = (s16)(orc.right * 720 / 1280);
        orc.top = (s16)(orc.top * 480 / 720);
        orc.bottom = (s16)(orc.bottom * 480 / 720);
        break;

      case VID_SYS_PAL_N:
      case VID_SYS_PAL:
      case VID_SYS_PAL_NC:
      case VID_SYS_576P_50HZ:
        orc.left = (s16)(orc.left * 720 / 1280);
        orc.right = (s16)(orc.right * 720 / 1280);
        orc.top = (s16)(orc.top * 576 / 720);
        orc.bottom = (s16)(orc.bottom * 576 / 720);
        break;

      default:
        break;
      }
  #endif
  //avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                             // orc.left, orc.top, orc.right, orc.bottom);

  avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                     (u16)orc.left, (u16)orc.top, (u16)orc.right, (u16)orc.bottom);
  
  return SUCCESS;
}

static RET_CODE nvod_play_by_shift_event_index(control_t *p_cont)
{
  u32 i = 0;
  u32 shift_event_count = 0;
  u32 ref_event_count = 0;
  control_t *p_ref_event_cont = ctrl_get_child_by_id(p_cont,IDC_REF_EVENT_LIST_CONT);
  control_t* p_ref_event_list = ctrl_get_child_by_id(p_ref_event_cont, IDC_REF_EVENT_LIST); 
  //control_t* p_ref_svc_list = ctrl_get_child_by_id(p_cont, IDC_REF_SVC_LIST);
  control_t *p_shift_event_cont = ctrl_get_child_by_id(p_cont,IDC_SHIFT_EVENT_LIST_CONT);
  control_t* p_shift_event_list = ctrl_get_child_by_id(p_shift_event_cont, IDC_SHIFT_EVENT_LIST);
  //u16 ref_svc_index = 0;
  u16 ref_event_index = 0;
  u16 shift_event_index = 0;
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  nvod_reference_svc_evt_t *p_ref_event = NULL;
  time_shifted_svc_evt_t *p_shift_event = NULL;
  time_shifted_svc_evt_t *p_PlayingShiftEvent = NULL;
  time_shifted_svc_t *p_ts_svc = NULL;
  //rect_t orc = {0};
  if(nvod_data_get_ref_svc_cnt(h_nvod) == 0)
  {
    return SUCCESS;
  }

  ref_event_index = list_get_focus_pos(p_ref_event_list);

  p_ref_event = nvod_data_get_ref_evt(h_nvod, NULL, &ref_event_count);

  if(p_ref_event != NULL)
  {
    for (i = 0; i<ref_event_index; i++)
    {
      if(p_ref_event == NULL)
      {
        break;
      }
      
      p_ref_event = nvod_data_get_ref_evt_next(h_nvod, p_ref_event);
    }
  }

  if(p_ref_event != NULL)
  {
    p_shift_event = nvod_data_get_shift_evt(h_nvod, p_ref_event, &shift_event_count);
  }

  shift_event_index = list_get_focus_pos(p_shift_event_list);
  
  if(p_shift_event != NULL)
  {
    for (i = 0; (i<shift_event_count)&&(i<shift_event_index); i++)
    {
      if(p_shift_event == NULL)
      {
        break;
      }
      
      p_shift_event = nvod_data_get_shift_evt_next(h_nvod, p_shift_event);
    }
  }

  if(p_shift_event != NULL && p_shift_event->evt_status == 1)
  {
    p_PlayingShiftEvent = ui_get_playing_nvod_time_shifted_svc_evt();
    if((NULL == p_PlayingShiftEvent)
        ||(p_PlayingShiftEvent->time_shifted_svc_idx != p_shift_event->time_shifted_svc_idx))
    {
      if(p_ref_event != NULL)
      {
        p_ref_svc = nvod_data_get_ref_svc(h_nvod, p_ref_event->ref_svc_idx);
        p_ts_svc = nvod_data_get_shift_evt_svc(h_nvod, p_ref_event, p_shift_event);

        ui_set_playing_nvod_reference_svc_evt(p_ref_event);
        ui_set_playing_nvod_time_shifted_svc_evt(p_shift_event);
        #if 0
        manage_find_preview(ROOT_ID_NVOD, &orc);
        avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                   orc.left, orc.top, orc.right, orc.bottom);
        #endif
        // ui_play_nvod(ui_get_timeshift_svc_pgid(p_ts_svc));
        ui_play_nvod(p_ts_svc, &p_ref_svc->tp);
      }
    }
  }
  else
  {
    ui_set_playing_nvod_reference_svc_evt(NULL);
    ui_set_playing_nvod_time_shifted_svc_evt(NULL);
    ui_stop_play_nvod(STOP_PLAY_BLACK, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_nvod_refresh(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  class_handle_t h_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  nvod_reference_svc_t *p_ref_svc = NULL;
  u32 shift_event_count = 0;
  
  UI_PRINTF("on_nvod_refresh msg:%x\n",msg);
  
  switch(msg)
  {
    case MSG_NVOD_REF_SVC_FOUND:      
    case MSG_NVOD_REF_EVT_FOUND:
      refresh_refrence_event_list(p_cont, is_shift_evt_found  ? TRUE : FALSE);
      break;
      
    case MSG_NVOD_SHIFT_EVT_FOUND:
      shift_event_count = (u32)refresh_shift_event_list(p_cont, is_shift_evt_found  ? TRUE : FALSE);
      if(0 != shift_event_count)
      {
        is_shift_evt_found = TRUE;
        refresh_refrence_event_list(p_cont, TRUE);
        refresh_shift_event_list(p_cont, TRUE);
      }
      nvod_play_by_shift_event_index(p_cont);
      break;
      
    case MSG_NVOD_SVC_SCAN_END:
      //refresh_shift_event_list(p_cont, TRUE);
      if(nvod_data_get_ref_svc_cnt(h_nvod))
      {
        p_ref_svc = nvod_data_get_ref_svc(h_nvod, 0);
        ui_nvod_start_ref_svc(&p_ref_svc->tp);
      }
      break;
    
    default:
      break;
  }
  
  return SUCCESS;
}

/*static RET_CODE on_nvod_destroy(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_nvod_exit();
  
  return SUCCESS;
}*/


BEGIN_KEYMAP(nvod_cont_keymap, ui_comm_root_keymap) 
END_KEYMAP(nvod_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(nvod_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_nvod_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_nvod_exit)
  ON_COMMAND(MSG_NVOD_SVC_SCAN_END, on_nvod_refresh)
  ON_COMMAND(MSG_NVOD_REF_SVC_FOUND, on_nvod_refresh)
  ON_COMMAND(MSG_NVOD_REF_EVT_FOUND, on_nvod_refresh)
  ON_COMMAND(MSG_NVOD_SHIFT_EVT_FOUND, on_nvod_refresh)
  //ON_COMMAND(MSG_NVOD_PRE_SCAN_END, on_nvod_refresh)
  ON_COMMAND(MSG_NVOD_FULL_PLAY_EXIT, on_nvod_back_to_preview)
  //ON_COMMAND(MSG_DESTROY, on_nvod_destroy)
END_MSGPROC(nvod_cont_proc, ui_comm_root_proc)

/*BEGIN_KEYMAP(nvod_list_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(nvod_list_keymap, NULL)

BEGIN_MSGPROC(nvod_list_proc, list_class_proc)
  //ON_COMMAND(MSG_SELECT, on_select_cbox)
END_MSGPROC(nvod_list_proc, list_class_proc)*/

BEGIN_KEYMAP(ref_svc_list_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)    
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ref_svc_list_keymap, NULL)

BEGIN_MSGPROC(ref_svc_list_proc, list_class_proc)
   //ON_COMMAND(MSG_FOCUS_RIGHT, on_ref_svc_list_msg)
   //ON_COMMAND(MSG_FOCUS_LEFT, on_ref_svc_list_msg)
   //ON_COMMAND(MSG_FOCUS_UP, on_ref_svc_list_msg)
   //ON_COMMAND(MSG_FOCUS_DOWN, on_ref_svc_list_msg)
   //ON_COMMAND(MSG_PAGE_UP, on_ref_svc_list_msg)
   //ON_COMMAND(MSG_PAGE_DOWN, on_ref_svc_list_msg)
END_MSGPROC(ref_svc_list_proc, list_class_proc)

BEGIN_KEYMAP(ref_event_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)    
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ref_event_keymap, NULL)

BEGIN_MSGPROC(ref_event_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_ref_event_list_msg)
  ON_COMMAND(MSG_FOCUS_LEFT, on_ref_event_list_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ref_event_list_msg)
  ON_COMMAND(MSG_FOCUS_UP, on_ref_event_list_msg)
  ON_COMMAND(MSG_PAGE_DOWN, on_ref_event_list_msg)
  ON_COMMAND(MSG_PAGE_UP, on_ref_event_list_msg)
END_MSGPROC(ref_event_proc, list_class_proc)

BEGIN_KEYMAP(shift_event_list_keymap, NULL) 
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)    
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(shift_event_list_keymap, NULL)

BEGIN_MSGPROC(shift_event_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_shift_event_list_msg)
  ON_COMMAND(MSG_FOCUS_LEFT, on_shift_event_list_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_shift_event_list_msg)
  ON_COMMAND(MSG_FOCUS_UP, on_shift_event_list_msg)
  ON_COMMAND(MSG_PAGE_DOWN, on_shift_event_list_msg)
  ON_COMMAND(MSG_PAGE_UP, on_shift_event_list_msg)
  ON_COMMAND(MSG_SELECT, on_shift_event_list_ok)
END_MSGPROC(shift_event_list_proc, list_class_proc)


