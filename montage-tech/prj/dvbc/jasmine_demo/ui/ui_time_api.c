/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"

u16 ui_time_evtmap(u32 event);

void ui_time_init(void)
{
  cmd_t cmd = {0};

  // activate upg
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_TIME;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_TIME, ui_time_evtmap);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_EPG);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_TIMER);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_TIME_SET);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_PROG_BAR);
  fw_register_ap_msghost(APP_TIME, ROOT_ID_TIMESHIFT_BAR); 
  fw_register_ap_msghost(APP_TIME, ROOT_ID_PVR_REC_BAR); 
  fw_register_ap_msghost(APP_TIME, ROOT_ID_PVR_PLAY_BAR); 
  fw_register_ap_msghost(APP_TIME, ROOT_ID_SMALL_LIST); 
}

void ui_time_release(void)
{
  cmd_t cmd = {0};

  // deactivate upg
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_TIME;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_evtmap(APP_TIME);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_BACKGROUND); 
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_EPG); 
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_TIMER); 
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_TIME_SET);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_PROG_BAR);
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_TIMESHIFT_BAR); 
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_PVR_REC_BAR); 
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_PVR_PLAY_BAR);   
  fw_unregister_ap_msghost(APP_TIME, ROOT_ID_SMALL_LIST);
}

void ui_time_enable_heart_beat(BOOL is_enable)
{
  cmd_t cmd = {0};

  if(is_enable)
  {
    cmd.id = TIME_CMD_OPEN_HEART;
  }
  else
  {
    cmd.id = TIME_CMD_CLOSE_HEART;
  }
  ap_frm_do_command(APP_TIME, &cmd);
}


#if 0
void ui_time_req_tdt(void)
{
  cmd_t cmd = {0};

  cmd.id = TIME_CMD_REQ_TDT;

  ap_frm_do_command(APP_TIME, &cmd);
}
#endif

BEGIN_AP_EVTMAP(ui_time_evtmap)
  CONVERT_EVENT(TIME_EVT_TIME_UPDATE, MSG_TIME_UPDATE) 
  CONVERT_EVENT(TIME_EVT_LNB_SHORT, MSG_LNB_SHORT)
  CONVERT_EVENT(TIME_EVT_HEART_BEAT, MSG_HEART_BEAT)
END_AP_EVTMAP(ui_time_evtmap)
