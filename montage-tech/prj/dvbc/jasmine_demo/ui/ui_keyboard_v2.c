/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/

#include "ui_common.h"

#include "ui_keyboard_v2.h"

enum local_msg
{
  MSG_LABEL_CHANGE = MSG_LOCAL_BEGIN + 1000,
  MSG_DEL_LETTER,
  MSG_LABEL_SAVE,
  MSG_LABEL_CANCEL,
};

enum cont_child_id
{
  IDC_INVALID = 0,
  IDC_EDIT = 1,
  IDC_KEYBOARD_CONT = 2,
  IDC_HELP = 3,
};

enum kb_cont_child_idm
{
  IDC_LABEL_CONT = 1,
  IDC_MBOX = 2,
  IDC_MBOX_BUTTON = 3,
};

enum label_child_id
{
  /* input method */
  IDC_IPM_1 = 1,
  IDC_IPM_2 = 2,
  IDC_IPM_3 = 3,
  IDC_IPM_4 = 4,
  IDC_IPM_5 = 5,
};

enum input_method_id
{
  IDC_LITTLE_ENGLISH = 1,
  IDC_CAPITAL_ENGLISH,
  IDC_LITTLE_LATIN,
  IDC_CAPITAL_LATIN,
  IPM_SYMBOL
};

static u8 input_method = IDC_LITTLE_ENGLISH;
static u16 cur_pos = 0;
static kb_param_t new_name;

#ifdef WIN32
static kb_param_t temp_kb;
static u16 str_name[17];
#endif

