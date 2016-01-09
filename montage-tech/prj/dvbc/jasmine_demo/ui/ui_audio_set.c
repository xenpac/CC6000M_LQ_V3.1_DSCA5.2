/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/

#include "ui_common.h"

#include "ui_mute.h"
#include "ui_audio_set.h"
#include "ui_pvr_play_bar.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_CHANNEL,
  IDC_TRACK,
  IDC_TITLE,
};

typedef struct
{
  dvbs_prog_node_t program;
  pvr_audio_info_t record;
  ui_audio_set_type_e type;
}ui_audio_set_t;

static ui_audio_set_t audio_set;
static audio_set_t g_audio;

RET_CODE audio_set_comm_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 audio_set_cont_keymap(u16 key);
RET_CODE audio_set_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


static ui_audio_set_t *_ui_audio_set_v_get_private_data()
{
  return &audio_set;
}

//lint -e438 -e550 -e830 -e732
static RET_CODE _ui_audio_set_v_fill_track_info(control_t *ctrl,
                                                u16 focus,
                                                u16 *p_str,
                                                u16 max_length)
{
  u8 asc_buf[20] = {0};
  const u8 *p_language = NULL;
  u16 curn_aid, audio_type, len = 0;
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  if(private_data->type == AUDIO_SET_PLAY_RECORD)
  {
    p_language = (u8 *)iso_639_2_idx_to_desc(private_data->record.audio[focus].language_index);
    curn_aid = private_data->record.audio[focus].p_id;
    audio_type = private_data->record.audio[focus].type;
  }
  else
  {
    p_language = (u8 *)iso_639_2_idx_to_desc(private_data->program.audio[focus].language_index);
    curn_aid = private_data->program.audio[focus].p_id;
    audio_type = private_data->program.audio[focus].type;
  }

  if(memcmp("udf", p_language, 3) == 0)
  {
    switch(audio_type)
    {
      case AUDIO_AC3_VSB:
        sprintf((char *)asc_buf, "snd%d [%s%.4d]", focus + 1, "AC3 ", curn_aid);
        break;
      case AUDIO_EAC3:
        sprintf((char *)asc_buf, "snd%d [%s%.4d]", focus + 1, "AC3+ ", curn_aid);
        break;
      default:
        sprintf((char *)asc_buf, "snd%d [%.4d]", focus + 1, curn_aid);
        break;
    }
    gui_get_string(IDS_AUDIO_TRACK_X, p_str, max_length);
    len = (u16)uni_strlen(p_str);
    MT_ASSERT(len < max_length - 2);
  }
  else
  {
    memcpy(asc_buf, p_language, LANGUAGE_LEN);
    switch(audio_type)
    {
      case AUDIO_AC3_VSB:
        sprintf((char *)&asc_buf[strlen((char *)asc_buf)], " [%s%.4d]", "AC3 ", curn_aid);
        break;
      case AUDIO_EAC3:
        sprintf((char *)&asc_buf[strlen((char *)asc_buf)], " [%s%.4d]", "AC3+ ", curn_aid);
        break;
      default:
        sprintf((char *)&asc_buf[strlen((char *)asc_buf)], " [%.4d]", curn_aid);
        break;
    }
  }
  str_asc2uni(asc_buf, p_str);

  return SUCCESS;
}
//lint +e438 +e550 +e830

static RET_CODE _ui_audio_set_v_program_set_channel(u8 channel)
{
  cmd_t cmd;
  u16 pg_id;
  dvbs_prog_node_t pg_node = {0};
  av_set_t av_set = {0};

  pg_id = sys_status_get_curn_group_curn_prog_id();
  MT_ASSERT(pg_id != INVALIDID);
  db_dvbs_get_pg_by_id(pg_id, &pg_node);
  sys_status_get_av_set(&av_set);

  /* 1 means BS out */
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == pg_node.audio[channel].type)
       || (AUDIO_EAC3 == pg_node.audio[channel].type)))
  {
    cmd.id = PB_CMD_SWITCH_AUDIO_CHANNEL;
    cmd.data1 = pg_node.audio[channel].p_id;
    cmd.data2 = AUDIO_SPDIF;
    ap_frm_do_command(APP_PLAYBACK, &cmd);
  }
  else
  {
    cmd.id = PB_CMD_SWITCH_AUDIO_CHANNEL;
    cmd.data1 = pg_node.audio[channel].p_id;
    cmd.data2 = pg_node.audio[channel].type;
    ap_frm_do_command(APP_PLAYBACK, &cmd);
  }

  return SUCCESS;
}


