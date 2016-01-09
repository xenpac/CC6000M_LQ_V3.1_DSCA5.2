/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "ui_common.h"
#include "pti.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "ui_ota_user_input.h"
#include "nim.h"
//#include "ap_ota.h"
#include "ui_ota_upgrade.h"
#include "ui_ota_api_v2.h"
#include "ota_dm_api.h"
#include "ota_public_def.h"

enum ota_user_input_ctrl_id
{
  IDC_OTA_USER_INPUT_TPFREQ_T = 1,
  IDC_OTA_USER_INPUT_TPFREQ,
  IDC_OTA_USER_INPUT_SYM_T,
  IDC_OTA_USER_INPUT_SYM,
  IDC_OTA_USER_INPUT_QAM_T,
  IDC_OTA_USER_INPUT_QAM,
  IDC_OTA_USER_INPUT_DPID_T,
  IDC_OTA_USER_INPUT_DPID,
  IDC_OTA_USER_INPUT_START,
  IDC_OTA_USER_INPUT_HELP,
  IDC_OTA_USER_INPUT_TTL,
  IDC_OTA_USER_INPUT_TDI_INFO,
};

u16 ota_user_input_keymap(u16 key);
RET_CODE ota_user_input_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ota_user_input_num_keymap(u16 key);

u16 ota_user_input_cbox_keymap(u16 key);
RET_CODE ota_user_input_cbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ota_user_input_start_keymap(u16 key);
RET_CODE ota_user_input_start_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

