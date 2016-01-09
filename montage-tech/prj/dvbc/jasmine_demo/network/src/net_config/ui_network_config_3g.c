/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_network_config_3g.h"
#include "ui_keyboard_v2.h"
#include "ui_network_config_wifi.h"

#ifndef WIN32
 #include "ppp.h"
 #include "modem.h"
#endif

#include "network_monitor.h"
#include "ethernet.h"
#include "wifi.h"

enum ip_ctrl_id
{
  IDC_CONNECT_TYPE = 1,
  IDC_3G_NAME ,
  IDC_3G_DIAL,
  IDC_3G_APN,  
  IDC_3G_USRNAME,
  IDC_3G_PASSWORD,  
  IDC_3G_STATUS,  
  IDC_3G_SIGNAL,
  IDC_3G_PING_TEST,
  IDC_3G_CONNECT, 
};

typedef struct tag_g3_signal_info
{
   u8 operator[32];
   u8 status[32];
   u8 signal[32];
}g3_signal_info_t;



u16 network_config_3g_cont_keymap(u16 key);
u16 g3_connect_keymap(u16 key);
u16 g3_pwd_keymap(u16 key);
u16 g3_ping_test_keymap(u16 key);


RET_CODE network_config_3g_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE g3_connect_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE g3_apn_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE g3_dial_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE g3_usrname_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE g3_pwd_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE g3_ping_test_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


extern void do_cmd_connect_g3(g3_conn_info_t *p_g3_info);
extern void do_cmd_disconnect_g3(void);
extern void do_cmd_disconnect_gprs(void);



static BOOL g3_conn_btn_press = FALSE;
static g3_signal_info_t g_g3_signal_info = {{0}, };
static g3_conn_info_t  g_3g_conn_info = {{0}, }; 


void set_3g_connect_status(u32 conn_stats, u32 conn_signal, u8 *operator)
{   
#ifndef WIN32
     net_conn_stats_t  conn_status = {0};
      switch(conn_stats)
      {
        case MODEM_DEVICE_HARDWARE_INITIALIZE:
          strcpy((char*)g_g3_signal_info.status, "Hardware Initialize");
        break;
        
        case MODEM_DEVICE_COMMUNICATE_ERROR:
          strcpy((char*)g_g3_signal_info.status, "Connect Error");
        break; 

        case MODEM_DEVICE_NO_SIM_CARD:
          strcpy((char*)g_g3_signal_info.status, "No SIM Card");
        break;
        
        case MODEM_DEVICE_NO_SIGNAL:
          strcpy((char*)g_g3_signal_info.status, "No Signal");
        break;
        
        case MODEM_DEVICE_NETWORK_UNREGISTERED:
          strcpy((char*)g_g3_signal_info.status, "Invalid Network");
        break;
        
        case MODEM_DEVICE_NETWORK_UNCONNECTED:
          strcpy((char*)g_g3_signal_info.status, "Unconnected");

          conn_status = ui_get_net_connect_status();
          conn_status.is_3g_conn = FALSE;
          ui_set_net_connect_status(conn_status);
          g3_conn_btn_press = FALSE;

          paint_3g_conn_status(FALSE);
          
        break;
        
        case MODEM_DEVICE_NETWORK_DIALING:
          strcpy((char*)g_g3_signal_info.status, "Dialing ...");
        break;
        
        case MODEM_DEVICE_NETWORK_CONNECTED:
          strcpy((char*)g_g3_signal_info.status, "Connected");

          conn_status = ui_get_net_connect_status();
          conn_status.is_3g_conn = TRUE;
          ui_set_net_connect_status(conn_status);
          g3_conn_btn_press = TRUE;    

          paint_3g_conn_status(TRUE);
        break; 
        
        default:
          strcpy((char*)g_g3_signal_info.status, "Default");
        break;

      }

      switch(conn_signal)
      {
        case 0:
          strcpy((char*)g_g3_signal_info.signal, "0%");
        break;
        
        case 10:
          strcpy((char*)g_g3_signal_info.signal, "20%");    
        break;
        
        case 20:
          strcpy((char*)g_g3_signal_info.signal, "40%");       
        break;
        
        case 30:
          strcpy((char*)g_g3_signal_info.signal, "60%");    
        break;
        
        case 40:
          strcpy((char*)g_g3_signal_info.signal, "80%");      
        break;
        
        case 50:
          strcpy((char*)g_g3_signal_info.signal, "100%");       
        break;
        
        case 100:
          strcpy((char*)g_g3_signal_info.signal, "Signal Abnormal");      
        break;    
        default:
          strcpy((char*)g_g3_signal_info.signal, "0%");
        break;
      }

      if(conn_stats != MODEM_DEVICE_NETWORK_CONNECTED)
      {
          strcpy((char*)g_g3_signal_info.signal, "0%");
      }

      if(operator)
      {
         memcpy(g_g3_signal_info.operator, operator, sizeof(g_g3_signal_info.operator));
      }

#endif
}


