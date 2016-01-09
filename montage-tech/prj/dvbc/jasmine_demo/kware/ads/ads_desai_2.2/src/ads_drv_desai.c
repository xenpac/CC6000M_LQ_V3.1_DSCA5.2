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
#include "mtos_timer.h"
#include "charsto.h"

#include "mem_manager.h"
#include "nim.h"
#include "dmx.h"
#include "ads_ware.h"
#include "ads_api_desai.h"
#include "ads_desai_include.h"

//#define ADS_DESAI_DRV_PRINTF
//#define ADS_DESAI_DRV_DEBUG
//#define ADS_DESAI_AD_PRINTF

#ifdef ADS_DESAI_DRV_PRINTF
    #define ADS_DRV_PRINTF OS_PRINTF
#else
    #define ADS_DRV_PRINTF DUMMY_PRINTF
#endif

#ifdef ADS_DESAI_DRV_DEBUG
    #define ADS_DRV_DEBUG OS_PRINTF
#else
    #define ADS_DRV_DEBUG OS_PRINTF
#endif

#define DS_ADS_TASK_STKSIZE    (32 * KBYTES)
#define DS_ADS_INVALID_CHANNEL_ID   (0xffff)
#define DS_ADS_DMX_BUF_SIZE  (32 * KBYTES + 188)
#define DS_ADS_FILTER_MAX_NUM    (10)
#define DS_ADS_QUEUE_MAX_DEPTH (32)
#define DS_ADS_TASK_NUM  (10)
#define NIT_PID (0x10)
#define PAT_PID (0x0)
#define NIT_TABLE_ID  (0x40)
#define PAT_TABLE_ID  (0x0)
#define PMT_TABLE_ID  (0x02)

typedef enum
{
    DS_ADS_FREE = 0,
    DS_ADS_USED
}ds_ads_status_t;

typedef struct _s_ds_task_struct
{
    u32 task_prio;
    ds_ads_status_t task_status;
}s_ds_ads_task_struct_t;

static s_ds_ads_task_struct_t ds_ads_task_record[DS_ADS_TASK_NUM] =
{
    {0, DS_ADS_FREE},
    {0, DS_ADS_FREE},
    {0, DS_ADS_FREE},
    {0, DS_ADS_FREE},
    {0, DS_ADS_FREE},
};

#if 1
typedef struct
{
    dmx_device_t          *p_dev;
    EN_DSAD_DATA_ID data_type;
    ds_ads_status_t       filter_status;
    dmx_chanid_t          channel_filter_id;
    u16                         channel_pid;
    u16                         filter_num;
    u8                           filter_data[DSAD_FILTER_MAX_LEN];
    u8                           filter_mask[DSAD_FILTER_MAX_LEN];
    u8                           *p_buf;
    u32                          start_time;
    u32                          timeout;
    void (*callback_func)(u16 pid, u8 *p_data, u16 data_len);
}ds_ads_filter_struct_t;

static ds_ads_filter_struct_t ds_ads_filter_info[DS_ADS_FILTER_MAX_NUM] = {{0,},};
static u8 ds_ads_filter_buf[DS_ADS_FILTER_MAX_NUM][DS_ADS_DMX_BUF_SIZE] = {{0,},};
#endif

u16 g_ads_pid = 0xffff;
os_sem_t g_ads_ds_lock = 0;
//lint -e{64}
ST_DSAD_FILTER_INFO nit_filter_info = {0};
extern ads_desai_priv_t *p_ads_ds_priv;

