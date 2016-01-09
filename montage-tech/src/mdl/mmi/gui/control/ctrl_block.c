/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_block.h
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

#include "ctrl_block.h"

/*!
  block node.
  */
typedef struct
{
  /*!
    string type.
    */
  block_str_type_t str_type;
  /*!
    string data.
    */
  u32 str_data;
  /*!
    image type.
    */
  block_img_type_t img_type;
  /*!
    image data.
    */
  u32 img_data;
  /*!
    block frame (base on control frame).
    */
  rect_t block_frame;
  /*!
    str align style
    */
  u32 align_style;
  /*!
    fstyle
    */
  u32 fstyle_idx;    
  /*!
    next node.
    */
  void *p_next;
  /*!
    do reflection?
    */
  BOOL do_reflection;
  /*!
    node context.
    */
  u32 context;    
}block_node_t;

/*!
  Thist structure defines private data of block control.
  */
typedef struct
{
  /*!
    control base.
    */
  control_t base;    
  /*!
    img data.
    */
  block_node_t *p_head;
  /*!
    new focus noed.
    */
  block_node_t *p_focus;  
  /*!
    old focus node.
    */
  block_node_t *p_old_focus;
  /*!
    draw frame.
    */
  rect_t draw_frame;
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
    draw start x. base on t_dc.
    */
  u16 draw_xoff;
  /*!
    draw start y. base on t_dc.
    */
  u16 draw_yoff;
  /*!
    new draw_xoff.
    */
  u16 new_xoff;
  /*!
    new draw_yoff
    */
  u16 new_yoff;
  /*!
    border color.
    */
  u32 border_color; 
  /*!
    is focus block scaled.
    */
  BOOL is_focus_scaled;    
} ctrl_block_t;

/*!
  animation frames.
  */
#define BLOCKS_ANIM_FRAMES  5

/*!
  invalid offset.
  */
#define BLOCKS_INVALID_OFFSET 0xFFFF

/*!
  border thickness
  */
#define BORDER_THICK    2  

/*!
  stroke depth
  */
#define STROKE_DEPTH  20

/*!
  scale ratio
  */
#define SCALE_RATIO 60  

static void _block_draw_border(control_t *p_ctrl, hdc_t hdc, rect_t *p_border)
{
  rect_t fill_rect = {0};
  ctrl_block_t *p_block = NULL;
  
  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block != NULL);
  MT_ASSERT(p_border != NULL);
  MT_ASSERT(hdc != 0);
  
  //top border.
  fill_rect.top = p_border->top - BORDER_THICK;
  fill_rect.bottom = p_border->top;
  fill_rect.left = p_border->left - BORDER_THICK;
  fill_rect.right = p_border->right + BORDER_THICK;
  gdi_fill_rect(hdc, &fill_rect, p_block->border_color);

  //left border
  fill_rect.top = p_border->top - BORDER_THICK;
  fill_rect.bottom = p_border->bottom + BORDER_THICK;
  fill_rect.left = p_border->left - BORDER_THICK;
  fill_rect.right = p_border->left;
  gdi_fill_rect(hdc, &fill_rect, p_block->border_color);

  //right border
  fill_rect.top = p_border->top - BORDER_THICK;
  fill_rect.bottom = p_border->bottom + BORDER_THICK;
  fill_rect.left = p_border->right;
  fill_rect.right = p_border->right + BORDER_THICK;
  gdi_fill_rect(hdc, &fill_rect, p_block->border_color);

  //bottom border
  fill_rect.top = p_border->bottom;
  fill_rect.bottom = p_border->bottom + BORDER_THICK;
  fill_rect.left = p_border->left - BORDER_THICK;
  fill_rect.right = p_border->right + BORDER_THICK;
  gdi_fill_rect(hdc, &fill_rect, p_block->border_color);  
}

/*!
  draw node on t_dc.
  */