static control_t *create_network_config_menu()
{
   control_t *p_menu = NULL;

   p_menu = ui_comm_root_create_netmenu(ROOT_ID_NETWORK_CONFIG_3G,0, IM_INDEX_NETWORK_BANNER, IDS_3G);
  
  ctrl_set_keymap(p_menu, network_config_3g_cont_keymap);
  ctrl_set_proc(p_menu, network_config_3g_cont_proc);

  return p_menu;
}

static RET_CODE ssdata_save_3g_info(control_t *p_cont)
{
  control_t *p_ctrl_operator = NULL, *p_ctrl_apn = NULL, *p_ctrl_num = NULL;
  control_t *p_ctrl_usrname = NULL, *p_ctrl_passwd = NULL;
  u16 *p_uni_operator = NULL, *p_uni_apn = NULL, *p_uni_num = NULL;
  u16 *p_uni_usrname = NULL, *p_uni_passwd = NULL;

  MT_ASSERT(p_cont != NULL);
  
  p_ctrl_operator = ctrl_get_child_by_id(p_cont, IDC_3G_NAME);
  p_ctrl_apn = ctrl_get_child_by_id(p_cont, IDC_3G_APN);
  p_ctrl_num = ctrl_get_child_by_id(p_cont, IDC_3G_DIAL); 
  p_ctrl_usrname = ctrl_get_child_by_id(p_cont, IDC_3G_USRNAME); 
  p_ctrl_passwd = ctrl_get_child_by_id(p_cont, IDC_3G_PASSWORD);    

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
     str_uni2asc(g_3g_conn_info.name, p_uni_operator);    
  }
  
  if(p_uni_apn != NULL) 
  {
     str_uni2asc(g_3g_conn_info.apn, p_uni_apn);    
  }
  
  if(p_uni_num != NULL) 
  {
     str_uni2asc(g_3g_conn_info.dial_num, p_uni_num);    
  } 
  
  if(p_uni_usrname != NULL) 
  {
     str_uni2asc(g_3g_conn_info.usr_name, p_uni_usrname);    
  }
  
  if(p_uni_passwd != NULL)
  {
     str_uni2asc(g_3g_conn_info.passwd, p_uni_passwd);
  }  

  sys_status_set_3g_info(&g_3g_conn_info);
  sys_status_save();

  return SUCCESS;
  
}


