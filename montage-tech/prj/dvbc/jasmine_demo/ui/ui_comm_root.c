/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/

#include "ui_common.h"
#include "ui_mute.h"

#define EN_TITLE_HELP

/*********************************************************************
 * COMMON TITLE
 *********************************************************************/
enum comm_title_idc
{
  IDC_COMM_TITLE_1 = 1,
  IDC_COMM_TITLE_2,
  IDC_COMM_TITLE_TXT,
#ifdef ENABLE_NETWORK
  IDC_COMM_TITLE_IC,
#endif
};

static u8 g_epg_mode_root_id = ROOT_ID_EPG;

u8 ui_comm_get_epg_root(void)
{
  return g_epg_mode_root_id;
}


void ui_comm_set_epg_root(u8 root_id)
{
  g_epg_mode_root_id = root_id;
}


void ui_comm_title_create(control_t *parent, u16 strid, u32 rstyle)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_txt, *p_first;
  MT_ASSERT(parent != NULL);

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_TITLE_CONT,
                            0, 26, TITLE_W, TITLE_H,
                            NULL, 0);
  ctrl_set_rstyle(p_cont, rstyle, rstyle, rstyle);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_TITLE_TXT,
                           TITLE_TXT_X, 10, TITLE_TXT_W, TITLE_TXT_H,
                           p_cont, 0);
  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_TITLE_TXT, FSI_TITLE_TXT, FSI_TITLE_TXT);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, strid);
  
  // insert to parent
  p_first = parent->p_child;

  if(p_first != NULL)
  {
    p_first->p_prev = p_cont;
    p_cont->p_next = p_first;
  }
  parent->p_child = p_cont;
  p_cont->p_parent = parent;
#endif
}


void ui_comm_title_set_content(control_t *parent, u16 strid)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_txt;
  if((p_cont = ctrl_get_child_by_id(parent, IDC_COMM_TITLE_CONT)) == NULL)
  {
    return;
  }

  p_txt = ctrl_get_child_by_id(p_cont, IDC_COMM_TITLE_TXT);
  text_set_content_by_strid(p_txt, strid);
#endif
}


void ui_comm_title_update(control_t *parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_txt;
  rect_t rc;
  if((p_cont = ctrl_get_child_by_id(parent, IDC_COMM_TITLE_CONT)) == NULL)
  {
    return;
  }

  p_txt = ctrl_get_child_by_id(p_cont, IDC_COMM_TITLE_TXT);
  ctrl_get_frame(p_txt, &rc);
  ctrl_add_rect_to_invrgn(p_cont, &rc);
  ctrl_paint_ctrl(p_cont, FALSE);
#endif
}


void ui_comm_title_hide(control_t *parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont;
  MT_ASSERT(parent != NULL);

  p_cont = ctrl_get_child_by_id(parent, IDC_COMM_TITLE_CONT);
  ctrl_set_sts(p_cont, OBJ_STS_HIDE);

#endif
}

/***********************************************************************
 *   COMMON  HELP BAR
 ***********************************************************************/

void ui_comm_help_create(comm_help_data_t *p_data, control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  u16 w, h;
  u8 i, row;
  
  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);
  
  (void)ctrl_destroy_child_by_id(p_parent, IDC_COMM_HELP_CONT);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  w = (u16)(ctrl_get_width(p_parent) - 2 * HELP_CONT_H_GAP);
  h = (HELP_ITEM_H + HELP_ITEM_V_GAP) * row - HELP_ITEM_V_GAP;

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            HELP_CONT_H_GAP,
                            (u16)(ctrl_get_height(p_parent) - h -
                                  HELP_CONT_V_GAP),
                            w, h, p_parent, 0);

  ctrl_set_rstyle(p_cont, RSI_HELP_BG, RSI_HELP_BG, RSI_HELP_BG);

  //w = w - 2 * HELP_ITEM_H_GAP, h = h - 2 * HELP_ITEM_V_GAP;
  w = w - 2 * HELP_ITEM_H_GAP - (HELP_ICON_X + HELP_ICON_W);
  h = h - 2 * HELP_ITEM_V_GAP;
  
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_HELP_MBOX,
                            HELP_ITEM_H_GAP, HELP_ITEM_V_GAP, w, h,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_mbox, 0, 0, w, h);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  mbox_set_item_interval(p_mbox, 0, 0);
  mbox_set_item_rstyle(p_mbox,
                       RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_mbox, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_mbox, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_mbox, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_mbox, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }
