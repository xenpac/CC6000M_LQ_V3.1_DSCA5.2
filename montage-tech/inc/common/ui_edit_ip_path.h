/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_EDIT_IP_PATH_H__
#define __UI_EDIT_IP_PATH_H__

/* coordinate */
/*!
  fix me
  */
#define EDIT_IP_PATH_CONT_X        ((SCREEN_WIDTH - EDIT_IP_PATH_CONT_W) / 2)
/*!
  fix me
  */
#define EDIT_IP_PATH_CONT_Y        ((SCREEN_HEIGHT - EDIT_IP_PATH_CONT_H) / 2)
/*!
  fix me
  */
#define EDIT_IP_PATH_CONT_W        560//460
/*!
  fix me
  */
#define EDIT_IP_PATH_CONT_H        200

/*!
  fix me
  */
#define EDIT_IP_PATH_TITLE_X       (EDIT_IP_PATH_CONT_W - EDIT_IP_PATH_TITLE_W) / 2
/*!
  fix me
  */
#define EDIT_IP_PATH_TITLE_Y       EDIT_IP_PATH_ITEM_VGAP
/*!
  fix me
  */
#define EDIT_IP_PATH_TITLE_W       180//100
/*!
  fix me
  */
#define EDIT_IP_PATH_TITLE_H       36


/*!
  fix me
  */
#define EDIT_IP_PATH_TXTX          (EDIT_IP_PATH_CONT_W - EDIT_IP_PATH_TXTW - \
                                    EDIT_IP_PATH_ITEM_HGAP)
/*!
  fix me
  */
#define EDIT_IP_PATH_TXTY          (EDIT_IP_PATH_TITLE_Y + EDIT_IP_PATH_TITLE_H)
/*!
  fix me
  */
#define EDIT_IP_PATH_TXTW          540
/*!
  fix me
  */
#define EDIT_IP_PATH_TXTH          30//28
/*!
  fix me
  */
 #define EDIT_IP_PATH_EBOX_X        20
/*!
  fix me
  */
#define EDIT_IP_PATH_EBOX_Y        (EDIT_IP_PATH_TXTY + EDIT_IP_PATH_TXTH)
/*!
  fix me
  */
#define EDIT_IP_PATH_EBOX_W        (EDIT_IP_PATH_CONT_W-2*EDIT_IP_PATH_EBOX_X)

/*!
  fix me
  */
#define EDIT_IP_PATH_ITEM_VGAP     10
/*!
  fix me
  */
#define EDIT_IP_PATH_ITEM_HGAP     10
/* rect style */
/*!
  fix me
  */
#define RSI_EDIT_IP_PATH_FRM       RSI_COMMAN_BG
/*!
  fix me
  */
#define RSI_EDIT_IP_PATH_EBOX      RSI_WINDOW_2

/* font style */
/*fstyle*/
/*!
  fix me
  */
#define FSI_EDIT_IP_PATH_SH        FSI_WHITE         //font styel of single satellite information edit
/*!
  fix me
  */
#define FSI_EDIT_IP_PATH_HL        FSI_BLACK

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
} edit_ip_path_param_t;

/*!
  fix me
  */
RET_CODE open_edit_ip_path(u32 para1, u32 para2);

#endif
