/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GUI_PAINT_H__
#define __GUI_PAINT_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file gui_paint.h

   This file defined some interfaces of osd layer for application layer modules.
   These interfaces are mainly
   about how to get resources and draw them.
  */

/*!
   Draw style : normal draw, draw string without underline.
  */
#define STRDRAW_NORMAL            0x0000L
/*!
   Draw style : draw string with underline.
  */
#define STRDRAW_WITH_UNDERLINE    0x1000L

/*!
  Draw style : draw string with invert color style.
  */
#define STRDRAW_WITH_INVERT     0x2000L
  
/*!
   Enable break word.
  */
#define STRDRAW_BREAK_WORD        0x4000L

/*!
  Draw style : draw string ignore new line.(ignore "\n")
  */
#define STRDRAW_IGNORE_NLINE     0x8000L

/*!
   Make a combined style with a single draw style and a specified data.
  */
#define MAKE_DRAW_STYLE(style, data) \
  ((u32)(((u16)(style)) << 16 | (u16)(data)))

/*!
   Get draw style from a combined style which created by "MAKE_DRAW_STYLE".
  */
#define GET_DRAW_STYLE(x)             ((u16)(x >> 16))
/*!
   Get draw data from a combined style which created by "MAKE_DRAW_STYLE".
  */
#define GET_DRAW_DATA(x)              ((u16)(x & 0xFFFF))

/*!
   Make a combined style with a single draw style and a specified data.
  */
#define MAKE_DRAW_STATUS(end, num)    ((u32)(((u16)(end)) << 16 | (u16)(num)))

/*!
   Get draw style from a combined style which created by "MAKE_DRAW_STYLE".
  */
#define STRING_IS_EOF(s)              ((u16)(s >> 16))
/*!
   Get draw data from a combined style which created by "MAKE_DRAW_STYLE".
  */
#define GET_DRAWED_CHARS(s)           ((u16)(s & 0xFFFF))


/*!
   Return value, it means meet a  "0" when copy data or the string length is
   greater
   than MAX_STRING_LENGTH, it means the end of string.
  */
#define END_OF_STRING    0xFF
/*!
   Return value, it means the width is  greater than allowable width, need a new
   line.
  */
#define END_OF_LINE      0xFE
/*!
   Return value, it means meet a " " when copy data, it's the end of one word.
  */
#define END_OF_WORD      0xFD
/*!
   Return value, it means meet a "\n" when copy data, and a new line start.
  */
#define END_OF_NLINE     0xFC

/*!
   the string is from right to left,such as arabic language.
  */
#define BIDI_RIGHT      0x01
/*!
   the string is from left to right
  */
#define BIDI_LEFT     0x00

/*!
  gui paint param
  */
typedef struct
{
  /*!
    max string length.
    */
  u32 max_str_len;
  /*!
    max string lines.
    */
  u32 max_str_lines;
  /*!
    resource handle
    */
  handle_t rsc_handle;
  /*!
    pango handle.
    */
  handle_t h_pango;    
}paint_param_t;

/*!
  animation mode : in/out.
  */
typedef enum
{ 
  /*!
    animation disable.
    */
  ANIM_MODE_NONE = 0,    
  /*!
    animation mode in.
    */
  ANIM_MODE_IN,
  /*!
    animation mode out.
    */
  ANIM_MODE_OUT,
  /*!
    animation mode max.
    */
  ANIM_MODE_MAX,    
}anim_mode_t;


/*!
  control animation style.
  */
typedef enum
{
   /*!
    animation none.
    */
  ANIM_IN_NONE = 0, 
  /*!
    fade in.
    */
  ANIM_FADE_IN,
  /*!
    rotate by z direction.
    */
  ANIM_ZROTATE_IN,
  /*!
    step in from left.
    */
  ANIM_LEFT_IN,
  /*!
    step in from top.
    */
  ANIM_TOP_IN,
  /*!
    step in from bottom
    */
  ANIM_BOTTOM_IN,
  /*!
    step in from right
    */
  ANIM_RIGHT_IN,
  /*!
    animation max.
    */
  ANIM_IN_MAX,    
}anim_in_t;

