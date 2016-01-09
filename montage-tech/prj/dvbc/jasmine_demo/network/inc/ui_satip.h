/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_SATIP_H__
#define __UI_SATIP_H__

/* coordinate */
#define SATIP_ITEM_CNT		1
#define SATIP_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define SATIP_ITEM_Y			COMM_ITEM_OY_IN_ROOT
#define SATIP_ITEM_LW		COMM_ITEM_LW
#define SATIP_ITEM_RW		(COMM_ITEM_MAX_WIDTH - SATIP_ITEM_LW)
#define SATIP_ITEM_H			COMM_ITEM_H
#define SATIP_ITEM_V_GAP		16

#define SATIP_MSG_X     (2*SATIP_ITEM_X)
#define SATIP_MSG_Y     (SATIP_ITEM_Y + SATIP_ITEM_H + SATIP_ITEM_V_GAP)
#define SATIP_MSG_W     (SCREEN_WIDTH - 2*SATIP_MSG_X)
#define SATIP_MSG_H     200


/* rect style */
#define SATIP_OTHERS_FRM		RSI_WINDOW_1
/* font style */


#define SAT_IP_ITEM 2

typedef enum
{
    SATIP_OFF = 0,
    SATIP_ON,
    SATIP_OPENING,
    SATIP_ON_FAILED,
    SATIP_CLOSING,
    SATIP_OFF_FAILED,
}satip_sts;


void ui_set_satip_status(u8 status);
u8 ui_get_satip_status(void);
u16 ui_get_satip_client_pg_id();
void ui_set_satip_client_pg_id(u16 pg_id);

RET_CODE open_satip (u32 para1, u32 para2);
u32 ui_dvbs_calc_mid_freq(dvbs_tp_node_t *p_tp, sat_node_t *p_sat);
void  ui_set_satip_pg_info(void);

#endif

