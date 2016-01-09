/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_sleep_hotkey.h"

/* others */
enum control_id
{
  IDC_INVALID = 0,
  IDC_SLEEP_HKEY_SWITCH,
};

RET_CODE hkey_sleep_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


void sleep_hotkey_set_content(u8 focus)
{
  u8 sleep_str[SLEEP_CNT][7] = {"Off", "10min", "30min", "60min", "90min", "120min"};  
  control_t *p_cont = NULL, *p_switch = NULL;
  osd_set_t osd_set = {0};

  MT_ASSERT(focus < SLEEP_CNT);

  sys_status_get_osd_set(&osd_set);
  
  p_cont = fw_find_root_by_id(ROOT_ID_SLEEP_HOTKEY);
  if(p_cont != NULL)
  {
    p_switch = ctrl_get_child_by_id(p_cont, IDC_SLEEP_HKEY_SWITCH);
    
    text_set_content_type(p_switch, TEXT_STRTYPE_UNICODE);  
    text_set_content_by_ascstr(p_switch, sleep_str[focus]);
    ctrl_paint_ctrl(p_switch, TRUE);

    sys_status_get_osd_set(&osd_set);

    fw_tmr_reset(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT, osd_set.timeout * 1000);
  }
  else
  {
    open_sleep_hotkey(focus, 0);

    fw_tmr_create(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT, osd_set.timeout * 1000, FALSE);

  }
}

RET_CODE open_sleep_hotkey(u32 para1, u32 para2)
{
  control_t *p_cont= NULL, *p_switch = NULL;
  //u8 sleep_str[SLEEP_CNT][4] = {"Off", "10", "30", "60", "90", "120"};  
  u8  sleep_str[SLEEP_CNT][7] = {"Off", "10min", "30min", "60min", "90min", "120min"};  
  /* pre-create */
  p_cont = fw_create_mainwin(ROOT_ID_SLEEP_HOTKEY, 
                        SLEEP_HOTKEY_CONT_X, SLEEP_HOTKEY_CONT_Y, 
                        SLEEP_HOTKEY_CONT_W, SLEEP_HOTKEY_CONT_H,
                        0, 0, OBJ_ATTR_INACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_cont, hkey_sleep_proc);
  
  /*time set*/
  p_switch = ctrl_create_ctrl(CTRL_TEXT, IDC_SLEEP_HKEY_SWITCH,
    SLEEP_HOTKEY_ITEM_X, SLEEP_HOTKEY_ITEM_Y, SLEEP_HOTKEY_ITEM_W, SLEEP_HOTKEY_ITEM_H,
    p_cont, 0);
  ctrl_set_rstyle(p_switch, RSI_ITEM_6_HL, RSI_ITEM_6_HL, RSI_ITEM_6_HL);
  text_set_content_type(p_switch, TEXT_STRTYPE_UNICODE);
  
  MT_ASSERT(para1 < SLEEP_CNT);
  text_set_content_by_ascstr(p_switch, sleep_str[para1]);  

  ctrl_default_proc(p_switch, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_hkey_sleep_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  fw_destroy_mainwin_by_id(ROOT_ID_SLEEP_HOTKEY);
  
  fw_tmr_destroy(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT);
  return SUCCESS;
}


BEGIN_MSGPROC(hkey_sleep_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_hkey_sleep_exit)
END_MSGPROC(hkey_sleep_proc, cont_class_proc)
