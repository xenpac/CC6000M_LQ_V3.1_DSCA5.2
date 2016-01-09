/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_album.h
   this file  implement the funcs defined in  ctrl_bitmap.h,and some internal
   used functions.
   These functions are about register,set feature and draw a bitmap control.
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

//#include "osd.h"
//#include "gpe.h"
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

#include "ctrl_album.h"

/*!
  album ctrl state.
  */
typedef enum
{
  /*!
    albums scroll none.
    */
  ALBUM_SCROLL_N = 0,    
  /*!
    album scroll left.
    */
  ALBUM_SCROLL_L,
  /*!
    album scroll right.
    */
  ALBUM_SCROLL_R,
}album_sts_t;

/*!
   Thist structure defines private data of album control,
   mainly about bitmap id, left interval and top interval
  */
typedef struct
{
  /*!
    control base.
    */
  control_t base;    
  /*!
    total
    */
  u16 total;
  /*!
    focus
    */
  u16 focus;
  /*!
    img data.
    */
  u32 *p_img_data;
  /*!
    str data.
    */
  u32 *p_str_data;
  /*!
    album scroll states.
    */
  album_sts_t scroll_sts;
} ctrl_album_t;

/*!
  on show albums.
  */
#define ALBUMS_ON_SHOW      5

/*!
  animation frames.
  */
#define ALBUMS_ANIM_FRAMES  4

/*!
  album param.
  */
typedef struct
{
  /*!
    handle.
    */
  handle_t s_handle[ALBUMS_ON_SHOW + 1];
  /*!
    dc.
    */
  hdc_t s_dc[ALBUMS_ON_SHOW + 1];
  /*!
    addr.
    */
  u32 s_addr[ALBUMS_ON_SHOW + 1];
  /*!
    width.
    */
  u16 s_w[ALBUMS_ON_SHOW + 1];
  /*!
    height.
    */
  u16 s_h[ALBUMS_ON_SHOW + 1];
  /*!
    handle.
    */
  handle_t t_handle;
  /*!
    dc.
    */
  hdc_t t_dc;
  /*!
    addr.
    */
  u32 t_addr;
  /*!
    width.
    */
  u16 t_w;
  /*!
    height.
    */
  u16 t_h;
}album_param_t;

