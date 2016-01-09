/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_mutex.h"
#include "mtos_printk.h"
#include "mtos_timer.h"
#include "charsto.h"

#include "mem_manager.h"
#include "nim.h"
#include "dmx.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"
#include "common.h"

#include "vdec.h"
#include "display.h"
#include "browser_adapter.h"

#define KVDB_DRV_DEBUG_PRINTF
#ifdef KVDB_DRV_DEBUG_PRINTF
#define KVDB_DRV_DEBUG OS_PRINTF
#else
#define KVDB_DRV_DEBUG(...) do{}while(0)
#endif
#define KVDB_TABLE_ID_PAT 0x00
#define KVDB_TABLE_ID_PMT 0x02

#define KVDB_MID_DATA_PROCESS  8*1024
#define KVDB_FILTER_BUFFER_SIZE (32*KBYTES + 188)
#define MAX_SECTION_DATA_LEN 4096
extern bowser_adapter_priv_t g_browser_priv;

u16 KVDB_MID_IPANEL_PID = 0;
u32 *stack =NULL;
typedef RET_CODE (*notify_call_back)(u8 *data ,u32 data_size, void *filter, u16 pid ); 

typedef struct
{
  BOOL used;
  BOOL started;
  u16 pid;
  u32 start_time;
  dmx_chanid_t chanid;
  u8 *p_buffer;
  notify_call_back notify;
  
}adv_demux_filter_t;
void kvdb_catch_table(u16 pid, u16 table_id);


#define KVDB_MID_MAX_FILTER  4
struct dmx_device *p_ipannel_demux = NULL;
static adv_demux_filter_t st_filter[KVDB_MID_MAX_FILTER];
//static adv_demux_filter_t *sn_silter = NULL;
u32 s_g_lock = 0;
void mid_kvdb_lock()
{
      BOOL ret;
      ret = mtos_sem_take((os_sem_t *)(&s_g_lock),0);
      MT_ASSERT(ret != TRUE);
}
void mid_kvdb_unlock()
{
      BOOL ret;
      ret = mtos_sem_give((os_sem_t *)(&s_g_lock));
      MT_ASSERT(ret != TRUE);
       
}
void stop_filter(adv_demux_filter_t * p_filter);

