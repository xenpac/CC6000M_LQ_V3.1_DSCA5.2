/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_volume.h"
#include "ui_mute.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

#ifdef ENABLE_ADS
#include "ui_ads_display.h"
#include "ui_ad_api.h"
#endif
enum local_msg
{
  MSG_AUDIO = MSG_LOCAL_BEGIN + 900,
  MSG_PARENT,
};
enum control_id
{
  IDC_INVALID = 0,
  IDC_VOL_ICON,
  IDC_VOL_TEXT,
  IDC_BAR,
};

static dvbs_prog_node_t curn_prog;
static audio_set_t audio_set;
BOOL is_no_audio = FALSE;

u16 volume_keymap(u16 key);
RET_CODE volume_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 volume_bar_keymap(u16 key);
RET_CODE volume_bar_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);


void set_volume(u8 volume)
{    
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID), volume);
  curn_prog.volume = volume;
}


RET_CODE open_volume(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_frm;
  control_t *p_bar = NULL;
  control_t *p_vol_icon = NULL, *p_text = NULL;
  u8 volume;
  u16 prog_id;
  
  prog_id = sys_status_get_curn_group_curn_prog_id();
  if (db_dvbs_get_pg_by_id(prog_id, &curn_prog) != DB_DVBS_OK)
  {
    return ERR_FAILURE;
  }

  is_no_audio =(BOOL) para2;

  if(ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  sys_status_get_audio_set(&audio_set);
  if(audio_set.is_global_volume)
  {
    curn_prog.volume = audio_set.global_volume; 
    volume = audio_set.global_volume;
  }
  else
  {
    volume = (u8)curn_prog.volume; 
  }
  
  switch(para1)
  {
    case V_KEY_LEFT:
      if(volume > 0)
      {
        volume--;
        /* set volume */
        set_volume(volume);
      }
      break;
    case V_KEY_RIGHT:
      if(volume < VOLUME_MAX)
      {
        volume++;
        /* set volume */
        set_volume(volume);
      }
      break;
    default:
      break;
      /* do nothing */
  }
  p_cont = fw_create_mainwin(ROOT_ID_VOLUME,
                             VOLUME_L, VOLUME_T,
                             VOLUME_W, VOLUME_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_VOLUME_CONT,
                  RSI_VOLUME_CONT, RSI_VOLUME_CONT);
  ctrl_set_proc(p_cont, volume_proc);
  ctrl_set_keymap(p_cont, volume_keymap);

  p_frm = ctrl_create_ctrl(CTRL_CONT, 0,
                           VOLUME_BAR_CONT_X, VOLUME_BAR_CONT_Y,
                           VOLUME_BAR_CONT_W, VOLUME_BAR_CONT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_frm,
                  RSI_VOLUME_BAR_FRAME,
                  RSI_VOLUME_BAR_FRAME,
                  RSI_VOLUME_BAR_FRAME);


  p_vol_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_VOL_ICON,
                           VOLUME_BAR_ICON_X, VOLUME_BAR_ICON_Y,
                           VOLUME_BAR_ICON_W, VOLUME_BAR_ICON_H,
                           p_frm, 0);
  bmap_set_content_by_id(p_vol_icon, IM_VOLUME_HORN);
  
  p_bar = ctrl_create_ctrl(CTRL_PBAR, IDC_BAR,
                           VOLUME_BAR_X, VOLUME_BAR_Y,
                           VOLUME_BAR_W, VOLUME_BAR_H,
                           p_frm, 0);
  ctrl_set_rstyle(p_bar, RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_BG);
  ctrl_set_mrect(p_bar, 0, 0, VOLUME_BAR_W, VOLUME_BAR_H);
  pbar_set_rstyle(p_bar, RSI_PROGRESS_BAR_MID_GREEN, RSI_IGNORE, INVALID_RSTYLE_IDX);
  pbar_set_count(p_bar, 0, VOLUME_MAX, VOLUME_MAX);
  pbar_set_direction(p_bar, 1);
  pbar_set_workmode(p_bar, 1, 0);
  pbar_set_current(p_bar, volume);
  ctrl_set_proc(p_bar, volume_bar_proc);
  ctrl_set_keymap(p_bar, volume_bar_keymap);

  p_text =  ctrl_create_ctrl(CTRL_TEXT, IDC_VOL_TEXT,
                           VOLUME_TEXT_X, VOLUME_TEXT_Y,
                           VOLUME_TEXT_W, VOLUME_TEXT_H,
                           p_frm, 0);
  ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_text, STL_CENTER| STL_VCENTER);
  text_set_content_type(p_text, TEXT_STRTYPE_DEC);
  text_set_content_by_dec(p_text, pbar_get_current(p_bar));

  ctrl_default_proc(p_bar, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  #ifdef ENABLE_CA
  #ifndef ONLY1_CA_VER
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif
  #endif
  
  #ifdef ENABLE_ADS
  ui_adv_pic_play(ADS_AD_TYPE_VOLBAR, ROOT_ID_VOLUME);
  #endif
  return SUCCESS;
}


