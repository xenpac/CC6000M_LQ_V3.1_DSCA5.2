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
#include "IPTVDataProvider.h"
#include "ui_iptv_api.h"
#include "ui_iptv_prot.h"

//#include "lib_char.h"
#if ENABLE_NETWORK

#define IS_MASKED(x, mask)    (((x) & (mask)) == mask)

typedef enum
{
    IPTV_EVT_INIT_SUCCESS = ((APP_IPTV << 16) + 0),
    IPTV_EVT_INIT_FAIL,
    IPTV_EVT_DEINIT_SUCCESS,
    IPTV_EVT_NEW_RES_NAME_ARRIVE,
    IPTV_EVT_GET_RES_NAME_FAIL,
    IPTV_EVT_NEW_RES_CATGRY_ARRIVE,
    IPTV_EVT_GET_RES_CATGRY_FAIL,
    IPTV_EVT_NEW_PAGE_VDO_ARRIVE,
    IPTV_EVT_GET_PAGE_VDO_FAIL,
    IPTV_EVT_NEW_SEARCH_VDO_ARRIVE,
    IPTV_EVT_GET_SEARCH_VDO_FAIL,
    IPTV_EVT_NEW_VDO_INFO_ARRIVE,
    IPTV_EVT_GET_VDO_INFO_FAIL,
    IPTV_EVT_NEW_INFO_URL_ARRIVE,
    IPTV_EVT_GET_INFO_URL_FAIL,
    IPTV_EVT_NEW_PLAY_URL_ARRIVE,
    IPTV_EVT_GET_PLAY_URL_FAIL,
    IPTV_EVT_NEW_RECMND_INFO_ARRIVE,
    IPTV_EVT_GET_RECMND_INFO_FAIL,
    IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE,
    IPTV_EVT_GET_SOURCE_FORMAT_FAIL,
    IPTV_EVT_MAX
} iptv_api_evt_t;

typedef enum
{
    IPTV_QUERY_MODE_CATGRY,
    IPTV_QUERY_MODE_PINYIN,
} iptv_query_mode_t;


u16 ui_iptv_evtmap(u32 event);


extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;

static IPTV_DP_HDL_T *g_pIptvData = NULL;
static volatile u32 vdo_identify_code;
u32 search_res_id;
static u16 g_dp_state = IPTV_DP_STATE_UNINIT;
static u8  search_keyword[MAX_KEYWORD_LENGTH*3+1];

