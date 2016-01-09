/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_SIGNAL_H__
#define __UI_SIGNAL_H__

/* coordinate */
#define SIGNAL_CONT_FULL_X  ((SCREEN_WIDTH-SIGNAL_CONT_FULL_W)/2)
#define SIGNAL_CONT_FULL_Y  ((SCREEN_HEIGHT-SIGNAL_CONT_FULL_H)/2 -20)
#define SIGNAL_CONT_FULL_W  350
#define SIGNAL_CONT_FULL_H  140

#define SIGNAL_TXT_FULL_X   ((SIGNAL_CONT_FULL_W-SIGNAL_TXT_FULL_W)/2)
#define SIGNAL_TXT_FULL_Y   ((SIGNAL_CONT_FULL_H-SIGNAL_TXT_FULL_H)/2)
#define SIGNAL_TXT_FULL_W   (SIGNAL_CONT_FULL_W - 30)
#define SIGNAL_TXT_FULL_H   (SIGNAL_CONT_FULL_H - 20)

#define SIGNAL_CONT_PREVIEW_W 300
#define SIGNAL_CONT_PREVIEW_H 100

#define SIGNAL_TXT_PREVIEW_X  0
#define SIGNAL_TXT_PREVIEW_Y  0
#define SIGNAL_TXT_PREVIEW_W  SIGNAL_CONT_PREVIEW_W
#define SIGNAL_TXT_PREVIEW_H  SIGNAL_CONT_PREVIEW_H


void update_signal (void);

void close_signal(void);


void update_ca_message(u16 strid);

void update_encrypt_message();

void update_encrypt_message_radio();

u16 get_message_strid(void);
#if 0//#ifdef CA_SUPPORT
void clean_ca_prompt(void);
#endif

void clean_pg_scramble(void);
void open_signal(u16 content);
#endif