#if 1
void ds_ad_data_debug(u8 *p_data, u32 data_len)
{

    u32 j = 0;

    if(NULL == p_data || data_len == 0)
    {
        ADS_DRV_DEBUG("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
	{
		ADS_DRV_DEBUG("[drv] data_len:%d \n\n",data_len);
#if 1
		 {
		     while(j < data_len)
		     {
		            ADS_DRV_DEBUG("%02x ",p_data[j]);
		             j++;
		             if(j%0x10 == 0)
		                     ADS_DRV_DEBUG("\n");
		      }
		 }
		ADS_DRV_DEBUG("\n");
	}
}
#endif

void ds_ad_set_pid(u16 pid)
{
OS_PRINTF("ds_ad_set_pid  pid=0x%x\n",pid);
    g_ads_pid = pid;
}

u16 ds_ad_get_pid(void)
{
    return g_ads_pid;
}

static void ds_ads_lock(void)
{
    INT32 ret = FALSE;
    ret = mtos_sem_take((os_sem_t *)&g_ads_ds_lock, 0);

    if (!ret)
    {
        MT_ASSERT(0);
    }
}

static void ds_ads_unlock(void)
{
    INT32 ret = FALSE;

    //OS_PRINTF("[DRV]%s,%d, give semaphare[0x%x] = 0x%x \n",__func__,__LINE__,&g_ads_ds_lock,g_ads_ds_lock);
    ret = mtos_sem_give((os_sem_t *)&g_ads_ds_lock);
    if (!ret)
    {
        MT_ASSERT(0);
    }
}

BOOL ds_ad_nim_lock(u32 freq, u32 symbolrate, u16 qammode)
{
    s32 ret = FALSE;
    //u32 frequency = freq;
    //u32 symbol_rate = symbolrate;
    //u8   mode = qammode;
    nim_device_t *p_nim_dev = NULL;
    nim_channel_info_t channel_info = {0};

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    p_nim_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    if(NULL == p_nim_dev)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    channel_info.frequency = freq;
    channel_info.param.dvbc.symbol_rate = symbolrate;
    channel_info.param.dvbc.modulation = NIM_MODULA_QAM64;
    switch(qammode)
    {
        case 16:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM16;
            break;

        case 32:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM32;
            break;

        case 64:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM64;
            break;

        case 128:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM128;
            break;

        case 256:
            channel_info.param.dvbc.modulation = NIM_MODULA_QAM256;
            break;

        default:
            break;
    }

    ret = nim_channel_connect(p_nim_dev, &channel_info, FALSE);
    
    ADS_DRV_PRINTF("[drv] %s %d ,ret value = %d \n",__func__,__LINE__,ret);
    
    if(channel_info.lock != 1)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        ret = FALSE;
        //MT_ASSERT(0);
    }
    else
    {
      ret = TRUE;
    }
    return ret;
}

BOOL ds_ad_set_filter_nit_table(void)
{
    EN_DSAD_FUNCTION_TYPE ret = DSAD_FUN_ERR_PARA;

    ADS_DRV_PRINTF("[drv] %s %d ! \n",__func__,__LINE__);
    nit_filter_info.enDataId = (EN_DSAD_DATA_ID)0;
    nit_filter_info.usChannelPid = NIT_PID;
    nit_filter_info.ucFilterLen = 1;
    nit_filter_info.aucFilter[0] = NIT_TABLE_ID;
    nit_filter_info.aucMask[0] = 0xff;
    nit_filter_info.ucWaitSeconds = 10;
    nit_filter_info.pfDataNotifyFunction = NULL;

    ret = DS_AD_SetFilter(&nit_filter_info);
    if(ret != DSAD_FUN_OK)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    return TRUE;
}

void ds_ads_parser_nit(u8 *p_data, u32 data_len)
{
    u16 ads_pid = 0;
    s16 length = 0;
    u32 index = 0;
    u8   descriptor_tag = 0;
    u8   descriptor_len = 0;

    if(p_data[0] != NIT_TABLE_ID)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    index = 8;
    length = (s16)(((p_data[index] & 0x0f) << 8) | p_data[index + 1]);
    index += 2;

    while(length > 0)
    {
        descriptor_tag = p_data[index];
        index++;
        length --;

        switch(descriptor_tag)
        {
            case 0xa6:
                descriptor_len = p_data[index];
                index++;
                length--;
                ads_pid = (u16)((p_data[index] << 8) | (p_data[index + 1]));
                ADS_DRV_PRINTF("[drv], ads_pid: 0x%x \n",ads_pid);
                //index++;
                index += descriptor_len;
                length -= descriptor_len;
                break;

            default:
                descriptor_len = p_data[index];
                index++;
                length--;
		  index += descriptor_len;
                length -= descriptor_len;
                break;
        }
    }

    ds_ad_set_pid(ads_pid);
}

void ds_ads_ts_play(u8 *p_data, u32 data_len,u16 v_pid, u16 a_pid, u16 pcr_pid,u8 mode)
{
                   
}

void ds_ads_av_play(u8 *p_data, u32 data_len)
{
    //u8 pmt_pid = 0xff;
    u16 v_pid = 0;
    u16 a_pid = 0;
    u16 pcr_pid =0;

    //play ts
    ds_ads_ts_play(p_data,data_len,v_pid,a_pid,pcr_pid,0);
}

void ds_ads_filter_free(ds_ads_filter_struct_t *ds_ads_filter)
{
    RET_CODE ret = 0;

    ret = dmx_chan_stop(ds_ads_filter->p_dev, ds_ads_filter->channel_filter_id);
    MT_ASSERT(ret == SUCCESS);

    ret = dmx_chan_close(ds_ads_filter->p_dev, ds_ads_filter->channel_filter_id);
    MT_ASSERT(ret == SUCCESS);
}

void ds_ad_info_parser(ST_DSAD_PROGRAM_SHOW_INFO *p_show_info)
{
    u8 *p_data = NULL;
    ads_ad_type_t ad_type = ADS_AD_TYPE_NUM;

    if(NULL == p_show_info)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
    else
    {
        if(p_show_info->aucFlag[0] != 0)
        {
            OS_PRINTF("pos type: %d \n",p_show_info->stPictureShow.enPosType);
            OS_PRINTF("pic type : %d \n",p_show_info->stPictureShow.enPicType);
            OS_PRINTF("X          : %d \n",p_show_info->stPictureShow.usStartX);
            OS_PRINTF("Y          : %d \n",p_show_info->stPictureShow.usStartY);
            OS_PRINTF("width    : %d \n",p_show_info->stPictureShow.usWidth);
            OS_PRINTF("height   : %d \n",p_show_info->stPictureShow.usHeigth);
            OS_PRINTF("size      : %d \n",p_show_info->stPictureShow.uiDataLen);
            OS_PRINTF("data     : %x \n",p_show_info->stPictureShow.pucPicData[0]);
            OS_PRINTF("data   addr  : 0x%x \n",p_show_info->stPictureShow.pucPicData);
            
            ad_type = ADS_AD_TYPE_PIC;

            #ifdef ADS_DESAI_DRV_DEBUG
               ds_ad_data_debug(p_show_info->stPictureShow.pucPicData,
                                               p_show_info->stPictureShow.uiDataLen);
            #else
               p_data = (u8 *)&p_show_info->stPictureShow;
                  p_ads_ds_priv->notify_msg_to_ui(TRUE, 0, ad_type, (u32)p_data);
            #endif
        }
         
        if(p_show_info->aucFlag[1] != 0)
        {
            ADS_DRV_DEBUG("pos type: %d \n",p_show_info->stAvPlay.enPosType);
            ADS_DRV_DEBUG("av  type : %d \n",p_show_info->stAvPlay.enAvType);
            ADS_DRV_DEBUG("X          : %d \n",p_show_info->stAvPlay.usStartX);
            ADS_DRV_DEBUG("Y          : %d \n",p_show_info->stAvPlay.usStartY);
            ADS_DRV_DEBUG("width    : %d \n",p_show_info->stAvPlay.usWidth);
            ADS_DRV_DEBUG("height   : %d \n",p_show_info->stAvPlay.usHeigth);
            ADS_DRV_DEBUG("size      : %d \n",p_show_info->stAvPlay.uiDataLen);
            ADS_DRV_DEBUG("data     : %x \n",p_show_info->stAvPlay.pucAvData[0]);

            ad_type = ADS_AD_TYPE_AV;

            #ifdef ADS_DESAI_DRV_DEBUG
               ds_ad_data_debug(p_show_info->stAvPlay.pucAvData,
                                               p_show_info->stAvPlay.uiDataLen);
            #else
               p_data = (u8 *)&p_show_info->stAvPlay;
               p_ads_ds_priv->notify_msg_to_ui(TRUE, 0, ad_type, (u32)p_data);
            #endif
        }
    }
 
}

void ds_ad_osd_info_parser(ST_DSAD_OSD_PROGRAM_SHOW_INFO *p_osd_show)
{
    u8 *p_data = NULL;
    p_data = (u8 *)p_osd_show;
    p_ads_ds_priv->notify_msg_to_ui(FALSE, 0, ADS_AD_TYPE_OSD, (u32)p_data);
}

BOOL DS_AD_Flash_Read(UINT32 puiStartAddr,  UINT8 *pucData,  UINT32 uiLen)
{
    RET_CODE ret = FALSE;
    u32 offset = puiStartAddr;
    u8 *p_data = pucData;
    u32 size = uiLen;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    if(NULL == pucData)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    if(NULL != p_ads_ds_priv->nvram_read&& size != 0)
    {
#if 1
        ret = p_ads_ds_priv->nvram_read(offset, p_data, &size);
        if(TRUE != ret)
        {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
            MT_ASSERT(0);
        }
#else
        u8 *src = offset + g_temp_nvram;
        memcpy(p_data,src,size);
        ret = TRUE;
#endif
    }
    return ret;
}


BOOL DS_AD_Flash_Write(UINT32 puiStartAddr,  UINT8 *pucData,  UINT32 uiLen)
{
    RET_CODE ret = FALSE;
    u32 offset = puiStartAddr;
    u8 *p_data = pucData;
    u32 size = uiLen;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    if(NULL == pucData)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    ADS_DRV_PRINTF("[drv], write addr=0x%x, len=%d \n",puiStartAddr,uiLen);

    if(NULL != p_ads_ds_priv->nvram_write&& size != 0)
    {
 #if 1
        ret = p_ads_ds_priv->nvram_erase2(offset,  size);
        ret = p_ads_ds_priv->nvram_write(offset, p_data, size);
        if(TRUE != ret)
        {
            ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
            MT_ASSERT(0);
        }
 #else
        u8 *src = offset + g_temp_nvram;
        memcpy(src,p_data,size);
        ret = TRUE;
 #endif
    }
    return ret;
}

BOOL DS_AD_Flash_Erase(UINT32 puiStartAddr,  UINT32 uiLen)
{
    RET_CODE ret = FALSE;
    u32 size = uiLen;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    /*注意我们现在我们现在是将整个block擦除
       如果有问题，需要修改擦除函数增加地址*/
    if(NULL != p_ads_ds_priv->nvram_erase2 && size != 0)
    {
        ADS_DRV_PRINTF("[drv], erase addr=0x%x, len=%d \n",puiStartAddr,uiLen);
#if 1
        ret = p_ads_ds_priv->nvram_erase2(puiStartAddr,size);
        if(TRUE != ret)
        {
            ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
            MT_ASSERT(0);
        }
#else
#endif
    }
    return ret;
}

void* DSAD_Malloc(UINT32 uiBufferSize)
{
    void *p_buf = 0;
    u32 size = uiBufferSize;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
   p_buf = (void *)mtos_malloc(size);
   MT_ASSERT(NULL != p_buf);
   memset(p_buf, 0, size);

   return p_buf;
}

void DSAD_Free(IN void* pucBuffer)
{
    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pucBuffer)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
    mtos_free(pucBuffer);
    pucBuffer = NULL;
	//lint -e{438}
}