static al_iptv_name_res_list_t *ui_iptv_gen_name_res_list(IPTV_CATEGORY_LIST_T *p_param)
{
    al_iptv_name_res_list_t *p_data = NULL;
    IPTV_CATEGORY_T *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    if ((p_param->array != NULL) && (p_param->number > 0))
    {
        p_data = (al_iptv_name_res_list_t *)mtos_malloc(sizeof(al_iptv_name_res_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_res = (u16)p_param->number;
        p_data->resList = (al_iptv_name_res_item_t *)mtos_malloc(p_data->total_res * sizeof(al_iptv_name_res_item_t));
        MT_ASSERT(p_data->resList != NULL);
        memset(p_data->resList, 0, p_data->total_res * sizeof(al_iptv_name_res_item_t));

        for (i = 0; i < p_data->total_res; i++)
        {
            p_item = p_param->array + i;

            p_data->resList[i].res_id = p_item->id;
            if (p_item->name != NULL)
            {
                src_len = strlen(p_item->name) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->name;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->resList[i].name = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
           }
        }
    }

    return p_data;
}

static void ui_iptv_free_name_res_list(u32 param)
{
    al_iptv_name_res_list_t *p_data = (al_iptv_name_res_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->resList)
        {
            for (i = 0; i < p_data->total_res; i++)
            {
                if (p_data->resList[i].name)
                {
                    mtos_free(p_data->resList[i].name);
                }
            }
            mtos_free(p_data->resList);
        }
        mtos_free(p_data);
    }
}

static al_iptv_cat_res_list_t *ui_iptv_gen_cat_res_list(IPTV_CATEGORY_TYPE_ALL_T *p_param)
{
    al_iptv_cat_res_list_t *p_data = NULL;
    IPTV_CATEGORY_TYPE_LIST_T *p_res_item;
    IPTV_CATEGORY_TYPE_T *p_cat_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i, j;

    if ((p_param->p_list != NULL) && (p_param->cat_count > 0))
    {
        p_data = (al_iptv_cat_res_list_t *)mtos_malloc(sizeof(al_iptv_cat_res_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_res = (u16)p_param->cat_count;
        p_data->resList = (al_iptv_cat_res_item_t *)mtos_malloc(p_data->total_res * sizeof(al_iptv_cat_res_item_t));
        MT_ASSERT(p_data->resList != NULL);
        memset(p_data->resList, 0, p_data->total_res * sizeof(al_iptv_cat_res_item_t));

        for (i = 0; i < p_data->total_res; i++)
        {
            p_res_item = p_param->p_list + i;

            p_data->resList[i].res_id = p_res_item->cat_id;
            if ((p_res_item->array != NULL) && (p_res_item->item_count > 0))
            {
                p_data->resList[i].total_cat = p_res_item->item_count;
                p_data->resList[i].catList = (al_iptv_cat_item_t *)mtos_malloc(p_data->resList[i].total_cat * sizeof(al_iptv_cat_item_t));
                MT_ASSERT(p_data->resList[i].catList != NULL);
                memset(p_data->resList[i].catList, 0, p_data->resList[i].total_cat * sizeof(al_iptv_cat_item_t));

                for (j = 0; j < p_data->resList[i].total_cat; j++)
                {
                    p_cat_item = p_res_item->array + j;

                    if (p_cat_item->title != NULL)
                    {
                        src_len = strlen(p_cat_item->title) + 1;
                        dest_len = src_len * sizeof(u16);
                        inbuf = p_cat_item->title;
                        OS_PRINTF("Title: %s\n", p_cat_item->title);
                        {
                          int ii;
                          for (ii=0; ii<src_len; ii++)
                          {
                            OS_PRINTF("%02x ", p_cat_item->title[ii]);
                          }
                          OS_PRINTF("\n");
                        }
                        outbuf = (char *)mtos_malloc(dest_len);
                        MT_ASSERT(outbuf != NULL);
                        p_data->resList[i].catList[j].name = (u16 *)outbuf;
                        iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
                        OS_PRINTF("After iconv, src_len %d, dest_len %d\n", src_len, dest_len);
                    }

                    if (p_cat_item->keys != NULL)
                    {
                        dest_len = strlen(p_cat_item->keys) + 1;
                        outbuf = (char *)mtos_malloc(dest_len);
                        MT_ASSERT(outbuf != NULL);
                        strcpy((char *)outbuf, (char *)p_cat_item->keys);
                        p_data->resList[i].catList[j].key = (u8 *)outbuf;
                    }

                    p_data->resList[i].catList[j].total_vdo = p_cat_item->total;
                }
            }
            else
            {
                p_data->resList[i].total_cat = 0;
                p_data->resList[i].catList = NULL;
            }
        }
    }

    return p_data;
}

static void ui_iptv_free_cat_res_list(u32 param)
{
    al_iptv_cat_res_list_t *p_data = (al_iptv_cat_res_list_t *)param;
    u16 i, j;

    if (p_data)
    {
        if (p_data->resList)
        {
            for (i = 0; i < p_data->total_res; i++)
            {
                if (p_data->resList[i].catList)
                {
                    for (j = 0; j < p_data->resList[i].total_cat; j++)
                    {
                        if (p_data->resList[i].catList[j].name)
                        {
                            mtos_free(p_data->resList[i].catList[j].name);
                        }
                        if (p_data->resList[i].catList[j].key)
                        {
                            mtos_free(p_data->resList[i].catList[j].key);
                        }
                    }
                    mtos_free(p_data->resList[i].catList);
                }
            }
            mtos_free(p_data->resList);
        }
        mtos_free(p_data);
    }
}

static al_iptv_vdo_list_t *ui_iptv_gen_vdo_list(IPTV_CHANNEL_LIST_T *p_param)
{
    al_iptv_vdo_list_t *p_data = NULL;
    IPTV_CHANNEL_T *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    p_data = (al_iptv_vdo_list_t *)mtos_malloc(sizeof(al_iptv_vdo_list_t));
    MT_ASSERT(p_data != NULL);

    if ((p_param->array != NULL) && (p_param->number > 0))
    {
        p_data->total_page = (u32)p_param->pagecount;
        p_data->page_num = (u32)p_param->page_num;
        p_data->total_vdo = (u32)p_param->totalcount;
        p_data->vdo_cnt = (u16)p_param->number;
        p_data->vdoList = (al_iptv_vdo_item_t *)mtos_malloc(p_data->vdo_cnt * sizeof(al_iptv_vdo_item_t));
        MT_ASSERT(p_data->vdoList != NULL);
        memset(p_data->vdoList, 0, p_data->vdo_cnt * sizeof(al_iptv_vdo_item_t));

        OS_PRINTF("@@@number=%d", p_param->number);
        for (i = 0; i < p_data->vdo_cnt; i++)
        {
            p_item = p_param->array + i;
            OS_PRINTF("@@@i=%d, vdo_id=%d, res_id=%d\n", i, p_item->id, p_item->cat_id);
            p_data->vdoList[i].vdo_id = (u32)p_item->id;
            p_data->vdoList[i].res_id = (u32)p_item->cat_id;
            p_data->vdoList[i].b_single_page = (u8)p_item->issinglepage;
            if (p_item->title != NULL)
            {
                src_len = strlen(p_item->title) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->title;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->vdoList[i].name = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
            }
            if (p_item->image != NULL)
            {
                dest_len = strlen(p_item->image) + 1;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)p_item->image);
                p_data->vdoList[i].img_url = (u8 *)outbuf;
            }
        }
    }
    else
    {
        p_data->total_page = (u32)p_param->pagecount;
        p_data->page_num = (u32)p_param->page_num;
        p_data->total_vdo = (u32)p_param->totalcount;
        p_data->vdo_cnt = (u16)p_param->number;
        p_data->vdoList = NULL;
    }

    return p_data;
}

static void ui_iptv_free_vdo_list(u32 param)
{
    al_iptv_vdo_list_t *p_data = (al_iptv_vdo_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->vdoList)
        {
            for (i = 0; i < p_data->vdo_cnt; i++)
            {
                if (p_data->vdoList[i].name)
                {
                    mtos_free(p_data->vdoList[i].name);
                }
                if (p_data->vdoList[i].img_url)
                {
                    mtos_free(p_data->vdoList[i].img_url);
                }
            }
            mtos_free(p_data->vdoList);
        }
        mtos_free(p_data);
    }
}

