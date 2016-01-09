/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_network_config.h"
#include "ethernet.h"
#include "ui_wifi.h"
#include "wifi.h"
#include "ui_keyboard_v2.h"

#ifndef WIN32
 #include "ppp.h"
 #include "modem.h"
#endif

enum ip_ctrl_id
{
  IDC_CONNECT_TYPE = 1,
  IDC_CNFIG_MODE,
  IDC_IP_ADDRESS,
  IDC_NETMASK,
  IDC_GATEWAY,
  IDC_DNS_SERVER,
  IDC_MAC_ADDRESS,
  IDC_PING_TEST,
  IDC_STORAGE_PATH,
  IDC_CONNECT_NETWORK,
  
  IDC_CONNECT_STATUS_TXT,
  IDC_CONNECT_STATUS,
  
  IDC_GPRS_APN,
  IDC_GPRS_DIAL,
  IDC_GPRS_IP,
  IDC_GPRS_MASK,
  IDC_GPRS_DNS_PREFER,
  IDC_GPRS_DNS_ALTER,
  IDC_GPRS_SIGNAL,
  IDC_GPRS_PING_TEST,
  IDC_GPRS_CONNECT,    
};

u16 network_config_cont_keymap(u16 key);
u16 ping_test_keymap(u16 key);
u16 connect_network_keymap(u16 key);
u16 link_type_keymap(u16 key);
u16 gprs_ping_test_keymap(u16 key);
u16 gprs_connect_keymap(u16 key);

RET_CODE network_config_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE ping_test_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE network_mode_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE connect_network_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE link_type_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_apn_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_dial_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_ping_test_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_connect_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

extern void init_ethernet_ethcfg(ethernet_cfg_t *ethcfg);
extern void auto_connect_ethernet();
extern void do_cmd_connect_gprs();
extern void do_cmd_disconnect_gprs();

void do_cmd_connect_network(ethernet_cfg_t *ethcfg, ethernet_device_t * eth_dev);

static u8 ip_separator[IPBOX_MAX_ITEM_NUM] = {'.', '.', '.', ' '};
static partition_t *p_partition = NULL;
static u32 g_partition_count = 0;
//static BOOL is_enable_dhcp = TRUE; 
static ethernet_cfg_t      ethcfg = {0};
static ethernet_cfg_t      static_wifi_ethcfg = {0};
static ip_address_set_t ss_ip = {{0}};
//static link_type_t link_type = LINK_TYPE_LAN;
static net_config_t g_net_config;
static net_conn_stats_t eth_connt_stats;
static BOOL gprs_conn_btn_press = FALSE;
static u8 g_gprs_status[32] = {0};

static  void ssdata_ip_address_get()
{
  sys_status_get_ipaddress(&ss_ip);
}

static void ssdata_ip_address_set()
{
  sys_status_set_ipaddress(&ss_ip);
  sys_status_save();
}

static RET_CODE fill_partition_name(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
  uni_strncpy(p_str, p_partition[focus].name, max_length);
  return SUCCESS;
}

static control_t *create_network_config_menu()
{
   control_t *p_menu = NULL;

   p_menu = ui_comm_root_create_netmenu(ROOT_ID_NETWORK_CONFIG,0, IM_INDEX_NETWORK_BANNER, IDS_NETWORK_CONFIG);
  
  ctrl_set_keymap(p_menu, network_config_cont_keymap);
  ctrl_set_proc(p_menu, network_config_cont_proc);

  return p_menu;
}

static void create_config_ctrl_items(control_t *p_menu)
{
  control_t *p_ctrl[IP_CTRL_CNT] = { NULL };
  u16 network_str[CONFIG_MODE_TYPE_CNT] = { IDS_DHCP, IDS_STATIC_IP, IDS_PPPOE };
  u16 network_str_wifi[CONFIG_MODE_TYPE_CNT - 1] = { IDS_DHCP, IDS_STATIC_IP};
  u16 type_str[3] = { IDS_LAN, IDS_WIFI, IDS_GPRS};
  u16 ctrl_str[IP_CTRL_CNT] =
  {
    IDS_LINK_TYPE,IDS_CONFIG_MODE,IDS_IP_ADDRESS,IDS_NETMASK,IDS_GATEWAY,
    IDS_DNS_SREVER, IDS_DHCP_SERVER,IDS_PING_TEST,IDS_PREFERRED_STORAGE,0
  };
  ip_address_t temp_address = {0};
  u8 macaddr[128] = {0};
  u8 mac[6][2] = {{0},};
  u16 i, j, y;

  sys_status_get_net_config_info(&g_net_config);
  y = NETWORK_ITEM_Y;
  for(i = 0;i < IP_CTRL_CNT;i++)
  {
     switch(i)
      {
        case 0: 
          p_ctrl[i] = ui_comm_select_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
	       ui_comm_ctrl_set_proc(p_ctrl[i], link_type_proc);
          ui_comm_select_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,1, CBOX_ITEM_STRTYPE_STRID, NULL);
          //for(j = 0;j < 3;j++)
          //{
          ui_comm_select_set_content(p_ctrl[i],0,type_str[g_net_config.link_type]);
          //}
          if(g_net_config.link_type == LINK_TYPE_LAN)
          {
            ui_comm_select_set_focus(p_ctrl[i], 0);
          }
          else if(g_net_config.link_type == LINK_TYPE_WIFI)
          {
            ui_comm_select_set_focus(p_ctrl[i], 1);
          }
          else
          {
            ui_comm_select_set_focus(p_ctrl[i], 2);
          }
          ui_comm_select_create_droplist(p_ctrl[i], 3);
			break;		
        case 1:
          p_ctrl[i] = ui_comm_select_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_select_set_static_txt(p_ctrl[i], ctrl_str[i]);
          if(g_net_config.link_type == LINK_TYPE_WIFI)
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT - 1, CBOX_ITEM_STRTYPE_STRID, NULL);
            for(j = 0;j < CONFIG_MODE_TYPE_CNT - 1;j++)
            {
               ui_comm_select_set_content(p_ctrl[i], j, network_str_wifi[j]);
            }
            if(g_net_config.config_mode == DHCP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 0);
            }
            else if(g_net_config.config_mode == STATIC_IP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 1);
            }
            
            ui_comm_ctrl_set_proc(p_ctrl[i],network_mode_proc);
     //       ui_comm_select_create_droplist(p_ctrl[i], CONFIG_MODE_TYPE_CNT - 1);
          }
          else
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
            for(j = 0;j < CONFIG_MODE_TYPE_CNT;j++)
            {
               ui_comm_select_set_content(p_ctrl[i], j, network_str[j]);
            }
            OS_PRINTF("####open func   is_enable_dhcp == %d########\n", g_net_config.config_mode);
            if(g_net_config.config_mode == DHCP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 0);
            }
            else if(g_net_config.config_mode == STATIC_IP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 1);
            }
            else 
            {
              ui_comm_select_set_focus(p_ctrl[i], 2);
            }
            ui_comm_ctrl_set_proc(p_ctrl[i],network_mode_proc);
            
          }
          ui_comm_select_create_droplist(p_ctrl[i], CONFIG_MODE_TYPE_CNT);
			break;		
        case 2:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_ipaddress), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 3:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_netmask), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 4:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_gateway), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 5:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 6:
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          ui_comm_static_set_static_txt(p_ctrl[i], IDS_MAC_ADDR);//MAC ADDRESS
          #ifndef WIN32
          ethcfg.hwaddr[0] = sys_status_get_mac_by_index(0);
          ethcfg.hwaddr[1] = sys_status_get_mac_by_index(1);
          ethcfg.hwaddr[2] = sys_status_get_mac_by_index(2);
          ethcfg.hwaddr[3] = sys_status_get_mac_by_index(3);
          ethcfg.hwaddr[4] = sys_status_get_mac_by_index(4);
          ethcfg.hwaddr[5] = sys_status_get_mac_by_index(5);
