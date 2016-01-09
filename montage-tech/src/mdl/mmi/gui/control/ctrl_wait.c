/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_progressbar.c
   this file  implement the functions defined in  ctrl_progressbar.h, also it
   implement some internal used
   function. All these functions are about how to decribe, set and draw a wait
   control.
  */
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

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"

#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_wait.h"
#include "math.h"

/*!
  pi
  */
#define PI 3.1415926


/*!
  max steps.
  */
#define WAIT_STEPS  24

/*!
  minus block size.   1/20 * mid_w, mid_w == mid_h
  */
#define WAIT_MIN_SIZE 24

/*!
  block number
  */
#define WAIT_BLOCK_NUMB 16

/*!
  cicular progress-bar control.
  */
typedef struct
{
  /*!
    base contro.
    */
  control_t base;
  /*!
    block position.
    */
  point_t pos[WAIT_STEPS]; 
  /*!
     Current value of wait.
    */
  u16 curn_step;
  /*!
    wait img type.
    */
  wait_img_t img_type;    
  /*!
    img data.
    */
  u32 img_data; 
}ctrl_wait_t;

/*!
  wait param.
  */
typedef struct
{
  /*!
    handle.
    */
  handle_t s_handle;
  /*!
    dc.
    */
  hdc_t s_dc;
  /*!
    addr.
    */
  u32 s_addr;
  /*!
    width.
    */
  u16 s_w;
  /*!
    height.
    */
  u16 s_h;
}wait_param_t;

static wait_param_t *_wait_init(control_t *p_ctrl)
{
  ctrl_wait_t *p_wait = NULL;
  rect_t rect = {0}, mrect = {0};
  rsc_bitmap_t hdr_bmp = {{0}};
  bitmap_t bmp = {0}, *p_bmp = NULL;
  u8 *p_bits = NULL;
  handle_t rsc_handle = NULL;
  BOOL ret_boo = FALSE;
  rsc_palette_t hdr_pal = {{0}};
  u8 *p_entrys = NULL;
  handle_t h_src = NULL;
  u32 addr_src = 0;
  hdc_t src_dc = 0;
  u16 temp_w = 0, temp_h = 0;
  wait_param_t *p_param = NULL;
  
  p_param = mmi_alloc_buf(sizeof(wait_param_t));
  MT_ASSERT(p_param != NULL);
  memset(p_param, 0, sizeof(wait_param_t));  

  MT_ASSERT(p_param != NULL);
  MT_ASSERT(p_ctrl != NULL);

  p_wait = (ctrl_wait_t *)p_ctrl;

  ctrl_get_mrect(p_ctrl, &mrect);
  
  if(p_wait->img_type == WAIT_IMG_ID)
  {
    rsc_handle = gui_get_rsc_handle();
    MT_ASSERT(rsc_handle != NULL);
    
    ret_boo = rsc_get_bmp(rsc_handle, p_wait->img_data, &hdr_bmp, &p_bits);
    MT_ASSERT(ret_boo == TRUE);

    bmp.format = (u8)hdr_bmp.pixel_type;
    bmp.enable_ckey = (u8)hdr_bmp.enable_ckey;
    bmp.bpp = gdi_get_bpp((pix_type_t)hdr_bmp.pixel_type);
    bmp.p_bits = p_bits;
    bmp.colorkey = hdr_bmp.colorkey;
    bmp.width = hdr_bmp.width;
    bmp.height = hdr_bmp.height;

    if(bmp.bpp <= 8)
    {
      bmp.pal.cnt = 1 << bmp.bpp;
      rsc_get_palette(rsc_handle, rsc_get_curn_palette(rsc_handle), &hdr_pal, &p_entrys);
      bmp.pal.p_entry = (color_t *)p_entrys;
      MT_ASSERT(bmp.pal.cnt == hdr_pal.color_num);
    }

    bmp.pitch = hdr_bmp.pitch;

    p_bmp = &bmp;
  }
  else if(p_wait->img_type == WAIT_IMG_EXT)
  {
    p_bmp = (bitmap_t *)p_wait->img_data;
  }
  else
  {
    MT_ASSERT(0);
  }

  MT_ASSERT(p_bmp != NULL);

  h_src = gdi_create_mem_surf(FALSE, p_bmp->width, p_bmp->height, &addr_src);
  MT_ASSERT(h_src != NULL);
  MT_ASSERT(addr_src != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = p_bmp->width;
  rect.bottom = p_bmp->height;
 
  src_dc = gdi_get_mem_dc(&rect, h_src);
  MT_ASSERT(src_dc != 0);

  ret_boo = gdi_fill_bitmap(src_dc, &rect, (hbitmap_t)p_bmp, 0);
  MT_ASSERT(ret_boo == TRUE);

  //blt from src dc to img dc.
  temp_w = temp_h = RECTW(mrect)  / WAIT_MIN_SIZE;

  MT_ASSERT(temp_w * 7 > p_bmp->width);
  MT_ASSERT(temp_h * 7 > p_bmp->height);
  
  p_param->s_handle = gdi_create_mem_surf(FALSE, temp_w, temp_h, &p_param->s_addr);
  MT_ASSERT(p_param->s_handle != NULL);
  MT_ASSERT(p_param->s_addr != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = temp_w;
  rect.bottom = temp_h;

  p_param->s_w = temp_w;
  p_param->s_h = temp_h;
  
  p_param->s_dc = gdi_get_mem_dc(&rect, p_param->s_handle);
  MT_ASSERT(p_param->s_dc != 0);

  gdi_stretch_blt(src_dc, 0, 0, p_bmp->width, p_bmp->height,
    p_param->s_dc, 0, 0, temp_w, temp_h);

  if(src_dc != 0)
  {
    gdi_release_mem_dc(src_dc);

    gdi_delete_mem_surf(h_src, addr_src);
  }  
    
  return p_param;
}

static void _wait_release(control_t *p_ctrl, wait_param_t *p_param)
{
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_param != NULL);

  if(p_param->s_dc != 0)
  {
    gdi_release_mem_dc(p_param->s_dc);

    gdi_delete_mem_surf(p_param->s_handle, p_param->s_addr);

    p_param->s_handle = NULL;

    p_param->s_addr = 0;

    p_param->s_dc = 0;
  }

  mmi_free_buf(p_param);

  return;
}

static void _wait_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_wait_t *p_wait = NULL;
  rect_t mrect = {0};
  u16 radius = 0, i = 0;
  u16 x = 0, y = 0, w = 0, h = 0, dot_idx = 0;
  wait_param_t *p_param = NULL;
  u32 alpha = 0;

  ctrl_get_mrect(p_ctrl, &mrect);

  //for circular progress bar, width of mrect must equsls to height of mrect.
  MT_ASSERT(RECTW(mrect) == RECTH(mrect));

  gui_paint_frame(hdc, p_ctrl);

  p_wait = (ctrl_wait_t *)p_ctrl;

  MT_ASSERT(p_wait->curn_step < WAIT_STEPS);


  dot_idx = p_wait->curn_step;

  p_param = _wait_init(p_ctrl);

  radius = (RECTW(mrect) - 2 * p_param->s_w)/ 2;
  
  for(i = 0; i < WAIT_BLOCK_NUMB; i++)
  {
    w = p_param->s_w;
    h = p_param->s_h;
    x = p_wait->pos[dot_idx].x * radius / 100 + mrect.left + p_param->s_w;
    y = p_wait->pos[dot_idx].y * radius / 100 + mrect.top + p_param->s_h;

    alpha = 0xFF * i / WAIT_BLOCK_NUMB;
    
    gdi_bitblt(p_param->s_dc, 0, 0, p_param->s_w, p_param->s_h,
      hdc, x, y, FLINGER_ROP_PATCOPY, alpha);

    dot_idx = (dot_idx + 1 + WAIT_STEPS) % WAIT_STEPS;
  }

  _wait_release(p_ctrl, p_param);
}


