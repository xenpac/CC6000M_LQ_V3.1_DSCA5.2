/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_USB_PICTURE_H_
#define _UI_USB_PICTURE_H_

/*coordinates*/

//preview
#define PIC_PREV_X  PREV_COMMON_X
#define PIC_PREV_Y  PREV_COMMON_Y
#define PIC_PREV_W  PREV_COMMON_W
#define PIC_PREV_H  PREV_COMMON_H

#define PIC_MSG_X  (PIC_PREV_X + (PIC_PREV_W -  PIC_MSG_W) / 2)
#define PIC_MSG_Y  (PIC_PREV_Y + (PIC_PREV_H -  PIC_MSG_H) / 2)
#define PIC_MSG_W  250 //450
#define PIC_MSG_H  80

//group
#define PIC_GROUP_X  (PIC_PREV_X + PIC_PREV_W + 20)
#define PIC_GROUP_Y  PIC_PREV_Y
#define PIC_GROUP_W  (COMM_BG_W - PIC_GROUP_X - WIN_LR_SPACE)  //470
#define PIC_GROUP_H  COMM_ITEM_H

//list scroll bar
#define PIC_LIST_SBARX  (PIC_LISTX + PIC_LISTW + 4)
#define PIC_LIST_SBARY  (PIC_LISTY)
#define PIC_LIST_SBARW  6
#define PIC_LIST_SBARH  PIC_LISTH

#define PIC_LIST_SBAR_MIDL  0
#define PIC_LIST_SBAR_MIDT  12
#define PIC_LIST_SBAR_MIDR  6
#define PIC_LIST_SBAR_MIDB  (PIC_LIST_SBARH - PIC_LIST_SBAR_MIDT)

//list
#define PIC_LISTX  PIC_GROUP_X
#define PIC_LISTY  (PIC_GROUP_Y + PIC_GROUP_H + 10)
#define PIC_LISTW  (PIC_GROUP_W - 10)
#define PIC_LISTH  330//420

#define PIC_LIST_MIDL  8
#define PIC_LIST_MIDT  8
#define PIC_LIST_MIDW  (PIC_LISTW - 2 * PIC_LIST_MIDL)
#define PIC_LIST_MIDH  (PIC_LISTH - 2 * PIC_LIST_MIDT)

#define PIC_LCONT_LIST_VGAP 4

//path picture

//path text
#define PIC_PATH_TEXT_X  PIC_LISTX
#define PIC_PATH_TEXT_Y  (PIC_LISTY + PIC_LISTH + 6)
#define PIC_PATH_TEXT_W  PIC_LISTW
#define PIC_PATH_TEXT_H  36

//detail container
#define PIC_DETAIL_CONT_X  PIC_PREV_X
#define PIC_DETAIL_CONT_Y  (PIC_PREV_Y + PIC_PREV_H + 10)
#define PIC_DETAIL_CONT_W  MUSIC_PREV_W
#define PIC_DETAIL_CONT_H  150//140

#define PIC_DETAIL_PIXEL_X  20
#define PIC_DETAIL_PIXEL_Y  30
#define PIC_DETAIL_PIXEL_W  300
#define PIC_DETAIL_PIXEL_H  30

#define PIC_DETAIL_FILE_SIZE_X  20
#define PIC_DETAIL_FILE_SIZE_Y  (PIC_DETAIL_PIXEL_Y + PIC_DETAIL_PIXEL_H + 30)
#define PIC_DETAIL_FILE_SIZE_W  PIC_DETAIL_PIXEL_W
#define PIC_DETAIL_FILE_SIZE_H  PIC_DETAIL_PIXEL_H

//play container


//play list container
#define PIC_PLAY_LIST_CONT_X  PIC_GROUP_X
#define PIC_PLAY_LIST_CONT_Y  PIC_GROUP_Y
#define PIC_PLAY_LIST_CONT_W  PIC_GROUP_W
#define PIC_PLAY_LIST_CONT_H  460

//play list title
#define PIC_PLAY_LIST_TITLE_X  ((PIC_PLAY_LIST_CONT_W - PIC_PLAY_LIST_TITLE_W)/2)
#define PIC_PLAY_LIST_TITLE_Y  10
#define PIC_PLAY_LIST_TITLE_W  180
#define PIC_PLAY_LIST_TITLE_H  30

//play list scroll bar
#define PIC_PLAY_LIST_SBARX  (PIC_PLAY_LIST_LIST_X + PIC_PLAY_LIST_LIST_W + 4)
#define PIC_PLAY_LIST_SBARY  (PIC_PLAY_LIST_LIST_Y)
#define PIC_PLAY_LIST_SBARW  6
#define PIC_PLAY_LIST_SBARH  PIC_PLAY_LIST_LIST_H

#define PIC_PLAY_LIST_SBAR_MIDL  0
#define PIC_PLAY_LIST_SBAR_MIDT  12
#define PIC_PLAY_LIST_SBAR_MIDR  6
#define PIC_PLAY_LIST_SBAR_MIDB  (PIC_PLAY_LIST_SBARH - PIC_PLAY_LIST_SBAR_MIDT)

