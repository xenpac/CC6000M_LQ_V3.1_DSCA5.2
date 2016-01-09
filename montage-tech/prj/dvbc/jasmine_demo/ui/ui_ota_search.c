/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ota_search.h"

enum ota_search_collect_id
{
  IDC_COLLECT_INFO = 1,
  IDC_COLLECT_EXIT,
};

enum ota_search_check_id
{
  IDC_CHECK_LOCAL = 1,
  IDC_CHECK_SERVICE,
  IDC_CHECK_TXT,
  IDC_CHECK_YES,
  IDC_CHECK_NO,
};

enum ota_search_ctrl_id
{
  IDC_OTA_SEARCH_COLLECT = 1,
  IDC_OTA_SEARCH_CHECK,
};

ota_info_t ota_upgrade_information;
nim_info_t ota_nim_lock_info;

RET_CODE ota_search_proc(control_t * p_cont, u16 msg, u32 para1, u32 para2);
u16 ota_search_btn_keymap(u16 key);
RET_CODE ota_search_btn_proc(control_t * p_btn, u16 msg, u32 para1, u32 para2);
u16 ota_search_check_keymap(u16 key);


static ota_info_t *_ui_ota_search_v_get_private_data()
{
  return &ota_upgrade_information;
}

#ifndef DTMB_PROJECT
static void trans_ota_nim_info(void)
{
  memset(&ota_nim_lock_info, 0, sizeof(ota_nim_lock_info));
  
  ota_nim_lock_info.lock_mode = ota_upgrade_information.sys_mode;
  ota_nim_lock_info.data_pid = ota_upgrade_information.download_data_pid;
  memcpy(&(ota_nim_lock_info.lockc), &(ota_upgrade_information.lockc), sizeof(ota_upgrade_information.lockc));

  OS_PRINTF("%s(Line: %d): ota_tp = %d, ota_sym = %d, ota_pid_ %d \n ", 
                    __FUNCTION__, __LINE__,ota_nim_lock_info.lockc.tp_freq, ota_nim_lock_info.lockc.tp_sym,
                      ota_nim_lock_info.data_pid);
  return;
}
#else
static void trans_ota_nim_info(void)
{
  memset(&ota_nim_lock_info, 0, sizeof(ota_nim_lock_info));
  
  ota_nim_lock_info.lock_mode = ota_upgrade_information.sys_mode;
  ota_nim_lock_info.data_pid = ota_upgrade_information.download_data_pid;
  memcpy(&(ota_nim_lock_info.lockt), &(ota_upgrade_information.lockt), sizeof(ota_upgrade_information.lockt));

  OS_PRINTF("%s(Line: %d): lock_mode = %d, ota_tp = %d, ota_bind_width = %d, ota_pid_ %d \n ", 
                    __FUNCTION__, __LINE__,ota_nim_lock_info.lock_mode, ota_nim_lock_info.lockt.tp_freq, ota_nim_lock_info.lockt.band_width,
                      ota_nim_lock_info.data_pid);
  return;
}
#endif

static void _ui_ota_search_v_set_menu_state(control_t *p_cont, BOOL is_collect, BOOL is_paint, u32 para)
{
  control_t *p_collect, *p_check, *p_service, *p_local, *p_btn;
  upg_check_info_t *p_ota_info = NULL;
  u16 old_ver = 0;
  u16 new_ver = 0;
  if(para != 0)
  {
    p_ota_info = (upg_check_info_t *)para;
    old_ver = (u16)p_ota_info->upg_old_version;
    new_ver = (u16)p_ota_info->upg_new_version;
  }

  OS_PRINTF("%s(Line: %d): old_ver = %d, new_ver = %d, is_collect = %d.\n", __FUNCTION__, __LINE__, old_ver, new_ver, is_collect);
  p_collect = ctrl_get_child_by_id(p_cont, IDC_OTA_SEARCH_COLLECT);
  p_btn = ctrl_get_active_ctrl(p_collect);

  ctrl_set_sts(p_collect, is_collect ? OBJ_STS_SHOW : OBJ_STS_HIDE);
  ctrl_default_proc(p_btn, is_collect ? MSG_GETFOCUS : MSG_LOSTFOCUS, 0, 0);
  
  p_check = ctrl_get_child_by_id(p_cont, IDC_OTA_SEARCH_CHECK);
  p_service = ctrl_get_child_by_id(p_check, IDC_CHECK_SERVICE);
  p_local = ctrl_get_child_by_id(p_check, IDC_CHECK_LOCAL);
  p_btn = ctrl_get_active_ctrl(p_check);

  ctrl_set_sts(p_check, is_collect?OBJ_STS_HIDE : OBJ_STS_SHOW);
  ctrl_default_proc(p_btn, is_collect?MSG_LOSTFOCUS : MSG_GETFOCUS, 0, 0);

  if(!is_collect)
  {
    ui_comm_static_set_content_by_dec(p_service, new_ver);
    ui_comm_static_set_content_by_dec(p_local, old_ver);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_cont, TRUE);
  }

  return;
}

