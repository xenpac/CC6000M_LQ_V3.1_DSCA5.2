/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_COMM_DLG_H__
#define __UI_COMM_DLG_H__

/* coordinate */
#define COMM_DLG_X         ((SCREEN_WIDTH - COMM_DLG_W) / 2)
#define COMM_DLG_Y         ((SCREEN_HEIGHT - COMM_DLG_H) / 2 + 10)
#define COMM_DLG_W        440 //300
#define COMM_DLG_H         160
#define COMM_DLG_WITHTEXT_H  160//200

#define DLG_CONTENT_GAP    20

#define DLG_BTN_W          80
#define DLG_BTN_H          COMM_ITEM_H//36

/* rect style */
#define RSI_DLG_CONT       RSI_WINDOW_2
#define RSI_DLG_BTN_SH     RSI_KEYBOARD_ITEM_2_SH
#define RSI_DLG_BTN_HL     RSI_KEYBOARD_ITEM_2_HL

/* font style */
#define FSI_DLG_BTN_SH     FSI_WHITE
#define FSI_DLG_BTN_HL     FSI_WHITE
#define FSI_DLG_CONTENT    FSI_WHITE

#define MAX_BNT_COUNT      2


//comm loading for network
#define COMM_LOAD_CONT_X ((SCREEN_WIDTH - COMM_LOAD_CONT_W) / 2)
#define COMM_LOAD_CONT_Y (120)
#define COMM_LOAD_CONT_W (120)
#define COMM_LOAD_CONT_H (120)

#define COMM_LOAD_BMP_X (0)
#define COMM_LOAD_BMP_Y (0)
#define COMM_LOAD_BMP_W (COMM_LOAD_CONT_W)
#define COMM_LOAD_BMP_H (COMM_LOAD_CONT_H)

#define COMM_LOAD_TEXT_X ((COMM_LOAD_BMP_W - COMM_LOAD_TEXT_W)/2)
#define COMM_LOAD_TEXT_Y ((COMM_LOAD_BMP_H - COMM_LOAD_TEXT_H)/2)
#define COMM_LOAD_TEXT_W (COMM_LOAD_CONT_W - 20)
#define COMM_LOAD_TEXT_H (COMM_LOAD_CONT_H - 20)

enum dlg_type
{
  DLG_FOR_ASK = 0x00, //popup dialog type:	with  yes/no button
  DLG_FOR_CONFIRM,    //popup dialog type:	with ok button
  DLG_FOR_SHOW,       //popup dialog type:	without button
};

typedef enum 
{
  DLG_RET_YES,
  DLG_RET_NO,
  DLG_RET_NULL,
}dlg_ret_t;

enum dlg_str_mode
{
  DLG_STR_MODE_STATIC = 0x10,
  DLG_STR_MODE_EXTSTR = 0x20,
};

typedef struct
{
  u8        parent_root;
  u8        style;
  u16       x;
  u16       y;
  u16       w;
  u16       h;
  u32       content;
  u32       dlg_tmout;
  BOOL      specify_bnt;     //draw button by input content
  u8        specify_bnt_count;
  u16       specify_bnt_cont[MAX_BNT_COUNT];
}comm_dlg_data_t;

typedef struct
{
  u8        parent_root;
  u8        title_style;
  u8        text_style;
  u16       x;
  u16       y;
  u16       w;
  u16       h;
  u32       title_strID;  //标题
  u32       title_content;//标题
  u32       text_strID;   //正文
  u32       text_content; //正文
  u32       dlg_tmout;
}comm_dlg_data_t2;

typedef enum 
{
    LOAD_TIME,
    LOAD_BPS,
}load_type_t;

typedef void (*do_func_t)(void);
typedef void (*do_func_withpara_t)(u32 para1, u32 para2);

dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data);

dlg_ret_t ui_comm_dlg_open2(comm_dlg_data_t *p_data);

void ui_comm_dlg_close(void);

void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out);

void ui_comm_showdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);

void ui_comm_ask_for_dodlg_open_ex(rect_t *p_dlg_rc, u16 strid,
  do_func_withpara_t do_func, u32 para1, u32 para2, u32 tm_out);

BOOL ui_comm_ask_for_savdlg_open(rect_t *p_dlg_rc,
                                   u16 strid,
                                   do_func_t do_save,
                                   do_func_t undo_save, u32 tm_out);
void ui_comm_cfmdlg_open_gb(rect_t *p_dlg_rc, u8* p_gb, 
                                   do_func_t do_cmf, u32 tm_out);
#ifdef ENABLE_NETWORK
void ui_comm_cfmdlg_open_unistr(rect_t *p_dlg_rc, u16* p_unistr, do_func_t do_cmf, u32 tm_out);

void ui_comm_ask_for_dodlg_open_xxx(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);

void ui_comm_ask_for_dodlg_open_unistr(rect_t *p_dlg_rc, u16 *p_unistr,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);
                                  
void ui_comm_dlg_close2(void);
#endif

void ui_comm_loading_create(control_t *p_mainwin);

void ui_comm_loading_show(load_type_t type, u32 time, u16 net_bps, u16 percent);

void ui_comm_loading_hide(void);
#endif


