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
#include "pti.h"

#include "mdl.h"

#include "service.h"
#include "dvb_protocol.h"

#include "fcrc.h"
#include "lib_util.h"
#include "simple_queue.h"

#include "dvb_svc.h"

/*!
  maxinum of section buffers
  */
#define DVB_MAX_TABLE (32)

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
#define DVB_ATTACH_SIZE(x) ((sizeof(dvb_request_t)) * (x))

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
  //u32 param_list[PTI_HW_FILTER_SUM];
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
    Indicating how many filter dvb have allocated. If this value was
    decreased to zero it means dvb has nothing to do.
    */
  u16 pending_section_num;

  /*!
    PTI driver's handle
    */
  pti_device_t *p_pti_dev;
  
  /*!
    dvb's section buffer start address
    */
  dvb_section_t *p_sec;

  /*!
    simple queue handle
    */
  class_handle_t q_handle;

  svc_container_t *p_container;

  BOOL dvb_actived;
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
  MT_ASSERT(p_sec->pti_handle != NULL);
  pti_dev_channel_free(p_priv->p_pti_dev, p_sec->pti_handle);
  p_sec->pti_handle = NULL;
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
      MT_ASSERT(p_sec->pti_handle == NULL);
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
      MT_ASSERT(p_sec->pti_handle == NULL);
      //push to wait queue back
      push_simple_queue(q_handle, p_svc_data->wait_queue, sec_id);
    }
  }
}

static void msg_proc(handle_t handle, os_msg_t *p_msg)
{
  service_t *p_svc = (service_t *)handle;
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;
  os_msg_t notify = {0};

  MT_ASSERT(p_priv->dvb_actived);
  
  switch(p_msg->content)
  {
    case DVB_REQUEST:
      do_request_cmd(p_svc, (dvb_request_t *)p_msg->para1);
      break;
    case DVB_FREE:
      do_free_cmd(p_svc, (dvb_request_t *)p_msg->para1);
      notify.content = DVB_FREED;
      notify.para1 = p_msg->para1;
      notify.para2 = sizeof(dvb_request_t);
      p_svc->notify(p_svc, &notify);
      break;
    case DVB_TABLE_TS_UPDATE:  //dummy case according to dvb_svc1.c
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
    MT_ASSERT(p_sec->pti_handle == NULL);
    MT_ASSERT(sec_id == p_sec->id);
    //OS_PRINTF("t_id [%d] :cur_tick[%d], start_ticks[%d], interval_time[%d]\n",
    //p_sec->table_id,
    //  cur_ticks,  p_sec->start_ticks, p_sec->interval_time);
    if((cur_ticks - p_sec->start_ticks)*10 > p_sec->interval_time)  //it's turn
    {
      push_simple_queue(q_handle, p_svc_data->wait_queue, sec_id);
    }
    else  //push it back
    {
      push_simple_queue(q_handle, p_svc_data->block_queue, sec_id);
    }
  }
}

static pti_type_t trans_type(filter_type_t filter_mode)
{
  return (FILTER_TYPE_TSPKT == filter_mode) ? 
    PTI_TYPE_TSPKT : PTI_TYPE_SECTION;
}

