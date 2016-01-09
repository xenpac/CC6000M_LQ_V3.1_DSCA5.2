/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

enum comm_control_type
{
  COMM_INVALID_CTRL = 0,
  COMM_STATIC,
  COMM_SELECT,
  COMM_TIMEDIT,
  COMM_NUMEDIT,
  COMM_NUMSEL,
  COMM_PWDEDIT,
  COMM_T9EDIT,
#ifdef ENABLE_NETWORK
  COMM_IPEDIT,
  COMM_T9_V2_EDIT,
#endif
};

typedef void (*SET_ALIGN_TYPE)(control_t *, u32);
typedef void (*SET_FONT_STYLE)(control_t *, u32, u32, u32);

static control_t *_comm_ctrl_create(u8 type,
                                    control_t *parent,
                                    u8 ctrl_id,
                                    u16 x,
                                    u16 y,
                                    u16 lw,
                                    u16 rw)
{
  control_t *cont, *txt, *p_ctrl = NULL;
  control_t *p_ctrl_num = NULL;
  control_t *p_ctrl_sel = NULL;
  u8 rsi_hl;
#ifdef ENABLE_NETWORK
  u8 rsi_sh = RSI_PBACK, rsi_gray = RSI_PBACK;
#endif
  ctrl_type_t class_name = CTRL_TCNT;

  //ctrl_type_t class_name;
  SET_ALIGN_TYPE set_align_type = NULL;
  SET_FONT_STYLE set_font_style = NULL;
  keymap_t keymap;
  u16 num_w = 64;
  u16 num_x = 24;

  cont = ctrl_create_ctrl(CTRL_CONT, ctrl_id,
                     x, y, (u16)(lw + rw), COMM_CTRL_H,
                     parent, 0);
  ctrl_set_style (cont, STL_EX_WHOLE_HL);
  ctrl_set_rstyle(cont,
                  RSI_COMM_CONT_SH,
                  RSI_COMM_CONT_HL,
                  RSI_COMM_CONT_GRAY);
  ctrl_set_style(cont, STL_EX_WHOLE_HL);

  if(lw > 0)
  {
    txt =
      ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_TXT,
                       0, 0, lw, COMM_CTRL_H,
                       cont, 0);
    ctrl_set_rstyle(txt,
                    RSI_COMM_TXT_SH,
                    RSI_COMM_TXT_HL,
                    RSI_COMM_TXT_GRAY);
    text_set_content_type(txt, TEXT_STRTYPE_STRID);
    text_set_font_style(txt,
                        FSI_COMM_TXT_SH,
                        FSI_COMM_TXT_HL,
                        FSI_COMM_TXT_GRAY);
    text_set_align_type(txt, STL_LEFT | STL_VCENTER);
    text_set_offset(txt, COMM_CTRL_OX, 0);
  }

  switch(type)
  {
    case COMM_STATIC:
      class_name = CTRL_TEXT;
      rsi_hl = RSI_COMM_STATIC_HL;
      keymap = ui_comm_static_keymap;
      set_align_type = (SET_ALIGN_TYPE)text_set_align_type;
      set_font_style = (SET_FONT_STYLE)text_set_font_style;
      break;
    case COMM_SELECT:
      class_name = CTRL_CBOX;
      rsi_hl = RSI_COMM_SELECT_HL;
      keymap = ui_comm_select_keymap;
      set_align_type = (SET_ALIGN_TYPE)cbox_set_align_style;
      set_font_style = (SET_FONT_STYLE)cbox_set_font_style;
      break;
    case COMM_NUMSEL:
      class_name = CTRL_CONT;
      rsi_hl = RSI_COMM_SELECT_HL;
      keymap = NULL;
      set_align_type = NULL;
      set_font_style = NULL;
      break;
    case COMM_TIMEDIT:
      class_name = CTRL_TBOX;
      rsi_hl = RSI_COMM_TIMEDIT_HL;
      keymap = ui_comm_tbox_keymap;
      set_align_type = (SET_ALIGN_TYPE)tbox_set_align_type;
      set_font_style = (SET_FONT_STYLE)tbox_set_font_style;
      break;    
    case COMM_NUMEDIT:
      class_name = CTRL_NBOX;
      rsi_hl = RSI_COMM_NUMEDIT_HL;
      keymap = ui_comm_num_keymap;
      set_align_type = (SET_ALIGN_TYPE)nbox_set_align_type;
      set_font_style = (SET_FONT_STYLE)nbox_set_font_style;
      break;
    case COMM_PWDEDIT:
      class_name = CTRL_EBOX;
      rsi_hl = RSI_COMM_PWDEDIT_HL;
      keymap = ui_comm_edit_keymap;
      set_align_type = (SET_ALIGN_TYPE)ebox_set_align_type;
      set_font_style = (SET_FONT_STYLE)ebox_set_font_style;
      break;
    case COMM_T9EDIT:
      class_name = CTRL_EBOX;
      if(lw > 0)
      {
        rsi_hl = RSI_COMM_T9EDIT_HL;
      }
      else
      {
        rsi_hl = RSI_ITEM_1_HL;
      }
      keymap = ui_comm_t9_keymap;
      set_align_type = (SET_ALIGN_TYPE)ebox_set_align_type;
      set_font_style = (SET_FONT_STYLE)ebox_set_font_style;
      break;
#ifdef ENABLE_NETWORK     
      case COMM_IPEDIT:
      class_name = CTRL_IPBOX;
      rsi_sh = RSI_COMM_IPEDIT_SH;
      rsi_hl = RSI_COMM_IPEDIT_HL;
      rsi_gray = RSI_COMM_IPEDIT_GRAY;
      keymap = ui_comm_ipbox_keymap;
      set_align_type = (SET_ALIGN_TYPE)ipbox_set_align_type;
      set_font_style = (SET_FONT_STYLE)ipbox_set_font_style;
      break;
    case COMM_T9_V2_EDIT:
      class_name = CTRL_EBOX;
      if(lw > 0)
      {
        rsi_sh = RSI_COMM_T9EDIT_SH;
        rsi_hl = RSI_COMM_T9EDIT_HL;
        rsi_gray = RSI_COMM_T9EDIT_GRAY;
      }
      else
      {
        rsi_sh = RSI_ITEM_1_SH;
        rsi_hl = RSI_ITEM_1_HL;
        rsi_gray = RSI_ITEM_1_SH;
      }
      keymap = ui_comm_t9_v2_keymap;
      set_align_type = (SET_ALIGN_TYPE)ebox_set_align_type;
      set_font_style = (SET_FONT_STYLE)ebox_set_font_style;
      break;
#endif
    default:
      rsi_hl = 0;
      class_name = CTRL_TCNT;
      keymap = NULL;
      set_align_type = NULL;
      set_font_style = NULL;
      MT_ASSERT(0);
  }

  p_ctrl = ctrl_create_ctrl(class_name, IDC_COMM_CTRL,
                            lw , 0, rw, COMM_CTRL_H,
                            cont, 0);
  
#ifdef ENABLE_NETWORK
  ctrl_set_rstyle(p_ctrl, rsi_sh, rsi_hl, rsi_gray);
