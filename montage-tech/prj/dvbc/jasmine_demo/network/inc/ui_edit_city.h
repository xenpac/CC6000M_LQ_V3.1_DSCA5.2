/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_EDIT_CITY_H__
#define __UI_EDIT_CITY_H__

/* coordinate */
#define EDIT_CITY_CONT_X        ((SCREEN_WIDTH - EDIT_CITY_CONT_W) / 2)
#define EDIT_CITY_CONT_Y        ((SCREEN_HEIGHT - EDIT_CITY_CONT_H) / 2)
#define EDIT_CITY_CONT_W        560//460
#define EDIT_CITY_CONT_H        200

#define EDIT_CITY_TITLE_X       (EDIT_CITY_CONT_W - EDIT_CITY_TITLE_W) / 2
#define EDIT_CITY_TITLE_Y       EDIT_CITY_ITEM_VGAP
#define EDIT_CITY_TITLE_W       180//100
#define EDIT_CITY_TITLE_H       36

#define EDIT_CITY_CAPS_BMAPX    (EDIT_CITY_CAPS_TXTX - EDIT_CITY_CAPS_BMAPW)
#define EDIT_CITY_CAPS_BMAPY    EDIT_CITY_CAPS_TXTY
#define EDIT_CITY_CAPS_BMAPW    24
#define EDIT_CITY_CAPS_BMAPH    24

#define EDIT_CITY_CAPS_TXTX     (EDIT_CITY_CONT_W - EDIT_CITY_CAPS_TXTW - \
                              EDIT_CITY_ITEM_HGAP)
#define EDIT_CITY_CAPS_TXTY     (EDIT_CITY_TITLE_Y + EDIT_CITY_TITLE_H)
#define EDIT_CITY_CAPS_TXTW     100
#define EDIT_CITY_CAPS_TXTH     30//28

#define EDIT_CITY_EBOX_X        20
#define EDIT_CITY_EBOX_Y        (EDIT_CITY_CAPS_TXTY + EDIT_CITY_CAPS_TXTH)
#define EDIT_CITY_EBOX_W        (EDIT_CITY_CONT_W-2*EDIT_CITY_EBOX_X)

#define EDIT_CITY_ITEM_VGAP     10
#define EDIT_CITY_ITEM_HGAP     10
/* rect style */
#define RSI_EDIT_CITY_FRM       RSI_COMMAN_BG
#define RSI_EDIT_CITY_EBOX      RSI_WINDOW_2

/* font style */
/*fstyle*/
#define FSI_EDIT_CITY_SH        FSI_WHITE         //font styel of single satellite information edit
#define FSI_EDIT_CITY_HL        FSI_BLACK

/*others*/

typedef struct
{
  u16 *         uni_str;
  u16           max_len;
  u16           type;
  u16           add_type;
} edit_city_param_t;

RET_CODE open_edit_city(u32 para1, u32 para2);

#endif
