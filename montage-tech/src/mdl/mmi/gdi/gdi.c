/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "common.h"
#include "osd.h"
#include "gpe.h"
#include "gpe_vsb.h"

#include "mdl.h"
#include "mmi.h"
#include "surface.h"
#include "flinger.h"
#include "gdi.h"
#include "gdi_dc.h"
#include "gdi_anim.h"

#include "gui_resource.h"
#include "gdi_private.h"


RET_CODE dc_enter_drawing(dc_t *p_dc)
{
  if(!intersect_rect(&p_dc->rc_output, &p_dc->rc_output, &p_dc->ecrgn.bound))
  {
    return ERR_FAILURE;
  }

  if(dc_is_virtual_dc(p_dc))
  {
    if(!is_rect_intersected(&p_dc->vtrl_rc, &p_dc->rc_output))
    {
      return ERR_FAILURE;
    }
  }

  return SUCCESS;
}


void dc_leave_drawing(dc_t *p_dc)
{

}

static void init_screen_dc(void)
{
  gdi_main_t *p_gdi_info = NULL;
  rect_t *p_flinger_rect = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    p_gdi_info->top.screen_dc.type = TYPE_SCREEN;

    p_gdi_info->top.screen_dc.p_curn_flinger = p_gdi_info->top.p_flinger;
    p_gdi_info->top.screen_dc.p_back_flinger = NULL;

    gdi_init_cliprgn(&p_gdi_info->top.screen_dc.lcrgn, &p_gdi_info->cliprc_heap);
    gdi_init_cliprgn(&p_gdi_info->top.screen_dc.ecrgn, &p_gdi_info->cliprc_heap);

    p_gdi_info->top.screen_dc.p_gcrgn_info = NULL;

    p_flinger_rect = flinger_get_rect(p_gdi_info->top.p_flinger);
    
    p_gdi_info->top.screen_dc.dev_rc.left = p_gdi_info->top.screen_dc.dev_rc.top = 0;
    p_gdi_info->top.screen_dc.dev_rc.right = RECTWP(p_flinger_rect);
    p_gdi_info->top.screen_dc.dev_rc.bottom = RECTHP(p_flinger_rect);

    gdi_set_cliprgn(&p_gdi_info->top.screen_dc.lcrgn, &p_gdi_info->top.screen_dc.dev_rc);
    gdi_set_cliprgn(&p_gdi_info->top.screen_dc.ecrgn, &p_gdi_info->top.screen_dc.dev_rc);
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    p_gdi_info->bot.screen_dc.type = TYPE_SCREEN;

    p_gdi_info->bot.screen_dc.p_curn_flinger = p_gdi_info->bot.p_flinger;
    p_gdi_info->bot.screen_dc.p_back_flinger = NULL;

    gdi_init_cliprgn(&p_gdi_info->bot.screen_dc.lcrgn, &p_gdi_info->cliprc_heap);
    gdi_init_cliprgn(&p_gdi_info->bot.screen_dc.ecrgn, &p_gdi_info->cliprc_heap);

    p_gdi_info->bot.screen_dc.p_gcrgn_info = NULL;

    p_flinger_rect = flinger_get_rect(p_gdi_info->bot.p_flinger);

    p_gdi_info->bot.screen_dc.dev_rc.left = p_gdi_info->bot.screen_dc.dev_rc.top = 0;
    p_gdi_info->bot.screen_dc.dev_rc.right = RECTWP(p_flinger_rect);
    p_gdi_info->bot.screen_dc.dev_rc.bottom = RECTHP(p_flinger_rect);

    gdi_set_cliprgn(&p_gdi_info->bot.screen_dc.lcrgn, &p_gdi_info->bot.screen_dc.dev_rc);
    gdi_set_cliprgn(&p_gdi_info->bot.screen_dc.ecrgn, &p_gdi_info->bot.screen_dc.dev_rc);
  } 
}

static void dc_init_cliprgn(void)
{
  int i = 0;
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  for(i = 0; i < p_gdi_info->dc_slot_cnt; i++)
  {
    gdi_init_cliprgn(&p_gdi_info->p_dc_slot[i].lcrgn, &p_gdi_info->cliprc_heap);

    p_gdi_info->p_dc_slot[i].p_gcrgn_info = NULL;

    gdi_init_cliprgn(&p_gdi_info->p_dc_slot[i].ecrgn, &p_gdi_info->cliprc_heap);
  }
}

