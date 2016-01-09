/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_config.h"
#include "ui_live_tv_api.h"
#include "ui_live_tv_desktop.h"


#define IS_GROUP_IN_WEBSITE(groupIdx)   ((groupIdx < g_pLivetvData->groupCnt) ? (TRUE) : (FALSE))
#define IS_GROUP_IN_ALL(groupIdx)   ((groupIdx == LIVE_TV_INVALID_INDEX) ? (TRUE) : (FALSE))
#define IS_CHAN_IN_WEBSITE(id)   ((id < g_pLivetvData->cusChanStartId) ? (TRUE) : (FALSE))
#if ENABLE_CUSTOMER_URL
#define IS_GROUP_IN_CUSTOM(groupIdx)   ((groupIdx == g_pLivetvData->groupCnt) ? (TRUE) : (FALSE))
#define IS_CHAN_IN_CUSTOM(id)   ((id >= g_pLivetvData->cusChanStartId \
                                            && id < g_pLivetvData->cusChanStartId + g_pLivetvData->cusUrlCnt) ? (TRUE) : (FALSE))
#endif


enum
{
    LIVE_TV_EVT_CATEGORY_ARRIVAL = ((APP_LIVETV << 16) + 0),
    LIVE_TV_EVT_GET_ZIP,
    LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_EPG,
    LIVE_TV_EVT_GET_LOOKBACK_NAMES,
    LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_LOOKBACK,
    LIVE_TV_INIT_SUCCESS,
    LIVE_TV_INIT_FAILED,
    LIVE_TV_EVT_GET_CHANNELLIST,
};

typedef struct net_url_t
{
    u16 id;
    u16 groupIdx;
    u8 *name;
    u8 **ppUrlList;
    u8 urlCnt;
    u8 urlIdx;
    struct net_url_t *next;
}NetIptvChan, *LNetIptvChan;

typedef struct
{
    void *p_addr;
    u16 id;
} al_livetv_sort_node_t;

typedef enum
{
    ID_MODE,
    GROUP_MODE
} al_livetv_sort_type_t;

typedef BOOL (*livetv_sort_func)(void *prev_node, void *cur_node);

typedef struct
{
    u16 id;
    u8  *name;
    u16 startPos;
    u16 chanCnt;
} al_livetv_group_info_t;

typedef struct
{
    u16 groupCnt;
    al_livetv_group_info_t *groupList;

    u16 totalChan;
    LNetIptvChan pNetIptvChan;
    
    al_livetv_sort_node_t *idTable;//sort by id
    al_livetv_sort_node_t *grpTable;//sort by group
    
//#if ENABLE_CUSTOMER_URL
    u16 cusChanStartId;
    u16 cusUrlCnt;
    custom_url_t *pCusUrlArr;
//#endif
    LIVE_TV_DP_HDL_T *pLivetvHdl;
    u8 *pStackBuf;
}al_livetv_data_t;


static al_livetv_data_t *g_pLivetvData = NULL;

static al_livetv_data_t *ui_iptv_api_get_priv(void)
{
    return g_pLivetvData;
}


BOOL ui_is_init_pLivetvData()
{
  if(g_pLivetvData == NULL)
  {
    return FALSE;
  }
  else
  {
    return TRUE; 
  }
}

/************************************************************************
**************************customer livetv function******************************
************************************************************************/
#if ENABLE_CUSTOMER_URL

