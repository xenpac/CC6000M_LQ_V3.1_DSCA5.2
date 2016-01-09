/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_STB_NAME_H__
#define __UI_STB_NAME_H__

/* coordinate */
#define STB_NAME_ITEM_CNT		1
#define STB_NAME_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define STB_NAME_ITEM_Y			COMM_ITEM_OY_IN_ROOT
#define STB_NAME_ITEM_LW		COMM_ITEM_LW
#define STB_NAME_ITEM_RW		(COMM_ITEM_MAX_WIDTH - STB_NAME_ITEM_LW)
#define STB_NAME_ITEM_H			COMM_ITEM_H
#define STB_NAME_ITEM_V_GAP		16

/* rect style */
#define STB_NAME_OTHERS_FRM		RSI_WINDOW_1
/* font style */

#define MAX_DEVICE_NAME_LENGTH 64

RET_CODE open_stbname (u32 para1, u32 para2);

#endif