static u16 keyboard_input_method_keymap(u16 key);
static RET_CODE keyboard_input_method_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void help_column_creat(control_t *p_parent, u8 id)
{
  control_t *p_temp = NULL, *p_cont = NULL;
  u8 i = 0;
  u16 x1 = 0, x2 = 0;
  u16 content1[6] =
  {IM_EPG_COLORBUTTON_RED, IDS_DELETE, IM_EPG_COLORBUTTON_GREEN, IDS_PAGE_KEYBOARD, IM_OK, IDS_SELECT};
  u16 content2[4] =
  {IM_EPG_COLORBUTTON_YELLOW, IDS_SAVE, IM_EPG_COLORBUTTON_BLUE, IDS_CANCEL};
  u16 w1[6] = {38, 75, 38, 70, 40, 70};
  u16 w2[4] = {38, 75, 38, 80};


  p_cont = ctrl_create_ctrl(CTRL_CONT, id,
                            KEYBOARD_HELP_X, KEYBOARD_HELP_Y,
                            KEYBOARD_HELP_W, KEYBOARD_HELP_H + KEYBOARD_HELP_H,
                            p_parent, 0);

  for(i = 0; i < 6; i++)
  {
    if(i % 2)
    {
      p_temp = ctrl_create_ctrl(CTRL_TEXT, i, x1, 0, w1[i], KEYBOARD_HELP_H, p_cont, 0);
      text_set_font_style(p_temp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
      text_set_align_type(p_temp, STL_LEFT | STL_VCENTER);
      text_set_content_type(p_temp, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_temp, content1[i]);
    }
    else
    {
      p_temp = ctrl_create_ctrl(CTRL_BMAP, i, x1, 0, w1[i], KEYBOARD_HELP_H, p_cont, 0);
      bmap_set_align_type(p_temp, STL_LEFT | STL_VCENTER);
      bmap_set_content_by_id(p_temp, content1[i]);
    }
    x1 += w1[i];
  }
  for(i = 0; i < 4; i++)
  {
    if(i % 2)
    {
      p_temp = ctrl_create_ctrl(CTRL_TEXT, i + 6, x2, 40, w2[i], KEYBOARD_HELP_H, p_cont, 0);
      text_set_font_style(p_temp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
      text_set_align_type(p_temp, STL_LEFT | STL_VCENTER);
      text_set_content_type(p_temp, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_temp, content2[i]);
    }
    else
    {
      p_temp = ctrl_create_ctrl(CTRL_BMAP, i + 6, x2, 40, w2[i], KEYBOARD_HELP_H, p_cont, 0);
      bmap_set_align_type(p_temp, STL_LEFT | STL_VCENTER);
      bmap_set_content_by_id(p_temp, content2[i]);
    }
    x2 += w2[i];
  }
}


static void symbol_iput_update(control_t *p_mbox)
{
  u8 i = 0;
  u16 uni_str[2];
  u16 uni_char[46] =
  {
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
    0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
    0x3f, 0x40, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x7e, 0x7b,
    0x7d, 0xa1, 0xa2, 0xa3, 0xa5, 0xab, 0xbb, 0xb7, 0xbf, 0xf7,
    0x194, 0x195, 0x196, 0x197, 0x198, 0x199
  };

  uni_str[1] = '\0';
  for(i = 0; i < KEYBOARD_MBOX_V2_COUNT - 3; i++)
  {
    uni_str[0] = uni_char[i];
    mbox_set_content_by_unistr(p_mbox, i, uni_str);
  }
}


static void abc_iput_update(control_t *p_mbox)
{
  u8 i = 0;
  u16 uni_str[2];
  u16 uni_char[10] =
  {0x3f, 0x2c, 0x5c, 0x2f, 0x22, 0x27, 0x2e, 0x40};

  uni_str[1] = '\0';

  uni_str[0] = 0x30;
  for(i = 0; i < KEYBOARD_MBOX_V2_COUNT - 3; i++)
  {
    mbox_set_content_by_unistr(p_mbox, i, uni_str);
    if((uni_str[0] >= 0x30
       && uni_str[0] <= 0x3b)
      || (uni_str[0] >= 0x61
         && uni_str[0] < 0x7a))
    {
      uni_str[0]++;
    }
    else
    {
      uni_str[0] = uni_char[i - 37];
    }

    if(uni_str[0] == 0x3c)
    {
      uni_str[0] = 0x61;
    }
  }
}


static void ABC_iput_update(control_t *p_mbox)
{
  u8 i = 0;
  u16 uni_str[2];
  u16 uni_char[10] =
  {0x3f, 0x2c, 0x5c, 0x2f, 0x22, 0x27, 0x2e, 0x40};

  uni_str[1] = '\0';

  uni_str[0] = 0x30;
  for(i = 0; i < KEYBOARD_MBOX_V2_COUNT - 3; i++)
  {
    mbox_set_content_by_unistr(p_mbox, i, uni_str);
    if((uni_str[0] >= 0x30
       && uni_str[0] <= 0x3b)
      || (uni_str[0] >= 0x41
         && uni_str[0] < 0x5a))
    {
      uni_str[0]++;
    }
    else
    {
      uni_str[0] = uni_char[i - 37];
    }

    if(uni_str[0] == 0x3c)
    {
      uni_str[0] = 0x41;
    }
  }
}


static void latin_iput_update(control_t *p_mbox)
{
  u8 i = 0;
  u16 uni_str[2];
  u16 uni_char[14] =
  {
    0x015b, 0x015d, 0x015f, 0x0107, 0x0109, 0x0133, 0x0144,
    0x0148, 0x0153, 0x0177, 0x22, 0x27, 0x2e, 0x40
  };

  uni_str[1] = '\0';

  uni_str[0] = 0xdf;
  for(i = 0; i < KEYBOARD_MBOX_V2_COUNT - 3; i++)
  {
    mbox_set_content_by_unistr(p_mbox, i, uni_str);
    if((uni_str[0] >= 0xdf
       && uni_str[0] <= 0xf6)
      || (uni_str[0] >= 0xf8
         && uni_str[0] < 0xff))
    {
      uni_str[0]++;
    }
    else
    {
      if(i >= 31)
        uni_str[0] = uni_char[i - 31];
    }

    if(uni_str[0] == 0xf7)
    {
      uni_str[0] = 0xf8;
    }
  }
}


static void LATIN_iput_update(control_t *p_mbox)
{
  u8 i = 0;
  u16 uni_str[2];
  u16 uni_char[15] =
  {
    0x0178, 0x015a, 0x015c, 0x015e, 0x0106, 0x0108, 0x0132, 0x0143, 0x0147,
    0x0152, 0x0176, 0x22, 0x27, 0x2e, 0x40
  };

  uni_str[1] = '\0';

  uni_str[0] = 0xdf;
  mbox_set_content_by_unistr(p_mbox, 0, uni_str);
  uni_str[0] = 0xc0;
  for(i = 1; i < KEYBOARD_MBOX_V2_COUNT - 3; i++)
  {
    mbox_set_content_by_unistr(p_mbox, i, uni_str);
    if((uni_str[0] >= 0xc0
       && uni_str[0] <= 0xd6)
      || (uni_str[0] >= 0xd8
         && uni_str[0] < 0xde))
    {
      uni_str[0]++;
    }
    else
    {
      if(i >= 30)
      uni_str[0] = uni_char[i - 30];
    }

    if(uni_str[0] == 0xd7)
    {
      uni_str[0] = 0xd8;
    }
  }
}


static void update_input_method(control_t *p_mbox)
{
  switch(input_method)
  {
    case IDC_LITTLE_ENGLISH:
      abc_iput_update(p_mbox);
      break;

    case IDC_CAPITAL_ENGLISH:
      ABC_iput_update(p_mbox);
      break;

    case IDC_LITTLE_LATIN:
      latin_iput_update(p_mbox);
      break;

    case IDC_CAPITAL_LATIN:
      LATIN_iput_update(p_mbox);
      break;
    case IPM_SYMBOL:
      symbol_iput_update(p_mbox);
      break;

    default:
      OS_PRINTF("input method update fail!!\n");
      break;
  }
}


RET_CODE open_keyboard_v2(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_edit = NULL, *p_kb_cont = NULL;
  control_t *p_label_cont = NULL, *p_mbox = NULL;
  control_t *p_ipm = NULL;
  u8 i = 0;
  u16 x = 0;
  u16 str_ipm[5][4] = {{'a', 'b', 'c', '\0'},
                       {'A', 'B', 'C', '\0'},
                       {0x00e4, 0x00e7, 0x00e9, '\0'},
                       {0x00e4, 0x00c4, 0x00c9, '\0'},
                       {'!', '&', '#', '\0'},};
  u16 bmp_id[3] = {IM_KEYBOARD_ITEM_LEFT, IM_KEYBOARD_ITEM_RIGHT, IM_KEYBOARD_ITEM_BLANK};

  //#ifdef WIN32
  //temp_kb.max_len = 16;
  // temp_kb.uni_str = str_name;

  memcpy(&new_name, (kb_param_t *)para2, sizeof(kb_param_t));
  //#else
  // memcpy((void *)&new_name, (void *)para2, sizeof(kb_param_t));
  // #endif
  p_cont =
    fw_create_mainwin((u8)(ROOT_ID_KEYBOARD_V2),
                      KEYBOARD_V2_MENU_X, KEYBOARD_V2_MENU_Y,
                      KEYBOARD_V2_MENU_W, KEYBOARD_V2_MENU_H,
                      (u8) para1, 0, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  p_edit = ctrl_create_ctrl(CTRL_EBOX, IDC_EDIT, NAME_EDIT_X, NAME_EDIT_Y,
                            NAME_EDIT_W, NAME_EDIT_H, p_cont, 0);
  ctrl_set_rstyle(p_edit, RSI_KEYBOARD_BAR_V2, RSI_KEYBOARD_BAR_V2, RSI_KEYBOARD_BAR_V2);
  ebox_set_font_style(p_edit, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  ebox_set_maxtext(p_edit, new_name.max_len);
  ebox_set_worktype(p_edit, EBOX_WORKTYPE_SHIFT);
  if(new_name.uni_str != NULL)
  {
    ebox_set_content_by_unistr(p_edit, new_name.uni_str);
  }

  p_kb_cont = ctrl_create_ctrl(CTRL_CONT, IDC_KEYBOARD_CONT,
                               KEYBOARD_CONT_X, KEYBOARD_CONT_Y,
                               KEYBOARD_CONT_W, KEYBOARD_CONT_H,
                               p_cont, 0);

  p_label_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LABEL_CONT,
                                  LABEL_CONT_X, LABEL_CONT_Y,
                                  LABEL_CONT_W, LABEL_CONT_H,
                                  p_kb_cont, 0);
  ctrl_set_rstyle(p_label_cont, RSI_KEYBOARD_LABEL_SH,
                  RSI_KEYBOARD_LABEL_SH, RSI_KEYBOARD_LABEL_SH);
  x = 2;
  for(i = 0; i < 5; i++)
  {
    p_ipm = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_IPM_1 + i),
                             x, IPM_COM_Y, IPM_COM_W, IPM_COM_H,
                             p_label_cont, 0);
    x += (IPM_COM_W + 2);
    if(input_method == (i + 1))
    {
      ctrl_set_rstyle(p_ipm, RSI_KEYBOARD_LABEL_HL,
                      RSI_KEYBOARD_LABEL_SH, RSI_KEYBOARD_LABEL_SH);
      text_set_align_type(p_ipm, STL_CENTER | STL_BOTTOM);
      text_set_font_style(p_ipm, FSI_WHITE, FSI_WHITE, FSI_GRAY);
    }
    else
    {
      ctrl_set_rstyle(p_ipm, RSI_KEYBOARD_LABEL_SH,
                      RSI_KEYBOARD_LABEL_HL, RSI_KEYBOARD_LABEL_SH);
      text_set_align_type(p_ipm, STL_CENTER | STL_VCENTER);
      text_set_font_style(p_ipm, FSI_WHITE, FSI_WHITE, FSI_GRAY);
    }
    text_set_content_type(p_ipm, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_ipm, str_ipm[i]);
  }

  /* specific IPM*/
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_MBOX,
                            KEYBOARD_MBOX_V2_X, KEYBOARD_MBOX_V2_Y,
                            KEYBOARD_MBOX_V2_W, KEYBOARD_MBOX_V2_H,
                            p_kb_cont, 0);
  ctrl_set_keymap(p_mbox, keyboard_input_method_keymap);
  ctrl_set_proc(p_mbox, keyboard_input_method_proc);
  ctrl_set_mrect(p_mbox, 0, 0, KEYBOARD_MBOX_V2_W, KEYBOARD_MBOX_V2_H);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, KEYBOARD_MBOX_V2_COUNT, KEYBOARD_MBOX_V2_COL, KEYBOARD_MBOX_V2_ROW);
  mbox_set_item_interval(p_mbox, KEYBOARD_MBOX_V2_HGAP, KEYBOARD_MBOX_V2_VGAP);
  mbox_set_item_rstyle(p_mbox, RSI_KEYBOARD_BUTTON_HL,
                       RSI_KEYBOARD_BUTTON_SH, RSI_KEYBOARD_BUTTON_HL);
  mbox_set_string_fstyle(p_mbox, FSI_BLACK, FSI_BLACK, FSI_BLACK);
  mbox_set_icon_offset(p_mbox, 0, 0);
  mbox_set_icon_align_type(p_mbox, STL_CENTER | STL_VCENTER);
  mbox_set_string_offset(p_mbox, 0, 0);
  mbox_set_string_align_type(p_mbox, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_UNICODE);
  mbox_set_focus(p_mbox, cur_pos);
  for(i = 46; i < KEYBOARD_MBOX_V2_COUNT; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, bmp_id[i - 46], bmp_id[i - 46]);
  }
  update_input_method(p_mbox);

  help_column_creat(p_cont, IDC_HELP);

  ebox_enter_edit(p_edit);
  ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}


