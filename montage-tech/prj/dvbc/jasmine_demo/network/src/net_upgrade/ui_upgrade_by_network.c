/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_upgrade_by_network.h"
//#include "ui_rename.h"
#include "ui_keyboard_v2.h"
#include "ui_net_upg_api.h"

typedef enum
{
  IDC_INVALID = 0,
  IDC_PROTOCOL,
  IDC_URL_TYPE,
  IDC_NUM_URL,
  IDC_USER,
  IDC_PWD,
  IDC_START_UP,
  IDC_NET_UPG_PROC_BAR,
  IDC_NET_UPG_STS,
  IDC_STR_URL,
}upgrade_by_net_control_id_t;

/*static rect_t g_net_upgrade_dlg_rc =
{
  UPGRADE_BY_NET_DLG_X,  UPGRADE_BY_NET_DLG_Y,
  UPGRADE_BY_NET_DLG_X + UPGRADE_BY_NET_DLG_W,
  UPGRADE_BY_NET_DLG_Y + UPGRADE_BY_NET_DLG_H,
};*/
static u16 upgrade_by_net_cont_keymap(u16 key);
static RET_CODE upgrade_by_net_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_protocol_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_url_type_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_url_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_usr_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_pwd_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_start_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


static net_upg_info_t g_net_upg_info = {0};
static protocol_t g_protocol = FTP;
static ui_url_type_t g_url_type = NUMBER;
static kb_param_t upgrade_by_net_url_param = { NULL,32,KB_INPUT_TYPE_SENTENCE};
static kb_param_t upgrade_by_net_usr_param = { NULL,32,KB_INPUT_TYPE_SENTENCE};
static kb_param_t upgrade_by_net_pwd_param = { NULL,32,KB_INPUT_TYPE_SENTENCE};
/*************************************************************
    ************* ********create UI  func***********************
*************************************************************/
static control_t *create_net_upg_menu()
{
  control_t *p_cont = NULL;

  p_cont = ui_comm_root_create(ROOT_ID_UPGRADE_BY_NETWORK, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, 0, IDS_UPGRADE_BY_NET);
  
  ctrl_set_keymap(p_cont, upgrade_by_net_cont_keymap);
  ctrl_set_proc(p_cont, upgrade_by_net_cont_proc);

  return p_cont;
}

