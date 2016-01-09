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
#include "mtos_misc.h"
#include "mtos_task.h"
#include "common.h"
#include "drv_dev.h"
#include "lib_memp.h"
#include "lib_rect.h"
#include "pdec.h"

#include "hal_misc.h"
//util
#include "class_factory.h"
#include "simple_queue.h"

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
#include "pic_input_pin_intra.h"
#include "pic_output_pin_intra.h"

#include "ifilter.h"
#include "transf_filter.h"
#include "eva_filter_factory.h"
#include "pic_filter.h"
#include "pic_filter_intra.h"
#include "lib_rect.h"
#include "common_filter.h"
#include "mp3_player_filter.h"
#include "net_filter_interface.h"
#include "fifo_db.h"

/*!
  dec line number
  */
#define DEC_LINE_NUM (1)

/*!
  dib_header_t for gif
  */
typedef struct tag_dib_header
{
  /*!
    header_sz
    */
  u32 header_sz;
  /*!
    width
    */
  s32 width;
  /*!
    height
    */
  s32 height;
  /*!
    nplanes
    */
  u16 nplanes;
  /*!
    bitspp
    */
  u16 bitspp;
  /*!
    compress_type
    */
  u32 compress_type;
  /*!
    bmp_bytesz
    */
  u32 bmp_bytesz;
  /*!
    hres
    */
  s32 hres;
  /*!
    vres
    */
  s32 vres;
  /*!
    ncolors
    */
  u32 ncolors;
  /*!
    nimpcolors
    */
  u32 nimpcolors;
}dib_header_t;


static void gif_loop_transform(handle_t _this);

static pic_filter_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((pic_filter_t *)_this)->private_data;
}

static void *pic_filter_malloc(pic_filter_private_t *p_priv, u32 size)
{
  if(p_priv->ins.p_heap != NULL)
  {
    return lib_memp_align_alloc(p_priv->ins.p_heap, size, 8);
  }
  else
  {
    return mtos_align_malloc(size, 8);
  }
}

static void pic_filter_free(pic_filter_private_t *p_priv, void *p_buffer)
{
  if(p_priv->ins.p_heap != NULL)
  {
    lib_memp_align_free(p_priv->ins.p_heap, p_buffer);
  }
  else
  {
    mtos_align_free(p_buffer);
  }
}

void pic_filter_free_sample_data(pic_filter_t *p_pic_filter)
{
  /*pic_filter_private_t *p_priv = NULL;

  if(p_pic_filter == NULL)
  {
    return;
  }

  p_priv = (pic_filter_private_t *)(&p_pic_filter->private_data);

  if(p_priv->p_output != NULL)
  {
    pic_filter_free(p_priv, p_priv->p_output);

    p_priv->p_output = NULL;
  }*/
}

static void pic_filter_free_all(pic_filter_private_t *p_priv)
{
  p_priv->is_pic_dec_getinfo = 0;

  if(p_priv->p_gif)
  {
    pic_filter_free(p_priv, p_priv->p_gif);
    
    p_priv->p_gif = NULL;
  }

  if(p_priv->p_input)
  {
    pic_filter_free(p_priv, p_priv->p_input);   
  }
  
  p_priv->p_input = NULL;
  

  if (p_priv->p_output)
  {
    pic_filter_free(p_priv, p_priv->p_output);
    
    p_priv->p_output = NULL;
  }  
}

static RET_CODE pic_on_start(handle_t _this)
{
  pic_filter_private_t *p_priv = get_priv(_this);

  p_priv->is_gif_render = 0;
  p_priv->pic_dec_inerrupted = 0;
  p_priv->is_pic_dec_getinfo = 0;
  p_priv->payload = 0;
  p_priv->frm_index = 0;
  p_priv->frm_cnt = 0;
  p_priv->p_gif = NULL;
  
  return SUCCESS;
}

static RET_CODE pic_on_stop(handle_t _this)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = NULL;
  drv_dev_t *p_pic_dev = NULL;

  p_ifilter = (ifilter_t *)p_priv->p_this;
  p_ifilter->set_active_enter(_this, NULL);
  
  //if((p_priv->frm_cnt != 0) && (p_priv->dec_mode != DEC_ALL_MODE))
  {
    p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
    pdec_stop(p_pic_dev, &p_priv->ins);
    p_priv->frm_cnt = 0;
  }

  pic_filter_free_all(p_priv);

  p_priv->payload = 0;
  p_priv->pic_dec_inerrupted = 0;
  //fix
  p_priv->ins.image_fmt = IMAGE_FORMAT_UNKNOWN;
  return SUCCESS;
}

static void pic_on_destroy(handle_t _this)
{
  mtos_free((void *)_this);
}

static RET_CODE pic_filter_cfg(pic_filter_private_t *p_priv, pic_cfg_t *p_cfg)
{
  os_msg_t msg = {0};
  ifilter_t *p_ifilter = (ifilter_t *)p_priv->p_this;
  
  if (p_priv->p_input)
  {
    pic_filter_free(p_priv, p_priv->p_input);
    p_priv->p_input = NULL;
  }
  
  MT_ASSERT(p_cfg != NULL);

  p_priv->size.w = p_cfg->size.w;
  p_priv->size.h = p_cfg->size.h;
  p_priv->flip = p_cfg->flip;
  p_priv->out_fmt = p_cfg->out_fmt;
  p_priv->dec_mode = p_cfg->dec_mode;
  p_priv->ins.p_heap = p_cfg->p_heap;
  p_priv->input_size = p_cfg->in_size;
  p_priv->handle = p_cfg->handle;
  p_priv->is_thumb = p_cfg->is_thumb;
  p_priv->scale_mode = p_cfg->scale_mode;
  p_priv->use_bg_color = p_cfg->use_bg_color;

  //if do not cfg in size, we will use push sample data on recieve.  
  if(p_cfg->in_size > 0)
  {
    p_priv->p_input = pic_filter_malloc(p_priv, p_cfg->in_size);
    
    if(p_priv->p_input == NULL)
    {
      msg.content = PIC_IS_TOO_LARGE;
      msg.para1 = p_cfg->handle;
      p_ifilter->send_message_out(p_ifilter, &msg);  
      pic_in_pin_cfg(&p_priv->m_in_pin, 0, NULL, p_cfg->is_push);
      return ERR_FAILURE;
    }
  }

  //if(p_cfg->in_size > 0)
  {
    pic_in_pin_cfg(&p_priv->m_in_pin, p_cfg->in_size, p_priv->p_input, p_cfg->is_push);
  }
  return SUCCESS;
}

