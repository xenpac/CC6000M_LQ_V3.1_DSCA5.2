/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_LEVEL_H__
#define __UI_CONDITIONAL_ACCEPT_LEVEL_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_LEVEL_CONT_X          COMM_ITEM_OX_IN_ROOT
#define CONDITIONAL_ACCEPT_LEVEL_CONT_Y          COMM_ITEM_OY_IN_ROOT
#define CONDITIONAL_ACCEPT_LEVEL_CONT_W          SCREEN_WIDTH
#define CONDITIONAL_ACCEPT_LEVEL_CONT_H          SCREEN_HEIGHT

//level control items
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT      3

#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X        CONDITIONAL_ACCEPT_LEVEL_CONT_X
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y        CONDITIONAL_ACCEPT_LEVEL_CONT_Y
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW       (COMM_ITEM_MAX_WIDTH/2)
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW       (COMM_ITEM_MAX_WIDTH/2)
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H        COMM_ITEM_H

#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP    20

#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X        (CONDITIONAL_ACCEPT_LEVEL_CONT_X + (COMM_ITEM_MAX_WIDTH - CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W)/2)
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W        100
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H        COMM_ITEM_H

//change level result
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X        CONDITIONAL_ACCEPT_LEVEL_CONT_X
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W       COMM_ITEM_MAX_WIDTH
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H        COMM_ITEM_H

/* others */
#define CONDITIONAL_ACCEPT_WATCH_LEVEL_TOTAL 9

RET_CODE open_conditional_accept_level(u32 para1, u32 para2);

#endif