static RET_CODE on_label_change_mbox(control_t *p_mbox, u16 msg,
                                     u32 para1, u32 para2)
{
  control_t *p_label_cont = NULL;
  control_t *p_ipm = NULL;

  cur_pos = mbox_get_focus(p_mbox);

  p_label_cont = ctrl_get_child_by_id(p_mbox->p_parent, IDC_LABEL_CONT);
  p_ipm = ctrl_get_child_by_id(p_label_cont, input_method);
  ctrl_set_rstyle(p_ipm, RSI_KEYBOARD_LABEL_SH,
                  RSI_KEYBOARD_LABEL_HL, RSI_KEYBOARD_LABEL_SH);
  text_set_font_style(p_ipm, FSI_GRAY, FSI_WHITE, FSI_GRAY);
  ctrl_paint_ctrl(p_ipm, TRUE);
  if(IPM_SYMBOL == input_method)
  {
    input_method = IDC_LITTLE_ENGLISH;
  }
  else
  {
    input_method++;
  }
  p_ipm = ctrl_get_child_by_id(p_label_cont, input_method);
  ctrl_set_rstyle(p_ipm, RSI_KEYBOARD_LABEL_HL,
                  RSI_KEYBOARD_LABEL_SH, RSI_KEYBOARD_LABEL_SH);
  text_set_font_style(p_ipm, FSI_WHITE, FSI_GRAY, FSI_GRAY);
  ctrl_paint_ctrl(p_ipm, TRUE);
  update_input_method(p_mbox);
  mbox_set_focus(p_mbox, cur_pos);
  ctrl_paint_ctrl(p_mbox, TRUE);

  return SUCCESS;
}

