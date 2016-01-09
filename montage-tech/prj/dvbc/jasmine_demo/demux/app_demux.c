/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
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

#include "drv_dev.h"
#include "dmx.h"

#include "sys_app.h"

#include "app_demux.h"

#include "hal_misc.h"

#define APP_DMX_DEFAULT_THREAD_STACK_SIZE (16*KBYTES)
#define APP_DMX_DEFAULT_FILTER_BUFFER_SIZE (4096*2)
#define APP_DMX_INVALID_FILTER_ID ((u16)(~0))

/********************************************************************************************/





static app_dmx_class *app_dmx = NULL;

/********************************************************************************************/


static RET_CODE _create_dmx_service(app_dmx_thread *thread);

/********************************************************************************************/


static RET_CODE _dmx_lock(void)
{
  int result;
  
  MT_ASSERT(app_dmx != NULL);
  MT_ASSERT(app_dmx->mutex != NULL);
  result = mtos_mutex_take(app_dmx->mutex);
  return result? SUCCESS : ERR_FAILURE;
}

static RET_CODE _dmx_unlock(void)
{
  int result;
  
  MT_ASSERT(app_dmx != NULL);
  MT_ASSERT(app_dmx->mutex != NULL);
  result = mtos_mutex_give(app_dmx->mutex);
  return result? SUCCESS : ERR_FAILURE;
}

RET_CODE app_dmx_init(u8 thread_prio)
{
  RET_CODE ret;
  
  if (app_dmx != NULL)
  {
    return ERR_FAILURE;
  }
  app_dmx = mtos_malloc(sizeof(app_dmx_class));
  MT_ASSERT(app_dmx != NULL);
  memset(app_dmx, 0, sizeof(app_dmx_class));
  app_dmx->thread.thread_prio = thread_prio;
  app_dmx->thread.thread_stack_size = APP_DMX_DEFAULT_THREAD_STACK_SIZE;
  app_dmx->thread.thread_stack = mtos_malloc(app_dmx->thread.thread_stack_size);
  MT_ASSERT(app_dmx->thread.thread_stack != NULL);

  app_dmx->mutex = mtos_mutex_create(1);
  MT_ASSERT(app_dmx->mutex != NULL);

  ret = _create_dmx_service(&app_dmx->thread);

  return ret;
}

static app_dmx_channel_handle _check_duplicate_pid(u16 pid)
{
  app_dmx_channel *p;

  p = app_dmx->channel_list;

  while (p != NULL)
  {
    if (p->pid == pid)
    {
      break;
    }
    p = p->next;
  }

  return p;
}

static BOOL _check_valid_channel(app_dmx_channel *c)
{
  app_dmx_channel *p;
  BOOL valid = FALSE;

  if (c == NULL)
  {
    return FALSE;
  }

  p = app_dmx->channel_list;

  while (p != NULL)
  {
    if (p == c)
    {
      valid = TRUE;
      break;
    }
    p = p->next;
  }

  return valid;
}

static BOOL _check_valid_descrambler(app_dmx_descrambler *d)
{
  app_dmx_descrambler *p;
  BOOL valid = FALSE;

  if (d == NULL)
  {
    return FALSE;
  }

  p = app_dmx->descrambler_list;

  while (p != NULL)
  {
    if (p == d)
    {
      valid = TRUE;
      break;
    }
    p = p->next;
  }

  return valid;
}


// channel list alloc node
static app_dmx_channel *_clist_alloc_node(void)
{
  app_dmx_channel *c;

  c = mtos_malloc(sizeof(app_dmx_channel));
  if (c == NULL)
  {
    return NULL;
  }
  memset(c, 0, sizeof(app_dmx_channel));

  if (app_dmx->channel_list == NULL)
  {
    app_dmx->channel_list = c;
  }
  else
  {
    app_dmx_channel *p = app_dmx->channel_list;

    while (p->next != NULL)
    {
      p = p->next;
    }
    p->next = c;
  }

  return c;
}

