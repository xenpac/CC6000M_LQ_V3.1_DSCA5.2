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

#include "hal_misc.h"

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
#include "osd.h"
#include "gpe.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"
#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "gdi_private.h"
#include "class_factory.h"


/*!
 Create a compitable bitmap control with the specified reference DC.
 */
hbitmap_t gdi_create_compatible_bitmap(hdc_t hdc, bitmap_t *p_bmp)
{
  return (hbitmap_t)(p_bmp);
}


/*!
 Delect a compitable bitmap control.
 */
void gdi_delete_compatible_bitmap(hbitmap_t bmp)
{
}


static void tile_bitblt(void *p_surf, rect_t *p_orc,
  bitmap_t *p_bmp, s32 flags)
{
  rect_t rc = *p_orc;
  s32 h = 0, v = 0, h_cnt = 0, v_cnt = 0;
  
  if((flags & FILL_DIRECTION_HORI) && (flags & FILL_DIRECTION_VERT))
  {
    h_cnt = RECTW(rc) / p_bmp->width + (RECTW(rc) % p_bmp->width ? 1 : 0);
    v_cnt = RECTH(rc) / p_bmp->height + (RECTH(rc) % p_bmp->height ? 1 : 0);
  }
  else if(flags & FILL_DIRECTION_HORI)
  {
    h_cnt = RECTW(rc) / p_bmp->width + (RECTW(rc) % p_bmp->width ? 1 : 0);
    v_cnt = 1;
  }
  else if(flags & FILL_DIRECTION_VERT)
  {
    h_cnt = 1;
    v_cnt = RECTH(rc) / p_bmp->height + (RECTH(rc) % p_bmp->height ? 1 : 0);
  }
  else
  {
    h_cnt = v_cnt = 1;
  }

  rc.right = rc.left + p_bmp->width;
  rc.bottom = rc.top + p_bmp->height;

  for(h = 0; h < h_cnt; h++)
  {
    for(v = 0; v < v_cnt; v++)
    {
      flinger_fill_bmp(p_surf, &rc, p_bmp);

      if(v + 1 != v_cnt)
      {
        offset_rect(&rc, 0, p_bmp->height);
      }
    }
    // reset top
    rc.top = p_orc->top;
    rc.bottom = rc.top + p_bmp->height;

    if(h + 1 != h_cnt)
    {
      offset_rect(&rc, p_bmp->width, 0);
    }
  }
}


/*!
 Fills a box with a BITMAP control.
 */
BOOL gdi_fill_bitmap(hdc_t hdc, rect_t *p_rc, const hbitmap_t bmp, u32 flags)
{
  bitmap_t *p_bmp = (bitmap_t *)(bmp);
  gdi_main_t *p_gdi_info = NULL;
  cliprc_t *p_crc = NULL;
  rect_t eff_rc, orc = *p_rc;
#ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
#else
  chip_ic_t chip_ic_id = IC_MAGIC;
#endif

  dc_t *p_dc = dc_hdc2pdc(hdc);

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  if(RECTW(orc) <= 0 || RECTH(orc) <= 0)
  {
    return FALSE;
  }

  if((chip_ic_id == IC_MAGIC) || (chip_ic_id == IC_WIZARDS))
  {
    // chk, if not fit to dc
    if(p_bmp->bpp != flinger_get_bpp(p_dc->p_curn_flinger))
    {
      return FALSE;
    }
  }
  
  // chk, lock rgn
  if(dc_is_general_dc(p_dc))
  {
    LOCK_GCRINFO(p_dc);
    if(!dc_generate_ecrgn(p_dc, FALSE))
    {
      UNLOCK_GCRINFO(p_dc);
      return FALSE;
    }
  }

  /* transfer device coordinate to screen coordinate. */
  coor_dp2sp(p_dc, &orc.left, &orc.top);
  coor_dp2sp(p_dc, &orc.right, &orc.bottom);

  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  //ENTER_DRAWING(p_dc);
  if(dc_enter_drawing(p_dc) < 0)
  {
    UNLOCK_GCRINFO(p_dc);
    return FALSE;
  }  

    /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, &orc.left, &orc.top);
  coor_sp2sp(p_dc, &orc.right, &orc.bottom);
  normalize_rect(&orc);

  p_crc = p_dc->ecrgn.p_head;
  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_dc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_dc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

      if(flags & FILL_FLAG_TILE)
      {
        tile_bitblt(p_dc->p_curn_flinger, &orc, p_bmp, flags);
      }
      else if(flags & FILL_FLAG_STRETCH)
      {
      }
      else if(flags & FILL_FLAG_CENTER)
      {
        orc.left += (RECTW(orc) - p_bmp->width) / 2;
        orc.right = orc.left + p_bmp->width;

        orc.top += (RECTH(orc) - p_bmp->height) / 2;
        orc.bottom = orc.top + p_bmp->height;

        flinger_fill_bmp(p_dc->p_curn_flinger, &orc, p_bmp);
      }
      else
      {
        flinger_fill_bmp(p_dc->p_curn_flinger, &orc, p_bmp);
      }
    }

    p_crc = p_crc->p_next;
  }
  /* disable clipper */
  flinger_set_cliprect(p_dc->p_curn_flinger, NULL);

  //LEAVE_DRAWING(p_dc);
  dc_leave_drawing(p_dc);

  UNLOCK_GCRINFO(p_dc);

  return TRUE;
}