//get custmoer iptv program link list, return customer iptv program numbers
static void init_customer_iptv(custom_url_t** ppCusUrlArr, u16 *cusNum)
{
    u16 pg_num = 0;
    u8* p_line = NULL, *p_buf = NULL;
    u8 asc_name[CUS_NAME_LEN];
    u8 asc_url[CUS_URL_LEN];
    void *p_dm_hdl = NULL;
    s32 dataLen = 0;
    custom_url_t *p_arr = * ppCusUrlArr;

    p_dm_hdl = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_hdl != NULL);
    dm_direct_read(p_dm_hdl, CUS_URL_BLOCK_ID, CUSTOM_URL_BLOCK_SIZE, CUSTOM_URL_FLASH_LEN, (u8 *)&dataLen);
    OS_PRINTF("##%s, dataLen=[%d]##\n", __FUNCTION__, dataLen);
    if(dataLen <= 0)
    {
        return ;
    }

    p_buf = (u8*)mtos_malloc(dataLen);
    MT_ASSERT(p_buf != NULL);
    dm_direct_read(p_dm_hdl, CUS_URL_BLOCK_ID, 0, (u32)dataLen, p_buf);
   
    p_buf[--dataLen] = '\0';
    if(dataLen > 0)
    {
        while(pg_num < MAX_CUSTOM_URL && (p_line = strtok(p_line ? NULL : p_buf, "\r\n")))
        {
            memset(asc_name, 0, CUS_NAME_LEN);
            memset(asc_url, 0, CUS_URL_LEN);

            sscanf(p_line, "%[^,],%s", asc_name, asc_url);
            p_arr[pg_num].name = (u8 *)mtos_malloc(strlen(asc_name)+1);
            MT_ASSERT(p_arr[pg_num].name != NULL);
            p_arr[pg_num].ppUrlList = (u8 **)mtos_malloc(1*sizeof(u8*));
            p_arr[pg_num].ppUrlList[0] = (u8 *)mtos_malloc(strlen(asc_url)+1);
            MT_ASSERT(p_arr[pg_num].ppUrlList[0] != NULL);
            strcpy(p_arr[pg_num].name, asc_name);
            strcpy(p_arr[pg_num].ppUrlList[0], asc_url);

            pg_num++;
        }
    }

    if(p_buf)
    {
        mtos_free(p_buf);
        p_buf = NULL;
    }

    *cusNum = pg_num;
}


void save_customer_iptv(u8* p_data, u32 len)
{
    u32 dataLen = len;
    void *p_dm_hdl = class_get_handle_by_id(DM_CLASS_ID);

    MT_ASSERT(p_dm_hdl != NULL);
    if(len > CUSTOM_URL_BLOCK_SIZE - 1)
    {
        dataLen = CUSTOM_URL_BLOCK_SIZE - 1; 
    }

    dm_direct_erase(p_dm_hdl, CUS_URL_BLOCK_ID, 0, CUSTOM_URL_BLK_SIZE - 1);
    MT_ASSERT(DM_SUC == dm_direct_write(p_dm_hdl, CUS_URL_BLOCK_ID, CUSTOM_URL_BLOCK_SIZE, CUSTOM_URL_FLASH_LEN, (u8 *)&dataLen));
    MT_ASSERT(DM_SUC == dm_direct_write(p_dm_hdl, CUS_URL_BLOCK_ID, 0, dataLen, p_data));
}

void ui_livetv_init_custom_chan_list(void)
{
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    al_livetv_release_custom_chan_list();
    p_priv->pCusUrlArr  = (custom_url_t*) mtos_malloc(sizeof(custom_url_t) * MAX_CUSTOM_URL);
    MT_ASSERT(p_priv->pCusUrlArr != NULL);
    memset(p_priv->pCusUrlArr, 0, sizeof(custom_url_t) * MAX_CUSTOM_URL);

    init_customer_iptv(&p_priv->pCusUrlArr, &p_priv->cusUrlCnt);
}

void al_livetv_release_custom_chan_list(void)
{
    u16 i;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    if (p_priv->pCusUrlArr != NULL)
    {
        for (i = 0; i < p_priv->cusUrlCnt; i++)
        {
            if (p_priv->pCusUrlArr[i].name != NULL)
            {
                mtos_free(p_priv->pCusUrlArr[i].name);
                p_priv->pCusUrlArr[i].name = NULL;
            }

            if (p_priv->pCusUrlArr[i].ppUrlList != NULL)
            {
                if (p_priv->pCusUrlArr[i].ppUrlList[0] != NULL)
                {
                    mtos_free(p_priv->pCusUrlArr[i].ppUrlList[0]);
                    p_priv->pCusUrlArr[i].ppUrlList[0] = NULL;
                }
                mtos_free(p_priv->pCusUrlArr[i].ppUrlList);
                p_priv->pCusUrlArr[i].ppUrlList = NULL;
            }
        }

        mtos_free(p_priv->pCusUrlArr);
        p_priv->pCusUrlArr = NULL;
    }
}
#endif


/************************************************************************
**************************livetv sort function******************************
************************************************************************/
static BOOL _id_cmp(void *cur_node, void *next_node)
{
    return ((NetIptvChan *)cur_node)->id < ((NetIptvChan *)next_node)->id;
}

static BOOL _grp_cmp(void *cur_node, void *next_node)
{
    return ((NetIptvChan *)cur_node)->groupIdx < ((NetIptvChan *)next_node)->groupIdx;
}

