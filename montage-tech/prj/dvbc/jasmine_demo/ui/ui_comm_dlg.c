/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/

#include "ui_common.h"
#include "ui_mute.h"
#include "iconv_ext.h"

enum comm_dlg_idc
{
  IDC_BTN_YES = 1,
  IDC_BTN_NO,
  IDC_CONTENT
};

enum comm_dlg_load_idc
{
    IDC_COMM_LOAD_CONT = 0x3c,
    IDC_COMM_LOAD_BMP,
    IDC_COMM_LOAD_TEXT,
};

enum comm_dlg_msg
{
  MSG_DLG_TIME_OUT = MSG_LOCAL_BEGIN + 550
};

static dlg_ret_t g_dlg_ret;
static control_t *gp_load_cont;

u16 comm_dlg_keymap(u16 key);

RET_CODE comm_dlg_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#define DO_DLG_FUNC(func) \
  if (func != NULL) (*func)(); 

#define DO_DLG_FUNC_WITHPARA(func, para1, para2) \
  if (func != NULL) (*func)(para1, para2); 
extern BOOL ui_is_desktop_inited(void);

dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data)
{
  control_t *p_cont;
  control_t *p_btn, *p_txt;
  u16 i, cnt, x, y;
  u8 type, mode;
  
  u16 btn[2] = { IDS_YES, IDS_NO };
  dlg_ret_t ret, last_ret;
  if(TRUE != ui_is_desktop_inited())
  {
	  OS_PRINTF("TRUE != ui_is_desktop_inited\n");
	  return ERR_FAILURE;		
  }

  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) // already opened
  {
    UI_PRINTF("UI: already open a dialog, force close it! \n");
    ui_comm_dlg_close();
  }

  last_ret = g_dlg_ret;
  p_cont = p_btn = p_txt = NULL;
  x = y = cnt = 0;

  type = p_data->style & 0x0F;
  mode = p_data->style & 0xF0;

  // create root at first
  p_cont = ctrl_create_ctrl(CTRL_CONT, ROOT_ID_POPUP,
                            p_data->x, p_data->y, p_data->w, p_data->h,
                            NULL, 0);
  ctrl_set_attr(p_cont, (obj_attr_t)(type == \
                             DLG_FOR_SHOW ? OBJ_ATTR_INACTIVE : OBJ_ATTR_ACTIVE));
  ctrl_set_rstyle(p_cont,
                  RSI_COMMAN_BG,
                  RSI_COMMAN_BG,
                  RSI_COMMAN_BG);

  ctrl_set_keymap(p_cont, comm_dlg_keymap);
  ctrl_set_proc(p_cont, comm_dlg_proc);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           DLG_CONTENT_GAP, DLG_CONTENT_GAP,
                           (u16)(p_data->w - 2 * DLG_CONTENT_GAP),
                           (u16)(p_data->h - 2 * DLG_CONTENT_GAP - 24), //bug fixed 9878 no see font 
                           p_cont, 0);

  text_set_font_style(p_txt, FSI_DLG_CONTENT, FSI_DLG_CONTENT, FSI_DLG_CONTENT);
  if (mode == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_line_gap(p_txt, 6);
    text_set_content_by_strid(p_txt, (u16)p_data->content);
  }
  else
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
    text_set_line_gap(p_txt, 6);
    text_set_content_by_unistr(p_txt, (u16*)p_data->content);
  }

  switch (type)
  {
    case DLG_FOR_ASK:
      x = p_data->w / 3 - DLG_BTN_W / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 2;
      break;
    case DLG_FOR_CONFIRM:
      x = (p_data->w - DLG_BTN_W) / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 1;
      break;
    case DLG_FOR_SHOW:
      cnt = 0;
      break;
    default:
      MT_ASSERT(0);
  }

  for (i = 0; i < cnt; i++)
  {
    p_btn = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_BTN_YES + i),
                             x, y, DLG_BTN_W, DLG_BTN_H, p_cont, 0);
    ctrl_set_rstyle(p_btn,
                    RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
    text_set_font_style(p_btn,
                        FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
    text_set_content_type(p_btn, TEXT_STRTYPE_STRID);

    if(p_data->specify_bnt == TRUE)
    {
      text_set_content_by_strid(p_btn, p_data->specify_bnt_cont[i]);
    }
    else
    {
      text_set_content_by_strid(p_btn,
                                (u16)(cnt == 1 ? IDS_OK : btn[i]));
    }
    if(cnt != 0)
    {
      ctrl_set_related_id(p_btn,
                          (u8)((i - 1 + cnt) % cnt + 1), /* left */
                          0,                             /* up */
                          (u8)((i + 1) % cnt + 1),       /* right */
                          0);                            /* down */
    }
    x += p_data->w / 3;
  }

  // attach the root onto mainwin
  OS_PRINTF("p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
  fw_attach_root(p_cont, p_data->parent_root);

  // paint
  if (cnt > 0)
  {
#if defined(CUSTOMER_JIESAI_WUNING) || defined(CUSTOMER_JIZHONG_ANXIN)
  	p_btn = ctrl_get_child_by_id(p_cont, IDC_BTN_YES);
#endif
    ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
  }

  
  ctrl_paint_ctrl(p_cont, FALSE);

  if(p_data->dlg_tmout != 0)
  {
    ret = (dlg_ret_t)fw_tmr_create(ROOT_ID_POPUP, MSG_CANCEL, p_data->dlg_tmout, FALSE);

    MT_ASSERT(ret == SUCCESS);
  }

  // start loop to get msg
  if (type == DLG_FOR_SHOW)
  {
    ret = DLG_RET_NULL;
  }
  else
  {
    fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
  }

  g_dlg_ret = last_ret;

  return ret;
}


dlg_ret_t ui_comm_dlg_open2(comm_dlg_data_t *p_data)
{
  control_t *p_cont;
  control_t *p_btn, *p_txt;
  u16 i, cnt, x, y;
  u8 type, mode;
  u16 btn[2] = { IDS_YES, IDS_NO };
  dlg_ret_t ret;

  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) // already opened
  {
    UI_PRINTF("UI: already open a dialog, force close it! \n");
    ui_comm_dlg_close();
  }

  p_cont = p_btn = p_txt = NULL;
  x = y = cnt = 0;

  type = p_data->style & 0x0F;
  mode = p_data->style & 0xF0;

  // create root at first
  p_cont = ctrl_create_ctrl(CTRL_CONT, ROOT_ID_POPUP,
                            p_data->x, p_data->y, p_data->w, p_data->h,
                            NULL, 0);
  ctrl_set_attr(p_cont, (obj_attr_t)(type == \
                             DLG_FOR_SHOW ? OBJ_ATTR_INACTIVE : OBJ_ATTR_ACTIVE));
  ctrl_set_rstyle(p_cont,
                  RSI_COMMAN_BG,
                  RSI_COMMAN_BG,
                  RSI_COMMAN_BG);

  ctrl_set_keymap(p_cont, comm_dlg_keymap);
  ctrl_set_proc(p_cont, comm_dlg_proc);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           DLG_CONTENT_GAP, DLG_CONTENT_GAP,
                           (u16)(p_data->w - 2 * DLG_CONTENT_GAP),
                           (u16)(p_data->h - 2 * DLG_CONTENT_GAP - DLG_BTN_H),
                           p_cont, 0);

  text_set_font_style(p_txt, FSI_DLG_CONTENT, FSI_DLG_CONTENT, FSI_DLG_CONTENT);
  if (mode == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt, (u16)p_data->content);
  }
  else
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_txt, (u16*)p_data->content);
  }

  switch (type)
  {
    case DLG_FOR_ASK:
      x = p_data->w / 3 - DLG_BTN_W / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 2;
      break;
    case DLG_FOR_CONFIRM:
      x = (p_data->w - DLG_BTN_W) / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 1;
      break;
    case DLG_FOR_SHOW:
      cnt = 0;
      break;
    default:
      MT_ASSERT(0);
  }

  for (i = 0; i < cnt; i++)
  {
    p_btn = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_BTN_YES + i),
                             x, y, DLG_BTN_W, DLG_BTN_H, p_cont, 0);
    ctrl_set_rstyle(p_btn,
                    RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
    text_set_font_style(p_btn,
                        FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
    text_set_content_type(p_btn, TEXT_STRTYPE_STRID);

    if(p_data->specify_bnt == TRUE)
    {
      text_set_content_by_strid(p_btn, p_data->specify_bnt_cont[i]);
    }
    else
    {
      text_set_content_by_strid(p_btn,
                                (u16)(cnt == 1 ? IDS_OK : btn[i]));
    }
    if(cnt != 0)
    {
      ctrl_set_related_id(p_btn,
                          (u8)((i - 1 + cnt) % cnt + 1), /* left */
                          0,                             /* up */
                          (u8)((i + 1) % cnt + 1),       /* right */
                          0);                            /* down */
    }
    x += p_data->w / 3;
  }

  // attach the root onto mainwin
  OS_PRINTF("p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
  fw_attach_root(p_cont, p_data->parent_root);

  // paint
  if (cnt > 0)
  {
    p_btn = ctrl_get_child_by_id(p_cont,IDC_BTN_YES);
    ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
  }
  ctrl_paint_ctrl(p_cont, FALSE);

  if(p_data->dlg_tmout != 0)
  {
    ret = (dlg_ret_t)fw_tmr_create(ROOT_ID_POPUP, MSG_DLG_TIME_OUT, p_data->dlg_tmout, FALSE);

    MT_ASSERT(ret == SUCCESS);
  }

  // start loop to get msg
  if (type == DLG_FOR_SHOW)
  {
    //ret = DLG_RET_NULL;
    fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
  }
  else
  {
    fw_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
  }

  return ret;
}


void ui_comm_dlg_close(void)
{
  u8 index;
  BOOL is_check_mute = FALSE;
     
  fw_tmr_destroy(ROOT_ID_POPUP, MSG_CANCEL);
  manage_close_menu(ROOT_ID_POPUP, 0, 0);
  
  index = fw_get_focus_id();
  if(index == ROOT_ID_PROG_LIST
    || index == ROOT_ID_EPG
    || ui_is_fullscreen_menu(index))
  {
    is_check_mute = TRUE; //fix bug 18847
  }

  if(ui_is_mute() && is_check_mute)
  {
    open_mute(0, 0);
  }
}


static void fill_dlg_data(comm_dlg_data_t *p_data, u8 style, u32 strid,
                          rect_t *p_dlg_rc, u32 tm_out)
{
  memset(p_data, 0, sizeof(comm_dlg_data_t));
  
  p_data->parent_root = 0;
  p_data->style = style;
  p_data->content = strid;
  p_data->dlg_tmout = tm_out;

  if (p_dlg_rc != NULL)
  {
    p_data->x = (u16)p_dlg_rc->left;
    p_data->y = (u16)p_dlg_rc->top;
    p_data->w = (u16)RECTWP(p_dlg_rc);
    p_data->h = (u16)RECTHP(p_dlg_rc);
  }
  else
  {
    p_data->x = COMM_DLG_X;
    p_data->y = COMM_DLG_Y;
    p_data->w = COMM_DLG_W;
    p_data->h = COMM_DLG_H;
  }
}


void ui_comm_showdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  
  fill_dlg_data(&dlg_data, DLG_FOR_SHOW | DLG_STR_MODE_STATIC, strid, p_dlg_rc, tm_out);
  ui_comm_dlg_open(&dlg_data);
  
  //DO_DLG_FUNC(do_cmf);
  //ui_comm_dlg_close();
}


