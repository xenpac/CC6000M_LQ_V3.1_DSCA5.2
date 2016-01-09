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
//#include "commonData.h"
#include "NetMediaDataProvider.h"
#include "al_netmedia.h"

//#include "ui_rename.h"
//#include "lib_char.h"
#if ENABLE_NETWORK
#define APP_NETMEDIA    APP_GBOX

typedef enum
{
    EVT_NETMEDIA_NEW_PAGE_VDO_ARRIVED = ((APP_NETMEDIA << 16) + 0),
    EVT_NETMEDIA_GET_PAGE_VDO_FAILED,
    EVT_NETMEDIA_NEW_SEARCH_VDO_ARRIVED,
    EVT_NETMEDIA_GET_SEARCH_VDO_FAILED,
    EVT_NETMEDIA_NEW_PROGRAM_INFO_ARRIVED,
    EVT_NETMEDIA_GET_PROGRAM_INFO_FAILED,
    EVT_NETMEDIA_NEW_COLLECTION_LIST_ARRIVED,
    EVT_NETMEDIA_GET_COLLECTION_LIST_FAILED,
    EVT_NETMEDIA_NEW_PLAY_URLS_ARRIVED,
    EVT_NETMEDIA_GET_PLAY_URLS_FAILED,

    EVT_NETMEDIA_NEW_CLASS_LIST_ARRIVED,
    EVT_NETMEDIA_GET_CLASS_LIST_FAILED,
    EVT_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED,
    EVT_NETMEDIA_GET_SUBCLASS_LIST_FAILED,
    EVT_NETMEDIA_WEBSITE_INIT_SUCCESS,
    EVT_NETMEDIA_WEBSITE_INIT_FAILED,
    EVT_NETMEDIA_WEBSITE_DEINIT_SUCCESS,
    EVT_NETMEDIA_WEBSITE_DEINIT_FAILED,
    EVT_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED,
    EVT_NETMEDIA_GET_WEBSITE_INFO_FAILED,

    EVT_NETMEDIA_INIT_SUCCESS,
    EVT_NETMEDIA_INIT_FAILED,
    EVT_NETMEDIA_DEINIT_SUCCESS,
    EVT_NETMEDIA_DEINIT_FAILED,

    EVT_NETMEDIA_MAX
} evt_netmedia_t;

u16 al_netmedia_evtmap(u32 event);

extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;
static handle_t g_hNetMedia = NULL;
static volatile u32 g_vdo_identify_code;

static al_netmedia_vdo_list_t *al_netmedia_gen_vdo_list(NM_PAGE_INFO_T *p_param)
{
    al_netmedia_vdo_list_t *p_data;
    NM_ITEM_INFO_T *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    p_data = (al_netmedia_vdo_list_t *)mtos_malloc(sizeof(al_netmedia_vdo_list_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_netmedia_vdo_list_t));

    if ((p_param->items != NULL) && (p_param->count > 0))
    {
        p_data->total_vdo = (u16)p_param->total_items;
        p_data->vdo_cnt = (u16)p_param->count;
        p_data->vdoList = (al_netmedia_vdo_item_t *)mtos_malloc(p_param->count * sizeof(al_netmedia_vdo_item_t));
        MT_ASSERT(p_data->vdoList != NULL);
        memset(p_data->vdoList, 0, p_param->count * sizeof(al_netmedia_vdo_item_t));

        for (i = 0; i < p_param->count; i++)
        {
            p_item = p_param->items + i;

            if (p_item->item_id != NULL)
            {
                dest_len = strlen(p_item->item_id) + 1;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)p_item->item_id);
                p_data->vdoList[i].id = (u8 *)outbuf;
            }
            if (p_item->name != NULL)
            {
                src_len = strlen(p_item->name) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->name;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->vdoList[i].title = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);

                src_len = strlen(p_item->name) + 1;
                inbuf = p_item->name;
                outbuf = (char *)mtos_malloc(src_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, inbuf);
                p_data->vdoList[i].key = (u8 *)outbuf;
            }
            if (p_item->image_url != NULL)
            {
                dest_len = strlen(p_item->image_url) + 1;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)p_item->image_url);
                p_data->vdoList[i].img_url = (u8 *)outbuf;
            }
            if (p_item->date != NULL)
            {
                src_len = strlen(p_item->date) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->date;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->vdoList[i].date = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
            }
            if (p_item->duration != NULL)
            {
                src_len = strlen(p_item->duration) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->duration;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->vdoList[i].duration = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
            }
            if (p_item->description != NULL)
            {
                src_len = strlen(p_item->description) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->description;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->vdoList[i].description = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
            }
            if (p_item->score != NULL)
            {
                p_data->vdoList[i].score = atof(p_item->score);
            }
        }
    }

    return p_data;
}