static void _block_draw_node(control_t *p_ctrl, block_node_t *p_node)
{
  rsc_bitmap_t hdr_bmp = {{0}};
  bitmap_t bmp = {0}, *p_bmp = NULL;
  u8 *p_bits = NULL;
  handle_t rsc_handle = NULL;
  BOOL ret_boo = FALSE;
  rsc_palette_t hdr_pal = {{0}};
  u8 *p_entrys = NULL;
  ctrl_block_t *p_block = NULL;
  rect_t rect = {0}, mrect = {0}, str_rect = {0};
  s16 dx = 0, dy= 0;
  handle_t h_grad = NULL, h_temp = NULL;
  u32 addr_grad = 0, addr_temp = 0;
  hdc_t grad_dc = 0, temp_dc = 0 ;
  cct_gpe_paint2opt_t popt = {0};
  cct_gpe_blt3opt_t opt3 = {0};
  u32 stop_offset[4] = {0};
  u16 block_w = 0, block_h = 0;
  
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_node != NULL);

  ctrl_get_mrect(p_ctrl, &mrect);

  p_block = (ctrl_block_t *)p_ctrl;
  MT_ASSERT(p_block != NULL);

  block_w = RECTW(p_node->block_frame);
  block_h = RECTH(p_node->block_frame);

  dx = p_node->block_frame.left - p_block->draw_frame.left;
  dy = p_node->block_frame.top - p_block->draw_frame.top;

  MT_ASSERT(dx >= 0);
  MT_ASSERT(dy >= 0);

  switch(p_node->img_type)
  {
    case BLOCK_IMG_CLR:
      rect.left = dx;
      rect.top = dy;
      rect.right = rect.left + block_w;
      rect.bottom = rect.top + block_h;
      
      gdi_fill_rect(p_block->t_dc, &rect, p_node->img_data);
      break;

    case BLOCK_IMG_EXT:
      p_bmp = (bitmap_t *)(p_node->img_data);

      MT_ASSERT(p_bmp != NULL);
      break;

    case BLOCK_IMG_ID:
      rsc_handle = gui_get_rsc_handle();
      MT_ASSERT(rsc_handle != NULL);
      
      ret_boo = rsc_get_bmp(rsc_handle, p_node->img_data, &hdr_bmp, &p_bits);
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
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  if(p_bmp != NULL)
  {
    //create memory dc.
    h_temp = gdi_create_mem_surf(FALSE, p_bmp->width, p_bmp->height, &addr_temp);
    MT_ASSERT(h_temp != NULL);
    MT_ASSERT(addr_temp != 0);
    
    rect.left = 0;
    rect.top = 0;
    rect.right = p_bmp->width;
    rect.bottom = p_bmp->height;

    temp_dc = gdi_get_mem_dc(&rect, h_temp);
    MT_ASSERT(temp_dc != 0);

    ret_boo = gdi_fill_bitmap(temp_dc, &rect, (hbitmap_t)p_bmp, 0);
    MT_ASSERT(ret_boo == TRUE);

    gdi_stretch_blt(temp_dc, 0, 0, p_bmp->width, p_bmp->height,
      p_block->t_dc, dx, dy, block_w, block_h);

    str_rect.left = dx;
    str_rect.top = dy;
    str_rect.right = dx + block_w;
    str_rect.bottom = dy + block_h;
    
    switch(p_node->str_type)
    {
      case BLOCK_STR_ID:
        gui_draw_strid(p_block->t_dc, &str_rect, p_node->align_style,
          0, 0, 0, (u16)p_node->str_data, p_node->fstyle_idx, STRDRAW_NORMAL);
        break;

      case BLOCK_STR_EXT:
        gui_draw_unistr(p_block->t_dc, &str_rect, p_node->align_style,
          0, 0, 0, (u16 *)p_node->str_data, p_node->fstyle_idx, STRDRAW_NORMAL);
        break;

      default:
        MT_ASSERT(0);
        break;
    }

    if(temp_dc != 0)
    {
      gdi_release_mem_dc(temp_dc);

      gdi_delete_mem_surf(h_temp, addr_temp);
    }
  }

  //do reflection.
  if(p_node->do_reflection)
  {
    h_grad = gdi_create_mem_surf(FALSE, block_w, RECTH(mrect) / 5, &addr_grad);
    MT_ASSERT(h_grad != NULL);
    MT_ASSERT(addr_grad != 0);

    rect.left = 0;
    rect.top = 0;
    rect.right = block_w;
    rect.bottom = RECTH(mrect) / 5;
    
    grad_dc = gdi_get_mem_dc(&rect, h_grad);
    MT_ASSERT(grad_dc != 0);

    h_temp = gdi_create_mem_surf(FALSE, block_w, RECTH(mrect) / 5, &addr_temp);
    MT_ASSERT(h_temp != NULL);
    MT_ASSERT(addr_temp != 0);

    rect.left = 0;
    rect.top = 0;
    rect.right = block_w;
    rect.bottom = RECTH(mrect) / 5;
    
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
    popt.paint_cfg.paint_liner_gradt.end.y = RECTH(mrect) / 5;
    popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
    popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xD0FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop1.argb = 0x00FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop2.argb = 0x00FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop3.argb = 0x00FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop0.offset = stop_offset[0];
    popt.paint_cfg.paint_liner_gradt.stop1.offset = stop_offset[1];
    popt.paint_cfg.paint_liner_gradt.stop2.offset = stop_offset[2];
    popt.paint_cfg.paint_liner_gradt.stop3.offset = stop_offset[3];    

    gdi_paint_blt(grad_dc, 0, 0, block_w, RECTH(mrect) / 5, &popt);

    if(block_h >= RECTH(mrect) / 5)
    {
      gdi_rotate_mirror(p_block->t_dc, dx, dy + block_h - RECTH(mrect) / 5, block_w, RECTH(mrect) / 5,
        temp_dc, 0, 0, LLD_GFX_ROTATE_NONE, LLD_GFX_MIRROR_TB);
    }
    else
    {
      gdi_rotate_mirror(p_block->t_dc, dx, dy, block_w, block_h,
        temp_dc, 0, 0, LLD_GFX_ROTATE_NONE, LLD_GFX_MIRROR_TB);
    }
    
    opt3.enableDrawMult = TRUE;
    opt3.enableRop = TRUE;
    opt3.ropCfg.rop_a_id = ROP_COPYPEN;
    opt3.ropCfg.rop_c_id = ROP_COPYPEN;
    
    gdi_blt_3src(
      temp_dc, 0, 0, block_w, RECTH(mrect) / 5,
      p_block->t_dc, dx, dy + block_h, block_w, RECTH(mrect) / 5,
      grad_dc, 0, 0, block_w, RECTH(mrect) / 5,
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
  }
}

static void _block_draw_nodes(control_t *p_ctrl)
{
  ctrl_block_t *p_block = NULL;
  block_node_t *p_node = NULL;
  rect_t rect = {0}, mrect = {0};
  
  MT_ASSERT(p_ctrl != NULL);

  ctrl_get_mrect(p_ctrl, &mrect);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block != NULL);
  
  empty_rect(&p_block->draw_frame);

  //recaculate draw frame.
  p_node = p_block->p_head;
  while(p_node != NULL)
  {
    generate_boundrect(&p_block->draw_frame, &p_block->draw_frame, &p_node->block_frame);

    p_node = p_node->p_next;
  }  

  //delete old dc.
  if(p_block->t_dc != 0)
  {
    gdi_release_mem_dc(p_block->t_dc);

    gdi_delete_mem_surf(p_block->t_handle, p_block->t_addr);
  }

  //create new dc.
  p_block->t_handle = gdi_create_mem_surf(
    FALSE, RECTW(p_block->draw_frame), RECTH(p_block->draw_frame) + RECTH(mrect) / 5, &p_block->t_addr);
  MT_ASSERT(p_block->t_handle != NULL);
  MT_ASSERT(p_block->t_addr != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = RECTW(p_block->draw_frame);
  rect.bottom = RECTH(p_block->draw_frame) + RECTH(mrect) / 5;
  
  p_block->t_dc = gdi_get_mem_dc(&rect, p_block->t_handle);
  MT_ASSERT(p_block->t_dc != 0);

  gdi_fill_rect(p_block->t_dc, &rect, 0);

  p_node = p_block->p_head;
  while(p_node != NULL)
  {
    _block_draw_node(p_ctrl, p_node);

    p_node = p_node->p_next;
  }
}

/*!
  pre-scale node to saved dc.
  */
static void _block_prescale_node(control_t *p_ctrl,
  block_node_t *p_node, hdc_t hdc, s16 scale_w, s16 scale_h)
{
  ctrl_block_t *p_block = NULL;
  rect_t rect = {0};
  s16 sx = 0, sy = 0;
  s16 temp_w = 0, temp_h = 0;
  rect_t show_rect = {0}, mrect = {0};
  u16 block_w = 0, block_h = 0;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_node != NULL);

  ctrl_get_mrect(p_ctrl, &mrect);

  p_block = (ctrl_block_t *)p_ctrl;
  MT_ASSERT(p_block != NULL);
  
  block_w = RECTW(p_node->block_frame);
  block_h = RECTH(p_node->block_frame);
  
  //check block node is on show.
  show_rect.left = p_block->draw_xoff;
  show_rect.top = p_block->draw_yoff;
  show_rect.right = show_rect.left + RECTW(mrect);
  show_rect.bottom = show_rect.top + RECTH(mrect);
  if(!is_rect_intersected(&show_rect, &p_node->block_frame))
  {
    MT_ASSERT(0);
  }

  MT_ASSERT(p_block->s_dc == 0);
  MT_ASSERT(p_block->s_addr == 0);
  MT_ASSERT(p_block->s_handle == NULL);

  //create memory dc for scale node.
  temp_w = block_w + 2 * scale_w;
  if(p_node->do_reflection)
  {
    temp_h = block_h + 2 * scale_h + RECTH(mrect) / 5;
  }
  else
  {
    temp_h = block_h + 2 * scale_h;
  }
  MT_ASSERT(temp_w > 0);
  MT_ASSERT(temp_h > 0);
  
  p_block->s_handle = gdi_create_mem_surf(FALSE, temp_w, temp_h, &p_block->s_addr);
  MT_ASSERT(p_block->s_handle != NULL);
  MT_ASSERT(p_block->s_addr != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = temp_w;
  rect.bottom = temp_h;

  p_block->s_dc = gdi_get_mem_dc(&rect, p_block->s_handle);
  MT_ASSERT(p_block->s_dc != 0);

  sx = p_node->block_frame.left - p_block->draw_xoff + mrect.left - scale_w;
  sy = p_node->block_frame.top - p_block->draw_yoff + mrect.top - scale_h;

  if(sx < 0)
  {
    sx = 0;
  }

  if(sy < 0)
  {
    sy = 0;
  }

  MT_ASSERT(sx >= 0);
  MT_ASSERT(sy >= 0);

  gdi_bitblt(hdc, sx, sy, temp_w, temp_h, p_block->s_dc, 0, 0, 0, 0);
}

/*!
  scale node to hdc.
  */
static void _block_scale_node(control_t *p_ctrl,
  block_node_t *p_node, hdc_t hdc, s16 scale_w, s16 scale_h)
{
  ctrl_block_t *p_block = NULL;
  handle_t h_temp = NULL;
  u32 addr_temp = 0;
  hdc_t temp_dc = 0;
  rect_t rect = {0};
  s16 dx = 0, dy= 0, sx = 0, sy = 0, sw = 0, sh = 0;
  s16 temp_w = 0, temp_h = 0;
  rect_t show_rect = {0}, mrect = {0};
  u16 block_w = 0, block_h = 0;
  cct_gpe_blt3opt_t opt3 = {0};
  handle_t h_grad = NULL, h_ref = NULL;
  u32 addr_grad = 0, addr_ref = 0;
  hdc_t grad_dc = 0, ref_dc = 0;
  cct_gpe_paint2opt_t popt = {0};
  u32 stop_offset[4] = {0};

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_node != NULL);

  ctrl_get_mrect(p_ctrl, &mrect);

  p_block = (ctrl_block_t *)p_ctrl;
  MT_ASSERT(p_block != NULL);
  MT_ASSERT(p_block->s_addr != 0);
  MT_ASSERT(p_block->s_dc != 0);
  MT_ASSERT(p_block->s_handle != NULL);
  
  block_w = RECTW(p_node->block_frame);
  block_h = RECTH(p_node->block_frame);
  
  //check block node is on show.
  show_rect.left = p_block->draw_xoff;
  show_rect.top = p_block->draw_yoff;
  show_rect.right = show_rect.left + RECTW(mrect);
  show_rect.bottom = show_rect.top + RECTH(mrect);
  if(!is_rect_intersected(&show_rect, &p_node->block_frame))
  {
    MT_ASSERT(0);
  }

  //create memory dc for scale node.
  temp_w = block_w + 2 * scale_w;
  temp_h = block_h + 2 * scale_h;
  MT_ASSERT(temp_w > 0);
  MT_ASSERT(temp_h > 0);
  
  h_temp = gdi_create_mem_surf(FALSE, temp_w, temp_h, &addr_temp);
  MT_ASSERT(h_temp != NULL);
  MT_ASSERT(addr_temp != 0);
  
  rect.left = 0;
  rect.top = 0;
  rect.right = temp_w;
  rect.bottom = temp_h;

  temp_dc = gdi_get_mem_dc(&rect, h_temp);
  MT_ASSERT(temp_dc != 0);

  sx = p_node->block_frame.left - p_block->draw_frame.left;
  sy = p_node->block_frame.top - p_block->draw_frame.top;

  MT_ASSERT(sx >= 0);
  MT_ASSERT(sy >= 0);

  gdi_stretch_blt(p_block->t_dc, sx, sy, block_w, block_h,
    temp_dc, 0, 0, temp_w, temp_h);

  if((mrect.left + p_node->block_frame.left - p_block->draw_xoff) < scale_w)
  {
    sx = scale_w - (mrect.left + p_node->block_frame.left - p_block->draw_xoff);
    sw = temp_w - sx;
    dx = 0;
  }
  else
  {
    sx = 0;
    sw = temp_w;
    dx = (mrect.left + p_node->block_frame.left - p_block->draw_xoff) - scale_w;
  }

  if((mrect.top + p_node->block_frame.top - p_block->draw_yoff) < scale_h)
  {
    sy = scale_h - (mrect.top + p_node->block_frame.top - p_block->draw_yoff);
    sh = temp_h - sy;
    dy = 0;
  }
  else
  {
    sy = 0;
    sh = temp_h;
    dy = (mrect.top + p_node->block_frame.top - p_block->draw_yoff) - scale_h;
  }  

  //clear scale area before blt it.
  if(p_node->do_reflection)
  {
    gdi_bitblt(p_block->s_dc, sx, sy, sw, sh + RECTH(mrect) / 5, hdc, dx, dy, 0, 0);
  }
  else
  {
    gdi_bitblt(p_block->s_dc, sx, sy, sw, sh, hdc, dx, dy, 0, 0);
  } 

  gdi_bitblt(temp_dc, sx, sy, sw, sh, hdc, dx, dy, FLINGER_ROP_BLEND_SRC, 0);

  if(p_node->do_reflection)
  {
    h_grad = gdi_create_mem_surf(FALSE, temp_w, RECTH(mrect) / 5, &addr_grad);
    MT_ASSERT(h_grad != NULL);
    MT_ASSERT(addr_grad != 0);

    rect.left = 0;
    rect.top = 0;
    rect.right = temp_w;
    rect.bottom = RECTH(mrect) / 5;

    grad_dc = gdi_get_mem_dc(&rect, h_grad);
    MT_ASSERT(grad_dc != 0);

    h_ref = gdi_create_mem_surf(FALSE, temp_w, RECTH(mrect) / 5, &addr_ref);
    MT_ASSERT(h_ref != NULL);
    MT_ASSERT(addr_ref != 0);

    rect.left = 0;
    rect.top = 0;
    rect.right = temp_w;
    rect.bottom = RECTH(mrect) / 5;

    ref_dc = gdi_get_mem_dc(&rect, h_ref);
    MT_ASSERT(ref_dc != 0);
    
    stop_offset[0] = 0;
    stop_offset[1] = 4095;
    stop_offset[2] = 4095;
    stop_offset[3] = 4095;
    
    popt.paint_cfg.paint_type = VG_PAINT_TYPE_LINEAR_GRADIENT;
    popt.paint_cfg.paint_liner_gradt.begin.x = 0;
    popt.paint_cfg.paint_liner_gradt.begin.y = 0;
    popt.paint_cfg.paint_liner_gradt.end.x = 0;
    popt.paint_cfg.paint_liner_gradt.end.y = RECTH(mrect) / 5;
    popt.paint_cfg.paint_liner_gradt.spread_mod = VG_COLOR_RAMP_SPREAD_REPEAT;
    popt.paint_cfg.paint_liner_gradt.stop0.argb = 0xD0FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop1.argb = 0x00FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop2.argb = 0x00FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop3.argb = 0x00FFFFFF;
    popt.paint_cfg.paint_liner_gradt.stop0.offset = stop_offset[0];
    popt.paint_cfg.paint_liner_gradt.stop1.offset = stop_offset[1];
    popt.paint_cfg.paint_liner_gradt.stop2.offset = stop_offset[2];
    popt.paint_cfg.paint_liner_gradt.stop3.offset = stop_offset[3];    

    gdi_paint_blt(grad_dc, 0, 0, temp_w, RECTH(mrect) / 5, &popt);

    if(temp_h >= RECTH(mrect) / 5)
    {
      gdi_rotate_mirror(temp_dc, 0, temp_h  - RECTH(mrect) / 5, temp_w, RECTH(mrect) / 5,
        ref_dc, 0, 0, LLD_GFX_ROTATE_NONE, LLD_GFX_MIRROR_TB);
    }
    else
    {
      gdi_rotate_mirror(temp_dc, 0, 0, temp_w, temp_h,
        ref_dc, 0, 0, LLD_GFX_ROTATE_NONE, LLD_GFX_MIRROR_TB);
    }

    opt3.enableDrawMult = TRUE;
    opt3.enableBld = TRUE;
    opt3.blendCfg.src_blend_fact = GL_SRC_ALPHA;
    opt3.blendCfg.dst_blend_fact = GL_ONE_MINUS_SRC_ALPHA;

    gdi_blt_3src(
      ref_dc, 0, 0, temp_w, RECTH(mrect) / 5,
      hdc, dx, dy + temp_h, temp_w, RECTH(mrect) / 5,
      grad_dc, 0, 0, temp_w, RECTH(mrect) / 5,
      &opt3);

    if(ref_dc != 0)
    {
      gdi_release_mem_dc(ref_dc);

      gdi_delete_mem_surf(h_ref, addr_ref);
    }  
        
    if(grad_dc != 0)
    {
      gdi_release_mem_dc(grad_dc);

      gdi_delete_mem_surf(h_grad, addr_grad);
    }      
  }

  if(temp_dc != 0)
  {
    gdi_release_mem_dc(temp_dc);

    gdi_delete_mem_surf(h_temp, addr_temp);
  }

  if(p_block->s_dc != 0)
  {
    gdi_release_mem_dc(p_block->s_dc);

    gdi_delete_mem_surf(p_block->s_handle, p_block->s_addr);

    p_block->s_dc = 0;
    p_block->s_handle = NULL;
    p_block->s_addr = 0;
  }
}

