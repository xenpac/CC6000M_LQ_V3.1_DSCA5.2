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
  MSG_T9_CHANGED,
  MSG_INPUT_CHANGED,
  MSG_HIDE_KEYBOARD,

  MSG_RENAME_CHECK,
  MSG_RENAME_UPDATE,  
  MSG_FAV_UPDATE,
  MSG_SORT_UPDATE,
  MSG_NUM_SELECT,
  MSG_TIME_UPDATE,
  MSG_TVRADIO,
  MSG_KEY_UPDATE,
  MSG_LNB_SHORT,
  MSG_TEST_MENU_UPDATE,
  MSG_SLEEP_TMROUT,
  MSG_PLUG_IN,
  MSG_PLUG_OUT,
  MSG_USB_DEV_PLUG_IN,
  MSG_SEARCH_UPDATE,
  MSG_SEARCH_CHECK,
  MSG_UIO_EVENT,
  MSG_TIMER_OPEN,
  MSG_MEDIA_PLAY_TMROUT,
  MSG_CA_ZONE_CHECK_END,
  MSG_HIDE_ROLL,
  MSG_REFRESH_ADS_PIC,
  MSG_CA_HIDE_FINGER_MID,
  MSG_CA_HIDE_FP,
  MSG_CA_FORCETUNE,
  MSG_CA_ROLLING_MSG,
  MSG_PRESET_PG_FOR_AE, // for ae test
  MSG_CLOSE_ROLLING_MSG,
  MSG_DESKTOP_ROLLING_START,
  MSG_DESKTOP_ROLLING_STOP,
  MSG_WATCH_LIMIT_REBOOT,
  MSG_FINGER_HEART_BEAT,
  MSG_FINGER_PRINTF_OFF,
  MSG_FINGER_SHOW_TIME,
  MSG_FINGER_HIDE_TIME,
  MSG_FINGER_MOVE,

  MSG_DRAW_ROLL_TITLE,
  MSG_START_TR_TITLE,
  
  MSG_AD_BLINK_HANGAD,
  MSG_AD_HIDE_HANGAD,

  MSG_SUBT_DRAW_RECT,
  MSG_SUBT_DRAW_IMAGE,
  MSG_FACTORY_UART_TEST,
  MSG_POWER_OFF_FALSE,

  MSG_ADS_TO_UI,
  MSG_CORNER_LOGO_CLOSE,
  MSG_CARD_INIT_SUCCESS,
  MSG_START_FEEDING,
  MSG_STOP_FEEDING,
  MSG_MOTHER_CARD_REGU_INSERT,
  MSG_MOTHER_CARD_INVALID,
  MSG_SON_CARD_REGU_INSERT,
  MSG_SON_CARD_INVALID,
  MSG_MS_CARD_FEED_SUCCESS,
  MSG_SYS_SET,
  MSG_LOAD_PG_FROM_USB,
  MSG_EMON_SMC_FINGER,
  MSG_EMON_HIDE_FINGER,
  MSG_EXIT_MOSAIC_PROG_PLAY,
#ifdef ENABLE_NETWORK
//network
  MSG_PING_TEST,
  MSG_CONFIG_TCPIP,
  MSG_INTERNET_PLUG_IN,
  MSG_INTERNET_PLUG_OUT,
  MSG_EDIT_IP_PATH_CHECK,
  MSG_EDIT_IP_PATH_UPDATE,
  MSG_EDIT_CITY_CHECK,
  MSG_EDIT_CITY_UPDATE, 
  MSG_NET_DEV_MOUNT, 
  MSG_NETWORK_MOUNT_SUCCESS,
  MSG_NETWORK_MOUNT_FAIL,
  MSG_CONFIG_IP,
  MSG_GET_WIFI_AP_CNT,
  MSG_WIFI_AP_CONNECT,
  MSG_WIFI_AP_DISCONNECT,
  MSG_WIFI_AP_CONNECTING,
  MSG_WIFI_AP_CONNECT_FAIL,
  MSG_GPRS_STATUS_UPDATE,
  MSG_G3_STATUS_UPDATE,
  MSG_GET_3G_INFO,
  MSG_G3_READY,
  MSG_MUSIC_CHANGE_MODE,
  MSG_CONNECT_NETWORK,
  MSG_UPDATE_MODE,
  MSG_ON_OFF_SATIP_SUCCESS,
  MSG_ON_OFF_SATIP_FAILED,
  MSG_ON_OFF_DLNA_SUCCESS,
  MSG_ON_OFF_DLNA_FAILED,
  MSG_SATIP_CLIENT_REQ_CHANGE_PG,
  MSG_UI_REFRESH,
  MSG_OPEN_IPTV_CUS,
#endif
};

//user shift key define
enum user_shfit_msg
{
  MSG_OPEN_MENU_IN_TAB = MSG_SHIFT_USER_BEGIN + 0x0100, // increase 0x100 once.
};

