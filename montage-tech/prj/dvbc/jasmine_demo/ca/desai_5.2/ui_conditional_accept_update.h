/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_UPDATE__
#define __UI_CONDITIONAL_ACCEPT_UPDATE__

#define CA_UP_CONT_FULL_X  ((SCREEN_WIDTH-CA_UP_CONT_FULL_W)/2)
#define CA_UP_CONT_FULL_Y  ((SCREEN_HEIGHT-CA_UP_CONT_FULL_H)/2 -20)
#define CA_UP_CONT_FULL_W  640
#define CA_UP_CONT_FULL_H  160

#define CA_UP_TITLE_FULL_X  0
#define CA_UP_TITLE_FULL_Y  10
#define CA_UP_TITLE_FULL_W  CA_UP_CONT_FULL_W
#define CA_UP_TITLE_FULL_H  40

#define CA_UP_CONTENT_FULL_X  10
#define CA_UP_CONTENT_FULL_Y  (CA_UP_TITLE_FULL_Y + CA_UP_TITLE_FULL_H + 20)
#define CA_UP_CONTENT_FULL_W  (CA_UP_CONT_FULL_W - 2 * CA_UP_CONTENT_FULL_X)
#define CA_UP_CONTENT_FULL_H  60

#define CA_STATUS_BAR_LX 10
#define CA_STATUS_BAR_LY  (CA_UP_TITLE_FULL_Y + CA_UP_TITLE_FULL_H + 20)
#define CA_STATUS_BAR_LW 110
#define CA_STATUS_BAR_LH 50

#define CA_STATUS_BAR_MX (CA_STATUS_BAR_LX + CA_STATUS_BAR_LW + 5)
#define CA_STATUS_BAR_MY (CA_UP_TITLE_FULL_Y + CA_UP_TITLE_FULL_H + 38)
#define CA_STATUS_BAR_MW 420
#define CA_STATUS_BAR_MH 16

#define CA_STATUS_BAR_RX (CA_STATUS_BAR_MX + CA_STATUS_BAR_MW + 5)
#define CA_STATUS_BAR_RY (CA_UP_TITLE_FULL_Y + CA_UP_TITLE_FULL_H + 20)
#define CA_STATUS_BAR_RW 60
#define CA_STATUS_BAR_RH CA_STATUS_BAR_LH

#define MAX_UPDATE_STR_LEN 64

RET_CODE open_ca_card_update_info(u32 para1, u32 para2);
RET_CODE close_ca_card_update_info(control_t *cont, u16 msg, u32 para1, u32 para2);

#endif