/*!
 Get a box with a rectangle
 */
BOOL gdi_get_bits(hdc_t hdc, rect_t *p_rc, void *p_buf, u32 pitch)
{
  rect_t orc = *p_rc;
  dc_t *p_dc = dc_hdc2pdc(hdc);
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  if(RECTW(orc) <= 0 || RECTH(orc) <= 0)
  {
    return FALSE;
  }

  /* transfer device coordinate to screen coordinate. */
  coor_dp2sp(p_dc, &orc.left, &orc.top);
  coor_dp2sp(p_dc, &orc.right, &orc.bottom);

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, &orc.left, &orc.top);
  coor_sp2sp(p_dc, &orc.right, &orc.bottom);
  normalize_rect(&orc);

  flinger_get_bits(p_dc->p_curn_flinger, &orc, p_buf, pitch);

  return TRUE;
}


/*!
 Performs a bit-block transfer from a device context into 
 another device context.
 */
void gdi_bitblt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc, u16 dx,
                u16 dy, u32 rop, u32 rop_pat)
{
  dc_t *p_sdc = NULL;
  dc_t *p_ddc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t src_orc, dst_orc, eff_rc;
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  p_sdc = dc_hdc2pdc(hsdc);
  p_ddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  if(sw <= 0)
  {
    sw = RECTW(p_sdc->dev_rc);
  }
  if(sh <= 0)
  {
    sh = RECTH(p_sdc->dev_rc);
  }

  coor_dp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  set_rect(&src_orc, sx, sy, (s16)(sx + sw), (s16)(sy + sh));

  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + sw), (s16)(dy + sh));

  if(!dc_is_mem_dc(p_sdc) && !dc_is_mem_dc(p_ddc))
  {
    generate_boundrect(&p_ddc->rc_output, &src_orc, &dst_orc);
  }
  else
  {
    p_ddc->rc_output = dst_orc;
  }

  //ENTER_DRAWING(p_ddc);
  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  

  if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
  {
    p_crc = p_ddc->ecrgn.p_tail;
  }
  else
  {
    p_crc = p_ddc->ecrgn.p_head;
  }

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_set_cliprect(p_ddc->p_curn_flinger, &eff_rc);
      flinger_bitblt(p_sdc->p_curn_flinger, 
        sx, sy, sw, sh, p_ddc->p_curn_flinger, dx, dy, rop, rop_pat);
    }

    if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
    {
      p_crc = p_crc->p_prev;
    }
    else
    {
      p_crc = p_crc->p_next;
    }
  }
  
  /* disable clipper */
  flinger_set_cliprect(p_ddc->p_curn_flinger, NULL);

  //LEAVE_DRAWING(p_ddc);
  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}


/*!
 Copies a bit-block from a a device context into another device context, 
   streches it if necessary.
 */