static void al_netmedia_free_vdo_list(u32 param)
{
    al_netmedia_vdo_list_t *p_data = (al_netmedia_vdo_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->vdoList)
        {
            for (i = 0; i < p_data->vdo_cnt; i++)
            {
                if (p_data->vdoList[i].id)
                {
                    mtos_free(p_data->vdoList[i].id);
                }
                if (p_data->vdoList[i].title)
                {
                    mtos_free(p_data->vdoList[i].title);
                }
                if (p_data->vdoList[i].key)
                {
                    mtos_free(p_data->vdoList[i].key);
                }
                if (p_data->vdoList[i].img_url)
                {
                    mtos_free(p_data->vdoList[i].img_url);
                }
                if (p_data->vdoList[i].date)
                {
                    mtos_free(p_data->vdoList[i].date);
                }
                if (p_data->vdoList[i].duration)
                {
                    mtos_free(p_data->vdoList[i].duration);
                }
                if (p_data->vdoList[i].description)
                {
                    mtos_free(p_data->vdoList[i].description);
                }
            }
            mtos_free(p_data->vdoList);
        }
        mtos_free(p_data);
    }
}

static al_netmedia_program_info_t *al_netmedia_gen_program_info(NM_PROGINFO_T *p_param)
{
    al_netmedia_program_info_t *p_data;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    s32 view_count;
    p_data = (al_netmedia_program_info_t *)mtos_malloc(sizeof(al_netmedia_program_info_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_netmedia_program_info_t));
    if (p_param->item_id != NULL)
    {
        dest_len = strlen(p_param->item_id) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_param->item_id);
        p_data->id = (u8 *)outbuf;
    }
    if (p_param->name != NULL)
    {
        src_len = strlen(p_param->name) + 1;
        dest_len = src_len * sizeof(u16);
        inbuf = p_param->name;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        p_data->title = (u16 *)outbuf;
        iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
        src_len = strlen(p_param->name) + 1;
        inbuf = p_param->name;
        outbuf = (char *)mtos_malloc(src_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, inbuf);
        p_data->key = (u8 *)outbuf;
    }
    if (p_param->image_url != NULL)
    {
        dest_len = strlen(p_param->image_url) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_param->image_url);
        p_data->img_url = (u8 *)outbuf;
    }
    if (p_param->date != NULL)
    {
        src_len = strlen(p_param->date) + 1;
        dest_len = src_len * sizeof(u16);
        inbuf = p_param->date;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        p_data->date = (u16 *)outbuf;
        iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
    }
    if (p_param->duration != NULL)
    {
        src_len = strlen(p_param->duration) + 1;
        dest_len = src_len * sizeof(u16);
        inbuf = p_param->duration;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        p_data->duration = (u16 *)outbuf;
        iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
    }
    if (p_param->description != NULL)
    {
        src_len = strlen(p_param->description) + 1;
        dest_len = src_len * sizeof(u16);
        inbuf = p_param->description;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        p_data->description = (u16 *)outbuf;
        iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
    }
    if (p_param->score != NULL)
    {
        p_data->score = atof(p_param->score);
    }
    if (p_param->view_count != NULL)
    {
        view_count = atoi(p_param->view_count);
        if (view_count >= 0)
        {
            p_data->view_count = view_count;
        }
    }
    p_data->total_collection = p_param->collection_count;
    return p_data;
}
static void al_netmedia_free_program_info(u32 param)
{
    al_netmedia_program_info_t *p_data = (al_netmedia_program_info_t *)param;
    if (p_data)
    {
        if (p_data->id)
        {
            mtos_free(p_data->id);
        }
        if (p_data->title)
        {
            mtos_free(p_data->title);
        }
        if (p_data->key)
        {
            mtos_free(p_data->key);
        }
        if (p_data->img_url)
        {
            mtos_free(p_data->img_url);
        }
        if (p_data->date)
        {
            mtos_free(p_data->date);
        }
        if (p_data->duration)
        {
            mtos_free(p_data->duration);
        }
        if (p_data->description)
        {
            mtos_free(p_data->description);
        }
        mtos_free(p_data);
    }
}
static al_netmedia_collection_list_t *al_netmedia_gen_collection_list(NM_COLLETSINFO_T *p_param)
{
    al_netmedia_collection_list_t *p_data;
    NM_COLLETSINFO_ITEM_T *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;
    if ((p_param->collect_info != NULL) && (p_param->count > 0))
    {
        p_data = (al_netmedia_collection_list_t *)mtos_malloc(sizeof(al_netmedia_collection_list_t));
        MT_ASSERT(p_data != NULL);
        p_data->collection_cnt = (u16)p_param->count;
        p_data->collectionList = (al_netmedia_collection_item_t *)mtos_malloc(p_param->count * sizeof(al_netmedia_collection_item_t));
        MT_ASSERT(p_data->collectionList != NULL);
        memset(p_data->collectionList, 0, p_param->count * sizeof(al_netmedia_collection_item_t));
        for (i = 0; i < p_param->count; i++)
        {
            p_item = p_param->collect_info + i;
            if (p_item->collect_id != NULL)
            {
                dest_len = strlen(p_item->collect_id) + 1;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)p_item->collect_id);
                p_data->collectionList[i].id = (u8 *)outbuf;
            }
            if (p_item->collect_name != NULL)
            {
                src_len = strlen(p_item->collect_name) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->collect_name;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->collectionList[i].name = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
                src_len = strlen(p_item->collect_name) + 1;
                inbuf = p_item->collect_name;
                outbuf = (char *)mtos_malloc(src_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, inbuf);
                p_data->collectionList[i].key = (u8 *)outbuf;
            }
        }
        return p_data;
    }
    return NULL;
}
static void al_netmedia_free_collection_list(u32 param)
{
    al_netmedia_collection_list_t *p_data = (al_netmedia_collection_list_t *)param;
    u16 i;
    if (p_data)
    {
        if (p_data->collectionList)
        {
            for (i = 0; i < p_data->collection_cnt; i++)
            {
                if (p_data->collectionList[i].id)
                {
                    mtos_free(p_data->collectionList[i].id);
                }
                if (p_data->collectionList[i].name)
                {
                    mtos_free(p_data->collectionList[i].name);
                }
                if (p_data->collectionList[i].key)
                {
                    mtos_free(p_data->collectionList[i].key);
                }
            }
            mtos_free(p_data->collectionList);
        }
        mtos_free(p_data);
    }
}
static al_netmedia_play_url_list_t *al_netmedia_gen_play_url_list(NM_PLAY_URLS_T *p_param)
{
    al_netmedia_play_url_list_t *p_data;
    NM_PLAY_URL_ITEM *p_item;
    char *outbuf;
    size_t dest_len;
    u16 i, j;

    if ((p_param->playurls != NULL) && (p_param->url_count > 0))
    {
        p_data = (al_netmedia_play_url_list_t *)mtos_malloc(sizeof(al_netmedia_play_url_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->play_url_cnt = (u16)p_param->url_count;
        p_data->playUrlList = (al_netmedia_play_url_item_t *)mtos_malloc(p_param->url_count * sizeof(al_netmedia_play_url_item_t));
        MT_ASSERT(p_data->playUrlList != NULL);
        memset(p_data->playUrlList, 0, p_param->url_count * sizeof(al_netmedia_play_url_item_t));

        for (i = 0; i < p_param->url_count; i++)
        {
            p_item = p_param->playurls + i;

            p_data->playUrlList[i].resolution = p_item->resolution;
            p_data->playUrlList[i].fragment_url_cnt = p_item->count;
            p_data->playUrlList[i].pp_fragmntUrlList = (u8 **)mtos_malloc(p_item->count * sizeof(u8 *));
            MT_ASSERT(p_data->playUrlList[i].pp_fragmntUrlList != NULL);
            memset(p_data->playUrlList[i].pp_fragmntUrlList, 0, p_item->count * sizeof(u8 *));
            if (p_item->playurl != NULL)
            {
                for (j = 0; j < p_item->count; j++)
                {
                    if (p_item->playurl[j] != NULL)
                    {
                        dest_len = strlen(p_item->playurl[j]) + 1;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                        strcpy((char *)outbuf, (char *)p_item->playurl[j]);
                        p_data->playUrlList[i].pp_fragmntUrlList[j] = (u8 *)outbuf;
                    }
                }
            }
        }

        return p_data;
    }

    return NULL;
}

static void al_netmedia_free_play_url_list(u32 param)
{
    al_netmedia_play_url_list_t *p_data = (al_netmedia_play_url_list_t *)param;
    u16 i, j;

    if (p_data)
    {
        if (p_data->playUrlList)
        {
            for (i = 0; i < p_data->play_url_cnt; i++)
            {
                if (p_data->playUrlList[i].pp_fragmntUrlList)
                {
                    for (j = 0; j < p_data->playUrlList[i].fragment_url_cnt; j++)
                    {
                        if (p_data->playUrlList[i].pp_fragmntUrlList[j])
                        {
                            mtos_free(p_data->playUrlList[i].pp_fragmntUrlList[j]);
                        }
                    }
                    mtos_free(p_data->playUrlList[i].pp_fragmntUrlList);
                }
            }
            mtos_free(p_data->playUrlList);
        }
        mtos_free(p_data);
    }
}

static al_netmedia_class_list_t *al_netmedia_gen_class_list(NM_CLASS_INFO_T *p_param)
{
    al_netmedia_class_list_t *p_data;
    NM_CLASS_ITEM_INFO_T *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    p_data = (al_netmedia_class_list_t *)mtos_malloc(sizeof(al_netmedia_class_list_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_netmedia_class_list_t));

    if ((p_param->class_item != NULL) && (p_param->count > 0))
    {
        p_data->class_cnt = (u16)p_param->count;
        p_data->classList = (al_netmedia_class_item_t *)mtos_malloc(p_param->count * sizeof(al_netmedia_class_item_t));
        MT_ASSERT(p_data->classList != NULL);
        memset(p_data->classList, 0, p_param->count * sizeof(al_netmedia_class_item_t));

        for (i = 0; i < p_param->count; i++)
        {
            p_item = p_param->class_item + i;

            p_data->classList[i].id = p_item->class_id;
            p_data->classList[i].attr = p_item->attr;
            if (p_item->class_name != NULL)
            {
                src_len = strlen(p_item->class_name) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->class_name;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->classList[i].name = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);

                src_len = strlen(p_item->class_name) + 1;
                inbuf = p_item->class_name;
                outbuf = (char *)mtos_malloc(src_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, inbuf);
                p_data->classList[i].key = (u8 *)outbuf;
           }
        }
    }

    return p_data;
}

static void al_netmedia_free_class_list(u32 param)
{
    al_netmedia_class_list_t *p_data = (al_netmedia_class_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->classList)
        {
            for (i = 0; i < p_data->class_cnt; i++)
            {
                if (p_data->classList[i].name)
                {
                    mtos_free(p_data->classList[i].name);
                }
                if (p_data->classList[i].key)
                {
                    mtos_free(p_data->classList[i].key);
                }
            }
            mtos_free(p_data->classList);
        }
        mtos_free(p_data);
    }
}