u16 chrn (char *str, char c)
{
    u16 cnt = 0;
    while (*str != '\0')
    {
        if (*str == c)
        {
            cnt++;
        }
        str++;
    }
    return cnt;
}
static void ui_iptv_add_orgn_item(al_iptv_vdo_info_t *p_data, char *name)
{
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    src_len = strlen(name) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = name;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->orgnList[p_data->orgn_cnt].name = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    p_data->orgn_cnt++;
}
static al_iptv_vdo_info_t *ui_iptv_gen_vdo_info(IPTV_CHANNEL_INFO_T *p_param)
{
    al_iptv_vdo_info_t *p_data = NULL;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u8 separator = ',';
    char *p, *q;
    u16  cnt;

    p_data = (al_iptv_vdo_info_t *)mtos_malloc(sizeof(al_iptv_vdo_info_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_iptv_vdo_info_t));

    p_data->res_id = (u32)p_param->cat_id;
    p_data->vdo_id = (u32)p_param->id;

    if(p_param->title != NULL)
    {
    OS_PRINTF("@@111@%s %s\n", __FUNCTION__, p_param->title);
    src_len = strlen(p_param->title) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->title;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->name = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    OS_PRINTF("@@@%s %s %s\n", __FUNCTION__, outbuf, p_data->name);
    }

    if(p_param->area != NULL)
    {
    src_len = strlen(p_param->area) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->area;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->area = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

    if(p_param->director != NULL)
   {
    src_len = strlen(p_param->director) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->director;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->director = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

    if(p_param->starring != NULL)
    {
    src_len = strlen(p_param->starring) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->starring;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->actor = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }
    if(p_param->years != NULL)
    {
    src_len = strlen(p_param->years) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->years;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->years = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

    if(p_param->types != NULL)
    {
    src_len = strlen(p_param->types) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->types;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->attr = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }
    if(p_param->score != NULL)
    {
    src_len = strlen(p_param->score) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->score;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->score = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

   if(p_param->mark != NULL)
   {
    src_len = strlen(p_param->mark) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->mark;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->description = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

    if(p_param->img != NULL)
    {
    dest_len = strlen(p_param->img) + 1;
    outbuf = (char *)mtos_malloc(dest_len);
    MT_ASSERT(outbuf != NULL);
    strcpy((char *)outbuf, (char *)p_param->img);
    p_data->img_url = (u8 *)outbuf;
    }
    if (p_param->origins != NULL)
    {
        cnt = chrn(p_param->origins, separator) + 1;
        p_data->orgnList = mtos_malloc(cnt * sizeof(al_iptv_orgn_item_t));
        MT_ASSERT(p_data->orgnList != NULL);
        q = p_param->origins;
        while (strlen(q) > 0)
        {
            p = strchr(q, separator);
            if (p == NULL)
            {
                ui_iptv_add_orgn_item(p_data, q);
                break;
            }
            else if (*p == separator)
            {
                *p = '\0';
                p++;
                ui_iptv_add_orgn_item(p_data, q);
                q = p;
                continue;
            }
            break;
        }
    }

    return p_data;
}

static void ui_iptv_free_vdo_info(u32 param)
{
    al_iptv_vdo_info_t *p_data = (al_iptv_vdo_info_t *)param;
    if (p_data)
    {
        if (p_data->name)
        {
            mtos_free(p_data->name);
        }
        if (p_data->area)
        {
            mtos_free(p_data->area);
        }
        if (p_data->director)
        {
            mtos_free(p_data->director);
        }
        if (p_data->actor)
        {
            mtos_free(p_data->actor);
        }
        if (p_data->score)
        {
            mtos_free(p_data->score);
        }
        if (p_data->description)
        {
            mtos_free(p_data->description);
        }
        if (p_data->attr)
        {
            mtos_free(p_data->attr);
        }
        if (p_data->years)
        {
            mtos_free(p_data->years);
        }
        if (p_data->img_url)
        {
            mtos_free(p_data->img_url);
        }
        if (p_data->orgnList)
        {
            mtos_free(p_data->orgnList);
        }
        mtos_free(p_data);
    }
}

static al_iptv_info_url_list_t *ui_iptv_gen_info_url(IPTV_CHANNEL_INFO_T *p_param)
{
    al_iptv_info_url_list_t *p_data = NULL;
    IPTV_CHANNEL_URL_T *p_item;
    char *outbuf;
    size_t dest_len,src_len;
    u16 i;
    char *inbuf;

    p_data = (al_iptv_info_url_list_t *)mtos_malloc(sizeof(al_iptv_info_url_list_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_iptv_info_url_list_t));

    OS_PRINTF("url count @@@@@@@ %d\n",p_param->number);
    p_data->count = (u32)p_param->number;
    p_data->page_count = (u32)p_param->page_count;
    p_data->page_total = (u32)p_param->total_count;

    p_data->urlList = (al_iptv_info_url_item_t *)mtos_malloc(p_data->count * sizeof(al_iptv_info_url_item_t));
    memset( p_data->urlList,0,p_data->count * sizeof(al_iptv_info_url_item_t));
    MT_ASSERT(p_data->urlList != NULL);

    for (i = 0; i < p_data->count; i++)
    {
        p_item = p_param->urls + i;
        if(p_item->playurl != NULL)
        {
          dest_len = strlen(p_item->playurl) + 1;
          outbuf = (char *)mtos_malloc(dest_len);
          MT_ASSERT(outbuf != NULL);
          strcpy((char *)outbuf, (char *)p_item->playurl);
          p_data->urlList[i].playurl = (u8 *)outbuf;
        }
        if(p_item->collect != NULL)
        {
        dest_len = strlen(p_item->collect) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->collect);
        p_data->urlList[i].collect = (u8 *)outbuf;
        }
        if(p_item->origin != NULL)
        {
        dest_len = strlen(p_item->origin) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->origin);
        p_data->urlList[i].origin = (u8 *)outbuf;
        }
        //url title
        if(p_item->urltitle != NULL)
        {
          src_len = strlen(p_item->urltitle) + 1;
          dest_len = src_len * sizeof(u16);
          inbuf = p_item->urltitle;
          outbuf = (char *)mtos_malloc(dest_len);
          MT_ASSERT(outbuf != NULL);
          p_data->urlList[i].urltitle = (u16 *)outbuf;
          iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }
    }

    return p_data;
}

static void ui_iptv_free_info_url(u32 param)
{
    al_iptv_info_url_list_t *p_data = (al_iptv_info_url_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->urlList)
        {
            for (i = 0; i < p_data->count; i++)
            {
                if (p_data->urlList[i].playurl)
                {
                    mtos_free(p_data->urlList[i].playurl);
                }
                 if (p_data->urlList[i].origin)
                {
                    mtos_free(p_data->urlList[i].origin);
                }
                if (p_data->urlList[i].collect)
                {
                    mtos_free(p_data->urlList[i].collect);
                }
                if (p_data->urlList[i].urltitle)
                {
                    mtos_free(p_data->urlList[i].urltitle);
                } 
            }
            mtos_free(p_data->urlList);
        }
        mtos_free(p_data);
    }
}

static al_iptv_recomand_info_t *ui_iptv_gen_remend_info(IPTV_RECMD_INFO_T *p_param)
{
    al_iptv_recomand_info_t *p_data = NULL;
    IPTV_RECMD_CHANNEL_T *p_item;
    u8 i = 0;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;

    p_data = (al_iptv_recomand_info_t *)mtos_malloc(sizeof(al_iptv_recomand_info_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_iptv_recomand_info_t));
    
    p_data->number = (u32)p_param->channel_num;
    if(p_data->number == 0)
    {
        return p_data;
    }
    
    p_data->recmd  = (iptv_recmd_channel_t *)mtos_malloc(p_data->number * sizeof(iptv_recmd_channel_t));
    memset(p_data->recmd,0,p_data->number * sizeof(iptv_recmd_channel_t));
    MT_ASSERT(p_data->recmd != NULL);

    for (i = 0; i < p_data->number; i++)
    {
     p_item = p_param->p_channel + i;
      if(p_item->title != NULL)
      {
      src_len = strlen(p_item->title) + 1;
      dest_len = src_len * sizeof(u16);
      inbuf = p_item->title;
      outbuf = (char *)mtos_malloc(dest_len);
      MT_ASSERT(outbuf != NULL);
       p_data->recmd[i].title= (u16 *)outbuf;
      iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
      }
#if 0
        dest_len = strlen(p_item->title) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->title);
        p_data->recmd[i].title = (u8 *)outbuf;
#endif
        if(p_item->types != NULL)
       {
        dest_len = strlen(p_item->types) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->types);
        p_data->recmd[i].types = (u8 *)outbuf;
        }
        if(p_item->score != NULL)
        {
         dest_len = strlen(p_item->score) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->score);
        p_data->recmd[i].score = (u8 *)outbuf;
        }
        if(p_item->currentcollect != NULL)
        {
        dest_len = strlen(p_item->currentcollect) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->currentcollect);
        p_data->recmd[i].currentcollect = (u8 *)outbuf;
        }
        
        if(p_item->clarity != NULL)
        {
        dest_len = strlen(p_item->clarity) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->clarity);
        p_data->recmd[i].clarity = (u8 *)outbuf;
        }

        p_data->recmd[i].id = atoi(p_item->id);
        p_data->recmd[i].cat_id = p_item->cat_id;

        if(p_item->img != NULL)
        {
        dest_len = strlen(p_item->img) + 1;
        outbuf = (char *)mtos_malloc(dest_len);
        MT_ASSERT(outbuf != NULL);
        strcpy((char *)outbuf, (char *)p_item->img);
        p_data->recmd[i].img = (u8 *)outbuf;
        }
    }
    return p_data;
}