void gdi_stretch_blt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc,
  u16 dx, u16 dy, u16 dw, u16 dh)
{
  dc_t *p_sdc = NULL;
  dc_t *p_ddc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t src_orc, dst_orc, eff_rc;
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  p_sdc = dc_hdc2pdc(hsdc);
  p_ddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  if(sw <= 0)
  {
    sw = RECTW(p_sdc->dev_rc);
  }
  if(sh <= 0)
  {
    sh = RECTH(p_sdc->dev_rc);
  }

  coor_dp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  set_rect(&src_orc, sx, sy, (s16)(sx + sw), (s16)(sy + sh));

  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + dw), (s16)(dy + dh));

  if(!dc_is_mem_dc(p_sdc) && !dc_is_mem_dc(p_ddc))
  {
    generate_boundrect(&p_ddc->rc_output, &src_orc, &dst_orc);
  }
  else
  {
    p_ddc->rc_output = dst_orc;
  }

  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  

  if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
  {
    p_crc = p_ddc->ecrgn.p_tail;
  }
  else
  {
    p_crc = p_ddc->ecrgn.p_head;
  }

  //region must not clipped!!!
  MT_ASSERT(p_ddc->ecrgn.p_tail == p_ddc->ecrgn.p_head);
  MT_ASSERT(p_sdc->ecrgn.p_tail == p_sdc->ecrgn.p_head);
  
  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_stretch_blt(p_sdc->p_curn_flinger, sx, sy, sw, sh,
        p_ddc->p_curn_flinger, dx, dy, dw, dh);
    }

    if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
    {
      p_crc = p_crc->p_prev;
    }
    else
    {
      p_crc = p_crc->p_next;
    }
  }
  
  /* disable clipper */
  flinger_set_cliprect(p_ddc->p_curn_flinger, NULL);

  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}

void gdi_trape_blt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc,
  u16 dx, u16 dy, u16 dw, u16 dh, trape_t *p_trape)
{
  dc_t *p_sdc = NULL;
  dc_t *p_ddc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t src_orc, dst_orc, eff_rc;
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  p_sdc = dc_hdc2pdc(hsdc);
  p_ddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  if(sw <= 0)
  {
    sw = RECTW(p_sdc->dev_rc);
  }
  if(sh <= 0)
  {
    sh = RECTH(p_sdc->dev_rc);
  }

  coor_dp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  set_rect(&src_orc, sx, sy, (s16)(sx + sw), (s16)(sy + sh));

  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + dw), (s16)(dy + dh));

  if(!dc_is_mem_dc(p_sdc) && !dc_is_mem_dc(p_ddc))
  {
    generate_boundrect(&p_ddc->rc_output, &src_orc, &dst_orc);
  }
  else
  {
    p_ddc->rc_output = dst_orc;
  }

  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  

  if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
  {
    p_crc = p_ddc->ecrgn.p_tail;
  }
  else
  {
    p_crc = p_ddc->ecrgn.p_head;
  }

  //region must not clipped!!!
  MT_ASSERT(p_ddc->ecrgn.p_tail == p_ddc->ecrgn.p_head);
  MT_ASSERT(p_sdc->ecrgn.p_tail == p_sdc->ecrgn.p_head);
  
  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_trape_blt(p_sdc->p_curn_flinger, sx, sy, sw, sh,
        p_ddc->p_curn_flinger, dx, dy, dw, dh, p_trape);
    }

    if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
    {
      p_crc = p_crc->p_prev;
    }
    else
    {
      p_crc = p_crc->p_next;
    }
  }
  
  /* disable clipper */
  flinger_set_cliprect(p_ddc->p_curn_flinger, NULL);

  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}

