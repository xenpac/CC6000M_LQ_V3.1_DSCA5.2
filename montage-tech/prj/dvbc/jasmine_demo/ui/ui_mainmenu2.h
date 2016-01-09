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
#define MAINMENU_CONT_W           COMM_BG_W
#define MAINMENU_CONT_H           COMM_BG_H

#define MAINMENU_PREV_X           130
#define MAINMENU_PREV_Y           120
#define MAINMENU_PREV_W           480
#define MAINMENU_PREV_H           300

#define MAINMENU_AD_WINDOW_X           130
#define MAINMENU_AD_WINDOW_Y           (MAINMENU_PREV_Y+MAINMENU_PREV_H+20)
#define MAINMENU_AD_WINDOW_W           480
#define MAINMENU_AD_WINDOW_H           150

#define MAINMENU_CONT_ITEM_X      COMM_BG_W/2 + 30
#define MAINMENU_CONT_ITEM_Y      120
#define MAINMENU_CONT_ITEM_W      454
#define MAINMENU_CONT_ITEM_H      502

#define MAINMENU_ICON_X           0
#define MAINMENU_ICON_Y           0
#define MAINMENU_ICON_W           54
#define MAINMENU_ICON_H           54

#define MAINMENU_ITEM_X           (MAINMENU_ICON_X+MAINMENU_ICON_W)
#define MAINMENU_ITEM_Y           MAINMENU_ICON_Y
#define MAINMENU_ITEM_W           400
#define MAINMENU_ITEM_H           54

#define MAINMENU_ITEM_V_GAP       10

#define MAINMENU_DEL_ALL_X           (MAINMENU_CONT_X+(MAINMENU_CONT_W-MAINMENU_DEL_ALL_W)/2)
#define MAINMENU_DEL_ALL_Y           COMM_DLG_Y
#define MAINMENU_DEL_ALL_W           COMM_DLG_W
#define MAINMENU_DEL_ALL_H           COMM_DLG_H

#define MAINMENU_RESTORE_X           (MAINMENU_CONT_X+(MAINMENU_CONT_W-MAINMENU_RESTORE_W)/2)
#define MAINMENU_RESTORE_Y           (MAINMENU_CONT_Y+(MAINMENU_CONT_H+TITLE_H-MAINMENU_RESTORE_H)/2)
#define MAINMENU_RESTORE_W           300
#define MAINMENU_RESTORE_H           200

/* rect style */
#define RSI_MAINMENU_FRM          RSI_IGNORE
#define RSI_MAINMENU_ITEM_SH      RSI_MENU_BTN_SH
#define RSI_MAINMENU_ITEM_HL      RSI_MENU_BTN_HL

/* font style */

/* others */
#define MAINMENU_ENTRY_CNT          7
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

#define PWD_DLG_FOR_CHK_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define PWD_DLG_FOR_CHK_Y      ((SCREEN_HEIGHT-PWDLG_H)/2)
#define MAINMENU_MBOX_CNT         5
RET_CODE open_main_menu(u32 para1, u32 para2);

#endif