static void _album_prepare_image(control_t *p_ctrl,
  u16 index, handle_t *p_handle, hdc_t *p_dc, u32 *p_addr, u16 *p_img_w, u16 *p_img_h)
{
  ctrl_album_t *p_album = NULL;
  rect_t rect = {0}, mrect = {0};
  rsc_bitmap_t hdr_bmp = {{0}};
  bitmap_t bmp = {0}, *p_bmp = NULL;
  u8 *p_bits = NULL;
  handle_t rsc_handle = NULL;
  BOOL ret_boo = FALSE;
  rsc_palette_t hdr_pal = {{0}};
  u8 *p_entrys = NULL;
  cct_gpe_blt3opt_t opt3 = {0};
  handle_t h_grad = NULL, h_temp = NULL, h_src = NULL;
  u32 addr_grad = 0, addr_temp = 0, addr_src = 0;
  hdc_t grad_dc = 0, temp_dc = 0, src_dc = 0;
  cct_gpe_paint2opt_t popt = {0};
  u32 stop_offset[4] = {0};
  u16 temp_w = 0, temp_h = 0;

  MT_ASSERT(p_handle != NULL);
  MT_ASSERT(p_dc != NULL);
  MT_ASSERT(p_addr != NULL);
  MT_ASSERT(p_img_w != NULL);
  MT_ASSERT(p_img_h != NULL);
  MT_ASSERT(p_ctrl != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(index < p_album->total);

  ctrl_get_mrect(p_ctrl, &mrect);
  
  if(p_ctrl->priv_attr & ALBUM_IMGTYPE_MASK)
  {
    rsc_handle = gui_get_rsc_handle();
    MT_ASSERT(rsc_handle != NULL);
    
    ret_boo = rsc_get_bmp(rsc_handle, *(p_album->p_img_data + index), &hdr_bmp, &p_bits);
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
  else
  {
    p_bmp = (bitmap_t *)(*(p_album->p_img_data + index));
  }

  MT_ASSERT(p_bmp != NULL);

  h_src = gdi_create_mem_surf(FALSE, p_bmp->width, p_bmp->height * 3 / 2, &addr_src);
  MT_ASSERT(h_src != NULL);
  MT_ASSERT(addr_src != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = p_bmp->width;
  rect.bottom = p_bmp->height * 3 / 2 + 2;
 
  src_dc = gdi_get_mem_dc(&rect, h_src);
  MT_ASSERT(src_dc != 0);

  ret_boo = gdi_fill_bitmap(src_dc, &rect, (hbitmap_t)p_bmp, 0);
  MT_ASSERT(ret_boo == TRUE);

  //draw reflection.
  h_grad = gdi_create_mem_surf(FALSE, p_bmp->width, p_bmp->height / 2, &addr_grad);
  MT_ASSERT(h_grad != NULL);
  MT_ASSERT(addr_grad != 0);

  rect.left = 0;
  rect.top = 0;
  rect.right = p_bmp->width;
  rect.bottom = p_bmp->height / 2;

  grad_dc = gdi_get_mem_dc(&rect, h_grad);
  MT_ASSERT(grad_dc != 0);

  h_temp = gdi_create_mem_surf(FALSE, p_bmp->width, p_bmp->height / 2, &addr_temp);
  MT_ASSERT(h_temp != NULL);
  MT_ASSERT(addr_temp != 0);

  rect.left = 0;
  rect.top = 0;
  rect.right = p_bmp->width;
  rect.bottom = p_bmp->height / 2;

  temp_dc = gdi_get_mem_dc(&rect, h_temp);
  MT_ASSERT(temp_dc != 0);
  
  stop_offset[0] = 0;
  stop_offset[1] = 4095;
  stop_offset[2] = 4095;
  stop_offset[3] = 4095;
  
  popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
  popt.paint_cfg.paint_liner_gradt.begin.x = 0;
  popt.paint_cfg.paint_liner_gradt.begin.y = 0;
  popt.paint_cfg.paint_liner_gradt.end.x = 0;
  popt.paint_cfg.paint_liner_gradt.end.y = p_bmp->height / 2;
  popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_PAD;
  popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xE0FFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop1.argb = 0x00FFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop2.argb = 0x00FFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop3.argb = 0x00FFFFFF;
  popt.paint_cfg.paint_liner_gradt.stop0.offset = stop_offset[0];
  popt.paint_cfg.paint_liner_gradt.stop1.offset = stop_offset[1];
  popt.paint_cfg.paint_liner_gradt.stop2.offset = stop_offset[2];
  popt.paint_cfg.paint_liner_gradt.stop3.offset = stop_offset[3];    

  gdi_paint_blt(grad_dc, 0, 0, p_bmp->width, p_bmp->height / 2, &popt);

  gdi_rotate_mirror(src_dc, 0, p_bmp->height / 2, p_bmp->width, p_bmp->height / 2,
    temp_dc, 0, 0, LLD_GFX_ROTATE_NONE, LLD_GFX_MIRROR_TB);

  opt3.enableDrawMult = TRUE;
  opt3.enableRop = TRUE;
  opt3.ropCfg.rop_a_id = ROP_COPYPEN;
  opt3.ropCfg.rop_c_id = ROP_COPYPEN;
  
  gdi_blt_3src(
    temp_dc, 0, 0, p_bmp->width, p_bmp->height / 2,
    src_dc, 0, p_bmp->height + 2, p_bmp->width, p_bmp->height / 2,
    grad_dc, 0, 0, p_bmp->width, p_bmp->height / 2,
    &opt3);

  if(temp_dc != 0)
  {
    gdi_release_mem_dc(temp_dc);

    gdi_delete_mem_surf(h_temp, addr_temp);
  }  
      
  if(grad_dc != 0)
  {
    gdi_release_mem_dc(grad_dc);

    gdi_delete_mem_surf(h_grad, addr_grad);
  }  

  //blt from src dc to img dc.
  temp_w = RECTW(mrect) * 10 / 28;
  temp_h = RECTH(mrect);
  
  *p_handle = gdi_create_mem_surf(FALSE, temp_w, temp_h, p_addr);
  MT_ASSERT((*p_handle) != NULL);
  MT_ASSERT((*p_addr) != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = temp_w;
  rect.bottom = temp_h;

  *p_img_w = temp_w;
  *p_img_h = temp_h;
  
  *p_dc = gdi_get_mem_dc(&rect, *p_handle);
  MT_ASSERT((*p_dc) != 0);

  gdi_stretch_blt(src_dc, 0, 0, p_bmp->width, p_bmp->height * 3 /2,
    *p_dc, 0, 0, temp_w, temp_h);

  if(src_dc != 0)
  {
    gdi_release_mem_dc(src_dc);

    gdi_delete_mem_surf(h_src, addr_src);
  }  
    
  return;
}

static album_param_t *_album_init(control_t *p_ctrl)
{
  u16 index = 0, i = 0;
  ctrl_album_t *p_album = NULL;
  album_param_t *p_param = NULL;
  rect_t mrect = {0}, rect = {0};

  MT_ASSERT(p_ctrl != NULL);

  p_param = mmi_alloc_buf(sizeof(album_param_t));
  MT_ASSERT(p_param != NULL);
  memset(p_param, 0, sizeof(album_param_t));

  p_album = (ctrl_album_t *)p_ctrl;

  switch(p_album->scroll_sts)
  {
    case ALBUM_SCROLL_L:
      //prepare on show image.
      for(i = 0; i < ALBUMS_ON_SHOW; i++)
      {
        index = (p_album->focus - 3 + p_album->total + i) % p_album->total;

        _album_prepare_image(p_ctrl, index, &p_param->s_handle[i],
          &p_param->s_dc[i], &p_param->s_addr[i], &p_param->s_w[i], &p_param->s_h[i]);
      }
    
      //prepare new image.
      _album_prepare_image(p_ctrl, 
        (p_album->focus + 2 + p_album->total) % p_album->total,
        &p_param->s_handle[ALBUMS_ON_SHOW], &p_param->s_dc[ALBUMS_ON_SHOW],
        &p_param->s_addr[ALBUMS_ON_SHOW],
        &p_param->s_w[ALBUMS_ON_SHOW], &p_param->s_h[ALBUMS_ON_SHOW]);
      break;

    case ALBUM_SCROLL_R:
      //prepare new image.
      _album_prepare_image(p_ctrl,
        (p_album->focus - 2 + p_album->total) % p_album->total,
        &p_param->s_handle[0],
        &p_param->s_dc[0], &p_param->s_addr[0], &p_param->s_w[0], &p_param->s_h[0]);

      //prepare on show image.
      for(i = 0; i < ALBUMS_ON_SHOW; i++)
      {
        index = (p_album->focus - 1 + p_album->total + i) % p_album->total;

        _album_prepare_image(p_ctrl, index, &p_param->s_handle[i + 1],
          &p_param->s_dc[i + 1], &p_param->s_addr[i + 1],
          &p_param->s_w[i + 1], &p_param->s_h[i+ 1]);
      }

      break;

    default:
      for(i = 0; i < ALBUMS_ON_SHOW; i++)
      {
        index = (p_album->focus + p_album->total - 2 + i) % p_album->total;

        _album_prepare_image(p_ctrl, index, &p_param->s_handle[i],
          &p_param->s_dc[i], &p_param->s_addr[i], &p_param->s_w[i], &p_param->s_h[i]);
      }
      break;
  }

  ctrl_get_mrect(p_ctrl, &mrect);

  p_param->t_w = RECTW(mrect) * 10 / 28;
  p_param->t_h = RECTH(mrect);
  
  p_param->t_handle = gdi_create_mem_surf(FALSE, p_param->t_w, p_param->t_h, &p_param->t_addr);
  MT_ASSERT(p_param->t_handle != NULL);
  MT_ASSERT(p_param->t_addr != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = p_param->t_w;
  rect.bottom = p_param->t_h;
  
  p_param->t_dc = gdi_get_mem_dc(&rect, p_param->t_handle);
  MT_ASSERT(p_param->t_dc != 0);

  return p_param;
}

static void _album_release(control_t *p_ctrl, album_param_t *p_param)
{
  u16 i = 0;
  
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_param != NULL);

  for(i = 0; i < (ALBUMS_ON_SHOW + 1); i++)
  {
    if(p_param->s_dc[i] != 0)
    {
      gdi_release_mem_dc(p_param->s_dc[i]);

      gdi_delete_mem_surf(p_param->s_handle[i], p_param->s_addr[i]);

      p_param->s_handle[i] = NULL;

      p_param->s_addr[i] = 0;

      p_param->s_dc[i] = 0;
    }
  }  

  if(p_param->t_dc != 0)
  {
    gdi_release_mem_dc(p_param->t_dc);

    gdi_delete_mem_surf(p_param->t_handle, p_param->t_addr);

    p_param->t_handle = 0;

    p_param->t_addr = 0;
    
    p_param->t_dc = 0;
  }  

  mmi_free_buf(p_param);

  return;
}

static void _album_scroll_none(control_t *p_ctrl, album_param_t *p_param, hdc_t hdc)
{
  ctrl_album_t *p_album = NULL;
  u16 temp_w = 0, temp_h = 0, dx = 0, dy = 0;
  rect_t mrect = {0};
  trape_t trape = {0};

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_param != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(p_album->scroll_sts == ALBUM_SCROLL_N);

  ctrl_get_mrect(p_ctrl, &mrect);

  //paint background.
  gui_paint_frame(hdc, p_ctrl);

  //step 1.    draw left img.
  temp_w = p_param->t_w * 6 / 10;
  temp_h = p_param->t_h * 6 / 10;

  gdi_stretch_blt(p_param->s_dc[0], 0, 0, p_param->s_w[0], p_param->s_h[0],
    p_param->t_dc, 0, 0, temp_w, temp_h);

  trape.top_start_x = 4096 * temp_h / 6;
  trape.top_len = temp_h * 2 / 3;
  trape.bottom_start_x = 0;
  trape.bottom_len = temp_h;
  trape.dert_deno = 0xFFFFFFFF;
  trape.dert_num = 1;

  dx = mrect.left + RECTW(mrect) / 28 * 2;
  dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

  gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);

  //step 2.    draw left center img.
  temp_w = p_param->t_w * 8 / 10;
  temp_h = p_param->t_h * 8 / 10;

  gdi_stretch_blt(p_param->s_dc[1], 0, 0, p_param->s_w[1], p_param->s_h[1],
    p_param->t_dc, 0, 0, temp_w, temp_h);

  trape.top_start_x = 4096 * temp_h / 6;
  trape.top_len = temp_h * 2 / 3;
  trape.bottom_start_x = 0;
  trape.bottom_len = temp_h;
  trape.dert_deno = 0xFFFFFFFF;
  trape.dert_num = 1;

  dx = mrect.left + RECTW(mrect) / 28 * 5;
  dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

  gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, 
    hdc, dx, dy, temp_w, temp_h, &trape);

  //step 3.    draw right img.
  temp_w = p_param->t_w * 6 / 10;
  temp_h = p_param->t_h * 6 / 10;

  gdi_stretch_blt(p_param->s_dc[4], 0, 0, p_param->s_w[4], p_param->s_h[4],
    p_param->t_dc, 0, 0, temp_w, temp_h);

  trape.top_start_x = 4096 * temp_h / 6;
  trape.top_len = temp_h * 2 / 3;
  trape.bottom_start_x = 0;
  trape.bottom_len = temp_h;
  trape.direction = 1;
  trape.dert_deno = 0xFFFFFFFF;
  trape.dert_num = 1;

  dx = mrect.left + RECTW(mrect) / 28 * 20;
  dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

  gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);

  //step 4.    draw right center img.
  temp_w = p_param->t_w * 8 / 10;
  temp_h = p_param->t_h * 8 / 10;

  gdi_stretch_blt(p_param->s_dc[3], 0, 0, p_param->s_w[3], p_param->s_h[3],
    p_param->t_dc, 0, 0, temp_w, temp_h);

  trape.top_start_x = 4096 * temp_h / 6;
  trape.top_len = temp_h * 2 / 3;
  trape.bottom_start_x = 0;
  trape.bottom_len = temp_h;
  trape.direction = 1;
  trape.dert_deno = 0xFFFFFFFF;
  trape.dert_num = 1;
  
  dx = mrect.left + RECTW(mrect) / 28 * 15;
  dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

  gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);

  //step 5.    draw center img.
  temp_w = p_param->t_w;
  temp_h = p_param->t_h;

  gdi_stretch_blt(p_param->s_dc[2], 0, 0, p_param->s_w[2], p_param->s_h[2],
    p_param->t_dc, 0, 0, temp_w, temp_h);

  trape.top_start_x = 0;
  trape.top_len = temp_h;
  trape.bottom_start_x = 0;
  trape.bottom_len = temp_h;
  trape.direction = 1;
  trape.dert_deno = 0xFFFFFFFF;
  trape.dert_num = 1;
  
  dx = mrect.left + RECTW(mrect) / 28 * 9;
  dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

  gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);

