/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_USB_MUSIC_H_
#define _UI_USB_MUSIC_H_

#include "ui_common.h"

/*coordinates*/

//preview
#define MUSIC_PREV_X  PREV_COMMON_X
#define MUSIC_PREV_Y  PREV_COMMON_Y
#define MUSIC_PREV_W  PREV_COMMON_W
#define MUSIC_PREV_H  PREV_COMMON_H

//preview logo
#define MUSIC_PREV_LOGO_X  20
#define MUSIC_PREV_LOGO_Y  70
#define MUSIC_PREV_LOGO_W ( MUSIC_PREV_W- 2 *MUSIC_PREV_LOGO_X)
#define MUSIC_PREV_LOGO_H  80//150

//group
#define MP_GROUP_X  (MUSIC_PREV_X + MUSIC_PREV_W + 20)  //420
#define MP_GROUP_Y  MUSIC_PREV_Y
#define MP_GROUP_W  (COMM_BG_W - MP_GROUP_X - WIN_LR_SPACE) //470
#define MP_GROUP_H  45

//list scroll bar
#define MUSIC_LIST_SBAR_X  (MUSIC_LIST_X + MP_LISTW + 4)
#define MUSIC_LIST_SBAR_Y  (MUSIC_LIST_Y)
#define MP_LIST_SBARW  6
#define MP_LIST_SBARH  MP_LISTH

#define MP_LIST_SBAR_MIDL  0
#define MP_LIST_SBAR_MIDT  12
#define MP_LIST_SBAR_MIDR  6
#define MP_LIST_SBAR_MIDB  (MP_LIST_SBARH - MP_LIST_SBAR_MIDT)

//list
#define MUSIC_LIST_X  MP_GROUP_X
#define MUSIC_LIST_Y  (MP_GROUP_Y + MP_GROUP_H + 10) //166
#define MP_LISTW      (MP_GROUP_W - 10) //460
#define MP_LISTH      325//420

#define MP_LIST_MIDL  8
#define MP_LIST_MIDT  8
#define MP_LIST_MIDW  (MP_LISTW - 2 * MP_LIST_MIDL)  //444
#define MP_LIST_MIDH  (MP_LISTH - 2 * MP_LIST_MIDT)  //404

#define MP_LCONT_LIST_VGAP 4

//bottom infor
#define MUSIC_BOTTOM_INFO_X  MUSIC_LIST_X  //420
#define MUSIC_BOTTOM_INFO_Y  (MUSIC_LIST_Y + MP_LISTH + 6)
#define MUSIC_BOTTOM_INFO_W  MP_LISTW
#define MUSIC_BOTTOM_INFO_H  36

//play container
#define MP_PLAY_CONT_X   MUSIC_PREV_X  
#define MP_PLAY_CONT_Y   (MUSIC_PREV_Y + MUSIC_PREV_H + 10)  //350
#define MP_PLAY_CONT_W   MUSIC_PREV_W  //330
#define MP_PLAY_CONT_H   150//140

#define MP_DETAIL_FILENAME_X  18
#define MP_DETAIL_FILENAME_Y  10
#define MP_DETAIL_FILENAME_W  (MP_PLAY_CONT_W - 2 * MP_DETAIL_FILENAME_X)//400
#define MP_DETAIL_FILENAME_H  30

#define MP_PLAY_PROGRESS_X  18
#define MP_PLAY_PROGRESS_Y  (MP_DETAIL_FILENAME_Y + MP_DETAIL_FILENAME_H + 10)
#define MP_PLAY_PROGRESS_W  (MP_PLAY_CONT_W - 2 * MP_DETAIL_FILENAME_X - 20)//400
#define MP_PLAY_PROGRESS_H  14
#define MP_PLAY_PROGRESS_MIDX 0
#define MP_PLAY_PROGRESS_MIDY 0
#define MP_PLAY_PROGRESS_MIDW MP_PLAY_PROGRESS_W
#define MP_PLAY_PROGRESS_MIDH MP_PLAY_PROGRESS_H
#define MP_PLAY_PROGRESS_MIN RSI_PLAY_PBAR_FRONT
#define MP_PLAY_PROGRESS_MAX RSI_PLAY_PBAR_BACK
#define MP_PLAY_PROGRESS_MID INVALID_RSTYLE_IDX

