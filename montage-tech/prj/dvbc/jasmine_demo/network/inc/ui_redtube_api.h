/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_REDTUBE_API_H__
#define __UI_REDTUBE_API_H__

#define IDENTIFY_CODE(feed_type, cate_index, page_num, item_num) \
        (u32)((((u8)feed_type)<<24) | (((u8)cate_index)<<16) | (((u8)page_num)<<8) | ((u8)item_num))
#define FEED_TYPE(identify_code)        (u8)(((u32)identify_code) >> 24)
#define CATE_INDEX(identify_code)        (u8)((((u32)identify_code) << 8) >> 24)
#define PAGE_NUM(identify_code)        (u8)((((u32)identify_code) << 16) >> 24)
#define ITEM_INDEX(identify_code)        (u8)(((u32)identify_code) & 0xff)


#define RTB_MAX_TITLE_LEN                   (REDTUBE_TITLE_LEN)
#define RTB_MAX_UPLOAD_TIME_LEN      (REDTUBE_ADDED_TIME_LEN)
#define RTB_MAX_DURATION_LEN            (REDTUBE_DURATION_LEN)
#define RTB_MAX_IMAGE_URL_LEN          (REDTUBE_IMAGE_LEN)
#define RTB_MAX_CATENAME_LEN           (32)

#define REDTUBE_RATED_ICON_NUM        (5)
#define REDTUBE_RATED_ICON_MASK       ((1 << (REDTUBE_RATED_ICON_NUM + REDTUBE_RATED_ICON_NUM)) - 1)

typedef enum
{
    MSG_RT_EVT_BEGIN = MSG_EXTERN_BEGIN + 1350,
    MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE = MSG_RT_EVT_BEGIN,
    MSG_RT_EVT_GET_PAGE_VDO_FAIL,
    MSG_RT_EVT_NEW_PLAY_URL_ARRIVE,
    MSG_RT_EVT_GET_PLAY_URL_FAIL,
    MSG_RT_EVT_NEW_CATE_LIST_ARRIVE,
    MSG_RT_EVT_GET_CATE_LIST_FAIL,

    MSG_RT_EVT_END = MSG_RT_EVT_BEGIN + 50
} redtube_api_msg_t;

typedef enum{
    REDTUBE_RESOLUTION_NORMAL,
    REDTUBE_RESOLUTION_HIGH,
} redtube_resolution_t;

typedef struct
{
    u16 title[RTB_MAX_TITLE_LEN+1];  /*title of each item in one page*/
    u8  upload_time[RTB_MAX_UPLOAD_TIME_LEN+1];
    u8  duration[RTB_MAX_DURATION_LEN+1];       /*rating of each item in one page*/
    u16 rated_mask;
    u32 view_count;
    u8  thumnail_url[RTB_MAX_IMAGE_URL_LEN+1];   /*thumnail of each item in one page*/
} redtube_vdo_item_t;

typedef struct
{
    u32 total_page;
    u16 count;                             /*total item of current page*/
    redtube_vdo_item_t  *vdoList;
} redtube_vdo_page_t;

typedef struct
{
    u16  cate_name[RTB_MAX_CATENAME_LEN+1]; /*reserve name of category*/
} redtube_cate_item_t;

typedef struct
{
    u16  count;  /*total category*/
    redtube_cate_item_t *cateList;
} redtube_cate_page_t;

typedef struct
{
    u8  *url;
} redtube_vdo_play_url_t;


void ui_redtube_free_msg_data(u16 msg, u32 para1, u32 para2);
void ui_redtube_dp_init(void);
void ui_redtube_dp_deinit(void);
void ui_redtube_register_msg(void);
void ui_redtube_unregister_msg(void);
void ui_redtube_set_page_size(s32 size);
void ui_redtube_get_default_video(s32 page_idx);
void ui_redtube_get_cate_list(void);
void ui_redtube_get_cate_video(s32 cate_idx, s32 page_idx);
void ui_redtube_search(u8 *keyword);
void ui_redtube_get_search_video(s32 page_idx);
void ui_redtube_get_play_url(s32 index);


#endif