void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  fill_dlg_data(&dlg_data,
                DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(do_cmf);
}

#ifdef ENABLE_NETWORK
void ui_comm_cfmdlg_open_unistr(rect_t *p_dlg_rc, u16* p_unistr, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;


  fill_dlg_data(&dlg_data,
                DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
                (u32)p_unistr, p_dlg_rc, tm_out);
  ui_comm_dlg_open(&dlg_data);
  DO_DLG_FUNC(do_cmf);
}

void ui_comm_ask_for_dodlg_open_xxx(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;

  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                (u32)strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC(do_func);
  }
  else if(ret == DLG_RET_NO)
  {
    DO_DLG_FUNC(undo_func);
  }
}


void ui_comm_ask_for_dodlg_open_unistr(rect_t *p_dlg_rc, u16 *p_unistr,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;

  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_EXTSTR,
                (u32)p_unistr, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC(do_func);
  }
  else if (ret == DLG_RET_NO)
  {
    DO_DLG_FUNC(undo_func);
  }
}

void ui_comm_dlg_close2(void)
{
  return ui_comm_dlg_close();

}
#endif

void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  fill_dlg_data(&dlg_data,
                DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
                IDS_MSG_SAVING, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(func);

  ui_comm_dlg_close();
}


void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;
  
  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC(do_func);
  }
  else
  {
    DO_DLG_FUNC(undo_func);
  }
}

