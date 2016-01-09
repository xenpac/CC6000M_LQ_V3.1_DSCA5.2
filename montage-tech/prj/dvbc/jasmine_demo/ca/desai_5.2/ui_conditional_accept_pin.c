/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_pin.h"
#include "ui_ca_public.h"

enum ca_pin_modify_id
{
  IDC_CA_PIN_MODIFY_PSW_OLD = 1,
  IDC_CA_PIN_MODIFY_PSW_NEW,
  IDC_CA_PIN_MODIFY_PSW_CONFIRM,
  IDC_CA_PIN_MODIFY_PSW_OK,
  IDC_CA_PIN_CHANGE_RESULT,
  IDC_CA_PIN_CHANGE_RESULT2,
};

static cas_pin_modify_t pin_modify;
u16 conditional_accept_pin_cont_keymap(u16 key);
RET_CODE conditional_accept_pin_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_pin_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_pin_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);



RET_CODE open_conditional_accept_pin(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_ctrl = NULL,*p_ctrl2 = NULL;
  u8 i;
  u16 y;
  u16 stxt_pin[CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT] =
  { 
    IDS_CA_INPUT_OLD_PIN, IDS_CA_INPUT_NEW_PIN,IDS_CA_CONFIRM_NEW_PIN
  };
#if 0
  static comm_help_data_t ca_pin_help_data = //help bar data
  {
    3,
    3,
    {
      IDS_BACK,
      IDS_SELECTED,
      //IDS_FN_SELECT,
      IDS_FN_SELECT,
    },
    { 
      IM_ICON_BACK,
      IM_ICON_SELECT,
      //IM_ICON_ARROW_LEFT_RIGHT,
      IM_ICON_ARROW_UP_DOWN,
    },
  };
#endif

  p_cont = ui_comm_root_create(ROOT_ID_CONDITIONAL_ACCEPT_PIN, 0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,IM_TITLEICON_TV,
                               IDS_CA_PWD_MODIFY);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_pin_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_pin_cont_proc);

  //pin modify
  y = CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y;
  for (i = 0; i<CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
      case 1:
      case 2:
        p_ctrl = ui_comm_pwdedit_create(p_cont, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                                             CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X, y,
                                             CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW,
                                             CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW);
        ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_pin[i]);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        ui_comm_ctrl_set_keymap(p_ctrl,conditional_accept_pin_pwdedit_keymap);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_pin_pwdedit_proc);
        break;
      case 3:
        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                                CONDITIONAL_ACCEPT_PIN_OK_ITEM_X, 
                                y,
                                CONDITIONAL_ACCEPT_PIN_OK_ITEM_W, 
                                CONDITIONAL_ACCEPT_PIN_OK_ITEM_H,
                                p_cont, 0);
        ctrl_set_rstyle(p_ctrl,RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
        text_set_font_style(p_ctrl,FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl, IDS_OK);
        break;
      default:  
        break;
      }
      ctrl_set_related_id(p_ctrl,
                          0,                                     /* left */
                          (u8)((i - 1 +
                                CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT) %
                               CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD),           /* up */
                          0,                                     /* right */
                          (u8)((i + 1) % CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD));/* down */

      y += CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H + CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP;
  }