static void dc_init(dc_t *p_dc, rect_t *p_rc, crgn_info_t *p_gcrgn_info)
{
  if(dc_is_general_dc(p_dc))
  {
    p_dc->p_gcrgn_info = p_gcrgn_info;

    LOCK_GCRINFO(p_dc);

    if(p_dc->p_gcrgn_info != NULL)
    {
      gdi_copy_cliprgn(&p_dc->ecrgn, &p_dc->p_gcrgn_info->crgn);
    }
    else
    {
      gdi_set_cliprgn(&p_dc->ecrgn, p_rc);
    }

    p_dc->dev_rc = *p_rc;

    gdi_intersect_cliprect(&p_dc->ecrgn, p_rc);

    UNLOCK_GCRINFO(p_dc);
  }
  else if(dc_is_mem_dc(p_dc))
  {
    p_dc->p_gcrgn_info = p_gcrgn_info;

    LOCK_GCRINFO(p_dc);

    gdi_set_cliprgn(&p_dc->ecrgn, p_rc);
    p_dc->dev_rc = *p_rc;

    UNLOCK_GCRINFO(p_dc);
  }
}


// this function generates effective clip region from
// local clip region and global clip region.
// if the global clip region has a new age,
// this function empty effective clip region first,
// and then intersect local clip region and global clip region.

BOOL dc_generate_ecrgn(dc_t *p_dc, BOOL is_is_force)
{
  cliprc_t *p_cr = NULL;

  // is global clip region is empty?
  if((!is_is_force) && (!dc_is_visible(p_dc)))
  {
    return FALSE;
  }

  // need regenerate?
  if(is_is_force)
  {
    /* copy local clipping region to effective clipping region. */
    gdi_copy_cliprgn(&p_dc->ecrgn, &p_dc->lcrgn);

    /* transfer device coordinates to screen coordinates. */
    p_cr = p_dc->ecrgn.p_head;
    while(NULL != p_cr)
    {
      coor_dp2sp(p_dc, &p_cr->rc.left, &p_cr->rc.top);
      coor_dp2sp(p_dc, &p_cr->rc.right, &p_cr->rc.bottom);

      p_cr = p_cr->p_next;
    }
    coor_dp2sp(p_dc, &p_dc->ecrgn.bound.left, &p_dc->ecrgn.bound.top);
    coor_dp2sp(p_dc, &p_dc->ecrgn.bound.right, &p_dc->ecrgn.bound.bottom);


    /* intersect with global clipping region. */
    if(p_dc->lcrgn.p_head == NULL)
    {
      if(p_dc->p_gcrgn_info != NULL)
      {
        gdi_copy_cliprgn(&p_dc->ecrgn, &p_dc->p_gcrgn_info->crgn);
      }
      else
      {
        gdi_set_cliprgn(&p_dc->ecrgn, &p_dc->dev_rc);
      }
    }
    else
    {
      if(p_dc->p_gcrgn_info != NULL)
      {
        gdi_intersect_cliprgn(
          &p_dc->ecrgn, &p_dc->ecrgn, &p_dc->p_gcrgn_info->crgn);
      }
    }

    /*
      * update p_dc->dev_rc, and restrict the effective
      * clipping region more with p_dc->dev_rc.
      */
    gdi_intersect_cliprect(&p_dc->ecrgn, &p_dc->dev_rc);
  }

  return TRUE;
}


void dc_reset_lcrgn(dc_t *p_dc)
{
  if(p_dc->lcrgn.p_head == NULL)
  {
    rect_t rc;
    set_rect(&rc, 0, 0, 0x7FFF, 0x7FFF);
    gdi_set_cliprgn(&p_dc->lcrgn, &rc);
  }
}

