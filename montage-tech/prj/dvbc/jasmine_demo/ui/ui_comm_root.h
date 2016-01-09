/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_COMM_BG_H__
#define __UI_COMM_BG_H__

/*********************************************************************
 * COMMON DEFINATION
 *********************************************************************/
#define IDC_COMM_TITLE_CONT     0xFE   /* special for title */
#define IDC_COMM_ROOT_CONT      0xFD   /* special for signbar */
#define IDC_COMM_SIGN_CONT      0xFC   /* special for signbar */
#define IDC_COMM_HELP_CONT      0xFB   /* special for signbar */

#define IDC_COMM_TITLE_ICON      0xFA   /* special for title icon */

#define IDC_COMM_TITLE_TEXT      0xF9   /* special for title text */




/*********************************************************************
 * COMMON COORDINATE
 *********************************************************************/
#define COMM_BG_X             0//147
#define COMM_BG_Y             0//71
#define COMM_BG_W             UI_COMMON_WIDTH //1280
#define COMM_BG_H             UI_COMMON_HEIGHT //720

#define COMM_ITEM_OX_IN_ROOT    120
#define COMM_ITEM_OY_IN_ROOT    130//120
#define COMM_ITEM_LW            400//700
#define COMM_ITEM_H             COMM_CTRL_H
#define COMM_ITEM_MAX_WIDTH     (COMM_BG_W - 2 * COMM_ITEM_OX_IN_ROOT)
#define COMM_SBAR_VERTEX_GAP    12

#define WIN_LR_SPACE      70

#define PREV_COMMON_X     WIN_LR_SPACE
#define PREV_COMMON_Y     120
#define PREV_COMMON_W     330//530
#define PREV_COMMON_H     220

#define COMM_WIN_SHIFT_X 160
#define COMM_WIN_SHIFT_Y 60

/*********************************************************************
 * COMMON TITLE
 *********************************************************************/
/* coordinate */
#define TITLE_W                 COMM_BG_W//1280
#define TITLE_H                 90//110

#define TITLE_TXT_X             0//120
#define TITLE_TXT_Y             15
#define TITLE_TXT_W            TITLE_W //490
#define TITLE_TXT_H             60//38

#define FSI_TITLE_TXT           FSI_WHITE_48

void ui_comm_title_create(control_t *parent, u16 strid, u32 rstyle);

void ui_comm_title_set_content(control_t *parent, u16 strid);

void ui_comm_title_update(control_t *parent);

void ui_comm_title_hide(control_t *parent);

/****************************************************
 * COMMON HELP BAR
 ****************************************************/

/* coordinate */
#define HELP_ICON_X          0
#define HELP_ICON_W          80

#define HELP_CONT_H_GAP      0//200
#define HELP_CONT_V_GAP      0

#define HELP_CONT_POP_DLG_H_GAP      10//16
#define HELP_CONT_POP_DLG_V_GAP      16

#define HELP_ITEM_H_GAP      6
#define HELP_ITEM_V_GAP      0
#define HELP_ITEM_MAX_CNT    16
#define HELP_ITEM_H          80

#define HELP_ITEM_ICON_W     36
#define HELP_ITEM_ICON_OX    0
#define HELP_ITEM_ICON_OY    0

#define HELP_ITEM_TEXT_OX    HELP_ITEM_ICON_W
#define HELP_ITEM_TEXT_OY    0

/* rect style */
#define RSI_HELP_CNT         RSI_PBACK

/* font style */
#define FSI_HELP_TEXT        FSI_WHITE
#define FSI_KEYBOARD_ITEM_HL        FSI_BLACK
#define FSI_KEYBOARD_ITEM_SH        FSI_GRAY



typedef struct
{
  u8 item_cnt;                        //item number
  u8 col_cnt;                         //item number per row
  u16 str_id[HELP_ITEM_MAX_CNT];      //string id
  u16 bmp_id[HELP_ITEM_MAX_CNT];      //bmp id
}comm_help_data_t;

void ui_comm_help_create(comm_help_data_t *p_data, control_t *p_parent);

