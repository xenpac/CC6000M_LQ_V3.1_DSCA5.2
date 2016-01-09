/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_EDIT_CITY_H__
#define __UI_EDIT_CITY_H__

/* coordinate */
/*!
  fix me
  */
#define EDIT_CITY_CONT_X        ((SCREEN_WIDTH - EDIT_CITY_CONT_W) / 2)
/*!
  fix me
  */
#define EDIT_CITY_CONT_Y        ((SCREEN_HEIGHT - EDIT_CITY_CONT_H) / 2)
/*!
  fix me
  */
#define EDIT_CITY_CONT_W        560//460
/*!
  fix me
  */
#define EDIT_CITY_CONT_H        200

/*!
  fix me
  */
#define EDIT_CITY_TITLE_X       (EDIT_CITY_CONT_W - EDIT_CITY_TITLE_W) / 2
/*!
  fix me
  */
#define EDIT_CITY_TITLE_Y       EDIT_CITY_ITEM_VGAP
/*!
  fix me
  */
#define EDIT_CITY_TITLE_W       180//100
/*!
  fix me
  */
#define EDIT_CITY_TITLE_H       36

/*!
  fix me
  */
#define EDIT_CITY_CAPS_BMAPX    (EDIT_CITY_CAPS_TXTX - EDIT_CITY_CAPS_BMAPW)
/*!
  fix me
  */
#define EDIT_CITY_CAPS_BMAPY    EDIT_CITY_CAPS_TXTY
/*!
  fix me
  */
#define EDIT_CITY_CAPS_BMAPW    24
/*!
  fix me
  */
#define EDIT_CITY_CAPS_BMAPH    24

/*!
  fix me
  */
#define EDIT_CITY_CAPS_TXTX     (EDIT_CITY_CONT_W - EDIT_CITY_CAPS_TXTW - \
                              EDIT_CITY_ITEM_HGAP)
/*!
  fix me
  */
#define EDIT_CITY_CAPS_TXTY     (EDIT_CITY_TITLE_Y + EDIT_CITY_TITLE_H)
/*!
  fix me
  */
#define EDIT_CITY_CAPS_TXTW     100
/*!
  fix me
  */
#define EDIT_CITY_CAPS_TXTH     30//28

/*!
  fix me
  */
#define EDIT_CITY_EBOX_X        20
/*!
  fix me
  */
#define EDIT_CITY_EBOX_Y        (EDIT_CITY_CAPS_TXTY + EDIT_CITY_CAPS_TXTH)
/*!
  fix me
  */
#define EDIT_CITY_EBOX_W        (EDIT_CITY_CONT_W-2*EDIT_CITY_EBOX_X)

/*!
  fix me
  */
#define EDIT_CITY_ITEM_VGAP     10
/*!
  fix me
  */
#define EDIT_CITY_ITEM_HGAP     10
/* rect style */
/*!
  fix me
  */
#define RSI_EDIT_CITY_FRM       RSI_COMMAN_BG
/*!
  fix me
  */
#define RSI_EDIT_CITY_EBOX      RSI_WINDOW_2

/* font style */
/*fstyle*/
/*!
  fix me
  */
#define FSI_EDIT_CITY_SH        FSI_WHITE         //font styel of single satellite information edit
/*!
  fix me
  */
#define FSI_EDIT_CITY_HL        FSI_BLACK

/*others*/

/*!
  fix me
  */
typedef struct
{
/*!
  fix me
  */
  u16 *         uni_str;
/*!
  fix me
  */
  u16           max_len;
/*!
  fix me
  */
  u16           type;
/*!
  fix me
  */
  u16           add_type;
} edit_city_param_t;

/*!
  fix me
  */
RET_CODE open_edit_city(u32 para1, u32 para2);

#endif
