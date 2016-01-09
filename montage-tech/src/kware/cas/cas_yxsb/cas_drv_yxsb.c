/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"

#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_event.h"
#include "charsto.h"
#include "nim.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "drv_svc.h"
#include "../cas_adapter.h"
#include "dmx.h"

#include "CDCASS.h"
#include "CDCAS_include.h"

#define YXSB_DRV_DEBUG_EN


#ifdef YXSB_DRV_DEBUG_EN
#define YXSB_DRV_DEBUG OS_PRINTF
#else
#define YXSB_DRV_DEBUG(...) do{}while(0)
#endif

typedef enum _yxsb_task_status
{
    YXSB_TASK_STATUS_FREE,
    YXSB_TASK_STATUS_BUSY
}yxsb_task_status_e;

typedef struct _s_ds_task_struct
{
    u32 task_prio;
    yxsb_task_status_e task_status;
}s_yxsb_task_struct_t;

static s_yxsb_task_struct_t yxsb_task_record[YXSB_CAS_TASK_NUM] =
{
    {0, YXSB_TASK_STATUS_FREE},
    {0, YXSB_TASK_STATUS_FREE},
    {0, YXSB_TASK_STATUS_FREE},
    {0, YXSB_TASK_STATUS_FREE},
    {0, YXSB_TASK_STATUS_FREE},
    {0, YXSB_TASK_STATUS_FREE},
};

typedef enum _e_yxsb_filter_status
{
    YXSB_FILTER_STATUS_FREE,
    YXSB_FILTER_STATUS_BUSY
}e_yxsb_filter_status_t;

typedef struct _s_yxsb_filter_struct
{
    e_yxsb_filter_status_t filter_status;
    u8 once_filter_flag;
    u8 req_id;
    u16 pid;
    //u8 filter_data[YXSB_FILTER_DEPTH_SIZE];
    //u8 filter_mask[YXSB_FILTER_DEPTH_SIZE];
    u16 req_handle;
    u32 start_ms;
    u32 req_timeout;
    u8 *p_buf;
}s_yxsb_filter_struct_t;

typedef struct _s_yxsb_data_got
{
    u8 req_id;
    u8 timeout;
    u16 pid;
    u8 *p_data;
    u16 length;
}s_yxsb_data_got_t;

static s_yxsb_filter_struct_t yxsb_filter[YXSB_FILTER_MAX_NUM] = {{0,},};
os_sem_t yxsb_filter_lock = 0;
static u8 yxsb_filter_buf[YXSB_FILTER_MAX_NUM][YXSB_FILTER_BUFFER_SIZE] = {{0,},};
static s_yxsb_data_got_t yxsb_data[YXSB_FILTER_MAX_NUM] = {{0,},};
static u8 yxsb_data_buf[YXSB_FILTER_MAX_NUM][YXSB_DATA_BUFFER_SIZE] = {{0,},};

static u16 v_channel = 0xffff;
static u16 a_channel = 0xffff;
//static u16 old_v_channel = 0xffff;
//static u16 old_a_channel = 0xffff;

/*globle variable for adt IOCTL*/
ipp_buy_info_t IppvBuyInfo = {0};
msg_info_t OsdMsg = {0, {0,},};
finger_msg_t FingerInfo = {{0,},};
cas_force_channel_t ForceChannelInfo = {0,};

/*globle semaphore for the about variables*/
os_sem_t FingerInfo_lock = 0;
os_sem_t OsdMsg_lock = 0;
os_sem_t IppvBuyInfo_lock = 0;
os_sem_t ForceChannelInfo_lock = 0;

static cas_adapter_priv_t *p_g_cas_priv = NULL;
extern scard_device_t *p_yxsb_scard_dev;

static void data_dump(u8 *p_addr, u32 size)
{
   u32 i = 0;

   for(i = 0; i < size; i++)
   {
      YXSB_DRV_DEBUG("%02x ", p_addr[i]);
      if ((0 == ((i + 1) % 20)) && (i != 0))
     {
         YXSB_DRV_DEBUG("\n");
     }
   }
   YXSB_DRV_DEBUG("\n");
}


void YXSB_STB_Drv_CaReqLock(os_sem_t *pSemaphore)
{
    BOOL ret = FALSE;
    ret = mtos_sem_take((os_sem_t *)pSemaphore, 0);
    if(ret == FALSE)
      YXSB_DRV_DEBUG("[YXSB]SEM TAKE ERROR\n");
}

void YXSB_STB_Drv_CaReqUnLock(os_sem_t *pSemaphore)
{
    BOOL ret = FALSE;
    ret = mtos_sem_give((os_sem_t *)pSemaphore);
    if(ret == FALSE)
      YXSB_DRV_DEBUG("[YXSB]SEM GIVE ERROR\n");
}

static void CDCAS_SendEvent(u32 event, u32 pucMessage)
{
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    s_cas_yxsb_priv_t *p_priv = (s_cas_yxsb_priv_t *)p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    if (event == 0)
    {
        return;
    }
     
    cas_send_event(p_priv->slot, CAS_ID_TF, event, (u32)pucMessage);
}


CDCA_BOOL CDSTBCA_RegisterTask( const char* szName,
                                       CDCA_U8     byPriority,
                                       void*       pTaskFun,
                                       void*       pParam,
                                       CDCA_U16    wStackSize  )
{
    u8 ret = 0;
    u8 uIndex = 0;
    u32 priority = 0;
    u32 *p_stack = NULL;
    s_cas_yxsb_priv_t *p_cas_yxsb_priv = NULL;
    
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d,create task ! Name[%s], Priority[%d], StackSize[%d]\n", 
                                            __FUNCTION__, __LINE__, szName, byPriority, wStackSize);

    p_cas_yxsb_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;
    
    MT_ASSERT(NULL != szName);
    MT_ASSERT(NULL != pTaskFun);

    for(uIndex = 0; uIndex < YXSB_CAS_TASK_NUM; uIndex++)
    {
        if(YXSB_TASK_STATUS_FREE == yxsb_task_record[uIndex].task_status)
        {
            yxsb_task_record[uIndex].task_status = YXSB_TASK_STATUS_BUSY;
            break;
        }
    }

    if(uIndex >= YXSB_CAS_TASK_NUM)
    {
        YXSB_DRV_DEBUG("[YXSB]CDSTBCA_RegisterTask too many task or invalid task priority!\n");
        return CDCA_FALSE;
    }

    if(p_cas_yxsb_priv->task_prio)
        priority = p_cas_yxsb_priv->task_prio;
    else
        byPriority = YXSB_TASK_PRIORITY_0;

    yxsb_task_record[uIndex].task_prio= priority + uIndex;

    p_stack = (u32 *)mtos_malloc(wStackSize);
    MT_ASSERT(p_stack!=NULL);

    ret = mtos_task_create((u8 *)szName,
                                            (void *)pTaskFun,
                                            (void *)pParam,
                                            yxsb_task_record[uIndex].task_prio,
                                            p_stack,
                                            wStackSize);
    if(!ret)
    {
        OS_PRINTF("[YXSB]%s:LINE:%d create task error = 0x%08x!\n",__FUNCTION__, __LINE__, ret);
        yxsb_task_record[uIndex].task_status = YXSB_TASK_STATUS_FREE;
        return CDCA_FALSE;
    }
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d,create task success! Name[%s], Priority[%d]\n", __FUNCTION__, __LINE__, szName, yxsb_task_record[uIndex].task_prio);

    return CDCA_TRUE;
}

u8 task[5] = {0, };
extern u8 OSPrioCur;

void CDSTBCA_Sleep(CDCA_U16 wMilliSeconds)
{
    //u32 start_ms = 0;
    //start_ms = mtos_ticks_get();
    //u32 end_ms = 0;
    
    //u32 wMilliSeconds_Tmp = wMilliSeconds;
    //wMilliSeconds_Tmp *= 9;
    //wMilliSeconds_Tmp /= 10;
    //wMilliSeconds = (u16)wMilliSeconds_Tmp;
    u8 tmp = OSPrioCur -YXSB_TASK_PRIORITY_0;
    if(wMilliSeconds != 100)
        YXSB_DRV_DEBUG("[YXSB ]CDSTBCA_Sleep:%dms\n", wMilliSeconds);
    
    if(task[tmp])
    {
        task[tmp] = 0;
        mtos_task_sleep(95);
    }
    else
    {
        task[tmp] = 1;
        mtos_task_sleep(90);
    }
    //end_ms = mtos_ticks_get();
    //if(wMilliSeconds > 100)
    //    YXSB_DRV_DEBUG("[YXSB ]CDSTBCA_Sleep:%dms, end_ms = %d, sleep_s = %ds\n", wMilliSeconds,end_ms,  (end_ms - start_ms) /100);
}

void CDSTBCA_SemaphoreInit( CDCA_Semaphore* pSemaphore, CDCA_BOOL bInitVal )
{
    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in, bInitVal =%d!\n", __FUNCTION__, __LINE__, bInitVal);

    MT_ASSERT(pSemaphore != NULL);
    bInitVal = bInitVal ? 1 : 0;
    if(!mtos_sem_create((os_sem_t *)pSemaphore, bInitVal))
    {
        YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SemaphoreInit Failed!\n");
        MT_ASSERT(0);
    }
}

void CDSTBCA_SemaphoreSignal( CDCA_Semaphore* pSemaphore )
{
    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    MT_ASSERT(pSemaphore != NULL);
    if(!mtos_sem_give((os_sem_t *)pSemaphore))
    {
        YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SemaphoreSignal Failed!\n");
        MT_ASSERT(0);
    }
}

void CDSTBCA_SemaphoreWait( CDCA_Semaphore* pSemaphore )
{
    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    MT_ASSERT(pSemaphore != NULL);
    if(!mtos_sem_take((os_sem_t *)pSemaphore, 0))
    {
        YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SemaphoreWait Failed!\n");
        MT_ASSERT(0);
    }
}

void* CDSTBCA_Malloc( CDCA_U32 byBufSize)
{
    void *p_buf = NULL;
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    p_buf = mtos_malloc(byBufSize);
    MT_ASSERT(NULL != p_buf);
    //memset(p_buf, 0, byBufSize);
    return p_buf;
}

void  CDSTBCA_Free( void* pBuf )
{
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    if(NULL == pBuf)
        return;
    mtos_free(pBuf);
}

void  CDSTBCA_Memset( void*    pDestBuf,
                             CDCA_U8  c,
                             CDCA_U32 wSize )
{
    memset(pDestBuf, c, wSize);
}