// channel list free node
static RET_CODE _clist_free_node(app_dmx_channel *c)
{
  MT_ASSERT(c != NULL);

  if (c == app_dmx->channel_list)
  {
    app_dmx->channel_list = app_dmx->channel_list->next;
  }
  else
  {
    app_dmx_channel *p_pre = NULL;
    app_dmx_channel *p_cur = app_dmx->channel_list;
    
    while (p_cur != NULL)
    {
      if (p_cur == c && p_pre != NULL)
      {
        p_pre->next = p_cur->next;
        break;
      }
      p_pre = p_cur;
      p_cur = p_cur->next;
    }
  }
  //lint -e668
  memset(c, 0, sizeof(app_dmx_channel));
  mtos_free(c);
  return SUCCESS;
}


// filter list alloc node
static app_dmx_filter*_flist_alloc_node(void)
{
  app_dmx_filter *f;

  f = mtos_malloc(sizeof(app_dmx_filter));
  if (f == NULL)
  {
    return f;
  }
  memset(f, 0, sizeof(app_dmx_filter));
  f->filter_id = APP_DMX_INVALID_FILTER_ID;

  if (app_dmx->filter_list == NULL)
  {
    app_dmx->filter_list = f;
  }
  else
  {
    app_dmx_filter *p = app_dmx->filter_list;

    while (p->next != NULL)
    {
      p = p->next;
    }
    p->next = f;
  }

  return f;
}

// filter list free node
static RET_CODE _flist_free_node(app_dmx_filter *f)
{
  MT_ASSERT(f != NULL);

  if (f == app_dmx->filter_list)
  {
    app_dmx->filter_list = app_dmx->filter_list->next;
  }
  else
  {
    app_dmx_filter *p_pre = NULL;
    app_dmx_filter *p_cur = app_dmx->filter_list;
    
    while (p_cur != NULL)
    {
      if (p_cur == f && p_pre != NULL)
      {
        p_pre->next = p_cur->next;
        break;
      }
      p_pre = p_cur;
      p_cur = p_cur->next;
    }
  }
  memset(f, 0, sizeof(app_dmx_filter));
  mtos_free(f);
  return SUCCESS;
}


// descrambler list alloc node
static app_dmx_descrambler *_dlist_alloc_node(void)
{
  app_dmx_descrambler *d;

  d = mtos_malloc(sizeof(app_dmx_descrambler));
  if (d == NULL)
  {
    return NULL;
  }
  memset(d, 0, sizeof(app_dmx_descrambler));

  if (app_dmx->descrambler_list == NULL)
  {
    app_dmx->descrambler_list = d;
  }
  else
  {
    app_dmx_descrambler *p = app_dmx->descrambler_list;

    while (p->next != NULL)
    {
      p = p->next;
    }
    p->next = d;
  }

  return d;
}

// descrambler list free node
static RET_CODE _dlist_free_node(app_dmx_descrambler *d)
{
  MT_ASSERT(d != NULL);

  if (d == app_dmx->descrambler_list)
  {
    app_dmx->descrambler_list = app_dmx->descrambler_list->next;
  }
  else
  {
    app_dmx_descrambler *p_pre = NULL;
    app_dmx_descrambler *p_cur = app_dmx->descrambler_list;
    
    while (p_cur != NULL)
    {
      if (p_cur == d && p_pre != NULL)
      {
        p_pre->next = p_cur->next;
        break;
      }
      p_pre = p_cur;
      p_cur = p_cur->next;
    }
  }
  memset(d, 0, sizeof(app_dmx_descrambler));
  mtos_free(d);
  return SUCCESS;
}



static BOOL _check_valid_filter(app_dmx_filter *f)
{
  app_dmx_filter *p;
  BOOL valid = FALSE;

  if (f == NULL)
  {
    return FALSE;
  }

  p = app_dmx->filter_list;

  while (p != NULL)
  {
    if (p == f)
    {
      valid = TRUE;
      break;
    }
    p = p->next;
  }
  return valid;
}


app_dmx_channel_handle app_dmx_alloc_channel(void)
{
  app_dmx_channel *c;

  _dmx_lock();
  do
  {
    c = _clist_alloc_node();
    if (c == NULL)
    {
      break;
    }
    c->status = APP_DMX_CHANNEL_STOPED;
  }while (0);
  _dmx_unlock();
  
  return (app_dmx_channel_handle)c;
}

