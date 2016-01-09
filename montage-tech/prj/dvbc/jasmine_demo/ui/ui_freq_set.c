/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_freq_set.h"


enum freq_set_local_msg
{
  MSG_STOP_SCAN = MSG_LOCAL_BEGIN + 150,
  MSG_CLOSE_ALL,
  MSG_START_SCAN,
};

enum control_id
{
  IDC_SPECIFY_TP_FRM = 1,
  IDC_SIGNAL_INFO_FRM,
};

enum specify_tp_frm_id
{
  IDC_SPECIFY_TP_FREQ = 1,
  IDC_SPECIFY_TP_SYM,
  IDC_SPECIFY_TP_DEMOD,
  IDC_SPECIFY_START_SEARCH,
};

static dvbc_lock_t main_tp = {0};

#if 0
static comm_help_data_t2 freq_set_help_data[] = //help bar data
{
  {
    2, 150, {33,400,},
    {
      HELP_RSC_BMP   | IM_ARROW_LEFTRIGHT,
      HELP_RSC_STRID   | IDS_INSTALLATION_HELP_AUTO_SCAN_FRE,
    },
  },
  
  {
    2, 150, {33,400,},
    {
      HELP_RSC_BMP   | IM_ARROW_LEFTRIGHT,
      HELP_RSC_STRID   | IDS_INSTALLATION_HELP_AUTO_SCAN_SYM,
    },
  },
  
  {
    4, 150, {33,110,33,200,},
    {
      HELP_RSC_BMP   | IM_ARROW_UPDOWN,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_UP_DOWN,
      HELP_RSC_BMP   | IM_ARROW_LEFTRIGHT,
      HELP_RSC_STRID | IDS_INSTALLATION_HELP_AUTO_SCAN_CONST,
    },
  },
  
  {
    1, 280, {200,},
    {
      HELP_RSC_STRID | IDS_HELP_OK,
    },
  }

};
#endif
/*
static comm_help_data_t freq_set_help_data = 
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
u16 freq_set_cont_keymap(u16 key);

RET_CODE freq_set_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

u16 freq_set_ok_keymap(u16 key);

RET_CODE freq_set_ok_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 freq_set_tp_frm_keymap(u16 key);
RET_CODE freq_set_tp_frm_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


RET_CODE open_freq_set(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_ctrl_item[FREQ_SET_SPECIFY_TP_ITEM_CNT], *p_specify_tp_frm;
  u16 i,y;
  u16 stxt[FREQ_SET_SPECIFY_TP_ITEM_CNT] =
  { IDS_FREQUENCY, IDS_SYMBOL, IDS_CONSTELLATION};
  u32 total_item_cnt=0;
  memset(p_ctrl_item, 0, sizeof(control_t *) * FREQ_SET_SPECIFY_TP_ITEM_CNT);
  sys_status_get_main_tp1(&main_tp);

  p_cont = ui_comm_root_create(ROOT_ID_FREQ_SET,  0,
    COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, IM_TITLEICON_TV, IDS_FREQUENCY_SETTING);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, freq_set_cont_keymap);
  ctrl_set_proc(p_cont, freq_set_cont_proc);

  //specify tp frm
  p_specify_tp_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_SPECIFY_TP_FRM,
                            FREQ_SET_SPECIFY_TP_FRM_X, FREQ_SET_SPECIFY_TP_FRM_Y,
                            FREQ_SET_SPECIFY_TP_FRM_W, FREQ_SET_SPECIFY_TP_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_specify_tp_frm, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_specify_tp_frm, freq_set_tp_frm_keymap);
  ctrl_set_proc(p_specify_tp_frm, freq_set_tp_frm_proc);

  //create menu item
  y = FREQ_SET_SPECIFY_TP_ITEM_Y;


  if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
  {
    main_tp.tp_freq = main_tp.tp_freq;
    total_item_cnt = 2;
  }
  else
    total_item_cnt = FREQ_SET_SPECIFY_TP_ITEM_CNT;

  for (i = 0; i < total_item_cnt; i++)
  {
    if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
    {
      switch (i)
      {
        case 0:
          p_ctrl_item[i] = ui_comm_numedit_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ + i),
                                               FREQ_SET_SPECIFY_TP_ITEM_X, y,
                                               FREQ_SET_SPECIFY_TP_ITEM_LW,
                                               FREQ_SET_SPECIFY_TP_ITEM_RW);
          ui_comm_numedit_set_static_txt(p_ctrl_item[i], stxt[i]);
          ui_comm_numedit_set_param(p_ctrl_item[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
          SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, SEARCH_FREQ_BIT, SEARCH_FREQ_BIT);
          if(main_tp.tp_freq < SEARCH_FREQ_MIN)
            main_tp.tp_freq = SEARCH_FREQ_MIN;
          if(main_tp.tp_freq > SEARCH_FREQ_MAX)
            main_tp.tp_freq = SEARCH_FREQ_MAX;
          ui_comm_numedit_set_num(p_ctrl_item[0], main_tp.tp_freq);
          break;
        case 1:
          p_ctrl_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_SPECIFY_TP_FREQ + i),
                             FREQ_SET_SPECIFY_TP_ITEM_X, y,
                             FREQ_SET_SPECIFY_TP_ITEM_LW + FREQ_SET_SPECIFY_TP_ITEM_RW, FREQ_SET_SPECIFY_TP_ITEM_H,
                             p_specify_tp_frm, 0);
          ctrl_set_keymap(p_ctrl_item[i], freq_set_ok_keymap);
          ctrl_set_proc(p_ctrl_item[i], freq_set_ok_proc);
          ctrl_set_rstyle(p_ctrl_item[i], RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_GRAY);
          text_set_content_type(p_ctrl_item[i], TEXT_STRTYPE_STRID);
          text_set_font_style(p_ctrl_item[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
          text_set_align_type(p_ctrl_item[i], STL_LEFT| STL_VCENTER);
          text_set_offset(p_ctrl_item[i], COMM_CTRL_OX, 0);
          text_set_content_by_strid(p_ctrl_item[i], IDS_OK);
          break;
        default:
          //MT_ASSERT(0);
          break;
      }
    }
    else
    {
      switch (i)
      {
        case 0:
        case 1:
          p_ctrl_item[i] = ui_comm_numedit_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ + i),
                                               FREQ_SET_SPECIFY_TP_ITEM_X, y,
                                               FREQ_SET_SPECIFY_TP_ITEM_LW,
                                               FREQ_SET_SPECIFY_TP_ITEM_RW);
          ui_comm_numedit_set_static_txt(p_ctrl_item[i], stxt[i]);
          ui_comm_numedit_set_postfix(p_ctrl_item[i], (i==1) ? IDS_UNIT_SYMB : IDS_UNIT_FREQ);

          if(i==0)
          {
            ui_comm_numedit_set_param(p_ctrl_item[0], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 45000, 862000, 6, 0);
            ui_comm_numedit_set_decimal_places(p_ctrl_item[i], 3);
            ui_comm_numedit_set_num(p_ctrl_item[0], main_tp.tp_freq);
          }
          else
          {
            ui_comm_numedit_set_param(p_ctrl_item[1], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, 0, 9999, 4, 0);
            ui_comm_numedit_set_decimal_places(p_ctrl_item[i], 3);
            ui_comm_numedit_set_num(p_ctrl_item[1], main_tp.tp_sym);
          }
          break;
          
        case 2:
          p_ctrl_item[i] = ui_comm_select_create(p_specify_tp_frm, (u8)(IDC_SPECIFY_TP_FREQ + i),
                                              FREQ_SET_SPECIFY_TP_ITEM_X, y,
                                              FREQ_SET_SPECIFY_TP_ITEM_LW,
                                              FREQ_SET_SPECIFY_TP_ITEM_RW);
          ui_comm_select_set_static_txt(p_ctrl_item[i], stxt[i]);
          if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
          {
            ui_comm_select_set_param(p_ctrl_item[i], TRUE,
                                       CBOX_WORKMODE_STATIC, 2,
                                       CBOX_ITEM_STRTYPE_STRID,
                                       NULL);
            ui_comm_select_set_content(p_ctrl_item[i], 0, IDS_QAM64);
            ui_comm_select_set_content(p_ctrl_item[i], 1, IDS_QAM256);
            if(main_tp.nim_modulate == 8)
              ui_comm_select_set_focus(p_ctrl_item[i], 1);
            else
              ui_comm_select_set_focus(p_ctrl_item[i], 0);
          }
          else
          {
            ui_comm_select_set_param(p_ctrl_item[i], TRUE,
                                       CBOX_WORKMODE_STATIC, 9,
                                       CBOX_ITEM_STRTYPE_STRID,
                                       NULL);
            ui_comm_select_set_content(p_ctrl_item[i], 0, IDS_AUTO);
            ui_comm_select_set_content(p_ctrl_item[i], 1, IDS_BPSK);
            ui_comm_select_set_content(p_ctrl_item[i], 2, IDS_QPSK);
            ui_comm_select_set_content(p_ctrl_item[i], 3, IDS_8PSK);
            ui_comm_select_set_content(p_ctrl_item[i], 4, IDS_QAM16);
            ui_comm_select_set_content(p_ctrl_item[i], 5, IDS_QAM32);
            ui_comm_select_set_content(p_ctrl_item[i], 6, IDS_QAM64);
            ui_comm_select_set_content(p_ctrl_item[i], 7, IDS_QAM128);
            ui_comm_select_set_content(p_ctrl_item[i], 8, IDS_QAM256);
            ui_comm_select_set_focus(p_ctrl_item[i], main_tp.nim_modulate);
          }
          break;

        case 3:
          p_ctrl_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_SPECIFY_TP_FREQ + i),
                             FREQ_SET_SPECIFY_TP_ITEM_X, y,
                             FREQ_SET_SPECIFY_TP_ITEM_LW + FREQ_SET_SPECIFY_TP_ITEM_RW, FREQ_SET_SPECIFY_TP_ITEM_H,
                             p_specify_tp_frm, 0);
          ctrl_set_keymap(p_ctrl_item[i], freq_set_ok_keymap);
          ctrl_set_proc(p_ctrl_item[i], freq_set_ok_proc);
          ctrl_set_rstyle(p_ctrl_item[i], RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_GRAY);
          text_set_content_type(p_ctrl_item[i], TEXT_STRTYPE_STRID);
          text_set_font_style(p_ctrl_item[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
          text_set_align_type(p_ctrl_item[i], STL_LEFT| STL_VCENTER);
          text_set_offset(p_ctrl_item[i], COMM_CTRL_OX, 0);
          text_set_content_by_strid(p_ctrl_item[i], IDS_OK);
          break;
          
        default:
          MT_ASSERT(0);
          break;
      }
    }
    y += (FREQ_SET_SPECIFY_TP_ITEM_H + FREQ_SET_SPECIFY_TP_ITEM_VGAP);
    
    ctrl_set_related_id(p_ctrl_item[i],
                      0,                                     /* left */
                      (u8)((i - 1 +
                              FREQ_SET_SPECIFY_TP_ITEM_CNT) %
                             FREQ_SET_SPECIFY_TP_ITEM_CNT + IDC_SPECIFY_TP_FREQ),           /* up */
                      0,                                     /* right */
                      (u8)((i + 1) % FREQ_SET_SPECIFY_TP_ITEM_CNT + IDC_SPECIFY_TP_FREQ));/* down */
  }

  //ui_comm_help_create2(&freq_set_help_data[0], p_cont, FALSE);
 // ui_comm_help_create(&freq_set_help_data, p_cont);

  ctrl_default_proc(p_ctrl_item[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  return SUCCESS;
}

static RET_CODE on_freq_set_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  dvbc_lock_t tp1 = {0};

  p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SPECIFY_TP_FREQ);
  tp1.tp_freq = ui_comm_numedit_get_num(p_ctrl);
  if(g_customer.customer_id != CUSTOMER_HUANGSHI && g_customer.customer_id != CUSTOMER_JIZHONG_DTMB)
  {
    p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SPECIFY_TP_SYM);
    tp1.tp_sym = ui_comm_numedit_get_num(p_ctrl);

    p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SPECIFY_TP_DEMOD);
    if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
    {
      if(ui_comm_select_get_focus(p_ctrl) == 0)
        tp1.nim_modulate = 6;
      else
        tp1.nim_modulate = 8;
    }
    else
      tp1.nim_modulate = (u8)ui_comm_select_get_focus(p_ctrl);
  }
  memcpy(&main_tp, &tp1, sizeof(dvbc_lock_t));

  sys_status_set_main_tp1(&main_tp);

  sys_status_save();
  manage_close_menu(ROOT_ID_FREQ_SET, 0, 0);
  
  return ERR_NOFEATURE;
}

