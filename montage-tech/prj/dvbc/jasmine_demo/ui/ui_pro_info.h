/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_PRO_INFO_H__
#define __UI_PRO_INFO_H__

#define PRO_INFO_X             ((PRO_INFO_W-SCREEN_WIDTH)/2)
#define PRO_INFO_Y             ((PRO_INFO_H-SCREEN_HEIGHT)/2)
#define PRO_INFO_W             COMM_BG_W
#define PRO_INFO_H             COMM_BG_H

#define PRO_INFO_CHANNEL_TYPE_X              170
#define PRO_INFO_CHANNEL_TYPE_Y              140
#define PRO_INFO_CHANNEL_TYPE_W              120
#define PRO_INFO_CHANNEL_TYPE_H              30

#define PRO_INFO_COLUMN0_W                     130
#define PRO_INFO_COLUMN1_W                     240//400
#define PRO_INFO_COLUMN2_W                     160
#define PRO_INFO_COLUMN3_W                     240//400

#define PRO_INFO_COLUMN_HGAP                   6
#define PRO_INFO_ROW_VGAP                      20

#define SIG_STRENGTH_V_GAP 16

//signal strength0
#define SIG_STRENGTH_NAME_X       170
#define SIG_STRENGTH_NAME_Y       450
#define SIG_STRENGTH_NAME_W       150
#define SIG_STRENGTH_NAME_H       30

#define SIG_STRENGTH_PERCENT_X    (SIG_STRENGTH_NAME_X + SIG_STRENGTH_NAME_W)
#define SIG_STRENGTH_PERCENT_Y    SIG_STRENGTH_NAME_Y
#define SIG_STRENGTH_PERCENT_W    90
#define SIG_STRENGTH_PERCENT_H    30

#define SIG_STRENGTH_PBAR_X    (SIG_STRENGTH_PERCENT_X + SIG_STRENGTH_PERCENT_W)
#define SIG_STRENGTH_PBAR_Y    (SIG_STRENGTH_NAME_Y + (SIG_STRENGTH_NAME_H - SIG_STRENGTH_PBAR_H)/2)
#define SIG_STRENGTH_PBAR_W       (COMM_BG_W - 2 * SIG_STRENGTH_NAME_X - SIG_STRENGTH_NAME_W - SIG_STRENGTH_PERCENT_W)
#define SIG_STRENGTH_PBAR_H       16

//signal strength1
#define SIG_SET_NAME_X       SIG_STRENGTH_NAME_X
#define SIG_SET_NAME_Y       (SIG_STRENGTH_NAME_Y+SIG_STRENGTH_NAME_H+SIG_STRENGTH_V_GAP)
#define SIG_SET_NAME_W       SIG_STRENGTH_NAME_W
#define SIG_SET_NAME_H       SIG_STRENGTH_NAME_H

#define SIG_SET_PERCENT_X    (SIG_SET_NAME_X + SIG_SET_NAME_W)
#define SIG_SET_PERCENT_Y    SIG_SET_NAME_Y
#define SIG_SET_PERCENT_W    SIG_STRENGTH_PERCENT_W
#define SIG_SET_PERCENT_H    SIG_STRENGTH_PERCENT_H

#define SIG_SET_PBAR_X       (SIG_SET_PERCENT_X + SIG_SET_PERCENT_W)
#define SIG_SET_PBAR_Y       (SIG_SET_NAME_Y + (SIG_SET_NAME_H - SIG_SET_PBAR_H)/2)
#define SIG_SET_PBAR_W       SIG_STRENGTH_PBAR_W
#define SIG_SET_PBAR_H       SIG_STRENGTH_PBAR_H

//signal strength2
#define SIG_SNR_NAME_X       SIG_STRENGTH_NAME_X
#define SIG_SNR_NAME_Y       (SIG_SET_NAME_Y+SIG_STRENGTH_NAME_H+SIG_STRENGTH_V_GAP)
#define SIG_SNR_NAME_W       SIG_STRENGTH_NAME_W
#define SIG_SNR_NAME_H       SIG_STRENGTH_NAME_H

#define SIG_SNR_PERCENT_X    (SIG_SNR_NAME_X + SIG_SNR_NAME_W)
#define SIG_SNR_PERCENT_Y    SIG_SNR_NAME_Y
#define SIG_SNR_PERCENT_W    SIG_STRENGTH_PERCENT_W
#define SIG_SNR_PERCENT_H    SIG_STRENGTH_PERCENT_H

#define SIG_SNR_PBAR_X       (SIG_SNR_PERCENT_X + SIG_SNR_PERCENT_W)
#define SIG_SNR_PBAR_Y       (SIG_SNR_NAME_Y + (SIG_SNR_NAME_H - SIG_SNR_PBAR_H)/2)
#define SIG_SNR_PBAR_W       SIG_STRENGTH_PBAR_W
#define SIG_SNR_PBAR_H       SIG_STRENGTH_PBAR_H

RET_CODE open_pro_info(u32 para1, u32 para2);

#endif

