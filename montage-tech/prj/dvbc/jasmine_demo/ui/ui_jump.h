/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_JUMP_H__
#define __UI_JUMP_H__

/* coordinate */
#define JUMP_CONT_X			((COMM_BG_W-JUMP_CONT_W)/2)
#define JUMP_CONT_H			420

#define JUMP_ITEM_CNT		  1
#define JUMP_ITEM_X			  COMM_ITEM_OX_IN_ROOT
#define JUMP_ITEM_Y			  COMM_ITEM_OY_IN_ROOT
#define JUMP_ITEM_LW		    COMM_ITEM_LW
#define JUMP_ITEM_RW		    (COMM_ITEM_MAX_WIDTH - JUMP_ITEM_LW)
#define JUMP_ITEM_H			  COMM_ITEM_H
#define JUMP_ITEM_V_GAP		2

#define JUMP_SYSTEM_VGAP   40

#define JUMP_SYS_X			 JUMP_ITEM_X
#define JUMP_SYS_Y			 (JUMP_SYSTEM_VGAP + JUMP_ITEM_Y + JUMP_ITEM_CNT * (JUMP_ITEM_H + JUMP_ITEM_V_GAP)) 
#define JUMP_SYS_W		   200
#define JUMP_SYS_H		   40

#define JUMP_NAME_X		 (JUMP_SYS_X + JUMP_SYS_W)
#define JUMP_NAME_Y     JUMP_SYS_Y
#define JUMP_NAME_W		 200
#define JUMP_NAME_H		 40


/* rect style */
#define RSI_JUMP_FRM		RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_jump (u32 para1, u32 para2);
u16 ui_jump_para_get(void);
#endif