//no animation, but focus block is scaled.
static void _block_draw_hlight(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_block_t *p_block = NULL;
  rect_t mrect = {0}, border = {0}, frame = {0};
  u16 i = 0;
 
  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;
  
  MT_ASSERT(p_block != NULL);

  MT_ASSERT(p_block->p_old_focus == NULL);
  MT_ASSERT(p_block->new_xoff == BLOCKS_INVALID_OFFSET);
  MT_ASSERT(p_block->new_yoff == BLOCKS_INVALID_OFFSET);

  ctrl_get_mrect(p_ctrl, &mrect);
  ctrl_get_frame(p_ctrl, &frame);

  //paint background.
  gui_paint_frame(hdc, p_ctrl);

  if(!p_block->is_focus_scaled)
  {
    //do animation to scale up focus.
    for(i = 0; i <= BLOCKS_ANIM_FRAMES; i++)
    {
      gdi_start_batch(dc_is_top_hdc(hdc));
    
      //paint background.
      gui_paint_frame(hdc, p_ctrl);  

      //pre scale node.
      _block_prescale_node(p_ctrl, p_block->p_focus, hdc,
        RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES,
        RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES);

      //blt from t_dc to hdc.
      gdi_bitblt(p_block->t_dc, p_block->draw_xoff, p_block->draw_yoff, RECTW(mrect), RECTH(mrect),
        hdc, mrect.left, mrect.top, FLINGER_ROP_BLEND_SRC, 0);
      
      _block_scale_node(p_ctrl, p_block->p_focus, hdc,
        RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES,
        RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES);

      border.left = p_block->p_focus->block_frame.left -
        RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
      border.top = p_block->p_focus->block_frame.top -
        RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;
      border.right = p_block->p_focus->block_frame.right +
        RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
      border.bottom = p_block->p_focus->block_frame.bottom +
        RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;

      offset_rect(&border, mrect.left, mrect.top);
      _block_draw_border(p_ctrl, hdc, &border);     

      gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &frame);
    }  
  }
  else
  {
    //pre scale node.
    _block_prescale_node(p_ctrl,
      p_block->p_focus, hdc, RECTW(mrect) / SCALE_RATIO, RECTH(mrect) / SCALE_RATIO);

    //blt from t_dc to hdc.
    gdi_bitblt(p_block->t_dc, p_block->draw_xoff, p_block->draw_yoff, RECTW(mrect), RECTH(mrect),
      hdc, mrect.left, mrect.top, FLINGER_ROP_BLEND_SRC, 0);

    MT_ASSERT(p_block->p_focus != NULL);

    _block_scale_node(p_ctrl,
      p_block->p_focus, hdc, RECTW(mrect) / SCALE_RATIO, RECTH(mrect) / SCALE_RATIO);
    border.left = 
      p_block->p_focus->block_frame.left - RECTW(mrect) / SCALE_RATIO - p_block->draw_xoff;
    border.top = 
      p_block->p_focus->block_frame.top - RECTH(mrect) / SCALE_RATIO - p_block->draw_yoff;
    border.right = 
      p_block->p_focus->block_frame.right + RECTW(mrect) / SCALE_RATIO - p_block->draw_xoff;
    border.bottom = 
      p_block->p_focus->block_frame.bottom + RECTH(mrect) / SCALE_RATIO - p_block->draw_yoff;

    offset_rect(&border, mrect.left, mrect.top);
    _block_draw_border(p_ctrl, hdc, &border);
  }
  
  p_block->is_focus_scaled = TRUE;

  gdi_rect_stroke(hdc,
    border.left,
    border.top,
    RECTW(border),
    RECTH(border), 
    STROKE_DEPTH, p_block->border_color);      
}

