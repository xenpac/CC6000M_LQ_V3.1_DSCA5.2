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
#include "mtos_sem.h"

//hal
#include "hal_misc.h"

//drv
//#include "usb_drv.h"
#include "common.h"
#include "lib_memp.h"
#include "lib_rect.h"
#include "drv_dev.h"
#include "gpe_vsb.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"
#include "ifilter.h"
#include "eva_filter_factory.h"
#include "common_filter.h"
#include "pic_render_filter.h"
#include "pic_render_pin_intra.h"
#include "lib_memp.h"
#include "pdec.h"
#include "display.h"
#include "region.h"
#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_pic_render_filter_imp.h"

#include <stdlib.h>

/*!
  max scaler ratio
  */
#define MAX_SCALER_RATIO (6)

/*!
  msgq depth
  */
#define RENDER_TASK_MSGQ_DEPTH  10

/*!
  render task msgq time out
  */
#define RENDER_TASK_MSGQ_TIMEOUT 10

/*!
  render task command
  */
typedef enum
{
  /*!
    animation start
    */
  PIC_REND_ANIM_START = 0,
  /*!
    animation stop
    */
  PIC_REND_ANIM_STOP,
  /*!
    animation pause.
    */
  PIC_REND_ANIM_PAUSE,
  /*!
    animation resume.
    */
  PIC_REND_ANIM_RESUME,    
}pic_rend_cmd_t;

#define RAND(x)    ((u16)mtos_ticks_get()%(x)) 

static pic_render_pin_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((pic_render_pin_t *)_this)->private_data;
}

static void *lib_memp_dma_alloc(lib_memp_t *p_memp, u32 size)
{
  void *p_buf = NULL;
  hal_cache_info_t cache_info = {0};

  hal_cache_info_get(&cache_info);
  
  p_buf = lib_memp_align_alloc(p_memp, size, cache_info.dcache_line_size);
  if(NULL == p_buf)
  {
    return NULL;
  }

  hal_dcache_flush(p_buf, size);

  return (void *)hal_addr_nc((u32)p_buf);
}

static void lib_memp_dma_free(lib_memp_t *p_memp, void *p_piece)
{
  u32 buf_addr = (u32)p_piece;

  if((buf_addr & 0xA0000000) == 0xA0000000)
  {
    //for warrior/sonata/concerto
    buf_addr -= 0x20000000;
  }
  else if((buf_addr & 0x10000000) == 0x10000000)
  {
    //for magic
    buf_addr -= 0x10000000;
  }

  lib_memp_align_free(p_memp, (void *)buf_addr);

  return;
}

static void *pic_rend_malloc(pic_render_pin_private_t *p_priv, u32 size)
{
#ifndef WIN32
  if(p_priv->p_heap != NULL)
  {
    return lib_memp_dma_alloc(p_priv->p_heap, size);
  }
  else
#endif
  {
    return mtos_align_malloc(size, 8);
  }
}

static void pic_rend_free(pic_render_pin_private_t *p_priv, void *p_buffer)
{
#ifndef WIN32
  if(p_priv->p_heap != NULL)
  {
    lib_memp_dma_free(p_priv->p_heap, p_buffer);
  }
  else
 #endif
  {
    mtos_align_free(p_buffer);
  }
}

void pic_rend_pin_show(pic_render_pin_t *p_pin, BOOL bshow)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);
  MT_ASSERT(p_priv != NULL);
  
  p_priv->is_show = bshow;

}

void pic_rend_pin_clear(pic_render_pin_t *p_pin, u32 param, void *p_param)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);
  void *p_gpe = NULL, *p_disp = NULL;
  u32 color = param;
  rect_t *p_rect = (rect_t *)p_param;

  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  if(p_gpe == NULL)
  {
    return;
  }

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

  if(p_disp == NULL)
  {
    return;
  }
  
  if(p_priv == NULL)
  {
    return;
  }
  
  mtos_sem_take(&p_priv->rend_sem, 0);

  if((p_priv->p_rgn != NULL) && !is_invalid_rect(&p_priv->win_rect))
  {
    if((p_rect != NULL) && !is_invalid_rect(p_rect))
    {
      gpe_draw_rectangle_vsb(p_gpe, p_priv->p_rgn, p_rect, color);

      disp_layer_update_region(p_disp, p_priv->p_rgn, NULL);
    }
    else if((p_rect == NULL) && !is_invalid_rect(&p_priv->win_rect))
    {
      gpe_draw_rectangle_vsb(p_gpe, p_priv->p_rgn, &p_priv->win_rect, color);

      disp_layer_update_region(p_disp, p_priv->p_rgn, NULL);
    }
  }
  
  mtos_sem_give(&p_priv->rend_sem);
}

void pic_rend_pin_cfg(pic_render_pin_t *p_pin, pic_rend_cfg_t *p_cfg)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_cfg != NULL);

  memcpy((void *)&p_priv->win_rect, &p_cfg->win_rect, sizeof(rect_t));

  //MT_ASSERT(p_cfg->p_rgn != NULL);

  p_priv->anim = p_cfg->anim;

  if(p_priv->anim == REND_ANIM_RAND)
  {
    if(p_priv->chip_ic == IC_CONCERTO)
    {
      p_priv->anim = RAND(REND_ANIM_3D_ROTATE + 1);
    }
    else
    {
      p_priv->anim = RAND(REND_ANIM_HFEN + 1);
    }
  }

  p_priv->style = p_cfg->style;

  p_priv->frames = p_cfg->frames;

  p_priv->frame_rate = p_cfg->frame_rate;

  p_priv->p_rgn = p_cfg->p_rgn;

  p_priv->p_heap = p_cfg->p_heap;

  p_priv->handle = p_cfg->handle;
}

static void pic_rend_start_animation(handle_t _this)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  os_msg_t msg = {0};

  MT_ASSERT(p_priv != NULL);
  if(p_priv->msgq_id != 0xFFFFFFFF)
  {
    msg.content = PIC_REND_ANIM_START;

    OS_PRINTF("@@@@ animation start\n");

    mtos_messageq_send(p_priv->msgq_id, &msg);
  }
}

void pic_rend_stop_anim(pic_render_pin_t *p_pin)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);
  os_msg_t msg = {0};

  MT_ASSERT(p_priv != NULL);

  OS_PRINTF("@@@@ animation stop\n");

  if(p_priv->msgq_id != 0xFFFFFFFF)
  {
    msg.content = PIC_REND_ANIM_STOP;
    msg.is_sync = TRUE;

    mtos_messageq_send(p_priv->msgq_id, &msg);
  }
}

void pic_rend_pause_anim(pic_render_pin_t *p_pin)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);
  os_msg_t msg = {0};

  MT_ASSERT(p_priv != NULL);

  OS_PRINTF("@@@@ animation pause\n");

  if(p_priv->msgq_id != 0xFFFFFFFF)
  {
    msg.content = PIC_REND_ANIM_PAUSE;
    msg.is_sync = TRUE;

    mtos_messageq_send(p_priv->msgq_id, &msg);
  }
}

void pic_rend_resume_anim(pic_render_pin_t *p_pin)
{
  pic_render_pin_private_t *p_priv = get_priv(p_pin);
  os_msg_t msg = {0};

  MT_ASSERT(p_priv != NULL);

  OS_PRINTF("@@@@ animation resume\n");

  if(p_priv->msgq_id != 0xFFFFFFFF)
  {
    msg.content = PIC_REND_ANIM_RESUME;
    msg.is_sync = TRUE;

    mtos_messageq_send(p_priv->msgq_id, &msg);
  }
}

static void pic_rend_anim_release(pic_render_pin_private_t *p_priv)
{
  void *p_disp = NULL;
  
  if(p_priv->p_anim_rgn != NULL)
  {
    p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);

    region_delete(p_priv->p_anim_rgn);

    p_priv->p_anim_rgn = NULL;
  }  

  if(p_priv->p_anim_buffer != NULL)
  {
    pic_rend_free(p_priv, p_priv->p_anim_buffer);

    p_priv->p_anim_buffer = NULL;
  }

  if(p_priv->p_ref_rgn != NULL)
  {
    p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);

    region_delete(p_priv->p_ref_rgn);

    p_priv->p_ref_rgn = NULL;
  }  

  if(p_priv->p_ref_buffer != NULL)
  {
    pic_rend_free(p_priv, p_priv->p_ref_buffer);

    p_priv->p_ref_buffer = NULL;
  }

  if(p_priv->p_old_rgn != NULL)
  {
    p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);

    region_delete(p_priv->p_old_rgn);

    p_priv->p_old_rgn = NULL;
  }  

  if(p_priv->p_old_buffer != NULL)
  {
    pic_rend_free(p_priv, p_priv->p_old_buffer);

    p_priv->p_old_buffer = NULL;
  }

  if(p_priv->p_grad_rgn != NULL)
  {
    p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);

    region_delete(p_priv->p_grad_rgn);

    p_priv->p_grad_rgn = NULL;
  }  

  if(p_priv->p_grad_buffer != NULL)
  {
    pic_rend_free(p_priv, p_priv->p_grad_buffer);

    p_priv->p_grad_buffer = NULL;
  }

  p_priv->is_first_half = TRUE;
  p_priv->stop_offset[0] = 0;
  p_priv->stop_offset[1] = 0;
  p_priv->stop_offset[2] = 0;
  p_priv->stop_offset[3] = 0;  
}


static BOOL pic_rend_anim_init(pic_render_pin_private_t *p_priv, void *p_data)
{
  rect_size_t rect_size = {0};
  RET_CODE ret = SUCCESS;
  void *p_disp = NULL;
  u32 buf_size = 0;
  rect_t anim_rect = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  void *p_gpe = NULL;
  gpe_param_vsb_t gpe_param = {0};
  surface_info_t surface_info = {0};
  cct_gpe_blt2opt_t bopt2 = {0};
  
  OS_PRINTF("rend anim init %d, media type %d\n", p_priv->anim, p_priv->anim_param.media_type);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp != NULL);  
  
  MT_ASSERT(p_priv->p_anim_rgn == NULL);

  //create anim region.
  rect_size.w = RECTW(p_priv->anim_param.fill_rect);
  rect_size.h = RECTH(p_priv->anim_param.fill_rect);
  
  p_priv->p_anim_rgn = region_create(&rect_size, p_priv->anim_param.fmt);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  
  buf_size = rect_size.w * rect_size.h * 4 + 10240;

  p_priv->p_anim_buffer = pic_rend_malloc(p_priv, buf_size);
  if(p_priv->p_anim_buffer == NULL)
  {
    pic_rend_anim_release(p_priv);

    OS_PRINTF("@#anim rgn failed!\n");
    return FALSE;
  }

  memset(p_priv->p_anim_buffer, 0, buf_size);
  
  ((region_t *)(p_priv->p_anim_rgn))->p_buf_odd = p_priv->p_anim_buffer;

  //fill anim region.
  switch(p_priv->anim_param.media_type)
  {
    case MF_IMAGE:
    case MF_JPEG:
    case MF_BMP:
    case MF_PNG:
      anim_rect.left = 0;
      anim_rect.top = 0;
      anim_rect.right = RECTW(p_priv->anim_param.fill_rect);
      anim_rect.bottom = RECTH(p_priv->anim_param.fill_rect);

      p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
      MT_ASSERT(p_gpe != NULL);

      gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
      gpe_param.colorkey = p_priv->anim_param.color_key;      
        
      gpe_draw_image_vsb(p_gpe, p_priv->p_anim_rgn,
        &anim_rect, p_data, p_priv->anim_param.pal_entry,
        p_priv->anim_param.pal_num, p_priv->anim_param.pitch,
        p_priv->anim_param.pitch * p_priv->anim_param.image_height, p_priv->anim_param.fmt,
        &gpe_param, &p_priv->anim_param.fill_rect);
      break;

    case MF_COLOR:
      anim_rect.left = 0;
      anim_rect.top = 0;
      anim_rect.right = RECTW(p_priv->anim_param.fill_rect);
      anim_rect.bottom = RECTH(p_priv->anim_param.fill_rect);

      gpe_draw_rectangle_vsb(p_gpe, p_priv->p_anim_rgn, &anim_rect, *((u32 *)p_data));    
      break;

    default:
      MT_ASSERT(0);
      break;
  }  

  //create & blt referance region, according to animation type, prepare for animation.
  switch(p_priv->anim)
  {
    case REND_ANIM_FADE:
      //save new region.
      rect_size.w = RECTW(p_priv->anim_param.fill_rect);
      rect_size.h = RECTH(p_priv->anim_param.fill_rect);
      
      p_priv->p_ref_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;

      p_priv->p_ref_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_ref_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);

        OS_PRINTF("@#ref rgn failed!\n");
        return FALSE;
      }

      memset(p_priv->p_ref_buffer, 0, buf_size);

      ((region_t *)(p_priv->p_ref_rgn))->p_buf_odd = p_priv->p_ref_buffer;

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = RECTW(p_priv->anim_param.fill_rect);
      src_vsb.h = RECTH(p_priv->anim_param.fill_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->anim_param.fill_rect);
      drc_vsb.h = RECTH(p_priv->anim_param.fill_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;
      ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_ref_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);   

      //save old region.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);
      
      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;

      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);

        OS_PRINTF("@#old rgn failed!\n");
        return FALSE;
      }

      memset(p_priv->p_old_buffer, 0, buf_size);

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;

      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;
      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);   
      break;

    case REND_ANIM_BOX_IN:
    case REND_ANIM_BOX_OUT:
    case REND_ANIM_BOX_INOUT:
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);
      
      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;

      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);

        OS_PRINTF("@#anim rgn failed!\n");
        return FALSE;
      }

      memset(p_priv->p_old_buffer, 0, buf_size);

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;

      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;
      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);

      p_priv->is_first_half = TRUE;
      break;

    case REND_ANIM_SMOOTH_MOVE:
      OS_PRINTF("smooth init.\n");

      //create ref rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_ref_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;

      p_priv->p_ref_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_ref_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);

        OS_PRINTF("@#ref rgn failed!\n");
        return FALSE;
      }

      memset(p_priv->p_ref_buffer, 0, buf_size);

      ((region_t *)(p_priv->p_ref_rgn))->p_buf_odd = p_priv->p_ref_buffer;
      
      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = RECTW(p_priv->anim_param.fill_rect);
      src_vsb.h = RECTH(p_priv->anim_param.fill_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->anim_param.fill_rect);
      drc_vsb.h = RECTH(p_priv->anim_param.fill_rect);

      drc_vsb.x += 
        ((RECTW(p_priv->anim_param.rend_rect) - RECTW(p_priv->anim_param.fill_rect)) / 2);

      drc_vsb.y += 
        ((RECTH(p_priv->anim_param.rend_rect) - RECTH(p_priv->anim_param.fill_rect)) / 2);

      drc_vsb.x += p_priv->anim_param.rend_rect.left;
      drc_vsb.y += p_priv->anim_param.rend_rect.top;
      
      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;
      ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_ref_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);           

      //create old rgn & blt current rgn to old rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;

      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);

        OS_PRINTF("@#ref rgn failed!\n");
        return FALSE;
      }

      memset(p_priv->p_old_buffer, 0, buf_size);

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;

      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);
    
      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;
      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);        
      break;

    case REND_ANIM_UNCOVER_RIGHT:
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);
      
      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;

      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);

        OS_PRINTF("@#anim rgn failed!\n");
        return FALSE;
      }

      memset(p_priv->p_old_buffer, 0, buf_size);

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;

      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      OS_PRINTF("uncover fmt %d, x %d, y %d, w %d, h %d\n", 
        p_priv->anim_param.fmt, src_vsb.x, src_vsb.y, src_vsb.w, src_vsb.h);
      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);       
      break;

    case REND_ANIM_BLINDS:
      //ref rgn for gradiant alpha map.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_ref_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_ref_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_ref_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_ref_rgn))->p_buf_odd = p_priv->p_ref_buffer;

      //save old rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;

      //blt old region.
      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 0;
      p_priv->is_first_half = TRUE;
      break;

    case REND_ANIM_SQUARE_BOARD:
    case REND_ANIM_DIAMOND_BOARD:
      //ref rgn for gradiant alpha map.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_ref_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_ref_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_ref_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_ref_rgn))->p_buf_odd = p_priv->p_ref_buffer;

      //gradient rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      rect_size.w = (rect_size.w + p_priv->frames - 1) / p_priv->frames;
      rect_size.h = (rect_size.h + p_priv->frames - 1) / p_priv->frames;

      p_priv->p_grad_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_grad_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_grad_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_grad_rgn))->p_buf_odd = p_priv->p_grad_buffer;

      //save old rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;     

      //blt old region.
      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 4095;
      p_priv->is_first_half = TRUE;      
      break;

    case REND_ANIM_ROUND_BOARD:
      //ref rgn for gradiant alpha map.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_ref_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_ref_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_ref_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_ref_rgn))->p_buf_odd = p_priv->p_ref_buffer;

      //gradient rgn.
      rect_size.w = (rect_size.w + p_priv->frames - 1) / p_priv->frames * 2;
      rect_size.h = rect_size.w;

      p_priv->p_grad_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_grad_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_grad_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_grad_rgn))->p_buf_odd = p_priv->p_grad_buffer;

      //save old rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;     

      //blt old region.
      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 4095;
      p_priv->stop_offset[2] = 4095;
      p_priv->stop_offset[3] = 4095;
      p_priv->is_first_half = TRUE;      
      break;      

    case REND_ANIM_FADE_DIAMOND:
      //ref rgn for gradiant alpha map.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_ref_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_ref_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_ref_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_ref_rgn))->p_buf_odd = p_priv->p_ref_buffer;

      //gradient rgn.
      rect_size.w = RECTW(p_priv->win_rect) / 2;
      rect_size.h = RECTH(p_priv->win_rect) / 2;

      p_priv->p_grad_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_grad_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_grad_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_grad_rgn))->p_buf_odd = p_priv->p_grad_buffer;

      //save old rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;     

      //blt old region.
      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 4095;
      p_priv->is_first_half = TRUE;      
      break;

    case REND_ANIM_3D_ROTATE:
      //ref rgn for gradiant alpha map.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_ref_rgn = region_create(&rect_size, PIX_FMT_ARGB8888);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_ref_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_ref_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_ref_rgn))->p_buf_odd = p_priv->p_ref_buffer;

      //save old rgn.
      rect_size.w = RECTW(p_priv->win_rect);
      rect_size.h = RECTH(p_priv->win_rect);

      p_priv->p_old_rgn = region_create(&rect_size, p_priv->anim_param.fmt);

      buf_size = rect_size.w * rect_size.h * 4 + 10240;
      
      p_priv->p_old_buffer = pic_rend_malloc(p_priv, buf_size);
      if(p_priv->p_old_buffer == NULL)
      {
        pic_rend_anim_release(p_priv);
        
        return FALSE;
      }

      ((region_t *)(p_priv->p_old_rgn))->p_buf_odd = p_priv->p_old_buffer;

      //blt old region.
      src_vsb.x = p_priv->win_rect.left;
      src_vsb.y = p_priv->win_rect.top;
      src_vsb.w = RECTW(p_priv->win_rect);
      src_vsb.h = RECTH(p_priv->win_rect);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->anim_param.p_rgn, p_priv->p_old_rgn, &src_vsb, &drc_vsb,
        &surface_info, &surface_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);
      break;
      
    default:
      break;
  }

  OS_PRINTF("rend anim init done.\n");
          
  return TRUE;
}