#define MP_PLAY_MODE_X  (MP_PLAY_PROGRESS_X + MP_PLAY_PROGRESS_W + 5)//420
#define MP_PLAY_MODE_Y  (MP_PLAY_PROGRESS_Y - 7)
#define MP_PLAY_MODE_W  28
#define MP_PLAY_MODE_H  28

#define MP_PLAY_CURTM_X  18
#define MP_PLAY_CURTM_Y  (MP_PLAY_PROGRESS_Y + MP_PLAY_PROGRESS_H + 5)//(MP_PLAY_CONT_H - MP_PLAY_CURTM_H - 10)
#define MP_PLAY_CURTM_W  60//120
#define MP_PLAY_CURTM_H  30

#define MP_PLAY_TOLTM_X  240//350
#define MP_PLAY_TOLTM_Y  MP_PLAY_CURTM_Y
#define MP_PLAY_TOLTM_W  MP_PLAY_CURTM_W
#define MP_PLAY_TOLTM_H  MP_PLAY_CURTM_H

#define MP_PLAY_CTRL_ICON_X  60//100
#define MP_PLAY_CTRL_ICON_Y  (MP_PLAY_TOLTM_Y + MP_PLAY_TOLTM_H + 5)//40
#define MP_PLAY_CTRL_ICON_W  220//270
#define MP_PLAY_CTRL_ICON_H  28

//play list container
#define MP_PLAY_LIST_CONT_X  MP_GROUP_X
#define MP_PLAY_LIST_CONT_Y  MP_GROUP_Y
#define MP_PLAY_LIST_CONT_W  MP_GROUP_W
#define MP_PLAY_LIST_CONT_H  380//480

//play list title
#define MP_PLAY_LIST_TITLE_X  ((MP_PLAY_LIST_CONT_W - MP_PLAY_LIST_TITLE_W)/2)
#define MP_PLAY_LIST_TITLE_Y  10
#define MP_PLAY_LIST_TITLE_W  180
#define MP_PLAY_LIST_TITLE_H  30

//play list scroll bar
#define MP_PLAY_LIST_SBARX  (MP_PLAY_LIST_LIST_X + MP_PLAY_LIST_LIST_W + 4)
#define MP_PLAY_LIST_SBARY  (MP_PLAY_LIST_LIST_Y)
#define MP_PLAY_LIST_SBARW  6
#define MP_PLAY_LIST_SBARH  MP_PLAY_LIST_LIST_H

#define MP_PLAY_LIST_SBAR_MIDL  0
#define MP_PLAY_LIST_SBAR_MIDT  12
#define MP_PLAY_LIST_SBAR_MIDR  6
#define MP_PLAY_LIST_SBAR_MIDB  (MP_PLAY_LIST_SBARH - MP_PLAY_LIST_SBAR_MIDT)

//play list
#define MP_PLAY_LIST_LIST_X  14
#define MP_PLAY_LIST_LIST_Y  (MP_PLAY_LIST_TITLE_Y + MP_PLAY_LIST_TITLE_H + 10)
#define MP_PLAY_LIST_LIST_W  (MP_PLAY_LIST_CONT_W - MP_PLAY_LIST_LIST_X*2)  //446
#define MP_PLAY_LIST_LIST_H  280

#define MP_PLAY_LIST_LIST_MIDL  8
#define MP_PLAY_LIST_LIST_MIDT  8
#define MP_PLAY_LIST_LIST_MIDW  (MP_PLAY_LIST_LIST_W - 2 * MP_PLAY_LIST_LIST_MIDL)
#define MP_PLAY_LIST_LIST_MIDH  (MP_PLAY_LIST_LIST_H - 2 * MP_PLAY_LIST_LIST_MIDT)

#define MP_PLAY_LIST_LIST_VGAP 4

//play list help
#define MP_PLAY_LIST_HELP_X  20
#define MP_PLAY_LIST_HELP_Y  (MP_PLAY_LIST_LIST_Y + MP_PLAY_LIST_LIST_H)
#define MP_PLAY_LIST_HELP_W  (MP_PLAY_LIST_CONT_W - MP_PLAY_LIST_HELP_X*2)
#define MP_PLAY_LIST_HELP_H  40

