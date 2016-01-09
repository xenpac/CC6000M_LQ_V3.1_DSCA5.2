/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_DLNA_H__
#define __UI_DLNA_H__


typedef enum
{
    DLNA_OFF = 0,
    DLNA_ON,
    DLNA_OPENING,
    DLNA_ON_FAILED,
    DLNA_CLOSING,
    DLNA_OFF_FAILED,
}dlna_sts;

#if 1
/*!
   x style
  */
typedef struct
{
  /*!
     Normal color
    */
  u32 n_xstyle;
  /*!
     Focus color
    */
  u32 f_xstyle;
    /*!
     Gray color
    */
  u32 g_xstyle;
}item_xstyle_t;

typedef struct
{
  u32                 str_id;
  u32                  type_attr;
  u32                  align_attr;
  u16                  left;
  u16                    top;
  u16                  width;
  u16                  heigt;
  item_xstyle_t * rstyle;
  item_xstyle_t * fstyle;
}dlna_item_attr_t;
#endif

#define MAX_DLNA_TEXT_CTRL 10

RET_CODE open_dlna_start(u32 para1, u32 para2);
void ui_set_dlna_status(u32 status);
u8 ui_get_dlna_status(void);
BOOL ui_get_closing_satip_at_dlna(void);

#endif