#endif

  ctrl_set_rstyle(p_ctrl, RSI_PBACK, rsi_hl, RSI_PBACK);
  ctrl_set_keymap(p_ctrl, keymap);

  if((NULL != set_align_type) || (NULL !=  set_font_style))
  {
    if(set_align_type != NULL)
    {
      set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
    }
    if(set_font_style != NULL)
    {
      set_font_style(p_ctrl,
                     FSI_COMM_CTRL_SH,
                     FSI_COMM_CTRL_HL,
                     FSI_COMM_CTRL_GRAY);
    }
  }
  /* after creating */
  switch(type)
  {
    case COMM_NUMEDIT:
      /* set default proc for chk data */
      ctrl_set_proc(p_ctrl, ui_comm_num_proc);
      break;
    case COMM_TIMEDIT:
      ctrl_set_proc(p_ctrl, ui_comm_time_proc);
      break;
#ifdef ENABLE_NETWORK      
    case COMM_IPEDIT:
      ctrl_set_proc(p_ctrl, ui_comm_ipbox_proc);
      break;
#endif      
    case COMM_PWDEDIT:
      ebox_set_worktype(p_ctrl, EBOX_WORKTYPE_HIDE);
      ctrl_set_proc(p_ctrl, ui_comm_edit_proc);
      break;
    case COMM_T9EDIT:
      ctrl_set_proc(p_ctrl, ui_comm_t9_proc);
      break;
    case COMM_NUMSEL:
      ctrl_set_style(p_ctrl, STL_EX_WHOLE_HL);
      /*creat num box*/
      p_ctrl_num = ctrl_create_ctrl(CTRL_NBOX, IDC_COMM_EXT_NUM,
                                num_x, 0, num_w, COMM_CTRL_H,
                                p_ctrl, 0);
      ctrl_set_rstyle(p_ctrl_num, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      ctrl_set_keymap(p_ctrl_num, ui_comm_numsel_keymap);
      ctrl_set_proc(p_ctrl_num, ui_comm_numsel_nbox_proc);
      nbox_set_align_type(p_ctrl_num, STL_RIGHT| STL_VCENTER);
      nbox_set_font_style(p_ctrl_num,
                     FSI_COMM_CTRL_SH,
                     FSI_COMM_CTRL_HL,
                     FSI_COMM_CTRL_GRAY);
      nbox_set_num_type(p_ctrl_num, NBOX_NUMTYPE_DEC);
      /*creat select box*/
      p_ctrl_sel = ctrl_create_ctrl(CTRL_CBOX, IDC_COMM_EXT_SEL,
                            num_x + num_w, 0, rw - num_x - num_w - 60, 
                            COMM_CTRL_H, p_ctrl, 0);
      ctrl_set_rstyle(p_ctrl_sel, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      ctrl_set_keymap(p_ctrl_sel, ui_comm_numsel_keymap);

      cbox_set_align_style(p_ctrl_sel, STL_CENTER| STL_VCENTER);
      cbox_set_font_style(p_ctrl_sel,
                   FSI_COMM_CTRL_SH,
                   FSI_COMM_CTRL_HL,
                   FSI_COMM_CTRL_GRAY);        
      ctrl_set_proc(p_ctrl_sel, ui_comm_numsel_cbox_proc);
      ctrl_set_active_ctrl(p_ctrl, p_ctrl_sel);
      break;
    default:
      /* do nothing */
      break;
  }

  ctrl_set_active_ctrl(cont, p_ctrl);
  return cont;
}


void _comm_ctrl_set_static_txt(control_t *p_ctrl, u16 strid)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_TXT);
  if(txt != NULL)
  {
    text_set_content_by_strid(txt, strid);
  }
}


void ui_comm_ctrl_set_cont_rstyle(control_t *cont,
                                  u8 sh_rstyle,
                                  u8 hl_rstyle,
                                  u8 gr_rstyle)
{
  ctrl_set_rstyle(cont, sh_rstyle, hl_rstyle, gr_rstyle);
}


void ui_comm_ctrl_set_txt_rstyle(control_t *cont,
                                 u8 sh_rstyle,
                                 u8 hl_rstyle,
                                 u8 gr_rstyle)
{
  control_t *p_txt;

  p_txt = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  ctrl_set_rstyle(p_txt, sh_rstyle, hl_rstyle, gr_rstyle);
}


void ui_comm_ctrl_set_ctrl_rstyle(control_t *cont,
                                  u8 sh_rstyle,
                                  u8 hl_rstyle,
                                  u8 gr_rstyle)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  ctrl_set_rstyle(p_ctrl, sh_rstyle, hl_rstyle, gr_rstyle);
}


void ui_comm_ctrl_set_keymap(control_t *p_ctrl, keymap_t keymap)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ctrl_set_keymap(sub_ctrl, keymap);
}


void ui_comm_ctrl_set_proc(control_t *p_ctrl, msgproc_t proc)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ctrl_set_proc(sub_ctrl, proc);
}


void ui_comm_ctrl_update_attr(control_t *p_ctrl, BOOL is_enable)
{
  control_t *sub_ctrl;
  rect_t rc;
  u8 attr = ctrl_get_attr(p_ctrl);

  if(is_enable)
  {
    if(attr == OBJ_ATTR_INACTIVE)
    {
      ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
    }
  }
  else
  {
    ctrl_set_attr(p_ctrl, OBJ_ATTR_INACTIVE);
  }

  if(attr != ctrl_get_attr(p_ctrl))
  {
    /*paint all*/
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  }
  else
  {
    /*only p_ctrl*/
    sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
    ctrl_get_frame(sub_ctrl, &rc);
    ctrl_add_rect_to_invrgn(p_ctrl, &rc);
  }
}


void ui_comm_ctrl_paint_ctrl(control_t *p_ctrl, BOOL is_force)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ctrl_paint_ctrl(sub_ctrl, is_force);
}


/*********************************************************************
 * COMMON SELECT
 *********************************************************************/
control_t *ui_comm_select_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw)
{
  return _comm_ctrl_create(COMM_SELECT, parent, ctrl_id, x, y, lw, rw);
}

void ui_comm_select_right_set_mrect(control_t *p_ctrl,
								     u16 left, u16 top, u16 right, u16 bottom)
{
	control_t *cbox;
	cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
	ctrl_set_mrect(cbox, left, top, right, bottom);
}
void ui_comm_select_set_param(control_t *p_ctrl,
                              BOOL is_cycle,
                              u32 workmode,
                              u16 total,
                              u32 content_type,
                              cbox_dync_update_t callback)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_enable_cycle_mode(cbox, is_cycle);
  cbox_set_work_mode(cbox, workmode);

  switch(workmode)
  {
    case CBOX_WORKMODE_STATIC:
      cbox_static_set_count(cbox, total);
      cbox_static_set_content_type(cbox, content_type);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      cbox_dync_set_count(cbox, total);
      cbox_dync_set_update(cbox, callback);
      break;
    case CBOX_WORKMODE_NUMBER:
      cbox_num_set_type(cbox, content_type);
      break;
    default:
      /* do nothing */;
  }
}

void ui_comm_select_set_count(control_t *p_ctrl, u32 workmode, u16 total)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  switch(workmode)
  {
    case CBOX_WORKMODE_STATIC:
      cbox_static_set_count(cbox, total);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      cbox_dync_set_count(cbox, total);
      break;
    case CBOX_WORKMODE_NUMBER:
      break;
    default:
      /* do nothing */
      break;
  }
}

static list_xstyle_t dlist_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

static list_xstyle_t dlist_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};