BOOL gdi_init(gdi_cfg_t *p_config)
{
  u32 slice = 0;
  u32 size = 0;
  class_handle_t p_gdi_handle = NULL;
  gdi_main_t *p_gdi_info = NULL;
  flinger_param_t param = {0};

  p_gdi_handle = (void *)mmi_alloc_buf(sizeof(gdi_main_t));
  memset((void *)p_gdi_handle, 0, sizeof(gdi_main_t));
  class_register(GDI_CLASS_ID, p_gdi_handle);

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);

  slice = sizeof(cliprc_t);
  size = slice * p_config->max_cliprect_cnt;
  p_gdi_info->p_cliprc_heap_addr = 
    mmi_create_memf(&p_gdi_info->cliprc_heap, p_config->max_cliprect_cnt, slice);
  MT_ASSERT(p_gdi_info->p_cliprc_heap_addr != NULL);

  size = sizeof(dc_t) * p_config->max_dc_cnt;
  p_gdi_info->p_dc_slot = (dc_t *)mmi_alloc_buf(size);
  MT_ASSERT(p_gdi_info->p_dc_slot != NULL);
  memset(p_gdi_info->p_dc_slot, 0, size);
  p_gdi_info->dc_slot_cnt = p_config->max_dc_cnt;

  dc_init_cliprgn();

  MT_ASSERT(p_config->p_screen_rect != NULL);

  copy_rect(&p_gdi_info->screen_rect, p_config->p_screen_rect);

  //create top flinger.
  if(p_config->p_top != NULL)
  {
    param.cdef = p_config->cdef;
    param.ckey = p_config->ckey;
    param.format = p_config->format;
    param.p_pal = p_config->p_pal;
    param.p_cfg = p_config->p_top;
    param.p_flinger_rect = p_config->p_screen_rect;
    
    p_gdi_info->top.p_flinger = flinger_create(&param);
    MT_ASSERT(p_gdi_info->top.p_flinger != NULL);

    p_gdi_info->top.layer = p_config->p_top->layer;
  }

  //create bottom flinger.
  if(p_config->p_bot != NULL)
  {
    param.cdef = p_config->cdef;
    param.ckey = p_config->ckey;
    param.format = p_config->format;
    param.p_pal = p_config->p_pal;
    param.p_cfg = p_config->p_bot;
    param.p_flinger_rect = p_config->p_screen_rect;
    
    p_gdi_info->bot.p_flinger = flinger_create(&param);
    MT_ASSERT(p_gdi_info->bot.p_flinger != NULL);

    p_gdi_info->bot.layer = p_config->p_bot->layer;
  }  

  if(p_config->p_pal != NULL)
  {
    p_gdi_info->global_pal.cnt = p_config->p_pal->cnt;
    p_gdi_info->global_pal.p_entry = p_config->p_pal->p_entry;
  }
  
  p_gdi_info->screen_format = p_config->format;
  p_gdi_info->screen_cdef = p_config->cdef;
  p_gdi_info->screen_ckey = p_config->ckey;    

  init_screen_dc();

  gdi_init_vsurf(p_config->vsurf_buf_addr, p_config->vsurf_buf_size);

  gdi_init_anim(p_config->anim_addr, p_config->anim_size);
    
  return TRUE;
}

void gdi_release(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  gdi_release_vsurf();

  gdi_release_anim();

  mmi_free_buf(p_gdi_info->p_dc_slot);

  mmi_destroy_memf(&p_gdi_info->cliprc_heap, p_gdi_info->p_cliprc_heap_addr);

  p_gdi_info->p_cliprc_heap_addr = NULL;

  mmi_free_buf((void *)p_gdi_info);
}