static RET_CODE on_g3_dongle_connect(control_t *p_ctrl,
                                               u16 msg,
                                               u32 para1,
                                               u32 para2)                                               
{
#ifndef WIN32
  net_conn_stats_t conn_status = {0};
  net_conn_info_t net_conn_info = {0};
  control_t *p_status = NULL;
  control_t *p_ctrl_conn = ctrl_get_parent(p_ctrl);
  comm_dlg_data_t wifi_dlg_data  =
  {
    ROOT_ID_NETWORK_CONFIG_3G,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_DISCONNECTING_WIFI,
    5000,
  };
  if(p_ctrl_conn)
  {
     ssdata_save_3g_info(p_ctrl_conn);
  }

 //
  conn_status = ui_get_net_connect_status();
  if(!conn_status.is_3g_insert)
  {
      OS_PRINTF("@@@3g device not insert@@@\n");
      ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
      return ERR_FAILURE;
  }
  p_status = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_STATUS);
  net_conn_info = ui_get_conn_info();  
  if(net_conn_info.g3_conn_info == 2)
  {
    OS_PRINTF("@@@SIM Card not insert@@@\n");
    if(p_status)
   {
       ui_comm_static_set_content_by_ascstr(p_status, (u8*)"No SIM Card");
       ctrl_paint_ctrl(p_status, TRUE);
   } 
    return ERR_FAILURE;
  }
  if(g3_conn_btn_press == TRUE)               
  {
     do_cmd_disconnect_g3();
     g3_conn_btn_press = FALSE;
     text_set_content_by_strid(p_ctrl, IDS_CONNECT); 

     if(p_status)
     {
         ui_comm_static_set_content_by_ascstr(p_status, (u8*)"Disconnecting 3g...");
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
      
      if (net_conn_info.gprs_conn_info >= MODEM_DEVICE_NETWORK_START_DIAL) 
      {
         do_cmd_disconnect_gprs();
         
         if(p_status)
         {
             ui_comm_static_set_content_by_ascstr(p_status, (u8*)"Disconnecting gprs...");
         }         
      }
      fw_tmr_create(ROOT_ID_NETWORK_CONFIG_3G, MSG_CONNECT_NETWORK, SECOND, TRUE);
  }

  if(p_status)
  {
      ctrl_paint_ctrl(p_status, TRUE);
  } 
#endif
  return SUCCESS;
}


static RET_CODE on_g3_ping_test_select(control_t *p_ctrl,
                                               u16 msg,
                                               u32 para1,
                                               u32 para2)    
{
  manage_open_menu(ROOT_ID_PING_TEST, 0, 0);
  
  return SUCCESS;  
}



static RET_CODE edit_g3_apn_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL, *p_ctrl_usr = NULL, *p_ctrl_passwd = NULL;
  u16 *p_apn = NULL;
  
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_APN);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  //
  p_ctrl_usr = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_USRNAME);
  p_ctrl_passwd = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_PASSWORD);
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

static RET_CODE on_g3_edit_apn(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_g3_apn_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

static RET_CODE edit_g3_dial_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_DIAL);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  return SUCCESS;

}

static RET_CODE on_g3_edit_dial(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_g3_dial_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

static RET_CODE edit_g3_usrname_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_USRNAME);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  return SUCCESS;

}

static RET_CODE on_g3_edit_username(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_g3_usrname_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

static RET_CODE edit_g3_password_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_PASSWORD);
  if(p_edit_cont == NULL)
  {
    OS_PRINTF("########keyboard label save update failed will return from %s#####\n", __FUNCTION__);
    return ERR_FAILURE;
  }
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_g3_edit_password(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = 32;
  param.cb = edit_g3_password_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}


