/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_pic_play_mode_set.h"

enum control_id
{
  IDC_SLIDE_SHOW_TIME = 1,
  IDC_SLIDE_SHOW_REPEAT,
  IDC_SLIDE_SHOW_SAVE,
  IDC_SLIDE_SHOW_NET_STS,
  IDC_SLIDE_SHOW_NET_TIME,
  IDC_SLIDE_SHOW_NET_EFFECT,
};

static pic_showmode_t pic_showmode = {0, 2, TRUE, TRUE};
BOOL g_is_net = FALSE;

static u16 pic_play_mode_set_cont_keymap(u16 key);

static RET_CODE pic_play_mode_set_select_proc(control_t *p_ctrl,
                                      u16 msg,
                                      u32 para1,
                                      u32 para2);

static u16 pic_play_mode_set_select_keymap(u16 key);
RET_CODE pic_play_mode_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);


void pic_play_update_slide_show(BOOL is_net)
{
  sys_status_get_pic_showmode(&pic_showmode, FALSE);
 // g_is_net = FALSE;
  
}
  
BOOL pic_play_get_slide_show_sts(void)
{
  return pic_showmode.slide_show;
}

u8 pic_play_get_slide_interval(void)
{
  u8 offset = 0;
  
  if(g_is_net)
  {
    offset = 3;
  }

  return pic_showmode.slide_time + offset;
}

BOOL pic_play_get_slide_repeat(void)
{
  return pic_showmode.slide_repeat;
}

BOOL pic_play_get_special_effect(void)
{
  return pic_showmode.special_effect;
}

#ifdef ENABLE_NETWORK
#define NET_PIC_MODE_SET_ITEM_CNT (PIC_PLAY_MODE_SET_ITEM_CNT + 1)
static RET_CODE open_pic_play_mode_set_net(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[NET_PIC_MODE_SET_ITEM_CNT];
  u8 i, j;
  u16 stxt[NET_PIC_MODE_SET_ITEM_CNT] =
  {
    IDS_SLIDE_SHOW, IDS_SLIDE_SHOW_TIME, IDS_SPECCIAL_EFFECT
  };

  u8 opt_cnt_net[NET_PIC_MODE_SET_ITEM_CNT] = {2, 7, 2};
  u16 opt_data_net[NET_PIC_MODE_SET_ITEM_CNT][7] = {
    {IDS_OFF, IDS_ON},
    { IDS_THREE_SEC, IDS_FOUR_SEC,
      IDS_FIVE_SEC, IDS_SIX_SEC, IDS_SEVEN_SEC, IDS_EIGHT_SEC,
      IDS_NINE_SEC,
    },
    {IDS_OFF, IDS_ON},
  };
  u16 y, item_cnt, item_gap;
  u16 droplist_page[NET_PIC_MODE_SET_ITEM_CNT] = {5, 2, 2};
  
  p_cont = fw_create_mainwin(ROOT_ID_PIC_PLAY_MODE_SET,
                                  PIC_PLAY_MODE_SET_CONT_X, PIC_PLAY_MODE_SET_CONT_Y,
                                  PIC_PLAY_MODE_SET_CONT_W, PIC_PLAY_MODE_SET_CONT_H+60,
                                  para1, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  //init global variable
  y = PIC_PLAY_MODE_SET_ITEM_Y + 10;
  item_cnt = NET_PIC_MODE_SET_ITEM_CNT;
  item_gap = 16;
  pic_play_update_slide_show(g_is_net);

  ctrl_set_rstyle(p_cont,RSI_POPUP_BG,RSI_POPUP_BG,RSI_POPUP_BG);
  ctrl_set_keymap(p_cont, pic_play_mode_set_cont_keymap);
  ctrl_set_proc(p_cont, pic_play_mode_cont_proc);
  for(i = 0; i < item_cnt; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_SLIDE_SHOW_NET_STS + i),
                                      PIC_PLAY_MODE_SET_ITEM_X, y,
                                      PIC_PLAY_MODE_SET_ITEM_LW,
                                      PIC_PLAY_MODE_SET_ITEM_RW);

    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_keymap(p_ctrl[i], pic_play_mode_set_select_keymap);
    ui_comm_ctrl_set_proc(p_ctrl[i], pic_play_mode_set_select_proc);

    ui_comm_select_set_param(p_ctrl[i], TRUE,
                             CBOX_WORKMODE_STATIC, opt_cnt_net[i],
                             CBOX_ITEM_STRTYPE_STRID, NULL);
    for (j = 0; j < opt_cnt_net[i]; j++)
    {
      ui_comm_select_set_content(p_ctrl[i], j, opt_data_net[i][j]);
    }

    ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);

    ctrl_set_related_id(p_ctrl[i],
    0,                                     /* left */
    (u8)((i - 1 + item_cnt) % item_cnt + IDC_SLIDE_SHOW_NET_STS),            /* up */
    0,                                     /* right */
    (u8)((i + 1) % item_cnt + IDC_SLIDE_SHOW_NET_STS)); /* down */
    y += PIC_PLAY_MODE_SET_ITEM_H + item_gap;
  }
  ui_comm_select_set_focus(p_ctrl[0], pic_showmode.slide_show);
  ui_comm_select_set_focus(p_ctrl[1], pic_showmode.slide_time);
  ui_comm_select_set_focus(p_ctrl[2], pic_showmode.special_effect);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}
#endif

