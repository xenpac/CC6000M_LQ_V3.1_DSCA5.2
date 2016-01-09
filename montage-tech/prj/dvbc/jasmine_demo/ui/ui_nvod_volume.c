/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_nvod_volume.h"
#include "ui_mute.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_BAR,
  IDC_NVOD_VOLUME_TITLE,
  IDC_NVOD_VOLUME_NUM,
};

static audio_set_t g_audio = {0};

u16 nvod_volume_keymap(u16 key);
RET_CODE nvod_volume_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 nvod_volume_bar_keymap(u16 key);
RET_CODE nvod_volume_bar_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);


static void set_nvod_volume(u8 volume)
{
  u8 actual_volume = volume;
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID), actual_volume);
  g_audio.global_volume = volume;

  sys_status_set_audio_set(&g_audio);
  sys_status_save();
}


RET_CODE open_nvod_volume(u32 para1, u32 para2)
{
  control_t *p_cont, *p_bar,  *p_frm;
  u8 volume;

  sys_status_get_audio_set(&g_audio);

  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  /* get volume */
  volume = (u8)g_audio.global_volume ;

  switch (para1)
  {
    case V_KEY_LEFT:
      if (volume > 0)
      {
        volume--;
        /* set volume */
        set_nvod_volume(volume);
      }
      break;
    case V_KEY_RIGHT:
      if (volume < AP_VOLUME_MAX)
      {
        volume++;
        /* set volume */
        set_nvod_volume(volume);
      }
      break;
    default:
      /* do nothing */;
  }

  p_cont = fw_create_mainwin(ROOT_ID_NVOD_VOLUME,
                           NVOD_VOLUME_L, NVOD_VOLUME_T,
                           NVOD_VOLUME_W, NVOD_VOLUME_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_NVOD_VOLUME_CONT,
                  RSI_NVOD_VOLUME_CONT, RSI_NVOD_VOLUME_CONT);
  ctrl_set_proc(p_cont, nvod_volume_proc);
  ctrl_set_keymap(p_cont, nvod_volume_keymap);

  p_frm = ctrl_create_ctrl(CTRL_CONT, 0,
                           NVOD_VOLUME_BAR_CONT_X, NVOD_VOLUME_BAR_CONT_Y,
                           NVOD_VOLUME_BAR_CONT_W, NVOD_VOLUME_BAR_CONT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_frm,
                  RSI_NVOD_VOLUME_BAR_FRAME,
                  RSI_NVOD_VOLUME_BAR_FRAME,
                  RSI_NVOD_VOLUME_BAR_FRAME);

  p_bar = ctrl_create_ctrl(CTRL_PBAR, IDC_BAR,
                           NVOD_VOLUME_BAR_X, NVOD_VOLUME_BAR_Y,
                           NVOD_VOLUME_BAR_W, NVOD_VOLUME_BAR_H,
                           p_frm, 0);
  ctrl_set_rstyle(p_bar, RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_BG);
  ctrl_set_mrect(p_bar, 0, 0, NVOD_VOLUME_BAR_W, NVOD_VOLUME_BAR_H);
  pbar_set_rstyle(p_bar, RSI_PROGRESS_BAR_MID_GREEN, RSI_IGNORE, INVALID_RSTYLE_IDX);
  pbar_set_count(p_bar, 0, AP_VOLUME_MAX, AP_VOLUME_MAX);
  pbar_set_direction(p_bar, 1);
  pbar_set_workmode(p_bar, 1, 0);
  pbar_set_current(p_bar, volume);
  ctrl_set_proc(p_bar, nvod_volume_bar_proc);
  ctrl_set_keymap(p_bar, nvod_volume_bar_keymap);

  ctrl_default_proc(p_bar, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


RET_CODE close_nvod_volume(void)
{
  manage_close_menu(ROOT_ID_NVOD_VOLUME, 0, 0);
  return SUCCESS;
}


static RET_CODE on_exit_nvod_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_nvod_volume();
  return SUCCESS;
}

static RET_CODE on_change_nvod_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 volume = 0;

  volume = (u8)pbar_get_current(p_ctrl);;
  
  if(msg == MSG_INCREASE)
  {
    if(volume < AP_VOLUME_MAX)
    {
      volume++;
    }
  }
  else
  {
    if(volume > 0)
    {
      volume--;
    }
  }
  /* set volume */
  set_nvod_volume(volume);
  pbar_class_proc(p_ctrl, msg, para1, para2);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  
  return SUCCESS;
}
static RET_CODE on_nvod_volume_updown(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_NVOD_VOLUME, NOTIFY_T_KEY, FALSE, key, 0, 0);

  on_exit_nvod_volume(p_ctrl, msg, para1, para2);
  return SUCCESS;
}


BEGIN_KEYMAP(nvod_volume_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(nvod_volume_keymap, NULL)

BEGIN_MSGPROC(nvod_volume_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_nvod_volume)
END_MSGPROC(nvod_volume_proc, cont_class_proc)

BEGIN_KEYMAP(nvod_volume_bar_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(nvod_volume_bar_keymap, NULL)

BEGIN_MSGPROC(nvod_volume_bar_proc, pbar_class_proc)
  ON_COMMAND(MSG_INCREASE, on_change_nvod_volume)
  ON_COMMAND(MSG_DECREASE, on_change_nvod_volume)
  ON_COMMAND(MSG_FOCUS_UP, on_nvod_volume_updown)
  ON_COMMAND(MSG_FOCUS_DOWN, on_nvod_volume_updown)
  ON_COMMAND(MSG_EXIT, on_exit_nvod_volume)
END_MSGPROC(nvod_volume_bar_proc, pbar_class_proc)


