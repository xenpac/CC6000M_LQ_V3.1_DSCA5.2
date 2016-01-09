/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_EDIT_IP_PATH_H__
#define __UI_EDIT_IP_PATH_H__

/* coordinate */
#define EDIT_IP_PATH_CONT_X        ((SCREEN_WIDTH - EDIT_IP_PATH_CONT_W) / 2)
#define EDIT_IP_PATH_CONT_Y        ((SCREEN_HEIGHT - EDIT_IP_PATH_CONT_H) / 2)
#define EDIT_IP_PATH_CONT_W        560//460
#define EDIT_IP_PATH_CONT_H        200

#define EDIT_IP_PATH_TITLE_X       (EDIT_IP_PATH_CONT_W - EDIT_IP_PATH_TITLE_W) / 2
#define EDIT_IP_PATH_TITLE_Y       EDIT_IP_PATH_ITEM_VGAP
#define EDIT_IP_PATH_TITLE_W       180//100
#define EDIT_IP_PATH_TITLE_H       36

//#define EDIT_IP_PATH_CAPS_BMAPX    (EDIT_IP_PATH_CAPS_TXTX - EDIT_IP_PATH_CAPS_BMAPW)
//#define EDIT_IP_PATH_CAPS_BMAPY    EDIT_IP_PATH_CAPS_TXTY
//#define EDIT_IP_PATH_CAPS_BMAPW    24
//#define EDIT_IP_PATH_CAPS_BMAPH    24

#define EDIT_IP_PATH_TXTX          (EDIT_IP_PATH_CONT_W - EDIT_IP_PATH_TXTW - \
                              EDIT_IP_PATH_ITEM_HGAP)
#define EDIT_IP_PATH_TXTY          (EDIT_IP_PATH_TITLE_Y + EDIT_IP_PATH_TITLE_H)
#define EDIT_IP_PATH_TXTW          540
#define EDIT_IP_PATH_TXTH          30//28

#define EDIT_IP_PATH_EBOX_X        20
#define EDIT_IP_PATH_EBOX_Y        (EDIT_IP_PATH_TXTY + EDIT_IP_PATH_TXTH)
#define EDIT_IP_PATH_EBOX_W        (EDIT_IP_PATH_CONT_W-2*EDIT_IP_PATH_EBOX_X)

#define EDIT_IP_PATH_ITEM_VGAP     10
#define EDIT_IP_PATH_ITEM_HGAP     10
/* rect style */
#define RSI_EDIT_IP_PATH_FRM       RSI_COMMAN_BG
#define RSI_EDIT_IP_PATH_EBOX      RSI_WINDOW_2

/* font style */
/*fstyle*/
#define FSI_EDIT_IP_PATH_SH        FSI_WHITE         //font styel of single satellite information edit
#define FSI_EDIT_IP_PATH_HL        FSI_BLACK

/*others*/

typedef struct
{
  u16 *         uni_str;
  u16           max_len;
  u16           type;
  u16           add_type;
} edit_ip_path_param_t;

RET_CODE open_edit_ip_path(u32 para1, u32 para2);

#endif