static al_netmedia_website_info_t *al_netmedia_gen_website_info(NM_WEBSITE_INFO_T *p_param)
{
    al_netmedia_website_info_t *p_data;
    NM_WEBSITE_ITEM_T *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;
    if ((p_param->item != NULL) && (p_param->count > 0))
    {
        p_data = (al_netmedia_website_info_t *)mtos_malloc(sizeof(al_netmedia_website_info_t));
        MT_ASSERT(p_data != NULL);
        p_data->web_count = (u8)p_param->count;
        p_data->website_list = (al_netmedia_website_item_t *)mtos_malloc(p_param->count * sizeof(al_netmedia_website_item_t));
        MT_ASSERT(p_data->website_list != NULL);
        memset(p_data->website_list, 0, p_param->count * sizeof(al_netmedia_website_item_t));
        for (i = 0; i < p_param->count; i++)
        {
            p_item = p_param->item + i;
            p_data->website_list[i].ui_style = p_item->ui_style;
            p_data->website_list[i].ui_attr = p_item->ui_attr;
            p_data->website_list[i].dp_type = p_item->dp_type; 
            if (p_item->title != NULL)
            {
                src_len = strlen(p_item->title) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->title;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->website_list[i].title = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
            }
            if (p_item->logo_url != NULL)
            {
                src_len = strlen(p_item->logo_url) + 1;
                inbuf = p_item->logo_url;
                outbuf = (char *)mtos_malloc(src_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, inbuf);
                p_data->website_list[i].logo_url = (u8 *)outbuf;
            }
        }
        return p_data;
    }
    return NULL;
}
static void al_netmedia_free_website_info(u32 param)
{
    al_netmedia_website_info_t *p_data = (al_netmedia_website_info_t *)param;
    u16 i;
    if (p_data)
    {
        if (p_data->website_list)
        {
            for (i = 0; i < p_data->web_count; i++)
            {
                if (p_data->website_list[i].title)
                {
                    mtos_free(p_data->website_list[i].title);
                }
                if (p_data->website_list[i].logo_url)
                {
                    mtos_free(p_data->website_list[i].logo_url);
                }
            }
            mtos_free(p_data->website_list);
        }
        mtos_free(p_data);
    }
}
RET_CODE  al_netmedia_dp_event_callback(NM_EVENT_TYPE event, u32 param1, u32 param2)
{
    event_t evt;

    switch(event)
    {
        case NM_PAGE_ARRIVAL:
        {
            NM_PAGE_INFO_T *p_param = (NM_PAGE_INFO_T *)param2;
            MT_ASSERT(p_param != NULL);

            if (p_param->identify == g_vdo_identify_code)
            {
                OS_PRINTF("@@@NM_PAGE_ARRIVAL STATUS=%d\n", param1);
                if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
                {
                    al_netmedia_vdo_list_t *p_data = al_netmedia_gen_vdo_list(p_param);

                    evt.id = EVT_NETMEDIA_NEW_PAGE_VDO_ARRIVED;
                    evt.data1 = p_param->identify;
                    evt.data2 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                    break;
                }
                evt.id = EVT_NETMEDIA_GET_PAGE_VDO_FAILED;
				evt.data1 = p_param->identify;
                ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            }
        }
            break;

        case NM_SEARCH_ARRIVAL:
        {
            NM_PAGE_INFO_T *p_param = (NM_PAGE_INFO_T *)param2;
            MT_ASSERT(p_param != NULL);

            if (p_param->identify == g_vdo_identify_code)
            {
                OS_PRINTF("@@@NM_SEARCH_ARRIVAL STATUS=%d\n", param1);
                if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
                {
                    al_netmedia_vdo_list_t *p_data = al_netmedia_gen_vdo_list(p_param);

                    evt.id = EVT_NETMEDIA_NEW_SEARCH_VDO_ARRIVED;
                    evt.data1 = p_param->identify;
                    evt.data2 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                    break;
                }
                evt.id = EVT_NETMEDIA_GET_SEARCH_VDO_FAILED;
				evt.data1 = p_param->identify;
                ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            }
        }
            break;
        case NM_PROGRAM_INFO_ARRIVAL:
            if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
            {
                NM_PROGINFO_T *p_param = (NM_PROGINFO_T *)param2;
                if (p_param != NULL)
                {
                    al_netmedia_program_info_t *p_data = al_netmedia_gen_program_info(p_param);
                    if (p_data != NULL)
                    {
                        evt.id = EVT_NETMEDIA_NEW_PROGRAM_INFO_ARRIVED;
                        evt.data2 = (u32)p_data;
                        ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                        break;
                    }
                }
            }
            evt.id = EVT_NETMEDIA_GET_PROGRAM_INFO_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_COLLECTS_INFO_ARRIVAL:
            if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
            {
                NM_COLLETSINFO_T *p_param = (NM_COLLETSINFO_T *)param2;
                if (p_param != NULL)
                {
                    al_netmedia_collection_list_t *p_data = al_netmedia_gen_collection_list(p_param);
                    if (p_data != NULL)
                    {
                        evt.id = EVT_NETMEDIA_NEW_COLLECTION_LIST_ARRIVED;
                        evt.data2 = (u32)p_data;
                        ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                        break;
                    }
                }
            }
            evt.id = EVT_NETMEDIA_GET_COLLECTION_LIST_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;

        case NM_PLAY_URLS_ARRIVAL:
            if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
            {
                NM_PLAY_URLS_T *p_param = (NM_PLAY_URLS_T *)param2;
                if (p_param != NULL)
                {
                    al_netmedia_play_url_list_t *p_data = al_netmedia_gen_play_url_list(p_param);

                    if (p_data != NULL)
                    {
                        evt.id = EVT_NETMEDIA_NEW_PLAY_URLS_ARRIVED;
                        evt.data2 = (u32)p_data;
                        ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                        break;
                    }
                }
            }
            evt.id = EVT_NETMEDIA_GET_PLAY_URLS_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;

        case NM_CLASS_ARRIVAL:
            if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
            {
                NM_CLASS_INFO_T *p_param = (NM_CLASS_INFO_T *)param2;
                if (p_param != NULL)
                {
                    al_netmedia_class_list_t *p_data = al_netmedia_gen_class_list(p_param);

                    evt.id = EVT_NETMEDIA_NEW_CLASS_LIST_ARRIVED;
                    evt.data2 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                    break;
                }
            }
            evt.id = EVT_NETMEDIA_GET_CLASS_LIST_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;

        case NM_SUBCLASS_ARRIVAL:
            if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
            {
                NM_CLASS_INFO_T *p_param = (NM_CLASS_INFO_T *)param2;
                if (p_param != NULL)
                {
                    al_netmedia_class_list_t *p_data = al_netmedia_gen_class_list(p_param);

                    evt.id = EVT_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED;
                    evt.data2 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                    break;
                }
            }
            evt.id = EVT_NETMEDIA_GET_SUBCLASS_LIST_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_WEBSITE_INIT_SUCCESS:
            OS_PRINTF("@@@NM_WEBSITE_INIT_SUCCESS\n");
            evt.id = EVT_NETMEDIA_WEBSITE_INIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_WEBSITE_INIT_FAIL:
            OS_PRINTF("@@@NM_WEBSITE_INIT_FAIL\n");
            evt.id = EVT_NETMEDIA_WEBSITE_INIT_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_WEBSITE_DEINIT_SUCCESS:
            OS_PRINTF("@@@NM_WEBSITE_DEINIT_SUCCESS\n");
            evt.id = EVT_NETMEDIA_WEBSITE_DEINIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_WEBSITE_DEINIT_FAIL:
            OS_PRINTF("@@@NM_WEBSITE_INIT_FAIL\n");
            evt.id = EVT_NETMEDIA_WEBSITE_DEINIT_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_WEBSITE_INFO_ARRIVAL:
            if ((NETMEDIA_DATA_STATUS)param1 == NETMEDIA_DATA_SUCCESS)
            {
                NM_WEBSITE_INFO_T *p_param = (NM_WEBSITE_INFO_T *)param2;
                if (p_param != NULL)
                {
                    al_netmedia_website_info_t *p_data = al_netmedia_gen_website_info(p_param);
                    if (p_data != NULL)
                    {
                        evt.id = EVT_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED;
                        evt.data2 = (u32)p_data;
                        ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
                        break;
                    }
                }
            }
            evt.id = EVT_NETMEDIA_GET_WEBSITE_INFO_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;

        case NM_INIT_SUCCESS:
            OS_PRINTF("@@@NM_INIT_SUCCESS\n");
            evt.id = EVT_NETMEDIA_INIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_INIT_FAIL:
            OS_PRINTF("@@@NM_INIT_FAIL\n");
            evt.id = EVT_NETMEDIA_INIT_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_DEINIT_SUCCESS:
            OS_PRINTF("@@@NM_DEINIT_SUCCESS\n");
            evt.id = EVT_NETMEDIA_DEINIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;
        case NM_DEINIT_FAIL:
            OS_PRINTF("@@@NM_DEINIT_FAIL\n");
            evt.id = EVT_NETMEDIA_DEINIT_FAILED;
            ap_frm_send_evt_to_ui(APP_NETMEDIA, &evt);
            break;

        default:
            break;
    }

    return  SUCCESS;
}