static list_xstyle_t dlist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static void ui_comm_droplist_creat(control_t *p_ctrl_cbox, u16 page)
{
  control_t *cbox = NULL;
  rect_t rc_cbox = {0};

  if ((page == 0) || (NULL == p_ctrl_cbox))
  {
    /* don't need drop-down list */
    return;
  }

  cbox = p_ctrl_cbox;
  ctrl_get_frame(cbox, &rc_cbox);

  cbox_create_droplist(cbox, page, 6, 4);
  cbox_droplist_set_rstyle(cbox, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  cbox_droplist_set_mid_rect(cbox, 8, 4, (u16)RECTW(rc_cbox) - 16 - 16,
                             (u16)(RECTH(rc_cbox) * page - 4), 0);
  cbox_droplist_set_item_rstyle(cbox, &dlist_item_rstyle);
  cbox_droplist_set_field_attr(cbox, STL_CENTER | STL_VCENTER,
                               (u16)(RECTW(rc_cbox) - 16 - 16 - 16 - 8), 16, 0);
  cbox_droplist_set_field_rect_style(cbox, &dlist_field_rstyle);
  cbox_droplist_set_field_font_style(cbox, &dlist_field_fstyle);
  
  cbox_droplist_set_sbar_rstyle(cbox,
                                RSI_SCROLL_BAR_BG,
                                RSI_SCROLL_BAR_BG,
                                RSI_SCROLL_BAR_BG);
 // cbox_droplist_set_sbar_mid_rect(cbox, 0, 6, 6, RECTH(rc_cbox) * page - 8 - 12);
  cbox_droplist_set_sbar_mid_rstyle(cbox,
                                    RSI_SCROLL_BAR_MID,
                                    RSI_SCROLL_BAR_MID,
                                    RSI_SCROLL_BAR_MID);
  return;
}

void ui_comm_select_create_droplist(control_t *p_ctrl, u16 page)
{
  control_t *cbox = NULL;
  
  if (page == 0)
  {
    /* don't need drop-down list */
    return;
  }
  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ui_comm_droplist_creat(cbox, page);
  return;
}

void ui_comm_select_droplist_set_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_rstyle(p_cbox, sh_style, hl_style, gr_style);
}

void ui_comm_select_droplist_set_item_rstyle(control_t *p_ctrl,
  list_xstyle_t *p_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_item_rstyle(p_cbox, p_style);
}

void ui_comm_select_droplist_set_sbar_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_sbar_rstyle(p_cbox,  sh_style, hl_style, gr_style);
}

void ui_comm_select_droplist_set_sbar_mid_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox;

  p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  cbox_droplist_set_sbar_mid_rstyle(p_cbox,  sh_style, hl_style, gr_style);
}

void ui_comm_select_set_num_range(control_t *p_ctrl,
                                  s32 min,
                                  s32 max,
                                  u32 step,
                                  u8 bit_length,
                                  u16 *post_fix)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_num_set_range(cbox, min, max, step, bit_length);
  if(post_fix != NULL)
  {
    cbox_num_set_postfix_type(cbox, CBOX_ITEM_POSTFIX_TYPE_EXTSTR);
    cbox_num_set_postfix_by_extstr(cbox, post_fix);
  }
}


void ui_comm_select_get_num_range(control_t *p_ctrl,
                                  s32 *min,
                                  s32 *max,
                                  u32 *step)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_num_get_range(cbox, min, max, step);
}


void ui_comm_select_set_num(control_t *p_ctrl, s32 num)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_num_set_curn(cbox, num);
}

s32 ui_comm_select_get_num(control_t *p_ctrl)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return cbox_num_get_curn(cbox);
}


void ui_comm_select_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_select_set_content(control_t *p_ctrl, u8 idx, u16 strid)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_static_set_content_by_strid(cbox, idx, strid);
}

void ui_comm_select_set_content_by_unistr(control_t *p_ctrl, u8 idx, u16 *p_unistr)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_static_set_content_by_unistr(cbox, idx, p_unistr);
}

void ui_comm_select_set_content_by_dec(control_t *p_ctrl, u8 idx, s32 num)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  cbox_static_set_content_by_dec(cbox, idx, num);
}


void ui_comm_select_set_focus(control_t *p_ctrl, u16 focus)
{
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      cbox_static_set_focus(cbox, focus);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      cbox_dync_set_focus(cbox, focus);
      break;
    default:
      MT_ASSERT(0);
  }
}


u16 ui_comm_select_get_focus(control_t *p_ctrl)
{
  u16 focus = 0;
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      focus = cbox_static_get_focus(cbox);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      focus = cbox_dync_get_focus(cbox);
      break;
   case CBOX_WORKMODE_NUMBER:
     focus = (u16)cbox_num_get_curn(cbox);
    break;

    default:
      MT_ASSERT(0);
  }
  return focus;
}


u16 ui_comm_select_get_count(control_t *p_ctrl)
{
  u16 count = 0;
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      count = cbox_static_get_count(cbox);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      count = cbox_dync_get_count(cbox);
      break;
    default:
      MT_ASSERT(0);
  }
  return count;
}


u32 ui_comm_select_get_content(control_t *p_ctrl)
{
  u32 content = 0;
  control_t *cbox;

  cbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  switch(cbox_get_work_mode(cbox))
  {
    case CBOX_WORKMODE_STATIC:
      content = cbox_get_content(cbox, cbox_static_get_focus(p_ctrl));
      break;
    case CBOX_WORKMODE_DYNAMIC:
      content = cbox_get_content(cbox, cbox_dync_get_focus(p_ctrl));
      break;
    case CBOX_WORKMODE_NUMBER:
      content = (u32)cbox_num_get_curn(cbox);
      break;
    default:
      MT_ASSERT(0);
  }
  return content;
}

/*********************************************************************
 * COMMON NUMSEL
 *********************************************************************/
control_t *ui_comm_numsel_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_NUMSEL, parent, ctrl_id, x, y, lw, rw);
}
void ui_comm_numsel_set_param(control_t *p_ctrl,
                              BOOL is_cycle,
                              u32 workmode,
                              u16 total,
                              u8 bit_length,
                              u32 content_type,
                              cbox_dync_update_t callback)
{
    control_t *p_numsel_cont = NULL;
    control_t *p_ctrl_num = NULL;
    control_t *p_ctrl_sel = NULL;
    
    p_numsel_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
    p_ctrl_num = ctrl_get_child_by_id(p_numsel_cont, IDC_COMM_EXT_NUM);
    p_ctrl_sel = ctrl_get_child_by_id(p_numsel_cont, IDC_COMM_EXT_SEL);

    if((p_ctrl_num == NULL) || (p_ctrl_sel == NULL))
    {
        return;
    }
    
    /*set cbox para*/
    cbox_enable_cycle_mode(p_ctrl_sel, is_cycle);
    cbox_set_work_mode(p_ctrl_sel, workmode);
    switch(workmode)
    {
      case CBOX_WORKMODE_STATIC:
        cbox_static_set_count(p_ctrl_sel, total);
        cbox_static_set_content_type(p_ctrl_sel, content_type);
        break;
      case CBOX_WORKMODE_DYNAMIC:
        cbox_dync_set_count(p_ctrl_sel, total);
        cbox_dync_set_update(p_ctrl_sel, callback);
        break;
      case CBOX_WORKMODE_NUMBER:
        cbox_num_set_type(p_ctrl_sel, content_type);
        break;
      default:
        /* do nothing */
        break;
    }
    
    /*set num box para*/
    nbox_set_range(p_ctrl_num, 1, total, bit_length);
    nbox_set_focus(p_ctrl_num, bit_length -1);
    
    return;
}

#if 0
static u32 ui_comm_numsel_get_num_idx(control_t *p_cont)
{
  control_t *p_ctrl_num = NULL;
  s32 max_num = 0;
  s32 min_num = 0;
  u32 num = 0;
  
  p_ctrl_num = ctrl_get_child_by_id(p_cont, IDC_COMM_EXT_NUM);

  nbox_get_range(p_ctrl_num, &min_num, &max_num);
  num = nbox_get_num(p_ctrl_num);
  num = num - (u32)min_num;
  return num;
}

static void ui_comm_numsel_set_num_idx(control_t *p_cont, u32 idx)
{
  control_t *p_ctrl_num = NULL;
  s32 max_num = 0;
  s32 min_num = 0;
  u32 num = 0;
  
  p_ctrl_num = ctrl_get_child_by_id(p_cont, IDC_COMM_EXT_NUM);

  nbox_get_range(p_ctrl_num, &min_num, &max_num);
  num = idx + (u32)min_num;
  nbox_set_num_by_dec(p_ctrl_num, num);

}
#endif

