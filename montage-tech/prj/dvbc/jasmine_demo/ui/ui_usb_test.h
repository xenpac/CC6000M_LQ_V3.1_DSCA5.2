/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_USB_TEST_H__
#define __UI_USB_TEST_H__


#define USB_TEST_ITEM_CNT        7

#define USB_TEST_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define USB_TEST_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define USB_TEST_ITEM_W       COMM_ITEM_MAX_WIDTH
#define USB_TEST_ITEM_H        COMM_ITEM_H
#define USB_TEST_ITEM_V_GAP    16

/* rect style */
#define RSI_USB_TEST_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_usb_test(u32 para1, u32 para2);

#endif