void  CDSTBCA_Memcpy( void*       pDestBuf,
                             const void* pSrcBuf,
                             CDCA_U32    wSize )
{
    memcpy(pDestBuf, pSrcBuf, wSize);
}

void CDSTBCA_ReadBuffer( CDCA_U8   byBlockID,
                                CDCA_U8*  pbyData,
                                CDCA_U32* pdwLen )
{
    u8 ret = 0;
    s_cas_yxsb_priv_t *p_cas_yxsb_priv = NULL;
    p_cas_yxsb_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    MT_ASSERT(pbyData != NULL);
    if((p_cas_yxsb_priv->nvram_read != NULL) && (*pdwLen != 0))
    {
        ret = p_cas_yxsb_priv->nvram_read((u32)byBlockID, (u8 *)pbyData, (u32 *)pdwLen);
        if(ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, Read data from nvram failed!\n", __FUNCTION__, __LINE__);
        }
    }
}

void CDSTBCA_WriteBuffer( CDCA_U8        byBlockID,
                                 const CDCA_U8* pbyData,
                                 CDCA_U32       dwLen )
{
    u8 ret = 0;
    s_cas_yxsb_priv_t *p_cas_yxsb_priv = NULL;
    p_cas_yxsb_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    MT_ASSERT(pbyData != NULL);
    if((p_cas_yxsb_priv->nvram_write!= NULL) && (dwLen != 0))
    {
        ret = p_cas_yxsb_priv->nvram_write((u32)byBlockID, (u8 *)pbyData, dwLen);
        if(ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, Write data from nvram failed!\n", __FUNCTION__, __LINE__);
        }
    }
}

static RET_CODE _yxsb_filter_free(s_yxsb_filter_struct_t *filter)
{
    s32 ret = SUCCESS;
    dmx_device_t *p_dev = NULL;

    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_chan_stop(p_dev, filter->req_handle);
    MT_ASSERT(ret == SUCCESS);

    ret = dmx_chan_close(p_dev, filter->req_handle);
    MT_ASSERT(ret == SUCCESS);

    return SUCCESS;
}

CDCA_BOOL CDSTBCA_SetPrivateDataFilter( CDCA_U8        byReqID,  
											   const CDCA_U8* pbyFilter,  
											   const CDCA_U8* pbyMask, 
											   CDCA_U8        byLen, 
											   CDCA_U16       wPid, 
											   CDCA_U8        byWaitSeconds )
{
    u8 index = 0;
    s16 ret = ERR_FAILURE;
    u8 once_filter_flag = 0;
    dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;
    s_cas_yxsb_priv_t* p_priv = NULL;
    p_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    YXSB_DRV_DEBUG("[YXSB]设置过滤表CDSTBCA_SetPrivateDataFilter start\n");
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in, byReqId=%d, wPid=0x%x, byLen = %d,byWaitSeconds=%d!\n", __FUNCTION__, __LINE__, byReqID, wPid, byLen, byWaitSeconds);

    MT_ASSERT(pbyFilter != NULL);
    MT_ASSERT(pbyMask != NULL);

#if 0
    YXSB_DRV_DEBUG("dmx chan request\n");
    YXSB_DRV_DEBUG("pbyMask: \n");
    u32 i = 0;

    for(i = 0; i < YXSB_FILTER_DEPTH_SIZE; i++)
    {
        YXSB_DRV_DEBUG("%02x ", pbyMask[i]);
        if ((0 == ((i + 1) % 20)) && (i != 0))
        {
            YXSB_DRV_DEBUG("\n");
        }
    }
    YXSB_DRV_DEBUG("pbyFilter: \n");
    for(i = 0; i < YXSB_FILTER_DEPTH_SIZE; i++)
    {
        YXSB_DRV_DEBUG("%02x ", pbyFilter[i]);
        if ((0 == ((i + 1) % 20)) && (i != 0))
        {
            YXSB_DRV_DEBUG("\n");
        }
    }
#endif
    if((byReqID & 0x80) == 0x80)
    {
        once_filter_flag = 1;

        if(!(wPid == 0x10
            || wPid == p_priv->cas_desc.ca_pid
            || wPid == p_priv->audio_desc.a_cas_desc.ca_pid
            || wPid == p_priv->vedio_desc.v_cas_desc.ca_pid))
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, Free ECM filter!\n", __FUNCTION__, __LINE__);
            return CDCA_TRUE;
        }
        //byReqID &= 0x7F;
    }

    YXSB_STB_Drv_CaReqLock(&yxsb_filter_lock );
    
#if 0    
    if ((byReqID) >= YXSB_FILTER_MAX_NUM)
    {
        YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, filter is full!!!!!\n", __FUNCTION__, __LINE__);
        return CDCA_FALSE;
    }
#endif
    ///TODO: free the old used filter with the same byreqid except ecm
    for (index = 0; index < YXSB_FILTER_MAX_NUM; index ++)
    {
        if ((YXSB_FILTER_STATUS_BUSY == yxsb_filter[index].filter_status)
                && (byReqID == yxsb_filter[index].req_id)
                && ((byReqID & 0x80) != 0x80))
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, flag = %d, Filter is used, cancel old filter and request new filter!\n",
                            __FUNCTION__, __LINE__, (byReqID & 0x80));
            ret = _yxsb_filter_free(&yxsb_filter[index]);
            if (SUCCESS != ret)
            {
                YXSB_DRV_DEBUG("%s:LINE:%d, Free filter failed!\n", __FUNCTION__, __LINE__);
                YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock);
                return CDCA_FALSE;
            }
            memset((void *)&yxsb_filter[index], 0x00, sizeof(s_yxsb_filter_struct_t));
        }
    }

    for(index = 0; index < YXSB_FILTER_MAX_NUM; index++)
    {
        if(YXSB_FILTER_STATUS_FREE == yxsb_filter[index].filter_status)
            break;
    }

    if(index >= YXSB_FILTER_MAX_NUM)
    {
        OS_PRINTF("[YXSB][%s] %d, Filter is full!\n", __FUNCTION__, __LINE__);
        YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock);
        return CDCA_FALSE;
    }
    
    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    p_slot.in = DMX_INPUT_EXTERN0;
    p_slot.pid = wPid;
    p_slot.type = DMX_CH_TYPE_SECTION;

    ret = dmx_si_chan_open(p_dev, &p_slot, &yxsb_filter[index].req_handle);
    if (SUCCESS != ret)
    {
        YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, open chan failed, ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
        return CDCA_FALSE;
    }

    yxsb_filter[index].p_buf = yxsb_filter_buf[index];

    ret = dmx_si_chan_set_buffer(p_dev, yxsb_filter[index].req_handle, yxsb_filter[index].p_buf, YXSB_FILTER_BUFFER_SIZE);
    if (SUCCESS != ret)
    {
        YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, chan set buffer failed! ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
        return CDCA_FALSE;
    }

    p_param.req_mode = 0;
    p_param.continuous = TRUE;
    p_param.en_crc = FALSE;
    memset(p_param.mask, 0, DMX_SECTION_FILTER_SIZE);
    memset(p_param.value, 0, DMX_SECTION_FILTER_SIZE);
    memcpy(&p_param.mask[0], &pbyMask[0], byLen);
    memcpy(&p_param.value[0], &pbyFilter[0], byLen);
    #if 0
    YXSB_DRV_DEBUG("dmx chan request\n");
    YXSB_DRV_DEBUG("p_param.mask: \n");
    data_dump(p_param.mask, YXSB_FILTER_DEPTH_SIZE);
    YXSB_DRV_DEBUG("p_param.value: \n");
    data_dump(p_param.value, YXSB_FILTER_DEPTH_SIZE);
    #endif

    ret = dmx_si_chan_set_filter(p_dev, yxsb_filter[index].req_handle, &p_param);
    if (SUCCESS != ret)
    {
        YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
        OS_PRINTF("[YXSB]set filter failed!!!!!!!!!\n");
        return CDCA_FALSE;
    }

    ret = dmx_chan_start(p_dev, yxsb_filter[index].req_handle);
    //YXSB_DRV_DEBUG("[YXSB]dmx chan start\n");
    if (SUCCESS != ret)
    {
        YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, chan start failed! ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
        return CDCA_FALSE;
    }

    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, nWaitSeconds=%d!\n", __FUNCTION__, __LINE__, byWaitSeconds);
    if (0 == byWaitSeconds)
    {
        yxsb_filter[index].req_timeout = 0xffffffff;
    }
    else
    {
        yxsb_filter[index].req_timeout = byWaitSeconds * 1000;
    }
    yxsb_filter[index].filter_status = YXSB_FILTER_STATUS_BUSY;
    if(once_filter_flag)
        yxsb_filter[index].once_filter_flag = 1;
    yxsb_filter[index].req_id = byReqID;
    yxsb_filter[index].pid = wPid;
    //memcpy(yxsb_filter[byReqID].filter_data, pbyFilter, byLen);
    //memcpy(yxsb_filter[byReqID].filter_mask, pbyMask, byLen);
    yxsb_filter[index].start_ms = mtos_ticks_get();
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, ===filter data:byReqID=%d,yxsb_filter[%d].req_id=%d,yxsb_filter[%d].pid=0x%x!\n",
            __FUNCTION__, __LINE__, byReqID, index, yxsb_filter[index].req_id, index, yxsb_filter[index].pid);

    YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );

    //YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SetPrivateDataFilter OK\n");

    return CDCA_TRUE;
    

}
void CDSTBCA_ReleasePrivateDataFilter( CDCA_U8  byReqID, CDCA_U16 wPid )
{
    u32 ret, uIndex;
    //YXSB_DRV_DEBUG("[YXSB]CDSTBCA_ReleasePrivateDataFilter start\n");
    //YXSB_DRV_DEBUG("[YXSB]byReqID = %d, wPid = %d\n", byReqID, wPid);

    YXSB_STB_Drv_CaReqLock(&yxsb_filter_lock );
    for(uIndex = 0;uIndex < YXSB_FILTER_MAX_NUM; uIndex++)
    {
        if((yxsb_filter[uIndex].pid == wPid) && (yxsb_filter[uIndex].req_id == byReqID))
        {
            ret = _yxsb_filter_free(&yxsb_filter[uIndex]);
            if(ret != SUCCESS)
            {
                //YXSB_DRV_DEBUG("[DS]%s:LINE:%d, Free filter failed!!!!\n", __FUNCTION__, __LINE__);
                YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
                return;
            }
            memset((void *)&yxsb_filter[uIndex], 0, sizeof(s_yxsb_filter_struct_t));
            yxsb_filter[uIndex].filter_status = YXSB_FILTER_STATUS_FREE;
         }
    }
    YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
}

