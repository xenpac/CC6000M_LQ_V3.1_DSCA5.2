/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_wifi_link_info.h"
#include "ui_rename.h"
#include "ui_keyboard_v2.h"
#include "ui_wifi.h"
#include "wifi.h"
#include "ui_network_config_wifi.h"
#include "net_svc.h"
#include "ui_dlna.h"
#include "modem.h"

enum control_id
{
  IDC_SSID = 1,
  IDC_ENCRYPT_MODE,

  IDC_KEYS,
  IDC_SHOW_KEY,
  IDC_SAVE_KEY,
  IDC_MAX_ITEM,
  IDC_WIFI_LINK_INFO_TITLE,

};

enum local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 200,
};

static comm_help_data_t conn_help_data = //help bar data
{
  2,                                    //select  + scroll page + exit
  2,
  {IDS_CONNECT,
   IDS_CANCEL},
   {IM_HELP_RED,
   IM_HELP_BLUE}
};

static comm_help_data_t disconn_help_data = //help bar data
{
  2,                                    //select  + scroll page + exit
  2,
  {IDS_DISCONNECT,
   IDS_CANCEL},
   {IM_HELP_RED,
   IM_HELP_BLUE}
};

static u32 is_add_wifi = 0;
static BOOL is_show_key = TRUE;
static BOOL is_save_key = TRUE;
static BOOL is_connect_curn = FALSE;
static ethernet_cfg_t ethcfg = {0};
static wifi_ap_info_t ap_info = {{0}};
static u16 list_focus = 0;
static u16 g_conn_list_focus = 0;
static wifi_info_t p_wifi_info = {{0},{0}};
static wifi_info_t p_temp_wifi_info = {{0},{0}};
static net_conn_stats_t wifi_connt_stats = {FALSE, FALSE, FALSE, FALSE};

u16 wifi_link_info_cont_keymap(u16 key);
u16 is_show_key_keymap(u16 key);
u16 is_save_key_keymap(u16 key);

RET_CODE wifi_link_info_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE is_show_key_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE edit_ssid_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE edit_mode_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE input_key_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE is_save_key_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

extern void paint_wifi_status(BOOL is_conn, BOOL is_paint);
extern void paint_connect_status(BOOL is_conn, BOOL is_paint);
extern void paint_list_field_is_connect(u16 index, BOOL is_connect, BOOL is_paint);
extern u16 get_wifi_conn_focus(void);
extern void set_wifi_conn_focus(u16 focus);
extern void paint_wifi_list_field_not_connect(control_t *p_list);
extern ethernet_cfg_t get_static_wifi_ethcfg(void);
extern void do_cmd_disconnect_gprs(void);
extern void do_cmd_disconnect_g3(void);
extern void get_addr_param(u8 *p_buffer, ip_address_t *p_addr);

void paint_connect_net_status(u16 index, BOOL is_connect);

rename_param_t edit_ssid_param = { NULL,MAX_SSID_LEN,KB_INPUT_TYPE_SENTENCE};
rename_param_t edit_mode_param = { NULL,WIFI_LINK_INFO_MAX_LENGTH,KB_INPUT_TYPE_SENTENCE};
rename_param_t input_key_param = { NULL,MAX_PASS_WORD_LEN,KB_INPUT_TYPE_SENTENCE};

void set_temp_wifi_info(wifi_info_t temp)
{
  p_temp_wifi_info = temp;
}
wifi_info_t get_temp_wifi_info()
{
  return p_temp_wifi_info;
}

