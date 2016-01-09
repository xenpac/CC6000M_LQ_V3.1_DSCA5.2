/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_upgrade_api.h"
#ifdef ENABLE_CA
#include "config_cas.h"
#endif
#ifdef WIN32
#include "uart_win32.h"
#endif
extern BOOL is_finished_DS;

static RET_CODE upgrade_null_write_byte(u8 id, u8 chval)
{
  return SUCCESS;
}


static RET_CODE upgrade_null_read_byte(u8 id, u8 *data, u32 timeout)
{
  return SUCCESS;
}


void ui_init_upg(app_id_t app_id, u8 root_id, ap_evtmap_t p_evtmap)
{
  cmd_t cmd = {0};

  if((APP_FRAMEWORK >= app_id )||(APP_LAST <= app_id))
  {
      MT_ASSERT(0);
  }

#ifdef  CORE_DUMP_DEBUG
  mtos_task_suspend(TRANSPORT_SHELL_TASK_PRIORITY);
#endif

  // deactivate others
  ui_epg_stop();
  ui_enable_playback(FALSE);

  ui_release_signal();
  ui_time_release();
  ui_set_book_flag(FALSE);

  // activate upg
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = app_id;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  fw_register_ap_evtmap(app_id, p_evtmap);
  fw_register_ap_msghost(app_id, root_id);

}


void ui_release_upg(app_id_t app_id, u8 root_id)
{
  cmd_t cmd = {0};

  if((APP_FRAMEWORK >= app_id )||(APP_LAST <= app_id))
  {
      MT_ASSERT(0);
  }

  // deactivate upg
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = app_id;
   ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_evtmap(app_id);
  fw_unregister_ap_msghost(app_id, root_id);
  //activate others

#if((!defined WIN32) )
  mtos_register_putchar((s32 (*)(u8, u8))(uart_write_byte));
  mtos_register_getchar((s32 (*)(u8, u8 *, u32))(uart_read_byte));
#endif
  ui_time_init();
  ui_init_signal();

  ui_enable_playback(TRUE);
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  ui_set_book_flag(TRUE);
#ifdef DIVI_CA  
  ui_ca_set_roll_status(TRUE);
#endif
#ifdef CAS_CONFIG_DS_5_0
  if(osd_roll_stop_msg_unsend())
  {
  	ui_ca_do_cmd(CAS_CMD_OSD_ROLL_OVER, 0, 0);
  }
#endif
  is_finished_DS = TRUE;
#ifdef  CORE_DUMP_DEBUG
  mtos_task_resume(TRANSPORT_SHELL_TASK_PRIORITY);
#endif
}


void ui_start_upgrade(upg_mode_t md, u8 block_num, upg_block_t *block)
{
  cmd_t cmd;

  cmd.id = UPG_CMD_START;
  cmd.data1 = (md << 16) | block_num;
  cmd.data2 = (u32)block;

  ap_frm_do_command(APP_UPG, &cmd);

#ifndef WIN32
  mtos_register_putchar(upgrade_null_write_byte);
  mtos_register_getchar(upgrade_null_read_byte);
#else
  uart_init(1);
#endif
}


void ui_exit_upgrade(app_id_t app_id, BOOL is_exit_all)
{
  cmd_t cmd;

  cmd.id = UPG_CMD_EXIT;
  cmd.data1 = (u32)is_exit_all;
  cmd.data2 = 0;

  ap_frm_do_command(app_id, &cmd);
}



BEGIN_AP_EVTMAP(ui_upg_evtmap)
  CONVERT_EVENT(UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(UPG_EVT_QUIT_UPG, MSG_UPG_QUIT)
  CONVERT_EVENT(UPG_EVT_SUCCESS, MSG_UPG_OK)
END_AP_EVTMAP(ui_upg_evtmap)