void al_netmedia_free_msg_data(u16 msg, u32 para1, u32 para2)
{
    if (msg < MSG_NETMEDIA_BEGIN || msg >= MSG_NETMEDIA_END)
    {
        return;
    }

    switch(msg)
    {
        case MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED:
        case MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED:
            al_netmedia_free_vdo_list(para2);
            break;
        case MSG_NETMEDIA_NEW_PROGRAM_INFO_ARRIVED:
            al_netmedia_free_program_info(para2);
            break;
        case MSG_NETMEDIA_NEW_COLLECTION_LIST_ARRIVED:
            al_netmedia_free_collection_list(para2);
            break;
        case MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED:
            al_netmedia_free_play_url_list(para2);
            break;
        case MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED:
        case MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED:
            al_netmedia_free_class_list(para2);
            break;
        case MSG_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED:
            al_netmedia_free_website_info(para2);
            break;    
        default:
            break;
    }
}

void al_netmedia_register_msg(void)
{
    OS_PRINTF("@@@al_netmedia_register_msg\n");
    fw_register_ap_evtmap(APP_NETMEDIA, al_netmedia_evtmap);
    fw_register_ap_msghost(APP_NETMEDIA, ROOT_ID_SUBMENU_NM);
}
void al_netmedia_unregister_msg(void)
{
    OS_PRINTF("@@@al_netmedia_unregister_msg\n");
    fw_unregister_ap_evtmap(APP_NETMEDIA);
    fw_unregister_ap_msghost(APP_NETMEDIA, ROOT_ID_SUBMENU_NM);
}