static void _check_rect(rect_t *p_max, rect_t *p_curn)
{
  MT_ASSERT(p_max != NULL);
  MT_ASSERT(p_curn != NULL);

  if(p_curn->left < p_max->left)
  {
    p_curn->left = p_max->left;
  }

  if(p_curn->top < p_max->top)
  {
    p_curn->top = p_max->top;
  }

  if(p_curn->right > p_max->right)
  {
    p_curn->right = p_max->right;
  }

  if(p_curn->bottom > p_max->bottom)
  {
    p_curn->bottom = p_max->bottom;
  }  
}

//for chip concert.
static void _l2r_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_x = 0, step_y = 0;
  void *p_disp = NULL, *p_gpe = NULL;
  gpe_param_vsb_t gpe_param = {0};
  rect_t brc = {0}, src = {0}, drc = {0};
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_priv != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_x = ((rend_w + fill_w) / 2 + p_priv->frames - 1) / p_priv->frames;
  step_y = ((rend_h + fill_h) / 2 + p_priv->frames - 1) / p_priv->frames;

  src.top = p_priv->anim_param.fill_rect.top;
  src.bottom = p_priv->anim_param.fill_rect.bottom;
  
  drc.top = p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2;
  drc.bottom = drc.top + fill_h;

  brc.top = drc.top;
  brc.bottom = drc.bottom;

  if(step_x * (p_priv->anim_param.curn_frame) <= fill_w)
  {
    src.left = p_priv->anim_param.fill_rect.right - step_x * (p_priv->anim_param.curn_frame);
    src.right = p_priv->anim_param.fill_rect.right;

    drc.left = p_priv->anim_param.rend_rect.left;
    drc.right = p_priv->anim_param.rend_rect.left + step_x * (p_priv->anim_param.curn_frame);
  }
  else
  {
    src.left = p_priv->anim_param.fill_rect.left;
    src.right = p_priv->anim_param.fill_rect.right;

    drc.right = p_priv->anim_param.rend_rect.left + step_x * (p_priv->anim_param.curn_frame);
    if(drc.right < rend_w)
    {
      drc.left = drc.right - fill_w;

      if(drc.left > ((rend_w - fill_w) / 2))
      {
        drc.left = (rend_w - fill_w) / 2;    
        drc.right = drc.left + fill_w;        
      }
    }
    else
    {
      drc.left = (rend_w - fill_w) / 2;    
      drc.right = drc.left + fill_w;
    }
  }

  _check_rect(&p_priv->anim_param.fill_rect, &src);
  _check_rect(&p_priv->anim_param.rend_rect, &drc);

  brc.left = p_priv->anim_param.rend_rect.left;
  brc.right = drc.left;

  _check_rect(&p_priv->anim_param.rend_rect, &brc);
  
  offset_rect(&brc, p_priv->win_rect.left, p_priv->win_rect.top);
  offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp != NULL);

  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(p_gpe != NULL);

  if(p_priv->anim_param.use_bg_color)
  {
    if(!is_invalid_rect(&brc))
    {
      ret = gpe_draw_rectangle_vsb(p_gpe,
        p_priv->anim_param.p_rgn, &brc, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);
    }
  }

  gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
  gpe_param.colorkey = p_priv->anim_param.color_key;
  gpe_param.cmd = GPE_CMD_RASTER_OP;
  gpe_param.rop3 = SRCCOPY;

  if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
  {
    ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
      NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
    MT_ASSERT(ret == SUCCESS);
  }  
}

static void _r2l_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_x = 0, step_y = 0;
  void *p_disp = NULL, *p_gpe = NULL;
  gpe_param_vsb_t gpe_param = {0};
  rect_t brc = {0}, src = {0}, drc = {0};
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_priv != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_x = ((rend_w + fill_w) / 2 + p_priv->frames - 1) / p_priv->frames;
  step_y = ((rend_h + fill_h) / 2 + p_priv->frames - 1) / p_priv->frames;

  src.top = p_priv->anim_param.fill_rect.top;
  src.bottom = p_priv->anim_param.fill_rect.bottom;
  drc.top = p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2;
  drc.bottom = drc.top + fill_h;
  brc.top = drc.top;
  brc.bottom = drc.bottom;

  if(step_x * (p_priv->anim_param.curn_frame) <= fill_w)
  {
    src.left = p_priv->anim_param.fill_rect.left;
    src.right = p_priv->anim_param.fill_rect.left
      + step_x * (p_priv->anim_param.curn_frame);

    drc.left = p_priv->anim_param.rend_rect.right
      - step_x * (p_priv->anim_param.curn_frame);
    drc.right = p_priv->anim_param.rend_rect.right;
  }
  else
  {
    src.left = p_priv->anim_param.fill_rect.left;
    src.right = p_priv->anim_param.fill_rect.right;

    if(step_x * p_priv->anim_param.curn_frame > (fill_w + rend_w) / 2)
    {
      drc.left = (rend_w - fill_w) / 2 + p_priv->anim_param.rend_rect.left;    
      drc.right = drc.left + fill_w;   
    }    
    else
    {
      drc.left = p_priv->anim_param.rend_rect.right
                - step_x * (p_priv->anim_param.curn_frame);
      drc.right = drc.left + fill_w;
    }
  }

  _check_rect(&p_priv->anim_param.fill_rect, &src);
  _check_rect(&p_priv->anim_param.rend_rect, &drc);

  brc.left = drc.right;
  brc.right = p_priv->anim_param.rend_rect.right;

  _check_rect(&p_priv->anim_param.rend_rect, &brc);

  offset_rect(&brc, p_priv->win_rect.left, p_priv->win_rect.top);
  offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp != NULL);

  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(p_gpe != NULL);

  if(p_priv->anim_param.use_bg_color)
  {
    if(!is_invalid_rect(&brc))
    {
      ret = gpe_draw_rectangle_vsb(p_gpe,
        p_priv->anim_param.p_rgn, &brc, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);
    }
  }

  gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
  gpe_param.colorkey = p_priv->anim_param.color_key;
  gpe_param.cmd = GPE_CMD_RASTER_OP;
  gpe_param.rop3 = SRCCOPY;

  if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
  {
    ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
      NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
    MT_ASSERT(ret == SUCCESS);
  }
}

static void _t2b_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_x = 0, step_y = 0;
  void *p_disp = NULL, *p_gpe = NULL;
  gpe_param_vsb_t gpe_param = {0};
  rect_t brc = {0}, src = {0}, drc = {0};
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_priv != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_x = ((rend_w + fill_w) / 2 + p_priv->frames - 1) / p_priv->frames;
  step_y = ((rend_h + fill_h) / 2 + p_priv->frames - 1) / p_priv->frames;

  src.left = p_priv->anim_param.fill_rect.left;
  src.right = p_priv->anim_param.fill_rect.right;
  drc.left = p_priv->anim_param.rend_rect.left + (rend_w - fill_w) / 2;
  drc.right = drc.left + fill_w;
  brc.left = drc.left;
  brc.right = drc.right;

  if(step_y * (p_priv->anim_param.curn_frame) <= fill_h)
  {
    src.top = p_priv->anim_param.fill_rect.bottom
      - step_y * (p_priv->anim_param.curn_frame);
    src.bottom = p_priv->anim_param.fill_rect.bottom;

    drc.top = p_priv->anim_param.rend_rect.top;
    drc.bottom = p_priv->anim_param.rend_rect.top
      + step_y * (p_priv->anim_param.curn_frame);
  }
  else
  {
    src.top = p_priv->anim_param.fill_rect.top;
    src.bottom = p_priv->anim_param.fill_rect.bottom;

    drc.bottom = p_priv->anim_param.rend_rect.top
      + step_y * (p_priv->anim_param.curn_frame);
    if(drc.bottom < rend_h)
    {
      drc.top = drc.bottom - fill_h; 
      if(drc.top > ((rend_h - fill_h) / 2))
      {
        drc.top = (rend_h - fill_h) / 2;
        drc.bottom = drc.top + fill_h;       
      }
    }
    else
    {
      drc.top = (rend_h - fill_h) / 2;
      drc.bottom = drc.top + fill_h;
    }
  }

  _check_rect(&p_priv->anim_param.fill_rect, &src);
  _check_rect(&p_priv->anim_param.rend_rect, &drc);

  brc.top = p_priv->anim_param.rend_rect.top;
  brc.bottom = drc.top;

  _check_rect(&p_priv->anim_param.rend_rect, &brc);
  
  offset_rect(&brc, p_priv->win_rect.left, p_priv->win_rect.top);
  offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp != NULL);

  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(p_gpe != NULL);

  if(p_priv->anim_param.use_bg_color)
  {
    if(!is_invalid_rect(&brc))
    {
      ret = gpe_draw_rectangle_vsb(p_gpe,
        p_priv->anim_param.p_rgn, &brc, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);
    }
  }

  gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
  gpe_param.colorkey = p_priv->anim_param.color_key;
  gpe_param.cmd = GPE_CMD_RASTER_OP;
  gpe_param.rop3 = SRCCOPY;

  if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
  {
    ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
      NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
    MT_ASSERT(ret == SUCCESS);
  }
}