void ui_comm_numsel_set_focus(control_t *p_ctrl, u16 focus)
{
  control_t *p_numsel_cont = NULL;
  control_t *p_ctrl_sel = NULL;
  control_t *p_ctrl_num = NULL;
  s32 max_num = 0;
  s32 min_num = 0;
  u32 num = 0;

  //return;
  p_numsel_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  p_ctrl_sel = ctrl_get_child_by_id(p_numsel_cont, IDC_COMM_EXT_SEL);
  p_ctrl_num = ctrl_get_child_by_id(p_numsel_cont, IDC_COMM_EXT_NUM);
  if((NULL == p_ctrl_sel) || (NULL == p_ctrl_num))
  {
      return;
  }
  switch(cbox_get_work_mode(p_ctrl_sel))
  {
    case CBOX_WORKMODE_STATIC:
      cbox_static_set_focus(p_ctrl_sel, focus);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      cbox_dync_set_focus(p_ctrl_sel, focus);
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  nbox_get_range(p_ctrl_num, &min_num, &max_num);
  num = (u32)min_num + focus;
  nbox_set_num_by_dec(p_ctrl_num, (u32)num);
  ctrl_add_rect_to_invrgn(p_numsel_cont, NULL); /**/

}

u16 ui_comm_numsel_get_focus(control_t *p_ctrl)
{
  control_t *p_numsel_cont = NULL;
  control_t *p_ctrl_sel = NULL;
  u16 focus = 0;

  p_numsel_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  p_ctrl_sel = ctrl_get_child_by_id(p_numsel_cont, IDC_COMM_EXT_SEL);
  
  switch(cbox_get_work_mode(p_ctrl_sel))
  {
    case CBOX_WORKMODE_STATIC:
      focus = cbox_static_get_focus(p_ctrl_sel);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      focus = cbox_dync_get_focus(p_ctrl_sel);
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return focus;
}
void ui_comm_numsel_create_droplist(control_t *p_ctrl, u16 page)
{
  control_t *p_cbox = NULL;
  control_t *p_cont = NULL;
  
  if (page == 0)
  {
    /* don't need drop-down list */
    return;
  }
  p_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  p_cbox = ctrl_get_child_by_id(p_cont, IDC_COMM_EXT_SEL);
  ui_comm_droplist_creat(p_cbox, page);
  return;
}

void ui_comm_numsel_droplist_set_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox = NULL;
  control_t *p_cont = NULL;

  p_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  p_cbox = ctrl_get_child_by_id(p_cont, IDC_COMM_EXT_SEL);
  cbox_droplist_set_rstyle(p_cbox, sh_style, hl_style, gr_style);
}

void ui_comm_numsel_droplist_set_item_rstyle(control_t *p_ctrl,
  list_xstyle_t *p_style)
{
  control_t *p_cbox = NULL;
  control_t *p_cont = NULL;

  p_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  p_cbox = ctrl_get_child_by_id(p_cont, IDC_COMM_EXT_SEL);

  cbox_droplist_set_item_rstyle(p_cbox, p_style);
}

void ui_comm_numsel_droplist_set_sbar_rstyle(control_t *p_ctrl,
  u8 sh_style, u8 hl_style, u8 gr_style)
{
  control_t *p_cbox = NULL;
  control_t *p_cont = NULL;

  p_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  p_cbox = ctrl_get_child_by_id(p_cont, IDC_COMM_EXT_SEL);

  cbox_droplist_set_sbar_rstyle(p_cbox,  sh_style, hl_style, gr_style);
}

/*********************************************************************
 * COMMON TIMEBOX
 *********************************************************************/
static RET_CODE on_time_unselect(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  RET_CODE ret = SUCCESS;

  if(tbox_is_on_edit(p_ctrl))
  {
    if(tbox_is_invalid_time(p_ctrl))
    {
      ret = ctrl_process_msg(p_ctrl, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}


static RET_CODE on_time_out_range(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_MSG_OUT_OF_RANGE, NULL, 0);
  return SUCCESS;
}


control_t *ui_comm_timedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_TIMEDIT, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_timedit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_timedit_set_param(control_t *p_ctrl,
                               u8 bit,
                               u8 item,
                               u32 time_type,
                               u32 sepa_type,
                               u8 num_width)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  tbox_set_time_type(p_tbox, time_type);
  tbox_set_focus(p_tbox, bit, (tbox_item_t)item);
  tbox_set_separator_type(p_tbox, sepa_type);
  tbox_set_max_num_width(p_tbox, num_width);
}


void ui_comm_timedit_set_separator_by_bmapid(control_t *p_ctrl,
                                             u8 index,
                                             u16 bmapid)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_bmpid(p_tbox, index, bmapid);
}


void ui_comm_timedit_set_separator_by_extstr(control_t *p_ctrl,
                                             u8 index,
                                             u16 *p_str)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_extstr(p_tbox, index, p_str);
}


void ui_comm_timedit_set_separator_by_strid(control_t *p_ctrl,
                                            u8 index,
                                            u16 strid)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_strid(p_tbox, index, strid);
}


void ui_comm_timedit_set_separator_by_ascchar(control_t *p_ctrl,
                                              u8 index,
                                              u8 ascchar)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_ascchar(p_tbox, index, ascchar);
}


void ui_comm_timedit_set_separator_by_unichar(control_t *p_ctrl,
                                              u8 index,
                                              u16 unichar)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_separator_by_unichar(p_tbox, index, unichar);
}


void ui_comm_timedit_set_time(control_t *p_ctrl, utc_time_t *p_time)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_set_time(p_tbox, p_time);
}


void ui_comm_timedit_get_time(control_t *p_ctrl, utc_time_t *p_time)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  tbox_get_time(p_tbox, p_time);
}

BOOL ui_comm_timedit_get_is_on_edit(control_t *p_ctrl)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return tbox_is_on_edit(p_tbox);
}

BOOL ui_comm_timedit_enter_edit(control_t *p_ctrl)
{
  control_t *p_tbox;

  p_tbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return tbox_enter_edit(p_tbox);
}

#ifdef ENABLE_NETWORK      
/*********************************************************************
 * COMMON IPBOX
 *********************************************************************/
static RET_CODE on_ipbox_unselect(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  RET_CODE ret = SUCCESS;

  if(ipbox_is_on_edit(p_ctrl))
  {
    if(ipbox_is_invalid_address(p_ctrl))
    {
      ret = ctrl_process_msg(p_ctrl, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}


static RET_CODE on_ipaddress_out_range(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_MSG_OUT_OF_RANGE, NULL, 0);
  return SUCCESS;
}


control_t *ui_comm_ipedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_IPEDIT, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_ipedit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_ipedit_set_param(control_t *p_ctrl,
                               u8 bit,
                               u8 item,
                               u32 address_type,
                               u32 sepa_type,
                               u8 num_width)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ipbox_set_address_type(p_ipbox, address_type);
  ipbox_set_focus(p_ipbox, bit, item);
  ipbox_set_separator_type(p_ipbox, sepa_type);
  ipbox_set_max_num_width(p_ipbox, num_width);
}


void ui_comm_ipedit_set_separator_by_bmapid(control_t *p_ctrl,
                                             u8 index,
                                             u16 bmapid)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ipbox_set_separator_by_bmpid(p_ipbox, index, bmapid);
}


void ui_comm_ipedit_set_separator_by_extstr(control_t *p_ctrl,
                                             u8 index,
                                             u16 *p_str)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ipbox_set_separator_by_extstr(p_ipbox, index, p_str);
}


void ui_comm_ipedit_set_separator_by_strid(control_t *p_ctrl,
                                            u8 index,
                                            u16 strid)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ipbox_set_separator_by_strid(p_ipbox, index, strid);
}


void ui_comm_ipedit_set_separator_by_ascchar(control_t *p_ctrl,
                                              u8 index,
                                              u8 ascchar)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ipbox_set_separator_by_ascchar(p_ipbox, index, ascchar);
}


