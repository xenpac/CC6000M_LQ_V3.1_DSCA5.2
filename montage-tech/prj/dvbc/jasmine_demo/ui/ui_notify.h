/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NOTIFY_H__
#define __UI_NOTIFY_H__

/* coordinate */
#define NOTIFY_CONT_X     ((SCREEN_WIDTH - NOTIFY_CONT_W)/2)
#define NOTIFY_CONT_Y     30
#define NOTIFY_CONT_W     180
#define NOTIFY_CONT_H     COMM_ITEM_H//36

/* rect style */
#define RSI_NOTIFY_TXT    RSI_COMMAN_BG

/* font style */
#define FSI_NOTIFY_TXT    FSI_WHITE

#define NOTIFY_AUTOCLOSE_5000MS 5000
#define NOTIFY_AUTOCLOSE_3000MS 3000

enum notify_type
{
  NOTIFY_TYPE_STRID = 0,
  NOTIFY_TYPE_ASC,
  NOTIFY_TYPE_UNI,
};

void ui_set_notify(rect_t *p_rect, u32 type, u32 content, u32 duration);

BOOL ui_is_notify(void);

RET_CODE open_notify(u32 para1, u32 para2);

void close_notify(void);

void  ui_set_notify_autoclose(BOOL is_autoclose);
#endif