static void create_net_upg_items(control_t *p_menu, net_upg_info_t *p_net_info)
{
  control_t *p_ctrl[UPGRADE_BY_NET_ITEM_CNT] = {NULL};
  control_t *p_pwd_ebox = NULL;
  control_t *p_num_url = NULL;
  u8 ip_separator[IPBOX_MAX_ITEM_NUM] = {'.', '.', '.', ' '};
  u16 stxt[UPGRADE_BY_NET_ITEM_CNT] = {IDS_PROTOCOL, IDS_URL_TYPE, IDS_URL, IDS_USR, IDS_PASSWD, IDS_START};
  u16 protocol[UPGRADE_BY_NET_PROTOCOL_CNT] = {IDS_FTP/*, IDS_HTTP*/};
  u16 url_type[UPGRADE_BY_NET_URL_TYPE_CNT] = {IDS_NUMBER, IDS_STRING};
  u16 i, j, y;

  y = UPGRADE_BY_NET_ITEM1_Y;
  for (i = 0; i < UPGRADE_BY_NET_ITEM_CNT; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case 0:
        p_ctrl[i] = ui_comm_select_create(p_menu, IDC_PROTOCOL, UPGRADE_BY_NET_ITEM1_X, y,
                          UPGRADE_BY_NET_ITEM1_LW, UPGRADE_BY_NET_ITEM1_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           UPGRADE_BY_NET_PROTOCOL_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < UPGRADE_BY_NET_PROTOCOL_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, protocol[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], (p_net_info->protocol==FTP)?0:1);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_protocol_proc);
        break;
      case 1:
        p_ctrl[i] = ui_comm_select_create(p_menu, IDC_URL_TYPE, UPGRADE_BY_NET_ITEM1_X, y,
                          UPGRADE_BY_NET_ITEM1_LW, UPGRADE_BY_NET_ITEM1_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           UPGRADE_BY_NET_URL_TYPE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < UPGRADE_BY_NET_URL_TYPE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, url_type[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], (p_net_info->url_type==NUMBER)?0:1);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_url_type_proc);
        break;
      case 2:
        p_num_url = ui_comm_ipedit_create(p_menu, IDC_NUM_URL, UPGRADE_BY_NET_ITEM1_X, y,
                            UPGRADE_BY_NET_ITEM1_LW, UPGRADE_BY_NET_ITEM1_RW);
        ui_comm_ipedit_set_static_txt(p_num_url, IDS_URL);
        ui_comm_ipedit_set_param(p_num_url, 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
        ui_comm_ipedit_set_address(p_num_url, (ip_address_t *)p_net_info->ip_address);
        ui_comm_ctrl_set_keymap(p_num_url, ui_comm_ipbox_keymap);
        ui_comm_ctrl_set_proc(p_num_url, ui_comm_ipbox_proc);
        for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
        {
          ui_comm_ipedit_set_separator_by_ascchar(p_num_url, (u8)j, ip_separator[j]);
        }
        ctrl_set_sts(p_num_url, ((p_net_info->url_type==NUMBER)?OBJ_STS_SHOW:OBJ_STS_HIDE));

        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, IDC_STR_URL, UPGRADE_BY_NET_ITEM1_X, y, 
                           UPGRADE_BY_NET_ITEM1_LW, UPGRADE_BY_NET_ITEM1_RW, ROOT_ID_UPGRADE_BY_NETWORK);
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_url_proc);    
        upgrade_by_net_url_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
        ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], p_net_info->url);
        ctrl_set_sts(p_ctrl[i], ((p_net_info->url_type==STRING)?OBJ_STS_SHOW:OBJ_STS_HIDE));
        ctrl_set_related_id(p_num_url, 0, IDC_URL_TYPE, 0, IDC_USER);
        break;
      case 3:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, IDC_USER, UPGRADE_BY_NET_ITEM1_X, y, 
                           UPGRADE_BY_NET_ITEM1_LW, UPGRADE_BY_NET_ITEM1_RW, ROOT_ID_UPGRADE_BY_NETWORK);
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], p_net_info->usr);
        upgrade_by_net_usr_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_usr_proc);   
        if(HTTP == g_protocol)
        {
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        break;
      case 4:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, IDC_PWD, UPGRADE_BY_NET_ITEM1_X, y, 
                           UPGRADE_BY_NET_ITEM1_LW, UPGRADE_BY_NET_ITEM1_RW, ROOT_ID_UPGRADE_BY_NETWORK);
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], stxt[i]);
        upgrade_by_net_pwd_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
        
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_pwd_proc);
        if(HTTP == g_protocol)
        {
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        p_pwd_ebox = ctrl_get_child_by_id(p_ctrl[i], IDC_COMM_CTRL);
        ebox_set_worktype(p_pwd_ebox, EBOX_WORKTYPE_HIDE);
        break;  
      case 5:
        p_ctrl[i] = ui_comm_static_create(p_menu, IDC_START_UP, UPGRADE_BY_NET_ITEM1_X, y,
                          UPGRADE_BY_NET_ITEM1_LW, UPGRADE_BY_NET_ITEM1_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_start_proc);
       
        break;
      default:
        break;
    }

    y += UPGRADE_BY_NET_ITEM1_H + UPGRADE_BY_NET_ITEM1_V_GAP;
    ctrl_set_related_id(p_ctrl[i],
                        0,                       /* left */
                        (u8)((i - 1 + UPGRADE_BY_NET_ITEM_CNT)
                             % UPGRADE_BY_NET_ITEM_CNT + 1),    /* up */
                        0,                       /* right */
                        (u8)((i + 1) % UPGRADE_BY_NET_ITEM_CNT + 1));    /* down */
  }
  
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); //protocol get focus
}

