/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include <stdio.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_mutex.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "drv_dev.h"
#include "dmx.h"



#include "kvdbdtv_api.h"
#include "kvdb_event.h"
#include "kvdb_demux.h"
#include "browser_adapter.h "


#define  KVDB_FILTER_NUMBER     4
#define KVDB_FILTER_BUFFER_SIZE    (64 * (KBYTES))
#define  INVALID_ID 	0xFFFFFFFF

#define KVDB_STACK_SIZE 80*1024
 struct kvdb_filter
{
	unsigned char*       buffer_addr;          
	unsigned char*       cur_buffer_addr;     
	unsigned char         total_section;    
	unsigned short	      cur_secnum;           
	unsigned long        section_len;  
	unsigned long        filter_id;               
	unsigned long        ext_id;                   
};
extern bowser_adapter_priv_t g_browser_priv;

u32 *p_stack = NULL;

static int event[3] = {0}; 
#define KVDB_DEMUX_DEBUG_PRINTF
#ifdef KVDB_DEMUX_DEBUG_PRINTF
#define KVDB_DEMUX_DEBUG OS_PRINTF
#else
#define KVDB_DEMUX_DEBUG DUMMY_PRINTF
#endif

static int s_tFilterBuf[KVDB_FILTER_NUMBER][KVDB_FILTER_BUFFER_SIZE] = {{0,},};
//static int s_kvdb_lock = 0;

