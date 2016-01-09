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
#define COMM_DLG_Y         ((SCREEN_HEIGHT - COMM_DLG_H) / 2)
#define COMM_DLG_W         300
#define COMM_DLG_H         160

#define DLG_CONTENT_GAP    20

#define DLG_BTN_W          64
#define DLG_BTN_H          30

/* rect style */
#define RSI_DLG_CONT       RSI_DLG_FRM
#define RSI_DLG_BTN_SH     RSI_WHITE
#define RSI_DLG_BTN_HL     RSI_YELLOW

/* font style */
#define FSI_DLG_BTN_SH     FSI_BLACK
#define FSI_DLG_BTN_HL     FSI_BLACK
#define FSI_DLG_CONTENT    FSI_BLACK

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
}comm_dlg_data_t;

typedef void (*do_func_t)(void);

dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data);

void ui_comm_dlg_close(void);

void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out);

void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);

void ui_comm_ask_for_savdlg_open(rect_t *p_dlg_rc,
                                   u16 strid,
                                   do_func_t do_save,
                                   do_func_t undo_save, u32 tm_out);

#endif