static void _b2t_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_x = 0, step_y = 0;
  void *p_disp = NULL, *p_gpe = NULL;
  gpe_param_vsb_t gpe_param = {0};
  rect_t brc = {0}, src = {0}, drc = {0};
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_priv != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_x = ((rend_w + fill_w) / 2 + p_priv->frames - 1) / p_priv->frames;
  step_y = ((rend_h + fill_h) / 2 + p_priv->frames - 1) / p_priv->frames;

  src.left = p_priv->anim_param.fill_rect.left;
  src.right = p_priv->anim_param.fill_rect.right;
  drc.left = p_priv->anim_param.rend_rect.left + (rend_w - fill_w) / 2;
  drc.right = drc.left + fill_w;
  brc.left = drc.left;
  brc.right = drc.right;

  if(step_y * (p_priv->anim_param.curn_frame) <= fill_h)
  {
    src.top = p_priv->anim_param.fill_rect.top;
    src.bottom = p_priv->anim_param.fill_rect.top
      + step_y * (p_priv->anim_param.curn_frame);

    drc.top = p_priv->anim_param.rend_rect.bottom
      - step_y * (p_priv->anim_param.curn_frame);
    drc.bottom = p_priv->anim_param.rend_rect.bottom;
  }
  else
  {
    src.top = p_priv->anim_param.fill_rect.top;
    src.bottom = p_priv->anim_param.fill_rect.bottom;

    if(p_priv->anim_param.rend_rect.bottom > (step_y * p_priv->anim_param.curn_frame))
    {
      drc.top = p_priv->anim_param.rend_rect.bottom
        - step_y * (p_priv->anim_param.curn_frame);
    }
    else
    {
     drc.top = (rend_h - fill_h) / 2;
    }
    drc.bottom = drc.top + fill_h;
  }

  _check_rect(&p_priv->anim_param.fill_rect, &src);
  _check_rect(&p_priv->anim_param.rend_rect, &drc);

  brc.top = drc.bottom;
  brc.bottom = p_priv->anim_param.rend_rect.bottom;

  _check_rect(&p_priv->anim_param.rend_rect, &brc);

  offset_rect(&brc, p_priv->win_rect.left, p_priv->win_rect.top);
  offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp != NULL);

  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(p_gpe != NULL);

  if(p_priv->anim_param.use_bg_color)
  {
    if(!is_invalid_rect(&brc))
    {
      gpe_draw_rectangle_vsb(p_gpe,
        p_priv->anim_param.p_rgn, &brc, p_priv->anim_param.bg_color);
    }
  }

  gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
  gpe_param.colorkey = p_priv->anim_param.color_key;
  gpe_param.cmd = GPE_CMD_RASTER_OP;
  gpe_param.rop3 = SRCCOPY;

  if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
  {
    ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
      NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
    MT_ASSERT(ret == SUCCESS);
  }
}

static void _shth_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 copyy = 0;
  u32 i = 0;
  void *p_gpe = NULL;
  void *p_disp = NULL;
  rect_t src = {0}, drc = {0};
  gpe_param_vsb_t gpe_param = {0};
  u32 in_size = 0;
  RET_CODE ret = SUCCESS;

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_disp != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  copyy = (fill_h + p_priv->frames - 1) / p_priv->frames;

  in_size = p_priv->anim_param.image_height * p_priv->anim_param.pitch;

  for(i = 0; i < copyy; i++)
  {
    src.left = p_priv->anim_param.fill_rect.left;
    src.right = src.left + fill_w;
    src.top = p_priv->anim_param.fill_rect.top + i * p_priv->frames
      + p_priv->anim_param.curn_frame - 2;
    src.bottom = src.top + 1;

    drc.left = p_priv->anim_param.rend_rect.left + (rend_w - fill_w) / 2;
    drc.right = drc.left + fill_w;
    drc.top = p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2 +
      i * p_priv->frames + p_priv->anim_param.curn_frame - 2;
    drc.bottom = drc.top + 1;

    _check_rect(&p_priv->anim_param.fill_rect, &src);
    _check_rect(&p_priv->anim_param.rend_rect, &drc);
    
    offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);

    gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
    gpe_param.colorkey = p_priv->anim_param.color_key;
    gpe_param.cmd = GPE_CMD_RASTER_OP;
    gpe_param.rop3 = SRCCOPY;

    if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
    {
      ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
        NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
      MT_ASSERT(ret == SUCCESS);
    }
  }
}

static void _shtv_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 copyx = 0;
  u32 i = 0;
  void *p_gpe = NULL;
  void *p_disp = NULL;
  rect_t src = {0}, drc = {0};
  gpe_param_vsb_t gpe_param = {0};
  u32 in_size = 0;
  RET_CODE ret = SUCCESS;

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp != NULL);
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(p_gpe != NULL);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  copyx = (fill_w + p_priv->frames - 1) / p_priv->frames;

  in_size = p_priv->anim_param.image_height * p_priv->anim_param.pitch;

  for(i = 0; i < copyx; i++)
  {
    src.left = p_priv->anim_param.fill_rect.left + i * p_priv->frames
      + p_priv->anim_param.curn_frame - 2;
    src.right = src.left + 1;
    src.top = p_priv->anim_param.fill_rect.top;
    src.bottom = src.top + fill_h;

    drc.left = p_priv->anim_param.rend_rect.left + (rend_w - fill_w) / 2 +
      i * p_priv->frames + p_priv->anim_param.curn_frame - 2;
    drc.right = drc.left + 1;
    drc.top = p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2;
    drc.bottom = drc.top + fill_h;

    _check_rect(&p_priv->anim_param.fill_rect, &src);
    _check_rect(&p_priv->anim_param.rend_rect, &drc);
    
    offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);

    gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
    gpe_param.colorkey = p_priv->anim_param.color_key;
    gpe_param.cmd = GPE_CMD_RASTER_OP;
    gpe_param.rop3 = SRCCOPY;

    if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
    {
      ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
        NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
      MT_ASSERT(ret == SUCCESS);
    }
  }
}

static void _mosaic_init(pic_render_pin_private_t *p_priv)
{
  u32 total = 0, i = 0, n = 0, r = 0;
  u32 *p_temp = NULL;

  total = p_priv->frames * p_priv->frames;

  p_temp = (u32 *)mtos_malloc(4 * total);
  MT_ASSERT(p_temp != NULL);

  p_priv->anim_param.p_array = (u32 *)mtos_malloc(4 * total);
  MT_ASSERT(p_priv->anim_param.p_array != NULL);

  //init temp array
  for(i = 0; i < total; i++)
  {
    *(p_temp + i) = i;
  }

  //init rand array
  r = total;
  for (i = 0; i < total; i++)
  {
    //n = (mtos_ticks_get() % r);
    n = (rand() % r);

    *(p_priv->anim_param.p_array + i) = *(p_temp + n);
    *(p_temp + n) = *(p_temp + r - 1);
    r--;
  }

  mtos_free(p_temp);
}

static void _mosaic_deinit(pic_render_pin_private_t *p_priv)
{
  if(p_priv->anim_param.p_array != NULL)
  {
    mtos_free(p_priv->anim_param.p_array);

    p_priv->anim_param.p_array = NULL;
  }
}

static void _mosaic_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 copyx = 0, copyy = 0;
  u32 i = 0, in_size = 0;
  void *p_gpe = NULL;
  void *p_disp = NULL;
  rect_t src = {0}, drc = {0};
  gpe_param_vsb_t gpe_param = {0};
  u32 index = 0, x_index = 0, y_index = 0;
  RET_CODE ret = SUCCESS;

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_disp != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  copyx = (fill_w + p_priv->frames - 1) / p_priv->frames;
  copyy = (fill_h + p_priv->frames - 1) / p_priv->frames;

  in_size = p_priv->anim_param.image_height * p_priv->anim_param.pitch;

  for(i = 0; i < p_priv->frames; i++)
  {

    index = *(p_priv->anim_param.p_array
      + (p_priv->anim_param.curn_frame - 1) * p_priv->frames + i);

    x_index = index % p_priv->frames;
    y_index = index / p_priv->frames;

    src.left = p_priv->anim_param.fill_rect.left + x_index * copyx;
    src.right = src.left + copyx;
    src.top = p_priv->anim_param.fill_rect.top + y_index * copyy;
    src.bottom = src.top + copyy;

    drc.left = p_priv->anim_param.rend_rect.left + (rend_w - fill_w) / 2 + x_index * copyx;
    drc.right = drc.left + copyx;
    drc.top = p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2 + y_index * copyy;
    drc.bottom = drc.top + copyy;

    _check_rect(&p_priv->anim_param.fill_rect, &src);
    _check_rect(&p_priv->anim_param.rend_rect, &drc);

    drc.right = drc.left + RECTW(src);
    drc.bottom = drc.top + RECTH(src);
      
    offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);

    gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
    gpe_param.colorkey = p_priv->anim_param.color_key;
    gpe_param.cmd = GPE_CMD_RASTER_OP;
    gpe_param.rop3 = SRCCOPY;
    
    if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
    {
      ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
        NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
      MT_ASSERT(ret == SUCCESS);
    }
  }
}

static void _hfen_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 copyx = 0, copyy = 0, stepx = 0;
  u32 i = 0;
  void *p_gpe = NULL;
  void *p_disp = NULL;
  rect_t src = {0}, drc = {0}, b_rc = {0};
  gpe_param_vsb_t gpe_param = {0};
  u32 in_size = 0;
  RET_CODE ret = SUCCESS;

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_disp != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  copyx = (fill_w + p_priv->frames - 1) / p_priv->frames;
  copyy = (fill_h + p_priv->frames - 1) / p_priv->frames;
  stepx = ((3 * fill_w + rend_w) / 2 + p_priv->frames - 1) / p_priv->frames;

  in_size = p_priv->anim_param.image_height * p_priv->anim_param.pitch;

  for(i = 0; i < p_priv->frames; i++)
  {
    src.top = p_priv->anim_param.fill_rect.top + i * copyy;
    src.bottom = src.top + copyy;
    src.right = p_priv->anim_param.fill_rect.right;
    
    if(stepx * p_priv->anim_param.curn_frame <= (fill_w + i * copyx))
    {
      src.left = p_priv->anim_param.fill_rect.right - 
        stepx * p_priv->anim_param.curn_frame - i * copyx;
    }
    else
    {
      src.left = p_priv->anim_param.fill_rect.left;
    }
    
    drc.top = p_priv->anim_param.rend_rect.top +
      (rend_h - fill_h) / 2 + i * copyy;
    drc.bottom = drc.top + copyy;

    b_rc.top = drc.top;
    b_rc.bottom = drc.bottom;
    
    if(stepx * p_priv->anim_param.curn_frame <= (fill_w + i * copyx))
    {    
      drc.left = p_priv->anim_param.rend_rect.left;
      drc.right = drc.left + stepx * p_priv->anim_param.curn_frame - i * copyx;

      b_rc.left = p_priv->anim_param.rend_rect.left;
      b_rc.right = p_priv->anim_param.rend_rect.left;
    }
    else if(stepx * p_priv->anim_param.curn_frame <= (((fill_w + rend_w) / 2) + i * copyx))
    {
      drc.left = p_priv->anim_param.rend_rect.left +
        stepx * p_priv->anim_param.curn_frame - i * copyx - fill_w;
      drc.right = drc.left + fill_w;

      b_rc.left = p_priv->anim_param.rend_rect.left;
      b_rc.right = drc.right;      
    }
    else
    {
      drc.left = p_priv->anim_param.rend_rect.left + (rend_w - fill_w) / 2;
      drc.right = drc.left + fill_w;

      b_rc.left = p_priv->anim_param.rend_rect.left;
      b_rc.right = drc.left;
    }
    
    _check_rect(&p_priv->anim_param.fill_rect, &src);
    _check_rect(&p_priv->anim_param.rend_rect, &drc);
    _check_rect(&p_priv->anim_param.rend_rect, &b_rc);

    drc.right = drc.left + RECTW(src);
    drc.bottom = drc.top + RECTH(src);    
    
    offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);
    offset_rect(&b_rc, p_priv->win_rect.left, p_priv->win_rect.top);

    if(p_priv->anim_param.use_bg_color)
    {
      if(!is_invalid_rect(&b_rc))
      {
        ret = gpe_draw_rectangle_vsb(p_gpe,
          p_priv->anim_param.p_rgn, &b_rc, p_priv->anim_param.bg_color);
        MT_ASSERT(ret == SUCCESS);
      }
    }
    
    gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
    gpe_param.colorkey = p_priv->anim_param.color_key;
    gpe_param.cmd = GPE_CMD_RASTER_OP;
    gpe_param.rop3 = SRCCOPY;

    if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
    {
      ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
        NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
      MT_ASSERT(ret == SUCCESS);
    }
  }
}

static void _vfen_step(pic_render_pin_private_t *p_priv)
{
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 copyx = 0, copyy = 0, stepy = 0;
  u32 i = 0;
  u32 temp_h = 0, temp_y = 0;
  void *p_gpe = NULL;
  void *p_disp = NULL;
  rect_t src = {0}, drc = {0}, b_rc = {0};
  gpe_param_vsb_t gpe_param = {0};
  u32 in_size = 0;
  RET_CODE ret = SUCCESS;

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_disp != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  copyx = (fill_w + p_priv->frames - 1) / p_priv->frames;
  copyy = (fill_h + p_priv->frames - 1) / p_priv->frames;
  stepy = ((3 * fill_h + rend_h) / 2 + p_priv->frames - 1) / p_priv->frames;

  in_size = p_priv->anim_param.image_height * p_priv->anim_param.pitch;

  for(i = 0; i < p_priv->frames; i++)
  {
    temp_y = stepy * p_priv->anim_param.curn_frame;
    if(temp_y > ((3 * fill_h + rend_h) / 2))
    {
      temp_y = (3 * fill_h + rend_h) / 2;
    }

    temp_h = i * copyy;
    if(temp_h > fill_h)
    {
      temp_h = fill_h;
    }
  
    src.left = p_priv->anim_param.fill_rect.left + i * copyx;
    src.right = src.left + copyx;
    src.bottom = p_priv->anim_param.fill_rect.bottom;
    
    if(temp_y <= (fill_h + temp_h))
    {
      src.top = p_priv->anim_param.fill_rect.bottom - 
        temp_y - temp_h;
    }
    else
    {
      src.top = p_priv->anim_param.fill_rect.top;
    }
    
    drc.left = p_priv->anim_param.rend_rect.left +
      (rend_w - fill_w) / 2 + i * copyx;
    drc.right = drc.left + copyx;

    b_rc.left = drc.left;
    b_rc.right = drc.right;
    
    if(temp_y <= (fill_h + temp_h))
    {    
      drc.top = p_priv->anim_param.rend_rect.top;
      drc.bottom = drc.top + temp_y - temp_h;

      b_rc.top = p_priv->anim_param.rend_rect.top;
      b_rc.bottom = p_priv->anim_param.rend_rect.top;
    }
    else if(temp_y <= (((fill_h + rend_h) / 2) + temp_h))
    {
      drc.top = p_priv->anim_param.rend_rect.top +
        temp_y - temp_h - fill_h;
      drc.bottom = drc.top + fill_h;

      b_rc.top = p_priv->anim_param.rend_rect.top;
      b_rc.bottom = drc.top;      
    }
    else
    {
      drc.top = p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2;
      drc.bottom = drc.top + fill_h;

      b_rc.top = p_priv->anim_param.rend_rect.top;
      b_rc.bottom = drc.top;
    }
    
    _check_rect(&p_priv->anim_param.fill_rect, &src);
    _check_rect(&p_priv->anim_param.rend_rect, &drc);
    _check_rect(&p_priv->anim_param.rend_rect, &b_rc);

    drc.right = drc.left + RECTW(src);
    drc.bottom = drc.top + RECTH(src);    
    
    offset_rect(&drc, p_priv->win_rect.left, p_priv->win_rect.top);
    offset_rect(&b_rc, p_priv->win_rect.left, p_priv->win_rect.top);

    if(p_priv->anim_param.use_bg_color)
    {
      if(!is_invalid_rect(&b_rc))
      {
        ret = gpe_draw_rectangle_vsb(p_gpe,
          p_priv->anim_param.p_rgn, &b_rc, p_priv->anim_param.bg_color);
        MT_ASSERT(ret == SUCCESS);
      }
    }
    
    gpe_param.enable_colorkey = p_priv->anim_param.enable_ckey;
    gpe_param.colorkey = p_priv->anim_param.color_key;
    gpe_param.cmd = GPE_CMD_RASTER_OP;
    gpe_param.rop3 = SRCCOPY;

    //OS_PRINTF("@# src %d %d %d %d\n", src.left, src.right, src.top, src.bottom);
    //OS_PRINTF("@# drc %d %d %d %d\n", drc.left, drc.right, drc.top, drc.bottom);

    if((!is_invalid_rect(&drc)) && (!is_invalid_rect(&src)))
    {
      ret = gpe_bitblt_vsb(p_gpe, p_priv->anim_param.p_rgn, &drc,
        NULL, NULL, p_priv->p_anim_rgn, &src, &gpe_param);
      MT_ASSERT(ret == SUCCESS);
    }
  }
}

