/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GDI_PRIVATE_H__
#define __DDI_PRIVATE_H__

/*!
  screen info.
  */
typedef struct
{
  /*!
    surface layer.
    */
  surface_layer_t layer;    
  /*!
    Screen dc.
    */
  dc_t screen_dc;
  /*!
    Screen surface.
    */
  void *p_flinger; 
}screen_t;

/*!
  Structure for global variable.
  */
typedef struct
{
  /*!
    is top screen support.
    */
  BOOL is_dual_osd;    
  /*!
    screen rect.
    */
  rect_t screen_rect;
  /*!
    top screen.
    */
  screen_t top;
  /*!
    bottom screen.
    */
  screen_t bot;
  /*!
    Screen format.
    */
  pix_type_t screen_format;
  /*!
    Screen default color.
    */
  u32 screen_cdef;
  /*!
    Screen color key.
    */
  u32 screen_ckey;
  /*!
    Global palette.
    */
  palette_t global_pal;    
  /*!
    DC slot.
    */
  dc_t *p_dc_slot;
  /*!
    DC slot count.
    */
  u16 dc_slot_cnt;
  /*!
    Clip rectangle heap.
    */
  lib_memf_t cliprc_heap;
  /*!
    Clip rectangle heap address.
    */
  void *p_cliprc_heap_addr;
  /*!
    DC count.
    */
  u32 dc_cnt;
  /*!
    DC max count.
    */
  u32 dc_max_cnt;
  /*!
    Clip rectangle max count.
    */
  u32 cliprc_max_cnt;
  /*!
    Clip rectangle current count.
    */
  u32 cliprc_curn_cnt;
  /*!
    Virtual surface buffer address.
    */
  void * p_vsurf_buf;
  /*!
    Virtual surface buffer size.
    */
  u32 vsurf_buf_size;
  /*!
    animation new buffer address. for save new surface.
    */
  void * p_anim_new;
  /*!
    animation surface buffer size.
    */
  u32 anim_new_size;
  /*!
    animation old buffer addr. for save old surface.
    */
  void *p_anim_old;
  /*!
    animation new buffer size. for save old surface.
    */
  u32 anim_old_size;    
  /*!
    animation tmp buffer addr. for save tmp surface.
    */
  void *p_anim_tmp;
  /*!
    animation tmp buffer size. for save tmp surface.
    */
  u32 anim_tmp_size;    
}gdi_main_t;

/*!
  dc enter drawing.
  */
RET_CODE dc_enter_drawing(dc_t *p_dc);

/*!
  dc leave drawing.
  */
void dc_leave_drawing(dc_t *p_dc);

/*!
  dc generate eclip-region.
  */
BOOL dc_generate_ecrgn(dc_t *p_dc, BOOL is_is_force);

#endif