void al_netmedia_dp_init(void)
{
#ifndef WIN32
    NM_INIT_PARAM task_param;

   	task_param.priority = NM_DP_PRIORITY;
   	//task_param.nm_dp_type = type;
	task_param.cb = al_netmedia_dp_event_callback;
    g_hNetMedia =  NetMedia_initDataProvider(&task_param);
#endif
    g_vdo_identify_code = 0;
}
void al_netmedia_dp_deinit(void)
{
    if (g_hNetMedia != NULL)
    {
#ifndef WIN32
        NetMedia_deinitDataProvider(g_hNetMedia);
#endif
    }
}

void al_netmedia_website_init(NETMEDIA_DP_TYPE nm_dp_type)
{
    NM_WEBSITE_INIT_PARAM param;
    if (g_hNetMedia != NULL)
    {
        param.nm_dp_type = nm_dp_type;
#ifndef WIN32
        NetMedia_initWebsite(g_hNetMedia, &param);
#endif
    }
}

void al_netmedia_website_deinit(void)
{
    if (g_hNetMedia != NULL)
    {
#ifndef WIN32
        NetMedia_deinitWebsite(g_hNetMedia);
#endif
    }
}

void al_netmedia_get_website_info(void)
{
    if (g_hNetMedia != NULL)
    {
    	#ifndef WIN32
        NetMedia_getWebsiteInfo(g_hNetMedia);
	#endif
    }
}

