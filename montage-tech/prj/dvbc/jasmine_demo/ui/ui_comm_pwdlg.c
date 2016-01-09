/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/

#include "ui_common.h"
#include "ui_mute.h"
#include "ui_signal.h"

#ifdef ENABLE_CA
#include "config_cas.h"
#endif
enum comm_pwdlg_idc
{
  IDC_TXT = 1,
  IDC_EDIT,
  IDC_PROMPT
};

struct pwdlg_int_data
{
  u8  type;               // PWDLG_T_
  u8  total_bit;          // total bit
  u8  input_bit;          // input bit
  u32 total_value;        // total value
  u32 input_value;        // input value
  u32 super_value;        // super password
};

static struct pwdlg_int_data g_pwdlg_idata;

#if defined (ENABLE_CA) && defined (CAS_CONFIG_ABV)
#define PWD_LENGTH_CA 4
#else
#define PWD_LENGTH_CA 6
#endif

#ifdef ENABLE_CA
static cas_pin_modify_t pin_veriify;
#endif
u16 pwdlg_edit_keymap(u16 key);

RET_CODE pwdlg_edit_proc(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2);

BOOL ui_comm_pwdlg_open(comm_pwdlg_data_t* p_data)
{
  control_t *p_cont, *p_title, *p_edit, *p_help;

  u16 help_text[64];
  u16 uni_str[32];
  u16 i=0;
  u16 pw_edit_x = 0, pw_edit_w = 0;
  pwd_set_t pwd_set;
    
  sys_status_get_pwd_set(&pwd_set);
  if(fw_find_root_by_id(ROOT_ID_PASSWORD) != NULL) // already opened
  {
    UI_PRINTF("UI: already open a password dialog, force close it! \n");
    ui_comm_pwdlg_close();
  }
  
  // init idata
  g_pwdlg_idata.type = p_data->password_type;
  if(p_data->password_type == PWDLG_T_CA)
  {
    g_pwdlg_idata.total_bit = PWD_LENGTH_CA;
  }
  else if(p_data->password_type == PWDLG_T_IPPV)
  {
    g_pwdlg_idata.total_bit = PWD_LENGTH_IPPV;
  }
  else
  {
    g_pwdlg_idata.total_bit = PWD_LENGTH_COMMON;
  }
  g_pwdlg_idata.input_bit = 0;
  g_pwdlg_idata.total_value = pwd_set.normal;
  g_pwdlg_idata.input_value = 0;
  g_pwdlg_idata.super_value = pwd_set.super;

  // chk the parent
  if (p_data->parent_root == ROOT_ID_INVALID)
  {
    p_data->parent_root = fw_get_focus_id();
  }

  // create window
  p_cont = fw_create_mainwin(ROOT_ID_PASSWORD,
                                  p_data->left, p_data->top,
                                  PWDLG_W, PWDLG_H,
                                  p_data->parent_root, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return FALSE;
  }

  ctrl_set_rstyle(p_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_cont, p_data->keymap);
  ctrl_set_proc(p_cont, p_data->proc);


  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_TXT,
                             PWDLG_TXT1_L, PWDLG_TXT1_T,
                             PWDLG_TXT1_W, PWDLG_TXT1_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_title, FSI_PWDLG_TXT,
                      FSI_PWDLG_TXT, FSI_PWDLG_TXT);
  text_set_align_type(p_title, STL_CENTER| STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, p_data->strid);
  
  pw_edit_x = PWDLG_EDIT_L;
  pw_edit_w = PWDLG_EDIT_W;
  if(g_pwdlg_idata.total_bit == PWD_LENGTH_CA)
  {
    pw_edit_x = PWDLG_EDIT_L - 50;
    pw_edit_w = (u16)(PWDLG_W - 2 * pw_edit_x);
  }
  p_edit = ctrl_create_ctrl(CTRL_MBOX, IDC_EDIT,
                              pw_edit_x, PWDLG_EDIT_T,
                              pw_edit_w, PWDLG_EDIT_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_edit, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  ctrl_set_keymap(p_edit, pwdlg_edit_keymap);
  ctrl_set_proc(p_edit, pwdlg_edit_proc);
  mbox_set_icon_align_type(p_edit, STL_CENTER | STL_VCENTER);
  mbox_enable_string_mode(p_edit, TRUE);  
  mbox_set_string_fstyle(p_edit, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_align_type(p_edit, STL_CENTER | STL_VCENTER);

  // Fix bug 3056
  if(g_pwdlg_idata.type == PWDLG_T_IPPV)
  {
    mbox_set_count(p_edit, PWD_LENGTH_IPPV, PWD_LENGTH_IPPV, 1);
  }
  else if(g_pwdlg_idata.type == PWDLG_T_CA)
  {
    mbox_set_count(p_edit, PWD_LENGTH_CA, PWD_LENGTH_CA, 1);
  }
  else
  {
    mbox_set_count(p_edit, PWD_LENGTH_COMMON, PWD_LENGTH_COMMON, 1);
  }

  mbox_set_item_rstyle(p_edit, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_icon_offset(p_edit, 0, 0);

  mbox_set_icon_align_type(p_edit, STL_CENTER | STL_VCENTER);
  mbox_set_focus(p_edit, 0);
  for(i=0; i <g_pwdlg_idata.total_bit; i++)
  {
    mbox_set_content_by_icon(p_edit, i, IM_PASSWORD_2, IM_PASSWORD_1);
  }

  //ebox_set_hide_mask(p_edit, '-', '*');

  p_help = ctrl_create_ctrl(CTRL_TEXT, IDC_PROMPT,
                            PWDLG_HELP_L, PWDLG_HELP_T,
                            PWDLG_HELP_W, PWDLG_HELP_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_help, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_help, FSI_PWDLG_HELP,
                      FSI_PWDLG_HELP, FSI_PWDLG_HELP);
  text_set_align_type(p_help, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_help, TEXT_STRTYPE_UNICODE);
  str_asc2uni((u8 *)"0~9:", help_text);
  gui_get_string(IDS_INPUT, uni_str, 32);
  uni_strcat(help_text, uni_str, 64);
  #ifdef JHC_DALIAN_ZHUANGGUANG
  {
    u8 ansstr[32];
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char *)ansstr, "      退出键:");
    gb2312_to_unicode(ansstr,sizeof(ansstr),uni_str,32);
  }
  #else
  str_asc2uni((u8 *)"      Menu/Cancel:", uni_str);
  #endif
  uni_strcat(help_text, uni_str, 64);
  gui_get_string(IDS_EXIT, uni_str, 32);
  uni_strcat(help_text, uni_str, 64);
  text_set_content_by_unistr(p_help, help_text);

  /* enter edit status */
  ctrl_default_proc(p_edit, MSG_GETFOCUS, 0, 0);
  ebox_enter_edit(p_edit);

  ctrl_paint_ctrl(p_cont, FALSE);
  return TRUE;

}