#endif
}

void ui_comm_help_create2(comm_help_data_t2 *p_data, control_t *p_parent, BOOL force)
{
  control_t *p_cont = NULL;
  control_t *p_txt, *p_bmp;
  u8 i, ctrl_id = 1;
  u16 x, y, w, h;

  MT_ASSERT(p_parent != NULL);
  
  if(p_data == NULL)
  {
    return;
  }
  
  (void)ctrl_destroy_child_by_id(p_parent, IDC_COMM_HELP_CONT);
  
  //if((p_cont = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT)) != NULL)
  //{
  //  cont_class_proc(p_cont, MSG_DESTROY, 0, 0);
  //}
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            HELP_CONT_X, HELP_CONT_Y,
                            HELP_CONT_W, HELP_CONT_H,
                            p_parent, 0);
  OS_PRINTF("\n fuction:%s, line:%d, HELP_CONT_Y:%d, HELP_CONT_W:%d, HELP_CONT_H:%d\n",
  	__FUNCTION__,__LINE__,HELP_CONT_Y,HELP_CONT_W,HELP_CONT_H);
  if(NULL == p_cont)
  {
      return;
  }
  ctrl_set_rstyle(p_cont, RSI_HELP_BG, RSI_HELP_BG, RSI_HELP_BG);

  x = (u16)p_data->offset;
  y = 5;
  h = (HELP_CONT_H - y);
  for(i=0; i<p_data->item_cnt; i++)
  {
    w = p_data->rsc_width[i];
    switch(p_data->rsc_id[i]&HELP_RSC_MASK)
    {
      case HELP_RSC_BMP:
        p_bmp = ctrl_create_ctrl(CTRL_BMAP, ctrl_id, x, y, w, h, p_cont, 0);
        bmap_set_content_by_id(p_bmp, (u16)(p_data->rsc_id[i]&(~HELP_RSC_MASK)));
        break;

      case HELP_RSC_STRID:
        p_txt = ctrl_create_ctrl(CTRL_TEXT, ctrl_id, x, y, w, h, p_cont, 0);
  OS_PRINTF("\n fuction:%s, line:%d, x:%d, y:%d, w:%d, h:%d\n",
  	__FUNCTION__,__LINE__,x,y,w,h);
  
        text_set_font_style(p_txt, FSI_WHITE, FSI_WHITE, FSI_WHITE);
        text_set_align_type(p_txt, STL_LEFT | STL_VCENTER);
        text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_txt, (u16)(p_data->rsc_id[i]&(~HELP_RSC_MASK)));
        break;
      default:
        break;
    }
    ctrl_id++;
    x += w;
  }

  if(force)
  {
    ctrl_paint_ctrl(p_cont, TRUE);
  }
}

void ui_comm_help_update2(control_t *p_parent)
{
  control_t *p_cont;

  if((p_cont = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT)) == NULL)
  {
    return;
  }
  
  ctrl_paint_ctrl(p_cont, TRUE);
}

void ui_comm_help_move_pos(control_t *p_parent, u16 x, u16 offset_top,
                           u16 w, u16 offset_bottom, u16 str_offset)
{
  control_t *p_tmp = NULL;
  rect_t frm = {0};
  
  p_tmp = ctrl_get_child_by_id(p_parent,IDC_COMM_HELP_CONT);
  ctrl_get_frame(p_tmp, &frm);
  frm.top = (s16)(frm.top +offset_top);
  frm.bottom = (s16)(frm.bottom + offset_bottom);
  frm.left= (s16)x;
  frm.right= (s16)(x + w);
  ctrl_set_frame(p_tmp, &frm);

  ctrl_add_rect_to_invrgn(p_tmp, NULL);

  p_tmp = ctrl_get_child_by_id(p_tmp,IDC_COMM_HELP_MBOX);
  ctrl_get_frame(p_tmp, &frm);
  frm.left= 0;
  frm.right= (s16)w;
  ctrl_set_frame(p_tmp, &frm);

  ctrl_add_rect_to_invrgn(p_tmp, NULL);
  ctrl_set_mrect(p_tmp, 0, 0, w, (u16)(frm.bottom-frm.top));
  mbox_set_item_interval(p_tmp, 0, 0);
  mbox_set_string_offset(p_tmp, (u8)str_offset, 0);
}

