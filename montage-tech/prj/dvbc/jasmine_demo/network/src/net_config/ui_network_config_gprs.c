/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_network_config_gprs.h"
#include "ethernet.h"
#include "ui_wifi.h"
#include "wifi.h"
#include "ui_keyboard_v2.h"
#include "ui_network_config_wifi.h"

#ifndef WIN32
 #include "ppp.h"
 #include "modem.h"
#endif
#include "network_monitor.h"

enum ip_ctrl_id
{
  IDC_CONNECT_TYPE = 1,
  IDC_GRPS_OPERATOR ,
  IDC_GPRS_DIAL,
  IDC_GPRS_APN,  
  IDC_GPRS_USRNAME,
  IDC_GPRS_PASSWORD,
  IDC_GPRS_STATUS,
  IDC_GPRS_PING_TEST,
  IDC_GPRS_CONNECT,  

  IDC_CONFIG_TOTAL,
};


u16 network_config_gprs_cont_keymap(u16 key);
u16 gprs_ping_test_keymap(u16 key);
u16 gprs_connect_keymap(u16 key);
u16 gprs_pwd_keymap(u16 key);


RET_CODE network_config_gprs_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

RET_CODE gprs_apn_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_dial_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_ping_test_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_connect_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_usrname_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_pwd_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

extern void do_cmd_connect_gprs(gprs_info_t *p_gprs_info);
extern void do_cmd_disconnect_gprs(void);
extern void do_cmd_disconnect_g3(void);


static BOOL gprs_conn_btn_press = FALSE;
static u8 g_gprs_status[32] = {0};
static gprs_info_t g_gprs_info = {{0}, };



void set_gprs_connect_status(u32 conn_stats)
{
#ifndef WIN32
      net_conn_stats_t  conn_status = {0};
      
      switch(conn_stats)
      {
        case MODEM_DEVICE_HARDWARE_INITIALIZE:
          strcpy((char*)g_gprs_status, "Hardware Initialize");
        break;
        
        case MODEM_DEVICE_COMMUNICATE_ERROR:
          strcpy((char*)g_gprs_status, "Connect Error");
        break; 

        case MODEM_DEVICE_NO_SIM_CARD:
          strcpy((char*)g_gprs_status, "No SIM Card");
        break;
        
        case MODEM_DEVICE_NO_SIGNAL:
          strcpy((char*)g_gprs_status, "No Signal");
        break;
        
        case MODEM_DEVICE_NETWORK_UNREGISTERED:
          strcpy((char*)g_gprs_status, "Invalid Network");
        break;
        
        case MODEM_DEVICE_NETWORK_UNCONNECTED:
          strcpy((char*)g_gprs_status, "Unconnected");

          conn_status = ui_get_net_connect_status();
          conn_status.is_gprs_conn = FALSE;
          ui_set_net_connect_status(conn_status);
          gprs_conn_btn_press = FALSE;

          paint_gprs_conn_status(FALSE);
          
        break;
        
        case MODEM_DEVICE_NETWORK_DIALING:
          strcpy((char*)g_gprs_status, "Dialing ...");
        break;
        
        case MODEM_DEVICE_NETWORK_CONNECTED:
          strcpy((char*)g_gprs_status, "Connected");

          conn_status = ui_get_net_connect_status();
          conn_status.is_gprs_conn = TRUE;
          ui_set_net_connect_status(conn_status);
          gprs_conn_btn_press = TRUE;    

          paint_gprs_conn_status(TRUE);
        break; 
        
        default:
          strcpy((char*)g_gprs_status, "Default");
        break;

      }
#endif
}