static void _livetv_sort(al_livetv_sort_node_t *p_sort, int cnt, al_livetv_sort_type_t sort_type)
{
    int i, j;
    livetv_sort_func sort_func = NULL;
    al_livetv_sort_node_t tmp;

    switch(sort_type)
    {
        case ID_MODE:
            sort_func = _id_cmp;
            break;
        case GROUP_MODE:
            sort_func = _grp_cmp;
            break;
        default:
            MT_ASSERT(0);
            break;
    }

    for (i = 0; i < cnt - 1; i++)
    {
        for (j = i + 1; j < cnt; j++)
        {
            if (sort_func(p_sort[i].p_addr, p_sort[j].p_addr) == FALSE)
            {
                tmp = p_sort[i];
                p_sort[i] = p_sort[j];
                p_sort[j] = tmp;
            }
        }
    }
}

void print_iptv_urls(LNetIptvChan pHead)
{
    LNetIptvChan p = pHead->next;

    while(p != NULL)
    {
        OS_PRINTF("##grpid=[%d],id=[%d],name=[%s],url=[%s]##\n",p->groupIdx,p->id,p->name,p->ppUrlList[0]);
        p = p->next;
    }   
}

static void al_livetv_sort_chan(void)
{
    u16 i = 0;
    u16 sum = 0;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();
    LNetIptvChan p_chanData = p_priv->pNetIptvChan->next;

    p_priv->idTable = (al_livetv_sort_node_t *)mtos_malloc(p_priv->totalChan * sizeof(al_livetv_sort_node_t));
    p_priv->grpTable = (al_livetv_sort_node_t *)mtos_malloc(p_priv->totalChan * sizeof(al_livetv_sort_node_t));


    while(p_chanData != NULL && i < p_priv->totalChan)
    {
     //   OS_PRINTF("##grpid=[%d],id=[%d],name=[%s],url=[%s]##\n",p_chanData->groupIdx,p_chanData->id,p_chanData->name,p_chanData->ppUrlList[0]);
        p_priv->idTable[i].p_addr = p_priv->grpTable[i].p_addr = p_chanData;
        p_priv->idTable[i].id = p_chanData->id;
        p_chanData = p_chanData->next;
        i++;
    }   

    //channel id sort
    _livetv_sort(p_priv->idTable, p_priv->totalChan, ID_MODE);

    //group sort
    _livetv_sort(p_priv->grpTable, p_priv->totalChan, GROUP_MODE);

    for (i = 0, sum = 0; i < p_priv->groupCnt; i++)
    {
        //OS_PRINTF("###i=[%d], chancnt=[%d]###\n", i,p_priv->groupList[i].chanCnt);
        if (p_priv->groupList[i].chanCnt > 1)
        {
            _livetv_sort(p_priv->grpTable + sum, p_priv->groupList[i].chanCnt, ID_MODE);
        }
        p_priv->groupList[i].startPos = sum;
        sum += p_priv->groupList[i].chanCnt;
    }

    if (p_priv->totalChan > 0)
    {
        p_chanData = (LNetIptvChan)p_priv->idTable[p_priv->totalChan - 1].p_addr;
        if (p_chanData != NULL)
        {
            p_priv->cusChanStartId = p_chanData->id + 1;
        }
    }

    //mtos_printk("##%s, id=[%d],grp name=[%s]##\n",__FUNCTION__,pNew->id,pNew->ppUrlList[0]);
}


void sort_link_by_id(LNetIptvChan pHead)
{
    LNetIptvChan temp1, temp2, q, p;

    for(q = pHead;q->next != NULL;q = q->next)
    {
        for(p = q->next;p->next != NULL;p = p->next)
        {
            if(p->next->id < q->next->id)
            {
                if(q->next == p)
                {
                    temp1 = p->next;
                    p->next = p->next->next;
                    temp1->next = q->next;
                    q->next = temp1;
                    p = temp1;
                }
                else
                {
                    temp1 = p->next;
                    temp2 = q->next;
                    p->next = p->next->next;
                    q->next = q->next->next;
                    temp1->next = q->next;
                    q->next = temp1;
                    temp2->next = p->next;
                    p->next = temp2;
                }
            }
        }
    }

}

/************************************************************************
****************************livetv get network data***************************
************************************************************************/
static inline u16 _al_livetv_get_group_idx(u16 group_id)
{
    u16 i;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    for (i = 0; i < p_priv->groupCnt; i++)
    {
        if (p_priv->groupList[i].id == group_id)
        {
            return i;
        }
    }

    return LIVE_TV_INVALID_INDEX;
}


static void al_livetv_release_group_list(void)
{
    u16 i;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    if (p_priv->groupList != NULL)
    {
        for (i = 0; i < p_priv->groupCnt; i++)
        {
            if (p_priv->groupList[i].name != NULL)
            {
                mtos_free(p_priv->groupList[i].name);
                p_priv->groupList[i].name = NULL;
            }
        }

        mtos_free(p_priv->groupList);
        p_priv->groupList = NULL;
    }
}