struct dmx_filter_pattern 
{
   u8  value[DMX_SECTION_FILTER_SIZE];
   u8  mask[DMX_SECTION_FILTER_SIZE];
   u8 mask_len;
};
static struct kvdb_filter filter_array [KVDB_FILTER_NUMBER];
static  kdvb_filter_struct_t st_filter[KVDB_FILTER_NUMBER];
#if 0
static void kvdb_sem_unlock(void)
{
    //CAS_DIVI_PRINTF("[divi],divi_ca_unlock:%d \n",s_ca_lock);
    mtos_sem_give((os_sem_t *)&s_kvdb_lock);
}
static void kvdb_sem_lock(void)
{
    mtos_sem_take((os_sem_t *)&s_kvdb_lock, 0);
}
#endif
unsigned int kvdb_porting_dvb_play_av(unsigned int vid, unsigned int aid, unsigned int pcrid)
{
    return 1;
}
static RET_CODE dmx_callback(struct dmx_device *dev, u32 filter_id, u8* data_got, u32 data_size)
{
	unsigned char temp_buffer[4096];
	unsigned char last_sec_num,sec_num;
	unsigned char i;
  //  KVDB_DEMUX_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);
	for(i=0;i<KVDB_FILTER_NUMBER;i++)
	{
		if(filter_array[i].filter_id==filter_id)
		{
			break;
		}
	}
	if(i==KVDB_FILTER_NUMBER)
	{    
		return ERR_FAILURE;
	}
	sec_num = data_got[6];
  last_sec_num = data_got[7];
//  KVDB_DEMUX_DEBUG("sec_num = %d,  last_sec_num = %d, cur sect num = %d \n", sec_num, last_sec_num, filter_array[i].cur_secnum);
	if(filter_array[i].cur_secnum==sec_num)
		filter_array[i].cur_secnum++;//确保按顺序过滤，但有影响速度，不建议
	else 
		return SUCCESS;
    if(filter_array[i].buffer_addr == NULL)
    {
          filter_array[i].buffer_addr  = mtos_malloc((last_sec_num+1)*4096);

          MT_ASSERT(filter_array[i].buffer_addr != NULL);
          filter_array[i].cur_buffer_addr = filter_array[i].buffer_addr;
    }
	//过滤到的数据要以段号为顺序升序组合
    memcpy(temp_buffer, data_got, data_size);
	memcpy(filter_array[i].cur_buffer_addr,temp_buffer+8, data_size-12);
      filter_array[i].cur_buffer_addr+=data_size-12;
	filter_array[i].section_len+=data_size-12;
	filter_array[i].total_section++;
	
	if(filter_array[i].total_section==last_sec_num+1)
	{
   	   event[0]= DEMUX_MESSAGE;		
   	   event[1]=filter_array[i].ext_id;  
   	   event[2]=filter_id;                   
	   KVDB_DEMUX_DEBUG("have data lenth=%d\n",filter_array[i].section_len);
	   KVDB_DEMUX_PROC(event);
	}
	return SUCCESS;
}
/*
static signed int alloc_channel(unsigned short PID)
{
    struct dmx_slot_setting_t slot;
    dmx_chanid_t p_channel;
    dmx_device_t *p_dev = NULL;
    RET_CODE ret;
    memset(&slot, 0x0, sizeof(dmx_slot_setting_t));
    slot.in = DMX_INPUT_EXTERN0;
    slot.pid = PID;
    slot.type = DMX_CH_TYPE_SECTION;
      
   ret =  dmx_si_chan_open(p_dev, &slot, & p_channel);
   if(ret != SUCCESS)
   {
      return ERR_FAILURE;
   }
   return (signed int)p_channel;
}

*/
static int  alloc_filter()
{
    u32 i = 0;
    for(i = 0; i <KVDB_FILTER_NUMBER; i ++ )
    {
        if(st_filter[i].m_FilterStatus ==  FILTER_FREE)
        {
            return st_filter[i].m_byReqId; 
        }
    }
    if(i == KVDB_FILTER_NUMBER)
     {
        return 0xff;
    }
    
    return SUCCESS;
}
static RET_CODE filter_set_channel_pid(struct dmx_device *dmx, int filter_id, unsigned short PID)
{
    u32 index = 0;
    RET_CODE ret;
    KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);
    KVDB_DEMUX_DEBUG("filter_id = %d , pid = %d \n", filter_id, PID);
    dmx_slot_setting_t slot = {0};
   // dmx_filter_setting_t param = {0} ;

    for(index = 0; index <KVDB_FILTER_NUMBER; index++)
    {
        if( st_filter[index].m_byReqId == filter_id )
          {
                KVDB_DEMUX_DEBUG("FIND m_byReqid = %d \n",st_filter[index].m_byReqId );
                 break;
          }
    }
    if(index == KVDB_FILTER_NUMBER)
    return ERR_FAILURE;
    memset(&slot, 0x0, sizeof(dmx_slot_setting_t));
    st_filter[index].m_PID = PID;
    slot.in = DMX_INPUT_EXTERN0;
    slot.pid = PID;
    slot.type = DMX_CH_TYPE_SECTION;
    ret =  dmx_si_chan_open(dmx, &slot, &st_filter[index].m_dwReqHandle);
    KVDB_DEMUX_DEBUG(" filter_set_channel_pid channel handle = 0x%x, pid = 0x%x \n ", st_filter[index].m_dwReqHandle, slot.pid);
    MT_ASSERT(ret == SUCCESS);
 
    st_filter[index].started=  TRUE;
    st_filter[index].p_buf = (u8 *)&s_tFilterBuf[index];
   ret =  dmx_si_chan_set_buffer(dmx, st_filter[index].m_dwReqHandle, st_filter[index].p_buf, KVDB_FILTER_BUFFER_SIZE);
    MT_ASSERT(ret == SUCCESS);
    #if 0
    param.continuous= TRUE;
    param.en_crc = FALSE;
    param.req_mode = DMX_REQ_MODE_CONTINUOUS_SECTION;
    param.value[0] = 0x3b;
    param.mask[0] = 0xff;
    ret = dmx_si_chan_set_filter(dmx, st_filter[index].m_dwReqHandle, &param);
    MT_ASSERT(ret == SUCCESS);
    ret = dmx_chan_start( dmx, st_filter[index].m_dwReqHandle);
     MT_ASSERT(ret == SUCCESS);
     
    st_filter[index].m_FilterStatus= FILTER_BUSY;
    st_filter[index].m_dwStartMS = mtos_ticks_get();
    #endif
    return SUCCESS;
}


 static RET_CODE filter_set_pattern(struct dmx_device *dmx, int filter_id, struct dmx_filter_pattern *dfp)
 {
    u32 i,  index = 0;
    RET_CODE ret;
        KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);
      KVDB_DEMUX_DEBUG("filter_id = %d , value = 0x%x \n", filter_id, dfp->value[0]);
   dmx_filter_setting_t param  = {0};
    for(index = 0; index <KVDB_FILTER_NUMBER; index++)
    {
        if( st_filter[index].m_byReqId == filter_id )
          {
                 break;
          }
    }
    if(index == KVDB_FILTER_NUMBER)
      return ERR_FAILURE;
   memcpy(st_filter[index].m_uFilterData,dfp->value,dfp->mask_len);
    memcpy(st_filter[index].m_uFilterMask,dfp->mask,dfp->mask_len);
    param.continuous= TRUE;
    param.en_crc = FALSE;
   param.req_mode = DMX_REQ_MODE_CONTINUOUS_SECTION;
