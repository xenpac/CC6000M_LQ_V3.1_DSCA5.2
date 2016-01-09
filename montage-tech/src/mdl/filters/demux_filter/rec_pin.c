/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"

//driver
#include "drv_dev.h"
#include "dmx.h"

//util
#include "class_factory.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "ipin.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "input_pin.h"
#include "output_pin.h"
#include "source_pin.h"
#include "ifilter.h"
#include "chain.h"
#include "demux_filter.h"
#include "demux_interface.h"
#include "demux_pin_intra.h"

#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_dmx_filter_rec_pin_imp.h"
#include "err_check_def.h"

#define MAX_REC_SLOT_NUM      (16)

/*!
  the rec pin attribute define
  */
typedef struct tag_rec_pin_attribute
{
 /*!
  pid
  */
  u16 pid;
 /*!
  dmx chan id
  */
  dmx_chanid_t chan_id;
}rec_pin_attr_t;

/*!
  the rec pin private data define
  */
typedef struct tag_rec_pin_private
{
  /*!
    this point !!
    */
  demux_pin_t      *p_this;
  /*!
    dmx manager handle
    */
  dmx_device_t     *p_dmx_dev;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
  /*!
    using arry
    */
  rec_pin_attr_t    slot_arry[MAX_REC_SLOT_NUM];
  /*!
    using number
    */
  u32               used_num;
  /*!
    stream hold
    */
  BOOL              b_hold;
  /*!
    the buffer of the hardware written, all slot share the buffer 
    */
  rec_buf_cfg_t     cfg;
  /*!
    the data length have got.
    */
  u64               obtain_data_len;
}rec_pin_private_t;

//fix me, record pin depends the input DMX_REC_CONFIG0 (temp solution)

static rec_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return (rec_pin_private_t *)(((demux_pin_t *)_this)->p_child_priv);
}

static RET_CODE rec_pin_request_pid(handle_t _this, void *p_data)
{
  rec_pin_private_t  *p_priv    = get_priv(_this);
  rec_pin_attr_t     *p_rec     = NULL;  
  dmx_device_t       *p_dmx_dev = p_priv->p_dmx_dev;  
  rec_request_data_t *p_para    = (rec_request_data_t *)p_data;
  dmx_rec_setting_t   rec_set   = { 0 };
  RET_CODE            ret       = ERR_FAILURE;
  u32                 index     = 0;

  CHECK_FAIL_RET_CODE(p_para    != NULL);
  CHECK_FAIL_RET_CODE(p_dmx_dev != NULL);
  
  //check the mapping list
  if(p_priv->used_num >= MAX_REC_SLOT_NUM)
  {
    return ERR_FAILURE;
  }

  //check new pid.
  for(index = 0; index < p_priv->used_num; index++)
  {
    if(p_priv->slot_arry[index].pid == p_para->pid)
    {
      return SUCCESS;
    }
  }

  //alloc resource
  p_rec = p_priv->slot_arry + index;
  rec_set.pid = p_para->pid;      
  rec_set.type_mode = p_para->rec_mode;
  rec_set.stream_in = p_para->demux_index;
  rec_set.rec_in = p_para->rec_in;
 
  ret = dmx_rec_chan_open(p_priv->p_dmx_dev, &rec_set, &p_rec->chan_id);
  if(ret == SUCCESS)
  {
    CHECK_FAIL_RET_CODE(p_rec->chan_id != DMX_INVALID_CHANNEL_ID);
    p_rec->pid = p_para->pid;
    p_priv->used_num ++;
    return SUCCESS;
  }
  else
  {
    OS_PRINTF("\n##debug: rec_pin_request_pid [%d]error!!\n", p_para->pid);
    return ERR_FAILURE;
  }
}

static RET_CODE rec_pin_free_pid(handle_t _this, void *p_data)
{
  rec_pin_private_t *p_priv = get_priv(_this);

  memset(p_priv->slot_arry, 0, sizeof(p_priv->slot_arry));
  p_priv->used_num = 0;
  return SUCCESS;
}

