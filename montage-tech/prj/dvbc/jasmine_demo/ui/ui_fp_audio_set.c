/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/

#include "ui_common.h"

#include "ui_mute.h"
#include "ui_fp_audio_set.h"
#include "ui_video.h"

enum control_id
{
  IDC_FP_INVALID = 0,
  IDC_FP_CHANNEL,
  IDC_FP_TRACK,
  IDC_FP_TITLE,
};

typedef struct
{
    u32 audio_ch_num;
    u32 audio_type;
}fp_audio_info_t;

static fp_audio_info_t fp_prog;


RET_CODE audio_fp_set_comm_select_proc(control_t * p_ctrl, u16 msg, u32 para1, u32 para2);

u16 audio_fp_set_cont_keymap(u16 key);
RET_CODE audio_fp_set_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static RET_CODE fill_track_info(control_t *ctrl, u16 focus, u16 *p_str,
                         u16 max_length)
{

   ui_video_c_get_audio_track_lang_by_idx(focus, p_str, max_length);
   
   return SUCCESS;
}

static RET_CODE on_fp_audio_set_comm_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  int      cnt = 0;
  u32      tmp = 0;
  
  if (cbox_static_get_count(p_ctrl) < 2)
  {
    return SUCCESS;
  }
  /* before switch */
  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  cnt = ui_video_c_get_audio_track_num();

  tmp = fp_prog.audio_ch_num;

  OS_PRINTF("[%s] msg:%d, audio:%d\n", __FUNCTION__, msg, tmp);
  
  if(MSG_INCREASE == msg)
  {
    if((cnt - 1) == (int)tmp)
    {
      tmp = 0;
    }
    else
    {
      tmp++;
    }
  }
  else
  {
    if(0 == tmp)
    {
      tmp = (u32)(cnt - 1);
    }
    else
    {
      tmp--;
    }
  }
  
  /* after switch */
  switch (ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_FP_CHANNEL:
      ret = ui_video_c_change_audio_track((int)tmp);

      OS_PRINTF("[%s] ret:%d, audio:%d\n", __FUNCTION__, ret, tmp);
      
      if(SUCCESS == ret)
      {
        fp_prog.audio_ch_num = tmp;
        ret = cbox_class_proc(p_ctrl, msg, para1, para2);
      }
      else
      { 
        ret = SUCCESS;
      }
      break;

    case IDC_FP_TRACK:
      ret = cbox_class_proc(p_ctrl, msg, para1, para2);
      fp_prog.audio_type = (int)cbox_static_get_focus(p_ctrl);
      avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), (avc_audio_mode_1_t)cbox_static_get_focus(p_ctrl)); 
      break;

    default:
      /* do nothing */;
  }
  OS_PRINTF("change_audio channel%d,type%d\n",fp_prog.audio_ch_num,fp_prog.audio_type);
  return ret;
}

RET_CODE open_fp_audio_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[FP_AUDIO_SET_ITEM_CNT], *p_txt;
  u8 i, j, opt_cnt[FP_AUDIO_SET_ITEM_CNT] = { 0, 4 };
  u16 stxt [FP_AUDIO_SET_ITEM_CNT] =  { IDS_AUDIO_TRACK, IDS_AUDIO_MODE };
  u16 y;
  u16 content [FP_AUDIO_SET_ITEM_CNT][4] = {
                                            { 0 },
                                            { IDS_AUDIO_MODE_STEREO, IDS_AUDIO_MODE_LEFT,
                                              IDS_AUDIO_MODE_RIGHT, IDS_AUDIO_MODE_MONO }
                                          };

  /*!
   * Load audio info from stream,then number of audio track can be got.
   */
  ui_video_c_load_all_audio_track();
  
  opt_cnt[0] = (u8)ui_video_c_get_audio_track_num();
  
  OS_PRINTF("@@@@@@@@@@@---------@@@@@@@@@@@audio_ch_num:%d\n",opt_cnt[0]);
  if (opt_cnt[0] == 0)
  {
    opt_cnt[0] = 1; // force to set TRACK 1
  }
  /* cancel the mute */
  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
  p_cont = fw_create_mainwin(ROOT_ID_FP_AUDIO_SET,
                                  FP_AUDIO_SET_CONT_X, FP_AUDIO_SET_CONT_Y,
                                  FP_AUDIO_SET_CONT_W, FP_AUDIO_SET_CONT_H,
                                  0, 0, OBJ_ATTR_ACTIVE,
                                  0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_cont, audio_fp_set_cont_keymap);
  ctrl_set_proc(p_cont, audio_fp_set_cont_proc);

  //create title
  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_FP_TITLE,
                           FP_AUDIO_SET_TITLE_TXT_X, FP_AUDIO_SET_TITLE_TXT_Y,
                           FP_AUDIO_SET_TITLE_TXT_W, FP_AUDIO_SET_TITLE_TXT_H,
                           p_cont, 0);
  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, IDS_AUDIO_SET);

  y = FP_AUDIO_SET_ITEM_Y;

  for (i = 0; i < FP_AUDIO_SET_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_FP_CHANNEL + i),
                                        FP_AUDIO_SET_ITEM_X, y, FP_AUDIO_SET_ITEM_LW,
                                        FP_AUDIO_SET_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], audio_fp_set_comm_select_proc);

    switch (i)
    {
      case 0:
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,
                                   opt_cnt[i], 0, fill_track_info);
        break;
      default:
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                                   opt_cnt[i], CBOX_ITEM_STRTYPE_STRID, NULL);
        for (j = 0; j < opt_cnt[i]; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, content[i][j]);
        }
    }


    ctrl_set_related_id(p_ctrl[i],
                        0,                                                   /* left */
                        (u8)((i - 1 +
                              FP_AUDIO_SET_ITEM_CNT) % FP_AUDIO_SET_ITEM_CNT + 1), /* up */
                        0,                                                   /* right */
                        (u8)((i + 1) % FP_AUDIO_SET_ITEM_CNT + 1));             /* down */

    y += FP_AUDIO_SET_ITEM_H + FP_AUDIO_SET_ITEM_V_GAP;
  }

  // fill info
  ui_comm_select_set_focus(p_ctrl[0], (u16)fp_prog.audio_ch_num);
  ui_comm_select_set_focus(p_ctrl[1], (u16)fp_prog.audio_type);
  OS_PRINTF("open_audio channel%d,type%d\n",fp_prog.audio_ch_num,fp_prog.audio_type);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(audio_fp_set_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_AUDIO, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
#if 0//#ifdef CONFIG_CUSTOMER
  ON_EVENT(V_KEY_BLUE, MSG_EXIT)
#endif
END_KEYMAP(audio_fp_set_cont_keymap, NULL)

BEGIN_MSGPROC(audio_fp_set_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_EXIT, on_fp_audio_set_exit)
  //ON_COMMAND(MSG_SAVE, on_fp_audio_set_exit)
END_MSGPROC(audio_fp_set_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(audio_fp_set_comm_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_fp_audio_set_comm_select)
  ON_COMMAND(MSG_DECREASE, on_fp_audio_set_comm_select)
END_MSGPROC(audio_fp_set_comm_select_proc, cbox_class_proc)