static control_t *create_network_config_menu()
{
   control_t *p_menu = NULL;

   p_menu = ui_comm_root_create_netmenu(ROOT_ID_NETWORK_CONFIG_GPRS,0, IM_INDEX_NETWORK_BANNER, IDS_GPRS);
  
  ctrl_set_keymap(p_menu, network_config_gprs_cont_keymap);
  ctrl_set_proc(p_menu, network_config_gprs_cont_proc);

  return p_menu;
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
#ifndef WIN32
  net_conn_info_t net_conn_info = {0};
  control_t *p_status = NULL;
  comm_dlg_data_t wifi_dlg_data  =
  {
    ROOT_ID_NETWORK_CONFIG_GPRS,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_DISCONNECTING_WIFI,
    5000,
  };  
  
  p_status = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_STATUS);
  if(gprs_conn_btn_press == TRUE)
  {
      do_cmd_disconnect_gprs();
      gprs_conn_btn_press = FALSE;
      text_set_content_by_strid(p_ctrl, IDS_CONNECT); 

      if(p_status)
      {
         ui_comm_static_set_content_by_ascstr(p_status, (u8*)"Disconnecting gprs...");
      } 
      ctrl_paint_ctrl(p_ctrl, TRUE);
  } 
  else
  {
      net_conn_info = ui_get_conn_info();  
      if(net_conn_info.wifi_conn_info >= WIFI_CONNECTING)
      {
         dis_conn_wifi(&wifi_dlg_data);
         if(p_status)
         {
             ui_comm_static_set_content_by_ascstr(p_status, (u8*)"Disconnecting wifi...");
         }         
      }
        
      if(net_conn_info.g3_conn_info >= MODEM_DEVICE_NETWORK_START_DIAL)    
      {
          do_cmd_disconnect_g3();
         if(p_status)
         {
             ui_comm_static_set_content_by_ascstr(p_status, (u8*)"Disconnecting 3g...");
         }          
      }
      
      fw_tmr_create(ROOT_ID_NETWORK_CONFIG_GPRS, MSG_CONNECT_NETWORK, SECOND, TRUE);

  }

  if(p_status)
  {
      ctrl_paint_ctrl(p_status, TRUE);
  }
#endif
  return SUCCESS;
} 

static RET_CODE edit_gprs_apn_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL, *p_ctrl_usr = NULL, *p_ctrl_passwd = NULL;
  u16 *p_apn = NULL;
  
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_APN);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  //
  p_ctrl_usr = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_USRNAME);
  p_ctrl_passwd = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_PASSWORD);
  if((p_ctrl_usr == NULL) || (p_ctrl_passwd == NULL))
  {
    return ERR_FAILURE;
  }
  p_apn = ui_comm_t9edit_get_content(p_edit_cont);
  if(p_apn[0] == '\0')
  {
     ui_comm_ctrl_update_attr(p_ctrl_usr, FALSE);
     ui_comm_ctrl_update_attr(p_ctrl_passwd, FALSE);
  }
  else
  {
     
     ui_comm_ctrl_update_attr(p_ctrl_usr, TRUE);
     ui_comm_ctrl_update_attr(p_ctrl_passwd, TRUE);     
  }
  ctrl_paint_ctrl(p_ctrl_usr, TRUE);
  ctrl_paint_ctrl(p_ctrl_passwd, TRUE);  

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

static RET_CODE edit_gprs_dial_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_DIAL);
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

static RET_CODE edit_gprs_usrname_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_USRNAME);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_gprs_edit_username(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_gprs_usrname_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

static RET_CODE edit_gprs_password_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_PASSWORD);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_gprs_edit_password(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_gprs_password_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}

