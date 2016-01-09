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
#include "redxDataProvider.h"
#include "ui_redtube_api.h"

//#include "ui_rename.h"
//#include "lib_char.h"
#if ENABLE_NETWORK

#define APP_REDTUBE    APP_NVOD

typedef enum
{
    REDTUBE_EVT_NEW_PAGE_VDO_ARRIVE = ((APP_REDTUBE << 16) + 0),
    REDTUBE_EVT_GET_PAGE_VDO_FAIL,
    REDTUBE_EVT_NEW_PLAY_URL_ARRIVE,
    REDTUBE_EVT_GET_PLAY_URL_FAIL,
    REDTUBE_EVT_NEW_CATE_LIST_ARRIVE,
    REDTUBE_EVT_GET_CATE_LIST_FAIL,
    
    REDTUBE_EVT_MAX
} redtube_api_evt_t;

u16 ui_redtube_evtmap(u32 event);

extern iconv_t g_cd_utf8_to_utf16le;

static REDTUBE_DP_HDL_T *g_pRedTubeData = NULL;
static DO_CMD_TASK_CONFIG_T g_p_config;

static redtube_vdo_page_t *ui_redtube_gen_vdo_page(REDTUBE_VIDEO_LIST_T *p_param)
{
    redtube_vdo_page_t *p_data;
    redtube_item *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    s32 view_count = 0;
    float rated = 0;
    u16 i = 0;
    u8  shift = 0;

    p_data = (redtube_vdo_page_t *)mtos_malloc(sizeof(redtube_vdo_page_t));
    MT_ASSERT(p_data != NULL);

    p_data->total_page = p_param->grp_total;
    p_data->count = p_param->num;

    if ((p_param->item_list != NULL) && (p_param->num > 0))
    {
        p_data->vdoList = (redtube_vdo_item_t *)mtos_malloc(p_data->count * sizeof(redtube_vdo_item_t));
        MT_ASSERT(p_data->vdoList != NULL);
        memset(p_data->vdoList, 0, p_data->count * sizeof(redtube_vdo_item_t));

        for (i = 0; i < p_data->count; i++)
        {
            p_item = p_param->item_list + i;

            src_len = strlen(p_item->title) + 1;
            dest_len = sizeof(p_data->vdoList[i].title);
            inbuf = p_item->title;
            outbuf = (char *)p_data->vdoList[i].title;
            iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

            strncpy((char *)p_data->vdoList[i].upload_time, (char *)p_item->added_time, RTB_MAX_UPLOAD_TIME_LEN);

            strncpy((char *)p_data->vdoList[i].duration, (char *)p_item->length, RTB_MAX_DURATION_LEN);

            rated = atof(p_item->rating);
            shift = (u8) ((REDTUBE_RATED_ICON_NUM - rated) / 0.5);
            p_data->vdoList[i].rated_mask = ((REDTUBE_RATED_ICON_MASK >> shift) << shift);
            OS_PRINTF("@@@p_rating=%s, rated_mask=0x%04x\n", p_item->rating, p_data->vdoList[i].rated_mask);

            view_count = atoi(p_item->view_times);
            if (view_count < 0)
            {
                view_count = 0;
            }
            p_data->vdoList[i].view_count = view_count;
            OS_PRINTF("@@@p_viewcount=%s, view_count=%d\n", p_item->view_times, p_data->vdoList[i].view_count);

            if (strlen(p_item->image) > 0)
            {
                strncpy((char *)p_data->vdoList[i].thumnail_url, (char *)p_item->image, RTB_MAX_IMAGE_URL_LEN);
            }
        }
    }

    return p_data;
}