static BOOL _ui_audio_set_v_program_is_ac3_bsout(u8 channel)
{
  av_set_t av_set = {0};
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  sys_status_get_av_set(&av_set);

  /* 1 means BS out */
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == private_data->program.audio[channel].type)
       || (AUDIO_EAC3 == private_data->program.audio[channel].type)))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


static BOOL _ui_audio_set_v_program_is_modify(void)
{
  dvbs_prog_node_t pg;
  BOOL is_modify;
  control_t *ctrl;
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  memcpy(&pg, &(private_data->program), sizeof(dvbs_prog_node_t));

  ctrl = ui_comm_root_get_ctrl(ROOT_ID_AUDIO_SET, IDC_CHANNEL);
  pg.audio_channel = ui_comm_select_get_focus(ctrl);
  ctrl = ui_comm_root_get_ctrl(ROOT_ID_AUDIO_SET, IDC_TRACK);
  pg.audio_track = ui_comm_select_get_focus(ctrl);

  if(g_audio.is_global_track)
  {
    g_audio.global_track = (u8)ui_comm_select_get_focus(ctrl);
  }
  
  is_modify = memcmp(&pg, &(private_data->program), sizeof(dvbs_prog_node_t));
  if(is_modify)
  {
    if(pg.audio_channel != private_data->program.audio_channel)
    {
      pg.is_audio_channel_modify = TRUE;
    }
    else
    {
      pg.is_audio_channel_modify = FALSE;
    }

    memcpy(&(private_data->program), &pg, sizeof(dvbs_prog_node_t));
  }

  return is_modify;
}


static BOOL _ui_audio_set_v_program_save_data(void)
{
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  if((private_data->type == AUDIO_SET_PLAY_PROGRAM)
    && _ui_audio_set_v_program_is_modify())
  {
    db_dvbs_edit_program(&(private_data->program));
    db_dvbs_save_pg_edit(&(private_data->program));

    sys_status_set_audio_set(&g_audio);
    return TRUE;
  }

  return FALSE;
}


static BOOL _ui_audio_set_v_record_exit(void)
{
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  if(private_data->type == AUDIO_SET_PLAY_RECORD)
  {
    return TRUE;
  }

  return FALSE;
}


static RET_CODE _ui_audio_set_v_record_set_channel(u8 channel)
{
  u8 type = 0;
  u16 pid;
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  pid = private_data->record.audio[channel].p_id;
  type = (u8)private_data->record.audio[channel].type;
  ts_player_change_audio(pid, type);

  return SUCCESS;
}


static BOOL _ui_audio_set_v_record_is_same(pvr_audio_info_t *p_rec_info)
{
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  /* last time is play program, but this time play record or null*/
  if((private_data->type == AUDIO_SET_PLAY_PROGRAM)
    || (private_data->type == AUDIO_SET_PLAY_NULL))
  {
    return FALSE;
  }

  /* check the record name */
  if(memcmp(private_data->record.program_name, p_rec_info->program_name, sizeof(u16) *
            PROGRAM_NAME_MAX) != 0)
  {
    return FALSE;
  }

  /* check the total size and total time */
  if((private_data->record.total_size != p_rec_info->total_size)
    || (private_data->record.total_time != p_rec_info->total_time))
  {
    return FALSE;
  }

  return TRUE;
}