void ui_comm_help_create_ext(u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               comm_help_data_t *p_data,
                               control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  //u16 w, h;
  u8 i, row;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  //w = ctrl_get_width(p_parent) - 2 * HELP_CONT_H_GAP;
  //h = (HELP_ITEM_H + HELP_ITEM_V_GAP) * row - HELP_ITEM_V_GAP;

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            x, y,
                            w, h,
                            p_parent, 0);

  ctrl_set_rstyle(p_cont, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  w = w - 2 * HELP_ITEM_H_GAP;
  h = h - 2 * HELP_ITEM_V_GAP;
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_HELP_MBOX,
                            HELP_ITEM_H_GAP, HELP_ITEM_V_GAP, w, h,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  ctrl_set_mrect(p_mbox, 0, 0, w, h);
  mbox_set_item_interval(p_mbox, 0, 0);
  mbox_set_item_rstyle(p_mbox,
                       RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_mbox, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_mbox, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_mbox, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_mbox, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }
#endif
}

control_t *ui_comm_help_create_for_pop_dlg(comm_help_data_t *p_data, control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  u16 w, h;
  u8 i, row;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  w = (u16)ctrl_get_width(p_parent) - 2*HELP_CONT_POP_DLG_H_GAP;
  h = 80;

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            HELP_CONT_POP_DLG_H_GAP,
                            (u16)(ctrl_get_height(p_parent) - h),
                            w, h, p_parent, 0);

  ctrl_set_rstyle(p_cont, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  w = w - 2 * HELP_ITEM_H_GAP, h = h - 2 * HELP_ITEM_V_GAP;
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_HELP_MBOX,
                            HELP_ITEM_H_GAP, HELP_ITEM_V_GAP, w, h,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_mbox, 0, 0, w, h);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  mbox_set_item_interval(p_mbox, 0, 0);
  mbox_set_item_rstyle(p_mbox,
                       RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_mbox, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_mbox, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_mbox, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_mbox, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }

  return p_cont;
#else
  return NULL;
#endif
}


void ui_comm_help_set_data(comm_help_data_t *p_data, control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  u8 i, row;
  if((p_cont = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT)) == NULL)
  {
    return;
  }

  p_mbox = ctrl_get_child_by_id(p_cont, IDC_COMM_HELP_MBOX);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }
#endif
}


void ui_comm_help_update(control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont;
  rect_t rc;
  if((p_cont = ctrl_get_child_by_id(p_parent, IDC_COMM_HELP_CONT)) == NULL)
  {
    return;
  }
  ctrl_get_frame(p_cont, &rc);
  ctrl_add_rect_to_invrgn(p_parent, &rc);

  ctrl_paint_ctrl(p_parent, TRUE);
#endif
}


/*********************************************************************
 * COMMON ROOT
 *********************************************************************/
static BOOL g_is_locked = FALSE;

RET_CODE comm_mask_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


// ON COMM ROOT
static RET_CODE on_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();

  return SUCCESS;
}


static RET_CODE on_exit_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 root_id;

  MT_ASSERT(p_ctrl != NULL);

  root_id = (u8)ctrl_get_ctrl_id(p_ctrl);
  if(root_id == IDC_COMM_ROOT_CONT)
  {
    root_id = (u8)ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl));
  }

  manage_close_menu(root_id, 0, 0);

  return SUCCESS;
}


// ON COMM MASK
static RET_CODE on_save(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *cont;

  cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_ROOT_CONT);
  if(cont != NULL)
  {
    ctrl_process_msg(cont, msg, para1, para2);
  }

  return SUCCESS;
}

static RET_CODE on_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root_cont;
  control_t *cont;

  p_root_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_ROOT_CONT);
  cont = ctrl_get_child_by_id(p_root_cont, IDC_COMM_SIGN_CONT);
  if(cont != NULL)
  {
    ui_enable_signal_monitor(FALSE);

    ui_set_frontpanel_by_curn_pg();
  }

  // bottom half process by default msgmap
  return ERR_NOFEATURE;
}

