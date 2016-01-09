/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
#include "commonData.h"
#include "youtubeDataProvider.h"
#include "ui_youtube_api.h"

//#include "ui_rename.h"
//#include "lib_char.h"
#if ENABLE_NETWORK



#define APP_YOUTUBE    APP_NVOD
typedef enum
{
    YOUTUBE_EVT_VDO_REQ_OPEN_SUCC = ((APP_YOUTUBE << 16) + 0),
    YOUTUBE_EVT_VDO_REQ_OPEN_FAIL,
    YOUTUBE_EVT_CATE_REQ_OPEN_SUCC,
    YOUTUBE_EVT_CATE_REQ_OPEN_FAIL,
    YOUTUBE_EVT_SEARCH_SUCC,
    YOUTUBE_EVT_SEARCH_FAIL,
    //YOUTUBE_EVT_NEW_CATEGORY_ARRIVE,
    YOUTUBE_EVT_NEW_PAGE_ITEM_ARRIVE,
    YOUTUBE_EVT_GET_PAGE_FAIL,
    YOUTUBE_EVT_NEW_PLAY_URL_ARRIVE,
    YOUTUBE_EVT_GET_URL_FAIL,
    //YOUTUBE_EVT_SET_MAX_ITEM_NUM_OK,
    //YOUTUBE_EVT_GET_CATEGORY_TOTAL_OK,
    
    YOUTUBE_EVT_MAX
} youtube_api_evt_t;

u16 ui_youtube_evtmap(u32 event);

//extern int utf8_to_utf16(const char *ins, tchar_t *outs, u32 max_len);

extern iconv_t g_cd_utf8_to_utf16le;

static YOUTUBE_DATA_T *g_pYouTubeData = NULL;
static 	INIT_TASK_PARAM_T g_task_param;

static youtube_vdo_req_t *ui_youtube_gen_vdo_page(YT_DP_EVENT_PARAM_T *p_param)
{
    youtube_vdo_req_t *p_data;
    char *inbuf, *outbuf;
    s32 duration = 0;
    s32 view_count = 0;
    size_t src_len, dest_len;
    float rated = 0;
    u16 i = 0;
    u8  hour, min, sec;
    u8  shift = 0;

    p_data = (youtube_vdo_req_t *)mtos_malloc(sizeof(youtube_vdo_req_t));
    MT_ASSERT(p_data != NULL);

    p_data->total_count = p_param->item_num_total;
    p_data->count = p_param->item_num;
    p_data->identify_code = p_param->identify_code;
    p_data->vdoList = (youtube_vdo_item_t *)mtos_malloc(p_data->count * sizeof(youtube_vdo_item_t));
    MT_ASSERT(p_data->vdoList != NULL);
    memset(p_data->vdoList, 0, p_data->count * sizeof(youtube_vdo_item_t));

    for (i = 0; i < p_data->count; i++)
    {
        if (p_param->p_item_title[i] != NULL)
        {
            src_len = strlen(p_param->p_item_title[i]) + 1;
            dest_len = sizeof(p_data->vdoList[i].title);
            inbuf = p_param->p_item_title[i];
            outbuf = (char *)p_data->vdoList[i].title;
            iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }

        inbuf = p_param->p_item_author[i];
        outbuf = (char*) p_data->vdoList[i].author;
        src_len = strlen(inbuf) + 1;
        dest_len = sizeof(p_data->vdoList[i].author);
        iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        //utf8_to_utf16(p_param->p_item_author[i], p_data->vdoList[i].author, sizeof(p_data->vdoList[i].author));

        duration = atoi(p_param->p_duration[i]);
        if (duration < 0)
        {
            duration = 0;
        }
        hour = (u8) (duration / 3600);
        min = (u8) ((duration % 3600) / 60);
        sec = (u8) (duration % 60);
        sprintf(p_data->vdoList[i].duration, "%02d:%02d:%02d", hour, min, sec);
        OS_PRINTF("@@@i=%d, p_duration=%s, duration=%s\n", i, p_param->p_duration[i], p_data->vdoList[i].duration);

        rated = atof(p_param->p_rating[i]);
        shift = (u8) ((YOUTUBE_RATED_ICON_NUM - rated) / 0.5);
        p_data->vdoList[i].rated_mask = ((YOUTUBE_RATED_ICON_MASK >> shift) << shift);
        OS_PRINTF("@@@p_rating=%s, rated_mask=0x%04x\n", p_param->p_rating[i], p_data->vdoList[i].rated_mask);

        view_count = atoi(p_param->p_viewcount[i]);
        if (view_count < 0)
        {
            view_count = 0;
        }
        p_data->vdoList[i].view_count = view_count;
        OS_PRINTF("@@@p_viewcount=%s, view_count=%d\n", p_param->p_viewcount[i], p_data->vdoList[i].view_count);

        if (p_param->p_thumnailhq[i] != NULL && strlen(p_param->p_thumnailhq[i]) > 0)
        {
            strncpy((char *)p_data->vdoList[i].thumnail_url, (char *)p_param->p_thumnaildef[i], MAX_URL_LEN);
        }
        if (p_param->p_thumnailhq[i] != NULL && strlen(p_param->p_thumnailhq[i]) > 0)
        {
            inbuf = p_param->p_thumnailhq[i];
        }
        else if (p_param->p_thumnailmq[i] != NULL && strlen(p_param->p_thumnailmq[i]) > 0)
        {
            inbuf = p_param->p_thumnailmq[i];
        }
        else if (p_param->p_thumnailsd[i] != NULL && strlen(p_param->p_thumnailsd[i]) > 0)
        {
            inbuf = p_param->p_thumnailsd[i];
        }
        else
        {
            inbuf = p_data->vdoList[i].thumnail_url;
        }
        strncpy((char *)p_data->vdoList[i].info_thumnail_url, (char *)inbuf, MAX_URL_LEN);
        
        inbuf = p_param->p_item_description[i];
        outbuf = (char*) p_data->vdoList[i].info_description;
        src_len = strlen(inbuf) + 1;
        dest_len = sizeof(p_data->vdoList[i].info_description);
        iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        //utf8_to_utf16(p_param->p_item_description[i], p_data->vdoList[i].info_description, sizeof(p_data->vdoList[i].info_description));
        //OS_PRINTF("@@@p_item_description=%s\n", p_param->p_item_description[i]);
    }

    return p_data;
}

