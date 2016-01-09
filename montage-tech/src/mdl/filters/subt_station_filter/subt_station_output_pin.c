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
#include "mtos_sem.h"

//drv
#include "lib_rect.h"
#include "common.h"
#include "drv_dev.h"
#include "region.h"
#include "display.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "drv_misc.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "output_pin.h"
#include "ifilter.h"

#include "transf_output_pin.h"
#include "common_filter.h"
#include "subt_station_filter.h"

#include "subt_station_database.h"
#include "subt_station_parse.h"
#include "subt_station_output_pin_intra.h"



static subt_station_out_pin_priv_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((subt_station_out_pin_t *)_this)->private_data;
}






static RET_CODE subt_station_out_decide_buffer_size(handle_t _this)
{
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);
  imem_allocator_t *p_alloc = p_priv->p_alloc;

  p_alloc->get_properties(p_alloc, &p_priv->properties);
  p_priv->properties.use_virtual_space = TRUE;
  p_priv->properties.buffers = 1;
  
  p_priv->properties.buffer_size = p_priv->buf_size;
  p_alloc->set_properties(p_alloc, &p_priv->properties, NULL);
  return SUCCESS;
}



void subt_station_out_push_down(handle_t _this, u8 line_num, u8 *p_data)
{
  transf_output_pin_t *p_out = (transf_output_pin_t *)_this;
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);
  subt_station_content_t *p_content = (subt_station_content_t *)p_data;
  u8 i = 0;
  str_param_t *p_str = NULL;
  str_data_t *p_temp = NULL;

  memset(p_priv->p_out_buf, 0 ,KBYTES);
  p_out->query_sample(p_out, &p_priv->p_out_sample);
  p_priv->p_out_sample->p_data = p_priv->p_out_buf;
#if 0
  if(p_content->codec_fmt == SUBT_CODEC_UTF8)
  {
    p_priv->p_out_sample->context = 4;// utf8
  }
  else
  {
    p_priv->p_out_sample->context = 1;// gb2312
  }
  p_priv->p_out_sample->format.stream_type = MT_FILE_DATA;
  p_priv->p_out_sample->payload = line_num;
  p_priv->p_out_sample->data_offset = 2;

  p_string = p_priv->p_out_buf;
  
  for(i = 0; i < line_num; i ++)
  {
    *p_string++ = 0x00; //start 2 bytes
    *p_string++ = 0x00;
    
    *p_string++ = 0xff; //end 2 bytes
    *p_string++ = 0xff; 

    len = 6 +  strlen(&p_content->event[i][0]); //len
    
    *p_string++ = len & 0xff; //len 2 bytes
    *p_string++ = (len >>8) & 0xff;
    
    memcpy(p_string, &p_content->event[i][0], strlen(&p_content->event[i][0]));
    
    p_string += strlen(&p_content->event[i][0]);
  }
#else
  p_str = (str_param_t *)p_priv->p_out_sample->p_data;


  if(p_content->codec_fmt == SUBT_CODEC_UTF8)
  {
   p_str->fmt = STR_UTF8;
  }
  else
  {
    p_str->fmt = STR_GB2312;
  }


  p_str->str_union.str_data.lines = 0;
  p_str->str_union.str_data.p_data = (str_data_t *)((u8 *)p_str + sizeof(str_param_t));

  p_temp = p_str->str_union.str_data.p_data;
  for(i = 0; i < line_num; i ++)
  {
    p_temp->str_style = 0;
    p_temp->str_len = strlen(&p_content->event[i][0]);
    p_temp->str_len = (p_temp->str_len + 3) / 4 * 4;
    
    if(0 == p_temp->str_len)
    {
      continue;
    }

    p_temp->align = p_priv->align;
    p_temp->p_str = (void *)((u8 *)p_temp + sizeof(str_data_t));
    memcpy(p_temp->p_str, &p_content->event[i][0], p_temp->str_len);

    p_temp = (str_data_t *)((u8 *)p_temp + p_temp->str_len + sizeof(str_data_t));
    p_str->str_union.str_data.lines ++;
  }
  p_priv->p_out_sample->format.stream_type = MT_FILE_DATA;