static void create_gprs_items(control_t *p_menu)
{
  control_t *p_ctrl[GPRS_CTRL_CNT] = { NULL };
  u16 i = 0, y = 0;
  u16 ctrl_str[GPRS_CTRL_CNT] =
  {
    0, IDS_3G_OPERATOR, IDS_GPRS_DIAL, IDS_GPRS_APN, IDS_ACCOUNT, IDS_PWD,
    IDS_GPRS_CONNECT_STATUS, IDS_PING_TEST, IDS_CONNECT
  };  

  u8 connect_type[64] = {0};
  
  memset(&g_gprs_info, 0, sizeof(gprs_info_t));
  memcpy(&g_gprs_info, sys_status_get_gprs_info(), sizeof(gprs_info_t));
  y = NETWORK_GPRS_ITEM_Y;
  //y += NETWORK_GPRS_ITEM_H + NETWORK_GPRS_ITEM_VGAP;
  for(i = 0; i < GPRS_CTRL_CNT; i++)
  {
     switch(i)
      {
        case 0:
          p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_TYPE + i, NETWORK_GPRS_ITEM_X, y, NETWORK_GPRS_ITEM_LW+NETWORK_GPRS_ITEM_RW, COMM_CTRL_H, p_menu, 0);
          text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
          text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
          
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);  
          strcpy((char*)connect_type, "Link Type:                      GPRS");
          text_set_content_by_ascstr(p_ctrl[i], connect_type);     
          break;
        case 1: //operator name
          p_ctrl[i] = ui_comm_static_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_GPRS_ITEM_X, y, NETWORK_GPRS_ITEM_LW, 
                                                NETWORK_GPRS_ITEM_RW);   
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], g_gprs_info.name);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
          
        break;
      
        case 2:  //dial
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_GPRS_ITEM_X, y, NETWORK_GPRS_ITEM_LW, 
                                                NETWORK_GPRS_ITEM_RW, ROOT_ID_NETWORK_CONFIG_GPRS);
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], gprs_dial_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_gprs_info.dial_num);
                 
        break;

        case 3:  //apn       
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_GPRS_ITEM_X, y, NETWORK_GPRS_ITEM_LW, 
                                                NETWORK_GPRS_ITEM_RW, ROOT_ID_NETWORK_CONFIG_GPRS);                                             
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], gprs_apn_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_gprs_info.apn);
          
        break;

        case 4: //username
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_GPRS_ITEM_X, y, NETWORK_GPRS_ITEM_LW, 
                                                NETWORK_GPRS_ITEM_RW, ROOT_ID_NETWORK_CONFIG_GPRS);
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], gprs_usrname_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_gprs_info.usr_name);
          if(g_gprs_info.apn[0] == 0)
          {
             ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
          }
          
        break;

        case 5: //password
          p_ctrl[i] = ui_comm_pwdedit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                             NETWORK_GPRS_ITEM_X, y, NETWORK_GPRS_ITEM_LW, 
                                             NETWORK_GPRS_ITEM_RW);
          ui_comm_pwdedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_keymap(p_ctrl[i], gprs_pwd_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], gprs_pwd_set_proc); 
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_gprs_info.passwd);
          if(g_gprs_info.apn[0] == 0)
          {
             ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
          }

        break;

        case 6:   //signal status
          p_ctrl[i] = ui_comm_static_create(p_menu, (u8)(IDC_CONNECT_TYPE + i),NETWORK_GPRS_ITEM_X, 
                                            y, NETWORK_GPRS_ITEM_LW, NETWORK_GPRS_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], g_gprs_status);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);

        break;

        case 7:  //ping test
          p_ctrl[i] = ui_comm_static_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                            NETWORK_GPRS_ITEM_X, y, NETWORK_GPRS_ITEM_LW, NETWORK_GPRS_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ctrl_set_keymap(p_ctrl[i], gprs_ping_test_keymap);
          ctrl_set_proc(p_ctrl[i], gprs_ping_test_proc);   

        break;

        case 8:  //connect
          p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CONNECT_TYPE + i), NETWORK_GPRS_ITEM_X, 
                                       y, NETWORK_GPRS_ITEM_LW+NETWORK_GPRS_ITEM_RW, COMM_CTRL_H, p_menu, 0);
          ctrl_set_rstyle(p_ctrl[i], RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
          text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
          text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
          text_set_content_by_strid(p_ctrl[i], ctrl_str[i]);  
          ctrl_set_keymap(p_ctrl[i], gprs_connect_keymap);
          ctrl_set_proc(p_ctrl[i], gprs_connect_proc);   
        break;
      default:
        break;
      }

      y += NETWORK_GPRS_ITEM_H + NETWORK_GPRS_ITEM_VGAP;
     if(i != 0)
     {
        if(i == 1)
        {
            ctrl_set_related_id(p_ctrl[i], 0, 9, 0, 3);
        }
        else if(i == 8)
        {
             ctrl_set_related_id(p_ctrl[i], 0, 8, 0, 2);
        }
        else
        {
            ctrl_set_related_id(p_ctrl[i], 0, i + 1 - 1, 0, i + 1 + 1);
        }

     }

  }

  ctrl_default_proc(p_ctrl[2], MSG_GETFOCUS, 0, 0);
  
}



