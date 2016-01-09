/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __AL_NETMEDIA_H__
#define __AL_NETMEDIA_H__


#if 0
#define MAX_CATENAME_LEN           (32)
#define MAX_TITLE_LEN                   (128)
#define MAX_AUTHOR_LEN               (32)
#define MAX_DURATION_LEN            (8)
#define MAX_URL_LEN                     (512)
#define MAX_DESCRIPTION_LEN       (640)

#define YOUTUBE_RATED_ICON_NUM        (5)
#define YOUTUBE_RATED_ICON_MASK       ((1 << (YOUTUBE_RATED_ICON_NUM + YOUTUBE_RATED_ICON_NUM)) - 1)
#endif

typedef enum
{
    MSG_NETMEDIA_BEGIN = MSG_EXTERN_BEGIN + 1600,
    MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED = MSG_NETMEDIA_BEGIN,
    MSG_NETMEDIA_GET_PAGE_VDO_FAILED,
    MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED,
    MSG_NETMEDIA_GET_SEARCH_VDO_FAILED,
    MSG_NETMEDIA_NEW_PROGRAM_INFO_ARRIVED,
    MSG_NETMEDIA_GET_PROGRAM_INFO_FAILED,
    MSG_NETMEDIA_NEW_COLLECTION_LIST_ARRIVED,
    MSG_NETMEDIA_GET_COLLECTION_LIST_FAILED,
    MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED,
    MSG_NETMEDIA_GET_PLAY_URLS_FAILED,

    MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED,
    MSG_NETMEDIA_GET_CLASS_LIST_FAILED,
    MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED,
    MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED,

    MSG_NETMEDIA_WEBSITE_INIT_SUCCESS,
    MSG_NETMEDIA_WEBSITE_INIT_FAILED,
    MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS,
    MSG_NETMEDIA_WEBSITE_DEINIT_FAILED,
    MSG_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED,
    MSG_NETMEDIA_GET_WEBSITE_INFO_FAILED,
    MSG_NETMEDIA_INIT_SUCCESS,
    MSG_NETMEDIA_INIT_FAILED,
    MSG_NETMEDIA_DEINIT_SUCCESS,
    MSG_NETMEDIA_DEINIT_FAILED,
    
    MSG_NETMEDIA_END = MSG_NETMEDIA_BEGIN + 50
} youtube_api_msg_t;



typedef struct
{
    u8  *id;

    u16 *title;
    u8  *key;
    u8  *img_url;
    u16 *date;
    u16 *duration;
    u16 *description;
    float score;
} al_netmedia_vdo_item_t;

typedef struct
{
    u32 total_vdo;
    u16 vdo_cnt;
    al_netmedia_vdo_item_t *vdoList;
} al_netmedia_vdo_list_t;

typedef struct
{
    u8  *id;
    u16 *title;
    u8  *key;
    u8  *img_url;
    u16 *date;
    u16 *duration;
    u16 *description;
    float score;
    u32 view_count;
    u32 total_collection;
} al_netmedia_program_info_t;
typedef struct
{
    u8  *id;
    u16 *name;
    u8  *key;
} al_netmedia_collection_item_t;
typedef struct
{
    u16 collection_cnt;
    al_netmedia_collection_item_t *collectionList;
} al_netmedia_collection_list_t;
typedef struct
{
    NETMEDIA_VIDEO_RESOLUTION_E resolution;
    u16 fragment_url_cnt;
    u8  **pp_fragmntUrlList;
} al_netmedia_play_url_item_t;

typedef struct
{
    u16 play_url_cnt;
    al_netmedia_play_url_item_t *playUrlList;
} al_netmedia_play_url_list_t;

typedef struct
{
    u32 id;
    u16 *name;//for show
    u8  *key;//for data request 
    /*
      to tell its sub item is classes or programs
    */
    NM_SUBCLASS_ATTR_E attr;
} al_netmedia_class_item_t;

typedef struct
{
    u16 class_cnt;
    al_netmedia_class_item_t *classList;
} al_netmedia_class_list_t;

typedef struct
{
    NETMEDIA_DP_TYPE  dp_type;
    u16 *title;
    u8 *logo_url;
    u8 ui_style;
    u32 ui_attr;
} al_netmedia_website_item_t;

typedef struct
{
    u8 web_count;
    al_netmedia_website_item_t *website_list;
} al_netmedia_website_info_t;
void al_netmedia_free_msg_data(u16 msg, u32 para1, u32 para2);

void al_netmedia_dp_init(void);

void al_netmedia_dp_deinit(void);

void al_netmedia_website_init(NETMEDIA_DP_TYPE nm_dp_type);

void al_netmedia_website_deinit(void);

void al_netmedia_register_msg(void);

void al_netmedia_unregister_msg(void);

void al_netmedia_get_class_list(void);

void al_netmedia_get_subclass_list(u32 class_id, u8 *class_key);

void al_netmedia_set_page_size(u16 page_size);

void al_netmedia_get_video_list(u32 class_id, u8 *class_key, u32 subclass_id, u8 *subclass_key, u32 page_num);

void al_netmedia_set_search_keyword(u16 *keyword);

void al_netmedia_search(u32 class_id, u32 page_num);

void al_netmedia_get_play_urls(u8 *vdo_id, u8 *vdo_key);

int al_netmedia_vdo_idntfy_cmp(u32 unidntfy_code);
void al_netmedia_get_website_info(void);
#endif