//  gdi_rect_stroke(hdc,
//    dx, dy, temp_w, temp_h * 2 / 3, p_album->shadow_depth, p_album->shadow_color);
}

static void _album_scroll_left(control_t *p_ctrl, album_param_t *p_param, hdc_t hdc)
{
  ctrl_album_t *p_album = NULL;
  u16 temp_w = 0, temp_h = 0, dx = 0, dy = 0;
  rect_t mrect = {0}, screen = {0};
  trape_t trape = {0};
  u16 i = 0, left = 0, right = 0;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_param != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(p_album->scroll_sts == ALBUM_SCROLL_L);

  ctrl_get_mrect(p_ctrl, &mrect);
  copy_rect(&screen, &mrect);
  ctrl_client2screen(p_ctrl, &screen);

  for(i = 1; i <= ALBUMS_ANIM_FRAMES; i++)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    gui_paint_frame(hdc, p_ctrl);
    
    //step new image.  0% -----> 60%
    temp_w = p_param->t_w * 6 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 6 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[5], 0, 0, p_param->s_w[5], p_param->s_h[5],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 1;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.right - RECTW(mrect) / 28 * 2 / ALBUMS_ANIM_FRAMES * i - temp_w;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);      
    
    //right. 60%------->80%
    temp_w = p_param->t_w * 6 / 10 + p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 6 / 10 + p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[4], 0, 0, p_param->s_w[4], p_param->s_h[4],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 1;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.right - 
      RECTW(mrect) / 28 * 2 - RECTW(mrect) / 28 * 3 / ALBUMS_ANIM_FRAMES * i - temp_w;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);      

    //right center. 80%------>100% 
    left = mrect.right - RECTW(mrect) / 28 * 5 - RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i;

    dx = mrect.right - RECTW(mrect) / 28 * 19
      + RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i - temp_w;
    temp_w = p_param->t_w - p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

    right = dx + temp_w;

    if(left <= right)
    {
      temp_w = p_param->t_w * 8 / 10 + p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
      temp_h = p_param->t_h * 8 / 10 + p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

      gdi_stretch_blt(p_param->s_dc[3], 0, 0, p_param->s_w[3], p_param->s_h[3],
        p_param->t_dc, 0, 0, temp_w, temp_h);

      trape.top_start_x = 4096 * temp_h / 6 * (ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.top_len = temp_h - 2 * temp_h / 6 * ( ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.bottom_start_x = 0;
      trape.bottom_len = temp_h;
      trape.direction = 1;
      trape.dert_deno = 0xFFFFFFFF;
      trape.dert_num = 1;

      dx = mrect.right -
        RECTW(mrect) / 28 * 5 - RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i - temp_w;
      dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

      gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    
    }
    
    //left. 60%----->0%
    temp_w = p_param->t_w * 6 / 10 - p_param->t_w * 6 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 6 / 10 - p_param->t_h * 6 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[0], 0, 0, p_param->s_w[0], p_param->s_h[0],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 0;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) / 28 * 2 - RECTW(mrect) / 28 * 2 / ALBUMS_ANIM_FRAMES * i;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    

    //left center. 80%------>60%.
    temp_w = p_param->t_w * 8 / 10 - p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 8 / 10 - p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[1], 0, 0, p_param->s_w[1], p_param->s_h[1],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 0;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) / 28 * 5 - RECTW(mrect) / 28 * 3 / ALBUMS_ANIM_FRAMES * i;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);  

    //center. 100%------>80%
    temp_w = p_param->t_w - p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h - p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[2], 0, 0, p_param->s_w[2], p_param->s_h[2],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6 * i / ALBUMS_ANIM_FRAMES;
    trape.top_len = temp_h - 2 * temp_h / 6 * i / ALBUMS_ANIM_FRAMES;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 0;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) / 28 * 9 - RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    

    //right center. 80%--->100%
    if(left > right)
    {
      temp_w = p_param->t_w * 8 / 10 + p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
      temp_h = p_param->t_h * 8 / 10 + p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

      gdi_stretch_blt(p_param->s_dc[3], 0, 0, p_param->s_w[3], p_param->s_h[3],
        p_param->t_dc, 0, 0, temp_w, temp_h);

      trape.top_start_x = 4096 * temp_h / 6 * (ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.top_len = temp_h - 2 * temp_h / 6 * ( ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.bottom_start_x = 0;
      trape.bottom_len = temp_h;
      trape.direction = 1;
      trape.dert_deno = 0xFFFFFFFF;
      trape.dert_num = 1;

      dx = mrect.right -
        RECTW(mrect) / 28 * 5 - RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i - temp_w;
      dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

      gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    
    }

    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &screen);
  }

  temp_w = p_param->t_w * 8 / 10 +
    p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * ALBUMS_ANIM_FRAMES;
  temp_h = p_param->t_h * 8 / 10 +
    p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * ALBUMS_ANIM_FRAMES;

  dx = mrect.right - RECTW(mrect) / 28 * 5 -
    RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * ALBUMS_ANIM_FRAMES - temp_w;
  dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