/*            
          ethcfg.hwaddr[0] = 0xD8;
          ethcfg.hwaddr[1] = 0xA5;
          ethcfg.hwaddr[2] = 0xB7;
          ethcfg.hwaddr[3] = 0x80;
          ethcfg.hwaddr[4] = 0x01;
          ethcfg.hwaddr[5] = 0x00;
*/          
          OS_PRINTF("lou 9999999999999999999   %x:%x:%x:%x:%x:%x\n ",
                    ethcfg.hwaddr[0] ,ethcfg.hwaddr[1],
                    ethcfg.hwaddr[2] ,ethcfg.hwaddr[3],
                    ethcfg.hwaddr[4] ,ethcfg.hwaddr[5]);
          for(j = 0; j < 6; j ++)  
          {
           sys_status_get_mac(j, mac[j]);
           strcat(macaddr,mac[j]);
           if(j != 5)
           {
           strcat(macaddr,":");
           }
          }
          macaddr[17]='\0';
         // strcpy(macaddr, "D8:A5:B7:80:01:01");
         #endif
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], macaddr);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
        break;
        case 7:
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ctrl_set_keymap(p_ctrl[i], ping_test_keymap);
          ctrl_set_proc(p_ctrl[i], ping_test_proc);
        break;
       case 8:
          p_ctrl[i] = ui_comm_select_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          g_partition_count = file_list_get_partition(&p_partition);
          ui_comm_select_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_select_set_focus(p_ctrl[i], 0);
         if(g_partition_count)
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,g_partition_count, CBOX_ITEM_STRTYPE_UNICODE, fill_partition_name);
         //   ui_comm_select_create_droplist(p_ctrl[i], 4);
          }
          else
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,1, 
            CBOX_ITEM_STRTYPE_STRID, NULL);
            ui_comm_select_set_content(p_ctrl[i], 0, IDS_NO_DEVICE);
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
         break;
       case 9:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW+NETWORK_ITEM_RW, COMM_CTRL_H, p_menu, 0);
        ctrl_set_keymap(p_ctrl[i], connect_network_keymap);
        ctrl_set_proc(p_ctrl[i], connect_network_proc);
        ctrl_set_rstyle(p_ctrl[i], RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
        text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
        text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        if(g_net_config.link_type == LINK_TYPE_LAN && g_net_config.config_mode != PPPOE)
        {
          text_set_content_by_strid(p_ctrl[i], IDS_CONNECT);
        }
		  else if(g_net_config.link_type == LINK_TYPE_LAN && g_net_config.config_mode == PPPOE)
        {
          text_set_content_by_strid(p_ctrl[i], IDS_PPPOE_CONN);
        }
        else if(g_net_config.link_type == LINK_TYPE_WIFI)
        {
          text_set_content_by_strid(p_ctrl[i], IDS_SEARCH);
        }
        
        break;
       
      }
      y += NETWORK_ITEM_H + NETWORK_ITEM_VGAP;
      ctrl_set_related_id(p_ctrl[i],
                        0,                       /* left */
                        (u8)((i - 1 + IP_CTRL_CNT)
                             % IP_CTRL_CNT + 1),    /* up */
                        0,                       /* right */
                        (u8)((i + 1) % IP_CTRL_CNT + 1));    /* down */
  }

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  
}