static RET_CODE pic_on_evt(handle_t _this, os_msg_t *p_msg)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  
  switch (p_msg->content)
  {
    case PIC_FILTER_STOP:
      pic_on_stop(_this);
      break;
    case PIC_FILTER_CFG:
      return pic_filter_cfg(p_priv, (pic_cfg_t *)p_msg->para2);
    case PIC_FILTER_PAUSE_GIF:
      p_priv->is_gif_render = 0;
      break;
    case PIC_FILTER_RESUME_GIF:
      p_priv->is_gif_render = 1;
      break;
    default:
      break;
  }
  
  return SUCCESS;
}

static RET_CODE pic_on_command(handle_t _this, icmd_t *p_cmd)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)_this;
  
  switch (p_cmd->cmd)
  {
    case PIC_FILTER_STOP:
      p_ifilter->forward_command(_this, p_cmd);
      break;
    case PIC_FILTER_INTERRUPT_PIC_DEC:
      p_priv->pic_dec_inerrupted = 1;
      #ifndef WIN32
      pdec_forced_stop(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC), &p_priv->ins);
      #endif
      break;
    case PIC_FILTER_CFG:
      p_ifilter->forward_command(_this, p_cmd);
      if(p_priv->input_size && (p_priv->p_input == NULL))
      {
        return ERR_FAILURE;
      }
      break;
    case PIC_FILTER_PAUSE_GIF:
    case PIC_FILTER_RESUME_GIF:
      p_ifilter->forward_command(_this, p_cmd);
      break;
      
    case PIC_FILTER_UN_CFG:
      pic_filter_free_all(p_priv);     
      break;   
      
    default:
      break;
  }

  return SUCCESS;
}


static void pic_calc_scale(rect_size_t *p_src, rect_size_t *p_max, pic_flip_rotate_type_t flip,
  rect_size_t *p_dst, pic_param_t *p_pic_param, scale_mode_t scale_mode)
{
  rect_size_t src_size = {0}, dst_size = {0};
  MT_ASSERT(p_src != NULL);
  MT_ASSERT(p_max != NULL);
  MT_ASSERT(p_dst != NULL);

  switch(flip)
  {
    case PIC_R_90:
    case PIC_R_270:
    case PIC_F_H_R_90:
    case PIC_F_V_R_90:
      src_size.w = p_src->h;
      src_size.h = p_src->w;
      break;

    default:
      src_size.w = p_src->w;
      src_size.h = p_src->h;
      break;
  }  
  
  switch(scale_mode)
  {
    case SCALE_MODE_NONE:
      p_pic_param->scale_w_num = 1;
      p_pic_param->scale_w_demo = 1;
      p_pic_param->scale_h_num = 1;
      p_pic_param->scale_h_demo = 1;

      dst_size.w = src_size.w;
      dst_size.h = src_size.h;      
      break;

    case SCALE_MODE_FULL:
      if(src_size.w <= p_max->w)
      {
        p_pic_param->scale_w_num = 1;
        p_pic_param->scale_w_demo = 1;

        dst_size.w = src_size.w;
      }
      else
      {
        p_pic_param->scale_w_num = p_max->w;
        p_pic_param->scale_w_demo = src_size.w;

        dst_size.w = p_max->w;      
      }

      if(src_size.h <= p_max->h)
      {
        p_pic_param->scale_h_num = 1;
        p_pic_param->scale_h_demo = 1;
        
        dst_size.h = src_size.h;  
      }
      else
      {
        p_pic_param->scale_h_num = p_max->h;
        p_pic_param->scale_h_demo = src_size.h;
        
        dst_size.h = p_max->h;     
      }
      break;

    case SCALE_MODE_SMART:
      if(src_size.w <= p_max->w)
      {
        if(src_size.h <= p_max->h)
        {
          p_pic_param->scale_w_num = 1;
          p_pic_param->scale_w_demo = 1;
          p_pic_param->scale_h_num = 1;
          p_pic_param->scale_h_demo = 1;

          dst_size.w = src_size.w;
          dst_size.h = src_size.h;
        }
        else
        {
          p_pic_param->scale_w_num = p_max->h;
          p_pic_param->scale_w_demo = src_size.h;
          p_pic_param->scale_h_num = p_max->h;
          p_pic_param->scale_h_demo = src_size.h;

          dst_size.w = src_size.w * p_max->h / src_size.h;
          dst_size.h = p_max->h;
        }
      }
      else
      {
        if(src_size.h <= p_max->h)
        {
          p_pic_param->scale_w_num = p_max->w;
          p_pic_param->scale_w_demo = src_size.w;
          p_pic_param->scale_h_num = p_max->w;
          p_pic_param->scale_h_demo = src_size.w;

          dst_size.w = p_max->w;
          dst_size.h = src_size.h * p_max->w / src_size.w;
        }
        else
        {
          if(src_size.w * p_max->h >= src_size.h * p_max->w)
          {
            p_pic_param->scale_w_num = p_max->w;
            p_pic_param->scale_w_demo = src_size.w;
            p_pic_param->scale_h_num = p_max->w;
            p_pic_param->scale_h_demo = src_size.w;

            dst_size.w = p_max->w;
            dst_size.h = p_max->w * src_size.h / src_size.w;
          }
          else
          {
            p_pic_param->scale_w_num = p_max->h;
            p_pic_param->scale_w_demo = src_size.h;
            p_pic_param->scale_h_num = p_max->h;
            p_pic_param->scale_h_demo = src_size.h;

            dst_size.w = src_size.w * p_max->h / src_size.h;
            dst_size.h = p_max->h;
          }
        }
      }
      break;

    default:
      MT_ASSERT(0);
      break;
  }
  
  switch(flip)
  {
    case PIC_R_90:
    case PIC_R_270:
    case PIC_F_H_R_90:
    case PIC_F_V_R_90:
      p_pic_param->disp_width = dst_size.h ;
      p_pic_param->disp_height = dst_size.w ;
      break;

    default:
      p_pic_param->disp_width = dst_size.w ;
      p_pic_param->disp_height = dst_size.h ;
      break;
  }  

  p_dst->w = dst_size.w;
  p_dst->h = dst_size.h;
}