RET_CODE wait_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_wait_t *p_default_wait = NULL;
  u8 i = 0;

  register_info.data_size = sizeof(ctrl_wait_t);
  register_info.max_cnt = max_cnt;

  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_wait = (ctrl_wait_t *)register_info.p_default_ctrl;
  
  p_default_ctrl->p_proc = wait_class_proc;
  p_default_ctrl->p_paint = _wait_draw;

  p_default_wait->curn_step = 0;

  for(i = 0; i < WAIT_STEPS; i++)
  {
    p_default_wait->pos[i].x = (u32)(100 * cos(360 / WAIT_STEPS * i * PI / 180)) + 100;
    p_default_wait->pos[i].y = (u32)(100 * sin(360 / WAIT_STEPS * i * PI / 180)) + 100;

    OS_PRINTF("@# %d--%d %d\n", i, p_default_wait->pos[i].x, p_default_wait->pos[i].y);
  }
  
  if(ctrl_register_ctrl_class(CTRL_WAIT, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}

void wait_set_img(control_t *p_ctrl, wait_img_t img_type, u32 img_data)
{
  ctrl_wait_t *p_wait = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_wait = (ctrl_wait_t *)p_ctrl;

  p_wait->img_type = img_type;
  p_wait->img_data = img_data;
}


static RET_CODE on_wait_increase(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_wait_t *p_wait = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_wait = (ctrl_wait_t *)p_ctrl;

  p_wait->curn_step++;

  if(p_wait->curn_step >= WAIT_STEPS)
  {
    p_wait->curn_step = 0;
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  
  return SUCCESS;
}


static RET_CODE on_wait_decrease(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_wait_t *p_wait = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_wait = (ctrl_wait_t *)p_ctrl;

  if(p_wait->curn_step > 0)
  {
    p_wait->curn_step--;
  }
  else
  {
    p_wait->curn_step = (WAIT_STEPS - 1);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  
  return SUCCESS;
}

// define the default msgmap of class
BEGIN_CTRLPROC(wait_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_INCREASE, on_wait_increase)
ON_COMMAND(MSG_DECREASE, on_wait_decrease)
END_CTRLPROC(wait_class_proc, ctrl_default_proc)