void paint_connect_status(BOOL is_conn, BOOL is_paint)
{
  control_t *p_conn_status = NULL;
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG);
  
  if(root == NULL)
  {
    OS_PRINTF("########not need to paint connect status , is_conn == %d####\n", is_conn);
    return ;
  }
  OS_PRINTF("########need to paint connect status , is_conn == %d####\n", is_conn);
  p_conn_status = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_CONNECT_STATUS);

  if(is_conn)
  {
    text_set_font_style(p_conn_status, FSI_GREEN, FSI_GREEN, FSI_GREEN);
    text_set_content_by_strid(p_conn_status, IDS_CONNECT);
  }
  else
  {
    text_set_font_style(p_conn_status, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_by_strid(p_conn_status, IDS_DISCONNECT);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_conn_status, TRUE);
  }
  
}

static void create_connect_status_ctrl(control_t *p_menu)
{
  control_t *p_status_txt = NULL, *p_status = NULL;

  p_status_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS_TXT, NETWORK_CONNECT_STATUS_TEXTX, 
  NETWORK_CONNECT_STATUS_TEXTY, NETWORK_CONNECT_STATUS_TEXTW, NETWORK_CONNECT_STATUS_TEXTH, p_menu, 0);
  
  ctrl_set_rstyle(p_status_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_status_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_status_txt, FSI_WHITE, FSI_BLACK, FSI_WHITE);
  text_set_content_type(p_status_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_status_txt, IDS_NETWORK_STATUS);

  p_status = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS, NETWORK_CONNECT_STATUSX, 
  NETWORK_CONNECT_STATUSY, NETWORK_CONNECT_STATUSW, NETWORK_CONNECT_STATUSH, p_menu, 0);
  
  ctrl_set_rstyle(p_status, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_status, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_status, TEXT_STRTYPE_STRID);

  eth_connt_stats = ui_get_net_connect_status();
  OS_PRINTF("################print connect status, %d,%d,%d,%d############", eth_connt_stats.is_eth_conn,eth_connt_stats.is_eth_insert,eth_connt_stats.is_wifi_conn,eth_connt_stats.is_wifi_insert);
  if(!eth_connt_stats.is_eth_conn && !eth_connt_stats.is_wifi_conn)
  {
    paint_connect_status(FALSE, FALSE);
  }
  else
  {
    paint_connect_status(TRUE, FALSE);
  }
  
}


static void ctrl_gprs_cfg_show(control_t *p_menu, BOOL b_gprs_show)
{
  control_t *p_ctrl = NULL;
  obj_sts_t obj_sts = 0;

  if(b_gprs_show == TRUE)
  {
    obj_sts = OBJ_STS_HIDE;
  }
  else
  {
    obj_sts = OBJ_STS_SHOW;
  }
  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_CNFIG_MODE);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_IP_ADDRESS);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_NETMASK);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GATEWAY);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_DNS_SERVER);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_MAC_ADDRESS);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_PING_TEST);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_STORAGE_PATH);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_CONNECT_NETWORK);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_CONNECT_STATUS_TXT);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_CONNECT_STATUS);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }  

  //
  if(b_gprs_show == TRUE)
  {
    obj_sts = OBJ_STS_SHOW;
  }
  else
  {
    obj_sts = OBJ_STS_HIDE;
  }  
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_APN);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  }    
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_DIAL);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_IP);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_MASK);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_DNS_PREFER);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_DNS_ALTER);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_SIGNAL);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_PING_TEST);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 
  p_ctrl = ctrl_get_child_by_id(p_menu, IDC_GPRS_CONNECT);
  if(p_ctrl != NULL)
  {
    ctrl_set_sts(p_ctrl, obj_sts);
  } 

  if(obj_sts == OBJ_STS_SHOW)
  {
     if(gprs_conn_btn_press)
     {
        text_set_content_by_strid(p_ctrl, IDS_DISCONNECT); 
     }
     else
     {
       text_set_content_by_strid(p_ctrl, IDS_CONNECT); 
     }
  }
  
  ctrl_paint_ctrl(p_menu, TRUE);
}


static RET_CODE on_gprs_ping_test_select(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
    manage_open_menu(ROOT_ID_PING_TEST, 0, 0);
    
    return SUCCESS;
}                            

