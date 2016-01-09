/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_MAINMENU_H__
#define __UI_MAINMENU_H__

/* coordinate */
#define MAINMENU_CONT_X           0
#define MAINMENU_CONT_Y           0
#define MAINMENU_CONT_W           UI_COMMON_WIDTH
#define MAINMENU_CONT_H           UI_COMMON_HEIGHT

#ifdef ENABLE_NETWORK
#define MAINMENU_MBOX_TITLE_X           70
#define MAINMENU_MBOX_TITLE_Y           0
#define MAINMENU_MBOX_TITLE_W          820//1000
#define MAINMENU_MBOX_TITLE_H           120
#else
#define MAINMENU_MBOX_TITLE_X           120
#define MAINMENU_MBOX_TITLE_Y           0
#define MAINMENU_MBOX_TITLE_W          720//1000
#define MAINMENU_MBOX_TITLE_H           120
#endif

#define MAINMENU_TITLE_ICON_X    0
#define MAINMENU_TITLE_ICON_Y    65 
#define MAINMENU_TITLE_ICON_W   UI_COMMON_WIDTH
#define MAINMENU_TITLE_ICON_H    90

#define MAINMENU_TITLE_X       480
#define MAINMENU_TITLE_Y       160 
#define MAINMENU_TITLE_W      400
#define MAINMENU_TITLE_H       60

#define MAINMENU_PREV_X           70
#define MAINMENU_PREV_Y           200
#define MAINMENU_PREV_W          300// 600
#define MAINMENU_PREV_H           400

#define MAINMENU_CONT_ITEM_X      MAINMENU_TITLE_X//(MAINMENU_PREV_X + MAINMENU_PREV_W + 30)
#define MAINMENU_CONT_ITEM_Y      (MAINMENU_TITLE_Y + MAINMENU_TITLE_H + 10) //MAINMENU_PREV_Y
#define MAINMENU_CONT_ITEM_W      MAINMENU_TITLE_W//330
#define MAINMENU_CONT_ITEM_H      330//MAINMENU_PREV_H

#define MAINMENU_ARROW_UP_X           ((MAINMENU_CONT_ITEM_W - MAINMENU_ARROW_UP_W)/2)
#define MAINMENU_ARROW_UP_Y           0
#define MAINMENU_ARROW_UP_W           18
#define MAINMENU_ARROW_UP_H           10

#define MAINMENU_ITEM_V_GAP       1
#define MENU_LIST_ITEM_V_GAP     1

#define MAINMENU_ITEM_X          0//MAINMENU_TITLE_X// ((MAINMENU_CONT_ITEM_W - MAINMENU_ITEM_W)/2)
#define MAINMENU_ITEM_Y          (MAINMENU_ARROW_UP_Y + MAINMENU_ARROW_UP_H + 10)// (MAINMENU_TITLE_Y + MAINMENU_TITLE_H)//36
#define MAINMENU_ITEM_W           400//310
#define MAINMENU_ITEM_H           COMM_ITEM_H 

#define MAINMENU_LIST_X          MAINMENU_ITEM_X
#define MAINMENU_LIST_Y          MAINMENU_ITEM_Y
#define MAINMENU_LIST_W         MAINMENU_ITEM_W
#define MAINMENU_LIST_H         (MENU_LIST_PAGE_NUM * (MAINMENU_ITEM_H + MAINMENU_ITEM_V_GAP) - MAINMENU_ITEM_V_GAP)  //270

#define MAINMENU_LIST_MID_L              8
#define MAINMENU_LIST_MID_T              0
#define MAINMENU_LIST_MID_W              (MAINMENU_LIST_W - 2 * MAINMENU_LIST_MID_L)
#define MAINMENU_LIST_MID_H              (MAINMENU_LIST_H - 2 * MAINMENU_LIST_MID_T)

#define MAINMENU_ITEM_V_LINE_W    MAINMENU_ITEM_W
#define MAINMENU_ITEM_V_LINE_H    MAINMENU_ITEM_V_GAP

#define MAINMENU_ARROW_DOWN_X           MAINMENU_ARROW_UP_X
#define MAINMENU_ARROW_DOWN_Y           (MAINMENU_CONT_ITEM_H -10)
#define MAINMENU_ARROW_DOWN_W           MAINMENU_ARROW_UP_W
#define MAINMENU_ARROW_DOWN_H           MAINMENU_ARROW_UP_H

#define MAINMENU_AD_WINDOW_X           90//
#define MAINMENU_AD_WINDOW_Y           200
#define MAINMENU_AD_WINDOW_W           300
#define MAINMENU_AD_WINDOW_H           330

/* rect style */
#define RSI_MAINMENU_FRM          RSI_MENU_ITEM_BG
#define RSI_MAINMENU_ITEM_SH      RSI_PBACK
#define RSI_MAINMENU_ITEM_HL      RSI_MENU_BTN_HL

/* font style */

/* others */
#define MENU_LIST_PAGE_NUM         7
#define MENU_LIST_FIELD_NUM         1
#define MAINMENU_ENTRY_CNT          7
#define INVALID_LIST_CONTEXT 0x1FFF

typedef void (*CHK_ITEM_ATTR)(control_t *p_ctrl);
// return FALSE if it need to close
typedef BOOL (*CHK_MAINMENU_DISP)(void);

typedef struct sub_menu_data
{
  u16           title_strid;                       // title string id
  u8            item_cnt;                          // item count
  u8            item_pos;                          // focus
  BOOL          chk_pwd;                           // need check password when enter
  CHK_ITEM_ATTR chk_item;                          // the function to check item attribute when painting
  CHK_MAINMENU_DISP chk_menu;                      // the fucntion to check menu display or not
  u16           item_strid[MAINMENU_ENTRY_CNT]; // string id
  u8            item_entry[MAINMENU_ENTRY_CNT]; // sub menu root id
  u16           cur_layer;                         // title string id
  struct sub_menu_data_t *p_next;
}sub_menu_data_t;

#define PWD_DLG_FOR_CHK_X ((SCREEN_WIDTH + MAINMENU_AD_WINDOW_W + MAINMENU_AD_WINDOW_X - PWDLG_W)/2) //((SCREEN_WIDTH-PWDLG_W)/2)
#define PWD_DLG_FOR_CHK_Y ((SCREEN_HEIGHT-PWDLG_H)/2 + 50)
#define MAINMENU_MBOX_CNT         5
RET_CODE open_main_menu(u32 para1, u32 para2);
void ui_set_mainmenu_open_level(u32 type);


#endif