static control_t *create_wifi_info_menu()
{
  control_t *p_menu = NULL;

   p_menu = fw_create_mainwin(ROOT_ID_WIFI_LINK_INFO, WIFI_LINK_INFO_CONT_X,
    WIFI_LINK_INFO_CONT_Y,WIFI_LINK_INFO_CONT_W,
    WIFI_LINK_INFO_CONT_H, ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);

  ctrl_set_rstyle(p_menu, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
  ctrl_set_keymap(p_menu, wifi_link_info_cont_keymap);
  ctrl_set_proc(p_menu, wifi_link_info_cont_proc);

  return p_menu;

}

static void create_wifi_info_title(control_t *p_menu)
{
  control_t *p_title = NULL;

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_WIFI_LINK_INFO_TITLE,
                             WIFI_LINK_INFO_TITLE_X, WIFI_LINK_INFO_TITLE_Y,
                             WIFI_LINK_INFO_TITLE_W, WIFI_LINK_INFO_TITLE_H,
                             p_menu, 0);
  text_set_font_style(p_title, FSI_BLACK, FSI_BLACK, FSI_BLACK);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_WIFI_CONN);

}

static void create_wifi_info_items(control_t *p_menu)
{
  control_t *p_ctrl[WIFI_LINK_INFO_ITEM_CNT] = {NULL};
  u16 wifi_info_str[WIFI_LINK_INFO_ITEM_CNT] =
  {IDS_NETWORK_SSID,IDS_ENCRYPT_MODE,IDS_KEY_0_EMP,IDS_SHOW_KEY,IDS_SAVE_KEY};
  u16 wifi_enable_str[2] = {IDS_YES,IDS_NO};
  u16 wifi_type_str[4] = {IDS_NONE, IDS_WIFI_WEP, IDS_WIFI_WPA1, IDS_WIFI_WPA2};
  u16 i , y;
  u8 j;

  y = WIFI_LINK_INFO_EBOX_Y;
  for(i = 0;i < WIFI_LINK_INFO_ITEM_CNT;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_SSID + i),WIFI_LINK_INFO_EBOX_X,y,
          WIFI_LINK_INFO_EBOX_LW, WIFI_LINK_INFO_EBOX_RW,ROOT_ID_WIFI_LINK_INFO);
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], wifi_info_str[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], edit_ssid_proc);

        edit_ssid_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
        if(!is_add_wifi)
        {
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], (u8*)ap_info.essid);
          OS_PRINTF("ebox_set_content_by_ascstr  ap_info.essid is not NULL, == %s!!!!@@\n",ap_info.essid);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        break;
        
      case 1:
        p_ctrl[i] = ui_comm_select_create(p_menu, (u8)(IDC_SSID + i),
                                    WIFI_LINK_INFO_EBOX_X,y,
                                    WIFI_LINK_INFO_EBOX_LW, WIFI_LINK_INFO_EBOX_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], wifi_info_str[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC, 4, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < 4; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, wifi_type_str[j]);
        }

        if(ap_info.EncrypType == IW_ENC_NONE)
        {
          ui_comm_select_set_focus(p_ctrl[i], 0);
        }
        else if(ap_info.EncrypType == IW_ENC_WEP)
        {
          ui_comm_select_set_focus(p_ctrl[i], 1);
        }
        else if(ap_info.EncrypType == IW_ENC_WPA1)
        {
          ui_comm_select_set_focus(p_ctrl[i], 2);
        }
        else if(ap_info.EncrypType == IW_ENC_WPA2)
        {
          ui_comm_select_set_focus(p_ctrl[i], 3);
        }
        else
        {
          ui_comm_select_set_focus(p_ctrl[i], 0);
        }
        ui_comm_select_create_droplist(p_ctrl[i], 4);
        if(!is_add_wifi)
        {
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        break;

      case 2:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_SSID + i),WIFI_LINK_INFO_EBOX_X,y,
          WIFI_LINK_INFO_EBOX_LW, WIFI_LINK_INFO_EBOX_RW,ROOT_ID_WIFI_LINK_INFO);
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], wifi_info_str[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], input_key_proc);

        input_key_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
        if( is_add_wifi == 0 && ap_info.EncrypType == IW_ENC_NONE)
        {
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        }
        if( is_add_wifi == 0 && strcmp((char*)p_wifi_info.essid, ap_info.essid) == 0)
        {
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], p_wifi_info.key);
        }
        break;
      case 3:
        p_ctrl[i] = ui_comm_select_create(p_menu, (u8)(IDC_SSID + i),
                                    WIFI_LINK_INFO_EBOX_X,y,
                                    WIFI_LINK_INFO_EBOX_LW, WIFI_LINK_INFO_EBOX_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], wifi_info_str[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,2, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < 2; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, wifi_enable_str[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_select_create_droplist(p_ctrl[i], 2);
        ui_comm_ctrl_set_keymap(p_ctrl[i], is_show_key_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], is_show_key_proc);
        if(is_add_wifi == 0 && ap_info.EncrypType == IW_ENC_NONE)
        {
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        }
        break;
      case 4:
        p_ctrl[i] = ui_comm_select_create(p_menu, (u8)(IDC_SSID + i),
                                    WIFI_LINK_INFO_EBOX_X,y,
                                    WIFI_LINK_INFO_EBOX_LW, WIFI_LINK_INFO_EBOX_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], wifi_info_str[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,2, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < 2; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, wifi_enable_str[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_select_create_droplist(p_ctrl[i], 2);
        ui_comm_ctrl_set_keymap(p_ctrl[i], is_save_key_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], is_save_key_proc);
        if(is_add_wifi == 0 && ap_info.EncrypType == IW_ENC_NONE)
        {
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        }
        break;
      default:
        break;
    }
    y += WIFI_LINK_INFO_EBOX_H + WIFI_LINK_INFO_ITEM_VGAP;

  ctrl_set_related_id(p_ctrl[i],
                        0,                       /* left */
                        (u8)((i - 1 + WIFI_LINK_INFO_ITEM_CNT)
                             % WIFI_LINK_INFO_ITEM_CNT + 1),    /* up */
                        0,                       /* right */
                        (u8)((i + 1) % WIFI_LINK_INFO_ITEM_CNT + 1));    /* down */

 }

  if(!is_add_wifi)
  {
    ctrl_default_proc(p_ctrl[2], MSG_GETFOCUS, 0, 0);
  }
  else
  {
    ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  }

}

