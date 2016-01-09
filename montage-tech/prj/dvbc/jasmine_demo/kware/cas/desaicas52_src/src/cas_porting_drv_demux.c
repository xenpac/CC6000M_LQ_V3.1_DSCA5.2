/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "dmx.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "cas_porting_drv_demux.h"


static s_cas_filter_struct_t cas_filter[CAS_FILTER_MAX_NUM] = {{CAS_FILTER_STATUS_FREE,0,0,0,{0,},{0,},0,0,0,0,NULL},};

static u8 cas_filter_buf[CAS_FILTER_MAX_NUM][CAS_FILTER_BUFFER_SIZE] = {{0,},};

s_cas_data_got_t cas_data[CAS_FILTER_MAX_NUM] = {{0,},};
static u8 cas_data_buf[CAS_FILTER_MAX_NUM][CAS_DATA_BUFFER_SIZE] = {{0,},};

static os_sem_t cas_filter_lock = 0;

void (*pCaDataCallbackFun)(u8);



void cas_porting_drv_dmx_init(u32 monitor_pri,void (*pFun)(u8))
{
    u32 *p_stack = NULL;
    u8 task_name[] = "CAS_Data_Monitor";
    //RET_CODE ret = 0;
    
    MT_ASSERT(TRUE == mtos_sem_create((os_sem_t *)&cas_filter_lock, TRUE));
    
    memset((void *)&cas_filter[0], 0, sizeof(s_cas_filter_struct_t) * CAS_FILTER_MAX_NUM);
    
    p_stack = (u32 *)mtos_malloc(CAS_SECTION_TASK_STACK_SIZE);
    MT_ASSERT(NULL != p_stack);

    //ret = mtos_task_create(task_name,
    mtos_task_create(task_name,
                                            cas_porting_drv_data_monitor,
                                            (void *)0,
                                            monitor_pri,
                                            p_stack,
                                            CAS_SECTION_TASK_STACK_SIZE);
    pCaDataCallbackFun = pFun;
}

RET_CODE cas_porting_drv_dmx_filter_free(s_cas_filter_struct_t *filter)
{
    s32 ret = ERR_FAILURE;
    dmx_device_t *p_dev = NULL;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_chan_stop(p_dev, filter->req_handle);
    MT_ASSERT(ret == SUCCESS);

    ret = dmx_chan_close(p_dev, filter->req_handle);
    MT_ASSERT(ret == SUCCESS);

    memset((void *)filter, 0x00, sizeof(s_cas_filter_struct_t));

    if(ret == SUCCESS){

    }
	
    return SUCCESS;
}

void cas_porting_drv_dmx_release_filter( u8  byReqID, u16 wPid )
{
    RET_CODE ret = ERR_FAILURE;
    u32 uIndex;
    dmx_device_t *p_dev = NULL;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);
    
    mtos_sem_take((os_sem_t *)&cas_filter_lock, 0);
    for(uIndex = 0;uIndex < CAS_FILTER_MAX_NUM; uIndex++)
    {
        if((cas_filter[uIndex].pid == wPid) && (cas_filter[uIndex].req_id == byReqID))
        {
              ret = dmx_chan_stop(p_dev, cas_filter[uIndex].req_handle);
              MT_ASSERT(ret == SUCCESS);

              ret = dmx_chan_close(p_dev, cas_filter[uIndex].req_handle);
              MT_ASSERT(ret == SUCCESS);

              memset((void *)&cas_filter[uIndex], 0x00, sizeof(s_cas_filter_struct_t));
         }
    }
    mtos_sem_give((os_sem_t *)&cas_filter_lock);
	
    if(ret == SUCCESS){

    }
}


void cas_porting_drv_dmx_release_once_filter( void )
{
    RET_CODE ret = ERR_FAILURE;
    u32 uIndex;
    
    dmx_device_t *p_dev = NULL;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    mtos_sem_take((os_sem_t *)&cas_filter_lock, 0);
    for(uIndex = 0;uIndex < CAS_FILTER_MAX_NUM; uIndex++)
    {
        if(cas_filter[uIndex].once_filter_flag)
        {
              ret = dmx_chan_stop(p_dev, cas_filter[uIndex].req_handle);
              MT_ASSERT(ret == SUCCESS);

              ret = dmx_chan_close(p_dev, cas_filter[uIndex].req_handle);
              MT_ASSERT(ret == SUCCESS);

              memset((void *)&cas_filter[uIndex], 0x00, sizeof(s_cas_filter_struct_t));
         }
    }
    mtos_sem_give((os_sem_t *)&cas_filter_lock);
/*
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
*/    
    if(ret == SUCCESS){

    }
}