RET_CODE close_volume(void)
{
  if(audio_set.is_global_volume)
  {
    audio_set.global_volume = (u8)curn_prog.volume;
    sys_status_set_audio_set(&audio_set);
    sys_status_save();
  }

  db_dvbs_edit_program(&curn_prog);
  db_dvbs_save_pg_edit(&curn_prog);
  manage_close_menu(ROOT_ID_VOLUME, 0, 0);
  
  #ifdef ENABLE_ADS
  pic_adv_stop();
#ifdef TEMP_SUPPORT_DS_AD
  ui_check_fullscr_ad_play();
#endif
#ifdef TEMP_SUPPORT_SZXC_AD
  ui_check_corner_ad_play();
#endif
#endif
  return SUCCESS;
}


static RET_CODE on_exit_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_volume();
  return SUCCESS;
}

static RET_CODE update_vol_value(control_t *p_ctrl, u8 volume)
{
  control_t  *p_text = NULL;

  p_text = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_VOL_TEXT);
  
  text_set_content_by_dec(p_text, (s32)volume);
  ctrl_paint_ctrl(p_text,TRUE);
  return SUCCESS;
}

static RET_CODE on_change_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 volume = 0;

  volume = (u8)curn_prog.volume;
  
  if(msg == MSG_INCREASE)
  {
    if(volume < VOLUME_MAX)
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
  set_volume(volume);
  pbar_class_proc(p_ctrl, msg, para1, para2);
  update_vol_value(p_ctrl,volume);
  ctrl_paint_ctrl(p_ctrl,TRUE);

  #ifdef ENABLE_CA
  #ifndef ONLY1_CA_VER
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif
  #endif
  return SUCCESS;
}

static RET_CODE on_audio(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 vkey = para1;
   if(!is_no_audio)
  {
      manage_open_menu(ROOT_ID_AUDIO_SET, vkey, 0);
  }
  return SUCCESS;
}

static RET_CODE on_exit_volume_soon(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  fw_notify_parent(ROOT_ID_VOLUME, NOTIFY_T_KEY, FALSE, (u16)para1, 0, 0);
  close_volume();
  return SUCCESS;
}

static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}
BEGIN_KEYMAP(volume_keymap, NULL)
//ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
ON_EVENT(V_KEY_BACK, MSG_EXIT)
ON_EVENT(V_KEY_AUDIO,MSG_AUDIO)
ON_EVENT(V_KEY_REC,MSG_PARENT)
ON_EVENT(V_KEY_BLUE, MSG_PARENT)
//ON_EVENT(V_KEY_INFO,MSG_EXIT)
END_KEYMAP(volume_keymap, NULL)

BEGIN_MSGPROC(volume_proc, cont_class_proc)
ON_COMMAND(MSG_EXIT, on_exit_volume)
ON_COMMAND(MSG_AUDIO, on_audio)
ON_COMMAND(MSG_PARENT, on_exit_volume_soon)
ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
END_MSGPROC(volume_proc, cont_class_proc)

BEGIN_KEYMAP(volume_bar_keymap, NULL)
ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
ON_EVENT(V_KEY_VUP, MSG_INCREASE)
END_KEYMAP(volume_bar_keymap, NULL)

BEGIN_MSGPROC(volume_bar_proc, pbar_class_proc)
ON_COMMAND(MSG_INCREASE, on_change_volume)
ON_COMMAND(MSG_DECREASE, on_change_volume)
END_MSGPROC(volume_bar_proc, pbar_class_proc)