void ui_comm_ask_for_dodlg_open_ex(rect_t *p_dlg_rc, u16 strid,
  do_func_withpara_t do_func, u32 para1, u32 para2, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;
  
  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC_WITHPARA(do_func,para1,para2);
  }
}



BOOL ui_comm_ask_for_savdlg_open(rect_t *p_dlg_rc,
                                   u16 strid,
                                   do_func_t do_save,
                                   do_func_t undo_save, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;
  
  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);
  if (ret == DLG_RET_YES)
  {
    ui_comm_savdlg_open(p_dlg_rc, do_save, 0);
     return TRUE;
  }
  else
  {
    DO_DLG_FUNC(undo_save);
    return FALSE;
  }
}

static RET_CODE on_dlg_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_dlg_close();
  return SUCCESS;
}


static RET_CODE on_dlg_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;
  
  p_btn = ctrl_get_child_by_id(p_ctrl, IDC_BTN_NO);

  if(p_btn != NULL)
  {
    ctrl_set_active_ctrl(p_ctrl, p_btn);
  }
  
  ui_comm_dlg_close();
  return SUCCESS;
}

static RET_CODE on_dlg_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
   //ctrl_set_active_ctrl(p_ctrl, NULL);  
  ui_comm_dlg_close();
  return SUCCESS;
}