//change result
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_PIN_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_X,
                              y + 100,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_W,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_GRAY);
  text_set_font_style(p_ctrl, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
  ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);

  p_ctrl2 = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_PIN_CHANGE_RESULT2,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_X,
                              y + 100,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_W,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ctrl2, RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_GRAY);
  text_set_font_style(p_ctrl2, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_align_type(p_ctrl2, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl2, TEXT_STRTYPE_UNICODE);   
  ctrl_set_sts(p_ctrl2,OBJ_STS_HIDE);

  //ui_comm_help_create(&ca_pin_help_data, p_cont);

  ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_PIN_MODIFY_PSW_OLD), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
  
  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_pwdedit_maxtext(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_active;
  //u8 ctrl_id;

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  //ctrl_id = ctrl_get_ctrl_id(p_active);
  ctrl_get_ctrl_id(p_active);

  ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  BOOL enable_psw_pinold, enable_psw_pinnew ,enable_psw_pinnew_confirm;
  //u32 psw_len_confirm_all;
  control_t *p_result, *p_ctrl_temp;
  u32 pin_new_psw,pin_old_psw;
  u32 pin_new_psw_confirm;
  u8 i = 0;
  //change result
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);
  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_OLD);
  pin_old_psw = ui_comm_pwdedit_get_value(p_ctrl_temp);
  enable_psw_pinold = ui_comm_pwdedit_is_full(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_NEW);
  pin_new_psw = ui_comm_pwdedit_get_value(p_ctrl_temp);
  enable_psw_pinnew = ui_comm_pwdedit_is_full(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_CONFIRM);
  pin_new_psw_confirm = ui_comm_pwdedit_get_value(p_ctrl_temp);
  enable_psw_pinnew_confirm = ui_comm_pwdedit_is_full(p_ctrl_temp);

  //psw_len_confirm_all = enable_psw_pinold + enable_psw_pinnew + enable_psw_pinnew_confirm;
  if(!enable_psw_pinold || !enable_psw_pinnew ||!enable_psw_pinnew_confirm)
  {
    text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
    ctrl_paint_ctrl(p_result, TRUE);
  }
  else if(pin_new_psw == pin_new_psw_confirm)
  {
    pin_modify.new_pin[0]=(u8)((pin_new_psw & 0x00ff0000)>>16);
    pin_modify.new_pin[1]=(u8)((pin_new_psw & 0x0000ff00)>>8);
    pin_modify.new_pin[2]=(u8)(pin_new_psw & 0x000000ff);

    pin_modify.old_pin[0]=(u8)((pin_old_psw & 0x00ff0000)>>16);
    pin_modify.old_pin[1]=(u8)((pin_old_psw & 0x0000ff00)>>8);
    pin_modify.old_pin[2]=(u8)(pin_old_psw & 0x000000ff);
    ui_ca_do_cmd((u32)CAS_CMD_PIN_SET, (u32)&pin_modify ,0);
  }
  else
  {
    text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
    ctrl_paint_ctrl(p_result, TRUE);
  }
  for(i=0; i<3; i++)
  {
    p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD+i));
    ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_ctrl_temp), MSG_EMPTY, 0, 0);
    ctrl_paint_ctrl(ui_comm_ctrl_get_ctrl(p_ctrl_temp), TRUE);
  }
  
  return SUCCESS;
}

static void get_pin_error_str(u16 * uni_str, u16 str_id1,u16 str_id2, u32 content)
{
  u16 str[30] = {0}; 
  u16 len = 0;
  
  gui_get_string(str_id1, uni_str, 64);
  convert_i_to_dec_str(str, (s32)content); 
  uni_strcat(uni_str, str,64);
  
  len = (u16)uni_strlen(uni_str);
  gui_get_string(str_id2, str, 64 - len); 
  uni_strcat(uni_str, str, 64); 

  return;
}

static RET_CODE on_conditional_accept_pin_ca_modify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result,*p_result2;
  //RET_CODE ret;
  u16 uni_num[64];
  u16 pin_spare_num = 0;
  cas_pin_modify_t *ca_pin_info = NULL;
  
  ca_pin_info = (cas_pin_modify_t *)para2;
  //ret = (RET_CODE)para1;
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);
  p_result2 = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT2);

  if(7 == para1)
  {
    ctrl_set_sts(p_result2,OBJ_STS_SHOW);
    ctrl_set_sts(p_result,OBJ_STS_HIDE);
  }
  else
  {
    ctrl_set_sts(p_result,OBJ_STS_SHOW);
    ctrl_set_sts(p_result2,OBJ_STS_HIDE);
  }
  
  if(para1 == 2)
  {
    text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
  }
  else if(para1 == 18)
  {
    text_set_content_by_strid(p_result, IDS_CA_PARAME_SET_ERROR);
  }
  else if(para1 == 1)
  {
    text_set_content_by_strid(p_result, IDS_CA_IC_CMD_FAIL);
  }
  else if(para1 == 9)
  {
    text_set_content_by_strid(p_result, IDS_CA_CARD_PIN_LOCK);
  }
  else if(para1 == 7)
  {
    pin_spare_num = (u16)ca_pin_info->pin_spare_num;
    get_pin_error_str(uni_num, IDS_CA_PASSWORD_ERROR, IDS_CA_PIN_CHANCES_LEFT, pin_spare_num);
    text_set_content_by_unistr(p_result2,uni_num);
  }
  else
  {
    text_set_content_by_strid(p_result, IDS_CA_PASSWORD_ERROR);
  }
  ctrl_paint_ctrl(p_result, TRUE);
  ctrl_paint_ctrl(p_result2,TRUE);
  
  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_conditional_accept_pin_ca_frm_ok)  
  ON_COMMAND(MSG_CA_PIN_SET_INFO, on_conditional_accept_pin_ca_modify)
END_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_pin_pwdedit_maxtext)
END_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)