static RET_CODE  youtube_dp_event_callback(YOUTUBE_DP_EVENT_TYPE event, u32 param)
{
    YT_DP_EVENT_PARAM_T *p_param = NULL;
    youtube_vdo_play_url_t *p_urlData = NULL;
    char *inbuf, *outbuf;
    youtube_config_t config;
    event_t evt = {0};
    size_t src_len, dest_len;
    u16 i = 0, len;
    u8  feed_type;
    
    switch(event)
    {
        case YT_DP_EVE_INIT_OK:
            p_param = (YT_DP_EVENT_PARAM_T *)param;

            feed_type = FEED_TYPE(p_param->identify_code);
            OS_PRINTF("@@@YT_DP_EVE_INIT_OK feed_type=%d, item_num_total=%d, item_num=%d\n", feed_type, p_param->item_num_total, p_param->item_num);
            if ((feed_type == STAND_FEED_TOP_POPULAR) 
                || (feed_type == STAND_FEED_TOP_FAVORITE)
                || (feed_type == STAND_FEED_TOP_RATED)
                || (feed_type == STAND_FEED_MOST_RECENT)
                || (feed_type == STAND_FEED_MOST_RESPONDED))
            {
                youtube_vdo_req_t *p_data = ui_youtube_gen_vdo_page(p_param);

                evt.id = YOUTUBE_EVT_VDO_REQ_OPEN_SUCC;
                evt.data1 = (u32)p_data;
                evt.data2 = (u32)p_data->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            }
            else if (feed_type == VIDEO_FEED)
            {
                youtube_cate_page_t *p_data = (youtube_cate_page_t *)mtos_malloc(sizeof(youtube_cate_page_t));
                MT_ASSERT(p_data != NULL);

                p_data->count = p_param->category_num;
                p_data->identify_code = p_param->identify_code;
                p_data->cateList = (youtube_cate_item_t  *)mtos_malloc(p_data->count * sizeof(youtube_cate_item_t));
                MT_ASSERT(p_data->cateList != NULL);
                memset(p_data->cateList, 0, p_data->count * sizeof(youtube_cate_item_t));

                OS_PRINTF("@@@category_num=%d, p_category_name[0]=0x%x\n", p_param->category_num, p_param->p_category_name[0]);
                for (i = 0; i < p_data->count; i++)
                {
                    OS_PRINTF("@@@p_category_name=%s, add=0x%x\n", p_param->p_category_name[i], p_param->p_category_name[i]);
                    inbuf = p_param->p_category_name[i];
                    outbuf = (char*) p_data->cateList[i].cate_name;
                    src_len = strlen(inbuf) + 1;
                    dest_len = sizeof(p_data->cateList[i].cate_name);
                    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
                    //utf8_to_utf16(p_param->p_category_name[i], p_data->cateList[i].cate_name, sizeof(p_data->cateList[i].cate_name));
                }

                evt.id = YOUTUBE_EVT_CATE_REQ_OPEN_SUCC;
                evt.data1 = (u32)p_data;
                evt.data2 = (u32)p_data->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            }
            break;

        case YT_DP_EVE_SEARCH_OK:
            p_param = (YT_DP_EVENT_PARAM_T *)param;

            feed_type = FEED_TYPE(p_param->identify_code);
            OS_PRINTF("@@@YT_DP_EVE_SEARCH_OK feed_type=%d, item_num_total=%d, item_num=%d\n", feed_type, p_param->item_num_total, p_param->item_num);
            if (feed_type == SEARCH_FEED_TYPE)
            {
                youtube_vdo_req_t *p_data = ui_youtube_gen_vdo_page(p_param);

                evt.id = YOUTUBE_EVT_SEARCH_SUCC;
                evt.data1 = (u32)p_data;
                evt.data2 = (u32)p_data->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            }
            break;

        case YT_DP_EVE_DEINIT_OK:
            OS_PRINTF("@@@YT_DP_EVE_DEINIT_OK\n");
            break;

        case YT_DP_EVE_NEW_PAGE_ITEM_ARRIVE:
            p_param = (YT_DP_EVENT_PARAM_T *)param;

            feed_type = FEED_TYPE(p_param->identify_code);
            OS_PRINTF("@@@YT_DP_EVE_NEW_PAGE_ITEM_ARRIVE feed_type=%d, item_num_total=%d, item_num=%d\n", feed_type, p_param->item_num_total, p_param->item_num);
            if ((feed_type == VIDEO_FEED)
                || (feed_type == STAND_FEED_TOP_POPULAR) 
                || (feed_type == STAND_FEED_TOP_FAVORITE)
                || (feed_type == STAND_FEED_TOP_RATED)
                || (feed_type == STAND_FEED_MOST_RECENT)
                || (feed_type == STAND_FEED_MOST_RESPONDED)
                || (feed_type == SEARCH_FEED_TYPE))
            {
                youtube_vdo_req_t *p_data = ui_youtube_gen_vdo_page(p_param);

                evt.id = YOUTUBE_EVT_NEW_PAGE_ITEM_ARRIVE;
                evt.data1 = (u32)p_data;
                evt.data2 = (u32)p_data->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            }
            break;

        case YT_DP_EVE_NEW_PLAY_URL_ARRIVE:
            p_param = (YT_DP_EVENT_PARAM_T *)param;

            feed_type = FEED_TYPE(p_param->identify_code);
            OS_PRINTF("@@@YT_DP_EVE_NEW_PLAY_URL_ARRIVE feed_type=%d\n", feed_type);

            if (p_param->filmNum <= 0)
            {
                evt.id = YOUTUBE_EVT_GET_URL_FAIL;
                evt.data2 = (u32)p_param->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
                break;
            }

            p_urlData = (youtube_vdo_play_url_t *)mtos_malloc(sizeof(youtube_vdo_play_url_t));
            MT_ASSERT(p_urlData != NULL);

            sys_status_get_youtube_config_info(&config);
            if (config.resolution == YOUTUBE_RESOLUTION_NORMAL)
            {
                i = (p_param->filmNum - 1);
            }
            else if (config.resolution == YOUTUBE_RESOLUTION_HIGH)
            {
                i = (p_param->filmNum >= 2) ? (p_param->filmNum - 2) : (p_param->filmNum - 1);
            }
            OS_PRINTF("@@@p_url[%d]=%s\n", i, p_param->p_url[i]);

            p_urlData->identify_code = p_param->identify_code;
            len = strlen(p_param->p_url[i]);
            p_urlData->url = (u8 *)mtos_malloc(len+1);
            strcpy((char *)p_urlData->url, (char *)p_param->p_url[i]);

            evt.id = YOUTUBE_EVT_NEW_PLAY_URL_ARRIVE;
            evt.data1 = (u32)p_urlData;
            evt.data2 = (u32)p_urlData->identify_code;
            ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            break;

        case YT_DP_EVE_FAIL_OPERATION:
            p_param = (YT_DP_EVENT_PARAM_T *)param;

            feed_type = FEED_TYPE(p_param->identify_code);
            OS_PRINTF("@@@YT_DP_EVE_FAIL_OPERATION feed_type=%d, fail_no=%d\n", feed_type, p_param->fail_no);
            if (p_param->fail_no == INIT_FAIL)
            {
                if ((feed_type == STAND_FEED_TOP_POPULAR) 
                    || (feed_type == STAND_FEED_TOP_FAVORITE)
                    || (feed_type == STAND_FEED_TOP_RATED)
                    || (feed_type == STAND_FEED_MOST_RECENT)
                    || (feed_type == STAND_FEED_MOST_RESPONDED))
                {
                    evt.id = YOUTUBE_EVT_VDO_REQ_OPEN_FAIL;
                    evt.data2 = (u32)p_param->identify_code;
                    ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
                }
                else if (feed_type == VIDEO_FEED)
                {
                    evt.id = YOUTUBE_EVT_CATE_REQ_OPEN_FAIL;
                    evt.data2 = (u32)p_param->identify_code;
                    ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
                }
            }
            else if (p_param->fail_no == SEARCH_FAIL)
            {
                evt.id = YOUTUBE_EVT_SEARCH_FAIL;
                evt.data2 = (u32)p_param->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            }
            else if (p_param->fail_no == ITEM_NUM_ZERO || p_param->fail_no == GET_PAGE_FAIL)
            {
                evt.id = YOUTUBE_EVT_GET_PAGE_FAIL;
                evt.data2 = (u32)p_param->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            }
            else if (p_param->fail_no == URL_NULL)
            {
                evt.id = YOUTUBE_EVT_GET_URL_FAIL;
                evt.data2 = (u32)p_param->identify_code;
                ap_frm_send_evt_to_ui(APP_YOUTUBE, &evt);
            }
            break;

        case YT_DP_EVE_NEW_CATEGORY_ARRIVE:
            OS_PRINTF("@@@YT_DP_EVE_NEW_CATEGORY_ARRIVE\n");
            break;
        case YT_DP_EVE_SET_MAX_ITEM_NUM_OK:
            OS_PRINTF("@@@YT_DP_EVE_SET_MAX_ITEM_NUM_OK\n");
            break;
        case YT_DP_EVE_GET_CATEGORY_TOTAL_OK:
            OS_PRINTF("@@@YT_DP_EVE_GET_CATEGORY_TOTAL_OK\n");
            break;
        default:
            break;
    }

    return  SUCCESS;
}

