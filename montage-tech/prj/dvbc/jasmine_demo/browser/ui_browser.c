/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "ui_browser.h"
#include "ui_browser_api.h"
#include "browser_adapter.h"

#include "ap_framework.h"
#include "ap_browser.h"
#include "kvdb_event.h"
u16 browser_root_keymap(u16 key);
RET_CODE browser_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
extern play_param_t g_pb_info;
//extern void* p_sub_rgn;

RET_CODE open_browser(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
    p_cont = fw_create_mainwin(ROOT_ID_BROWSER,
                                 BROWSER_L, BROWSER_T,
                                  BROWSER_W, BROWSER_H,
                                  RSC_INVALID_ID, 0,
                                  OBJ_ATTR_ACTIVE, 0);

  if(NULL == p_cont)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, browser_root_keymap);
  ctrl_set_proc(p_cont, browser_root_proc);
  ctrl_set_rstyle(p_cont, RSI_BROWSER_CONT, RSI_BROWSER_CONT, RSI_BROWSER_CONT);

  ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont),FALSE);

OS_PRINTF("ui_browser_enter \n");
  ui_browser_enter();
  /*enter ipanel browser*/
  return SUCCESS;

}
static RET_CODE on_browser_send_irkey_msg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  cmd_t cmd = {0};
  BOOL status = TRUE;
  #ifndef WIN32
  switch(msg)
  {

    case MSG_BROWSER_IRKEY_UP:
       cmd.data1 =IR_KEY_UP;
      break;
    case MSG_BROWSER_IRKEY_DOWN:
       cmd.data1 =IR_KEY_DOWN;
      break;
    case MSG_BROWSER_IRKEY_LEFT:
       cmd.data1 =IR_KEY_LEFT;
      break;
    case MSG_BROWSER_IRKEY_RIGHT:
       cmd.data1 =IR_KEY_RIGHT;
      break;
    case MSG_BROWSER_IRKEY_SELECT:
       cmd.data1 =IR_KEY_OK;
      break;
  
    case MSG_BROWSER_IRKEY_EXIT:
       cmd.data1 =IR_KEY_EXIT;
      break;
   
   default :
     status = FALSE;
     break;
   }
  #endif
  if(status == TRUE)
  {
     cmd.id = CMD_SEND_IRKEY_MSG;
     cmd.data2 = 0;
     ap_frm_do_command(APP_SI_MON, &cmd);
   }


  return SUCCESS;
}

 static RET_CODE on_browser_stop(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  OS_PRINTF("\n on_browser_stop");
  ui_browser_exit();
    if(fw_find_root_by_id(ROOT_ID_MAINMENU) == NULL)
  {
    manage_open_menu(ROOT_ID_MAINMENU,0,0);
  }
//  manage_open_menu(ROOT_ID_MAINMENU, 0, 0);
  return SUCCESS;
}
/*
static RET_CODE on_browser_play_music(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  cmd_t cmd;
  u8 view_id;
  u16 pg_cnt =0,prog_pos = 0,prog_id = 0;
  dvbs_prog_node_t pg;
  //OS_PRINTF("\n dave test play music  s_id :%d",para2);
     if (ui_is_pause())
   {
     ui_set_pause(FALSE);
   }
    view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
    pg_cnt = db_dvbs_get_count(view_id);
     for(;prog_pos <pg_cnt;prog_pos++)
    {
      prog_id = db_dvbs_get_id_by_view_pos(view_id, prog_pos);
      db_dvbs_get_pg_by_id(prog_id, &pg);
      if(pg.s_id == para2)
      {
        break;
       }
     }


//   MT_ASSERT(prog_pos < pg_cnt);
   load_play_paramter_by_pgid(&g_pb_info, prog_id);
   // to play
  cmd.id = PB_CMD_PLAY;
  cmd.data1 = (u32)&g_pb_info;
  cmd.data2 = 0;
  ap_frm_do_command(APP_PLAYBACK, &cmd);
   return SUCCESS;
}

static RET_CODE browser_stop_music(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  cmd_t cmd;
 // OS_PRINTF("\n  test stop music ***************** ");
  cmd.id = PB_CMD_STOP;
  cmd.data1 = STOP_PLAY_NONE;
  cmd.data2 = 0;
  ap_frm_do_command(APP_PLAYBACK, &cmd);
  return SUCCESS;
}
*/

