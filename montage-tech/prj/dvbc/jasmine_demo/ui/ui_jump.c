/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_jump.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_JUMP,
};
static u16 g_jump_para = 60;
u16 g_ts_jump_select = 3;
u16 g_opt_data[JUMP_ITEM_CNT][6] = { { IDS_5SEC, IDS_10SEC, IDS_30SEC, IDS_1MIN, IDS_5MIN, IDS_10MIN},};
u16 jump_cont_keymap(u16 key);

RET_CODE jump_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1,  u32 para2);
RET_CODE jump_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

void parse_jump_para(u16 para)
{
  switch (g_opt_data[0][para])
  {
    case IDS_5SEC:
      g_jump_para = 5;
      g_ts_jump_select = 0;
    break;
    case IDS_10SEC:
      g_jump_para = 10;
      g_ts_jump_select = 1;
    break;
    case IDS_30SEC:
      g_jump_para = 30;
      g_ts_jump_select = 2;
    break;
    case IDS_1MIN:
      g_jump_para = 60;
      g_ts_jump_select = 3;
    break;
    case IDS_5MIN:
      g_jump_para = 300;
      g_ts_jump_select = 4;
    break;
    case IDS_10MIN:
      g_jump_para = 600;
      g_ts_jump_select = 5;
    break;
    default:
      MT_ASSERT(0);
    break;
  }
}

static RET_CODE on_jump_change_select(control_t *p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  u16 focus;
  RET_CODE ret = SUCCESS;

  ret = cbox_class_proc(p_ctrl, msg, para1, para2);
  focus = cbox_static_get_focus(p_ctrl);

  switch (ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_JUMP:
      break;
    default:
      ;
  }
  parse_jump_para(focus);
  return ret;
}

RET_CODE open_jump(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[JUMP_ITEM_CNT];
  u8 i, j;
  
#if 1//#ifndef SPT_SUPPORT
  u16 stxt[JUMP_ITEM_CNT] = { IDS_JUMP};
  u8 opt_cnt[JUMP_ITEM_CNT] = { 6 };
  u16 y;
#endif

#if 1//#ifndef SPT_SUPPORT
  p_cont =
    ui_comm_root_create(ROOT_ID_JUMP, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, IM_TITLEICON_TV, IDS_JUMP);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, jump_cont_keymap);
  ctrl_set_proc(p_cont, jump_cont_proc);

  y = JUMP_ITEM_Y;
  for (i = 0; i < JUMP_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_JUMP + i),
                                        JUMP_ITEM_X, y,
                                        JUMP_ITEM_LW,
                                        JUMP_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], jump_select_proc);

    ui_comm_select_set_param(p_ctrl[i], TRUE,
                               CBOX_WORKMODE_STATIC, opt_cnt[i],
                               CBOX_ITEM_STRTYPE_STRID, NULL);
    for (j = 0; j < opt_cnt[i]; j++)
    {
      ui_comm_select_set_content(p_ctrl[i], j, g_opt_data[i][j]);
    }


    ctrl_set_related_id(p_ctrl[i],
                        0,                                                           /* left */
                        (u8)((i - 1 +
                              JUMP_ITEM_CNT) %
                             JUMP_ITEM_CNT + 1),                                   /* up */
                        0,                                                           /* right */
                        (u8)((i + 1) % JUMP_ITEM_CNT + 1));                        /* down */

    y += JUMP_ITEM_H + JUMP_ITEM_V_GAP;
  }
#else
#endif

  ui_comm_select_set_focus(p_ctrl[0], g_ts_jump_select);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_jump_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_JUMP, 0, 0);
  
  return SUCCESS;
}

u16 ui_jump_para_get()
{
    return g_jump_para;
}

BEGIN_KEYMAP(jump_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(jump_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(jump_select_proc, cbox_class_proc)
	ON_COMMAND(MSG_CHANGED, on_jump_change_select)
  //ON_COMMAND(MSG_INCREASE, on_jump_change_select)
  //ON_COMMAND(MSG_DECREASE, on_jump_change_select)
END_MSGPROC(jump_select_proc, cbox_class_proc)

BEGIN_MSGPROC(jump_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PLUG_OUT, on_jump_plug_out)  
END_MSGPROC(jump_cont_proc, ui_comm_root_proc)