static RET_CODE on_gprs_connect(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{ 
  control_t  *p_ctrl_conn = NULL, *p_ctrl_apn = NULL, *p_ctrl_dial = NULL;
  gprs_info_t gprs_info = {{0},{0}};
  u16 *p_uni_apn = NULL, *p_uni_dail = NULL;
  u8 asc_apn[32 + 1] = {0}, asc_dial[32 + 1] = {0};
  u32 apn_len = 0, dial_len = 0;

  p_ctrl_conn = ctrl_get_parent(p_ctrl);
  p_ctrl_apn = ctrl_get_child_by_id(p_ctrl_conn, IDC_GPRS_APN);
  p_ctrl_dial = ctrl_get_child_by_id(p_ctrl_conn, IDC_GPRS_DIAL);

  p_uni_apn = ui_comm_t9_v2_edit_get_content(p_ctrl_apn);
  p_uni_dail = ui_comm_t9_v2_edit_get_content(p_ctrl_dial);
  if((p_uni_apn == NULL) || (p_uni_dail == NULL))
  {
     return ERR_FAILURE;
  }
  
  apn_len = str_uni2asc(asc_apn, p_uni_apn);
  dial_len = str_uni2asc(asc_dial, p_uni_dail);
  
  if(gprs_conn_btn_press == FALSE)
  {    
     memcpy(&gprs_info.apn, &asc_apn, apn_len);
     memcpy(&gprs_info.dial_num, &asc_dial, dial_len);
     
     do_cmd_connect_gprs(&gprs_info);
     gprs_conn_btn_press = TRUE;
     text_set_content_by_strid(p_ctrl, IDS_DISCONNECT); 
  }
  else
  {
     do_cmd_disconnect_gprs();
     gprs_conn_btn_press = FALSE;
     text_set_content_by_strid(p_ctrl, IDS_CONNECT); 
  }

  ctrl_paint_ctrl(p_ctrl, TRUE);
  
  return SUCCESS;
} 
 

//static void edit_gprs_apn_update(void)
static RET_CODE edit_gprs_apn_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_APN);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_gprs_edit_apn(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_gprs_apn_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

//static void edit_gprs_dial_update(void)
static RET_CODE edit_gprs_dial_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_DIAL);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_gprs_edit_dial(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_gprs_dial_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

static void create_gprs_items(control_t *p_menu)
{
  control_t *p_ctrl[GPRS_CTRL_CNT] = { NULL };
  u16 i = 0, j = 0, y = 0;
  u16 ctrl_str[IP_CTRL_CNT] =
  {
    IDS_GPRS_APN, IDS_GPRS_DIAL, IDS_IP_ADDRESS, IDS_NETMASK,IDS_GATEWAY,
    IDS_DHCP_SERVER, IDS_GPRS_SIGNAL_STATUS, IDS_PING_TEST, IDS_PREFERRED_STORAGE, 0
  };  
  ip_address_t temp_address = {0};

  y = NETWORK_ITEM_Y;
  y += NETWORK_ITEM_H + NETWORK_ITEM_VGAP;
  for(i = 0; i < GPRS_CTRL_CNT; i++)
  {
     switch(i)
      {
        case 0:  //apn 
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, IDC_GPRS_APN + i, 
                                                NETWORK_ITEM_X, y, NETWORK_ITEM_LW, 
                                                NETWORK_ITEM_RW, ROOT_ID_NETWORK_CONFIG);                                             
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], gprs_apn_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], "CMNET");
          
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;

        case 1:  //dial       
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, IDC_GPRS_APN + i, 
                                                NETWORK_ITEM_X, y, NETWORK_ITEM_LW, 
                                                NETWORK_ITEM_RW, ROOT_ID_NETWORK_CONFIG);
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], gprs_dial_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], "*99#");
          
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;

        case 2:     //ip address
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_GPRS_APN + i, 
                                            NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2 
                                   | IPBOX_S_B3 | IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_ipaddress), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;
        
        case 3:    //subnet mask
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_GPRS_APN + i, 
                                            NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2 | IPBOX_S_B3 
                                  | IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_netmask), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;
        
        case 4:  //DNS Preferred
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_GPRS_APN + i, 
                                            NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2 | IPBOX_S_B3 
                                   | IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_gateway), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;
        
        case 5:  //DNS Alternet
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_GPRS_APN + i, NETWORK_ITEM_X, 
                                            y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2 | IPBOX_S_B3 
                                   | IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;  

        case 6:   //signal status
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_GPRS_APN + i,NETWORK_ITEM_X, 
                                            y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], g_gprs_status);
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;

        case 7:  //ping test
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_GPRS_APN + i, 
                                            NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ctrl_set_keymap(p_ctrl[i], gprs_ping_test_keymap);
          ctrl_set_proc(p_ctrl[i], gprs_ping_test_proc);   
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;

        case 8:  //connect
          p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_GPRS_APN + i, NETWORK_ITEM_X, 
                                       y, NETWORK_ITEM_LW+NETWORK_ITEM_RW, COMM_CTRL_H, p_menu, 0);
          ctrl_set_rstyle(p_ctrl[i], RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
          text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
          text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
          text_set_content_by_strid(p_ctrl[i], IDS_CONNECT);  
          ctrl_set_keymap(p_ctrl[i], gprs_connect_keymap);
          ctrl_set_proc(p_ctrl[i], gprs_connect_proc);   
          ctrl_set_sts(p_ctrl[i], OBJ_STS_HIDE);
        break;
        
      }

      y += NETWORK_ITEM_H + NETWORK_ITEM_VGAP;
      if(p_ctrl[i] != NULL)
      {
          if(i == 0)
          {
             ctrl_set_related_id(p_ctrl[i], 0, IDC_CONNECT_TYPE, 0, IDC_GPRS_DIAL);
          }
          else
          {
              ctrl_set_related_id(p_ctrl[i],
                                  0,                       /* left */
                                  (u8)((i + 12 - 1 + 21)
                                       % (21)+ 1),    /* up */
                                  0,                       /* right */
                                  (u8)((i + 12 + 1) % 21 + 1));    /* down */   
          }
        
      }

  }


}


RET_CODE open_network_config(u32 para1, u32 para2)
{
  control_t *p_menu = NULL, *p_link_type = NULL;
  
  ssdata_ip_address_get();
  
  //create main menu
  p_menu = create_network_config_menu();

  //create config items
  create_config_ctrl_items(p_menu);
  
  //create network connect status
  create_connect_status_ctrl(p_menu);

  //create gprs items
  create_gprs_items(p_menu);

  if((g_net_config.link_type == LINK_TYPE_GPRS))
  {
     ctrl_gprs_cfg_show(p_menu, TRUE);

     p_link_type = ctrl_get_child_by_id(p_menu, IDC_CONNECT_TYPE);
     if(p_link_type != NULL)
     {
        ctrl_set_related_id(p_link_type, 0, IDC_GPRS_CONNECT, 0, IDC_GPRS_APN);
     }
  } 

  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);
  
  return SUCCESS;

}


static RET_CODE on_config_test_select(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
  manage_open_menu(ROOT_ID_PING_TEST, 0, 0);
  
  return SUCCESS;
}

static void static_ip_to_dhcp()
{
  ip_address_t temp_address = {0};
  control_t *p_ctrl[4] = {NULL};
  u16 i;
  
  ssdata_ip_address_get();
  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_IP_ADDRESS);
        memcpy(&temp_address, &(ss_ip.sys_ipaddress), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 1:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_NETMASK);
        memcpy(&temp_address, &(ss_ip.sys_netmask), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 2:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GATEWAY);
        memcpy(&temp_address, &(ss_ip.sys_gateway), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 3:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG,IDC_DNS_SERVER);
        memcpy(&temp_address, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;   
     default:
        break;
    }
  }
  
}

