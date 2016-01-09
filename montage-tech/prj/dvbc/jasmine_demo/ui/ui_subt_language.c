/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_subt_language.h"
#include "iso_639_2.h"

#if ENABLE_TTX_SUBTITLE
enum subt_control_id 
{
  IDC_SUBT_LANG_TITLE = 1,
  IDC_SUBT_LANG_LIST,
  IDC_SUBT_LANG_LIST_SBAR,
};


static list_xstyle_t subt_lang_list_item_rstyle =
{
   RSI_PBACK,
   RSI_PBACK,
   RSI_ITEM_1_HL,
   RSI_ITEM_1_HL,
   RSI_ITEM_1_HL,
};

static list_xstyle_t subt_lang_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_xstyle_t subt_lang_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_field_attr_t subt_lang_list_attr[SUBT_LANG_FIELD] =
{
  { LISTFIELD_TYPE_ICON | STL_LEFT | STL_VCENTER, 
  20, 5, 0, &subt_lang_list_field_rstyle,  &subt_lang_list_field_rstyle},
  { LISTFIELD_TYPE_UNISTR,
  40, 25, 0, &subt_lang_list_field_rstyle,  &subt_lang_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
  160, 70, 0, &subt_lang_list_field_rstyle,  &subt_lang_list_field_fstyle },
};

extern BOOL get_subt_description_1(class_handle_t handle, pb_subt_info_t *p_info, u16 pg_id);

u16 subt_language_keymap(u16 key);
RET_CODE subt_language_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static pb_subt_info_t subt_info_all = {0};
static pb_subt_info_t subt_info_64M_support = {0};

static void _choose_support_subt_in_64M(void)
{
  u8 i= 0, j = 0;
  for(i = 0; i < subt_info_all.service_cnt; i ++)
  {
    if(subt_info_all.service[i].type < SUBT_TYPE_TTX_NORMAL)
    {
      subt_info_64M_support.service_cnt ++;
      subt_info_64M_support.service[j].type = subt_info_all.service[i].type;
      memcpy(subt_info_64M_support.service[j].lang, &subt_info_all.service[i].lang, LANG_NUM);
      j ++;
    }
  }

  if(0 != subt_info_64M_support.service_cnt)
  {
    if(subt_info_all.cur_service.type < SUBT_TYPE_TTX_NORMAL)
      {
        memcpy(subt_info_64M_support.cur_service.lang, subt_info_all.cur_service.lang, LANG_NUM);
        subt_info_64M_support.cur_service.type = subt_info_all.cur_service.type;
      }
    else
      {
        memcpy(subt_info_64M_support.cur_service.lang, subt_info_64M_support.service[0].lang, LANG_NUM);
        subt_info_64M_support.cur_service.type = subt_info_64M_support.service[0].type;
      }
  }
  return;
}

static RET_CODE subt_lang_list_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i = 0;
  u8 asc_str[8] = {0};

  list_set_field_content_by_ascstr(ctrl, 0, 1, "");
  list_set_field_content_by_ascstr(ctrl, 0, 2, "Subtitle OFF");

  for(i = 0; i < subt_info_64M_support.service_cnt; i++)
  { 
    sprintf(asc_str, "%d ", (u16)( i + 1));
    list_set_field_content_by_ascstr(ctrl, (u16)(1 + i), 1, asc_str);
    list_set_field_content_by_ascstr(ctrl, i+1, 2, (u8 *)subt_info_64M_support.service[i].lang);
  }

  ctrl_paint_ctrl(ctrl, TRUE);
  return SUCCESS;
}


static RET_CODE on_close_subt(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  osd_set_t osd_set = {0};
  cmd_t cmd = {0};

  sys_status_get_osd_set(&osd_set);

  if(osd_set.enable_subtitle == TRUE)
  {
    osd_set.enable_subtitle = FALSE;
    cmd.id = PB_CMD_HIDE_SUBT;
    ap_frm_do_command(APP_PLAYBACK, &cmd);
    sys_status_set_osd_set(&osd_set);
    sys_status_save();
#ifndef MULTI_PIC_ADV
    ui_show_watermark();
#endif
  }
  return SUCCESS;
}