#if 0
static RET_CODE browser_restore_region(void)
{
  void* p_disp;
  s32 ret;
  rect_size_t rect_size = {0};
  point_t pos;
  //OS_PRINTF("\n dave test browser_restore_region****************");
  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);

  rect_size.w = AD_SCREEN_WIDTH;
  rect_size.h = AD_SCREEN_HEIGHT;
  p_sub_rgn = region_create(&rect_size,PIX_FMT_ARGB8888);
  MT_ASSERT(NULL != p_sub_rgn);

  pos.x = 0;
  pos.y = 0;

  ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_SUBTITL, p_sub_rgn, &pos,
NULL);
  MT_ASSERT(SUCCESS == ret);

  region_show(p_sub_rgn, TRUE);

  disp_layer_alpha_onoff(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
  region_alpha_onoff(p_sub_rgn, FALSE);
  return SUCCESS;

}
#endif

static RET_CODE on_browser_exit(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
   //browser_restore_region();
   cmd_t cmd = {0};
   //OS_PRINTF("\n dave test browser_exit *********************");
   cmd.id = CMD_STOP_BROWSER;
   cmd.data2 = 0;
   ap_frm_do_command(APP_SI_MON, &cmd);
   return SUCCESS;
}

BEGIN_KEYMAP(browser_root_keymap, NULL)

  ON_EVENT(V_KEY_UP, MSG_BROWSER_IRKEY_UP)
  ON_EVENT(V_KEY_DOWN, MSG_BROWSER_IRKEY_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_BROWSER_IRKEY_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_BROWSER_IRKEY_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_BROWSER_IRKEY_SELECT)
 
  ON_EVENT(V_KEY_EXIT, MSG_BROWSER_IRKEY_EXIT)
  //ON_EVENT(V_KEY_INFO, MSG_BROWSER_IRKEY_HELP)

  //ON_EVENT(V_KEY_FOWARD, MSG_BROWSER_IRKEY_FASTFORWARD)
  //ON_EVENT(V_KEY_REWIND, MSG_BROWSER_IRKEY_REWIND)
 // ON_EVENT(V_KEY_HOMEPAGE, MSG_BROWSER_IRKEY_HOMEPAGE)
  //ON_EVENT(V_KEY_REFRESH, MSG_BROWSER_IRKEY_REFRESH)
  //ON_EVENT(V_KEY_PAGE_UP, MSG_BROWSER_IRKEY_PAGE_UP)
//  ON_EVENT(V_KEY_PAGE_DOWN, MSG_BROWSER_IRKEY_RAGE_DN)
  //ON_EVENT(V_KEY_ESC, MSG_BROWSER_IRKEY_ESC)
//  ON_EVENT(V_KEY_INFO, MSG_BROWSER_IRKEY_INFO)
 // ON_EVENT(V_KEY_CH_UP, MSG_BROWSER_IRKEY_CH_UP)
 // ON_EVENT(V_KEY_CH_DN, MSG_BROWSER_IRKEY_CH_DN)
 // ON_EVENT(V_KEY_RED, MSG_BROWSER_IRKEY_RED)
  //ON_EVENT(V_KEY_YELLOW,MSG_BROWSER_IRKEY_YELLOW)
  //ON_EVENT(V_KEY_GREEN,MSG_BROWSER_IRKEY_GREEN)
 // ON_EVENT(V_KEY_BLUE,MSG_BROWSER_IRKEY_BLUE)
  //ON_EVENT(V_KEY_FORWARD,MSG_BROWSER_IRKEY_FORWARD)
  //ON_EVENT(V_KEY_BACKWARD,MSG_BROWSER_IRKEY_BACKWARD)
   //ON_EVENT(V_KEY_IME,MSG_BROWSER_IRKEY_IME)
 //  ON_EVENT(V_KEY_LANGUAGE_SWITCH,MSG_BROWSER_IRKEY_LANGUAGE)
END_KEYMAP(browser_root_keymap, NULL)

BEGIN_MSGPROC(browser_root_proc, NULL)

  ON_COMMAND(MSG_BROWSER_IRKEY_UP, on_browser_send_irkey_msg)
  ON_COMMAND(MSG_BROWSER_IRKEY_DOWN, on_browser_send_irkey_msg)
  ON_COMMAND(MSG_BROWSER_IRKEY_LEFT, on_browser_send_irkey_msg)
  ON_COMMAND(MSG_BROWSER_IRKEY_RIGHT, on_browser_send_irkey_msg)
  ON_COMMAND(MSG_BROWSER_IRKEY_SELECT, on_browser_send_irkey_msg)
  ON_COMMAND(MSG_BROWSER_IRKEY_EXIT, on_browser_send_irkey_msg)
   ON_COMMAND(MSG_BROWSER_EXIT, on_browser_exit)
  ON_COMMAND(MSG_BROWSER_STOP, on_browser_stop)

END_MSGPROC(browser_root_proc, NULL)