//no animation and focus block is not scaled.
static void _block_draw_normal(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_block_t *p_block = NULL;
  rect_t mrect = {0}, border = {0}, frame = {0};
  u16 i = 0;
 
  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;
  
  MT_ASSERT(p_block != NULL);

  MT_ASSERT(p_block->p_old_focus == NULL);
  MT_ASSERT(p_block->new_xoff == BLOCKS_INVALID_OFFSET);
  MT_ASSERT(p_block->new_yoff == BLOCKS_INVALID_OFFSET);

  ctrl_get_frame(p_ctrl, &frame);
  ctrl_get_mrect(p_ctrl, &mrect);

  if(p_block->is_focus_scaled)
  {
    //do animation to scale down focus.
    for(i = 0; i <= BLOCKS_ANIM_FRAMES; i++)
    {
      gdi_start_batch(dc_is_top_hdc(hdc));

      //paint background.
      gui_paint_frame(hdc, p_ctrl);  

      //pre scale node.
      _block_prescale_node(p_ctrl, p_block->p_focus, hdc,
        RECTW(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES,
        RECTH(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES);    

      //blt from t_dc to hdc.
      gdi_bitblt(p_block->t_dc, p_block->draw_xoff, p_block->draw_yoff, RECTW(mrect), RECTH(mrect),
        hdc, mrect.left, mrect.top, FLINGER_ROP_BLEND_SRC, 0);
      
      _block_scale_node(p_ctrl, p_block->p_focus, hdc,
        RECTW(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES,
        RECTH(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES);

      border.left = p_block->p_focus->block_frame.left + RECTW(mrect) / SCALE_RATIO -
        RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
      border.top = p_block->p_focus->block_frame.top + RECTH(mrect) / SCALE_RATIO -
        RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;
      border.right = p_block->p_focus->block_frame.right + RECTW(mrect) / SCALE_RATIO -
        RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
      border.bottom = p_block->p_focus->block_frame.bottom + RECTH(mrect) / SCALE_RATIO -
        RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;


      offset_rect(&border, mrect.left, mrect.top);
      _block_draw_border(p_ctrl, hdc, &border);      

      gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &frame);
    }    
  }
  else
  {
    //paint background.
    gui_paint_frame(hdc, p_ctrl);
  
    //blt from t_dc to hdc.
    gdi_bitblt(p_block->t_dc, p_block->draw_xoff, p_block->draw_yoff, RECTW(mrect), RECTH(mrect),
      hdc, mrect.left, mrect.top, FLINGER_ROP_BLEND_SRC, 0);
  }
  
  p_block->is_focus_scaled = FALSE;
}

//scale down old focus block & scale up new focus block.
static void _block_draw_anim(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_block_t *p_block = NULL;
  rect_t mrect = {0}, frame = {0}, border = {0};
  u16 i = 0;
  s16 new_xoff = 0, new_yoff = 0, old_xoff = 0, old_yoff = 0, temp_xoff = 0, temp_yoff = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block != NULL);

  MT_ASSERT(p_block->p_old_focus != NULL);

  ctrl_get_mrect(p_ctrl, &mrect);
  ctrl_get_frame(p_ctrl, &frame);

  offset_rect(&frame, 0 - frame.left, 0 - frame.top);

  //scale down old focus.
  for(i = 0; i <= BLOCKS_ANIM_FRAMES; i++)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //paint background.
    gui_paint_frame(hdc, p_ctrl);  

    //pre scale node.
    _block_prescale_node(p_ctrl, p_block->p_old_focus, hdc,
      RECTW(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES,
      RECTH(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES);    

    //blt from t_dc to hdc.
    gdi_bitblt(p_block->t_dc, p_block->draw_xoff, p_block->draw_yoff, RECTW(mrect), RECTH(mrect),
      hdc, mrect.left, mrect.top, FLINGER_ROP_BLEND_SRC, 0);
    
    _block_scale_node(p_ctrl, p_block->p_old_focus, hdc,
      RECTW(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES,
      RECTH(mrect) / SCALE_RATIO * (BLOCKS_ANIM_FRAMES - i) / BLOCKS_ANIM_FRAMES);

    border.left = p_block->p_old_focus->block_frame.left +
      (p_block->p_focus->block_frame.left - p_block->p_old_focus->block_frame.left) *
      i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
    border.top = p_block->p_old_focus->block_frame.top +
      (p_block->p_focus->block_frame.top - p_block->p_old_focus->block_frame.top) *
      i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;
    border.right = p_block->p_old_focus->block_frame.right +
      (p_block->p_focus->block_frame.right - p_block->p_old_focus->block_frame.right) *
      i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
    border.bottom = p_block->p_old_focus->block_frame.bottom +
      (p_block->p_focus->block_frame.bottom - p_block->p_old_focus->block_frame.bottom) *
      i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;

    offset_rect(&border, mrect.left, mrect.top);
    _block_draw_border(p_ctrl, hdc, &border);      

    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &frame);
  }
  
  new_xoff = old_xoff = p_block->draw_xoff;
  new_yoff = old_yoff = p_block->draw_yoff;

  if(p_block->new_xoff != BLOCKS_INVALID_OFFSET)
  {
    new_xoff = p_block->new_xoff;
    p_block->new_xoff = BLOCKS_INVALID_OFFSET;
  }

  if(p_block->new_yoff != BLOCKS_INVALID_OFFSET)
  {
    new_yoff = p_block->new_yoff;
    p_block->new_yoff = BLOCKS_INVALID_OFFSET;
  }

  //move  old xoff/yoff ----->  new xoff/yoff
  if((new_xoff != old_xoff) || (new_yoff != old_yoff))
  {
    for(i = 0; i <= BLOCKS_ANIM_FRAMES; i++)
    {
      temp_xoff = old_xoff + (new_xoff - old_xoff) * i / BLOCKS_ANIM_FRAMES;
      temp_yoff = old_yoff + (new_yoff - old_yoff) * i / BLOCKS_ANIM_FRAMES;

      gdi_start_batch(dc_is_top_hdc(hdc));
      
      //paint background.
      gui_paint_frame(hdc, p_ctrl);  

      //blt from t_dc to hdc.
      gdi_bitblt(p_block->t_dc, temp_xoff, temp_yoff, RECTW(mrect), RECTH(mrect),
        hdc, mrect.left, mrect.top, FLINGER_ROP_BLEND_SRC, 0);

      border.left = p_block->p_focus->block_frame.left - temp_xoff;
      border.top = p_block->p_focus->block_frame.top - temp_yoff;
      border.right = p_block->p_focus->block_frame.right - temp_xoff;
      border.bottom = p_block->p_focus->block_frame.bottom - temp_yoff;

      offset_rect(&border, mrect.left, mrect.top);
      _block_draw_border(p_ctrl, hdc, &border);           

      gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &frame);
    }
  }

  p_block->draw_xoff = new_xoff;
  p_block->draw_yoff = new_yoff;

  //scale up new focus
  for(i = 0; i <= BLOCKS_ANIM_FRAMES; i++)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));
  
    //paint background.
    gui_paint_frame(hdc, p_ctrl);  

    //pre scale node.
    _block_prescale_node(p_ctrl, p_block->p_focus, hdc,
      RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES,
      RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES);

    //blt from t_dc to hdc.
    gdi_bitblt(p_block->t_dc, p_block->draw_xoff, p_block->draw_yoff, RECTW(mrect), RECTH(mrect),
      hdc, mrect.left, mrect.top, FLINGER_ROP_BLEND_SRC, 0);
    
    _block_scale_node(p_ctrl, p_block->p_focus, hdc,
      RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES,
      RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES);

    border.left = p_block->p_focus->block_frame.left -
      RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
    border.top = p_block->p_focus->block_frame.top -
      RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;
    border.right = p_block->p_focus->block_frame.right +
      RECTW(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_xoff;
    border.bottom = p_block->p_focus->block_frame.bottom +
      RECTH(mrect) / SCALE_RATIO * i / BLOCKS_ANIM_FRAMES - p_block->draw_yoff;

    offset_rect(&border, mrect.left, mrect.top);
    _block_draw_border(p_ctrl, hdc, &border);     

    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &frame);
  }

  gdi_rect_stroke(hdc,
    border.left,
    border.top,
    RECTW(border),
    RECTH(border), 
    STROKE_DEPTH, p_block->border_color);

  p_block->is_focus_scaled = TRUE;
}


