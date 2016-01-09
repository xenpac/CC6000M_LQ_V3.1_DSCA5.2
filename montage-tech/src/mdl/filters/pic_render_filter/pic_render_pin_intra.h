/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_RENDER_PIN_INTRA_H_
#define __PIC_RENDER_PIN_INTRA_H_

/*!
  animation parameter
  */
typedef struct
{
  /*!
    display format
    */
  pix_fmt_t fmt;  
  /*!
    last ticks
    */
  u32 last_ticks;
  /*!
    step time(tick)
    */
  u32 step_time;
  /*!
    animation media type
    */
  media_type_t media_type;  
  /*!
    cache buffer addr
    */
  void *p_cache;  
  /*!
    fill rect
    */
  rect_t fill_rect;
  /*!
    draw rect
    */
  rect_t rend_rect;
  /*!
    image width
    */
  u32 image_width;
  /*!
    image height
    */
  u32 image_height; 
  /*!
    curn step
    */
  u32 curn_frame;   
  /*!
    pitch
    */
  u32 pitch;
  /*!
    pal
    */
  void *pal_entry;
  /*!
    pal number
    */
  u32 pal_num;
  /*!
    bg color enable.
    */
  BOOL use_bg_color;    
  /*!
    background color.
    */
  u32 bg_color;  
  /*!
    enable color key.
    */
  BOOL enable_ckey;
  /*!
    color key
    */
  u32 color_key;    
  /*!
    random array. (Just for mosaic used.)
    */
  u32 *p_array;   
  /*!
    region
    */
  void *p_rgn;    
}anim_param_t;

/*!
  render task state
  */
typedef enum
{
  /*!
    idle state
    */
  PIC_REND_SM_IDLE = 0,
  /*!
    animation state
    */
  PIC_REND_SM_ANIM,
  /*!
    animation pause.
    */
  PIC_REND_SM_ANIM_PAUSE,    
}pic_rend_sm_t;

/*!
  pic render pin private data
  */
typedef struct
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    osd rgn
    */
  void *p_rgn;
  /*!
    render window coordinates, it should be smaller than region rect.
    */
  rect_t win_rect;
  /*!
    task priority
    */
  u32 priority;
  /*!
    stack
    */
  void *p_stack;
  /*!
    msgq id.
    */
  u32 msgq_id;    
  /*!
    task state
    */
  pic_rend_sm_t sm;    
  /*!
    animation parameter.
    */
  anim_param_t anim_param;
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
    rend sem
    */
  os_sem_t rend_sem;
  /*!
   is show
    */
  BOOL is_show;  
  /*!
    lock sample
    */
  void *p_lock_sample;     
  /*!
    anim region.
    */
  void *p_anim_rgn;
  /*!
    anim region buffer.
    */
  void *p_anim_buffer; 
  /*!
    referance region.
    */
  void *p_ref_rgn;
  /*!
    referance region buffer.
    */
  void *p_ref_buffer;    
  /*!
    referance region.
    */
  void *p_old_rgn;
  /*!
    referance region buffer.
    */
  void *p_old_buffer;
  /*!
    gradient region.
    */
  void *p_grad_rgn;
  /*!
    gradient buffer.
    */
  void *p_grad_buffer;    
  /*!
    handle
    */  
  u32 handle;
  /*!
    is first half, used in animation type like in-out..
    */
  BOOL is_first_half;
  /*!
    stop offset, used for gradient paint.
    */
  u32 stop_offset[4];  
  /*!
    chip ic.
    */
  chip_ic_t chip_ic;    
}pic_render_pin_private_t;

/*!
  the input pin
  */
typedef struct tag_pic_render_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_pin_t base_pin;
  /*!
    private data
    */
  pic_render_pin_private_t private_data;
}pic_render_pin_t;

/*!
  cfg pic rend pin.

  \param[in] p_pin : pin
  \param[in] p_cfg : config parameter
  */
void pic_rend_pin_cfg(pic_render_pin_t *p_pin, pic_rend_cfg_t *p_cfg);

/*!
  pic_rend_pin_show.

  \param[in] p_pin : pin
  \param[in] bshow : is or not show pic
  */
void pic_rend_pin_show(pic_render_pin_t *p_pin, BOOL bshow);

/*!
  pic_rend_pin_clear.

  \param[in] p_pin : pin
  \param[in] color : color
  */
void pic_rend_pin_clear(pic_render_pin_t *p_pin, u32 param, void *p_param);

/*!
  pic_rend_stop_anim.
  */
void pic_rend_stop_anim(pic_render_pin_t *p_pin);

/*!
  pic_rend_pause_anim.
  */
void pic_rend_pause_anim(pic_render_pin_t *p_pin);

/*!
  pic_rend_resume_anim.
  */
void pic_rend_resume_anim(pic_render_pin_t *p_pin);

/*!
  create a pin.
  
  \param[in] p_pin : pin.
  \param[in] p_owner : owner.
  \param[in] p_para : private para.

  \return return the instance of video_pic_render_pin_t
  */
pic_render_pin_t * pic_render_pin_create(
  pic_render_pin_t *p_pin, interface_t *p_owner, pic_rend_para_t *p_para);

#endif // End for __PIC_RENDER_PIN_INTRA_H_
