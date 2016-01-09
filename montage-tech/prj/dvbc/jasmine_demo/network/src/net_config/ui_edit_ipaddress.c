/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_edit_ipaddress.h"
#include "lib_char.h"


enum local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 200,
  MSG_CONFIRM,
};

enum control_id
{
  IDC_EDIT_IP_TITLE = 1,
  IDC_EDIT_IP_EBOX,
};

static comm_help_data_t help_data = //help bar data
{
  2,
  2,
  {IDS_SAVE,
   IDS_CANCEL},
  {IM_HELP_YELLOW,
   IM_HELP_BLUE},
};
static BOOL g_is_add = FALSE;
static u8 ip_separator[IPBOX_MAX_ITEM_NUM] = {'.', '.', '.', ' '};
static edit_ip_param_t edit_ip_param;


u16 edit_ip_cont_keymap(u16 key);
RET_CODE edit_ip_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


RET_CODE open_edit_ipaddress(u32 para1, u32 para2)
{
  control_t *p_cont, *p_title;
  control_t *p_edit_ip, *p_help;
  u8 i = 0;
  ip_address_t *ip_address =  NULL;//mtos_malloc(sizeof(ip_address_t));

  g_is_add = (BOOL)para1;

  memcpy((void *)&edit_ip_param, (void *)para2, sizeof(edit_ip_param_t));


  p_cont = fw_create_mainwin(ROOT_ID_EDIT_IPADDRESS,
                             EDIT_IP_CONT_X, EDIT_IP_CONT_Y,
                             EDIT_IP_CONT_W, EDIT_IP_CONT_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_EDIT_IP_FRM, RSI_EDIT_IP_FRM, RSI_EDIT_IP_FRM);
  ctrl_set_keymap(p_cont, edit_ip_cont_keymap);
  ctrl_set_proc(p_cont, edit_ip_cont_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_EDIT_IP_TITLE,
                             EDIT_IP_TITLE_X, EDIT_IP_TITLE_Y,
                             EDIT_IP_TITLE_W,
                             EDIT_IP_TITLE_H, p_cont,
                             0);
  text_set_font_style(p_title, FSI_BLACK, FSI_BLACK, FSI_BLACK);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, g_is_add ? IDS_ADD_TP: IDS_EDIT_TP);

 //create ipbox edit control
  p_edit_ip = ui_comm_ipedit_create(p_cont, IDC_EDIT_IP_EBOX,EDIT_IP_EBOX_X, EDIT_IP_EBOX_Y, EDIT_IP_EBOX_LW,
EDIT_IP_EBOX_RW);
  ctrl_set_rstyle(p_edit_ip, RSI_PBACK, RSI_ITEM_1_HL, RSI_PBACK);
  ui_comm_ipedit_set_param(p_edit_ip, 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
  ui_comm_ipedit_set_static_txt(p_edit_ip, IDS_IP_ADDRESS);
  ui_comm_ctrl_set_keymap(p_edit_ip, ui_comm_ipbox_keymap);
  ui_comm_ctrl_set_proc(p_edit_ip, ui_comm_ipbox_proc);
  for(i = 0; i < IPBOX_MAX_ITEM_NUM; i++)
  {
    ui_comm_ipedit_set_separator_by_ascchar(p_edit_ip, (u8)i, ip_separator[i]);
  }

  
  if(!g_is_add)
  {
  //  memcpy(ip_address, (ip_address_t *)(edit_ip_param.name), sizeof(ip_address_t));
    ip_address = (ip_address_t *)(edit_ip_param.name);
  // OS_PRINTF("lou ipaddress %d  %d  %d  %d\n",ip_address->s_b1,ip_address->s_b2,ip_address->s_b3,ip_address->s_b4);
   ui_comm_ipedit_set_address(p_edit_ip, ip_address);
  }

  // add help bar
  p_help = ui_comm_help_create_for_pop_dlg(&help_data, p_cont);
  ui_comm_help_set_font(p_help, FSI_BLACK, FSI_BLACK, FSI_BLACK);

  ctrl_default_proc(p_edit_ip, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_edit_ipaddress_confirm(control_t *p_cont,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{

  control_t *p_ctrl = NULL;
  u8 ipaddress[128] = "";
  ip_address_t addr;
  control_t *p_cont_ebox;
  control_t *p_edit_ip;
  RET_CODE ret = SUCCESS;

  //u16 *unistr;
  p_cont_ebox =  ctrl_get_child_by_id(p_cont, IDC_EDIT_IP_EBOX);
  p_edit_ip = ctrl_get_child_by_id(p_cont_ebox, 2);
  ret = ctrl_process_msg(p_edit_ip, MSG_UNSELECT, 0, 0);
  if(ret != SUCCESS)
  {
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_EDIT_IP_EBOX);
  ui_comm_ipedit_get_address(p_ctrl, &addr);
  sprintf((char*)ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
  OS_PRINTF("###on_edit_ipaddress_confirm ip address value is:%s####\n",ipaddress);

  /*dvb_to_unicode(ipaddress,(s32)( edit_ip_param.max_len - 1),
      unistr,(s32)(edit_ip_param.max_len));*/

   if(edit_ip_param.cb(g_is_add, (void *)(&addr)) != SUCCESS)
  {
    // save data is failed, don't exit this menu
    return SUCCESS;
  }
  //should add callback function
  manage_close_menu(ROOT_ID_EDIT_IPADDRESS, 0, 0);
  }
  return SUCCESS;

}


static RET_CODE on_edit_ipaddress_cancel(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_EDIT_IPADDRESS, 0, 0);
  return SUCCESS;
}


BEGIN_KEYMAP(edit_ip_cont_keymap, NULL)
  ON_EVENT(V_KEY_YELLOW, MSG_CONFIRM)
  ON_EVENT(V_KEY_BLUE, MSG_CANCEL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(edit_ip_cont_keymap, NULL)

BEGIN_MSGPROC(edit_ip_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CONFIRM, on_edit_ipaddress_confirm)
  ON_COMMAND(MSG_CANCEL, on_edit_ipaddress_cancel)
END_MSGPROC(edit_ip_cont_proc, cont_class_proc)