//  gdi_rect_stroke(hdc,
//    dx, dy, temp_w, temp_h * 2 / 3, p_album->shadow_depth, p_album->shadow_color);
}

static void _album_scroll_right(control_t *p_ctrl, album_param_t *p_param, hdc_t hdc)
{
  ctrl_album_t *p_album = NULL;
  u16 temp_w = 0, temp_h = 0, dx = 0, dy = 0;
  rect_t mrect = {0}, screen = {0};
  trape_t trape = {0};
  u16 i = 0, left = 0, right = 0;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_param != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(p_album->scroll_sts == ALBUM_SCROLL_R);

  ctrl_get_mrect(p_ctrl, &mrect);
  copy_rect(&screen, &mrect);
  ctrl_client2screen(p_ctrl, &screen);  

  for(i = 1; i <= ALBUMS_ANIM_FRAMES; i++)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    gui_paint_frame(hdc, p_ctrl);
    
    //step new image.  0% -----> 60%
    temp_w = p_param->t_w * 6 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 6 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[0], 0, 0, p_param->s_w[0], p_param->s_h[0],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 0;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) / 28 * 2 / ALBUMS_ANIM_FRAMES * i;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);      
    
    //left. 60%------->80%
    temp_w = p_param->t_w * 6 / 10 + p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 6 / 10 + p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[1], 0, 0, p_param->s_w[1], p_param->s_h[1],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 0;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) / 28 * 2 + RECTW(mrect) / 28 * 3 / ALBUMS_ANIM_FRAMES * i;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);      

    //left center. 80%------>100% 
    temp_w = p_param->t_w * 8 / 10 + p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
    dx = mrect.left + RECTW(mrect) / 28 * 5 + RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i;

    right = dx + temp_w;

    left = mrect.left + RECTW(mrect) - RECTW(mrect) / 28 * 9
      + RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i - temp_w;

    if(left > right)
    {
      temp_w = p_param->t_w * 8 / 10 + p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
      temp_h = p_param->t_h * 8 / 10 + p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

      gdi_stretch_blt(p_param->s_dc[2], 0, 0, p_param->s_w[2], p_param->s_h[2],
        p_param->t_dc, 0, 0, temp_w, temp_h);

      trape.top_start_x = 4096 * temp_h / 6 * (ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.top_len = temp_h - 2 * temp_h / 6 * ( ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.bottom_start_x = 0;
      trape.bottom_len = temp_h;
      trape.direction = 0;
      trape.dert_deno = 0xFFFFFFFF;
      trape.dert_num = 1;

      dx = mrect.left + RECTW(mrect) / 28 * 5 + RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i;
      dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

      gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    
    }
    
    //right. 60%----->0%
    temp_w = p_param->t_w * 6 / 10 - p_param->t_w * 6 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 6 / 10 - p_param->t_h * 6 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[5], 0, 0, p_param->s_w[5], p_param->s_h[5],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 1;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) - RECTW(mrect) / 28 * 2
      + RECTW(mrect) / 28 * 2 / ALBUMS_ANIM_FRAMES * i - temp_w;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    

    //right center. 80%------>60%.
    temp_w = p_param->t_w * 8 / 10 - p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h * 8 / 10 - p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[4], 0, 0, p_param->s_w[4], p_param->s_h[4],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6;
    trape.top_len = temp_h  * 2 / 3;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 1;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) - RECTW(mrect) / 28 * 5
      + RECTW(mrect) / 28 * 3 / ALBUMS_ANIM_FRAMES * i - temp_w;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);  

    //center. 100%------>80%
    temp_w = p_param->t_w - p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
    temp_h = p_param->t_h - p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

    gdi_stretch_blt(p_param->s_dc[3], 0, 0, p_param->s_w[3], p_param->s_h[3],
      p_param->t_dc, 0, 0, temp_w, temp_h);

    trape.top_start_x = 4096 * temp_h / 6 * i / ALBUMS_ANIM_FRAMES;
    trape.top_len = temp_h - 2 * temp_h / 6 * i / ALBUMS_ANIM_FRAMES;
    trape.bottom_start_x = 0;
    trape.bottom_len = temp_h;
    trape.direction = 1;
    trape.dert_deno = 0xFFFFFFFF;
    trape.dert_num = 1;

    dx = mrect.left + RECTW(mrect) - RECTW(mrect) / 28 * 9
      + RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i - temp_w;
    dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

    gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    

    if(left <= right)
    {
      temp_w = p_param->t_w * 8 / 10 + p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * i;
      temp_h = p_param->t_h * 8 / 10 + p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * i;

      gdi_stretch_blt(p_param->s_dc[2], 0, 0, p_param->s_w[2], p_param->s_h[2],
        p_param->t_dc, 0, 0, temp_w, temp_h);

      trape.top_start_x = 4096 * temp_h / 6 * (ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.top_len = temp_h - 2 * temp_h / 6 * ( ALBUMS_ANIM_FRAMES - i) / ALBUMS_ANIM_FRAMES;
      trape.bottom_start_x = 0;
      trape.bottom_len = temp_h;
      trape.direction = 0;
      trape.dert_deno = 0xFFFFFFFF;
      trape.dert_num = 1;

      dx = mrect.left + RECTW(mrect) / 28 * 5 + RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * i;
      dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

      gdi_trape_blt(p_param->t_dc, 0, 0, temp_w, temp_h, hdc, dx, dy, temp_w, temp_h, &trape);    
    }

    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &screen);
  }

  temp_w = p_param->t_w * 8 / 10 +
    p_param->t_w * 2 / 10 / ALBUMS_ANIM_FRAMES * ALBUMS_ANIM_FRAMES;
  temp_h = p_param->t_h * 8 / 10 +
    p_param->t_h * 2 / 10 / ALBUMS_ANIM_FRAMES * ALBUMS_ANIM_FRAMES;

  dx = mrect.left + RECTW(mrect) / 28 * 5 +
    RECTW(mrect) / 28 * 4 / ALBUMS_ANIM_FRAMES * ALBUMS_ANIM_FRAMES;
  dy = mrect.top + (RECTH(mrect) - temp_h) / 2;