void ui_youtube_free_msg_data(u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_req_t *p_vdoReqData = NULL;
    youtube_cate_page_t *p_catePageData = NULL;
    youtube_vdo_play_url_t *p_playUrldata = NULL;

    if (msg < MSG_YT_EVT_BEGIN || msg >= MSG_YT_EVT_END)
    {
        return;
    }

    switch(msg)
    {
        case MSG_YT_EVT_VDO_REQ_OPEN_SUCC:
        case MSG_YT_EVT_SEARCH_SUCC:
        case MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE:
            p_vdoReqData = (youtube_vdo_req_t *)para1;
            if (p_vdoReqData)
            {
                if (p_vdoReqData->vdoList)
                {
                    mtos_free(p_vdoReqData->vdoList);
                }
                mtos_free(p_vdoReqData);
            }
            break;
        case MSG_YT_EVT_CATE_REQ_OPEN_SUCC:
            p_catePageData = (youtube_cate_page_t *)para1;
            if (p_catePageData)
            {
                if (p_catePageData->cateList)
                {
                    mtos_free(p_catePageData->cateList);
                }
                mtos_free(p_catePageData);
            }
            break;
        case MSG_YT_EVT_NEW_PLAY_URL_ARRIVE:
            p_playUrldata = (youtube_vdo_play_url_t *)para1;
            if (p_playUrldata)
            {
                if (p_playUrldata->url)
                {
                    mtos_free(p_playUrldata->url);
                }
                mtos_free(p_playUrldata);
            }
            break;
    }
    
}