static void jpeg_transform(handle_t _this,
  pic_param_t *p_pic_param, pic_info_t *p_pic_info, void *p_src)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  ifilter_t *p_ifilter = (ifilter_t *)_this;
  rend_param_t *p_render = NULL;
  rect_size_t dst_size = {0};
  drv_dev_t *p_pic_dev = NULL;
  media_sample_t *p_out_sample = NULL;
  os_msg_t msg = {0};
  RET_CODE ret = SUCCESS;
  u32 out_size = 0;
#ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
#else
  chip_ic_t chip_ic_id = IC_MAGIC;
#endif

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);

  pic_calc_scale(&p_priv->src_size,
    &p_priv->size, p_priv->flip, &dst_size, p_pic_param, p_priv->scale_mode);

  switch (chip_ic_id)
  {
  case IC_MAGIC:
  case IC_WIZARDS:
    p_pic_param->output_format = PIX_FMT_ARGB8888;
    break;
  case IC_ENSEMBLE:
    p_pic_param->output_format = PIX_FMT_CBY0CRY18888;
    break;
  case IC_WARRIORS:
  case IC_SONATA:
  case IC_CONCERTO:
  case IC_JAZZ:
    p_pic_param->output_format = PIX_FMT_AYCBCR8888;
    break;

  default:
    p_pic_param->output_format = PIX_FMT_AYCBCR8888;
    break;
  }
  out_size = (dst_size.w + 32) * (dst_size.h + 32) * 4;
  if (p_priv->p_output)
  {
    pic_filter_free(p_priv, p_priv->p_output);
    p_priv->p_output = NULL;
  }
  p_priv->p_output = pic_filter_malloc(p_priv, out_size);
  if(p_priv->p_output == NULL)
  {
    pdec_stop(p_pic_dev, &p_priv->ins);

    pic_filter_free_all(p_priv);
    
    msg.content = PIC_IS_TOO_LARGE;
    msg.para1 = p_priv->handle;
    p_ifilter->send_message_out(p_ifilter, &msg);     

    return;
  }

  p_render = (rend_param_t *)p_priv->p_output;
  memset(p_render, 0, sizeof(rend_param_t));
  p_render->fmt = p_pic_param->output_format;
  
  p_pic_param->p_src_buf = p_src;
  p_pic_param->p_dst_buf = (u8 *)p_priv->p_output + sizeof(rend_param_t);
  p_pic_param->flip = p_priv->flip;
  //p_pic_param->output_format = p_render->fmt;
  p_pic_param->src_size = p_priv->payload;

  if (!p_priv->is_pic_dec_getinfo)
  {
    pdec_stop(p_pic_dev, &p_priv->ins);

    pic_filter_free_all(p_priv);
    OS_PRINTF("\n##pic line %lu, chain [%s] pic filter used error!!\n",
      __LINE__, p_ifilter->_interface.iid.name);
    return;
  }
  
  ret = pdec_setinfo(p_pic_dev, p_pic_param, &p_priv->ins);
  //fix 42055
  p_render->fmt = p_pic_param->output_format;
  if(ret != SUCCESS)
  {
    pdec_stop(p_pic_dev, &p_priv->ins);

    pic_filter_free_all(p_priv);
    
    msg.content = PIC_DATA_ERROR;
    msg.para1 = p_priv->handle;
    p_ifilter->send_message_out(p_ifilter, &msg);     

    return;
  }
  
  if ((p_priv->pic_dec_inerrupted == 1)
    || (!p_priv->is_pic_dec_getinfo))
  {
    pic_filter_free_all(p_priv);
    return ;
  }
  ret = pdec_start(p_pic_dev, &p_priv->ins);
  if(ret != SUCCESS)
  {
    pdec_stop(p_pic_dev, &p_priv->ins);

    pic_filter_free_all(p_priv);
    
    if (ret == FORCED_STOP)
    {
      return;
    }
    
    if (ret == MEM_LACK_ERR)
    {
      msg.content = PIC_IS_TOO_LARGE;
    }
    else if(ret == SRC_DATA_ERR)
    {
      msg.content = PIC_DATA_ERROR;
    }
    else if(ret == UNSOPPORT_PICTURE)
    {
      msg.content = PIC_IS_UNSUPPORT;
    }

    msg.para1 = p_priv->handle;
    p_ifilter->send_message_out(p_ifilter, &msg);     

    return;
  }
  
  if (p_priv->pic_dec_inerrupted == 1)
  {
    pic_filter_free_all(p_priv);
    return ;
  }
  
  pdec_stop(p_pic_dev, &p_priv->ins);

  p_render->p_data = (u8 *)p_priv->p_output + sizeof(rend_param_t);
  MT_ASSERT(((u32)(p_render->p_data) % 8) == 0);

  p_render->rend_rect.top = 0;
  p_render->rend_rect.left = 0;
  p_render->rend_rect.right = p_priv->size.w;
  p_render->rend_rect.bottom = p_priv->size.h;

  p_render->fill_rect.top = p_pic_param->disp_y;
  p_render->fill_rect.left = p_pic_param->disp_x;
  p_render->fill_rect.right = p_pic_param->disp_width;
  p_render->fill_rect.bottom = p_pic_param->disp_height;
  p_render->image_width = p_pic_param->disp_width;
  p_render->image_height = p_pic_param->disp_height;
  if (p_render->fmt == PIX_FMT_CBY0CRY18888)
  {
    p_render->pitch = p_pic_param->disp_stride * 2;//argb8888 ,yuv444 *4
  }
  else
  {
    p_render->pitch = p_pic_param->disp_stride * 4;
  }
  p_render->use_bg_color = p_priv->use_bg_color;
  p_render->bg_color = 0x0;
  p_render->pal_entry = NULL;
  p_render->pal_num = 0;

  p_out_pin->query_sample(p_out_pin, &p_out_sample);
  MT_ASSERT(p_out_sample != NULL);

  p_out_sample->p_data = (u8 *)p_priv->p_output;
  p_out_sample->format.stream_type = MT_IMAGE;
  p_out_sample->format.media_type = MF_JPEG;

  p_out_pin->push_down(p_out_pin, p_out_sample);
}

