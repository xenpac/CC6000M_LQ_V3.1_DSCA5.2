/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifdef ENABLE_IPTV

#ifndef __UI_IPTV_H__
#define __UI_IPTV_H__

/*!
  pg list coordinate
  */
#define IPTV_LIST_CONT_X 50
#define IPTV_LIST_CONT_W 380
#define IPTV_LIST_CONT_H 480
#define IPTV_LIST_X   10
#define IPTV_LIST_Y   30
#define IPTV_LIST_W  350
#define IPTV_LIST_H   425
#define IPTV_LIST_MID_L 4
#define IPTV_LIST_MID_T 4
#define IPTV_LIST_MID_R (IPTV_LIST_W - IPTV_LIST_MID_L)
#define IPTV_LIST_MID_B  (IPTV_LIST_H - IPTV_LIST_MID_T)

/*!
  sbar coordinate
  */
#define IPTV_LIST_SBAR_X  (IPTV_LIST_X + IPTV_LIST_W)
#define IPTV_LIST_SBAR_Y   IPTV_LIST_Y
#define IPTV_LIST_SBAR_W  6
#define IPTV_LIST_SBAR_H   IPTV_LIST_H

/*!
  other
  */
#define IPTV_LIST_V_GAP  4
#define IPTV_LIST_PAGE 10
#define IPTV_LIST_FIELD_NUM 2

void iptv_get_url(void);
void iptv_start_play(u16 focus);
void iptv_stop_play(void);
RET_CODE open_iptv(u32 para1, u32 para2);
#endif
#endif