static void create_3g_items(control_t *p_menu)
{

  control_t *p_ctrl[G3_CTRL_CNT] = { NULL };
  u16 i = 0, y = 0;
  u16 ctrl_str[G3_CTRL_CNT] =
  {
    0, IDS_3G_OPERATOR, IDS_GPRS_DIAL, IDS_GPRS_APN, IDS_USR, IDS_PASSWD,
    IDS_GPRS_CONNECT_STATUS, IDS_SIGNAL_INTENSITY, IDS_PING_TEST,IDS_CONNECT
  };   
  net_conn_stats_t conn_status = {0};
  u8 connect_type[64] = {0};

  memcpy(&g_3g_conn_info, sys_status_get_3g_info(), sizeof(g3_conn_info_t));

  y = NETWORK_3G_ITEM_Y;
 // y += NETWORK_3G_ITEM_H + NETWORK_3G_ITEM_VGAP;
  for(i = 0; i < G3_CTRL_CNT; i++)
  {
     switch(i)
      {
        case 0:     
          p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_TYPE + i, NETWORK_3G_ITEM_X, y, NETWORK_3G_ITEM_LW+NETWORK_3G_ITEM_RW, COMM_CTRL_H, p_menu, 0);
          text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
          text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
          
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);  
          strcpy((char*)connect_type, "Link Type:                      3G");
          text_set_content_by_ascstr(p_ctrl[i], connect_type);     
          break;
        case 1:  //operator name  
          p_ctrl[i] = ui_comm_static_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_3G_ITEM_X, y, NETWORK_3G_ITEM_LW, 
                                                NETWORK_3G_ITEM_RW);                                             
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          if(g_g3_signal_info.operator != NULL)
            ui_comm_static_set_content_by_ascstr(p_ctrl[i], g_g3_signal_info.operator);
          else
            ui_comm_static_set_content_by_ascstr(p_ctrl[i], (u8*)"Unknown");
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);

        break;

        case 2:  //dial
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_3G_ITEM_X, y, NETWORK_3G_ITEM_LW, 
                                                NETWORK_3G_ITEM_RW, ROOT_ID_NETWORK_CONFIG_3G);
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], g3_dial_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_3g_conn_info.dial_num);
       
        break;

        case 3:  //apn       
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_3G_ITEM_X, y, NETWORK_3G_ITEM_LW, 
                                                NETWORK_3G_ITEM_RW, ROOT_ID_NETWORK_CONFIG_3G);                                             
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], g3_apn_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_3g_conn_info.apn);
          
        break;

        case 4: //username
          p_ctrl[i] = ui_comm_t9_v2_edit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_3G_ITEM_X, y, NETWORK_3G_ITEM_LW, 
                                                NETWORK_3G_ITEM_RW, ROOT_ID_NETWORK_CONFIG_3G);
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i], g3_usrname_set_proc);    
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_3g_conn_info.usr_name);
          if(g_3g_conn_info.apn[0] == 0)
          {
             ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
          }

        break;

        case 5: //password
          p_ctrl[i] = ui_comm_pwdedit_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                             NETWORK_3G_ITEM_X, y, NETWORK_3G_ITEM_LW, 
                                             NETWORK_3G_ITEM_RW);
          ui_comm_pwdedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ctrl_set_keymap(p_ctrl[i], g3_pwd_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], g3_pwd_set_proc); 
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], g_3g_conn_info.passwd);
          if(g_3g_conn_info.apn[0] == 0)
          {
             ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
          }

        break;

        case 6:  //signal status      
          p_ctrl[i] = ui_comm_static_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                                NETWORK_3G_ITEM_X, y, NETWORK_3G_ITEM_LW, 
                                                NETWORK_3G_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]); 
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], g_g3_signal_info.status);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);

        break;

        case 7:   //signal stregth
          p_ctrl[i] = ui_comm_static_create(p_menu, (u8)(IDC_CONNECT_TYPE + i),NETWORK_3G_ITEM_X, 
                                            y, NETWORK_3G_ITEM_LW, NETWORK_3G_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE); 
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], g_g3_signal_info.signal);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);

        break;

        case 8:  //ping test
          p_ctrl[i] = ui_comm_static_create(p_menu, (u8)(IDC_CONNECT_TYPE + i), 
                                            NETWORK_3G_ITEM_X, y, 
                                            NETWORK_3G_ITEM_LW, NETWORK_3G_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);  
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE); 
          ctrl_set_keymap(p_ctrl[i], g3_ping_test_keymap);
          ctrl_set_proc(p_ctrl[i], g3_ping_test_proc);            

        break;

        case 9:  //connect
          p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CONNECT_TYPE + i), NETWORK_3G_ITEM_X, 
                                       y, NETWORK_3G_ITEM_LW+NETWORK_3G_ITEM_RW, 
                                       COMM_CTRL_H, p_menu, 0);
          ctrl_set_rstyle(p_ctrl[i], RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
          text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
          text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);

          conn_status = ui_get_net_connect_status();
          if(conn_status.is_3g_conn)
          {
             text_set_content_by_strid(p_ctrl[i], IDS_DISCONNECT); 
             g3_conn_btn_press = TRUE;
          }
          else
          {
             text_set_content_by_strid(p_ctrl[i], IDS_CONNECT);  
             g3_conn_btn_press = FALSE;
          }       
          
          ctrl_set_keymap(p_ctrl[i], g3_connect_keymap);
          ctrl_set_proc(p_ctrl[i], g3_connect_proc);   

        break;
      default:
        break;
      }

      y += NETWORK_3G_ITEM_H + NETWORK_3G_ITEM_VGAP;
      if(i != 0)
      {
         if(i == 1)
         {
             ctrl_set_related_id(p_ctrl[i], 0, 10, 0, 3);
         }
         else if(i == 9)
         {
              ctrl_set_related_id(p_ctrl[i], 0, 9, 0, 2);
         }
         else
         {
             ctrl_set_related_id(p_ctrl[i], 0, i + 1 - 1, 0, i + 1 + 1);
         }
      
      }

  }

  ctrl_default_proc(p_ctrl[2], MSG_GETFOCUS, 0, 0); 

}


