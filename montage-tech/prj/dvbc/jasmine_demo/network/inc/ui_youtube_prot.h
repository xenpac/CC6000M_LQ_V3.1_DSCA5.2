/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_YOUTUBE_PROT_H__
#define __UI_YOUTUBE_PROT_H__

//#define MAX_NAME_LENGTH (255)

/*!
 * Youtube message
 */
typedef enum
{
    MSG_INFO = MSG_LOCAL_BEGIN + 0x100,
    MSG_INPUT_COMP,
    MSG_OPEN_REQ,
    MSG_BACK,
    MSG_MAIN,
    MSG_OPEN_CFMDLG_REQ,
    MSG_CLOSE_CFMDLG_NTF,

} ui_youtube_msg_t;
#if 0
typedef struct
{
    u16 total_count;
    u16 count;                             /*total item of current page*/
    youtube_vdo_item_t  *vdoList;
} ui_youtube_vdo_req_t;
#endif
typedef struct
{
    u16  count;                             /*total item of current page*/
    youtube_vdo_item_t  *vdoList;
} ui_youtube_vdo_page_t;

typedef struct
{
    u16  count;  /*total category*/
    youtube_cate_item_t *cateList;
} ui_youtube_cate_page_t;

#endif