#if 1
    for(i = 0; i < dfp->mask_len; ++i)
    {
        param.value[i] = dfp->value[i];
        param.mask[i] = dfp->mask[i];
        KVDB_DEMUX_DEBUG("value[%d] = 0x%x \n", i ,param.value[i] );
        KVDB_DEMUX_DEBUG("value[%d] = 0x%x \n", i ,param.mask[i] );
    }
    if(st_filter[index].m_FilterStatus == FILTER_BUSY)
      {
        dmx_chan_stop(dmx, st_filter[index].m_dwReqHandle);
        st_filter[index].m_FilterStatus = FILTER_FREE;
    }
    #endif
    
    KVDB_DEMUX_DEBUG("st_filter[index].m_dwReqHandle = 0X%x \n ",st_filter[index].m_dwReqHandle );
    ret = dmx_si_chan_set_filter(dmx, st_filter[index].m_dwReqHandle, &param);
    MT_ASSERT(ret == SUCCESS);
    ret = dmx_chan_start( dmx, st_filter[index].m_dwReqHandle);
     MT_ASSERT(ret == SUCCESS);

    st_filter[index].m_FilterStatus= FILTER_BUSY;
    st_filter[index].m_dwStartMS = mtos_ticks_get();
    
    return SUCCESS;
 }
 static RET_CODE filter_set_callback(struct dmx_device *dmx, u32 filter_id, kvdb_dmx_event_notify notify, u32 timeout )
  {
      KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);
KVDB_DEMUX_DEBUG("filter_set_callback filter_id = %d\n", filter_id);
     u32 index = 0;
    for(index = 0; index <KVDB_FILTER_NUMBER; index++)
    {
        if( st_filter[index].m_byReqId == filter_id )
          {
                KVDB_DEMUX_DEBUG("filter_set_callback m_byReqId = %d \n ", st_filter[index].m_byReqId );  
                 break;
          }
    }
    if(index == KVDB_FILTER_NUMBER)
    return ERR_FAILURE;
    if (timeout == 0)
    {
     st_filter[index].m_timeout = 0xffffffff;
    }
    else
    {
      st_filter[index].m_timeout = timeout;
    }
    st_filter[index].call_back = notify;
    return SUCCESS;

 }
 /*
static RET_CODE filter_link_channel(struct dmx_device *dmx, int  filter_id, signed int vc)
{
      u32 index = 0;
    for(index = 0; index <KVDB_FILTER_NUMBER; index++)
    {
        if( st_filter[index].m_byReqId == filter_id )
          {
                 break;
          }
    }
    if(index == KVDB_FILTER_NUMBER)
    return ERR_FAILURE;
    st_filter[index].m_dwReqHandle = vc;
}*/
unsigned int kvdb_porting_filter_open(unsigned short PID,unsigned char tableID, unsigned short ext)
{
	int filter_id;
  
	struct dmx_device *dmx;
	struct dmx_filter_pattern dfp;
	signed int i;
    KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);
    KVDB_DEMUX_DEBUG("pid = 0x%x, table_id = 0x%x , ext = 0x%x\n", PID, tableID, ext);
  dmx = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
      MT_ASSERT(NULL != dmx->p_base);
  //    kvdb_sem_lock();
       for(i=0;i<KVDB_FILTER_NUMBER;++i)
	{
		if(INVALID_ID == filter_array[i].filter_id)
		{
		/*
			vc = alloc_channel(PID);	
			if (vc<0) 
			{
				return 0xFFFFFFFF;
			}
        */
			filter_id=alloc_filter();
          KVDB_DEMUX_DEBUG("alloc filter id = %d\n",filter_id );
			filter_array[i].filter_id=filter_id;
			filter_array[i].ext_id=ext;
			filter_array[i].cur_secnum=0;
			
			memset(&dfp, 0, sizeof(dfp));
			dfp.value[0] = tableID;
			dfp.mask[0] = 0xff;
			if ((ext != 0xFFFF)) 
			{
			    {
				dfp.value[1] = ext>>8;
				dfp.value[2] = ext&0xff;
				dfp.mask[1] = 0xff;
				dfp.mask[2] = 0xff;
				dfp.mask_len = 3;
                }
             } 
			else 
			{
				dfp.mask_len = 1;
			}
      KVDB_DEMUX_DEBUG("dfp.mask_len = %d, filter_id = %d \n ", dfp.mask_len, filter_id);
           filter_set_channel_pid(dmx, filter_id, PID);
          	filter_set_callback(dmx, filter_id, dmx_callback,  0);
			filter_set_pattern(dmx, filter_id, &dfp);
			//filter_link_channel(dmx, filter_id, vc);
          KVDB_DEMUX_DEBUG("start search pid  ----\n");

      //    kvdb_sem_unlock();
			return (unsigned int)i;
		}
	}
 //      kvdb_sem_unlock();
   return 0xFFFFFFFF;
}

