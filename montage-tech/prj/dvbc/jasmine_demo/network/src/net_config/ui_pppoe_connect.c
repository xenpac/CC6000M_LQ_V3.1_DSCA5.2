/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_network_config_lan.h"
#include "ui_rename.h"
#include "ui_keyboard_v2.h"
#include "net_svc.h"
#include "ethernet.h"
#ifndef WIN32
#include "netif\ppp.h"
#endif

/* coordinate */
#define PPPOE_CONT_X          ((SCREEN_WIDTH - PPPOE_CONT_W) / 2)
#define PPPOE_CONT_Y           ((SCREEN_HEIGHT- PPPOE_CONT_H) / 2)
#define PPPOE_CONT_W          600
#define PPPOE_CONT_H          (2*PPPOE_TOP_BTM_VGAP+(PPPOE_TITLE_H + PPPOE_ITEM_VGAP)+PPPOE_ITEM_CNT*PPPOE_ITEM_VGAP+(PPPOE_ITEM_CNT+1)*PPPOE_EBOX_H)

//title
#define PPPOE_TITLE_X 20
#define PPPOE_TITLE_Y PPPOE_ITEM_VGAP
#define PPPOE_TITLE_W (PPPOE_CONT_W - 2*PPPOE_TITLE_X)
#define PPPOE_TITLE_H 30

//ebox
#define PPPOE_EBOX_X 20
#define PPPOE_EBOX_Y (20 + PPPOE_TITLE_H)
#define PPPOE_EBOX_LW 220
#define PPPOE_EBOX_RW (PPPOE_CONT_W-2*PPPOE_EBOX_X-PPPOE_EBOX_LW)
#define PPPOE_EBOX_H COMM_CTRL_H

#define PPPOE_TOP_BTM_VGAP 10
#define PPPOE_ITEM_VGAP 10

/*others*/
#define PPPOE_ITEM_CNT 4
#define PPPOE_MAX_LENGTH 32

enum local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 200,
};

enum pppoe_control_id
{
  IDC_PPPOE_USR = 1,
  IDC_PPPOE_PWD,
  IDC_PPPOE_SHOW_KEY,
  IDC_PPPOE_SAVE_KEY,
  IDC_PPPOE_TITLE,

};

static comm_help_data_t conn_pppoe_help_data = //help bar data
{
  2,                                    //select  + scroll page + exit
  2,
  {IDS_CONNECT,
   IDS_CANCEL},
   {IM_HELP_RED,
   IM_HELP_BLUE}
};

static comm_help_data_t disconn_pppoe_help_data = //help bar data
{
  2,                                    //select  + scroll page + exit
  2,
  {IDS_DISCONNECT,
   IDS_CANCEL},
   {IM_HELP_RED,
   IM_HELP_BLUE}
};

static BOOL is_conn_pppoe = FALSE;
static BOOL is_show_pwd = TRUE;
static BOOL is_save_pwd = TRUE;
static net_conn_stats_t pppoe_connt_stats;
static pppoe_info_t pppoe_info = {{0},{0}};
rename_param_t pppoe_usr_param = { NULL,PPPOE_MAX_LENGTH,KB_INPUT_TYPE_SENTENCE};
rename_param_t pppoe_pwd_param = { NULL,PPPOE_MAX_LENGTH,KB_INPUT_TYPE_SENTENCE};

u16 pppoe_cont_keymap(u16 key);
u16 is_show_pwd_keymap(u16 key);
u16 is_save_pwd_keymap(u16 key);

RET_CODE pppoe_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE pppoe_usr_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE pppoe_pwd_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE is_show_pwd_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE is_save_pwd_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
extern void paint_connect_status(BOOL is_conn, BOOL is_paint);

static control_t *create_pppoe_menu()
{
  control_t *p_menu = NULL;

   p_menu = fw_create_mainwin(ROOT_ID_PPPOE_CONNECT, PPPOE_CONT_X,
    PPPOE_CONT_Y,PPPOE_CONT_W,
    PPPOE_CONT_H, ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);

  ctrl_set_rstyle(p_menu, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
  ctrl_set_keymap(p_menu, pppoe_cont_keymap);
  ctrl_set_proc(p_menu, pppoe_cont_proc);

  return p_menu;
}

static void create_pppoe_title(control_t *p_menu)
{
  control_t *p_title = NULL;

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_PPPOE_TITLE,
                             PPPOE_TITLE_X, PPPOE_TITLE_Y,
                             PPPOE_TITLE_W, PPPOE_TITLE_H,
                             p_menu, 0);
  text_set_font_style(p_title, FSI_BLACK, FSI_BLACK, FSI_BLACK);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_PPPOE_CONN);

}