static void _fade_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  cct_gpe_blt2opt_t bopt2 = {0};
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u8 alpha = 0;
  RET_CODE ret = SUCCESS;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  //caculate alpha according to current frame.
  if(p_priv->anim_param.curn_frame == p_priv->frames)
  {
    alpha = 0;
  }
  else
  {
    alpha = 255 - 255 / p_priv->frames * p_priv->anim_param.curn_frame;
  }

  memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_PATCOPY;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = alpha << 24;

  src_vsb.x = p_priv->win_rect.left;
  src_vsb.y = p_priv->win_rect.top;
  src_vsb.w = RECTW(p_priv->win_rect);
  src_vsb.h = RECTH(p_priv->win_rect);
  
  drc_vsb.x = p_priv->win_rect.left;
  drc_vsb.y = p_priv->win_rect.top;
  drc_vsb.w = RECTW(p_priv->win_rect);
  drc_vsb.h = RECTH(p_priv->win_rect);

  ret = gpe_bitblt2src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn,
    &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
  MT_ASSERT(ret == SUCCESS);
  
  //alpha blend.
  src_vsb.x = 0;
  src_vsb.y = 0;
  src_vsb.w = fill_w;
  src_vsb.h = fill_h;  

  drc_vsb.x = 0;
  drc_vsb.y = 0;
  drc_vsb.w = fill_w;
  drc_vsb.h = fill_h;

  drc_vsb.x += (rend_w - fill_w) / 2;
  drc_vsb.y += (rend_h - fill_h) / 2;

  drc_vsb.x += p_priv->anim_param.rend_rect.left;
  drc_vsb.y += p_priv->anim_param.rend_rect.top;

  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;
  
  memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
  bopt2.enableBld = TRUE;
  bopt2.blendCfg.src_blend_fact = GL_ONE_MINUS_DST_ALPHA;
  bopt2.blendCfg.dst_blend_fact = GL_DST_ALPHA;
  
  ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->anim_param.p_rgn,
    &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
  MT_ASSERT(ret == SUCCESS);
}

static void _box_in_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_w = 0, step_h = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  RET_CODE ret = SUCCESS;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  drc_vsb.w = p_priv->anim_param.curn_frame * step_w;
  drc_vsb.h = p_priv->anim_param.curn_frame * step_h;

  drc_vsb.w = step_w * p_priv->anim_param.curn_frame;
  drc_vsb.h = step_h * p_priv->anim_param.curn_frame;

  if(drc_vsb.w > fill_w)
  {
    drc_vsb.w = fill_w;
  }

  if(drc_vsb.h > fill_h)
  {
    drc_vsb.h = fill_h;
  }
  
  drc_vsb.x = (rend_w - drc_vsb.w) / 2;
  drc_vsb.y = (rend_h - drc_vsb.h) / 2;

  drc_vsb.x += p_priv->anim_param.rend_rect.left;
  drc_vsb.y += p_priv->anim_param.rend_rect.top;

  src_vsb.x = (fill_w - drc_vsb.w) / 2;
  src_vsb.y = (fill_h - drc_vsb.h) / 2;
  src_vsb.w = drc_vsb.w;
  src_vsb.h = drc_vsb.h;

  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;
  
  ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);  
  MT_ASSERT(ret == SUCCESS);
  
  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;

  src_vsb.x = 0;
  src_vsb.y = 0;
  src_vsb.w = RECTW(p_priv->win_rect);
  src_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x = 0;
  drc_vsb.y = 0;
  drc_vsb.w = RECTW(p_priv->win_rect);
  drc_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;
  
  ret = gpe_bitblt2src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);  
  MT_ASSERT(ret == SUCCESS);
}

static void _box_out_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_w = 0, step_h = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  rect_t rect = {0};
  RET_CODE ret = SUCCESS;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);

  rect.left = 0;
  rect.top = 0;
  rect.right = RECTW(p_priv->win_rect);
  rect.bottom = RECTH(p_priv->win_rect);

  ret = gpe_draw_rectangle_vsb(p_gpe, p_priv->p_old_rgn, &rect, p_priv->anim_param.bg_color);
  MT_ASSERT(ret == SUCCESS);
  
  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_w = (fill_w + p_priv->frames - 1) / p_priv->frames;
  step_h = (fill_h + p_priv->frames - 1) / p_priv->frames;

  drc_vsb.w = p_priv->anim_param.curn_frame * step_w;
  drc_vsb.h = p_priv->anim_param.curn_frame * step_h;

  if(drc_vsb.w > fill_w)
  {
    drc_vsb.w = fill_w;
  }

  if(drc_vsb.h > fill_h)
  {
    drc_vsb.h = fill_h;
  }

  drc_vsb.w = fill_w - drc_vsb.w;
  drc_vsb.h = fill_h - drc_vsb.h;

  drc_vsb.x = (rend_w - drc_vsb.w) / 2;
  drc_vsb.y = (rend_h - drc_vsb.h) / 2;

  drc_vsb.x += p_priv->anim_param.rend_rect.left;
  drc_vsb.y += p_priv->anim_param.rend_rect.top;

  src_vsb.x = (fill_w - drc_vsb.w) / 2;
  src_vsb.y = (fill_h - drc_vsb.h) / 2;
  src_vsb.w = drc_vsb.w;
  src_vsb.h = drc_vsb.h;

  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;
  
  if((drc_vsb.w != 0) && (drc_vsb.h != 0))
  {  
    ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);  
    MT_ASSERT(ret == SUCCESS);
  }
  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;

  src_vsb.x = 0;
  src_vsb.y = 0;
  src_vsb.w = RECTW(p_priv->win_rect);
  src_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x = 0;
  drc_vsb.y = 0;
  drc_vsb.w = RECTW(p_priv->win_rect);
  drc_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;
  
  ret = gpe_bitblt2src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2); 
  MT_ASSERT(ret == SUCCESS);
}

static void _box_inout_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_w = 0, step_h = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  rect_t rect = {0};
  RET_CODE ret = SUCCESS;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_w = (fill_w + p_priv->frames - 1) / p_priv->frames;
  step_h = (fill_h + p_priv->frames - 1) / p_priv->frames;

  if(p_priv->is_first_half == FALSE)
  {
    drc_vsb.w = p_priv->anim_param.curn_frame * step_w;
    drc_vsb.h = p_priv->anim_param.curn_frame * step_h;

    if(drc_vsb.w > fill_w)
    {
      drc_vsb.w = fill_w;
    }

    if(drc_vsb.h > fill_h)
    {
      drc_vsb.h = fill_h;
    }
    
    drc_vsb.w = fill_w - drc_vsb.w;
    drc_vsb.h = fill_h - drc_vsb.h;    

    rect.left = 0;
    rect.top = 0;
    rect.right = RECTW(p_priv->win_rect);
    rect.bottom = RECTH(p_priv->win_rect);

    ret = gpe_draw_rectangle_vsb(p_gpe, p_priv->p_old_rgn, &rect, p_priv->anim_param.bg_color);
    MT_ASSERT(ret == SUCCESS);
  }
  else
  {
    drc_vsb.w = p_priv->anim_param.curn_frame * step_w;
    drc_vsb.h = p_priv->anim_param.curn_frame * step_h;

    if(drc_vsb.w > fill_w)
    {
      drc_vsb.w = fill_w;
    }

    if(drc_vsb.h > fill_h)
    {
      drc_vsb.h = fill_h;
    }

    if(p_priv->anim_param.curn_frame == p_priv->frames)
    {
      p_priv->anim_param.curn_frame = 0;

      p_priv->is_first_half = FALSE;
    }
  }

  drc_vsb.x = (rend_w - drc_vsb.w) / 2;
  drc_vsb.y = (rend_h - drc_vsb.h) / 2;

  drc_vsb.x += p_priv->anim_param.rend_rect.left;
  drc_vsb.y += p_priv->anim_param.rend_rect.top;

  src_vsb.x = (fill_w - drc_vsb.w) / 2;
  src_vsb.y = (fill_h - drc_vsb.h) / 2;
  src_vsb.w = drc_vsb.w;
  src_vsb.h = drc_vsb.h;
  
  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;

  if((drc_vsb.w != 0) && (drc_vsb.h != 0))
  {
    ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
        &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2); 
    MT_ASSERT(ret == SUCCESS);
  }

  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;

  src_vsb.x = 0;
  src_vsb.y = 0;
  src_vsb.w = RECTW(p_priv->win_rect);
  src_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x = 0;
  drc_vsb.y = 0;
  drc_vsb.w = RECTW(p_priv->win_rect);
  drc_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;

  ret = gpe_bitblt2src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2); 
  MT_ASSERT(ret == SUCCESS);
}

static void _smooth_move_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  u32 step_x = 0, step_y = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  RET_CODE ret = SUCCESS;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);

  OS_PRINTF("@#smooth move %d.\n", p_priv->anim_param.curn_frame);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);
  
  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_x = (wind_w + p_priv->frames - 1) / p_priv->frames;
  step_y = (wind_h + p_priv->frames - 1) / p_priv->frames;

  //ref rgn blt.
  if(step_x * p_priv->anim_param.curn_frame >= wind_w)
  {
    src_vsb.x = 0;
    src_vsb.w = wind_w;
  }
  else
  {
    src_vsb.x = wind_w - step_x * p_priv->anim_param.curn_frame;
    src_vsb.w = step_x * p_priv->anim_param.curn_frame;
  }
  src_vsb.y = 0;
  src_vsb.h = wind_h;

  drc_vsb.x = 0;
  drc_vsb.y = 0;
  drc_vsb.w = src_vsb.w;
  drc_vsb.h = src_vsb.h;
  
  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;

  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;
  ret = gpe_bitblt2src(p_gpe, p_priv->p_ref_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);  
  MT_ASSERT(ret == SUCCESS);

  //old rgn blt.
  src_vsb.x = 0;
  src_vsb.y = 0;
  src_vsb.h = wind_h;
  if(step_x * p_priv->anim_param.curn_frame < wind_w)
  {
    src_vsb.w = wind_w - step_x * p_priv->anim_param.curn_frame;
  }
  else
  {
    src_vsb.w = 0;
  }

  drc_vsb.x = wind_w - src_vsb.w;
  drc_vsb.y = 0;
  drc_vsb.w = src_vsb.w;
  drc_vsb.h = wind_h;
  
  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;

  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;
  if(src_vsb.w != 0)
  {
    ret = gpe_bitblt2src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn,
        &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
    MT_ASSERT(ret == SUCCESS);
  }
}

static void _uncover_right_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 step_x = 0, step_y = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  RET_CODE ret = SUCCESS;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_old_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_x = ((rend_w + fill_w) / 2 +
    p_priv->anim_param.rend_rect.left + p_priv->frames - 1) / p_priv->frames;
  step_y = ((rend_h + fill_h) / 2 +
    p_priv->anim_param.rend_rect.top + p_priv->frames - 1) / p_priv->frames;

  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;

  src_vsb.x = 0;
  src_vsb.y = 0;
  src_vsb.w = RECTW(p_priv->win_rect);
  src_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x = 0;
  drc_vsb.y = 0;
  drc_vsb.w = RECTW(p_priv->win_rect);
  drc_vsb.h = RECTH(p_priv->win_rect);

  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;
  
  ret = gpe_bitblt2src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
  MT_ASSERT(ret == SUCCESS);
  
  if(step_x * (p_priv->anim_param.curn_frame) <= fill_w)
  {
    src_vsb.x = fill_w - step_x * (p_priv->anim_param.curn_frame);
    src_vsb.w = step_x * (p_priv->anim_param.curn_frame);
    src_vsb.y = 0;
    src_vsb.h = fill_h;

    drc_vsb.x = 0;
    drc_vsb.w = step_x * (p_priv->anim_param.curn_frame);
    drc_vsb.y = (p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2);
    drc_vsb.h = fill_h;
  }
  else
  {
    src_vsb.x = 0;
    src_vsb.w = fill_w;
    src_vsb.y = 0;
    src_vsb.h = fill_h;

    drc_vsb.x = step_x * (p_priv->anim_param.curn_frame) - fill_w;
    drc_vsb.w = fill_w;
    drc_vsb.y = (p_priv->anim_param.rend_rect.top + (rend_h - fill_h) / 2);
    drc_vsb.h = fill_h;    
  }

  drc_vsb.x += p_priv->win_rect.left;
  drc_vsb.y += p_priv->win_rect.top;

  bopt2.enableRop = TRUE;
  bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
  bopt2.ropCfg.rop_pattern = 0;
  
  ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
  MT_ASSERT(ret == SUCCESS);
}