static void ui_iptv_free_remend_info(u32 param)
{
    al_iptv_recomand_info_t *p_data = (al_iptv_recomand_info_t *)param;
    u16 i;
    if (p_data)
    {
        if (p_data->recmd)
        {
            for (i = 0; i < p_data->number; i++)
            {
                if (p_data->recmd[i].title)
                {
                    mtos_free(p_data->recmd[i].title);
                }
                if (p_data->recmd[i].img)
                {
                    mtos_free(p_data->recmd[i].img);
                }
                if (p_data->recmd[i].types)
                {
                    mtos_free(p_data->recmd[i].types);
                }
                if (p_data->recmd[i].score)
                {
                    mtos_free(p_data->recmd[i].score);
                }
                if (p_data->recmd[i].currentcollect)
                {
                    mtos_free(p_data->recmd[i].currentcollect);
                }
                if (p_data->recmd[i].clarity)
                {
                    mtos_free(p_data->recmd[i].clarity);
                }
            }
            mtos_free(p_data->recmd);
        }
        mtos_free(p_data);
    }
}

static al_iptv_source_format_t *ui_iptv_gen_source_format(u32 param)
{
    al_iptv_source_format_t *p_data = NULL;

    if (param > 0)
    {
        p_data = (al_iptv_source_format_t *)mtos_malloc(sizeof(al_iptv_source_format_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_format = 0;
        p_data->formatList = (u8 *)mtos_malloc(IPTV_MAX_FORMAT_COUNT);
        MT_ASSERT(p_data->formatList != NULL);

        if (IS_MASKED(param, IPTV_API_FORMAT_NORMAL))
        {
            p_data->formatList[p_data->total_format] = IPTV_API_FORMAT_NORMAL;
            p_data->total_format++;
        }
        if (IS_MASKED(param, IPTV_API_FORMAT_HIGH))
        {
            p_data->formatList[p_data->total_format] = IPTV_API_FORMAT_HIGH;
            p_data->total_format++;
        }
        if (IS_MASKED(param, IPTV_API_FORMAT_SUPER))
        {
            p_data->formatList[p_data->total_format] = IPTV_API_FORMAT_SUPER;
            p_data->total_format++;
        }
        if (IS_MASKED(param, IPTV_API_FORMAT_TOPSPEED))
        {
            p_data->formatList[p_data->total_format] = IPTV_API_FORMAT_TOPSPEED;
            p_data->total_format++;
        }
    }

    return p_data;
}

static void ui_iptv_free_source_format(u32 param)
{
    al_iptv_source_format_t *p_data = (al_iptv_source_format_t *)param;

    if (p_data)
    {
        if (p_data->formatList)
        {
            mtos_free(p_data->formatList);
        }
        mtos_free(p_data);
    }
}

static al_iptv_play_url_list_t *ui_iptv_gen_play_url(IPTV_CHANNEL_PLAY_URL_T *p_param)
{
    al_iptv_play_url_list_t *p_data = NULL;
    IPTV_CHANNEL_URL_T *p_item;
    char *outbuf;
    size_t dest_len;
    u16 i;

    if ((p_param->playurl != NULL) && (p_param->number > 0))
    {
        p_data = (al_iptv_play_url_list_t *)mtos_malloc(sizeof(al_iptv_play_url_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_url = (u16)p_param->number;
        p_data->pp_urlList = (u8 **)mtos_malloc(p_data->total_url * sizeof(u8 *));
        MT_ASSERT(p_data->pp_urlList != NULL);

        for (i = 0; i < p_data->total_url; i++)
        {
            p_item = p_param->playurl + i;

            if (p_item->playurl != NULL)
            {
                dest_len = strlen(p_item->playurl) + 1;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)p_item->playurl);
                p_data->pp_urlList[i] = (u8 *)outbuf;
            }
        }
    }

    return p_data;
}

static void ui_iptv_free_play_url(u32 param)
{
    al_iptv_play_url_list_t *p_data = (al_iptv_play_url_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->pp_urlList)
        {
            for (i = 0; i < p_data->total_url; i++)
            {
                if (p_data->pp_urlList[i])
                {
                    mtos_free(p_data->pp_urlList[i]);
                }
            }
            mtos_free(p_data->pp_urlList);
        }
        mtos_free(p_data);
    }
}


static RET_CODE  iptv_dp_event_callback(IPTV_EVENT_TYPE event, u32 param)
{
    event_t evt = {0};
    IPTV_CHANNEL_INFO_T *p_param = NULL;
    OS_PRINTF("%s: event %d start\n", __FUNCTION__, event);
    switch(event)
    {
        case IPTV_INIT_SUCCESS:
        {
            evt.id = IPTV_EVT_INIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_INIT_FAIL:
        {
            evt.id = IPTV_EVT_INIT_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_DEINIT_SUCCESS:
        {
            evt.id = IPTV_EVT_DEINIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_CATEGORY_ARRIVAL:
        {
            if((IPTV_CATEGORY_LIST_T *)param != NULL)
            {
             IPTV_CATEGORY_LIST_T *p_param = (IPTV_CATEGORY_LIST_T *)param;
 
             OS_PRINTF("@@@IPTV_CATEGORY_ARRIVAL status=%d\n", p_param->status);
             if (p_param->status == IPTV_DATA_SUCCESS)
             {
                 al_iptv_name_res_list_t *p_data = ui_iptv_gen_name_res_list(p_param);
 
                 if (p_data != NULL)
                 {
                     evt.id = IPTV_EVT_NEW_RES_NAME_ARRIVE;
                     evt.data1 = (u32)p_data;
                     ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                     break;
                 }
             }
            }
            evt.id = IPTV_EVT_GET_RES_NAME_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_CATEGORY_TYPE_ARRIVAL:
        {
           if((IPTV_CATEGORY_TYPE_ALL_T *)param != NULL)
           {
            IPTV_CATEGORY_TYPE_ALL_T *p_param = (IPTV_CATEGORY_TYPE_ALL_T *)param;

            OS_PRINTF("@@@IPTV_CATEGORY_TYPE_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_cat_res_list_t *p_data = ui_iptv_gen_cat_res_list(p_param);

                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_RES_CATGRY_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
           }
            evt.id = IPTV_EVT_GET_RES_CATGRY_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_NEW_PAGE_ARRIVAL:
        {
            IPTV_CHANNEL_LIST_T *p_param = (IPTV_CHANNEL_LIST_T *)param;

            OS_PRINTF("@@@IPTV_NEW_PAGE_ARRIVAL status=%d\n", p_param->status);
            if (p_param->identify == vdo_identify_code)
            {
                if (p_param->user_data0 == IPTV_QUERY_MODE_CATGRY)
                {
                    OS_PRINTF("@@@IPTV_QUERY_MODE_CATGRY\n");
                    if (p_param->status == IPTV_DATA_SUCCESS)
                    {
                        al_iptv_vdo_list_t *p_data = ui_iptv_gen_vdo_list(p_param);

                        evt.id = IPTV_EVT_NEW_PAGE_VDO_ARRIVE;
                        evt.data1 = (u32)p_data;
                        evt.data2 = p_param->identify;
                        ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                        break;
                    }
                    evt.id = IPTV_EVT_GET_PAGE_VDO_FAIL;
                    evt.data2 = p_param->identify;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
                else if (p_param->user_data0 == IPTV_QUERY_MODE_PINYIN)
                {
                    OS_PRINTF("@@@IPTV_QUERY_MODE_PINYIN\n");
                    if (p_param->status == IPTV_DATA_SUCCESS)
                    {
                        al_iptv_vdo_list_t *p_data = ui_iptv_gen_vdo_list(p_param);

                        evt.id = IPTV_EVT_NEW_SEARCH_VDO_ARRIVE;
                        evt.data1 = (u32)p_data;
                        evt.data2 = p_param->identify;
                        ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                        break;
                    }
                    evt.id = IPTV_EVT_GET_SEARCH_VDO_FAIL;
                    evt.data2 = p_param->identify;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
            }
            else
            {
                OS_PRINTF("@@@identify=%d vdo_identify_code=%d\n", p_param->identify, vdo_identify_code);
            }
        }
            break;

        case IPTV_VIDEO_INFO_ARRIVAL:
        {
            if((IPTV_CHANNEL_INFO_T *)param ==NULL)
            {  
                evt.id = IPTV_EVT_GET_VDO_INFO_FAIL;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                break;
            }
            p_param = (IPTV_CHANNEL_INFO_T *)param;

            OS_PRINTF("@@@IPTV_VIDEO_INFO_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                if (p_param->is_description == FALSE)
                {
                    al_iptv_info_url_list_t *p_data = ui_iptv_gen_info_url(p_param);
                    evt.id = IPTV_EVT_NEW_INFO_URL_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
                else
                {
                    al_iptv_vdo_info_t *p_data = ui_iptv_gen_vdo_info(p_param);
                    evt.id = IPTV_EVT_NEW_VDO_INFO_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
            }
            else
            {
                evt.id = IPTV_EVT_GET_VDO_INFO_FAIL;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
            }
        }
            break;

        case IPTV_RECOMMEND_INFO_ARRIVAL:
        {
          if((IPTV_RECMD_INFO_T *)param!=NULL)
          {
            IPTV_RECMD_INFO_T *p_param = (IPTV_RECMD_INFO_T *)param;

            OS_PRINTF("@@@IPTV_RECOMMEND_INFO_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_recomand_info_t *p_data = ui_iptv_gen_remend_info(p_param);
                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_RECMND_INFO_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
          }
            evt.id = IPTV_EVT_GET_RECMND_INFO_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_FORMAT_TYPE_ARRIVAL:
        {
            al_iptv_source_format_t *p_data;

            OS_PRINTF("@@@IPTV_FORMAT_TYPE_ARRIVAL param=%d\n", param);
            p_data = ui_iptv_gen_source_format(param);
            if (p_data != NULL)
            {
                evt.id = IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE;
                evt.data1 = (u32)p_data;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                break;
            }
            evt.id = IPTV_EVT_GET_SOURCE_FORMAT_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_PLAY_URL_ARRIVAL:
        {
          if((IPTV_CHANNEL_PLAY_URL_T *)param!= NULL)
          {
            IPTV_CHANNEL_PLAY_URL_T *p_param = (IPTV_CHANNEL_PLAY_URL_T *)param;

            OS_PRINTF("@@@IPTV_PLAY_URL_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_play_url_list_t *p_data = ui_iptv_gen_play_url(p_param);

                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_PLAY_URL_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
          }
            evt.id = IPTV_EVT_GET_PLAY_URL_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        default:
            break;
    }
OS_PRINTF("%s: event %d end\n", __FUNCTION__, event);
    return SUCCESS;
}

void ui_iptv_free_msg_data(u16 msg, u32 para1, u32 para2)
{
    if (msg < MSG_IPTV_EVT_BEGIN || msg >= MSG_IPTV_EVT_END)
    {
        return;
    }

    switch(msg)
    {
        case MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE:
        {
            ui_iptv_free_name_res_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE:
        {
            ui_iptv_free_cat_res_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE:
        case MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE:
        {
            ui_iptv_free_vdo_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE:
        {
            ui_iptv_free_vdo_info(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE:
        {
            ui_iptv_free_info_url(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE:
        {
            ui_iptv_free_remend_info(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE:
        {
            ui_iptv_free_source_format(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE:
        {
            ui_iptv_free_play_url(para1);
        }
            break;
        default:
            break;
    }
}

#if 0
static int iptv_dp_read_flash(void *start, int len)
{
  sys_status_get_iptv_priv_info((u8*)start, len);
  return SUCCESS;
}

static int iptv_dp_write_flash(void *start, int len)
{
  sys_status_set_iptv_priv_info((u8*)start, len);
  sys_status_save();
  return SUCCESS;
}
#endif


inline u16 ui_iptv_get_dp_state(void)
{
    OS_PRINTF("@@@ui_iptv_get_dp_state dp_state=%d\n", g_dp_state);
    return g_dp_state;
}

inline void ui_iptv_set_dp_state(u16 state)
{
    g_dp_state = state;
}

void ui_iptv_dp_init(void)
{
    DO_CMD_TASK_CONFIG_T config;

    OS_PRINTF("@@@ui_iptv_dp_init\n");
    memset(&config, 0, sizeof(DO_CMD_TASK_CONFIG_T));
    vdo_identify_code = 0;
    config.stack_size = 1024*1024;
    config.dp = ZGW_IPTV_DP;//BSW_IPTV_DP;
    config.sub_dp = SUB_DP_ID;
    config.read_flash = NULL;
    config.write_flash = NULL;
    config.mac = mtos_malloc(MAC_CHIPID_LEN);
    config.id = mtos_malloc(MAC_CHIPID_LEN);
    ui_get_stb_mac_addr_chip_id(config.mac, config.id);
#ifndef WIN32
  	config.priority = VOD_DP_PRIORITY;
  	config.http_priority = YT_HTTP_PRIORITY;
    g_pIptvData = IPTV_initDataProvider(&config);
    MT_ASSERT(g_pIptvData != NULL);
    g_pIptvData->register_event_callback(g_pIptvData, iptv_dp_event_callback);
#endif
    ui_iptv_set_dp_state(IPTV_DP_STATE_INITING);
    mtos_free(config.id);
    mtos_free(config.mac);
}

void ui_iptv_dp_deinit(void)
{
    if (g_pIptvData != NULL)
    {
#ifndef WIN32
        IPTV_deinitDataProvider(g_pIptvData);
#endif
        g_pIptvData = NULL;
    }
}

void ui_iptv_register_msg(void)
{
    OS_PRINTF("@@@ui_iptv_register_msg\n");
    fw_register_ap_evtmap(APP_IPTV, ui_iptv_evtmap);
    fw_register_ap_msghost(APP_IPTV, ROOT_ID_IPTV);
}

void ui_iptv_unregister_msg(void)
{
    OS_PRINTF("@@@ui_iptv_unregister_msg\n");
    fw_unregister_ap_evtmap(APP_IPTV);
    fw_unregister_ap_msghost(APP_IPTV, ROOT_ID_IPTV);
}

void ui_iptv_get_res_list(void)
{
    if (g_pIptvData != NULL)
    {
#ifndef WIN32
        IPTV_getCategoryList(g_pIptvData, 0);
#endif
    }
}

void ui_iptv_get_catgry_list(void)
{
    if (g_pIptvData != NULL)
    {
#ifndef WIN32
        IPTV_getCategoryTypeList(g_pIptvData, 0, 0);
#endif
    }
}

void ui_iptv_get_video_list(u32 res_id, u16 *cat_name, u8 *key, u32 page_num)
{
    char *inbuf, *outbuf;
    IPTV_UPPAGE_REQ_T req = {0};
    size_t src_len, dest_len;

    if (g_pIptvData != NULL)
    {
        req.cat_id = (int)res_id;

        if (cat_name)
        {
            inbuf = (char *)cat_name;
            outbuf = (char *)req.types;
            src_len = (uni_strlen(cat_name) + 1) * sizeof(u16);
            dest_len = sizeof(req.types);
            iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }

        if (key)
        {
            strcpy(req.keys, key);
        }

        req.page_index = (int)page_num;
        req.user_data0 = IPTV_QUERY_MODE_CATGRY;
        vdo_identify_code++;
        req.identify = vdo_identify_code;
        req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
        IPTV_updatePage(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_search_set_res_id(u32 res_id)
{
    search_res_id = res_id;
}

void ui_iptv_search_set_keyword(u16 *keyword)
{
    char *inbuf, *outbuf;
    size_t src_len, dest_len;

    inbuf = (char *)keyword;
    outbuf = (char *)search_keyword;
    src_len = (uni_strlen(keyword) + 1) * sizeof(u16);
    dest_len = sizeof(search_keyword);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
}

void ui_iptv_search(u32 page_num)
{
    IPTV_UPPAGE_REQ_T req = {0};

    if (g_pIptvData != NULL)
    {
        req.cat_id = (int)search_res_id;
        strcpy(req.pinyin, search_keyword);
        req.page_index = page_num;

        req.user_data0 = IPTV_QUERY_MODE_PINYIN;
        vdo_identify_code++;
        req.identify = vdo_identify_code;
        req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
        IPTV_updatePage(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_video_info(u32 vdo_id, u8 cat_id)
{
    IPTV_VIDEO_INFO_REQ_T req = {0};

    if (g_pIptvData != NULL)
    {
        req.cat_id = cat_id;
        req.page_index = 1;
        //req.id = vdo_id;
        req.is_description = TRUE;
#ifndef WIN32
        IPTV_getVideoInfo(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_info_url(u32 vdo_id, u8 cat_id, u16 *origin, u32 page_num, u32 page_size)
{
    char *inbuf, *outbuf;
    IPTV_VIDEO_INFO_REQ_T req = {0};
    size_t src_len, dest_len;

    if (g_pIptvData != NULL)
    {
        //req.id = vdo_id;

        inbuf = (char *)origin;
        outbuf = (char *)req.origin;
        src_len = (uni_strlen(origin) + 1) * sizeof(u16);
        dest_len = sizeof(req.origin);
        iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

        req.cat_id = cat_id;
        req.is_description = FALSE;

        req.page_index = page_num;
        req.page_size = page_size;
#ifndef WIN32
        IPTV_getVideoInfo(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_recmnd_info(u32 vdo_id, u32 res_id, u16 *area)
{
    char *inbuf, *outbuf;
    IPTV_RECOMMEND_REQ_T req = {0};
    size_t src_len, dest_len;

    if (g_pIptvData != NULL)
    {
        //req.id = vdo_id;
        req.cat_id = res_id;
        inbuf = (char *)area;
        outbuf = (char *)req.area;
        src_len = (uni_strlen(area) + 1) * sizeof(u16);
        dest_len = sizeof(req.area);
        iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
#ifndef WIN32
        IPTV_getRecommendInfo(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_play_format(u8 *url)
{
    IPTV_VIDEO_URL_REQ_T req;

    if (g_pIptvData != NULL)
    {
        memset((void *)&req, 0, sizeof(IPTV_VIDEO_URL_REQ_T));
        strcpy((char *)req.url, (char *)url);
#ifndef WIN32
        IPTV_getPlayUrlFormat(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_play_url(IPTV_API_FORMAT_T format,u8 *url)
{
    IPTV_VIDEO_URL_REQ_T req;

    if (g_pIptvData != NULL)
    {
        memset((void *)&req, 0, sizeof(IPTV_VIDEO_URL_REQ_T));
        strcpy((char *)req.url, (char *)url);
        req.format = format;
#ifndef WIN32
        IPTV_getPlayUrl(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_set_page_size(u16 page_size)
{
    if (g_pIptvData != NULL)
    {
#ifndef WIN32
        IPTV_setPageSize(g_pIptvData, (int)page_size);
#endif
    }
}

int ui_iptv_vdo_idntfy_cmp(u32 unidntf_code)
{
    return vdo_identify_code - unidntf_code;
}


BEGIN_AP_EVTMAP(ui_iptv_evtmap)
CONVERT_EVENT(IPTV_EVT_INIT_SUCCESS, MSG_IPTV_EVT_INIT_SUCCESS)
CONVERT_EVENT(IPTV_EVT_INIT_FAIL, MSG_IPTV_EVT_INIT_FAIL)
CONVERT_EVENT(IPTV_EVT_DEINIT_SUCCESS, MSG_IPTV_EVT_DEINIT_SUCCESS)
CONVERT_EVENT(IPTV_EVT_NEW_RES_NAME_ARRIVE, MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RES_NAME_FAIL, MSG_IPTV_EVT_GET_RES_NAME_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_RES_CATGRY_ARRIVE, MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RES_CATGRY_FAIL, MSG_IPTV_EVT_GET_RES_CATGRY_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_PAGE_VDO_ARRIVE, MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_PAGE_VDO_FAIL, MSG_IPTV_EVT_GET_PAGE_VDO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SEARCH_VDO_ARRIVE, MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SEARCH_VDO_FAIL, MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_VDO_INFO_ARRIVE, MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_VDO_INFO_FAIL, MSG_IPTV_EVT_GET_VDO_INFO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_INFO_URL_ARRIVE, MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_INFO_URL_FAIL, MSG_IPTV_EVT_GET_INFO_URL_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_PLAY_URL_ARRIVE, MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_PLAY_URL_FAIL, MSG_IPTV_EVT_GET_PLAY_URL_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_RECMND_INFO_ARRIVE, MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RECMND_INFO_FAIL, MSG_IPTV_EVT_GET_RECMND_INFO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE, MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SOURCE_FORMAT_FAIL, MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL)
END_AP_EVTMAP(ui_iptv_evtmap)

#endif