/*!
  control animation style.
  */
typedef enum
{
   /*!
    animation none.
    */
  ANIM_OUT_NONE = 0, 
  /*!
    fade in.
    */
  ANIM_FADE_OUT,
  /*!
    rotate by z direction.
    */
  ANIM_ZROTATE_OUT,
  /*!
    step in from left.
    */
  ANIM_LEFT_OUT,
  /*!
    step in from top.
    */
  ANIM_TOP_OUT,
  /*!
    step in from bottom
    */
  ANIM_BOTTOM_OUT,
  /*!
    step in from right
    */
  ANIM_RIGHT_OUT,
  /*!
    animation max.
    */
  ANIM_OUT_MAX,    
}anim_out_t;

/*!
   Initial gui paint.(allocate memory)

   \param[in] p_param : gui paint param.

   \return : NULL
  */
void gui_paint_init(paint_param_t *p_param);

/*!
   Release gui paint.(release memory)
   \return : NULL
  */
void gui_paint_release(void);

/*!
  gui get resource handle.

  \return resource handle.
  */
handle_t gui_get_rsc_handle(void);

/*!
   Get DC of paint.

   \param[in] p_ctrl : control
   \param[in] parent_dc  : parent DC
   \return : dc for paint.
  */
hdc_t gui_begin_paint(control_t *p_ctrl, hdc_t parent_dc);


/*!
   Release DC of paint.

   \param[in] p_ctrl : control
   \param[in] hdc : dc for paint.
   \return : NULL
  */
void gui_end_paint(control_t *p_ctrl, hdc_t hdc);

/*!
  release dc but don't empty the clip rgn.
  */
void gui_end_paint_ex(control_t *p_ctrl, hdc_t hdc);

/*!
   Paint control background.

   \param[in] hdc    : DC
   \param[in] p_ctrl : control
   \return : NULL
  */
void gui_paint_frame(hdc_t hdc, control_t *p_ctrl);

/*!
  Get left top position by rect and align type.

  \param[in] p_rc : RECT where we can paint
  \param[in] style : align style
  \param[in] x_offset : horizon offset
  \param[in] y_offset : vertical offset
  \param[in] width : content width which will be paint
  \param[in] height : content height which will be paint
  \param[out] p_left : output left positin
  \param[out] p_top : output top positin
  \return : NULL
  */
void gui_get_posi(rect_t *p_rc,
                  u32 style,
                  u16 x_offset,
                  u16 y_offset,
                  u16 width,
                  u16 height,
                  u16 *p_left,
                  u16 *p_top);

/*!
   Draw vertical line. it seems didn't realized yet.

   \param[in] hdc : DC
   \param[in] x : start position x
   \param[in] y : start position y
   \param[in] height : length
   \param[in] color_type : color type
   \param[in] color : color
   \return : NULL
  */
void osd_draw_ver_line(hdc_t hdc,
                       s16 x,
                       s16 y,
                       u16 height,
                       u8 color_type,
                       u32 color);

/*!
  Draw horizon line.

  \param[in] hdc : DC
  \param[in] x : start position x
  \param[in] y : start position y
  \param[in] width : length
  \param[in] color : color
  \return : NULL
  */
void gui_draw_hor_line(hdc_t hdc, u16 x, u16 y, u16 width, u32 color);

/*!
  Draw a bitmap.

  \param[in] hdc : DC
  \param[in] p_rc : rect
  \param[in] style : style
  \param[in] x_offset : x offset in rect
  \param[in] y_offset : y offset in rect
  \param[in] bmp_id : bitmap id
  \return : NULL
  */
void gui_draw_picture(hdc_t hdc,
                      rect_t *p_rc,
                      u32 style,
                      u16 x_offset,
                      u16 y_offset,
                      u16 bmp_id);

