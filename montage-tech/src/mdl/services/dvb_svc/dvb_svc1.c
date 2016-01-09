/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_misc.h"

#include "class_factory.h"

#include "drv_dev.h"
#include "dmx.h"

#include "mdl.h"

#include "service.h"
#include "dvb_protocol.h"

#include "fcrc.h"
#include "lib_util.h"
#include "simple_queue.h"

#include "dvb_svc.h"
#include "hal_misc.h"

/*!
  maxinum of section buffers
  */
#define DVB_MAX_TABLE (64)

/*!
  maxinum of section buffers
  */
#define DVB_MAX_SECTION_NUM (128)

/*!
  Message queue depth, worse case is PMT number in a PAT
  */
#define DVB_MSG_DEPTH (128)

/*!
  Message queue size
  */
#define DVB_ATTACH_SIZE ((sizeof(dvb_request_t)) * (DVB_MSG_DEPTH))

/*!
  section data buffer size
  */
#define DATA_BUFFER_SIZE          (4*(KBYTES)+192)

/*!
  dvb reserved buffer size
  */
#define RESERVED_BUFFER_SIZE          (10*(KBYTES))
/*!
  filter alloc result definition
  */
typedef enum
{
  ALLOC_SUC = 0,
  ALLOC_FILTER_FULL,
  ALLOC_FAIL
} alloc_ret_t;

/*!
  Internal state machine
  */
typedef enum
{
  /*!
    Idle state
    */
  DVB_IDLE = 0,
  /*!
    Allocating PTI filter
    */
  DVB_RUNNING,
  /*!
    dvb task stoped
    */
  DVB_DO_STOP
} dvb_state_t;

/*!
  PSI table info structure
  */
typedef struct
{
  /*!
    Unique PSI table id
    */
  u16 table_id;
  /*!
    flag this section use which mode /see request_mode_t
    */
  request_mode_t request_mode;
  /*!
    Callback function for request
    */
  request_proc_t request;
    /*!
    Callback function for free
    */
  free_proc_t free;
  /*!
    Callback function for parse
    */
  parse_proc_t parse;
  /*!
    parameter list which have been requested
    */
  u32 param_list[DMX_HW_FILTER_SUM];
} dvb_table_info_t;

typedef struct 
{
  /*!
    table map
    */
  dvb_table_info_t table_map[DVB_MAX_TABLE];
  
  /*!
    Free section queue id. All free dvb's section buffer will be pushed
    into this queue
    */
  u8 free_queue;

  /*!
    Free data buffer queue id. All free data buffer will be pushed
    into this queue
    */
  u8 free_buffer_queue;

    /*!
    using data buffer queue id. All using data buffer will be pushed
    into this queue
    */
  u8 using_buffer_queue;

  /*!
    Indicating how many filter dvb have allocated. If this value was
    decreased to zero it means dvb has nothing to do.
    */
  u16 pending_section_num;

  /*!
    PTI driver's handle
    */
  dmx_device_t *p_dmx_dev;
  
  /*!
    dvb's section buffer start address
    */
  dvb_section_t *p_sec;

  /*!
    data buffer start address
    */
  u8 *p_data_buffer;

  /*!
    simple queue handle
    */
  class_handle_t q_handle;
  /*!
    expected current ts in
    */
  u8 current_ts_in;  
  /*!
    expected new ts in
    */
  u8 new_ts_in;  
  /*!
    main ts in
   */
  u8 main_ts_in;    
  svc_container_t *p_container;
} dvb_priv_t;

typedef struct
{
  dvb_t *p_this;
  /*!
    Waiting section queue id. Means the section is waiting filter.
    */
  u8 wait_queue;
  
  /*!
    block section queue id. All block dvb's section buffer will be pushed
    into this stack. If need request a section as cycle mode, the section will
    push here after finish previous process.
    */
  u8 block_queue;

  /*!
    using section queue id. All alloc_filter dvb's section buffer will be pushed
    into this stack. 
    */
  u8 using_queue;
  /*!
    state
    */
  dvb_state_t state;

} dvb_svc_data_t;

/*!
  dmx using hardware or software
*/
static BOOL g_use_soft_filter = FALSE;
static void dvb_update_ts(service_t *p_svc);

static void show_sec(dvb_priv_t *p_priv)
{
  int i = 0;
  dvb_section_t *p_sec = NULL;

  OS_PRINTF("show section ~~~~~\n");
  for(i = 0; i < DVB_MAX_SECTION_NUM; i++)
  {
    p_sec = p_priv->p_sec + i;
    OS_PRINTF("sec[%d]: id %d, table_id 0x%x, pid 0x%x\n", p_sec->id, p_sec->table_id, p_sec->pid);
  }
}