/*static void create_net_upg_pbar(control_t *p_menu)
{
  
  control_t *p_bar = NULL;
  
  p_bar = ui_comm_bar_create(p_menu, IDC_NET_UPG_PROC_BAR, UPGRADE_BY_NET_PBAR1_X,
                         UPGRADE_BY_NET_PBAR1_Y,UPGRADE_BY_NET_PBAR1_W, UPGRADE_BY_NET_PBAR1_H,
                          UPGRADE_BY_NET_TXT1_X, UPGRADE_BY_NET_PBAR1_Y - 3, UPGRADE_BY_NET_TXT1_W, UPGRADE_BY_NET_TXT1_H,
                          UPGRADE_BY_NET_PER1_X, UPGRADE_BY_NET_PBAR1_Y - 3, UPGRADE_BY_NET_PER1_W, UPGRADE_BY_NET_PER1_H);
  ui_comm_bar_set_param(p_bar, RSC_INVALID_ID, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,RSI_UPGRADE_BY_NET_PBAR_BG, RSI_UPGRADE_BY_NET_PBAR_MID,
                                                RSI_IGNORE, FSI_WHITE,RSI_PBACK, FSI_WHITE);
  ui_comm_bar_update(p_bar, 0, TRUE);
  
}*/

static void create_net_upg_text_sts(control_t *p_menu)
{
  control_t *p_sts = NULL;

  p_sts = ctrl_create_ctrl(CTRL_TEXT,IDC_NET_UPG_STS, UPGRADE_BY_NET_STS_X, 
                                          UPGRADE_BY_NET_STS_Y,UPGRADE_BY_NET_STS_W,
                                          UPGRADE_BY_NET_STS_H,p_menu, 0);

  ctrl_set_rstyle(p_sts, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_sts, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_sts, FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
  text_set_content_type(p_sts, TEXT_STRTYPE_UNICODE);
      
}

RET_CODE open_upgrade_by_network(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;

  sys_status_get_net_upg_config_info(&g_net_upg_info);


  g_protocol = 1;//g_net_upg_info.protocol;
  g_url_type = g_net_upg_info.url_type;
  
  //create mainmenu
  p_menu = create_net_upg_menu();

  //create items
  create_net_upg_items(p_menu, &g_net_upg_info);

  //create process bar
//  create_net_upg_pbar(p_menu);

  //create text status
  create_net_upg_text_sts(p_menu);
  
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  return SUCCESS;
}

/*************************************************************
    ************* ********container proc func***********************
*************************************************************/
static RET_CODE on_upg_by_net_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  
  manage_close_menu(ROOT_ID_UPGRADE_BY_NETWORK, 0, 0);
  return ERR_NOFEATURE;

}

static RET_CODE on_net_upg_by_net_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_net_upg_stop();
  return ERR_NOFEATURE;

}

/*************************************************************
    ************* ********protocol item proc func********************
*************************************************************/
static void update_usr_pwd_attr(BOOL is_enable)
{
  u16 i = 0;
  control_t *p_ctrl[2] = {NULL};
  for(i = 0;i < 2;i++)
  {
    p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_USER + i);
    ui_comm_ctrl_update_attr(p_ctrl[i], is_enable); 
    ctrl_paint_ctrl(p_ctrl[i], TRUE);
  }
}
static RET_CODE on_protocol_change(control_t *p_cbox, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  
  focus = ui_comm_select_get_focus(p_cbox->p_parent);
  switch(focus)
  {
    case 0:
      g_protocol = FTP; 
      update_usr_pwd_attr(TRUE);
      break;
    case 1:
      g_protocol = HTTP; 
      update_usr_pwd_attr(FALSE);
      break;
    default:
      break;
  }
  
  return SUCCESS;
}