void  mid_kvdb_filter_free(adv_demux_filter_t *f_filter)
{
//    mid_kvdb_lock();
    adv_demux_filter_t *m_filter = f_filter;
    MT_ASSERT(m_filter != NULL);
    stop_filter(m_filter);
    if(m_filter->p_buffer)
      {
          mtos_free(m_filter->p_buffer);
      }
    memset(m_filter, 0x0, sizeof(adv_demux_filter_t));
  //  mid_kvdb_unlock();
}
extern void kvdb_set_starup_pid(u16 pid);
extern void kvdb_set_turn_status( unsigned int event);
void process_ipanel_pid(void)
{
   // mtos_free(stack);
    kvdb_set_starup_pid(KVDB_MID_IPANEL_PID);
    kvdb_set_turn_status( 0x500);

}
static void pmt_parse(u8 *data, u32 data_size)
{
      u32 sec_len = 0;
      u32 prog_num = 0;
      u32 prog_info_len = 0;
      u32 es_info_len= 0;
      u32 element_pid = 0;
    //  u32 prc_len = 0;
      u8 stream_type = 0;
      u8 *d_ptr = NULL;
     // mid_kvdb_lock();
      sec_len = (((data[1]&0xf)<< 8)| data[2] )- 4;
      sec_len+=3;
      prog_num = ((data[3]<< 8)| data[4]);
      prog_info_len = ((data[10] &0xf << 8) | data[11]);
      //prc_len += 12;
      sec_len -= 12;
      d_ptr = data + 12;
      
      if(prog_info_len != 0)
        d_ptr += prog_info_len;
      
      sec_len -= prog_info_len;
      
      while(sec_len > 0)
        {
                  element_pid = (((d_ptr[1]&0x0f)<< 8)|d_ptr[2]);
                  es_info_len = (((d_ptr[3]&0x0f)<<8)|d_ptr[4]);
                  stream_type = d_ptr[0];
                  if(stream_type == 0x0b)
                    {
                       KVDB_MID_IPANEL_PID = element_pid;
                       KVDB_DRV_DEBUG("KVDB_MID_IPANEL_PID pid = 0x%x\n",KVDB_MID_IPANEL_PID );
                       process_ipanel_pid();
                        break;
                    }
                   /*
                  switch (stream_type)
                    {
                      case 0x0b:
                            KVDB_MID_IPANEL_PID = element_pid;
                        break;
                       default:
                        ;
                        break;
                    }*/
                  d_ptr +=5;
                  d_ptr += es_info_len;
                  sec_len -= 5;
                  sec_len -= es_info_len;
                 
        }
      
     // mid_kvdb_unlock();
}
static void pat_parse(u8 *data, u32 data_size)
{
    u32 section_len = 0;
    u32 program_len = 0;
    u16 prog_no = 0;
    u16 pmt_pid = 0;
    u8 *d_ptr = NULL;
 //   mid_kvdb_lock();
    section_len = (data[1] &0x0f) << 8 | data[2];
  //  KVDB_DRV_DEBUG("section_len = %d \n", section_len);
    section_len += 3;
    program_len = (section_len - 8 - 4);
    d_ptr = &data[8];
    while(program_len > 0)
     {
        prog_no = ((d_ptr[0] << 8) | d_ptr[1]);
        pmt_pid = (((d_ptr[2]&0x0f)<< 8) | d_ptr[3]);
        if(prog_no != 0xfffe)
        {
                kvdb_catch_table(pmt_pid,KVDB_TABLE_ID_PMT );
        }
         program_len -= 4;
         d_ptr += 4;
    }

//    mid_kvdb_unlock();
}
RET_CODE call_back_data_procee(u8 *data, u32 data_size, void *i_filter, u16 pid)
{
    adv_demux_filter_t *m_filter = NULL;
    
    if((!data) || (data_size == 0) || (i_filter == NULL))
      {
        return ERR_FAILURE;
    }
    KVDB_DRV_DEBUG("%s, %d \n ", __FUNCTION__, __LINE__);
    KVDB_DRV_DEBUG("pid === %d \n", pid);
    m_filter = ( adv_demux_filter_t *)i_filter;
    mid_kvdb_filter_free(m_filter);
    if(data[0] == KVDB_TABLE_ID_PAT )
    {
        pat_parse(data, data_size);
    }
    else if(data[0] == KVDB_TABLE_ID_PMT)
    {
        pmt_parse(data, data_size);
    }
    
    return SUCCESS;
}
static void mid_data_monitor()
{
    u32 index = 0;
    RET_CODE ret;
    u8 *data_got = NULL;
    u32 data_size = 0;
    adv_demux_filter_t *d_filter;
    u8 tmp_data[MAX_SECTION_DATA_LEN];
    KVDB_DRV_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);

    while(1)
      {
          for(index = 0; index < KVDB_MID_MAX_FILTER; ++index)
           {
                d_filter = &st_filter[index];
              //  KVDB_DRV_DEBUG("index = %d ,start = %d, used = %d \n",index, d_filter->started , d_filter->used );
                if(d_filter->started && d_filter->used)
                 {

                        ret = dmx_si_chan_get_data(p_ipannel_demux, d_filter->chanid, &data_got, &data_size);
                        if(ret != SUCCESS || data_size == 0)
                          {
                                if(mtos_ticks_get() - d_filter->start_time > 2000)
                                {
                                    mid_kvdb_filter_free(d_filter);
                                }
                                continue;
                          }
                         if(d_filter->notify != NULL)
                          {
                              MT_ASSERT(data_size <  MAX_SECTION_DATA_LEN);
                              memcpy(tmp_data,data_got,data_size );
                         //     KVDB_DRV_DEBUG("DATA COME DATA SIZE =%d, pid = %d \n", data_size, d_filter->pid);
                              d_filter->notify(tmp_data, data_size, d_filter, d_filter->pid);
                              
                        }
                     }
              
               
                
           }
         
          if(KVDB_MID_IPANEL_PID != 0)
          {
             KVDB_DRV_DEBUG("jumb the while ---0x%x \n", KVDB_MID_IPANEL_PID);
             break;
          }
          mtos_task_sleep(30);
      }
}
void mid_kvdb_demux_init()
{
    RET_CODE ret;
    KVDB_DRV_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);
    stack = mtos_malloc(KVDB_MID_DATA_PROCESS);
    MT_ASSERT(stack != NULL);
    mtos_sem_create((os_sem_t *)(&s_g_lock), 0);
    p_ipannel_demux =  dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    memset(st_filter, 0x0, KVDB_MID_MAX_FILTER*sizeof(adv_demux_filter_t));
    ret = mtos_task_create((u8 *)"kvdb_data_monitor",
                                                mid_data_monitor, 
                                                    (void*) 0,
                                                              g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start + 2,
                                                             stack,
                                        KVDB_MID_DATA_PROCESS);
    if(!ret)
      {
          KVDB_DRV_DEBUG("return falue ret = 0x%x \n", ret);
          MT_ASSERT(0);
      }

    
}
void mid_kvdb_demux_deinit(void)
{
         u32 index = 0;
    for(index = 0; index < KVDB_MID_MAX_FILTER; ++index)
    {
         mid_kvdb_filter_free(&st_filter[index]);
    }
    KVDB_MID_IPANEL_PID = 0;
    mtos_task_delete(g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start + 2);
    if(stack)
      {
          mtos_free(stack);

      }
  
}
static adv_demux_filter_t * alloc_filter(u16 pid)
{
    RET_CODE ret;
    dmx_slot_setting_t slot;
    u32 index = 0;
//   u32  index_bak = 0;
   adv_demux_filter_t *filter_n = NULL;
   KVDB_DRV_DEBUG("%s, %d  \n", __FUNCTION__, __LINE__);
 //  mid_kvdb_lock();
   KVDB_DRV_DEBUG("%s, %d  in----- \n", __FUNCTION__, __LINE__);
    for(index = 0; index < KVDB_MID_MAX_FILTER; ++index)
    {
        if(st_filter[index].used == TRUE&& st_filter[index].pid == pid)
          {
            //   mid_kvdb_unlock();
                 return NULL;
          }
    }
        for(index = 0; index < KVDB_MID_MAX_FILTER; ++index)
    {
        if(st_filter[index].used == FALSE)
          {
              filter_n = &st_filter[index];
              break;
          }
    }
    if(index == KVDB_MID_MAX_FILTER)
    {
      //   mid_kvdb_unlock();
         return NULL;
    }
    KVDB_DRV_DEBUG("alloc index = %d \n", index );
    filter_n->pid = pid;
    filter_n->used = 1;
    slot.in = DMX_INPUT_EXTERN0,
    slot.pid = pid;
    slot.type = DMX_CH_TYPE_SECTION;
    ret = dmx_si_chan_open(p_ipannel_demux, &slot, &filter_n->chanid);
    KVDB_DRV_DEBUG("filter_handle_id = 0x%x \n", filter_n->chanid);
    MT_ASSERT(ret == SUCCESS);
    filter_n->p_buffer = mtos_malloc(KVDB_FILTER_BUFFER_SIZE);
    MT_ASSERT( filter_n->p_buffer != NULL);
    ret  = dmx_si_chan_set_buffer(p_ipannel_demux, filter_n->chanid, filter_n->p_buffer, KVDB_FILTER_BUFFER_SIZE);
    MT_ASSERT(ret == SUCCESS);
//    mid_kvdb_unlock();
    return filter_n;
}