static RET_CODE _ui_audio_set_v_key_left_right(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u8 channel_change = 0;
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();
  control_t *ctrl_channel = NULL;

  if(cbox_static_get_count(p_ctrl) < 2)
  {
    return SUCCESS;
  }
  /* before switch */
  if(ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  ret = cbox_class_proc(p_ctrl, msg, para1, para2);
  if(!ui_is_playing()
    && (private_data->type == AUDIO_SET_PLAY_PROGRAM))
  {
    /* just change value */
    return SUCCESS;
  }

  /* after switch */
  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_CHANNEL:
      channel_change = (u8)cbox_static_get_focus(p_ctrl);
      if(private_data->type == AUDIO_SET_PLAY_RECORD)
      {
        _ui_audio_set_v_record_set_channel(channel_change);
        private_data->record.audio_channel = channel_change;
      }
      else
      {
        _ui_audio_set_v_program_set_channel(channel_change);
        ctrl_channel = ui_comm_root_get_ctrl(ROOT_ID_AUDIO_SET, IDC_TRACK);
        if(_ui_audio_set_v_program_is_ac3_bsout(channel_change))
        {
          ctrl_set_attr(ctrl_channel, OBJ_ATTR_INACTIVE);
        }
        else
        {
          ctrl_set_attr(ctrl_channel, OBJ_ATTR_ACTIVE);
        }
        ctrl_paint_ctrl(ctrl_channel, TRUE);
      }
      break;
    case IDC_TRACK:
      avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), (avc_audio_mode_1_t)cbox_static_get_focus(p_ctrl));
      if(private_data->type == AUDIO_SET_PLAY_RECORD)
      {
        private_data->record.audio_track = cbox_static_get_focus(p_ctrl);
      }
      break;
    default:
      /* do nothing */;
      break;
  }

  return ret;
}


static RET_CODE _ui_audio_set_v_key_menu_exit_audio(control_t *p_cont,
                                                    u16 msg,
                                                    u32 para1,
                                                    u32 para2)
{
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  if(private_data->type == AUDIO_SET_PLAY_PROGRAM)
  {
    _ui_audio_set_v_program_save_data();
  }
  else
  {
    _ui_audio_set_v_record_exit();
  }

  return ERR_NOFEATURE;
}