static void do_request_cmd(service_t *p_svc, dvb_request_t *p_para)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  u16 i = 0;
  dvb_section_t sec = {0};

  sec.table_id = p_para->table_id;
  sec.interval_time = p_para->period;
  sec.p_svc = p_svc;
  sec.ts_in = p_para->ts_in;

  for(i = 0; i < DVB_MAX_TABLE; i++)
  {
    if(p_priv->table_map[i].request == NULL)
    {
      break;  //found the end
    }
    else if((p_priv->table_map[i].table_id == p_para->table_id)
      && (p_priv->table_map[i].request_mode == p_para->req_mode))
    {
      sec.table_map_index = i;
      //set filter code
      if(p_para->filter_code != NULL)
      {
        memcpy(sec.filter_code, p_para->filter_code, MAX_FILTER_MASK_BYTES);
      }
      //set filter mask
      if(p_para->filter_mask != NULL)
      {
        memcpy(sec.filter_mask, p_para->filter_mask, MAX_FILTER_MASK_BYTES);
      }
      sec.r_context = p_para->context;
      p_priv->table_map[i].request(&sec, p_para->para1, p_para->para2);
      break;
    }
  }
    //OS_PRINTF("xxx req table_id 0x%x, t %d\n", p_para->table_id, mtos_ticks_get());
}

static BOOL match_free_sec(dvb_priv_t *p_priv,
  dvb_section_t *p_sec, dvb_request_t *p_para)
{
  dvb_table_info_t *p_table = p_priv->table_map + p_sec->table_map_index;
  return ((p_sec->table_id == p_para->table_id)
        && (p_table->request_mode == p_para->req_mode));
}

static void free_filter(dvb_priv_t *p_priv, dvb_section_t *p_sec)
{
  u16 buf_num = 0;
  u16 i = 0;
  u16 buf_id = 0;
  MT_ASSERT(p_sec->dmx_handle != 0xffff);

  buf_num = get_simple_queue_len(p_priv->q_handle, p_priv->using_buffer_queue);
  for (i = 0; i < buf_num; i++)
  {
    pop_simple_queue(p_priv->q_handle, p_priv->using_buffer_queue, &buf_id);
    if (buf_id == p_sec->buf_id)
    {
      push_simple_queue(p_priv->q_handle, p_priv->free_buffer_queue, buf_id);
      break;
    }
    else
    {
      push_simple_queue(p_priv->q_handle, p_priv->using_buffer_queue, buf_id);
    }
  }
  MT_ASSERT(i < buf_num);
  
  dmx_chan_stop(p_priv->p_dmx_dev, p_sec->dmx_handle);
  dmx_chan_close(p_priv->p_dmx_dev, p_sec->dmx_handle);
  
  //pti_dev_channel_free(p_priv->p_pti_dev, p_sec->pti_handle);
  p_sec->dmx_handle = 0xffff;
  p_sec->buf_id = 0xffff;
}

static void do_free_cmd(service_t *p_svc, dvb_request_t *p_para)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  u16 i = 0;
  u16 sec_id = 0;
  dvb_section_t *p_sec = NULL;
  u16 sec_num = 0;
  class_handle_t q_handle = p_priv->q_handle;

  //A.sreach section in filter array(it's used)-------->
  sec_num = get_simple_queue_len(q_handle, p_svc_data->using_queue);
  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->using_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    MT_ASSERT(sec_id == p_sec->id);

    if(match_free_sec(p_priv, p_sec, p_para))
    {
      free_filter(p_priv, p_sec);
      push_simple_queue(q_handle, p_priv->free_queue, sec_id);
      if(p_priv->table_map[p_sec->table_map_index].free != NULL)
      {
        p_priv->table_map[p_sec->table_map_index].free();
      }
    }
    else
    {
      //push to using queue back
      push_simple_queue(q_handle, p_svc_data->using_queue, sec_id);
    }
  }
  
  //B.sreach section in block queue------------>
  sec_num = get_simple_queue_len(q_handle, p_svc_data->block_queue);

  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->block_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    MT_ASSERT(sec_id == p_sec->id);
    if(match_free_sec(p_priv, p_sec, p_para))
    {
      //push to free queue
      push_simple_queue(q_handle, p_priv->free_queue, sec_id);
      if(p_priv->table_map[p_sec->table_map_index].free != NULL)
      {
        p_priv->table_map[p_sec->table_map_index].free();
      }
    }
    else
    {
      MT_ASSERT(p_sec->dmx_handle == 0xffff);
      //push to block queue back
      push_simple_queue(q_handle, p_svc_data->block_queue, sec_id);
    }
  }

  
  //C.sreach section in wait queue--------------->
  sec_num = get_simple_queue_len(q_handle, p_svc_data->wait_queue);

  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->wait_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    MT_ASSERT(sec_id == p_sec->id);
    if(match_free_sec(p_priv, p_sec, p_para))
    {
      //push to free queue
      push_simple_queue(q_handle, p_priv->free_queue, sec_id);
      if(p_priv->table_map[p_sec->table_map_index].free != NULL)
      {
        p_priv->table_map[p_sec->table_map_index].free();
      }
    }
    else
    {
      //MT_ASSERT(p_sec->dmx_handle == 0xffff);
      //push to wait queue back
      push_simple_queue(q_handle, p_svc_data->wait_queue, sec_id);
    }
  }
}

