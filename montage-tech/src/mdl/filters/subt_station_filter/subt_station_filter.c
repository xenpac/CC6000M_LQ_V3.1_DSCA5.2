/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "common.h"
#include "drv_dev.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "lib_rect.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "transf_input_pin.h"
#include "transf_output_pin.h"

#include "drv_misc.h"

#include "ifilter.h"
#include "transf_filter.h"
#include "common_filter.h"
#include "subt_station_filter.h"
#include "subt_station_database.h"
#include "subt_station_parse.h"
#include "subt_station_input_pin_intra.h"
#include "subt_station_output_pin_intra.h"
#include "subt_station_filter_intra.h"


static subt_station_filter_priv_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((subt_station_filter_t *)_this)->private_data;
}

static void process_in_subt(subt_station_filter_priv_t *p_priv)
{
  RET_CODE ret = SUCCESS;
  u8 *p_addr = NULL;
  u32 size = 0;
  u32 pts_in_subt = 0;
  u32 last_start = 0, last_end = 0;
  media_sample_t sample = {{0}};
  u32 pts_ms = 0;

  if(p_priv->subt_data_get == NULL)
  {
    return;
  }
  pts_ms = p_priv->pts_get();

  if(pts_ms == 0)
  {
    return;
  }
  OS_PRINTF("pts subt in %d \n", pts_ms);
  
  ret = p_priv->subt_data_get(&p_addr, (int *)&size, (int *)&pts_in_subt);
  if(size > 0 && ret == SUCCESS)
  {
    //copy data to priv
    size = size > MAX_SUBT_DATA_SIZE ? MAX_SUBT_DATA_SIZE : size;
    memcpy(p_priv->subt_data, p_addr, size);
    p_priv->subt_data[MAX_SUBT_DATA_SIZE - 1] = 0;
    sample.p_data = p_priv->subt_data;
    sample.payload = size;
    sst_in_pin_pre_parse_in_subt((handle_t)&p_priv->m_in_pin, &sample);
  }

    
  // find a subt to show it
  sample.context = pts_ms;
  ret = sst_in_pin_process_internal_subt((handle_t)&p_priv->m_in_pin, &sample);
  if(ret == SUCCESS)
  {
    p_priv->nomal_play = TRUE;
  }


  {
    //check whether need hide it
    subt_satation_out_get_showed_time((handle_t)&p_priv->m_out_pin, &last_start, &last_end);
   if(pts_ms > last_end || pts_ms < last_start)
    {
      // hide it
      if(p_priv->state != STATE_HIDE)
      {
        subt_station_out_hide((handle_t)&p_priv->m_out_pin, (void *)NULL);
        p_priv->state = STATE_HIDE;
        OS_PRINTF("SST_INT_HIDE_SUBT %d\n",pts_ms);
      }        
    }

    if(pts_ms < last_start && p_priv->nomal_play)
    {
      //seek back
      OS_PRINTF("seek back %d \n", pts_ms);
      p_priv->nomal_play = FALSE;
      //reset the db      
    }
  }
}


static void process_out_subt(subt_station_filter_priv_t *p_priv)
{
  u32 pts_ms = 0;
  subt_station_key_point_t  key = {0};
  RET_CODE ret = SUCCESS;

    pts_ms = p_priv->pts_get();
    if(pts_ms == 0)
    {
      return;
    }

  //check the pts in db?
    ret = sst_db_find_out_item(pts_ms, &key);
    if(ret == SUCCESS)
    {
     // OS_PRINTF("SST_INT_PTS_UPDATE %d\n",key.start);
      if(p_priv->start != key.start)
      {
        p_priv->start = key.start;
        sst_in_pin_request_data((handle_t)&p_priv->m_in_pin, key.start, key.pos, 1);
      }
    }
    else if(ret == ERR_TIMEOUT)
    {
      if(p_priv->state != STATE_HIDE)
      {
        OS_PRINTF("SST_INT_HIDE_SUBT %d\n",pts_ms);
        subt_station_out_hide((handle_t)&p_priv->m_out_pin, (void *)NULL);
        p_priv->state = STATE_HIDE;
      }
    }
    else
    {
      //notify filter to get new subt
      OS_PRINTF("SST_INT_PTS_UPDATE2 %d\n", pts_ms);
     // sst_in_pin_request_data((handle_t)&p_priv->m_in_pin, pts_ms, 0, 0);
    }  
    sst_in_pin_pre_scan((handle_t)&p_priv->m_in_pin);
  }