static void _blinds_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  RET_CODE ret = SUCCESS;
  surface_info_t src_info = {0}, dst_info = {0}, ref_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0}, rrc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  cct_gpe_paint2opt_t popt = {0};
  cct_gpe_blt3opt_t opt3 = {0};
  u32 blinds_width = 0;
  u32 step_small = 0, step_large = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  rect_t rect = {0};
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);
  
  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_small = (4095 + p_priv->frames - 1) / p_priv->frames;
  step_large = 3 * step_small;
  blinds_width = (wind_w + p_priv->frames - 1) / p_priv->frames;

  if(p_priv->is_first_half)
  {
    //caculate stop offset.
    if(p_priv->stop_offset[2] == 4095)
    {
     p_priv->stop_offset[1] += step_large;
    }
    else
    {
      p_priv->stop_offset[1] += step_small;
    }
    
    p_priv->stop_offset[2] += step_large;
    
    if(p_priv->stop_offset[1] > 4095)
    {
      p_priv->stop_offset[1] = 4095;
    }
    
    if(p_priv->stop_offset[2] > 4095)
    {
      p_priv->stop_offset[2] = 4095;
    }

    p_priv->stop_offset[0] = 0;
    p_priv->stop_offset[3] = 4095;
    
    popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
    popt.paint_cfg.paint_liner_gradt.begin.x = 0;
    popt.paint_cfg.paint_liner_gradt.begin.y = 0;
    popt.paint_cfg.paint_liner_gradt.end.x = blinds_width;
    popt.paint_cfg.paint_liner_gradt.end.y = 0;
    popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
    popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
    popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
    popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
    popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];  
    
    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = RECTW(p_priv->win_rect);
    drc_vsb.h = RECTH(p_priv->win_rect);

    ret = gpe_paint_blit(p_gpe, p_priv->p_ref_rgn, &drc_vsb, &dst_info, &popt);
    MT_ASSERT(ret == SUCCESS);
    
    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    opt3.enableDrawSten = TRUE;
    opt3.enableBld = TRUE;
    opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
    opt3.blendCfg.dst_blend_fact = GL_ZERO;

    ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
    MT_ASSERT(ret == SUCCESS);

    if(p_priv->anim_param.curn_frame == p_priv->frames)
    {
      p_priv->anim_param.curn_frame = 0;

      p_priv->is_first_half = FALSE;

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 0;

      //clear old rgn.
      rect.left = 0;
      rect.top = 0;
      rect.right = wind_w;
      rect.bottom = wind_h;
      
      ret = gpe_draw_rectangle_vsb(p_gpe, p_priv->p_old_rgn, &rect, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = fill_w;
      src_vsb.h = fill_h;  

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = fill_w;
      drc_vsb.h = fill_h;

      drc_vsb.x += (rend_w - fill_w) / 2;
      drc_vsb.y += (rend_h - fill_h) / 2;

      drc_vsb.x += p_priv->anim_param.rend_rect.left;
      drc_vsb.y += p_priv->anim_param.rend_rect.top;

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
        &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  else
  {
      if(p_priv->stop_offset[2] == 4095)
      {
        p_priv->stop_offset[1] += step_large;
      }
      else
      {
        p_priv->stop_offset[1] += step_small;
      }

      p_priv->stop_offset[2] += step_large;

      if(p_priv->stop_offset[1] > 4095)
      {
        p_priv->stop_offset[1] = 4095;
      }

      if(p_priv->stop_offset[2] > 4095)
      {
        p_priv->stop_offset[2] = 4095;
      }

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[3] = 4095;

      popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
      popt.paint_cfg.paint_liner_gradt.begin.x = 0;
      popt.paint_cfg.paint_liner_gradt.begin.y = 0;
      popt.paint_cfg.paint_liner_gradt.end.x = blinds_width;
      popt.paint_cfg.paint_liner_gradt.end.y = 0;
      popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
      popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
      popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
      popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
      popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];

      ret = gpe_paint_blit(p_gpe, p_priv->p_ref_rgn, &drc_vsb, &dst_info, &popt);
      MT_ASSERT(ret == SUCCESS);

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = wind_w;
      src_vsb.h = wind_h;

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;

      drc_vsb.x += p_priv->win_rect.left;
      drc_vsb.y += p_priv->win_rect.top;

      rrc_vsb.x = 0;
      rrc_vsb.y = 0;
      rrc_vsb.w = wind_w;
      rrc_vsb.h = wind_h;      

      opt3.enableDrawSten = TRUE;
      opt3.enableBld = TRUE;
      opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
      opt3.blendCfg.dst_blend_fact = GL_ZERO;
      
      ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
        &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
      MT_ASSERT(ret == SUCCESS);
  }
}

static void _square_board_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  RET_CODE ret = SUCCESS;
  surface_info_t src_info = {0}, dst_info = {0}, ref_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0}, rrc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  cct_gpe_paint2opt_t popt = {0};
  cct_gpe_blt3opt_t opt3 = {0};
  u32 board_sizex = 0, board_sizey = 0;
  u32 step_small = 0, step_large = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  rect_t rect = {0};
  pos_t pat_beg = {0};
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);
  
  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_small = (4095 + p_priv->frames - 1) / p_priv->frames;
  step_large = 3 * step_small;
  board_sizex = (wind_w + p_priv->frames - 1) / p_priv->frames;
  board_sizey = (wind_h + p_priv->frames - 1) / p_priv->frames;

  if(p_priv->is_first_half)
  {
    //caculate stop offset.
    if(p_priv->stop_offset[2] == 4095)
    {
     p_priv->stop_offset[1] += step_large;
    }
    else
    {
      p_priv->stop_offset[1] += step_small;
    }
    
    p_priv->stop_offset[2] += step_large;
    
    if(p_priv->stop_offset[1] > 4095)
    {
      p_priv->stop_offset[1] = 4095;
    }
    
    if(p_priv->stop_offset[2] > 4095)
    {
      p_priv->stop_offset[2] = 4095;
    }

    p_priv->stop_offset[0] = 0;
    p_priv->stop_offset[3] = 4095;
    
    popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
    popt.paint_cfg.paint_liner_gradt.begin.x = 0;
    popt.paint_cfg.paint_liner_gradt.begin.y = 0;
    popt.paint_cfg.paint_liner_gradt.end.x = board_sizex;
    popt.paint_cfg.paint_liner_gradt.end.y = board_sizey;
    popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
    popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
    popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
    popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
    popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];  
    
    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = board_sizex;
    drc_vsb.h = board_sizey;

    ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
    MT_ASSERT(ret == SUCCESS);
    
    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = board_sizex;
    src_vsb.h = board_sizey;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;    
    ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REPEAT, 0);
    MT_ASSERT(ret == SUCCESS);

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    opt3.enableDrawSten = TRUE;
    opt3.enableBld = TRUE;
    opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
    opt3.blendCfg.dst_blend_fact = GL_ZERO;

    ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
    MT_ASSERT(ret == SUCCESS);

    if(p_priv->anim_param.curn_frame == p_priv->frames)
    {
      p_priv->anim_param.curn_frame = 0;

      p_priv->is_first_half = FALSE;

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 4095;

      //clear old rgn.
      rect.left = 0;
      rect.top = 0;
      rect.right = wind_w;
      rect.bottom = wind_h;
      
      ret = gpe_draw_rectangle_vsb(p_gpe, p_priv->p_old_rgn, &rect, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = fill_w;
      src_vsb.h = fill_h;  

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = fill_w;
      drc_vsb.h = fill_h;

      drc_vsb.x += (rend_w - fill_w) / 2;
      drc_vsb.y += (rend_h - fill_h) / 2;

      drc_vsb.x += p_priv->anim_param.rend_rect.left;
      drc_vsb.y += p_priv->anim_param.rend_rect.top;

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
        &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  else
  {
      if(p_priv->stop_offset[2] == 4095)
      {
        p_priv->stop_offset[1] += step_large;
      }
      else
      {
        p_priv->stop_offset[1] += step_small;
      }

      p_priv->stop_offset[2] += step_large;

      if(p_priv->stop_offset[1] > 4095)
      {
        p_priv->stop_offset[1] = 4095;
      }

      if(p_priv->stop_offset[2] > 4095)
      {
        p_priv->stop_offset[2] = 4095;
      }

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[3] = 4095;

      popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
      popt.paint_cfg.paint_liner_gradt.begin.x = 0;
      popt.paint_cfg.paint_liner_gradt.begin.y = 0;
      popt.paint_cfg.paint_liner_gradt.end.x = board_sizex;
      popt.paint_cfg.paint_liner_gradt.end.y = board_sizey;
      popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
      popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
      popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
      popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
      popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = board_sizex;
      drc_vsb.h = board_sizey;      

      ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = board_sizex;
      src_vsb.h = board_sizey;

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;    
      ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
        &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REPEAT, 0);
      MT_ASSERT(ret == SUCCESS);      

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = RECTW(p_priv->win_rect);
      drc_vsb.h = RECTH(p_priv->win_rect);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = wind_w;
      src_vsb.h = wind_h;

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;

      drc_vsb.x += p_priv->win_rect.left;
      drc_vsb.y += p_priv->win_rect.top;

      rrc_vsb.x = 0;
      rrc_vsb.y = 0;
      rrc_vsb.w = wind_w;
      rrc_vsb.h = wind_h;      

      opt3.enableDrawSten = TRUE;
      opt3.enableBld = TRUE;
      opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
      opt3.blendCfg.dst_blend_fact = GL_ZERO;
      
      ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
        &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
      MT_ASSERT(ret == SUCCESS);
  }
}

static void _diamond_board_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  RET_CODE ret = SUCCESS;
  surface_info_t src_info = {0}, dst_info = {0}, ref_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0}, rrc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  cct_gpe_paint2opt_t popt = {0};
  cct_gpe_blt3opt_t opt3 = {0};
  u32 board_sizex = 0, board_sizey = 0;
  u32 step_small = 0, step_large = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  rect_t rect = {0};
  pos_t pat_beg = {0};
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);
  
  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_small = (4095 + p_priv->frames - 1) / p_priv->frames;
  step_large = 3 * step_small;
  board_sizex = (wind_w + p_priv->frames - 1) / p_priv->frames;
  board_sizey = (wind_h + p_priv->frames - 1) / p_priv->frames;

  if(p_priv->is_first_half)
  {
    //caculate stop offset.
    if(p_priv->stop_offset[2] == 4095)
    {
     p_priv->stop_offset[1] += step_large;
    }
    else
    {
      p_priv->stop_offset[1] += step_small;
    }
    
    p_priv->stop_offset[2] += step_large;
    
    if(p_priv->stop_offset[1] > 4095)
    {
      p_priv->stop_offset[1] = 4095;
    }
    
    if(p_priv->stop_offset[2] > 4095)
    {
      p_priv->stop_offset[2] = 4095;
    }

    p_priv->stop_offset[0] = 0;
    p_priv->stop_offset[3] = 4095;
    
    popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
    popt.paint_cfg.paint_liner_gradt.begin.x = board_sizex;
    popt.paint_cfg.paint_liner_gradt.begin.y = board_sizey;
    popt.paint_cfg.paint_liner_gradt.end.x = 0;
    popt.paint_cfg.paint_liner_gradt.end.y = 0;
    popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
    popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
    popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
    popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
    popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];  
    
    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = board_sizex;
    drc_vsb.h = board_sizey;

    ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
    MT_ASSERT(ret == SUCCESS);
    
    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = board_sizex;
    src_vsb.h = board_sizey;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;    
    ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REFLECT, 0);
    MT_ASSERT(ret == SUCCESS);

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    opt3.enableDrawSten = TRUE;
    opt3.enableBld = TRUE;
    opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
    opt3.blendCfg.dst_blend_fact = GL_ZERO;

    ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
    MT_ASSERT(ret == SUCCESS);

    if(p_priv->anim_param.curn_frame == p_priv->frames)
    {
      p_priv->anim_param.curn_frame = 0;

      p_priv->is_first_half = FALSE;

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 4095;

      //clear old rgn.
      rect.left = 0;
      rect.top = 0;
      rect.right = wind_w;
      rect.bottom = wind_h;
      
      ret = gpe_draw_rectangle_vsb(p_gpe, p_priv->p_old_rgn, &rect, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = fill_w;
      src_vsb.h = fill_h;  

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = fill_w;
      drc_vsb.h = fill_h;

      drc_vsb.x += (rend_w - fill_w) / 2;
      drc_vsb.y += (rend_h - fill_h) / 2;

      drc_vsb.x += p_priv->anim_param.rend_rect.left;
      drc_vsb.y += p_priv->anim_param.rend_rect.top;

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
        &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  else
  {
      if(p_priv->stop_offset[2] == 4095)
      {
        p_priv->stop_offset[1] += step_large;
      }
      else
      {
        p_priv->stop_offset[1] += step_small;
      }

      p_priv->stop_offset[2] += step_large;

      if(p_priv->stop_offset[1] > 4095)
      {
        p_priv->stop_offset[1] = 4095;
      }

      if(p_priv->stop_offset[2] > 4095)
      {
        p_priv->stop_offset[2] = 4095;
      }

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[3] = 4095;

      popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
      popt.paint_cfg.paint_liner_gradt.begin.x = board_sizex;
      popt.paint_cfg.paint_liner_gradt.begin.y = board_sizey;
      popt.paint_cfg.paint_liner_gradt.end.x = 0;
      popt.paint_cfg.paint_liner_gradt.end.y = 0;
      popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
      popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
      popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
      popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
      popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = board_sizex;
      drc_vsb.h = board_sizey;

      ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = board_sizex;
      src_vsb.h = board_sizey;

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;    
      ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
        &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REFLECT, 0);
      MT_ASSERT(ret == SUCCESS);      

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = wind_w;
      src_vsb.h = wind_h;

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;

      drc_vsb.x += p_priv->win_rect.left;
      drc_vsb.y += p_priv->win_rect.top;

      rrc_vsb.x = 0;
      rrc_vsb.y = 0;
      rrc_vsb.w = wind_w;
      rrc_vsb.h = wind_h;      

      opt3.enableDrawSten = TRUE;
      opt3.enableBld = TRUE;
      opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
      opt3.blendCfg.dst_blend_fact = GL_ZERO;
      
      ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
        &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
      MT_ASSERT(ret == SUCCESS);
  }
}

