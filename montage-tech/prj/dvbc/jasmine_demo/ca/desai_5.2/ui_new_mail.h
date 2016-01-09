/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_NEW_MAIL_H__
#define __UI_NEW_MAIL_H__

/* coordinate */
#define NEW_MAIL_CONT_FULL_X			100
#define NEW_MAIL_CONT_FULL_Y			60//100
#define NEW_MAIL_CONT_FULL_W			60
#define NEW_MAIL_CONT_FULL_H			60

#define NEW_MAIL_ICON_FULL_X			0
#define NEW_MAIL_ICON_FULL_Y			0
#define NEW_MAIL_ICON_FULL_W			NEW_MAIL_CONT_FULL_W
#define NEW_MAIL_ICON_FULL_H			NEW_MAIL_CONT_FULL_H

typedef enum
{
  EMAIL_PIC_SHOW = 0,
  EMAIL_PIC_HIDE,
  EMAIL_PIC_FLASH
}email_pic_show_mode_t;

BOOL ui_is_new_mail(void);

void ui_set_new_mail(BOOL is_new_mail);

BOOL ui_is_full_mail(void);

void ui_set_full_mail(BOOL is_full_mail);

RET_CODE open_ui_new_mail(u32 para1, u32 para2);

void close_new_mail(void);

void update_email_picture(email_pic_show_mode_t e_show_mode);
#endif


