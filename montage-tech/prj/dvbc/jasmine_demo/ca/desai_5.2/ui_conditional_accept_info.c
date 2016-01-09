/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_info.h"

#include "ui_ca_public.h"

enum ca_ca_info_id
{
  IDC_CA_CARD_SP_ID = 1,
  IDC_CA_CARD_SP_NAME,
  IDC_CA_CARD_SP_TEL,
  IDC_CA_CARD_SP_ADDR,
};


u16 conditional_accept_info_cont_keymap(u16 key);
RET_CODE conditional_accept_info_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static void conditional_accept_info_get_ascstr(u8 * asc_str, u8 * str_addr)
{
  sprintf((char *)asc_str, "%s", str_addr);
}

static void conditional_accept_info_set_content(control_t *p_cont, cas_operators_info_t *p_card_info)
{
  control_t *p_card_sp_id = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_ID);
  control_t *p_card_name = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_NAME);
  control_t *p_card_sp_tel = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_TEL);
  control_t *p_card_sp_addr = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_SP_ADDR);
  u8 asc_str[CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH];
  u16 uni_str[CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH + 1] = {0};
  
  if(p_card_info == NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_card_sp_id, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_card_name,(u8 *) " ");
    ui_comm_static_set_content_by_ascstr(p_card_sp_tel, (u8 *)" ");
    ui_comm_static_set_content_by_ascstr(p_card_sp_addr, (u8 *)" ");
  }
  else
  {
    //sp_id
    conditional_accept_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_id_ascii);
    ui_comm_static_set_content_by_ascstr(p_card_sp_id, p_card_info->p_operator_info[0].operator_id_ascii);

    //SP-name
    //conditional_accept_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_name);
    gb2312_to_unicode(p_card_info->p_operator_info[0].operator_name, 
                                        CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH, 
                                        uni_str, CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH+1);
    ui_comm_static_set_content_by_unistr(p_card_name, uni_str);
    
    //sp_tel
    //conditional_accept_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_phone);
    //OS_PRINTF("operator_phone=%s\n",asc_str);
   gb2312_to_unicode(p_card_info->p_operator_info[0].operator_phone, 
                                    CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH, 
                                    uni_str, CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH+1);
    //ui_comm_static_set_content_by_ascstr(p_card_sp_tel, asc_str);
    ui_comm_static_set_content_by_unistr(p_card_sp_tel, uni_str);
    
    //sp_addr
    //conditional_accept_info_get_ascstr(asc_str, p_card_info->p_operator_info[0].operator_address);
    gb2312_to_unicode(p_card_info->p_operator_info[0].operator_address, 
                                        CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH, 
                                        uni_str, CONDITIONAL_ACCEPT_INFO_CA_INFO_MAX_LENGTH+1);
    ui_comm_static_set_content_by_unistr(p_card_sp_addr, uni_str);
  }
}

RET_CODE open_conditional_accept_info(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl = NULL;
  u8 i;
  u16 y;
  u16 stxt_ca_info[CONDITIONAL_ACCEPT_INFO_ITEM_CNT] =
  { 
     IDS_CA_SP_ID, IDS_CA_SP_NAME, IDS_CA_OPERATOR_TEL,IDS_CA_OPERATOR_ADDRES
  };
  #if 0
  comm_help_data_t ca_info_help_data = //help bar data
  {
    1,
    1,
    {
      IDS_BACK,
    },
    { 
      IM_ICON_BACK,
    },
  };
  #endif

  p_cont = ui_comm_root_create(ROOT_ID_CONDITIONAL_ACCEPT_INFO, 0,
                             COMM_BG_X,
                             COMM_BG_Y, COMM_BG_W,
                             COMM_BG_H, IM_TITLEICON_TV,
                             IDS_CA_SP_INFO);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_info_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_info_cont_proc);

  //CA info
  y = CONDITIONAL_ACCEPT_INFO_ITEM_Y;
  for (i = 0; i<CONDITIONAL_ACCEPT_INFO_ITEM_CNT; i++)
  {
  
    p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_CARD_SP_ID + i),
                                           CONDITIONAL_ACCEPT_INFO_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_INFO_ITEM_LW,
                                           CONDITIONAL_ACCEPT_INFO_ITEM_RW);
    ui_comm_static_set_static_txt(p_ctrl, stxt_ca_info[i]);
    ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
    ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    ui_comm_ctrl_set_cont_rstyle(p_ctrl, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH);

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CONDITIONAL_ACCEPT_INFO_ITEM_CNT) %
                             CONDITIONAL_ACCEPT_INFO_ITEM_CNT + IDC_CA_CARD_SP_ID),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CONDITIONAL_ACCEPT_INFO_ITEM_CNT + IDC_CA_CARD_SP_ID));/* down */

    y += CONDITIONAL_ACCEPT_INFO_ITEM_H + ACCEPT_CA_OPER_INFO_ITEM_V_GAP;
  }

  //ui_comm_help_create(&ca_info_help_data, p_cont);

  //conditional_accept_info_set_content(p_cont, (cas_operators_info_t *) para1);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  #ifndef WIN32
  ui_ca_do_cmd((u32)CAS_CMD_OPERATOR_INFO_GET, 0 ,0);
  #else
  on_conditional_accept_info_update(p_cont, 0, 0, para1);
  #endif
  
  return SUCCESS;
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_info_set_content(p_cont, (cas_operators_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_info_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_info_cont_proc, ui_comm_root_proc)
  //ON_COMMAND(MSG_FOCUS_UP, on_conditional_accept_cont_focus_change)
  //ON_COMMAND(MSG_FOCUS_DOWN, on_conditional_accept_cont_focus_change)
  ON_COMMAND(MSG_CA_OPE_INFO, on_conditional_accept_info_update)
END_MSGPROC(conditional_accept_info_cont_proc, ui_comm_root_proc)