void ui_comm_pwdlg_close(void)
{
  u8 index;
  BOOL is_check_mute = FALSE;

  manage_enable_autoswitch(FALSE);
  manage_close_menu(ROOT_ID_PASSWORD, 0, 0);
  manage_enable_autoswitch(TRUE);

  index = fw_get_focus_id();
  if(index == ROOT_ID_PROG_LIST
    || index == ROOT_ID_EPG
    || ui_is_fullscreen_menu(index))
  {
    is_check_mute = TRUE; //fix bug 18847
  }

  if(ui_is_mute() && is_check_mute)
  {
    open_mute(0, 0);
  }
}

static void edit_empty_content(control_t *p_mbox)
{
  u16 i;
  u8 content_len = 0;
  
  if(g_pwdlg_idata.type == PWDLG_T_CA || g_pwdlg_idata.type == PWDLG_T_IPPV)
    content_len = PWD_LENGTH_CA;
  else
    content_len = PWD_LENGTH_COMMON;
  
  for(i=0; i <content_len; i++)
  {
    mbox_set_content_by_ascstr(p_mbox, i, (u8 *)"");
  }
  ctrl_paint_ctrl(p_mbox, TRUE);
}

static RET_CODE on_pwdlg_char(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  control_t *p_parent, *p_help;
  BOOL is_pass = FALSE;
  
  mbox_set_content_by_ascstr(p_edit, mbox_get_focus(p_edit),(u8 *)"*");

  // process MSG_CHAR
  mbox_class_proc(p_edit, MSG_FOCUS_RIGHT, para1, para2);

  g_pwdlg_idata.input_bit++;
  g_pwdlg_idata.input_value = g_pwdlg_idata.input_value * 10 +
                              (msg & MSG_DATA_MASK) - '0';
  if(g_pwdlg_idata.type == PWDLG_T_IPPV)//desai ipp used
  {
    if (g_pwdlg_idata.input_bit == g_pwdlg_idata.total_bit) // finish input 
    {
      #ifdef ENABLE_CA
        memset(&pin_veriify, 0 , sizeof(cas_pin_modify_t));
	    pin_veriify.old_pin[0] = (g_pwdlg_idata.input_value & 0x00ff0000)>>16;
	    pin_veriify.old_pin[1] = (g_pwdlg_idata.input_value & 0x0000ff00)>>8;
	    pin_veriify.old_pin[2] = (g_pwdlg_idata.input_value & 0x000000ff);
      ui_ca_do_cmd((u32)CAS_CMD_PIN_VERIFY, (u32)&pin_veriify ,0);
      #ifdef WIN32
      on_ca_ippv_pin_verify(p_edit, msg, para1, para2);
      #endif
      #endif
    }
  }
  else if(g_pwdlg_idata.type == PWDLG_T_CA)
  {
    p_parent = ctrl_get_parent(p_edit);
    if (g_pwdlg_idata.input_bit == g_pwdlg_idata.total_bit) // finish input 
    {    
      ctrl_process_msg(p_parent, MSG_CORRECT_PWD, g_pwdlg_idata.input_value, 0);
      g_pwdlg_idata.input_bit = 0;
      g_pwdlg_idata.input_value = 0;
    }
  }
  else
  {
    if (g_pwdlg_idata.input_bit == g_pwdlg_idata.total_bit) // finish input ?
    {
          p_parent = ctrl_get_parent(p_edit);
          if(g_pwdlg_idata.type == PWDLG_T_SUPER)
          {
              if(g_pwdlg_idata.input_value == g_pwdlg_idata.super_value)
              {
                  is_pass = TRUE;
              }
          }
          else
          {
              if((g_pwdlg_idata.input_value == g_pwdlg_idata.total_value) 
                  || (g_pwdlg_idata.input_value == g_pwdlg_idata.super_value))
              {
                  is_pass = TRUE;
              }
          }

      if(is_pass)
      {
        ctrl_process_msg(p_parent, MSG_CORRECT_PWD, g_pwdlg_idata.input_value, 0);
      }
      else
      {
        ctrl_process_msg(p_parent, MSG_WRONG_PWD, g_pwdlg_idata.input_value, 0);
        // reset & reinput
          edit_empty_content(p_edit);
        g_pwdlg_idata.input_bit = 0;
        g_pwdlg_idata.input_value = 0;
        // update view
        p_help = ctrl_get_child_by_id(p_parent, IDC_TXT);
        MT_ASSERT(p_help != NULL);
        text_set_content_by_strid(p_help, IDS_MSG_REINPUT_PASSWORD);
        ctrl_paint_ctrl(p_help, TRUE);
      }
    }
  }
  return SUCCESS;
}
#ifdef ENABLE_CA
static RET_CODE on_ca_ippv_pin_verify(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{  
  control_t *p_parent, *p_txt;
  RET_CODE ret;
  u16 pin_spare_num = 0;
  u8 asc_str[64];
  u16 uni_num[64];
  cas_pin_modify_t *ca_pin_info = NULL;
  comm_dlg_data_t dlg_data=
  {
    ROOT_ID_PASSWORD,
    DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
    440, 210,
    420,200,
    IDS_CA_IPPV_BUY,
    60000,
  };
  
  ca_pin_info = (cas_pin_modify_t *)para2;
  p_parent = ctrl_get_parent(p_edit);
  
  if(para1 == 2)
  {
    ctrl_process_msg(p_parent, MSG_CORRECT_PWD, 0, 0);
    return SUCCESS;
  }
  else if(para1 == 18)
  {
    gui_get_string(IDS_CA_PARAME_SET_ERROR,uni_num,64);
  }
  else if(para1 == 1)
  {
    gui_get_string(IDS_CA_IC_CMD_FAIL,uni_num,64);
  }
  else if(para1 == 9)
  {
    gui_get_string(IDS_CA_PIN_NUMB_ERROR_MAX,uni_num,64);
  }
  else if(para1 == 7) 
  {
    pin_spare_num = (u16)ca_pin_info->pin_spare_num;
    if(LANGUAGE_SIMPLIFIED_CHINESE == rsc_get_curn_language(gui_get_rsc_handle()))
    {
      sprintf((char *)asc_str, "PIN码错误，还剩%d次机会", pin_spare_num);
    }
    else
    {
      sprintf((char *)asc_str, "PIN ERROR,%d chances left", pin_spare_num);
    }
    gb2312_to_unicode(asc_str,sizeof(asc_str),uni_num,63);
  }
  
  dlg_data.content = (u32)uni_num;
  ret = ui_comm_dlg_open(&dlg_data);
  if(ret == DLG_RET_YES)
  {
    edit_empty_content(p_edit);
    mbox_set_focus(p_edit, 0);
    g_pwdlg_idata.input_bit = 0;
    g_pwdlg_idata.input_value = 0;
    // update view
    p_txt = ctrl_get_child_by_id(p_parent, IDC_TXT);
    MT_ASSERT(p_txt != NULL);
    text_set_content_by_strid(p_txt, IDS_CA_IPPV_BUY);
    ctrl_paint_ctrl(p_parent, TRUE);
  }
  return SUCCESS;

}
#endif
static RET_CODE on_pwdlg_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_get_proc(fw_get_focus())(fw_get_focus(), MSG_PAINT, TRUE, 0);
  return ERR_NOFEATURE;
}