RET_CODE open_network_config_gprs(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;

  //create main menu
  p_menu = create_network_config_menu();

  //create gprs items
  create_gprs_items(p_menu);

  //ctrl_default_proc(p_menu, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);
  
  return SUCCESS;

}


void paint_gprs_conn_status(BOOL is_conn)
{
       control_t *p_gprs_conn = NULL;
    
       p_gprs_conn = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_CONNECT);
       if(p_gprs_conn != NULL)
       {
          if(is_conn == FALSE) 
          {
              text_set_content_by_strid(p_gprs_conn, IDS_CONNECT); 
          }
          else
          {
              text_set_content_by_strid(p_gprs_conn, IDS_DISCONNECT); 
          }
          ctrl_paint_ctrl(p_gprs_conn, TRUE);
       }
       
       p_gprs_conn = NULL;
       p_gprs_conn = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_STATUS);
       if(p_gprs_conn != NULL)
       {
          if(is_conn == FALSE) 
          {
              ui_comm_static_set_content_by_ascstr(p_gprs_conn, (u8*)"unconnected"); 
          }
          else
          {
              ui_comm_static_set_content_by_ascstr(p_gprs_conn, (u8*)"connected"); 
          }
          ctrl_paint_ctrl(p_gprs_conn, TRUE);
       }


}

static RET_CODE ssdata_save_gprs_info(control_t *p_cont)
{
  control_t *p_ctrl_operator = NULL, *p_ctrl_apn = NULL, *p_ctrl_num = NULL;
  control_t *p_ctrl_usrname = NULL, *p_ctrl_passwd = NULL;
  u16 *p_uni_operator = NULL, *p_uni_apn = NULL, *p_uni_num = NULL;
  u16 *p_uni_usrname = NULL, *p_uni_passwd = NULL;

  MT_ASSERT(p_cont != NULL);
  
  p_ctrl_operator = ctrl_get_child_by_id(p_cont, IDC_GRPS_OPERATOR);
  p_ctrl_apn = ctrl_get_child_by_id(p_cont, IDC_GPRS_APN);
  p_ctrl_num = ctrl_get_child_by_id(p_cont, IDC_GPRS_DIAL); 
  p_ctrl_usrname = ctrl_get_child_by_id(p_cont, IDC_GPRS_USRNAME); 
  p_ctrl_passwd = ctrl_get_child_by_id(p_cont, IDC_GPRS_PASSWORD);    
  if((p_ctrl_operator == NULL) || (p_ctrl_apn == NULL) || (p_ctrl_num == NULL) 
     || (p_ctrl_usrname == NULL) || (p_ctrl_passwd == NULL))
  {
    return ERR_FAILURE;
  }

  p_uni_operator = (u16 *)ui_comm_static_get_content(p_ctrl_operator);
  p_uni_apn = ui_comm_t9edit_get_content(p_ctrl_apn);
  p_uni_num = ui_comm_t9edit_get_content(p_ctrl_num);
  p_uni_usrname = ui_comm_t9edit_get_content(p_ctrl_usrname);
  p_uni_passwd = ui_comm_t9edit_get_content(p_ctrl_passwd);
  
  if(p_uni_operator != NULL)
  {
     str_uni2asc(g_gprs_info.name, p_uni_operator);    
  }
  
  if(p_uni_apn != NULL) 
  {
     str_uni2asc(g_gprs_info.apn, p_uni_apn);    
  }
  
  if(p_uni_num != NULL) 
  {
     str_uni2asc(g_gprs_info.dial_num, p_uni_num);    
  } 
  
  if(p_uni_usrname != NULL) 
  {
     str_uni2asc(g_gprs_info.usr_name, p_uni_usrname);    
  }
  
  if(p_uni_passwd != NULL)
  {
     str_uni2asc(g_gprs_info.passwd, p_uni_passwd);
  }  

  sys_status_set_gprs_info(&g_gprs_info);
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

  ssdata_save_gprs_info(p_cont);
  
  fw_tmr_destroy(ROOT_ID_NETWORK_CONFIG_GPRS, MSG_CONNECT_NETWORK);

  manage_close_menu(ROOT_ID_NETWORK_CONFIG_GPRS, 0, 0);

  return SUCCESS;
}