#endif
  p_out->push_down(p_out,p_priv->p_out_sample);
}

static void subt_station_out_report(handle_t _this, void *p_state)
{
  
}

BOOL subt_station_out_show(handle_t _this, void *p_data)
{
  subt_station_content_t *p_content = (subt_station_content_t *)p_data;
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);

  if(p_priv->last_start != p_content->start
      || p_priv->last_end != p_content->end)
  {
    subt_station_out_push_down(_this, p_content->line_num, p_data);
    p_priv->last_start = p_content->start;
    p_priv->last_end = p_content->end;
    return TRUE;
  }
  return FALSE;
}

u32 subt_satation_out_get_showed_time(handle_t _this, u32 *p_start, u32 *p_end)
{
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);

  *p_start = p_priv->last_start;
  *p_end = p_priv->last_end;
  return SUCCESS;
}

void subt_station_out_hide(handle_t _this, void *p_data)
{
  transf_output_pin_t *p_out = (transf_output_pin_t *)_this;
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);
   str_param_t *p_str = NULL;

  memset(p_priv->p_out_buf, 0 ,KBYTES);
  p_out->query_sample(p_out, &p_priv->p_out_sample);
  p_priv->p_out_sample->p_data = p_priv->p_out_buf;

  p_str = (str_param_t *)p_priv->p_out_sample->p_data;

  p_str->fmt = STR_COLOR;
  p_str->str_union.clr_data.color = 0;
  p_out->push_down(p_out,p_priv->p_out_sample);

}

void subt_station_out_config(handle_t _this, str_align_t align)
{
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);

  p_priv->align = align;

}
static RET_CODE subt_station_out_pin_on_start(handle_t _this)
{
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);

  memset(&p_priv->report, 0, sizeof(subt_station_out_report_t));
  p_priv->p_out_buf = mtos_malloc(KBYTES);
  return SUCCESS;
}

static RET_CODE subt_station_out_pin_on_stop(handle_t _this)
{
  subt_station_out_pin_priv_t *p_priv = get_priv(_this);
  if(p_priv->p_out_buf != NULL)
  {
    mtos_free(p_priv->p_out_buf);
    p_priv->p_out_buf = NULL;
  }
  return SUCCESS;
}

subt_station_out_pin_t *subt_station_out_pin_create
  (subt_station_out_pin_t *p_pin, interface_t *p_owner)
{
  subt_station_out_pin_priv_t *p_priv = NULL;
  ipin_t *p_ipin = NULL;
  base_output_pin_t *p_output_pin = NULL;
//  interface_t *p_interface = NULL;
  transf_output_pin_para_t transf_pin_para = {0};
  media_format_t media_format = {MT_FILE_DATA};
  interface_t *p_interface = (interface_t *)p_pin;
  //check input parameter
  MT_ASSERT(p_pin != NULL);

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "subt_station_out_pin";
  transf_output_pin_create(&p_pin->m_pin, &transf_pin_para);

  p_interface->report = subt_station_out_report;
  //init private date
  p_priv = &(p_pin->private_data);
  memset(p_priv, 0x0, sizeof(subt_station_out_pin_priv_t));
  p_priv->p_this = (u8 *)p_pin;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);

//  p_priv->buffers = 1;
  p_priv->buf_size = KBYTES;



  //over load virtual function
  p_output_pin = (base_output_pin_t *)p_pin;
  p_output_pin->decide_buffer_size = subt_station_out_decide_buffer_size;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  p_ipin->on_start = subt_station_out_pin_on_start;
  p_ipin->on_stop = subt_station_out_pin_on_stop;

  return p_pin;
}

