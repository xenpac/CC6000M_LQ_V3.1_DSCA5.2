/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept.h"
#include "ui_ca_public.h"


#define TENGRUI_CONDITIONAL_ACCEPT_BTN_CNT  5
enum ds_ca_btn_id
{
  IDC_BTN_CA_INFO = 1,
  IDC_BTN_CA_LEVEL_CONTROL,
  IDC_BTN_CA_WORK_DURATION,
  IDC_BTN_CA_PIN_MODIFY,
  IDC_BTN_CA_ENTTITLE_INFO,
  IDC_BTN_CA_BURSE,
  IDC_BTN_CA_OPERATE,
  IDC_BTN_CA_EMAIL,
};
#ifndef WIN32
cas_card_info_t *p_card_info = NULL;
#else
cas_card_info_t p_card_info = {0,};
product_entitles_info_t p_entitle_info = {0,};
cas_operators_info_t p_oper_info;
#endif

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

/*
static comm_help_data_t ca_menu_help_data = //help bar data
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
*/

u16 conditional_accept_cont_keymap(u16 key);
RET_CODE conditional_accept_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE open_conditional_accept(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_ctrl[CONDITIONAL_ACCEPT_ITEM_CNT];
  u16 i = 0, y = 0;
  u16 str_id[CONDITIONAL_ACCEPT_ITEM_CNT] = 
  {
    IDS_CA_SMARTCARD_INFO, IDS_CA_LEVEL_CONTROL_SET,
    IDS_CA_WOR_DURATION_SET, IDS_CA_PWD_MODIFY, IDS_CA_ACCREDIT_PRODUCK_INFO,
    IDS_CA_SLOT_INFO, IDS_CA_SP_INFO, IDS_CA_EMAIL
  };

  p_cont = ui_comm_root_create(ROOT_ID_CONDITIONAL_ACCEPT, 0, COMM_BG_X, COMM_BG_Y,
    COMM_BG_W, COMM_BG_H, 0, IDS_CA);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_cont_proc);

  y = CONDITIONAL_ACCEPT_ITEM_Y;

  for (i = 0; i < CONDITIONAL_ACCEPT_ITEM_CNT; i++)
  {
    p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u16)(IDC_BTN_CA_INFO + i),
                                              CONDITIONAL_ACCEPT_ITEM_X, y,
                                              CONDITIONAL_ACCEPT_ITEM_W, CONDITIONAL_ACCEPT_ITEM_H,
                                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_ITEM_1_HL, RSI_PBACK);
    text_set_font_style(p_ctrl[i], FSI_COMM_TXT_SH, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
    text_set_offset(p_ctrl[i], 20, 0);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl[i], str_id[i]);

    ctrl_set_related_id(p_ctrl[i],
                      0,                                     /* left */
                      (u8)((i - 1 +
                            CONDITIONAL_ACCEPT_ITEM_CNT) %
                           CONDITIONAL_ACCEPT_ITEM_CNT + 1),            /* up */
                      0,                                     /* right */
                      (u8)((i + 1) % CONDITIONAL_ACCEPT_ITEM_CNT + 1)); /* down */

    y += CONDITIONAL_ACCEPT_ITEM_H + CONDITIONAL_ACCEPT_ITEM_V_GAP;
  }
  //ui_comm_help_create(&game_help_data, p_cont);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  //ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0, 0);
  //ui_ca_do_cmd(CAS_CMD_BURSE_INFO_GET, 0,0);
  return SUCCESS;
}

#ifdef WIN32
void for_test()
{
  u8 i;
  p_card_info.card_work_level = 5;
  p_card_info.work_time.start_hour = 10;
  p_card_info.work_time.start_minute = 13;
  p_card_info.work_time.start_second = 59;
  p_card_info.work_time.end_hour = 23;
  p_card_info.work_time.end_minute = 11;
  p_card_info.work_time.end_second = 33;

  p_entitle_info.max_num = 13;
  for(i = 0; i < 13; i++)
  {
    p_entitle_info.p_entitle_info[i].product_id= i + 10;
    p_entitle_info.p_entitle_info[i].start_time[0] = 0x07;
    p_entitle_info.p_entitle_info[i].start_time[1] = 0xDD;
    p_entitle_info.p_entitle_info[i].start_time[2] = 12;
    p_entitle_info.p_entitle_info[i].start_time[3] = 31;
    p_entitle_info.p_entitle_info[i].start_time[4] = 18;
    p_entitle_info.p_entitle_info[i].start_time[5] = 01;
    p_entitle_info.p_entitle_info[i].start_time[6] = 59;
    
    p_entitle_info.p_entitle_info[i].expired_time[0] = 0x07;
    p_entitle_info.p_entitle_info[i].expired_time[1] = 0xDE;
    p_entitle_info.p_entitle_info[i].expired_time[2] = 12;
    p_entitle_info.p_entitle_info[i].expired_time[3] = 31;
    p_entitle_info.p_entitle_info[i].expired_time[4] = 18;
    p_entitle_info.p_entitle_info[i].expired_time[5] = 01;
    p_entitle_info.p_entitle_info[i].expired_time[6] = 59;
  }

  memcpy(p_oper_info.p_operator_info[0].operator_name, "yaphy montage", CAS_OPERATOR_NAME_MAX_LEN);
  memcpy(p_oper_info.p_operator_info[0].operator_phone, "0512-67776777", CAS_OPERATOR_PHONE_MAX_LEN);
  memcpy(p_oper_info.p_operator_info[0].operator_address, "suzhou montage ", CAS_OPERATOR_ADDR_MAX_LEN);
  memcpy(p_oper_info.p_operator_info[0].operator_id_ascii, "180", 6);
}
#endif
static RET_CODE on_ca_frm_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 ctrlID;
  #ifdef WIN32
  for_test();
  #endif
  ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl));
  switch(ctrlID)
  {
    case IDC_BTN_CA_INFO:
      manage_open_menu(ROOT_ID_CA_CARD_INFO, 0, 0); 
      break;

    case IDC_BTN_CA_LEVEL_CONTROL:
      #ifndef WIN32
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, (u32)para1, 0);
      #else
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, &p_card_info, 0);
      #endif
      break;
      
    case IDC_BTN_CA_WORK_DURATION:
      #ifndef WIN32
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, (u32)para1, 0);
      #else
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, &p_card_info, 0);
      #endif
      break;
      
    case IDC_BTN_CA_PIN_MODIFY:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_PIN, (u32)para1, 0);
      break;
      
    case IDC_BTN_CA_ENTTITLE_INFO:
      #ifndef WIN32
      manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, (u32)para1, 0);
      #else 
      manage_open_menu(ROOT_ID_CA_ENTITLE_INFO, &p_entitle_info, 0);
      #endif
      break;
      
    case IDC_BTN_CA_BURSE:
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH, (u32)para1, 0);
      break;
      
    case IDC_BTN_CA_OPERATE:
      #ifndef WIN32
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_INFO, (u32)para1, 0);
      #else
      manage_open_menu(ROOT_ID_CONDITIONAL_ACCEPT_INFO, &p_oper_info, 0);
      #endif
      break;  
    case IDC_BTN_CA_EMAIL:
      manage_open_menu(ROOT_ID_EMAIL_MESS, 0, 0);
      break;
    default:
      break;
  }
  
  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_cont_focus_change)  
  ON_COMMAND(MSG_SELECT, on_ca_frm_btn_select)
END_MSGPROC(conditional_accept_cont_proc, ui_comm_root_proc)




