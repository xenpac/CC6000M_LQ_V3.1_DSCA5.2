/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_NETWORK_MUSIC_SEARCH_H__
#define __UI_NETWORK_MUSIC_SEARCH_H__

#include "ui_common.h"

#define NM_SEARCH_CONT_X ((COMM_BG_W/2) - (NM_SEARCH_CONT_W/2))
#define NM_SEARCH_CONT_Y ((COMM_BG_H/2)- (NM_SEARCH_CONT_H/2))
#define NM_SEARCH_CONT_W 510
#define NM_SEARCH_CONT_H 200

#define NM_SEARCH_DLG_BTN_W 80
#define NM_SEARCH_DLG_BTN_H 30




RET_CODE ui_net_music_search_open(u32 para1, u32 para2);

s16 ui_network_music_get_focus_btn_index(void);

#endif