void al_netmedia_get_class_list(void)
{
    if (g_hNetMedia != NULL)
    {
    	#ifndef WIN32
        NetMedia_getClassList(g_hNetMedia);
	#endif
    }
}

void al_netmedia_get_subclass_list(u32 class_id, u8 *class_key)
{
    NM_SUBCLASS_REQ req;

    if (g_hNetMedia != NULL)
    {
        memset(&req, 0, sizeof(NM_SUBCLASS_REQ));

        req.class_id = class_id;
        strcpy(req.class_name, class_key);
	#ifndef WIN32
        NetMedia_getSubClassList(g_hNetMedia, &req);
	#endif
    }
}

void al_netmedia_set_page_size(u16 page_size)
{
    if (g_hNetMedia != NULL)
    {
    	#ifndef WIN32
        NetMedia_setPageSize(g_hNetMedia, page_size);
	#endif
    }
}

void al_netmedia_get_video_list(u32 class_id, u8 *class_key, u32 subclass_id, u8 *subclass_key, u32 page_num)
{
    NM_UPDATEPAGE_REQ req;

    if (g_hNetMedia != NULL)
    {
        req.class_id = class_id;
        if (class_key != NULL)
        {
            strcpy(req.class_name, class_key);
        }
        req.subclass_id = subclass_id;
        if (subclass_key != NULL)
        {
            strcpy(req.subclass_name, subclass_key);
        }
        req.page_number = page_num;
        req.cb = al_netmedia_vdo_idntfy_cmp;
        g_vdo_identify_code++;
        req.identify = g_vdo_identify_code;
	#ifndef WIN32
        NetMedia_updatPage(g_hNetMedia, &req);
	#endif
    }
}