static void al_livetv_release_chan_list(void)
{
    u16 i;
    LNetIptvChan pChan = NULL;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();
    
    if (p_priv && p_priv->pNetIptvChan)
    {
        pChan = p_priv->pNetIptvChan->next;
    }

    while(pChan != NULL)
    {
       // OS_PRINTF("##grpid=[%d],id=[%d],name=[%s],url=[%s]##\n",pChan->groupIdx,pChan->id,pChan->name,pChan->ppUrlList[0]);
        if (pChan->name)
        {
            mtos_free(pChan->name);
            pChan->name = NULL;
        }
        
        if(pChan->ppUrlList)
        {
            for(i = 0;i < pChan->urlCnt;i++)
            {
                mtos_free(pChan->ppUrlList[i]);
                pChan->ppUrlList[i] = NULL;
            }
            mtos_free(pChan->ppUrlList);
            pChan->ppUrlList = NULL;
        }

        mtos_free(pChan);
        pChan = NULL;
        pChan = pChan->next;
    }   

}
static void al_livetv_init_group_list(LIVE_TV_CATEGORY_LIST_T *p_param)
{
    LIVE_TV_CATEGORY_T *p_item;
    char *outbuf;
    size_t dest_len;
    u16 i;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    if ((p_param->array != NULL) && (p_param->num > 0))
    {
        p_priv->groupCnt = (u16)p_param->num;
        p_priv->groupList = (al_livetv_group_info_t *)mtos_malloc(p_param->num * sizeof(al_livetv_group_info_t));
        MT_ASSERT(p_priv->groupList != NULL);
        memset(p_priv->groupList, 0, p_param->num * sizeof(al_livetv_group_info_t));

        for (i = 0; i < p_param->num; i++)
        {
            p_item = p_param->array + i;

            p_priv->groupList[i].id = p_item->id;

            if (p_item->name != NULL)
            {
                dest_len = strlen(p_item->name) + 1;
                outbuf = (char *)mtos_malloc(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)p_item->name);
                p_priv->groupList[i].name = (u8 *)outbuf;
          //      mtos_printk("##%s, id=[%d],grp name=[%s]##\n",__FUNCTION__,p_priv->groupList[i].id,p_priv->groupList[i].name);
            }
        }
    }
}


static void al_livetv_destory_livetv_data(void)
{
    if (g_pLivetvData != NULL)
    {
        al_livetv_release_group_list();
        al_livetv_release_chan_list();
        if (g_pLivetvData->idTable != NULL)
        {
            mtos_free(g_pLivetvData->idTable);
        }
        if (g_pLivetvData->grpTable != NULL)
        {
            mtos_free(g_pLivetvData->grpTable);
        }
#if ENABLE_CUSTOMER_URL
        al_livetv_release_custom_chan_list();
#endif
        mtos_free(g_pLivetvData);
        g_pLivetvData = NULL;
    }
}


 void al_livetv_create_livetv_data(void)
{
    if (g_pLivetvData != NULL)
    {
        al_livetv_destory_livetv_data();
    }

    g_pLivetvData = (al_livetv_data_t *)mtos_malloc(sizeof(al_livetv_data_t));
    MT_ASSERT(g_pLivetvData != NULL);
    memset(g_pLivetvData, 0, sizeof(al_livetv_data_t));
}


