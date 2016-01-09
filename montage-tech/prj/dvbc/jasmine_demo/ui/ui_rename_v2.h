/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_RENAME_V2_H__
#define __UI_RENAME_V2_H__

/* coordinate */
#define RENAME_V2_CONT_X        ((SCREEN_WIDTH - RENAME_V2_CONT_W) / 2)
#define RENAME_V2_CONT_Y        ((SCREEN_HEIGHT - RENAME_V2_CONT_H) / 2)
#define RENAME_V2_CONT_W        520//460
#define RENAME_V2_CONT_H        200

#define RENAME_V2_TITLE_X       ((RENAME_V2_CONT_W - RENAME_V2_TITLE_W) / 2)
#define RENAME_V2_TITLE_Y       RENAME_V2_ITEM_VGAP
#define RENAME_V2_TITLE_W       120//100
#define RENAME_V2_TITLE_H       36

#define RENAME_V2_CAPS_BMAPX    (RENAME_V2_CAPS_TXTX - RENAME_V2_CAPS_BMAPW)
#define RENAME_V2_CAPS_BMAPY    RENAME_V2_CAPS_TXTY
#define RENAME_V2_CAPS_BMAPW    24
#define RENAME_V2_CAPS_BMAPH    24

#define RENAME_V2_CAPS_TXTX     (RENAME_V2_CONT_W - RENAME_V2_CAPS_TXTW - \
                              RENAME_V2_ITEM_HGAP)
#define RENAME_V2_CAPS_TXTY     (RENAME_V2_TITLE_Y + RENAME_V2_TITLE_H)
#define RENAME_V2_CAPS_TXTW     100
#define RENAME_V2_CAPS_TXTH     30//28

#define RENAME_V2_EBOX_X        20
#define RENAME_V2_EBOX_Y        (RENAME_V2_CAPS_TXTY + RENAME_V2_CAPS_TXTH)
#define RENAME_V2_EBOX_W        (RENAME_V2_CONT_W-2*RENAME_V2_EBOX_X)

#define RENAME_V2_ITEM_VGAP     10
#define RENAME_V2_ITEM_HGAP     10
/* rect style */
#define RSI_RENAME_V2_FRM       RSI_COMMAN_BG
#define RSI_RENAME_V2_EBOX      RSI_WINDOW_2

/* font style */
/*fstyle*/
#define FSI_RENAME_V2_SH        FSI_WHITE         //font styel of single satellite information edit
#define FSI_RENAME_V2_HL        FSI_BLACK

/*others*/

typedef struct
{
  u16 *         uni_str;
  u16           max_len;
} rename_param_t_v2;

RET_CODE open_rename_v2(u32 para1, u32 para2);

#endif