static void check_is_auto_connect()
{
  wifi_connt_stats = ui_get_net_connect_status();
  if(wifi_connt_stats.is_wifi_conn)
  {
    is_connect_curn = TRUE;
  }
  else
  {
    is_connect_curn = FALSE;
  }
}

static void create_wifi_help_bar(control_t *p_menu)
{
  // add help bar p_temp_wifi_info
  if(strcmp((char*)p_temp_wifi_info.essid, (char*)p_wifi_info.essid) == 0 && strcmp((char*)p_wifi_info.essid, ap_info.essid) == 0 && is_connect_curn && !is_add_wifi)
  {
    ui_comm_help_create_for_pop_dlg(&disconn_help_data, p_menu);
  }
  else
  {
    ui_comm_help_create_for_pop_dlg(&conn_help_data, p_menu);
  }
}

static void init_wifi_conn_focus()
{
  g_conn_list_focus = get_wifi_conn_focus();
  OS_PRINTF("#########link info get g_conn_list_focus == %d###\n",g_conn_list_focus);
}
RET_CODE open_wifi_link_info(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  ethernet_device_t* p_wifi_dev = NULL;

  is_add_wifi = para1;
  p_wifi_dev = get_wifi_dev_handle();

  init_wifi_conn_focus();
  list_focus = (u16)para2;
  #ifndef WIN32
  wifi_ap_info_get(p_wifi_dev,list_focus,&ap_info);
  #endif

  sys_status_get_wifi_info(&p_wifi_info,ap_info.essid);
  
  OS_PRINTF("####open_wifi_link_info get para2 == %d, ap_info.ssid == %s####\n", para2, ap_info.essid);
  check_is_auto_connect();
  
  //create menu
  p_menu = create_wifi_info_menu();

  //create title
  create_wifi_info_title(p_menu);

  //create wifi link info items
  create_wifi_info_items(p_menu);

  //create help bar
  create_wifi_help_bar(p_menu);

  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  return SUCCESS;
}

