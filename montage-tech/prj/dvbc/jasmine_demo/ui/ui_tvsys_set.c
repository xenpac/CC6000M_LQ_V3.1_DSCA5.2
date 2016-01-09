/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_tvsys_set.h"

enum control_id
{
  IDC_VIDEO_MODE = 1,
  IDC_VIDEO_RESOLUTION,
  IDC_ASPECT_MODE,
  IDC_VIDEO_EFFECTS,
  IDC_IS_VOL_GLOBAL,
  IDC_IS_TRACK_GLOBAL,
  IDC_AUDIO_MODE,
  IDC_DIGITAL_OUTPUT,
  IDC_SCART_OUT,
};

dvbs_prog_node_t pg = {0};
static u16 tvsys_set_cont_keymap(u16 key);

static RET_CODE tvsys_set_select_proc(control_t *p_ctrl,
                                                                                              u16 msg,
                                                                                              u32 para1,
                                                                                              u32 para2);
/*
static comm_help_data_t tvsys_set_help_data = 
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
static RET_CODE tvsys_set_comm_fill_content(control_t *p_ctrl,
                                            u16 focus,
                                            u16 *p_str,
                                            u16 max_length)
{
  static u16 video_resolution_sd_str[] = {IDS_PAL, IDS_NTSC, IDS_AUTO};
  static u16 video_resolution_hd_str_50hz[] = {IDS_576I, IDS_576P, IDS_720P, IDS_1080I, IDS_1080P};
  static u16 video_resolution_hd_str_60hz[] = {IDS_480I, IDS_480P, IDS_720P, IDS_1080I, IDS_1080P};
  static u16 aspect_mode_str[]  = {IDS_AUTO, IDS_43LETTERBOX, IDS_43PANSCAN, IDS_169};
  static u16 audio_output_str[] = {IDS_LPCM_OUT, IDS_BS_OUT};
  static u16 video_effects_str[] = {IDS_SHARPENING, IDS_ADAPTIVE, IDS_SOFT,IDS_STANDARD,IDS_SORT_DEFAULT,IDS_VIDEO_VIVID};
  static u16 audio_global_vol_str[] = {IDS_NO, IDS_YES};
  static u16 audio_global_track_str[] = {IDS_NO, IDS_YES};
  static u16 audio_mode_str[] = {IDS_AUDIO_MODE_STEREO, IDS_AUDIO_MODE_LEFT, IDS_AUDIO_MODE_RIGHT, IDS_AUDIO_MODE_MONO};
  static u16 scart_out[] = {IDS_CVBS, IDS_RGB};

  u16 str_id = RSC_INVALID_ID;
  av_set_t av_set;
  class_handle_t avc_handle = 0;
  disp_sys_t video_std = VID_SYS_AUTO;

  avc_handle = class_get_handle_by_id(AVC_CLASS_ID);

  sys_status_get_av_set(&av_set);

  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_VIDEO_MODE:
      str_id = video_resolution_sd_str[focus];
      break;
     case IDC_SCART_OUT:
        str_id = scart_out[focus];
        break;
    case IDC_VIDEO_RESOLUTION:
  #ifdef WIN32
      avc_video_switch_chann(avc_handle, DISP_CHANNEL_HD);
  #else
      avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);
  #endif
      video_std = avc_get_video_mode_1(avc_handle);

      switch (video_std)
      {
        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
        case VID_SYS_PAL_M:
          str_id = video_resolution_hd_str_60hz[focus];
          break;
        case VID_SYS_PAL:
        case VID_SYS_PAL_N:
        case VID_SYS_PAL_NC:
          str_id = video_resolution_hd_str_50hz[focus];
          break;
        default:
          MT_ASSERT(0);
          break;
      }
      break;
    case IDC_ASPECT_MODE:
      str_id = aspect_mode_str[focus];
      break;
    case IDC_DIGITAL_OUTPUT:
      str_id = audio_output_str[focus];
      break;
    case IDC_VIDEO_EFFECTS:
      str_id = video_effects_str[focus];
      break;
    case IDC_IS_VOL_GLOBAL:
      str_id = audio_global_vol_str[focus];
      break;
    case IDC_IS_TRACK_GLOBAL:
      str_id = audio_global_track_str[focus];
      break;
    case  IDC_AUDIO_MODE:
      str_id = audio_mode_str[focus];
      break;
    default:
      MT_ASSERT(0);
  }
  
  gui_get_string(str_id, p_str, max_length);
  //str_nasc2uni(content, p_str, max_length);
  

  return SUCCESS;
}

//lint -e571
RET_CODE open_tvsys_set(u32 para1, u32 para2)
{
  u8 i;
  av_set_t av_set;
  audio_set_t audio_set;
  BOOL is_scart = FALSE;
  u16 prog_id = 0;
  BOOL pg_valid = FALSE;
  control_t *p_cont, *p_ctrl[TVSYS_SET_ITEM_CNT];

  u16 stxt[TVSYS_SET_ITEM_CNT] =
  {
    IDS_TV_SYSTEM, IDS_VIDEO_RESOLUTION, 
    IDS_ASPECT_MODE, IDS_VIDEO_EFFECTS,
    IDS_GLOBAL_VOLUME, IDS_GLOBAL_TRACK, 
    IDS_AUDIO_MODE,IDS_DIGITAL_AUDIO_OUT,IDS_SCART_OUT
  };
  u8 opt_cnt[TVSYS_SET_ITEM_CNT] = {3, 5, 4, 6, 2, 2, 4, 2, 2};
  u16 y;
  u16 droplist_page[TVSYS_SET_ITEM_CNT] = {4, 6, 5, 4, 3, 3, 4, 3, 2};

  /* set focus according to current info */
  sys_status_get_av_set(&av_set);
  sys_status_get_audio_set(&audio_set);
  sys_status_get_status(BS_IS_SCART_OUT, &is_scart);
  OS_PRINTF("%s(Line: %d): av setting.scart out = %d.\n", __FUNCTION__, __LINE__, is_scart);

  memset(&pg, 0, sizeof(dvbs_prog_node_t));
  if((prog_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
  {
     if(db_dvbs_get_pg_by_id(prog_id, &pg) == DB_DVBS_OK)
    {
      pg_valid = TRUE;
    }
  }
  
  p_cont =
    ui_comm_root_create(ROOT_ID_TVSYS_SET, 0,
    COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, IM_TITLEICON_TV, IDS_AV_SETTING);

  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, tvsys_set_cont_keymap);

  y = TVSYS_SET_ITEM_Y;
  for(i = 0; i < TVSYS_SET_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_VIDEO_MODE + i),
                                      TVSYS_SET_ITEM_X, y,
                                      TVSYS_SET_ITEM_LW,
                                      TVSYS_SET_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], tvsys_set_select_proc);

    ui_comm_select_set_param(p_ctrl[i], TRUE,
                             CBOX_WORKMODE_DYNAMIC, opt_cnt[i],
                             0, tvsys_set_comm_fill_content);
    ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);
    ctrl_set_related_id(p_ctrl[i],
                        0,                                                            /* left */
                        (u8)((i - 1 +
                              TVSYS_SET_ITEM_CNT) %
                             TVSYS_SET_ITEM_CNT + 1),                  /* up */
                        0,                                                            /* right */
                        (u8)((i + 1) % TVSYS_SET_ITEM_CNT + 1)); /* down */

    y += TVSYS_SET_ITEM_H + TVSYS_SET_ITEM_V_GAP;
  }

  ui_comm_select_set_focus(p_ctrl[0], av_set.tv_mode);
  ui_comm_select_set_focus(p_ctrl[1], av_set.tv_resolution);
  ui_comm_select_set_focus(p_ctrl[2], av_set.tv_ratio);
  ui_comm_select_set_focus(p_ctrl[3], av_set.video_effects);
  ui_comm_select_set_focus(p_ctrl[4], audio_set.is_global_volume);
  ui_comm_select_set_focus(p_ctrl[5], audio_set.is_global_track);
  
  if(pg_valid && (!audio_set.is_global_track))
    ui_comm_select_set_focus(p_ctrl[6], (u16)pg.audio_track);
  else
    ui_comm_select_set_focus(p_ctrl[6], audio_set.global_track);
  
  ui_comm_select_set_focus(p_ctrl[7], av_set.digital_audio_output);
  ui_comm_select_set_focus(p_ctrl[8], av_set.video_output);

  if(!is_scart)
  {
    ctrl_set_sts(p_ctrl[IDC_SCART_OUT - 1], OBJ_STS_HIDE);
  }
  if(!pg_valid)
  {
    ctrl_set_attr(p_ctrl[6], OBJ_ATTR_INACTIVE);
  }
  //ui_comm_help_create(&tvsys_set_help_data, p_cont);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}