void al_netmedia_set_search_keyword(u16 *keyword)
{
    char *inbuf, *outbuf, *p_ascStr;
    size_t str_len, src_len, dest_len;

    if (g_hNetMedia != NULL)
    {
    str_len = uni_strlen(keyword);

    src_len = (str_len + 1) * sizeof(u16);
    dest_len = str_len * 3 + 1;
    inbuf = (char *)keyword;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_ascStr = outbuf;
    iconv(g_cd_utf16le_to_utf8, (char **)&inbuf, &src_len, (char **)&outbuf, &dest_len);
	#ifndef WIN32
    NetMedia_setSearchKeyword(g_hNetMedia, p_ascStr);
	#endif

    mtos_free(p_ascStr);
    }
}

void al_netmedia_search(u32 class_id, u32 page_num)
{
    NM_SEARCH_REQ req;

    if (g_hNetMedia != NULL)
    {
        req.page_number = page_num;
        req.cb = al_netmedia_vdo_idntfy_cmp;
        g_vdo_identify_code++;
        req.identify = g_vdo_identify_code;
	#ifndef WIN32
        NetMedia_search(g_hNetMedia, &req);
	#endif
    }
}

void al_netmedia_get_play_urls(u8 *vdo_id, u8 *vdo_key)
{
    NM_PLAY_URLS_REQ req;

    if (g_hNetMedia != NULL)
    {
        memset(&req, 0, sizeof(NM_PLAY_URLS_REQ));

        strcpy(req.item_id, vdo_id);
        strcpy(req.item_name, vdo_key);
	#ifndef WIN32
        NetMedia_getPlayUrls(g_hNetMedia, &req);
	#endif
    }
}