static void YXSB_STB_ScrSetAudioCW(const CDCA_U8* pbyOddKey,  
							                        const CDCA_U8* pbyEvenKey, 
							                        CDCA_U8        byKeyLen, 
							                        CDCA_U16 a_pid)
{
    s32 ret = 0;

    dmx_device_t* p_dev = NULL;
#ifdef CONCERTO_TF_CDCAS
    dmx_device_t* p_recDev = NULL;
    u16 rec_achannel = 0xffff;
#endif
    if((NULL == pbyOddKey) || (NULL == pbyEvenKey))
        return;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
    if (ret != SUCCESS)
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by a_pid failed!\n", __FUNCTION__, __LINE__);
        return;
    }

    //YXSB_DRV_DEBUG("%s:LINE:%d, set key old_a_channel = %x\n",  __FUNCTION__, __LINE__, old_a_channel);

    YXSB_DRV_DEBUG("%s:LINE:%d, set key a_pid = %x\n",  __FUNCTION__, __LINE__, a_pid );
    YXSB_DRV_DEBUG("%s:LINE:%d, set key  a_channel = %x\n", __FUNCTION__, __LINE__, a_channel);
    //YXSB_DRV_DEBUG("%s:LINE:%d, set key old_a_channel = %x\n",  __FUNCTION__, __LINE__, old_a_channel);

    if (a_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, a_channel, FALSE);
        ret = dmx_descrambler_onoff(p_dev, a_channel, TRUE);
        if (ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, enable audio descrambler failed!\n", __FUNCTION__, __LINE__);
            return;
        }

        ret = dmx_descrambler_set_odd_keys(p_dev, a_channel, (u8 *)pbyOddKey, byKeyLen);
        ret = dmx_descrambler_set_even_keys(p_dev, a_channel, (u8 *)pbyEvenKey, byKeyLen);
        
#ifdef CONCERTO_TF_CDCAS
        {
            p_recDev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
            ret = dmx_get_chanid_bypid(p_recDev, a_pid, &rec_achannel);
            if(ret != SUCCESS)
            {
                YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by a_pid failed!\n", __FUNCTION__, __LINE__);
                return;
            }
            ret = dmx_descrambler_onoff(p_recDev, rec_achannel, FALSE);
            ret = dmx_descrambler_onoff(p_recDev, rec_achannel, TRUE);
            if (ret != SUCCESS)
            {
                YXSB_DRV_DEBUG("%s:LINE:%d, enable audio descrambler failed!\n", __FUNCTION__, __LINE__);
                return;
            }

            ret = dmx_descrambler_set_odd_keys(p_recDev, rec_achannel, (u8 *)pbyOddKey, byKeyLen);
            ret = dmx_descrambler_set_even_keys(p_recDev, rec_achannel, (u8 *)pbyEvenKey, byKeyLen);
        }
#endif

    }
    else
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, No audio prpgram playing now!!!\n", __FUNCTION__, __LINE__);
    }

    //old_a_channel = a_channel;
}

static void YXSB_STB_ScrSetVideoCW(const CDCA_U8* pbyOddKey,  
							                        const CDCA_U8* pbyEvenKey, 
							                        CDCA_U8        byKeyLen, 
							                        CDCA_U16 v_pid)
{
    u32 ret = 0;
    dmx_device_t* p_dev = NULL;  
    
#ifdef CONCERTO_TF_CDCAS
    dmx_device_t* p_recDev = NULL;
    u16 rec_vchannel = 0xffff;
#endif
    if((NULL == pbyOddKey) || (NULL == pbyEvenKey))
        return;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
    if(ret != SUCCESS)
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by v_pid failed!\n", __FUNCTION__, __LINE__);
        return;
    }

    //YXSB_DRV_DEBUG("%s:LINE:%d, set key old_v_channel= %x \n",  __FUNCTION__, __LINE__, old_v_channel);

    YXSB_DRV_DEBUG("%s:LINE:%d, set key v_pid= %x \n", __FUNCTION__, __LINE__, v_pid);
    YXSB_DRV_DEBUG("%s:LINE:%d, set key v_channel= %x \n", __FUNCTION__, __LINE__, v_channel);
    //YXSB_DRV_DEBUG("%s:LINE:%d, set key old_v_channel= %x \n",  __FUNCTION__, __LINE__, old_v_channel);

    if (v_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, v_channel, FALSE);
        ret = dmx_descrambler_onoff(p_dev, v_channel, TRUE);
        if (ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, enable video descrambler failed!\n", __FUNCTION__, __LINE__);
            return;
        }

        ret = dmx_descrambler_set_odd_keys(p_dev, v_channel, (u8 *)pbyOddKey, byKeyLen);
        ret = dmx_descrambler_set_even_keys(p_dev, v_channel, (u8 *)pbyEvenKey, byKeyLen);
        
#ifdef CONCERTO_TF_CDCAS
        {
            p_recDev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
            ret = dmx_get_chanid_bypid(p_recDev, v_pid, &rec_vchannel);
            if(ret != SUCCESS)
            {
                YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by v_pid failed!\n", __FUNCTION__, __LINE__);
                return;
            }
            ret = dmx_descrambler_onoff(p_recDev, rec_vchannel, FALSE);
            ret = dmx_descrambler_onoff(p_recDev, rec_vchannel, TRUE);
            if (ret != SUCCESS)
            {
                YXSB_DRV_DEBUG("%s:LINE:%d, enable video descrambler failed!\n", __FUNCTION__, __LINE__);
                return;
            }

            ret = dmx_descrambler_set_odd_keys(p_recDev, rec_vchannel, (u8 *)pbyOddKey, byKeyLen);
            ret = dmx_descrambler_set_even_keys(p_recDev, rec_vchannel, (u8 *)pbyEvenKey, byKeyLen);
        }
#endif
    }
    else
        YXSB_DRV_DEBUG("%s:LINE:%d, No video prpgram playing now!!!\n", __FUNCTION__, __LINE__);

    //old_v_channel = v_channel;
}

static void YXSB_STB_ScrSetAudioVideoCW(const CDCA_U8* pbyOddKey,  
							                        const CDCA_U8* pbyEvenKey, 
							                        CDCA_U8        byKeyLen, 
							                        CDCA_U16 v_pid,
							                        CDCA_U16 a_pid)
{
    u32 ret = 0;
    dmx_device_t* p_dev = NULL;
#ifdef CONCERTO_TF_CDCAS
    dmx_device_t* p_recDev = NULL;
    u16 rec_vchannel = 0xffff;
    u16 rec_achannel = 0xffff;
#endif

    if((NULL == pbyOddKey) || (NULL == pbyEvenKey))
        return;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
    if(ret != SUCCESS)
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by v_pid failed!\n", __FUNCTION__, __LINE__);
        return;
    }
    
    ret = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
    if(ret != SUCCESS)
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by a_pid failed!\n", __FUNCTION__, __LINE__);
        return;
    }

    YXSB_DRV_DEBUG("%s:LINE:%d, set key v_pid, a_pid = %x \n", __FUNCTION__, __LINE__, v_pid, a_pid);
    YXSB_DRV_DEBUG("%s:LINE:%d, set key v_channel= %x, a_channel= %x \n", __FUNCTION__, __LINE__, v_channel, a_channel);
    //YXSB_DRV_DEBUG("%s:LINE:%d, set key old_v_channel= %x, old_a_channel= %x \n",  __FUNCTION__, __LINE__, old_v_channel, old_a_channel);

    if (v_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, v_channel, FALSE);
        ret = dmx_descrambler_onoff(p_dev, v_channel, TRUE);
        if (ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, enable video descrambler failed!\n", __FUNCTION__, __LINE__);
            return;
        }

        ret = dmx_descrambler_set_odd_keys(p_dev, v_channel, (u8 *)pbyOddKey, byKeyLen);
        ret = dmx_descrambler_set_even_keys(p_dev, v_channel, (u8 *)pbyEvenKey, byKeyLen);

    }
    else
        YXSB_DRV_DEBUG("%s:LINE:%d, No video prpgram playing now!!!\n", __FUNCTION__, __LINE__);
    
    if (a_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, a_channel, FALSE);
        ret = dmx_descrambler_onoff(p_dev, a_channel, TRUE);
        if (ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, enable video descrambler failed!\n", __FUNCTION__, __LINE__);
            return;
        }

        ret = dmx_descrambler_set_odd_keys(p_dev, a_channel, (u8 *)pbyOddKey, byKeyLen);
        ret = dmx_descrambler_set_even_keys(p_dev, a_channel, (u8 *)pbyEvenKey, byKeyLen);
    }
    else
        YXSB_DRV_DEBUG("%s:LINE:%d, No video prpgram playing now!!!\n", __FUNCTION__, __LINE__);
    

#ifdef CONCERTO_TF_CDCAS
    if (v_channel != 0xffff)
    {
        p_recDev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
        ret = dmx_get_chanid_bypid(p_recDev, v_pid, &rec_vchannel);
        if(ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by v_pid failed!\n", __FUNCTION__, __LINE__);
            return;
        }
        else
        {
            ret = dmx_descrambler_onoff(p_recDev, rec_vchannel, FALSE);
            ret = dmx_descrambler_onoff(p_recDev, rec_vchannel, TRUE);
            if (ret != SUCCESS)
            {
                YXSB_DRV_DEBUG("%s:LINE:%d, enable video descrambler failed!\n", __FUNCTION__, __LINE__);
            }
            
            ret = dmx_descrambler_set_odd_keys(p_recDev, rec_vchannel, (u8 *)pbyOddKey, byKeyLen);
            ret = dmx_descrambler_set_even_keys(p_recDev, rec_vchannel, (u8 *)pbyEvenKey, byKeyLen);
        }
    }

    
    if (a_channel != 0xffff)
    {
        p_recDev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
        ret = dmx_get_chanid_bypid(p_recDev, a_pid, &rec_achannel);
        if(ret != SUCCESS)
        {
            YXSB_DRV_DEBUG("%s:LINE:%d, get chan id by a_pid failed!\n", __FUNCTION__, __LINE__);
        }
        else
        {
            ret = dmx_descrambler_onoff(p_recDev, rec_achannel, FALSE);
            ret = dmx_descrambler_onoff(p_recDev, rec_achannel, TRUE);
            if (ret != SUCCESS)
            {
                YXSB_DRV_DEBUG("%s:LINE:%d, enable audio descrambler failed!\n", __FUNCTION__, __LINE__);
                return;
            }
            
            ret = dmx_descrambler_set_odd_keys(p_recDev, rec_achannel, (u8 *)pbyOddKey, byKeyLen);
            ret = dmx_descrambler_set_even_keys(p_recDev, rec_achannel, (u8 *)pbyEvenKey, byKeyLen);
        }
    }
