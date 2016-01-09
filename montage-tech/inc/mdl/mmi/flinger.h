/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FLINGER_H__
#define __FLINGER_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
  osd config
  */
typedef struct
{
  /*!
    surface layer
    */
  surface_layer_t layer;
  /*!
    rect count.
    */
  u32 rect_cnt;    
  /*!
    Screen rect.
    */
  rect_t *p_rgn_rect;
}flinger_cfg_t;

/*!
  flinger config param.
  */
typedef struct
{
  /*!
    screen rect.
    */
  rect_t *p_flinger_rect;    
  /*!
    config.
    */
  flinger_cfg_t *p_cfg;
  /*!
    Screen format.
    */
  pix_type_t format;
  /*!
    Global palette.
    */
  palette_t *p_pal;
  /*!
    Screen default color.
    */
  u32 cdef;
  /*!
    Color key.
    */
  u32 ckey;    
}flinger_param_t;

/*!
  virtual flinger config param.
  */
typedef struct
{
  /*!
    screen rect.
    */
  rect_t *p_flinger_rect;   
  /*!
    config.
    */
  flinger_cfg_t *p_cfg;
  /*!
    pitch
    */
  u32 pitch;    
  /*!
    Screen format.
    */
  pix_type_t format;
  /*!
    Global palette.
    */
  palette_t *p_pal;
  /*!
    virtual buffer.
    */
  void *p_virtual_buf;   
}flinger_virtual_param_t;


/*!
   Defines raster operation type
  */
typedef enum
{
  /*!
     Set to the new value, erase the original.
    */
  FLINGER_ROP_SET = 0,
  /*!
     AND'd the new value with the original.
    */
  FLINGER_ROP_AND,
  /*!
     OR'd the new value with the original.
    */
  FLINGER_ROP_OR,
  /*!
     XOR'd the new value with the original.
    */
  FLINGER_ROP_XOR,
  /*!
    copy with pattern.
    */
  FLINGER_ROP_PATCOPY,
  /*!
    rop alpha blending by dst alpha.
    */
  FLINGER_ROP_BLEND_DST,    
  /*!
    rop alpha blending by src alpha.
    */
  FLINGER_ROP_BLEND_SRC,    
} flinger_rop_t;

/*!
  flinger create.
  */
void *flinger_create(flinger_param_t *p_param);

/*!
  create virtual flinger.
  */
void *flinger_create_virtual(flinger_virtual_param_t *p_param);  

/*!
  flinger delete.
  */
RET_CODE flinger_delete(void *p_flinger);

/*!
  flinger enable.
  */
void flinger_enable(void *p_flinger, BOOL is_enable);

/*!
  flinger set display.
  */
void flinger_set_display(void *p_flinger, BOOL is_display);

/*!
  set trans.
  */
void flinger_set_trans(void *p_flinger, u32 index, u8 alpha);

/*!
  set alpha.
  */
void flinger_set_alpha(void *p_flinger, u8 alpha);

/*!
  set clip-rect.
  */
void flinger_set_cliprect(void *p_flinger, rect_t *p_rc);

/*!
  set ckey.
  */
void flinger_set_colorkey(void *p_flinger, u32 ckey);

/*!
  set palette.
  */
void flinger_set_palette(void *p_flinger, u16 start, u16 len, color_t *p_entry);

/*!
  get attr.  
  */
u8 flinger_get_attr(void *p_flinger);

/*!
  get ckey.
  */
u32 flinger_get_colorkey(void *p_flinger);

/*!
  get bpp.
  */
u8 flinger_get_bpp(void *p_flinger);

/*!
  get format.  
  */
pix_type_t flinger_get_format(void *p_flinger);

/*!
  get palette addr.
  */
palette_t *flinger_get_palette_addr(void *p_flinger);

/*!
  get palette.
  */
void flinger_get_palette(void *p_flinger, u16 start, u16 len, color_t *p_entry);

/*!
  get flinger rect.
  */
rect_t *flinger_get_rect(void *p_flinger);

/*!
  offset.
  */
void flinger_offset(void *p_flinger, s16 x_off, s16 y_off);

/*!
  bit-blt.
  */
void flinger_bitblt(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy, flinger_rop_t flinger_rop, u32 rop_pat);

/*!
  fill rect.
  */
void flinger_fill_rect(void *p_flinger, rect_t *p_rc, u32 value);

/*!
  fill bitmap.
  */
void flinger_fill_bmp(void *p_flinger, rect_t *p_rc, bitmap_t *p_bmp);

/*!
  get bit.
  */
void flinger_get_bits(void *p_flinger, rect_t *p_rc, void *p_buf, u32 pitch);

/*!
  get surface.
  */
s32 flinger_get_surface(void *p_flinger, u32 idx);

/*!
  start batch.
  */
void flinger_start_batch(void *p_flinger);

/*!
  end batch.
  */
void flinger_end_batch(void *p_flinger, BOOL is_sync, rect_t *p_rect);

/*!
  dert scale
  */
void flinger_dert_scale(void *p_src_flinger, rect_t *p_src, void *p_dst_flinger, rect_t *p_dst);

/*!
  stretch blt.
  */
void flinger_stretch_blt(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy, u16 dw, u16 dh);

/*!
  trape blt.
  */
void flinger_trape_blt(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy, u16 dw, u16 dh, trape_t *p_trape);

/*!
  paint blt.
  */
void flinger_paint_blt(void *p_dst_flinger,
  u16 dx, u16 dy, u16 dw, u16 dh, cct_gpe_paint2opt_t *p_popt);

/*!
  rect stroke.
  */
void flinger_rect_stroke(void *p_dst_flinger,
  u16 dx, u16 dy, u16 dw, u16 dh, u16 depth, u32 color);
  
/*!
  rotate mirror.
  */
void flinger_rotate_mirror(void *p_src_flinger, u16 sx, u16 sy,
  u16 sw, u16 sh, void *p_dst_flinger, u16 dx, u16 dy,
  lld_gfx_rotator_t rotate_mod, lld_gfx_mirror_t mirror_mod);

/*!
  3-src blt.
  */
void flinger_blt_3src(void *p_src_flinger, rect_t *p_src,
  void *p_dst_flinger, rect_t *p_dst,
  void *p_ref_flinger, rect_t *p_ref, cct_gpe_blt3opt_t *p_opt3);

/*!
  draw pie.
  */
void flinger_draw_pie(void *p_flinger,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u32 color);

/*!
  flinger draw arc.
  */
void flinger_draw_arc(void *p_flinger,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u16 thick, u32 color);
  
#ifdef  __cplusplus
}
#endif
#endif
