/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef _UI_BROWSE_API_H
#define _UI_BROWSE_API_H

enum browser_msg
{
  MSG_BROWSER_STOP = MSG_EXTERN_BEGIN + 600,
  MSG_BROWSER_PLAY_MUSIC,
  MSG_BROWSER_STOP_MUSIC,
  MSG_BROWSER_EXIT,
  MSG_BROWSER_IRKEY_POWER,
  MSG_BROWSER_IRKEY_0,
  MSG_BROWSER_IRKEY_1,
  MSG_BROWSER_IRKEY_2,
  MSG_BROWSER_IRKEY_3,
  MSG_BROWSER_IRKEY_4,
  MSG_BROWSER_IRKEY_5,
  MSG_BROWSER_IRKEY_6,
  MSG_BROWSER_IRKEY_7,
  MSG_BROWSER_IRKEY_8,
  MSG_BROWSER_IRKEY_9,
  MSG_BROWSER_IRKEY_UP,
  MSG_BROWSER_IRKEY_DOWN,
  MSG_BROWSER_IRKEY_LEFT,
  MSG_BROWSER_IRKEY_RIGHT,
  MSG_BROWSER_IRKEY_SELECT,
  MSG_BROWSER_IRKEY_LAST_CHANNEL,
  MSG_BROWSER_IRKEY_VOL_UP,
  MSG_BROWSER_IRKEY_VOL_DOWN,
  MSG_BROWSER_IRKEY_MUTE,
  MSG_BROWSER_IRKEY_MENU,
  MSG_BROWSER_IRKEY_EXIT,
  MSG_BROWSER_IRKEY_BACK,
  MSG_BROWSER_IRKEY_HELP,
  MSG_BROWSER_IRKEY_PLAY,
  MSG_BROWSER_IRKEY_STOP,
  MSG_BROWSER_IRKEY_PAUSE,
  MSG_BROWSER_IRKEY_REC,
  MSG_BROWSER_IRKEY_FASTFORWARD,
  MSG_BROWSER_IRKEY_REWIND,
  MSG_BROWSER_IRKEY_HOMEPAGE,
  MSG_BROWSER_IRKEY_REFRESH,
  MSG_BROWSER_IRKEY_PAGE_UP,
  MSG_BROWSER_IRKEY_RAGE_DN,
  MSG_BROWSER_IRKEY_ESC,
  MSG_BROWSER_IRKEY_INFO,
  MSG_BROWSER_IRKEY_CH_UP,
  MSG_BROWSER_IRKEY_CH_DN,
  MSG_BROWSER_IRKEY_RED,
  MSG_BROWSER_IRKEY_YELLOW,
  MSG_BROWSER_IRKEY_GREEN,
  MSG_BROWSER_IRKEY_BLUE,
  MSG_BROWSER_IRKEY_FORWARD,
  MSG_BROWSER_IRKEY_BACKWARD,
  MSG_BROWSER_IRKEY_IME,
  MSG_BROWSER_IRKEY_LANGUAGE,
 };

void ui_browser_init(void);
void ui_browser_release(void);

void ui_browser_start(void);

void ui_browser_enter(void);
void ui_browser_exit(void);

#endif