RET_CODE ui_live_tv_api_all_chan_arrived(LIVE_TV_CHANNEL_LIST_T * param)
{
    u16 i = 0;
    u8 j = 0;
    u16 name_len = 0;
    LNetIptvChan pHead, pTail, pNew;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();
    LIVE_TV_CHANNEL_LIST_T *p_chl = (LIVE_TV_CHANNEL_LIST_T *)param;
    MT_ASSERT(NULL != p_chl);

    OS_PRINTF("##%s, totalcount == %d#\n", __FUNCTION__, p_chl->totalcount);
    p_priv->totalChan = p_chl->totalcount;
    pHead = (LNetIptvChan)mtos_malloc(sizeof(LNetIptvChan));
    MT_ASSERT(pHead != NULL);
    pTail = pHead;
    pTail->next = NULL;

    for(i = 0; i < p_chl->totalcount; i++)
    {
        pNew = (LNetIptvChan)mtos_malloc(sizeof(LNetIptvChan));
        MT_ASSERT(pNew != NULL);
        memset(pNew, 0, sizeof(LNetIptvChan));

        //id
        pNew->id = p_chl->array[i].id;
        
        //groupIdx
        pNew->groupIdx = p_chl->array[i].category_id;
        pNew->groupIdx  = _al_livetv_get_group_idx(pNew->groupIdx);
        if (pNew->groupIdx != LIVE_TV_INVALID_INDEX)
        {
            p_priv->groupList[pNew->groupIdx].chanCnt++;
        }

        //name
        if(p_chl->array[i].title)
        {
            name_len = strlen(p_chl->array[i].title)+1;
            pNew->name = (u8 *)mtos_malloc(name_len);
            MT_ASSERT(pNew->name != NULL);
            memset(pNew->name, 0, name_len);
            strcpy(pNew->name, p_chl->array[i].title);
        }
        
        //urlCnt
        pNew->urlCnt = p_chl->array[i].num;

        //urlIdx
        pNew->urlIdx = 0;

        //ppUrlList
        pNew->ppUrlList = (u8 **)mtos_malloc(pNew->urlCnt * sizeof(u8 *));
        MT_ASSERT(pNew->ppUrlList != NULL);
        for (j = 0; j < pNew->urlCnt && p_chl->array[i].playurl[j]; j++)
        {
            name_len = strlen(p_chl->array[i].playurl[j])+1;
            pNew->ppUrlList[j] = (u8 *)mtos_malloc(name_len);
            MT_ASSERT(pNew->ppUrlList[j] != NULL);
            memset(pNew->ppUrlList[j], 0, name_len);
            strcpy(pNew->ppUrlList[j], p_chl->array[i].playurl[j]);
        }
       // mtos_printk("##%s, id=[%d],grp name=[%s]##\n",__FUNCTION__,pNew->id,pNew->ppUrlList[0]);

        pNew->next = NULL;
        pTail->next = pNew;
        pTail = pNew;

    }

   // sort_link_by_id(pHead);
    p_priv->pNetIptvChan = pHead;
    al_livetv_sort_chan();

    return SUCCESS;
}

RET_CODE ui_live_tv_api_all_chan_zip_load()
{
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    OS_PRINTF("##%s, start##\n", __FUNCTION__);
#ifndef WIN32
    LiveTV_getLzmaChannelData(p_priv->pLivetvHdl);
#endif
    OS_PRINTF("##%s, end##\n", __FUNCTION__);
    return SUCCESS;
}


RET_CODE ui_live_tv_api_group_arrived(u32 param)
{
    LIVE_TV_CATEGORY_LIST_T *p_group  = (LIVE_TV_CATEGORY_LIST_T *)param;

    if(NULL == p_group)
    {
        return ERR_FAILURE;
    }

    al_livetv_init_group_list(p_group);

    return SUCCESS;
}


/************************************************************************
**************************livetv UI function******************************
************************************************************************/
BOOL ui_livetv_storage_get_group_info_by_group_index(ui_livetv_group_info_t *p_groupInfo, u16 iGroup)
{
    al_livetv_group_info_t *p_groupData = NULL;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    MT_ASSERT(p_groupInfo != NULL);
    memset(p_groupInfo, 0, sizeof(ui_livetv_group_info_t));

    if (IS_GROUP_IN_WEBSITE(iGroup))
    {
        p_groupData = &p_priv->groupList[iGroup];
        if (p_groupData->name != NULL)
        {
            convert_utf8_chinese_asc2unistr(p_groupData->name, p_groupInfo->name, MAX_LIVETV_NAME_LEN);
        }

        return TRUE;
    }

    #if ENABLE_CUSTOMER_URL
    else if (IS_GROUP_IN_CUSTOM(iGroup))
    {
        uni_strncpy(p_groupInfo->name, (u16 *)gui_get_string_addr(IDS_CUSTOMER_ID), MAX_LIVETV_NAME_LEN);
        return TRUE;
    }
#endif

    return FALSE;
}


u16 ui_livetv_storage_get_group_cnt(void)
{
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    return p_priv->groupCnt
    #if ENABLE_CUSTOMER_URL
        + 1
    #endif
        ;
}

