/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_FILTER_H_
#define __PIC_FILTER_H_

/*!
  scale mode
  */
typedef enum
{
  /*!
    auto fit window, use same factor on both direction.
    */
  SCALE_MODE_SMART = 0,
  /*!
    no scale.
    */
  SCALE_MODE_NONE,
  /*!
    auto fit window, use different factor on both direction.
    */
  SCALE_MODE_FULL,
}scale_mode_t;

/*!
  jpeg filter command define
  */
typedef enum
{
  /*!
    filter config.
    */
  PIC_FILTER_CFG,
  /*!
    for release buffer, when chain open error.
    */
  PIC_FILTER_UN_CFG,    
  /*!
    filter stop
    */
  PIC_FILTER_STOP,
  /*!
    filter interrupt pic dec
    */
  PIC_FILTER_INTERRUPT_PIC_DEC,
  /*!
    filter pause gif
    */
  PIC_FILTER_PAUSE_GIF,
  /*!
    filter resume gif
    */
  PIC_FILTER_RESUME_GIF,
}pic_filter_cmd_t;

/*!
  pic filter event define
  */
typedef enum
{
  /*!
    pic is too large.
    */
  PIC_IS_TOO_LARGE = JPEG_FILTER << 16,
  /*!
    pic size update.
    */
  PIC_SIZE_UPDATE,
  /*!
    picture unsupport
    */
  PIC_IS_UNSUPPORT,
  /*!
    picture data error
    */
  PIC_DATA_ERROR,
}pic_filter_evt_t;

/*!
  out format
  */
typedef enum tag_pic_outfmt
{
  /*!
    PIC_OUTFMT_422
    */
    PIC_OUTFMT_422 = 0,  
  /*!
    PIC_OUTFMT_444
    */
    PIC_OUTFMT_444,
}pic_outfmt_t;

/*!
  pic render config parameter
  */
typedef struct
{
  /*!
    rectangle size.
    */
  rect_size_t size;
  /*!
    flip
    */
  pic_flip_rotate_type_t flip;
  /*!
    output format.
    */
  u32 out_fmt;
  /*!
    dec mode.
    */
  dec_mode_t dec_mode;
  /*!
    scale mode.
    */
  scale_mode_t scale_mode;
  /*!
    memp
    */ 
  lib_memp_t *p_heap;
  /*!
    input size.
    */
  u32 in_size;
  /*!
    output size.
    */
  u32 out_size;    
  /*!
    is push data.
    */
  BOOL is_push;   
  /*!
    handle
    */  
  u32 handle;  
  /*!
    is thumbnail, for gif, we will just decoder one loop when it's thumbnail show.
    */
  BOOL is_thumb;    
  /*!
    use bg color
    */    
  BOOL use_bg_color;
}pic_cfg_t;

/*!
  create a file source instance
  filter ID:JPEG_FILTER
  \return return the instance of pic_filter_t
  */
ifilter_t * pic_filter_create(void *p_para);

#endif // End for __PIC_FILTER_H_
