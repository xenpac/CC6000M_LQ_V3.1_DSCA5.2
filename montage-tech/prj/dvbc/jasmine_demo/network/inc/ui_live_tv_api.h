#ifndef __UI_LIVE_TV_API_H__
#define __UI_LIVE_TV_API_H__
#include "commonData.h"
#include "tvLiveDataProvider.h"

#define IPTV_P

#ifdef IPTV_P
#define IPTV_DBG OS_PRINTF
#else
#define IPTV_DBG(...) do{}while(0)
#endif

#define LIVE_TV_TASK_STACK_SIZE (512*KBYTES)

#define LIVE_TV_INVALID_ID      (0xffff)
#define LIVE_TV_INVALID_INDEX (0xff)
#define MAX_LIVETV_NAME_LEN       (32)

#define CUS_NAME_LEN (128)
#define CUS_URL_LEN (1024)
#define MAX_CUSTOM_URL (1024)


enum 
{
    MSG_GET_LIVE_TV_CATEGORY_ARRIVAL = MSG_EXTERN_BEGIN + 2900,
    MSG_GET_LIVE_TV_GET_ZIP,
    MSG_GET_LIVE_TV_LOOKBACK_NAMES,
    MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_EPG,
    MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_LOOKBACK,
    MSG_INIT_LIVETV_SUCCESS,
    MSG_INIT_LIVETV_FAILED,
    MSG_GET_LIVE_TV_CHANNELLIST,
};

typedef enum
{
    LIVETV_UNINIT,
    LIVETV_INITING,
    LIVETV_INIT_SUCCESS,
    LIVETV_INIT_FAILED,
}livetv_init_sts;


typedef  struct
{
    u8 *name;
    u8 *url;
} livetv_url;

typedef enum
{
    ADD_FAV,
    DEL_FAV,
    FULL_FAV,
}livetv_fav_sts;


typedef struct
{
    u16 name[MAX_LIVETV_NAME_LEN + 1];
} ui_livetv_group_info_t;

typedef struct
{
    u16 id;
    u16 name[MAX_LIVETV_NAME_LEN + 1];
    u8  **ppUrlList;
    u8  urlCnt;
    u8  urlIdx;
} ui_livetv_chan_info_t;

typedef struct
{
    u8 *name;
    u8 **ppUrlList;
}custom_url_t;

typedef enum
{
	ENTRY_ALL_GRP,
	ENTRY_CUS_GRP,
}ui_livetv_entry_grp_t;

u16 ui_live_tv_evtmap(u32 event);
/*********************************************************
*************************functions *************************
**********************************************************/
void al_livetv_dp_init();
void al_livetv_dp_deinit(void);
LIVE_TV_DP_HDL_T *get_live_tv_handle();

RET_CODE ui_live_tv_api_group_arrived(u32 param);
RET_CODE ui_live_tv_api_all_chan_arrived(LIVE_TV_CHANNEL_LIST_T * param);
RET_CODE ui_live_tv_api_all_chan_zip_load();
void ui_live_tv_get_categorylist();


//functions for livetv UI
BOOL ui_livetv_storage_get_group_info_by_group_index(ui_livetv_group_info_t *p_groupInfo, u16 iGroup);
u16 ui_livetv_storage_get_group_cnt(void);
s32 ui_livetv_storage_get_chan_info_by_id(ui_livetv_chan_info_t *p_pg, u16 id);
BOOL ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(ui_livetv_chan_info_t *p_chanInfo, u16 iGroup, u16 pos);
u16 ui_livetv_storage_get_chan_cnt_by_group_index(u16 iGroup);
s32 ui_livetv_storage_get_chan_pos_by_id(u16 id);
BOOL ui_livetv_storage_set_pref_chan_url(u16 id, u8 urlIdx);


void ui_livetv_init_custom_chan_list(void);

void al_livetv_release_custom_chan_list(void);

void save_customer_iptv(u8* p_data, u32 len);

#endif