static void do_ts_update(service_t *p_svc, u32 table_id, u8 ts_in)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;

  if(ts_in != p_priv->current_ts_in)
  {
      p_priv->current_ts_in = ts_in;
      OS_PRINTF("$$$$%s,line %d, %x\n",__FUNCTION__,__LINE__, ts_in);
      dvb_update_ts(p_svc);    
 }
}

static void msg_proc(handle_t handle, os_msg_t *p_msg)
{
  service_t *p_svc = (service_t *)handle;
  os_msg_t notify = {0};
  switch(p_msg->content)
  {
    case DVB_REQUEST:
      do_request_cmd(p_svc, (dvb_request_t *)p_msg->para1);
      break;
    case DVB_FREE:
      do_free_cmd(p_svc, (dvb_request_t *)p_msg->para1);
      OS_PRINTF("free table %x context %x\n",
        ((dvb_request_t *)p_msg->para1)->table_id, ((dvb_request_t *)p_msg->para1)->context);
      notify.content = DVB_FREED;
      notify.para1 = p_msg->para1;
      notify.para2 = sizeof(dvb_request_t);
      p_svc->notify(p_svc, &notify);
      break;
    case DVB_TABLE_TS_UPDATE:
      do_ts_update(p_svc, p_msg->para1, p_msg->para2);
      break;
    default:
      MT_ASSERT(0);
      break;
  }
}

static void dvb_active_block_sec(service_t *p_svc)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  class_handle_t q_handle = p_priv->q_handle;
  dvb_section_t *p_sec = NULL;
  u16 sec_id = 0;
  u16 i = 0;
  u16 sec_num = get_simple_queue_len(q_handle, p_svc_data->block_queue);
  u32 cur_ticks = mtos_ticks_get();

  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->block_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    MT_ASSERT(p_sec->dmx_handle == 0xffff);
    MT_ASSERT(sec_id == p_sec->id);
    if(((cur_ticks - p_sec->start_ticks)*10 > p_sec->interval_time) 
        || (p_sec->ts_in != p_priv->current_ts_in))  //it's turn    
    {
      push_simple_queue(q_handle, p_svc_data->wait_queue, sec_id);
    }
    else  //push it back
    {
      push_simple_queue(q_handle, p_svc_data->block_queue, sec_id);
    }
  }
}

static dmx_ch_type_t trans_type(filter_type_t filter_mode)
{
  return (FILTER_TYPE_TSPKT == filter_mode) ?
    DMX_CH_TYPE_TSPKT : DMX_CH_TYPE_SECTION;
}