static RET_CODE on_gprs_status_update(control_t *p_cont, u16 msg,
                                              u32 para1, u32 para2)
{
#ifndef WIN32	
  control_t *p_status = NULL, *p_operator = NULL;
  u8 *operator = NULL;

  p_status = ctrl_get_child_by_id(p_cont, IDC_GPRS_STATUS);
  if(p_status == NULL)
  {
    return ERR_FAILURE;
  }

  p_operator = ctrl_get_child_by_id(p_cont, IDC_GRPS_OPERATOR);
  if(p_operator == NULL)
  {
    return ERR_FAILURE;
  }

  operator = (u8 *)para2;
  ui_comm_static_set_content_by_ascstr(p_operator, operator);
  ctrl_paint_ctrl(p_operator, TRUE);
  

  switch(para1)
  {
    case MODEM_DEVICE_HARDWARE_INITIALIZE:      
      strcpy((char*)g_gprs_status, "Hardware Initialize");
    break;
    
    case MODEM_DEVICE_COMMUNICATE_ERROR:
      strcpy((char*)g_gprs_status, "Connect Error");
    break; 

    case MODEM_DEVICE_NO_SIM_CARD:
      strcpy((char*)g_gprs_status, "No SIM Card");
    break;
    
    case MODEM_DEVICE_NO_SIGNAL:
      strcpy((char*)g_gprs_status, "No Signal");
    break;
    
    case MODEM_DEVICE_NETWORK_UNREGISTERED:
      strcpy((char*)g_gprs_status, "Invalid Network");
    break;
    
    case MODEM_DEVICE_NETWORK_UNCONNECTED:
      strcpy((char*)g_gprs_status, "Unconnected");
    break;
    
    case MODEM_DEVICE_NETWORK_DIALING:
      strcpy((char*)g_gprs_status, "Dialing ...");
    break;
    
    case MODEM_DEVICE_NETWORK_CONNECTED:
      strcpy((char*)g_gprs_status, "Connected");
    break; 
    
    default:
      strcpy((char*)g_gprs_status, "Default");
    break;
  }
  ui_comm_static_set_content_by_ascstr(p_status, g_gprs_status);
  ctrl_paint_ctrl(p_status, TRUE);
#endif
  
  return SUCCESS;
}