void ui_youtube_dp_init(void)
{
#ifndef WIN32
	g_task_param.priority = YT_DP_PRIORITY;
	g_task_param.http_priority = YT_HTTP_PRIORITY;
    g_task_param.p_stack_mem = (char *)mtos_malloc(520*1024);
	memset(g_task_param.p_stack_mem,0,520*1024);
	g_task_param.stack_size = 520*1024;

    g_pYouTubeData =  register_youtube_dataprovider((void *)&g_task_param);
    g_pYouTubeData->register_event_callback(g_pYouTubeData, (void *)youtube_dp_event_callback);
#endif
}

void ui_youtube_dp_deinit(void)
{
#ifndef WIN32
    if (g_pYouTubeData != NULL)
    {
        unregister_youtube_dataprovider((void  *)g_pYouTubeData);
        g_pYouTubeData = NULL;
    }
#endif

   if(g_task_param.p_stack_mem)
   {
       mtos_free(g_task_param.p_stack_mem);
       g_task_param.p_stack_mem = NULL;
   }

}

void ui_youtube_register_msg(void)
{
    OS_PRINTF("@@@ui_youtube_register_msg\n");
    fw_register_ap_evtmap(APP_YOUTUBE, ui_youtube_evtmap);
    fw_register_ap_msghost(APP_YOUTUBE, ROOT_ID_YOUTUBE);
}