RET_CODE app_dmx_set_channel(app_dmx_channel_handle h_channel, app_dmx_channel_param *param)
{
  app_dmx_channel_handle handle;
  app_dmx_channel *c = (app_dmx_channel *)h_channel;
  RET_CODE ret = SUCCESS;

  MT_ASSERT(param != NULL);
  _dmx_lock();
  do
  {
    if (!_check_valid_channel(c))
    {
      ret = ERR_FAILURE;
      break;
    }
    
    handle = _check_duplicate_pid(param->pid);
    if (handle != h_channel)
    {
      APPLOGW("pid:0x%x already used by channel:%p\n", param->pid, handle);
    }
    
    c->pid = param->pid;
    c->channel_type = param->channel_type;
  }while (0);
  _dmx_unlock();
  
  return ret;
}

RET_CODE app_dmx_free_channel(app_dmx_channel_handle h_channel)
{
  app_dmx_channel *c = (app_dmx_channel *)h_channel;
  RET_CODE ret = SUCCESS;

  _dmx_lock();
  do
  {
    if (!_check_valid_channel(c))
    {
      ret = ERR_FAILURE;
      break;
    }
    if (c->status != APP_DMX_CHANNEL_STOPED)
    {
      APPLOGE("Please stop channel first\n");
      ret = ERR_FAILURE;
      break;
    }
    ret = _clist_free_node(c);
  }while (0);
  _dmx_unlock();
  
  return ret;
}

RET_CODE app_dmx_start_channel(app_dmx_channel_handle h_channel)
{
  app_dmx_channel *c = (app_dmx_channel *)h_channel;
  RET_CODE ret = SUCCESS;

  _dmx_lock();
  do
  {
    if (!_check_valid_channel(c))
    {
      ret = ERR_FAILURE;
      break;
    }
    c->status = APP_DMX_CHANNEL_STARTED;

    if (app_dmx->thread.thread_status == APP_DMX_THREAD_SUSPEND)
    {
      ret = (RET_CODE)mtos_task_resume(app_dmx->thread.thread_prio);
      MT_ASSERT(ret == SUCCESS);
      app_dmx->thread.thread_status = APP_DMX_THREAD_RUNNING;
    }
  }while (0);
  _dmx_unlock();
  
  return ret;
}

RET_CODE app_dmx_stop_channel(app_dmx_channel_handle h_channel)
{
  app_dmx_channel *tmp_c;
  app_dmx_channel *c = (app_dmx_channel *)h_channel;
  RET_CODE ret = SUCCESS;

  _dmx_lock();
  do
  {
    if (!_check_valid_channel(c))
    {
      ret = ERR_FAILURE;
      break;
    }
    c->status = APP_DMX_CHANNEL_STOPED;

    tmp_c = app_dmx->channel_list;
    while (tmp_c != NULL)
    {
      if (tmp_c->status != APP_DMX_CHANNEL_STOPED)
      {
        break;
      }
      tmp_c = tmp_c->next;
    }
    if (tmp_c == NULL)
    {
      ret = (RET_CODE)mtos_task_suspend(app_dmx->thread.thread_prio);
      MT_ASSERT(ret == SUCCESS);
      app_dmx->thread.thread_status = APP_DMX_THREAD_SUSPEND;
    }
  }while (0);
  _dmx_unlock();
  
  return ret;
}

app_dmx_filter_handle app_dmx_alloc_filter(app_dmx_channel_handle h_channel)
{
  app_dmx_filter *filter = NULL;
  app_dmx_channel *c = (app_dmx_channel *)h_channel;

  _dmx_lock();
  do
  {
    if (!_check_valid_channel(c))
    {
      break;
    }
    
    filter = _flist_alloc_node();
    if (filter == NULL)
    {
      break;
    }
    filter->parent = c;
  }while (0);
  _dmx_unlock();
  
  return filter;
}

RET_CODE app_dmx_free_filter(app_dmx_filter_handle h_filter)
{
  app_dmx_filter *f = (app_dmx_filter *)h_filter;
  RET_CODE ret = SUCCESS;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  _dmx_lock();
  do
  {
    if (!_check_valid_filter(f))
    {
      ret = ERR_FAILURE;
      break;
    }
    if (f->status != APP_DMX_FILTER_STOPED)
    {
      APPLOGE("Please stop filter first\n");
      ret = ERR_FAILURE;
      break;
    }
    ret = dmx_chan_close(dmx_dev, f->filter_id);
    if (ret  !=  SUCCESS)
    {
      APPLOGE("Free filter Failed\n");
      break;
    }
    if (f->buffer != NULL)
    {
      mtos_free(f->buffer);
    }
    ret = _flist_free_node(f);
  }while (0);
  _dmx_unlock();
  
  return ret;
}

