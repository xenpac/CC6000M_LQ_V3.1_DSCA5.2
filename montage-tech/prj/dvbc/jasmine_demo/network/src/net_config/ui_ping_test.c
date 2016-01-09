/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_ping_test.h"
#include "ethernet.h"
#include "ui_network_config_lan.h"
#include "net_svc.h"
#include "ui_rename.h"
#include "ui_keyboard_v2.h"

enum ip_ctrl_id
{
  IDC_IP_ADDR = 1, 
  IDC_TIMES,
  IDC_START,
  IDC_DLG_CONT,
  IDC_DLG_TITLE,
  IDC_DLG_TIMES,
  

};

static u16 ping_test_cont_keymap(u16 key);
static u16 ping_test_item_keymap(u16 key);

static RET_CODE ping_test_item_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE ping_test_url_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


static rename_param_t ping_addr_param = { NULL,32,KB_INPUT_TYPE_SENTENCE};
static s32 ok_times;
static s32 bad_times;


RET_CODE open_ping_test(u32 para1, u32 para2)
{
  control_t *p_menu = NULL,  *p_ctrl[PING_TEST_ITEM_CNT] = {NULL};
  control_t *p_dlg_cont = NULL,*p_dlg_title = NULL,*p_dlg_item[8] = {NULL};
  
  u16 stxt[PING_TEST_ITEM_CNT] = {IDS_IP_ADDRESS, IDS_TIMES, IDS_START};
  u16 text_str[PING_TEST__DLG_ITEM_CNT] ={IDS_TIMES, IDS_BYTES, IDS_OK, IDS_BAD};
  u16 x = 0, y = 0, w = 0, h = 0;
  u32 text_type = 0, align_type = 0;
  
  
  u8 i = 0;
  p_menu = ui_comm_root_create_netmenu(ROOT_ID_PING_TEST,
    0, IM_INDEX_NETWORK_BANNER, IDS_NETWORK_CONFIG);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_menu, ping_test_cont_keymap);

  y = NETWORK_LAN_ITEM_Y;
  for(i = 0;i<PING_TEST_ITEM_CNT;i++)
  {
     switch(i)
      {
        case 0:
          p_ctrl[i]  = ui_comm_t9_v2_edit_create(p_menu, IDC_IP_ADDR + i, NETWORK_LAN_ITEM_X,
          y, NETWORK_LAN_ITEM_LW, NETWORK_LAN_ITEM_RW, ROOT_ID_PING_TEST);
          ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i] , stxt[i]);
          ui_comm_ctrl_set_proc(p_ctrl[i] , ping_test_url_proc);    
          ping_addr_param.uni_str = ui_comm_t9_v2_edit_get_content(p_ctrl[i]);
          ui_comm_t9_v2_edit_set_content_by_ascstr(p_ctrl[i], (u8*)"www.baidu.com");
  
        break;
        case 1:
          p_ctrl[i] = ui_comm_numedit_create(p_menu, IDC_IP_ADDR + i, NETWORK_LAN_ITEM_X, 
          y, NETWORK_LAN_ITEM_LW, NETWORK_LAN_ITEM_RW);
          ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC,
                                                            PING_TIMES_MIN, PING_TIMES_MAX,
                                                            2, 1);
          ui_comm_numedit_set_num(p_ctrl[i], 20);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_num_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_num_proc);
        break;
        case 2:
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_IP_ADDR + i, NETWORK_LAN_ITEM_X, 
          y, NETWORK_LAN_ITEM_LW, NETWORK_LAN_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
          ctrl_set_keymap(p_ctrl[i], ping_test_item_keymap);
          ctrl_set_proc(p_ctrl[i], ping_test_item_proc);
        break;
      default:
        break;
      }

      y += NETWORK_LAN_ITEM_H+NETWORK_LAN_ITEM_VGAP;
      ctrl_set_related_id(p_ctrl[i],
                                      0,                       /* left */
                                      (u8)((i - 1 + PING_TEST_ITEM_CNT)
                                           % PING_TEST_ITEM_CNT + 1),    /* up */
                                      0,                       /* right */
                                      (u8)((i + 1) % PING_TEST_ITEM_CNT + 1));    /* down */
  }
  
  //dlg container ,hide first
  p_dlg_cont = ctrl_create_ctrl(CTRL_CONT, IDC_DLG_CONT, (COMM_BG_W-400)/2, y+NETWORK_LAN_ITEM_VGAP, 400, 200, p_menu, 0);
  ctrl_set_rstyle(p_dlg_cont, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  ctrl_set_sts(p_dlg_cont, OBJ_STS_HIDE);
  p_dlg_title = ctrl_create_ctrl(CTRL_TEXT, IDC_DLG_TITLE, 0, 0, 400, 40, p_dlg_cont, 0);
  ctrl_set_rstyle(p_dlg_title, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
  text_set_font_style(p_dlg_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_dlg_title, STL_CENTER | STL_CENTER);
  text_set_content_type(p_dlg_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_dlg_title, IDS_PING_TEST);

  //dlg items
  x = 0;
  y = 40;
  h = 40;
  for(i=0; i<8; i++)
  {
    switch(i%2)
    {
      case 0:
        x = 0;
        y = 40+i/2*(40+0);
        w = 200;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_CENTER | STL_CENTER;
        break;

      case 1:
        x = 0+200+0;
        w = 200;
        text_type = TEXT_STRTYPE_DEC;
        align_type = STL_CENTER| STL_VCENTER;
        break;
      default:
        break;
    }

    p_dlg_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_DLG_TIMES+i),
                              x, y, w, h, p_dlg_cont, 0);
    ctrl_set_rstyle(p_dlg_item[i], RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
    text_set_font_style(p_dlg_item[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_dlg_item[i], align_type);
    text_set_content_type(p_dlg_item[i], text_type);

    if(i%2==0)
    {
      text_set_content_by_strid(p_dlg_item[i], text_str[i/2]);
    }
  }
  ctrl_default_proc(p_ctrl[2], MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);
  return SUCCESS;

}

static void do_cmd_to_ping_test(ethernet_device_t * p_dev, s32 ping_count, ip_address_t *addr, u8 *url_str)
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  if(p_net_svc->net_svc_is_busy(p_net_svc) == TRUE)
  {
    ui_comm_cfmdlg_open(NULL, IDS_NETWORK_BUSY, NULL, 2000);
    OS_PRINTF("###########net service is busy ,return %d#########\n",p_net_svc->net_svc_is_busy(p_net_svc));
    return ;
  }
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;
  net_svc_para.ping_para.count = (u16)ping_count;
  net_svc_para.ping_para.size = 64;
  net_svc_para.ping_para.ipaddr[0] = (u8)addr->s_b1;
  net_svc_para.ping_para.ipaddr[1] = (u8)addr->s_b2;
  net_svc_para.ping_para.ipaddr[2] = (u8)addr->s_b3;
  net_svc_para.ping_para.ipaddr[3] = (u8)addr->s_b4;
  memcpy(net_svc_para.ping_para.url, url_str, sizeof(net_svc_para.ping_para.url));
  OS_PRINTF("####do_cmd_to_ping_test url is %s#####\n",net_svc_para.ping_para.url);
  p_server = (service_t *)ui_get_net_svc_instance();
  
  p_server->do_cmd(p_server, NET_DO_PING_TEST, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  OS_PRINTF("###########check net service  return %d#########\n",p_net_svc->net_svc_is_busy(p_net_svc));
  OS_PRINTF("########now , do cmd to ping test ########\n");
  
}

static void get_url_strings(u8 *asc)
{
  control_t *p_ctrl = NULL;
  u16 *temp;
  
  p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_PING_TEST, IDC_IP_ADDR);
  temp = ui_comm_t9_v2_edit_get_content(p_ctrl);

  if(temp != NULL)
  {
    str_uni2asc(asc, temp);
  }
}

