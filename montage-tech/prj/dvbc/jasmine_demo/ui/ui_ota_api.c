/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#include "ui_common.h"

BOOL b_ota_init = FALSE;

u16 ui_ota_evtmap(u32 event);


static BOOL _ui_ota_api_get_init_flag()
{
  return b_ota_init;
}

static void _ui_ota_api_set_init_flag(BOOL index)
{
  b_ota_init = index;

  return;
}

void ui_ota_api_init(void)
{
  cmd_t cmd = {0};

  if(_ui_ota_api_get_init_flag())
  {
    return;
  }
  
  OS_PRINTF("%s(Line: %d): ui_ota_api_init.\n",__FUNCTION__, __LINE__);
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_OTA;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_register_ap_evtmap(APP_OTA, ui_ota_evtmap);
  fw_register_ap_msghost(APP_OTA, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_OTA, ROOT_ID_OTA_SEARCH);
  _ui_ota_api_set_init_flag(TRUE);

  return;
}

void ui_ota_api_stop(u32 para1, u32 para2)
{
  cmd_t cmd;

  if(!_ui_ota_api_get_init_flag())
  {
    return;
  }

  OS_PRINTF("%s(Line: %d): OTA_CMD_STOP.\n",__FUNCTION__, __LINE__);
  cmd.id = OTA_CMD_STOP;
  cmd.data1 = para1;
  cmd.data2 = para2;
  ap_frm_do_command(APP_OTA, &cmd);   

  return;
}

void ui_ota_api_release(void)
{
  cmd_t cmd = {0};
  
  OS_PRINTF("%s(Line: %d): ui_ota_api_release.\n",__FUNCTION__, __LINE__);
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_OTA;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_evtmap(APP_OTA);
  fw_unregister_ap_msghost(APP_OTA, ROOT_ID_BACKGROUND);
  fw_unregister_ap_msghost(APP_OTA, ROOT_ID_OTA_SEARCH);
  _ui_ota_api_set_init_flag(FALSE);

  return;
}

void ui_ota_api_start_search(u32 para1, u32 para2)
{
  cmd_t cmd = {0};

  if(!_ui_ota_api_get_init_flag())
  {
    return;
  }

  OS_PRINTF("%s(Line: %d): OTA_CMD_START_SEARCHING.\n",__FUNCTION__, __LINE__);
  cmd.id = OTA_CMD_START_SEARCHING;
  cmd.data1 = para1;
  cmd.data2 = para2;
  ap_frm_do_command(APP_OTA, &cmd);   

  return;
}

void ui_ota_api_stop_search(u32 para1, u32 para2)
{
  cmd_t cmd;

  if(!_ui_ota_api_get_init_flag())
  {
    return;
  }

  OS_PRINTF("%s(Line: %d): OTA_CMD_STOP_SEARCHING.\n",__FUNCTION__, __LINE__);
  cmd.id = OTA_CMD_STOP_SEARCHING;
  cmd.data1 = para1;
  cmd.data2 = para2;
  
  ap_frm_do_command(APP_OTA, &cmd);   
}


BEGIN_AP_EVTMAP(ui_ota_evtmap)
  CONVERT_EVENT(OTA_EVT_UPG_INFO_FOUND,   MSG_OTA_FOUND)
  CONVERT_EVENT(OTA_EVT_TRIGGER_RESET, MSG_OTA_TRIGGER_RESET)  
END_AP_EVTMAP(ui_ota_evtmap)
