/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "ui_browser_api.h"
#include "ui_config.h"
#include "ap_playback.h"
#include "browser_adapter.h"
#include "ap_browser.h"


#include "ap_framework.h"

#include "db_dvbs.h"
#include "dvb_protocol.h"
#define BROWSER_MEM_SIZE (12* 1024*1024 + 512 * 1024)
static BOOL b_browser_started = FALSE;
BOOL g_browser_flag = FALSE;
u16 ui_browser_evtmap(u32 event);
#if 0
static s32 get_ui_region_handle(void)
{
  gdi_set_enable(FALSE);

  manage_enable_autoswitch(FALSE);
  //fw_destroy_all_mainwin(TRUE);
  manage_enable_autoswitch(TRUE);

  gdi_set_enable(TRUE);

  return gdi_get_screen_handle();
}

static void recover_ui_region(void)
{

  gdi_set_enable(FALSE);

  manage_enable_autoswitch(TRUE);

  gdi_clear_screen();

  ui_config_normal_osd();
  gdi_set_enable(TRUE);

}
#endif
void ui_browser_init(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_SI_MON;
  OS_PRINTF("ap_frm_do_command BEGIN \n");
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
OS_PRINTF("SEND COMMAND framwork APP_SI_MON ACTIVE \n");
  fw_register_ap_evtmap(APP_SI_MON, ui_browser_evtmap);
  fw_register_ap_msghost(APP_SI_MON, ROOT_ID_BROWSER);


}

void ui_browser_release(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_SI_MON;
  OS_PRINTF("DEACTIVE APP_SI_MON\n");
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  OS_PRINTF("DEACTIVE APP_SI_MON end\n");

  fw_unregister_ap_msghost(APP_SI_MON, ROOT_ID_BROWSER);
  fw_unregister_ap_evtmap(APP_SI_MON);
}

/*
 RET_CODE get_browser_pg_para(browser_pg_para *p_para)
{
   u8 view_id;
   u16 prog_pos = 0, prog_id = 0,pg_cnt = 0;
   dvbs_prog_node_t pg;

    view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
    pg_cnt = db_dvbs_get_count(view_id);
    for(;prog_pos <pg_cnt;prog_pos++)
    {
      prog_id = db_dvbs_get_id_by_view_pos(view_id, prog_pos);
      db_dvbs_get_pg_by_id(prog_id, &pg);
      if(pg.service_type == DVB_DATA_BROADCAST_REF_SVC)
       {
         p_para->net_id = pg.orig_net_id;
         p_para->ts_id = pg.ts_id;
         p_para->s_id = pg.s_id;
       }
     }
    OS_PRINTF("net_id :%d,ts_id:%d, service_id: %d\n", p_para->net_id,p_para->ts_id \
                          ,p_para->s_id );
    if(prog_pos <pg_cnt)
      return SUCCESS;
    else
      return ERR_FAILURE;
}
*/
void ui_browser_start(void)
{
  cmd_t cmd = {0};
  //get_browser_pg_para(&pg_param);
  cmd.id = CMD_START_BROWSER;
  //cmd.data1 =(u32)&pg_param;
  cmd.data2 = BROWSER_MEM_SIZE;
  ap_frm_do_command(APP_SI_MON, &cmd);

}
extern void nc_enable_monitor(class_handle_t handle, u8 tuner_id, BOOL enable);
;

void ui_browser_enter(void)
{
  // dvbs_tp_node_t tp = {0};
    u8 tuner_id = 0, ts_in = 0;

  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  manage_close_menu(ROOT_ID_MAINMENU,0,0);
  gdi_clear_screen();
  ui_stop_play(STOP_PLAY_BLACK, TRUE);

  ui_epg_stop();
  ui_epg_release();
 ui_enable_signal_monitor(FALSE);
  ui_release_signal();
    nc_get_main_tuner_ts(nc_handle, &tuner_id , &ts_in);
  nc_enable_monitor(nc_handle,tuner_id, FALSE);
  //nc_clr_tp_info(nc_handle);
 // nc_enter_monitor_idle(nc_handle);
  ui_browser_init();
  OS_PRINTF("ui_browser_start \n");
  ui_browser_start();
  g_browser_flag = TRUE;
  b_browser_started = TRUE;
}
extern play_param_t g_pb_info;
void ui_browser_exit(void)
{
 // u16 curn_group;
  //u16 prog_id;
  //u16 prog_pos;
  //u8 curn_mode;
  //u32 context;
    //dvbs_prog_node_t pg={0};

    u8 tuner_id = 0, ts_in = 0;

   class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  if(b_browser_started)
  {
      nc_get_main_tuner_ts(nc_handle, &tuner_id , &ts_in);
    nc_enable_monitor(nc_handle,tuner_id , TRUE);
    ui_browser_release();
 //   ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
    ui_epg_init();
    ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
    ui_init_signal();
    fw_destroy_mainwin_by_id(ROOT_ID_BROWSER);
     b_browser_started = FALSE;
     g_browser_flag  = FALSE;
   }
}

BEGIN_AP_EVTMAP(ui_browser_evtmap)
  CONVERT_EVENT(BROWSER_EVT_STOP,   MSG_BROWSER_STOP)
 // CONVERT_EVENT(BROWSER_EVT_PALY_MUSIC,   MSG_BROWSER_PLAY_MUSIC)
  //CONVERT_EVENT(BROWSER_EVT_STOP_MUSIC,   MSG_BROWSER_STOP_MUSIC)
  CONVERT_EVENT(BROWSER_EVT_EXIT,   MSG_BROWSER_EXIT)
END_AP_EVTMAP(ui_browser_evtmap)