//lint +e571
#if 0
static void refresh_audio_mode(control_t *p_ctrl, u8 is_global_track)
{
  control_t * p_sub_ctrl = NULL;
  audio_set_t audio_set;
  
  sys_status_get_audio_set(&audio_set);
  
  p_sub_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_AUDIO_MODE);
  if(is_global_track)
  {
    ui_comm_select_set_focus(p_sub_ctrl, audio_set.global_track);
  }
  else
  {
    ui_comm_select_set_focus(p_sub_ctrl, pg.audio_track);
  }
  ctrl_erase_ctrl(p_sub_ctrl);
  return;
}
#endif
static RET_CODE on_tvsys_set_select_change(control_t *p_ctrl,
                                           u16 msg,
                                           u32 para1,
                                           u32 para2)
{
  u8 to;
  av_set_t av_set;
  audio_set_t audio_set;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  RET_CODE ret = SUCCESS;
  control_t *p_video_resolution = NULL;
  void *p_disp = NULL , *p_dev = NULL;
  u16 prog_id = 0;
  BOOL pg_valid = FALSE;

  sys_status_get_av_set(&av_set);
  sys_status_get_audio_set(&audio_set);

  to = (u8)(para2);

  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_VIDEO_MODE:
      if (!ctrl_is_onfocus(p_ctrl)
        || av_set.tv_mode == to)
      {
        return SUCCESS;
      }
      av_set.tv_mode = to;
#if ENABLE_TTX_SUBTITLE
      if (is_enable_vbi_on_setting())
      {
        ui_enable_vbi_insert(FALSE);
      }
#endif
      avc_switch_video_mode_1(avc_handle, sys_status_get_sd_mode(av_set.tv_mode));

      avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);

      avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));