/*!
   Draw bitmap

   \param[in] hdc : DC
   \param[in] x : bitmap position x
   \param[in] y : bitmap position y
   \param[in] bmp_id : bitmap id
   \return : NULL
  */
void gui_draw_picture_ext(hdc_t hdc, s16 x, s16 y, u16 bmp_id);

/*!
   Fill a rect withe a specified color.

   \param[in] hdc : DC
   \param[in] p_rc : rect to be filled.
   \param[in] color : the specified color.
   \return : NULL
  */
void gui_fill_rect(hdc_t hdc, rect_t *p_rc, u32 color);

/*!
  Fill a rect withe a specified color.

  \param[in] hdc : DC
  \param[in] p_rc : rect to be filled.
  \param[in] color : the specified color.
  \return : NULL
  */
void gui_fill_round_rect(hdc_t hdc, rect_t *p_rc, u32 color);


/*!
   Draw rect with a spcified style.

   \param[in] hdc : DC
   \param[in] p_rc : Rect , referrence is DC
   \param[in] p_rstyle : rect style
   \return : NULL

   indicates:
   flag = 0 means use key color
   left top icon , left bottom icon , right top icon , right bottom icon : will
   be drawn one time
   left icon, right icon : will be tiled in vertical direction
   top icon, bottom icon : will be tiled in horizontal direction
   bg icon : will be tiled in whole rect
  */
void gui_draw_style_rect(hdc_t hdc, rect_t *p_rc, rsc_rstyle_t *p_rstyle);

/*!
  draw image in rectangle.
  */
void gui_draw_image_rect(hdc_t hdc, rect_t *p_rc, u16 win_bg);

/*!
   Draw string with a font style.

   \param[in] hdc : DC
   \param[in] p_rc : rect
   \param[in] style : string style
   \param[in] x_offset : x offset in rect
   \param[in] y_offset : y offset in rect
   \param[in] line_space : space between 2 lines
   \param[in] p_str : string data
   \param[in] f_style_idx : font style index
   \param[in] draw_style : draw style.
   \return : string is end | how many chars have been processed(include format
   sign)
  */
u32 gui_draw_unistr(hdc_t hdc,
                    rect_t *p_rc,
                    u32 style,
                    u16 x_offset,
                    u16 y_offset,
                    u8 line_space,
                    u16 *p_str,
                    u32 f_style_idx,
                    u32 draw_style);

  /*!
    FIXME
    */
u16 gui_draw_unistr_offset(hdc_t hdc, rect_t *p_rc,
  u16 *p_str, u32 f_style_idx, u32 draw_style,
  u16 str_drawed, u16 str_width, u16 str_height, s16 *p_str_offset);

/*!
   Draw a charater withe a font style.
   
   \param[in] hdc : hdc
   \param[in] p_rc : rect
   \param[in] bmp_data : point to bmp
   \param[in] bmp_drawed : font style for draw
   \param[in] bmp_width: bmp width
   \param[in] bmp_height : bmp height
   \param[out] *p_bmp_offset : 
   \return :
  */
u16 gui_draw_picture_offset(hdc_t hdc, rect_t *p_rc,
    u32 bmp_data, BOOL is_rawdata, u16 bmp_drawed, u16 bmp_width, u16 bmp_height, s16 *p_bmp_offset);


/*!
   Draw a charater withe a font style.
   
   \param[in] bmp_data : point to bmp
   \param[in] is_rawdata : raw data otherwise bmp ID
   \param[out] *bmp_width : bmp width
   \param[out] *bmp_height : bmp height
   \return : 
  */
u16 gui_get_bmp_attr(u32 bmp_data, BOOL is_rawdata, u16 *bmp_width, u16 *bmp_height);

/*!
   Draw a charater withe a font style.
   
   \param[in] hdc : hdc
   \param[in] p_fstyle : font style for draw
   \param[in] char_code : unicode charactor
   \param[in] left : left offset.
   \param[in] top : top offset.
   \return : left offset of next charactor, for draw next charator.
  */