/*************************************************************
    ************* *******url type item proc func********************
*************************************************************/
static void change_sts_and_related_id(BOOL is_from_str_to_num)
{
  control_t *p_url_type_ctrl = NULL;
  control_t *p_num_ctrl = NULL;
  control_t *p_str_ctrl = NULL;
  control_t *p_usr_ctrl = NULL;

  p_url_type_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_URL_TYPE);
  p_num_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_NUM_URL);
  p_str_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_STR_URL);
  p_usr_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_USER);
  if(is_from_str_to_num)
  {
    ctrl_set_sts(p_str_ctrl, OBJ_STS_HIDE);
    ctrl_set_sts(p_num_ctrl, OBJ_STS_SHOW);
    ctrl_set_related_id(p_url_type_ctrl, 0, IDC_PROTOCOL, 0, IDC_NUM_URL);
    ctrl_set_related_id(p_num_ctrl, 0, IDC_URL_TYPE, 0, IDC_USER);
    ctrl_set_related_id(p_usr_ctrl, 0, IDC_NUM_URL, 0, IDC_PWD);
  }
  else
  {
    ctrl_set_sts(p_str_ctrl, OBJ_STS_SHOW);
    ctrl_set_sts(p_num_ctrl, OBJ_STS_HIDE);
    ctrl_set_related_id(p_url_type_ctrl, 0, IDC_PROTOCOL, 0, IDC_STR_URL);
    ctrl_set_related_id(p_num_ctrl, 0, IDC_URL_TYPE, 0, IDC_USER);
    ctrl_set_related_id(p_usr_ctrl, 0, IDC_STR_URL, 0, IDC_PWD);
  }
  ctrl_paint_ctrl(ctrl_get_parent(p_num_ctrl), TRUE);
  
}

static void change_url_ctrl()
{
  
  if(g_url_type == STRING)
  {
    change_sts_and_related_id(TRUE);
    g_url_type = NUMBER;
  }
  else if(g_url_type == NUMBER)
  {
    change_sts_and_related_id(FALSE);
    g_url_type = STRING;
  }
  
}

static RET_CODE on_url_type_change(control_t *p_cbox, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  
  focus = ui_comm_select_get_focus(p_cbox->p_parent);
  change_url_ctrl();
  
  return SUCCESS;
}

/*************************************************************
    ************* *******url item proc func********************
*************************************************************/
RET_CODE edit_upg_by_net_url_update(u16 *p_unistr)
{
  control_t *p_edit_cont;
  
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_STR_URL);
  ctrl_default_proc(p_edit_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_edit_cont), FALSE);
  return SUCCESS;
}

static RET_CODE on_edit_upg_by_net_url(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = upgrade_by_net_url_param.type;
  param.max_len = 32;
  param.cb = edit_upg_by_net_url_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

/*************************************************************
    ************* *******user item proc func********************
*************************************************************/
RET_CODE edit_upg_by_net_usr_update(u16 *p_unistr)
{
  control_t *p_edit_cont;
  
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_USER);
  ctrl_default_proc(p_edit_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_edit_cont), FALSE);
  return SUCCESS;
  
}

static RET_CODE on_edit_upg_by_net_usr(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = upgrade_by_net_usr_param.type;
  param.max_len = 32;
  param.cb = edit_upg_by_net_usr_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

/*************************************************************
    ************* *******pwd item proc func********************
*************************************************************/
RET_CODE edit_upg_by_net_pwd_update(u16 *p_unistr)
{
  control_t *p_edit_cont;
  
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_PWD);
  ctrl_default_proc(p_edit_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_edit_cont), FALSE);
  return SUCCESS;
}

