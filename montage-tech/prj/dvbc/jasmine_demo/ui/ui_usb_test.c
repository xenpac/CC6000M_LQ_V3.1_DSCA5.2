/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_usb_test.h"

typedef enum
{
  IDC_PACKET_CONFIG = 1,
  IDC_J_STATE,    
  IDC_K_STATE,
  IDC_JK_ACK,
  IDC_BUS_SUSPEND,
  IDC_BUS_RESUME,
  IDC_BUS_RESET,  
}usb_test_id_t;

#ifdef USB_TEST
extern void usb_test_pck_cfg();
extern void usb_test_J_state();
extern void usb_test_K_state();
extern void usb_test_JK_SE0_ACK();
extern void usb_test_suspend();
extern void usb_test_resume();
extern void usb_test_reset();
#endif

static u16 usb_test_cont_keymap(u16 key);
static u16 usb_test_text_keymap(u16 key);

static RET_CODE usb_test_text_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_usb_test(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[USB_TEST_ITEM_CNT];
  u8 i;  u16 y;
  u16 str_id[USB_TEST_ITEM_CNT] = 
  {
    IDS_TEST_PACKET_CONFIG,
    IDS_TEST_J_STATE,
    IDS_TEST_K_STATE,
    IDS_J_K_SEO_ACK,
    IDS_BUS_SUSPEND,
    IDS_BUS_RESUME,
    IDS_BUS_RESET,
  };

  p_cont =
    ui_comm_root_create(ROOT_ID_USB_TEST, 0, 
    COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, IM_TITLEICON_TV, IDS_USB_TEST);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, usb_test_cont_keymap);
  

  y = USB_TEST_ITEM_Y;
  for(i = 0; i < USB_TEST_ITEM_CNT; i++)
  {
    p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_PACKET_CONFIG + i),
      USB_TEST_ITEM_X, y, USB_TEST_ITEM_W, USB_TEST_ITEM_H, p_cont, 0);
    ctrl_set_keymap(p_ctrl[i], usb_test_text_keymap);
    ctrl_set_proc(p_ctrl[i], usb_test_text_proc);
    ctrl_set_rstyle(p_ctrl[i], RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_SH);
    text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL, FSI_COMM_CTRL_SH);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ctrl[i], str_id[i]);

    ctrl_set_related_id(p_ctrl[i],
                        0,                                                            /* left */
                        (u8)((i - 1 +
                              USB_TEST_ITEM_CNT) %
                             USB_TEST_ITEM_CNT + 1),                  /* up */
                        0,                                                            /* right */
                        (u8)((i + 1) % USB_TEST_ITEM_CNT + 1)); /* down */

    y += USB_TEST_ITEM_H + USB_TEST_ITEM_V_GAP;
  }

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_usb_test_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#ifdef USB_TEST
  switch(ctrl_get_ctrl_id(p_ctrl))
  {
    case IDC_PACKET_CONFIG:
      usb_test_pck_cfg();
      break;
      
    case IDC_J_STATE:
      usb_test_J_state();
      break;

    case IDC_K_STATE:
      usb_test_K_state();
      break;
      
    case IDC_JK_ACK:
      usb_test_JK_SE0_ACK();
      break;

    case IDC_BUS_SUSPEND:
      usb_test_suspend();
      break;

    case IDC_BUS_RESUME:
      usb_test_resume();
      break;

    case IDC_BUS_RESET:
      usb_test_reset();
      break;
      
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }
#endif

  return SUCCESS;
}

BEGIN_KEYMAP(usb_test_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(usb_test_cont_keymap, ui_comm_root_keymap)

BEGIN_KEYMAP(usb_test_text_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
END_KEYMAP(usb_test_text_keymap, NULL)

BEGIN_MSGPROC(usb_test_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_usb_test_select)
END_MSGPROC(usb_test_text_proc, text_class_proc)
