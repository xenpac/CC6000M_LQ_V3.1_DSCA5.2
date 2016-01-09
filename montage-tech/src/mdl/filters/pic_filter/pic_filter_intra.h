/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_FILTER_INTRA_H_
#define __PIC_FILTER_INTRA_H_

/*!
  private data
  */
typedef struct tag_pic_filter_private
{
  /*!
    this point !!
    */
  void *p_this;

  /*!
    it's input pin
    */
  pic_in_pin_t m_in_pin;
  
  /*!
    it's output pin
    */
  pic_out_pin_t m_out_pin;

  /*!
    using sample
    */
  media_sample_t *p_output_sample;
  /*!
    scale
    */
  rect_size_t size;
  /*!
    pdec mode
    */
  dec_mode_t dec_mode;
  /*!
    pdec mode
    */
  scale_mode_t scale_mode;
  /*!
    flip
    */
  pic_flip_rotate_type_t flip;
  /*!
    output format
    */
  u32 out_fmt; 
  /*!
    payload
    */
  u32 payload;
  /*!
    src width
    */
  rect_size_t src_size;
  /*!
    frame index, for gif
    */
  u16 frm_index;
  /*!
    frame cnt, for gif
    */
  u16 frm_cnt;
  /*!
    last frm shows time
    */
  u32 last_time;    
  /*!
    frm_delay_time, for gif
    */
  u32 frm_delay_time;
  /*!
    for gif frames
    */
  u8 *p_gif;
  /*!
    input buffer
    */
  void *p_input;
  /*!
    input buffer size
    */
  u32 input_size;  
  /*!
    output buffer
    */
  void *p_output;
  /*!
    ptr.(just for multi instance.)
    */
  pdec_ins_t ins;
  /*!
    handle
    */  
  u32 handle;
  /*!
    is new gif, to clear background if a new gif is received.
    */
  u32 is_new_gif : 1;  
  /*!
    is thumbnail, for gif, we will just decoder one loop when it's thumbnail show.
    */
  u32 is_thumb : 1;    
  /*!
    use bg color
    */ 
  u32 use_bg_color : 1;
  /*!
    pic dec interrupted
    */ 
  u32 pic_dec_inerrupted : 1;
  /*!
    is gif render
    */ 
  u32 is_gif_render : 1;
  /*!
    is pic dec get info ok
    */ 
  u32 is_pic_dec_getinfo : 1;
  /*!
    reserved
    */
  u32 reserved : 26;
}pic_filter_private_t;

/*!
  the pic filter define
  */
typedef struct tag_pic_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data
    */
  pic_filter_private_t private_data;
}pic_filter_t;

/*!
  pic filter free sample data.
  */
void pic_filter_free_sample_data(pic_filter_t *p_pic_filter);

#endif // End for __PIC_FILTER_INTRA_H_