static alloc_ret_t alloc_filter(dvb_priv_t *p_priv, dvb_section_t *p_sec)
{
  RET_CODE pti_ret = ERR_FAILURE;
  dmx_chanid_t dmx_handle = 0xffff;
  u16 buf_id = 0;
  u8 *p_buffer = NULL;
  //pti_alloc_param_t alloc_param = {0};
  dmx_slot_setting_t slot = {0};
  dmx_filter_setting_t filter = {0};
  request_mode_t mode = DATA_SINGLE;
  BOOL ret_boo = FALSE;
  u32 buf_size = 0;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif

  MT_ASSERT(NULL != p_sec);

  MT_ASSERT(p_sec->dmx_handle == 0xffff);
  p_sec->p_buffer = NULL;
  mode = p_priv->table_map[p_sec->table_map_index].request_mode;
  slot.type =  trans_type(p_sec->filter_mode);
  slot.pid = p_sec->pid;
  slot.in = p_priv->current_ts_in;  
  if(chip_ic_id == IC_CONCERTO)
  {
    slot.muldisp = DMX_SLOT_MUL_SEC_DIS_DISABLE;//for bug 63129
  }
  //alloc_param.type = p_sec->filter_mode;
  //alloc_param.pid = p_sec->pid;
  if(p_sec->filter_code != NULL)
  {
    memcpy(filter.value, p_sec->filter_code, MAX_FILTER_MASK_BYTES);
  }
  if(p_sec->filter_mask != NULL)
  {
    memcpy(filter.mask, p_sec->filter_mask, MAX_FILTER_MASK_BYTES);
  }
  //p_sec->crc_enable = TRUE;
  filter.en_crc = p_sec->crc_enable;

  //hack for warriors bug 22712,37060, all use soft filter.
  if(p_sec->use_soft_filter == TRUE)// && mode == DATA_SINGLE)
  {
    slot.soft_filter_flag = TRUE;
  }
  
  if(DMX_CH_TYPE_TSPKT == slot.type)
    filter.ts_packet_mode = DMX_ONE_MODE;

    ret_boo = pop_simple_queue(p_priv->q_handle, p_priv->free_buffer_queue, &buf_id);
    if(!ret_boo)
    {
      return ALLOC_FILTER_FULL;
    }
    //MT_ASSERT(ret_boo == TRUE);

 
  //OS_PRINTF("xxx alloc pti table_id 0x%x, t %d\n", p_sec->table_id, mtos_ticks_get());
  //pti_ret = pti_dev_channel_alloc(p_priv->p_pti_dev, &alloc_param, &pti_handle);
  pti_ret = dmx_si_chan_open(p_priv->p_dmx_dev, &slot, &dmx_handle);
  if(SUCCESS == pti_ret)
  {
    MT_ASSERT(dmx_handle != 0xffff);
    
    //ret_boo = pop_simple_queue(p_priv->q_handle, p_priv->free_buffer_queue, &buf_id);
    //MT_ASSERT(ret_boo == TRUE);
    
    if(mode != DATA_MULTI)
    {
      filter.continuous = FALSE;
      p_buffer = p_priv->p_data_buffer + buf_id * DATA_BUFFER_SIZE;
      buf_size = DATA_BUFFER_SIZE;
      if(DMX_CH_TYPE_TSPKT == slot.type)
      {
        buf_size = 192;
        OS_PRINTF("SET buf size 188 in ts packet mode \n");
      }
    }
    else
    {
       filter.continuous = TRUE;
       p_buffer = p_sec->p_ext_buf_list->p_buf;
       buf_size = p_sec->p_ext_buf_list->size;
    }
    
    p_buffer = (u8 *)(((u32)p_buffer + 7) & (~7)); //align 4

    pti_ret = dmx_si_chan_set_buffer(p_priv->p_dmx_dev, dmx_handle, p_buffer, buf_size);
    MT_ASSERT(SUCCESS == pti_ret);

    pti_ret = dmx_si_chan_set_filter(p_priv->p_dmx_dev, dmx_handle, &filter);
    MT_ASSERT(SUCCESS == pti_ret);
    
    pti_ret = dmx_chan_start(p_priv->p_dmx_dev, dmx_handle);
    MT_ASSERT(SUCCESS == pti_ret);

    p_sec->start_ticks = mtos_ticks_get();
    push_simple_queue(p_priv->q_handle, p_priv->using_buffer_queue, buf_id);
    p_sec->dmx_handle = dmx_handle;
    p_sec->buf_id = buf_id;
    
    return ALLOC_SUC;
  }
  else if(0)//(ERR_FAILURE == pti_ret)  //what's the fail??
  {
    return ALLOC_FAIL;
  }
  else
  {
    push_simple_queue(p_priv->q_handle, p_priv->free_buffer_queue, buf_id);
    return ALLOC_FILTER_FULL;
  }
}

static void dvb_alloc_filter(service_t *p_svc)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  class_handle_t q_handle = p_priv->q_handle;
  dvb_section_t *p_sec = NULL;
  u16 sec_id = 0;
  alloc_ret_t ret = ALLOC_SUC;
  u16 i = 0;
  u16 sec_num = get_simple_queue_len(q_handle, p_svc_data->wait_queue);
  
  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->wait_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    MT_ASSERT(sec_id == p_sec->id);
    // has jobs to do, try to allocate PTI filter
    ret = alloc_filter(p_priv, p_sec);
    if(ALLOC_SUC == ret)
    {
      MT_ASSERT(p_sec->dmx_handle != 0xffff);
      // alloc filter suc, push it to using q
      push_simple_queue(q_handle, p_svc_data->using_queue, sec_id);
      break;
    }
    else if(ALLOC_FILTER_FULL == ret)
    {
      // no pti filter, push it back
      push_simple_queue_on_head(q_handle, p_svc_data->wait_queue, sec_id);
      break;
    }
    else if(ALLOC_FAIL == ret)
    {
      //in this case, have a same PID between ts and section request
      //push it back, search continue
      OS_PRINTF("xxx dvb alloc filter failed continue... \n");
      push_simple_queue(q_handle, p_svc_data->wait_queue, sec_id);
    }
  }
}