static void gif_loop_transform(handle_t _this)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  drv_dev_t *p_pic_dev = NULL;
  rend_param_t *p_render = NULL;
  ifilter_t *p_ifilter = (ifilter_t *)p_priv->p_this;
  gif_output_t *p_gif_dst = (gif_output_t *)p_priv->p_gif;
  os_msg_t msg = {0};
  RET_CODE ret = SUCCESS;
  u32 curn_time = 0, out_size = 0;

  if (!p_priv->is_gif_render)
  {
    return;
  }
  
  curn_time = mtos_ticks_get();
  if((curn_time - p_priv->last_time) < (p_priv->frm_delay_time / 10))
  {
    return;
  }
  
  p_priv->last_time = curn_time;

  p_gif_dst = (gif_output_t *)(p_priv->p_gif);
  MT_ASSERT(p_gif_dst != NULL);

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
  MT_ASSERT(p_pic_dev != NULL);
  
  out_size = (p_gif_dst->biWidth + 16) * (p_gif_dst->biHeight + 16) * 4;
  if (p_priv->p_output)
  {
    pic_filter_free(p_priv, p_priv->p_output);
  }
  p_priv->p_output = pic_filter_malloc(p_priv, out_size);
  if(p_priv->p_output == NULL)
  {
    pdec_stop(p_pic_dev, &p_priv->ins);

    pic_filter_free_all(p_priv);
    
    msg.content = PIC_IS_TOO_LARGE;
    msg.para1 = p_priv->handle;
    p_ifilter->send_message_out(p_ifilter, &msg);
    p_ifilter->set_active_enter(_this, NULL);
    return;
  }      

  p_render = (rend_param_t *)p_priv->p_output;

  memset(p_render, 0, sizeof(rend_param_t));

  switch(p_priv->dec_mode)
  {
    case DEC_ALL_MODE:
      p_render->rend_rect.top = 0;
      p_render->rend_rect.left = 0;
      p_render->rend_rect.right = p_priv->size.w;
      p_render->rend_rect.bottom = p_priv->size.h;

      p_render->fill_rect.top = 0;
      p_render->fill_rect.left = 0;
      p_render->fill_rect.right = p_gif_dst->biWidth;
      p_render->fill_rect.bottom = p_gif_dst->biHeight;
      p_render->image_width = p_gif_dst->biWidth;
      p_render->image_height = p_gif_dst->biHeight;
      p_render->pitch = (p_render->image_width * 4 + 7) / 8 * 8;

      p_render->pal_entry = NULL;
      p_render->pal_num = 0;
      p_render->fmt = PIX_FMT_ARGB8888;
      p_render->p_data = (u8 *)p_priv->p_output + sizeof(rend_param_t);

      memcpy(p_render->p_data, p_gif_dst->frmdata[p_priv->frm_index], p_gif_dst->frmsize);

      p_priv->frm_index++;
      if(p_priv->frm_index >= p_priv->frm_cnt)
      {
        p_render->is_last = TRUE;
        p_priv->frm_index = 0;
      }
      else
      {
        p_render->is_last = FALSE;
      }

      if(p_priv->is_thumb && p_render->is_last)
      {
        p_ifilter->set_active_enter(_this, NULL);  
      }
      
      p_out_pin->query_sample(p_out_pin, &p_out_sample);
      MT_ASSERT(p_out_sample != NULL);

      p_out_sample->p_data = (u8 *)p_priv->p_output;
      p_out_sample->format.stream_type = MT_IMAGE;
      p_out_sample->format.media_type = MF_GIF;
  
      p_out_pin->push_down(p_out_pin, p_out_sample);
      break;

    case DEC_FRAME_MODE:
      if ((p_priv->pic_dec_inerrupted == 1)
        || (!p_priv->is_pic_dec_getinfo))
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      
      ret = pdec_start(p_pic_dev, &p_priv->ins);
      if(ret != SUCCESS)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        if (ret == FORCED_STOP)
        {
          return;
        }
        
        if(ret == MEM_LACK_ERR)
        {
          msg.content = PIC_IS_TOO_LARGE;
        }
        else if(ret == SRC_DATA_ERR)
        {
          msg.content = PIC_DATA_ERROR;
        }
        else if(ret == UNSOPPORT_PICTURE)
        {
          msg.content = PIC_IS_UNSUPPORT;
        }

        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        p_ifilter->set_active_enter(_this, NULL);
        return;
      }
      
      if (p_priv->pic_dec_inerrupted == 1)
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      p_gif_dst = (gif_output_t *)(p_priv->p_gif);

      p_render->rend_rect.top = 0;
      p_render->rend_rect.left = 0;
      p_render->rend_rect.right = p_priv->size.w;
      p_render->rend_rect.bottom = p_priv->size.h;

      p_render->fill_rect.top = 0;
      p_render->fill_rect.left = 0;
      p_render->fill_rect.right = p_gif_dst->biWidth;
      p_render->fill_rect.bottom = p_gif_dst->biHeight;
      p_render->image_width = p_gif_dst->biWidth;
      p_render->image_height = p_gif_dst->biHeight;
      p_render->pitch = (p_render->image_width * 4 + 7) / 8 * 8;

      p_render->pal_entry = NULL;
      p_render->pal_num = 0;
      p_render->fmt = PIX_FMT_ARGB8888;
      p_render->p_data = (u8 *)p_priv->p_output + sizeof(rend_param_t);

      memcpy(p_render->p_data, p_gif_dst->frmdata[0], p_gif_dst->frmsize);

      if((p_priv->frm_index == 0) && p_priv->is_new_gif)
      {
        p_render->use_bg_color = TRUE;
        p_render->bg_color = 0x0;
        p_priv->is_new_gif = FALSE;
      }
      if(0 == p_priv->frm_index)
        {
          p_render->is_first = TRUE;
        }
      else
        {
        p_render->is_first = FALSE;
        }
      p_priv->frm_index ++;
      if(p_gif_dst->b_DecEnd || p_priv->frm_cnt == 1)
      {
        p_render->is_last = TRUE;
        p_priv->frm_index = 0;
      }
      else
      {
        p_render->is_last = FALSE;
      }

      if((p_priv->is_thumb && p_render->is_last) || p_priv->frm_cnt == 1)
      {
        p_ifilter = (ifilter_t *)p_priv->p_this;
        p_ifilter->set_active_enter(_this, NULL);  
      }

      p_out_pin->query_sample(p_out_pin, &p_out_sample);
      MT_ASSERT(p_out_sample != NULL);

      p_out_sample->p_data = (u8 *)p_priv->p_output;
      p_out_sample->format.stream_type = MT_IMAGE;
      p_out_sample->format.media_type = MF_GIF;      
      
      p_out_pin->push_down(p_out_pin, p_out_sample);
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  if(p_priv->p_output != NULL)
  {
    pic_filter_free(p_priv, p_priv->p_output);

    p_priv->p_output = NULL;
  }
}