static RET_CODE on_signal_update(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  control_t *cont;
  if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
  {
    control_t *p_root_cont;

    p_root_cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_ROOT_CONT);
    cont = ctrl_get_child_by_id(p_root_cont, IDC_COMM_SIGN_CONT);

    if(cont != NULL)
    {
      struct signal_data *data = (struct signal_data *)(para1);
      ui_comm_root_update_signbar(p_ctrl, data->intensity,
                                  data->quality, data->lock);

      if(data->lock != g_is_locked)
      {
        g_is_locked = data->lock;

        fw_tmr_reset(ctrl_get_ctrl_id(p_ctrl), MSG_BEEPER,
                     g_is_locked ? BEEPER_LOCKD_TMOUT :
                     BEEPER_UNLKD_TMOUT);
      }
    }
  }
  else
  {
    cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_ROOT_CONT);
    cont->p_proc(cont, msg, para1, para2);
  }
  return SUCCESS;
}


control_t *ui_comm_root_create(u8 root_id,
                               u8 parent_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid)
{
  control_t *p_mask, *p_cont;//, *p_help;

  p_mask = fw_create_mainwin(root_id,
                             SCREEN_POS_X, SCREEN_POS_Y, UI_COMMON_WIDTH, UI_COMMON_HEIGHT,
                             parent_root, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_mask != NULL);
  ctrl_set_rstyle(p_mask, RSI_TRANSPARENT,
                  RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_proc(p_mask, comm_mask_proc);

  if(root_id == ROOT_ID_EPG
    || root_id == ROOT_ID_EPG_FIND
    || root_id == ROOT_ID_SCHEDULE)
  {
    ui_comm_set_epg_root(root_id);
  }

  // create container
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_ROOT_CONT,
                            x, y, w, h, p_mask, 0);
  ctrl_set_rstyle(p_cont, RSI_MAINMENU_BG, RSI_MAINMENU_BG, RSI_MAINMENU_BG);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  //title icon
  ctrl_create_ctrl(CTRL_BMAP, IDC_COMM_TITLE_ICON,
                                  451, 64, 100, 84,
                                  p_cont, 0);
  if(title_icon != RSC_INVALID_ID)
  {
    //bmap_set_content_by_id(p_title_icon, title_icon);
  }

  //bottom rect
  #if 0
  h = 80;
  p_help = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            HELP_CONT_H_GAP,
                            (u16)(ctrl_get_height(p_cont) - h),
                            w, h, p_cont, 0);

  ctrl_set_rstyle(p_help, RSI_HELP_BG, RSI_HELP_BG, RSI_HELP_BG);

  //title text
  p_title_text = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_TITLE_TEXT,
                                  (w - 300) / 2, 20, 300, 40,
                                  p_cont, 0);
  ctrl_set_rstyle(p_title_text,
                  RSI_PBACK,
                  RSI_PBACK,
                  RSI_PBACK);

  text_set_font_style(p_title_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  if(title_strid != RSC_INVALID_ID)
  {
    text_set_content_by_strid(p_title_text, title_strid);
  }
  //text_set_content_type(p_title_text, STL_VCENTER);
#endif
  // create title
  ui_comm_title_create(p_cont, title_strid, RSI_TITLE_BG);
  
  return p_cont;
}


control_t *ui_comm_root_create_fullbg(u8 root_id,
                                      u8 parent_root,
                                      u16 x,
                                      u16 y,
                                      u16 w,
                                      u16 h,
                                      u16 title_icon,
                                      u16 title_strid)
{
  control_t *p_cont;

  p_cont = ui_comm_root_create(root_id, parent_root, x, y, w, h, title_icon, title_strid);
  ctrl_set_rstyle(p_cont,
                  RSI_LIGHT_BLUE_BACKGROUND,
                  RSI_LIGHT_BLUE_BACKGROUND,
                  RSI_LIGHT_BLUE_BACKGROUND);
  return p_cont;
}