s32 ui_livetv_storage_get_chan_info_by_id(ui_livetv_chan_info_t *p_chanInfo, u16 id)
{
    LNetIptvChan p_chanData = NULL;
    u16 c_start = 0;
    u16 c_end = 0;
    u16 cur = 0;
    #if ENABLE_CUSTOMER_URL
    custom_url_t *p_custChanData = NULL;
    #endif
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    MT_ASSERT(p_chanInfo != NULL);
    memset(p_chanInfo, 0, sizeof(ui_livetv_chan_info_t));

    if (IS_CHAN_IN_WEBSITE(id))
    {
        c_start = 0;
        c_end = p_priv->totalChan;

        while (c_start <= c_end) //ÕÛ°ë²éÕÒ
        {
            cur = (c_end  + c_start) >> 1;
            if (id > p_priv->idTable[cur].id)
            {
                c_start = cur + 1;
            }
            else if (id < p_priv->idTable[cur].id)
            {
                c_end = cur - 1;
            }
            else
            {
                p_chanData = (LNetIptvChan)p_priv->idTable[cur].p_addr;
                break;
            }
        }

        if (p_chanData != NULL)
        {
            if (p_chanData->name != NULL)
            {
            convert_utf8_chinese_asc2unistr(p_chanData->name, p_chanInfo->name, MAX_LIVETV_NAME_LEN);
            }
            p_chanInfo->ppUrlList = p_chanData->ppUrlList;
            p_chanInfo->id = p_chanData->id;
            p_chanInfo->urlCnt = p_chanData->urlCnt;
            p_chanInfo->urlIdx = p_chanData->urlIdx;

            return cur;
        }
    }

    #if ENABLE_CUSTOMER_URL
    else if (IS_CHAN_IN_CUSTOM(id))
    {
        p_custChanData = &p_priv->pCusUrlArr[id - p_priv->cusChanStartId];
        if (p_custChanData != NULL)
        {
            if (p_custChanData->name != NULL)
            {
            convert_gb2312_chinese_asc2unistr(p_custChanData->name, p_chanInfo->name, MAX_LIVETV_NAME_LEN);
            }
            p_chanInfo->ppUrlList = p_custChanData->ppUrlList;
            p_chanInfo->id = id;
            p_chanInfo->urlCnt = 1;
            p_chanInfo->urlIdx = 0;

            return p_priv->totalChan + (id - p_priv->cusChanStartId);
        }
    }
#endif

    return -1;
}


BOOL ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(ui_livetv_chan_info_t *p_chanInfo, u16 iGroup, u16 pos)
{
    al_livetv_group_info_t *p_groupData = NULL;
    LNetIptvChan p_chanData = NULL;
#if ENABLE_CUSTOMER_URL
    custom_url_t *p_custChanData = NULL;
#endif
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    MT_ASSERT(p_chanInfo != NULL);
    memset(p_chanInfo, 0, sizeof(ui_livetv_chan_info_t));

    if (IS_GROUP_IN_ALL(iGroup))
    {
        if (pos < p_priv->totalChan)
        {
            p_chanData = (LNetIptvChan)p_priv->idTable[pos].p_addr;
        }
        #if ENABLE_CUSTOMER_URL
        else
        {
            pos -= p_priv->totalChan;
            if (pos < p_priv->cusUrlCnt)
            {
                p_custChanData = &p_priv->pCusUrlArr[pos];
            }
        }
        #endif
    }
    else if (IS_GROUP_IN_WEBSITE(iGroup))
    {
        p_groupData = &p_priv->groupList[iGroup];
        if (pos < p_groupData->chanCnt)
        {
            p_chanData = (LNetIptvChan)p_priv->grpTable[p_groupData->startPos + pos].p_addr;
        }
    }
    #if ENABLE_CUSTOMER_URL
    else if (IS_GROUP_IN_CUSTOM(iGroup))
    {
        if (pos < p_priv->cusUrlCnt)
        {
            p_custChanData = &p_priv->pCusUrlArr[pos];
        }
    }
#endif

    if (p_chanData != NULL)
    {
        if (p_chanData->name != NULL)
        {
        convert_utf8_chinese_asc2unistr(p_chanData->name, p_chanInfo->name, MAX_LIVETV_NAME_LEN);
        }
        p_chanInfo->ppUrlList = p_chanData->ppUrlList;
        p_chanInfo->id = p_chanData->id;
        p_chanInfo->urlCnt = p_chanData->urlCnt;
        p_chanInfo->urlIdx = p_chanData->urlIdx;

        return TRUE;
    }

#if ENABLE_CUSTOMER_URL
    if (p_custChanData != NULL)
    {
        if (p_custChanData->name != NULL)
        {
        convert_gb2312_chinese_asc2unistr(p_custChanData->name, p_chanInfo->name, MAX_LIVETV_NAME_LEN);
        }
        p_chanInfo->ppUrlList = p_custChanData->ppUrlList;
        p_chanInfo->id = p_priv->cusChanStartId + pos;
        p_chanInfo->urlCnt = 1;
        p_chanInfo->urlIdx = 0;

        return TRUE;
    }
#endif

    return FALSE;
}