void ui_comm_ipedit_set_separator_by_unichar(control_t *p_ctrl,
                                              u8 index,
                                              u16 unichar)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ipbox_set_separator_by_unichar(p_ipbox, index, unichar);
}


void ui_comm_ipedit_set_address(control_t *p_ctrl, ip_address_t *p_address)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ipbox_set_address(p_ipbox, p_address);
}


void ui_comm_ipedit_get_address(control_t *p_ctrl, ip_address_t *p_address)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ipbox_get_address(p_ipbox, p_address);
}

BOOL ui_comm_ipedit_get_is_on_edit(control_t *p_ctrl)
{
  control_t *p_ipbox;

  p_ipbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return ipbox_is_on_edit(p_ipbox);
}

void ui_comm_t9edit_set_font(control_t *cont,
                                 u32 n_fstyle,
                                 u32 h_fstyle,
                                 u32 g_fstyle)
{
  control_t *p_txt;

  p_txt = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  ebox_set_font_style(p_txt, n_fstyle, h_fstyle, g_fstyle);
}

/*********************************************************************
 * COMMON T9_V2 
 *********************************************************************/
control_t *ui_comm_t9_v2_edit_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw,
                                 u8 root_id)
{
  return _comm_ctrl_create(COMM_T9_V2_EDIT, parent, ctrl_id, x, y, lw, rw);
}

void ui_comm_t9_v2_edit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}

void ui_comm_t9_v2_edit_set_content_by_unistr(control_t *p_ctrl, u16 *unistr)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ebox_set_content_by_unistr(ebox, unistr);
}

void ui_comm_t9_v2_edit_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ebox_set_content_by_ascstr(ebox, ascstr);
}

u16 *ui_comm_t9_v2_edit_get_content(control_t *p_ctrl)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return ebox_get_content(ebox);
}

void ui_comm_t9_v2_edit_set_font(control_t *cont,
                                 u32 n_fstyle,
                                 u32 h_fstyle,
                                 u32 g_fstyle)
{
  control_t *p_txt;

  p_txt = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  ebox_set_font_style(p_txt, n_fstyle, h_fstyle, g_fstyle);
}
#endif

/*********************************************************************
 * COMMON NUMEDIT
 *********************************************************************/
static RET_CODE num_spin(control_t *p_ctrl, BOOL is_decrease)
{
  s32 min, max;
  u32 num;

  if(nbox_is_on_edit(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  num = nbox_get_num(p_ctrl);
  nbox_get_range(p_ctrl, &min, &max);

  if(is_decrease)
  {
    if((num <= (u32)min) || (num > (u32)max))
    {
      num = (u32)max;
    }
    else
    {
      num--;
    }
  }
  else
  {
    if((num >= (u32)max) || (num < (u32)min))
    {
      num = (u32)min;
    }
    else
    {
      num++;
    }
  }
  nbox_set_num_by_dec(p_ctrl, num);
  ctrl_paint_ctrl(p_ctrl, TRUE);

  return SUCCESS;
}


static RET_CODE on_num_spin_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  return num_spin(p_ctrl, FALSE);
}

static RET_CODE on_num_focus_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_ctrl != NULL);
  if((p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_ctrl, &border);
    
    nbox_exit_edit(p_ctrl);
    
    if(is_out)
    {
      return SUCCESS;
    }
  }

  return ERR_NOFEATURE;
}

static RET_CODE on_num_focus_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_ctrl != NULL);
  if((p_ctrl->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_ctrl, &border);
    
    nbox_exit_edit(p_ctrl);
    
    if(is_out)
    {
      return SUCCESS;
    }
  }

  return ERR_NOFEATURE;
}

static RET_CODE on_num_spin_down(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  return num_spin(p_ctrl, TRUE);
}


static RET_CODE on_num_unselect(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  RET_CODE ret = SUCCESS;
  u32 border;

  if(nbox_is_on_edit(p_ctrl))
  {
    if(nbox_is_outrange(p_ctrl, &border))
    {
      ret = ctrl_process_msg(p_ctrl, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}


static RET_CODE on_num_out_range(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_MSG_OUT_OF_RANGE, NULL, 0);
  return SUCCESS;
}


control_t *ui_comm_numedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_NUMEDIT, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_numedit_set_param(control_t *p_ctrl,
                               u32 type,
                               u32 min,
                               u32 max,
                               u8 bit_length,
                               u8 focus)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_num_type(nbox, type);
  nbox_set_postfix_type(nbox, type);
  nbox_set_range(nbox, (s32)min, (s32)max, bit_length);
  nbox_set_focus(nbox, focus);
}


void ui_comm_numedit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_numedit_set_num(control_t *p_ctrl, u32 num)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_num_by_dec(nbox, num);
}


u32 ui_comm_numedit_get_num(control_t *p_ctrl)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_get_num(nbox);
}


void ui_comm_numedit_set_postfix(control_t *p_ctrl, u16 strid)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_postfix_type(nbox, NBOX_ITEM_POSTFIX_TYPE_STRID);
  nbox_set_postfix_by_strid(nbox, strid);
}


u32 ui_comm_numedit_get_postfix(control_t *p_ctrl)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_get_postfix(nbox);
}


void ui_comm_numedit_set_decimal_places(control_t *p_ctrl, u8 places)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_separator(nbox, '.');
  nbox_set_separator_pos(nbox, places);
}


u8 ui_comm_numedit_get_decimal_places(control_t *p_ctrl)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_get_separator_pos(nbox);
}


void ui_comm_numedit_set_separator(control_t *p_ctrl, u8 separator, u8 places)
{
  control_t *nbox;

  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  nbox_set_separator(nbox, separator);
  nbox_set_separator_pos(nbox, places);
}


BOOL ui_comm_numedit_is_on_edit(control_t *p_ctrl)
{
  control_t *nbox;
  nbox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  return nbox_is_on_edit(nbox);
}

/*********************************************************************
 * COMMON STATIC
 *********************************************************************/
control_t *ui_comm_static_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw)
{
  return _comm_ctrl_create(COMM_STATIC, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_static_set_param(control_t *p_ctrl, u32 content_type)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_type(txt, content_type);
}


void ui_comm_static_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_static_set_content_by_strid(control_t *p_ctrl, u16 strid)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_strid(txt, strid);
}


void ui_comm_static_set_content_by_dec(control_t *p_ctrl, s32 num)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_dec(txt, num);
}


void ui_comm_static_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_ascstr(txt, ascstr);
}


void ui_comm_static_set_content_by_unistr(control_t *p_ctrl, u16 *unistr)
{
  control_t *txt;

  txt = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  text_set_content_by_unistr(txt, unistr);
}


void ui_comm_static_set_rstyle(control_t *cont,
                               u8 rsi_cont,
                               u8 rsi_stxt,
                               u8 rsi_ctrl)
{
  control_t *p_ctrl;

  ctrl_set_rstyle(cont, rsi_cont, rsi_cont, rsi_cont);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  ctrl_set_rstyle(p_ctrl, rsi_stxt, rsi_stxt, rsi_stxt);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  ctrl_set_rstyle(p_ctrl, rsi_ctrl, rsi_ctrl, rsi_ctrl);
}


void ui_comm_static_set_align_type(control_t *cont,
                                   u16 stxt_ox,
                                   u32 stxt_align,
                                   u16 ctrl_ox,
                                   u32 ctrl_align)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_TXT);
  text_set_align_type(p_ctrl, stxt_align);
  text_set_offset(p_ctrl, stxt_ox, 0);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);
  text_set_align_type(p_ctrl, ctrl_align);
  text_set_offset(p_ctrl, ctrl_ox, 0);
}


u32 ui_comm_static_get_content(control_t *cont)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_CTRL);

  return (u32)text_get_content(p_ctrl);
}


/**************************************************************
   COMMON PASSWORD EDIT
 **************************************************************/
static RET_CODE on_edit_get_focus(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  ebox_enter_edit(p_ctrl);
  return ERR_NOFEATURE;
}