//  gdi_rect_stroke(hdc,
//    dx, dy, temp_w, temp_h * 2 / 3, p_album->shadow_depth, p_album->shadow_color);  
}

static void _album_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_album_t *p_album = NULL;
  album_param_t *p_param = NULL;
  
  MT_ASSERT(p_ctrl != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  p_param = _album_init(p_ctrl);
  MT_ASSERT(p_param != NULL);

  //scroll left/right.
  switch(p_album->scroll_sts)
  {
    case ALBUM_SCROLL_L:
      _album_scroll_left(p_ctrl, p_param, hdc);
      break;

    case ALBUM_SCROLL_R:
      _album_scroll_right(p_ctrl, p_param, hdc);
      break;

    default:
      _album_scroll_none(p_ctrl, p_param, hdc);
      break;
  }
  
  _album_release(p_ctrl, p_param);

  p_album->scroll_sts = ALBUM_SCROLL_N;
}

RET_CODE album_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_album_t *p_default_data = NULL;
  
  register_info.data_size = sizeof(ctrl_album_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_data = (ctrl_album_t *)register_info.p_default_ctrl;
  
  // initialize the default control of bitmap class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER;
  p_default_ctrl->p_proc = album_class_proc;
  p_default_ctrl->p_paint = _album_draw;

  // initalize the default data of bitmap class
  p_default_data->scroll_sts = ALBUM_SCROLL_N;
  
  if(ctrl_register_ctrl_class(CTRL_ALBUM, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}

void album_set_img_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~ALBUM_IMGTYPE_MASK);
  p_ctrl->priv_attr |= (type & ALBUM_IMGTYPE_MASK);
}