static void _round_board_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  RET_CODE ret = SUCCESS;
  surface_info_t src_info = {0}, dst_info = {0}, ref_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0}, rrc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  cct_gpe_paint2opt_t popt = {0};
  cct_gpe_blt3opt_t opt3 = {0};
  u32 board_sizex = 0, board_sizey = 0;
  u32 step_small = 0, step_large = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  rect_t rect = {0};
  pos_t pat_beg = {0};
  s32 offset1 = 0, offset2 = 0;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);
  
  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_small = (4095 + p_priv->frames - 1) / p_priv->frames;
  step_large = 3 * step_small;
  board_sizex = (wind_w + p_priv->frames - 1) / p_priv->frames;
  board_sizey = (wind_h + p_priv->frames - 1) / p_priv->frames;

  if(p_priv->is_first_half)
  {
    //caculate stop offset.
    offset1 = p_priv->stop_offset[1];
    offset2 = p_priv->stop_offset[2];

    offset1 -= step_large;
    
    if(offset1 < 0)
    {
      p_priv->stop_offset[1] = 0;
    }
    else
    {
      p_priv->stop_offset[1] = offset1;
    }
    
    if(p_priv->stop_offset[1] == 0)
    {
      offset2 -= step_large;
    }
    else
    {
      offset2 -= step_small;
    }

    if(offset2 < 0)
    {
      p_priv->stop_offset[2] = 0;
    }
    else
    {
      p_priv->stop_offset[2] = offset2;
    }

    p_priv->stop_offset[0] = 0;
    p_priv->stop_offset[3] = 4095;
    
    popt.paint_cfg.paint_type = VG_PAINT_TYPE_RADIAL_GRADIENT;
    popt.paint_cfg.paint_radial_gradt.center.x = board_sizex;
    popt.paint_cfg.paint_radial_gradt.center.y = board_sizex;
    popt.paint_cfg.paint_radial_gradt.focus.x = board_sizex;
    popt.paint_cfg.paint_radial_gradt.focus.y = board_sizex;
    popt.paint_cfg.paint_radial_gradt.radius = board_sizex * 20 / 14;    
    popt.paint_cfg.paint_radial_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REFLECT;
    popt.paint_cfg.paint_radial_gradt.stop0.argb = 0xffffffff;
    popt.paint_cfg.paint_radial_gradt.stop1.argb = 0xffffffff;
    popt.paint_cfg.paint_radial_gradt.stop2.argb = 0xff000000;
    popt.paint_cfg.paint_radial_gradt.stop3.argb = 0xff000000;
    popt.paint_cfg.paint_radial_gradt.stop0.offset = p_priv->stop_offset[0];
    popt.paint_cfg.paint_radial_gradt.stop1.offset = p_priv->stop_offset[1];
    popt.paint_cfg.paint_radial_gradt.stop2.offset = p_priv->stop_offset[2];
    popt.paint_cfg.paint_radial_gradt.stop3.offset = p_priv->stop_offset[3];  

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = 2 * board_sizex;
    drc_vsb.h = 2 * board_sizex;

    ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
    MT_ASSERT(ret == SUCCESS);
    
    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = 2 * board_sizex;
    src_vsb.h = 2 * board_sizex;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;    
    ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REPEAT, 0);
    MT_ASSERT(ret == SUCCESS);

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    opt3.enableDrawSten = TRUE;
    opt3.enableBld = TRUE;
    opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
    opt3.blendCfg.dst_blend_fact = GL_ZERO;

    ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
    MT_ASSERT(ret == SUCCESS);

    if(p_priv->anim_param.curn_frame == p_priv->frames)
    {
      p_priv->anim_param.curn_frame = 0;

      p_priv->is_first_half = FALSE;

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 4095;

      //clear old rgn.
      rect.left = 0;
      rect.top = 0;
      rect.right = wind_w;
      rect.bottom = wind_h;
      
      ret = gpe_draw_rectangle_vsb(p_gpe, p_priv->p_old_rgn, &rect, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = fill_w;
      src_vsb.h = fill_h;  

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = fill_w;
      drc_vsb.h = fill_h;

      drc_vsb.x += (rend_w - fill_w) / 2;
      drc_vsb.y += (rend_h - fill_h) / 2;

      drc_vsb.x += p_priv->anim_param.rend_rect.left;
      drc_vsb.y += p_priv->anim_param.rend_rect.top;

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
        &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  else
  {
    offset1 = p_priv->stop_offset[1];
    offset2 = p_priv->stop_offset[2];

    offset2 += step_large;

    if(offset2 > 4095)
    {
      p_priv->stop_offset[2] = 4095;
    }
    else
    {
      p_priv->stop_offset[2] = offset2;
    }

    if(p_priv->stop_offset[2] == 4095)
    {
      offset1 += step_large;
    }
    else
    {
      offset1 += step_small;
    }

    if(offset1 > 4095)
    {
      p_priv->stop_offset[1] = 4095;
    }
    else
    {
      p_priv->stop_offset[1] = offset1;
    }
    
    popt.paint_cfg.paint_type = VG_PAINT_TYPE_RADIAL_GRADIENT;
    popt.paint_cfg.paint_radial_gradt.center.x = board_sizex;
    popt.paint_cfg.paint_radial_gradt.center.y = board_sizex;
    popt.paint_cfg.paint_radial_gradt.focus.x = board_sizex;
    popt.paint_cfg.paint_radial_gradt.focus.y = board_sizex;
    popt.paint_cfg.paint_radial_gradt.radius = board_sizex * 20 / 14;    
    popt.paint_cfg.paint_radial_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REFLECT;
    popt.paint_cfg.paint_radial_gradt.stop0.argb = 0xffffffff;
    popt.paint_cfg.paint_radial_gradt.stop1.argb = 0xffffffff;
    popt.paint_cfg.paint_radial_gradt.stop2.argb = 0xff000000;
    popt.paint_cfg.paint_radial_gradt.stop3.argb = 0xff000000;
    popt.paint_cfg.paint_radial_gradt.stop0.offset = p_priv->stop_offset[0];
    popt.paint_cfg.paint_radial_gradt.stop1.offset = p_priv->stop_offset[1];
    popt.paint_cfg.paint_radial_gradt.stop2.offset = p_priv->stop_offset[2];
    popt.paint_cfg.paint_radial_gradt.stop3.offset = p_priv->stop_offset[3];  

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = 2 * board_sizex;
    drc_vsb.h = 2 * board_sizex;

    ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
    MT_ASSERT(ret == SUCCESS);
    
    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = 2 * board_sizex;
    src_vsb.h = 2 * board_sizex;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;    
    ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REPEAT, 0);
    MT_ASSERT(ret == SUCCESS);
    
    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = RECTW(p_priv->win_rect);
    drc_vsb.h = RECTH(p_priv->win_rect);

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;      

    opt3.enableDrawSten = TRUE;
    opt3.enableBld = TRUE;
    opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
    opt3.blendCfg.dst_blend_fact = GL_ZERO;
    
    ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
    MT_ASSERT(ret == SUCCESS);
  }
}

static void _fade_diamond_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  RET_CODE ret = SUCCESS;
  surface_info_t src_info = {0}, dst_info = {0}, ref_info = {0};
  rect_vsb_t src_vsb = {0}, drc_vsb = {0}, rrc_vsb = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  cct_gpe_paint2opt_t popt = {0};
  cct_gpe_blt3opt_t opt3 = {0};
  u32 board_sizex = 0, board_sizey = 0;
  u32 step_small = 0, step_large = 0;
  cct_gpe_blt2opt_t bopt2 = {0};
  rect_t rect = {0};
  pos_t pat_beg = {0};
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);
  
  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_small = (4095 + p_priv->frames - 1) / p_priv->frames;
  step_large = 3 * step_small;
  board_sizex = (wind_w + p_priv->frames - 1) / p_priv->frames;
  board_sizey = (wind_h + p_priv->frames - 1) / p_priv->frames;

  if(p_priv->is_first_half)
  {
    //caculate stop offset.
    if(p_priv->stop_offset[2] == 4095)
    {
     p_priv->stop_offset[1] += step_large;
    }
    else
    {
      p_priv->stop_offset[1] += step_small;
    }
    
    p_priv->stop_offset[2] += step_large;
    
    if(p_priv->stop_offset[1] > 4095)
    {
      p_priv->stop_offset[1] = 4095;
    }
    
    if(p_priv->stop_offset[2] > 4095)
    {
      p_priv->stop_offset[2] = 4095;
    }

    p_priv->stop_offset[0] = 0;
    p_priv->stop_offset[3] = 4095;
    
    popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
    popt.paint_cfg.paint_liner_gradt.begin.x = 0;
    popt.paint_cfg.paint_liner_gradt.begin.y = 0;
    popt.paint_cfg.paint_liner_gradt.end.x = wind_w / 2;
    popt.paint_cfg.paint_liner_gradt.end.y = wind_h / 2;
    popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
    popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xff000000;
    popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xffffffff;
    popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
    popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
    popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
    popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];  
    
    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w / 2;
    drc_vsb.h = wind_h / 2;

    ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
    MT_ASSERT(ret == SUCCESS);
    
    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w / 2;
    src_vsb.h = wind_h / 2;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;    
    ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REFLECT, 0);
    MT_ASSERT(ret == SUCCESS);

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    opt3.enableDrawSten = TRUE;
    opt3.enableBld = TRUE;
    opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
    opt3.blendCfg.dst_blend_fact = GL_ZERO;

    ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
      &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
    MT_ASSERT(ret == SUCCESS);

    if(p_priv->anim_param.curn_frame == p_priv->frames)
    {
      p_priv->anim_param.curn_frame = 0;

      p_priv->is_first_half = FALSE;

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[1] = 0;
      p_priv->stop_offset[2] = 0;
      p_priv->stop_offset[3] = 4095;

      //clear old rgn.
      rect.left = 0;
      rect.top = 0;
      rect.right = wind_w;
      rect.bottom = wind_h;
      
      ret = gpe_draw_rectangle_vsb(p_gpe, p_priv->p_old_rgn, &rect, p_priv->anim_param.bg_color);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = fill_w;
      src_vsb.h = fill_h;  

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = fill_w;
      drc_vsb.h = fill_h;

      drc_vsb.x += (rend_w - fill_w) / 2;
      drc_vsb.y += (rend_h - fill_h) / 2;

      drc_vsb.x += p_priv->anim_param.rend_rect.left;
      drc_vsb.y += p_priv->anim_param.rend_rect.top;

      memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
      bopt2.enableRop = TRUE;
      bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
      bopt2.ropCfg.rop_pattern = 0;

      ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
        &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  else
  {
      if(p_priv->stop_offset[2] == 4095)
      {
        p_priv->stop_offset[1] += step_large;
      }
      else
      {
        p_priv->stop_offset[1] += step_small;
      }

      p_priv->stop_offset[2] += step_large;

      if(p_priv->stop_offset[1] > 4095)
      {
        p_priv->stop_offset[1] = 4095;
      }

      if(p_priv->stop_offset[2] > 4095)
      {
        p_priv->stop_offset[2] = 4095;
      }

      p_priv->stop_offset[0] = 0;
      p_priv->stop_offset[3] = 4095;

      popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
      popt.paint_cfg.paint_liner_gradt.begin.x = wind_w / 2;
      popt.paint_cfg.paint_liner_gradt.begin.y = wind_h / 2;
      popt.paint_cfg.paint_liner_gradt.end.x = 0;
      popt.paint_cfg.paint_liner_gradt.end.y = 0;
      popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
      popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop1.argb = 0xffffffff;
      popt.paint_cfg.paint_liner_gradt.stop2.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop3.argb = 0xff000000;
      popt.paint_cfg.paint_liner_gradt.stop0.offset = p_priv->stop_offset[0];
      popt.paint_cfg.paint_liner_gradt.stop1.offset = p_priv->stop_offset[1];
      popt.paint_cfg.paint_liner_gradt.stop2.offset = p_priv->stop_offset[2];
      popt.paint_cfg.paint_liner_gradt.stop3.offset = p_priv->stop_offset[3];

      ret = gpe_paint_blit(p_gpe, p_priv->p_grad_rgn, &drc_vsb, &dst_info, &popt);
      MT_ASSERT(ret == SUCCESS);

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = wind_w / 2;
      src_vsb.h = wind_h / 2;

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;    
      ret = gpe_pattern_paint(p_gpe, p_priv->p_grad_rgn, p_priv->p_ref_rgn,
        &src_vsb, &drc_vsb, &pat_beg, VG_TILE_REFLECT, 0);
      MT_ASSERT(ret == SUCCESS);      

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;

      src_vsb.x = 0;
      src_vsb.y = 0;
      src_vsb.w = wind_w;
      src_vsb.h = wind_h;

      drc_vsb.x = 0;
      drc_vsb.y = 0;
      drc_vsb.w = wind_w;
      drc_vsb.h = wind_h;

      drc_vsb.x += p_priv->win_rect.left;
      drc_vsb.y += p_priv->win_rect.top;

      rrc_vsb.x = 0;
      rrc_vsb.y = 0;
      rrc_vsb.w = wind_w;
      rrc_vsb.h = wind_h;      

      opt3.enableDrawSten = TRUE;
      opt3.enableBld = TRUE;
      opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
      opt3.blendCfg.dst_blend_fact = GL_ZERO;
      
      ret = gpe_bitblt3src(p_gpe, p_priv->p_old_rgn, p_priv->anim_param.p_rgn, p_priv->p_ref_rgn,
        &src_vsb, &drc_vsb, &rrc_vsb, &src_info, &dst_info, &ref_info, &opt3);
      MT_ASSERT(ret == SUCCESS);
  }
}