#endif

    //old_a_channel = a_channel;
    //old_v_channel = v_channel;
}

void CDSTBCA_ScrSetCW( CDCA_U16       wEcmPID,  
							  const CDCA_U8* pbyOddKey,  
							  const CDCA_U8* pbyEvenKey, 
							  CDCA_U8        byKeyLen, 
							  CDCA_BOOL      bTapingEnabled )
{
    u16 v_pid = 0;
    u16 a_pid = 0;
    s_cas_yxsb_priv_t* p_cas_yxsb_priv = NULL;

    YXSB_DRV_DEBUG("%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    
    if((NULL == pbyOddKey) || (NULL == pbyEvenKey))
        return;

    /*OS_PRINTF("OddKey %02x %02x %02x %02x %02x %02x %02x %02x\tEvenKey %02x %02x %02x %02x %02x %02x %02x %02x\n",
                            pbyOddKey[0], pbyOddKey[1], pbyOddKey[2], pbyOddKey[3],
                            pbyOddKey[4], pbyOddKey[5], pbyOddKey[6], pbyOddKey[7],
                            pbyEvenKey[0], pbyEvenKey[1], pbyEvenKey[2], pbyEvenKey[3],
                            pbyEvenKey[4], pbyEvenKey[5], pbyEvenKey[6], pbyEvenKey[7]);*/

    p_cas_yxsb_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    v_pid = p_cas_yxsb_priv->vedio_desc.v_pid;
    a_pid = p_cas_yxsb_priv->audio_desc.a_pid;

    //YXSB_DRV_DEBUG("%s:LINE:%d, a_pid = 0x%x, v_pid = 0x%x!\n", __FUNCTION__, __LINE__, a_pid, v_pid);
    OS_PRINTF("wecm pid = 0x%x, total ecm pid = 0x%x, video ecm pid = 0x%x, audio ecm pid = 0x%x\n",
                                wEcmPID,
                                p_cas_yxsb_priv->cas_desc.ca_pid,
                                p_cas_yxsb_priv->vedio_desc.v_cas_desc.ca_pid,
                                p_cas_yxsb_priv->audio_desc.a_cas_desc.ca_pid);

    if(p_cas_yxsb_priv->cas_desc.ca_pid == wEcmPID)
        YXSB_STB_ScrSetAudioVideoCW(pbyOddKey, pbyEvenKey, byKeyLen, v_pid, a_pid);
    else
    {
        if(p_cas_yxsb_priv->vedio_desc.v_cas_desc.ca_pid == wEcmPID)
        {
            YXSB_STB_ScrSetVideoCW(pbyOddKey, pbyEvenKey, byKeyLen, v_pid);
        }
        if(p_cas_yxsb_priv->audio_desc.a_cas_desc.ca_pid == wEcmPID)
        {
            //return;
            YXSB_STB_ScrSetAudioCW(pbyOddKey, pbyEvenKey, byKeyLen, a_pid);
        }
    }
}

CDCA_BOOL CDSTBCA_SCReset( CDCA_U8* pbyATR, CDCA_U8* pbyLen )
{
    scard_atr_desc_t atr = {0};
    u8 index = 0;
    s16 ret = 0;
    u8 buf[32];

    atr.p_buf = buf;

    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    for (index = 0; index < 3; index ++)
    {
        ret = scard_active(p_yxsb_scard_dev, &atr);
        if (SUCCESS == ret)
        {
            break;
        }
    }
    if (SUCCESS != ret)
    {
        YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, active smart card failed!\n", __FUNCTION__, __LINE__);
        *pbyLen = 0;
        return CDCA_FALSE;
    }

    if(atr.atr_len >= 33)
    {
        YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, active len is more then 33!\n", __FUNCTION__, __LINE__);
        *pbyLen = 0;
        return CDCA_FALSE;
    }
    
    *pbyLen = atr.atr_len;
    memcpy(pbyATR, atr.p_buf, *pbyLen);

    YXSB_DRV_DEBUG("[YXSB]CDSTBCA_SCReset OVER\n");

    return CDCA_TRUE;
}

CDCA_BOOL CDSTBCA_SCPBRun( const CDCA_U8* pbyCommand, 
								  CDCA_U16       wCommandLen,  
								  CDCA_U8*       pbyReply,  
								  CDCA_U16*      pwReplyLen  )
{
    scard_opt_desc_t opt_desc = {0};
    RET_CODE ret = ERR_FAILURE;
    s8 retry_times = 3;
    u8 *p_buf = NULL;

    MT_ASSERT(pbyCommand != NULL);
    MT_ASSERT(pbyReply != NULL);
    MT_ASSERT(pwReplyLen != NULL);

//    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    #if 0
    YXSB_DRV_DEBUG("Tx:\n");
    data_dump(pbyCommand, wCommandLen);
    #endif

    if(!wCommandLen)
        return CDCA_FALSE;

    p_buf = (u8 *)mtos_malloc(wCommandLen);
    MT_ASSERT(p_buf != NULL);
    memcpy(p_buf, pbyCommand, wCommandLen);
    
    opt_desc.p_buf_out = p_buf;
    opt_desc.bytes_out = wCommandLen;
    opt_desc.p_buf_in = pbyReply;
    opt_desc.bytes_in_actual = 0;

    do
    {
        ret = scard_pro_rw_transaction(&opt_desc);
        if(ret == SUCCESS)
        {
            //YXSB_DRV_DEBUG("[YXSB]T0 transaction success!\n");
            break;
        }
        retry_times--;
    }while (retry_times > 0);

    if(ret == SUCCESS)
    {
        //YXSB_DRV_DEBUG("Rx:\n");
        //data_dump(opt_desc.p_buf_in, opt_desc.bytes_in_actual);

        *pwReplyLen = opt_desc.bytes_in_actual;
        mtos_free(p_buf);
        return CDCA_TRUE;
    }
    else
    {
        YXSB_DRV_DEBUG("T0 transaction failed!\n");
        YXSB_DRV_DEBUG("Tx:\n");
        data_dump(opt_desc.p_buf_out, *pwReplyLen);
        
        *pwReplyLen = 0;
        mtos_free(p_buf);
        return CDCA_FALSE;
    }
}
void CDSTBCA_EntitleChanged( CDCA_U16 wTvsID )
{
    OS_PRINTF("[YXSB][%s %d]CDSTBCA_EntitleChanged, wTvsID = %d\n", __FUNCTION__, __LINE__, wTvsID);

    CDCAS_SendEvent(CAS_S_PROG_AUTH_CHANGE, wTvsID);
}

void CDSTBCA_DetitleReceived( CDCA_U8 bstatus )
{
    u32 event = 0;
    
    switch(bstatus)
    {
        case CDCA_Detitle_All_Read:
            event = CAS_C_DETITLE_ALL_READED;
            OS_PRINTF("[YXSB][%s %d]CDCA_Detitle_All_Read\n", __FUNCTION__, __LINE__);
            break;
        case CDCA_Detitle_Received:
            event = CAS_C_DETITLE_RECEIVED;
            OS_PRINTF("[YXSB][%s %d]CDCA_Detitle_Received\n", __FUNCTION__, __LINE__);
            break;
        case CDCA_Detitle_Space_Small:
            event = CAS_C_DETITLE_SPACE_SMALL;
            OS_PRINTF("[YXSB][%s %d]CDCA_Detitle_Space_Small\n", __FUNCTION__, __LINE__);
            break;
        case CDCA_Detitle_Ignore:
            OS_PRINTF("[YXSB][%s %d]CDCA_Detitle_Ignore\n", __FUNCTION__, __LINE__);
            break;
        default:
            OS_PRINTF("[YXSB][%s %d]CDSTBCA_DetitleReceived\n", __FUNCTION__, __LINE__);
            return;
    }

    CDCAS_SendEvent(event, 0);
}

void CDSTBCA_GetSTBID( CDCA_U16* pwPlatformID,
                              CDCA_U32* pdwUniqueID)
{
    u8 p_buf[4] = {0};
    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    s_cas_yxsb_priv_t *p_priv = (s_cas_yxsb_priv_t *)p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;
    
    *pwPlatformID = CDCASTB_GetPlatformID();
    if(p_priv->machine_serial_get != NULL)
    {
        p_priv->machine_serial_get(p_buf, sizeof(p_buf));
        YXSB_DRV_DEBUG("p_buf[0] = %d, p_buf[1] = %d, p_buf[2] = %d, p_buf[3] = %d\n",
                                            p_buf[0], p_buf[1], p_buf[2], p_buf[3]);
    }
    
    *pdwUniqueID = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
    YXSB_DRV_DEBUG("pdwUniqueID = 0x%x, pwPlatformID = 0x%x\n", *pdwUniqueID, *pwPlatformID);
    
}

CDCA_U16 CDSTBCA_SCFunction( CDCA_U8* pData)
{
    *pData = (u8)(NTSM_ERR_UNSUPPORT >> 8);
    *(pData + 1) = (u8)NTSM_ERR_UNSUPPORT;  
    return NTSM_ERR_UNSUPPORT;
}

u32 CAS_YXSB_ConvertDate(u16 days, cas_yxsb_time_t* yxsb_time)
{
    u32 nyear = 0;
    u32 nMon = 0;
    u32 nday = 0;
    u32 nTemp = 0;

    /*year.*/
    nyear = 2000;
    do
    {
        if((nyear % 4 == 0 && nyear % 100 != 0) || nyear % 400 == 0)
        {
            nTemp = 366;
        }
        else
        {
            nTemp = 365;
        }

        if(nTemp > days)
        {
            break;
        }

        days -= nTemp;
        nyear ++;
    }while(1);

    /*month.*/
    nMon = 1;
    do
    {  
        switch (nMon)
        {
            case 2 :
                if ((nyear % 4 == 0 && nyear % 100 != 0) || nyear % 400 == 0)
                    nTemp = 29;
                else
                    nTemp = 28;
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                nTemp = 30;
                break;
            default:
                nTemp = 31;
                break;
        }
        if(nTemp > days)
        {
            break;
        }
        days -= nTemp;
        nMon ++;
    }while(1);

    if(1 > nMon || 12 < nMon)
    {
        YXSB_DRV_DEBUG("ERR ON nMon = %d\n", nMon);
        return CDCA_FALSE;
    }

    /*day.*/
    nday = 1;
    nday += days; 
    if(1 > nday || 31 < nday)
    {
        YXSB_DRV_DEBUG("ERR ON nday = %d\n", nday);
        return CDCA_FALSE;
    }

    YXSB_DRV_DEBUG("day = %d, mon = %d, year = %d\n", nday, nMon, nyear);
    yxsb_time->day = (u8)nday;
    yxsb_time->month = (u8)nMon;
    yxsb_time->year = (u16)nyear;

    return 0;
}

void CDSTBCA_StartIppvBuyDlg( CDCA_U8                 byMessageType,
                                     CDCA_U16                wEcmPid,
                                     const SCDCAIppvBuyInfo* pIppvProgram  )
{
    u8 uIndex = 0;
    cas_yxsb_time_t yxsb_time = {0};
    
    YXSB_STB_Drv_CaReqLock(&IppvBuyInfo_lock);
    memset(&IppvBuyInfo, 0x00, sizeof(IppvBuyInfo));

    IppvBuyInfo.message_type = byMessageType;

    YXSB_DRV_DEBUG("IPPV Program buy information: EcmPid = %x,Type = %d \n", wEcmPid, byMessageType);

    IppvBuyInfo.burse_id = pIppvProgram->m_bySlotID;
    YXSB_DRV_DEBUG("burse_id = %d\n", IppvBuyInfo.burse_id);
    
    if(pIppvProgram->m_byPriceNum > CDCA_MAXNUM_PRICE)
        IppvBuyInfo.price_num = CDCA_MAXNUM_PRICE;
    else
        IppvBuyInfo.price_num = pIppvProgram->m_byPriceNum;

    YXSB_DRV_DEBUG("price_num = %d\n", IppvBuyInfo.price_num);
    
    for(uIndex = 0; uIndex < IppvBuyInfo.price_num; uIndex++)
    {
        if(pIppvProgram->m_Price[uIndex].m_byPriceCode == CDCA_IPPVPRICETYPE_TPPVVIEW)
            IppvBuyInfo.ipp_price[uIndex].price_type = CA_IPPVPRICETYPE_TPPNOTAP;
        else
            IppvBuyInfo.ipp_price[uIndex].price_type = CA_IPPVPRICETYPE_TPPTAP;

        IppvBuyInfo.ipp_price[uIndex].price = pIppvProgram->m_Price[uIndex].m_wPrice;
        YXSB_DRV_DEBUG("price_type = %d, m_byPriceCode = %d, price = %d\n",
                                            IppvBuyInfo.ipp_price[uIndex].price_type,
                                            pIppvProgram->m_Price[uIndex].m_byPriceCode,
                                            IppvBuyInfo.ipp_price[uIndex].price);
    }

    IppvBuyInfo.product_id = pIppvProgram->m_dwProductID;
    IppvBuyInfo.ecm_pid = wEcmPid;

    YXSB_DRV_DEBUG("product_id = %d, ecm_pid = %d\n", IppvBuyInfo.product_id, IppvBuyInfo.ecm_pid);
    
    if(byMessageType == CDCA_IPPT_PAYVIEWED_SEGMENT)
    {
        IppvBuyInfo.expired_time[0] = 0;
        IppvBuyInfo.expired_time[1] = 0;
        IppvBuyInfo.expired_time[2] = 0;
        IppvBuyInfo.expired_time[3] = 0;
        ///TODO: fix the time format :IPPT
        IppvBuyInfo.expired_time[4] = pIppvProgram->m_wIPPVTime.m_wIntervalMin & 0xFF;
        IppvBuyInfo.expired_time[5] = (pIppvProgram->m_wIPPVTime.m_wIntervalMin >>8) & 0xFF;
        
        YXSB_DRV_DEBUG("ippt interval = %d, %d\n", 
                                            pIppvProgram->m_wIPPVTime.m_wIntervalMin,
                                            ((IppvBuyInfo.expired_time[5] << 8) | IppvBuyInfo.expired_time[4]));
    }
    else
    {
        ///TODO:: check CAS_YXSB_ConvertDate 
        CAS_YXSB_ConvertDate(pIppvProgram->m_wIPPVTime.m_wExpiredDate, &yxsb_time);
        IppvBuyInfo.expired_time[0] = yxsb_time.year / 100;
        IppvBuyInfo.expired_time[1] = yxsb_time.year % 100;
        IppvBuyInfo.expired_time[2] = yxsb_time.month;
        IppvBuyInfo.expired_time[3] = yxsb_time.day;
        IppvBuyInfo.expired_time[4] = yxsb_time.hour;
        IppvBuyInfo.expired_time[5] = yxsb_time.minute;
        
        YXSB_DRV_DEBUG("m_wExpiredDate = %d, year = %d, month = %d, day = %d, hour = %d, minute = %d\n",
                                            pIppvProgram->m_wIPPVTime.m_wExpiredDate,
                                            (IppvBuyInfo.expired_time[0] * 100 + IppvBuyInfo.expired_time[1]),
                                            IppvBuyInfo.expired_time[2],
                                            IppvBuyInfo.expired_time[3],
                                            IppvBuyInfo.expired_time[4],
                                            IppvBuyInfo.expired_time[5]);
    }
    IppvBuyInfo.tvs_id = pIppvProgram->m_wTvsID;
    YXSB_DRV_DEBUG("tvs_id = %d\n", IppvBuyInfo.tvs_id);
    YXSB_STB_Drv_CaReqUnLock(&IppvBuyInfo_lock);

    CDCAS_SendEvent(CAS_C_IPP_BUY_OR_NOT, (u32)&IppvBuyInfo);
}

void CDSTBCA_HideIPPVDlg(CDCA_U16 wEcmPid)
{
    YXSB_DRV_DEBUG("IPPV BUY INFROM hide ECMPID = %x \n", wEcmPid);
    CDCAS_SendEvent(CAS_C_HIDE_IPPV, wEcmPid);
}                                    

void CDSTBCA_EmailNotifyIcon( CDCA_U8 byShow, CDCA_U32 dwEmailID )
{
    YXSB_DRV_DEBUG("Email Notify. Show Type = %d, Email ID = %d \n", byShow, dwEmailID);

    u32 event;
    
    switch(byShow)
    {
        case CDCA_Email_IconHide:
            event = CAS_C_HIDE_NEW_EMAIL;
            break;
        case CDCA_Email_New:
            event = CAS_C_SHOW_NEW_EMAIL;
            break;
        case CDCA_Email_SpaceExhaust:
            event = CAS_C_SHOW_SPACE_EXHAUST;
            break;
        default:
            OS_PRINTF("[YXSB][%s %d]", __FUNCTION__, __LINE__);
            return;
    }

    CDCAS_SendEvent(event, dwEmailID);
}


void  CDSTBCA_ShowOSDInfo(SCDCAOSDInfo *pOSDInfo)
{
    OS_PRINTF("[YXSB][%s %d]", __FUNCTION__, __LINE__);
    u16 uOSDLen = 0;
    uOSDLen = (strlen(pOSDInfo->szContent) > CDCA_MAXLEN_OSD) ? CDCA_MAXLEN_OSD : strlen(pOSDInfo->szContent);
    YXSB_DRV_DEBUG("OSD len = %d, msg = %s\n", uOSDLen, pOSDInfo->szContent);

    if(pOSDInfo == NULL)
    {
        CDCAS_SendEvent(CAS_C_HIDE_OSD_TEXT, 0);
        return;
    }

    YXSB_STB_Drv_CaReqLock(&OsdMsg_lock);
    memset(&OsdMsg, 0x00, sizeof(OsdMsg));
    OsdMsg.type = 0;
    OsdMsg.display_front_size = pOSDInfo->byFontSize;
    OsdMsg.font_color = pOSDInfo->dwFontColor;
    OsdMsg.background_color = pOSDInfo->dwBackgroundColor;
    OsdMsg.background_area = pOSDInfo->byBackgroundArea;
    memcpy(OsdMsg.data, pOSDInfo->szContent, uOSDLen);
    OsdMsg.data_len = uOSDLen;
    
    YXSB_STB_Drv_CaReqUnLock(&OsdMsg_lock);
    
    YXSB_DRV_DEBUG("OSD font size = %d\n", OsdMsg.display_front_size);
    YXSB_DRV_DEBUG("OSD font color = %d\n", OsdMsg.font_color);
    YXSB_DRV_DEBUG("OSD bg color = %d\n", OsdMsg.background_color);
    YXSB_DRV_DEBUG("OSD bg area = %d\n", OsdMsg.background_area);
    YXSB_DRV_DEBUG("OSD data = %s\n", OsdMsg.display_front_size);
    
    CDCAS_SendEvent(CAS_C_SHOW_OSD, (u32)&OsdMsg);
}


void CDSTBCA_ShowOSDMessage( CDCA_U8     byStyle,
                                    const char* szMessage )
{
    u8 uOSDLen = 0;
    u8 uIndex = 0;
    u32 event = 0;
    u8 *p_buf = NULL;

    YXSB_DRV_DEBUG("[YXSB]%s,line = %d, byStyle = %d, ticks = %d\n", __FUNCTION__, __LINE__, byStyle, mtos_ticks_get());

    if(NULL == szMessage) 
    {
        return;
    }

    if(byStyle == CDCA_OSD_TOP)
        event = CAS_C_SHOW_OSD_UP;
    else if(byStyle == CDCA_OSD_BOTTOM)
        event = CAS_C_SHOW_OSD_DOWN;
    else if(byStyle == CDCA_OSD_FULLSCREEN)
        event = CAS_C_SHOW_OSD_FULLSCREEN;
    else if(byStyle == CDCA_OSD_HALFSCREEN)
        event = CAS_C_SHOW_OSD_HALFSCREEN;

    uOSDLen = (strlen(szMessage) > CDCA_MAXLEN_OSD) ? CDCA_MAXLEN_OSD : strlen(szMessage);
    
    p_buf = (u8 *)mtos_malloc(uOSDLen);
    MT_ASSERT(p_buf != NULL);
    memcpy(p_buf, szMessage, uOSDLen);

    for(uIndex = 0; uIndex < uOSDLen; uIndex++)
    {
        if(p_buf[uIndex] == '\n' || p_buf[uIndex] == '\r')
        {
            p_buf[uIndex] = ' ';
        }
    }

    YXSB_DRV_DEBUG("szMsg = %s Show Style = %d \n", p_buf, byStyle);
    
    YXSB_STB_Drv_CaReqLock(&OsdMsg_lock);

    memset(&OsdMsg, 0x00, sizeof(OsdMsg));
    memcpy(OsdMsg.data, p_buf, uOSDLen);

    OsdMsg.osd_display.roll_mode = 0;
    OsdMsg.osd_display.roll_value = 0;
    OsdMsg.type = 0;

    YXSB_STB_Drv_CaReqUnLock(&OsdMsg_lock);
    mtos_free(p_buf);

    CDCAS_SendEvent(event, (u32)&OsdMsg);

}

void CDSTBCA_HideOSDMessage( CDCA_U8 byStyle )
{
    u32 event = 0;

    YXSB_DRV_DEBUG("[YXSB]%s,line = %d, byStyle = %d, ticks = %d\n", __FUNCTION__, __LINE__, byStyle, mtos_ticks_get());
    
    if(byStyle == CDCA_OSD_TOP)
        event = CAS_C_HIDE_OSD_UP;
    else if(byStyle == CDCA_OSD_BOTTOM)
        event = CAS_C_HIDE_OSD_DOWN;
    else if(byStyle == CDCA_OSD_FULLSCREEN)
        event = CAS_C_HIDE_OSD_FULLSCREEN;
    else if(byStyle == CDCA_OSD_HALFSCREEN)
        event = CAS_C_HIDE_OSD_HALFSCREEN;

    CDCAS_SendEvent(event, 0);
}


void  CDSTBCA_RequestFeeding( CDCA_BOOL bReadStatus )
{
    OS_PRINTF("CDSTBCA_RequestFeeding, bReadStatus = %d\n", bReadStatus);

    if(bReadStatus)
        CDCAS_SendEvent(CAS_S_SON_CARD_REGU_INSERT, bReadStatus);
    else
        CDCAS_SendEvent(CAS_E_FEED_DATA_ERROR, bReadStatus);
}


void CDSTBCA_LockService( const SCDCALockService* pLockService )
{
    u8 uIndex = 0;
    u32 uFreq = 0;      //KHz
    u32 uSymrate = 0;       //KHz

    if(NULL == pLockService)
        return;

    for(uIndex = 0; uIndex < 8; uIndex++)
    {
        uFreq = uFreq * 10;
        uFreq += (pLockService->m_dwFrequency >> ((7 - uIndex) * 4)) & 0x0000000F;

        uSymrate = uSymrate * 10;
        uSymrate += (pLockService->m_symbol_rate >> ((7 - uIndex)* 4)) & 0x0000000F;
    }
    uFreq /= 1000;

    YXSB_STB_Drv_CaReqLock(&ForceChannelInfo_lock);
    memset(&ForceChannelInfo, 0x00, sizeof(ForceChannelInfo));

    ForceChannelInfo.frequency = (u16)uFreq;
    ForceChannelInfo.symbol_rate = uSymrate;
    ForceChannelInfo.pcr_pid = pLockService->m_wPcrPid;
    YXSB_DRV_DEBUG("[YXSB]freq:%dKHz, BCD:%x, sym_rate:%dKHz, BCD:%x, pcr_pid = %d,\n", 
                                        ForceChannelInfo.frequency,
                                        pLockService->m_dwFrequency,
                                        ForceChannelInfo.symbol_rate,
                                        pLockService->m_symbol_rate,
                                        ForceChannelInfo.pcr_pid);

    switch(pLockService->m_Modulation)
    {
        case 1:
            ForceChannelInfo.modulation = NIM_MODULA_QAM16;
            break;
        case 2:
            ForceChannelInfo.modulation = NIM_MODULA_QAM32;
            break;
        case 3:
            ForceChannelInfo.modulation = NIM_MODULA_QAM64;
            break;
        case 4:
            ForceChannelInfo.modulation = NIM_MODULA_QAM128;
            break;
        case 5:
            ForceChannelInfo.modulation = NIM_MODULA_QAM256;
            break;
        default:
            break;
    }

    ForceChannelInfo.component_num= pLockService->m_ComponentNum;
    YXSB_DRV_DEBUG("[YXSB]component_num = %d\n", ForceChannelInfo.component_num);
    
    for(uIndex = 0; uIndex < pLockService->m_ComponentNum; uIndex++)
    {
        ForceChannelInfo.CompArr[uIndex].comp_PID = pLockService->m_CompArr[uIndex].m_wCompPID;
        ForceChannelInfo.CompArr[uIndex].ECM_PID= pLockService->m_CompArr[uIndex].m_wECMPID;
        ForceChannelInfo.CompArr[uIndex].comp_type= pLockService->m_CompArr[uIndex].m_CompType;
        YXSB_DRV_DEBUG("[YXSB]comp_PID = %d, ECM_PID = %d, comp_type = %d\n",
                                            ForceChannelInfo.CompArr[uIndex].comp_PID,
                                            ForceChannelInfo.CompArr[uIndex].ECM_PID,
                                            ForceChannelInfo.CompArr[uIndex].comp_type);
    }

    ForceChannelInfo.m_fec_outer = pLockService->m_fec_outer;
    ForceChannelInfo.m_fec_inner = pLockService->m_fec_inner;
    
    YXSB_DRV_DEBUG("[YXSB]moudulation = %d, m_fec_outer = %d, m_fec_outer = %d\n",
                                        ForceChannelInfo.modulation,
                                        ForceChannelInfo.m_fec_outer,
                                        ForceChannelInfo.m_fec_outer);

    YXSB_STB_Drv_CaReqUnLock(&ForceChannelInfo_lock);

    CDCAS_SendEvent(CAS_C_FORCE_CHANNEL, (u32)&ForceChannelInfo);
}

void CDSTBCA_UNLockService( void )
{
    ///TODO: finish the func
    OS_PRINTF("[YXSB][%s %d] in\n", __FUNCTION__, __LINE__);
    
    CDCAS_SendEvent(CAS_C_UNFORCE_CHANNEL, 0);
}


void CDSTBCA_ShowBuyMessage( CDCA_U16 wEcmPID,
                                    CDCA_U8  byMessageType )
{
    u32 event;
    
    switch(byMessageType)
    {
        case CDCA_MESSAGE_CANCEL_TYPE:/*清除显示*/
            OS_PRINTF("\n[YXSB][%s %d] 清除显示 \n", __FUNCTION__, __LINE__);
            event = CAS_S_CLEAR_DISPLAY;
            break;

        case CDCA_MESSAGE_BADCARD_TYPE:/*无法识别卡*/
            OS_PRINTF("\[YXSB][%s %d] 无法识别卡\n", __FUNCTION__, __LINE__);
            event = CAS_E_ILLEGAL_CARD;
            break;

        case CDCA_MESSAGE_EXPICARD_TYPE:/*智能卡过期，请更换新卡*/
            OS_PRINTF("\nT[YXSB][%s %d] 智能卡过期，请更换新卡\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_OVERDUE;

            break;

        case CDCA_MESSAGE_INSERTCARD_TYPE:/*加扰节目，请插入智能卡*/
            OS_PRINTF("\n[YXSB][%s %d] 加扰节目，请插入智能卡\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_REGU_INSERT;
            break;

        case CDCA_MESSAGE_NOOPER_TYPE:/*卡中不存在节目运营商*/
            OS_PRINTF("\n[YXSB][%s %d] 卡中不存在节目运营商\n", __FUNCTION__, __LINE__);
            event = CAS_E_NO_OPTRATER_INFO;
            break;

        case CDCA_MESSAGE_BLACKOUT_TYPE:/*条件禁播*/
            OS_PRINTF("\n[YXSB][%s %d] 条件禁播\n", __FUNCTION__, __LINE__);
            event = CAS_E_BLACKOUT;
            break;

        case CDCA_MESSAGE_OUTWORKTIME_TYPE:/* 当前时段被设定为不能观看 */
            OS_PRINTF("\n[YXSB][%s %d] 当前时段被设定为不能观看\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_OUT_WORK_TIME;
            break;

        case CDCA_MESSAGE_WATCHLEVEL_TYPE:/*节目级别高于设定观看级别*/
            OS_PRINTF("\n[YXSB][%s %d] 节目级别高于设定观看级别\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_CHILD_UNLOCK;
            break;

        case CDCA_MESSAGE_PAIRING_TYPE:/* 智能卡与本机顶盒不对应 */
            OS_PRINTF("\n[YXSB][%s %d] 智能卡与本机顶盒不对应\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_DIS_PARTNER;
            break;

        case CDCA_MESSAGE_NOENTITLE_TYPE:/*没有授权*/
            OS_PRINTF("\n[YXSB][%s %d] 没有授权\n", __FUNCTION__, __LINE__);
            event = CAS_E_PROG_UNAUTH;
            break;

        case CDCA_MESSAGE_DECRYPTFAIL_TYPE:/*节目解密失败*/
            OS_PRINTF("\n[YXSB][%s %d] 节目解密失败\n", __FUNCTION__, __LINE__);
            event = CAS_E_CW_INVALID;
            break;

        case CDCA_MESSAGE_NOMONEY_TYPE:/*卡内金额不足*/
            OS_PRINTF("\n[YXSB][%s %d] 卡内金额不足\n", __FUNCTION__, __LINE__);
            event = CAS_E_CADR_NO_MONEY;
            break;

        case CDCA_MESSAGE_ERRREGION_TYPE:/*区域不正确*/
            OS_PRINTF("\n[YXSB][%s %d] 区域不正确\n", __FUNCTION__, __LINE__);
            event = CAS_E_ZONE_CODE_ERR;
            break;

        case CDCA_MESSAGE_NEEDFEED_TYPE:/*子卡需要和母卡对应，请插入母卡 */
            OS_PRINTF("\n[YXSB][%s %d] 子卡需要和母卡对应，请插入母卡\n", __FUNCTION__, __LINE__);
            event = CAS_S_MOTHER_CARD_REGU_INSERT;
            break;

        case CDCA_MESSAGE_ERRCARD_TYPE:/*智能卡校验失败，请联系运营商*/
            OS_PRINTF("\n[YXSB][%s %d] 智能卡校验失败，请联系运营商\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_INIT_FAIL;
            break;

        case CDCA_MESSAGE_UPDATE_TYPE: /*智能卡升级中，请不要拔卡或者关机 */
            OS_PRINTF("\n[YXSB][%s %d] 智能卡升级中，请不要拔卡或者关机\n", __FUNCTION__, __LINE__);
            event = CAS_S_CARD_UPGRADING;
            break;

        case CDCA_MESSAGE_LOWCARDVER_TYPE:/*请升级智能卡*/
            OS_PRINTF("\n[YXSB][%s %d] 请升级智能卡\n", __FUNCTION__, __LINE__);
            event = CAS_S_CARD_NEED_UPG;
            break;

        case CDCA_MESSAGE_VIEWLOCK_TYPE:/*请勿频繁切换频道*/
            OS_PRINTF("\n[YXSB][%s %d] 请勿频繁切换频道\n", __FUNCTION__, __LINE__);
            event = CAS_E_VIEWLOCK;
            break;

        case CDCA_MESSAGE_MAXRESTART_TYPE:/* 智能卡暂时休眠，请5分钟后重新开机 */
            OS_PRINTF("\n[YXSB][%s %d] 智能卡暂时休眠，请5分钟后重新开机\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_SLEEP;
            break;

        case CDCA_MESSAGE_FREEZE_TYPE:/*智能卡已冻结，请联系运营商*/
            OS_PRINTF("\n[YXSB][%s %d] 智能卡已冻结，请联系运营商\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_FREEZE;
            break;

        case CDCA_MESSAGE_CALLBACK_TYPE:/*智能卡已暂停，请回传收视记录给运营商*/
            OS_PRINTF("\n[YXSB][%s %d] 智能卡已暂停，请回传收视记录给运营商\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_PAUSE;
            break;
        case CDCA_MESSAGE_CURTAIN_TYPE:/*高级预览节目，该阶段不能免费观看*/
            OS_PRINTF("\n[YXSB][%s %d]高级预览节目，该阶段不能免费观看\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_CURTAIN;
            break;
        case CDCA_MESSAGE_CARDTESTSTART_TYPE:/*升级测试卡测试中...*/
            OS_PRINTF("\n[YXSB][%s %d] 升级测试卡测试中... \n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_TESTSTART;
            break;
            
        case CDCA_MESSAGE_CARDTESTFAILD_TYPE:/*升级测试卡测试失败，请检查机卡通讯模块*/
            OS_PRINTF("\n[YXSB][%s %d] 升级测试卡测试失败，请检查机卡通讯模块\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_TESTFAILD;
            break;
            
        case CDCA_MESSAGE_CARDTESTSUCC_TYPE:/*升级测试卡测试成功*/
            OS_PRINTF("\n[YXSB][%s %d] 升级测试卡测试成功 \n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_TESTSUCC;
            break;
            
        case CDCA_MESSAGE_NOCALIBOPER_TYPE:/*卡中不存在移植库定制运营商*/
            OS_PRINTF("\n[YXSB][%s %d] 卡中不存在移植库定制运营商\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_NOCALIBOPER;
            break;
            
        case CDCA_MESSAGE_STBLOCKED_TYPE:/* 请重启机顶盒 */
            OS_PRINTF("\n[YXSB][%s %d]请重启机顶盒\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_STBLOCKED;
            break;
            
        case CDCA_MESSAGE_STBFREEZE_TYPE:/* 机顶盒被冻结 */
            OS_PRINTF("\n[YXSB][%s %d]  机顶盒被冻结\n", __FUNCTION__, __LINE__);
            event = CAS_E_CARD_STBFREEZE;
            break;
            
        default:
            event = CAS_E_UNKNOW_ERR;
            OS_PRINTF("CA unknow type:0x%02x\n", byMessageType);
            break;
      }

    CDCAS_SendEvent(event, wEcmPID);
}

void CDSTBCA_ShowFingerInfo ( CDCA_U16 wEcmPID,
                         const SCDCAFingerInfo* pFingerInfo )
{
    OS_PRINTF("[YXSB][%s %d] in!\n", __FUNCTION__, __LINE__);
    SCDCAFingerInfo* p_finger_info = NULL;

    if(pFingerInfo == NULL)
        YXSB_DRV_DEBUG("pFingerInfo is NULL\n");
    if(pFingerInfo->bIsVisible)
        YXSB_DRV_DEBUG("pFingerInfo->bIsVisible = %d\n", pFingerInfo->bIsVisible);
    if(pFingerInfo->szContent == NULL)
        YXSB_DRV_DEBUG("pFingerInfo->szContent == NULL");

    if(pFingerInfo == NULL || pFingerInfo->bIsVisible || pFingerInfo->szContent == NULL)
    {
        CDCAS_SendEvent(CAS_C_HIDE_NEW_FINGER, wEcmPID);
        return;
    }

    p_finger_info = (SCDCAFingerInfo *)mtos_malloc(sizeof(SCDCAFingerInfo));
    MT_ASSERT(p_finger_info != NULL);
    memcpy(p_finger_info, pFingerInfo, sizeof(SCDCAFingerInfo));

    YXSB_STB_Drv_CaReqLock(&FingerInfo_lock);
    memset(&FingerInfo, 0x00, sizeof(FingerInfo));

    FingerInfo.show_postion_x = p_finger_info->byX_position;
    FingerInfo.show_postion_y = p_finger_info->byY_position;

    memcpy(FingerInfo.data, p_finger_info->szContent, strlen(p_finger_info->szContent));
    FingerInfo.data_len = strlen(p_finger_info->szContent);
    YXSB_DRV_DEBUG("[YXSB] data_len = %d, %d!\n", FingerInfo.data_len, strlen(p_finger_info->szContent));
    FingerInfo.finger_property.m_dwFontARGB = p_finger_info->dwFontColor;
    FingerInfo.finger_property.m_dwBackgroundARGB = p_finger_info->dwBackgroundColor;
    FingerInfo.show_front_size = p_finger_info->byFontSize;
    FingerInfo.bIsVisible = p_finger_info->bIsVisible;
    FingerInfo.byType = p_finger_info->byType;

    mtos_free(p_finger_info);

    YXSB_DRV_DEBUG("[YXSB] dwFontColor: 0x%02x!\n", FingerInfo.finger_property.m_dwFontARGB);
    YXSB_DRV_DEBUG("[YXSB] dwBackgroundColor: 0x%02x!\n", FingerInfo.finger_property.m_dwBackgroundARGB);
    YXSB_DRV_DEBUG("[YXSB] show_postion_X: %d!\n", FingerInfo.show_postion_x);
    YXSB_DRV_DEBUG("[YXSB] show_postion_Y: %d!\n", FingerInfo.show_postion_y);
    YXSB_DRV_DEBUG("[YXSB] bIsVisible: %d!\n", FingerInfo.bIsVisible);
    YXSB_DRV_DEBUG("[YXSB] byType: %d!\n", FingerInfo.byType);
    YXSB_DRV_DEBUG("[YXSB] show_front_size: %d!\n", FingerInfo.show_front_size);
    YXSB_DRV_DEBUG("[YXSB] data: %s!\n", FingerInfo.data);

    YXSB_STB_Drv_CaReqUnLock(&FingerInfo_lock);

    CDCAS_SendEvent(CAS_C_SHOW_NEW_FINGER, (u32)&FingerInfo);
    
}


void  CDSTBCA_ShowCurtainNotify( CDCA_U16 wEcmPID,
                                    CDCA_U16  wCurtainCode)
{
    u32 event;
    u32 CurtainCode = 0;
    YXSB_DRV_DEBUG("CDSTBCA_ShowCurtainNotify, wEcmPID = %d, wCurtainCode = %d\n", wEcmPID, wCurtainCode);
    
    switch(wCurtainCode)
    {
        case CDCA_CURTAIN_CANCLE:
            event = CAS_E_CARD_CURTAIN_CANCLE;
            break;
        case CDCA_CURTAIN_OK:
        case CDCA_CURTAIN_TOTTIME_ERROR:
        case CDCA_CURTAIN_WATCHTIME_ERROR:
        case CDCA_CURTAIN_TOTCNT_ERROR:
        case CDCA_CURTAIN_ROOM_ERROR:
        case CDCA_CURTAIN_PARAM_ERROR:
        case CDCA_CURTAIN_TIME_ERROR:
            event = CAS_E_CARD_CURTAIN_OK;
            break;
        default:
            OS_PRINTF("[YXSB][%s %d]CDSTBCA_ShowCurtainNotify not known\n", __FUNCTION__, __LINE__);
            return;
    }

    CurtainCode = (wEcmPID << 16) |  wCurtainCode;

    CDCAS_SendEvent(event, CurtainCode);
}


void CDSTBCA_ShowProgressStrip( CDCA_U8 byProgress,
                                       CDCA_U8 byMark )
{
    u32 ProgressStripPara = 0;

    ProgressStripPara = (byMark << 8) | byProgress;
    YXSB_DRV_DEBUG("CDSTBCA_ShowProgressStrip, byProgress = %d, byMark = %d, ProgressStripPara = %d\n", 
                                        byProgress, byMark, ProgressStripPara);
    
    CDCAS_SendEvent(CAS_C_CARD_UPDATE_PROGRESS, ProgressStripPara);
}

void  CDSTBCA_ActionRequest( CDCA_U16 wTVSID,
                                    CDCA_U8  byActionType )
{
    
}

void CDSTBCA_ShowFingerMessage( CDCA_U16 wEcmPID,
                                       CDCA_U32 dwCardID )
{
    
}


CDCA_U16 CDSTBCA_Strlen(const char* pString )
{
    return strlen(pString);
}

void CDSTBCA_Printf(CDCA_U8 byLevel, const char* szMesssage )
{
    OS_PRINTF("debug level[0x%x], %s\n", byLevel, szMesssage);
}

void YXSB_STB_ReleaseOncePrivateDataFilter( void )
{
    u32 ret, uIndex;
    //YXSB_DRV_DEBUG("[YXSB]YXSB_STB_ReleasePrivateDataFilter start\n");
    //YXSB_DRV_DEBUG("[YXSB]byReqID = %d, wPid = %d\n", byReqID, wPid);

    YXSB_STB_Drv_CaReqLock(&yxsb_filter_lock );
    for(uIndex = 0;uIndex < YXSB_FILTER_MAX_NUM; uIndex++)
    {
        if(yxsb_filter[uIndex].once_filter_flag && (yxsb_filter[uIndex].pid != 0x10))
        {
            ret = _yxsb_filter_free(&yxsb_filter[uIndex]);
            if(ret != SUCCESS)
            {
                //YXSB_DRV_DEBUG("[DS]%s:LINE:%d, Free filter failed!!!!\n", __FUNCTION__, __LINE__);
                YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
                return;
            }
            memset((void *)&yxsb_filter[uIndex], 0, sizeof(s_yxsb_filter_struct_t));
            yxsb_filter[uIndex].filter_status = YXSB_FILTER_STATUS_FREE;
         }
    }
    YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );

    dmx_device_t* p_dev = NULL;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    if (v_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, v_channel, FALSE);
        v_channel = 0xffff;
    }

    if (a_channel != 0xffff)
    {
        ret = dmx_descrambler_onoff(p_dev, a_channel, FALSE);
        a_channel = 0xffff;
    }
}


static void YXSB_STB_ReleasePrivateDataFilter( CDCA_U8  byReqID, CDCA_U16 wPid )
{
    u32 ret, uIndex;
    //YXSB_DRV_DEBUG("[YXSB]YXSB_STB_ReleasePrivateDataFilter start\n");
    //YXSB_DRV_DEBUG("[YXSB]byReqID = %d, wPid = %d\n", byReqID, wPid);

    for(uIndex = 0;uIndex < YXSB_FILTER_MAX_NUM; uIndex++)
    {
        if((yxsb_filter[uIndex].pid == wPid) && (yxsb_filter[uIndex].req_id == byReqID))
        {
            ret = _yxsb_filter_free(&yxsb_filter[uIndex]);
            if(ret != SUCCESS)
            {
                //YXSB_DRV_DEBUG("[DS]%s:LINE:%d, Free filter failed!!!!\n", __FUNCTION__, __LINE__);
                return;
            }
            memset((void *)&yxsb_filter[uIndex], 0, sizeof(s_yxsb_filter_struct_t));
            yxsb_filter[uIndex].filter_status = YXSB_FILTER_STATUS_FREE;
         }
    }
}

static void YXSB_STB_Drv_SendDataToCA(u8 uDataNum)
{
    u8 uIndex = 0;

    if(uIndex > YXSB_FILTER_MAX_NUM)
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, Too much data, uIndex = %d\n", __FUNCTION__, __LINE__, uIndex);
        return;
    }

    for (uIndex = 0; uIndex < uDataNum; uIndex ++)
    {
        //YXSB_DRV_DEBUG("%s:LINE:%d, =====Send data to CA: uIndex=%d,yxsb_data[uIndex].req_id=%d,yxsb_data[uIndex].pid=0x%x yxsb_data[uIndex].p_data[0]=0x%x \n",
        //        __FUNCTION__, __LINE__, uIndex, yxsb_data[uIndex].req_id, yxsb_data[uIndex].pid, yxsb_data[uIndex].p_data[0]);

        CDCASTB_PrivateDataGot((CDCA_U8)yxsb_data[uIndex].req_id, 
                                                     (CDCA_BOOL)yxsb_data[uIndex].timeout,
                                                     (CDCA_U16)yxsb_data[uIndex].pid,
                                                     (const CDCA_U8 *)yxsb_data[uIndex].p_data,
                                                     (CDCA_U16)yxsb_data[uIndex].length);
    }

    if (NULL != yxsb_data[uIndex].p_data)
    {
        yxsb_data[uIndex].p_data = NULL;
    }
    memset(&yxsb_data[uIndex], 0, sizeof(s_yxsb_data_got_t));
}

static void YXSB_STB_Drv_CopyData(s_yxsb_filter_struct_t *p_filter_struct, u8 timeout, u16 length, u8 uIndex)
{
    if(uIndex >= YXSB_FILTER_MAX_NUM)
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, Too much data, uIndex = %d\n", __FUNCTION__, __LINE__, uIndex);
        return;
    }

    if(p_filter_struct->p_buf != NULL)
    {
        YXSB_DRV_DEBUG("%s:LINE:%d, req_id = %d, table_id = 0x%x, pid = 0x%x\n", __FUNCTION__, __LINE__, p_filter_struct->req_id, p_filter_struct->p_buf[0], p_filter_struct->pid);
    }
    yxsb_data[uIndex].req_id = p_filter_struct->req_id;
    yxsb_data[uIndex].timeout = timeout;
    yxsb_data[uIndex].pid = p_filter_struct->pid;

    if ((NULL == p_filter_struct->p_buf) || (0 == length))
    {
        yxsb_data[uIndex].p_data = NULL;
    }
    else
    {
        yxsb_data[uIndex].p_data = yxsb_data_buf[uIndex];
        memcpy(yxsb_data[uIndex].p_data, p_filter_struct->p_buf, length);
    }
    yxsb_data[uIndex].length = length;
}


void YXSB_STB_Drv_CaDataMonitor(void *pData)
{
    u8 *p_GotData = NULL;
    u8 uIndex = 0;
    u32 uDataLen = 0;
    u8 uDataNum = 0;
    s32 nRet = SUCCESS;
    u8 aFilterIndex[YXSB_FILTER_MAX_NUM] = {0};
    u8 uValidFilterNum = 0;
    s_yxsb_filter_struct_t *p_yxsb_filter_struct = NULL;
    dmx_device_t *p_dev = NULL;

    //YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    while(1)
    {
        uValidFilterNum = 0;
        memset(aFilterIndex, 0, sizeof(aFilterIndex));

        YXSB_STB_Drv_CaReqLock(&yxsb_filter_lock );

        /* find valid filter */
        for(uIndex = 0; uIndex < YXSB_FILTER_MAX_NUM; uIndex++)
        {
            if (YXSB_FILTER_STATUS_BUSY == yxsb_filter[uIndex].filter_status)
            {
                aFilterIndex[uValidFilterNum] = uIndex;
                uValidFilterNum ++;
            }
        }

        /* no valid filter */
        if(uValidFilterNum == 0)
        {
            YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
            mtos_task_sleep(20);
            continue;
        }

        /* read data*/
        uDataNum = 0;
        for (uIndex = 0; uIndex < uValidFilterNum; uIndex++)
        {
            if(YXSB_FILTER_STATUS_BUSY == yxsb_filter[aFilterIndex[uIndex]].filter_status)
            {
                nRet = dmx_si_chan_get_data(p_dev, yxsb_filter[aFilterIndex[uIndex]].req_handle,
                            &p_GotData, &uDataLen);

                if (nRet != SUCCESS)        /* no data and time out */
                {
                    if (mtos_ticks_get() - yxsb_filter[aFilterIndex[uIndex]].start_ms
                            > yxsb_filter[aFilterIndex[uIndex]].req_timeout)
                    {
                        YXSB_DRV_DEBUG("%s:LINE:%d, ########data time out!\n", __FUNCTION__, __LINE__);
                        p_yxsb_filter_struct = &yxsb_filter[aFilterIndex[uIndex]];
                        p_yxsb_filter_struct->p_buf = NULL;
                        YXSB_STB_Drv_CopyData(p_yxsb_filter_struct, CDCA_TRUE, 0, uDataNum);
                        uDataNum++;

                        /*free ECM filter every time*/
                        if(yxsb_filter[aFilterIndex[uIndex]].once_filter_flag)
                        {
                            YXSB_STB_ReleasePrivateDataFilter(yxsb_filter[aFilterIndex[uIndex]].req_id, 
                                                                                         yxsb_filter[aFilterIndex[uIndex]].pid);
                        }
                    }
                }
                else         /* get data */
                {
                    p_yxsb_filter_struct = &yxsb_filter[aFilterIndex[uIndex]];
                    p_yxsb_filter_struct->p_buf = p_GotData;
                    YXSB_STB_Drv_CopyData(p_yxsb_filter_struct, CDCA_FALSE, uDataLen, uDataNum);
                    uDataNum++;

                    /*free ECM filter every time*/
                    if(yxsb_filter[aFilterIndex[uIndex]].once_filter_flag)
                    {
                        YXSB_STB_ReleasePrivateDataFilter(yxsb_filter[aFilterIndex[uIndex]].req_id, 
                                                                                     yxsb_filter[aFilterIndex[uIndex]].pid);
                    }
                }
            }
        }
        YXSB_STB_Drv_CaReqUnLock(&yxsb_filter_lock );
        
        YXSB_STB_Drv_SendDataToCA(uDataNum);
        mtos_task_sleep(10);
    }
}

RET_CODE YXSB_STB_Drv_AdtInit(void)
{
    u32 *p_stack = NULL;
    s_cas_yxsb_priv_t *p_priv = NULL;
    RET_CODE ret = 0;
    u8 task_name[] = "YXSB_Data_Monitor";

    p_g_cas_priv = cas_get_private_data();
    p_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    YXSB_DRV_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    MT_ASSERT(TRUE == mtos_sem_create(&yxsb_filter_lock, TRUE));
    OS_PRINTF("[YXSB][%s] %d, yxsb_filter_lock = %d, &yxsb_filter_lock = %x!\n", __FUNCTION__, __LINE__, yxsb_filter_lock, &yxsb_filter_lock);   
    MT_ASSERT(TRUE == mtos_sem_create(&OsdMsg_lock, TRUE));
    MT_ASSERT(TRUE == mtos_sem_create(&IppvBuyInfo_lock, TRUE));
    MT_ASSERT(TRUE == mtos_sem_create(&ForceChannelInfo_lock, TRUE));
    MT_ASSERT(TRUE == mtos_sem_create(&FingerInfo_lock, TRUE));
    
    p_stack = (u32 *)mtos_malloc(YXSB_SECTION_TASK_STACK_SIZE);
    MT_ASSERT(NULL != p_stack);

    ret = mtos_task_create(task_name,
                                            YXSB_STB_Drv_CaDataMonitor,
                                            (void *)0,
                                            YXSB_CAS_SECTION_TASK_PRIO,
                                            p_stack,
                                            YXSB_SECTION_TASK_STACK_SIZE);

    if(ret == FALSE)
    {
        YXSB_DRV_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!\n", __FUNCTION__, __LINE__);
        return ERR_FAILURE;
    }

    YXSB_DRV_DEBUG("[YXSB]YXSB_STB_Drv_AdtInit OK!\n");
    return SUCCESS;
}