s32 open_ota_user_input(u32 para1, u32 para2)
{
  struct control *p_cont, *p_title;
  struct control *p_item[OTA_USER_INPUT_ITEM_CNT];

  u8 stxt[OTA_USER_INPUT_ITEM_CNT] =
  {
    IDS_FREQUECY,0,
    IDS_SYMBOL_RATE,0,
    IDS_MODULATION,0,
    IDS_DOWNLOAD_PID,0,
    IDS_START,0
  };
  u16 y;
  u8 i;
  ota_param_t ota_param;
  memset(&ota_param,0,sizeof(ota_param_t));
  
  mul_ota_dm_api_read_ota_param(&ota_param);
  p_cont = fw_create_mainwin(ROOT_ID_OTA_USER_INPUT, 
    OTA_USER_INPUT_CONTX, OTA_USER_INPUT_CONTY, 
    OTA_USER_INPUT_CONTW, OTA_USER_INPUT_CONTH, 
    0, 0, OBJ_ATTR_ACTIVE, 0);

  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  ctrl_set_proc(p_cont, ota_user_input_proc);
  ctrl_set_keymap(p_cont, ota_user_input_keymap);  

  y = OTA_USER_INPUT_ITEMY;
  
  for(i = 0; i < OTA_USER_INPUT_ITEM_CNT; i++)
  {
    switch(i + 1)
    {
      case IDC_OTA_USER_INPUT_TPFREQ_T:
      case IDC_OTA_USER_INPUT_SYM_T:
      case IDC_OTA_USER_INPUT_QAM_T:
      case IDC_OTA_USER_INPUT_DPID_T:
        p_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMLX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        text_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        text_set_align_type(p_item[i], STL_LEFT | STL_VCENTER);
        text_set_content_type(p_item[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_item[i], stxt[i]);
        break;
        
      case IDC_OTA_USER_INPUT_TPFREQ:
      case IDC_OTA_USER_INPUT_SYM:
      case IDC_OTA_USER_INPUT_DPID:
        p_item[i] = ctrl_create_ctrl(CTRL_NBOX, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMRX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_keymap(p_item[i], ota_user_input_num_keymap);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        nbox_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        nbox_set_align_type(p_item[i], STL_CENTER | STL_VCENTER);
        nbox_set_num_type(p_item[i], NBOX_NUMTYPE_DEC);
        switch(i + 1)
        {
          case IDC_OTA_USER_INPUT_TPFREQ:
            nbox_set_range(p_item[i], OTA_TPFREQ_MIN, OTA_TPFREQ_MAX, OTA_TPFREQ_BIT);
            nbox_set_focus(p_item[i], (OTA_TPFREQ_BIT - 1));
            nbox_set_postfix_type(p_item[i], NBOX_ITEM_PREFIX_TYPE_STRID);
            nbox_set_postfix_by_strid(p_item[i], IDS_UNIT_KHZ);
            nbox_set_num_by_dec(p_item[i], ota_param.ota_tp.lockc.tp_freq);
            break;
          case IDC_OTA_USER_INPUT_SYM:
            nbox_set_range(p_item[i], OTA_SYM_MIN, OTA_SYM_MAX, OTA_SYM_BIT);
            nbox_set_focus(p_item[i], (OTA_SYM_BIT - 1));
            nbox_set_postfix_type(p_item[i], NBOX_ITEM_PREFIX_TYPE_STRID);
            nbox_set_postfix_by_strid(p_item[i], IDS_UNIT_KSS);
            nbox_set_num_by_dec(p_item[i], ota_param.ota_tp.lockc.tp_sym);
            break;
          case IDC_OTA_USER_INPUT_DPID:
            nbox_set_range(p_item[i], OTA_DPID_MIN, OTA_DPID_MAX, OTA_DPID_BIT);
            nbox_set_focus(p_item[i], (OTA_DPID_BIT - 1));
            nbox_set_num_by_dec(p_item[i], ota_param.ota_tp.data_pid);
            break;
          default:
            MT_ASSERT(0);
            break;
        }

        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);
        break;

      case IDC_OTA_USER_INPUT_QAM:
        p_item[i] = ctrl_create_ctrl(CTRL_CBOX, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T+i),
          OTA_USER_INPUT_ITEMRX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_keymap(p_item[i], ota_user_input_cbox_keymap);
        ctrl_set_proc(p_item[i], ota_user_input_cbox_proc);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        cbox_enable_cycle_mode(p_item[i], TRUE);
        cbox_set_work_mode(p_item[i], CBOX_WORKMODE_STATIC);
        cbox_set_align_style(p_item[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        cbox_static_set_count(p_item[i], 5);
        cbox_static_set_content_type(p_item[i], CBOX_ITEM_STRTYPE_UNICODE);
        cbox_static_set_content_by_ascstr(p_item[i], 0, (u8*)"QAM16");
        cbox_static_set_content_by_ascstr(p_item[i], 1, (u8*)"QAM32");
        cbox_static_set_content_by_ascstr(p_item[i], 2, (u8*)"QAM64");
        cbox_static_set_content_by_ascstr(p_item[i], 3, (u8*)"QAM128");
        cbox_static_set_content_by_ascstr(p_item[i], 4, (u8*)"QAM256");
        cbox_static_set_focus(p_item[i], (ota_param.ota_tp.lockc.nim_modulate - NIM_MODULA_QAM16));
        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);
        break;    
        
      case IDC_OTA_USER_INPUT_START:
        p_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMLX, y, 
          (OTA_USER_INPUT_ITEMRW + OTA_USER_INPUT_ITEMLW), 
          OTA_USER_INPUT_ITEMH, p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        text_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        text_set_align_type(p_item[i], STL_LEFT | STL_VCENTER);
        text_set_content_type(p_item[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_item[i], stxt[i]);
        ctrl_set_keymap(p_item[i], ota_user_input_start_keymap);
        ctrl_set_proc(p_item[i], ota_user_input_start_proc);     
        y += (OTA_USER_INPUT_ITEMH + 2*OTA_USER_INPUT_ITEM_VGAP);        
        break;
      case IDC_OTA_USER_INPUT_HELP:
        p_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMLX, y, 
          (OTA_USER_INPUT_ITEMRW + OTA_USER_INPUT_ITEMLW), 
          OTA_USER_INPUT_ITEMH, p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        text_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        text_set_align_type(p_item[i], STL_LEFT | STL_VCENTER);
        text_set_content_type(p_item[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_item[i], IDS_PRESS_EXIT_TO_EXIT_UPGRADE_AND_RESTART);
        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);        
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }
  ctrl_set_related_id(p_item[1],  0,  9, 0, 4);//freq
  ctrl_set_related_id(p_item[3],  0,  2, 0, 6);//sym
  ctrl_set_related_id(p_item[5],  0,  4, 0, 8);//qam
  ctrl_set_related_id(p_item[7],  0,  6, 0, 9);//pid
  ctrl_set_related_id(p_item[8],  0,  8, 0, 2);//start

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_USER_INPUT_TTL,
    OTA_USER_INPUT_TTLX, OTA_USER_INPUT_TTLY, 
    OTA_USER_INPUT_TTLW, OTA_USER_INPUT_TTLH,
    p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);  
  text_set_content_by_strid(p_title, IDS_OTA_USER_INPUT);

  ctrl_default_proc(p_item[1], MSG_GETFOCUS, 0, 0);//focus on tp freq
  
  ctrl_paint_ctrl(p_cont, FALSE);
  return SUCCESS;
}

static RET_CODE on_ota_user_input_exit(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ota_bl_info_t bl_info = {0};
    
  mul_ota_dm_api_read_bootload_info(&bl_info);
  
  OS_PRINTF("\r\n[OTA]%s:upg ota_tri[%d] ",__FUNCTION__, bl_info.ota_status);
  
  bl_info.ota_status = OTA_TRI_MODE_NONE;
  bl_info.load_block_id = MAINCODE_BLOCK_ID;
  mul_ota_dm_api_save_bootload_info(&bl_info);
   
#ifndef WIN32
  mtos_task_delay_ms(1000);

  //hal_watchdog_enable();
  hal_watchdog_init(0);
  hal_watchdog_enable();

  //hal_pm_reset();
#endif
  return SUCCESS;
}

static RET_CODE on_ota_user_input_cbox_changed(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cbox_class_proc(p_ctrl, msg, 0, 0);
	return SUCCESS;
}

extern void  ui_ota_api_save_do_ota_tp(nim_info_t *tp_info);
extern void ui_ota_api_auto_mode_start(ota_work_t  mode);

static RET_CODE on_ota_user_input_start(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_cont;
  struct control *p_tpfreq, *p_symb, *p_modulation;
  struct control *p_dpid;
  nim_info_t tp_info;
  ota_info_t upgrade;
  memset(&tp_info,0,sizeof(nim_info_t));
  //mul_ota_dm_api_read_ota_param(&ota_param);
  p_cont = ctrl_get_parent(p_ctrl);
  p_tpfreq = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_TPFREQ);
  p_symb = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_SYM);
  p_modulation = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_QAM);
  p_dpid = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_DPID);
  
  //to be continued.
  upgrade.sys_mode = SYS_DVBC;
  upgrade.ota_tri = OTA_TRI_AUTO;
  upgrade.lockc.tp_freq = nbox_get_num(p_tpfreq); //KHz
  upgrade.lockc.tp_sym= nbox_get_num(p_symb);
  upgrade.lockc.nim_modulate = (cbox_static_get_focus(p_modulation) + NIM_MODULA_QAM16);
  upgrade.download_data_pid = nbox_get_num(p_dpid);
  
  
  tp_info.lock_mode = upgrade.sys_mode;
  tp_info.data_pid = upgrade.download_data_pid;
  memcpy(&(tp_info.lockc), &(upgrade.lockc), sizeof(upgrade.lockc));
  tp_info.lockc.tp_sym = upgrade.lockc.tp_sym;
  tp_info.lockc.tp_freq = upgrade.lockc.tp_freq;
  tp_info.lockc.nim_modulate = upgrade.lockc.nim_modulate;
  ui_ota_api_save_do_ota_tp(&tp_info);
  ui_ota_api_stop();
  //ui_ota_api_manual_check_mode(&tp_info);
   ui_ota_api_auto_mode_start(OTA_WORK_SELECT_AUTO);
  //ui_ota_relock(&upgrade);

  if(fw_find_root_by_id(ROOT_ID_OTA_UPGRADE) == NULL)
  {
	 manage_open_menu(ROOT_ID_OTA_UPGRADE, 0, 0);
  }  

	return SUCCESS;
}