static RET_CODE on_wifi_link_info_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  is_show_key = TRUE;
  is_save_key = TRUE;

  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
  {
    ui_comm_dlg_close();
  }
  
  wifi_connt_stats = ui_get_net_connect_status();
  OS_PRINTF("#########exit,,,,,,,,,,,wifi_connt_stats.is_wifi_conn == %d###\n",wifi_connt_stats.is_wifi_conn);
  if(!wifi_connt_stats.is_wifi_conn)
  {
    paint_connect_net_status(g_conn_list_focus, FALSE);
    memset(p_wifi_info.essid, 0, sizeof(p_wifi_info.essid));
    memset(p_wifi_info.key, 0, sizeof(p_wifi_info.key));
    is_connect_curn = FALSE;
  }

  fw_tmr_destroy(ROOT_ID_WIFI_LINK_INFO, MSG_CONNECT_NETWORK);
  
  manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);
 
  return ERR_NOFEATURE;
}

static void get_ctrls_strings(u16 ctrl_idc, u8 *asc)
{
  control_t *p_ctrl = NULL;
//  u8 i = 0;
  u16 *temp;
  
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_WIFI_LINK_INFO, (u8)ctrl_idc);
  temp = ui_comm_t9_v2_edit_get_content(p_ctrl);
/*  while(temp[i] != 0)
  {
    OS_PRINTF("###the i ==%d temp string is %c##\n",i+1,(u8)temp[i]);
    i++;
  }*/
  if(temp != NULL)
  {
    str_uni2asc(asc, temp);
  }
//  OS_PRINTF("###get strl asc == %s##\n",asc); 
}

static void get_key_essid(u8 *key, u8 *ssid)
{
  get_ctrls_strings(IDC_KEYS, key);
  get_ctrls_strings(IDC_SSID, ssid);
}

static void paint_other_wifi_not_connect()
{
  control_t *p_list = NULL;

  p_list = ui_comm_root_get_ctrl(ROOT_ID_WIFI, 8);//IDC_WIFI_LIST
  paint_wifi_list_field_not_connect(p_list);
}
void do_cmd_to_conn_wifi()
{
  u8 key_asc[MAX_PASS_WORD_LEN+1] = {0};
  u8 ssid_asc[MAX_SSID_LEN+1] = {0};
  ethernet_device_t * p_dev = NULL;
  //lint -e64
  wifi_ap_info_t wap_info = {{0}};
  wifi_encrypt_type_t wifi_encrypt_type = 0;
  //lint +e64
  control_t *p_ctrl = NULL;
  u16 focus = 0;

  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_WIFI_LINK_INFO, IDC_ENCRYPT_MODE);
  p_dev = get_wifi_dev_handle();
  wifi_connt_stats = ui_get_net_connect_status();
  
#ifndef WIN32
  wifi_ap_info_get(p_dev, list_focus, &wap_info);