void gdi_paint_blt(hdc_t hddc, u16 dx, u16 dy, u16 dw, u16 dh, cct_gpe_paint2opt_t *p_popt)
{
  dc_t *p_ddc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t dst_orc = {0}, eff_rc = {0};
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  p_ddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + dw), (s16)(dy + dh));

  p_ddc->rc_output = dst_orc;

  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  


  p_crc = p_ddc->ecrgn.p_head;

  //region must not clipped!!!
  MT_ASSERT(p_ddc->ecrgn.p_tail == p_ddc->ecrgn.p_head);
  
  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_paint_blt(p_ddc->p_curn_flinger, dx, dy, dw, dh, p_popt);
    }

    p_crc = p_crc->p_next;
  }
  
  /* disable clipper */
  flinger_set_cliprect(p_ddc->p_curn_flinger, NULL);

  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}

void gdi_rect_stroke(hdc_t hddc, u16 dx, u16 dy, u16 dw, u16 dh, u16 depth, u32 color)
{
  dc_t *p_ddc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t dst_orc = {0}, eff_rc = {0};
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  p_ddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + dw), (s16)(dy + dh));

  p_ddc->rc_output = dst_orc;

  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  


  p_crc = p_ddc->ecrgn.p_head;

  //region must not clipped!!!
  MT_ASSERT(p_ddc->ecrgn.p_tail == p_ddc->ecrgn.p_head);
  
  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_rect_stroke(p_ddc->p_curn_flinger, dx, dy, dw, dh, depth, color);
    }

    p_crc = p_crc->p_next;
  }
  
  /* disable clipper */
  flinger_set_cliprect(p_ddc->p_curn_flinger, NULL);

  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}

void gdi_rotate_mirror(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc,
  u16 dx, u16 dy, lld_gfx_rotator_t rotate_mod, lld_gfx_mirror_t mirror_mod)
{
  dc_t *p_sdc = NULL;
  dc_t *p_ddc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t src_orc, dst_orc, eff_rc;
  gdi_main_t *p_gdi_info = NULL;

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  p_sdc = dc_hdc2pdc(hsdc);
  p_ddc = dc_hdc2pdc(hddc);
  // chk, lock rgn
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  if(sw <= 0)
  {
    sw = RECTW(p_sdc->dev_rc);
  }
  if(sh <= 0)
  {
    sh = RECTH(p_sdc->dev_rc);
  }

  coor_dp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  set_rect(&src_orc, sx, sy, (s16)(sx + sw), (s16)(sy + sh));

  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + sw), (s16)(dy + sh));

  if(!dc_is_mem_dc(p_sdc) && !dc_is_mem_dc(p_ddc))
  {
    generate_boundrect(&p_ddc->rc_output, &src_orc, &dst_orc);
  }
  else
  {
    p_ddc->rc_output = dst_orc;
  }

  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  

  if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
  {
    p_crc = p_ddc->ecrgn.p_tail;
  }
  else
  {
    p_crc = p_ddc->ecrgn.p_head;
  }

  //region must not clipped!!!
  MT_ASSERT(p_ddc->ecrgn.p_tail == p_ddc->ecrgn.p_head);
  MT_ASSERT(p_sdc->ecrgn.p_tail == p_sdc->ecrgn.p_head);
  
  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_rotate_mirror(p_sdc->p_curn_flinger, sx, sy, sw, sh,
        p_ddc->p_curn_flinger, dx, dy, rotate_mod, mirror_mod);
    }

    if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
    {
      p_crc = p_crc->p_prev;
    }
    else
    {
      p_crc = p_crc->p_next;
    }
  }
  
  /* disable clipper */
  flinger_set_cliprect(p_ddc->p_curn_flinger, NULL);

  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}

