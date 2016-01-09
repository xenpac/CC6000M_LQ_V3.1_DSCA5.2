/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ts_ota_search.h"

#if 0//#ifdef OTA_SUPPORT
#ifdef DSMCC_OTA_ENABLE   

ota_info_t upgrade;

u16 ts_ota_search_keymap(u16 key);
RET_CODE ts_ota_search_proc(control_t * p_cont, u16 msg, u32 para1, u32 para2);

static RET_CODE on_ts_ota_search_exit(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  manage_close_menu(ROOT_ID_TS_OTA_SEARCH, 0, 0);
  ui_ts_ota_stop_search(FALSE);
    
  return SUCCESS;
}

static RET_CODE on_ts_ota_search_found(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  comm_dlg_data_t ota_choose =
  {
    ROOT_ID_TS_OTA_SEARCH,
    DLG_FOR_ASK | DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
    0,
    0,
  }; 
  u16 uni_str[32];
  
  str_asc2uni("OTA found, upgrade now?", uni_str);
  ota_choose.content = (u32)uni_str;
  
  if(ui_comm_dlg_open(&ota_choose) == DLG_RET_YES)//start
  {
    ui_ts_ota_stop_search(TRUE); 
  }
  else
  {
    ui_ts_ota_stop_search(FALSE); 
    manage_close_menu(ROOT_ID_TS_OTA_SEARCH, 0, 0);
  }
    
  return SUCCESS;
}

static RET_CODE on_ts_ota_search_save(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  ui_ts_ota_stop_search(TRUE);

  return SUCCESS;
}

static RET_CODE on_ts_ota_search_reset(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  #ifndef WIN32
  hal_pm_reset();
  #endif

  return SUCCESS;
}

static RET_CODE on_ts_ota_status(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  control_t *p_item[TS_OTA_ITEM_CNT];
  u32 str_char[TS_OTA_ITEM_CNT];
  u8 i, asc_str[64];

  memcpy(asc_str, (u8 *)para1, 63);
  asc_str[63] = 0;

  for (i = 0; i < TS_OTA_ITEM_CNT; i++)
  {
    p_item[i] = ctrl_get_child_by_id(p_cont, (u8)(i + 1));
    str_char[i] = text_get_content(p_item[i]);
  }
  
  for (i = 0; i < (TS_OTA_ITEM_CNT - 1); i++)
  {
    text_set_content_by_unistr(p_item[i], (u16 *)str_char[i + 1]);
  }

  text_set_content_by_ascstr(p_item[TS_OTA_ITEM_CNT - 1], asc_str);



  for (i = 0; i < TS_OTA_ITEM_CNT; i++)
  {
    ctrl_paint_ctrl(p_item[i], TRUE);
  }

  return SUCCESS;
}

RET_CODE open_ts_ota_search(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_item[TS_OTA_ITEM_CNT];
  u16 i, y;
  
  memcpy((void *)&upgrade, (void *)para2, sizeof(ota_info_t));
  
	p_cont = ui_comm_root_create(ROOT_ID_TS_OTA_SEARCH, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, IM_TITLEICON_TV, IDS_UPGRADE_BY_SAT);
  ctrl_set_keymap(p_cont, ts_ota_search_keymap);
  ctrl_set_proc(p_cont, ts_ota_search_proc);

  y = TS_OTA_SEARCH_ITEM_Y;

  for(i = 0; i < TS_OTA_ITEM_CNT; i++)
  {
    p_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1),
     TS_OTA_SEARCH_ITEM_X, y,
     TS_OTA_SEARCH_ITEM_W, TS_OTA_SEARCH_ITEM_H, p_cont, 0);
    ctrl_set_rstyle(p_item[i], RSI_ITEM_1_SH, RSI_ITEM_1_SH, RSI_ITEM_1_SH);
    text_set_align_type(p_item[i], STL_CENTER | STL_VCENTER);
    text_set_font_style(p_item[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_item[i], TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_item[i], "    ");
    y += (TS_OTA_SEARCH_ITEM_H + TS_OTA_SEARCH_ITEM_VGAP);
  }

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);

  ui_ts_ota_start_search((u32)&upgrade);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(ts_ota_search_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ts_ota_search_keymap, NULL)

BEGIN_MSGPROC(ts_ota_search_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ts_ota_search_exit)
  ON_COMMAND(MSG_OTA_FOUND, on_ts_ota_search_found)
  ON_COMMAND(MSG_SAVE, on_ts_ota_search_save)
  ON_COMMAND(MSG_OTA_TRIGGER_RESET, on_ts_ota_search_reset)
  ON_COMMAND(MSG_TS_OTA_STATUS, on_ts_ota_status)
END_MSGPROC(ts_ota_search_proc, cont_class_proc)

#endif
#endif

