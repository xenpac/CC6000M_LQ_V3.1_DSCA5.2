/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_FREQ_SET_H__
#define __UI_FREQ_SET_H__

/* coordinate */
#define FREQ_SET_CONT_X            ((COMM_BG_W - FREQ_SET_CONT_W) / 2)
#define FREQ_SET_CONT_Y            ((COMM_BG_H - FREQ_SET_CONT_H) / 2)
#define FREQ_SET_CONT_W            COMM_BG_W
#define FREQ_SET_CONT_H            COMM_BG_H

//specify tp frm
#define FREQ_SET_SPECIFY_TP_FRM_X     0
#define FREQ_SET_SPECIFY_TP_FRM_Y     0
#define FREQ_SET_SPECIFY_TP_FRM_W     COMM_BG_W
#define FREQ_SET_SPECIFY_TP_FRM_H     400

#define FREQ_SET_SPECIFY_TP_ITEM_CNT     4
#define FREQ_SET_SPECIFY_TP_ITEM_X       COMM_ITEM_OX_IN_ROOT
#define FREQ_SET_SPECIFY_TP_ITEM_Y       COMM_ITEM_OY_IN_ROOT
#define FREQ_SET_SPECIFY_TP_ITEM_LW      COMM_ITEM_LW
#define FREQ_SET_SPECIFY_TP_ITEM_RW      (COMM_ITEM_MAX_WIDTH - FREQ_SET_SPECIFY_TP_ITEM_LW)                              

#define FREQ_SET_SPECIFY_TP_ITEM_H       COMM_ITEM_H
#define FREQ_SET_SPECIFY_TP_ITEM_VGAP    10 //20

//searching info frm
#define FREQ_SET_SIG_INFO_FRM_X        80
#define FREQ_SET_SIG_INFO_FRM_Y        360
#define FREQ_SET_SIG_INFO_FRM_W        544
#define FREQ_SET_SIG_INFO_FRM_H        200

#define FREQ_SET_V_GAP               10

//signal strength name
#define FREQ_SET_SIG_STRENGTH_NAME_X       0
#define FREQ_SET_SIG_STRENGTH_NAME_Y       0
#define FREQ_SET_SIG_STRENGTH_NAME_W       110
#define FREQ_SET_SIG_STRENGTH_NAME_H       30

//signal strength percent
#define FREQ_SET_SIG_STRENGTH_PERCENT_X    (FREQ_SET_SIG_STRENGTH_NAME_X + FREQ_SET_SIG_STRENGTH_NAME_W)
#define FREQ_SET_SIG_STRENGTH_PERCENT_Y    FREQ_SET_SIG_STRENGTH_NAME_Y
#define FREQ_SET_SIG_STRENGTH_PERCENT_W    100
#define FREQ_SET_SIG_STRENGTH_PERCENT_H    30

//signal strength pbar
#define FREQ_SET_SIG_STRENGTH_PBAR_X       (FREQ_SET_SIG_STRENGTH_PERCENT_X + FREQ_SET_SIG_STRENGTH_PERCENT_W)
#define FREQ_SET_SIG_STRENGTH_PBAR_Y       (FREQ_SET_SIG_STRENGTH_PERCENT_Y + (FREQ_SET_SIG_STRENGTH_NAME_H - FREQ_SET_SIG_STRENGTH_PERCENT_H)/2)
#define FREQ_SET_SIG_STRENGTH_PBAR_W       120
#define FREQ_SET_SIG_STRENGTH_PBAR_H       16

//signal BER name
#define FREQ_SET_SIG_BER_NAME_X       FREQ_SET_SIG_STRENGTH_NAME_X
#define FREQ_SET_SIG_BER_NAME_Y       (FREQ_SET_SIG_STRENGTH_NAME_Y + FREQ_SET_SIG_STRENGTH_NAME_H + FREQ_SET_V_GAP)
#define FREQ_SET_SIG_BER_NAME_W       FREQ_SET_SIG_STRENGTH_NAME_W
#define FREQ_SET_SIG_BER_NAME_H       FREQ_SET_SIG_STRENGTH_NAME_H

//signal BER percent
#define FREQ_SET_SIG_BER_PERCENT_X    FREQ_SET_SIG_STRENGTH_PERCENT_X
#define FREQ_SET_SIG_BER_PERCENT_Y    FREQ_SET_SIG_BER_NAME_Y
#define FREQ_SET_SIG_BER_PERCENT_W    FREQ_SET_SIG_STRENGTH_PERCENT_W
#define FREQ_SET_SIG_BER_PERCENT_H    FREQ_SET_SIG_STRENGTH_PERCENT_H

//signal BER pbar
#define FREQ_SET_SIG_BER_PBAR_X       FREQ_SET_SIG_STRENGTH_PBAR_X
#define FREQ_SET_SIG_BER_PBAR_Y       (FREQ_SET_SIG_BER_NAME_Y + (FREQ_SET_SIG_BER_NAME_H - FREQ_SET_SIG_BER_PBAR_H)/2)
#define FREQ_SET_SIG_BER_PBAR_W       FREQ_SET_SIG_STRENGTH_PBAR_W
#define FREQ_SET_SIG_BER_PBAR_H       FREQ_SET_SIG_STRENGTH_PBAR_H

//signal SNR name
#define FREQ_SET_SIG_SNR_NAME_X       FREQ_SET_SIG_STRENGTH_NAME_X
#define FREQ_SET_SIG_SNR_NAME_Y       (FREQ_SET_SIG_BER_NAME_Y + FREQ_SET_SIG_BER_NAME_H + FREQ_SET_V_GAP)
#define FREQ_SET_SIG_SNR_NAME_W       FREQ_SET_SIG_STRENGTH_NAME_W
#define FREQ_SET_SIG_SNR_NAME_H       FREQ_SET_SIG_STRENGTH_NAME_H

//signal SNR percent
#define FREQ_SET_SIG_SNR_PERCENT_X    FREQ_SET_SIG_STRENGTH_PERCENT_X
#define FREQ_SET_SIG_SNR_PERCENT_Y    FREQ_SET_SIG_SNR_NAME_Y
#define FREQ_SET_SIG_SNR_PERCENT_W    FREQ_SET_SIG_STRENGTH_PERCENT_W
#define FREQ_SET_SIG_SNR_PERCENT_H    FREQ_SET_SIG_STRENGTH_PERCENT_H

//signal SNR pbar
#define FREQ_SET_SIG_SNR_PBAR_X       FREQ_SET_SIG_STRENGTH_PBAR_X
#define FREQ_SET_SIG_SNR_PBAR_Y       (FREQ_SET_SIG_SNR_NAME_Y + (FREQ_SET_SIG_SNR_NAME_H - FREQ_SET_SIG_SNR_PBAR_H)/2)
#define FREQ_SET_SIG_SNR_PBAR_W       FREQ_SET_SIG_STRENGTH_PBAR_W
#define FREQ_SET_SIG_SNR_PBAR_H       FREQ_SET_SIG_STRENGTH_PBAR_H


/* rect style */
#define RSI_FREQ_SET_LIST              RSI_TIP_BOX
#define RSI_FREQ_SET_ITEM              RSI_SUB_BUTTON_SH
#define RSI_FREQ_SET_PBAR_BG           RSI_PROGRESS_BAR_BG
#define RSI_FREQ_SET_PBAR_MID          RSI_PROGRESS_BAR_MID_PURPLE

/* others */

RET_CODE open_freq_set(u32 para1, u32 para2);

#endif