static void _3d_rotate_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  RET_CODE ret = SUCCESS;
  surface_info_t src_info = {0};
  surface_info_t dst_info = {0};
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  u32 step_x = 0, step_y = 0;
  cct_gpe_fillopt_t fopt = {0};
  rect_vsb_t src_vsb = {0}, rrc_vsb = {0}, drc_vsb = {0};
  cct_gpe_blt2opt_t bopt2 = {0};
  u32 temp = 0;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);
  MT_ASSERT(p_priv->p_anim_rgn != NULL);
  MT_ASSERT(p_priv->p_ref_rgn != NULL);
  MT_ASSERT(p_priv->p_old_rgn != NULL);

  fill_w = RECTW(p_priv->anim_param.fill_rect);
  fill_h = RECTH(p_priv->anim_param.fill_rect);
  rend_w = RECTW(p_priv->anim_param.rend_rect);
  rend_h = RECTH(p_priv->anim_param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  step_x = (wind_w + p_priv->frames - 1) / p_priv->frames;
  step_y = (wind_h + p_priv->frames - 1) / p_priv->frames;


  temp = step_x * p_priv->anim_param.curn_frame;

  if(temp > wind_w)
  {
    temp = wind_w;
  }

  if(temp < (wind_w / 2))
  {
    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    ret = gpe_fill_rect_ex(p_gpe,
      p_priv->p_ref_rgn, &rrc_vsb, p_priv->anim_param.bg_color, 0, &fopt);
    MT_ASSERT(ret == SUCCESS);

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = temp;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w - 2 * temp;
    drc_vsb.h = wind_h;

    ret = gpe_dert_scale(p_gpe, p_priv->p_old_rgn, p_priv->p_ref_rgn, 
      &src_vsb, &drc_vsb,NULL);
    MT_ASSERT(ret == SUCCESS);

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;  

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
    bopt2.enableRop = TRUE;
    bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
    bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
    bopt2.ropCfg.rop_pattern = 0;

    ret = gpe_bitblt2src(p_gpe, p_priv->p_ref_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
    MT_ASSERT(ret == SUCCESS);    
  }
  else
  {
    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    ret = gpe_fill_rect_ex(p_gpe,
      p_priv->p_ref_rgn, &rrc_vsb, p_priv->anim_param.bg_color, 0, &fopt);
    MT_ASSERT(ret == SUCCESS);

    rrc_vsb.x = 0;
    rrc_vsb.y = 0;
    rrc_vsb.w = wind_w;
    rrc_vsb.h = wind_h;

    ret = gpe_fill_rect_ex(p_gpe,
      p_priv->p_old_rgn, &rrc_vsb, p_priv->anim_param.bg_color, 0, &fopt);
    MT_ASSERT(ret == SUCCESS);   

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = fill_w;
    src_vsb.h = fill_h;  

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = fill_w;
    drc_vsb.h = fill_h;

    drc_vsb.x += (rend_w - fill_w) / 2;
    drc_vsb.y += (rend_h - fill_h) / 2;

    drc_vsb.x += p_priv->anim_param.rend_rect.left;
    drc_vsb.y += p_priv->anim_param.rend_rect.top;

    memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
    bopt2.enableRop = TRUE;
    bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
    bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
    bopt2.ropCfg.rop_pattern = 0;

    ret = gpe_bitblt2src(p_gpe, p_priv->p_anim_rgn, p_priv->p_old_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
    MT_ASSERT(ret == SUCCESS);    

    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;

    drc_vsb.x = wind_w - temp;
    drc_vsb.y = 0;
    drc_vsb.w = 2 * temp - wind_w;
    drc_vsb.h = wind_h;

    if(drc_vsb.w != 0)
    {
      ret = gpe_dert_scale(p_gpe, p_priv->p_old_rgn, p_priv->p_ref_rgn, 
        &src_vsb, &drc_vsb,NULL);
      MT_ASSERT(ret == SUCCESS);
    }
    src_vsb.x = 0;
    src_vsb.y = 0;
    src_vsb.w = wind_w;
    src_vsb.h = wind_h;  

    drc_vsb.x = 0;
    drc_vsb.y = 0;
    drc_vsb.w = wind_w;
    drc_vsb.h = wind_h;

    drc_vsb.x += p_priv->win_rect.left;
    drc_vsb.y += p_priv->win_rect.top;

    memset(&bopt2, 0, sizeof(cct_gpe_blt2opt_t));
    bopt2.enableRop = TRUE;
    bopt2.ropCfg.rop_a_id = ROP_COPYPEN;
    bopt2.ropCfg.rop_c_id = ROP_COPYPEN;
    bopt2.ropCfg.rop_pattern = 0;

    ret = gpe_bitblt2src(p_gpe, p_priv->p_ref_rgn, p_priv->anim_param.p_rgn,
      &src_vsb, &drc_vsb, &src_info, &dst_info, &bopt2);
    MT_ASSERT(ret == SUCCESS);    
  }
}

static BOOL pic_render_animation_step(pic_render_pin_private_t *p_priv)
{
  void *p_gpe = NULL;
  void *p_disp = NULL;
  os_msg_t msg = {0};
  ipin_t *p_ipin = NULL;
  
  MT_ASSERT(p_priv != NULL);

  if(p_priv->anim == REND_ANIM_NONE)
  {
    return FALSE;
  }

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_disp != NULL);
  MT_ASSERT(p_priv->anim_param.p_rgn != NULL);

  OS_PRINTF("anim step.\n");

  if(p_priv->anim_param.curn_frame != 0)
  {
    switch(p_priv->anim)
    {
      case REND_ANIM_L2R:
        _l2r_step(p_priv);
        break;

      case REND_ANIM_R2L:
        _r2l_step(p_priv);
        break;

      case REND_ANIM_T2B:
        _t2b_step(p_priv);
        break;

      case REND_ANIM_B2T:
        _b2t_step(p_priv);
        break;

      case REND_ANIM_SHTH:
        _shth_step(p_priv);
        break;

      case REND_ANIM_SHTV:
        _shtv_step(p_priv);
        break;

      case REND_ANIM_MOS:
        _mosaic_step(p_priv);
        break;

      case REND_ANIM_VFEN:
        _vfen_step(p_priv);
        break;

      case REND_ANIM_HFEN:
        _hfen_step(p_priv);
        break;

      case REND_ANIM_FADE:
        _fade_step(p_priv);
        break;

      case REND_ANIM_BOX_IN:
        _box_in_step(p_priv);
        break;        

      case REND_ANIM_BOX_OUT:
        _box_out_step(p_priv);
        break;

      case REND_ANIM_BOX_INOUT:
        _box_inout_step(p_priv);
        break;

      case REND_ANIM_SMOOTH_MOVE:
        _smooth_move_step(p_priv);
        break;

      case REND_ANIM_UNCOVER_RIGHT:
        _uncover_right_step(p_priv);
        break;

      case REND_ANIM_BLINDS:
        _blinds_step(p_priv);
        break;

      case REND_ANIM_SQUARE_BOARD:
        _square_board_step(p_priv);
        break;

      case REND_ANIM_DIAMOND_BOARD:
        _diamond_board_step(p_priv);
        break;

      case REND_ANIM_ROUND_BOARD:
        _round_board_step(p_priv);
        break;

      case REND_ANIM_FADE_DIAMOND:
        _fade_diamond_step(p_priv);
        break;

      case REND_ANIM_3D_ROTATE:
        _3d_rotate_step(p_priv);
        break;
        
      default:
        MT_ASSERT(0);
        break;
    }
  }
  else
  {
    //try to clear background.
    if(p_priv->anim_param.use_bg_color)
    {
      gpe_draw_rectangle_vsb(p_gpe, p_priv->anim_param.p_rgn,
        &p_priv->win_rect, p_priv->anim_param.bg_color);
    }
  }

  OS_PRINTF("update region\n");
  disp_layer_update_region(p_disp, p_priv->anim_param.p_rgn, NULL);

  p_priv->anim_param.curn_frame++;

  if(p_priv->anim_param.curn_frame > p_priv->frames)
  {
    //pic_render_stop_animation(p_priv);

    msg.content = PIC_DRAW_END;
    msg.para1 = p_priv->handle;
    p_ipin = (ipin_t *)p_priv->p_this;
    
    p_ipin->send_message_out(p_ipin, &msg);

    return FALSE;
  }

  return TRUE;
}

static void pic_render_task_entry(void *p_data)
{
  pic_render_pin_private_t *p_priv = (pic_render_pin_private_t *)p_data;
  os_msg_t msg = {0};
  u32 curn_ticks = 0;

  MT_ASSERT(p_priv != NULL);

  while(1)
  {
    if(mtos_messageq_receive(p_priv->msgq_id, &msg, RENDER_TASK_MSGQ_TIMEOUT))
    {
      switch(msg.content)
      {
        case PIC_REND_ANIM_START:
          p_priv->sm = PIC_REND_SM_ANIM;

          p_priv->anim_param.last_ticks = mtos_ticks_get();

          switch(p_priv->anim)
          {
            case REND_ANIM_MOS:
              _mosaic_init(p_priv);
              break;

            default:

              break;
          }
          break;

        case PIC_REND_ANIM_STOP:
          p_priv->sm = PIC_REND_SM_IDLE;

          OS_PRINTF("@@@@ animation stop\n");

          pic_rend_anim_release(p_priv);
  
          switch(p_priv->anim)
          {
            case REND_ANIM_MOS:
              _mosaic_deinit(p_priv);
              break;

            default:

              break;
          }

          memset(&p_priv->anim_param, 0, sizeof(anim_param_t));

          mtos_messageq_ack(p_priv->msgq_id);
          break;

        case PIC_REND_ANIM_PAUSE:
          if(p_priv->sm == PIC_REND_SM_ANIM)
          {
            p_priv->sm = PIC_REND_SM_ANIM_PAUSE;
          }

          mtos_messageq_ack(p_priv->msgq_id);
          break;

        case PIC_REND_ANIM_RESUME:
          if(p_priv->sm == PIC_REND_SM_ANIM_PAUSE)
          {
            p_priv->sm = PIC_REND_SM_ANIM;
          }   

          mtos_messageq_ack(p_priv->msgq_id);
          break;

        default:
          break;
      }
    }
    else
    {
      switch(p_priv->sm)
      {
        case PIC_REND_SM_IDLE:
          break;
          
        case PIC_REND_SM_ANIM:
          if(p_priv->anim_param.media_type != MF_COLOR 
                  && p_priv->anim_param.media_type != MF_IMAGE
                  && p_priv->anim_param.media_type != MF_JPEG
                  && p_priv->anim_param.media_type != MF_BMP
                  && p_priv->anim_param.media_type != MF_PNG)
          {
            MT_ASSERT(0);
          }

          curn_ticks = mtos_ticks_get();
          if(curn_ticks - p_priv->anim_param.last_ticks >= p_priv->anim_param.step_time)
          {
            p_priv->anim_param.last_ticks = curn_ticks;
            if(!pic_render_animation_step(p_priv))
            {
              p_priv->sm = PIC_REND_SM_IDLE;

              pic_rend_anim_release(p_priv);
          
              switch(p_priv->anim)
              {
                case REND_ANIM_MOS:
                  _mosaic_deinit(p_priv);
                  break;

                default:

                  break;
              }

              memset(&p_priv->anim_param, 0, sizeof(anim_param_t));              
            }
          }
          break;

        default:
          break;
      }
    }
  }
}


static RET_CODE on_open(handle_t _this)
{
  //pic_render_pin_private_t *p_priv = get_priv(_this);

  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  //media_sample_t *p_sample = (media_sample_t *)p_priv->lock_sample;
  MT_ASSERT(p_priv != NULL);
/*
*/
  pic_rend_stop_anim((pic_render_pin_t *)p_priv->p_this);
  return SUCCESS;
}

static void on_destroy(handle_t _this)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);

  if(p_priv->priority != 0)
  {
    pic_rend_stop_anim((pic_render_pin_t *)p_priv->p_this);
    
    mtos_task_delete(p_priv->priority);

    mtos_messageq_release(p_priv->msgq_id);
  }

  if(p_priv->p_stack != NULL)
  {
    mtos_free(p_priv->p_stack);
  } 

  mtos_sem_destroy(&p_priv->rend_sem, 0);

  pic_rend_anim_release(p_priv);
}

static void image_render(handle_t _this, void *p_data, rend_param_t *p_param)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  gpe_param_vsb_t gpe_param = {0};
  u32 pitch = 0, in_size = 0;
  void *p_gpe = NULL;
  rect_t draw_rect = {0};
  u32 rend_w = 0, rend_h = 0;
  u32 fill_w = 0, fill_h = 0;
  u32 h_cnt = 0, v_cnt = 0, h = 0, v = 0;
  u32 bpp = 0;
  void *p_rgn = NULL;
  image_t img = {0};
  RET_CODE ret = SUCCESS;
  
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_priv != NULL);
  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_param != NULL);
  MT_ASSERT(RECTW(p_param->fill_rect) <= p_param->image_width);
  MT_ASSERT(RECTH(p_param->fill_rect) <= p_param->image_height);

  if(p_param->p_rgn != 0)
  {
    p_rgn = p_param->p_rgn;
  }
  else
  {
    MT_ASSERT(p_priv->p_rgn != NULL);

    p_rgn = p_priv->p_rgn;
  }

  switch(p_param->fmt)
  {
    case PIX_FMT_RGBPALETTE8:
    case PIX_FMT_YUVPALETTE8:
      bpp = 8;
      break;

    case PIX_FMT_RGBPALETTE4:
    case PIX_FMT_YUVPALETTE4:
      bpp = 4;
      break;

    case PIX_FMT_RGBPALETTE2:
    case PIX_FMT_YUVPALETTE2:
      bpp = 2;
      break;

    case PIX_FMT_RGBPALETTE1:
    case PIX_FMT_YUVPALETTE1:
      bpp = 1;
      break;

    case PIX_FMT_ARGB1555:
    case PIX_FMT_RGB565:
      bpp = 16;
      break;

    case PIX_FMT_ARGB8888:
    case PIX_FMT_AYCBCR8888:
      bpp = 32;
      break;

    default:
      MT_ASSERT(0);
  }
  pitch = p_param->pitch;
  in_size = p_param->image_height * pitch;

  rend_w = RECTW(p_param->rend_rect);
  rend_h = RECTH(p_param->rend_rect);
  fill_w = RECTW(p_param->fill_rect);
  fill_h = RECTH(p_param->fill_rect);

  MT_ASSERT(fill_w <= rend_w);
  MT_ASSERT(fill_h <= rend_h);

  gpe_param.enable_colorkey = p_param->enable_ckey;
  gpe_param.colorkey = p_param->color_key;

  if(p_param->p_alpha != NULL)
  {
    gpe_param.cmd = GPE_CMD_ALPHA_MAP;
    gpe_param.p_alpha = p_param->p_alpha;
    gpe_param.alpha_pitch = p_param->alpha_pitch;
  }

  if(p_param->use_bg_color)
  {
    gpe_draw_rectangle_vsb(p_gpe, p_priv->p_rgn, &p_priv->win_rect, p_param->bg_color);
  }

  switch(p_priv->style)
  {
    case REND_STYLE_CENTER:
      //caculate the render coordinates.
      draw_rect.left = p_priv->win_rect.left + p_param->rend_rect.left + (rend_w - fill_w) / 2;
      draw_rect.right = draw_rect.left + fill_w;

      draw_rect.top = p_priv->win_rect.top + p_param->rend_rect.top + (rend_h - fill_h) / 2;
      draw_rect.bottom = draw_rect.top + fill_h;

      //draw image
      gpe_draw_image_vsb(p_gpe, p_rgn, &draw_rect, p_data, p_param->pal_entry,
        p_param->pal_num, pitch, in_size, p_param->fmt, &gpe_param, &p_param->fill_rect);
      break;

    case REND_STYLE_TILE:
      h_cnt = rend_w / fill_w + (rend_w % fill_w ? 1 : 0);
      v_cnt = rend_h / fill_h + (rend_h % fill_h ? 1 : 0);

      for(h = 0; h < h_cnt; h++)
      {

        draw_rect.left = p_priv->win_rect.left + p_param->rend_rect.left + h * fill_w;
        draw_rect.right = draw_rect.left + fill_w;

        for(v = 0; v < v_cnt; v++)
        {
          draw_rect.top = p_priv->win_rect.top + p_param->rend_rect.top + v * fill_h;
          draw_rect.bottom = draw_rect.top + fill_h;

          gpe_draw_image_vsb(p_gpe, p_rgn, &draw_rect, p_data, NULL, 0, pitch,
            in_size, p_param->fmt, &gpe_param, &p_param->fill_rect);
        }
      }
      break;

    case REND_STYLE_STRECH:
      draw_rect.left = p_priv->win_rect.left + p_param->rend_rect.left;
      draw_rect.right = draw_rect.left + rend_w;
      draw_rect.top = p_priv->win_rect.top + p_param->rend_rect.top;
      draw_rect.bottom = draw_rect.top + rend_h;

      img.pitch = p_param->pitch;
      img.width = p_param->image_width;
      img.height = p_param->image_height;
      img.pix_fmt = p_param->fmt;
      if(p_param->pal_entry != NULL)
      {
        img.b_palette = TRUE;
        img.p_palette = p_param->pal_entry;
        img.pal_buf_sz = p_param->pal_num * sizeof(u32);
        img.entry_cnt = p_param->pal_num;
      } 
      img.alpha = 0;
      img.b_alpha = 0;
      img.p_buf_addr = p_data;
      img.buf_sz = in_size;
      img.little_endian = TRUE;
      
      ret = gpe_draw_image_scale(p_gpe, &img, &p_param->fill_rect, p_rgn, &draw_rect);
      MT_ASSERT(ret == SUCCESS);
      break;

    default:
      break;
  }
}

