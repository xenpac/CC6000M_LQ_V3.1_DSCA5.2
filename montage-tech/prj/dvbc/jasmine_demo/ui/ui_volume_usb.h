/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_VOLUME_USB_H__
#define __UI_VOLUME_USB_H__

#define VOLUME_USB_L			       ((SCREEN_WIDTH-VOLUME_USB_W)/2)//(PREV_COMMON_X + 10) //((SCREEN_WIDTH-VOLUME_USB_W)/2)
#define VOLUME_USB_T			       (530+SCREEN_POS_Y)//(PREV_COMMON_Y + PREV_COMMON_H -VOLUME_USB_H - 10)//580
#define VOLUME_USB_W		         820//( PREV_COMMON_W - 20)//  650//576
#define VOLUME_USB_H			       50//40

#define VOLUME_BAR_USB_CONT_X	       0
#define VOLUME_BAR_USB_CONT_Y   	0
#define VOLUME_BAR_USB_CONT_W      VOLUME_USB_W
#define VOLUME_BAR_USB_CONT_H	VOLUME_USB_H

#define VOLUME_BAR_USB_ICON_X	    20
#define VOLUME_BAR_USB_ICON_Y	    0
#define VOLUME_BAR_USB_ICON_W	    30
#define VOLUME_BAR_USB_ICON_H	    VOLUME_USB_H

#define VOLUME_BAR_USB_X	              (VOLUME_BAR_USB_ICON_X  + VOLUME_BAR_USB_ICON_W + 10)//10
#define VOLUME_BAR_USB_Y	              ((VOLUME_USB_H - VOLUME_BAR_USB_H)/2)
#define VOLUME_BAR_USB_W	              (VOLUME_BAR_USB_CONT_W -  2 * VOLUME_BAR_USB_X)
#define VOLUME_BAR_USB_H	              10

#define VOLUME_TEXT_USB_X	    (VOLUME_BAR_USB_X + VOLUME_BAR_USB_W + 5)
#define VOLUME_TEXT_USB_Y	    10
#define VOLUME_TEXT_USB_W	    50
#define VOLUME_TEXT_USB_H	    30

#define VOLUME_USB_MAX                     AP_VOLUME_MAX

/* rect style */
#define RSI_VOLUME_USB_CONT             RSI_IGNORE
#define RSI_VOLUME_BAR_USB_FRAME  RSI_COMMON_RECT1

void set_volume_usb(u8 volume);

RET_CODE open_volume_usb (u32 para1, u32 para2);

RET_CODE close_volume_usb(void);

//extern BOOL is_display_pg;

#endif