static RET_CODE on_label_save_mbox(control_t *p_mbox, u16 msg,
                                     u32 para1, u32 para2)
{
  control_t *p_edit = NULL;
  u16 *uni_str = NULL;

  p_edit = ctrl_get_child_by_id(p_mbox->p_parent->p_parent, IDC_EDIT);
  uni_str = ebox_get_content(p_edit);
  memcpy(new_name.uni_str, uni_str, new_name.max_len*2);

   if(new_name.cb(new_name.uni_str)!= SUCCESS)
  {
    // save data is failedS, don't exit this menu
    return SUCCESS;
  }


  ebox_exit_edit(p_edit);
  fw_notify_parent(ROOT_ID_KEYBOARD_V2, NOTIFY_T_MSG, FALSE,
                        MSG_SORT_UPDATE, MSG_SELECT, 0);
  ctrl_process_msg(p_mbox->p_parent->p_parent, MSG_EXIT, 0, 0);
  return SUCCESS;
}

static RET_CODE on_label_cancel_mbox(control_t *p_mbox, u16 msg,
                                     u32 para1, u32 para2)
{
  control_t *p_edit = NULL;

  p_edit = ctrl_get_child_by_id(p_mbox->p_parent->p_parent, IDC_EDIT);
  ebox_exit_edit(p_edit);
  ctrl_process_msg(p_mbox->p_parent->p_parent, MSG_EXIT, 0, 0);
  return SUCCESS;
}