static void gif_transform(handle_t _this,
  pic_param_t *p_pic_param, pic_info_t *p_pic_info, void *p_src)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  gif_output_t *p_gif_dst = NULL;
  u32 i = 0;
  u8 *p_buffer = NULL;
  drv_dev_t *p_pic_dev = NULL;
  ifilter_t *p_ifilter = NULL;
  RET_CODE ret = SUCCESS;
  os_msg_t msg = {0};
  rect_size_t dst_size = {0};
#ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
#else
  chip_ic_t chip_ic_id = IC_MAGIC;
#endif

  p_ifilter = (ifilter_t *)p_priv->p_this;

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);

  p_priv->frm_index = 0;
  p_priv->last_time = 0;
  p_priv->frm_delay_time = p_pic_info->daleytime;
  if (chip_ic_id == IC_CONCERTO)
  {
    if(p_priv->frm_delay_time < 100)
    {
      p_priv->frm_delay_time = 100;
    }
  }
  else
  {
    if(p_priv->frm_delay_time < 200)
    {
      p_priv->frm_delay_time = 200;
    }
  }
  p_priv->is_new_gif = 1;

  p_pic_param->p_src_buf = p_src;
  p_pic_param->flip = p_priv->flip;
  p_pic_param->output_format = p_priv->out_fmt;
  p_pic_param->src_size = p_priv->payload;
  p_pic_param->disp_stride = p_priv->size.w;

  p_pic_param->disp_width = p_pic_info->src_width;
  p_pic_param->disp_height = p_pic_info->src_height;

  pic_calc_scale(&p_priv->src_size, &p_priv->size, p_priv->flip, &dst_size, p_pic_param,
                           p_priv->scale_mode);

  switch(p_pic_param->dec_mode)
  {
    case DEC_ALL_MODE:
      p_pic_info->frame_nums = pdec_get_frmnum(p_pic_dev, &p_priv->ins);
      if(p_pic_info->frame_nums == 0)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        msg.content = PIC_DATA_ERROR;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      if(p_priv->p_gif)
      {
        pic_filter_free(p_priv, p_priv->p_gif);
        p_priv->p_gif = NULL;
      }
      p_priv->p_gif = pic_filter_malloc(p_priv,
        p_pic_info->frame_nums * p_pic_info->dst_frm_size + ((sizeof(gif_output_t) + 7) / 8 * 8));
      MT_ASSERT(p_priv->p_gif != NULL);
      memset(p_priv->p_gif, 0, 
        p_pic_info->frame_nums * p_pic_info->dst_frm_size + ((sizeof(gif_output_t) + 7) / 8 * 8));

      p_buffer = p_priv->p_gif;
      p_gif_dst = (gif_output_t *)p_buffer;
      p_buffer += ((sizeof(gif_output_t) + 7) / 8 * 8);
      MT_ASSERT((u32)p_buffer % 8 == 0);

      p_gif_dst->frmnum = p_pic_info->frame_nums;
      p_gif_dst->frmsize = p_pic_info->dst_frm_size;
      p_gif_dst->biHeight = p_pic_info->src_height;
      p_gif_dst->biWidth = p_pic_info->src_width;
      p_gif_dst->oncedecnum = p_pic_info->frame_nums;

      for(i = 0; i < p_pic_info->frame_nums; i ++)
      {
        p_gif_dst->frmdata[i] = p_buffer + i * p_pic_info->dst_frm_size;
      }

      p_pic_param->p_dst_buf = p_gif_dst;
      
      if (!p_priv->is_pic_dec_getinfo)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        OS_PRINTF("\n##pic line %lu, chain [%s] pic filter used error!!\n",
          __LINE__, p_ifilter->_interface.iid.name);
        return;
      }

      ret = pdec_setinfo(p_pic_dev, p_pic_param, &p_priv->ins);
      if(ret != SUCCESS)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        msg.content = PIC_DATA_ERROR;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      
      if ((p_priv->pic_dec_inerrupted == 1)
        || (!p_priv->is_pic_dec_getinfo))
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      
      ret = pdec_start(p_pic_dev, &p_priv->ins);
      if(ret == MEM_LACK_ERR
        || ret == SRC_DATA_ERR
        || ret == FORCED_STOP)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        if (ret == FORCED_STOP)
        {
          return;
        }
        
        if (ret == MEM_LACK_ERR)
        {
          msg.content = PIC_IS_TOO_LARGE;
        }
        else
        {
          msg.content = PIC_DATA_ERROR;
        }
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      
      if (p_priv->pic_dec_inerrupted == 1)
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      
      p_priv->is_gif_render = 1;
      pdec_stop(p_pic_dev, &p_priv->ins);
      p_priv->frm_cnt = p_pic_info->frame_nums;
      p_ifilter->set_active_enter(_this, gif_loop_transform);
      break;

    case DEC_FRAME_MODE:
      p_pic_info->frame_nums = pdec_get_frmnum(p_pic_dev, &p_priv->ins);
      if(p_priv->p_gif)
      {
        pic_filter_free(p_priv, p_priv->p_gif);
        p_priv->p_gif = NULL;
      }
      p_priv->p_gif = pic_filter_malloc(p_priv,
        p_pic_info->dst_frm_size + ((sizeof(gif_output_t) + 7) / 8 * 8));
      // MT_ASSERT(p_priv->p_gif != NULL);
      if(p_priv->p_gif == NULL)	  
      {
        pdec_stop(p_pic_dev, &p_priv->ins);
        pic_filter_free_all(p_priv);
        
        msg.content = PIC_IS_TOO_LARGE;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      memset(p_priv->p_gif, 0, p_pic_info->dst_frm_size + ((sizeof(gif_output_t) + 7) / 8 * 8));

      p_buffer = p_priv->p_gif;
      p_gif_dst = (gif_output_t *)p_buffer;
      p_buffer += ((sizeof(gif_output_t) + 7) / 8 * 8);
      MT_ASSERT((u32)p_buffer % 8 == 0);

      p_gif_dst->frmnum = p_pic_info->frame_nums;
      p_gif_dst->frmsize = p_pic_info->dst_frm_size;
      p_gif_dst->biHeight = p_pic_info->src_height;
      p_gif_dst->biWidth = p_pic_info->src_width;
      p_gif_dst->oncedecnum = 1;
      p_gif_dst->frmdata[0] = p_buffer;

      p_pic_param->p_dst_buf = p_gif_dst;
      
      if (!p_priv->is_pic_dec_getinfo)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        OS_PRINTF("\n##pic line %lu, chain [%s] pic filter used error!!\n",
          __LINE__, p_ifilter->_interface.iid.name);
        return;
      }

      ret = pdec_setinfo(p_pic_dev, p_pic_param, &p_priv->ins);
      if(ret != SUCCESS)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        msg.content = PIC_IS_TOO_LARGE;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      
      p_gif_dst->frmsize = p_pic_param->disp_stride * 4 * p_pic_param->disp_height;
      p_gif_dst->biHeight = p_pic_param->disp_height;
      p_gif_dst->biWidth = p_pic_param->disp_width;      

      p_priv->is_gif_render = 1;
      p_ifilter->set_active_enter(_this, gif_loop_transform);
      p_priv->frm_cnt = p_pic_info->frame_nums;
      break;

    default:
      MT_ASSERT(0);
      break;
  }
}