void ui_youtube_unregister_msg(void)
{
    OS_PRINTF("@@@ui_youtube_unregister_msg\n");
    fw_unregister_ap_evtmap(APP_YOUTUBE);
    fw_unregister_ap_msghost(APP_YOUTUBE, ROOT_ID_YOUTUBE); 
}

void ui_youtube_request_open(YT_FEED_TYPE type)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->init != NULL)
    {
        g_pYouTubeData->init(g_pYouTubeData, type);
    }
}

void ui_youtube_request_close(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->deinit != NULL)
    {
        g_pYouTubeData->deinit(g_pYouTubeData, INVALD_FEED_TYPE);
    }
}

void ui_youtube_search(u8 *keyword)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->search != NULL)
    {
        g_pYouTubeData->search(g_pYouTubeData, keyword);
    }
}

s32 ui_youtube_get_focus_item_index(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_focus_item_index != NULL)
    {
        return g_pYouTubeData->get_focus_item_index(g_pYouTubeData);
    }

    return -1;
}

s32 ui_youtube_get_focus_category_index(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_focus_category_index != NULL)
    {
        return g_pYouTubeData->get_focus_category_index(g_pYouTubeData);
    }

    return -1;
}

s32 ui_youtube_get_page_index(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_page_index != NULL)
    {
        return g_pYouTubeData->get_page_index(g_pYouTubeData);
    }

    return -1;
}

void ui_youtube_get_next_page(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_next_page != NULL)
    {
        g_pYouTubeData->get_next_page(g_pYouTubeData);
    }
}

void ui_youtube_get_prev_page(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_prev_page != NULL)
    {
        g_pYouTubeData->get_prev_page(g_pYouTubeData);
    }
}

void ui_youtube_get_cur_page(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_cur_page != NULL)
    {
        g_pYouTubeData->get_cur_page(g_pYouTubeData);
    }
}

void ui_youtube_set_category_index(s32 index)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->set_focus_category_index != NULL)
    {
        g_pYouTubeData->set_focus_category_index(g_pYouTubeData, index);
    }
}

