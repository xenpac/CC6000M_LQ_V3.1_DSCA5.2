/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_EPG_H_
#define _UI_EPG_H_

#include "ui_common.h"

//preview
#define EPG_PREV_X  PREV_COMMON_X
#define EPG_PREV_Y  PREV_COMMON_Y
#define EPG_PREV_W  PREV_COMMON_W
#define EPG_PREV_H  PREV_COMMON_H

//short event cont
#define EPG_DETAIL_CONT_X  (EPG_PREV_X + 20)
#define EPG_DETAIL_CONT_Y  (EPG_PREV_Y + 60)
#define EPG_DETAIL_CONT_W  (EPG_PREV_W - 40)
#define EPG_DETAIL_CONT_H  (EPG_PREV_H - 120)

//short event
#define EPG_DETAIL_X  8
#define EPG_DETAIL_Y  8
#define EPG_DETAIL_W  (EPG_DETAIL_CONT_W - 2*EPG_DETAIL_X)
#define EPG_DETAIL_H  (EPG_DETAIL_CONT_H - 2*EPG_DETAIL_Y)

//prog
#define EPG_PG_CONT_X   90//EPG_PREV_X
#define EPG_PG_CONT_Y   140//(EPG_PREV_Y + EPG_PREV_H + 10)
#define EPG_PG_CONT_W   300//EPG_PREV_W
#define EPG_PG_CONT_H   388//182

//title
#define EPG_LIST_TITLE_X 0
#define EPG_LIST_TITLE_Y 0
#define EPG_LIST_TITLE_W (EPG_PG_CONT_W)
#define EPG_LIST_TITLE_H COMM_ITEM_H

//list
#define EPG_LIST_PGX     EPG_LIST_TITLE_X
#define EPG_LIST_PGY     (EPG_LIST_TITLE_Y + EPG_LIST_TITLE_H + 10)  //55
#define EPG_LIST_PGW    (EPG_PG_CONT_W - 10)
#define EPG_LIST_PGH    (EPG_PG_CONT_H - EPG_LIST_PGY)  //333

#define EPG_LIST_SBARX  (EPG_LIST_PGX + EPG_LIST_PGW +4)
#define EPG_LIST_SBARY  EPG_LIST_PGY
#define EPG_LIST_SBARW  6
#define EPG_LIST_SBARH  EPG_LIST_PGH

//time y/m/d
#define EPG_TIME_X  700//1000
#define EPG_TIME_Y  116//94//40
#define EPG_TIME_W  160
#define EPG_TIME_H  24

#define EPG_TIME_MBOX_TOL  2
#define EPG_TIME_MBOX_COL  2
#define EPG_TIME_MBOX_ROW  1
#define EPG_TIME_MBOX_HGAP  4
#define EPG_TIME_MBOX_VGAP  0

//event
#define EPG_EVENT_CONTX  (EPG_PG_CONT_X + EPG_PG_CONT_W + 10)//(EPG_PREV_X + EPG_PREV_W + 10)
#define EPG_EVENT_CONTY   EPG_PG_CONT_Y//EPG_PREV_Y
#define EPG_EVENT_CONTW  (COMM_BG_W - EPG_EVENT_CONTX - WIN_LR_SPACE)
#define EPG_EVENT_CONTH  EPG_PG_CONT_H//520

#define EPG_WEEKDAY_X         0
#define EPG_WEEKDAY_Y         0
#define EPG_WEEKDAY_W         (EPG_EVENT_CONTW - 2 * EPG_WEEKDAY_X)
#define EPG_WEEKDAY_H         COMM_ITEM_H//45
#define EPG_WEEKDAY_HGAP      0
#define EPG_WEEKDAY_LEFT      0
#define EPG_WEEKDAY_TOP       0
#define EPG_WEEKDAY_RIGHT     (EPG_WEEKDAY_W-EPG_WEEKDAY_LEFT) 
#define EPG_WEEKDAY_BOTTOM    EPG_WEEKDAY_H

#define EPG_LIST_EVTX  0
#define EPG_LIST_EVTY  50//23
#define EPG_LIST_EVTW  (EPG_EVENT_CONTW - 10)
#define EPG_LIST_EVTH  (EPG_EVENT_CONTH - EPG_LIST_EVTY)//460

#define EPG_EVT_LIST_PAGE 8//11
#define EPG_EVT_LIST_FIELD 4

#define EPG_EVT_SBARX  (EPG_LIST_EVTX + EPG_LIST_EVTW + 4) 
#define EPG_EVT_SBARY  (EPG_LIST_EVTY + 10)
#define EPG_EVT_SBARW  6
#define EPG_EVT_SBARH  (EPG_LIST_EVTH - 20)

#define EPG_VGAP 4
#define EPG_HGAP 4

/*rstyle*/
#define RSI_EPG_MENU    RSI_IGNORE
#define RSI_EPG_PREV  RSI_TV
#define RSI_EPG_DETAIL  RSI_COMMON_RECT1
#define RSI_EPG_LIST_CONT RSI_PBACK
#define RSI_EPG_LIST  RSI_COMMON_RECT1
#define RSI_EPG_TIME  RSI_PBACK
#define RSI_EPG_SBAR RSI_SCROLL_BAR_BG
#define RSI_EPG_SBAR_MID RSI_SCROLL_BAR_MID

/*others*/
#define EPG_GROUP_LIST_FIELD  1
#define EPG_LIST_PAGE  8// 4
#define EPG_LIST_PAGE_TOTAL  7
#define EPG_LIST_FIELD  2



/*fstyle*/
#define FSI_EPG_TIME_MBOX FSI_WHITE_18

/*others*/
#define SEVEN 7
#define ONE_DAY_SECONDS 86400 //24*60*60

RET_CODE open_epg(u32 para1, u32 para2);

#endif
