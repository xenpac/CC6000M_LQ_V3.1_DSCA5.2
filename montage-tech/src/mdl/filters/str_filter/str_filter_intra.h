/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __STR_FILTER_INTRA_H_
#define __STR_FILTER_INTRA_H_

/*!
  private data
  */
typedef struct
{
  /*!
    this point !!
    */
  void *p_this;

  /*!
    it's input pin
    */
  str_in_pin_t m_in_pin;
  
  /*!
    it's output pin
    */
  str_out_pin_t m_out_pin;

  /*!
    using sample
    */
  media_sample_t *p_output_sample;
  /*!
    render rect
    */
  rect_t render_rect;
  /*!
    fill rect
    */
  rect_t fill_rect;
  /*!
    palette
    */
  void *p_pal;
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
    use bg color.
    */
  BOOL use_font_bg;
  /*!
    bg color.
    */
  u32 font_bg;
  /*!
	window bg
	*/
  u32 win_bg;
  /*!
    previous sample h.
    */
  u32 pre_h;    
  /*!
    start coordinate y, when receive multi lines string.
    */
  u16 s_y;
  /*!
    line width. 
    */
  u16 *p_linew;
  /*!
    line height. 
    */
  u16 *p_lineh; 
  /*!
    line position. 
    */
  u16 **pp_linep;  
  /*!
    line character numbers. 
    */
  u16 *p_lines;    
  /*!
    rsc_get_data
    */  
  RET_CODE (*rsc_get_glyph)(glyph_input_t *p_input, glyph_output_t *p_output);
  /*!
    string charset
    */  
  str_fmt_t str_charset;
  /*!
    convert handle of gb2312_to_utf16le
    */ 
  iconv_t gb2312_to_utf16le;
  /*!
    convert handle of utf8_to_utf16le
    */ 
  iconv_t utf8_to_utf16le;
}str_filter_private_t;

/*!
  the char filter define
  */
typedef struct
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_filter_t m_filter;
  /*!
    private data buffer
    */
  str_filter_private_t private_data;
}str_filter_t;


#endif // End for __CHAR_FILTER_INTRA_H_