static void update_ipbox_attr(BOOL is_enable)
{
  u16 i = 0;
  control_t *p_ctrl[4] = {NULL};
  for(i = 0;i < 4;i++)
  {
    p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_IP_ADDRESS + i);
    ui_comm_ctrl_update_attr(p_ctrl[i], is_enable); 
    ctrl_paint_ctrl(p_ctrl[i], TRUE);
  }
}

static RET_CODE on_network_mode_change(control_t *p_cbox, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  control_t *p_ctrl = NULL;
  
  focus = ui_comm_select_get_focus(p_cbox->p_parent);
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_CONNECT_NETWORK);
  switch(focus)
  {
    case 0:
      g_net_config.config_mode = DHCP; 
      static_ip_to_dhcp();
      update_ipbox_attr(FALSE);
      if(g_net_config.link_type == LINK_TYPE_WIFI)
      {
        text_set_content_by_strid(p_ctrl, IDS_SEARCH);
      }
      else
      {
        text_set_content_by_strid(p_ctrl, IDS_CONNECT);
      }
      break;
    case 1:
      g_net_config.config_mode = STATIC_IP; 
      update_ipbox_attr(TRUE);
      break;
    case 2:
    if(g_net_config.link_type == LINK_TYPE_WIFI)
      return ERR_FAILURE;
      g_net_config.config_mode = PPPOE; 
      update_ipbox_attr(FALSE);
      if(g_net_config.link_type == LINK_TYPE_LAN)
      {
        text_set_content_by_strid(p_ctrl, IDS_PPPOE_CONN);
      }
      break;
    default:
      break;
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();
  
  return SUCCESS;
}


void get_addr_param(u8 *p_buffer, ip_address_t *p_addr)
{

  p_addr->s_b1 = p_buffer[0];
  p_addr->s_b2 = p_buffer[1];
  p_addr->s_b3 = p_buffer[2];
  p_addr->s_b4 = p_buffer[3];
}

void on_config_ip_update(ethernet_device_t * p_dev)
{
  ip_address_t addr = {0,};
  control_t *p_ctrl[4] = {NULL};
  u8 ipaddress[32] = {0};
  u8 i;

#ifndef WIN32
  u8 ipaddr[20] = {0};
  u8 netmask[20] = {0};
  u8 gw[20] = {0};
  u8 primarydns[20] = {0};
  u8 alternatedns[20] = {0};
  
  get_net_device_addr_info(p_dev, ipaddr, netmask, gw, primarydns, alternatedns);

#endif

  if(g_net_config.link_type == LINK_TYPE_GPRS)
  {
      for(i = 0;i < 4;i++)
      {
        switch(i)
        {
          case 0:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_IP);
            #ifndef WIN32
            get_addr_param(ipaddr, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp ip address value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 1:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_MASK);
            #ifndef WIN32
            get_addr_param(netmask, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp netmask value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 2:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_DNS_PREFER);
            #ifndef WIN32
            get_addr_param(gw, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp gateway value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 3:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG,IDC_GPRS_DNS_ALTER);
            #ifndef WIN32
            get_addr_param(primarydns, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp dns server value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;   
         default:
            break;
        }
      }
  }
  else
  {
      for(i = 0;i < 4;i++)
      {
        switch(i)
        {
          case 0:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_IP_ADDRESS);
            #ifndef WIN32
            get_addr_param(ipaddr, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp ip address value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 1:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_NETMASK);
            #ifndef WIN32
            get_addr_param(netmask, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp netmask value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 2:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GATEWAY);
            #ifndef WIN32
            get_addr_param(gw, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp gateway value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 3:
            p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG,IDC_DNS_SERVER);
            #ifndef WIN32
            get_addr_param(primarydns, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp dns server value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;   
         default:
            break;
        }
      }
  }

  ssdata_ip_address_set();
  
}



BOOL check_is_connect_ok(ethernet_device_t *p_eth_dev)
{
  u8 ipaddr[20] = {0};
  u8 netmask[20] = {0};
  u8 gw[20] = {0};
  u8 primarydns[20] = {0};
  u8 alternatedns[20] = {0};

  MT_ASSERT(p_eth_dev != NULL);
  #ifndef WIN32
  get_net_device_addr_info(p_eth_dev,ipaddr,netmask,gw,primarydns,alternatedns);
  #endif
  OS_PRINTF("###ipaddr %d.%d.%d.%d###\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
 // if(strcmp(ipaddr, "0.0.0.0") == 0)
  if((0 == ipaddr[0]) && (0 == ipaddr[1]) && (0 == ipaddr[2]) && (0 == ipaddr[3]))
  {
    OS_PRINTF("####get dhcp ipadress failed###\n");
    return FALSE;
  }
  else
  {
    OS_PRINTF("####get dhcp ipadress successfully###\n");
    return TRUE;
  }
  
}

static void config_network_ethcfg()
{
  control_t *p_ctrl[4] = { NULL };
  u8 ipaddress[32] = {0};
  ip_address_t addr;
  u16 i;
  
  ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  if(g_net_config.config_mode == DHCP)
  {
    ethcfg.is_enabledhcp = TRUE;
  }
  else
  {
    ethcfg.is_enabledhcp = FALSE;
  }
  OS_PRINTF("@@@config_network_ethcfg, ethcfg.is_enabledhcp = %d@@@@\n",g_net_config.config_mode);

  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        if(g_net_config.link_type == LINK_TYPE_GPRS)
        {
          p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_IP);
        }
        else
        {
          p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_IP_ADDRESS);
        }
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
      
        ethcfg.ipaddr[0] = addr.s_b4;
        ethcfg.ipaddr[1] = addr.s_b3;
        ethcfg.ipaddr[2] = addr.s_b2;
        ethcfg.ipaddr[3] = addr.s_b1;
        OS_PRINTF("###ip address  is:%s####\n",ipaddress);
       break;
      case 1:
        if(g_net_config.link_type == LINK_TYPE_GPRS)
        {
           p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_MASK);
        }
        else
        {
           p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_NETMASK);
        }
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.netmask[0] = addr.s_b4;
        ethcfg.netmask[1] = addr.s_b3;
        ethcfg.netmask[2] = addr.s_b2;
        ethcfg.netmask[3] = addr.s_b1;
        OS_PRINTF("###netmask  is:%s####\n",ipaddress);
       break;
      case 2:
        if(g_net_config.link_type == LINK_TYPE_GPRS)
        {
           p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_DNS_ALTER);
        }
        else
        {
           p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GATEWAY);
        }
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.gateway[0] = addr.s_b4;
        ethcfg.gateway[1] = addr.s_b3;
        ethcfg.gateway[2] = addr.s_b2;
        ethcfg.gateway[3] = addr.s_b1;
        OS_PRINTF("###gateway  is:%s####\n",ipaddress);
       break;
      case 3:
        if(g_net_config.link_type == LINK_TYPE_GPRS)
        {
          p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GPRS_DNS_PREFER);
        }
        else
        {
          p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_DNS_SERVER);
        }
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.primaryDNS[0] = addr.s_b4;
        ethcfg.primaryDNS[1] = addr.s_b3;
        ethcfg.primaryDNS[2] = addr.s_b2;
        ethcfg.primaryDNS[3] = addr.s_b1;
        OS_PRINTF("###dns server is:%s####\n",ipaddress);
       break;
     default:
        break;
    }
  }
  
}