void ui_comm_help_move_pos(control_t *p_parent, u16 x, u16 offset_top,
                           u16 w, u16 offset_bottom, u16 str_offset);

control_t * ui_comm_help_create_for_pop_dlg(comm_help_data_t *p_data, control_t *p_parent);

void ui_comm_help_set_data(comm_help_data_t *p_data, control_t *p_parent);

void ui_comm_help_update(control_t *p_parent);

void ui_comm_help_create_ext(u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               comm_help_data_t *p_data,
                               control_t *p_parent);

/*********************************************************************
 * COMMON ROOT
 *********************************************************************/

/* coordinate */
#define ROOT_SIGN_CONT_H             160//130
#define ROOT_SIGN_CONT_GAP           4

#define ROOT_SIGN_BAR_STXT_X         50
#define ROOT_SIGN_BAR_STXT_Y         10
#define ROOT_SIGN_BAR_STXT_W       200//160
#define ROOT_SIGN_BAR_STXT_H         30

#define ROOT_SIGN_BAR_X \
  (ROOT_SIGN_BAR_STXT_X + \
   ROOT_SIGN_BAR_STXT_W)
#define ROOT_SIGN_BAR_Y \
  (ROOT_SIGN_BAR_STXT_Y +  \
   (ROOT_SIGN_BAR_STXT_H - \
    ROOT_SIGN_BAR_H) / 2)
#define ROOT_SIGN_BAR_W \
  (COMM_BG_W - 2 *      \
   ROOT_SIGN_BAR_STXT_X - \
   ROOT_SIGN_BAR_STXT_W - \
   ROOT_SIGN_BAR_PERCENT_W)
#define ROOT_SIGN_BAR_H              16

#define ROOT_SIGN_BAR_PERCENT_X \
  (ROOT_SIGN_BAR_X + \
   ROOT_SIGN_BAR_W)
#define ROOT_SIGN_BAR_PERCENT_Y      ROOT_SIGN_BAR_STXT_Y
#define ROOT_SIGN_BAR_PERCENT_W      70
#define ROOT_SIGN_BAR_PERCENT_H      ROOT_SIGN_BAR_STXT_H

#define ROOT_SIGN_CTRL_V_GAP         0

/* rect style */
#define RSI_ROOT_SIGN_CONT           RSI_WINDOW_2
#define RSI_ROOT_SIGN_BAR            RSI_PROGRESS_BAR_BG
#define RSI_ROOT_SIGN_BAR_MID_1      RSI_PROGRESS_BAR_MID_ORANGE
#define RSI_ROOT_SIGN_BAR_MID_2      RSI_PROGRESS_BAR_MID_BLUE

#define RSI_ROOT_SIGN_BAR_STXT       RSI_PBACK
#define RSI_ROOT_SIGN_BAR_PERCENT    RSI_PBACK

/* font style */
#define FSI_ROOT_SIGN_BAR_STXT       FSI_WHITE
#define FSI_ROOT_SIGN_BAR_PERCENT    FSI_WHITE

/* others */
#define BEEPER_UNLKD_TMOUT           2000
#define BEEPER_LOCKD_TMOUT           1000

#define HELP_CONT_X      30
#define HELP_CONT_Y      535//640
#define HELP_CONT_W      (COMM_BG_W - 2* HELP_CONT_X)//1280
#define HELP_CONT_H      40

#define HELP_RSC_MASK         (0xF0000000)
#define HELP_RSC_BMP          (0x10000000)
#define HELP_RSC_STRID        (0x20000000)

typedef struct
{
  int item_cnt;                        //item number
  int offset;                         //offset
  u16 rsc_width[HELP_ITEM_MAX_CNT];      //string id
  u32 rsc_id[HELP_ITEM_MAX_CNT];      //string id or bitmap id
}comm_help_data_t2;

enum comm_signbar_idc
{
  IDC_COMM_SIGN_INTENSITY = 0xE0,
  IDC_COMM_SIGN_QUALITY,
  IDC_COMM_SIGN_LOCK,
};