void album_set_total(control_t *p_ctrl, u16 total)
{
  ctrl_album_t *p_album = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(total > p_album->focus);
  MT_ASSERT(total > ALBUMS_ON_SHOW);
  
  p_album->total = total;

  p_album->p_img_data = mmi_realloc_buf(p_album->p_img_data, total * sizeof(u32 *));
  MT_ASSERT(p_album->p_img_data != NULL);

  memset(p_album->p_img_data, 0, total * sizeof(u32 *));
}

void album_set_focus(control_t *p_ctrl, u16 focus)
{
  ctrl_album_t *p_album = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(focus < p_album->total);

  p_album->focus = focus;
}

u16 album_get_focus(control_t *p_ctrl)
{
  ctrl_album_t *p_album = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  return p_album->focus;
}

void album_set_img_id(control_t *p_ctrl, u16 idx, u16 img_id)
{
  ctrl_album_t *p_album = NULL;

  MT_ASSERT(p_ctrl != NULL);

  MT_ASSERT((p_ctrl->priv_attr & ALBUM_IMGTYPE_MASK) != 0);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(idx < p_album->total);
  
  *(p_album->p_img_data + idx) = img_id;
}


void album_set_img_ext(control_t *p_ctrl, u16 idx, bitmap_t *p_bmp)
{
  ctrl_album_t *p_album = NULL;

  MT_ASSERT(p_ctrl != NULL);

  MT_ASSERT((p_ctrl->priv_attr & ALBUM_IMGTYPE_MASK) == 0);

  p_album = (ctrl_album_t *)p_ctrl;

  MT_ASSERT(idx < p_album->total);
  
  *(p_album->p_img_data + idx) = (u32)p_bmp;
}

