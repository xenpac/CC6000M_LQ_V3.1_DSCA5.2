/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_BOOK_LIST_H__
#define __UI_BOOK_LIST_H__

/* coordinate */
#define BLIST_LCONT_X        COMM_ITEM_OX_IN_ROOT
#define BLIST_LCONT_Y        (COMM_ITEM_OY_IN_ROOT - 10)
#define BLIST_LCONT_W       (COMM_BG_W - 2 * BLIST_LCONT_X)//400//502//552
#define BLIST_LCONT_H        (COMM_BG_H - BLIST_LCONT_Y - 50)

//head
#define BLIST_LIST_HEAD_X        BLIST_LIST_X
#define BLIST_LIST_HEAD_Y        8
#define BLIST_LIST_HEAD_W       (BLIST_LCONT_W - 2 * BLIST_LIST_X)
#define BLIST_LIST_HEAD_H        36

#define BLIST_LIST_X          10
#define BLIST_LIST_Y          50
#define BLIST_LIST_W          (BLIST_LCONT_W - 2 * BLIST_LIST_X)
#define BLIST_LIST_H          (BLIST_LCONT_H - BLIST_LIST_Y - 10)

#define BLIST_PAGE            10//12
#define BLIST_FIELD_NUM      6

#define BLIST_ITEM_H          36
#define BLIST_ITEM_V_GAP      0

#define BLIST_MID_X           0
#define BLIST_MID_Y           0
#define BLIST_MID_W          (BLIST_LIST_W - 10)
#define BLIST_MID_H           (BLIST_LIST_H - 2 * BLIST_MID_Y)

#define BLIST_BAR_X             (BLIST_LCONT_W - BLIST_BAR_W - 10)//490//540
#define BLIST_BAR_Y             (BLIST_LIST_Y + BLIST_MID_Y)
#define BLIST_BAR_W             6
#define BLIST_BAR_H             (BLIST_LIST_H - 2 * BLIST_MID_Y)


#define BLIST_SBAR_H_GAP      8
#define BLIST_SBAR_V_GAP      0
#define BLIST_SBAR_VERTEX_GAP         12
#define BLIST_SBAR_WIDTH      6
#define BLIST_SBAR_HEIGHT     BLIST_LIST_H

/* rect style */
#define RSI_BLIST_FRM         RSI_WINDOW_1
#define RSI_BLIST_LIST        RSI_PBACK
#define RSI_BLIST_SBAR_BG     RSI_SCROLL_BAR_BG
#define RSI_BLIST_SBAR_MID    RSI_SCROLL_BAR_MID

/* font style */
/*fstyle*/
#define FSI_BLIST_SH          FSI_WHITE       //font styel of single satellite information edit
#define FSI_BLIST_HL          FSI_BLACK

/*others*/

RET_CODE open_book_list(u32 para1, u32 para2);

#endif