static void _block_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_block_t *p_block = NULL;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  
  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block != NULL);
  MT_ASSERT(p_block->t_dc != 0);
  MT_ASSERT(p_block->t_handle != NULL);
  MT_ASSERT(p_block->t_addr != 0);

  attr = ctrl_get_attr(p_ctrl);

  if(attr == OBJ_ATTR_HL)
  {
    //scroll left/right.
    if(p_block->p_old_focus != NULL)
    {
      _block_draw_anim(p_ctrl, hdc);
    }
    else
    {
      _block_draw_hlight(p_ctrl, hdc);
    }
  }
  else
  {
    MT_ASSERT(p_block->p_old_focus == NULL);
    
    _block_draw_normal(p_ctrl, hdc);
  }
  
  p_block->p_old_focus = NULL;
}

RET_CODE block_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_block_t *p_default_data = NULL;
  
  register_info.data_size = sizeof(ctrl_block_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_data = (ctrl_block_t *)register_info.p_default_ctrl;
  
  // initialize the default control of bitmap class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER;
  p_default_ctrl->p_proc = block_class_proc;
  p_default_ctrl->p_paint = _block_draw;

  // initalize the default data of bitmap class
  p_default_data->p_old_focus = NULL;
  p_default_data->new_xoff = BLOCKS_INVALID_OFFSET;
  p_default_data->new_yoff = BLOCKS_INVALID_OFFSET;
  p_default_data->is_focus_scaled = FALSE;
  
  empty_rect(&p_default_data->draw_frame);
  
  if(ctrl_register_ctrl_class(CTRL_BLOCK, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}

void block_set_border_color(control_t *p_ctrl, u32 color)
{
  ctrl_block_t *p_block = NULL;
  
  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block != NULL);

  p_block->border_color = color;
}

void block_update(control_t *p_ctrl)
{
  _block_draw_nodes(p_ctrl);
}

u32 block_get_focus_context(control_t *p_ctrl)
{
  ctrl_block_t *p_block = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_ctrl->ctrl_type == CTRL_BLOCK);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block->p_focus != NULL);
  
  return p_block->p_focus->context;
}