static RET_CODE on_album_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_album_t *p_album = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  switch(msg)
  {
    case MSG_INCREASE:
      MT_ASSERT(p_album->scroll_sts == ALBUM_SCROLL_N);
      
      p_album->focus++;
      if(p_album->focus == p_album->total)
      {
        p_album->focus = 0;
      }

      p_album->scroll_sts = ALBUM_SCROLL_L;
      break;

    case MSG_DECREASE:
      MT_ASSERT(p_album->scroll_sts == ALBUM_SCROLL_N);
      
      if(p_album->focus == 0)
      {
        p_album->focus = (p_album->total - 1);
      }
      else
      {
        p_album->focus--;
      }

      p_album->scroll_sts = ALBUM_SCROLL_R;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  ctrl_paint_ctrl(p_ctrl, TRUE);

  return SUCCESS;
}

static RET_CODE on_album_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  ctrl_album_t *p_album = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_album = (ctrl_album_t *)p_ctrl;

  if(p_album->p_img_data != NULL)
  {
    mmi_free_buf(p_album->p_img_data);

    p_album->p_img_data = NULL;
  }

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}

BEGIN_CTRLPROC(album_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_INCREASE, on_album_change_focus)
  ON_COMMAND(MSG_DECREASE, on_album_change_focus)
  ON_COMMAND(MSG_DESTROY, on_album_destroy)
END_CTRLPROC(album_class_proc, ctrl_default_proc)
