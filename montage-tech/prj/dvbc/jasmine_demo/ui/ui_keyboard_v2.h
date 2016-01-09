/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_KEYBOARD_V2_H__
#define __UI_KEYBOARD_V2_H__
//coordinate

#define KEYBOARD_V2_MENU_X  60
#define KEYBOARD_V2_MENU_Y  150
#define KEYBOARD_V2_MENU_W  460
#define KEYBOARD_V2_MENU_H  420

#define NAME_EDIT_X 7
#define NAME_EDIT_Y 20
#define NAME_EDIT_W (KEYBOARD_V2_MENU_W - 2*NAME_EDIT_X)
#define NAME_EDIT_H 36

#define KEYBOARD_CONT_X 4
#define KEYBOARD_CONT_Y 64
#define KEYBOARD_CONT_W (KEYBOARD_V2_MENU_W - KEYBOARD_CONT_X*2)
#define KEYBOARD_CONT_H (KEYBOARD_V2_MENU_H - KEYBOARD_CONT_Y - 30)

#define LABEL_CONT_X 0
#define LABEL_CONT_Y 0
#define LABEL_CONT_W KEYBOARD_CONT_W
#define LABEL_CONT_H 35 

#define IPM_COM_Y 5
#define IPM_COM_W 85
#define IPM_COM_H (LABEL_CONT_H - IPM_COM_Y)

#define KEYBOARD_MBOX_V2_X 60
#define KEYBOARD_MBOX_V2_Y 50
#define KEYBOARD_MBOX_V2_W 328
#define KEYBOARD_MBOX_V2_H 216
#define KEYBOARD_MBOX_V2_COUNT 49
#define KEYBOARD_MBOX_V2_ROW 6
#define KEYBOARD_MBOX_V2_COL 9
#define KEYBOARD_MBOX_V2_HGAP 5
#define KEYBOARD_MBOX_V2_VGAP 5

#define KEYBOARD_HELP_X 58
#define KEYBOARD_HELP_Y 330
#define KEYBOARD_HELP_W (KEYBOARD_V2_MENU_W - KEYBOARD_HELP_X*2)
#define KEYBOARD_HELP_H 40
typedef enum 
{
  KB_INPUT_TYPE_NONE = 0,
  KB_INPUT_TYPE_SENTENCE = 0x1,
  KB_INPUT_TYPE_DEC_NUMERIC = (0x1 << 1),
  KB_INPUT_TYPE_HEX_NUMERIC = (0x1 << 2),
  KB_INPUT_TYPE_ENGLISH = (0x1 << 3),
  KB_INPUT_TYPE_MAX
} kb_input_type_t;

typedef  RET_CODE (*OBJ_KEYBOARD_CB_V2)(u16 *p_unistr);

typedef  RET_CODE (*edit_save_cb)(u16 *p_unistr);

typedef struct
{
  u16 *         uni_str;
  u16           max_len;
  u16            type;
  OBJ_KEYBOARD_CB_V2 cb;
} kb_param_t;

RET_CODE open_keyboard_v2(u32 para1, u32 para2);

#define CODING 
#endif