u16 ui_livetv_storage_get_chan_cnt_by_group_index(u16 iGroup)
{
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    if (IS_GROUP_IN_ALL(iGroup))
    {
        return p_priv->totalChan
        #if ENABLE_CUSTOMER_URL
            + p_priv->cusUrlCnt
        #endif
            ;
    }
    else if (IS_GROUP_IN_WEBSITE(iGroup))
    {
        return p_priv->groupList[iGroup].chanCnt;
    }
#if ENABLE_CUSTOMER_URL
    else if (IS_GROUP_IN_CUSTOM(iGroup))
    {
        return p_priv->cusUrlCnt;
    }
#endif

    return 0;
}

s32 ui_livetv_storage_get_chan_pos_by_id(u16 id)
{
    u16 c_start = 0;
    u16 c_end = 0;
    u16 cur = 0;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    if (IS_CHAN_IN_WEBSITE(id))
    {
        c_start = 0;
        c_end = p_priv->totalChan;

        while (c_start <= c_end)
        {
            cur = (c_end  + c_start) >> 1;
            if (id > p_priv->idTable[cur].id)
            {
                c_start = cur + 1;
            }
            else if (id < p_priv->idTable[cur].id)
            {
                c_end = cur - 1;
            }
            else
            {
                return cur;
            }
        }
    }
#if ENABLE_CUSTOMER_URL
    else if (IS_CHAN_IN_CUSTOM(id))
    {
        return p_priv->totalChan + (id - p_priv->cusChanStartId);
    }
#endif

    return -1;
}

BOOL ui_livetv_storage_set_pref_chan_url(u16 id, u8 urlIdx)
{
    s32 pos;
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    if (IS_CHAN_IN_WEBSITE(id))
    {
        pos = ui_livetv_storage_get_chan_pos_by_id(id);
        if (pos >= 0)
        {
            ((LNetIptvChan)p_priv->idTable[pos].p_addr)->urlIdx = urlIdx;
            return TRUE;
        }
    }

    return FALSE;
}

/************************************************************************
**************************livetv callback function******************************
************************************************************************/
RET_CODE ui_live_tv_proc(u32 event, u32 para1, u32 para2);


static RET_CODE  live_tv_event_callback(TVLIVE_EVENT_TYPE event, u32 param) 
{
  event_t evt = {0};
  
  OS_PRINTF("%s start\n", __FUNCTION__);
  
  switch(event) 
  {
    case PB_CHANNELS_NAME_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_LOOKBACK_NAMES;
      evt.data1 = (u32)param;
      break;
    case PB_CHANNEL_INFO_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_LOOKBACK;
      evt.data1 = (u32)param;
      break;
    case EPG_CHANNEL_INFO_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_EPG;
      evt.data1 = (u32)param;
      break;
      
    case CATEGORY_ARRIVAL:
      evt.id = LIVE_TV_EVT_CATEGORY_ARRIVAL;
      evt.data1 = (u32)param;
      break;
      
    case PLAY_CHANNEL_ZIP_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_ZIP;
      evt.data1 = (u32)param;
      break;
      
    case DATA_PROVIDER_INIT_SUCCESS:
      evt.id = LIVE_TV_INIT_SUCCESS;
      break;
      
    case DATA_PROVIDER_INIT_FAIL:
      evt.id = LIVE_TV_INIT_FAILED;
      break;

    case ALL_PLAY_CHANNEL_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_CHANNELLIST;
      evt.data1 = (u32)param;
      break;      
      
    default:
      break;
  }
  
  ap_frm_send_evt_to_ui(APP_IPTV, &evt);
  OS_PRINTF("%s end\n", __FUNCTION__);
  return SUCCESS;
}

/************************************************************************
**************************livetv init function*********************************
************************************************************************/
LIVE_TV_DP_HDL_T  *get_live_tv_handle()
{
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

    return p_priv->pLivetvHdl;
}

void ui_live_tv_get_categorylist()
{
    al_livetv_data_t *p_priv = ui_iptv_api_get_priv();

#ifndef WIN32
    LiveTV_getCategoryList(p_priv->pLivetvHdl);
#endif
}

void Iptv_Register_Evtmap()
{
    fw_register_ap_evtmap(APP_IPTV, ui_live_tv_evtmap);
    fw_register_ap_evtproc(APP_IPTV, ui_live_tv_proc);
    fw_register_ap_msghost(APP_IPTV, ROOT_ID_LIVE_TV);  
    fw_register_ap_msghost(APP_IPTV, ROOT_ID_BACKGROUND);  
}