void ui_youtube_set_page_index(s32 index)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->set_focus_page_index != NULL)
    {
        g_pYouTubeData->set_focus_page_index(g_pYouTubeData, index);
    }
}

void ui_youtube_set_item_index(s32 index)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->set_focus_item_index != NULL)
    {
        g_pYouTubeData->set_focus_item_index(g_pYouTubeData, index);
    }
}

void ui_youtube_set_max_category_cnt(s32 index)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->set_max_category_cnt != NULL)
    {
        g_pYouTubeData->set_max_category_cnt(g_pYouTubeData, index);
    }
}

void ui_youtube_set_max_item_cnt(s32 index)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->set_max_item_cnt != NULL)
    {
        g_pYouTubeData->set_max_item_cnt(g_pYouTubeData, index);
    }
}

void ui_youtube_get_video_play_url(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_play_url != NULL)
    {
        g_pYouTubeData->get_play_url(g_pYouTubeData);
    }
}

s32 ui_youtube_get_item_num(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_item_num != NULL)
    {
        return g_pYouTubeData->get_item_num(g_pYouTubeData);
    }

    return -1;
}

void ui_youtube_get_category_num(void)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->get_category_num != NULL)
    {
        g_pYouTubeData->get_category_num(g_pYouTubeData);
    }
}

void ui_youtube_init_url(u8 *url)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->init_url != NULL)
    {
        g_pYouTubeData->init_url(g_pYouTubeData, url);
    }
}

void ui_youtube_set_country(YT_REGION_ID_T region)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->set_region_id != NULL)
    {
        g_pYouTubeData->set_region_id(g_pYouTubeData, region);
    }
}

void ui_youtube_set_time_mode(YT_TIME_T mode)
{
    if (g_pYouTubeData != NULL && g_pYouTubeData->set_time != NULL)
    {
        g_pYouTubeData->set_time(g_pYouTubeData, mode);
    }
}


BEGIN_AP_EVTMAP(ui_youtube_evtmap)
CONVERT_EVENT(YOUTUBE_EVT_VDO_REQ_OPEN_SUCC, MSG_YT_EVT_VDO_REQ_OPEN_SUCC)
CONVERT_EVENT(YOUTUBE_EVT_VDO_REQ_OPEN_FAIL, MSG_YT_EVT_VDO_REQ_OPEN_FAIL)
CONVERT_EVENT(YOUTUBE_EVT_CATE_REQ_OPEN_SUCC, MSG_YT_EVT_CATE_REQ_OPEN_SUCC)
CONVERT_EVENT(YOUTUBE_EVT_CATE_REQ_OPEN_FAIL, MSG_YT_EVT_CATE_REQ_OPEN_FAIL)
CONVERT_EVENT(YOUTUBE_EVT_SEARCH_SUCC, MSG_YT_EVT_SEARCH_SUCC)
CONVERT_EVENT(YOUTUBE_EVT_SEARCH_FAIL, MSG_YT_EVT_SEARCH_FAIL)
//CONVERT_EVENT(YOUTUBE_EVT_NEW_CATEGORY_ARRIVE, MSG_YT_EVT_NEW_CATEGORY_ARRIVE)
CONVERT_EVENT(YOUTUBE_EVT_NEW_PAGE_ITEM_ARRIVE, MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE)
CONVERT_EVENT(YOUTUBE_EVT_GET_PAGE_FAIL, MSG_YT_EVT_GET_PAGE_FAIL)
CONVERT_EVENT(YOUTUBE_EVT_NEW_PLAY_URL_ARRIVE, MSG_YT_EVT_NEW_PLAY_URL_ARRIVE)
CONVERT_EVENT(YOUTUBE_EVT_GET_URL_FAIL, MSG_YT_EVT_GET_URL_FAIL)
//CONVERT_EVENT(YOUTUBE_EVT_SET_MAX_ITEM_NUM_OK, MSG_YT_EVT_SET_MAX_ITEM_NUM_OK)
//CONVERT_EVENT(YOUTUBE_EVT_GET_CATEGORY_TOTAL_OK, MSG_YT_EVT_GET_CATEGORY_TOTAL_OK)
END_AP_EVTMAP(ui_youtube_evtmap)

#endif