static RET_CODE rec_pin_reset_pid(handle_t _this, void *p_data)
{
  rec_pin_private_t  *p_priv    = get_priv(_this);
  rec_pin_attr_t     *p_rec     = NULL;  
  rec_reset_pid_data_t *p_para    = (rec_reset_pid_data_t *)p_data;
  dmx_rec_setting_t   rec_set   = { 0 };
  RET_CODE            ret       = ERR_FAILURE;
  u32                 index     = 0;
  BOOL b_found = FALSE;

  p_priv->b_hold = TRUE;

  for(index = 0; index < p_priv->used_num; index++)
  {
    p_rec = p_priv->slot_arry + index;    
    if (p_rec->chan_id != DMX_INVALID_CHANNEL_ID)
    {
      ret = dmx_chan_stop(p_priv->p_dmx_dev, p_rec->chan_id);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
  }
  
  for(index = 0; index < p_priv->used_num; index++)
  {
    p_rec = p_priv->slot_arry + index;    
    if(p_rec->chan_id != DMX_INVALID_CHANNEL_ID)
    {
      ret = dmx_chan_close(p_priv->p_dmx_dev, p_rec->chan_id);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
  }
  
  //check new pid.
  for(index = 0; index < p_priv->used_num; index++)
  {
    if(p_priv->slot_arry[index].pid == p_para->old_v_pid)
    {
      p_priv->slot_arry[index].pid = p_para->new_v_pid;
    }
    else if(p_priv->slot_arry[index].pid == p_para->old_a_pid)
    {
      p_priv->slot_arry[index].pid = p_para->new_a_pid;
    }
    else if(p_priv->slot_arry[index].pid == p_para->old_pcr_pid)
    {
      p_priv->slot_arry[index].pid = p_para->new_pcr_pid;
      b_found = TRUE;
    }
  }

  if (!b_found
    && (p_para->new_pcr_pid != p_para->new_v_pid)
    && (p_para->new_pcr_pid != p_para->new_a_pid))
  {
    p_priv->slot_arry[index].pid = p_para->new_pcr_pid;
    p_priv->slot_arry[index].chan_id = DMX_INVALID_CHANNEL_ID;
    p_priv->used_num ++;
  }
  
  for(index = 0; index < p_priv->used_num; index++)
  {
    p_rec = p_priv->slot_arry + index;
    rec_set.pid = p_rec->pid;
    rec_set.type_mode = p_para->rec_mode;
    rec_set.stream_in = p_para->demux_index;
    rec_set.rec_in = p_priv->cfg.rec_in;
    ret = dmx_rec_chan_open(p_priv->p_dmx_dev, &rec_set, &p_rec->chan_id);
    if (ret != SUCCESS)
    {
      p_rec->chan_id = DMX_INVALID_CHANNEL_ID;
    }
  }
  
  dmx_rec_chan_set_buffer(p_priv->p_dmx_dev, p_priv->cfg.rec_in,
                                    p_priv->cfg.p_rec_buffer, p_priv->cfg.total_buf_len);
  
  for(index = 0; index < p_priv->used_num; index++)
  {
    p_rec = p_priv->slot_arry + index;
    if (p_rec->chan_id != DMX_INVALID_CHANNEL_ID)
    {
      ret = dmx_chan_start(p_priv->p_dmx_dev, p_rec->chan_id);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
  }
  p_priv->b_hold = FALSE;
  p_priv->obtain_data_len = 0;
  return SUCCESS;
}

static RET_CODE rec_pin_pause_resume(handle_t _this, BOOL b_resume)
{
  rec_pin_private_t *p_priv       = get_priv(_this);
  u64 up = 0;
  u64 data_size = 0;
  u64 get_size = 0;
  u64 unit_size = p_priv->cfg.fill_unit_size;

  if (!b_resume)
  {
    return SUCCESS;
  }

  dmx_rec_get_data(p_priv->p_dmx_dev, p_priv->cfg.rec_in, &data_size);

  up = data_size - p_priv->obtain_data_len;
  get_size = up - up % unit_size;

  if (get_size > unit_size)
  {
    get_size -= unit_size;
  }
  else
  {
    get_size = 0;
  }
  p_priv->obtain_data_len += get_size;
  return SUCCESS;
}
static RET_CODE rec_pin_cfg_buffer(handle_t _this, void *p_para)
{
  rec_pin_private_t *p_priv = get_priv(_this);
  rec_buf_cfg_t     *p_cfg = (rec_buf_cfg_t *)p_para;
  
  if((p_cfg == NULL) || (p_cfg->p_rec_buffer == NULL) ||
    (p_cfg->total_buf_len == 0))
  {
    OS_PRINTF("error input para\n");
    return ERR_FAILURE;
  }

  if((p_cfg->total_buf_len % p_cfg->atom_size) != 0)
  {
    OS_PRINTF("error atom size\n");
    return ERR_FAILURE;
  }

  memcpy(&p_priv->cfg, p_cfg, sizeof(rec_buf_cfg_t));
  CHECK_FAIL_RET_CODE(p_priv->cfg.fill_expect_size > 0);
  CHECK_FAIL_RET_CODE(p_priv->cfg.fill_unit_size > 0);
  
  return dmx_rec_chan_set_buffer(p_priv->p_dmx_dev, p_cfg->rec_in,
                                  p_cfg->p_rec_buffer, p_cfg->total_buf_len);
}


static RET_CODE rec_pin_on_start(handle_t _this)
{
  rec_pin_private_t *p_priv = get_priv(_this);
  rec_pin_attr_t    *p_rec  = NULL;
  u32                index  = 0;
  RET_CODE           ret    = ERR_FAILURE;

  for(index = 0; index < p_priv->used_num; index++)
  {
    p_rec = p_priv->slot_arry + index;    
    if (p_rec->chan_id != DMX_INVALID_CHANNEL_ID)
    {
      ret = dmx_chan_start(p_priv->p_dmx_dev, p_rec->chan_id);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
  }
  
  p_priv->b_hold = FALSE;
  p_priv->obtain_data_len = 0;

  return SUCCESS;
}

static RET_CODE rec_pin_on_stop(handle_t _this)
{
  rec_pin_private_t *p_priv = get_priv(_this);
  rec_pin_attr_t    *p_rec = NULL;
  u32                index  = 0;
  RET_CODE           ret    = ERR_FAILURE;

  for(index = 0; index < p_priv->used_num; index++)
  {
    p_rec = p_priv->slot_arry + index;    
    if (p_rec->chan_id != DMX_INVALID_CHANNEL_ID)
    {
      ret = dmx_chan_stop(p_priv->p_dmx_dev, p_rec->chan_id);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
  }
  p_priv->b_hold = TRUE;

  return SUCCESS;
}

static RET_CODE rec_pin_on_close(handle_t _this)
{
  rec_pin_private_t *p_priv = get_priv(_this);
  rec_pin_attr_t    *p_rec = NULL;
  u32                index  = 0;
  RET_CODE           ret    = ERR_FAILURE;

  for(index = 0; index < p_priv->used_num; index++)
  {
    p_rec = p_priv->slot_arry + index;    
    if (p_rec->chan_id != DMX_INVALID_CHANNEL_ID)
    {
      ret = dmx_chan_close(p_priv->p_dmx_dev, p_rec->chan_id);
      CHECK_FAIL_RET_CODE(ret == SUCCESS);
    }
  }
  
  rec_pin_free_pid(_this, NULL);
  return SUCCESS;
}

static void rec_pin_on_destroy(handle_t _this)
{
  rec_pin_private_t *p_priv = get_priv(_this);

  if(p_priv != NULL)
  {
    mtos_free((void *)p_priv);
  }
}

static RET_CODE rec_pin_on_fill_sample(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, s64 position)
{
  rec_pin_private_t *p_priv       = get_priv(_this);
  u8                *p_data       = NULL;
  u64                up           = 0;
  u64                data_size    = 0;
  u64                get_size     = 0;
  u64                unit_size    = p_priv->cfg.fill_unit_size;
  u64                unit_left    = 0;
  u64                offset       = 0;
  u64                overflow_len = 0;
  u32                top          = 0;

  CHECK_FAIL_RET_CODE(_this != NULL);
  
  if(p_priv->b_hold)
  {
    return ERR_FAILURE;
  }

  dmx_rec_get_data(p_priv->p_dmx_dev, p_priv->cfg.rec_in, &data_size);

  if(data_size == 0)
  {
    //mtos_task_sleep(10);  //wait stream in
    return SUCCESS;
  }
  
  up = data_size - p_priv->obtain_data_len;
  get_size = up - up % unit_size;

  if(get_size >= p_priv->cfg.fill_expect_size)
  {
    if(get_size > p_priv->cfg.total_buf_len)
    {
      overflow_len = get_size - p_priv->cfg.total_buf_len;
      OS_PRINTF("\n\n\nrec data overflow %lld packet !!\n\n\n", overflow_len / 188);
      //find to the don't overwrite buffer
      p_priv->obtain_data_len += overflow_len;
      get_size -= overflow_len;
      get_size = get_size - get_size % unit_size;
      if(get_size < p_priv->cfg.fill_expect_size)
      {
        return ERR_FAILURE;
      }
    }

    offset = p_priv->obtain_data_len % p_priv->cfg.total_buf_len;
    p_data = p_priv->cfg.p_rec_buffer + (u32)offset;
#if 1
    unit_left = (u64)p_priv->cfg.total_buf_len - offset;
    if ((unit_left < unit_size) && (p_priv->cfg.total_buf_len % p_priv->cfg.fill_unit_size))
    {
      memcpy((void *)((u32)p_data + (u32)unit_left),
        p_priv->cfg.p_rec_buffer, (u32)(unit_size - unit_left));
      p_priv->obtain_data_len += unit_size;
      p_sample->payload = unit_size;
      p_sample->p_data = p_data;
      return SUCCESS;
    }
 #endif
    if(get_size + offset > p_priv->cfg.total_buf_len)
    {
      top = get_size + offset - p_priv->cfg.total_buf_len;
      get_size -= top;
      get_size = get_size - get_size % unit_size;
    }
    p_sample->p_data = p_data;
    p_sample->payload = get_size;
    p_priv->obtain_data_len += get_size;
    //OS_PRINTF("rec_fill_sample: offset [%d]*188, get_size[%d]*188\n",
    //  offset/188,  get_size / 188);
    log_perf(LOG_DMX_FILTER_REC_PIN, TAG_GET_PALYLOAD,
     (u32)(p_sample->payload / 188), 0);
    return SUCCESS;
  }
  else
  {
    mtos_task_sleep(10);  //wait stream in
    return ERR_FAILURE;
  }
}

static RET_CODE rec_pin_decide_buffer_size(handle_t _this)
{
  rec_pin_private_t     *p_priv     = get_priv(_this);
  imem_allocator_t      *p_alloc    = p_priv->p_alloc;
  allocator_properties_t properties = {0};
  
  //config mem_alloc
  p_alloc->get_properties(p_alloc, &properties);
  properties.buffers = 1;
  properties.buffer_size = 10 * KBYTES * KBYTES;
  properties.use_virtual_space = TRUE;
  p_alloc->set_properties(p_alloc, &properties, NULL);
  return SUCCESS;
}

demux_pin_t * attach_rec_pin_instance(handle_t _this)
{
  rec_pin_private_t *p_priv       = NULL;
  demux_pin_t       *p_rec_pin    = (demux_pin_t *)_this;
  base_output_pin_t *p_output_pin = (base_output_pin_t *)_this;
  ipin_t            *p_ipin       = (ipin_t *)_this;
  interface_t       *p_interface  = (interface_t *)_this;
  media_format_t     media_format = {MT_RECODE};

  //check input parameter
  CHECK_FAIL_RET_NULL(_this != NULL);

  //create base class
  p_interface->_rename(p_interface, "rec_pin");

  //init private date
  p_rec_pin->p_child_priv = mtos_malloc(sizeof(rec_pin_private_t));
  CHECK_FAIL_RET_NULL(p_rec_pin->p_child_priv != NULL);
  p_priv = (rec_pin_private_t *)(p_rec_pin->p_child_priv);
  memset(p_priv, 0x0, sizeof(rec_pin_private_t));
  p_priv->p_this = p_rec_pin;
  p_priv->b_hold = TRUE;

  p_priv->p_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_NULL(NULL != p_priv->p_dmx_dev);
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);
 
  //init member function
  p_rec_pin->request = rec_pin_request_pid;
  p_rec_pin->free    = rec_pin_free_pid;
  p_rec_pin->config  = rec_pin_cfg_buffer;
  p_rec_pin->reset_pid = rec_pin_reset_pid;
  p_rec_pin->pause_resume = rec_pin_pause_resume;

  //over load virtual function
  p_output_pin->decide_buffer_size = rec_pin_decide_buffer_size;
  p_output_pin->on_fill_sample     = rec_pin_on_fill_sample;

  p_ipin->on_open  = NULL;
  p_ipin->on_start = rec_pin_on_start;
  p_ipin->on_stop  = rec_pin_on_stop;
  p_ipin->on_close = rec_pin_on_close;
  p_ipin->_interface.on_destroy = rec_pin_on_destroy;

  return p_rec_pin;
}