RET_CODE app_dmx_set_filter(app_dmx_filter_handle h_filter, app_dmx_filter_param *param)
{
  u8 filter_depth;
  u8 i;
  dmx_slot_setting_t slot;
  app_dmx_channel *c;
  dmx_filter_setting_t f_param;
  app_dmx_filter *f = (app_dmx_filter *)h_filter;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  RET_CODE ret = SUCCESS;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif

  
  MT_ASSERT(param != NULL);

  _dmx_lock();
  do
  {
    if (!_check_valid_filter(f))
    {
      ret = ERR_FAILURE;
      break;
    }
    c = f->parent;
    if (!_check_valid_channel(c))
    {
      ret = ERR_FAILURE;
      break;
    }
    
    if (f->status != APP_DMX_FILTER_STOPED)
    {
      APPLOGE("filter is not stoped\n");
      ret = ERR_FAILURE;
      break;
    }
    if (f->filter_id == APP_DMX_INVALID_FILTER_ID)
    {
      // alloc a new filter
      //backup filter parameter.
      memcpy(&f->param, param, sizeof(app_dmx_filter_param));
      
      memset(&slot, 0, sizeof(slot));
      slot.in = DMX_INPUT_EXTERN0;
      slot.pid = c->pid;
      switch (c->channel_type)
      {
        case APP_DMX_CHANNEL_PSI:
          slot.type = DMX_CH_TYPE_SECTION;
          break;
        case APP_DMX_CHANNEL_PES:
          slot.type = DMX_CH_TYPE_PES;
          break;
        default:
          MT_ASSERT(0);
          break;
      }
      if(chip_ic_id == IC_CONCERTO)
      {
        slot.muldisp = DMX_SLOT_MUL_SEC_DIS_DISABLE;//for bug 63129
      }
      ret = dmx_si_chan_open(dmx_dev, &slot, &f->filter_id);
      if (ret != SUCCESS)
      {
        break;
      }
    }
    else
    {
      // filter already allocated. do not need relaloc
    }
    
    if (param->buffer_size != f->buffer_size && param->buffer_size != 0)
    {
      if (f->buffer != NULL)
      {
        mtos_free(f->buffer);
        f->buffer = NULL;
      }
    }
    if (f->buffer == NULL)
    {
      f->buffer_size = (param->buffer_size == 0) ? APP_DMX_DEFAULT_FILTER_BUFFER_SIZE : param->buffer_size;
      f->buffer = mtos_malloc(f->buffer_size);
    }
    MT_ASSERT(f->buffer != NULL);
    
    ret = dmx_si_chan_set_buffer(dmx_dev, f->filter_id, f->buffer, f->buffer_size);
    if (ret != SUCCESS)
    {
      dmx_chan_stop(dmx_dev, f->filter_id);
      mtos_free(f->buffer);
      break;
    }
    
    MT_ASSERT(param->depth <= APP_DMX_FILTER_MAX_DEPTH);
    memset(&f_param, 0, sizeof(f_param));
    f_param.continuous = (!param->one_shot);
    f_param.en_crc = param->enable_crc;
    
    for (i = APP_DMX_FILTER_MAX_DEPTH - 1; i > 0; i--)
    {
      if (param->mask[i] != 0)
      {
        break;//break for
      }
    }
    filter_depth = APP_DMX_FILTER_MAX_DEPTH - i;
    
    if (filter_depth != param->depth)
    {
      APPLOGW("Filter depth is not match with valid mask len\n");
      if (param->depth == 0)
      {
        APPLOGW("Filter depth is 0, auto set it based valid mask len:%d\n",filter_depth);
      }
      else
      {
        filter_depth = param->depth;
      }
    }
    
    memcpy(f_param.value, param->match, filter_depth);
    memcpy(f_param.mask, param->mask, filter_depth);
    f_param.req_mode = param->one_shot ? DMX_REQ_MODE_SINGLE_SECTION : DMX_REQ_MODE_FILTER_LOOPBUFF;
    ret = dmx_si_chan_set_filter(dmx_dev, f->filter_id, &f_param);
    if (ret != SUCCESS)
    {
      dmx_chan_stop(dmx_dev, f->filter_id);
      mtos_free(f->buffer);
      break;
    }
  }while (0);
  _dmx_unlock();
  
  return ret;
}


