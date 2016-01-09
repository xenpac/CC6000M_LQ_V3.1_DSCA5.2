/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "pti.h"
#include "dvb_svc.h"
#include "ap_ota.h"
#include "ui_ota_api.h"

u16 ui_ota_evtmap(u32 event);

void ui_ota_relock(ota_info_t *p_info)
{
  cmd_t cmd = {0};

  cmd.id = OTA_CMD_RELOCK;
  cmd.data1 = (u32)p_info;
  
  ap_frm_do_command(APP_OTA, &cmd);   
}

/***************************
para:is user force
*************************/
void ui_ota_start(u32 para)
{
  cmd_t cmd = {0};

  cmd.id = OTA_CMD_START;
  cmd.data2 = para;
  
  ap_frm_do_command(APP_OTA, &cmd);   
}


void ui_enable_ota(BOOL is_enable)
{
  cmd_t cmd = {0};

  cmd.id = (u8)is_enable ?
           AP_FRM_CMD_ACTIVATE_APP : AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_OTA;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  if(is_enable)
  {
    fw_register_ap_evtmap(APP_OTA, ui_ota_evtmap);
    fw_register_ap_msghost(APP_OTA, ROOT_ID_BACKGROUND);
    fw_register_ap_msghost(APP_OTA, ROOT_ID_OTA_USER_INPUT);
    fw_register_ap_msghost(APP_OTA, ROOT_ID_OTA_UPGRADE);
  }
  else
  {
    fw_unregister_ap_evtmap(APP_OTA);
    fw_unregister_ap_msghost(APP_OTA, ROOT_ID_BACKGROUND);
    fw_unregister_ap_msghost(APP_OTA, ROOT_ID_OTA_USER_INPUT);
    fw_unregister_ap_msghost(APP_OTA, ROOT_ID_OTA_UPGRADE);
  }  
}

void ui_ota_burn_flash(BOOL is_burn)
{
  cmd_t cmd = {0};

  cmd.id = OTA_CMD_BURN_START;
  cmd.data1 = (u32)is_burn;
  cmd.data2 = 0;
  
  ap_frm_do_command(APP_OTA, &cmd);     
}


void ui_enable_uio(BOOL is_enable)
{
  cmd_t cmd = {0};

  cmd.id = (u8)is_enable ?
           AP_FRM_CMD_ACTIVATE_APP : AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
}


BEGIN_AP_EVTMAP(ui_ota_evtmap)
  CONVERT_EVENT(OTA_EVT_PROGRESS, MSG_OTA_PROGRESS)  
  CONVERT_EVENT(OTA_EVT_LOCKED, MSG_OTA_LOCK)  
  CONVERT_EVENT(OTA_EVT_UNLOCKED,   MSG_OTA_UNLOCK)
  CONVERT_EVENT(OTA_EVT_START_DL, MSG_OTA_START_DL)  
  //CONVERT_EVENT(OTA_EVT_START_CHECKING,   MSG_OTA_CHECKING)
  CONVERT_EVENT(OTA_EVT_BURNING, MSG_OTA_BURNING)  
  CONVERT_EVENT(OTA_EVT_UPG_COMPLETE,   MSG_OTA_FINISH)
  //CONVERT_EVENT(OTA_EVT_UPGRADE, MSG_OTA_UPGRADE)
  CONVERT_EVENT(OTA_EVT_UNZIP_OK, MSG_OTA_UNZIP_OK)
  CONVERT_EVENT(OTA_EVT_TABLE_TIMEOUT, MSG_OTA_TABLE_TIMEOUT)
  CONVERT_EVENT(OTA_EVT_LOADER_MON_END, MSG_OTA_LOADER_MON_END)
  CONVERT_EVENT(OTA_EVT_UPG_INFO_FOUND, MSG_OTA_FOUND)
END_AP_EVTMAP(ui_ota_evtmap)

