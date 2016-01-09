/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_ONMOV_WEBSITES_H__
#define __UI_ONMOV_WEBSITES_H__


#define OMW_WLIST_CONTX     (120)
#define OMW_WLIST_CONTY     (190)
#define OMW_WLIST_CONTW     (1040)
#define OMW_WLIST_CONTH     (280)

#define OMW_WLIST_ITEM_X    (0)
#define OMW_WLIST_ITEM_Y    (0)
#define OMW_WLIST_ITEM_W    (250)
#define OMW_WLIST_ITEM_H    (250)

#define OMW_WLIST_PIC_X     (5)
#define OMW_WLIST_PIC_Y     (5)
#define OMW_WLIST_PIC_W     (OMW_WLIST_ITEM_W - OMW_WLIST_PIC_X - OMW_WLIST_PIC_X)
#define OMW_WLIST_PIC_H     (OMW_WLIST_ITEM_H - OMW_WLIST_PIC_Y - OMW_WLIST_PIC_Y)

//bottom line
#define OMW_BOTTOM_LINE_X    (0)
#define OMW_BOTTOM_LINE_Y    (OMW_BOTTOM_HELP_Y - OMW_BOTTOM_LINE_H)
#define OMW_BOTTOM_LINE_W    (COMM_BG_W)
#define OMW_BOTTOM_LINE_H    (3)

//bottom help container
#define OMW_BOTTOM_HELP_X  0
#define OMW_BOTTOM_HELP_Y  630
#define OMW_BOTTOM_HELP_W  1040
#define OMW_BOTTOM_HELP_H  80

#ifdef MODEST
#define OMW_WLIST_ITEM_NUM_ONE_PAGE  2// 4
#else
#define OMW_WLIST_ITEM_NUM_ONE_PAGE  4
#endif

#if 0 
#define NETPLAY_NAME_MAX_LEN        64
#endif

RET_CODE open_onmov_websites(u32 para1, u32 para2);
#endif