void block_add_node(control_t *p_ctrl,
  block_img_type_t img_type, u32 img_data,
  block_str_type_t str_type, u32 str_data, u32 fstyle_idx, u32 align_style,
  u16 x, u16 y, u16 w, u16 h, BOOL do_reflection, u32 context)
{
  ctrl_block_t *p_block = NULL;
  block_node_t *p_node = NULL, *p_temp = NULL, *p_tail = NULL;
  rect_t node_rect = {0};
  
  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block != NULL);

  node_rect.left = x;
  node_rect.top = y;
  node_rect.right = x + w;
  node_rect.bottom = y + h;

  normalize_rect(&node_rect);

  p_temp = p_block->p_head;
  
  while(p_temp != NULL)
  {
    if(is_rect_intersected(&node_rect, &p_temp->block_frame))
    {
      //block frame intersected.
      MT_ASSERT(0);
    }

    p_tail = p_temp;
    p_temp = p_temp->p_next;
  }
  
  p_node = (block_node_t *)mmi_alloc_buf(sizeof(block_node_t));
  MT_ASSERT(p_node != NULL);
  memset(p_node, 0, sizeof(block_node_t));
  
  p_node->img_type = img_type;
  p_node->img_data = img_data;
  p_node->img_data = img_data;
  p_node->str_data = str_data;
  p_node->do_reflection = do_reflection;
  p_node->context = context;
  p_node->align_style = align_style;
  p_node->fstyle_idx = fstyle_idx;
  p_node->p_next = NULL;
  copy_rect(&p_node->block_frame, &node_rect);
  normalize_rect(&p_node->block_frame);

  if(p_tail == NULL)
  {
    p_block->p_head = p_node;

    MT_ASSERT(p_block->p_focus == NULL);
    MT_ASSERT(p_block->p_old_focus == NULL);
    
    p_block->p_focus = p_node;
  }
  else
  {
    p_tail->p_next = p_node;
  }
}

