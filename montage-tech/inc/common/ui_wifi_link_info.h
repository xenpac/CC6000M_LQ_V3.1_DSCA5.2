/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_WIFI_LINK_INFO_H__
#define __UI_WIFI_LINK_INFO_H__

/* coordinate */
#define WIFI_LINK_INFO_CONT_X          ((SCREEN_WIDTH - WIFI_LINK_INFO_CONT_W) / 2)
#define WIFI_LINK_INFO_CONT_Y           ((SCREEN_HEIGHT- WIFI_LINK_INFO_CONT_H) / 2)
#define WIFI_LINK_INFO_CONT_W          600
#define WIFI_LINK_INFO_CONT_H          (2*WIFI_LINK_INFO_TOP_BTM_VGAP+(WIFI_LINK_INFO_TITLE_H + WIFI_LINK_INFO_ITEM_VGAP)+WIFI_LINK_INFO_ITEM_CNT*WIFI_LINK_INFO_ITEM_VGAP+(WIFI_LINK_INFO_ITEM_CNT+1)*WIFI_LINK_INFO_EBOX_H)

//title
#define WIFI_LINK_INFO_TITLE_X 20
#define WIFI_LINK_INFO_TITLE_Y WIFI_LINK_INFO_ITEM_VGAP
#define WIFI_LINK_INFO_TITLE_W WIFI_LINK_INFO_CONT_W - 2*WIFI_LINK_INFO_TITLE_X
#define WIFI_LINK_INFO_TITLE_H 30

//ebox
#define WIFI_LINK_INFO_EBOX_X 20
#define WIFI_LINK_INFO_EBOX_Y 20 + WIFI_LINK_INFO_TITLE_H
#define WIFI_LINK_INFO_EBOX_LW 220
#define WIFI_LINK_INFO_EBOX_RW (WIFI_LINK_INFO_CONT_W-2*WIFI_LINK_INFO_EBOX_X-WIFI_LINK_INFO_EBOX_LW)
#define WIFI_LINK_INFO_EBOX_H COMM_CTRL_H

#define WIFI_LINK_INFO_TOP_BTM_VGAP 10
#define WIFI_LINK_INFO_ITEM_VGAP 10
/* rect style */

/* font style */
/*fstyle*/
#define FSI_WIFI_LINK_INFO_SH          FSI_WHITE       //font styel of single satellite information edit
#define FSI_WIFI_LINK_INFO_HL          FSI_BLACK

/*others*/
#define WIFI_LINK_INFO_ITEM_CNT 5
#define WIFI_LINK_INFO_MAX_LENGTH 32
RET_CODE open_wifi_link_info(u32 para1, u32 para2);

#endif