static RET_CODE on_network_gprs_connect(control_t *p_cont, u16 msg,
                                                u32 para1, u32 para2)
{
#ifndef WIN32
      net_conn_info_t net_conn_info = {0};  

      control_t *p_ctrl = NULL;
      control_t  *p_ctrl_conn = NULL, *p_ctrl_apn = NULL, *p_ctrl_dial = NULL;
      control_t *p_ctrl_usrname = NULL, *p_ctrl_passwd = NULL;
      
      u16 *p_uni_apn = NULL, *p_uni_dail = NULL, *p_uni_usr = NULL, *p_uni_passwd = NULL;
      u8 asc_apn[64] = {0}, asc_dial[64] = {0}, asc_usr[64] = {0}, asc_passwd[64] = {0};
      u32 apn_len = 0, dial_len = 0, usr_len = 0, passwd_len = 0;   

      gprs_info_t gprs_info = {{0},{0}};
      //lint -e64
      net_config_t net_config = {0};
      //lint +e64
      sys_status_get_net_config_info(&net_config);
      net_conn_info = ui_get_conn_info();
      switch(net_config.link_type)
      {
          case LINK_TYPE_LAN:

            break;

          case LINK_TYPE_WIFI:
            fw_tmr_destroy(ROOT_ID_NETWORK_CONFIG_GPRS, MSG_CONNECT_NETWORK);
            
            break;

          case LINK_TYPE_GPRS:
            if((net_conn_info.wifi_conn_info < WIFI_CONNECTING) &&
              (net_conn_info.g3_conn_info < MODEM_DEVICE_NETWORK_START_DIAL))
            {
                  fw_tmr_destroy(ROOT_ID_NETWORK_CONFIG_GPRS, MSG_CONNECT_NETWORK);

                  //ui_set_net_connect_status(conn_status);                       
                  
                  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_GPRS, IDC_GPRS_CONNECT); 
                  if(p_ctrl == NULL)
                  {
                       break;
                  }                  
                  p_ctrl_conn = ctrl_get_parent(p_ctrl);
                  if(p_ctrl_conn == NULL)
                  {
                       break;
                  }
                  
                  p_ctrl_apn = ctrl_get_child_by_id(p_ctrl_conn, IDC_GPRS_APN);
                  p_ctrl_dial = ctrl_get_child_by_id(p_ctrl_conn, IDC_GPRS_DIAL);
                  p_ctrl_usrname = ctrl_get_child_by_id(p_ctrl_conn, IDC_GPRS_USRNAME);
                  p_ctrl_passwd = ctrl_get_child_by_id(p_ctrl_conn, IDC_GPRS_PASSWORD);
                  if((p_ctrl_apn == NULL) || (p_ctrl_dial == NULL) ||
                     (p_ctrl_usrname == NULL) || (p_ctrl_passwd == NULL))
                  {
                     break;
                  }
                  
                  p_uni_apn = ui_comm_t9_v2_edit_get_content(p_ctrl_apn);
                  p_uni_dail = ui_comm_t9_v2_edit_get_content(p_ctrl_dial);
                  p_uni_usr = ui_comm_t9_v2_edit_get_content(p_ctrl_usrname);
                  p_uni_passwd = ui_comm_t9_v2_edit_get_content(p_ctrl_passwd);             
                  /*if((p_uni_apn == NULL) || (p_uni_dail == NULL) ||
                     (p_uni_usr == NULL) || (p_uni_passwd == NULL))
                  {
                     break;
                  }*/
                  
                  apn_len = str_uni2asc(asc_apn, p_uni_apn);
                  dial_len = str_uni2asc(asc_dial, p_uni_dail);
                  usr_len = str_uni2asc(asc_usr, p_uni_usr);  
                  passwd_len = str_uni2asc(asc_passwd, p_uni_passwd);                 
                
                  //connect gprs
                  if(gprs_conn_btn_press == FALSE)
                  {    
                     memcpy(gprs_info.apn, asc_apn, apn_len);
                     memcpy(gprs_info.dial_num, asc_dial, dial_len);
                     memcpy(gprs_info.usr_name, asc_usr, usr_len);
                     memcpy(gprs_info.passwd, asc_passwd, passwd_len);
                     
                     do_cmd_connect_gprs(&gprs_info);
                     gprs_conn_btn_press = TRUE;
                     text_set_content_by_strid(p_ctrl, IDS_DISCONNECT); 
                  }

                  ctrl_paint_ctrl(p_ctrl, TRUE);
                        
            }
            break;

          case LINK_TYPE_3G:

            break;

          default:
            break;

      }

      
#endif
      return SUCCESS;
}


BEGIN_KEYMAP(network_config_gprs_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(network_config_gprs_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(network_config_gprs_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_network_config_exit)
  ON_COMMAND(MSG_GPRS_STATUS_UPDATE, on_gprs_status_update) 
  ON_COMMAND(MSG_CONNECT_NETWORK, on_network_gprs_connect)
END_MSGPROC(network_config_gprs_cont_proc, ui_comm_root_proc)


//gprs
BEGIN_MSGPROC(gprs_apn_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_edit_apn)
  ON_COMMAND(MSG_NUMBER, on_gprs_edit_apn)
END_MSGPROC(gprs_apn_set_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(gprs_dial_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_edit_dial)
  ON_COMMAND(MSG_NUMBER, on_gprs_edit_dial)
END_MSGPROC(gprs_dial_set_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(gprs_usrname_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_edit_username)
  ON_COMMAND(MSG_NUMBER, on_gprs_edit_username)
END_MSGPROC(gprs_usrname_set_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(gprs_pwd_keymap, ui_comm_t9_keymap)
  ON_EVENT(V_KEY_RED,MSG_SELECT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(gprs_pwd_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(gprs_pwd_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_gprs_edit_password)
  ON_COMMAND(MSG_NUMBER, on_gprs_edit_password)
END_MSGPROC(gprs_pwd_set_proc, ui_comm_edit_proc)

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
