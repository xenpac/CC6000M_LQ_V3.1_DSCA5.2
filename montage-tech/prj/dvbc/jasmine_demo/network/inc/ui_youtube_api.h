/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_YOUTUBE_API_H__
#define __UI_YOUTUBE_API_H__

#define IDENTIFY_CODE(feed_type, cate_index, page_num, item_num) \
        (u32)((((u8)feed_type)<<24) | (((u8)cate_index)<<16) | (((u8)page_num)<<8) | ((u8)item_num))
#define FEED_TYPE(identify_code)        (u8)(((u32)identify_code) >> 24)
#define CATE_INDEX(identify_code)        (u8)((((u32)identify_code) << 8) >> 24)
#define PAGE_NUM(identify_code)        (u8)((((u32)identify_code) << 16) >> 24)
#define ITEM_INDEX(identify_code)        (u8)(((u32)identify_code) & 0xff)

#define MAX_CATENAME_LEN           (32)
#define MAX_TITLE_LEN                   (128)
#define MAX_AUTHOR_LEN               (32)
#define MAX_DURATION_LEN            (8)
#define MAX_URL_LEN                     (512)
#define MAX_DESCRIPTION_LEN       (640)

#define YOUTUBE_RATED_ICON_NUM        (5)
#define YOUTUBE_RATED_ICON_MASK       ((1 << (YOUTUBE_RATED_ICON_NUM + YOUTUBE_RATED_ICON_NUM)) - 1)

typedef enum
{
    MSG_YT_EVT_BEGIN = MSG_EXTERN_BEGIN + 1300,
    MSG_YT_EVT_VDO_REQ_OPEN_SUCC = MSG_YT_EVT_BEGIN,
    MSG_YT_EVT_VDO_REQ_OPEN_FAIL,
    MSG_YT_EVT_CATE_REQ_OPEN_SUCC,
    MSG_YT_EVT_CATE_REQ_OPEN_FAIL,
    MSG_YT_EVT_SEARCH_SUCC,
    MSG_YT_EVT_SEARCH_FAIL,
    //MSG_YT_EVT_NEW_CATEGORY_ARRIVE,
    MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE,
    MSG_YT_EVT_GET_PAGE_FAIL,
    MSG_YT_EVT_NEW_PLAY_URL_ARRIVE,
    MSG_YT_EVT_GET_URL_FAIL,
    //MSG_YT_EVT_SET_MAX_ITEM_NUM_OK,
    //MSG_YT_EVT_GET_CATEGORY_TOTAL_OK,

    MSG_YT_EVT_END = MSG_YT_EVT_BEGIN + 50
} youtube_api_msg_t;

typedef enum{
    YOUTUBE_RESOLUTION_NORMAL,
    YOUTUBE_RESOLUTION_HIGH,
} youtube_resolution_t;

typedef struct
{
    u16 title[MAX_TITLE_LEN+1];  /*title of each item in one page*/
    u16 author[MAX_AUTHOR_LEN+1];
    u8  duration[MAX_DURATION_LEN+1];       /*rating of each item in one page*/
    u16 rated_mask;
    u32 view_count;
    u8  thumnail_url[MAX_URL_LEN+1];   /*thumnail of each item in one page*/
    u16 info_description[MAX_DESCRIPTION_LEN+1];
    u8  info_thumnail_url[MAX_URL_LEN+1];   /*thumnail of each item in one page*/
} youtube_vdo_item_t;

typedef struct
{
    u32 total_count;
    u16 count;                             /*total item of current page*/
    youtube_vdo_item_t  *vdoList;
    u32 identify_code;
} youtube_vdo_req_t;

#if 0
typedef struct
{
    u16  count;                             /*total item of current page*/
    youtube_vdo_item_t  *vdoList;
    u32 identify_code;
} youtube_vdo_page_t;
#endif

typedef struct
{
    u16  cate_name[MAX_CATENAME_LEN+1]; /*reserve name of category*/
} youtube_cate_item_t;

typedef struct
{
    u16  count;  /*total category*/
    youtube_cate_item_t *cateList;
    u32 identify_code;
} youtube_cate_page_t;

typedef struct
{
    u8  *url;
    u32 identify_code;
} youtube_vdo_play_url_t;

typedef struct
{
    u32 identify_code;
} youtube_fail_operation_t;

void ui_youtube_free_msg_data(u16 msg, u32 para1, u32 para2);
void ui_youtube_dp_init(void);
void ui_youtube_dp_deinit(void);
void ui_youtube_register_msg(void);
void ui_youtube_unregister_msg(void);
void ui_youtube_request_open(YT_FEED_TYPE type);
void ui_youtube_request_close(void);
void ui_youtube_search(u8 *keyword);
s32 ui_youtube_get_focus_item_index(void);
s32 ui_youtube_get_focus_category_index(void);
s32 ui_youtube_get_page_index(void);
void ui_youtube_get_next_page(void);
void ui_youtube_get_prev_page(void);
void ui_youtube_get_cur_page(void);
void ui_youtube_set_category_index(s32 index);
void ui_youtube_set_page_index(s32 index);
void ui_youtube_set_item_index(s32 index);
void ui_youtube_set_max_category_cnt(s32 index);
void ui_youtube_set_max_item_cnt(s32 index);
void ui_youtube_get_video_play_url(void);
s32 ui_youtube_get_item_num(void);
void ui_youtube_get_category_num(void);
void ui_youtube_init_url(u8 *url);
void ui_youtube_set_country(YT_REGION_ID_T region);
void ui_youtube_set_time_mode(YT_TIME_T mode);

#endif