static RET_CODE on_edit_upg_by_net_pwd(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = upgrade_by_net_pwd_param.type;
  param.max_len = 32;
  param.cb = edit_upg_by_net_pwd_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

/*************************************************************
    ************* *******start item proc func********************
*************************************************************/
static void get_url_ip_num(ip_address_t *ip_address)
{
  control_t *p_ctrl = NULL;

  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_NUM_URL);
  MT_ASSERT(p_ctrl != NULL);
  ui_comm_ipedit_get_address(p_ctrl, ip_address);
}
static void get_ctrl_string_info(u16 ctrl_idc, u8 *asc)
{
  control_t *p_ctrl = NULL;
  u16 *temp;
  
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, ctrl_idc);
  temp = ui_comm_t9_v2_edit_get_content(p_ctrl);
  if(temp != NULL)
  {
    str_uni2asc(asc, temp);
  }
    
}

/*static void ui_upgrade_by_net_sts_show(u16 *p_str)
{
  control_t *p_ctrl = NULL;

  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_NET_UPG_STS);
  text_set_content_by_unistr(p_ctrl, p_str);
  ctrl_paint_ctrl(p_ctrl, TRUE);
}

static void ui_upgrade_by_net_pbar_show()
{
  control_t *p_ctrl = NULL;
  u32 total_size = 5;
  u16 percent = 0;
  u32 downloaded_size = 0;
  
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_NET_UPG_PROC_BAR);
  while(downloaded_size < total_size)
  {
    downloaded_size += 1;
    percent = (downloaded_size * 100) / total_size;
    ui_comm_bar_update(p_ctrl, percent, TRUE);
    ui_comm_bar_paint(p_ctrl, TRUE);
    mtos_task_delay_ms(1000);
  }
}*/

static void change_start_sts(BOOL is_enable)
{
  control_t *p_ctrl = NULL;
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_START_UP);
  ui_comm_ctrl_update_attr(p_ctrl, is_enable); 
  ctrl_paint_ctrl(p_ctrl, TRUE);

}

void ui_refresh_net_upg_items()
{
  control_t *p_start = NULL;
  //control_t *p_pbar = NULL;

  p_start = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_START_UP);
  ui_comm_ctrl_update_attr(p_start, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_start, TRUE);

  /*p_pbar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_NETWORK, IDC_NET_UPG_PROC_BAR);
  ui_comm_bar_update(p_pbar, 0, TRUE);
  ui_comm_bar_paint(p_pbar, TRUE);
*/
}

static void close_network_upg_menus(void)
{
  control_t *root = NULL;

  root = fw_find_root_by_id(ROOT_ID_UPGRADE_BY_NETWORK);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }
}

static RET_CODE on_net_upg_evt_check_done(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  net_upg_api_info_t * p_upg_info = NULL;
  
  p_upg_info = ui_net_upg_get_upg_info();
  if (p_upg_info->item_cnt)
  {
    OS_PRINTF("\n##on_net_upg_evt_check_done [%d, %s]\n",
      p_upg_info->item_cnt, p_upg_info->item[0].url);
    ui_refresh_net_upg_items();
    manage_open_menu(ROOT_ID_NETWORK_UPGRADE, (u32)p_upg_info, 1);
  }
  else
  {
    //fix error string id here
    ui_comm_cfmdlg_open(NULL, IDS_NETWORK_BUSY, (do_func_t)close_network_upg_menus, 0);
  }
  return SUCCESS;
}

static RET_CODE on_upg_by_net_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 url_asc[32] = {0};
  u8 usr_asc[32] = {0};
  u8 pwd_asc[32] = {0};
  //u16 uni_str[UPGRADE_BY_NET_LEN] = {0};
  ip_address_t ip_address = {0, 0, 0, 0};
#if 0
  u8 usr_asc_temp[32] = {'1', '2', '3', 0};
  u8 pwd_asc_temp[32] = {'1', '1', '1', 0};
