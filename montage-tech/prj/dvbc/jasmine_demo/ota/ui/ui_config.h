/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/

#ifndef __PROJECT_CONFIG_H__
#define __PROJECT_CONFIG_H__

//#define UI_DEBUG

#ifdef UI_DEBUG
#define UI_PRINTF    OS_PRINTF
#else
#ifndef WIN32
#define UI_PRINTF(param ...)    do { } while (0)
#else
#define UI_PRINTF
#endif
#endif

enum user_msg
{
  MSG_POWER_ON = MSG_USER_BEGIN,
  MSG_POWER_OFF,
  MSG_TIMER_EXPIRED,
  MSG_CLOSE_MENU,       /* only for the system timer with the attribute of autoclose */
  MSG_EXIT_ALL,
  MSG_MUTE,
  MSG_PAUSE,
  MSG_AUTO_SWITCH,
  MSG_TVSYS_SWITCH,
  MSG_OPEN_MENU,
  MSG_BLACK_SCREEN,
  MSG_1ST_PIC,
  MSG_CHK_PWD,
  MSG_CORRECT_PWD,
  MSG_WRONG_PWD,

  MSG_SUBT,
  MSG_AUTO_TEST,

  MSG_BEEPER,
  //MSG_UPDATE,
  MSG_T9_CHANGED,
};

#define MAX_APP_CNT    (APP_LAST - APP_FRAMEWORK)

//user shift key define
enum user_shfit_msg
{
  MSG_OPEN_MENU_IN_TAB = MSG_SHIFT_USER_BEGIN,
};

// desktop config
#define SCREEN_WIDTH              960 //1280
#define SCREEN_HEIGHT             600//720

#define SCREEN_9PIC_WIDTH         960//1280
#define SCREEN_9PIC_HEIGHT        600//720

#define SCREEN_POS_PAL_L          ((1280 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_PAL_T          ((720 - SCREEN_HEIGHT) / 2)
#define SCREEN_POS_NTSC_L         ((1280 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_NTSC_T         ((720 - SCREEN_HEIGHT) / 2)

#define SCREEN_9PIC_POS_PAL_L     ((1280 - SCREEN_9PIC_WIDTH) / 2)
#define SCREEN_9PIC_POS_PAL_T     ((720 - SCREEN_9PIC_HEIGHT) / 2)
#define SCREEN_9PIC_POS_NTSC_L    ((1280 - SCREEN_9PIC_WIDTH) / 2)
#define SCREEN_9PIC_POS_NTSC_T    ((720 - SCREEN_9PIC_HEIGHT) / 2)


#define MAX_ROOT_CONT_CNT         6
#define MAX_MESSAGE_CNT           64
#define MAX_HOST_CNT              64

#define DST_IDLE_TMOUT            -1
#define POST_MSG_TMOUT            0     //100 ms
#define RECEIVE_MSG_TMOUT         0     //100 ms

// gdi config
#define MAX_CLIPRECT_CNT          128
#define MAX_SURFACE_CNT           3
#define MAX_DC_CNT                5

#define MAX_CLASS_CNT             11
#define MAX_OBJ_CNT               (MAX_OBJECT_NUMBER_CNT +  \
                                   MAX_OBJECT_NUMBER_TXT +  \
                                   MAX_OBJECT_NUMBER_BMP +  \
                                   MAX_OBJECT_NUMBER_CBB +  \
                                   MAX_OBJECT_NUMBER_LIST + \
                                   MAX_OBJECT_NUMBER_MTB +  \
                                   MAX_OBJECT_NUMBER_PGB +  \
                                   MAX_OBJECT_NUMBER_SCB +  \
                                   MAX_OBJECT_NUMBER_EDT +  \
                                   MAX_OBJECT_NUMBER_NUM + \
                                   MAX_OBJECT_NUMBER_TIM)

//how many control will be created at the same time
#define MAX_OBJECT_NUMBER_CNT     60
#define MAX_OBJECT_NUMBER_TXT     30
#define MAX_OBJECT_NUMBER_BMP     0
#define MAX_OBJECT_NUMBER_CBB     5
#define MAX_OBJECT_NUMBER_LIST    0
#define MAX_OBJECT_NUMBER_MTB     0
#define MAX_OBJECT_NUMBER_PGB     2
#define MAX_OBJECT_NUMBER_SCB     0
#define MAX_OBJECT_NUMBER_EDT     0
#define MAX_OBJECT_NUMBER_NUM     10
#define MAX_OBJECT_NUMBER_TIM     0

// db

//string length
#define MAX_CTRL_STRING_BUF_SIZE    2048
#define MAX_PAINT_STRING_LENGTH     128
#define MAX_PAINT_STRING_LINES        10

// others
#define AP_VOLUME_MAX            31
#define SS_PWD_LENGTH            4

#define DEFAULT_YEAR             2009
#define DEFAULT_MONTH            1
#define DEFAULT_DAY              1

#define SECOND                   1000


#endif

