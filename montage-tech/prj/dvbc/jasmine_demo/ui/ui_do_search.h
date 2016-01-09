/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_DO_SEARCH_H__
#define __UI_DO_SEARCH_H__

/* coordinate */

//top line
#define DO_SEARCH_TOP_LINE_X  (DO_SEARCH_FRM_X - 30)
#define DO_SEARCH_TOP_LINE_Y  (DO_SEARCH_FRM_Y-DO_SEARCH_TOP_LINE_H)
#define DO_SEARCH_TOP_LINE_W  (DO_SEARCH_FRM_W + 60)
#define DO_SEARCH_TOP_LINE_H  30

//bottom line
#define DO_SEARCH_BOTTOM_LINE_X  DO_SEARCH_TOP_LINE_X
#define DO_SEARCH_BOTTOM_LINE_Y  (DO_SEARCH_FRM_Y+DO_SEARCH_FRM_H)
#define DO_SEARCH_BOTTOM_LINE_W  DO_SEARCH_TOP_LINE_W
#define DO_SEARCH_BOTTOM_LINE_H  18

//prog info container
#define DO_SEARCH_FRM_X                 ((COMM_BG_W - DO_SEARCH_TITLE_CNT * DO_SEARCH_TITLE_BG_W -\
                                        (DO_SEARCH_TITLE_CNT - 1) * DO_SEARCH_TITLE_AND_TITLE_HGAP)/2) //100
#define DO_SEARCH_FRM_Y                 COMM_ITEM_OY_IN_ROOT//160
#define DO_SEARCH_FRM_W                 (DO_SEARCH_TITLE_BG_W * DO_SEARCH_TITLE_CNT + (DO_SEARCH_TITLE_CNT - 1) * DO_SEARCH_TITLE_AND_TITLE_HGAP)
#define DO_SEARCH_FRM_H                 (DO_SEARCH_LIST_H + DO_SEARCH_TITLE_BG_H + DO_SEARCH_TITLE_AND_LIST_VGAP)

//tv radio title
#define DO_SEARCH_TITLE_CNT              2
#define DO_SEARCH_TITLE_AND_TITLE_HGAP   40

#define DO_SEARCH_TITLE_BG_X             0 // 10
#define DO_SEARCH_TITLE_BG_Y             10
#define DO_SEARCH_TITLE_BG_W            360// 480
#define DO_SEARCH_TITLE_BG_H             COMM_ITEM_H

#define DO_SEARCH_TITLE_TXT_W            200
#define DO_SEARCH_TITLE_TXT_H            DO_SEARCH_TITLE_BG_H

#define DO_SEARCH_TITLE_NUM_W            70
#define DO_SEARCH_TITLE_NUM_H            DO_SEARCH_TITLE_BG_H

#define DO_SEARCH_TITLE_AND_LIST_VGAP     20
//tv radio  
#define DO_SEARCH_TVRADIO_X             (DO_SEARCH_LIST_W + 5)//DO_SEARCH_FRM_W/2 - 40
#define DO_SEARCH_TVRADIO_Y             85
#define DO_SEARCH_TVRADIO_W             70
#define DO_SEARCH_TVRADIO_H             110 
//search list 
#define DO_SEARCH_LIST_CNT              2
#define DO_SEARCH_LIST_PAGE              6//8

#define DO_SEARCH_LIST_FIELD_CNT        2
#define DO_SEARCH_LIST_ITEM_H           36
#define DO_SEARCH_LIST_ITEM_V_GAP       0

#define DO_SEARCH_LIST_X                DO_SEARCH_TITLE_BG_X
#define DO_SEARCH_LIST_Y                (DO_SEARCH_TITLE_BG_H + DO_SEARCH_TITLE_AND_LIST_VGAP)
#define DO_SEARCH_LIST_W                DO_SEARCH_TITLE_BG_W
#define DO_SEARCH_LIST_H                ((DO_SEARCH_LIST_ITEM_H +      \
                                          DO_SEARCH_LIST_ITEM_V_GAP) * \
                                          DO_SEARCH_LIST_PAGE)  //216

//tp list
#define DO_SEARCH_TP_LIST_FRM_X         DO_SEARCH_FRM_X
#define DO_SEARCH_TP_LIST_FRM_Y         (DO_SEARCH_FRM_H+DO_SEARCH_FRM_Y+5)  //346
#define DO_SEARCH_TP_LIST_FRM_W         DO_SEARCH_FRM_W
#define DO_SEARCH_TP_LIST_FRM_H         DO_SEARCH_TP_LIST_H   //144

#define DO_SEARCH_TP_LIST_PAGE           3// 4
#define DO_SEARCH_TP_LIST_FIELD_CNT     2
#define DO_SEARCH_TP_LIST_ITEM_H        36
#define DO_SEARCH_TP_LIST_ITEM_V_GAP    0

#define DO_SEARCH_TP_LIST_X             0
#define DO_SEARCH_TP_LIST_Y             0
#define DO_SEARCH_TP_LIST_W             400//500
#define DO_SEARCH_TP_LIST_H             ((DO_SEARCH_TP_LIST_ITEM_H +      \
                                          DO_SEARCH_TP_LIST_ITEM_V_GAP) * \
                                         DO_SEARCH_TP_LIST_PAGE)

#define DO_SEARCH_TP_LIST_NO_W          75
#define DO_SEARCH_TP_LIST_INFO_W        (DO_SEARCH_TP_LIST_W - DO_SEARCH_TP_LIST_NO_W)

#define DO_SEARCH_PBAR_X                DO_SEARCH_FRM_X
#define DO_SEARCH_PBAR_Y                (DO_SEARCH_TP_LIST_FRM_Y + DO_SEARCH_TP_LIST_PAGE * \
                                                              (DO_SEARCH_TP_LIST_ITEM_H + DO_SEARCH_TP_LIST_ITEM_V_GAP))
#define DO_SEARCH_PBAR_W                (DO_SEARCH_FRM_W - \
                                         DO_SEARCH_PBAR_PERCENT_W)
#define DO_SEARCH_PBAR_H                16

#define DO_SEARCH_PBAR_PERCENT_X        (DO_SEARCH_PBAR_X + DO_SEARCH_PBAR_W)
#define DO_SEARCH_PBAR_PERCENT_Y        (DO_SEARCH_PBAR_Y -          \
                                         (DO_SEARCH_PBAR_PERCENT_H - \
                                          DO_SEARCH_PBAR_H) / 2)
#define DO_SEARCH_PBAR_PERCENT_W        100
#define DO_SEARCH_PBAR_PERCENT_H        30

/* rect style */
#define RSI_DO_SEARCH_FRM               RSI_PBACK

#define RSI_DO_SEARCH_LIST              RSI_PBACK//RSI_COMMAN_MID_BG
#define RSI_DO_SEARCH_ITEM              RSI_IGNORE
#define RSI_DO_SEARCH_PBAR_BG           RSI_PROGRESS_BAR_BG
#define RSI_DO_SEARCH_PBAR_MID          RSI_PROGRESS_BAR_MID_ORANGE

/* font style */
#define FSI_DO_SEARCH_LIST_ITEM         FSI_WHITE

/* others */

RET_CODE open_do_search(u32 para1, u32 para2);

BOOL do_search_is_finish(void);
BOOL do_search_is_stop(void);
void ui_pre_nit_search_set();


#endif


