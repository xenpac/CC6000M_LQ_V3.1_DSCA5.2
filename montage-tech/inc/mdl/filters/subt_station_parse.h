/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SUBT_STATION_PARSE_H__
#define __SUBT_STATION_PARSE_H__


/*!
  SUBT_TEXT_LEN
  */
#define SUBT_TEXT_LEN (80)
/*!
  SUBT_TEXT_MAX_LINE
  */
#define SUBT_TEXT_MAX_LINE  (3)

/*!
  subt_sys_type_t
  */
typedef enum 
{
  /*!
    SUBT_SYS_SSA
    */
  SUBT_SYS_SSA,
  /*!
    SUBT_SYS_SRT
    */
  SUBT_SYS_SRT,
  /*!
    SUBT_SYS_SUB
    */
  SUBT_SYS_SUB,
  /*!
    SUBT_SYS_UNKOWN
    */
  SUBT_SYS_UNKOWN
}subt_sys_type_t;


/*!
  subt_sys_type_t
  */
typedef enum 
{
  /*!
     unicode
    */
  SUBT_CODEC_UNICODE = 0,  
  /*!
    gb2312
    */
  SUBT_CODEC_GB2312,
  /*!
    ascii
    */
  SUBT_CODEC_ASCII,
  /*!
    utf-8
    */
  SUBT_CODEC_UTF8,
   /*!
    unknown
    */
  SUBT_CODEC_UNKNOW,
}subt_codec_fmt_t;

/*!
  SUBT_SYS_ALIGN_LEFT
  */
#define  SUBT_SYS_ALIGN_LEFT    (1 << 0)
/*!
  SUBT_SYS_ALIGN_CENTER
  */
#define  SUBT_SYS_ALIGN_CENTER (1 << 1)
/*!
  SUBT_SYS_ALIGN_RIGHT
  */
#define  SUBT_SYS_ALIGN_RIGHT   (1 << 2)
/*!
  SUBT_SYS_ALIGN_TOP
  */
#define  SUBT_SYS_ALIGN_TOP   (1 << 3)
/*!
  SUBT_SYS_ALIGN_MID
  */
#define  SUBT_SYS_ALIGN_MID   (1 << 4)

/*!
  subt_font_attr_t
  */
typedef struct
{
  /*!
    The font size in pixels
    */
  u16 font_size;
  /*!
    The color of the text 0xRRGGBB  (native endianness)
    */
  u16 font_color;
  /*!
    < The transparency of the text.
                                   0x00 is fully opaque,
                                   0xFF fully transparent
    */
  u16 font_alpha;
  /*!
     The color of the shadow 0xRRGGBB
    */
  u16 shadow_color;
  /*!
    < The transparency of the shadow.
                                    0x00 is fully opaque,
                                    0xFF fully transparent
    */
  u16 shadow_alpha;
  /*!
     The color of the background 0xRRGGBB
    */
  u16 background_color;
  /*!
    < The transparency of the background.
                                   0x00 is fully opaque,
                                   0xFF fully transparent
    */
  u16 background_alpha;
  /*!
    < Background color for karaoke 0xRRGGBB
    */
  u16 karaoke_background_color;
  /*!
    The transparency of the karaoke bg
    */
  u16 karaoke_background_alpha;
  /*!
    is_under_line
    */
  BOOL is_under_line;
  /*!
    is_bold
    */
  BOOL is_bold;
  /*!
    is_italic
    */
  BOOL is_italic;
}subt_font_attr_t;


/*!
  subt_station_content_t
  */
typedef struct
{
  /*!
    type
    */
  subt_sys_type_t type;
  /*!
    original_height
    */
  u16 original_height;
  /*!
    original_width
    */
  u16 original_width;
  /*!
    line_num
    */
  u8 line_num;
  /*!
    event
    */
  u8 event[SUBT_TEXT_MAX_LINE][SUBT_TEXT_LEN];
  /*!
    10ms.
    */
  u32 start;
  /*!
    10ms.
    */
  u32 end;
  /*!
    alignment
    */
  u8  alignment;
  /*!
    font_attr
    */
  subt_font_attr_t font_attr;
  /*!
    codec fmt
    */
  subt_codec_fmt_t codec_fmt;  
}subt_station_content_t;



/*!
  Implement hooks
  */
typedef struct
{
  /*!
    type
    */
  subt_sys_type_t type;
  /*!
    parse
    */
  RET_CODE (*parse)(u8 *p_in_data, u32 in_len,
                subt_sys_type_t type, void *p_subt);
  /*!
    check type
    */
  RET_CODE (*check)(u8 *p_in_data, u32 in_len);
  /*!
    check type
    */
  RET_CODE (*scan)(u8 *p_in_data, u32 in_len, void *p_out);
    
}subt_station_parse_t;

/*!
  subt_station_parse_func
  */
RET_CODE subt_station_parse_func(u8 *p_in_data, u32 in_len,
                subt_sys_type_t type, void *p_subt);

/*!
  subt_station_get_type
  */
subt_sys_type_t subt_station_get_type(u8 *p_in_data, u32 in_len);

/*!
  subt_station_scan_func
  */
RET_CODE subt_station_scan_func(u8 *p_in_data, u32 in_len,
                subt_sys_type_t type, void *p_subt);

/*!
  subt_station_register
  */
void subt_station_register(void *p_parsing);

#endif

