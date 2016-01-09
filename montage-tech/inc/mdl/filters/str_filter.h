/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __STR_FILTER_H_
#define __STR_FILTER_H_

/*!
  jpeg filter command define
  */
typedef enum
{
  /*!
    render rect
    */
  STR_CFG_REND_RECT = 0,
  /*!
    data format
    */
  STR_CFG_SOURCE_FORMAT,
  /*!
    color format
    */
  STR_CFG_COLOR_FORMAT,
  /*!
    alpha
    */
  STR_CFG_SET,
  /*!
    charset
    */
  STR_CFG_CHARSET,
}str_filter_cmd_t;

/*!
  scale  dest: src
 */
typedef struct
{
  /*!
    rect
    */
  rect_t str_rect;
  /*!
    palette
    */
  u32 p_pal[256];
  /*!
    palette number.
    */
  u32 pal_num;
  /*!
    bpp
    */
  u32 bpp;
  /*!
    region fmt
    */
  pix_fmt_t fmt;
  /*!
    use bg
    */
  BOOL use_font_bg;
  /*!
    bg color
    */
  u32 font_bg;
  /*!
    window bg
    */
  u32 win_bg;    
}str_cfg_t;


/*!
  pic render config parameter
  */
typedef struct
{
 /*!
   get resource pixel info
   */   
  RET_CODE (*rsc_get_glyph)(glyph_input_t *p_input, glyph_output_t *p_output);
}str_para_t;

/*!
  create a file source instance
  filter ID:JPEG_FILTER
  \return return the instance of char_filter_t
  */
ifilter_t * str_filter_create(void *p_para);

#endif // End for __CHAR_FILTER_H_