control_t *ui_comm_root_create_with_signbar(u8 root_id,
                                            u8 parent_root,
                                            u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            u16 title_icon,
                                            u16 title_strid,
                                            BOOL is_beeper)
{
  control_t *p_cont, *p_frame, *p_ctrl;
  u16 i, stxt_y, bar_y, percent_y;
  u16 stxt_ids[] =
  {IDS_SIGNAL_INTENSITY, IDS_SIGNAL_QUALITY, 0};

  p_cont = ui_comm_root_create(root_id, parent_root,
                               x, y, w, h, title_icon, title_strid);
  //p_mask = ctrl_get_parent(p_cont);

  p_frame = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_SIGN_CONT,
                             0, h - ROOT_SIGN_CONT_H,
                             (u16)w, ROOT_SIGN_CONT_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_frame, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  stxt_y = ROOT_SIGN_BAR_STXT_Y;
  bar_y = ROOT_SIGN_BAR_Y;
  percent_y = ROOT_SIGN_BAR_PERCENT_Y;
  for(i = 0; i < 2; i++)
  {
    p_ctrl = ui_comm_bar_create(p_frame, (u8)(IDC_COMM_SIGN_INTENSITY + i),
                                ROOT_SIGN_BAR_X, bar_y,
                                ROOT_SIGN_BAR_W,
                                ROOT_SIGN_BAR_H,
                                ROOT_SIGN_BAR_STXT_X, stxt_y,
                                ROOT_SIGN_BAR_STXT_W,
                                ROOT_SIGN_BAR_STXT_H,
                                ROOT_SIGN_BAR_PERCENT_X, percent_y,
                                ROOT_SIGN_BAR_PERCENT_W,
                                ROOT_SIGN_BAR_PERCENT_H);
   // ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ui_comm_bar_set_style(p_ctrl,
                          RSI_ROOT_SIGN_BAR,
                          RSI_ROOT_SIGN_BAR_MID_1,
                          RSI_ROOT_SIGN_BAR_STXT,
                          FSI_ROOT_SIGN_BAR_STXT,
                          RSI_ROOT_SIGN_BAR_PERCENT,
                          FSI_ROOT_SIGN_BAR_PERCENT);
    ui_comm_bar_set_param(p_ctrl, stxt_ids[i], 0, 100, 100);
    ui_comm_bar_update(p_ctrl, 0, TRUE);

    stxt_y += ROOT_SIGN_BAR_STXT_H + ROOT_SIGN_CTRL_V_GAP;
    bar_y += ROOT_SIGN_BAR_STXT_H + ROOT_SIGN_CTRL_V_GAP;
    percent_y += ROOT_SIGN_BAR_STXT_H + ROOT_SIGN_CTRL_V_GAP;
  }

  /* TODO */
  ui_enable_signal_monitor(TRUE);
  if(is_beeper)
  {
       fw_tmr_create(root_id, MSG_BEEPER, g_is_locked ? BEEPER_LOCKD_TMOUT :
                   BEEPER_UNLKD_TMOUT, TRUE);
  }
//  g_is_locked = FALSE;

  return p_cont;
}


control_t *ui_comm_root_create_with_signbar_fullbg(u8 root_id,
                                                   u8 parent_root,
                                                   u16 x,
                                                   u16 y,
                                                   u16 w,
                                                   u16 h,
                                                   u16 title_icon,
                                                   u16 title_strid,
                                                   BOOL is_beeper)
{
  control_t *p_cont;

  p_cont = ui_comm_root_create_with_signbar(root_id, parent_root, x, y, w, h, \
                                            title_icon, title_strid, is_beeper);
  ctrl_set_rstyle(p_cont,
                  RSI_LIGHT_BLUE_BACKGROUND,
                  RSI_LIGHT_BLUE_BACKGROUND,
                  RSI_LIGHT_BLUE_BACKGROUND);
  return p_cont;
}


void update_comm_signbar(control_t *p_ctrl, u8 value, BOOL is_lock)
{
  control_t *p_bar;
  BOOL is_intensity = TRUE;
  
 switch(ctrl_get_ctrl_id(p_ctrl))
  {
    case IDC_COMM_SIGN_INTENSITY:
      is_intensity = TRUE;
      break;
    case IDC_COMM_SIGN_QUALITY:
       is_intensity = FALSE;
      break;
    default:  
      break;
    }
    
  if(ui_comm_bar_update(p_ctrl, value, FALSE)
    || (g_is_locked != is_lock))
  {
    p_bar = ctrl_get_child_by_id(p_ctrl, IDC_COMM_BAR);
    
    pbar_set_rstyle(
      p_bar,
      is_lock ? (is_intensity ? RSI_PROGRESS_BAR_MID_BLUE:
      RSI_PROGRESS_BAR_MID_GREEN):
      RSI_PROGRESS_BAR_MID_RED,
      RSI_PROGRESS_BAR_BG,
      INVALID_RSTYLE_IDX);

    ui_comm_bar_paint(p_ctrl, TRUE);
  }
}


