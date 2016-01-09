/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_ZOOM_H__
#define __UI_ZOOM_H__

/* coordinate */
#define ZOOM_CONT_X     (SCREEN_WIDTH - ZOOM_CONT_W - 40)
#define ZOOM_CONT_Y     (SCREEN_HEIGHT - ZOOM_CONT_H - 40)
#define ZOOM_CONT_W     160
#define ZOOM_CONT_H     160

#define ZOOM_STXT_X     10
#define ZOOM_STXT_Y     10
#define ZOOM_STXT_W     (ZOOM_CONT_W - 2 * ZOOM_STXT_X)
#define ZOOM_STXT_H     36

#define ZOOM_BOUND_X     ((ZOOM_CONT_W-ZOOM_BOUND_W)/2)
#define ZOOM_BOUND_Y     (ZOOM_CONT_H -(ZOOM_BOUND_X + ZOOM_BOUND_H))
#define ZOOM_BOUND_W     136
#define ZOOM_BOUND_H     102

#define ZOOM_ICON_X     ZOOM_BOUND_X
#define ZOOM_ICON_Y     ZOOM_BOUND_Y
#define ZOOM_ICON_W     ZOOM_BOUND_W
#define ZOOM_ICON_H     ZOOM_BOUND_H

#define ZOOM_ARROW_L_X ( ZOOM_BOUND_X - 9)
#define ZOOM_ARROW_L_Y ( ZOOM_BOUND_Y + (ZOOM_BOUND_H - ZOOM_ARROW_L_H)/2)
#define ZOOM_ARROW_L_W 6
#define ZOOM_ARROW_L_H 10

#define ZOOM_ARROW_T_X (ZOOM_BOUND_X + (ZOOM_BOUND_W - ZOOM_ARROW_T_W)/2)
#define ZOOM_ARROW_T_Y ( ZOOM_BOUND_Y - 9)
#define ZOOM_ARROW_T_W 10
#define ZOOM_ARROW_T_H 6

#define ZOOM_ARROW_R_X ( ZOOM_BOUND_X + ZOOM_BOUND_W + 3)
#define ZOOM_ARROW_R_Y ( ZOOM_BOUND_Y + (ZOOM_BOUND_H - ZOOM_ARROW_R_H)/2)
#define ZOOM_ARROW_R_W 6
#define ZOOM_ARROW_R_H 10

#define ZOOM_ARROW_B_X (ZOOM_BOUND_X + (ZOOM_BOUND_W - ZOOM_ARROW_B_W)/2)
#define ZOOM_ARROW_B_Y ( ZOOM_BOUND_Y + ZOOM_BOUND_H + 3)
#define ZOOM_ARROW_B_W 10
#define ZOOM_ARROW_B_H 6

/* rect style */
#define RSI_ZOOM_FRM    RSI_TV_SMALL//RSI_TV

#define RSI_ZOOM_TXT    RSI_TRANSPARENT
#define RSI_ZOOM_BOUND  RSI_TV_SMALL//RSI_TV
/* font style */
#define FSI_ZOOM_TXT      FSI_WHITE
/* others */

RET_CODE open_zoom(u32 para1, u32 para2);

#endif


