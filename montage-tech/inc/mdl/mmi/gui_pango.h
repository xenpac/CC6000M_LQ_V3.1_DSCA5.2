/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GUI_PANGO_H__
#define __GUI_PANGO_H__

typedef struct
{
  /*!
    font buffer addr.
    */
  void *p_font;
  /*!
    font size.
    */
  u32 font_size;
}pango_cfg_t;

/*!
  pango init.
  */
handle_t gui_pango_init(pango_cfg_t *p_cfg);

/*!
  pango release.
  */
void gui_pango_release(handle_t h_pango);

/*!
  pango draw unistr.
  */
u32 pango_draw_unistr(handle_t h_pango, hdc_t hdc, rect_t *p_rc, u32 style, u16 x_offset, u16 y_offset,
  u8 line_space, u16 *p_str, rsc_fstyle_t *p_fstyle, u32 draw_style);
  
#endif