static void create_pppoe_items(control_t *p_menu)
{
  control_t *p_ctrl[PPPOE_ITEM_CNT] = {NULL};
  u16 pppoe_item_str[PPPOE_ITEM_CNT] = {IDS_ACCOUNT, IDS_PWD, IDS_SHOW_KEY, IDS_SAVE_KEY};
  u16 pppoe_enable_str[2] = {IDS_YES, IDS_NO};
  u16 i, y;
  u8 j;

  sys_status_get_pppoe_config_info(&pppoe_info);
  y = PPPOE_EBOX_Y;
  for(i = 0;i < PPPOE_ITEM_CNT;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_PPPOE_USR + i),PPPOE_EBOX_X,y,
          PPPOE_EBOX_LW, PPPOE_EBOX_RW,ROOT_ID_PPPOE_CONNECT);
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], pppoe_item_str[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], pppoe_usr_proc);

        pppoe_usr_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
        if(is_conn_pppoe)
        {
       //   ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], pppoe_info.account);
          ctrl_set_attr(p_ctrl[i], OBJ_ATTR_INACTIVE);
        }
        ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], pppoe_info.account);

        break;
      case 1:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_PPPOE_USR + i),PPPOE_EBOX_X,y,
          PPPOE_EBOX_LW, PPPOE_EBOX_RW,ROOT_ID_PPPOE_CONNECT);
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], pppoe_item_str[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], pppoe_pwd_proc);

        pppoe_pwd_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
       
        if(is_conn_pppoe)
        {
          //ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], pppoe_info.pwd);
          ctrl_set_attr(p_ctrl[i], OBJ_ATTR_INACTIVE);
        }
        ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], pppoe_info.pwd);
        break;
      case 2:
        p_ctrl[i] = ui_comm_select_create(p_menu, (u8)(IDC_PPPOE_USR + i),
                                    PPPOE_EBOX_X,y,
                                    PPPOE_EBOX_LW, PPPOE_EBOX_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], pppoe_item_str[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,2, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < 2; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, pppoe_enable_str[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_select_create_droplist(p_ctrl[i], 2);
        ui_comm_ctrl_set_keymap(p_ctrl[i], is_show_pwd_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], is_show_pwd_proc);
        if(is_conn_pppoe)
        {
          ctrl_set_attr(p_ctrl[i], OBJ_ATTR_INACTIVE);
        }
        break;
      case 3:
        p_ctrl[i] = ui_comm_select_create(p_menu, (u8)(IDC_PPPOE_USR + i),
                                    PPPOE_EBOX_X,y,
                                    PPPOE_EBOX_LW, PPPOE_EBOX_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], pppoe_item_str[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,2, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < 2; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, pppoe_enable_str[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_select_create_droplist(p_ctrl[i], 2);
        ui_comm_ctrl_set_keymap(p_ctrl[i], is_save_pwd_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], is_save_pwd_proc);
        if(is_conn_pppoe)
        {
          ctrl_set_attr(p_ctrl[i], OBJ_ATTR_INACTIVE);
        }
        break;
      default:
        break;
    }
    y += PPPOE_EBOX_H + PPPOE_ITEM_VGAP;

  ctrl_set_related_id(p_ctrl[i],
                        0,                       /* left */
                        (u8)((i - 1 + PPPOE_ITEM_CNT)
                             % PPPOE_ITEM_CNT + 1),    /* up */
                        0,                       /* right */
                        (u8)((i + 1) % PPPOE_ITEM_CNT + 1));    /* down */

  }
 
  if(!is_conn_pppoe)
  {
    ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  }

}

static void create_pppoe_help(control_t *p_menu)
{
  // add help bar
  if(is_conn_pppoe)
  {
    ui_comm_help_create_for_pop_dlg(&disconn_pppoe_help_data, p_menu);
  }
  else
  {
    ui_comm_help_create_for_pop_dlg(&conn_pppoe_help_data, p_menu);
  }
}

RET_CODE open_pppoe_connect(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  
   //create menu
  p_menu = create_pppoe_menu();

  //create title
  create_pppoe_title(p_menu);

  //create pppoe items
  create_pppoe_items(p_menu);

  //add pppoe help bar
  create_pppoe_help(p_menu);

  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
 
  return SUCCESS;
}

static RET_CODE on_pppoe_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  is_show_pwd = TRUE;
  is_save_pwd = TRUE;

  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
  {
    ui_comm_dlg_close();
  }
  
  manage_close_menu(ROOT_ID_PPPOE_CONNECT, 0, 0);
  return SUCCESS;
}