control_t *ui_comm_root_create(u8 root_id,
                               u8 parent_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid);

control_t *ui_comm_root_create_fullbg(u8 root_id,
                               u8 rsi_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid);

control_t *ui_comm_root_create_with_signbar(u8 root_id,
                                            u8 parent_root,
                                            u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            u16 title_icon,
                                            u16 title_strid,
                                            BOOL is_beeper);

control_t *ui_comm_root_create_with_signbar_fullbg(u8 root_id,
                                            u8 parent_root,
                                            u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            u16 title_icon,
                                            u16 title_strid,
                                            BOOL is_beeper);

void ui_comm_root_update_signbar(control_t *root,
                                 u8 intensity,
                                 u8 quality,
                                 BOOL is_lock);

control_t *ui_comm_root_get_ctrl(u8 root_id, u8 ctrl_id);

control_t *ui_comm_root_get_root(u8 root_id);

u16 ui_comm_root_keymap(u16 key);

RET_CODE ui_comm_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
/*********************************************************************
 * COMMON MENU
 *********************************************************************/

/* coordinate */
#define MENU_X             ((SCREEN_WIDTH - MENU_W) / 2)
#define MENU_Y             COMM_BG_Y
#define MENU_W             260
#define MENU_BORDER        10

#define MENU_ITEM_CNT      7
#define MENU_ITEM_X        ((MENU_W - MENU_ITEM_W) / 2)
#define MENU_ITEM_Y        40
#define MENU_ITEM_W        240
#define MENU_ITEM_H        34
#define MENU_ITEM_V_GAP    6

typedef struct
{
  u8 id;
  u8 item_cnt;                       //item count
  u16 title_strid;                   //title string id
  msgproc_t proc;
  u16 item_strid[MENU_ITEM_CNT];     //string id
}comm_menu_data_t;

enum comm_help_idc
{
  IDC_COMM_HELP_MBOX = 1,
};

control_t *ui_comm_menu_create(comm_menu_data_t *p_data, u32 para);

u8 ui_comm_get_epg_root(void);

void ui_comm_set_epg_root(u8 root_id);

void ui_comm_help_create2(comm_help_data_t2 *p_data, control_t *p_parent, BOOL force);

void ui_comm_help_update2(control_t *p_parent);

#ifdef ENABLE_NETWORK
#define NET_TITLE_W             SCREEN_WIDTH
#define NET_TITLE_H             80
#define NET_TITLE_ICON_X             40
#define NET_TITLE_ICON_Y             0
#define NET_TITLE_ICON_W             70
#define NET_TITLE_ICON_H             NET_TITLE_H
#define NET_TITLE_TXT_X             140
#define NET_TITLE_TXT_Y             30
#define NET_TITLE_TXT_W            300 
#define NET_TITLE_TXT_H             30

#define FSI_NET_TITLE_TXT       FSI_WHITE

#define HELP_ITEM_H_GAP_NET      16
#define HELP_ITEM_V_GAP_NET      0
#define HELP_ITEM_MAX_CNT_NET    10
#define HELP_ITEM_H_NET          30
#define HELP_ITEM_ICON_W_NET     38

#define NET_MENU_LEFT_X 347
#define NET_MENU_LEFT_H 720
#define NET_MENU_LEFT_PIC_H 132

#define NET_LINE_WIDTH 3

void ui_comm_help_set_font(control_t *p_parent,
                                 u32 n_fstyle,
                                 u32 h_fstyle,
                                 u32 g_fstyle);
                                 
control_t *ui_comm_root_create_submenu(u8 root_id,
                               u8 parent_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid);
                                                                
void ui_comm_title_create_net(control_t *parent, u16 icon_id, u16 strid, u8 rstyle);

void ui_comm_help_create_net(comm_help_data_t *p_data, control_t *p_parent);

control_t *ui_comm_root_create_netmenu(u8 root_id,
                               u8 parent_root,
                               u16 title_icon,
                               u16 title_strid);
u8 ui_comm_get_focus_mainwin_id(void);
#endif

#endif
