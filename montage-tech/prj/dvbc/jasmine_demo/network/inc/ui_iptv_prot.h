/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_IPTV_PROT_H__
#define __UI_IPTV_PROT_H__

/*!
 * Const value
 */
#define MAX_KEYWORD_LENGTH                        (32)

#define RSI_OTT_BUTTON_SH                             (RSI_ITEM_2_SH)
#define RSI_OTT_BUTTON_HL                             (RSI_ITEM_2_HL)

#define RSI_OTT_SMALL_PANEL                          (RSI_BOX_2)

#define RSI_OTT_SCROLL_BAR_BG                     (RSI_SCROLL_BAR_BG)
#define RSI_OTT_SCROLL_BAR                            (RSI_SCROLL_BAR_MID)

#define RSI_BOX3                                                  (RSI_BOX_2)

/*!
 Macro
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


/*!
 * Type define
 */
// iptv message
typedef enum
{
    MSG_INFO = MSG_LOCAL_BEGIN + 0x100,
    MSG_FOCUS_KEY,
    MSG_OPEN_DLG_REQ,
    MSG_OPEN_CFMDLG_REQ,
    MSG_CLOSE_CFMDLG_NTF,

    MSG_OPEN_IPTV_REQ,

    MSG_OPEN_IPTV_SEARCH_REQ

} ui_iptv_msg_t;

typedef enum
{
   IPTV_MOVIE = 1,
   IPTV_TV,
   IPTV_VARIETY,
} ui_iptv_mode_t;

typedef struct
{
    u32 vdo_id;
    u32 res_id;
    u8  b_single_page;
} ui_iptv_description_param_t;

/*!
 * Function define
 */
void ui_iptv_open_dlg(u8 root_id, u16 str_id);
void ui_iptv_open_cfm_dlg(u8 root_id, u16 str_id);

#endif