#endif
  paint_connect_net_status(g_conn_list_focus, FALSE);
  paint_other_wifi_not_connect();
  {
    OS_PRINTF("##########g_conn_list_focus == %d######\n",g_conn_list_focus);
  }
  wifi_connt_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_connt_stats);
  //get key value
  get_key_essid(key_asc, ssid_asc);

  if(is_save_key)
  {
    OS_PRINTF("################is_save_key == %d#################\n",is_save_key);
    strcpy((char*)p_wifi_info.key, (char*)key_asc);
  }
  else
  {
    OS_PRINTF("################is_save_key == %d, will memset 0#################\n",is_save_key);
    memset(p_wifi_info.key, 0 ,sizeof(p_wifi_info.key));
  }
 
  strcpy((char*)p_wifi_info.essid, (char*)ssid_asc);
  //set_temp_wifi_info(p_wifi_info);//add for desktop get connected msg


  focus = ui_comm_select_get_focus(p_ctrl);
  if(focus == 0)
  {
    wifi_encrypt_type = IW_ENC_NONE;
  }
  else if(focus == 1)
  {
    wifi_encrypt_type = IW_ENC_WEP;
  }
  else if(focus == 2)
  {
    wifi_encrypt_type = IW_ENC_WPA1;
  }
  else if(focus == 3)
  {
    wifi_encrypt_type = IW_ENC_WPA2;
  }
  else
  {
    wifi_encrypt_type = IW_ENC_ERROR;
  }
  p_wifi_info.encrypt_type = wifi_encrypt_type;
  if(wifi_encrypt_type == IW_ENC_NONE)
  {
    memset(p_wifi_info.key, 0 ,sizeof(p_wifi_info.key));
    set_temp_wifi_info(p_wifi_info);//add for desktop get connected msg
  }
  
  if(ssid_asc != NULL)
  {
    service_t *p_server = NULL;
    net_svc_cmd_para_t net_svc_para;
    net_svc_t *p_net_svc = NULL;

    p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
    p_net_svc->net_svc_clear_msg(p_net_svc);
    memset(&net_svc_para, 0 , sizeof(net_svc_para));
    net_svc_para.p_eth_dev = p_dev;
    
    memcpy(&net_svc_para.net_cfg, &ethcfg, sizeof(ethernet_cfg_t));
    //strcpy(net_svc_para.net_cfg.wifi_para.key, key_asc);
    //strcpy(net_svc_para.net_cfg.wifi_para.ssid, ssid_asc);
    if(wifi_encrypt_type != IW_ENC_NONE)
    {
      memcpy(net_svc_para.net_cfg.wifi_para.key, key_asc, sizeof(key_asc));
    }
//    OS_PRINTF("###sizeof key is %d##\n",sizeof(key_asc));
    memcpy(net_svc_para.net_cfg.wifi_para.ssid, ssid_asc, sizeof(ssid_asc));
  //  OS_PRINTF("###sizeof ssid is %d##\n",sizeof(ssid_asc));
   // net_svc_para.net_cfg.wifi_para.is_enable_encrypt = ap_info.EncrypType;
    net_svc_para.net_cfg.wifi_para.is_enable_encrypt = wifi_encrypt_type;
    net_svc_para.net_cfg.wifi_para.encrypt_type = wifi_encrypt_type;
    OS_PRINTF("#####################net_svc_para essid == %s###################\n",net_svc_para.net_cfg.wifi_para.ssid);
    OS_PRINTF("#####################net_svc_para key == %s###################\n",net_svc_para.net_cfg.wifi_para.key);
    OS_PRINTF("#####################net_svc_para encrypt_type == %d###################\n",net_svc_para.net_cfg.wifi_para.is_enable_encrypt);
    p_server = (service_t *)ui_get_net_svc_instance();
    p_server->do_cmd(p_server, NET_DO_WIFI_CONNECT, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }

  

}

BOOL is_wifi_static_ip()
{
  net_config_t wifi_net_config;
   sys_status_get_net_config_info(&wifi_net_config);
   if(wifi_net_config.config_mode == STATIC_IP)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}
void init_wifi_ethcfg(ethernet_device_t * p_wifi_dev)
{
   if(is_wifi_static_ip())
   {
      ethcfg =  get_static_wifi_ethcfg();
   }
   else
   {
     ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
     ethcfg.is_enabledhcp = 1;
   }

  //mac address
  dev_io_ctrl(p_wifi_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&ethcfg.hwaddr[0]);
  OS_PRINTF("#######init_wifi_ethcfg#######MAC= %02x-%02x-%02x-%02x-%02x-%02x##############\n",
                          ethcfg.hwaddr[0], ethcfg.hwaddr[1],ethcfg.hwaddr[2],
                          ethcfg.hwaddr[3], ethcfg.hwaddr[4], ethcfg.hwaddr[5]);
//  e0:b2:f1:29:4e:4e

}