u16 gui_draw_char(hdc_t hdc,
                  rsc_fstyle_t *p_fstyle,
                  u16 char_code,
                  u16 left,
                  u16 top);

/*!
   Draw string by string id

   \param[in] hdc : DC
   \param[in] p_rc : rect
   \param[in] style : string style
   \param[in] x_offset : x offset in rect
   \param[in] y_offset : y offset in rect
   \param[in] line_space : space between 2 lines
   \param[in] str_id : string id
   \param[in] f_style_idx : font style index
   \param[in] draw_style : draw style.
   \return : string is end | how many chars have been processed(include format
   sign)
  */
u32 gui_draw_strid(hdc_t hdc,
                   rect_t *p_rc,
                   u32 style,
                   u16 x_offset,
                   u16 y_offset,
                   u8 line_space,
                   u16 str_id,
                   u32 f_style_idx,
                   u32 draw_style);

/*!
  Set palette.

  \param[in] is_top_screen : top screen.
  \param[in] pal_id : palette id
  \param[in] trans : trans
  \return : NULL
  */
void gui_set_palette(BOOL is_top_screen, u16 pal_id, u16 trans);

/*!
  Get string address on flash by id.

  \param[in] string_id : string id
  \return : address
  */
u32 gui_get_string_addr(u16 string_id);


/*!
  Get unicode string by id.

  \param[in] string_id : string id
  \param[in] p_buffer : unicode buffer
  \param[in] buffer_size : unicode size
  \return : if success, return TRUE.
  */
BOOL gui_get_string(u16 string_id, u16 *p_buffer, u16 buffer_size);

/*!
   Get bitmap information.

   \param[in] bmp_id : bitmap id.
   \param[in] p_width : return width of bitmap.
   \param[in] p_height : return height of bitmap.
   \return : if success, return TRUE, else return FALSE.
  */
BOOL gui_get_bmp_info(u16 bmp_id, u16 *p_width, u16 *p_height);

/*!
   Get string lines by an unicode string.

   \param[in] f_style_idx : font style index
   \param[in] style : style.
   \param[in] p_rc : rectangle for draw.
   \param[in] p_str : unicode string buffer
   \param[in] x_offset : x-offset.
   \param[in] draw_style : draw style
   
   \return : string lines.
  */
u16 gui_get_unistr_total_lines(u32 f_style_idx, u32 style,
  rect_t *p_rc, u16 *p_str, u16 x_offset, u32 draw_style);
/*!
  get uni string address by line.

  \param[in] f_style_idx : font style index
  \param[in] style : style.
  \param[in] p_rc : rectangle for draw.
  \param[in] p_str : string for draw.
  \param[in] x_offset : x-offset.
  \param[in] draw_style : draw style
  
  \return : str buf address.
  */
u16 *gui_get_next_line(u32 f_style_idx,
                           u32 style,
                           rect_t *p_rc,
                           u16 *p_str,
                           u16 x_offset,
                           u32 draw_style);
/*!
  get lines per page.

  \param[in] f_style_idx : font style index
  \param[in] style : style.
  \param[in] p_rc : rectangle for draw.
  \param[in] p_str : string for draw.
  \param[in] x_offset : x-offset.
  \param[in] y_offset : y-offset.
  \param[in] line_space : line space.
  \param[in] draw_style : draw style.
  
  \return : lines per page.
  */
u8 gui_get_lines_per_page(u32 f_style_idx,
                           u32 style,
                           rect_t *p_rc,
                           u16 *p_str,
                           u16 x_offset,
                           u16 y_offset,
                           u8 line_space,
                           u32 draw_style);

/*!
  set animation mode.
  */
void gui_set_anim_mode(anim_mode_t anim_mode);

/*!
  set animation.
  */
void gui_set_anim(anim_in_t anim_in, anim_out_t anim_out);
#ifdef __cplusplus
}
#endif                           

#endif