#if ENABLE_TTX_SUBTITLE
      if (is_enable_vbi_on_setting())
      {
        ui_enable_vbi_insert(TRUE);
      }
#endif
      if((av_set.tv_resolution == 0) //PAL->576i NTSC->480i
        ||(av_set.tv_resolution == 1)) //PAL->576p NTSC->480p
      {
        p_video_resolution = ui_comm_root_get_ctrl(ROOT_ID_TVSYS_SET, IDC_VIDEO_RESOLUTION);

        ui_comm_select_set_focus(p_video_resolution, av_set.tv_resolution);
        ctrl_paint_ctrl(p_video_resolution, TRUE);
      }
      break;

    case IDC_VIDEO_RESOLUTION:
      if (!ctrl_is_onfocus(p_ctrl)
        || av_set.tv_resolution == to)
      {
        return SUCCESS;
      }

      av_set.tv_resolution = to;
      avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));
      break;


    case IDC_ASPECT_MODE:
      if (!ctrl_is_onfocus(p_ctrl)
        || av_set.tv_ratio == to)
      {
        return SUCCESS;
      }
      av_set.tv_ratio = to;


      ui_reset_video_aspect_mode(sys_status_get_video_aspect(av_set.tv_ratio));
      avc_cfg_scart_aspect_1(avc_handle,
                           sys_status_get_scart_aspect(av_set.tv_ratio));
      break;
    case IDC_SCART_OUT:
      if(!ctrl_is_onfocus(p_ctrl) || av_set.video_output == to)
        {
          return SUCCESS;
        }

      av_set.video_output = to;
      #ifndef WIN32
      p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SCART);
      MT_ASSERT(NULL != p_dev);
      hal_put_u32((u32 *)0xbff700bc, (SCART_VID_CVBS == av_set.video_output)? 0xff560f08 : 0xff560f0f);
      dev_io_ctrl(p_dev, (u32)SCART_CMD_FORMAT_SET, (u32)av_set.video_output);
      #endif
      break;
      
    case IDC_DIGITAL_OUTPUT:

      if (!ctrl_is_onfocus(p_ctrl)
        || av_set.digital_audio_output == to)
      {
        return SUCCESS;
      }
      av_set.digital_audio_output = to;
      break;

    case IDC_VIDEO_EFFECTS:
      if (!ctrl_is_onfocus(p_ctrl)
        || av_set.video_effects == to)
      {
        return SUCCESS;
      }
      av_set.video_effects = to;

      p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
      MT_ASSERT(p_disp != NULL);
      #ifndef WIN32
			
      disp_set_postprocess_mode(p_disp, (disp_pp_mode_t)to);
      #endif
      break;

    case IDC_IS_VOL_GLOBAL:
      audio_set.is_global_volume = to;
      break;

    case IDC_IS_TRACK_GLOBAL:
      audio_set.is_global_track = to;
      //refresh_audio_mode(ctrl_get_parent(p_ctrl), audio_set.is_global_track);
      break;

    case IDC_AUDIO_MODE:
      if(audio_set.is_global_track)
      {
        audio_set.global_track = to;
      }
      avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), (avc_audio_mode_1_t)to);
      /* fix bug57260 */
      memset(&pg, 0, sizeof(dvbs_prog_node_t));
      if((prog_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
      {
        if(db_dvbs_get_pg_by_id(prog_id, &pg) == DB_DVBS_OK)
        {
          pg_valid = TRUE;
        }
      }
      if( pg_valid && (!(audio_set.is_global_track)) )
      {
        pg.audio_track = to;
        db_dvbs_edit_program(&pg);
        db_dvbs_save_pg_edit(&pg);
      }
      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  sys_status_set_av_set(&av_set);
  sys_status_set_audio_set(&audio_set);
  sys_status_save();

  return ret;
}

BEGIN_KEYMAP(tvsys_set_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(tvsys_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(tvsys_set_select_proc, cbox_class_proc)
ON_COMMAND(MSG_CHANGED, on_tvsys_set_select_change)
END_MSGPROC(tvsys_set_select_proc, cbox_class_proc)
