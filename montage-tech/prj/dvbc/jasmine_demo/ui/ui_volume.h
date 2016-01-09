/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_VOLUME_H__
#define __UI_VOLUME_H__

#define VOLUME_L			((SCREEN_WIDTH-VOLUME_W)/2)
#define VOLUME_T			530//540
#define VOLUME_W		    650
#define VOLUME_H			60

#define VOLUME_BAR_CONT_X	0
#define VOLUME_BAR_CONT_Y	0
#define VOLUME_BAR_CONT_W	VOLUME_W
#define VOLUME_BAR_CONT_H	VOLUME_H

#define VOLUME_BAR_ICON_X	    20
#define VOLUME_BAR_ICON_Y	    0
#define VOLUME_BAR_ICON_W	    30
#define VOLUME_BAR_ICON_H	    VOLUME_H

#define VOLUME_BAR_X	    72
#define VOLUME_BAR_Y	    ((VOLUME_H - VOLUME_BAR_H)/2)
#define VOLUME_BAR_W	 (VOLUME_BAR_CONT_W - 2*VOLUME_BAR_X)
#define VOLUME_BAR_H	    10

#define VOLUME_TEXT_X	    (VOLUME_BAR_X + VOLUME_BAR_W + 5)
#define VOLUME_TEXT_Y	    15
#define VOLUME_TEXT_W	    50
#define VOLUME_TEXT_H	    30

#define VOLUME_MAX    AP_VOLUME_MAX

/* rect style */
#define RSI_VOLUME_CONT     RSI_IGNORE
#define RSI_VOLUME_BAR_FRAME RSI_COMMON_RECT1//RSI_VOLUME_WIN

void set_volume(u8 volume);

RET_CODE open_volume (u32 para1, u32 para2);

RET_CODE close_volume(void);

extern BOOL is_display_pg;

#endif


