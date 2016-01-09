/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
   
#include "ui_mosaic_play.h"

enum mosaic_play_cont_id
{
  IDC_INVALID = 0,
  IDC_MOSAIC_PLAY_INFO_FRM,
};

#if 0
static comm_dlg_data_t mosaic_play_dlg =
{
  ROOT_ID_MOSAIC_PLAY,
  DLG_FOR_SHOW | DLG_STR_MODE_TITLE_STATIC,
  COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  IDS_NO_MOSAIC,
  RSC_INVALID_ID,
  2000,
};
#endif
u16 mosaic_play_cont_keymap(u16 key);
RET_CODE mosaic_play_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 mosaic_play_video_keymap(u16 key);
RET_CODE mosaic_play_video_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE mosaic_play_video_mbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 mosaic_play_text_keymap(u16 key);
RET_CODE mosaic_play_text_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


RET_CODE open_mosaic_play(u32 para1, u32 para2)
{
  control_t *p_mask,*p_cont;

#if 0
  p_cont = ui_comm_root_create(ROOT_ID_MOSAIC_PLAY, 0,
                             RSI_IGNORE,
                             0, 0,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             0, RSI_IGNORE);
#else
  p_mask = fw_create_mainwin(ROOT_ID_MOSAIC_PLAY,
                             0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                             ROOT_ID_MOSAIC, 0, OBJ_ATTR_ACTIVE, 0);

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_ROOT_CONT,
                            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, p_mask, 0);
#endif

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, mosaic_play_cont_keymap);
  ctrl_set_proc(p_cont, mosaic_play_cont_proc);

  ctrl_process_msg(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont,FALSE);
  return SUCCESS;
  
}

static void mosaic_play_do_ok(void)
{
  fw_notify_parent(ROOT_ID_MOSAIC_PLAY, NOTIFY_T_MSG, FALSE,
                      MSG_EXIT_MOSAIC_PROG_PLAY, 0, 0);
  manage_close_menu(ROOT_ID_MOSAIC_PLAY, 0, 0);
}

static void mosaic_play_do_cancel(void)
{
  
}


static RET_CODE on_close_mosaic_play(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_ask_for_dodlg_open(NULL, IDS_EXIT_MOSAIC,
                               mosaic_play_do_ok, mosaic_play_do_cancel, 0);
  return SUCCESS;
}


BEGIN_KEYMAP(mosaic_play_cont_keymap, ui_comm_root_keymap) 
  ON_EVENT(V_KEY_BACK, MSG_CLOSE_MENU)
  ON_EVENT(V_KEY_MENU, MSG_CLOSE_MENU)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(mosaic_play_cont_keymap, ui_comm_root_keymap)
  
BEGIN_MSGPROC(mosaic_play_cont_proc, ui_comm_root_proc)
   ON_COMMAND(MSG_CLOSE_MENU, on_close_mosaic_play)
   ON_COMMAND(MSG_EXIT, on_close_mosaic_play)
END_MSGPROC(mosaic_play_cont_proc, ui_comm_root_proc)