static void process_departed_sec(service_t *p_svc, dvb_section_t *p_sec)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  request_mode_t req_mode = 
    p_priv->table_map[p_sec->table_map_index].request_mode;
  
  if(DATA_SINGLE == req_mode)
  {
    free_filter(p_priv, p_sec);
    push_simple_queue(p_priv->q_handle, p_priv->free_queue, p_sec->id);
  }
  else if(DATA_PERIODIC == req_mode)
  {
    free_filter(p_priv, p_sec);
    push_simple_queue(p_priv->q_handle, p_svc_data->block_queue, p_sec->id);
  }
  else if(DATA_MULTI == req_mode)
  {
    MT_ASSERT(p_sec->dmx_handle != 0xffff);
    if(p_sec->ts_in != p_priv->current_ts_in)
     {
        free_filter(p_priv, p_sec);
        p_sec->ts_in = p_priv->current_ts_in;
        push_simple_queue(p_priv->q_handle, p_svc_data->wait_queue, p_sec->id); 
     }
    else
        push_simple_queue(p_priv->q_handle, p_svc_data->using_queue, p_sec->id); 
  }
}

static BOOL check_sec_crc(dvb_priv_t *p_priv, dvb_section_t *p_sec)
{
  u16 section_len = 0;
  u32 crc = 0;
  
  section_len = MAKE_WORD(p_sec->p_buffer[2], (p_sec->p_buffer[1] & 0x0f)); 
  section_len += 3;
  crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,
            0xFFFFFFFF,  (u8 *)p_sec->p_buffer, section_len - 4);
  //OS_PRINTF("crc check error table_id 0x%x, value: 0x%x, value2 : 0x%x\n",
  //	p_sec->table_id, crc, make32(p_sec->p_buffer + section_len - 4));
  return (crc == make32(p_sec->p_buffer + section_len -4));
}

static RET_CODE external_soft_filter(dvb_section_t *p_sec, u8 *p_data, u32 data_len)
{
  RET_CODE ret = ERR_FAILURE;
  s32 cmp_rslt = 0;
  u8 block_data[MAX_SOFT_FILTER_MASK_BYTES] = {0};
  u8 i = 0;

  MT_ASSERT(NULL != p_sec);
  MT_ASSERT(NULL != p_data);
  
  if((0 == p_sec->filter_exmask_size) ||(MAX_SOFT_FILTER_MASK_BYTES < p_sec->filter_exmask_size))
  {
      return ERR_FAILURE;
  }

  if((MAX_FILTER_MASK_BYTES + p_sec->filter_exmask_size) > data_len)
  {
      /*OS_PRINTF("error: the data len[%d] is less than filer mask size!,sec id[0x%x] \n",
                          p_data_block->len,p_sec->id);*/
      return ERR_FAILURE;
  }

  for(i = 0; i < p_sec->filter_exmask_size; i++)
  {
    block_data[i] = (p_data[MAX_FILTER_MASK_BYTES + i])&(p_sec->filter_exmask[i]);
  }

  cmp_rslt = memcmp(p_sec->filter_excode,block_data,p_sec->filter_exmask_size);
  if(0 == cmp_rslt)
  {
      return SUCCESS;
  }
  
  return ret;
}

static void do_time_out(service_t *p_svc, dvb_section_t *p_sec)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  request_mode_t req_mode = 
    p_priv->table_map[p_sec->table_map_index].request_mode;
  
  if(DATA_SINGLE == req_mode)
  {
    os_msg_t msg = {0};
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.para1 = p_sec->table_id;
    msg.para2 = (u32)p_sec;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
  }

  process_departed_sec(p_svc, p_sec);
}