BOOL gdi_reset_screen(screen_reset_t *p_scr_reset)
{
  gdi_main_t *p_gdi_info = NULL;
  flinger_param_t param = {0};

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  p_gdi_info->screen_format = p_scr_reset->format;

  p_gdi_info->screen_cdef = p_scr_reset->cdef;
  p_gdi_info->screen_ckey = p_scr_reset->ckey;

  if(p_scr_reset->p_pal != NULL)
  {
    p_gdi_info->global_pal.cnt = p_scr_reset->p_pal->cnt;
    p_gdi_info->global_pal.p_entry = p_scr_reset->p_pal->p_entry;
  }      

  //delete original flinger
  if(p_gdi_info->top.p_flinger != NULL)
  {
    flinger_enable(p_gdi_info->top.p_flinger, FALSE);
    flinger_set_display(p_gdi_info->top.p_flinger, FALSE);
    flinger_delete(p_gdi_info->top.p_flinger);
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    flinger_enable(p_gdi_info->bot.p_flinger, FALSE);
    flinger_set_display(p_gdi_info->bot.p_flinger, FALSE);
    flinger_delete(p_gdi_info->bot.p_flinger);
  }

  mtos_task_delay_ms(50);

  MT_ASSERT(p_scr_reset->p_screen_rect != NULL);

  copy_rect(&p_gdi_info->screen_rect, p_scr_reset->p_screen_rect);

  //create top flinger.
  if(p_scr_reset->p_top != NULL)
  {
    param.cdef = p_scr_reset->cdef;
    param.ckey = p_scr_reset->ckey;
    param.format = p_scr_reset->format;
    param.p_pal = p_scr_reset->p_pal;
    param.p_cfg = p_scr_reset->p_top;
    param.p_flinger_rect = p_scr_reset->p_screen_rect;
    
    p_gdi_info->top.p_flinger = flinger_create(&param);
    MT_ASSERT(p_gdi_info->top.p_flinger != NULL);
    
    p_gdi_info->top.layer = p_scr_reset->p_top->layer;

    gdi_empty_cliprgn(&p_gdi_info->top.screen_dc.lcrgn);
    gdi_empty_cliprgn(&p_gdi_info->top.screen_dc.ecrgn);  

    flinger_set_alpha(p_gdi_info->top.p_flinger, 0xFF);
  }

  //create bottom flinger.
  if(p_scr_reset->p_bot != NULL)
  {
    param.cdef = p_scr_reset->cdef;
    param.ckey = p_scr_reset->ckey;
    param.format = p_scr_reset->format;
    param.p_pal = p_scr_reset->p_pal;
    param.p_cfg = p_scr_reset->p_bot;
    param.p_flinger_rect = p_scr_reset->p_screen_rect;
    
    p_gdi_info->bot.p_flinger = flinger_create(&param);
    MT_ASSERT(p_gdi_info->bot.p_flinger != NULL);

    p_gdi_info->bot.layer = p_scr_reset->p_bot->layer;

    gdi_empty_cliprgn(&p_gdi_info->bot.screen_dc.lcrgn);
    gdi_empty_cliprgn(&p_gdi_info->bot.screen_dc.ecrgn);

    flinger_set_alpha(p_gdi_info->bot.p_flinger, 0xFF);
  }
  
  //init screen dc
  init_screen_dc();

  return TRUE;
}

BOOL gdi_reset_screen_and_scale(screen_reset_t *p_scr_reset, void (*reset_cb)(void *))
{
  gdi_main_t *p_gdi_info = NULL;
  flinger_param_t param = {0};

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  p_gdi_info->screen_format = p_scr_reset->format;

  p_gdi_info->screen_cdef = p_scr_reset->cdef;
  p_gdi_info->screen_ckey = p_scr_reset->ckey;

  if(p_scr_reset->p_pal != NULL)
  {
    p_gdi_info->global_pal.cnt = p_scr_reset->p_pal->cnt;
    p_gdi_info->global_pal.p_entry = p_scr_reset->p_pal->p_entry;
  }      

  //delete original flinger
  if(p_gdi_info->top.p_flinger != NULL)
  {
    flinger_enable(p_gdi_info->top.p_flinger, FALSE);
    flinger_set_display(p_gdi_info->top.p_flinger, FALSE);
    flinger_delete(p_gdi_info->top.p_flinger);
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    flinger_enable(p_gdi_info->bot.p_flinger, FALSE);
    flinger_set_display(p_gdi_info->bot.p_flinger, FALSE);
    flinger_delete(p_gdi_info->bot.p_flinger);
  }
  
  mtos_task_delay_ms(50);

  if(reset_cb != NULL)
  {
    reset_cb((void *)(p_scr_reset->p_screen_rect));
  }

  //create top flinger.
  if(p_scr_reset->p_top != NULL)
  {
    param.cdef = p_scr_reset->cdef;
    param.ckey = p_scr_reset->ckey;
    param.format = p_scr_reset->format;
    param.p_pal = p_scr_reset->p_pal;
    param.p_cfg = p_scr_reset->p_top;
    param.p_flinger_rect = p_scr_reset->p_screen_rect;
    
    p_gdi_info->top.p_flinger = flinger_create(&param);
    MT_ASSERT(p_gdi_info->top.p_flinger != NULL);
    
    p_gdi_info->top.layer = p_scr_reset->p_top->layer;

    gdi_empty_cliprgn(&p_gdi_info->top.screen_dc.lcrgn);
    gdi_empty_cliprgn(&p_gdi_info->top.screen_dc.ecrgn);    
  }

  //create bottom flinger.
  if(p_scr_reset->p_bot != NULL)
  {
    param.cdef = p_scr_reset->cdef;
    param.ckey = p_scr_reset->ckey;
    param.format = p_scr_reset->format;
    param.p_pal = p_scr_reset->p_pal;
    param.p_cfg = p_scr_reset->p_bot;
    param.p_flinger_rect = p_scr_reset->p_screen_rect;
    
    p_gdi_info->bot.p_flinger = flinger_create(&param);
    MT_ASSERT(p_gdi_info->bot.p_flinger != NULL);

    p_gdi_info->bot.layer = p_scr_reset->p_bot->layer;

    gdi_empty_cliprgn(&p_gdi_info->bot.screen_dc.lcrgn);
    gdi_empty_cliprgn(&p_gdi_info->bot.screen_dc.ecrgn);
  }

  //init screen dc
  init_screen_dc();

  return TRUE;
}