static RET_CODE on_change_focus_mbox(control_t *p_mbox, u16 msg,
                                     u32 para1, u32 para2)
{
  cur_pos = mbox_get_focus(p_mbox);

  switch(msg)
  {
    case MSG_FOCUS_RIGHT:
      if(cur_pos == 48)
      {
        cur_pos = 45;
        mbox_set_focus(p_mbox, cur_pos);
        ctrl_paint_ctrl(p_mbox, TRUE);
        return SUCCESS;
      }
      break;

    case MSG_FOCUS_LEFT:
      if(cur_pos == 45)
      {
        cur_pos = 48;
        mbox_set_focus(p_mbox, cur_pos);
        ctrl_paint_ctrl(p_mbox, TRUE);
        return SUCCESS;
      }
      break;

    case MSG_FOCUS_DOWN:
      if(cur_pos == 40
        || cur_pos == 41
        || cur_pos == 42
        || cur_pos == 43
        || cur_pos == 44)
      {
        switch(cur_pos)
        {
          case 40:
            cur_pos = 4;
            break;
          case 41:
            cur_pos = 5;
            break;
          case 42:
            cur_pos = 6;
            break;
          case 43:
            cur_pos = 7;
            break;
          case 44:
            cur_pos = 8;
            break;
          default:
            break;
        }
        mbox_set_focus(p_mbox, cur_pos);
        ctrl_paint_ctrl(p_mbox, TRUE);
        return SUCCESS;
      }
      break;

    case MSG_FOCUS_UP:
      if(cur_pos == 4
        || cur_pos == 5
        || cur_pos == 6
        || cur_pos == 7
        || cur_pos == 8)
      {
        switch(cur_pos)
        {
          case 4:
            cur_pos = 40;
            break;
          case 5:
            cur_pos = 41;
            break;
          case 6:
            cur_pos = 42;
            break;
          case 7:
            cur_pos = 43;
            break;
          case 8:
            cur_pos = 44;
            break;
          default:
            break;
        }
        mbox_set_focus(p_mbox, cur_pos);
        ctrl_paint_ctrl(p_mbox, TRUE);
        return SUCCESS;
      }
      break;
    default:
      break;
  }

  return ERR_NOFEATURE;
}