//play list
#define PIC_PLAY_LIST_LIST_X  12
#define PIC_PLAY_LIST_LIST_Y  (PIC_PLAY_LIST_TITLE_Y + PIC_PLAY_LIST_TITLE_H + 10)
#define PIC_PLAY_LIST_LIST_W  (PIC_PLAY_LIST_CONT_W - PIC_PLAY_LIST_LIST_X*2)
#define PIC_PLAY_LIST_LIST_H  336

#define PIC_PLAY_LIST_LIST_MIDL  8
#define PIC_PLAY_LIST_LIST_MIDT  8
#define PIC_PLAY_LIST_LIST_MIDW  (PIC_PLAY_LIST_LIST_W - 2 * PIC_PLAY_LIST_LIST_MIDL)
#define PIC_PLAY_LIST_LIST_MIDH  (PIC_PLAY_LIST_LIST_H - 2 * PIC_PLAY_LIST_LIST_MIDT)

#define PIC_PLAY_LIST_LIST_VGAP 4

//play list help
#define PIC_PLAY_LIST_HELP_X  15
#define PIC_PLAY_LIST_HELP_Y  (PIC_PLAY_LIST_CONT_H - PIC_PLAY_LIST_HELP_H)
#define PIC_PLAY_LIST_HELP_W  (PIC_PLAY_LIST_CONT_W - PIC_PLAY_LIST_HELP_X*2)
#define PIC_PLAY_LIST_HELP_H  70

//sort list
#define PIC_SORT_LIST_X  PIC_LISTX
#define PIC_SORT_LIST_Y  PIC_LISTY
#define PIC_SORT_LIST_W  250
#define PIC_SORT_LIST_H  130

#define PIC_SORT_LIST_MIDL       10
#define PIC_SORT_LIST_MIDT       10
#define PIC_SORT_LIST_MIDW       (PIC_SORT_LIST_W - 2*PIC_SORT_LIST_MIDL)
#define PIC_SORT_LIST_MIDH       (PIC_SORT_LIST_H - 2*PIC_SORT_LIST_MIDT)

#define PIC_SORT_LIST_VGAP       0

//save or no
#define PIC_DLG_W  300
#define PIC_DLG_H  160
#define PIC_DLG_L  (SCREEN_POS_X + PIC_PREV_X + (PIC_PREV_W - PIC_DLG_W)/2)
#define PIC_DLG_R  (PIC_DLG_L + PIC_DLG_W)
#define PIC_DLG_T  (SCREEN_POS_Y + PIC_PREV_Y + (PIC_PREV_H - PIC_DLG_H)/2) 
#define PIC_DLG_B  (PIC_DLG_T + PIC_DLG_H)


/*rstyle*/
#define RSI_PIC_PREV  RSI_TV
#define RSI_PIC_DETAIL  RSI_COMMON_RECT1
#define RSI_PIC_TITLE RSI_PBACK
#define RSI_PIC_SBAR RSI_SCROLL_BAR_BG
#define RSI_PIC_SBAR_MID RSI_SCROLL_BAR_MID
#define RSI_PIC_LCONT_LIST  RSI_COMMON_RECT1

/*others*/
#define PIC_LIST_PAGE  8//10
#define PIC_LIST_FIELD 4

#define PIC_PLAY_LIST_LIST_PAGE 8
#define PIC_PLAY_LIST_LIST_FIELD 4

#define PIC_PLAY_MBOX_TOL  4
#define PIC_PLAY_MBOX_COL  4
#define PIC_PLAY_MBOX_ROW  1
#define PIC_PLAY_MBOX_HGAP  0
#define PIC_PLAY_MBOX_VGAP  0

//
#define PIC_PLAY_LIST_HELP_MBOX_TOL  3
#define PIC_PLAY_LIST_HELP_MBOX_COL  2
#define PIC_PLAY_LIST_HELP_MBOX_ROW  2
#define PIC_PLAY_LIST_HELP_MBOX_HGAP  0
#define PIC_PLAY_LIST_HELP_MBOX_VGAP  0

//
#define PIC_HELP_MBOX_TOL  5
#define PIC_HELP_MBOX_COL  5
#define PIC_HELP_MBOX_ROW  1
#define PIC_HELP_MBOX_HGAP  0
#define PIC_HELP_MBOX_VGAP  0

//sort list
#define PIC_SORT_LIST_ITEM_TOL     3
#define PIC_SORT_LIST_ITEM_PAGE    PIC_SORT_LIST_ITEM_TOL
#define PIC_SORT_LIST_ITEM_HEIGHT  36
#define PIC_SORT_LIST_FIELD_NUM    1


#define PIC_TIME_PBAR_MIN 0
#define PIC_TIME_PBAR_MAX 66

#define PIC_TIME_TOT_HOURS 2

/*fstyle*/
#define FSI_PIC_LCONT_MBOX FSI_WHITE
#define FSI_PIC_INFO_MBOX FSI_WHITE

RET_CODE open_usb_picture(u32 para1, u32 para2);

media_fav_t *ui_pic_get_fav_media_by_index(u16 index);
#endif