void paint_connect_net_status(u16 index, BOOL is_connect)
{

  //MT_ASSERT(index != INVALIDID);
  if(is_connect)
  {
    wifi_connt_stats.is_wifi_conn = TRUE;

    //paint wifi list field connect status
    paint_list_field_is_connect(index, TRUE, TRUE);
    //g_conn_list_focus = index;
    set_wifi_conn_focus(index);
  }
  else
  {
    wifi_connt_stats.is_wifi_conn = FALSE;

    //paint wifi list field connect status
    paint_list_field_is_connect(index, FALSE, TRUE);
  }

  ui_set_net_connect_status(wifi_connt_stats);

  //paint wifi connect status
  paint_wifi_status(wifi_connt_stats.is_wifi_conn, TRUE);

   //paint network connect status
  paint_connect_status(wifi_connt_stats.is_wifi_conn, TRUE);

}

static void disconnect_curn_wifi(ethernet_device_t * p_dev)
{
  comm_dlg_data_t dis_wifi_dlg_data =
  {
    ROOT_ID_WIFI,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_DISCONNECTING_WIFI,
    0,
  };
  #ifndef WIN32
  set_wifi_disconnect(p_dev);
  #endif
  ui_comm_dlg_open(&dis_wifi_dlg_data);
 /* paint_connect_net_status(list_focus, FALSE);
  memset(p_wifi_info.essid, 0, sizeof(p_wifi_info.essid));
  memset(p_wifi_info.key, 0, sizeof(p_wifi_info.key));
   is_connect_curn = FALSE;
   wifi_connt_stats.is_wifi_conn = FALSE;
   ui_set_net_connect_status(wifi_connt_stats);
  manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);*/
}

static void pop_up_connecting_dlg()
{
  comm_dlg_data_t conn_dlg_data =
  {
    ROOT_ID_WIFI_LINK_INFO,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_CONNECTING_WIFI,
    0,
  };

  ui_comm_dlg_open(&conn_dlg_data);
}

static RET_CODE on_network_wifi_connect(control_t *p_cont, u16 msg,
                                                u32 para1, u32 para2)
{
    ethernet_device_t * p_dev = get_wifi_dev_handle();
    net_conn_info_t net_conn_info = {0}; 
    net_conn_info = ui_get_conn_info();

    if((net_conn_info.gprs_conn_info < MODEM_DEVICE_NETWORK_START_DIAL) &&
      !((net_conn_info.g3_conn_info > MODEM_DEVICE_NETWORK_UNCONNECTED)&&(net_conn_info.g3_conn_info <= MODEM_DEVICE_NETWORK_CONNECTED)))
    {
        fw_tmr_destroy(ROOT_ID_WIFI_LINK_INFO, MSG_CONNECT_NETWORK);
        pop_up_connecting_dlg();
        init_wifi_ethcfg(p_dev);
        do_cmd_to_conn_wifi();
    }

    return SUCCESS;
}

static RET_CODE on_connect_wifi(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  net_conn_info_t net_conn_info = {0};
  ethernet_device_t * p_dev = get_wifi_dev_handle();
  if(p_dev != NULL)
  {
    #ifndef WIN32
   // if(strcmp(p_wifi_info.essid, ap_info.essid) == 0 && is_connect_curn && !is_add_wifi)
    if(strcmp((char*)p_temp_wifi_info.essid, ap_info.essid) == 0 && is_connect_curn && !is_add_wifi)
    {
      disconnect_curn_wifi(p_dev);
      return SUCCESS;
    }
    net_conn_info = ui_get_conn_info(); 
    if (net_conn_info.gprs_conn_info >= MODEM_DEVICE_NETWORK_START_DIAL) 
    {
       do_cmd_disconnect_gprs();
    }

    if (net_conn_info.g3_conn_info >= MODEM_DEVICE_NETWORK_START_DIAL) 
    {
       do_cmd_disconnect_g3();
    }   
    fw_tmr_create(ROOT_ID_WIFI_LINK_INFO, MSG_CONNECT_NETWORK, SECOND, TRUE);
      
    #endif
  }

  return SUCCESS;
}

