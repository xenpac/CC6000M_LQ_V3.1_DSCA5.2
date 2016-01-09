/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PROMPT_H__
#define __UI_CA_PROMPT_H__

#define CA_PROMPT_X             ((CA_PROMPT_W-CA_PROMPT_W)/2)
#define CA_PROMPT_Y             ((CA_PROMPT_H-CA_PROMPT_H)/2)
#define CA_PROMPT_W             COMM_BG_W
#define CA_PROMPT_H             COMM_BG_H

#define CA_PROMPT_CHANNEL_TYPE_X              100
#define CA_PROMPT_CHANNEL_TYPE_Y              90
#define CA_PROMPT_CHANNEL_TYPE_W              120
#define CA_PROMPT_CHANNEL_TYPE_H              30

#define CA_PROMPT_COLUMN0_W                     130
#define CA_PROMPT_COLUMN1_W                     120
#define CA_PROMPT_COLUMN2_W                     130
#define CA_PROMPT_COLUMN3_W                     120

#define CA_PROMPT_COLUMN_HGAP                   6
#define CA_PROMPT_ROW_VGAP                      10

//signal strength0
#define SIG_STRENGTH_NAME_X       90
#define SIG_STRENGTH_NAME_Y       370
#define SIG_STRENGTH_NAME_W       120
#define SIG_STRENGTH_NAME_H       30

#define SIG_STRENGTH_PERCENT_X    (SIG_STRENGTH_NAME_X + SIG_STRENGTH_NAME_W)
#define SIG_STRENGTH_PERCENT_Y    SIG_STRENGTH_NAME_Y
#define SIG_STRENGTH_PERCENT_W    100
#define SIG_STRENGTH_PERCENT_H    30

#define SIG_STRENGTH_PBAR_X    (SIG_STRENGTH_PERCENT_X + SIG_STRENGTH_PERCENT_W)
#define SIG_STRENGTH_PBAR_Y    (SIG_STRENGTH_NAME_Y + (SIG_STRENGTH_NAME_H - SIG_STRENGTH_PBAR_H)/2)
#define SIG_STRENGTH_PBAR_W       230
#define SIG_STRENGTH_PBAR_H       16

//signal strength1
#define SIG_SET_NAME_X       SIG_STRENGTH_NAME_X
#define SIG_SET_NAME_Y       (SIG_STRENGTH_NAME_Y+SIG_STRENGTH_NAME_H)
#define SIG_SET_NAME_W       120
#define SIG_SET_NAME_H       30

#define SIG_SET_PERCENT_X    (SIG_SET_NAME_X + SIG_SET_NAME_W)
#define SIG_SET_PERCENT_Y    SIG_SET_NAME_Y
#define SIG_SET_PERCENT_W    100
#define SIG_SET_PERCENT_H    30

#define SIG_SET_PBAR_X       (SIG_SET_PERCENT_X + SIG_SET_PERCENT_W)
#define SIG_SET_PBAR_Y       (SIG_SET_NAME_Y + (SIG_SET_NAME_H - SIG_SET_PBAR_H)/2)
#define SIG_SET_PBAR_W       230
#define SIG_SET_PBAR_H       16

//signal strength2
#define SIG_SNR_NAME_X       SIG_STRENGTH_NAME_X
#define SIG_SNR_NAME_Y       (SIG_SET_NAME_Y+SIG_STRENGTH_NAME_H)
#define SIG_SNR_NAME_W       120
#define SIG_SNR_NAME_H       30

#define SIG_SNR_PERCENT_X    (SIG_SNR_NAME_X + SIG_SNR_NAME_W)
#define SIG_SNR_PERCENT_Y    SIG_SNR_NAME_Y
#define SIG_SNR_PERCENT_W    100
#define SIG_SNR_PERCENT_H    30

#define SIG_SNR_PBAR_X       (SIG_SNR_PERCENT_X + SIG_SNR_PERCENT_W)
#define SIG_SNR_PBAR_Y       (SIG_SNR_NAME_Y + (SIG_SNR_NAME_H - SIG_SNR_PBAR_H)/2)
#define SIG_SNR_PBAR_W       230
#define SIG_SNR_PBAR_H       16

RET_CODE open_ca_prompt(u32 para1, u32 para2);

#endif