void ui_comm_root_update_signbar(control_t *root,
                                 u8 intensity,
                                 u8 quality,
                                 BOOL is_lock)
{
  control_t *p_root_cont, *p_cont, *p_ctrl;

  p_root_cont = ctrl_get_child_by_id(root, IDC_COMM_ROOT_CONT);
  p_cont = ctrl_get_child_by_id(p_root_cont, IDC_COMM_SIGN_CONT);
  if(p_cont == NULL)
  {
    UI_PRINTF("ROOT: p_cont == NULL \n");
    return;
  }

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_COMM_SIGN_INTENSITY);
  update_comm_signbar(p_ctrl, intensity, is_lock);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_COMM_SIGN_QUALITY);
  update_comm_signbar(p_ctrl, quality, is_lock);

  // set front panel by quality
  ui_set_front_panel_by_num(quality);
}


control_t *ui_comm_root_get_ctrl(u8 root_id, u8 ctrl_id)
{
  control_t *root = fw_find_root_by_id(root_id);

  if(root == NULL)
  {
    return NULL;
  }
  if(ctrl_get_ctrl_id(root->p_child) == IDC_COMM_ROOT_CONT)
  {
    root = root->p_child;
  }
  return ctrl_get_child_by_id(root, ctrl_id);
}


control_t *ui_comm_root_get_root(u8 root_id)
{
  control_t *root = fw_find_root_by_id(root_id);

  if(root == NULL)
  {
    return NULL;
  }
  if(ctrl_get_ctrl_id(root->p_child) == IDC_COMM_ROOT_CONT)
  {
    root = root->p_child;
  }

  return root;
}


/*********************************************************************
 * COMMON MENU
 *********************************************************************/
enum comm_menu_idc
{
  IDC_COMM_MENU_MBOX = 1,
};

u16 comm_menu_mbox_keymap(u16 key);

u16 comm_menu_cont_keymap(u16 key);

control_t *ui_comm_menu_create(comm_menu_data_t *p_data, u32 para)
{
  control_t *p_cont, *p_mbox;
  u16 i, height;

  MT_ASSERT(p_data != NULL);
  MT_ASSERT(p_data->item_cnt > 0);

  height = p_data->item_cnt * (MENU_ITEM_H + MENU_ITEM_V_GAP) - MENU_ITEM_V_GAP;
  // create container
  p_cont = ui_comm_root_create(p_data->id, RSI_WINDOW_1,
                               MENU_X, MENU_Y, MENU_W,
                               (u16)(height + MENU_BORDER * 2 + TITLE_H),
                               p_data->title_strid, RSI_IGNORE);

  ctrl_set_keymap(p_cont, comm_menu_cont_keymap);
  ctrl_set_proc(p_cont, p_data->proc);

  // create mbox for items
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_MENU_MBOX,
                            MENU_ITEM_X, MENU_ITEM_Y, MENU_ITEM_W, height,
                            p_cont, 0);
  ctrl_set_keymap(p_mbox, comm_menu_mbox_keymap);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_mbox, 0, 0, MENU_ITEM_W, height);
  mbox_set_item_interval(p_mbox, 0, MENU_ITEM_V_GAP);
  mbox_set_item_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_mbox, FSI_BLACK, FSI_WHITE, FSI_WHITE);
  mbox_enable_icon_mode(p_mbox, FALSE);
  mbox_enable_string_mode(p_mbox, TRUE);

  mbox_set_count(p_mbox, p_data->item_cnt, 1, p_data->item_cnt);
  mbox_set_focus(p_mbox, 0);

  mbox_set_string_offset(p_mbox, 0, 0);
  mbox_set_string_align_type(p_mbox, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->item_strid[i]);
  }

  ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);

  if(para == 0)
  {
    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  }

  return p_cont;
}

#ifdef ENABLE_NETWORK
void ui_comm_help_set_font(control_t *p_cont,
                                 u32 n_fstyle,
                                 u32 h_fstyle,
                                 u32 g_fstyle)
{
#ifdef EN_TITLE_HELP
  control_t *p_mbox;

  if (p_cont == NULL)
  {
    return;
  }
  p_mbox = ctrl_get_child_by_id(p_cont, IDC_COMM_HELP_MBOX);
  mbox_set_string_fstyle(p_mbox, n_fstyle, h_fstyle, g_fstyle);
#endif
}