void init_ok_bad_times()
{
  ok_times = 0;
  bad_times = 0;
}
static RET_CODE on_ping_test_item_select(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_dlg_container = NULL, *p_ctrl = NULL;
  control_t *p_times = NULL, *p_ipaddr = NULL;
  u8 ping_url_asc[32+1] = {0};
  ip_address_t addr = {0};
  s32 ping_count = 0;
  net_conn_stats_t eth_connt_stats;
  
  init_ok_bad_times();
  p_dlg_container = ctrl_get_child_by_id(p_sub->p_parent, IDC_DLG_CONT);
  ctrl_set_sts(p_dlg_container,OBJ_STS_SHOW);
  ctrl_set_attr(p_dlg_container, OBJ_ATTR_ACTIVE);
  ctrl_paint_ctrl(p_dlg_container, TRUE);
  
  //get control address
  get_url_strings(ping_url_asc);
  OS_PRINTF("########ping test url is %s######\n", ping_url_asc);

  //get ping times
  p_times = ctrl_get_child_by_id(p_sub->p_parent, IDC_TIMES);
  ping_count = (s32)ui_comm_numedit_get_num(p_times);
  
   //reset ping times
  p_ctrl = ctrl_get_child_by_id(p_dlg_container, IDC_DLG_TIMES+1);
  text_set_content_by_dec(p_ctrl, ping_count);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  
  //reset ping bytes
  p_ctrl = ctrl_get_child_by_id(p_dlg_container, IDC_DLG_TIMES+3);
 // text_set_content_by_dec(p_ctrl, (s32)result.len);
  text_set_content_by_dec(p_ctrl, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  
   //reset ok times is 0
  p_ctrl = ctrl_get_child_by_id(p_dlg_container, IDC_DLG_TIMES+5);
  text_set_content_by_dec(p_ctrl, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  
  //reset bad times is 0
  p_ctrl = ctrl_get_child_by_id(p_dlg_container, IDC_DLG_TIMES+7);
  text_set_content_by_dec(p_ctrl, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  
  eth_connt_stats = ui_get_net_connect_status();
  if(!eth_connt_stats.is_eth_conn  &&  !eth_connt_stats.is_wifi_conn
     &&  !eth_connt_stats.is_gprs_conn &&  !eth_connt_stats.is_3g_conn)
  {
    OS_PRINTF("######ehternet and wifi are not connected, so will return###########\n");
    text_set_content_by_dec(p_ctrl, ping_count); //bad times set as ping_count
    ctrl_paint_ctrl(p_ctrl, TRUE);
    
    return SUCCESS;
  }
  else
  {
    OS_PRINTF("######ehternet is connected, will do cmd to ping test###########\n");
    do_cmd_to_ping_test(NULL, ping_count, &addr, ping_url_asc);
    ctrl_set_attr(p_sub, OBJ_ATTR_INACTIVE);
    ctrl_paint_ctrl(p_sub, TRUE);
    ctrl_set_attr(p_times, OBJ_ATTR_INACTIVE);
    ctrl_paint_ctrl(p_times, TRUE);
    p_ipaddr = ctrl_get_child_by_id(p_sub->p_parent, IDC_IP_ADDR);
    ctrl_set_attr(p_ipaddr, OBJ_ATTR_INACTIVE);
    ctrl_paint_ctrl(p_ipaddr, TRUE);
    return SUCCESS;
  }
}

static RET_CODE on_ping_test_update(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_dlg_container = NULL, *p_ctrl = NULL, *p_times = NULL, *p_satrt = NULL, *p_ipaddr = NULL;
  u16 ping_count = 0;
   
  OS_PRINTF("update ping test reesult, ok is %d\n", para1);
  p_dlg_container = ctrl_get_child_by_id(p_sub->p_parent, IDC_DLG_CONT);
  p_times = ctrl_get_child_by_id(p_sub->p_parent, IDC_TIMES);
  p_satrt = ctrl_get_child_by_id(p_sub->p_parent, IDC_START);
  ping_count = (u16)ui_comm_numedit_get_num(p_times);
  
  if(para1 == 1)
  {
    ok_times += 1;
  }
  else
  {
    bad_times += 1;
  }

 //ping bytes
  p_ctrl = ctrl_get_child_by_id(p_dlg_container, IDC_DLG_TIMES+3);
  text_set_content_by_dec(p_ctrl, 64);
 //ok times
  p_ctrl = ctrl_get_child_by_id(p_dlg_container, IDC_DLG_TIMES+5);
  text_set_content_by_dec(p_ctrl, (s32)ok_times);
  //bad times
  p_ctrl = ctrl_get_child_by_id(p_dlg_container, IDC_DLG_TIMES+7);
  text_set_content_by_dec(p_ctrl, (s32)bad_times);
  ctrl_paint_ctrl(p_dlg_container, TRUE);
  if(ok_times + bad_times == ping_count)
  {
   ctrl_set_attr(p_satrt,OBJ_ATTR_ACTIVE);
   ctrl_set_sts(p_satrt, OBJ_STS_SHOW);
   ctrl_process_msg(p_satrt, MSG_GETFOCUS, 0, 0);
   ctrl_paint_ctrl(p_satrt, TRUE);
   ctrl_set_attr(p_times, OBJ_ATTR_ACTIVE);
   ctrl_set_sts(p_times, OBJ_STS_SHOW);
   ctrl_paint_ctrl(p_times, TRUE);
   p_ipaddr = ctrl_get_child_by_id(p_sub->p_parent, IDC_IP_ADDR);
   ctrl_set_attr(p_ipaddr, OBJ_ATTR_ACTIVE);
   ctrl_set_sts(p_ipaddr, OBJ_STS_SHOW);
   ctrl_paint_ctrl(p_ipaddr, TRUE);
  }
  OS_PRINTF("#####after ping test the ok times = %d, bad times is %d######\n", ok_times, bad_times);

  return SUCCESS;
}

static RET_CODE on_ping_test_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  
  return ERR_NOFEATURE;
}


/*******************ping test url edit box func****************/
RET_CODE edit_ping_test_url_update(u16* p_unistr)
{
  control_t *p_edit_cont;
  
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_PING_TEST, IDC_IP_ADDR);
  MT_ASSERT(p_edit_cont != NULL);
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_edit_ping_test_url(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = ping_addr_param.type;
  param.max_len = 32;
  param.cb = edit_ping_test_url_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

BEGIN_KEYMAP(ping_test_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ping_test_cont_keymap, ui_comm_root_keymap)

BEGIN_KEYMAP(ping_test_item_keymap, ui_comm_static_keymap)
  
END_KEYMAP(ping_test_item_keymap, ui_comm_static_keymap)

BEGIN_MSGPROC(ping_test_item_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_ping_test_item_select)
  ON_COMMAND(MSG_PING_TEST, on_ping_test_update)
  ON_COMMAND(MSG_DESTROY, on_ping_test_destory)
END_MSGPROC(ping_test_item_proc, cont_class_proc)

BEGIN_MSGPROC(ping_test_url_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_ping_test_url)
END_MSGPROC(ping_test_url_proc, ui_comm_edit_proc)
