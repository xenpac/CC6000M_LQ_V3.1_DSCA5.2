/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_keyboard_v2.h"
#include "ui_mac_setting.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_MAC_SETTING,
  IDC_MAC_SAVE
};

static u8 g_mac_addr[32];

static u16 mac_setting_cont_keymap(u16 key);
static RET_CODE mac_setting_save_text_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE mac_setting_edit_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE mac_setting_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

void get_current_mac_addr(u8 *p_mac)
{
    u8 temp_mac[32] = {0};
    u16 i;
    u8 mac_map[3] = {0};
    
    for(i = 0; i < 6; i ++)  
    {
        sys_status_get_mac(i, mac_map);
        strcat(temp_mac,mac_map); 
    }
    temp_mac[12] = '\0';
    memcpy(p_mac, temp_mac, sizeof(temp_mac) - 1);
    OS_PRINTF("##%s, mac addr = [%s]##\n", __FUNCTION__, p_mac);
}

static control_t *create_mac_setting_menu()
{
  control_t *p_cont = NULL;
  control_t *p_mac_addr = NULL;
  control_t *p_save = NULL;
  u8 mac_addr[32] = {0};
  const char *save_msg = "STB will be restarted.";
  //const char *mac_msg = "MAC addr(like: 192A3B4C5D6F):";
  
  /*create the main window*/
  p_cont = ui_comm_root_create_netmenu(ROOT_ID_MAC_SETTING, 0, IM_INDEX_SYSTEMDETAIL_BANNER, IDS_MAC_ADDR);
  ctrl_set_keymap(p_cont, mac_setting_cont_keymap);
  ctrl_set_proc(p_cont, mac_setting_select_proc);
  
  /*create mac_addr*/  
  get_current_mac_addr(mac_addr);
  p_mac_addr = ui_comm_t9_v2_edit_create(p_cont, IDC_MAC_SETTING, MAC_SETTING_ITEM_X, MAC_SETTING_ITEM_Y,
					 MAC_SETTING_ITEM_LW, MAC_SETTING_ITEM_RW, ROOT_ID_MAC_SETTING);
  ui_comm_t9_v2_edit_set_static_txt(p_mac_addr, IDS_MAC_ADDR);
  ui_comm_t9_v2_edit_set_content_by_ascstr(p_mac_addr, (u8*)mac_addr);
  ui_comm_ctrl_set_proc(p_mac_addr, mac_setting_edit_proc);

  /*Save */
  p_save = ui_comm_static_create(p_cont, IDC_MAC_SAVE, MAC_SETTING_ITEM_X, MAC_SETTING_ITEM_Y+MAC_SETTING_ITEM_H*2,
                          MAC_SETTING_ITEM_LW, MAC_SETTING_ITEM_RW);
  ui_comm_static_set_static_txt(p_save, IDS_SAVE);
  ui_comm_static_set_param(p_save, TEXT_STRTYPE_UNICODE);
  ui_comm_static_set_content_by_ascstr(p_save, (u8*)save_msg);
  ui_comm_ctrl_set_proc(p_save, mac_setting_save_text_proc);  

  ctrl_set_related_id(p_mac_addr, 0, IDC_MAC_SAVE, 0, IDC_MAC_SAVE);
  ctrl_set_related_id(p_save, 0, IDC_MAC_SETTING, 0, IDC_MAC_SETTING);

  /*set the name focus*/
  ctrl_default_proc(p_mac_addr, MSG_GETFOCUS, 0, 0); /* focus on mac_addr */
  return p_cont;
}


/*the exit function*/
static RET_CODE on_mac_setting_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_MAC_SETTING, 0, 0);
  return SUCCESS;
}

RET_CODE edit_mac_addr_update(u16 *p_unistr)
{    
  control_t *p_edit_cont = NULL;

  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_MAC_SETTING, IDC_MAC_SETTING);
  MT_ASSERT(p_edit_cont != NULL);

  str_uni2asc(g_mac_addr, p_unistr);

  ui_comm_t9_v2_edit_set_content_by_ascstr(p_edit_cont, g_mac_addr);
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  return SUCCESS;
}


static RET_CODE on_edit_mac_addr(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
    kb_param_t param;  
    
    param.uni_str = ebox_get_content(p_ctrl);
    param.type = KB_INPUT_TYPE_HEX_NUMERIC;
    param.max_len = 12;
    param.cb = edit_mac_addr_update;
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    return SUCCESS;
}

RET_CODE open_mac_setting(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;

  p_menu = create_mac_setting_menu();
  MT_ASSERT(p_menu != NULL);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  return SUCCESS;
}

static void save_mac_addr(void)
{
  u8 i;
  comm_dlg_data_t p_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W+ 40,COMM_DLG_H,
    IDS_MSG_SAVING,
    0 ,
  };
  control_t *p_edit_cont = NULL;

  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_MAC_SETTING, IDC_MAC_SETTING);
  
  MT_ASSERT(p_edit_cont != NULL);
  if(strlen(g_mac_addr) < 12)
  {
    ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
    return ;
  }

  for (i=0; i<6; i++)
  {
    OS_PRINTF("%c%c", g_mac_addr[2*i], g_mac_addr[2*i+1]);
    sys_status_set_mac(i, g_mac_addr + 2*i);
  }

  sys_bg_data_save();
  OS_PRINTF("\n");

  ui_comm_dlg_open(&p_data);
  mtos_task_delay_ms(3000);
#ifndef WIN32
  hal_pm_reset();
#endif
}

static RET_CODE on_mac_setting_save(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  ui_comm_ask_for_savdlg_open(NULL, IDS_MSG_ASK_FOR_SAV,
						   (do_func_t)save_mac_addr, (do_func_t)NULL, 0);

  return SUCCESS;
}

BEGIN_KEYMAP(mac_setting_cont_keymap, NULL)
  ON_EVENT(V_KEY_MENU,MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL,MSG_CANCEL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(mac_setting_cont_keymap, NULL)

BEGIN_MSGPROC(mac_setting_select_proc, ui_comm_root_proc)
ON_COMMAND(MSG_CANCEL,on_mac_setting_exit)
ON_COMMAND(MSG_EXIT,on_mac_setting_exit)
END_MSGPROC(mac_setting_select_proc, ui_comm_root_proc)

BEGIN_MSGPROC(mac_setting_edit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_mac_addr)
  ON_COMMAND(MSG_NUMBER, on_edit_mac_addr)
END_MSGPROC(mac_setting_edit_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(mac_setting_save_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_mac_setting_save)
END_MSGPROC(mac_setting_save_text_proc, text_class_proc)