static void save_ip_address()
{
      
      u8 ipaddr[20] = {0};
      u8 netmask[20] = {0};
      u8 gw[20] = {0};
      u8 primarydns[20] = {0};
      u8 alternatedns[20] = {0};
      u8 ipaddress[32] = {0};
      ip_address_set_t ss_ip = {{0}};
      ip_address_t addr = {0,};
     ethernet_device_t* p_wifi_devv = NULL; 
      //p_wifi_devv = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);//add for wifi ui get wifi connected msg
	#ifndef WIN32
      //lint -e746
      p_wifi_devv = get_wifi_handle();
      //lint +e746
      MT_ASSERT(p_wifi_devv != NULL);
	  #endif
#ifndef WIN32
      get_net_device_addr_info(p_wifi_devv, (char*)ipaddr, (char*)netmask, (char*)gw, (char*)primarydns, (char*)alternatedns);
#endif
  
      get_addr_param(ipaddr, (ip_address_t *)&addr);
      memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
      sprintf((char*)ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
      OS_PRINTF("###wifi auto connect  get dhcp ip address value is:%s####\n",ipaddress);
      get_addr_param(netmask, (ip_address_t *)&addr);
      memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
      sprintf((char*)ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
      OS_PRINTF("###wifi auto connect  get dhcp netmask value is:%s####\n",ipaddress);
      get_addr_param(gw, (ip_address_t *)&addr);
      memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
      sprintf((char*)ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
      OS_PRINTF("###wifi auto connect  get dhcp gateway  value is:%s####\n",ipaddress);
      get_addr_param(primarydns, (ip_address_t *)&addr);
      memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
      sprintf((char*)ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
      OS_PRINTF("###wifi auto connect  get dhcp dns value is:%s####\n",ipaddress);
      sys_status_set_ipaddress(&ss_ip);
      sys_status_save();

}

static RET_CODE on_connect_wifi_ok(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
//  ethernet_device_t * p_wifi_dev = get_wifi_dev_handle();

  OS_PRINTF("###connect wifi successfully ####\n");
  paint_connect_net_status(list_focus, TRUE);
  sys_status_set_wifi_info(&p_wifi_info);
  sys_status_save();
  set_temp_wifi_info(p_wifi_info);
  is_connect_curn = TRUE;
  wifi_connt_stats.is_wifi_conn = TRUE;
  wifi_connt_stats.is_eth_conn = FALSE;
  ui_set_net_connect_status(wifi_connt_stats);
  if(!is_wifi_static_ip())
  {
    OS_PRINTF("####wifi connect ok, dhcp####\n");
  //on_config_ip_update(p_wifi_dev);//update wifi dhcp ipaddress
  }
  else
  {
    OS_PRINTF("####wifi connect ok, static ip####\n");
  }
  save_ip_address();
  ui_comm_dlg_close();
  manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);
  
  paint_connect_wifi_status(TRUE, FALSE);

  return SUCCESS;

}

static RET_CODE on_connect_wifi_failed(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  paint_connect_net_status(list_focus, FALSE);
  is_connect_curn = FALSE;
  wifi_connt_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_connt_stats);
  ui_comm_dlg_close();
  manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);
  paint_connect_wifi_status(FALSE, FALSE);
  return SUCCESS;

}

static RET_CODE on_is_show_key(control_t *p_cbox, u16 msg,u32 para1, u32 para2)
{
  control_t *p_keys = NULL;
  control_t *p_show_key = NULL;
  control_t *p_ebox = NULL;
  u16 focus = 0;

  p_show_key = ctrl_get_parent(p_cbox);
  p_keys = ctrl_get_child_by_id(ctrl_get_parent(p_show_key), IDC_KEYS);
  p_ebox = ctrl_get_child_by_id(p_keys, 2);
  focus = ui_comm_select_get_focus(p_show_key);

  if(1 == focus)
  {
    is_show_key = TRUE;
    ebox_set_worktype(p_ebox, EBOX_WORKTYPE_HIDE);
    ctrl_paint_ctrl(p_ebox,TRUE);
  }
  else
  {
    ebox_set_worktype(p_ebox, EBOX_WORKTYPE_EDIT);
    is_show_key = FALSE;
    ctrl_paint_ctrl(p_ebox,TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_is_save_key(control_t *p_cbox, u16 msg,u32 para1, u32 para2)
{
  control_t *p_save_key = NULL;
  u16 focus = 0;

  p_save_key = ctrl_get_parent(p_cbox);
  focus = ui_comm_select_get_focus(p_save_key);

  if(1 == focus)
  {
    is_save_key = FALSE;
  }
  else
  {
    is_save_key = TRUE;
  }

  return SUCCESS;
}

//void edit_ssid_update(void)
RET_CODE edit_ssid_update(u16* p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_WIFI_LINK_INFO, IDC_SSID);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  return SUCCESS;
}

//void edit_mode_update(void)
RET_CODE edit_mode_update(u16* p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_WIFI_LINK_INFO, IDC_ENCRYPT_MODE);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  
  return SUCCESS;
}

//void input_key_update(void)
RET_CODE input_key_update(u16* p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_WIFI_LINK_INFO, IDC_KEYS);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_edit_ssid(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = edit_ssid_param.type;
  param.max_len = MAX_SSID_LEN;
  param.cb = edit_ssid_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}

static RET_CODE on_edit_encrypt_mode(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = edit_mode_param.type;
  param.max_len = DB_DVBS_MAX_NAME_LENGTH;
  param.cb = edit_mode_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}

static RET_CODE on_input_key(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;
  control_t *p_ebox;
  if(!is_show_key)
  {
    ebox_set_worktype(p_ctrl, EBOX_WORKTYPE_HIDE);
  }
  p_ebox = ctrl_get_parent(p_ctrl);
  param.uni_str = ui_comm_t9_v2_edit_get_content(p_ebox);
  param.type = input_key_param.type;
  param.max_len = MAX_PASS_WORD_LEN;
  param.cb = input_key_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);

  return SUCCESS;
}

BEGIN_KEYMAP(wifi_link_info_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_BLUE, MSG_EXIT)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(wifi_link_info_cont_keymap, NULL)

BEGIN_MSGPROC(wifi_link_info_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_wifi_link_info_exit)
  ON_COMMAND(MSG_RED, on_connect_wifi)
  ON_COMMAND(MSG_WIFI_AP_CONNECT, on_connect_wifi_ok)
  ON_COMMAND(MSG_WIFI_AP_DISCONNECT, on_connect_wifi_failed)
  ON_COMMAND(MSG_CONNECT_NETWORK, on_network_wifi_connect)
END_MSGPROC(wifi_link_info_cont_proc, cont_class_proc)

//edit ssid
BEGIN_MSGPROC(edit_ssid_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_ssid)
  ON_COMMAND(MSG_NUMBER, on_edit_ssid)
END_MSGPROC(edit_ssid_proc, ui_comm_edit_proc)

//edit encrypt mode
BEGIN_MSGPROC(edit_mode_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_encrypt_mode)
  ON_COMMAND(MSG_NUMBER, on_edit_encrypt_mode)
END_MSGPROC(edit_mode_proc, ui_comm_edit_proc)

//input key
BEGIN_MSGPROC(input_key_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_input_key)
  ON_COMMAND(MSG_NUMBER, on_input_key)
END_MSGPROC(input_key_proc, ui_comm_edit_proc)

//is show key
BEGIN_KEYMAP(is_show_key_keymap, NULL)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
END_KEYMAP(is_show_key_keymap, NULL)

BEGIN_MSGPROC(is_show_key_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_is_show_key)
END_MSGPROC(is_show_key_proc, cbox_class_proc)

//is save key
BEGIN_KEYMAP(is_save_key_keymap, NULL)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
END_KEYMAP(is_save_key_keymap, NULL)

BEGIN_MSGPROC(is_save_key_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_is_save_key)
END_MSGPROC(is_save_key_proc, cbox_class_proc)