RET_CODE open_pic_play_mode_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[PIC_PLAY_MODE_SET_ITEM_CNT];
  u8 i, j;
  u16 stxt[PIC_PLAY_MODE_SET_ITEM_CNT] =
  {
    IDS_SLIDE_SHOW_TIME, IDS_SLIDE_SHOW_REPEAT
  };
  u8 opt_cnt[PIC_PLAY_MODE_SET_ITEM_CNT] = {10, 2};
  u16 opt_data[PIC_PLAY_MODE_SET_ITEM_CNT][10] = {
    { IDS_OFF,
      IDS_ONE_SEC, IDS_TWO_SEC, IDS_THREE_SEC, IDS_FOUR_SEC,
      IDS_FIVE_SEC, IDS_SIX_SEC, IDS_SEVEN_SEC, IDS_EIGHT_SEC,
      IDS_NINE_SEC,
    },
    {IDS_OFF, IDS_ON},
  };
  u16 y;
  u16 droplist_page[PIC_PLAY_MODE_SET_ITEM_CNT] = {3, 2};

#ifdef ENABLE_NETWORK
  if( ROOT_ID_ALBUMS == para1 )
  {
    g_is_net = TRUE;
    return open_pic_play_mode_set_net(para1, para2);
  }
  else
#endif
  {
    g_is_net = FALSE;
  }

  p_cont = fw_create_mainwin(ROOT_ID_PIC_PLAY_MODE_SET,
                                  PIC_PLAY_MODE_SET_CONT_X, PIC_PLAY_MODE_SET_CONT_Y,
                                  PIC_PLAY_MODE_SET_CONT_W, PIC_PLAY_MODE_SET_CONT_H,
                                  ROOT_ID_USB_PICTURE, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_PIC_PLAY_MODE_SET_FRM, RSI_PIC_PLAY_MODE_SET_FRM, RSI_PIC_PLAY_MODE_SET_FRM);
  ctrl_set_keymap(p_cont, pic_play_mode_set_cont_keymap);
  ctrl_set_proc(p_cont, pic_play_mode_cont_proc);

  y = PIC_PLAY_MODE_SET_ITEM_Y;
  for(i = 0; i < PIC_PLAY_MODE_SET_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_SLIDE_SHOW_TIME + i),
                                      PIC_PLAY_MODE_SET_ITEM_X, y,
                                      PIC_PLAY_MODE_SET_ITEM_LW,
                                      PIC_PLAY_MODE_SET_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_keymap(p_ctrl[i], pic_play_mode_set_select_keymap);
    ui_comm_ctrl_set_proc(p_ctrl[i], pic_play_mode_set_select_proc);

    ui_comm_select_set_param(p_ctrl[i], TRUE,
                             CBOX_WORKMODE_STATIC, opt_cnt[i],
                             CBOX_ITEM_STRTYPE_STRID, NULL);
    for (j = 0; j < opt_cnt[i]; j++)
    {
      ui_comm_select_set_content(p_ctrl[i], j, opt_data[i][j]);
    }
    ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);
    
    ctrl_set_related_id(p_ctrl[i],
      0,                                     /* left */
      (u8)((i - 1 + PIC_PLAY_MODE_SET_ITEM_CNT) % PIC_PLAY_MODE_SET_ITEM_CNT + 1),            /* up */
      0,                                     /* right */
      (u8)((i + 1) % PIC_PLAY_MODE_SET_ITEM_CNT + 1)); /* down */
      
    y += PIC_PLAY_MODE_SET_ITEM_H + PIC_PLAY_MODE_SET_ITEM_V_GAP;
  }

  ui_comm_select_set_focus(p_ctrl[0], pic_showmode.slide_time);
  ui_comm_select_set_focus(p_ctrl[1], pic_showmode.slide_repeat);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_pic_play_mode_set_select_change(control_t *p_ctrl,
                                           u16 msg,
                                           u32 para1,
                                           u32 para2)
{
  RET_CODE ret = SUCCESS;
  u8 focus = (u8)(para2);

  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_SLIDE_SHOW_TIME:
      pic_showmode.slide_time = focus;
      break;
    case IDC_SLIDE_SHOW_REPEAT:
      pic_showmode.slide_repeat = focus;
      break;
    case IDC_SLIDE_SHOW_NET_STS:
      pic_showmode.slide_show = focus;
      break;
    case IDC_SLIDE_SHOW_NET_TIME:
      pic_showmode.slide_time = focus;
      break;      
    case IDC_SLIDE_SHOW_NET_EFFECT:
      pic_showmode.special_effect = focus;
      break;

   default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  //sys_status_set_pic_showmode(&pic_showmode, g_is_net);
  sys_status_set_pic_showmode(&pic_showmode);
  sys_status_save();

  return ret;
}

static RET_CODE on_pic_play_mode_set_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_PIC_PLAY_MODE_SET, 0, 0);
  
  return SUCCESS;
}
static RET_CODE on_pic_play_mode_set_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  pic_stop();
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  UI_PRINTF("on_pic_play_mode_set_exit \n");
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(pic_play_mode_set_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
END_KEYMAP(pic_play_mode_set_cont_keymap, ui_comm_root_keymap)

BEGIN_KEYMAP(pic_play_mode_set_select_keymap, ui_comm_select_keymap)
END_KEYMAP(pic_play_mode_set_select_keymap, ui_comm_select_keymap)

BEGIN_MSGPROC(pic_play_mode_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_pic_play_mode_set_select_change)
END_MSGPROC(pic_play_mode_set_select_proc, cbox_class_proc)

BEGIN_MSGPROC(pic_play_mode_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PLUG_OUT, on_pic_play_mode_set_plug_out)  
  ON_COMMAND(MSG_EXIT_ALL, on_pic_play_mode_set_exit)
END_MSGPROC(pic_play_mode_cont_proc, ui_comm_root_proc)