static RET_CODE on_input_letter(control_t *p_mbox, u16 msg,
                                u32 para1, u32 para2)
{
  control_t *p_edit = NULL;
  control_t *p_kb_cont = NULL;
  u32 *p_str = NULL;
  u16 uni_str[2] = {0};

  p_kb_cont = p_mbox->p_parent;
  p_edit = ctrl_get_child_by_id(p_kb_cont->p_parent, IDC_EDIT);

  cur_pos = mbox_get_focus(p_mbox);

  if(cur_pos < 46
    || cur_pos == 48)              /* enter letter */
  {
    if(cur_pos == 48)
    {
      ebox_input_uchar(p_edit, 0x20);
    }
    else
    {
      p_str = mbox_get_content_str(p_mbox, cur_pos);
      uni_strcpy(uni_str, (u16 *)(*p_str));
      ebox_input_uchar(p_edit, uni_str[0]);
    }
    ctrl_paint_ctrl(p_edit, TRUE);
  }
  #if 0
  if(cur_pos == 49)
  {
    ebox_back_space(p_edit);
    ctrl_paint_ctrl(p_edit, TRUE);
  }
  #endif
  if(cur_pos == 46)
  {
    ctrl_process_msg(p_edit, MSG_FOCUS_LEFT, 0, 0);
  }

  if(cur_pos == 47)
  {
    ctrl_process_msg(p_edit, MSG_FOCUS_RIGHT, 0, 0);
  }

  return SUCCESS;
}


static RET_CODE on_del_letter(control_t *p_mbox, u16 msg,
                              u32 para1, u32 para2)
{
  control_t *p_edit = NULL;
  control_t *p_kb_cont = NULL;


  p_kb_cont = p_mbox->p_parent;
  p_edit = ctrl_get_child_by_id(p_kb_cont->p_parent, IDC_EDIT);

  ebox_back_space(p_edit);
  ctrl_paint_ctrl(p_edit, TRUE);

  return SUCCESS;
}


BEGIN_KEYMAP(keyboard_input_method_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_GREEN, MSG_LABEL_CHANGE)
ON_EVENT(V_KEY_RED, MSG_DEL_LETTER)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_YELLOW, MSG_LABEL_SAVE)
ON_EVENT(V_KEY_BLUE, MSG_LABEL_CANCEL)
END_KEYMAP(keyboard_input_method_keymap, NULL)

BEGIN_MSGPROC(keyboard_input_method_proc, mbox_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_change_focus_mbox)
ON_COMMAND(MSG_FOCUS_DOWN, on_change_focus_mbox)
ON_COMMAND(MSG_FOCUS_LEFT, on_change_focus_mbox)
ON_COMMAND(MSG_FOCUS_RIGHT, on_change_focus_mbox)
ON_COMMAND(MSG_LABEL_CHANGE, on_label_change_mbox)
ON_COMMAND(MSG_SELECT, on_input_letter)
ON_COMMAND(MSG_DEL_LETTER, on_del_letter)
ON_COMMAND(MSG_LABEL_SAVE, on_label_save_mbox)
ON_COMMAND(MSG_LABEL_CANCEL, on_label_cancel_mbox)
END_MSGPROC(keyboard_input_method_proc, mbox_class_proc)