ethernet_cfg_t get_static_wifi_ethcfg()
{
  return static_wifi_ethcfg;
}
static void set_static_wifi_ethcfg()
{
  control_t *p_ctrl[4] = { NULL };
  u8 ipaddress[32] = {0};
  ip_address_t addr;
  u16 i;
  static_wifi_ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  static_wifi_ethcfg.is_enabledhcp = FALSE;
  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_IP_ADDRESS);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.ipaddr[0] = addr.s_b4;
        static_wifi_ethcfg.ipaddr[1] = addr.s_b3;
        static_wifi_ethcfg.ipaddr[2] = addr.s_b2;
        static_wifi_ethcfg.ipaddr[3] = addr.s_b1;
        OS_PRINTF("###static wifi ip address  is:%s####\n",ipaddress);
       break;
      case 1:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_NETMASK);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.netmask[0] = addr.s_b4;
        static_wifi_ethcfg.netmask[1] = addr.s_b3;
        static_wifi_ethcfg.netmask[2] = addr.s_b2;
        static_wifi_ethcfg.netmask[3] = addr.s_b1;
        OS_PRINTF("###static wifi netmask  is:%s####\n",ipaddress);
       break;
      case 2:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_GATEWAY);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.gateway[0] = addr.s_b4;
        static_wifi_ethcfg.gateway[1] = addr.s_b3;
        static_wifi_ethcfg.gateway[2] = addr.s_b2;
        static_wifi_ethcfg.gateway[3] = addr.s_b1;
        OS_PRINTF("###static wifi gateway  is:%s####\n",ipaddress);
       break;
      case 3:
        p_ctrl[i] = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG, IDC_DNS_SERVER);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.primaryDNS[0] = addr.s_b4;
        static_wifi_ethcfg.primaryDNS[1] = addr.s_b3;
        static_wifi_ethcfg.primaryDNS[2] = addr.s_b2;
        static_wifi_ethcfg.primaryDNS[3] = addr.s_b1;
        OS_PRINTF("###static wifi dns server is:%s####\n",ipaddress);
       break;
     default:
        break;
    }
  }
}
static RET_CODE on_connect_network(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
  ethernet_device_t *eth_dev = NULL;
  
  eth_connt_stats = ui_get_net_connect_status();
  if(g_net_config.link_type == LINK_TYPE_WIFI)
  {
    if(!eth_connt_stats.is_wifi_insert)
    {
      OS_PRINTF("@@@wifi connect not ok@@@\n");
      ui_comm_cfmdlg_open(NULL, IDS_WIFI_DEV_NOT_EXIST, NULL, 2000);
      return ERR_FAILURE;
    }
    if(g_net_config.config_mode == STATIC_IP)
    {
      set_static_wifi_ethcfg();
    }
    manage_open_menu(ROOT_ID_WIFI, para1, para2);
    return SUCCESS;
  }
  
  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  if(eth_dev == NULL)
  {
    OS_PRINTF("#####eth_dev  == NULL######\n");
    return ERR_FAILURE;
  }
  
  if(!eth_connt_stats.is_eth_insert)
  {
    OS_PRINTF("@@@ethernet connect not ok@@@\n");
    ui_comm_cfmdlg_open(NULL, IDS_NET_CABLE_NOT_CONNECT, NULL, 2000);
    return ERR_FAILURE;
  }
  if(g_net_config.config_mode == PPPOE)
  {
      manage_open_menu(ROOT_ID_PPPOE_CONNECT, para1, para2);
      OS_PRINTF("####will open PPPoE connect window####\n");
      return SUCCESS;
  }
  config_network_ethcfg();
  OS_PRINTF("@@@begin  do_cmd_connect_network@@@\n");
  do_cmd_connect_network(&ethcfg, eth_dev);
  ssdata_ip_address_set();
  
  return SUCCESS;
  
}