RET_CODE cas_porting_drv_dmx_set_filter( u8        byReqID,  
											   const u8* pbyFilter,  
											   const u8* pbyMask, 
											   u8        byLen, 
											   u16       wPid, 
											   u8        byWaitSeconds,
											   u8        once_filter_flag)
{
    u8 index = 0;
    RET_CODE ret = ERR_FAILURE;
    dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;

    MT_ASSERT(pbyFilter != NULL);
    MT_ASSERT(pbyMask != NULL);

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);
    
    mtos_sem_take((os_sem_t *)&cas_filter_lock, 0);
    for (index = 0; index < CAS_FILTER_MAX_NUM; index ++)
    {
        if ((CAS_FILTER_STATUS_BUSY == cas_filter[index].filter_status)
                && (byReqID == cas_filter[index].req_id) && ((byReqID & 0x80) != 0x80))
        {
            ret = dmx_chan_stop(p_dev, cas_filter[index].req_handle);
            MT_ASSERT(ret == SUCCESS);

            ret = dmx_chan_close(p_dev, cas_filter[index].req_handle);
            MT_ASSERT(ret == SUCCESS);

            memset((void *)&cas_filter[index], 0x00, sizeof(s_cas_filter_struct_t));
        }
    }

    for(index = 0; index < CAS_FILTER_MAX_NUM; index++)
    {
        if(CAS_FILTER_STATUS_FREE == cas_filter[index].filter_status)
            break;
    }

    if(index >= CAS_FILTER_MAX_NUM)
    {
        OS_PRINTF("[CAS][%s] %d, Filter is full!\n", __FUNCTION__, __LINE__);
        mtos_sem_give((os_sem_t *)&cas_filter_lock);
        return ERR_FAILURE;
    }

    p_slot.in = DMX_INPUT_EXTERN0;
    p_slot.pid = wPid;
    p_slot.type = DMX_CH_TYPE_SECTION;

    ret = dmx_si_chan_open(p_dev, &p_slot, &cas_filter[index].req_handle);
    if (SUCCESS != ret)
    {
        OS_PRINTF("[CAS]%s:LINE:%d, open chan failed, ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        mtos_sem_give((os_sem_t *)&cas_filter_lock);
        return ERR_FAILURE;
    }

    cas_filter[index].p_buf = cas_filter_buf[index];

    ret = dmx_si_chan_set_buffer(p_dev, cas_filter[index].req_handle, cas_filter[index].p_buf, CAS_FILTER_BUFFER_SIZE);
    if (SUCCESS != ret)
    {
        OS_PRINTF("[CAS]%s:LINE:%d, chan set buffer failed! ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        mtos_sem_give((os_sem_t *)&cas_filter_lock);
        return ERR_FAILURE;
    }
    if(once_filter_flag == 1)
    {
      p_param.continuous = FALSE;
      //OS_PRINTF("dmx chan once_filter_flag\n");
    }
    else
    {
      p_param.continuous = TRUE;
    }
    p_param.en_crc = FALSE;
    memset(p_param.mask, 0, DMX_SECTION_FILTER_SIZE);
    memset(p_param.value, 0, DMX_SECTION_FILTER_SIZE);
    memcpy(&p_param.mask[0], &pbyMask[0], byLen);
    memcpy(&p_param.value[0], &pbyFilter[0], byLen);

    ret = dmx_si_chan_set_filter(p_dev, cas_filter[index].req_handle, &p_param);
    if (SUCCESS != ret)
    {
        OS_PRINTF("[CAS]set filter failed!!!!!!!!!\n");
        mtos_sem_give((os_sem_t *)&cas_filter_lock);
        return ERR_FAILURE;
    }

    ret = dmx_chan_start(p_dev, cas_filter[index].req_handle);

    if (SUCCESS != ret)
    {
        OS_PRINTF("[CAS]%s:LINE:%d, chan start failed! ret=0x%08x!!!!\n", __FUNCTION__, __LINE__, ret);
        mtos_sem_give((os_sem_t *)&cas_filter_lock);
        return ERR_FAILURE;
    }

    if (0 == byWaitSeconds)
    {
        cas_filter[index].req_timeout = 0xffffffff;
    }
    else
    {
        cas_filter[index].req_timeout = byWaitSeconds * 1000;
    }
    cas_filter[index].filter_status = CAS_FILTER_STATUS_BUSY;
    if(once_filter_flag)
        cas_filter[index].once_filter_flag = 1;
    cas_filter[index].req_id = byReqID;
    cas_filter[index].pid = wPid;
    memcpy(cas_filter[byReqID].filter_data, pbyFilter, byLen);
    memcpy(cas_filter[byReqID].filter_mask, pbyMask, byLen);
    cas_filter[byReqID].mask_len = byLen;
    cas_filter[index].start_ms = mtos_ticks_get() * 10;
    
   // OS_PRINTF("[CAS]%s:LINE:%d, ===filter data:byReqID=%d,cas_filter[%d].req_id=%d,cas_filter[%d].pid=0x%x!\n",
           // __FUNCTION__, __LINE__, byReqID, index, cas_filter[index].req_id, index, cas_filter[index].pid);
    mtos_sem_give((os_sem_t *)&cas_filter_lock);
    return SUCCESS;
}




static void cas_porting_drv_copydata(s_cas_filter_struct_t *p_filter_struct, u8 timeout, u16 length, u8 uIndex)
{
    if(uIndex >= CAS_FILTER_MAX_NUM)
    {
        OS_PRINTF("%s:LINE:%d, Too much data, uIndex = %d\n", __FUNCTION__, __LINE__, uIndex);
        return;
    }

    //OS_PRINTF("%s:LINE:%d, req_id = %d, table_id = 0x%x, pid = 0x%x\n", __FUNCTION__, __LINE__, p_filter_struct->req_id, p_filter_struct->p_buf[0], p_filter_struct->pid);
    cas_data[uIndex].req_id = p_filter_struct->req_id;
    cas_data[uIndex].timeout = timeout;
    cas_data[uIndex].pid = p_filter_struct->pid;

    if ((NULL == p_filter_struct->p_buf) || (0 == length))
    {
        cas_data[uIndex].p_data = NULL;
    }
    else
    {
        cas_data[uIndex].p_data = cas_data_buf[uIndex];
        memcpy(cas_data[uIndex].p_data, p_filter_struct->p_buf, length);
    }
    cas_data[uIndex].length = length;
}

void cas_porting_drv_data_monitor(void *pData)
{
    u8 *p_GotData = NULL;
    u8 uIndex = 0;
    u32 uDataLen = 0;
    u8 uDataNum = 0;
    s32 nRet = SUCCESS;
    u8 aFilterIndex[CAS_FILTER_MAX_NUM] = {0};
    u8 uValidFilterNum = 0;
    s_cas_filter_struct_t *p_filter_struct = NULL;
    RET_CODE ret = SUCCESS;
    dmx_device_t *p_dev = NULL;

    p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    //while(1)
    for(;;)
    {
   //     OS_PRINTF("$");
        uValidFilterNum = 0;
        memset(aFilterIndex, 0, sizeof(aFilterIndex));

        mtos_sem_take((os_sem_t *)&cas_filter_lock, 0);

        /* find valid filter */
        for(uIndex = 0; uIndex < CAS_FILTER_MAX_NUM; uIndex++)
        {
            if (CAS_FILTER_STATUS_BUSY == cas_filter[uIndex].filter_status)
            {
                aFilterIndex[uValidFilterNum] = uIndex;
                uValidFilterNum ++;
            }
        }

        /* no valid filter */
        if(uValidFilterNum == 0)
        {
            mtos_sem_give((os_sem_t *)&cas_filter_lock);
            mtos_task_sleep(20);
            continue;
        }

        /* read data*/
        uDataNum = 0;
        for (uIndex = 0; uIndex < uValidFilterNum; uIndex++)
        {
            if(CAS_FILTER_STATUS_BUSY == cas_filter[aFilterIndex[uIndex]].filter_status)
            {
                nRet = dmx_si_chan_get_data(p_dev, cas_filter[aFilterIndex[uIndex]].req_handle,
                            &p_GotData, &uDataLen);

                if (nRet != SUCCESS)        /* no data and time out */
                {
                    if(cas_filter[aFilterIndex[uIndex]].req_timeout != 0xffffffff)
                    {
                        if (mtos_ticks_get() * 10 - cas_filter[aFilterIndex[uIndex]].start_ms
                            > cas_filter[aFilterIndex[uIndex]].req_timeout)
                        {
                            p_filter_struct = &cas_filter[aFilterIndex[uIndex]];
                            p_filter_struct->p_buf = NULL;
                            cas_porting_drv_copydata(p_filter_struct, TRUE, 0, uDataNum);
                            uDataNum++;

                            //OS_PRINTF("****time out %d\n",  p_filter_struct->req_id);

                            /*free ECM filter every time*/
                            if(cas_filter[aFilterIndex[uIndex]].once_filter_flag)
                            {

                                ret = dmx_chan_stop(p_dev, cas_filter[aFilterIndex[uIndex]].req_handle);
                                MT_ASSERT(ret == SUCCESS);

                                ret = dmx_chan_close(p_dev, cas_filter[aFilterIndex[uIndex]].req_handle);
                                MT_ASSERT(ret == SUCCESS);

                                memset((void *)&cas_filter[aFilterIndex[uIndex]], 0x00, sizeof(s_cas_filter_struct_t));

                            }
                        }
                    }
                    
                }
                else         /* get data */
                {
                    p_filter_struct = &cas_filter[aFilterIndex[uIndex]];
                    p_filter_struct->p_buf = p_GotData;
                    cas_porting_drv_copydata(p_filter_struct, FALSE, (u16)uDataLen, uDataNum);
                    uDataNum++;

                     //OS_PRINTF("\n***get data%d\n",  p_filter_struct->req_id);

                    /*free ECM filter every time*/
                    if(cas_filter[aFilterIndex[uIndex]].once_filter_flag)
                    {
                            ret = dmx_chan_stop(p_dev, cas_filter[aFilterIndex[uIndex]].req_handle);
                            MT_ASSERT(ret == SUCCESS);

                            ret = dmx_chan_close(p_dev, cas_filter[aFilterIndex[uIndex]].req_handle);
                            MT_ASSERT(ret == SUCCESS);

                            memset((void *)&cas_filter[aFilterIndex[uIndex]], 0x00, sizeof(s_cas_filter_struct_t));
                    }
                }
            }
        }
        mtos_sem_give((os_sem_t *)&cas_filter_lock);
        
        //YXSB_STB_Drv_SendDataToCA(uDataNum);
        (*pCaDataCallbackFun)(uDataNum);
        mtos_task_sleep(10);
    }
 //lint -esym(550,ret)
	
}


RET_CODE cas_porting_drv_dmx_get_chanid_bypid(u16 pid, u16 *p_channel)
{
  dmx_device_t *p_dev = NULL;

  p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev->p_base);
  return dmx_get_chanid_bypid(p_dev, pid, p_channel);
}

RET_CODE cas_porting_drv_dmx_descrambler_onoff(u16 channel, BOOL enable)
{
  dmx_device_t *p_dev = NULL;

  p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev->p_base);
  return dmx_descrambler_onoff(p_dev, channel, enable);
}

RET_CODE cas_porting_drv_dmx_descrambler_set_even_keys(u16 channel, u8 *p_key, s32 key_length)
{
  dmx_device_t *p_dev = NULL;

  p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev->p_base);
  return dmx_descrambler_set_even_keys(p_dev, channel, p_key, key_length);
}

RET_CODE cas_porting_drv_dmx_descrambler_set_odd_keys(u16 channel, u8 *p_key, s32 key_length)
{
  dmx_device_t *p_dev = NULL;

  p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev->p_base);
  return dmx_descrambler_set_odd_keys(p_dev, channel, p_key, key_length);
}