void DSAD_Memset(IN void* pucBuffer, UINT8 ucValue, UINT32 uiSize)
{
    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    u8 *p = NULL;
    u8 *q = NULL;
    u8  b_value = 0;
    u8  a_value = 0;

    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    if(NULL == pucBuffer)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
	{
	    p = (u8 *)pucBuffer + uiSize;
	    b_value = *p;
	    //OS_PRINTF("memset, beofre = 0x%x \n",b_value);
	    memset(pucBuffer, ucValue, uiSize);
	    q = (u8 *)pucBuffer + uiSize;
	    a_value = *q;
	    //OS_PRINTF("memset, after = 0x%x \n",a_value);
	    if(b_value != a_value)
	    {
	    	OS_PRINTF("pucBuffer = 0x%x \n",pucBuffer);
			MT_ASSERT(0);
	    }
	}
}

void DSAD_Memcpy(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize)
{
    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    u8 *p = NULL;
    u8 *q = NULL;
    u8  b_value = 0;
    u8  a_value = 0;

    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    if(NULL == pucDestBuffer || NULL == pucSourceBuffer)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
	{
	    p = (u8 *)pucDestBuffer + uiSize;
	    //OS_PRINTF("memcpy, pucDestBuffer[0x%x] , p= 0x%x \n",pucDestBuffer,p);
	    //OS_PRINTF("memcpy1, 0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x \n",*(u8 *)pucDestBuffer,*(u8 *)(pucDestBuffer+1),
	    //*(u8 *)(pucDestBuffer+2),*(u8 *)(pucDestBuffer+3),*(u8 *)(pucDestBuffer+4),*(u8 *)(pucDestBuffer+5),*(u8 *)(pucDestBuffer+6),*(u8 *)(pucDestBuffer+7));
	    b_value = *p;
	    //OS_PRINTF("memcpy, before = 0x%x \n",b_value);
	    memcpy(pucDestBuffer, pucSourceBuffer, uiSize);
	    q = (u8 *)pucDestBuffer + uiSize;
	    a_value = *q;
	    //OS_PRINTF("memcpy, after = 0x%x \n",a_value);
	    //OS_PRINTF("memcpy2, 0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x,0x%x, 0x%x \n",*(u8 *)pucDestBuffer,*(u8 *)(pucDestBuffer+1),
	    //*(u8 *)(pucDestBuffer+2),*(u8 *)(pucDestBuffer+3),*(u8 *)(pucDestBuffer+4),*(u8 *)(pucDestBuffer+5),*(u8 *)(pucDestBuffer+6),*(u8 *)(pucDestBuffer+7));

	    if(b_value != a_value)
	    {
	    	OS_PRINTF("pucDestBuffer = 0x%x, pucSourceBuffer = 0x%x \n",pucDestBuffer,pucSourceBuffer);
			MT_ASSERT(0);
	    }
	}
}