static void dvb_process_filter(service_t *p_svc)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  class_handle_t q_handle = p_priv->q_handle;
  dvb_section_t *p_sec = NULL;
  u8 *p_data = NULL;
  u16 i = 0;
  u32 data_len = 0;
  RET_CODE ret = 0;
  u16 sec_id = 0;
  u16 sec_num = 0;
  u32 cur_ticks = mtos_ticks_get();
  u32 sr = 0;

  sec_num = get_simple_queue_len(q_handle, p_svc_data->using_queue);
  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->using_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    //MT_ASSERT(p_sec->pti_handle != NULL);
    if(p_sec->dmx_handle == 0xffff)
    {
      MT_ASSERT(0);
    }
    MT_ASSERT(sec_id == p_sec->id);

    //ret = pti_dev_get_data(p_priv->p_pti_dev, p_sec->pti_handle, &data_block);
    ret = dmx_si_chan_get_data(p_priv->p_dmx_dev, p_sec->dmx_handle, &p_data, &data_len);

    /*our chip just support 12 bytes filer. if need to filter the additional bytes more than 12 bytes
        we use external soft filter*/
    if((SUCCESS == ret)&&(TRUE == p_sec->b_filter_external))
    {
        ret =  external_soft_filter(p_sec, p_data, data_len);
    }

    if(SUCCESS == ret)  //a filter ready
    {
      if (data_len > (4 * KBYTES))
      {
        OS_PRINTF("warning-->>data len invaild %d, req pid[0x%x], dmx[0x%x]\n", 
            data_len, p_sec->pid, p_sec->dmx_handle);
        data_len = 4 * KBYTES;
        //MT_ASSERT(0);
      }

      if (p_priv->table_map[p_sec->table_map_index].request_mode == DATA_MULTI)
      {
        p_sec->p_buffer = p_priv->p_data_buffer + p_sec->buf_id * DATA_BUFFER_SIZE;
        mtos_critical_enter(&sr);
        memcpy(p_sec->p_buffer, p_data, data_len);
        mtos_critical_exit(sr);
      }
      else
      {
        p_sec->p_buffer = p_data;
      }

      if(p_sec->direct_data)
      {
        os_msg_t msg = {0};
        msg.content = DVB_ONE_SEC_FOUND;
        OS_PRINTF("DVB_ONE_SEC_FOUND %x\n", p_sec->table_id);
        msg.para1 = (u32)p_sec;
        msg.para2 = data_len;
        process_departed_sec(p_svc, p_sec);
        p_svc->notify(p_svc, &msg);
      }
      else if(!p_sec->crc_enable || 
        (p_sec->crc_enable && check_sec_crc(p_priv, p_sec)))
      {
        //do parse
        p_priv->table_map[p_sec->table_map_index].parse(p_svc, p_sec);
        process_departed_sec(p_svc, p_sec);
      }
      else //check crc fail
      {
        do_time_out(p_svc, p_sec);
      }
    }
    else if((cur_ticks - p_sec->start_ticks)*10 > p_sec->timeout)  //time out
    {
      do_time_out(p_svc, p_sec);
      p_sec->start_ticks = cur_ticks;
    }
    else
    {
      //push it back to using
      push_simple_queue(q_handle, p_svc_data->using_queue, sec_id);
    }
  }
}

static void dvb_update_ts(service_t *p_svc)
{
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  class_handle_t q_handle = p_priv->q_handle;
  dvb_section_t *p_sec = NULL;
  u16 i = 0;
  u16 sec_id = 0;
  u16 sec_num = 0;

  sec_num = get_simple_queue_len(q_handle, p_svc_data->wait_queue);
  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->wait_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    p_sec->ts_in = p_priv->current_ts_in;
    if(p_sec->dmx_handle != 0xffff)
        free_filter(p_priv, p_sec);    
    push_simple_queue_on_head(q_handle, p_svc_data->wait_queue, sec_id);
  }     

  sec_num = get_simple_queue_len(q_handle, p_svc_data->using_queue);
  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->using_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    if(p_sec->ts_in != p_priv->current_ts_in)
    {
        p_sec->ts_in = p_priv->current_ts_in;
        if(p_sec->dmx_handle != 0xffff)
            free_filter(p_priv, p_sec);        
        push_simple_queue_on_head(q_handle, p_svc_data->wait_queue, sec_id);        
    }
    else
        push_simple_queue_on_head(q_handle, p_svc_data->using_queue, sec_id);
  }  

  sec_num = get_simple_queue_len(q_handle, p_svc_data->block_queue);
  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->block_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    p_sec->ts_in = p_priv->current_ts_in;
    if(p_sec->dmx_handle != 0xffff)
        free_filter(p_priv, p_sec);
    push_simple_queue_on_head(q_handle, p_svc_data->block_queue, sec_id);
  }  
}

static void sm_proc(handle_t handle)
{
  service_t *p_svc = (service_t *)handle;
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);

  switch(p_svc_data->state)
  {
    case DVB_IDLE:
      break;
    case DVB_RUNNING:
      dvb_active_block_sec(p_svc);
      dvb_update_ts(p_svc);
      dvb_alloc_filter(p_svc);
      dvb_process_filter(p_svc);
      break;
    default:
      MT_ASSERT(0);
    break;
  }

}

static void on_decorate_cmd(handle_t handle, os_msg_t *p_msg)
{
  // Our command need mem copy
  if(DVB_REQUEST == p_msg->content
    || DVB_FREE == p_msg->content)
  {
    p_msg->is_ext = TRUE;
  }
  else
  {
    p_msg->is_ext = FALSE;
  }
}

static void register_table(class_handle_t handle, dvb_register_t *p_para)
{
  u16 i = 0;
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;
  dvb_table_info_t *p_table = p_priv->table_map;
  
  MT_ASSERT(p_para != NULL);

  for(i = 0; i < DVB_MAX_TABLE; i ++)
  {
    //go to end , don't found duplicate register, right
    if(p_table[i].request == NULL)
    {
      p_table[i].table_id = p_para->table_id;
      p_table[i].request_mode = p_para->req_mode;
      p_table[i].request = p_para->request_proc;
      p_table[i].free = p_para->free_proc;
      p_table[i].parse = p_para->parse_proc;
      memset(p_table[i].param_list, 0xFF, sizeof(p_table[i].param_list));
      return;
    }
    else if((p_table[i].table_id == p_para->table_id) 
      && (p_table[i].request_mode == p_para->req_mode))
    {
      //found duplicate register, do nothing
      return;
    }
  }
  //can't support any more tables
  MT_ASSERT(0);
}