static void png_transform(handle_t _this,
  pic_param_t *p_pic_param, pic_info_t *p_pic_info, void *p_src)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  rend_param_t *p_render = NULL;
  //rect_size_t src_size = {0};
  rect_size_t dst_size = {0};
  drv_dev_t *p_pic_dev = NULL;
  media_sample_t *p_out_sample = NULL;
  ifilter_t *p_ifilter = NULL;
  os_msg_t msg = {0};
  RET_CODE ret = SUCCESS;
  u32 out_size = 0;

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);

  p_ifilter = (ifilter_t *)p_priv->p_this;

  //show do some scale function here.
  pic_calc_scale(&p_priv->src_size,
    &p_priv->size, p_priv->flip, &dst_size, p_pic_param, p_priv->scale_mode);
    
  switch(p_pic_param->dec_mode)
  {
    case DEC_ALL_MODE:
    case DEC_FRAME_MODE:
      out_size = (dst_size.w + 16) * (dst_size.h + 16) * 4;
      if (p_priv->p_output)
      {
        pic_filter_free(p_priv, p_priv->p_output);
      }
      p_priv->p_output = pic_filter_malloc(p_priv, out_size);
      if(p_priv->p_output == NULL)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        msg.content = PIC_IS_TOO_LARGE;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);   

        return;
      }      

      p_render = (rend_param_t *)p_priv->p_output;
      memset(p_render, 0, sizeof(rend_param_t));

      p_render->fmt = PIX_FMT_ARGB8888;

      p_pic_param->p_src_buf = p_src;
      p_pic_param->p_dst_buf = (u8 *)p_priv->p_output + sizeof(rend_param_t);
      p_pic_param->flip = p_priv->flip;
      p_pic_param->output_format = p_render->fmt;
      p_pic_param->src_size = p_priv->payload;

      if (!p_priv->is_pic_dec_getinfo)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        OS_PRINTF("\n##pic line %lu, chain [%s] pic filter used error!!\n",
          __LINE__, p_ifilter->_interface.iid.name);
        return;
      }

      ret = pdec_setinfo(p_pic_dev, p_pic_param, &p_priv->ins);
      if(ret != SUCCESS)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        msg.content = PIC_DATA_ERROR;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      
      if ((p_priv->pic_dec_inerrupted == 1)
        || (!p_priv->is_pic_dec_getinfo))
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      
      ret = pdec_start(p_pic_dev, &p_priv->ins);
      if(ret == MEM_LACK_ERR
        || ret == SRC_DATA_ERR
        || ret == FORCED_STOP)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        if (ret == FORCED_STOP)
        {
          return;
        }
        
        if (ret == MEM_LACK_ERR)
        {
          msg.content = PIC_IS_TOO_LARGE;
        }
        else
        {
          msg.content = PIC_DATA_ERROR;
        }
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      
      if (p_priv->pic_dec_inerrupted == 1)
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      
      pdec_stop(p_pic_dev, &p_priv->ins);

      p_render->p_data = (u8 *)p_priv->p_output + sizeof(rend_param_t);
      MT_ASSERT(((u32)(p_render->p_data) % 8) == 0);

      p_render->rend_rect.top = 0;
      p_render->rend_rect.left = 0;
      p_render->rend_rect.right = p_priv->size.w;
      p_render->rend_rect.bottom = p_priv->size.h;

      p_render->fill_rect.top = 0;
      p_render->fill_rect.left = 0;
      p_render->fill_rect.right = dst_size.w;
      p_render->fill_rect.bottom = dst_size.h;
      p_render->image_width = dst_size.w;
      p_render->image_height = dst_size.h;
      p_render->pitch = (p_render->image_width * 4 + 7) / 8 * 8;
      p_render->use_bg_color = p_priv->use_bg_color;
      p_render->bg_color = 0x0;
      p_render->pal_entry = NULL;
      p_render->pal_num = 0;

      p_out_pin->query_sample(p_out_pin, &p_out_sample);
      MT_ASSERT(p_out_sample != NULL);
      
      p_out_sample->p_data = (u8 *)p_priv->p_output;
      p_out_sample->format.stream_type = MT_IMAGE;
      p_out_sample->format.media_type = MF_PNG;

      p_out_pin->push_down(p_out_pin, p_out_sample);
      break;

    default:
      MT_ASSERT(0);
      break;
  }
}

