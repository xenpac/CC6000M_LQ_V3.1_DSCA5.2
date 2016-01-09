/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_OTA_H__
#define __UI_OTA_H__

/* coordinate */
#define OTA_CONT_X        ((COMM_BG_W - OTA_CONT_W) / 2)
#define OTA_CONT_H        360

#ifndef DTMB_PROJECT
#define OTA_ITEM_CNT      5
#else
#define OTA_ITEM_CNT      3
#endif
#define OTA_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define OTA_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define OTA_ITEM_LW       COMM_ITEM_LW
#define OTA_ITEM_RW       (COMM_ITEM_MAX_WIDTH - OTA_ITEM_LW)
#define OTA_ITEM_H        COMM_ITEM_H
#define OTA_ITEM_V_GAP    0//16

/* rect style */
#define RSI_OTA_FRM       RSI_WINDOW_1

/* font style */

/* others */
#define OTA_FREQ_MIN 0
#define OTA_FREQ_MAX 99999
#define OTA_FREQ_BIT  5

#define OTA_SYM_MIN 0
#define OTA_SYM_MAX 99999
#define OTA_SYM_BIT 5

#define OTA_PID_MIN 0
#define OTA_PID_MAX 17777//0x1FFF
#define OTA_PID_BIT 5

#define OTA_V_GAP               15

//searching info frm
#define OTA_SIG_INFO_FRM_X        40
#define OTA_SIG_INFO_FRM_Y        (OTA_ITEM_Y + OTA_ITEM_CNT * (OTA_ITEM_H + OTA_ITEM_V_GAP) + OTA_V_GAP )//430
#define OTA_SIG_INFO_FRM_W        (COMM_BG_W - OTA_SIG_INFO_FRM_X)
#define OTA_SIG_INFO_FRM_H        140

//signal strength name
#define OTA_SIG_STRENGTH_NAME_X       100
#define OTA_SIG_STRENGTH_NAME_Y       0
#define OTA_SIG_STRENGTH_NAME_W       150
#define OTA_SIG_STRENGTH_NAME_H       30

//signal strength percent
#define OTA_SIG_STRENGTH_PERCENT_X    (OTA_SIG_STRENGTH_NAME_X + OTA_SIG_STRENGTH_NAME_W)
#define OTA_SIG_STRENGTH_PERCENT_Y    OTA_SIG_STRENGTH_NAME_Y
#define OTA_SIG_STRENGTH_PERCENT_W    90
#define OTA_SIG_STRENGTH_PERCENT_H    30

//signal strength pbar
#define OTA_SIG_STRENGTH_PBAR_X       (OTA_SIG_STRENGTH_PERCENT_X + OTA_SIG_STRENGTH_PERCENT_W)
#define OTA_SIG_STRENGTH_PBAR_Y       (OTA_SIG_STRENGTH_PERCENT_Y + (OTA_SIG_STRENGTH_NAME_H - OTA_SIG_STRENGTH_PERCENT_H)/2)
#define OTA_SIG_STRENGTH_PBAR_W       (COMM_BG_W - 2 * (OTA_SIG_INFO_FRM_X + OTA_SIG_STRENGTH_NAME_X) - OTA_SIG_STRENGTH_NAME_W - OTA_SIG_STRENGTH_PERCENT_W)
#define OTA_SIG_STRENGTH_PBAR_H       16

//signal BER name
#define OTA_SIG_BER_NAME_X       OTA_SIG_STRENGTH_NAME_X
#define OTA_SIG_BER_NAME_Y       (OTA_SIG_STRENGTH_NAME_Y + OTA_SIG_STRENGTH_NAME_H + OTA_V_GAP)
#define OTA_SIG_BER_NAME_W       OTA_SIG_STRENGTH_NAME_W
#define OTA_SIG_BER_NAME_H       OTA_SIG_STRENGTH_NAME_H

//signal BER percent
#define OTA_SIG_BER_PERCENT_X    OTA_SIG_STRENGTH_PERCENT_X
#define OTA_SIG_BER_PERCENT_Y    OTA_SIG_BER_NAME_Y
#define OTA_SIG_BER_PERCENT_W    OTA_SIG_STRENGTH_PERCENT_W
#define OTA_SIG_BER_PERCENT_H    OTA_SIG_STRENGTH_PERCENT_H

//signal BER pbar
#define OTA_SIG_BER_PBAR_X       OTA_SIG_STRENGTH_PBAR_X
#define OTA_SIG_BER_PBAR_Y       (OTA_SIG_BER_NAME_Y + (OTA_SIG_BER_NAME_H - OTA_SIG_BER_PBAR_H)/2)
#define OTA_SIG_BER_PBAR_W       OTA_SIG_STRENGTH_PBAR_W
#define OTA_SIG_BER_PBAR_H       OTA_SIG_STRENGTH_PBAR_H

//signal SNR name
#define OTA_SIG_SNR_NAME_X       OTA_SIG_STRENGTH_NAME_X
#define OTA_SIG_SNR_NAME_Y       (OTA_SIG_BER_NAME_Y + OTA_SIG_BER_NAME_H + OTA_V_GAP)
#define OTA_SIG_SNR_NAME_W       OTA_SIG_STRENGTH_NAME_W
#define OTA_SIG_SNR_NAME_H       OTA_SIG_STRENGTH_NAME_H

//signal SNR percent
#define OTA_SIG_SNR_PERCENT_X    OTA_SIG_STRENGTH_PERCENT_X
#define OTA_SIG_SNR_PERCENT_Y    OTA_SIG_SNR_NAME_Y
#define OTA_SIG_SNR_PERCENT_W    OTA_SIG_STRENGTH_PERCENT_W
#define OTA_SIG_SNR_PERCENT_H    OTA_SIG_STRENGTH_PERCENT_H

//signal SNR pbar
#define OTA_SIG_SNR_PBAR_X       OTA_SIG_STRENGTH_PBAR_X
#define OTA_SIG_SNR_PBAR_Y       (OTA_SIG_SNR_NAME_Y + (OTA_SIG_SNR_NAME_H - OTA_SIG_SNR_PBAR_H)/2)
#define OTA_SIG_SNR_PBAR_W       OTA_SIG_STRENGTH_PBAR_W
#define OTA_SIG_SNR_PBAR_H       OTA_SIG_STRENGTH_PBAR_H

#define RSI_OTA_PBAR_BG           RSI_PROGRESS_BAR_BG
#define RSI_OTA_PBAR_MID          RSI_PROGRESS_BAR_MID_ORANGE

RET_CODE open_ota(u32 para1, u32 para2);

#endif