RET_CODE app_dmx_start_filter(app_dmx_filter_handle h_filter)
{
  app_dmx_filter *f = (app_dmx_filter *)h_filter;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  RET_CODE ret = SUCCESS;

  _dmx_lock();
  do
  {
    if (!_check_valid_filter(f))
    {
      ret = ERR_FAILURE;
      break;
    }
    if (f->filter_id == APP_DMX_INVALID_FILTER_ID)
    {
      APPLOGE("Please set filter first ! f->filter_id = %d \n", f->filter_id);
      ret = ERR_FAILURE;
      break;
    }
    APPLOGI("[app_dmx_start_filter] f->filter_id = %d \n", f->filter_id);
    ret = dmx_chan_start(dmx_dev, f->filter_id);
    if (ret != SUCCESS)
    {
      break;
    }
    f->status = APP_DMX_FILTER_STARTED;
    f->start_time = mtos_ticks_get() * 10;
  }while (0);
  _dmx_unlock();
  
  return ret;
}

RET_CODE app_dmx_stop_filter(app_dmx_filter_handle h_filter)
{
  app_dmx_filter *f = (app_dmx_filter *)h_filter;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  RET_CODE ret = SUCCESS;

  _dmx_lock();
  do
  {
    if (!_check_valid_filter(f))
    {
      ret = ERR_FAILURE;
      break;
    }
    if (f->filter_id == APP_DMX_INVALID_FILTER_ID)
    {
      APPLOGE("Please set filter first ! f->filter_id = %d \n", f->filter_id);
      ret = ERR_FAILURE;
      break;
    }
    ret = dmx_chan_stop(dmx_dev, f->filter_id);
    if (ret != SUCCESS)
    {
      OS_PRINTF("app_dmx_stop_filter stop failed \n");
      break;
    }
    f->status = APP_DMX_FILTER_STOPED;
  }while (0);
  _dmx_unlock();

  return ret;
}



app_dmx_easy_filter_handle app_dmx_alloc_easy_filter(void)
{
  app_dmx_channel_handle hc;
  app_dmx_filter_handle hf;
  
  hc = app_dmx_alloc_channel();
  hf = app_dmx_alloc_filter(hc);

  return hf;
}
RET_CODE app_dmx_free_easy_filter(app_dmx_easy_filter_handle h_filter)
{
  RET_CODE ret;
  app_dmx_filter *f = (app_dmx_filter *)h_filter;
  app_dmx_filter *tmp_f;
  app_dmx_channel *c;

  c = f->parent;
  ret = app_dmx_free_filter((app_dmx_filter_handle)f);
  if (ret != SUCCESS)
  {
    return ret;
  }
  
  _dmx_lock();
  tmp_f = app_dmx->filter_list;
  while (tmp_f != NULL)
  {
    if (tmp_f->parent == c)
    {
      break;
    }
    tmp_f = tmp_f->next;
  }
  _dmx_unlock();
  
  if (tmp_f == NULL) // channel has no more filters. then we should free this channel.
  {
    ret = app_dmx_free_channel(c);
  }

  return ret;
}
RET_CODE app_dmx_set_easy_filter(app_dmx_easy_filter_handle h_filter, app_dmx_easy_filter_param *param)
{
  RET_CODE ret;
  app_dmx_filter *f = (app_dmx_filter *)h_filter;
  app_dmx_channel_param channel_param;

  MT_ASSERT(param != NULL);
  //lint -e613
  memset(&channel_param, 0 ,sizeof(channel_param));
  channel_param.channel_type = APP_DMX_CHANNEL_PSI;
  channel_param.pid = param->pid;
  ret = app_dmx_set_channel(f->parent, &channel_param);
  if (ret != SUCCESS)
  {
    return ret;
  }
  ret = app_dmx_set_filter(f, (app_dmx_filter_param *)param);

  return ret;
}
RET_CODE app_dmx_start_easy_filter(app_dmx_easy_filter_handle h_filter)
{
  RET_CODE ret;
  app_dmx_filter *f = (app_dmx_filter *)h_filter;

  ret = app_dmx_start_filter(f);
  if (ret != SUCCESS)
  {
    return ret;
  }
  ret = app_dmx_start_channel(f->parent);
  
  return ret;
}
RET_CODE app_dmx_stop_easy_filter(app_dmx_easy_filter_handle h_filter)
{
  RET_CODE ret;
  app_dmx_filter *tmp_f;
  app_dmx_filter *f = (app_dmx_filter *)h_filter;

  ret = app_dmx_stop_filter(f);
  if (ret != SUCCESS)
  {
    OS_PRINTF("failed to stop filter ,return \n");
    return ret;
  }
  // check if all of filters associate with this channel are stoped. if yes, then close this channel.
  _dmx_lock();
  tmp_f = app_dmx->filter_list;
  while (tmp_f != NULL)
  {
    if (tmp_f->parent == f->parent)
    {
      if (tmp_f->status != APP_DMX_FILTER_STOPED)
      {
        break; // break from while (tmp_f != NULL)
      }
    }
    tmp_f = tmp_f->next;
  }
  _dmx_unlock();
  if (tmp_f == NULL) // none filter is open in this channel, then we stop this channel.
  {
    ret = app_dmx_stop_channel(f->parent);
  }

  return ret;
}