static service_t * get_svc_instance(class_handle_t handle, u32 context)
{
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;
  dvb_svc_data_t *p_svc_data = NULL;
  service_t *p_svc = NULL;

  p_svc = p_priv->p_container->get_svc(p_priv->p_container, context);
  if(NULL == p_svc)
  {
    p_svc = construct_svc(context);
    p_svc->msg_proc = msg_proc;
    p_svc->sm_proc = sm_proc;
    p_svc->on_decorate_cmd = on_decorate_cmd;
    p_svc->allocate_data_buffer(p_svc, sizeof(dvb_svc_data_t));
    p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);

    p_svc_data->wait_queue = create_simple_queue1(p_priv->q_handle,
      DVB_MAX_SECTION_NUM);
    p_svc_data->block_queue = create_simple_queue1(p_priv->q_handle,
      DVB_MAX_SECTION_NUM);
    p_svc_data->using_queue = create_simple_queue1(p_priv->q_handle,
      DMX_HW_FILTER_SUM);
    p_svc_data->state = DVB_RUNNING;
    p_svc_data->p_this = p_this;
    p_priv->p_container->add_svc(p_priv->p_container, p_svc);
  }
  return p_svc;
}

static void rm_svc_instance(class_handle_t handle, service_t *p_svc)
{
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;

  p_priv->p_container->remove_svc(p_priv->p_container, p_svc);
}


static u32 get_mem_size(class_handle_t handle,  BOOL dynamic, u8 filter_num, u8 sec_num)
{
  u32 size = 0;
  //dvb_t *p_this = handle;
  //dvb_priv_t *p_priv = p_this->p_data;

  //pti_dev_get_mem_request(p_priv->p_pti_dev, dynamic, (s8)filter_num, &size);
  size = filter_num * DATA_BUFFER_SIZE;
  size += DVB_ATTACH_SIZE + DVB_MAX_SECTION_NUM * sizeof(dvb_section_t) + RESERVED_BUFFER_SIZE;
  return size;
}

static void start(class_handle_t handle, u8 *p_mem,
  u32 size, BOOL dynamic, u8 filter_num, u8 sec_num)
{
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;
  svc_container_t *p_container = p_priv->p_container;
  u8 *p_temp = NULL;
  u16 i = 0;
  //u32 buf_size = 0;
  //u32 pti_buf_size = 0;
  u32 total_size = 0;

  p_container->lock(p_container);
  total_size = get_mem_size(handle, FALSE, filter_num,sec_num);

  MT_ASSERT((p_mem != NULL) && (size >= total_size));
  MT_ASSERT(NULL == p_priv->p_sec);

  //p_temp = (u8 *)(((u32)p_mem + 3) & (~3)); //align 4
  p_temp = p_mem;
  p_priv->p_sec = (dvb_section_t *)p_temp;
  p_temp += DVB_MAX_SECTION_NUM * sizeof(dvb_section_t);
  memset(p_priv->p_sec, 0, sizeof(dvb_section_t) * DVB_MAX_SECTION_NUM);

  //create free queue
  p_priv->free_queue = create_simple_queue1(p_priv->q_handle,
    DVB_MAX_SECTION_NUM);

  for(i = 0; i < DVB_MAX_SECTION_NUM; i ++)
  {
    p_priv->p_sec[i].id = i;
    p_priv->p_sec[i].buf_id = 0xffff;
    p_priv->p_sec[i].ts_in = p_priv->main_ts_in;
    push_simple_queue(p_priv->q_handle, p_priv->free_queue, i);
  }

  //create free data buffer queue
  p_priv->free_buffer_queue = create_simple_queue1(p_priv->q_handle,
    DMX_HW_FILTER_SUM);

  for(i = 0; i < DMX_HW_FILTER_SUM; i ++)
  {
    push_simple_queue(p_priv->q_handle, p_priv->free_buffer_queue, i);
  }

  p_priv->using_buffer_queue = create_simple_queue1(p_priv->q_handle,
    DMX_HW_FILTER_SUM);

  //attach request buffer
  memset(p_temp, 0, DVB_ATTACH_SIZE);
  mtos_messageq_attach(p_container->get_msgq_id(p_container),
    p_temp, sizeof(dvb_request_t), DVB_MSG_DEPTH);

  p_temp += DVB_ATTACH_SIZE;
  p_temp = (u8 *)(((u32)p_temp + 7) & (~7)); //align 4  // 8 byte
  p_priv->p_data_buffer = p_temp;
  //pti_dev_get_mem_request(p_priv->p_pti_dev, dynamic,
    //filter_num, &pti_buf_size);
  //pti_dev_parser_init(p_priv->p_pti_dev, dynamic,
    //filter_num, p_temp, pti_buf_size);
    

  p_container->unlock(p_container);
}

