/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_dlna_info.h"

enum dlna_info_id
{
  IDC_DLNA_SSID = 1,
  IDC_DLNA_IP_ADDR,
  IDC_DLNA_MAC,
  IDC_DLNA_TEXT_CONT
};

u16 stxt_dlna_net_info[DLNA_INFO_ITEM_CNT] =
{
  IDS_SSID, 
  IDS_IP_ADDR,
  IDS_IP_MAC
};

u16 dlna_info_keymap(u16 key);
RET_CODE dlna_info_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

static RET_CODE on_update_dlna_info(control_t *p_ctrl, 
								u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t * p_dl_ssid = ctrl_get_child_by_id(p_ctrl, IDC_DLNA_SSID);
  control_t * p_dl_ip = ctrl_get_child_by_id(p_ctrl, IDC_DLNA_IP_ADDR);
  control_t * p_dl_mac = ctrl_get_child_by_id(p_ctrl, IDC_DLNA_MAC);
  if(0 == msg)
  {
    ui_comm_static_set_content_by_strid(p_dl_ssid,IDS_SSID);
    ui_comm_static_set_content_by_strid(p_dl_ip,IDS_SSID);
    ui_comm_static_set_content_by_strid(p_dl_mac,IDS_SSID);
  }
  else
  {
    //update content
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);
  return ret ;
}

RET_CODE open_dlna_net_info(u32 para1, u32 para2)
{
	control_t *p_menu,*p_cont,*p_text;
	u16 i = 0, y = 0;

	p_menu = fw_create_mainwin(ROOT_ID_DLNA_NET_INFO,
		                    120, 80, 610, 300,
		                    0, 0, OBJ_ATTR_ACTIVE, 0);
   
	if (p_menu == NULL)
	{
		return ERR_FAILURE;
	}  
  	ctrl_set_rstyle(p_menu, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);
    
    p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_DLNA_TEXT_CONT,
  	                                  0, 0,
  	                                  600, 300,
  	                                  p_menu, 0);
  ctrl_set_rstyle(p_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_cont,dlna_info_keymap);
  ctrl_set_proc(p_cont, dlna_info_proc);
  
  y= 15;
  
  for (i = 0; i<DLNA_INFO_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
      case 1:
      case 2:
        p_text = ui_comm_static_create(p_cont, (u8)(IDC_DLNA_SSID + i),
                                           40, y,
                                           220,
                                           220);
        ui_comm_static_set_static_txt(p_text, stxt_dlna_net_info[i]);
        ui_comm_static_set_param(p_text, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_text, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
        ui_comm_static_set_align_type(p_text, 0, STL_LEFT | STL_VCENTER, 0,STL_LEFT | STL_VCENTER);
        break;
	default:
	 break;		  
    }
    y += COMM_CTRL_H + DLNA_INFO_ITEM_V_GAP;
  }
  on_update_dlna_info(p_cont, 0, 0, 0);
 #if 0  
  p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_MMENU_CATE_TEXT_START,
  	                                  340, 230,
  	                                  160, 60,
  	                                  p_cont, 0);
  ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_text, FSI_WHITE_20, FSI_YELLOW_32, FSI_YELLOW_32);
  text_set_align_type(p_text, STL_CENTER| STL_VCENTER);
  text_set_content_type(p_text, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_text, IDS_DLNA);

 p_text_cont2 = ctrl_create_ctrl(CTRL_CONT, IDC_MMENU_CATE_TEXT_CONT_TBD,
  	                                  100, 100,
  	                                  500, 300,
  	                                  p_cont, 0);
 ctrl_set_rstyle(p_text_st, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  p_text_tbd = ctrl_create_ctrl(CTRL_TEXT, IDC_MMENU_CATE_TEXT_TBD,
	                                  10, 20,
	                                  260, 100,
	                                  p_text_cont2, 0);
  ctrl_set_rstyle(p_text_tbd, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_text_tbd, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
  text_set_align_type(p_text_tbd, STL_CENTER| STL_VCENTER);
  text_set_content_type(p_text_tbd, TEXT_STRTYPE_EXTSTR);
  text_set_content_by_extstr(p_text_tbd, (u16*)"TBD");
  ctrl_set_sts(p_text_cont2,OBJ_STS_HIDE);
  #endif
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, TRUE);
  
  return SUCCESS;

}

static RET_CODE on_dlna_info_exit(control_t *p_list, 
								u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = SUCCESS;
  
	ret = manage_close_menu(ROOT_ID_DLNA_NET_INFO, 0, 0); 

	return ret;
}

BEGIN_KEYMAP(dlna_info_keymap, NULL)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(dlna_info_keymap, NULL)

BEGIN_MSGPROC(dlna_info_proc,ui_comm_root_proc)
ON_COMMAND(MSG_EXIT, on_dlna_info_exit)
//ON_COMMAND(MSG_DLNA_INFO, on_update_dlna_info)
END_MSGPROC(dlna_info_proc, ui_comm_root_proc)