static void bmp_transform(handle_t _this,
  pic_param_t *p_pic_param, pic_info_t *p_pic_info, void *p_src)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  rend_param_t *p_render = NULL;
  //rect_size_t src_size = {0};
  rect_size_t dst_size = {0};
  drv_dev_t *p_pic_dev = NULL;
  media_sample_t *p_out_sample = NULL;
  ifilter_t *p_ifilter = NULL;
  os_msg_t msg = {0};
  RET_CODE ret = SUCCESS;
  u32 out_size = 0;
  
  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);

  p_ifilter = (ifilter_t *)p_priv->p_this;

  pic_calc_scale(&p_priv->src_size,
    &p_priv->size, p_priv->flip, &dst_size, p_pic_param, p_priv->scale_mode);
    
  switch(p_pic_param->dec_mode)
  {
    case DEC_ALL_MODE:
    case DEC_FRAME_MODE:
      out_size = (dst_size.w + 16) * (dst_size.h + 16) * 4;
      if (p_priv->p_output)
      {
        pic_filter_free(p_priv, p_priv->p_output);
      }
      p_priv->p_output = pic_filter_malloc(p_priv, out_size);
      if(p_priv->p_output == NULL)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        msg.content = PIC_IS_TOO_LARGE;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);    

        return;
      }

      p_render = (rend_param_t *)p_priv->p_output;
      memset(p_render, 0, sizeof(rend_param_t));

      p_render->fmt = PIX_FMT_ARGB8888;

      p_pic_param->p_src_buf = p_src;
      p_pic_param->p_dst_buf = (u8 *)p_priv->p_output + sizeof(rend_param_t);
      p_pic_param->flip = p_priv->flip;
      p_pic_param->output_format = p_render->fmt;
      p_pic_param->src_size = p_priv->payload;

      if (!p_priv->is_pic_dec_getinfo)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        OS_PRINTF("\n##pic line %lu, chain [%s] pic filter used error!!\n",
          __LINE__, p_ifilter->_interface.iid.name);
        return;
      }

      ret = pdec_setinfo(p_pic_dev, p_pic_param, &p_priv->ins);
      if(ret != SUCCESS)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        msg.content = PIC_DATA_ERROR;
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      
      if ((p_priv->pic_dec_inerrupted == 1)
        || (!p_priv->is_pic_dec_getinfo))
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      
      ret = pdec_start(p_pic_dev, &p_priv->ins);
      if(ret == MEM_LACK_ERR
        || ret == SRC_DATA_ERR
        || ret == FORCED_STOP)
      {
        pdec_stop(p_pic_dev, &p_priv->ins);

        pic_filter_free_all(p_priv);
        
        if (ret == FORCED_STOP)
        {
          return;
        }
        
        if (ret == MEM_LACK_ERR)
        {
          msg.content = PIC_IS_TOO_LARGE;
        }
        else
        {
          msg.content = PIC_DATA_ERROR;
        }
        
        msg.para1 = p_priv->handle;
        p_ifilter->send_message_out(p_ifilter, &msg);     

        return;
      }
      
      if (p_priv->pic_dec_inerrupted == 1)
      {
        pic_filter_free_all(p_priv);
        return ;
      }
      
      pdec_stop(p_pic_dev, &p_priv->ins);

      p_render->p_data = (u8 *)p_priv->p_output + sizeof(rend_param_t);
      MT_ASSERT(((u32)(p_render->p_data) % 8) == 0);

      p_render->rend_rect.top = 0;
      p_render->rend_rect.left = 0;
      p_render->rend_rect.right = p_priv->size.w;
      p_render->rend_rect.bottom = p_priv->size.h;

      p_render->fill_rect.top = 0;
      p_render->fill_rect.left = 0;
      p_render->fill_rect.right = dst_size.w;
      p_render->fill_rect.bottom = dst_size.h;
      p_render->image_width = dst_size.w;
      p_render->image_height = dst_size.h;
      p_render->pitch = (p_render->image_width * 4 + 7) / 8 * 8;
      p_render->use_bg_color = p_priv->use_bg_color;
      p_render->bg_color = 0x0;
      p_render->pal_entry = NULL;
      p_render->pal_num = 0;

      p_out_pin->query_sample(p_out_pin, &p_out_sample);
      MT_ASSERT(p_out_sample != NULL);
      
      p_out_sample->p_data = (u8 *)p_priv->p_output;
      p_out_sample->format.stream_type = MT_IMAGE;
      p_out_sample->format.media_type = MF_BMP;

      p_out_pin->push_down(p_out_pin, p_out_sample);
      break;

    default:
      MT_ASSERT(0);
      break;
  }
}