RET_CODE open_subt_language(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_list= NULL, *p_sbar = NULL, *p_title = NULL;
  u8 i = 0, total =0;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  u16 focus=0;
  osd_set_t osd_set = {0};
  class_handle_t handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);

  memset(&subt_info_all, 0 ,sizeof(pb_subt_info_t));
  memset(&subt_info_64M_support, 0, sizeof(pb_subt_info_t));

  get_subt_description_1(handle, &subt_info_all, prog_id);
   _choose_support_subt_in_64M();

 total = subt_info_64M_support.service_cnt;

  if (0 == total)
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_CONTENT, NULL, 1000); 
    return ERR_FAILURE;  
  }
 
  //create cont
  p_cont =
    fw_create_mainwin((u8)(ROOT_ID_SUBT_LANGUAGE),
    SUBT_LANG_LIST_MENU_CONT_X, SUBT_LANG_LIST_MENU_CONT_Y,
    SUBT_LANG_LIST_MENU_CONT_W, SUBT_LANG_LIST_MENU_CONT_H,
    0, 0, OBJ_ATTR_ACTIVE, 0);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  // create title
  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_SUBT_LANG_TITLE, SUBT_LANG_LIST_TITLE_X, 
    SUBT_LANG_LIST_TITLE_Y, SUBT_LANG_LIST_TITLE_W, SUBT_LANG_LIST_TITLE_H, p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_SUBTITLE_LANGUAGE);

  //create subt language list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_SUBT_LANG_LIST, SUBT_LANG_LIST_LIST_X,
    SUBT_LANG_LIST_LIST_Y, SUBT_LANG_LIST_LIST_W, SUBT_LANG_LIST_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, subt_language_keymap);
  ctrl_set_proc(p_list, subt_language_proc);
  ctrl_set_mrect(p_list, SUBT_LANG_LIST_MID_L, SUBT_LANG_LIST_MID_T,
    SUBT_LANG_LIST_MID_W+SUBT_LANG_LIST_MID_L, SUBT_LANG_LIST_MID_H+SUBT_LANG_LIST_MID_T);
  list_set_item_interval(p_list, SUBT_LANG_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &subt_lang_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, total+1, SUBT_LANG_LIST_PAGE);
  list_set_field_count(p_list, SUBT_LANG_FIELD, SUBT_LANG_LIST_PAGE);

  for (i = 0; i < SUBT_LANG_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(subt_lang_list_attr[i].attr), (u16)(subt_lang_list_attr[i].width),
      (u16)(subt_lang_list_attr[i].left), (u8)(subt_lang_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, subt_lang_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, subt_lang_list_attr[i].fstyle);
  }
  sys_status_get_osd_set(&osd_set);
  if(FALSE == osd_set.enable_subtitle)
  {
    list_set_focus_pos(p_list, 0);
  }
  else
  {
    for(i = 0; i < subt_info_64M_support.service_cnt; i++)
    {
      if(strcmp(subt_info_64M_support.cur_service.lang, subt_info_64M_support.service[i].lang)==0)
      {
        focus = i+1;
        break;
      }
    }
    list_set_focus_pos(p_list, focus);
    ui_set_subt_display(TRUE);
  }
  list_set_update(p_list, subt_lang_list_update, 0);
  subt_lang_list_update(p_list, list_get_valid_pos(p_list), SUBT_LANG_LIST_PAGE, 0);

  //create scroll bar
  p_sbar =
    ctrl_create_ctrl(CTRL_SBAR, IDC_SUBT_LANG_LIST_SBAR,
    SUBT_LANG_LIST_SBAR_X, SUBT_LANG_LIST_SBAR_Y,
    SUBT_LANG_LIST_SBAR_W, SUBT_LANG_LIST_SBAR_H,
    p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID,
    RSI_SCROLL_BAR_MID);
  list_set_scrollbar(p_list, p_sbar); 

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_subt_language_select(control_t *p_ctrl,
                                        u16 msg, u32 para1, u32 para2)
{
  u16 focus,prog_id,i;
  osd_set_t osd_set = {0};

  prog_id = sys_status_get_curn_group_curn_prog_id();
  focus = list_get_focus_pos(p_ctrl);

  if(focus == 0)
  {
    on_close_subt(p_ctrl,msg, para1,  para2);
  }
  else
  {
    for(i = 0; i< subt_info_64M_support.service_cnt; i++)
    {
      list_set_field_content_by_icon(p_ctrl, i+1, 0, 0);
    }
    list_set_field_content_by_icon(p_ctrl, focus, 0, IM_TV_SELECT);
    ctrl_paint_ctrl( p_ctrl, TRUE);

    sys_status_get_osd_set(&osd_set);       
    if(osd_set.enable_subtitle)
    {
      ui_set_subt_display(FALSE);
    }

    ui_set_subt_service_1((char *)subt_info_64M_support.service[focus-1].lang, SUBT_TYPE_NORMAL);
    ui_set_subt_display(TRUE);

    osd_set.enable_subtitle = TRUE;
    sys_status_set_osd_set(&osd_set);
    sys_status_save();

    ui_watermark_release();
  }

  manage_close_menu(ROOT_ID_SUBT_LANGUAGE, 0, 0);

  return  SUCCESS;
}


static RET_CODE on_subt_language_exit(control_t *p_ctrl,
                                      u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_SUBT_LANGUAGE, 0, 0);
  return  SUCCESS;
}

BEGIN_KEYMAP(subt_language_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_SUBT, MSG_EXIT)  
END_KEYMAP(subt_language_keymap, NULL)

BEGIN_MSGPROC(subt_language_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_subt_language_select)
  ON_COMMAND(MSG_EXIT, on_subt_language_exit)
END_MSGPROC(subt_language_proc, list_class_proc)
#endif