void gdi_blt_3src(
  hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh,
  hdc_t hddc, u16 dx, u16 dy, u16 dw, u16 dh,
  hdc_t hrdc, u16 rx, u16 ry, u16 rw, u16 rh, cct_gpe_blt3opt_t *p_opt3)
{
  dc_t *p_sdc = NULL, *p_ddc = NULL, *p_rdc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t src_orc = {0}, dst_orc = {0}, eff_rc = {0};
  gdi_main_t *p_gdi_info = NULL;
  rect_t src = {0}, drc = {0}, rrc = {0};

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
  p_sdc = dc_hdc2pdc(hsdc);
  p_ddc = dc_hdc2pdc(hddc);
  p_rdc = dc_hdc2pdc(hrdc);
  
  if(dc_is_general_dc(p_ddc))
  {
    LOCK_GCRINFO(p_ddc);
    if(!dc_generate_ecrgn(p_ddc, FALSE))
    {
      UNLOCK_GCRINFO(p_ddc);
      return;
    }
  }

  /* The coordinates should be in device space. */
  if(sw <= 0)
  {
    sw = RECTW(p_sdc->dev_rc);
  }
  if(sh <= 0)
  {
    sh = RECTH(p_sdc->dev_rc);
  }

  coor_dp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  set_rect(&src_orc, sx, sy, (s16)(sx + sw), (s16)(sy + sh));

  coor_dp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);
  set_rect(&dst_orc, dx, dy, (s16)(dx + sw), (s16)(dy + sh));

  if(!dc_is_mem_dc(p_sdc) && !dc_is_mem_dc(p_ddc))
  {
    generate_boundrect(&p_ddc->rc_output, &src_orc, &dst_orc);
  }
  else
  {
    p_ddc->rc_output = dst_orc;
  }

  if(dc_enter_drawing(p_ddc) < 0)
  {
    UNLOCK_GCRINFO(p_ddc);
    return;
  }  

  if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
  {
    p_crc = p_ddc->ecrgn.p_tail;
  }
  else
  {
    p_crc = p_ddc->ecrgn.p_head;
  }

  //region must not clipped!!!
  MT_ASSERT(p_ddc->ecrgn.p_tail == p_ddc->ecrgn.p_head);
  MT_ASSERT(p_sdc->ecrgn.p_tail == p_sdc->ecrgn.p_head);
  MT_ASSERT(p_rdc->ecrgn.p_tail == p_rdc->ecrgn.p_head);
  
  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_sdc, (s16 *)&sx, (s16 *)&sy);
  coor_sp2sp(p_ddc, (s16 *)&dx, (s16 *)&dy);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_ddc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_ddc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_ddc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      src.left = sx;
      src.top = sy;
      src.right = sx + sw;
      src.bottom = sy + sh;

      drc.left = dx;
      drc.top = dy;
      drc.right = dx + dw;
      drc.bottom = dy + dh;

      rrc.left = rx;
      rrc.top = ry;
      rrc.right = rx + rw;
      rrc.bottom = rh + rh;

      flinger_blt_3src(
        p_sdc->p_curn_flinger, &src,
        p_ddc->p_curn_flinger, &drc,
        p_rdc->p_curn_flinger, &rrc, p_opt3);
    }

    if(p_ddc->p_curn_flinger == p_sdc->p_curn_flinger && dy > sy)
    {
      p_crc = p_crc->p_prev;
    }
    else
    {
      p_crc = p_crc->p_next;
    }
  }
  
  /* disable clipper */
  flinger_set_cliprect(p_ddc->p_curn_flinger, NULL);

  dc_leave_drawing(p_ddc);

  UNLOCK_GCRINFO(p_ddc);
}