void gdi_release_screen(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);  

  if(p_gdi_info->top.p_flinger != NULL)
  {
    flinger_delete(p_gdi_info->top.p_flinger);
    p_gdi_info->top.p_flinger = NULL;
    
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    flinger_delete(p_gdi_info->bot.p_flinger);
    p_gdi_info->bot.p_flinger = NULL;
  }
}

void gdi_clear_screen(void)
{
  rect_t orc = {0};
  gdi_main_t *p_gdi_info = NULL;
  rect_t *p_flinger_rect = NULL;
 
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    p_flinger_rect = flinger_get_rect(p_gdi_info->top.p_flinger);
    set_rect(&orc, 0, 0, RECTWP(p_flinger_rect), RECTHP(p_flinger_rect));
    flinger_set_cliprect(p_gdi_info->top.p_flinger, NULL);
    flinger_fill_rect(p_gdi_info->top.p_flinger, &orc, p_gdi_info->screen_cdef);
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    p_flinger_rect = flinger_get_rect(p_gdi_info->bot.p_flinger);
    set_rect(&orc, 0, 0, RECTWP(p_flinger_rect), RECTHP(p_flinger_rect));
    flinger_set_cliprect(p_gdi_info->bot.p_flinger, NULL);
    flinger_fill_rect(p_gdi_info->bot.p_flinger, &orc, p_gdi_info->screen_cdef);
  }
}


void gdi_clear_top_screen(void)
{
  rect_t orc = {0};
  gdi_main_t *p_gdi_info = NULL;
  rect_t *p_flinger_rect = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    p_flinger_rect = flinger_get_rect(p_gdi_info->top.p_flinger);
    set_rect(&orc, 0, 0, RECTWP(p_flinger_rect), RECTHP(p_flinger_rect));

    flinger_set_cliprect(p_gdi_info->top.p_flinger, NULL);
    flinger_fill_rect(p_gdi_info->top.p_flinger, &orc, p_gdi_info->screen_cdef);
  }
}

void gdi_reset_trans(void)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    flinger_set_trans(p_gdi_info->top.p_flinger, p_gdi_info->screen_cdef, 0);
  }

  if(p_gdi_info->top.p_flinger != NULL)
  {  
    flinger_set_trans(p_gdi_info->bot.p_flinger, p_gdi_info->screen_cdef, 0);
  }
}


lib_memf_t *gdi_get_cliprc_heap(void)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  return &p_gdi_info->cliprc_heap;
}

handle_t gdi_get_screen_handle(BOOL is_top_screen)
{
  gdi_main_t *p_gdi_info = NULL;
  handle_t screen_handle = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(is_top_screen)
  {
    MT_ASSERT(p_gdi_info->top.p_flinger != NULL);

    screen_handle = (handle_t)flinger_get_surface(p_gdi_info->top.p_flinger, 0);
  }
  else
  {
    MT_ASSERT(p_gdi_info->bot.p_flinger != NULL);

    screen_handle = (handle_t)flinger_get_surface(p_gdi_info->bot.p_flinger, 0);
  }
  
  return screen_handle;
}

void gdi_set_global_alpha(u8 alpha)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    flinger_set_alpha(p_gdi_info->top.p_flinger, alpha);
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    flinger_set_alpha(p_gdi_info->bot.p_flinger, alpha);  
  }
}

void gdi_set_enable(BOOL is_enable)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    flinger_set_display(p_gdi_info->top.p_flinger, is_enable);
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    flinger_set_display(p_gdi_info->bot.p_flinger, is_enable);
  }  
}


BOOL gdi_offset_screen(s16 x_off, s16 y_off)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    flinger_offset(p_gdi_info->top.p_flinger, x_off, y_off);
  }

  if(p_gdi_info->bot.p_flinger != NULL)
  {
    flinger_offset(p_gdi_info->bot.p_flinger, x_off, y_off);
  }

  return TRUE;
}