static RET_CODE on_edit_lost_focus(control_t *p_ctrl,
                                   u16 msg,
                                   u32 para1,
                                   u32 para2)
{
  ebox_exit_edit(p_ctrl);
  return ERR_NOFEATURE;
}


control_t *ui_comm_pwdedit_create(control_t *parent,
                                  u8 ctrl_id,
                                  u16 x,
                                  u16 y,
                                  u16 lw,
                                  u16 rw)
{
  return _comm_ctrl_create(COMM_PWDEDIT, parent, ctrl_id, x, y, lw, rw);
}


void ui_comm_pwdedit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_pwdedit_set_param(control_t *p_ctrl, u16 maxtext)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_set_maxtext(ebox, maxtext);
}


void ui_comm_pwdedit_set_mask(control_t *p_ctrl, u8 bef_mask, u8 aft_mask)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_set_hide_mask(ebox, bef_mask, aft_mask);
}


u32 ui_comm_pwdedit_get_value(control_t *p_ctrl)
{
  control_t *ebox;
  u16 buf[SS_PWD_LENGTH + 1];
  u16 *str = NULL;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  str = ebox_get_content(ebox);

  memcpy(buf, str, SS_PWD_LENGTH * sizeof(u16));
  buf[SS_PWD_LENGTH] = '\0';

  return (u32)convert_dec_str_to_i(buf);
}


void ui_comm_pwdedit_empty_value(control_t *p_ctrl)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_empty_content(ebox);
}


#define T9_INVALID        0xFF

#define T9_MAX_KEY        10
#define T9_MAX_OPT        7

#define T9_INPUT_TMOUT    2000

enum local_msg
{
  MSG_TIMEOUT = MSG_LOCAL_BEGIN + 1000,
};

static struct t9_curninfo
{
  u8 root_id;
  BOOL is_upper;

  u8 curn_bit;
  u8 curn_key;
} g_t9_curninfo = {T9_INVALID, FALSE, T9_INVALID, T9_INVALID};

static u8 t9_opt[T9_MAX_KEY][T9_MAX_OPT] =
{
  {'0', ' ', '_', '-', },
  {'1', '?', '!', '@', '.', ',', '*'},
  {'2', 'a', 'b', 'c', },
  {'3', 'd', 'e', 'f', },
  {'4', 'g', 'h', 'i', },
  {'5', 'j', 'k', 'l', },
  {'6', 'm', 'n', 'o', },
  {'7', 'p', 'q', 'r', 's', },
  {'8', 't', 'u', 'v', },
  {'9', 'w', 'x', 'y', 'z'},
};

static u8 t9_bit_cnt[T9_MAX_KEY] =
{
  4,
  7,
  4,
  4,
  4,
  4,
  4,
  5,
  4,
  5,
};

static u8 t9edit_get_ascchar(u16 key)
{
  u8 ch;

  g_t9_curninfo.curn_bit %= t9_bit_cnt[key];

  ch = t9_opt[key][g_t9_curninfo.curn_bit];

  if(isalpha(ch))
  {
    if(g_t9_curninfo.is_upper)
    {
      ch = (u8)toupper(ch);
    }
  }

  return ch;
}


static void t9edit_notify_root(u16 msg, u32 para1, u32 para2)
{
  control_t *p_root;
  msgproc_t proc;

  p_root = fw_find_root_by_id(g_t9_curninfo.root_id);
  if(p_root != NULL)
  {
    proc = ctrl_get_proc(p_root);
    proc(p_root, msg, para1, para2);
  }
}


static RET_CODE on_t9_input(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 key, asc_char;
  u32 str_len;

  key = msg & MSG_DATA_MASK;
  str_len = uni_strlen((u16 *)ebox_get_content(p_ctrl));

  if((g_t9_curninfo.curn_bit == T9_INVALID)
    && (str_len == ebox_get_maxtext(p_ctrl)))
  {
    //do nothing, max length
  }
  else
  {
    if((key == g_t9_curninfo.curn_key)
      && (g_t9_curninfo.curn_bit != T9_INVALID))
    {
      //input the same char before time out.
      g_t9_curninfo.curn_bit++;
      fw_tmr_reset(g_t9_curninfo.root_id, MSG_TIMEOUT, T9_INPUT_TMOUT);
      ebox_back_space(p_ctrl);
      asc_char = t9edit_get_ascchar(key);
      ebox_input_char(p_ctrl, MSG_CHAR | asc_char);
    }
    else
    {
      //another key, or input the same key after timeout.
      g_t9_curninfo.curn_bit = 0;
      asc_char = t9edit_get_ascchar(key);
      ebox_input_char(p_ctrl, MSG_CHAR | asc_char);
      fw_tmr_destroy(g_t9_curninfo.root_id, MSG_TIMEOUT);
      fw_tmr_create(g_t9_curninfo.root_id, MSG_TIMEOUT, T9_INPUT_TMOUT, FALSE);
    }
    t9edit_notify_root(MSG_T9_CHANGED, (u32)ebox_get_content(p_ctrl), 0);
    //paint control
    ctrl_paint_ctrl(p_ctrl, FALSE);
    g_t9_curninfo.curn_key = key;
  }

  return SUCCESS;
}


static RET_CODE on_t9_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  g_t9_curninfo.root_id = T9_INVALID;
  return ERR_NOFEATURE;
}


static RET_CODE on_t9_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  g_t9_curninfo.curn_bit = T9_INVALID;
  fw_tmr_destroy(g_t9_curninfo.root_id, MSG_TIMEOUT);
  t9edit_notify_root(MSG_T9_CHANGED,
                     (u32)ebox_get_content(p_ctrl), 0);
  return SUCCESS;
}


control_t *ui_comm_t9edit_create(control_t *parent,
                                 u8 ctrl_id,
                                 u16 x,
                                 u16 y,
                                 u16 lw,
                                 u16 rw,
                                 u8 root_id)
{
  control_t *p_ctrl;

  if(g_t9_curninfo.root_id != T9_INVALID)  // is used
  {
    MT_ASSERT(0);
    return NULL;
  }

  if((p_ctrl =
        _comm_ctrl_create(COMM_T9EDIT, parent, ctrl_id, x, y, lw,
                          rw)) != NULL)
  {
    g_t9_curninfo.root_id = root_id;
    g_t9_curninfo.is_upper = 0;
    g_t9_curninfo.curn_bit = T9_INVALID;
    g_t9_curninfo.curn_key = T9_INVALID;
  }

  return p_ctrl;
}


void ui_comm_t9edit_set_static_txt(control_t *p_ctrl, u16 strid)
{
  _comm_ctrl_set_static_txt(p_ctrl, strid);
}


void ui_comm_t9edit_set_param(control_t *p_ctrl, u16 maxtext)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  ebox_set_maxtext(ebox, maxtext);
}


void ui_comm_t9edit_set_upper(control_t *p_ctrl, BOOL is_upper)
{
  if(g_t9_curninfo.root_id != T9_INVALID)
  {
    g_t9_curninfo.is_upper = is_upper;
  }
}


void ui_comm_t9edit_set_content_by_unistr(control_t *p_ctrl, u16 *unistr)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ebox_set_content_by_unistr(ebox, unistr);
}


void ui_comm_t9edit_set_content_by_ascstr(control_t *p_ctrl, u8 *ascstr)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  ebox_set_content_by_ascstr(ebox, (char *)ascstr);
}


u16 *ui_comm_t9edit_get_content(control_t *p_ctrl)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return ebox_get_content(ebox);
}