BEGIN_KEYMAP(ota_user_input_keymap, NULL)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ota_user_input_keymap, NULL)

BEGIN_MSGPROC(ota_user_input_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ota_user_input_exit)    
END_MSGPROC(ota_user_input_proc, cont_class_proc);

BEGIN_KEYMAP(ota_user_input_cbox_keymap, NULL)
	ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
	ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
END_KEYMAP(ota_user_input_cbox_keymap, NULL)

BEGIN_MSGPROC(ota_user_input_cbox_proc, cbox_class_proc)
  ON_COMMAND(MSG_DECREASE, on_ota_user_input_cbox_changed)
  ON_COMMAND(MSG_INCREASE, on_ota_user_input_cbox_changed)
END_MSGPROC(ota_user_input_cbox_proc, cbox_class_proc);

BEGIN_KEYMAP(ota_user_input_start_keymap, NULL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ota_user_input_start_keymap, NULL)

BEGIN_MSGPROC(ota_user_input_start_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_ota_user_input_start)
END_MSGPROC(ota_user_input_start_proc, text_class_proc);

BEGIN_KEYMAP(ota_user_input_num_keymap, NULL)
  ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
  ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
  ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
  ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
  ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
  ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
  ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
  ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
  ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
  ON_EVENT(V_KEY_9, MSG_NUMBER | 9)

  ON_EVENT(V_KEY_UP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)

  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_UNSELECT)
  ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ota_user_input_num_keymap, NULL)	