static void disconnect_pppoe(int ppp_fd)
{
  #ifndef WIN32
  pppClose(ppp_fd);
  #endif
  paint_connect_status(FALSE, TRUE);
  is_conn_pppoe = FALSE;
  pppoe_connt_stats.is_eth_conn = FALSE;
  ui_set_net_connect_status(pppoe_connt_stats);
  manage_close_menu(ROOT_ID_PPPOE_CONNECT, 0, 0);
}

static void ui_lwip_init_pppoe(ethernet_device_t *eth_dev)
{
  
  ethernet_cfg_t ethcfg = {0};
  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  MT_ASSERT(eth_dev != NULL);

  ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  ethcfg.is_enabledhcp = FALSE;
  ethcfg.hwaddr[0] = (u8)sys_status_get_mac_by_index(0);
  ethcfg.hwaddr[1] = (u8)sys_status_get_mac_by_index(1);
  ethcfg.hwaddr[2] = (u8)sys_status_get_mac_by_index(2);
  ethcfg.hwaddr[3] = (u8)sys_status_get_mac_by_index(3);
  ethcfg.hwaddr[4] = (u8)sys_status_get_mac_by_index(4);
  ethcfg.hwaddr[5] = (u8)sys_status_get_mac_by_index(5);
  OS_PRINTF("###pppoe mac address ==   %x:%x:%x:%x:%x:%x#####\n ",
                    ethcfg.hwaddr[0] ,ethcfg.hwaddr[1],
                    ethcfg.hwaddr[2] ,ethcfg.hwaddr[3],
                    ethcfg.hwaddr[4] ,ethcfg.hwaddr[5]);
  //ipaddress
  ethcfg.ipaddr[0] = 0;
  ethcfg.ipaddr[1] = 0;
  ethcfg.ipaddr[2] = 0;
  ethcfg.ipaddr[3] = 0;

  //netmask
  ethcfg.netmask[0] = 0;
  ethcfg.netmask[1] = 0;
  ethcfg.netmask[2] = 0;
  ethcfg.netmask[3] = 0;

  //gateway
  ethcfg.gateway[0] = 0;
  ethcfg.gateway[1] = 0;
  ethcfg.gateway[2] = 0;
  ethcfg.gateway[3] = 0;

  //dns server
  ethcfg.primaryDNS[0] = 0;
  ethcfg.primaryDNS[1] = 0;
  ethcfg.primaryDNS[2] = 0;
  ethcfg.primaryDNS[3] = 0;

  lwip_init_tcpip(eth_dev, &ethcfg);
}

static void get_ctrl_strings(u16 ctrl_idc, u8 *asc)
{
  control_t *p_ctrl = NULL;
  u16 *temp;
  
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_PPPOE_CONNECT, (u8)ctrl_idc);
  temp = ui_comm_t9_v2_edit_get_content(p_ctrl);
  if(temp != NULL)
  {
    str_uni2asc(asc, temp);
  }
    
}

static void get_usr_pwd(u8 *usr, u8 *pwd)
{
  get_ctrl_strings(IDC_PPPOE_USR, usr);
  get_ctrl_strings(IDC_PPPOE_PWD, pwd);
}

static RET_CODE on_pppoe_connect(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ethernet_device_t *eth_dev = NULL;
  u8 usr_asc[32] = {0};
  u8 pwd_asc[32] = {0};
  int ppp_fd = 0;

  pppoe_connt_stats = ui_get_net_connect_status();
  if(is_conn_pppoe)
  {
    disconnect_pppoe(ppp_fd);
    
    return SUCCESS;
  }
  
  get_usr_pwd(usr_asc, pwd_asc);
  OS_PRINTF("########connect pppoe, usr is:%s, pwd is:%s#########\n",usr_asc, pwd_asc);
  #ifndef WIN32
  ui_lwip_init_pppoe(eth_dev);
  pppSetAuth(PPPAUTHTYPE_ANY, (const char*)usr_asc, (const char*)pwd_asc);
  ppp_fd = pppOverEthernetOpen(netif_default, NULL, NULL, NULL, NULL);
  #endif
  OS_PRINTF("###########pppOverEthernetOpen fd = %d#########\n", ppp_fd);

  if(ppp_fd == 0)
  {
    OS_PRINTF("#########################pppoe connect successfully################\n");
   // strcpy(pppoe_info.account, usr_asc);
    //strcpy(pppoe_info.pwd, pwd_asc);  
   // sys_status_set_pppoe_config_info(&pppoe_info);
   // sys_status_save();
    is_conn_pppoe = TRUE;
    //on_config_ip_update(eth_dev);
    paint_connect_status(TRUE, TRUE);
    pppoe_connt_stats.is_eth_conn = TRUE;
  }
  else
  {
    OS_PRINTF("#########################pppoe connect falied################\n");
    is_conn_pppoe = FALSE;
    paint_connect_status(FALSE, TRUE);
    pppoe_connt_stats.is_eth_conn = FALSE;
  }
  strcpy((char*)pppoe_info.account, (char*)usr_asc);
  strcpy((char*)pppoe_info.pwd, (char*)pwd_asc);  
  sys_status_set_pppoe_config_info(&pppoe_info);
  sys_status_save();
  ui_set_net_connect_status(pppoe_connt_stats);
  manage_close_menu(ROOT_ID_PPPOE_CONNECT, 0, 0);
  return SUCCESS;

}

