/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 ****************************************************************************/
#ifndef __UI_MENU_MANAGER_H__
#define __UI_MENU_MANAGER_H__

enum ui_root_id
{
  ROOT_ID_BACKGROUND = 1,
  ROOT_ID_OTA_USER_INPUT =2,
  ROOT_ID_OTA_UPGRADE = 3,
  ROOT_ID_POPUP = 4,
};


#define PS_KEEP                 0 // keep status
#define PS_PLAY                 1 // play full screen
#define PS_PREV                 2 // preview play with logo
#define PS_LOGO                 3 // stop and show logo
#define PS_STOP                 4 // stop

#define OFF                     0
#define ON                      1

#define SM_OFF                  0 // needn't signal message
#define SM_LOCK                 1 // need message when lock status changed
#define SM_BAR                  2 // need message always

#define MENU_TYPE_FULLSCREEN    0
#define MENU_TYPE_PREVIEW       1
#define MENU_TYPE_NORMAL        2

typedef  s32 (*OPEN_MENU)(u32 para1, u32 para2);

struct menu_attr
{
  u8        root_id;
  u8        play_state;         //PS_
  u8        auto_close;         //OFF,ON
  u8        signal_msg;         //SM_
  OPEN_MENU open_function;      // open function
};

struct logo_attr
{
  u8  root_id;
  u32 logo_id;
};

struct preview_attr
{
  u8          root_id;
  rect_t position;
};

struct msgmap
{
  u32 ext_msg;
  u16 int_msg;
};

struct extmsg_mapdata
{
  u8 root_id;
  u16 cnt;
  struct msgmap *map;
};

RET_CODE ui_menu_manage(u32 event, u32 para1, u32 para2);

RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2);
#endif