static void color_render(handle_t _this, u32 color, rend_param_t *p_param)
{
  pic_render_pin_private_t *p_priv = get_priv(_this);
  void *p_gpe = NULL;
  rect_t draw_rect = {0};
  u32 rend_w = 0, rend_h = 0;
  void *p_rgn = NULL;

  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);

  MT_ASSERT(p_gpe != NULL);
  MT_ASSERT(p_param != NULL);

  if(p_param->p_rgn != 0)
  {
    p_rgn = p_param->p_rgn;
  }
  else
  {
    MT_ASSERT(p_priv->p_rgn != NULL);

    p_rgn = p_priv->p_rgn;
  }

  rend_w = RECTW(p_param->rend_rect);
  rend_h = RECTH(p_param->rend_rect);

  draw_rect.left = p_priv->win_rect.left + p_param->rend_rect.left;
  draw_rect.right = draw_rect.left + rend_w;
  draw_rect.top = p_priv->win_rect.top + p_param->rend_rect.top;
  draw_rect.bottom = draw_rect.top + rend_h;
  if(p_param->pal_entry != NULL && p_param->pal_num != 0)
  {
    region_set_palette(p_rgn, (u32 *)p_param->pal_entry, p_param->pal_num);
  }

  //draw bg color.
  if(p_param->use_bg_color)
  {
    gpe_draw_rectangle_vsb(p_gpe, p_priv->p_rgn, &p_priv->win_rect, p_param->bg_color);
  }

  //draw rectangle.
  gpe_draw_rectangle_vsb(p_gpe, p_rgn, &draw_rect, color);
}

static void on_receive(handle_t _this, media_sample_t *p_sample)
{
  ipin_t *p_ipin = (ipin_t *)_this;
  pic_render_pin_private_t *p_priv = get_priv(_this);
  rend_param_t param = {0};
  os_msg_t msg = {0};
  void *p_disp = NULL, *p_rgn = NULL;
  void *p_data = NULL;
  u32 fill_w = 0, fill_h = 0;
  u32 rend_w = 0, rend_h = 0;
  u32 wind_w = 0, wind_h = 0;
  u32 size = 0;
  BOOL is_draw_end = FALSE;

  log_perf(LOG_PIC_RENDER_FILTER, PIC_RENDER_FILTER_START_TICKS,
  (u32)(p_sample->format.media_type), 0);
  memcpy(&param, p_sample->p_data + p_sample->data_offset, sizeof(rend_param_t));

  if(p_priv->is_show == FALSE)
  {
    OS_PRINTF("\n\npic render filter not show !!!\n\n");
    return;
  }

  if(p_sample->format.stream_type != MT_IMAGE)
  {
    return;
  }

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp != NULL);

  mtos_sem_take(&p_priv->rend_sem, 0);

  if(param.p_rgn != NULL)
  {
    p_rgn = param.p_rgn;
  }
  else
  {
    MT_ASSERT(p_priv->p_rgn != NULL);

    p_rgn = p_priv->p_rgn;
  }

  if(((region_t *)p_rgn)->pix_fmt == PIX_FMT_CBY0CRY18888)
  {
    param.bg_color = 0x80108010;
  }  

  if((param.pal_entry != NULL) && (param.pal_num != 0))
  {
    region_set_palette(p_rgn, param.pal_entry, param.pal_num);
  }

  if(p_sample->format.media_type == MF_CMD)
  {
    #ifndef WIN32  
    disp_layer_update_region(p_disp, p_rgn, NULL);
    #endif
    mtos_sem_give(&p_priv->rend_sem);
    
    return;
  }

  fill_w = RECTW(param.fill_rect);
  fill_h = RECTH(param.fill_rect);
  rend_w = RECTW(param.rend_rect);
  rend_h = RECTH(param.rend_rect);
  wind_w = RECTW(p_priv->win_rect);
  wind_h = RECTH(p_priv->win_rect);

  MT_ASSERT(rend_w <= wind_w);
  MT_ASSERT(rend_h <= wind_h);

  //to make sure fill rectangle is smaller than rend rectangle.
  if(fill_w > rend_w)
  {
    param.fill_rect.left += (fill_w - rend_w) / 2;
    param.fill_rect.right = param.fill_rect.left + rend_w;
  }

  if(fill_h > rend_h)
  {
    param.fill_rect.top += (fill_h - rend_h) / 2;
    param.fill_rect.bottom = param.fill_rect.top + rend_h;
  }

  if(param.p_data != NULL)
  {
    p_data = param.p_data;
  }
  else
  {
    p_data = p_sample->p_data + p_sample->data_offset + sizeof(rend_param_t);
  }

  size = param.pitch * param.image_height;

  if(p_priv->anim != REND_ANIM_NONE)
  {
    switch(p_sample->format.media_type)
    {
      case MF_IMAGE:
      case MF_JPEG:
      case MF_BMP:
      case MF_PNG:
        //save the image data & media type.
        p_priv->anim_param.media_type = p_sample->format.media_type;

        memcpy(&p_priv->anim_param.rend_rect, &param.rend_rect, sizeof(rect_t));
        memcpy(&p_priv->anim_param.fill_rect, &param.fill_rect, sizeof(rect_t));
        p_priv->anim_param.image_height = param.image_height;
        p_priv->anim_param.image_width = param.image_width;
        p_priv->anim_param.fmt = param.fmt;
        p_priv->anim_param.pitch = param.pitch;
        p_priv->anim_param.pal_entry = param.pal_entry;
        p_priv->anim_param.pal_num = param.pal_num;
        p_priv->anim_param.use_bg_color = param.use_bg_color;
        p_priv->anim_param.bg_color = param.bg_color;
        p_priv->anim_param.enable_ckey = param.enable_ckey;
        p_priv->anim_param.color_key = param.color_key;
        p_priv->anim_param.p_rgn = p_rgn;

        p_priv->anim_param.step_time = 100 / p_priv->frame_rate;

        p_priv->anim_param.curn_frame = 0;

        if(pic_rend_anim_init(p_priv, p_data))
        {
          pic_rend_start_animation(_this);

          is_draw_end = FALSE;
        }
        else
        {
          //no enough for animation, so just show it.
          image_render(_this, p_data, &param);

          is_draw_end = TRUE;
        }
        break;

      case MF_COLOR:
        //save the color & media type
        p_priv->anim_param.media_type = p_sample->format.media_type;

        memcpy(&p_priv->anim_param.rend_rect, &param.rend_rect, sizeof(rect_t));
        memcpy(&p_priv->anim_param.fill_rect, &param.fill_rect, sizeof(rect_t));
        p_priv->anim_param.image_height = param.image_height;
        p_priv->anim_param.image_width = param.image_width;
        p_priv->anim_param.fmt = param.fmt;
        p_priv->anim_param.pitch = param.pitch;
        p_priv->anim_param.pal_entry = param.pal_entry;
        p_priv->anim_param.pal_num = param.pal_num;
        p_priv->anim_param.use_bg_color = param.use_bg_color;
        p_priv->anim_param.bg_color = param.bg_color;
        p_priv->anim_param.enable_ckey = param.enable_ckey;
        p_priv->anim_param.color_key = param.color_key;
        p_priv->anim_param.p_rgn = p_rgn;

        p_priv->anim_param.step_time = 100 / p_priv->frame_rate;

        p_priv->anim_param.curn_frame = 0;

        if(pic_rend_anim_init(p_priv, p_data))
        {
          pic_rend_start_animation(_this);

          is_draw_end = FALSE;
        }
        else
        {
          //no enough for animation, so just show it.
          color_render(_this, *((u32 *)p_data), &param);

          is_draw_end = TRUE;
        }
        break;

      case MF_GIF:
        image_render(_this, p_data, &param);

        if(param.is_last)
        {
          is_draw_end = TRUE;
        }
        else
        {
          is_draw_end = FALSE;
        }
        break;

      default:
        MT_ASSERT(0);
        break;
    }
  }
  else
  {
    switch(p_sample->format.media_type)
    {
      case MF_IMAGE:
      case MF_JPEG:
      case MF_BMP:
      case MF_PNG:
        image_render(_this, p_data, &param);

        is_draw_end = TRUE;
        break;

      case MF_COLOR:
        color_render(_this, *((u32 *)p_data), &param);

        is_draw_end = TRUE;
        break;
        
      case MF_GIF:
        image_render(_this, p_data, &param);

        if(param.is_last)
        {
          is_draw_end = TRUE;
        }
        else
        {
          is_draw_end = FALSE;
        }
        break;
        
      default:
        MT_ASSERT(0);
        break;
    }

    #ifndef WIN32
    if(param.not_update == FALSE)
    {
      disp_layer_update_region(p_disp, p_rgn, NULL);
    }
    #endif
  }

  if(is_draw_end)
  {
    log_perf(LOG_PIC_RENDER_FILTER,
      PIC_RENDER_FILTER_STOP_TICKS, (u32)(p_sample->format.media_type), 0);
      
    msg.content = PIC_DRAW_END;
    msg.para1 = p_priv->handle;
    p_ipin->send_message_out(p_ipin, &msg);
  }

  mtos_sem_give(&p_priv->rend_sem);
}

pic_render_pin_t * pic_render_pin_create(
  pic_render_pin_t *p_pin, interface_t *p_owner, pic_rend_para_t *p_para)
{
  pic_render_pin_private_t *p_priv = NULL;
  pic_render_pin_t *p_ins = p_pin;
  base_input_pin_t *p_input_pin = NULL;
  sink_pin_para_t sink_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t format = {0};
  BOOL ret_boo = FALSE;

  //check input parameter
  MT_ASSERT(p_ins != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
  sink_pin_para.p_filter = p_owner;
  sink_pin_para.p_name = "pic_rend_pin";
  sink_pin_create(&p_ins->base_pin, &sink_pin_para);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(pic_render_pin_private_t));
  p_priv->p_this = p_ins;

  p_priv->sm = PIC_REND_SM_IDLE;

  //add support media format.
  format.stream_type = MT_IMAGE;
  format.media_type = MF_IMAGE;
  p_ipin = (ipin_t *)p_ins;
  p_ipin->add_supported_media_format(p_ipin, &format);

  format.stream_type = MT_IMAGE;
  format.media_type = MF_COLOR;
  p_ipin->add_supported_media_format(p_ipin, &format);

  format.stream_type = MT_IMAGE;
  format.media_type = MF_JPEG;
  p_ipin->add_supported_media_format(p_ipin, &format);

  format.stream_type = MT_IMAGE;
  format.media_type = MF_GIF;
  p_ipin->add_supported_media_format(p_ipin, &format);

  format.stream_type = MT_IMAGE;
  format.media_type = MF_BMP;
  p_ipin->add_supported_media_format(p_ipin, &format);

  format.stream_type = MT_IMAGE;
  format.media_type = MF_PNG;
  p_ipin->add_supported_media_format(p_ipin, &format);  

  format.stream_type = MT_IMAGE;
  format.media_type = MF_CMD;
  p_ipin->add_supported_media_format(p_ipin, &format);  

#ifndef WIN32
  p_priv->chip_ic = hal_get_chip_ic_id();
#else
  p_priv->chip_ic = IC_MAGIC;
#endif  

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_ins;

  p_input_pin->on_receive = on_receive;
  p_ipin->_interface.on_destroy = on_destroy;
  p_ipin->on_open = on_open;
  p_ipin->on_close = on_close;

  p_priv->p_stack = NULL;
  p_priv->msgq_id = 0xFFFFFFFF;

  p_priv->is_show = TRUE;

  MT_ASSERT(sizeof(rend_param_t) % 8 == 0);

  if((p_para != NULL) && (p_para->task_priority != 0))
  {
    //should be created in filter create function.
    p_priv->p_stack = mtos_malloc(p_para->stack_size);
    MT_ASSERT(p_priv->p_stack != NULL);
    
    p_priv->priority = p_para->task_priority;

    p_priv->msgq_id = mtos_messageq_create(RENDER_TASK_MSGQ_DEPTH, (u8 *)"pic_render");

    ret_boo = mtos_task_create((u8 *)"pic_render",
                   pic_render_task_entry,
                   (void *)p_priv,
                   p_priv->priority,
                   p_priv->p_stack,
                   p_para->stack_size);

    MT_ASSERT(ret_boo == TRUE);

  }

  mtos_sem_create(&p_priv->rend_sem, TRUE);

  return p_ins;
}