void stop_filter(adv_demux_filter_t * p_filter)
{
    RET_CODE ret;
    adv_demux_filter_t *m_filter = p_filter;
    MT_ASSERT(m_filter != NULL);
    if(p_filter->started )
    {
      
      ret = dmx_chan_stop(p_ipannel_demux, m_filter->chanid);
      if (ret == SUCCESS)
      {
        p_filter->started = FALSE;
      }
      KVDB_DRV_DEBUG("ret = %d \n", ret);
        return ;

    }
}
  
void set_filter(adv_demux_filter_t * filter, dmx_filter_setting_t * p_param, notify_call_back notify)
{
  
      RET_CODE ret;
      adv_demux_filter_t * p_filter =  NULL;
  //    mid_kvdb_lock();
      p_filter =  filter;
      MT_ASSERT(p_filter != NULL);
      stop_filter(p_filter);
      KVDB_DRV_DEBUG("set filter_handle_id = 0x%x \n", p_filter->chanid);

      ret = dmx_si_chan_set_filter(p_ipannel_demux,p_filter->chanid, p_param);
      MT_ASSERT(ret == SUCCESS);
      p_filter->notify = notify;
  //    mid_kvdb_unlock();
}
void start_filter(adv_demux_filter_t * filter)
{
    RET_CODE ret;
   // mid_kvdb_lock();
    adv_demux_filter_t* sm_filter = filter;
    MT_ASSERT(sm_filter != NULL);
    KVDB_DRV_DEBUG("start filter_handle_id = 0x%x \n", sm_filter->chanid);

    ret = dmx_chan_start(p_ipannel_demux, sm_filter->chanid);
    sm_filter->start_time = mtos_ticks_get();
    sm_filter->started = TRUE;
   // mid_kvdb_unlock();
}

void kvdb_catch_table(u16 pid, u16 table_id)
{
     adv_demux_filter_t *sn_filter = NULL;
     dmx_filter_setting_t param;
     KVDB_DRV_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);
     KVDB_DRV_DEBUG("pid = %d , table_id = %d \n", pid, table_id);
     memset(&param,0x0, sizeof(dmx_filter_setting_t) );
      sn_filter = alloc_filter(pid);
      if(!sn_filter)
      {
            KVDB_DRV_DEBUG("sn_filter is NULL \n");
            return;
      }
      param.continuous = FALSE;
      param.en_crc = TRUE;
      param.req_mode =  DMX_REQ_MODE_SINGLE_SECTION;

      param.value[0] = table_id;
      param.mask[0] = 0xff,      
      set_filter( sn_filter, &param, call_back_data_procee);
      KVDB_DRV_DEBUG("start_filter ----\n");
      start_filter(sn_filter);
      return;
}