void gdi_draw_pie(hdc_t hdc, pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u32 color)
{
  dc_t *p_dc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t dst_orc = {0}, eff_rc = {0};
  gdi_main_t *p_gdi_info = NULL;
  handle_t t_handle = NULL;
  u32 t_addr = 0;
  pos_t t_pos = {0};

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
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

  MT_ASSERT(radius != 0);
  MT_ASSERT(cent.x >= radius);
  MT_ASSERT(cent.y >= radius);

  dst_orc.left = cent.x - radius;
  dst_orc.right = cent.x + radius;
  dst_orc.top = cent.y - radius;
  dst_orc.bottom = cent.y + radius;
  
  /* The coordinates should be in device space. */
  coor_dp2sp(p_dc, (s16 *)&dst_orc.left, (s16 *)&dst_orc.top);
  coor_dp2sp(p_dc, (s16 *)&dst_orc.right, (s16 *)&dst_orc.bottom);

  p_dc->rc_output = dst_orc;

  if(dc_enter_drawing(p_dc) < 0)
  {
    UNLOCK_GCRINFO(p_dc);
    return;
  }  

  t_handle = gdi_create_mem_surf(dc_is_top_hdc(hdc), 2 * radius, 2 * radius, &t_addr);
  
  t_pos.x = radius;
  t_pos.y = radius;
  
  flinger_draw_pie(t_handle, t_pos, radius, from_degree, to_degree, color);

  p_crc = p_dc->ecrgn.p_head;

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, (s16 *)&dst_orc.left, (s16 *)&dst_orc.top);
  coor_sp2sp(p_dc, (s16 *)&dst_orc.right, (s16 *)&dst_orc.bottom);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_dc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_dc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);
      flinger_bitblt(t_handle,
        eff_rc.left - dst_orc.left, eff_rc.top - dst_orc.top, RECTW(eff_rc), RECTH(eff_rc),
        p_dc->p_curn_flinger, eff_rc.left, eff_rc.top, FLINGER_ROP_BLEND_SRC, 0);
    }

    p_crc = p_crc->p_next;
  }

  gdi_delete_mem_surf(t_handle, t_addr);
  
  /* disable clipper */
  flinger_set_cliprect(p_dc->p_curn_flinger, NULL);

  dc_leave_drawing(p_dc);

  UNLOCK_GCRINFO(p_dc);
}

void gdi_draw_arc(hdc_t hdc,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u16 thick, u32 color)
{
  dc_t *p_dc = NULL;
  cliprc_t *p_crc = NULL;
  rect_t dst_orc = {0}, eff_rc = {0};
  gdi_main_t *p_gdi_info = NULL;
  handle_t t_handle = NULL;
  u32 t_addr = 0;
  pos_t t_pos = {0};

  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);
  
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

  MT_ASSERT(radius != 0);
  MT_ASSERT(cent.x >= radius);
  MT_ASSERT(cent.y >= radius);
  MT_ASSERT(radius > thick);

  dst_orc.left = cent.x - radius;
  dst_orc.right = cent.x + radius;
  dst_orc.top = cent.y - radius;
  dst_orc.bottom = cent.y + radius;
  
  /* The coordinates should be in device space. */
  coor_dp2sp(p_dc, (s16 *)&dst_orc.left, (s16 *)&dst_orc.top);
  coor_dp2sp(p_dc, (s16 *)&dst_orc.right, (s16 *)&dst_orc.bottom);

  p_dc->rc_output = dst_orc;

  if(dc_enter_drawing(p_dc) < 0)
  {
    UNLOCK_GCRINFO(p_dc);
    return;
  }  

  t_handle = gdi_create_mem_surf(dc_is_top_hdc(hdc), 2 * radius, 2 * radius, &t_addr);
  
  t_pos.x = radius;
  t_pos.y = radius;
  
  flinger_draw_arc(t_handle, t_pos, radius, from_degree, to_degree, thick, color);
  
  p_crc = p_dc->ecrgn.p_head;

  /* transfer screen coordinate to surface coordinate. */
  coor_sp2sp(p_dc, (s16 *)&dst_orc.left, (s16 *)&dst_orc.top);
  coor_sp2sp(p_dc, (s16 *)&dst_orc.right, (s16 *)&dst_orc.bottom);

  while(NULL != p_crc)
  {
    if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
    {
      /* transfer screen coordinate to surface coordinate. */
      coor_sp2sp(p_dc, &eff_rc.left, &eff_rc.top);
      coor_sp2sp(p_dc, &eff_rc.right, &eff_rc.bottom);
      normalize_rect(&eff_rc);

      flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);
      flinger_bitblt(t_handle,
        eff_rc.left - dst_orc.left, eff_rc.top - dst_orc.top, RECTW(eff_rc), RECTH(eff_rc),
        p_dc->p_curn_flinger, eff_rc.left, eff_rc.top, FLINGER_ROP_BLEND_SRC, 0);
    }

    p_crc = p_crc->p_next;
  }

  gdi_delete_mem_surf(t_handle, t_addr);
  
  /* disable clipper */
  flinger_set_cliprect(p_dc->p_curn_flinger, NULL);

  dc_leave_drawing(p_dc);

  UNLOCK_GCRINFO(p_dc);
}