static void _dmx_data_polling(void *param)
{
  RET_CODE ret;
  app_dmx_filter *f;
  app_dmx_filter *f_next;
  u8 tmp_data[4096];
  app_dmx_data_notify data_callback;
  app_dmx_event_notify event_callback;
  app_dmx_event_param evt_param;
  void *private_data;
  BOOL have_data;
  u8 *p_data = NULL;
  u32 data_size = 0;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  //lint -e716 while(1)
  while (1)
  {
    have_data = FALSE;
    
    _dmx_lock();
    f = app_dmx->filter_list;
    while (f != NULL)
    {
      f_next = f->next;
      if (f->status == APP_DMX_FILTER_STARTED && f->parent->status == APP_DMX_CHANNEL_STARTED)
      {
        ret = dmx_si_chan_get_data(dmx_dev, f->filter_id, &p_data, &data_size);
        if (ret == SUCCESS && data_size != 0 && f->param.data_callback != NULL)
        {
          memcpy(tmp_data, p_data, data_size);
          data_callback = f->param.data_callback;
          private_data = f->param.private_data;
          
          _dmx_unlock();
          data_callback((void*)f, tmp_data, data_size, private_data);
          _dmx_lock();
          have_data = TRUE;
        }
        if ((ret != SUCCESS || data_size == 0)
            && f->param.event_callback != NULL
            && f->param.timeout != APP_DMX_FILTER_NEVER_TIMEOUT)
        {
          if (mtos_ticks_get() * 10 - f->start_time > f->param.timeout)
          {
            memset(&evt_param, 0, sizeof(evt_param));
            evt_param.private_data = f->param.private_data;
            event_callback = f->param.event_callback;
            _dmx_unlock();
            event_callback((void*)f, APP_DMX_EVENT_DATA_TIMEOUT, &evt_param);
            _dmx_lock();
          }
        }
      }
      f = f_next;
    }
    _dmx_unlock();
    if (have_data)
    {
      mtos_task_sleep(10);
    }
    else
    {
      mtos_task_sleep(50);
    }
  }
}

static RET_CODE _create_dmx_service(app_dmx_thread *thread)
{
  BOOL result;
  RET_CODE ret;

  MT_ASSERT(thread != NULL);
  result = mtos_task_create((u8 *)"AppDmxService",
                        _dmx_data_polling,
                        NULL,
                        thread->thread_prio,
                        (u32 *)thread->thread_stack,
                        thread->thread_stack_size);
  if (!result)
  {
    return ERR_FAILURE;
  }
  thread->thread_status = APP_DMX_THREAD_RUNNING;

  ret = (RET_CODE)mtos_task_suspend(thread->thread_prio);
  MT_ASSERT(ret == SUCCESS);
  thread->thread_status = APP_DMX_THREAD_SUSPEND;
  return SUCCESS;
}


app_dmx_descrambler_handle app_dmx_alloc_descrambler(void)
{
  app_dmx_descrambler *d;

  _dmx_lock();
  do
  {
    d = _dlist_alloc_node();
    if (d == NULL)
    {
      break;
    }
    d->status = APP_DMX_DESCRAMBLER_STOPED;
  }while (0);
  _dmx_unlock();
  
  return (app_dmx_descrambler_handle)d;
}