BOOL ui_comm_t9edit_backspace(control_t *p_ctrl)
{
  control_t *ebox;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  if(ebox_back_space(ebox))
  {
    t9edit_notify_root(MSG_T9_CHANGED, (u32)ebox_get_content(ebox), 0);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*********************************************************************
 * COMMON PROGRESS BAR
 *********************************************************************/
control_t *ui_comm_bar_create(control_t *parent,
                              u8 ctrl_id,
                              u16 x,
                              u16 y,
                              u16 w,
                              u16 h,
                              u16 txt_x,
                              u16 txt_y,
                              u16 txt_w,
                              u16 txt_h,
                              u16 percent_x,
                              u16 percent_y,
                              u16 percent_w,
                              u16 percent_h)
{
  rect_t rc_cont, rc_tmp;
  control_t *cont, *bar, *txt, *percent;

  // calc the rect of container
  set_rect(&rc_cont, (s16)x, (s16)y,
           (s16)(x + w), (s16)(y + h));
  set_rect(&rc_tmp, (s16)txt_x, (s16)txt_y,
           (s16)(txt_x + txt_w), (s16)(txt_y + txt_h));
  generate_boundrect(&rc_cont, &rc_cont, &rc_tmp);

  set_rect(&rc_tmp, (s16)percent_x, (s16)percent_y,
           (s16)(percent_x + percent_w), (s16)(percent_y + percent_h));
  generate_boundrect(&rc_cont, &rc_cont, &rc_tmp);

  // create
  cont = ctrl_create_ctrl(CTRL_CONT, ctrl_id,
                          (u16)(rc_cont.left), (u16)(rc_cont.top),
                          (u16)RECTW(rc_cont), (u16)RECTH(rc_cont),
                          parent, 0);
  ctrl_set_rstyle(cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  txt = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_BAR_TXT,
                         (u16)(txt_x - rc_cont.left), (u16)(txt_y - rc_cont.top),
                         txt_w, txt_h,
                         cont, 0);
  text_set_align_type(txt, STL_LEFT | STL_VCENTER);

  bar = ctrl_create_ctrl(CTRL_PBAR, IDC_COMM_BAR,
                         (u16)(x - rc_cont.left), (u16)(y - rc_cont.top), w, h,
                         cont, 0);
  ctrl_set_mrect(bar,
                    COMM_BAR_BORDER, COMM_BAR_BORDER,
                    (s16)(w - COMM_BAR_BORDER), (s16)(h - COMM_BAR_BORDER));

  percent = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_BAR_PERCENT,
                             (u16)(percent_x - rc_cont.left),
                             (u16)(percent_y - rc_cont.top),
                             percent_w, percent_h,
                             cont, 0);
  text_set_align_type(percent, STL_RIGHT | STL_VCENTER);
  text_set_content_type(percent, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(percent, (u8 *)"0%");

  return cont;
}


void ui_comm_bar_set_style(control_t *cont,
                           u8 rsi_bar,
                           u8 rsi_mid,
                           u8 rsi_txt,
                           u8 fsi_txt,
                           u8 rsi_percent,
                           u8 fsi_percent)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_TXT);
  ctrl_set_rstyle(p_ctrl, rsi_txt, rsi_txt, rsi_txt);
  text_set_font_style(p_ctrl, fsi_txt, fsi_txt, fsi_txt);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  ctrl_set_rstyle(p_ctrl, rsi_bar, rsi_bar, rsi_bar);
  pbar_set_rstyle(p_ctrl, rsi_mid, RSI_IGNORE, INVALID_RSTYLE_IDX);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);

  ctrl_set_rstyle(p_ctrl, rsi_percent, rsi_percent, rsi_percent);


 
  text_set_font_style(p_ctrl, fsi_percent, fsi_percent, fsi_percent);
}


void ui_comm_bar_set_param(control_t *cont,
                           u16 rsc_id,
                           u16 min,
                           u16 max,
                           u16 step)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_TXT);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, rsc_id);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  pbar_set_count(p_ctrl, min, max, step);
}


BOOL ui_comm_bar_update(control_t *cont, u16 val, BOOL is_force)
{
  control_t *p_ctrl;
  u8 str_buf[10];
  BOOL is_redraw = FALSE;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  if(pbar_get_current(p_ctrl) != val || is_force)
  {
    pbar_set_current(p_ctrl, val);
    p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
    sprintf((char *)str_buf, "%d%%", val);
    text_set_content_by_ascstr(p_ctrl, str_buf);

    is_redraw = TRUE;
  }

  return is_redraw;
}

BOOL ui_comm_tp_bar_update(control_t *cont, u16 val, BOOL is_force,u8 *pox)
{
  control_t *p_ctrl;
  u8 str_buf[10];
  BOOL is_redraw = FALSE;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  if(pbar_get_current(p_ctrl) != val || is_force)
  {
    pbar_set_current(p_ctrl, val);
    p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
    sprintf((char*)str_buf, "%d%s", val,pox);
    text_set_align_type(p_ctrl, STL_CENTER|STL_VCENTER);
    text_set_content_by_ascstr(p_ctrl, (u8*)str_buf);

    is_redraw = TRUE;
  }
  
  return is_redraw;
}

void ui_comm_bar_paint(control_t *cont, BOOL is_force)
{
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR);
  ctrl_paint_ctrl(p_ctrl, is_force);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_COMM_BAR_PERCENT);
  ctrl_paint_ctrl(p_ctrl, is_force);
}


void ui_comm_set_tp_postfix(u16 *postfix, u32 tp_cnt, u32 max_length)
{
  u8 ascstr[20 + 1];

  sprintf((char *)ascstr, "/%d", (u16)tp_cnt);
  if(max_length < 20)
  {
    ascstr[max_length] = '\0';
  }
  str_asc2uni(ascstr, postfix);
}


static RET_CODE on_numsel_msg_proc(control_t *p_ctrl,
                                   u16 msg,
                                   u32 para1,
                                   u32 para2)
{
  //control_t *p_comm_cont = NULL;
  control_t *p_numsel_cont = NULL;
  control_t *p_ctrl_num = NULL;
  //control_t *p_ctrl_sel = NULL;
  control_t *p_active_child = NULL;
  //u32 focus_num = 0;
  //u16 tmp_msg = MSG_INVALID;
  
  p_numsel_cont = p_ctrl->p_parent;
  if(NULL == p_numsel_cont)
  {
      return ERR_FAILURE;
  }
 // p_comm_cont = p_numsel_cont->p_parent;
  
  p_ctrl_num = ctrl_get_child_by_id(p_numsel_cont, IDC_COMM_EXT_NUM);
  //p_ctrl_sel = ctrl_get_child_by_id(p_numsel_cont, IDC_COMM_EXT_SEL);
  p_active_child = ctrl_get_active_ctrl(p_numsel_cont);
  //if(((msg&MSG_NUMBER) == MSG_NUMBER)&&(cbox_droplist_is_opened(p_active_child))) //switch to number key process
  {
      if(IDC_COMM_EXT_NUM != ctrl_get_ctrl_id(p_active_child))
      {
          ctrl_set_active_ctrl(p_numsel_cont, p_ctrl_num);
          ctrl_paint_ctrl(p_numsel_cont, TRUE);
      }
      nbox_class_proc(p_ctrl_num, msg, para1, para2);
      return SUCCESS;
  }
#if 0
  if(IDC_COMM_EXT_NUM == ctrl_get_ctrl_id(p_active_child))  //numbox is active
  {
      switch(msg)
      {
          case MSG_SELECT:
            nbox_class_proc(p_ctrl_num, msg, para1, para2);
            focus_num = ui_comm_numsel_get_num_idx(p_numsel_cont);
            ctrl_set_active_ctrl(p_numsel_cont, p_ctrl_sel);
            ui_comm_numsel_set_focus(p_comm_cont, (u16)focus_num);
            if(ctrl_is_onfocus(p_numsel_cont))
            {
              ctrl_paint_ctrl(p_numsel_cont, TRUE);
            }
            return SUCCESS;
          case MSG_OUTRANGE:
            nbox_exit_edit(p_ctrl_num);
            return SUCCESS;
          case MSG_INCREASE:
            tmp_msg = MSG_FOCUS_RIGHT;
            nbox_class_proc(p_ctrl_num, tmp_msg, para1, para2);
            return SUCCESS;
          case MSG_DECREASE:
            tmp_msg = MSG_FOCUS_LEFT;
            nbox_class_proc(p_ctrl_num, tmp_msg, para1, para2);
            return SUCCESS;
          default:
            break;
      }
  }
  else      //cbox is active
  {
    switch(msg)
    {
      case MSG_SELECT:
        return ERR_NOFEATURE;
      case MSG_INCREASE:
        tmp_msg = MSG_FOCUS_RIGHT;
        on_num_spin_up(p_ctrl_num, tmp_msg, para1, para2);
        return ERR_NOFEATURE;
      case MSG_DECREASE:
        tmp_msg = MSG_FOCUS_LEFT;
        on_num_spin_down(p_ctrl_num, tmp_msg, para1, para2);
        return ERR_NOFEATURE;
      case MSG_CHANGED:
        focus_num = (u32)ui_comm_numsel_get_focus(p_comm_cont);
        ui_comm_numsel_set_num_idx(p_numsel_cont, focus_num);
        if(ctrl_is_onfocus(p_numsel_cont))
        {
          ctrl_paint_ctrl(p_ctrl_num, TRUE);
        }
        return ERR_NOFEATURE;
        break;
      default:
        break;
    }
  }
  return SUCCESS;
#endif
}