//void edit_usr_update(void)
RET_CODE edit_usr_update(u16* p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_PPPOE_CONNECT, IDC_PPPOE_USR);
  ctrl_paint_ctrl(p_edit_cont, TRUE);  
  return SUCCESS;
}

//void edit_pwd_update(void)
RET_CODE edit_pwd_update(u16* p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_PPPOE_CONNECT, IDC_PPPOE_PWD);
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_edit_usr(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = pppoe_usr_param.type;
  param.max_len = 32;
  param.cb = edit_usr_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}

static RET_CODE on_edit_pwd(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = pppoe_pwd_param.type;
  param.max_len = 32;
  param.cb = edit_pwd_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}

static RET_CODE on_is_show_pwd(control_t *p_cbox, u16 msg,u32 para1, u32 para2)
{
  control_t *p_pwd = NULL;
  control_t *p_show_pwd = NULL;
  control_t *p_ebox = NULL;
  u16 focus = 0;

  p_show_pwd = ctrl_get_parent(p_cbox);
  p_pwd = ctrl_get_child_by_id(ctrl_get_parent(p_show_pwd), IDC_PPPOE_PWD);
  p_ebox = ctrl_get_child_by_id(p_pwd, 2);
  focus = ui_comm_select_get_focus(p_show_pwd);

  if(1 == focus)
  {
    is_show_pwd = TRUE;
    ebox_set_worktype(p_ebox, EBOX_WORKTYPE_HIDE);
    ctrl_paint_ctrl(p_ebox,TRUE);
  }
  else
  {
    ebox_set_worktype(p_ebox, EBOX_WORKTYPE_EDIT);
    is_show_pwd = FALSE;
    ctrl_paint_ctrl(p_ebox,TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_is_save_pwd(control_t *p_cbox, u16 msg,u32 para1, u32 para2)
{
  control_t *p_save_pwd = NULL;
  u16 focus = 0;

  p_save_pwd = ctrl_get_parent(p_cbox);
  focus = ui_comm_select_get_focus(p_save_pwd);

  if(1 == focus)
  {
    is_save_pwd = FALSE;
  }
  else
  {
    is_save_pwd = TRUE;
  }

  return SUCCESS;
}

static RET_CODE on_pppoe_net_cable_plug_out(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  is_conn_pppoe = FALSE;
  pppoe_connt_stats.is_eth_conn = FALSE;
  ui_set_net_connect_status(pppoe_connt_stats);
  OS_PRINTF("###########at pppoe ui, the net cable plug out########\n");
  
  return SUCCESS;
}

BEGIN_KEYMAP(pppoe_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_BLUE, MSG_CANCEL)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(pppoe_cont_keymap, NULL)

BEGIN_MSGPROC(pppoe_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CANCEL, on_pppoe_exit)
  ON_COMMAND(MSG_RED, on_pppoe_connect)
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_pppoe_net_cable_plug_out)
//  ON_COMMAND(MSG_CONFIG_IP, on_connect_pppoe_over)
END_MSGPROC(pppoe_cont_proc, cont_class_proc)

//edit usr
BEGIN_MSGPROC(pppoe_usr_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_usr)
  ON_COMMAND(MSG_NUMBER, on_edit_usr)
END_MSGPROC(pppoe_usr_proc, ui_comm_edit_proc)

//edit pwd
BEGIN_MSGPROC(pppoe_pwd_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_pwd)
  ON_COMMAND(MSG_NUMBER, on_edit_pwd)
END_MSGPROC(pppoe_pwd_proc, ui_comm_edit_proc)

//is show pwd
BEGIN_KEYMAP(is_show_pwd_keymap, NULL)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
END_KEYMAP(is_show_pwd_keymap, NULL)

BEGIN_MSGPROC(is_show_pwd_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_is_show_pwd)
END_MSGPROC(is_show_pwd_proc, cbox_class_proc)

//is save pwd
BEGIN_KEYMAP(is_save_pwd_keymap, NULL)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
END_KEYMAP(is_save_pwd_keymap, NULL)

BEGIN_MSGPROC(is_save_pwd_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_is_save_pwd)
END_MSGPROC(is_save_pwd_proc, cbox_class_proc)

//lint -esym(551,is_save_pwd)
//lint -esym(551,is_show_pwd)

