/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_PPPOE_CONNECT_H__
#define __UI_PPPOE_CONNECT_H__

/* coordinate */
#define PPPOE_CONT_X          ((SCREEN_WIDTH - PPPOE_CONT_W) / 2)
#define PPPOE_CONT_Y           ((SCREEN_HEIGHT- PPPOE_CONT_H) / 2)
#define PPPOE_CONT_W          600
#define PPPOE_CONT_H          (2*PPPOE_TOP_BTM_VGAP+(PPPOE_TITLE_H + PPPOE_ITEM_VGAP)+PPPOE_ITEM_CNT*PPPOE_ITEM_VGAP+(PPPOE_ITEM_CNT+1)*PPPOE_EBOX_H)

//title
#define PPPOE_TITLE_X 20
#define PPPOE_TITLE_Y PPPOE_ITEM_VGAP
#define PPPOE_TITLE_W PPPOE_CONT_W - 2*PPPOE_TITLE_X
#define PPPOE_TITLE_H 30

//ebox
#define PPPOE_EBOX_X 20
#define PPPOE_EBOX_Y 20 + PPPOE_TITLE_H
#define PPPOE_EBOX_LW 220
#define PPPOE_EBOX_RW (PPPOE_CONT_W-2*PPPOE_EBOX_X-PPPOE_EBOX_LW)
#define PPPOE_EBOX_H COMM_CTRL_H

#define PPPOE_TOP_BTM_VGAP 10
#define PPPOE_ITEM_VGAP 10

/*others*/
#define PPPOE_ITEM_CNT 4
#define PPPOE_MAX_LENGTH 32

RET_CODE open_pppoe_connect(u32 para1, u32 para2);

#endif