static alloc_ret_t alloc_filter(dvb_priv_t *p_priv, dvb_section_t *p_sec)
{
  RET_CODE pti_ret = ERR_FAILURE;
  h_pti_channel_t pti_handle = NULL;
  pti_alloc_param_t alloc_param = {0};
  request_mode_t mode = DATA_SINGLE;

  MT_ASSERT(NULL != p_sec);
  p_sec->p_buffer = NULL;
  mode = p_priv->table_map[p_sec->table_map_index].request_mode;
  alloc_param.type = trans_type(p_sec->filter_mode);
  alloc_param.pid = p_sec->pid;
  if(p_sec->filter_code != NULL)
  {
    memcpy(alloc_param.code, p_sec->filter_code, MAX_FILTER_MASK_BYTES);
  }
  if(p_sec->filter_mask != NULL)
  {
    memcpy(alloc_param.mask, p_sec->filter_mask, MAX_FILTER_MASK_BYTES);
  }
  //p_sec->crc_enable = TRUE;
  alloc_param.crc_enable = p_sec->crc_enable;
  if(mode != DATA_MULTI)
  {
    alloc_param.buf_mode = PTI_BUF_MODE_LINEAR;
  }
  else
  {
    if(PTI_TYPE_TSPKT == alloc_param.type)
    {
      alloc_param.buf_mode = PTI_BUF_MODE_CIRCLE;
    }
    else
    {
      alloc_param.buf_mode = PTI_BUF_MODE_LINEAR_EXT;
    }
    alloc_param.p_ext_buf_list = (pti_ext_buf_t *)p_sec->p_ext_buf_list;
  }

  //OS_PRINTF("xxx alloc pti table_id 0x%x, t %d\n", p_sec->table_id, mtos_ticks_get());
  pti_ret = pti_dev_channel_alloc(p_priv->p_pti_dev, &alloc_param, &pti_handle);
  p_sec->start_ticks = mtos_ticks_get();
  if(SUCCESS == pti_ret)
  {
    MT_ASSERT(pti_handle != NULL);
    p_sec->pti_handle = pti_handle;
    return ALLOC_SUC;
  }
  else if(0)//(ERR_FAILURE == pti_ret)  //what's the fail??
  {
    return ALLOC_FAIL;
  }
  else
  {
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
      MT_ASSERT(p_sec->pti_handle != NULL);
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
    MT_ASSERT(p_sec->pti_handle != NULL);
    //push it back to using queue
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

static RET_CODE external_soft_filter(dvb_section_t *p_sec,pti_data_block_t *p_data_block)
{
  RET_CODE ret = ERR_FAILURE;
  s32 cmp_rslt = 0;
  u8 block_data[MAX_SOFT_FILTER_MASK_BYTES] = {0};
  u8 i = 0;

  MT_ASSERT(NULL != p_sec);
  MT_ASSERT(NULL != p_data_block);
  
  if((0 == p_sec->filter_exmask_size) ||(MAX_SOFT_FILTER_MASK_BYTES < p_sec->filter_exmask_size))
  {
      return ERR_FAILURE;
  }

  if((MAX_FILTER_MASK_BYTES + p_sec->filter_exmask_size) > p_data_block->len)
  {
      /*OS_PRINTF("error: the data len[%d] is less than filer mask size!,sec id[0x%x] \n",
                          p_data_block->len,p_sec->id);*/
      return ERR_FAILURE;
  }

  for(i = 0; i < p_sec->filter_exmask_size; i++)
  {
    block_data[i] = (p_data_block->p_data[MAX_FILTER_MASK_BYTES + i]) & (p_sec->filter_exmask[i]);
  }

  cmp_rslt = memcmp(p_sec->filter_excode, block_data, p_sec->filter_exmask_size);
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
  u16 i = 0;
  pti_data_block_t data_block = {0};
  RET_CODE ret = 0;
  u16 sec_id = 0;
  u16 sec_num = 0;
  u32 cur_ticks = mtos_ticks_get();

  sec_num = get_simple_queue_len(q_handle, p_svc_data->using_queue);
  for(i = 0; i < sec_num; i ++)
  {
    pop_simple_queue(q_handle, p_svc_data->using_queue, &sec_id);
    p_sec = p_priv->p_sec + sec_id;
    //MT_ASSERT(p_sec->pti_handle != NULL);
    if(p_sec->pti_handle == NULL)
    {
      MT_ASSERT(0);
    }
    MT_ASSERT(sec_id == p_sec->id);

    ret = pti_dev_get_data(p_priv->p_pti_dev, p_sec->pti_handle, &data_block);
    
    /*our chip just support 12 bytes filer. if need to filter the additional bytes more than 12 bytes
        we use external soft filter*/
    if((SUCCESS == ret)&&(TRUE == p_sec->b_filter_external))
    {
        ret =  external_soft_filter(p_sec, &data_block);
    }
    
    if(SUCCESS == ret)  //a filter ready
    {
      p_sec->p_buffer = data_block.p_data;
      if(p_sec->direct_data)
      {
        os_msg_t msg = {0};
        msg.content = DVB_ONE_SEC_FOUND;
        msg.para1 = (u32)p_sec;
        msg.para2 = data_block.len;
        process_departed_sec(p_svc, p_sec);
        p_svc->notify(p_svc, &msg);
        continue;
      }
      else if(!p_sec->crc_enable || 
        (p_sec->crc_enable && check_sec_crc(p_priv, p_sec)))
      {
        //do parse
        p_priv->table_map[p_sec->table_map_index].parse(p_svc, p_sec);
        process_departed_sec(p_svc, p_sec);
        continue;
      }
      else //check crc fail
      {
        //do_time_out(p_svc, p_sec);
        //if software crc fail, retry it.
      }
    }

    if((cur_ticks - p_sec->start_ticks)*10 > p_sec->timeout)  //time out
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

static void sm_proc(handle_t handle)
{
  service_t *p_svc = (service_t *)handle;
  dvb_svc_data_t *p_svc_data = (dvb_svc_data_t *)p_svc->get_data_buffer(p_svc);
  dvb_priv_t *p_priv = p_svc_data->p_this->p_data;

  if(!p_priv->dvb_actived)
  {
    return;
  }
  
  switch(p_svc_data->state)
  {
    case DVB_IDLE:
      break;
    case DVB_RUNNING:
      dvb_active_block_sec(p_svc);
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
      //memset(p_table[i].param_list, 0xFF, sizeof(p_table[i].param_list));
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
      PTI_HW_FILTER_SUM);
    p_svc_data->state = DVB_RUNNING;
    p_svc_data->p_this = p_this;
    p_priv->p_container->add_svc(p_priv->p_container, p_svc);
  }
  return p_svc;
}

static u32 get_mem_size(class_handle_t handle,  BOOL dynamic, u8 filter_num, u8 sec_num)
{
  u32 size = 0;
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;

  pti_dev_get_mem_request(p_priv->p_pti_dev, dynamic, (s8)filter_num, &size);

  size += DVB_ATTACH_SIZE(sec_num) + sec_num * sizeof(dvb_section_t);
  return size;
}

static void start(class_handle_t handle, u8 *p_mem,
  u32 size, BOOL dynamic, u8 filter_num, u8 sec_num)
{
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;
  svc_container_t *p_container = p_priv->p_container;
  u8 *p_temp = p_mem;
  u16 i = 0;
  //u32 buf_size = 0;
  u32 pti_buf_size = 0;
  u32 total_size = 0;

  p_container->lock(p_container);
  total_size = get_mem_size(handle, dynamic, filter_num, sec_num);

  MT_ASSERT(((u32)p_mem) % 4 == 0);//align 4
  MT_ASSERT((p_mem != NULL) && (size >= total_size));
  MT_ASSERT(NULL == p_priv->p_sec);

  p_priv->p_sec = (dvb_section_t *)p_temp;
  p_temp += sec_num * sizeof(dvb_section_t);
  memset(p_priv->p_sec, 0, sizeof(dvb_section_t) * sec_num);

  //create free queue
  p_priv->free_queue = create_simple_queue1(p_priv->q_handle,
    sec_num);

  for(i = 0; i < sec_num; i ++)
  {
    p_priv->p_sec[i].id = i;
    push_simple_queue(p_priv->q_handle, p_priv->free_queue, i);
  }

  //attach request buffer
  memset(p_temp, 0, DVB_ATTACH_SIZE(sec_num));
  mtos_messageq_attach(p_container->get_msgq_id(p_container),
    p_temp, sizeof(dvb_request_t), sec_num);

  p_temp += DVB_ATTACH_SIZE(sec_num);
  pti_dev_get_mem_request(p_priv->p_pti_dev, dynamic,
    filter_num, &pti_buf_size);
  pti_dev_parser_init(p_priv->p_pti_dev, dynamic,
    filter_num, p_temp, pti_buf_size);

  p_priv->dvb_actived = TRUE;

  p_container->unlock(p_container);
}

static void stop(class_handle_t handle)
{
  dvb_t *p_this = handle;
  dvb_priv_t *p_priv = p_this->p_data;
  svc_container_t *p_container = p_priv->p_container;
  dvb_svc_data_t *p_svc_data = NULL;
  service_t *p_svc_list[10];
  u8 svc_num = 0, i = 0;

  p_container->lock(p_container);
  p_priv->dvb_actived = FALSE;
  p_priv->p_sec = NULL;
  destory_simple_queue1(p_priv->q_handle, p_priv->free_queue);
  mtos_messageq_clr(p_container->get_msgq_id(p_container));
  mtos_messageq_detach(p_container->get_msgq_id(p_container), sizeof(dvb_request_t));
  pti_dev_parser_deinit(p_priv->p_pti_dev);
  p_container->unlock(p_container);
  // Todo: TBD, need release all buffers in services
  svc_num = p_container->get_svc_list(p_container, (void **)p_svc_list);
  for (i = 0; i < svc_num; i++)
  {
    p_container->remove_svc(p_container, p_svc_list[i]);
    p_svc_data = (dvb_svc_data_t *)p_svc_list[i]->get_data_buffer(p_svc_list[i]);
    destory_simple_queue1(p_priv->q_handle, p_svc_data->wait_queue);
    destory_simple_queue1(p_priv->q_handle, p_svc_data->using_queue);
    destory_simple_queue1(p_priv->q_handle, p_svc_data->block_queue);
    p_svc_list[i]->release_data_buffer(p_svc_list[i]);
    mtos_free(p_svc_list[i]->p_data);
    p_svc_list[i]->p_data = NULL;
    mtos_free(p_svc_list[i]);
    p_svc_list[i] = NULL;
  }
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
    show_sec(p_priv);
    id = p_sec->id;

   // MT_ASSERT(0);
  }

  memcpy(p_sec, p_src_sec, sizeof(dvb_section_t));
  p_sec->pti_handle = NULL;
  p_sec->p_buffer = NULL;
  p_sec->id = id;
  
//    push_simple_queue(p_priv->q_handle, p_svc_data->wait_queue, id);
  
  ret = alloc_filter(p_priv, p_sec);
  if(ALLOC_SUC == ret)
  {
    MT_ASSERT(p_sec->pti_handle != NULL);
    // alloc filter suc, push it to using q
    push_simple_queue(q_handle, p_svc_data->using_queue, id);
  }
  else
  {
    OS_PRINTF("xxx dvb_alloc_section failed continue... \n");
    push_simple_queue(p_priv->q_handle, p_svc_data->wait_queue, id);
  }
  
}

handle_t dvb_init(u32 priority, u32 stack_size)
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
  p_this->register_table = register_table;
  p_this->get_mem_size = get_mem_size;
  p_this->alloc_section = alloc_section;
  p_this->filter_data_error = filter_data_error;
  
  p_priv->p_container = p_container;
  p_priv->p_pti_dev = (pti_device_t *)dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  p_priv->q_handle = class_get_handle_by_id(SIMPLE_Q_CLASS_ID);
  p_this->p_data = p_priv;
  class_register(DVB_CLASS_ID, p_this);
  return p_this;
}

