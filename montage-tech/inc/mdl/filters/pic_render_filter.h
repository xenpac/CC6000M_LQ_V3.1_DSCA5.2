/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_RENDER_FILTER_H_
#define __PIC_RENDER_FILTER_H_

/*!
  jpeg pic_render filter command define
  */
typedef enum tag_pic_render_filter_cmd
{
  /*!
    render filter config
    */
  PIC_RENDER_CONFIG,
  /*!
    stop animation.
    */
  PIC_RENDER_STOP_ANIMATION,
  /*!
    is or not show 
    */
  PIC_RENDER_SHOW,
  /*!
    clear the win rect
    */
  PIC_RENDER_CLEAR,  
  /*!
    pause animation.
    */
  PIC_RENDER_PAUSE,
  /*!
    resume animation.
    */
  PIC_RENDER_RESUME,    
}pic_render_filter_cmd_t;

/*!
  pic_render filter event define
  */
typedef enum tag_pic_render_filter_evt
{
  /*!
    config read file' name
    */
  PIC_DRAW_END = PIC_RENDER_FILTER << 16,
  /*!
  	the first frame for the Dynamic picture
  */
  PIC_DRAW_START,
  /*!
    dynamic draw end
    */
  PIC_DRAW_DYNAMIC_END
}pic_render_filter_evt_t;

/*!
  animation type
  */
typedef enum
{
  /*!
    none animation.
    */
  REND_ANIM_NONE = 0,
  /*!
    left to right, pause at center.
    */
  REND_ANIM_L2R,
  /*!
    right to left, pause at center.
    */
  REND_ANIM_R2L,
  /*!
    top to bottom, pause at center.
    */
  REND_ANIM_T2B,
  /*!
    bottom to top, pause at center.
    */
  REND_ANIM_B2T,
  /*!
    mosaic.
    */
  REND_ANIM_MOS,
  /*!
    vertical shutter.
    */
  REND_ANIM_SHTV,
  /*!
    horizontal shutter.
    */
  REND_ANIM_SHTH,
  /*!
    vertical fence.
    */
  REND_ANIM_VFEN,
  /*!
    horizontal fence
    */
  REND_ANIM_HFEN, 
  /*!
    fade in
    */
  REND_ANIM_FADE,
  /*!
    box in
    */
  REND_ANIM_BOX_IN,    
  /*!
    box out
    */
  REND_ANIM_BOX_OUT,
  /*!
    box in-out
    */
  REND_ANIM_BOX_INOUT,
  /*!
    smooth move.
    */
  REND_ANIM_SMOOTH_MOVE,
  /*!
    uncover right
    */
  REND_ANIM_UNCOVER_RIGHT,
  /*!
    blinds
    */
  REND_ANIM_BLINDS,
  /*!
    square check board
    */
  REND_ANIM_SQUARE_BOARD,
  /*!
    diamond check board
    */
  REND_ANIM_DIAMOND_BOARD,
  /*!
    round check board
    */
  REND_ANIM_ROUND_BOARD,
  /*!
    fade diamond
    */
  REND_ANIM_FADE_DIAMOND,
  /*!
    3D rotate
    */
  REND_ANIM_3D_ROTATE,    
  /*!
    rend anim rand
    */
  REND_ANIM_RAND,    
}rend_anim_t;

/*!
  draw style
  */
typedef enum
{
  /*!
    center
    */
  REND_STYLE_CENTER = 0,
  /*!
    tile
    */
  REND_STYLE_TILE,
  /*!
    strech.
    */
  REND_STYLE_STRECH,
}rend_style_t;

/*!
  pic render config parameter
  */
typedef struct
{
  /*!
    window rectangle.
    */
  rect_t win_rect;
  /*!
    region handle
    */
  void *p_rgn;
  /*!
    anim style.
    */
  rend_anim_t anim;
  /*!
    rend style.
    */
  rend_style_t style;
  /*!
    frame rate per second(for animation, must less than 10). (Animation only.)
    */
  u32 frame_rate;
  /*!
    frames. (Animation only.)
    */
  u32 frames;  
  /*!
    memp
    */
  lib_memp_t *p_heap;    
  /*!
    handle
    */  
  u32 handle;     
}pic_rend_cfg_t;

/*!
  pic render config parameter
  */
typedef struct
{
  /*!
    task priority
    */
  u32 task_priority;
  /*!
    stack_size
    */
  u32 stack_size;
}pic_rend_para_t;

/*!
  pic render filter create.
  */
ifilter_t * pic_render_filter_create(void *p_para);

#endif // End for __PIC_RENDER_FILTER_H_
