/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_info.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "ui_usb_music.h"
#include "ui_volume_usb.h"
#include "ui_text_encode.h"
#include "ui_mute.h"

#define IDC_MUSIC_LOGO  1

enum info_ctrl_id
{
  IDC_MUSIC_FULL_SCREEN_LIST = 1,
  IDC_MUSIC_FULL_SCREEN_PICTURE ,
  IDC_MUSIC_FULL_SCREEN_LRC ,
};

enum mp_local_msg
{
  MSG_REC_FULL_NONE = MSG_LOCAL_BEGIN + 100,
  MSG_VOLUME_UP,
  MSG_VOLUME_DOWN,
  MSG_RED,
  MSG_GREEN,
};


static comm_help_data_t2 help_music_data1 = 
{
  2, 500, {40, 200},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
    HELP_RSC_STRID | IDS_TEXT_ENCODE,
  },
};

static comm_help_data_t2 help_music_data2 = 
{
  2, 500, {40, 200},
  {
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_TEXT_ENCODE,
  },
};

static BOOL g_fullscreen_play = FALSE;

u16 music_fullscreen_keymap(u16 key);


RET_CODE music_fullscreen_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

static void music_exit_fullscreen()
{
  rect_t logo_rect =  {COMM_WIN_SHIFT_X + MUSIC_PREV_X + 8,
                       COMM_WIN_SHIFT_Y + MUSIC_PREV_Y + 8,
                       COMM_WIN_SHIFT_X + MUSIC_PREV_X + MUSIC_PREV_W - 16,
                      COMM_WIN_SHIFT_Y +  MUSIC_PREV_Y + MUSIC_PREV_H - 16};

  
  ui_music_logo_show(TRUE,logo_rect);

}

static RET_CODE on_music_fullscreen_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  rect_t logo_rect = {0};
  ui_music_logo_show(FALSE, logo_rect);
  ui_music_logo_clear(0);

  ui_music_lrc_show(FALSE);
  manage_close_menu(ROOT_ID_USB_MUSIC_FULLSCREEN, 0, 0);
  music_exit_fullscreen();

  if(ui_is_mute())
  {
    open_mute(0, 0);
  }
  g_fullscreen_play = FALSE;

  return SUCCESS;
}


RET_CODE open_usb_music_fullscreen(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_cont_picture, *p_cont_lrc;//, *p_prev_logo;

  p_cont =
    ui_comm_root_create(ROOT_ID_USB_MUSIC_FULLSCREEN, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, IM_TITLEICON_TV, IDS_MUSIC);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  p_cont_picture = ctrl_create_ctrl(CTRL_CONT, IDC_MUSIC_FULL_SCREEN_PICTURE,
  MUSIC_FULL_PICTURE_X, MUSIC_FULL_PICTURE_Y, MUSIC_FULL_PICTURE_W, MUSIC_FULL_PICTURE_H, p_cont, 0);
  ctrl_set_rstyle(p_cont_picture, RSI_MUSIC_PIC_RECT, RSI_MUSIC_PIC_RECT, RSI_MUSIC_PIC_RECT);

  p_cont_lrc = ctrl_create_ctrl(CTRL_CONT, IDC_MUSIC_FULL_SCREEN_LRC,
  MUSIC_FULL_LRC_X, MUSIC_FULL_LRC_Y, MUSIC_FULL_LRC_W, MUSIC_FULL_LRC_H, p_cont, 0);
  ctrl_set_rstyle(p_cont_lrc, RSI_MUSIC_LYRICS_RECT, RSI_MUSIC_LYRICS_RECT, RSI_MUSIC_LYRICS_RECT);

  ctrl_set_keymap(p_cont, music_fullscreen_keymap);
  ctrl_set_proc(p_cont, music_fullscreen_cont_proc);

  switch(CUSTOMER_ID)
  {
    case CUSTOMER_DEFAULT:
      ui_comm_help_create2(&help_music_data1, p_cont, FALSE);
      break;

    default:
      ui_comm_help_create2(&help_music_data2, p_cont, FALSE);
      break;
  }

  //ui_comm_help_move_pos(p_cont, 750, 30, 190, 60, 42);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  g_fullscreen_play = TRUE;

  return SUCCESS;
}


BOOL ui_music_is_fullscreen_play()
{
   return g_fullscreen_play;

}

void ui_music_set_fullscreen_play(BOOL fullscreen_play)
{
    g_fullscreen_play = fullscreen_play;
}

static RET_CODE on_usb_rec_none(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_music_fullscreen_change_volume(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  open_volume_usb(ROOT_ID_USB_MUSIC_FULLSCREEN, para1);
  return SUCCESS;
}

static RET_CODE on_usb_music_full_screen_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_USB_MUSIC_FULLSCREEN, 0, 0);
  #if 0
  music_exit_fullscreen();

  g_fullscreen_play = FALSE;

  fw_notify_parent(ROOT_ID_USB_MUSIC_FULLSCREEN, NOTIFY_T_MSG, FALSE,
          MSG_PLUG_OUT, 0, 0);
  #endif

#ifdef MIN_AV_64M
  //restart epg
  ui_epg_init();
  
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
  return SUCCESS;
}

static RET_CODE on_usb_music_full_screen_open_text_encode(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  BOOL bOpen = FALSE;
    
  switch(CUSTOMER_ID)
  {
    case CUSTOMER_DEFAULT:
      if(msg == MSG_GREEN)
      {
        bOpen = TRUE;
      }
      break;

    default:
      if(msg == MSG_RED)
      {
        bOpen = TRUE;
      }
      break;
  }

  if(bOpen)
  {
    manage_open_menu(ROOT_ID_TEXT_ENCODE, 0, 0);
  }

  return SUCCESS;
}

BEGIN_KEYMAP(music_fullscreen_keymap, NULL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_REC,MSG_REC_FULL_NONE)
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
END_KEYMAP(music_fullscreen_keymap, NULL)

BEGIN_MSGPROC(music_fullscreen_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_VOLUME_DOWN, on_music_fullscreen_change_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_music_fullscreen_change_volume)
  ON_COMMAND(MSG_EXIT, on_music_fullscreen_exit)
  ON_COMMAND(MSG_REC_FULL_NONE,on_usb_rec_none)
  ON_COMMAND(MSG_PLUG_OUT, on_usb_music_full_screen_plug_out)
  ON_COMMAND(MSG_RED, on_usb_music_full_screen_open_text_encode)
  ON_COMMAND(MSG_GREEN, on_usb_music_full_screen_open_text_encode)
END_MSGPROC(music_fullscreen_cont_proc, ui_comm_root_proc)