static void dis_conn_wifi()
{
  ethernet_device_t* wifi_dev = NULL;
  net_conn_stats_t wifi_connt_stats = {0};
  comm_dlg_data_t net_wifi_dlg_data =
  {
    ROOT_ID_NETWORK_CONFIG,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_DISCONNECTING_WIFI,
    5000,
  };
  
  wifi_connt_stats = ui_get_net_connect_status();
  if(!wifi_connt_stats.is_wifi_insert || !wifi_connt_stats.is_wifi_conn)
  {
    OS_PRINTF("#######WIFI not insert or not connected, no need to disconnect wifi#####\n");
    return ;
  }
  wifi_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  if(wifi_dev == NULL)
  {
    OS_PRINTF("#######wifi_dev == NULL, no need to disconnect wifi#####\n");
    return ;
  }
  
  #ifndef WIN32
  set_wifi_disconnect(wifi_dev);
  ui_comm_dlg_open(&net_wifi_dlg_data);
  wifi_connt_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_connt_stats);
  mtos_task_sleep(4000);
  paint_connect_status(wifi_connt_stats.is_wifi_conn, TRUE);
  #endif
}

static RET_CODE on_link_type_changed(control_t *p_cbox, u16 msg,u32 para1, u32 para2)
{
  control_t *p_link_type = NULL;
  control_t *p_connect = NULL;
  control_t *p_config_mode = NULL;
  u16 network_str[CONFIG_MODE_TYPE_CNT] = { IDS_DHCP, IDS_STATIC_IP, IDS_PPPOE };
  u16 network_str_wifi[CONFIG_MODE_TYPE_CNT - 1] = { IDS_DHCP, IDS_STATIC_IP};
  u16 focus = 0;
  u16 j;
  
  p_link_type = ctrl_get_parent(p_cbox);
  focus = ui_comm_select_get_focus(p_link_type);
  p_connect = ctrl_get_child_by_id(p_link_type->p_parent, IDC_CONNECT_NETWORK);
  p_config_mode = ctrl_get_child_by_id(p_link_type->p_parent, IDC_CNFIG_MODE);
  
  if(p_connect == NULL || p_config_mode == NULL)
  {
    return ERR_FAILURE;
  }
   
  if(1 == focus)
  {
    text_set_content_by_strid(p_connect, IDS_SEARCH);
    g_net_config.link_type = LINK_TYPE_WIFI;
    ui_comm_select_set_param(p_config_mode, TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT - 1, CBOX_ITEM_STRTYPE_STRID, NULL);
    for(j = 0;j < CONFIG_MODE_TYPE_CNT - 1;j++)
    {
       ui_comm_select_set_content(p_config_mode, j, network_str_wifi[j]);
    }
    if(g_net_config.config_mode == STATIC_IP)
    {
      ui_comm_select_set_focus(p_config_mode, 1);
    }
    else
    {
      ui_comm_select_set_focus(p_config_mode, 0);
    }

    ctrl_gprs_cfg_show(p_link_type->p_parent, FALSE);

    ctrl_set_related_id(p_link_type, 0, IDC_CONNECT_NETWORK, 0, IDC_CNFIG_MODE);    
   // ui_comm_select_create_droplist(p_config_mode, CONFIG_MODE_TYPE_CNT - 1);
  }
  else if(0 == focus)
  {
    if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL) 
    {
    dis_conn_wifi();
    }
    text_set_content_by_strid(p_connect, IDS_CONNECT);
    g_net_config.link_type = LINK_TYPE_LAN;
    ui_comm_select_set_param(p_config_mode, TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
    for(j = 0;j < CONFIG_MODE_TYPE_CNT;j++)
    {
       ui_comm_select_set_content(p_config_mode, j, network_str[j]);
    }
    if(g_net_config.config_mode == DHCP)
    {
      ui_comm_select_set_focus(p_config_mode, 0);
    }
    else if(g_net_config.config_mode == STATIC_IP)
    {
      ui_comm_select_set_focus(p_config_mode, 1);
    }
    else 
    {
      ui_comm_select_set_focus(p_config_mode, 2);
    }

    ctrl_gprs_cfg_show(p_link_type->p_parent, FALSE);

    ctrl_set_related_id(p_link_type, 0, IDC_CONNECT_NETWORK, 0, IDC_CNFIG_MODE);    
   // ui_comm_select_create_droplist(p_config_mode, CONFIG_MODE_TYPE_CNT);
  }
  else if(2 == focus)
  {
    g_net_config.link_type = 2;

    ctrl_gprs_cfg_show(p_link_type->p_parent, TRUE);

    ctrl_set_related_id(p_link_type, 0, IDC_GPRS_CONNECT, 0, IDC_GPRS_APN);

  }
  
  ctrl_paint_ctrl(p_connect, TRUE);
  ctrl_paint_ctrl(p_config_mode, TRUE);
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();
  return SUCCESS;
}


static RET_CODE on_network_config_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
   if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
   {
      OS_PRINTF("###disconnecting not exit will return from on_network_config_exit####\n");
      return ERR_FAILURE;
   }
  config_network_ethcfg();
  ssdata_ip_address_set();
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();
  manage_close_menu(ROOT_ID_NETWORK_CONFIG, 0, 0);
  OS_PRINTF("@@@@@@@@@@@on_network_config_exit @@@@@@@@@\n");
  
  return SUCCESS;
}

static RET_CODE on_network_config_plug_in(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_config_t net_config = {0,};
  eth_connt_stats.is_eth_insert = TRUE;
  ui_set_net_connect_status(eth_connt_stats);
  sys_status_get_net_config_info(&net_config);
  if(net_config.link_type == LINK_TYPE_LAN)
  {
    auto_connect_ethernet();
  }
  
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_in @@@@@@@@@\n");
  
  return SUCCESS;
}