static void filter_free(struct dmx_device *dmx, int filter_id )
{
    u32 index = 0;
    RET_CODE ret;
     KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);
    for(index = 0; index < KVDB_FILTER_NUMBER; index ++)
     {
          if(st_filter[index].m_byReqId == filter_id)
            break;
     }
      if(index == KVDB_FILTER_NUMBER)
      {
        return;
      }
//    kvdb_sem_lock();
KVDB_DEMUX_DEBUG("filter_id = %d, channel handle = 0x%x \n",filter_id, st_filter[index].m_dwReqHandle);
    ret = dmx_chan_stop(dmx, st_filter[index].m_dwReqHandle);
    MT_ASSERT(ret == SUCCESS);
    st_filter[index].m_FilterStatus = FILTER_FREE;
   ret = dmx_chan_close(dmx,st_filter[index].m_dwReqHandle);
  MT_ASSERT(ret == SUCCESS);
  memset(&st_filter[index],0x0, sizeof(kdvb_filter_struct_t));
  st_filter[index].m_byReqId = index;

      KVDB_DEMUX_DEBUG("free----------------\n");
	filter_array[index].filter_id = INVALID_ID;
  if(filter_array[index].buffer_addr)
    {
        mtos_free(filter_array[index].buffer_addr);
    }
	filter_array[index].buffer_addr = NULL;
	filter_array[index].cur_buffer_addr = NULL;
	filter_array[index].total_section =  0;
	filter_array[index].section_len=0;
	filter_array[index].ext_id = INVALID_ID;	
  filter_array[index].cur_secnum = 0;
  