INT32 DSAD_Memcmp(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize)
{
    INT32 ret = 0;

    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pucDestBuffer || NULL == pucSourceBuffer)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
	    ret = memcmp(pucDestBuffer, pucSourceBuffer, uiSize);

    return ret;
}

INT32 DSAD_StrLen(IN const UINT8* pucFormatBuffer)
{
    INT32 str_len = 0;

    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pucFormatBuffer)
    {
        ADS_DRV_PRINTF("[drv] %s ,%d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
		str_len = (INT32)strlen((char *)pucFormatBuffer);

    return str_len;
}

void DSAD_Sleep(UINT16 usMilliSeconds)
{
    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    mtos_task_sleep(usMilliSeconds);
}

BOOL DSAD_RegisterTask (IN INT8* pucName, UINT8 ucPriority, IN void* pTaskFun )
{
    INT32 ret = FALSE;
    u32 *p_stack = NULL;
    u8 uIndex = 0;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pucName || NULL == pTaskFun)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    for(uIndex = 0; uIndex < DS_ADS_TASK_NUM; uIndex++)
    {
      if(DS_ADS_FREE == ds_ads_task_record[uIndex].task_status)
      {
        ds_ads_task_record[uIndex].task_status = DS_ADS_USED;
        break;
      }
    }

    if(uIndex >= DS_ADS_TASK_NUM)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
	{
	    ds_ads_task_record[uIndex].task_prio =  p_ads_ds_priv->task_prio_start + uIndex;

	    ADS_DRV_PRINTF("[drv] %s,%d \n",pucName,
	                                 ds_ads_task_record[uIndex].task_prio);

	    p_stack = (u32 *)mtos_malloc(DS_ADS_TASK_STKSIZE);
	    MT_ASSERT(p_stack != NULL);

	    ret = mtos_task_create((u8 *)pucName,
	                                           (void *)pTaskFun,
	                                           NULL,
	                                           ds_ads_task_record[uIndex].task_prio,
	                                           p_stack,
	                                           DS_ADS_TASK_STKSIZE);
	    if(!ret)
	    {
	        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
	        ds_ads_task_record[uIndex].task_status = DS_ADS_FREE;
	        MT_ASSERT(0);
	    }
	}
    return TRUE;
}

void DSAD_SemaphoreInit(IN DSAD_Semaphore* puiSemaphore , UINT8 ucInitVal)
{
    INT32 ret = FALSE;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == puiSemaphore)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    //ADS_DRV_PRINTF("[drv] puiSemaphore = 0x%x, val= %d ( 0 or 1) \n",puiSemaphore,ucInitVal);

    ret = mtos_sem_create((os_sem_t *)puiSemaphore, ucInitVal);
    if (!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    //ADS_DRV_PRINTF("[drv] semaphore 0x%x = %d \n",puiSemaphore,*puiSemaphore);
    //OS_PRINTF("[drv] init semaphore = 0x%x \n",*puiSemaphore);
}

void DSAD_ReleaseSemaphore(IN DSAD_Semaphore* puiSemaphore)
{
    INT32 ret = FALSE;

    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    //ADS_DRV_PRINTF("[drv] semaphore 0x%x = %d \n",puiSemaphore,*puiSemaphore);
    //OS_PRINTF("[drv] release semaphore = 0x%x \n",*puiSemaphore);

    if(NULL == puiSemaphore)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    ret = mtos_sem_give((os_sem_t *)puiSemaphore);
    if (!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        //MT_ASSERT(0);
    }
}

void DSAD_WaitSemaphore(IN DSAD_Semaphore* puiSemaphore)
{
    INT32 ret = FALSE;

    //ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);
    //ADS_DRV_PRINTF("[drv] semaphore 0x%x = %d \n",puiSemaphore,*puiSemaphore);
    //OS_PRINTF("[drv] wait semaphore = 0x%x \n",*puiSemaphore);

    if(NULL == puiSemaphore)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    ret = mtos_sem_take((os_sem_t *)puiSemaphore, 0);
    if (!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        //MT_ASSERT(0);
    }

}