RET_CODE open_network_config_3g(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  
  //create main menu
  p_menu = create_network_config_menu();

  //create gprs items
  create_3g_items(p_menu);

  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);
  
  return SUCCESS;

}

void paint_3g_conn_status(BOOL is_conn)
{
       control_t *p_3g_conn = NULL;      
       
       p_3g_conn = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_CONNECT);
       if(p_3g_conn != NULL)
       {
          if(is_conn == FALSE) 
          {
              text_set_content_by_strid(p_3g_conn, IDS_CONNECT); 
          }
          else
          {
            text_set_content_by_strid(p_3g_conn, IDS_DISCONNECT); 
          }
          ctrl_paint_ctrl(p_3g_conn, TRUE);
       }
       
       p_3g_conn = NULL;
       p_3g_conn = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_STATUS);
       if(p_3g_conn != NULL)
       {
          if(is_conn == FALSE) 
          {
              ui_comm_static_set_content_by_ascstr(p_3g_conn, (u8*)"unconnected"); 
              g3_conn_btn_press = FALSE;
          }
          else
          {
              ui_comm_static_set_content_by_ascstr(p_3g_conn, (u8*)"connected"); 
              g3_conn_btn_press = TRUE;
          }
          ctrl_paint_ctrl(p_3g_conn, TRUE);
       }


}

static RET_CODE on_network_config_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
   if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
   {
      OS_PRINTF("###disconnecting not exit will return from on_network_config_exit####\n");
      return ERR_FAILURE;
   }
   
   ssdata_save_3g_info(p_cont);

  //
  fw_tmr_destroy(ROOT_ID_NETWORK_CONFIG_3G, MSG_CONNECT_NETWORK);  
  
  manage_close_menu(ROOT_ID_NETWORK_CONFIG_3G, 0, 0);

  return SUCCESS;
}

static RET_CODE on_g3_status_update(control_t *p_cont, u16 msg,
                                              u32 para1, u32 para2)