#ifdef WIN32
ip_address_t ip_address_temp = {192, 168, 35, 179};
#else
ip_address_t ip_address_temp = {192, 168, 35, 179};
#endif
#endif
  u8 path[256] = {0};
  switch(g_protocol)
  {
    case HTTP: //protocol type has modified
      if(g_url_type == STRING)
      {
        get_ctrl_string_info(IDC_STR_URL, url_asc);
      }
      else if(g_url_type == NUMBER)
      {
        get_url_ip_num(&ip_address);
      }
      break;
    case FTP:
      if(g_url_type == STRING)
      {
        get_ctrl_string_info(IDC_STR_URL, url_asc);
      }
      else if(g_url_type == NUMBER)
      {
        get_url_ip_num(&ip_address);
      }
      get_ctrl_string_info(IDC_USER, usr_asc);
      get_ctrl_string_info(IDC_PWD, pwd_asc);
      break;
  }
  #if 0
  if (ip_address.s_b1 == 0)
  {
    memcpy(&ip_address, &ip_address_temp, sizeof(ip_address_t));
  }
  
  if (usr_asc[0] == 0)
  {
    strcpy(usr_asc, usr_asc_temp);
  }
  if (pwd_asc[0] == 0)
  {
    strcpy(pwd_asc, pwd_asc_temp);
  }
#endif
  OS_PRINTF("\n##on_upg_by_net_start [%d, %d, %d, %d][%s, %s]\n",
    ip_address.s_b1, ip_address.s_b2, ip_address.s_b3, ip_address.s_b4, usr_asc, pwd_asc);
  sprintf(path, "%d.%d.%d.%d", ip_address.s_b1, ip_address.s_b2, ip_address.s_b3, ip_address.s_b4);

  g_net_upg_info.protocol = (g_protocol == HTTP)?0:1;
  g_net_upg_info.url_type = (g_url_type == NUMBER)?0:1;
  strcpy(g_net_upg_info.usr, usr_asc);
  if(g_url_type == NUMBER)
  {
    memcpy(g_net_upg_info.ip_address, &ip_address, sizeof(ip_address_t));
  }
  else
  {
    strcpy(g_net_upg_info.url, url_asc);
  }
  sys_status_set_net_upg_config_info(&g_net_upg_info);
  //str_nasc2uni("Start to download config file...", uni_str, UPGRADE_BY_NET_LEN);
  //ui_upgrade_by_net_sts_show(uni_str);
  change_start_sts(FALSE);
  ui_net_upg_init();
  ui_net_upg_start_check(g_protocol, path, usr_asc, pwd_asc);
                                               
  return SUCCESS;
}

BEGIN_KEYMAP(upgrade_by_net_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(upgrade_by_net_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(upgrade_by_net_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_upg_by_net_exit)
  ON_COMMAND(MSG_EXIT, on_upg_by_net_exit)
  ON_COMMAND(MSG_DESTROY, on_net_upg_by_net_destory)
  ON_COMMAND(MSG_NET_UPG_EVT_CHECK_DONE, on_net_upg_evt_check_done)
END_MSGPROC(upgrade_by_net_cont_proc, ui_comm_root_proc)


BEGIN_MSGPROC(upgrade_by_net_protocol_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_protocol_change)
END_MSGPROC(upgrade_by_net_protocol_proc, cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_net_url_type_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_url_type_change)
END_MSGPROC(upgrade_by_net_url_type_proc, cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_net_url_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_upg_by_net_url)
  ON_COMMAND(MSG_NUMBER, on_edit_upg_by_net_url)
END_MSGPROC(upgrade_by_net_url_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(upgrade_by_net_usr_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_upg_by_net_usr)
  ON_COMMAND(MSG_NUMBER, on_edit_upg_by_net_usr)
END_MSGPROC(upgrade_by_net_usr_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(upgrade_by_net_pwd_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_upg_by_net_pwd)
  ON_COMMAND(MSG_NUMBER, on_edit_upg_by_net_pwd)
END_MSGPROC(upgrade_by_net_pwd_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(upgrade_by_net_start_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_upg_by_net_start)
END_MSGPROC(upgrade_by_net_start_proc, text_class_proc)