static RET_CODE pic_transform(handle_t _this, media_sample_t *p_in)
{
  pic_filter_private_t *p_priv = get_priv(_this);
  ifilter_t *p_ifilter = (ifilter_t *)_this;
  drv_dev_t *p_pic_dev = NULL;
  pic_info_t pic_info = {0};
  pic_param_t pic_param = {0};
  //void *p_temp = NULL;
  //u32 temp_size = 0;
  os_msg_t msg = {0};
  RET_CODE ret = 0;
  net_samp_user_data_t *p_net_user_data = NULL;
  void *p_src_buf = NULL;

  p_priv->payload += p_in->payload;

  //for net source svc filter,  malloc fail
  if(p_in->state == SAMP_STATE_MEM_INSUFFICIENT)
  {
      pic_filter_free_all(p_priv);
      
      msg.content = PIC_IS_TOO_LARGE;
      msg.para1 = p_priv->handle;
      p_ifilter->send_message_out(p_ifilter, &msg);
      
      return ERR_FAILURE;
  }

  if((p_in->state != SAMP_STATE_INSUFFICIENT) && (p_priv->p_input != NULL))
  {
    //data insufficient, continue to request data.
    
    return SUCCESS;
  }

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);  

  //if do not cfg input buffer, we will use sample buffer.
  if((p_priv->p_input == NULL) || (p_priv->input_size == 0))
  {
    if(p_in->payload > 0)
    {
       p_src_buf = p_in->p_data;
    }
    else
    {
      pic_filter_free_all(p_priv);
      
      msg.content = PIC_DATA_ERROR;
      msg.para1 = p_priv->handle;
      p_ifilter->send_message_out(p_ifilter, &msg);
      
      return ERR_FAILURE;
    }
  }
  else
  {
    p_src_buf = p_priv->p_input;
  }

  if(p_in->format.stream_type == MT_NETWORK_REAL_DATA)
  {
    p_net_user_data = (net_samp_user_data_t *)p_in->p_user_data;
    fifo_db_add(p_net_user_data->p_url, p_in->p_data, p_in->payload);
  }

  ret = pdec_getinfo(p_pic_dev, p_src_buf,
    p_priv->payload, &pic_info, &p_priv->ins);
  if(ret != SUCCESS)
  {
    pdec_stop(p_pic_dev, &p_priv->ins);
    
    pic_filter_free_all(p_priv);
    
    msg.content = PIC_DATA_ERROR;
    msg.para1 = p_priv->handle;
    p_ifilter->send_message_out(p_ifilter, &msg);
    
    return ERR_FAILURE;
  }
  p_priv->is_pic_dec_getinfo = 1;
  
  pic_param.dec_mode = p_priv->dec_mode;

  p_priv->src_size.w = pic_info.src_width;
  p_priv->src_size.h = pic_info.src_height;

  msg.content = PIC_SIZE_UPDATE;
  msg.para1 = pic_info.src_width;
  msg.para2 = pic_info.src_height;

  p_ifilter->send_message_out(p_ifilter, &msg);  

  if(pic_info.image_format == IMAGE_FORMAT_UNKNOWN)
  {
      pdec_stop(p_pic_dev, &p_priv->ins);

      pic_filter_free_all(p_priv);
      
      msg.content = PIC_IS_UNSUPPORT;
      msg.para1 = p_priv->handle;
      p_ifilter->send_message_out(p_ifilter, &msg);  
      
      return ERR_FAILURE;  
  }

  
  switch (pic_info.image_format)
  {
    case IMAGE_FORMAT_JPEG:
      jpeg_transform(_this, &pic_param, &pic_info, p_src_buf);
      break;

    case IMAGE_FORMAT_GIF:
      gif_transform(_this, &pic_param, &pic_info, p_src_buf);
      break;

    case IMAGE_FORMAT_PNG:
      png_transform(_this, &pic_param, &pic_info, p_src_buf);
      break;

    case IMAGE_FORMAT_BMP:
      bmp_transform(_this, &pic_param, &pic_info, p_src_buf);
      break;

    default:
      pic_filter_free_all(p_priv);
      
      msg.content = PIC_DATA_ERROR;
      msg.para1 = p_priv->handle;
      p_ifilter->send_message_out(p_ifilter, &msg);
      
      return ERR_FAILURE;
  }

  //for internet pic, net source filter malloc, free buffer here
  if((p_priv->p_input != NULL) && (p_priv->input_size == 0))
  {
    p_priv->p_input = NULL;
  }

  return SUCCESS;
}

ifilter_t * pic_filter_create(void *p_para)
{
  pic_filter_private_t *p_priv = NULL;
  pic_filter_t *p_ins = NULL;
  transf_filter_t *p_transf_filter = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_para_t transf_filter_para;

  transf_filter_para.dummy = 0;

  //create filter
  p_ins = mtos_malloc(sizeof(pic_filter_t));
  MT_ASSERT(p_ins != NULL);
  memset(p_ins, 0, sizeof(pic_filter_t));
  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(pic_filter_private_t));

  p_priv->p_this = p_ins; //this point
  p_priv->p_output_sample = NULL;

  p_priv->p_input = NULL;
  p_priv->p_output = NULL;
  p_priv->flip = PIC_NO_F_NO_R;
  p_priv->out_fmt = PIC_OUTFMT_422;
  p_priv->use_bg_color = 1;

  //init member function

  //over loading the virtual function
  p_transf_filter = (transf_filter_t *)p_ins;
  p_transf_filter->transform = pic_transform;

  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = pic_on_command;
  p_ifilter->on_start = pic_on_start;
  p_ifilter->on_stop = pic_on_stop;
  p_ifilter->on_process_evt = pic_on_evt;

  p_ifilter->_interface.on_destroy = pic_on_destroy;

  //create it's pin
  pic_in_pin_create(&p_priv->m_in_pin, (interface_t *)p_ins);
  pic_out_pin_create(&p_priv->m_out_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}


