/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_MOVIE_SEARCH_H__
#define __UI_MOVIE_SEARCH_H__

/* coordinate */
#define MOVIE_SEARCH_CONT_X        ((SCREEN_WIDTH - MOVIE_SEARCH_CONT_W) / 2)
#define MOVIE_SEARCH_CONT_Y        ((SCREEN_HEIGHT - MOVIE_SEARCH_CONT_H) / 2)
#define MOVIE_SEARCH_CONT_W        560//460
#define MOVIE_SEARCH_CONT_H        200

#define MOVIE_SEARCH_TITLE_X       (MOVIE_SEARCH_CONT_W - MOVIE_SEARCH_TITLE_W) / 2
#define MOVIE_SEARCH_TITLE_Y       MOVIE_SEARCH_ITEM_VGAP
#define MOVIE_SEARCH_TITLE_W       180//100
#define MOVIE_SEARCH_TITLE_H       36

#define MOVIE_SEARCH_CAPS_BMAPX    (MOVIE_SEARCH_CAPS_TXTX - MOVIE_SEARCH_CAPS_BMAPW)
#define MOVIE_SEARCH_CAPS_BMAPY    MOVIE_SEARCH_CAPS_TXTY
#define MOVIE_SEARCH_CAPS_BMAPW    24
#define MOVIE_SEARCH_CAPS_BMAPH    24

#define MOVIE_SEARCH_CAPS_TXTX     (MOVIE_SEARCH_CONT_W - MOVIE_SEARCH_CAPS_TXTW - \
                              MOVIE_SEARCH_ITEM_HGAP)
#define MOVIE_SEARCH_CAPS_TXTY     (MOVIE_SEARCH_TITLE_Y + MOVIE_SEARCH_TITLE_H)
#define MOVIE_SEARCH_CAPS_TXTW     100
#define MOVIE_SEARCH_CAPS_TXTH     30//28

#define MOVIE_SEARCH_EBOX_X        20
#define MOVIE_SEARCH_EBOX_Y        (MOVIE_SEARCH_CAPS_TXTY + MOVIE_SEARCH_CAPS_TXTH)
#define MOVIE_SEARCH_EBOX_W        (MOVIE_SEARCH_CONT_W-2*MOVIE_SEARCH_EBOX_X)

#define MOVIE_SEARCH_ITEM_VGAP     10
#define MOVIE_SEARCH_ITEM_HGAP     10
/* rect style */
#define RSI_MOVIE_SEARCH_FRM       RSI_COMMAN_BG
#define RSI_MOVIE_SEARCH_EBOX      RSI_WINDOW_2

/* font style */
/*fstyle*/
#define FSI_MOVIE_SEARCH_SH        FSI_WHITE         //font styel of single satellite information edit
#define FSI_MOVIE_SEARCH_HL        FSI_BLACK



RET_CODE open_movie_search(u32 para1, u32 para2);

#endif