{
#ifndef WIN32	
  control_t *p_status = NULL, *p_operator = NULL, *p_itensity = NULL;
  g3_status_info_t *p_g3_info = NULL;

  p_g3_info = (g3_status_info_t *)para1;  
  p_status = ctrl_get_child_by_id(p_cont, IDC_3G_STATUS);
  if(p_status == NULL)
  {
    return ERR_FAILURE;
  }

  switch(p_g3_info->status)
  {
    case MODEM_DEVICE_HARDWARE_INITIALIZE:
      strcpy((char*)g_g3_signal_info.status, "Hardware Initialize");
    break;
    
    case MODEM_DEVICE_COMMUNICATE_ERROR:
      strcpy((char*)g_g3_signal_info.status, "Connect Error");
    break; 

    case MODEM_DEVICE_NO_SIM_CARD:
      strcpy((char*)g_g3_signal_info.status, "No SIM Card");
    break;
    
    case MODEM_DEVICE_NO_SIGNAL:
      strcpy((char*)g_g3_signal_info.status, "No Signal");
    break;
    
    case MODEM_DEVICE_NETWORK_UNREGISTERED:
      strcpy((char*)g_g3_signal_info.status, "Invalid Network");
    break;
    
    case MODEM_DEVICE_NETWORK_UNCONNECTED:
      strcpy((char*)g_g3_signal_info.status, "Unconnected");
    break;
    
    case MODEM_DEVICE_NETWORK_DIALING:
      strcpy((char*)g_g3_signal_info.status, "Dialing ...");
    break;
    
    case MODEM_DEVICE_NETWORK_CONNECTED:
      strcpy((char*)g_g3_signal_info.status, "Connected");
    break; 
    
    default:
      strcpy((char*)g_g3_signal_info.status, "Default");
    break;

  }
  ui_comm_static_set_content_by_ascstr(p_status, g_g3_signal_info.status);
  ctrl_paint_ctrl(p_status, TRUE); 

  //
  p_operator = ctrl_get_child_by_id(p_cont, IDC_3G_NAME);
  if(p_operator == NULL)
  {
    return ERR_FAILURE;
  }
  p_itensity = ctrl_get_child_by_id(p_cont, IDC_3G_SIGNAL);
  if(p_itensity == NULL)
  {
    return ERR_FAILURE;
  }

  strcpy((char*)g_g3_signal_info.operator, (char*)p_g3_info->operator);
  ui_comm_static_set_content_by_ascstr(p_operator, p_g3_info->operator);
  ctrl_paint_ctrl(p_operator, TRUE);
  
  switch(p_g3_info->strength)
  {
    case 0:
      strcpy((char*)g_g3_signal_info.signal, "0%");
    break;
    
    case 10:
      strcpy((char*)g_g3_signal_info.signal, "20%");    
    break;
    
    case 20:
      strcpy((char*)g_g3_signal_info.signal, "40%");       
    break;
    
    case 30:
      strcpy((char*)g_g3_signal_info.signal, "60%");    
    break;
    
    case 40:
      strcpy((char*)g_g3_signal_info.signal, "80%");      
    break;
    
    case 50:
      strcpy((char*)g_g3_signal_info.signal, "100%");       
    break;
    
    case 100:
      strcpy((char*)g_g3_signal_info.signal, "Signal Abnormal");      
    break;    
    default:
      strcpy((char*)g_g3_signal_info.signal, "0%");
    break;
  }
  
  ui_comm_static_set_content_by_ascstr(p_itensity, (u8*)g_g3_signal_info.signal);
  ctrl_paint_ctrl(p_itensity, TRUE);

#endif
  
  return SUCCESS;
}


