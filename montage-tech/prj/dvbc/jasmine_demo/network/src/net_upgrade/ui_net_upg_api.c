/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_net_upg_api.h"


//extern u8 *ui_get_customer(void);

static net_upg_api_info_t s_net_upg_get;
static BOOL s_net_upg_started = FALSE;

u16 net_upg_evtmap(u32 event);

static void net_upg_callback(net_upg_api_evt_t event, void *p_data, u16 context)
{
  event_t evt = {0};
  
  switch (event)
  {
  case NET_UPG_EVT_CHECK_DONE:
    
    evt.id = UI_NET_UPG_EVT_CHECK_DONE;
    evt.data1 = (u32)context;
    evt.data2 = (u32)p_data;
    OS_PRINTF("\n##net_upg_callback NET_UPG_EVT_CHECK_DONE[%d]!\n",
      ((net_upg_api_info_t *)p_data)->item_cnt);
    memcpy(&s_net_upg_get, (void *)p_data, sizeof(net_upg_api_info_t));
    break;
  case NET_UPG_EVT_PROGRESS:
    if (context == 0)
    {
      evt.id = UI_NET_UPG_EVT_DOWN_PROGRESS;
    }
    else
    {
      evt.id = UI_NET_UPG_EVT_BURN_PROGRESS;
    }
    
    evt.data1 = (u32)context;
    evt.data2 = *((u32 *)p_data);
    break;
   case NET_UPG_EVT_FAIL:
    evt.id = UI_NET_UPG_EVT_FAIL;
    evt.data1 = 0;
    evt.data2 = 0;
    OS_PRINTF("\n##net_upg_callback NET_UPG_EVT_FAIL!\n");
    break;
  default:
    break;
  }
  ap_frm_send_evt_to_ui(APP_USB_UPG, &evt);
}

net_upg_api_info_t *ui_net_upg_get_upg_info(void)
{
  return (net_upg_api_info_t *)&s_net_upg_get;
}

RET_CODE ui_net_upg_init(void)
{
  mul_net_upg_attr_t net_upg_attr;

  if (!s_net_upg_started)
  {
    ui_release_signal();
    ui_time_release();
    ui_set_book_flag(FALSE);

    memset(&s_net_upg_get, 0, sizeof(mul_net_upg_attr_t));
    memset(&net_upg_attr, 0, sizeof(mul_net_upg_attr_t));

    net_upg_attr.stk_size = (32 * KBYTES);
    net_upg_attr.cb = (net_upg_call_back)net_upg_callback;

    fw_register_ap_evtmap(APP_USB_UPG, net_upg_evtmap);
    fw_register_ap_msghost(APP_USB_UPG, ROOT_ID_UPGRADE_BY_NETWORK); 
    fw_register_ap_msghost(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE); 
    s_net_upg_started = TRUE;
    return mul_net_upg_chn_create(&net_upg_attr);
  }
  else
  {
    return SUCCESS;
  }
}

RET_CODE ui_net_upg_start_check(protocol_t pt, u8 *p_addr, u8 *p_usr_asc, u8 *p_pwd_asc)
{
  net_upg_config_t cfg;
  dmh_block_info_t dm_head;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u8 version[20] = {0};
  
  if (!s_net_upg_started)
  {
    return ERR_FAILURE;
  }
  memset(&cfg, 0, sizeof(net_upg_config_t));
#if 1
  dm_get_block_header(dm_handle, 0xFF, &dm_head);
  cfg.hw = atoi(dm_head.version);
  dm_read(dm_handle, SS_DATA_BLOCK_ID, 0, 0, 20, version);
  cfg.sw = atoi(version);
#else
  cfg.hw = 1;
  cfg.sw = 2;
#endif
  cfg.type = pt;
  strcpy(cfg.customer, "Demo");
  strcpy(cfg.user_name, p_usr_asc);
  strcpy(cfg.pwd, p_pwd_asc);
  if (pt == FTP)
  {
    strcpy(cfg.ftp, p_addr);
  }

  strcpy(cfg.xml, "ftproot.xml");

  OS_PRINTF("\n##mul_net_upg_start_check [%d][%s][%s, %s]\n", pt, cfg.ftp, cfg.user_name, cfg.pwd);
  mul_net_upg_start_check(&cfg);
  return SUCCESS;
}

RET_CODE ui_net_upg_upgrade(net_upg_api_up_cfg_t *p_cfg)
{
  if (!s_net_upg_started)
  {
    return ERR_FAILURE;
  }
  OS_PRINTF("\n##ui_net_upg_upgrade sw[%lu]\n", p_cfg->sw);
  return mul_net_upg_start_upgrade(p_cfg);
}

RET_CODE ui_net_upg_burn(void)
{
  if (!s_net_upg_started)
  {
    return ERR_FAILURE;
  }
  return mul_net_upg_start_burn_flash();
}

RET_CODE ui_net_upg_stop(void)
{
  OS_PRINTF("\n##ui_net_upg_stop !\n");
  
  if (s_net_upg_started)
  {
    ui_time_init();
    ui_init_signal();
    ui_set_book_flag(TRUE);//fix 33800
    
    s_net_upg_started = FALSE;
    fw_unregister_ap_evtmap(APP_USB_UPG);
    fw_unregister_ap_msghost(APP_USB_UPG, ROOT_ID_UPGRADE_BY_NETWORK);
    fw_unregister_ap_msghost(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);
    mul_net_upg_stop();
    mul_net_upg_chn_destroy(0);
  }
  return SUCCESS;
}

BEGIN_AP_EVTMAP(net_upg_evtmap)
  CONVERT_EVENT(UI_NET_UPG_EVT_CHECK_DONE, MSG_NET_UPG_EVT_CHECK_DONE)
  CONVERT_EVENT(UI_NET_UPG_EVT_DOWN_PROGRESS, MSG_NET_UPG_EVT_DOWN_PROGRESS)
  CONVERT_EVENT(UI_NET_UPG_EVT_BURN_PROGRESS, MSG_NET_UPG_EVT_BURN_PROGRESS)
  CONVERT_EVENT(UI_NET_UPG_EVT_FAIL, MSG_NET_UPG_EVT_FAIL)
END_AP_EVTMAP(net_upg_evtmap)