BOOL gdi_move_screen(s16 x, s16 y)
{
  rect_t *p_flinger_rect = NULL;
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  if(p_gdi_info->top.p_flinger != NULL)
  {
    p_flinger_rect = flinger_get_rect(p_gdi_info->top.p_flinger);
  
    flinger_offset(p_gdi_info->top.p_flinger,
      x - p_flinger_rect->left,
      y - p_flinger_rect->top);
  }
  
  if(p_gdi_info->bot.p_flinger != NULL)
  {
    p_flinger_rect = flinger_get_rect(p_gdi_info->bot.p_flinger);
    
    flinger_offset(p_gdi_info->bot.p_flinger,
      x - p_flinger_rect->left,
      y - p_flinger_rect->top);

    return TRUE;
  }
  
  return FALSE;
}

pix_type_t gdi_get_screen_pixel_type(void)
{
  gdi_main_t *p_gdi_info = NULL;
  pix_type_t screen_format = 0;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  screen_format = p_gdi_info->screen_format;
  
  return screen_format;
}


void gdi_get_screen_rect(rect_t *p_rc)
{
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);
  
  MT_ASSERT(p_rc != NULL);

  copy_rect(p_rc, &p_gdi_info->screen_rect);
}

hdc_t gdi_get_dc(BOOL is_top,
                  rect_t *p_src,
                 rect_t *p_vrc,
                 crgn_info_t *p_gcrgn_info,
                 hdc_t parent)
{
  int i = 0;
  dc_t *p_cdc = NULL;
  dc_t *p_pdc = dc_hdc2pdc(parent);
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  MT_ASSERT(p_gdi_info != NULL);

  for(i = 0; i < p_gdi_info->dc_slot_cnt; i++)
  {
    p_cdc = &p_gdi_info->p_dc_slot[i];

    if(!p_cdc->is_used)
    {
      p_cdc->is_used = TRUE;

      if(p_pdc == NULL)
      {
        p_cdc->type = TYPE_GENERAL;

        if(is_top && (p_gdi_info->top.p_flinger != NULL))
        {
          MT_ASSERT(p_gdi_info->top.p_flinger != NULL);
          
          p_cdc->p_curn_flinger = p_gdi_info->top.p_flinger;

          p_cdc->type |= TYPE_TOP;
        }
        else
        {
          MT_ASSERT(p_gdi_info->bot.p_flinger != NULL);
          
          p_cdc->p_curn_flinger = p_gdi_info->bot.p_flinger;

          p_cdc->type &= (~TYPE_TOP);
        }
        
        p_cdc->p_back_flinger = NULL;

        copy_rect(&p_cdc->vtrl_rc, p_vrc);


        // try to create virtual surface
        if(gdi_create_vsurf((hdc_t)p_cdc, (s16)RECTWP(p_vrc), (s16)RECTHP(p_vrc)) == SUCCESS)
        {
          u32 cck = 0;
          
          // initialise virtual device rect
          //copy_rect(&p_cdc->vtrl_rc, p_vrc);

          // disable cck
          cck = flinger_get_colorkey(p_cdc->p_back_flinger);
          flinger_set_colorkey(p_cdc->p_back_flinger, SURFACE_INVALID_COLORKEY);

          // copy actual bits to virtual surface
          flinger_bitblt(p_cdc->p_back_flinger,
                         p_vrc->left, p_vrc->top, RECTWP(p_vrc), RECTHP(p_vrc),
                         p_cdc->p_curn_flinger, 0, 0, FLINGER_ROP_SET, 0);

          // enable ckk, again
          flinger_set_colorkey(p_cdc->p_back_flinger, cck);
        }

        // start gpe batch, if dc type is general
        gdi_start_batch(is_top && (p_gdi_info->top.p_flinger != NULL));
      }
      else
      {
        p_cdc->type = p_pdc->type | TYPE_INHERIT;

        p_cdc->p_curn_flinger = p_pdc->p_curn_flinger;
        p_cdc->p_back_flinger = NULL;

        // chk, if need to update the first vitrual dev rect
        if(p_pdc->type & TYPE_VIRTUAL)
        {
          copy_rect(&p_cdc->vtrl_rc, &p_pdc->vtrl_rc);
        }
      }

      break;
    }
  }

  if(i >= p_gdi_info->dc_slot_cnt)
  {
    return HDC_INVALID;
  }

  dc_init(&p_gdi_info->p_dc_slot[i], p_src, p_gcrgn_info);

  return (hdc_t)(&p_gdi_info->p_dc_slot[i]);
}