static RET_CODE on_block_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_block_t *p_block = NULL;
  block_node_t *p_temp = NULL, *p_new_focus = NULL;
  rect_t *p_focus_frame = {0}, *p_temp_frame = {0};
  u16 i = 0, intimacy_x = 0, intimacy_y = 0, new_intimacy_x = 0, new_intimacy_y = 0;
  rect_t mrect = {0};
  
  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;

  MT_ASSERT(p_block != NULL);
  MT_ASSERT(p_block->p_focus != NULL);
  MT_ASSERT(p_block->p_old_focus == NULL);
  MT_ASSERT(p_block->new_xoff == BLOCKS_INVALID_OFFSET);
  MT_ASSERT(p_block->new_yoff == BLOCKS_INVALID_OFFSET);

  ctrl_get_mrect(p_ctrl, &mrect);

  //try to found the dst node.
  p_focus_frame = &p_block->p_focus->block_frame;

  p_temp = p_block->p_head;
  switch(msg)
  {
    case MSG_FOCUS_LEFT:
      while(p_temp != NULL)
      {
        p_temp_frame = &p_temp->block_frame;

        if(p_temp_frame->right < p_focus_frame->left)
        {
          if(((p_temp_frame->top <= p_focus_frame->bottom) 
          && (p_temp_frame->top >= p_focus_frame->top)) ||
          ((p_temp_frame->bottom <= p_focus_frame->bottom) 
          && (p_temp_frame->bottom >= p_focus_frame->top)))
          {
            new_intimacy_x = p_focus_frame->left - p_temp_frame->right;
            new_intimacy_y = ABS((p_temp_frame->top + RECTHP(p_temp_frame) / 2) 
              - (p_focus_frame->top + RECTHP(p_focus_frame) / 2));
            
            if((p_new_focus == NULL) ||
              (new_intimacy_x < intimacy_x) ||
              ((new_intimacy_x == intimacy_x) && (new_intimacy_y < intimacy_y)))
            {
              intimacy_x = new_intimacy_x;
              intimacy_y = new_intimacy_y;

              p_new_focus = p_temp;
            }
          }
        }

        p_temp = p_temp->p_next;
        i++;
      }
      break;

    case MSG_FOCUS_RIGHT:
      while(p_temp != NULL)
      {
        p_temp_frame = &p_temp->block_frame;

        if(p_temp_frame->left > p_focus_frame->right)
        {
          if(((p_temp_frame->top <= p_focus_frame->bottom) 
          && (p_temp_frame->top >= p_focus_frame->top)) ||
          ((p_temp_frame->bottom <= p_focus_frame->bottom) 
          && (p_temp_frame->bottom >= p_focus_frame->top)))
          {
            new_intimacy_x = p_temp_frame->left - p_focus_frame->right;
            new_intimacy_y = ABS((p_temp_frame->top + RECTHP(p_temp_frame) / 2) 
              - (p_focus_frame->top + RECTHP(p_focus_frame) / 2));
            
            if((p_new_focus == NULL) ||
              (new_intimacy_x < intimacy_x) ||
              ((new_intimacy_x == intimacy_x) && (new_intimacy_y < intimacy_y)))
            {
              intimacy_x = new_intimacy_x;
              intimacy_y = new_intimacy_y;

              p_new_focus = p_temp;
            }
          }
        }

        p_temp = p_temp->p_next;
        i++;
      }    
      break;

    case MSG_FOCUS_UP:
      while(p_temp != NULL)
      {
        p_temp_frame = &p_temp->block_frame;

        if(p_temp_frame->bottom < p_focus_frame->top)
        {
          if(((p_temp_frame->left <= p_focus_frame->right) 
          && (p_temp_frame->left >= p_focus_frame->left)) ||
          ((p_temp_frame->right <= p_focus_frame->right) 
          && (p_temp_frame->right >= p_focus_frame->left)))
          {
            new_intimacy_x = ABS((p_temp_frame->left + RECTWP(p_temp_frame) / 2) 
              - (p_focus_frame->left + RECTWP(p_focus_frame) / 2));
            new_intimacy_y = p_focus_frame->top - p_temp_frame->bottom;
            
            if((p_new_focus == NULL) ||
              (new_intimacy_y < intimacy_y) ||
              ((new_intimacy_y == intimacy_y) && (new_intimacy_x < intimacy_x)))
            {
              intimacy_x = new_intimacy_x;
              intimacy_y = new_intimacy_y;

              p_new_focus = p_temp;
            }
          }
        }

        p_temp = p_temp->p_next;
        i++;
      }      
      break;

    case MSG_FOCUS_DOWN:
      while(p_temp != NULL)
      {
        p_temp_frame = &p_temp->block_frame;

        if(p_temp_frame->top > p_focus_frame->bottom)
        {
          if(((p_temp_frame->left <= p_focus_frame->right) 
          && (p_temp_frame->left >= p_focus_frame->left)) ||
          ((p_temp_frame->right <= p_focus_frame->right) 
          && (p_temp_frame->right >= p_focus_frame->left)))
          {
            new_intimacy_x = ABS((p_temp_frame->left + RECTWP(p_temp_frame) / 2) 
              - (p_focus_frame->left + RECTWP(p_focus_frame) / 2));
            new_intimacy_y = p_temp_frame->top - p_focus_frame->bottom;
            
            if((p_new_focus == NULL) ||
              (new_intimacy_y < intimacy_y) ||
              ((new_intimacy_y == intimacy_y) && (new_intimacy_x < intimacy_x)))
            {
              intimacy_x = new_intimacy_x;
              intimacy_y = new_intimacy_y;

              p_new_focus = p_temp;

            }
          }
        }

        p_temp = p_temp->p_next;
        i++;
      }      
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  if(p_new_focus != NULL)
  {
    p_block->p_old_focus = p_block->p_focus;
    p_block->p_focus = p_new_focus;

    switch(msg)
    {
      case MSG_FOCUS_LEFT:
      case MSG_FOCUS_UP:
        if(p_new_focus->block_frame.left < p_block->draw_xoff)
        {
          p_block->new_xoff = p_new_focus->block_frame.left;
        }
        else if(p_new_focus->block_frame.right > (p_block->draw_xoff + RECTW(mrect)))
        {
          p_block->new_xoff =  (p_new_focus->block_frame.right - RECTW(mrect));
        }

        if(p_new_focus->block_frame.top < p_block->draw_yoff)
        {
          p_block->new_yoff = p_new_focus->block_frame.top;
        }
        else if(p_new_focus->block_frame.bottom > (p_block->draw_yoff + RECTH(mrect)))
        {
          p_block->new_yoff = (p_new_focus->block_frame.bottom - RECTH(mrect));
        }
        break;

      case MSG_FOCUS_RIGHT:
      case MSG_FOCUS_DOWN:
        if(p_new_focus->block_frame.right > (p_block->draw_xoff + RECTW(mrect)))
        {
          p_block->new_xoff =  (p_new_focus->block_frame.right - RECTW(mrect));
        }
        else if(p_new_focus->block_frame.left < p_block->draw_xoff)
        {
          p_block->new_xoff = p_new_focus->block_frame.left;
        }        

        if(p_new_focus->block_frame.bottom > (p_block->draw_yoff + RECTH(mrect)))
        {
          p_block->new_yoff = (p_new_focus->block_frame.bottom - RECTH(mrect));
        }
        else if(p_new_focus->block_frame.top < p_block->draw_yoff)
        {
          p_block->new_yoff = p_new_focus->block_frame.top;
        }      
        
        break;
    }

    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_block_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  ctrl_block_t *p_block = NULL;
  block_node_t *p_node = NULL, *p_prev = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_block = (ctrl_block_t *)p_ctrl;

  p_node = p_block->p_head;
  
  while(p_node != NULL)
  {
    p_prev = p_node;

    p_node = p_node->p_next;

    mmi_free_buf(p_prev);
  }

  if(p_block->t_dc != 0)
  {
    gdi_release_mem_dc(p_block->t_dc);

    gdi_delete_mem_surf(p_block->t_handle, p_block->t_addr);
  }  

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}

BEGIN_CTRLPROC(block_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_block_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_block_change_focus)
  ON_COMMAND(MSG_FOCUS_UP, on_block_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_block_change_focus)
  ON_COMMAND(MSG_DESTROY, on_block_destroy)
END_CTRLPROC(block_class_proc, ctrl_default_proc)