BOOL DSAD_MsgQueueInit(IN INT8* pucName,
                                    UINT32* uiMsgQueueHandle,
                                    UINT32 uiMaxMsgLen,
                                    EN_DSAD_MSG_QUEUE_MODE enMode)
{
    INT32 ret = FALSE;
    UINT32 quenue_id = 0;
    void *p_quenue_buf = NULL;
    UINT32 quenue_buf_size = 0;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pucName || NULL == uiMsgQueueHandle)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
	{
	    ADS_DRV_PRINTF("[drv] uiMaxMsgLen = %d \n",uiMaxMsgLen);
	    quenue_id = mtos_messageq_create(uiMaxMsgLen,(UINT8 *)pucName);
           //lint -save -e650 -e737
	    MT_ASSERT(quenue_id != INVALID_MSGQ_ID);
           //lint -restore

	    quenue_buf_size = sizeof(ST_DSAD_MSG_QUEUE) * uiMaxMsgLen;
	    p_quenue_buf = mtos_malloc(quenue_buf_size);
	    MT_ASSERT(p_quenue_buf != NULL);
	    memset(p_quenue_buf,0,quenue_buf_size);

	    ret = mtos_messageq_attach(quenue_id,p_quenue_buf,
	                                            sizeof(ST_DSAD_MSG_QUEUE),
	                                            DS_ADS_QUEUE_MAX_DEPTH);
	    if(!ret)
	    {
	         mtos_free(p_quenue_buf);
	        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
	         MT_ASSERT(0);
	    }
		else
		    *uiMsgQueueHandle = quenue_id;
	}
    return ret;
}

BOOL DSAD_MsgQueueGetMsg(UINT32 uiMsgHandle,
                                            ST_DSAD_MSG_QUEUE* pstMsg,
                                            EN_DSAD_MSG_QUEUE_MODE enMode,
                                            UINT32 uiWaitMilliSecond)
{
    BOOL ret = FALSE;
    os_msg_t msg = {0};

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pstMsg)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
	else
	{
	    if(DSAD_MSG_QUEUE_WAIT == enMode)
	    {
	        ret = mtos_messageq_receive(uiMsgHandle,&msg,uiWaitMilliSecond);
	    }
	    else
	    {
	        ret = mtos_messageq_receive(uiMsgHandle,&msg,0);
	    }
		
	    if(!ret)
	    {
	        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
	        MT_ASSERT(0);
	    }
		else
	        memcpy(pstMsg,(ST_DSAD_MSG_QUEUE*)msg.para1,msg.para2);

	}
    return ret;
}