void gdi_release_dc(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);

  // chk, if need update
  if((p_dc->type & TYPE_VIRTUAL) && !(p_dc->type & TYPE_INHERIT))
  {
    void *p_src_flinger = NULL;
    void *p_dst_flinger = NULL;
    rect_t orc = {0}, eff_rc = {0};
    cliprc_t *p_crc = NULL;
    BOOL is_vsurf_delete = FALSE;

    p_src_flinger = p_dc->p_curn_flinger, p_dst_flinger = p_dc->p_back_flinger;

    // bitblt to the target dc
    // chk, lock rgn
    if(dc_is_general_dc(p_dc))
    {
      LOCK_GCRINFO(p_dc);
      if(!dc_generate_ecrgn(p_dc, FALSE))
      {
        UNLOCK_GCRINFO(p_dc);
        //delete vsurf.
        gdi_delete_vsurf(hdc);

        is_vsurf_delete = TRUE;
      }
    }

    if(!is_vsurf_delete)
    {
      /* virtual device coordinate is already screen coordinate,
         so, it don't need to convert */
      copy_rect(&orc, &p_dc->vtrl_rc);

      /* set output rect */
      copy_rect(&p_dc->rc_output, &orc);
      normalize_rect(&p_dc->rc_output);

      //ENTER_DRAWING(p_dc);
      if(dc_enter_drawing(p_dc) >= 0)
      {
        p_crc = p_dc->ecrgn.p_head;
        while(NULL != p_crc)
        {
          if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
          {
            flinger_set_cliprect(p_dst_flinger, &eff_rc);

            flinger_bitblt(p_src_flinger, 0, 0,
                           (u16)RECTW(orc), (u16)RECTH(
                             orc),
                           p_dst_flinger, (u16)orc.left, (u16)orc.top,
                           FLINGER_ROP_SET, 0);
          }

          p_crc = p_crc->p_next;
        }

        #ifdef CACHE_ON
        extern void flush_dcache_all();
        flush_dcache_all();
        #endif

        /* disable clipper */
        flinger_set_cliprect(p_dc->p_curn_flinger, NULL);

        //LEAVE_DRAWING(p_dc);
        dc_leave_drawing(p_dc);
      }

      UNLOCK_GCRINFO(p_dc);

      // delete virtual surface
      gdi_delete_vsurf(hdc);
    }
  }

  if(p_dc->type & TYPE_GENERAL 
    && !(p_dc->type & TYPE_INHERIT)) /* is parent dc */
  {
    // end gpe batch, if dc type is general
    gdi_end_batch((BOOL)(p_dc->type & TYPE_TOP), TRUE, &p_dc->ecrgn.bound);
    //gdi_end_batch(TRUE, &p_dc->dev_rc);
  }
  
#ifdef CACHE_ON
  extern void flush_dcache_all();

  if(p_dc->type & TYPE_GENERAL) /* is parent dc */
  {
    flush_dcache_all();
  }
#endif

  gdi_empty_cliprgn(&p_dc->lcrgn);
  gdi_empty_cliprgn(&p_dc->ecrgn);

  p_dc->p_gcrgn_info = NULL;

  p_dc->is_used = FALSE;
}

void gdi_release_anim_dc(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);
  
  if(!dc_is_anim_hdc(hdc))
  {
    return;
  }
  
  if((p_dc->type & TYPE_VIRTUAL) && !(p_dc->type & TYPE_INHERIT))
  {
    gdi_delete_vsurf(hdc);
  }

  if(p_dc->type & TYPE_ANIM)
  {
    gdi_delete_anim_surf(hdc);
  }

  gdi_empty_cliprgn(&p_dc->lcrgn);
  gdi_empty_cliprgn(&p_dc->ecrgn);

  p_dc->p_gcrgn_info = NULL;

  p_dc->is_used = FALSE;
  if(p_dc->anim_handle != NULL)
  {
    anim_free(p_dc->anim_handle);
    
    p_dc->anim_handle = NULL;
  }
}

