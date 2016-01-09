/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#ifndef __UI_COMM_PWDLG_H__
#define __UI_COMM_PWDLG_H__

#define PWDLG_T_COMMON       0  // super password or common password can pass , 4 bits
#define PWDLG_T_CA           1  // ca password can pass , 6 bits
#define PWDLG_T_SUPER        2  // super password can pass , 4 bits
#define PWDLG_T_IPPV           3  // ca password can pass , 6 bits

#define PWDLG_COMMON_X       370
#define PWDLG_COMMON_Y       350

#define PWD_LENGTH_COMMON    4
#define PWD_LENGTH_SUPER     4
#define PWD_LENGTH_IPPV 6

#define PWDLG_W             420//390// 342
#define PWDLG_H              160

#define PWDLG_TXT1_L         10
#define PWDLG_TXT1_T         20
#define PWDLG_TXT1_W        (PWDLG_W - 2 * PWDLG_TXT1_L)
#define PWDLG_TXT1_H         30

#define PWDLG_EDIT_L         100
#define PWDLG_EDIT_T         65
#define PWDLG_EDIT_W        (PWDLG_W - 2 * PWDLG_EDIT_L)
#define PWDLG_EDIT_H         40

#define PWDLG_HELP_L         0
#define PWDLG_HELP_T         (PWDLG_H - PWDLG_HELP_H - 10)
#define PWDLG_HELP_W         PWDLG_W
#define PWDLG_HELP_H         30

#define FSI_PWDLG_TXT        FSI_WHITE
#define FSI_PWDLG_HELP       FSI_WHITE

#define RSI_PWDLG_CONT       RSI_WINDOW_GAME

#define PWDLG_EDIT_ICON_W    40
#define PWDLG_EDIT_ICON_H    40
#define PWDLG_EDIT_MBOX_V    10

typedef struct
{
  u8 parent_root;
  u16                   left;
  u16                   top;
  u16                   strid;
  keymap_t keymap;
  msgproc_t proc;
  u8         password_type;
}comm_pwdlg_data_t;

BOOL ui_comm_pwdlg_open(comm_pwdlg_data_t* p_data);
void ui_comm_pwdlg_close(void);

#endif