static RET_CODE on_dlg_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;
  
  p_btn = ctrl_get_active_ctrl(p_ctrl);
  if (p_btn != NULL)
  {
    g_dlg_ret = ctrl_get_ctrl_id(p_btn) == IDC_BTN_YES ? \
      DLG_RET_YES : DLG_RET_NO;
  }
  else
  {
    g_dlg_ret = DLG_RET_NULL;
  }

  fw_tmr_destroy(ROOT_ID_POPUP, MSG_CANCEL);
  ctrl_get_proc(fw_get_focus())(fw_get_focus(), MSG_PAINT, TRUE, 0);
  return ERR_NOFEATURE;
}

extern iconv_t g_cd_gb2312_to_utf16le;


void gb2312_2_utf16le(char* inbuf, int in_len, char* outbuf, int out_len)
{
  char** p_inbuf = &inbuf;
  char** p_outbuf = &outbuf;

  iconv(g_cd_gb2312_to_utf16le, p_inbuf, (size_t *)&in_len, p_outbuf, (size_t *)&out_len);
}

void ui_comm_cfmdlg_open_gb(rect_t *p_dlg_rc, u8* p_gb, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  u16 uni_str[128] = {0};

  gb2312_2_utf16le((char *)p_gb, (int)strlen((char *)p_gb), (char*)uni_str, sizeof(uni_str)-1);
  fill_dlg_data(&dlg_data,
                DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
                (u32)uni_str, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(do_cmf);
}

void ui_comm_loading_create(control_t *p_mainwin)
{
    control_t *p_cont = NULL;
    control_t *p_sub = NULL;

    p_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_LOAD_CONT,
        COMM_LOAD_CONT_X, COMM_LOAD_CONT_Y, 
        COMM_LOAD_CONT_W, COMM_LOAD_CONT_H, 
    p_mainwin, 0);

    ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_sts(p_cont, OBJ_STS_HIDE);
    gp_load_cont = p_cont;

    p_sub = ctrl_create_ctrl(CTRL_BMAP, IDC_COMM_LOAD_BMP,
        COMM_LOAD_BMP_X, COMM_LOAD_BMP_Y,
        COMM_LOAD_BMP_W, COMM_LOAD_BMP_H, p_cont, 0);

    bmap_set_align_type(p_sub, STL_CENTER | STL_VCENTER);
    bmap_set_content_by_id(p_sub, IM_BLACK);

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_LOAD_TEXT,
        COMM_LOAD_TEXT_X, COMM_LOAD_TEXT_Y, 
        COMM_LOAD_TEXT_W, COMM_LOAD_TEXT_H, p_cont, 0);

    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_sub, FSI_GREEN, FSI_GREEN, FSI_GREEN);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);

}

void ui_comm_loading_show(load_type_t type, u32 time, u16 net_bps, u16 percent)
{
    control_t *p_cont = NULL;
    control_t *p_loading = NULL;
    control_t *p_loading_text = NULL;
    u8 asc_str[32] = {0};
    u16 bmp_id = 0;

    p_cont = gp_load_cont;
    MT_ASSERT(NULL != p_cont);
    ctrl_set_sts(p_cont, OBJ_STS_SHOW);
    p_loading = ctrl_get_child_by_id(p_cont, IDC_COMM_LOAD_BMP);
    bmp_id = bmap_get_content(p_loading);
    if(IM_NUMBER_9 == bmp_id)
    {
        bmp_id = IM_NUMBER_0;
    }
    else
    {
        bmp_id ++;
    }
    bmap_set_content_by_id(p_loading, bmp_id);
    
    
    if(type == LOAD_TIME)
    {
        sprintf(asc_str, "%d %s", (int)time, " s");
    }
    else if(type == LOAD_BPS)
    {
        sprintf(asc_str, " %d%s  %d%s", net_bps, "kb/s",percent, "\%");
    }
    
    p_loading_text = ctrl_get_child_by_id(p_cont, IDC_COMM_LOAD_TEXT);
    text_set_content_by_ascstr(p_loading_text, asc_str);
    ctrl_paint_ctrl(p_cont, TRUE);
}

void ui_comm_loading_hide(void)
{
    control_t *p_cont = NULL;

    p_cont = gp_load_cont;
    ctrl_set_sts(p_cont, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_cont);
}

BEGIN_KEYMAP(comm_dlg_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_EXIT, MSG_CANCEL)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(comm_dlg_keymap, NULL)


BEGIN_MSGPROC(comm_dlg_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_dlg_select)
  ON_COMMAND(MSG_CANCEL, on_dlg_cancel)
  ON_COMMAND(MSG_DLG_TIME_OUT, on_dlg_timeout)
  ON_COMMAND(MSG_DESTROY, on_dlg_destroy)
END_MSGPROC(comm_dlg_proc, cont_class_proc)