RET_CODE app_dmx_free_descrambler(app_dmx_descrambler_handle h_desc)
{
  app_dmx_descrambler *d = (app_dmx_descrambler *)h_desc;
  RET_CODE ret = SUCCESS;

  _dmx_lock();
  do
  {
    if (!_check_valid_descrambler(d))
    {
      ret = ERR_FAILURE;
      break;
    }
    if (d->status != APP_DMX_DESCRAMBLER_STOPED)
    {
      APPLOGE("Please stop descrambler first\n");
      ret = ERR_FAILURE;
      break;
    }
    ret = _dlist_free_node(d);
  }while (0);
  _dmx_unlock();
  
  return ret;
}
RET_CODE app_dmx_set_descrambler_pid(app_dmx_descrambler_handle h_desc, u16 pid)
{
  app_dmx_descrambler *d = (app_dmx_descrambler *)h_desc;
  RET_CODE ret = SUCCESS;

  _dmx_lock();
  do
  {
    if (!_check_valid_descrambler(d))
    {
      ret = ERR_FAILURE;
      break;
    }
    
    d->pid = pid;
  }while (0);
  _dmx_unlock();
  
  return ret;
}
RET_CODE app_dmx_set_descrambler_evenkey(app_dmx_descrambler_handle h_desc, u8 *key, u32 len)
{
  app_dmx_descrambler *d = (app_dmx_descrambler *)h_desc;
  RET_CODE ret = SUCCESS;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);


  MT_ASSERT(key != NULL);
  
  _dmx_lock();
  do
  {
    if (!_check_valid_descrambler(d))
    {
      ret = ERR_FAILURE;
      break;
    }

    ret = dmx_get_chanid_bypid(dmx_dev, d->pid, &d->descrambler_id);
    if (ret != SUCCESS)
    {
      break;
    }
    ret = dmx_descrambler_set_even_keys(dmx_dev, d->descrambler_id, key, (s32)len);

    
  }while (0);
  _dmx_unlock();

  return ret;
}
RET_CODE app_dmx_set_descrambler_oddkey(app_dmx_descrambler_handle h_desc, u8 *key, u32 len)
{
  app_dmx_descrambler *d = (app_dmx_descrambler *)h_desc;
  RET_CODE ret = SUCCESS;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);


  MT_ASSERT(key != NULL);
  
  _dmx_lock();
  do
  {
    if (!_check_valid_descrambler(d))
    {
      ret = ERR_FAILURE;
      break;
    }


    ret = dmx_descrambler_set_odd_keys(dmx_dev, d->descrambler_id, key, (s32)len);

    
  }while (0);
  _dmx_unlock();

  return ret;
}
RET_CODE app_dmx_start_descrambler(app_dmx_descrambler_handle h_desc)
{
  app_dmx_descrambler *d = (app_dmx_descrambler *)h_desc;
  RET_CODE ret = SUCCESS;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  _dmx_lock();
  do
  {
    if (!_check_valid_descrambler(d))
    {
      ret = ERR_FAILURE;
      break;
    }
    ret = dmx_get_chanid_bypid(dmx_dev, d->pid, &d->descrambler_id);
    if (ret != SUCCESS)
    {
      break;
    }
    ret = dmx_descrambler_onoff(dmx_dev, d->descrambler_id, TRUE);
    if (ret != SUCCESS)
    {
      break;
    }

    d->status = APP_DMX_DESCRAMBLER_STARTED;
    
  }while (0);
  _dmx_unlock();

  return ret;
}
RET_CODE app_dmx_stop_descrambler(app_dmx_descrambler_handle h_desc)
{
  app_dmx_descrambler *d = (app_dmx_descrambler *)h_desc;
  RET_CODE ret = SUCCESS;
  void *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  _dmx_lock();
  do
  {
    if (!_check_valid_descrambler(d))
    {
      ret = ERR_FAILURE;
      break;
    }
    ret = dmx_descrambler_onoff(dmx_dev, d->descrambler_id, FALSE);
    
    if (ret != SUCCESS)
    {
      break;
    }

    d->status = APP_DMX_DESCRAMBLER_STOPED;
  }while (0);
  _dmx_unlock();

  return ret;
}