static RET_CODE  redtube_dp_event_callback(REDTUBE_EVENT_TYPE event, u32 param)
{
    event_t evt = {0};

    switch(event)
    {
        case REDTUBE_VIDEO_LIST_SUCCESS:
        {
            REDTUBE_VIDEO_LIST_T *p_param = (REDTUBE_VIDEO_LIST_T *)param;
            REDTUBE_VIDEO_TYPE video_type = p_param->video_type;

            OS_PRINTF("@@@REDTUBE_VIDEO_LIST_SUCCESS video_type=%d, total_page=%d, item_num=%d\n", p_param->video_type, p_param->grp_total, p_param->num);
            if ((video_type == REDTUBE_VIDEO_DEF) 
                || (video_type == REDTUBE_VIDEO_CATE)
                || (video_type == REDTUBE_VIDEO_SEARCH))
            {
                redtube_vdo_page_t *p_data = ui_redtube_gen_vdo_page(p_param);

                evt.id = REDTUBE_EVT_NEW_PAGE_VDO_ARRIVE;
                evt.data1 = (u32)p_data;
                evt.data2 = (u32)video_type;
                ap_frm_send_evt_to_ui(APP_REDTUBE, &evt);
            }
        }
            break;

        case REDTUBE_VIDEO_LIST_FAIL:
        {
            OS_PRINTF("@@@REDTUBE_VIDEO_LIST_FAIL\n");
            evt.id = REDTUBE_EVT_GET_PAGE_VDO_FAIL;
            ap_frm_send_evt_to_ui(APP_REDTUBE, &evt);
        }
            break;

        case REDTUBE_CATE_GET_SUCCESS:
        {
            REDTUBE_CATE_LIST_T *p_param = (REDTUBE_CATE_LIST_T *)param;
            redtube_cate_page_t *p_data = NULL;
            char *inbuf, *outbuf;
            size_t src_len, dest_len;
            u16 i = 0;

            OS_PRINTF("@@@REDTUBE_CATE_GET_SUCCESS total_count=%d\n", p_param->cate_total);
            p_data = (redtube_cate_page_t *)mtos_malloc(sizeof(redtube_cate_page_t));
            MT_ASSERT(p_data != NULL);

            p_data->count = p_param->cate_total;

            if (p_param->cate_total > 0)
            {
                p_data->cateList = (redtube_cate_item_t *)mtos_malloc(p_data->count * sizeof(redtube_cate_item_t));
                MT_ASSERT(p_data->cateList != NULL);
                memset(p_data->cateList, 0, p_data->count * sizeof(redtube_cate_item_t));

                for (i = 0; i < p_data->count; i++)
                {
                    OS_PRINTF("@@@cate_name=%s, addr=0x%x\n", p_param->cate_name[i], p_param->cate_name[i]);
                    inbuf = p_param->cate_name[i];
                    outbuf = (char*) p_data->cateList[i].cate_name;
                    src_len = strlen(inbuf) + 1;
                    dest_len = sizeof(p_data->cateList[i].cate_name);
                    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
                }
            }

            evt.id = REDTUBE_EVT_NEW_CATE_LIST_ARRIVE;
            evt.data1 = (u32)p_data;
            ap_frm_send_evt_to_ui(APP_REDTUBE, &evt);
        }
            break;

        case REDTUBE_CATE_GET_FAIL:
        {
            OS_PRINTF("@@@REDTUBE_CATE_GET_FAIL\n");
            evt.id = REDTUBE_EVT_GET_CATE_LIST_FAIL;
            ap_frm_send_evt_to_ui(APP_REDTUBE, &evt);
        }
            break;

        case REDTUBE_URL_GET_SUCCESS:
        {
            REDTUBE_VIDEO_URL_T *p_param = (REDTUBE_VIDEO_URL_T *)param;
            u16 str_len = strlen(p_param->video_url);

            OS_PRINTF("@@@REDTUBE_URL_GET_SUCCESS\n");
            if (str_len > 0)
            {
                redtube_vdo_play_url_t *p_data = (redtube_vdo_play_url_t *)mtos_malloc(sizeof(redtube_vdo_play_url_t));
                MT_ASSERT(p_data != NULL);

                p_data->url = (u8 *)mtos_malloc(str_len+1);
                MT_ASSERT(p_data->url != NULL);

                OS_PRINTF("@@@video_url=%s\n", p_param->video_url);
                strcpy((char *)p_data->url, (char *)p_param->video_url);

                evt.id = REDTUBE_EVT_NEW_PLAY_URL_ARRIVE;
                evt.data1 = (u32)p_data;
                ap_frm_send_evt_to_ui(APP_REDTUBE, &evt);
            }
            else
            {
                evt.id = REDTUBE_EVT_GET_PLAY_URL_FAIL;
                ap_frm_send_evt_to_ui(APP_REDTUBE, &evt);
                break;
            }
        }
            break;

        case REDTUBE_URL_GET_FAIL:
        {
            OS_PRINTF("@@@REDTUBE_URL_GET_FAIL\n");
            evt.id = REDTUBE_EVT_GET_PLAY_URL_FAIL;
            ap_frm_send_evt_to_ui(APP_REDTUBE, &evt);
        }
            break;

        default:
            break;
    }

    return  SUCCESS;
}

void ui_redtube_free_msg_data(u16 msg, u32 para1, u32 para2)
{
    if (msg < MSG_RT_EVT_BEGIN || msg >= MSG_RT_EVT_END)
    {
        return;
    }

    switch(msg)
    {
        case MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE:
        {
            redtube_vdo_page_t *p_data = (redtube_vdo_page_t *)para1;
            if (p_data)
            {
                if (p_data->vdoList)
                {
                    mtos_free(p_data->vdoList);
                }
                mtos_free(p_data);
            }
        }
            break;
        case MSG_RT_EVT_NEW_PLAY_URL_ARRIVE:
        {
            redtube_vdo_play_url_t *p_data = (redtube_vdo_play_url_t *)para1;
            if (p_data)
            {
                if (p_data->url)
                {
                    mtos_free(p_data->url);
                }
                mtos_free(p_data);
            }
        }
            break;
        case MSG_RT_EVT_NEW_CATE_LIST_ARRIVE:
        {
            redtube_cate_page_t *p_data = (redtube_cate_page_t *)para1;
            if (p_data)
            {
                if (p_data->cateList)
                {
                    mtos_free(p_data->cateList);
                }
                mtos_free(p_data);
            }
        }
            break;
        default:
            break;
    }
}