// desktop config
#define SCREEN_WIDTH              (960 + 2*SCREEN_POS_X)
#define SCREEN_HEIGHT             (600 + 2*SCREEN_POS_Y)

#ifdef ENABLE_NETWORK
#define SCREEN_WIDTH_ADD              0 //1280
#define SCREEN_HEIGHT_ADD             0//720

#define MENU_WIDTH_ADD            UI_COMMON_X
#define MENU_HEIGHT_ADD           UI_COMMON_Y

//current mode
enum device_mode
{
  MODE_DLNA,
  MODE_NORMAL
};
#endif

#define UI_COMMON_WIDTH           (960)
#define UI_COMMON_HEIGHT          (600)
#define UI_COMMON_X               ((SCREEN_WIDTH - UI_COMMON_WIDTH) / 2)
#define UI_COMMON_Y               ((SCREEN_HEIGHT - UI_COMMON_HEIGHT) / 2)

#define SCREEN_9PIC_WIDTH         960//1280
#define SCREEN_9PIC_HEIGHT        600//720

#define SCREEN_FULL_PIC_WIDTH         1280
#define SCREEN_FULL_PIC_HEIGHT        720

#define SCREEN_POS_PAL_L          ((1280 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_PAL_T          ((720 - SCREEN_HEIGHT) / 2)
#define SCREEN_POS_NTSC_L         ((1280 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_NTSC_T         ((720 - SCREEN_HEIGHT) / 2)

#define SCREEN_9PIC_POS_PAL_L     ((1280 - SCREEN_9PIC_WIDTH) / 2)
#define SCREEN_9PIC_POS_PAL_T     ((720 - SCREEN_9PIC_HEIGHT) / 2)
#define SCREEN_9PIC_POS_NTSC_L    ((1280 - SCREEN_9PIC_WIDTH) / 2)
#define SCREEN_9PIC_POS_NTSC_T    ((720 - SCREEN_9PIC_HEIGHT) / 2)

#define MAX_ROOT_CONT_CNT         12
#define MAX_MESSAGE_CNT           64
#define MAX_HOST_CNT              128
#define MAX_TMR_CNT               32

#define DST_IDLE_TMOUT            (-1)
#define POST_MSG_TMOUT            0     //100 ms
#define RECEIVE_MSG_TMOUT         0     //100 ms

// gdi config
#define MAX_CLIPRECT_CNT          512
#define MAX_SURFACE_CNT           (MAX_ROLL_CNT + 1)
//roll
#define MAX_ROLL_CNT              10

#ifdef HW_ROLL_TIMER
#define ROLL_PPS                  2
#else
#define ROLL_PPS                  4
#endif

#define MAX_DC_CNT                6

#define MAX_CLASS_CNT             12
#ifdef ENABLE_NETWORK
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
                                   MAX_OBJECT_NUMBER_TIM + \
                                   MAX_OBJECT_NUMBER_SBX + \
                                   MAX_OBJECT_NUMBER_IP )
#else
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
                                   MAX_OBJECT_NUMBER_TIM + \
                                   MAX_OBJECT_NUMBER_SBX)
#endif

//how many control will be created at the same time
#define MAX_OBJECT_NUMBER_CNT     64
#define MAX_OBJECT_NUMBER_TXT     64
#define MAX_OBJECT_NUMBER_BMP     64
#define MAX_OBJECT_NUMBER_CBB     16
#define MAX_OBJECT_NUMBER_LIST    16
#define MAX_OBJECT_NUMBER_MTB     16
#define MAX_OBJECT_NUMBER_PGB     8
#define MAX_OBJECT_NUMBER_SCB     16
#define MAX_OBJECT_NUMBER_EDT     8
#define MAX_OBJECT_NUMBER_NUM     32
#define MAX_OBJECT_NUMBER_TIM     8
#define MAX_OBJECT_NUMBER_SBX     8
#define MAX_OBJECT_NUMBER_BLK     8
#define MAX_OBJECT_NUMBER_ALB     8
#ifdef ENABLE_NETWORK
#define MAX_OBJECT_NUMBER_IP       8
#endif


// db

//string length
#define MAX_CTRL_STRING_BUF_SIZE    8192
#ifndef ABV_CA
#define MAX_PAINT_STRING_LENGTH     1024
#else
#define MAX_PAINT_STRING_LENGTH     1152
#endif
#define MAX_PAINT_STRING_LINES        14

// others
#define AP_VOLUME_MAX            31
#ifdef SMSX_CA
#define SS_PWD_LENGTH            8
#else

#ifdef ENABLE_CONAXK
#define SS_PWD_LENGTH            4
#else
#define SS_PWD_LENGTH            6
#endif
#endif
#define DEFAULT_YEAR             2009
#define DEFAULT_MONTH            1
#define DEFAULT_DAY              1

#define SECOND                   1000

#define DEFAULT_TMOUT_MS          (10*SECOND)

#define INVALID_U32               0xFFFFFFFF
#endif