BOOL DSAD_MsgQueueSendMsg(UINT32 uiMsgHandle,
                                                ST_DSAD_MSG_QUEUE* pstMsg)
{
    INT32 ret = FALSE;
    os_msg_t msg = {0};

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    if(NULL == pstMsg)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    msg.is_ext = 0;
    msg.para1 = (UINT32)pstMsg;
    msg.para2 = sizeof(ST_DSAD_MSG_QUEUE);

    ret = mtos_messageq_send(uiMsgHandle, &msg);
    if(!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

    return TRUE;
}

EN_DSAD_FUNCTION_TYPE DS_AD_SetFilter(ST_DSAD_FILTER_INFO *pstFilterInfo)
{
	INT32 ret = FALSE;
	dmx_device_t *p_dev = NULL;
	dmx_slot_setting_t slot = {0,};
	dmx_filter_setting_t  filter_param = {0,};
	u32 index = 0;
	s8 i = 0;

	ADS_DRV_PRINTF("\n [drv] %s,%d \n",__func__,__LINE__);
	if(NULL == pstFilterInfo)
	{
		ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
		MT_ASSERT(0);
		return DSAD_FUN_ERR_PARA;
	}

	ADS_DRV_PRINTF("enDataId                 :%d \n",pstFilterInfo->enDataId);
	ADS_DRV_PRINTF("usChannelPid           :0x%x \n",pstFilterInfo->usChannelPid);
	ADS_DRV_PRINTF("ucFilterLen               :%d \n",pstFilterInfo->ucFilterLen);
	ADS_DRV_PRINTF("pfDataNotifyFunction :0x%x \n\n",pstFilterInfo->pfDataNotifyFunction);

	index = pstFilterInfo->enDataId;

	//OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	ds_ads_lock();
	//OS_PRINTF("[drv] %s %d lock end  \n",__func__,__LINE__);
	if(ds_ads_filter_info[index].filter_status == DS_ADS_USED)
	{
	    if(memcmp(ds_ads_filter_info[index].filter_data,pstFilterInfo->aucFilter,15) == 0
	        && memcmp(ds_ads_filter_info[index].filter_mask,pstFilterInfo->aucMask,15) == 0)
	    {

	        //ADS_DRV_PRINTF("[drv] %s %d unlock ,the same filter data ! \n",__func__,__LINE__);
	        ds_ads_unlock();
	        return DSAD_FUN_OK;
	    }
	    else
	    {
	        ds_ads_filter_info[index].filter_status = DS_ADS_FREE;
	        ds_ads_filter_free(&ds_ads_filter_info[index]);
	    }
	}

	//set filter
	p_dev = (dmx_device_t *)dev_find_identifier(NULL
	    , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
	MT_ASSERT(NULL != p_dev->p_base);

	ds_ads_filter_info[index].p_buf = ds_ads_filter_buf[index];
	memset(ds_ads_filter_info[index].p_buf,0,DS_ADS_DMX_BUF_SIZE);

	memset(&slot, 0, sizeof(dmx_slot_setting_t));
	slot.in = DMX_INPUT_EXTERN0;
	slot.pid   = pstFilterInfo->usChannelPid;
	slot.type = DMX_CH_TYPE_SECTION;

	ret = dmx_si_chan_open(p_dev, &slot, &ds_ads_filter_info[index].channel_filter_id);
	if(0 != ret)
	{
		ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
		ds_ads_unlock();
		MT_ASSERT(0);
		return DSAD_FUN_ERR_PARA;
	}

	ret	 = dmx_si_chan_set_buffer(p_dev,
	        ds_ads_filter_info[index].channel_filter_id,
	        ds_ads_filter_info[index].p_buf,
	        DS_ADS_DMX_BUF_SIZE);
	if(0 != ret)
	{
	ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
	ds_ads_unlock();
	MT_ASSERT(0);
	return DSAD_FUN_ERR_PARA;

	}

	ADS_DRV_PRINTF("\n data        mask: \n");
	for(i = 0; i < 15; i++)
	{
	    ADS_DRV_PRINTF(" %x     %x \n",pstFilterInfo->aucFilter[i],
	                                   pstFilterInfo->aucMask[i]);
	    ds_ads_filter_info[index].filter_data[i] = pstFilterInfo->aucFilter[i];
	    ds_ads_filter_info[index].filter_mask[i] = pstFilterInfo->aucMask[i];
	}

	if(pstFilterInfo->ucFilterLen > 14)
	{
	    OS_PRINTF("===request filter data len more than max len !===\n");
	}

	ADS_DRV_PRINTF("filter data  len = %d \n",pstFilterInfo->ucFilterLen);

	filter_param.value[0] = pstFilterInfo->aucFilter[0];
	filter_param.value[1] = pstFilterInfo->aucFilter[3];
	filter_param.value[2] = pstFilterInfo->aucFilter[4];
	filter_param.value[3] = pstFilterInfo->aucFilter[5];
	filter_param.value[4] = pstFilterInfo->aucFilter[6];
	filter_param.value[5] = pstFilterInfo->aucFilter[7];
	filter_param.value[6] = pstFilterInfo->aucFilter[8];
	filter_param.value[7] = pstFilterInfo->aucFilter[9];
	filter_param.value[8] = pstFilterInfo->aucFilter[10];
	filter_param.value[9] = pstFilterInfo->aucFilter[11];
	filter_param.value[10] = pstFilterInfo->aucFilter[12];
	filter_param.value[11] = pstFilterInfo->aucFilter[13];

	filter_param.mask[0] = pstFilterInfo->aucMask[0];
	filter_param.mask[1] = pstFilterInfo->aucMask[3];
	filter_param.mask[2] = pstFilterInfo->aucMask[4];
	filter_param.mask[3] = pstFilterInfo->aucMask[5];
	filter_param.mask[4] = pstFilterInfo->aucMask[6];
	filter_param.mask[5] = pstFilterInfo->aucMask[7];
	filter_param.mask[6] = pstFilterInfo->aucMask[8];
	filter_param.mask[7] = pstFilterInfo->aucMask[9];
	filter_param.mask[8] = pstFilterInfo->aucMask[10];
	filter_param.mask[9] = pstFilterInfo->aucMask[11];
	filter_param.mask[10] = pstFilterInfo->aucMask[12];
	filter_param.mask[11] = pstFilterInfo->aucMask[13];

	filter_param.continuous = TRUE;
	filter_param.en_crc = TRUE;
	filter_param.req_mode = DMX_REQ_MODE_SECTION_DOUBLE;
	filter_param.ts_packet_mode = DMX_ONE_MODE;

	ret = dmx_si_chan_set_filter(p_dev,
	                    ds_ads_filter_info[index].channel_filter_id,
	                    &filter_param);
	if(0 != ret)
	{
	ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
	ds_ads_unlock();
	MT_ASSERT(0);
	return DSAD_FUN_ERR_PARA;
	}

	ret = dmx_chan_start(p_dev,ds_ads_filter_info[index].channel_filter_id);
	if(0 != ret)
	{
	ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
	ds_ads_unlock();
	MT_ASSERT(0);
	return DSAD_FUN_ERR_PARA;
	}

	ds_ads_filter_info[index].p_dev = p_dev;
	ds_ads_filter_info[index].channel_pid = pstFilterInfo->usChannelPid;
	ds_ads_filter_info[index].start_time = mtos_ticks_get();
	ds_ads_filter_info[index].timeout = pstFilterInfo->ucWaitSeconds * 100;
	ds_ads_filter_info[index].callback_func = pstFilterInfo->pfDataNotifyFunction;
	ds_ads_filter_info[index].filter_status = DS_ADS_USED;
	ds_ads_unlock();
        
     return DSAD_FUN_OK;
}

EN_DSAD_FUNCTION_TYPE DS_AD_StopFilter(EN_DSAD_DATA_ID enDataID, UINT16 usPID)
{
    u32 index = 0;
    INT32 ret = FALSE;
    dmx_device_t *p_dev = NULL;
    ADS_DRV_PRINTF("\n [drv] %s,%d \n",__func__,__LINE__);

    index = enDataID;
    if(ds_ads_filter_info[index].filter_status != DS_ADS_USED)
    {
        return DSAD_FUN_OK;
    }
    
    ds_ads_lock();

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
           , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    ret = dmx_chan_stop(p_dev, ds_ads_filter_info[index].channel_filter_id);
    if(0 != ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        ds_ads_unlock();
        MT_ASSERT(0);
    }

    ret = dmx_chan_close(p_dev, ds_ads_filter_info[index].channel_filter_id);
    if(0 != ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        ds_ads_unlock();
        MT_ASSERT(0);
    }

    ds_ads_filter_info[index].filter_status = DS_ADS_FREE;
    ds_ads_unlock();
    
    return DSAD_FUN_OK;
}
void DSAD_ShowMessage(UINT8 ucMessageType, IN UINT8 * pucMessage)
{
    #ifndef ADS_DESAI_DRV_DEBUG
    u8 *p_data = NULL;
    #endif
    ads_ad_type_t ad_type = ADS_AD_TYPE_NUM;
    ST_DSAD_LOG_INFO *p_start_ad = NULL;
    ST_DSAD_PROGRAM_SHOW_INFO *p_send_ad = NULL;

    ADS_DRV_PRINTF("[drv] %s,%d \n",__func__,__LINE__);

    ADS_DRV_PRINTF("[drv] ucMessageType=0x%x \n",ucMessageType);

    switch(ucMessageType)
    {
//        case DSAD_MESSAGE_HIDEOSD_TYPE:
//            ADS_DRV_PRINTF("[drv] MESSAGE_HIDEOSD_TYPE \n");
//            break;

//        case DSAD_MESSAGE_SHOWOSD_TYPE:
//            ADS_DRV_PRINTF("[drv] MESSAGE_SHOWOSD_TYPE \n");
//            break;

        case DSAD_MESSAGE_UPDATE_LOG_TYPE:
            ADS_DRV_PRINTF("[drv] MESSAGE_UPDATE_LOG_TYPE \n");
            //p_ads_ds_priv->read_gif(sizeof(ST_LOG_INFO), pucMessage);
            p_start_ad = (ST_DSAD_LOG_INFO *)pucMessage;
            ADS_DRV_PRINTF("logtype: %d (1:pic,2:av)\n",p_start_ad->ucLogType);
            if(p_start_ad->ucLogType == 1)
            {
                ad_type = ADS_AD_TYPE_PIC;
                ADS_DRV_DEBUG("pos type: %d \n",p_start_ad->Element.stPictureType.enPosType);
                ADS_DRV_DEBUG("pic type : %d \n",p_start_ad->Element.stPictureType.enPicType);
                ADS_DRV_DEBUG("X          : %d \n",p_start_ad->Element.stPictureType.usStartX);
                ADS_DRV_DEBUG("Y          : %d \n",p_start_ad->Element.stPictureType.usStartY);
                ADS_DRV_DEBUG("width    : %d \n",p_start_ad->Element.stPictureType.usWidth);
                ADS_DRV_DEBUG("height   : %d \n",p_start_ad->Element.stPictureType.usHeigth);
                ADS_DRV_DEBUG("size      : %d \n",p_start_ad->Element.stPictureType.uiDataLen);
                ADS_DRV_DEBUG("data     : %x \n",p_start_ad->Element.stPictureType.pucPicData[0]);

                #ifdef ADS_DESAI_DRV_DEBUG
                ds_ad_data_debug(p_start_ad->Element.stPictureType.pucPicData,
                                 p_start_ad->Element.stPictureType.uiDataLen);
                #else
                p_data = (u8 *)&p_start_ad->Element.stPictureType;
                #endif
            }
            else if(p_start_ad->ucLogType == 2)
            {
                ad_type = ADS_AD_TYPE_AV;
                OS_PRINTF("#####################ADS_AD_TYPE_AV\n\n");
                #ifdef ADS_DESAI_DRV_DEBUG
                ds_ad_data_debug(p_start_ad->Element.stAvType.pucAvData,
                                 p_start_ad->Element.stAvType.uiDataLen);
                #else
                p_data = (u8 *)&p_start_ad->Element.stAvType;
                #endif
            }

            #ifndef ADS_DESAI_DRV_DEBUG
            p_ads_ds_priv->notify_msg_to_ui(TRUE, 0, ad_type, (u32)p_data);
            #endif
            break;

        case DSAD_MSEEAGE_SHOWFULLSRCEEN_TYPE:
            OS_PRINTF("[drv] MSEEAGE_SHOWFULLSRCEEN_TYPE \n");
            p_send_ad = (ST_DSAD_PROGRAM_SHOW_INFO *)pucMessage;
            ds_ad_info_parser(p_send_ad);
            break;

        case DSAD_MSEEAGE_HIDEFULLSRCEEN_TYPE:
            ADS_DRV_PRINTF("[drv] MSEEAGE_HIDEFULLSRCEEN_TYPE \n");
            #ifndef ADS_DESAI_DRV_DEBUG
            p_ads_ds_priv->notify_msg_to_ui(TRUE, 0, ADS_AD_TYPE_NUM, (u32)NULL);
            #endif
            break;
        case DSAD_MSEEAGE_UPDATEOSD_TYPE:
            ADS_DRV_PRINTF("[drv] DSAD_MSEEAGE_UPDATEOSD_TYPE \n");
            ad_type = ADS_AD_TYPE_OSD;
            #ifndef ADS_DESAI_DRV_DEBUG
            p_ads_ds_priv->notify_msg_to_ui(TRUE, 0, ad_type, (u32)NULL);
            #endif
            break;
        default:
            break;
    }


}

static void ds_ads_data_monitor(void *p_param)
{
    s32 ret = FALSE;
    u8 index = 0;
    u8 *p_data = NULL;
    u32 data_len = 0;
    u32 filter_time = 0;

	for(;;)
	{
	    //dmx_jazz_wait_for_data(20);
	    mtos_task_sleep(20);

	//           OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	    ds_ads_lock();
	//	     OS_PRINTF("[drv] %s %d lock end  \n",__func__,__LINE__);
	    for(index = 0; index < DS_ADS_FILTER_MAX_NUM; index++)
	    {
	        if(ds_ads_filter_info[index].filter_status == DS_ADS_FREE)
	        {
	            continue;
	        }

	        ret=dmx_si_chan_get_data(ds_ads_filter_info[index].p_dev,
	                                    ds_ads_filter_info[index].channel_filter_id,
	                                    &p_data,
	                                    &data_len);
	        if(0 != ret)
	        {
	            filter_time = mtos_ticks_get() - ds_ads_filter_info[index].start_time;
	 			//OS_PRINTF("[drv] %s %d 0 != ret\n",__func__,__LINE__);

	            if(filter_time > ds_ads_filter_info[index].timeout 
	                && NIT_PID != ds_ads_filter_info[index].channel_pid)
	            {
	    			//OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	    			ds_ads_unlock();
	 				//OS_PRINTF("[drv] %s %d lock end  \n",__func__,__LINE__);
	                //ADS_DRV_PRINTF("[drv] get data timeout  \n\n");
	                ds_ads_filter_info[index].callback_func(
	                            ds_ads_filter_info[index].channel_pid, NULL, 0);
					ds_ads_filter_info[index].start_time = mtos_ticks_get();
	    			ds_ads_lock();
	 			//OS_PRINTF("[drv] %s %d lock end  \n",__func__,__LINE__);
	            }
	        }
	        else
	        {
	            #if 0
	            ADS_DRV_PRINTF("[drv] get data[%d], %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
	                data_len,p_data[0],p_data[1],p_data[2],p_data[3],p_data[4],p_data[5],p_data[6],p_data[7],
	                p_data[8],p_data[9],p_data[10],p_data[11],p_data[12],p_data[13],p_data[14],p_data[15]);
	            #endif
	            //OS_PRINTF("[drv] ds_ads_filter_info[index].channel_pid=0x%x ,pdata[0]=0x%x\n",ds_ads_filter_info[index].channel_pid,p_data[0]);
	            if(NIT_PID == ds_ads_filter_info[index].channel_pid && p_data[0] == NIT_TABLE_ID)
	            {
	                OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	                ds_ads_parser_nit(p_data, data_len);
	                ds_ads_filter_info[index].filter_status = DS_ADS_FREE;
	                ds_ads_filter_free(&ds_ads_filter_info[index]);
	            }
	            else
	            {
	                ADS_DRV_PRINTF("index=%d, callback_func=0x%x \n",index,
	                                            ds_ads_filter_info[index].callback_func);
	    			//OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	    		   //OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	    		   ds_ads_unlock();
	 		   		//OS_PRINTF("[drv] %s %d lock end  \n",__func__,__LINE__);
	               ds_ads_filter_info[index].callback_func(ds_ads_filter_info[index].channel_pid,
	                                                                             p_data, (u16)data_len);
	    		   //OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	    		   ds_ads_lock();
	 		   		//OS_PRINTF("[drv] %s %d lock end  \n",__func__,__LINE__);
	                ds_ads_filter_info[index].start_time = mtos_ticks_get();
	                ds_ads_filter_info[index].filter_num = 0;
	            }
	        }
	    }
	    //OS_PRINTF("[drv] %s %d lock begin \n",__func__,__LINE__);
	    ds_ads_unlock();
	 //OS_PRINTF("[drv] %s %d lock end  \n",__func__,__LINE__);
	}
}

void ds_ads_client_init(void)
{
    BOOL ret = FALSE;
    u32 *p_stack = NULL;
    ret = mtos_sem_create((os_sem_t *)&g_ads_ds_lock, TRUE);
    if(!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }

   OS_PRINTF("[drv] %s task:%d \n",__func__,p_ads_ds_priv->task_prio_end);

    p_stack = (u32 *)mtos_malloc(DS_ADS_TASK_STKSIZE);
    MT_ASSERT(p_stack != NULL);

    ret = mtos_task_create((u8 *)"ads_monitor",
                                           ds_ads_data_monitor,
                                           NULL,
                                           //p_ads_ds_priv->task_prio_start - 1,
                                           p_ads_ds_priv->task_prio_end,
                                           p_stack,
                                           DS_ADS_TASK_STKSIZE);
    if(!ret)
    {
        ADS_DRV_PRINTF("[drv] %s %d ERR! \n",__func__,__LINE__);
        MT_ASSERT(0);
    }
}

 typedef char    *ck_va_list;
 typedef unsigned int  CK_NATIVE_INT;
 typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))


void DSAD_Printf(OUT const INT8 *string,...)
{
#ifdef ADS_DESAI_AD_PRINTF

     //ADS_DRV_PRINTF("#######desai debug ######\n");

    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200];

    CK_VA_START(p_args, string);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)string, p_args);

    CK_VA_END(p_args);
    OS_PRINTF(printk_buf);
    OS_PRINTF("\n");
#endif
}

INT32 DSAD_Sprintf(IN UINT8* pucDestBuffer, IN const UINT8* pucFormatBuffer, ...)
{
    ck_va_list	args;
    INT32   printed_len = 0;

    /* Emit the output into the temporary buffer */
    CK_VA_START(args, pucFormatBuffer);
    printed_len = ck_vsnprintf((char*)pucDestBuffer, 256, (const char*)pucFormatBuffer, args);
    CK_VA_END(args);
    return printed_len;
}