static void active_enter(handle_t _this)
{
  subt_station_filter_priv_t *p_priv = get_priv(_this);
  
 // OS_PRINTF("subt station filter active enter...\n");

  if(p_priv->pts_get == NULL)
  {
    return;
  }

  if(!p_priv->internal_subt)
  {
    process_out_subt(p_priv);
  }
  else
  {
    process_in_subt(p_priv);
  }  
  return;
}


static RET_CODE subt_station_on_command(handle_t _this, icmd_t *p_cmd)
{
  subt_station_filter_priv_t *p_priv = get_priv(_this);
  subt_station_params_cfg_t *p_cfg = NULL;
  ifilter_t *p_ifilter = NULL;
  
  
  switch (p_cmd->cmd)
  {
    case SUBT_STATION_CMD_CONFIG:
      p_cfg = (subt_station_params_cfg_t *)p_cmd->p_para;
      //p_priv->type = p_cfg->type;
      p_priv->pts_get = p_cfg->pts_get;
      p_priv->internal_subt = p_cfg->internal_subt;
      p_priv->subt_data_get = p_cfg->subt_data_get;
      
      sst_in_pin_config(&p_priv->m_in_pin,p_cfg->type, p_priv->internal_subt);
      subt_station_out_config(&p_priv->m_out_pin, p_cfg->align);
      break;
    case SUBT_STATION_CMD_START:
      p_ifilter = (ifilter_t *)_this;
      p_ifilter->set_active_enter(_this, active_enter);
    default:
      break;
  }
  return SUCCESS;
}



static RET_CODE subt_station_on_evt(handle_t _this, os_msg_t *p_msg)
{
  subt_station_filter_priv_t *p_priv = get_priv(_this);

  switch(p_msg->content)
  {
    case SST_INT_SHOW_SUBT:
      // call out pin to show this subt
      if(subt_station_out_show((handle_t)&p_priv->m_out_pin, (void *)p_msg->para1))
      {
        p_priv->state = STATE_SHOW;
      }
      break;
    default:
      break;
  }
  return SUCCESS;
}


static RET_CODE subt_station_on_open(handle_t _this)
{
  subt_station_filter_priv_t *p_priv = get_priv(_this);

  p_priv->state = STATE_HIDE;
  sst_db_init();
  sst_db_reset();
  sst_db_set_item_src(p_priv->internal_subt);
  return SUCCESS;
}

static RET_CODE subt_station_on_close(handle_t _this)
{
  sst_db_deinit();
  return SUCCESS;
}


static RET_CODE subt_station_transform(handle_t _this, media_sample_t *p_in)
{
  return SUCCESS;
}

static void on_destroy(handle_t _this)
{
  mtos_free((void *)_this);
}

ifilter_t * subt_station_filter_create(void *p_para)
{
  subt_station_filter_priv_t *p_priv = NULL;
  subt_station_filter_t *p_ins = NULL;
  transf_filter_t *p_transf_filter = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_para_t transf_filter_para = {0};

  transf_filter_para.dummy = 0;

  //create filter
  p_ins = mtos_malloc(sizeof(subt_station_filter_t));
  MT_ASSERT(p_ins != NULL);
  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);

  //init private date
  p_priv = (subt_station_filter_priv_t *)&p_ins->private_data;
  memset(p_priv, 0, sizeof(subt_station_filter_priv_t));

  p_priv->p_this = (u8 *)p_ins; //this point
  
  //over loading the virtual function
  p_transf_filter = (transf_filter_t *)p_ins;
  p_transf_filter->transform = subt_station_transform;
  
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = subt_station_on_command;
  p_ifilter->on_open = subt_station_on_open;
  p_ifilter->on_close = subt_station_on_close;
  p_ifilter->on_process_evt = subt_station_on_evt;
  p_ifilter->_interface.on_destroy = on_destroy;
  
  //create it's pin
  subt_station_in_pin_create(&p_priv->m_in_pin, (interface_t *)p_ins);
  subt_station_out_pin_create(&p_priv->m_out_pin, (interface_t *)p_ins);
  return (ifilter_t *)p_ins;
}


