/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_NM_PROT_H__
#define __UI_NM_PROT_H__

/*!
 Macro
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define IS_MASKED(x, mask)    (((x) & (mask)) == mask)

#define FOCUS_KEY_MAP(v_key, msg)   (msg = MSG_FOCUS_UP + v_key - V_KEY_UP)

#define PAGE_KEY_MAP(v_key, msg)   (msg = MSG_PAGE_UP + v_key - V_KEY_PAGE_UP)

/*!
 * Type define
 */
typedef struct
{
    u32 nm_type;
    u32 bmp_id;
    u32 str_id;
} ui_nm_dp_type_rsc_item;

// nm message
typedef enum
{
    MSG_FOCUS_KEY = MSG_LOCAL_BEGIN + 0x100,
    MSG_PAGE_KEY,
    MSG_SEARCH,
    MSG_INFO,
    MSG_CLOSE_CFMDLG_NTF,
    MSG_INPUT_COMP,
    MSG_OPEN_CLASSIC_REQ,
    MSG_OPEN_SIMPLE_REQ,
    MSG_OPEN_CFMDLG_REQ,
    MSG_OPEN_DLG_REQ,    
} ui_nm_simple_msg_t;

typedef struct
{ 
    u16 name;
} nm_class_list_ext_item;

typedef struct
{
    u16 id;
    u16 name;
} nm_opt_list_item;

extern nm_class_list_ext_item class_list_ext[1];

extern nm_opt_list_item g_resolution_opt[2];

#endif