static RET_CODE _ui_ota_search_v_found(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(ROOT_ID_BACKGROUND == fw_get_focus_id())
  {
    return SUCCESS;
  }
  
  _ui_ota_search_v_set_menu_state(p_cont, FALSE, TRUE, para1);

  return SUCCESS;
}

static RET_CODE _ui_ota_search_timeout(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t timeout_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    IDS_OTA_TIMEOUT,
    0,
  };
  if(ROOT_ID_BACKGROUND == fw_get_focus_id())
  {
    return SUCCESS;
  }
  ui_comm_dlg_open(&timeout_data);
  manage_close_menu(ROOT_ID_OTA_SEARCH, 0, 0);

  //stop ota, bug 51183
  ui_ota_api_stop();

  return SUCCESS;
}

static void _ui_ota_search_v_get_system_ota_data(ota_info_t *p_otai)
{
  memcpy((u8 *)p_otai, (u8 *)sys_status_get_ota_info(), sizeof(ota_info_t));

  return;
}

#if 0
static void _ui_ota_search_v_set_system_ota_data(ota_info_t *p_otai)
{
 sys_status_set_ota_info(p_otai);
  sys_status_save();

  return;
}
#endif

static RET_CODE _ui_ota_search_v_select(control_t *p_btn, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_parent;
  u8 parent_id, btn_id;
  ota_info_t ota_info;

  p_parent = ctrl_get_parent(p_btn);
  parent_id = (u8)ctrl_get_ctrl_id(p_parent);
  btn_id = (u8)ctrl_get_ctrl_id(p_btn);
  _ui_ota_search_v_get_system_ota_data(&ota_info);

  if((parent_id == IDC_OTA_SEARCH_COLLECT) ||
     ((parent_id == IDC_OTA_SEARCH_CHECK) && (btn_id == IDC_CHECK_NO)))
  {
    /* exit */
    ret = manage_close_menu(ROOT_ID_OTA_SEARCH, 0, 0);
    mtos_task_delay_ms(200); 
    
    //stop ota, bug 51183
    ui_ota_api_stop();
   // ui_ota_api_stop_search(0,0);
    //ota_info.ota_tri = OTA_TRI_NONE;
    //_ui_ota_search_v_set_system_ota_data(&ota_info);
  }
  else if((parent_id == IDC_OTA_SEARCH_CHECK) && (btn_id == IDC_CHECK_YES))
  {
    //ota_info.ota_tri = OTA_TRI_AUTO;
    //_ui_ota_search_v_set_system_ota_data(&ota_info);
    ui_ota_api_manual_save_ota_info();
#ifndef WIN32
    mtos_task_delay_ms(100);
	hal_watchdog_init(0);
  	hal_watchdog_enable();

    //hal_pm_reset();
#endif
  }
  else
  {
    OS_PRINTF("%s(Line: %d): parent_id = %d, btn_id = %d.\n", __FUNCTION__, __LINE__, parent_id, btn_id);
    MT_ASSERT(0);
  }

  return ret;
}