static void stop(class_handle_t handle)
{
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;
  svc_container_t *p_container = p_priv->p_container;

  p_container->lock(p_container);

  // Todo: TBD, need release all buffers in services
  
  p_container->unlock(p_container);
}

static void filter_data_error(service_t *p_svc, dvb_section_t *p_sec)
{
  os_msg_t msg = {0};
  msg.content = DVB_TABLE_TIMED_OUT;
  msg.is_ext = 0;
  msg.para1 = p_sec->table_id;
  msg.para2 = (u32)p_sec;
  msg.context = p_sec->r_context;
  p_svc->notify(p_svc, &msg);
}

static void alloc_section(class_handle_t handle, dvb_section_t *p_src_sec)
{
  u16 id = 0;
  service_t *p_svc = p_src_sec->p_svc;
  dvb_section_t *p_sec = NULL;
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  alloc_ret_t ret = ALLOC_SUC;
  BOOL ret_boo = FALSE;
  class_handle_t q_handle = p_priv->q_handle;

  MT_ASSERT(p_src_sec != NULL);

  ret_boo = pop_simple_queue(p_priv->q_handle, p_priv->free_queue, &id);

  //for warriors bug 22712,37060, prj shoudl select soft filter.
  p_src_sec->use_soft_filter = g_use_soft_filter;

  //fix me
  if(!ret_boo)
  {
    return; //drap it
  }

  if(id >= DVB_MAX_SECTION_NUM)
  {
    return; //drap it
  }
  
  MT_ASSERT(ret_boo == TRUE);
  p_sec = p_priv->p_sec + id;
  //MT_ASSERT(id == p_sec->id);
  if(id != p_sec->id)
  {
    OS_PRINTF("DVB WARNING!!!id %d, p_sec->id %d\n", id, p_sec->id);
    show_sec(p_priv);
    id = p_sec->id;

   // MT_ASSERT(0);
  }

  memcpy(p_sec, p_src_sec, sizeof(dvb_section_t));
  p_sec->dmx_handle = 0xffff;
  p_sec->p_buffer = NULL;
  p_sec->id = id;
  p_sec->buf_id = 0xffff;
  p_sec->ts_in = p_priv->current_ts_in;
  
//    push_simple_queue(p_priv->q_handle, p_svc_data->wait_queue, id);
  
  ret = alloc_filter(p_priv, p_sec);
  if(ALLOC_SUC == ret)
  {
    MT_ASSERT(p_sec->dmx_handle != 0xffff);
    // alloc filter suc, push it to using q
    push_simple_queue(q_handle, p_svc_data->using_queue, id);
  }
  else
  {
    OS_PRINTF("xxx dvb_alloc_section failed continue... \n");
    push_simple_queue(p_priv->q_handle, p_svc_data->wait_queue, id);
  }
  
}

void dvb_svc_use_soft(BOOL use_soft_filter)
{
  g_use_soft_filter = use_soft_filter;
}

BOOL dvb_svc_is_use_soft(void)
{
  return g_use_soft_filter;
}

handle_t dvb_init_1(u32 priority, u32 stack_size, u8 main_ts_in)
{
  dvb_t *p_this = mtos_malloc(sizeof(dvb_t));
  dvb_priv_t *p_priv = mtos_malloc(sizeof(dvb_priv_t));
  svc_container_t *p_container = construct_svc_container("dvb",
    priority, stack_size, DVB_MSG_DEPTH);
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_priv != NULL);

  memset(p_priv, 0, sizeof(dvb_priv_t));
  p_this->start = start;
  p_this->stop = stop;
  p_this->get_svc_instance = get_svc_instance;
  p_this->remove_svc_instance = rm_svc_instance;
  p_this->register_table = register_table;
  p_this->get_mem_size = get_mem_size;
  p_this->alloc_section = alloc_section;
  p_this->filter_data_error = filter_data_error;
  
  p_priv->p_container = p_container;
  p_priv->p_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  p_priv->q_handle = class_get_handle_by_id(SIMPLE_Q_CLASS_ID);
  p_priv->main_ts_in = main_ts_in;
  p_priv->current_ts_in = main_ts_in;
  p_this->p_data = p_priv;
  class_register(DVB_CLASS_ID, p_this);
  return p_this;
}