void al_livetv_dp_deinit(void)
{
    if(g_pLivetvData)
    {
        if (g_pLivetvData->pLivetvHdl != NULL)
        {
#ifndef WIN32
            LiveTV_deinitDataProvider((void *)g_pLivetvData->pLivetvHdl);
#endif
            g_pLivetvData->pLivetvHdl = NULL;
        }

        if (g_pLivetvData->pStackBuf != NULL)
        {
            mtos_free(g_pLivetvData->pStackBuf);
            g_pLivetvData->pStackBuf = NULL;
        }

        al_livetv_destory_livetv_data();
    }
}


void al_livetv_dp_init()
{
    DO_CMD_TASK_CONFIG_T task_param = {0,};
    al_livetv_data_t *p_priv = NULL;

    memset(&task_param, 0x00, sizeof(DO_CMD_TASK_CONFIG_T));

    OS_PRINTF("##%s, start##\n", __FUNCTION__);
    al_livetv_dp_deinit();
    al_livetv_create_livetv_data();

#ifndef WIN32
    task_param.priority = LIVE_TV_PRIORITY;
    task_param.http_priority = LIVE_TV_HTTP_PRIORITY;
    task_param.misc_priotiry = MISC_PRIORITY;
    task_param.stack_size = LIVE_TV_TASK_STACK_SIZE;

    p_priv = ui_iptv_api_get_priv();
    p_priv->pStackBuf = (char *)mtos_malloc(task_param.stack_size);
    MT_ASSERT(p_priv->pStackBuf != NULL);
    memset(p_priv->pStackBuf, 0, task_param.stack_size);
    task_param.p_mem_start = p_priv->pStackBuf;
    task_param.dp = BSW_IPTV_DP;
    task_param.sub_dp = SUB_DP_ID;
    task_param.mac = mtos_malloc(MAC_CHIPID_LEN);
    task_param.id = mtos_malloc(MAC_CHIPID_LEN);
    ui_get_stb_mac_addr_chip_id(task_param.mac, task_param.id);
    p_priv->pLivetvHdl =  LiveTV_initDataProvider((void *)&task_param);
    MT_ASSERT(p_priv->pLivetvHdl != NULL);
    p_priv->pLivetvHdl->register_event_callback(p_priv->pLivetvHdl, live_tv_event_callback);

    mtos_free(task_param.id);
    mtos_free(task_param.mac);
  
  OS_PRINTF("##%s, end##\n", __FUNCTION__);
#endif  

}


BEGIN_AP_EVTMAP(ui_live_tv_evtmap)
  CONVERT_EVENT(LIVE_TV_EVT_CATEGORY_ARRIVAL, MSG_GET_LIVE_TV_CATEGORY_ARRIVAL)
  CONVERT_EVENT(LIVE_TV_EVT_GET_ZIP, MSG_GET_LIVE_TV_GET_ZIP)
  CONVERT_EVENT(LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_EPG, MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_EPG)
  CONVERT_EVENT(LIVE_TV_EVT_GET_LOOKBACK_NAMES, MSG_GET_LIVE_TV_LOOKBACK_NAMES)
  CONVERT_EVENT(LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_LOOKBACK, MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_LOOKBACK)
  CONVERT_EVENT(LIVE_TV_INIT_SUCCESS, MSG_INIT_LIVETV_SUCCESS)
  CONVERT_EVENT(LIVE_TV_INIT_FAILED, MSG_INIT_LIVETV_FAILED)
  CONVERT_EVENT(LIVE_TV_EVT_GET_CHANNELLIST, MSG_GET_LIVE_TV_CHANNELLIST)
END_AP_EVTMAP(ui_live_tv_evtmap)

#if ENABLE_CUSTOMER_URL
BEGIN_AP_EVTPROC(ui_live_tv_proc)
ON_EVENTPROC(LIVE_TV_INIT_SUCCESS, Iptv_On_State_Process_Event)
ON_EVENTPROC(LIVE_TV_INIT_FAILED, Iptv_On_State_Process_Event)
ON_EVENTPROC(LIVE_TV_EVT_CATEGORY_ARRIVAL, Iptv_On_State_Process_Event)
ON_EVENTPROC(LIVE_TV_EVT_GET_ZIP, Iptv_On_State_Process_Event)
ON_EVENTPROC(LIVE_TV_EVT_GET_CHANNELLIST, Iptv_On_State_Process_Event)
END_AP_EVTPROC(ui_live_tv_proc)
#endif