//sort list
#define MUSIC_SORT_LIST_X  MUSIC_LIST_X
#define MUSIC_SORT_LIST_Y  MUSIC_LIST_Y
#define MUSIC_SORT_LIST_W  250
#define MUSIC_SORT_LIST_H  130

#define MUSIC_SORT_LIST_MIDL       10
#define MUSIC_SORT_LIST_MIDT       10
#define MUSIC_SORT_LIST_MIDW       (MUSIC_SORT_LIST_W - 2*MUSIC_SORT_LIST_MIDL)
#define MUSIC_SORT_LIST_MIDH       (MUSIC_SORT_LIST_H - 2*MUSIC_SORT_LIST_MIDT)

#define MP_SORT_LIST_VGAP       0

//help
#define MP_HELP_X  100 
#define MP_HELP_Y  505
#define MP_HELP_W  800
#define MP_HELP_H  28

//save or no
#define MP_DLG_W  300
#define MP_DLG_H  160
#define MP_DLG_L  (SCREEN_POS_X + MUSIC_PREV_X + (MUSIC_PREV_W - MP_DLG_W)/ 2)
#define MP_DLG_R  (MP_DLG_L + MP_DLG_W)
#define MP_DLG_T  (SCREEN_POS_Y + MUSIC_PREV_Y + (MUSIC_PREV_H - MP_DLG_H) /2) 
#define MP_DLG_B  (MP_DLG_T + MP_DLG_H)


/*rstyle*/
#define RSI_MP_PREV  RSI_TV
#define RSI_MP_DETAIL  RSI_COMMON_RECT1
#define RSI_MP_TITLE RSI_PBACK
#define RSI_MP_SBAR RSI_SCROLL_BAR_BG
#define RSI_MP_SBAR_MID RSI_SCROLL_BAR_MID
#define RSI_MP_LCONT_LIST  RSI_COMMON_RECT1

/*others*/
#define MUSIC_LIST_ITEM_NUM_ONE_PAGE  7//8//10
#define MP_LIST_FIELD 4

#define MUSIC_FAV_LIST_MAX_ITEM_NUME 6//8
#define MP_PLAY_LIST_LIST_FIELD 4

#define MP_PLAY_MBOX_TOL  4
#define MP_PLAY_MBOX_COL  4
#define MP_PLAY_MBOX_ROW  1
#define MP_PLAY_MBOX_HGAP  0
#define MP_PLAY_MBOX_VGAP  0

//
#define MP_PLAY_LIST_HELP_MBOX_TOL  3
#define MP_PLAY_LIST_HELP_MBOX_COL  3
#define MP_PLAY_LIST_HELP_MBOX_ROW  1
#define MP_PLAY_LIST_HELP_MBOX_HGAP  0
#define MP_PLAY_LIST_HELP_MBOX_VGAP  0

//
#define MP_HELP_MBOX_TOL  5
#define MP_HELP_MBOX_COL  5
#define MP_HELP_MBOX_ROW  1
#define MP_HELP_MBOX_HGAP  0
#define MP_HELP_MBOX_VGAP  0

//sort list
#define MUSIC_SORT_LIST_ITEM_TOL     3
#define MUSIC_SORT_LIST_ITEM_PAGE    MUSIC_SORT_LIST_ITEM_TOL
#define MUSIC_SORT_LIST_ITEM_HEIGHT  20
#define MP_SORT_LIST_FIELD_NUM    1


#define MP_TIME_PBAR_MIN 0
#define MP_TIME_PBAR_MAX 66

#define MP_TIME_TOT_HOURS 2

/*fstyle*/
#define FSI_MP_LCONT_MBOX FSI_WHITE
#define FSI_MP_INFO_MBOX FSI_WHITE

/////lrc
#define LRC_MULTI_LEFT   480
#define LRC_MULTI_TOP    100
#define LRC_SING_LEFT    480
#define LRC_SING_TOP     330
#define LRC_SING_HEIGHT   32
#define LYC_WIDTH        400
#define LYC_HEIGHT       280

#define LRC_SHOW_CNT     8


RET_CODE open_usb_music(u32 para1, u32 para2);

void ui_usb_music_exit();

void ui_music_reset_curn(u16 *p_path);
void music_reset_time_info(u16 *name);

#endif