RET_CODE open_ota_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_collect, *p_info, *p_exit;
  control_t *p_check, *p_local, *p_service, *p_txt, *p_yes, *p_no;
  u16 y;
  
  memcpy((void *)_ui_ota_search_v_get_private_data(), (void *)para2, sizeof(ota_info_t));
  p_cont = ui_comm_root_create(ROOT_ID_OTA_SEARCH, 0, 
                                COMM_BG_X, COMM_BG_Y, 
                                COMM_BG_W, COMM_BG_H, 
                                RSC_INVALID_ID, IDS_UPGRADE_BY_SAT);
  ctrl_set_proc(p_cont, ota_search_proc);

  /* collect information container */
  p_collect = ctrl_create_ctrl(CTRL_CONT, IDC_OTA_SEARCH_COLLECT, 
                                0, 0, 
                                OTA_SEARCH_CONT_W, OTA_SEARCH_CONT_H, 
                                p_cont, 0);
  y = OTA_SEARCH_ITEM_Y;
  p_info = ctrl_create_ctrl(CTRL_TEXT, IDC_COLLECT_INFO,
                              OTA_SEARCH_ITEM_X, y,
                              OTA_SEARCH_ITEM_W, OTA_SEARCH_ITEM_H,
                              p_collect, 0);
  text_set_align_type(p_info, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_info, FSI_OTA_SEARCH_INFO, FSI_OTA_SEARCH_INFO, FSI_OTA_SEARCH_INFO);
  text_set_content_type(p_info, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_info, IDS_MSG_COLLECTING);  
  
  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);
  p_exit = ctrl_create_ctrl(CTRL_TEXT, IDC_COLLECT_EXIT,
                              OTA_COLLECT_EXIT_X, y,
                              OTA_COLLECT_EXIT_W, OTA_COLLECT_EXIT_H,
                              p_collect, 0);
  ctrl_set_keymap(p_exit, ota_search_btn_keymap);
  ctrl_set_proc(p_exit, ota_search_btn_proc);
  ctrl_set_rstyle(p_exit, RSI_OTA_SEARCH_BTN_SH, RSI_OTA_SEARCH_BTN_HL, RSI_OTA_SEARCH_BTN_SH);
  text_set_font_style(p_exit, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN);
  text_set_content_type(p_exit, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_exit, IDS_STOP);  
  ctrl_set_active_ctrl(p_collect, p_exit);

  /* check container */
  p_check = ctrl_create_ctrl(CTRL_CONT, IDC_OTA_SEARCH_CHECK, 
                              0, 0,
                              OTA_SEARCH_CONT_W, OTA_SEARCH_CONT_H,
                              p_cont, 0);
  ctrl_set_keymap(p_check, ota_search_check_keymap);

  y = OTA_SEARCH_ITEM_Y;
  p_local = ui_comm_static_create(p_check, IDC_CHECK_LOCAL, 
                                    OTA_SEARCH_ITEM_X, y,
                                    OTA_SEARCH_ITEM_LW, OTA_SEARCH_ITEM_RW);
  ui_comm_static_set_param(p_local, TEXT_STRTYPE_DEC);
  ui_comm_static_set_static_txt(p_local, IDS_LOCAL_VERSION);
  
  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);
  p_service = ui_comm_static_create(p_check, IDC_CHECK_SERVICE,
                                      OTA_SEARCH_ITEM_X, y,
                                      OTA_SEARCH_ITEM_LW, OTA_SEARCH_ITEM_RW);
  ui_comm_static_set_param(p_service, TEXT_STRTYPE_DEC);
  ui_comm_static_set_static_txt(p_service, IDS_SERVICE_VERSION);  
  
  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);
  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CHECK_TXT,
                            OTA_SEARCH_ITEM_X, y,
                            OTA_SEARCH_ITEM_W, OTA_SEARCH_ITEM_H,
                            p_check, 0);
  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_OTA_SEARCH_INFO, FSI_OTA_SEARCH_INFO, FSI_OTA_SEARCH_INFO);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, IDS_MSG_ASK_FOR_UPGRADE);

  y += (OTA_SEARCH_ITEM_H + OTA_SEARCH_ITEM_VGAP);
  p_yes = ctrl_create_ctrl(CTRL_TEXT, IDC_CHECK_YES,
                            OTA_CHECK_YES_X, y,
                            OTA_CHECK_YES_W, OTA_CHECK_YES_H,
                            p_check, 0);
  ctrl_set_keymap(p_yes, ota_search_btn_keymap);
  ctrl_set_proc(p_yes, ota_search_btn_proc);  
  ctrl_set_rstyle(p_yes, RSI_OTA_SEARCH_BTN_SH, RSI_OTA_SEARCH_BTN_HL, RSI_OTA_SEARCH_BTN_SH);
  text_set_font_style(p_yes, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN);
  text_set_content_type(p_yes, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_yes, IDS_YES);  

  p_no = ctrl_create_ctrl(CTRL_TEXT, IDC_CHECK_NO,
                            OTA_CHECK_NO_X, y,
                            OTA_CHECK_NO_W, OTA_CHECK_NO_H,
                            p_check, 0);
  ctrl_set_keymap(p_no, ota_search_btn_keymap);
  ctrl_set_proc(p_no, ota_search_btn_proc);  
  ctrl_set_rstyle(p_no, RSI_OTA_SEARCH_BTN_SH, RSI_OTA_SEARCH_BTN_HL, RSI_OTA_SEARCH_BTN_SH);
  text_set_font_style(p_no, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN, FSI_OTA_SEARCH_BTN);
  text_set_content_type(p_no, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_no, IDS_NO);    
  ctrl_set_related_id(p_yes, IDC_CHECK_NO, 0, IDC_CHECK_NO, 0);
  ctrl_set_related_id(p_no, IDC_CHECK_YES, 0, IDC_CHECK_YES, 0);
  ctrl_set_active_ctrl(p_check, p_yes);

  _ui_ota_search_v_set_menu_state(p_cont, TRUE, FALSE, 0);
  sys_status_set_ota_info(_ui_ota_search_v_get_private_data());
  trans_ota_nim_info();
  ui_ota_api_manual_check_mode(&ota_nim_lock_info);
 // ui_ota_api_start_search((u32)_ui_ota_search_v_get_private_data(), TRUE);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_MSGPROC(ota_search_proc, cont_class_proc)
  //ON_COMMAND(MSG_OTA_FOUND, _ui_ota_search_v_found)
  ON_COMMAND(MSG_OTA_TRIGGER_RESET, _ui_ota_search_v_found)
  ON_COMMAND(MSG_OTA_TMOUT, _ui_ota_search_timeout)
END_MSGPROC(ota_search_proc, cont_class_proc)

BEGIN_KEYMAP(ota_search_btn_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ota_search_btn_keymap, NULL)

BEGIN_MSGPROC(ota_search_btn_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, _ui_ota_search_v_select)
END_MSGPROC(ota_search_btn_proc, text_class_proc)

BEGIN_KEYMAP(ota_search_check_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(ota_search_check_keymap, NULL)