static RET_CODE on_network_config_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_conn_stats_t net_connt_stats;
  net_connt_stats = ui_get_net_connect_status();
  net_connt_stats.is_eth_conn = FALSE;
  net_connt_stats.is_eth_insert = FALSE;
  
  if (!net_connt_stats.is_wifi_conn)
  {
    paint_connect_status(net_connt_stats.is_eth_conn, TRUE);
  }
  ui_set_net_connect_status(net_connt_stats);
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_out is_eth_conn = %d,is_eth_insert = %d@@@@@@@@@\n",net_connt_stats.is_eth_conn, net_connt_stats.is_eth_insert);
  
  return SUCCESS;
}
#if 0
static RET_CODE on_disconn_wifi_over(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_conn_stats_t wifi_stats = {0};
  control_t *root = NULL;
  
  OS_PRINTF("###before change link type from wifi to ethernet, disconnect  over###\n");
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
  {
    ui_comm_dlg_close();
  }
  root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);
  if(root)
  {
    OS_PRINTF("###WIFI link info ui exist,should ignore the disconnect msg at networkconfig UI###\n");
    return ERR_FAILURE;
  }
  wifi_stats = ui_get_net_connect_status();
  if(!wifi_stats.is_wifi_conn)
  {
    OS_PRINTF("###WIFI status is disconnect,should ignore the disconnect msg at networkconfig UI###\n");
    return ERR_FAILURE;
  }
  
  wifi_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_stats);
  paint_connect_status(wifi_stats.is_wifi_conn, TRUE);
  return SUCCESS;
}
#endif

static RET_CODE on_gprs_status_update(control_t *p_cont, u16 msg,
                                              u32 para1, u32 para2)
{
#ifndef WIN32	
  control_t *p_status = NULL;

  p_status = ctrl_get_child_by_id(p_cont, IDC_GPRS_SIGNAL);
  if(p_status == NULL)
  {
    return ERR_FAILURE;
  }

  switch(para1)
  {
    case MODEM_DEVICE_HARDWARE_INITIALIZE:
      ui_comm_static_set_content_by_ascstr(p_status, "initialize");
      strcpy(g_gprs_status, "initialize");
    break;
    
    case MODEM_DEVICE_COMMUNICATE_ERROR:
      ui_comm_static_set_content_by_ascstr(p_status, "communicate_error");
      strcpy(g_gprs_status, "communicate_error");
    break; 

    case MODEM_DEVICE_NO_SIM_CARD:
      ui_comm_static_set_content_by_ascstr(p_status, "no_sim");
      strcpy(g_gprs_status, "no_sim");
    break;
    
    case MODEM_DEVICE_NO_SIGNAL:
      ui_comm_static_set_content_by_ascstr(p_status, "no_signal");
      strcpy(g_gprs_status, "no_signal");
    break;
    
    case MODEM_DEVICE_NETWORK_UNREGISTERED:
      ui_comm_static_set_content_by_ascstr(p_status, "unregister");
      strcpy(g_gprs_status, "unregister");
    break;
    
    case MODEM_DEVICE_NETWORK_UNCONNECTED:
      ui_comm_static_set_content_by_ascstr(p_status, "unconnect");
      strcpy(g_gprs_status, "unconnect");
    break;
    
    case MODEM_DEVICE_NETWORK_DIALING:
      ui_comm_static_set_content_by_ascstr(p_status, "dialing");
      strcpy(g_gprs_status, "dialing");
    break;
    
    case MODEM_DEVICE_NETWORK_CONNECTED:
      ui_comm_static_set_content_by_ascstr(p_status, "connected");
      strcpy(g_gprs_status, "connected");
    break; 
    
    default:
      ui_comm_static_set_content_by_ascstr(p_status, "default");
      strcpy(g_gprs_status, "default");
    break;

  }

  ctrl_paint_ctrl(p_status, TRUE);
#endif
  
  return SUCCESS;
}

BEGIN_KEYMAP(network_config_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(network_config_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(network_config_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_network_config_exit)
  ON_COMMAND(MSG_INTERNET_PLUG_IN, on_network_config_plug_in)
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_network_config_plug_out)
  ON_COMMAND(MSG_GPRS_STATUS_UPDATE, on_gprs_status_update)
//  ON_COMMAND(MSG_WIFI_AP_DISCONNECT, on_disconn_wifi_over)
END_MSGPROC(network_config_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ping_test_keymap, ui_comm_static_keymap)
  
END_KEYMAP(ping_test_keymap, ui_comm_static_keymap)

BEGIN_MSGPROC(ping_test_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_config_test_select)
END_MSGPROC(ping_test_proc, cont_class_proc)

BEGIN_MSGPROC(network_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_network_mode_change)
END_MSGPROC(network_mode_proc, cbox_class_proc)

BEGIN_KEYMAP(connect_network_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(connect_network_keymap, NULL)

BEGIN_MSGPROC(connect_network_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_connect_network)
END_MSGPROC(connect_network_proc, text_class_proc)

BEGIN_KEYMAP(link_type_keymap, NULL)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
END_KEYMAP(link_type_keymap, NULL)

BEGIN_MSGPROC(link_type_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_link_type_changed)
END_MSGPROC(link_type_proc, cbox_class_proc)

//gprs
BEGIN_MSGPROC(gprs_apn_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_edit_apn)
  ON_COMMAND(MSG_NUMBER, on_gprs_edit_apn)
END_MSGPROC(gprs_apn_set_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(gprs_dial_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_edit_dial)
  ON_COMMAND(MSG_NUMBER, on_gprs_edit_dial)
END_MSGPROC(gprs_dial_set_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(gprs_ping_test_keymap, ui_comm_static_keymap)
  
END_KEYMAP(gprs_ping_test_keymap, ui_comm_static_keymap)

BEGIN_KEYMAP(gprs_connect_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(gprs_connect_keymap, NULL)

BEGIN_MSGPROC(gprs_ping_test_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_ping_test_select)
END_MSGPROC(gprs_ping_test_proc, cont_class_proc)

BEGIN_MSGPROC(gprs_connect_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_connect)
END_MSGPROC(gprs_connect_proc, text_class_proc)