//kvdb_sem_unlock();
}
int kvdb_porting_filter_close(unsigned int slot)
{
    KVDB_DEMUX_DEBUG("%s, %d , slot = %d \n",__FUNCTION__,  __LINE__, slot);

   	struct dmx_device *dmx;
   // u32 i = 0;
   // RET_CODE ret;
   u32 index = 0;
    dmx =  (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
      MT_ASSERT(NULL != dmx->p_base);
     
	if(slot>=KVDB_FILTER_NUMBER&&slot != 0xff)	
	  return 0;
   

  if(slot == 0xff)
   {
       for(index = 0; index < KVDB_FILTER_NUMBER; index ++)
        {
            	if(filter_array[index].filter_id == INVALID_ID)
	          {
            	  return 0;
            	}
              	filter_free(dmx, filter_array[index].filter_id);

        }
   }
  else
    {
        	filter_free(dmx, filter_array[slot].filter_id);
    }
  KVDB_DEMUX_DEBUG("slot = %d,filter_array[slot].filter_id = %d \n ", slot,filter_array[slot].filter_id );
    

//   kvdb_sem_unlock();
       return 0;
}

//max_len 为0，copy的数据长度就是过滤到数据长度
int kvdb_porting_filter_recv(unsigned int slot, unsigned char *buffer, int max_len)
{
    KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);

      if(slot>=KVDB_FILTER_NUMBER)	
  	  return 0;
	  
	if(filter_array[slot].filter_id == INVALID_ID)
	{
		return 0;
	}
	/*filter_array[slot].buffer_addr 里存放的数据必须是以段号为顺序的全部数据*/
	if(max_len==0)
		  memcpy(buffer, filter_array[slot].buffer_addr, filter_array[slot].section_len);
	else
		  memcpy(buffer, filter_array[slot].buffer_addr, max_len);
   // mtos_free(filter_array[slot].buffer_addr);
	return filter_array[slot].section_len;

}
static void filter_init(void)
{
   u32 index = 0;
   for(index = 0; index < KVDB_FILTER_NUMBER; index ++)
   {
        memset(&st_filter[index], 0x0, sizeof(kdvb_filter_struct_t));
        st_filter[index].m_byReqId = index;
        filter_array[index].filter_id = INVALID_ID;
        filter_array[index].buffer_addr = NULL;
        filter_array[index].cur_buffer_addr = NULL;
        filter_array[index].total_section =  0;
        filter_array[index].section_len=0;
        filter_array[index].ext_id = INVALID_ID;	

    }
   
}
static void data_monitor_task(void *p_data )
{
      struct dmx_device *dmx;
      dmx = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
      MT_ASSERT(NULL != dmx->p_base);
      u32 index = 0;
      signed int err;
      u32 data_size = 0;
      u8 *data_got = NULL;
       KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);

      while(1)
      {
        //   mtos_task_lock();
           for(index = 0; index < KVDB_FILTER_NUMBER; index++)
            {
             //   KVDB_DEMUX_DEBUG("st_filter[%d].m_FilterStatus = %d \n",index, st_filter[index].m_FilterStatus );
                if(st_filter[index].m_FilterStatus == FILTER_BUSY)
                  {
                    //      KVDB_DEMUX_DEBUG("st_filter[index].PID = 0x%x, channel_handle = 0x%x, req_id = %d \n",st_filter[index].m_PID,
                                           //            st_filter[index].m_dwReqHandle,st_filter[index].m_byReqId);
                          err = dmx_si_chan_get_data(dmx, st_filter[index].m_dwReqHandle, &data_got, &data_size);
                          if(err != SUCCESS)
                            {
                                 //  KVDB_DEMUX_DEBUG("NO DATA COME ----\n");
                            }
                          else
                            {
                                  if(data_size > 4096)
                                    {
                                          continue;
                                    }
                                  else
                                    {
                                     //     KVDB_DEMUX_DEBUG("data come- ----------------\n");
                                          if(st_filter[index].call_back)
                                            {
                                                st_filter[index].call_back(dmx, st_filter[index].m_byReqId, data_got, data_size);
                                            }
                                    }   
                            }
                           
                  }
            }
        //  mtos_task_unlock();
          mtos_task_sleep(40);
      }
}
void kvdb_porting_demux_init(void)
{
    int err;
        KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);

//    err = mtos_sem_create((os_sem_t *) (&s_kvdb_lock), 1);
    //if(!err)
    {
        //KVDB_DEMUX_DEBUG("create err s_dvb_lock");
    }
    filter_init();
    p_stack = mtos_malloc(KVDB_STACK_SIZE);
#if 1
    if(p_stack)
 
    {
          err = mtos_task_create((u8 *)"demxdatamonitor",
                                              data_monitor_task,
                                                                  (void* )0,
                                        g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start,
                                                              p_stack,
                                                              KVDB_STACK_SIZE);
        
          if(!err)
            {
                 KVDB_DEMUX_DEBUG("Create data monitor task error = 0x%08x!\n", err);
            }
    }
    KVDB_DEMUX_DEBUG("%s, %d \n",__FUNCTION__,  __LINE__);
#endif
}

void kvdb_porting_demux_exit(void)
{
    int index = 0;
    
    for(index = 0; index < KVDB_FILTER_NUMBER; ++index)
    {
       kvdb_porting_filter_close(index);        
    }
    mtos_task_delete( g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start);
    if(p_stack)
      {
          mtos_free(p_stack);
      }
	/*
	free all filter;
	free resouce;
	*/
}