BOOL ui_comm_pwdedit_is_full(control_t *p_ctrl)
{
  control_t *ebox;
  u16 *str = NULL;
  u16 leng = 0;

  ebox = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);
  str = ebox_get_content(ebox);
  leng = ebox_get_maxtext(ebox);
  if(uni_strlen(str) == leng)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

control_t * ui_comm_ctrl_get_ctrl(control_t *p_ctrl)
{
  control_t *sub_ctrl;

  sub_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_COMM_CTRL);

  return sub_ctrl;
}


// common controls keymap
BEGIN_KEYMAP(ui_comm_select_keymap, NULL)
ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
ON_EVENT(V_KEY_BACK, MSG_CANCEL)
END_KEYMAP(ui_comm_select_keymap, NULL)

BEGIN_KEYMAP(ui_comm_static_keymap, NULL)
ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ui_comm_static_keymap, NULL)

BEGIN_KEYMAP(ui_comm_t9_keymap, NULL)
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
END_KEYMAP(ui_comm_t9_keymap, NULL)

BEGIN_KEYMAP(ui_comm_num_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_CANCEL, MSG_UNSELECT)
ON_EVENT(V_KEY_EXIT, MSG_UNSELECT)
ON_EVENT(V_KEY_BACK, MSG_UNSELECT)
ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ui_comm_num_keymap, ui_comm_t9_keymap)

BEGIN_KEYMAP(ui_comm_tbox_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_UP, MSG_INCREASE)
ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_CANCEL, MSG_UNSELECT)
ON_EVENT(V_KEY_EXIT, MSG_UNSELECT)
ON_EVENT(V_KEY_BACK, MSG_UNSELECT)
ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ui_comm_tbox_keymap, ui_comm_t9_keymap)

BEGIN_KEYMAP(ui_comm_edit_keymap, NULL)
ON_EVENT(V_KEY_0, MSG_CHAR | '0')
ON_EVENT(V_KEY_1, MSG_CHAR | '1')
ON_EVENT(V_KEY_2, MSG_CHAR | '2')
ON_EVENT(V_KEY_3, MSG_CHAR | '3')
ON_EVENT(V_KEY_4, MSG_CHAR | '4')
ON_EVENT(V_KEY_5, MSG_CHAR | '5')
ON_EVENT(V_KEY_6, MSG_CHAR | '6')
ON_EVENT(V_KEY_7, MSG_CHAR | '7')
ON_EVENT(V_KEY_8, MSG_CHAR | '8')
ON_EVENT(V_KEY_9, MSG_CHAR | '9')
END_KEYMAP(ui_comm_edit_keymap, NULL)

BEGIN_KEYMAP(ui_comm_numsel_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
ON_EVENT(V_KEY_BACK, MSG_CANCEL)
END_KEYMAP(ui_comm_numsel_keymap, ui_comm_t9_keymap)

// common control's msgproc
BEGIN_MSGPROC(ui_comm_num_proc, nbox_class_proc)
ON_COMMAND(MSG_UNSELECT, on_num_unselect)
ON_COMMAND(MSG_OUTRANGE, on_num_out_range)
ON_COMMAND(MSG_FOCUS_LEFT, on_num_spin_down)
ON_COMMAND(MSG_FOCUS_RIGHT, on_num_spin_up)
ON_COMMAND(MSG_FOCUS_UP, on_num_focus_up)
ON_COMMAND(MSG_FOCUS_DOWN, on_num_focus_down)
END_MSGPROC(ui_comm_num_proc, nbox_class_proc)


BEGIN_MSGPROC(ui_comm_edit_proc, ebox_class_proc)
ON_COMMAND(MSG_GETFOCUS, on_edit_get_focus)
ON_COMMAND(MSG_LOSTFOCUS, on_edit_lost_focus)
END_MSGPROC(ui_comm_edit_proc, ebox_class_proc)

BEGIN_MSGPROC(ui_comm_t9_proc, ui_comm_edit_proc)
ON_COMMAND(MSG_NUMBER, on_t9_input)
ON_COMMAND(MSG_DESTROY, on_t9_destory)
ON_COMMAND(MSG_TIMEOUT, on_t9_timeout)
END_MSGPROC(ui_comm_t9_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(ui_comm_time_proc, tbox_class_proc)
ON_COMMAND(MSG_UNSELECT, on_time_unselect)
ON_COMMAND(MSG_OUTRANGE, on_time_out_range)
END_MSGPROC(ui_comm_time_proc, tbox_class_proc)

BEGIN_MSGPROC(ui_comm_numsel_cbox_proc, cbox_class_proc)
ON_COMMAND(MSG_NUMBER, on_numsel_msg_proc)
ON_COMMAND(MSG_INCREASE, on_numsel_msg_proc)
ON_COMMAND(MSG_DECREASE, on_numsel_msg_proc)
ON_COMMAND(MSG_SELECT, on_numsel_msg_proc)
ON_COMMAND(MSG_CHANGED, on_numsel_msg_proc)
END_MSGPROC(ui_comm_numsel_cbox_proc, cbox_class_proc)

BEGIN_MSGPROC(ui_comm_numsel_nbox_proc, nbox_class_proc)
ON_COMMAND(MSG_NUMBER, on_numsel_msg_proc)
ON_COMMAND(MSG_INCREASE, on_numsel_msg_proc)
ON_COMMAND(MSG_DECREASE, on_numsel_msg_proc)
ON_COMMAND(MSG_SELECT, on_numsel_msg_proc)
ON_COMMAND(MSG_OUTRANGE, on_numsel_msg_proc)
ON_COMMAND(MSG_CHANGED, on_numsel_msg_proc)
END_MSGPROC(ui_comm_numsel_nbox_proc, nbox_class_proc)

#ifdef ENABLE_NETWORK
BEGIN_KEYMAP(ui_comm_t9_v2_keymap, NULL)
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
END_KEYMAP(ui_comm_t9_v2_keymap, NULL)

BEGIN_KEYMAP(ui_comm_ipbox_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_UP, MSG_INCREASE)
ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_CANCEL, MSG_UNSELECT)
ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ui_comm_ipbox_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(ui_comm_ipbox_proc, ipbox_class_proc)
ON_COMMAND(MSG_UNSELECT, on_ipbox_unselect)
ON_COMMAND(MSG_OUTRANGE, on_ipaddress_out_range)
END_MSGPROC(ui_comm_ipbox_proc, ipbox_class_proc)
#endif