static RET_CODE on_fre_set_tp_frm_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
#if 0
  control_t *p_active;
  
  p_active = ctrl_get_active_ctrl(p_ctrl);

  switch(p_active->id)
  {
    case IDC_SPECIFY_TP_FREQ:
      if(msg == MSG_FOCUS_DOWN)
      {
        ui_comm_help_create2(&freq_set_help_data[1], p_ctrl->p_parent, TRUE);
      }
      else if(msg == MSG_FOCUS_UP)
      {
        ui_comm_help_create2(&freq_set_help_data[3], p_ctrl->p_parent, TRUE);
      }
      break;

    case IDC_SPECIFY_TP_SYM:
      if(msg == MSG_FOCUS_DOWN)
      {
        ui_comm_help_create2(&freq_set_help_data[2], p_ctrl->p_parent, TRUE);
      }
      else if(msg == MSG_FOCUS_UP)
      {
        ui_comm_help_create2(&freq_set_help_data[0], p_ctrl->p_parent, TRUE);
      }
      break;

    case IDC_SPECIFY_TP_DEMOD:
      if(msg == MSG_FOCUS_DOWN)
      {
        ui_comm_help_create2(&freq_set_help_data[3], p_ctrl->p_parent, TRUE);
      }
      else if(msg == MSG_FOCUS_UP)
      {
        ui_comm_help_create2(&freq_set_help_data[1], p_ctrl->p_parent, TRUE);
      }
      break;

    case IDC_SPECIFY_START_SEARCH:
      if(msg == MSG_FOCUS_DOWN)
      {
        ui_comm_help_create2(&freq_set_help_data[0], p_ctrl->p_parent, TRUE);
      }
      else if(msg == MSG_FOCUS_UP)
      {
        ui_comm_help_create2(&freq_set_help_data[2], p_ctrl->p_parent, TRUE);
      }
      break;
  }
#endif
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(freq_set_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(freq_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(freq_set_cont_proc, ui_comm_root_proc)
END_MSGPROC(freq_set_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(freq_set_ok_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(freq_set_ok_keymap, NULL)

BEGIN_MSGPROC(freq_set_ok_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_freq_set_ok)
END_MSGPROC(freq_set_ok_proc, text_class_proc)

BEGIN_KEYMAP(freq_set_tp_frm_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_START_SCAN)
END_KEYMAP(freq_set_tp_frm_keymap, NULL)

BEGIN_MSGPROC(freq_set_tp_frm_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_fre_set_tp_frm_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_fre_set_tp_frm_focus_change) 
END_MSGPROC(freq_set_tp_frm_proc, cont_class_proc)


