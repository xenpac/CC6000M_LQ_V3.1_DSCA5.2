/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_keyboard_v2.h"
#include "ui_video.h"
#include "ui_stb_name.h"
#include "ui_dlna.h"
#include "cdlna.h"
#include "ui_satip.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_STB_NAME,
  IDC_STB_HELP_CONT
};

static comm_help_data_t stbname_help_data = 
{
  3, 3,
  { IDS_MOVE, 
    IDS_OK,
    IDS_EXIT},
  { IM_HELP_ARROW,
    IM_HELP_OK,
    IM_HELP_MENU}
};


static u8 g_stb_name[128];
static u8 g_temp_stb_name[128];
static char g_ori_uni_content[128];

static u16 stbname_cont_keymap(u16 key);
static RET_CODE stbname_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE stbname_change_name_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static control_t *create_stbname_menu()
{
  control_t *p_cont = NULL;
  control_t *p_stb_name = NULL;
  control_t *p_name_help = NULL;
  char stb_name[MAX_DEVICE_NAME_LENGTH];
  
  /*create the main window*/
  p_cont = ui_comm_root_create_netmenu(ROOT_ID_STB_NAME, 0, IM_INDEX_SYSTEMDETAIL_BANNER, IDS_STB_NAME);
  ctrl_set_keymap(p_cont, stbname_cont_keymap);
  ctrl_set_proc(p_cont, stbname_select_proc);
  
  /*create name*/  
  sys_status_get_dlna_device_name(stb_name);
  p_stb_name = ui_comm_t9_v2_edit_create(p_cont, IDC_STB_NAME, STB_NAME_ITEM_X, STB_NAME_ITEM_Y,
					 STB_NAME_ITEM_LW, STB_NAME_ITEM_RW, ROOT_ID_DLNA_START);
  ui_comm_t9_v2_edit_set_static_txt(p_stb_name, IDS_STB_NAME);
  ui_comm_t9_v2_edit_set_content_by_ascstr(p_stb_name, (u8 *)stb_name);
  ui_comm_ctrl_set_proc(p_stb_name, stbname_change_name_proc);

  /*help bar */
  p_name_help = ctrl_create_ctrl(CTRL_CONT, IDC_STB_HELP_CONT,
  	                                  250, 520,
  	                                  900, 80,
  	                                  p_cont, 0);
  ctrl_set_rstyle(p_name_help, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ui_comm_help_create(&stbname_help_data, p_name_help);
  
  /*set the name focus*/
  ctrl_default_proc(p_stb_name, MSG_GETFOCUS, 0, 0); /* focus on stb_name */
  return p_cont;
}


/*the exit function*/
static RET_CODE on_stbname_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_STB_NAME, 0, 0);
  return SUCCESS;
}


static void save_device_name(void)
{
    control_t *p_edit_cont = NULL;

    p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_STB_NAME, IDC_STB_NAME);
    MT_ASSERT(p_edit_cont != NULL);
    if(strlen((s8 *)g_temp_stb_name) == 0)
    {
        OS_PRINTF("####%s, stb name cant be null####\n", __FUNCTION__);
        ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
        ui_comm_t9_v2_edit_set_content_by_ascstr(p_edit_cont, (u8 *)g_ori_uni_content);
        ctrl_paint_ctrl(p_edit_cont, TRUE);
        return ;
    }

    strcpy((s8 *)g_stb_name, (s8 *)g_temp_stb_name); 
    sys_status_set_dlna_device_name((s8 *)g_stb_name);
    ui_comm_t9_v2_edit_set_content_by_ascstr(p_edit_cont, g_stb_name);
    ctrl_paint_ctrl(p_edit_cont, TRUE);
	#ifndef WIN32
    cg_dlna_rename((s8 *)g_stb_name, NULL, NULL);
	#endif
}

static void unsave_device_name(void)
{
	control_t *p_edit_cont;
	p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_STB_NAME, IDC_STB_NAME);
	MT_ASSERT(p_edit_cont != NULL);
	ui_comm_t9_v2_edit_set_content_by_ascstr(p_edit_cont, (u8 *)g_ori_uni_content);
	ctrl_paint_ctrl(p_edit_cont, TRUE);
}


RET_CODE edit_stb_name_update(u16 *p_unistr)
{
  str_uni2asc(g_temp_stb_name, p_unistr);
 
  ui_comm_ask_for_savdlg_open(NULL, IDS_SAVE_NAME,
							   (do_func_t)save_device_name, (do_func_t)unsave_device_name, 0);
  return SUCCESS;
}


static RET_CODE on_edit_stb_name(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
    kb_param_t param;  
    u8 device_name[64];
    
    if(ui_get_dlna_status() == DLNA_ON)
    {
        ui_comm_cfmdlg_open(NULL, IDS_NAME_NOTI, NULL, 2000);
        return SUCCESS;
    }
    
    if(ui_get_satip_status() == SATIP_ON)
    {
        ui_comm_cfmdlg_open(NULL, IDS_SATIP_ON_NOT_CHANGE_STB, NULL, 2000);
        return SUCCESS;
    }
    
    param.uni_str = ebox_get_content(p_ctrl);
    str_uni2asc(device_name, param.uni_str);
    strcpy(g_ori_uni_content, (s8 *)device_name);
    param.type = KB_INPUT_TYPE_ENGLISH;
    param.max_len = MAX_DEVICE_NAME_LENGTH;
    param.cb = edit_stb_name_update;
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    return SUCCESS;
}

RET_CODE open_stbname(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;

  p_menu = create_stbname_menu();
  MT_ASSERT(p_menu != NULL);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(stbname_cont_keymap, NULL)
  ON_EVENT(V_KEY_MENU,MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL,MSG_CANCEL)
END_KEYMAP(stbname_cont_keymap, NULL)

BEGIN_MSGPROC(stbname_select_proc, ui_comm_root_proc)
ON_COMMAND(MSG_CANCEL,on_stbname_exit)
ON_COMMAND(MSG_EXIT,on_stbname_exit)
END_MSGPROC(stbname_select_proc, ui_comm_root_proc)


BEGIN_MSGPROC(stbname_change_name_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_stb_name)
  ON_COMMAND(MSG_NUMBER, on_edit_stb_name)
END_MSGPROC(stbname_change_name_proc, ui_comm_edit_proc)