void ui_redtube_dp_init(void)
{
    memset(&g_p_config, 0, sizeof(g_p_config));
#ifndef WIN32
    g_p_config.priority = YT_DP_PRIORITY;
    g_p_config.http_priority = YT_HTTP_PRIORITY;
    g_p_config.p_mem_start = (u8 *)mtos_malloc(520*1024);
    g_p_config.stack_size = 520*1024;
    g_pRedTubeData = register_redtube_dataprovider(&g_p_config);
    g_pRedTubeData->register_event_callback(g_pRedTubeData, redtube_dp_event_callback);
#endif
}

void ui_redtube_dp_deinit(void)
{
#ifndef WIN32
    if (g_pRedTubeData != NULL)
    {
        unregister_redtube_dataprovider(g_pRedTubeData);
    }
#endif
    if(g_p_config.p_mem_start)
    {
         mtos_free(g_p_config.p_mem_start);
         g_p_config.p_mem_start = NULL;
    }
}

void ui_redtube_register_msg(void)
{
    OS_PRINTF("@@@ui_redtube_register_msg\n");
    fw_register_ap_evtmap(APP_REDTUBE, ui_redtube_evtmap);
    fw_register_ap_msghost(APP_REDTUBE, ROOT_ID_REDTUBE);
}

void ui_redtube_unregister_msg(void)
{
    OS_PRINTF("@@@ui_redtube_unregister_msg\n");
    fw_unregister_ap_evtmap(APP_REDTUBE);
    fw_unregister_ap_msghost(APP_REDTUBE, ROOT_ID_REDTUBE); 
}

void ui_redtube_set_page_size(s32 size)
{
    if (g_pRedTubeData != NULL && g_pRedTubeData->set_grp_page_size != NULL)
    {
        g_pRedTubeData->set_grp_page_size(g_pRedTubeData, size);
    }
}

void ui_redtube_get_default_video(s32 page_idx)
{
    if (g_pRedTubeData != NULL && g_pRedTubeData->get_default_video != NULL)
    {
        g_pRedTubeData->get_default_video(g_pRedTubeData, page_idx);
    }
}

void ui_redtube_get_cate_list(void)
{
    if (g_pRedTubeData != NULL && g_pRedTubeData->get_category_num != NULL)
    {
        g_pRedTubeData->get_category_num(g_pRedTubeData);
    }
}

void ui_redtube_get_cate_video(s32 cate_idx, s32 page_idx)
{
    if (g_pRedTubeData != NULL && g_pRedTubeData->get_category_video != NULL)
    {
        g_pRedTubeData->get_category_video(g_pRedTubeData, cate_idx, page_idx);
    }
}

void ui_redtube_search(u8 *keyword)
{
    if (g_pRedTubeData != NULL && g_pRedTubeData->search != NULL)
    {
        g_pRedTubeData->search(g_pRedTubeData, keyword, 0);
    }
}

void ui_redtube_get_search_video(s32 page_idx)
{
    if (g_pRedTubeData != NULL && g_pRedTubeData->get_default_video != NULL)
    {
        g_pRedTubeData->get_default_video(g_pRedTubeData, page_idx);
    }
}

void ui_redtube_get_play_url(s32 index)
{
    if (g_pRedTubeData != NULL && g_pRedTubeData->get_play_url != NULL)
    {
        g_pRedTubeData->get_play_url(g_pRedTubeData, index);
    }
}


BEGIN_AP_EVTMAP(ui_redtube_evtmap)
CONVERT_EVENT(REDTUBE_EVT_NEW_PAGE_VDO_ARRIVE, MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE)
CONVERT_EVENT(REDTUBE_EVT_GET_PAGE_VDO_FAIL, MSG_RT_EVT_GET_PAGE_VDO_FAIL)
CONVERT_EVENT(REDTUBE_EVT_NEW_CATE_LIST_ARRIVE, MSG_RT_EVT_NEW_CATE_LIST_ARRIVE)
CONVERT_EVENT(REDTUBE_EVT_GET_CATE_LIST_FAIL, MSG_RT_EVT_GET_CATE_LIST_FAIL)
CONVERT_EVENT(REDTUBE_EVT_NEW_PLAY_URL_ARRIVE, MSG_RT_EVT_NEW_PLAY_URL_ARRIVE)
CONVERT_EVENT(REDTUBE_EVT_GET_PLAY_URL_FAIL, MSG_RT_EVT_GET_PLAY_URL_FAIL)
//CONVERT_EVENT(REDTUBE_EVT_SET_MAX_ITEM_NUM_OK, MSG_YT_EVT_SET_MAX_ITEM_NUM_OK)
//CONVERT_EVENT(REDTUBE_EVT_GET_CATEGORY_TOTAL_OK, MSG_YT_EVT_GET_CATEGORY_TOTAL_OK)
END_AP_EVTMAP(ui_redtube_evtmap)

#endif