void ui_comm_title_create_net(control_t *parent, u16 icon_id, u16 strid, u8 rstyle)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_txt, *p_first, *p_icon;
  MT_ASSERT(parent != NULL);

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_TITLE_CONT,
                            0, 0, NET_TITLE_W, NET_TITLE_H,
                            NULL, 0);
  ctrl_set_rstyle(p_cont, rstyle, rstyle, rstyle);

  //creat bmp icon
  p_icon = ctrl_create_ctrl(CTRL_BMAP,
                              IDC_COMM_TITLE_IC,
                              NET_TITLE_ICON_X,
                              NET_TITLE_ICON_Y,
                              NET_TITLE_ICON_W,
                              NET_TITLE_ICON_H,
                              p_cont,
                              0);
  bmap_set_content_by_id(p_icon, icon_id);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_TITLE_TXT,
                           NET_TITLE_TXT_X, NET_TITLE_TXT_Y, 
                           NET_TITLE_TXT_W, NET_TITLE_TXT_H,
                           p_cont, 0);
  text_set_align_type(p_txt, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_txt, FSI_NET_TITLE_TXT, FSI_NET_TITLE_TXT, FSI_NET_TITLE_TXT);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, strid);

  // insert to parent
  p_first = parent->p_child;

  if(p_first != NULL)
  {
    p_first->p_prev = p_cont;
    p_cont->p_next = p_first;
  }
  parent->p_child = p_cont;
  p_cont->p_parent = parent;
#endif
}

void ui_comm_help_create_net(comm_help_data_t *p_data, control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  u16 w, h;
  u8 i, row;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  w = ctrl_get_width(p_parent) - 2 * HELP_CONT_H_GAP;
  h = (HELP_ITEM_H_NET + HELP_ITEM_V_GAP_NET) * row - HELP_ITEM_V_GAP_NET;

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            HELP_CONT_H_GAP,
                            (u16)(ctrl_get_height(p_parent) - h -
                                  HELP_CONT_V_GAP),
                            w, h, p_parent, 0);

  ctrl_set_rstyle(p_cont, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  w = w - 2 * HELP_ITEM_H_GAP_NET, h = h - 2 * HELP_ITEM_V_GAP_NET;
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_HELP_MBOX,
                            HELP_ITEM_H_GAP_NET, HELP_ITEM_V_GAP_NET, w, h,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  ctrl_set_mrect(p_mbox, 0, 0, w, h);
  mbox_set_item_interval(p_mbox, 0, 0);
  mbox_set_item_rstyle(p_mbox,
                       RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_mbox, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_mbox, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_mbox, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_mbox, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }
#endif
}

control_t *ui_comm_root_create_netmenu(u8 root_id,
                               u8 parent_root,
                               u16 title_icon,
                               u16 title_strid)
{
control_t *p_mask, *p_cont;

  p_mask = fw_create_mainwin(root_id,
                             0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                             parent_root, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_mask != NULL);
  ctrl_set_rstyle(p_mask, RSI_TRANSPARENT,
                  RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_proc(p_mask, comm_mask_proc);

  if(root_id == ROOT_ID_EPG
    || root_id == ROOT_ID_VEPG
     /*|| root_id == ROOT_ID_EPG_FIND
   || root_id == ROOT_ID_SCHEDULE*/)
  {
    ui_comm_set_epg_root(root_id);
  }

  // create container
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_ROOT_CONT,
                            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, p_mask, 0);
  ctrl_set_rstyle(p_cont, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);
  ui_comm_title_create_net(p_cont, title_icon, title_strid, RSI_COMMON_TITLE);

  return p_cont;
}

u8 ui_comm_get_focus_mainwin_id(void)
{
  u8 root_id;
  root_id = fw_get_focus_id();
  if (root_id != ROOT_ID_POPUP)
  {
    return root_id;
  }
  return fw_get_first_active_id();
}
#endif

BEGIN_KEYMAP(ui_comm_root_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(ui_comm_root_keymap, NULL)

BEGIN_MSGPROC(comm_mask_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SAVE, on_save)
  ON_COMMAND(MSG_DESTROY, on_destory)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_signal_update)
END_MSGPROC(comm_mask_proc, ui_comm_root_proc)

BEGIN_MSGPROC(ui_comm_root_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_menu)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_all)
END_MSGPROC(ui_comm_root_proc, cont_class_proc)

BEGIN_KEYMAP(comm_menu_mbox_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(comm_menu_mbox_keymap, NULL)

BEGIN_KEYMAP(comm_menu_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(comm_menu_cont_keymap, ui_comm_root_keymap)