#ifdef DALIAN_ZHUANGGUANG
static RET_CODE on_pwdlg_exit(control_t *p_edit, u16 msg,
                              u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}
#endif

BEGIN_KEYMAP(pwdlg_edit_keymap, NULL)
  ON_EVENT(V_KEY_0, MSG_CHAR | '0')
  ON_EVENT(V_KEY_1, MSG_CHAR | '1')
  ON_EVENT(V_KEY_2, MSG_CHAR | '2')
  ON_EVENT(V_KEY_3, MSG_CHAR | '3')
  ON_EVENT(V_KEY_4, MSG_CHAR | '4')
  ON_EVENT(V_KEY_5, MSG_CHAR | '5')
  ON_EVENT(V_KEY_6, MSG_CHAR | '6')
  ON_EVENT(V_KEY_7, MSG_CHAR | '7')
  ON_EVENT(V_KEY_8, MSG_CHAR | '8')
  ON_EVENT(V_KEY_9, MSG_CHAR | '9')
  #ifdef DALIAN_ZHUANGGUANG
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  #endif
END_KEYMAP(pwdlg_edit_keymap, NULL)

BEGIN_MSGPROC(pwdlg_edit_proc, mbox_class_proc)
  ON_COMMAND(MSG_CHAR, on_pwdlg_char)
  ON_COMMAND(MSG_DESTROY, on_pwdlg_destroy)
  #ifdef DALIAN_ZHUANGGUANG
  ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
  #endif
  #ifdef ENABLE_CA
  ON_COMMAND(MSG_CA_PIN_VERIFY_INFO, on_ca_ippv_pin_verify)
  #endif
END_MSGPROC(pwdlg_edit_proc, mbox_class_proc)