int al_netmedia_vdo_idntfy_cmp(u32 unidntfy_code)
{
    return g_vdo_identify_code - unidntfy_code;
}

BEGIN_AP_EVTMAP(al_netmedia_evtmap)
CONVERT_EVENT(EVT_NETMEDIA_NEW_PAGE_VDO_ARRIVED, MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_PAGE_VDO_FAILED, MSG_NETMEDIA_GET_PAGE_VDO_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_NEW_SEARCH_VDO_ARRIVED, MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_SEARCH_VDO_FAILED, MSG_NETMEDIA_GET_SEARCH_VDO_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_NEW_PROGRAM_INFO_ARRIVED, MSG_NETMEDIA_NEW_PROGRAM_INFO_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_PROGRAM_INFO_FAILED, MSG_NETMEDIA_GET_PROGRAM_INFO_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_NEW_COLLECTION_LIST_ARRIVED, MSG_NETMEDIA_NEW_COLLECTION_LIST_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_COLLECTION_LIST_FAILED, MSG_NETMEDIA_GET_COLLECTION_LIST_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_NEW_PLAY_URLS_ARRIVED, MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_PLAY_URLS_FAILED, MSG_NETMEDIA_GET_PLAY_URLS_FAILED)

CONVERT_EVENT(EVT_NETMEDIA_NEW_CLASS_LIST_ARRIVED, MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_CLASS_LIST_FAILED, MSG_NETMEDIA_GET_CLASS_LIST_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED, MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_SUBCLASS_LIST_FAILED, MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED)

CONVERT_EVENT(EVT_NETMEDIA_WEBSITE_INIT_SUCCESS, MSG_NETMEDIA_WEBSITE_INIT_SUCCESS)
CONVERT_EVENT(EVT_NETMEDIA_WEBSITE_INIT_FAILED, MSG_NETMEDIA_WEBSITE_INIT_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_WEBSITE_DEINIT_SUCCESS, MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS)
CONVERT_EVENT(EVT_NETMEDIA_WEBSITE_DEINIT_FAILED, MSG_NETMEDIA_WEBSITE_DEINIT_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED, MSG_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED)
CONVERT_EVENT(EVT_NETMEDIA_GET_WEBSITE_INFO_FAILED, MSG_NETMEDIA_GET_WEBSITE_INFO_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_INIT_SUCCESS, MSG_NETMEDIA_INIT_SUCCESS)
CONVERT_EVENT(EVT_NETMEDIA_INIT_FAILED, MSG_NETMEDIA_INIT_FAILED)
CONVERT_EVENT(EVT_NETMEDIA_DEINIT_SUCCESS, MSG_NETMEDIA_DEINIT_SUCCESS)
CONVERT_EVENT(EVT_NETMEDIA_DEINIT_FAILED, MSG_NETMEDIA_DEINIT_FAILED)
END_AP_EVTMAP(al_netmedia_evtmap)

#endif