static RET_CODE _ui_audio_set_c_get_data(u32 para1, u32 para2)
{
  u16 prog_id;
  pvr_audio_info_t *p_rec_info = NULL;
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  /* play record tv */
  if(para1 == AUDIO_SET_PLAY_RECORD)
  {
    p_rec_info = (pvr_audio_info_t *)para2;
    if(p_rec_info == NULL)
    {
      return ERR_FAILURE;
    }

    if(!_ui_audio_set_v_record_is_same(p_rec_info))
    {
      memset(&(private_data->record), 0, sizeof(pvr_audio_info_t));
      memcpy(&(private_data->record), p_rec_info, sizeof(pvr_audio_info_t));
    }

    private_data->type = AUDIO_SET_PLAY_RECORD;
  }
  else /* play program */
  {
    if((prog_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
    {
      return ERR_FAILURE;
    }

    if(db_dvbs_get_pg_by_id(prog_id, &(private_data->program)) != DB_DVBS_OK)
    {
      return ERR_FAILURE;
    }
    private_data->type = AUDIO_SET_PLAY_PROGRAM;

    sys_status_get_audio_set(&g_audio);
  }

  return SUCCESS;
}


RET_CODE open_audio_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[AUDIO_SET_ITEM_CNT], *p_txt;
  u8 i, j, opt_cnt[AUDIO_SET_ITEM_CNT] = {0, 4};
  u16 y, stxt[AUDIO_SET_ITEM_CNT] = {IDS_AUDIO_TRACK, IDS_AUDIO_MODE};
  u16 content[AUDIO_SET_ITEM_CNT][4] =
  {
    {0},
    {IDS_AUDIO_MODE_STEREO, IDS_AUDIO_MODE_LEFT, IDS_AUDIO_MODE_RIGHT, IDS_AUDIO_MODE_MONO}
  };
  ui_audio_set_t *private_data = _ui_audio_set_v_get_private_data();

  if(_ui_audio_set_c_get_data(para1, para2) != SUCCESS)
  {
    return ERR_FAILURE;
  }

  /* get actual audio total number */
  if(private_data->type == AUDIO_SET_PLAY_RECORD)
  {
    opt_cnt[0] = (u8)private_data->record.audio_channel_total;
  }
  else
  {
    opt_cnt[0] = (u8)private_data->program.audio_ch_num;
  }
  if(opt_cnt[0] == 0)
  {
    /* force to set TRACK 1:IDS_AUDIO_MODE_STEREO */
    opt_cnt[0] = 1;
  }

  /* cancel the mute */
  if(ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  /* create mainwin */
  if(private_data->type == AUDIO_SET_PLAY_RECORD)
  {
    p_cont = fw_create_mainwin(ROOT_ID_AUDIO_SET_RECORD,
                               AUDIO_SET_CONT_X, AUDIO_SET_CONT_Y,
                               AUDIO_SET_CONT_W, AUDIO_SET_CONT_H,
                               0, 0, OBJ_ATTR_ACTIVE, 0);
  }
  else
  {
    p_cont = fw_create_mainwin(ROOT_ID_AUDIO_SET,
                               AUDIO_SET_CONT_X, AUDIO_SET_CONT_Y,
                               AUDIO_SET_CONT_W, AUDIO_SET_CONT_H,
                               0, 0, OBJ_ATTR_ACTIVE, 0);
  }
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_cont, audio_set_cont_keymap);
  ctrl_set_proc(p_cont, audio_set_cont_proc);

  /* create title */
  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE,
                           AUDIO_SET_TITLE_TXT_X, AUDIO_SET_TITLE_TXT_Y,
                           AUDIO_SET_TITLE_TXT_W, AUDIO_SET_TITLE_TXT_H,
                           p_cont, 0);
  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, IDS_AUDIO_SET);

  /* create options */
  y = AUDIO_SET_ITEM_Y;
  for(i = 0; i < AUDIO_SET_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_CHANNEL + i),
                                      AUDIO_SET_ITEM_X, y, AUDIO_SET_ITEM_LW,
                                      AUDIO_SET_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], audio_set_comm_select_proc);

    switch(i)
    {
      case 0:
        ui_comm_select_set_param(p_ctrl[i],
                                 TRUE,
                                 CBOX_WORKMODE_DYNAMIC,
                                 opt_cnt[i],
                                 0,
                                 _ui_audio_set_v_fill_track_info);
        break;
      case 1:
        ui_comm_select_set_param(p_ctrl[i],
                                 TRUE,
                                 CBOX_WORKMODE_STATIC,
                                 opt_cnt[i],
                                 CBOX_ITEM_STRTYPE_STRID,
                                 NULL);
        for(j = 0; j < opt_cnt[i]; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, content[i][j]);
        }

        if(_ui_audio_set_v_program_is_ac3_bsout((u8)private_data->program.audio_channel))
        {
          ctrl_set_attr(p_ctrl[1], OBJ_ATTR_INACTIVE);
        }
        break;
      default:
        break;
    }

    ctrl_set_related_id(p_ctrl[i],
                        0,                                                            /* left */
                        (u8)((i - 1 + AUDIO_SET_ITEM_CNT) % AUDIO_SET_ITEM_CNT + 1),  /* up */
                        0,                                                            /* right */
                        (u8)((i + 1) % AUDIO_SET_ITEM_CNT + 1));                      /* down */

    y += AUDIO_SET_ITEM_H + AUDIO_SET_ITEM_V_GAP;
  }

  /* fill information */
  if(private_data->type == AUDIO_SET_PLAY_RECORD)
  {
    ui_comm_select_set_focus(p_ctrl[0], (u16)private_data->record.audio_channel);
    ui_comm_select_set_focus(p_ctrl[1], (u16)private_data->record.audio_track);
  }
  else
  {
    ui_comm_select_set_focus(p_ctrl[0], private_data->program.audio_channel);
    if(g_audio.is_global_track)
    {
      ui_comm_select_set_focus(p_ctrl[1], g_audio.global_track);
    }
    else
    {
      ui_comm_select_set_focus(p_ctrl[1], private_data->program.audio_track);
    }
  }

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}
//lint +e732

BEGIN_KEYMAP(audio_set_cont_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
ON_EVENT(V_KEY_BACK, MSG_EXIT)
ON_EVENT(V_KEY_AUDIO, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(audio_set_cont_keymap, NULL)

BEGIN_MSGPROC(audio_set_cont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_EXIT, _ui_audio_set_v_key_menu_exit_audio)
END_MSGPROC(audio_set_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(audio_set_comm_select_proc, cbox_class_proc)
ON_COMMAND(MSG_INCREASE, _ui_audio_set_v_key_left_right)
ON_COMMAND(MSG_DECREASE, _ui_audio_set_v_key_left_right)
END_MSGPROC(audio_set_comm_select_proc, cbox_class_proc)