void gdi_select_cliprgn(hdc_t hdc, const cliprgn_t *p_rgn)
{
  dc_t *p_dc = NULL;

  p_dc = dc_hdc2pdc(hdc);
  if(dc_is_general_dc(p_dc))
  {
    gdi_copy_cliprgn(&p_dc->lcrgn, p_rgn);

    /* for general DC, regenerate effective region. */
    LOCK_GCRINFO(p_dc);
    dc_generate_ecrgn(p_dc, TRUE);
    UNLOCK_GCRINFO(p_dc);
  }
  else
  {
    gdi_copy_cliprgn(&p_dc->ecrgn, p_rgn);
    gdi_intersect_cliprect(&p_dc->ecrgn, &p_dc->dev_rc);
  }
}

typedef void (*fill_func_t)(void *p_flinger, rect_t *p_rc, u32 color);

static void _fill_x_rect(hdc_t hdc,
                         rect_t *p_rc,
                         u32 color,
                         fill_func_t p_fill_func)
{
  dc_t *p_dc = NULL;
  cliprc_t *p_clip_rc = NULL;
  rect_t eff_rc, orc = *p_rc;
  
  if(RECTW(orc) <= 0 || RECTH(orc) <= 0)
  {
    return;
  }

  p_dc = dc_hdc2pdc(hdc);
  // chk, lock rgn
  if(dc_is_general_dc(p_dc))
  {
    LOCK_GCRINFO(p_dc);
    if(!dc_generate_ecrgn(p_dc, FALSE))
    {
      UNLOCK_GCRINFO(p_dc);
      return;
    }
  }

  /* transfer device coordinate to screen coordinate. */
  coor_dp2sp(p_dc, &orc.left, &orc.top);
  coor_dp2sp(p_dc, &orc.right, &orc.bottom);

  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  p_dc->cur_pixel = color;

  if(dc_enter_drawing(p_dc) < 0)
  {
    UNLOCK_GCRINFO(p_dc);
    return;
  }

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, &orc.left, &orc.top);
  coor_sp2sp(p_dc, &orc.right, &orc.bottom);
  normalize_rect(&orc);

  p_clip_rc = p_dc->ecrgn.p_head;
  while(NULL != p_clip_rc)
  {
    if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_clip_rc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_dc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_dc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

      p_fill_func(p_dc->p_curn_flinger, &orc, p_dc->cur_pixel);
    }
    p_clip_rc = p_clip_rc->p_next;
  }
  /* disable clipper */
  flinger_set_cliprect(p_dc->p_curn_flinger, NULL);

  //LEAVE_DRAWING(p_dc);
  dc_leave_drawing(p_dc);

  UNLOCK_GCRINFO(p_dc);
}


/*
  * fills a rectangle box.
  */
void gdi_fill_rect(hdc_t hdc, rect_t *p_rc, u32 color)
{
  _fill_x_rect(hdc, p_rc, color, flinger_fill_rect);
}

u8 gdi_get_bpp(pix_type_t format)
{
  u8 bpp = 0;

  switch(format)
  {
    case COLORFORMAT_RGB4BIT:
      bpp = 4;
      break;
      
    case COLORFORMAT_RGB8BIT:
      bpp = 8;
      break;
      
    case COLORFORMAT_RGB565:
    case COLORFORMAT_RGBA5551:
    case COLORFORMAT_ARGB1555:
      bpp = 16;
      break;

    case COLORFORMAT_RGBA8888:
    case COLORFORMAT_ARGB8888:
      bpp = 32;
      break;

    default:
      bpp = 0;
      MT_ASSERT(0);
  }

  return bpp;
}

void gdi_start_batch(BOOL is_top_screen)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);

  if(is_top_screen)
  {
    MT_ASSERT(p_gdi_info->top.p_flinger != NULL);
    
    flinger_start_batch(p_gdi_info->top.p_flinger);
  }
  else
  {
    MT_ASSERT(p_gdi_info->bot.p_flinger != NULL);
    
    flinger_start_batch(p_gdi_info->bot.p_flinger);
  }
}

void gdi_end_batch(BOOL is_top_screen, BOOL is_sync, rect_t *p_rect)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);

  if(is_top_screen)
  {
    MT_ASSERT(p_gdi_info->top.p_flinger != NULL);
    
    flinger_end_batch(p_gdi_info->top.p_flinger, is_sync, p_rect);
  }
  else
  {
    MT_ASSERT(p_gdi_info->bot.p_flinger != NULL);
    
    flinger_end_batch(p_gdi_info->bot.p_flinger, is_sync, p_rect);
  }
}

