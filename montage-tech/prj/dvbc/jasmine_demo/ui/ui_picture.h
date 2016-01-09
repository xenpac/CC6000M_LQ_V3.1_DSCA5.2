/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_PICTURE_H_
#define _UI_PICTURE_H_

#include "ui_common.h"

/* coordinate */
#define PICTURE_CONT_X             ((SCREEN_WIDTH - PICTURE_CONT_W) / 2)
#define PICTURE_CONT_Y             ((SCREEN_HEIGHT - PICTURE_CONT_H) / 2)
#define PICTURE_CONT_W             SCREEN_WIDTH
#define PICTURE_CONT_H             SCREEN_HEIGHT

//title icons
#define PICTURE_PAUSE_X  (PICTURE_CONT_W-200)
#define PICTURE_PAUSE_Y  (PICTURE_CONT_H-200)
#define PICTURE_PAUSE_W  56
#define PICTURE_PAUSE_H  56

typedef  RET_CODE (*OBJ_PICTURE_CB)();

typedef struct
{
  BOOL is_fav_pic;
  OBJ_PICTURE_CB cb;
} picture_param_t;


RET_CODE open_picture(u32 para1, u32 para2);

#endif
