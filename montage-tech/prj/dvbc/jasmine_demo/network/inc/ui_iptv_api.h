/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_IPTV_API_H__
#define __UI_IPTV_API_H__

#define IPTV_MAX_FORMAT_COUNT   (4)

typedef enum
{
    MSG_IPTV_EVT_BEGIN = MSG_EXTERN_BEGIN + 1400,
    MSG_IPTV_EVT_INIT_SUCCESS = MSG_IPTV_EVT_BEGIN,
    MSG_IPTV_EVT_INIT_FAIL,
    MSG_IPTV_EVT_DEINIT_SUCCESS,
    MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE,
    MSG_IPTV_EVT_GET_RES_NAME_FAIL,
    MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE,
    MSG_IPTV_EVT_GET_RES_CATGRY_FAIL,
    MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE,
    MSG_IPTV_EVT_GET_PAGE_VDO_FAIL,
    MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE,
    MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL,
    MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE,
    MSG_IPTV_EVT_GET_VDO_INFO_FAIL,
    MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE,
    MSG_IPTV_EVT_GET_INFO_URL_FAIL,
    MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE,
    MSG_IPTV_EVT_GET_RECMND_INFO_FAIL,
    MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE,
    MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL,
    MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE,
    MSG_IPTV_EVT_GET_PLAY_URL_FAIL,
    MSG_IPTV_EVT_END = MSG_IPTV_EVT_BEGIN + 50
} iptv_api_msg_t;

////////////////////////////////////
typedef enum
{
    IPTV_DP_STATE_UNINIT,
    IPTV_DP_STATE_INITING,
    IPTV_DP_STATE_INIT_SUCC,
    IPTV_DP_STATE_INIT_FAIL,
} iptv_dp_state_t;

typedef enum
{
      IPTV_API_FORMAT_NORMAL = IPTV_FORMAT_NORMAL,
      IPTV_API_FORMAT_HIGH = IPTV_FORMAT_HIGH,
      IPTV_API_FORMAT_SUPER = IPTV_FORMAT_SUPER,
      IPTV_API_FORMAT_TOPSPEED = IPTV_FORMAT_JISU,
      IPTV_API_FORMAT_DEFAULT = IPTV_FORMAT_DEFAULT,
}IPTV_API_FORMAT_T;

typedef  struct
{
    u32 res_id;
    u16 *name;
} al_iptv_name_res_item_t;

typedef struct
{
    u16 total_res;
    al_iptv_name_res_item_t *resList;
} al_iptv_name_res_list_t;

/////////////////////////////////////
typedef struct
{
    u16 *name;
    u8  *key;

    u32 total_vdo;
} al_iptv_cat_item_t;

typedef struct
{
    u32 res_id;

    u16 total_cat;
    al_iptv_cat_item_t *catList;
} al_iptv_cat_res_item_t;

typedef struct
{
    u16 total_res;
    al_iptv_cat_res_item_t *resList;
} al_iptv_cat_res_list_t;

//////////////////////////////////////////////////

typedef struct
{
    u32 vdo_id;
    u32 res_id;
    u16 *name;
    u8  *img_url;
    u8 b_single_page;
} al_iptv_vdo_item_t;

typedef struct
{
    u32 total_page;
    u32 page_num;

    u32 total_vdo;
    u16 vdo_cnt;
    al_iptv_vdo_item_t *vdoList;
} al_iptv_vdo_list_t;

//////////////////////////////////////////////////
typedef struct
{
    u16 *name;
} al_iptv_orgn_item_t;
typedef struct
{
    u32 res_id;
    u32 vdo_id;

    u16 *name;
    u16 *area;
    u16 *director;
    u16 *actor;
    u16 *score;
    u16 *description;
    u16 *attr;
    u16 *years;
    u8  *img_url;
    u16 orgn_cnt;
    al_iptv_orgn_item_t *orgnList;
} al_iptv_vdo_info_t;
typedef struct
{
      int id;
      u16 *title;
      int cat_id;
      char *img;
      char *types;
      char *score;
      char *currentcollect;
      char *clarity;      
}iptv_recmd_channel_t;

typedef struct
{
   int number;
   iptv_recmd_channel_t *recmd;
}al_iptv_recomand_info_t;
//////////////////////////////////////////////////
typedef struct
{
    char *origin;
    char *collect;
    char *playurl;
    u16 *urltitle;
} al_iptv_info_url_item_t;

typedef struct
{
    u32 page_count;
    u32 page_total;
    u32 count;
    al_iptv_info_url_item_t *urlList;
} al_iptv_info_url_list_t;

typedef struct
{
    char *collect;
    char *playurl_list;
    u16 *urltitle;
}al_iptv_play_origin_item_t;

typedef struct
{
    u16 *origin;
    u32 url_count;
    al_iptv_play_origin_item_t *play_origin_info;
} al_iptv_play_origin_info_t;



//////////////////////////////////////////////////
typedef struct
{
    u16 total_url;
    u8  **pp_urlList;
} al_iptv_play_url_list_t;
///////////////////////////////////////////////////


typedef struct
{
    u8  total_format;
    u8  *formatList;
} al_iptv_source_format_t;

//////////////////////////////////////////////////

void ui_iptv_free_msg_data(u16 msg, u32 para1, u32 para2);
inline u16 ui_iptv_get_dp_state(void);
inline void ui_iptv_set_dp_state(u16 state);
void ui_iptv_dp_init(void);
void ui_iptv_dp_deinit(void);
void ui_iptv_register_msg(void);
void ui_iptv_unregister_msg(void);
void ui_iptv_get_res_list(void);
void ui_iptv_get_catgry_list(void);
void ui_iptv_get_video_list(u32 res_id, u16 *cat_name, u8 *key, u32 page_num);
void ui_iptv_search_set_res_id(u32 res_id);
void ui_iptv_search_set_keyword(u16 *keyword);
void ui_iptv_search(u32 page_num);
void ui_iptv_get_video_info(u32 vdo_id, u8 cat_id);
void ui_iptv_get_info_url(u32 vdo_id, u8 cat_id, u16 *origin, u32 page_num, u32 page_size);
void ui_iptv_get_recmnd_info(u32 vdo_id, u32 res_id, u16 *area);
void ui_iptv_get_play_url(IPTV_API_FORMAT_T format, u8 *url);
void ui_iptv_get_play_format(u8 *url);
void ui_iptv_set_page_size(u16 page_size);
int ui_iptv_vdo_idntfy_cmp(u32 unidntf_code);
#endif
