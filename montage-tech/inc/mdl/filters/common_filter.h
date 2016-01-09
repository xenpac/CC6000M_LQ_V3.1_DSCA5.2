/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __COMMON_FILTER_H_
#define __COMMON_FILTER_H_

/*!
  render param
  */
typedef struct
{
  /*!
    display format
    */
  pix_fmt_t fmt;
  /*!
    fill rect.(reference is the orign of the image)
    */
  rect_t fill_rect;
  /*!
    render rect.(reference is the window defined in render pin)
    */
  rect_t rend_rect;    
  /*!
    image width.
    */
  u32 image_width;
  /*!
    image height.
    */
  u32 image_height; 
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
    pitch
    */
  u32 pitch;    
  /*!
    palette entry
    */
  void *pal_entry;
  /*!
    palette number
    */
  u32 pal_num;
  /*!
    pic data.
    */
  u8 *p_data;
  /*!
    is last sample.(for gif show)
    */
  BOOL is_last;
  /*!
    is first sample.(for gif show)
    */
  BOOL is_first;
  /*!
    for reserve
  */
  unsigned char  reserved;
  /*!
    region handle. if NULL, means use the global region handle.
    */
  void *p_rgn;   
  /*!
    alpha map, vector font used.
    */
  void *p_alpha;
  /*!
    alpha pitch
    */
  u32 alpha_pitch;
  /*!
    True : don't update region. FALSE : update rgn.
    */
  BOOL not_update;
}rend_param_t;


/*!
  out format
  */
typedef enum
{
  /*!
    unicode
    */
  STR_UNICODE = 0,
  /*!
    gb2312
    */
  STR_GB2312,
  /*!
    ascii
    */
  STR_ASCII,
  /*!
    utf-8
    */
  STR_UTF8,
  /*!
    Arabic
    */
  STR_CP1256,
  /*!
    Turkish
    */
  STR_8859_9,
  /*!
    picture
    */
  STR_IMAGE,
  /*!
    color
    */
  STR_COLOR,
}str_fmt_t;

/*!
  char align
  */
typedef enum
{
  /*!
    unicode
    */
  STR_CENTER = 0,  
  /*!
    gb2312
    */
  STR_LEFT,
  /*!
    ascii
    */
  STR_RIGHT,
}str_align_t;

/*!
  detail for a single line.
  */
typedef struct
{
  /*!
    align style.
    */
  str_align_t align;    
  /*!
    string length.(in bytes)
    */
  u32 str_len;
  /*!
    is high-light.
    */
  u32 str_style;    
  /*!
    string
    */
  u8 *p_str;    
}str_data_t;

/*!
  data for string type. ascii, gb2312, big5...
  */
typedef struct
{
  /*!
    how many lines in the sample.
    */
  u8 lines;
  /*!
    data.
    */
  str_data_t *p_data;
}str_str_t;

/*!
  img data.
  */
typedef struct
{
  /*!
    align
    */
  str_align_t image_align;
  /*!
    width
    */
  u16 image_width;
  /*!
    image height.
    */
  u16 image_height;  
  /*!
    image pitch.
    */
  u16 image_pitch;
  /*!
    image data. must be align with 8 bytes.
    */  
  void *p_image;    
}str_img_t;

/*!
  color data. fill the whole window with the color.
  */
typedef struct
{
  /*!
    fill color.
    */
  u32 color;
}str_clr_t;
/*!
  union data.
  */
typedef union
{
  /*!
    string data.
    */
  str_str_t  str_data;
  /*!
    image data.
    */
  str_img_t img_data;
  /*!
    color data.
    */
  str_clr_t clr_data;
}str_union_t;

/*!
  char filter param, for communication with previous filters.
  */
typedef struct
{
  /*!
    coding. ASCII, UNICODE, GB2312....
    */
  str_fmt_t fmt;
  /*!
    char union data.
    */
  str_union_t str_union;
}str_param_t;

/*!
  high light
  */
#define STR_HL      0x00000001

/*!
  bold
  */
#define STR_BOLD    0x00000002

/*!
  italic
  */
#define STR_ITALIC  0x00000003

/*!
  strok
  */
#define STR_STROK   0x00000004

/*!
  glyph input param
  */
typedef struct
{
  /*!
    utf16 char_code. input param
    */   
  u16 char_code;
  /*!
    style
    */    
  u32 str_style;
  /*!
    pixel fmt. input param
    */
  pix_fmt_t fmt;  
   
}glyph_input_t;

/*!
  glyph output param
  */
typedef struct
{
  /*!
    char pic data width. output param
    */ 
  u16 width;
  /*!
    char pic data hight. output param
    */   
  u16 height;
  /*!
    pitch. out param
    */
  u32 pitch;    
  /*!
    char pic data. output param.
    */   
  u8 *p_char;
  /*!
    enable color key.
    */
  BOOL enable_ckey;    
  /*!
    color key
    */
  u32 ckey;
  /*!
    alpha map, just for vector font.
    */
  u8 *p_alpha;
  /*!
    alpha map pitch, just for vector font.
    */
  u16 alpha_pitch;
  /*!
    x step, just for vector font.
    */
  u16 x_step;   
  /*!
    step width, just for vector font.
    */
  u16 step_width;
  /*!
    step height, just for vector font.
    */
  u16 step_height;   
  /*!
    is use alpha blending, no used.
    */
  BOOL is_alpha_spt;  
  /*!
    xoffset, just for vector font.
    */
  s16 xoffset;
  /*!
    yoffset, just for vector font.
    */
  s16 yoffset;
  /*!
    strok alpha, just for vector font.
    */
  void *p_strok_alpha;
  /*!
    strok char, just for vector font.
    */
  void *p_strok_char;    
}glyph_output_t;


/*!
  flash_burn_in_t
  */
typedef struct
{
  /*!
    burn addr
    */  
  u32 addr;
  /*!
    burn size
    */  
  u32 size;
  /*!
    p_data for burn
    */  
  u8 *p_data;  
}flash_burn_in_t;

/*!
  packet size when burning flash
  */
#define UPG_BURN_PACKET_SIZE (64 * (KBYTES))

#endif // End for __TS_PLAYER_FILTER_H_