static RET_CODE on_network_3g_connect(control_t *p_cont, u16 msg,
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

      g3_conn_info_t g3_conn_info = {{0},{0}};  
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
            fw_tmr_destroy(ROOT_ID_NETWORK_CONFIG_3G, MSG_CONNECT_NETWORK);
            
            break;

          case LINK_TYPE_GPRS:

            break;

          case LINK_TYPE_3G:
            if((net_conn_info.wifi_conn_info < WIFI_CONNECTING) &&
              (net_conn_info.gprs_conn_info < MODEM_DEVICE_NETWORK_START_DIAL))
            {

                 fw_tmr_destroy(ROOT_ID_NETWORK_CONFIG_3G, MSG_CONNECT_NETWORK);

                  //ui_set_net_connect_status(conn_status);                   
                  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_3G, IDC_3G_CONNECT);
                  if(p_ctrl == NULL)
                  {
                    break;
                  }

                  p_ctrl_conn = ctrl_get_parent(p_ctrl);
                  if(p_ctrl_conn == NULL)
                  {
                    break;
                  }
                  p_ctrl_apn = ctrl_get_child_by_id(p_ctrl_conn, IDC_3G_APN);
                  p_ctrl_dial = ctrl_get_child_by_id(p_ctrl_conn, IDC_3G_DIAL);
                  p_ctrl_usrname = ctrl_get_child_by_id(p_ctrl_conn, IDC_3G_USRNAME);
                  p_ctrl_passwd = ctrl_get_child_by_id(p_ctrl_conn, IDC_3G_PASSWORD);
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
                     return ERR_FAILURE;
                  }*/
                  apn_len = str_uni2asc(asc_apn, p_uni_apn);
                  dial_len = str_uni2asc(asc_dial, p_uni_dail);
                  usr_len = str_uni2asc(asc_usr, p_uni_usr);  
                  passwd_len = str_uni2asc(asc_passwd, p_uni_passwd);
                                                
                  //connect 3g
                  if(g3_conn_btn_press == FALSE)
                  {    
                     memcpy(g3_conn_info.apn, asc_apn, apn_len);
                     memcpy(g3_conn_info.dial_num, asc_dial, dial_len);
                     memcpy(g3_conn_info.usr_name, asc_usr, usr_len);
                     memcpy(g3_conn_info.passwd, asc_passwd, passwd_len);

                     do_cmd_connect_g3(&g3_conn_info);
                     //g3_conn_btn_press = TRUE;
                     //text_set_content_by_strid(p_ctrl, IDS_DISCONNECT); 
                  }

                  ctrl_paint_ctrl(p_ctrl, TRUE);
                          
            }
            break;

          default:
            break;

      }

#endif

      return SUCCESS;
}


BEGIN_KEYMAP(network_config_3g_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(network_config_3g_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(network_config_3g_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_network_config_exit)
  ON_COMMAND(MSG_G3_STATUS_UPDATE, on_g3_status_update)
  ON_COMMAND(MSG_CONNECT_NETWORK, on_network_3g_connect)
END_MSGPROC(network_config_3g_cont_proc, ui_comm_root_proc)




//3g connect
BEGIN_MSGPROC(g3_apn_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_g3_edit_apn)
  ON_COMMAND(MSG_NUMBER, on_g3_edit_apn)
END_MSGPROC(g3_apn_set_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(g3_dial_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_g3_edit_dial)
  ON_COMMAND(MSG_NUMBER, on_g3_edit_dial)
END_MSGPROC(g3_dial_set_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(g3_usrname_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_g3_edit_username)
  ON_COMMAND(MSG_NUMBER, on_g3_edit_username)
END_MSGPROC(g3_usrname_set_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(g3_pwd_keymap, ui_comm_t9_keymap)
  ON_EVENT(V_KEY_RED,MSG_SELECT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(g3_pwd_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(g3_pwd_set_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_g3_edit_password)
  ON_COMMAND(MSG_NUMBER, on_g3_edit_password)
END_MSGPROC(g3_pwd_set_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(g3_ping_test_keymap, ui_comm_static_keymap)
  
END_KEYMAP(g3_ping_test_keymap, ui_comm_static_keymap)

BEGIN_MSGPROC(g3_ping_test_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_g3_ping_test_select)
END_MSGPROC(g3_ping_test_proc, cont_class_proc)

BEGIN_KEYMAP(g3_connect_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(g3_connect_keymap, NULL)

BEGIN_MSGPROC(g3_connect_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_g3_dongle_connect)
END_MSGPROC(g3_connect_proc, text_class_proc)




