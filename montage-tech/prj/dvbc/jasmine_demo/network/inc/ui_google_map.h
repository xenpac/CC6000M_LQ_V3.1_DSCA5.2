/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_GOOGLE_MAP_H__
#define __UI_GOOGLE_MAP_H__

//google map  area cont
#define MAP_AREA_CONT_X         30
#define MAP_AREA_CONT_Y         80
#define MAP_AREA_CONT_W         260
#define MAP_AREA_CONT_H         560

//map left item control
#define SELECT_ITEM_X 15
#define SELECT_ITEM_LW 20
#define SELECT_ITEM_RW 215
//google map detail container
#define MAP_DETAIL_CONT_X      MAP_AREA_CONT_X+MAP_AREA_CONT_W+10  
#define MAP_DETAIL_CONT_Y       MAP_AREA_CONT_Y 
#define MAP_DETAIL_CONT_W        SCREEN_WIDTH-MAP_AREA_CONT_W-20-50
#define MAP_DETAIL_CONT_H        560


#define MAP_DIRECT_X  MAP_DETAIL_CONT_X + 20
#define MAP_DIRECT_Y  MAP_DETAIL_CONT_Y + 20
#define MAP_DIRECT_W  58
#define MAP_DIRECT_H  58


#define MAP_SBAR_X   MAP_DETAIL_CONT_X + 40
#define MAP_SBAR_Y   MAP_DETAIL_CONT_Y + MAP_DIRECT_H + 40
#define MAP_SBAR_W   16
#define MAP_SBAR_H   150

//bottom help container
#define MAP_BOTTOM_HELP_X  MAP_AREA_CONT_X
#define MAP_BOTTOM_HELP_Y  MAP_DETAIL_CONT_Y + MAP_DETAIL_CONT_H + 10
#define MAP_BOTTOM_HELP_W  MAP_DETAIL_CONT_W + MAP_AREA_CONT_W
#define MAP_BOTTOM_HELP_H  50

/* rect style */
#define RSI_MAP_SBAR_BG     RSI_MAP_SCROLL_BAR_BG//RSI_SCROLL_BAR_BG//
#define RSI_MAP_SBAR_MID    RSI_MAP_SCROLL_BAR_MID//RSI_SCROLL_BAR_MID//

//Other
#define MAP_AREA_CNT 4
#define MAP_LEFT_ITEM_CNT 7


//city  area cont
#define MAP_AREA_LIST_X         45
#define MAP_AREA_LIST_Y         100
#define MAP_AREA_LIST_W         200
#define MAP_AREA_LIST_H         510

#define MAP_AREA_LIST_ITEM_VGAP 7
#define MAP_AREA_LIST_MID_L     9
#define MAP_AREA_LIST_MID_T     10
#define MAP_AREA_LIST_MID_W    220
#define MAP_AREA_LIST_MID_H    ((COMM_ITEM_H + MAP_AREA_LIST_ITEM_VGAP) * AREA_LIST_PAGE - MAP_AREA_LIST_ITEM_VGAP)



RET_CODE open_google_map(u32 para1, u32 para2);


#endif
