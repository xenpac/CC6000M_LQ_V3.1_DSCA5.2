/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GUI_RESOURCE_H__
#define __GUI_RESOURCE_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file gui_resource.h

   This file defined data structures for several kinds of resource head.
   And defines interfaces for users to get a specified resource.

   Development policy:
  */

/*!
   Invalid resource id, all resource id must base on 1
  */
#define RSC_INVALID_ID    0

/*!
   Buffer size for read data(bytes)
  */
#define DATA_BUF_SIZE    32

/*!
  xml font style start.
  */
#define XML_FSTYLE_START 0x80000000

/*!
  xml rectangle-style start.
  */
#define XML_RSTYLE_START 0x80000000

/*!
  xml bitmap idx start.
  */
#define XML_BMAP_START 0x8000

/*!
   Resource type id, for identify different kinds of resource
  */
typedef enum
{
  /*!
     Font resource
    */
  RSC_TYPE_FONT = 0,
  /*!
     Palette resource
    */
  RSC_TYPE_PALETTE,
  /*!
     Bitmap resource
    */
  RSC_TYPE_BITMAP,
  /*!
     String resource
    */
  RSC_TYPE_STRING,
  /*!
     Script resource
    */
  RSC_TYPE_SCRIPT,
  /*!
     Word resource
    */
  RSC_TYPE_WORD,
  /*!
     Resource type invalid
    */
  RSC_TYPE_INVALID
}rsc_type_t;


/*!
   Resource compress mode
  */
typedef enum
{
  /*!
     No compressed, the data is not compressed
    */
  CMP_MODE_NONE = 0,
  /*!
     RLE compressed, the data is RLE compressed
    */
  CMP_MODE_RLE,
  /*!
    lz4
    */
  CMP_MODE_LZ4,
  /*!
    lz77
    */
  CMP_MODE_LZ77,
  /*!
    lz77 compressed,
    */
  CMP_MODE_LZMA,
  /*!
    jpg compressed,
    */
  CMP_MODE_JPG,
  /*!
     Compressed mode invalid
    */
  CMP_MODE_INVALID
}rsc_compress_mode_t;

/*!
  rect style : tile
  */
#define R_TILE          0x00000000
/*!
  rect style : center
  */
#define R_CENTER        0x00100000
/*!
  rect style : center
  */
#define R_STRETCH        0x00200000
/*!
   Rect style : use this mask to get the icon fill style.
  */
#define R_ICON_MASK     0x00F00000

/*!
   Rect style : default style, Draw nothing.
  */
#define R_IGNORE         0x00000000
/*!
   Rect style : fill with icon
  */
#define R_ICON           0x10000000
/*!
   Rect style : fill with color
  */
#define R_COLOR          0x20000000
/*!
   Rect style : no style, just use its bg as the style.
  */
#define R_COPY_BG        0xF0000000
/*!
   Rect style : use this mask to get the fill type.
  */
#define R_STYLE_MASK     0xF0000000


/*!
   Rect style : snap to height.
  */
#define R_SNAP_HEIGHT    0x00000000
/*!
   Rect style : snap to border.
  */
#define R_SNAP_BORDER    0x01000000
/*!
   Rect style : snap to vertex.
  */
#define R_SNAP_VERTEX    0x02000000
/*!
   Rect style : use this mask to get the snap type.
  */
#define R_SNAP_MASK      0x0F000000


/*!
   Rect style : fill rect with icon and snap to border.
  */
#define R_FILL_ICON_TO_BORDER (R_ICON | R_SNAP_BORDER)
/*!
   Rect style : fill rect with icon and snap to vertex.
  */
#define R_FILL_ICON_TO_VERTEX (R_ICON | R_SNAP_VERTEX)
/*!
   Rect style : fill rect with color and snap to border.
  */
#define R_FILL_RECT_TO_BORDER (R_COLOR | R_SNAP_BORDER)
/*!
   Rect style : fill rect with color and snap to vertex.
  */
#define R_FILL_RECT_TO_VERTEX (R_COLOR | R_SNAP_VERTEX)

/*!
   Rect style : draw line with is 1.
  */
#define R_LINE1          (R_COLOR | R_SNAP_HEIGHT | 0x00000001)
/*!
   Rect style : draw line with is 2.
  */
#define R_LINE2          (R_COLOR | R_SNAP_HEIGHT | 0x00000002)
/*!
   Rect style : draw line with is 3.
  */
#define R_LINE3          (R_COLOR | R_SNAP_HEIGHT | 0x00000003)
/*!
   Rect style : draw line with is 4.
  */
#define R_LINE4          (R_COLOR | R_SNAP_HEIGHT | 0x00000004)
/*!
   Rect style : draw line with is 5.
  */
#define R_LINE5          (R_COLOR | R_SNAP_HEIGHT | 0x00000005)
/*!
   Rect style : draw line with is 6.
  */
#define R_LINE6          (R_COLOR | R_SNAP_HEIGHT | 0x00000006)
/*!
   Rect style : draw line with is 7.
  */
#define R_LINE7          (R_COLOR | R_SNAP_HEIGHT | 0x00000007)
/*!
   Rect style : draw line with is 8.
  */
#define R_LINE8          (R_COLOR | R_SNAP_HEIGHT | 0x00000008)
/*!
   Rect style : use this mask to get the line width.
  */
#define R_LINE_WIDTH_MASK      0x0000000F


/*!
   Macro to get icon fill style.
  */
#define RSTYLE_GET_ICON_STYLE(x)         (((x).style) & R_ICON_MASK)

/*!
   Macro to get style.
  */
#define RSTYLE_GET_STYLE(x)         (((x).style) & R_STYLE_MASK)
/*!
   Macro to get snap.
  */
#define RSTYLE_GET_SNAP(x)          (((x).style) & R_SNAP_MASK)
/*!
   Macro to get line width.
  */
#define RSTYLE_GET_LINE_WIDTH(x)    ((u8)(((x).style) & R_LINE_WIDTH_MASK))


/*!
   Macro to determine the style is matched with R_XXX or not.
  */
#define RSTYLE_IS_MATCHED(x, style) ((BOOL)(RSTYLE_GET_STYLE(x) == style))
/*!
   Macro to determine R_IGNORE or not.
  */
#define RSTYLE_IS_R_IGNORE(x)       (RSTYLE_IS_MATCHED(x, R_IGNORE))
/*!
   Macro to determine R_ICON or not.
  */
#define RSTYLE_IS_R_ICON(x)         (RSTYLE_IS_MATCHED(x, R_ICON))
/*!
   Macro to determine R_COLOR or not.
  */
#define RSTYLE_IS_R_COLOR(x)        (RSTYLE_IS_MATCHED(x, R_COLOR))
/*!
   Macro to determine R_COPY_BG or not.
  */
#define RSTYLE_IS_R_COPY_BG(x)       (RSTYLE_IS_MATCHED(x, R_COPY_BG))

/*!
   Macro to get icon idx.
  */
#define RSTYLE_GET_ICON(x)          ((u16)RSTYLE_GET_VALUE(x))
/*!
   Macro to get color.
  */
#define RSTYLE_GET_COLOR(x)         ((u32)RSTYLE_GET_VALUE(x))
/*!
   Macro to get value.
  */
#define RSTYLE_GET_VALUE(x)         ((x).value)

/*!
   structure of part style
  */
typedef struct
{
  /*!
     fill style
    */
  u32 style;
  /*!
     fill value
    */
  u32 value;
}rsc_part_style_t;

/*!
   Structure of rectangle style.
  */
typedef struct
{
  /*!
     Left part of rectangle.
    */
  rsc_part_style_t left;
  /*!
     Top part of rectangle.
    */
  rsc_part_style_t top;
  /*!
     Right part of rectangle.
    */
  rsc_part_style_t right;
  /*!
     Bottom part of rectangle.
    */
  rsc_part_style_t bottom;
  /*!
     Background of rectangle.
    */
  rsc_part_style_t bg;
  /*!
     Left-Top of rectangle.
    */
  rsc_part_style_t left_top;
  /*!
     Right-Top of rectangle.
    */
  rsc_part_style_t right_top;
  /*!
     Right-Bottom  of rectangle.
    */
  rsc_part_style_t right_bottom;
  /*!
     Left-Bottom of rectangle.
    */
  rsc_part_style_t left_bottom;
}rsc_rstyle_t;

/*!
  font type
  */
typedef enum
{
  /*!
    arrary font
    */
  FONT_ARRAY = 0,
  /*!
    vector font
    */
  FONT_VECTOR,   
  /*!
    mt defined font.
    */
  FONT_MT,    
}font_t;

/*!
  strok
  */
#define VFONT_STROK   0x00000001

/*!
  bold
  */
#define VFONT_BOLD    0x00000002

/*!
  italic
  */
#define VFONT_ITALIC  0x00000004

/*!
   Font style structure.
  */
typedef struct
{
  /*!
     Font lib id.
    */
  u16 font_id;
  /*!
     Color.
    */
  u32 color;
  /*!
    face. just for vector font
    */
  u8 face_id;
  /*!
    width. just for vector font
    */
  u8 width;
  /*!
    height. just for vector font
    */
  u8 height;    
  /*!
    attribute
    */
  u32 attr;    
}rsc_fstyle_t;

/*!
   Invalid rstyle id.
  */
#define INVALID_RSTYLE_IDX    0xFF
/*!
   Invalid fstyle id.
  */
#define INVALID_FSTYLE_IDX    0xFF


/*!
   Palette type
  */
typedef enum
{
/*!
   Palette in RGB type
  */
  RSC_PALETTE_RGB,
/*!
   Palette in RGBA type
  */
  RSC_PALETTE_RGBA,
/*!
   Palette in YUV type
  */
  RSC_PALETTE_YUV,
/*!
   Palette in YUVA type
  */
  RSC_PALETTE_YUVA
}palette_type_t;


/*!
   Resource information structure. Each resource type is relative to a "rsc_info"
   structure, and these structures are stored in the head of the  binary file one by one.
   Users can compare the resource type we needed with the type of structure, if the same,
   we can get the resource idx offset by the offset variable in structure.
  */
typedef struct
{
/*!
   Resource type of a specified resource
  */
  u8 type;
/*!
   Resource count, it specified how many resources of this type in the binary file.
  */
  u16 count;
/*!
   Resource idx table offset, it specified the offset to the resource idx table(the address of
   structure "rsc_idx").
  */
  u32 offset;
}rsc_info_t;


/*!
   Resource idx structure. Each resource id is relative to a "rsc_idx"structure,
   and these structures are stored in binary file one by one. Users can compare the
   resource id we needed with the resource id of structure, if the same ,we can get the
   resource head offset by the offset variable in structure.
  */
typedef struct
{
  /*!
     Resource id
    */
  u16 id;
  /*!
     It specified the offset to the resource head
    */
  u32 offset;
}rsc_idx_t;


/*!
   Common resource head structure. Different resource type has different resource head,
   but they all contains the common resource head in the begining of resource head, then
   follows the private information about the resource, after these it's the data we need.
   So, once we get the resource head, we get the the data offset too.
  */
typedef struct
{
  /*!
     Resource type
    */
  u8 type;
  /*!
     Data compressed mode
    */
  u8 c_mode;
  /*!
     Resource id
    */
  u16 id;
  /*!
     Data size after compress
    */
  u32 cmp_size;
  /*!
     Data size before compress
    */
  u32 org_size;
}rsc_head_t;


/*!
   Font resource head structure
  */
typedef struct
{
  /*!
     Common resource head
    */
  rsc_head_t head;
  /*!
     Font type. RSC_FONT_TYPE_ARRAY  or  RSC_FONT_TYPE_VECTOR
    */
  u8 type;
  /*!
     Font width, if non-zero then means monospaced font
    */
  u8 width;
  /*!
     Font height
    */
  u8 height;
  /*!
     Font count
    */
  u16 count;
  /*!
     Start code
    */
  u16 start_code;
  /*!
     End code
    */
  u16 end_code;
}rsc_font_t;


/*!
   Palette resource head sturcture
  */
typedef struct
{
  /*!
     Common resource head
    */
  rsc_head_t head;
  /*!
     Palette type. Refer to "palette_type_t"
    */
  u8 type;
  /*!
     Palette color number
    */
  u16 color_num;
}rsc_palette_t;

/*!
   Image type
  */
typedef enum
{
  /*!
     Invalid
    */
  IMG_TYPE_INVALID,
  /*!
     Bitmap
    */
  IMG_TYPE_BMP,
  /*!
     Png
    */
  IMG_TYPE_PNG,
  /*!
     Gif
    */
  IMG_TYPE_GIF
}image_type_t;

/*!
   Bitmap resource head sturcture
  */
typedef struct
{
  /*!
     Common resource head
    */
  rsc_head_t head;
  /*!
     Image type
    */
  image_type_t image_type;
  /*!
     Pixel type
    */
  u8 pixel_type;
  /*!
     Colorkey enable
    */
  u8 enable_ckey;
  /*!
     Palette id
    */
  u16 palette_id;
  /*!
     Bitmap width
    */
  u16 width;
  /*!
     Bitmap height
    */
  u16 height;
  /*!
     the color key value.
    */
  u32 colorkey;
  /*!
     Pitch
    */
  u32 pitch;
}rsc_bitmap_t;

/*!
   String resource head sturcture
  */
typedef struct
{
  /*!
     Resource head.
    */
  rsc_head_t head;
  /*!
     String count
    */
  u16 count;
}rsc_string_t;

/*!
   Script resource head sturcture
  */
typedef struct
{
  /*!
     Resource head.
    */
  rsc_head_t head;
  /*!
     Script
    */
  //  u8*                 script;
}rsc_script_t;

/*!
   Word resource head sturcture
  */
typedef struct
{
  /*!
     Common resource head.
    */
  rsc_head_t head;
  /*!
     Table address.
    */
  u8 *p_tab;
}rsc_charmap_t;

/*!
   Character cache content structure. It describes the data stored in the cache.
  */
typedef struct
{
  /*!
     The character code of the data
    */
  u16 char_code;
  /*!
     The font color of the character
    */
  rsc_fstyle_t char_style;
  /*!
     The width of the character stored in the cache
    */
  u16 width;
  /*!
     The height  of the character stored in the cache
    */
  u16 height;
  /*!
    x step
    */
  u16 x_step;    
  /*!
     A pointer to the cache buffer, which stores the character data.
    */
  void *p_data_buffer;
}char_cache_t;

/*!
   This structure defines some variables for initialize resource module, mainly about memory
   allocate
  */
typedef struct
{
  /*!
     Common data buf size
    */
  u32 comm_buf_size;
  /*!
     Max bitmap size
    */
  u32 bmp_buf_size;
  /*!
     Max language size
    */
  u32 strtab_buf_size;
  /*!
     Palette buffer size
    */
  u16 pal_buf_size;
  /*!
     Max font size
    */
  u32 font_buf_size;
  /*!
     Max script buffer size
    */
  u32 script_buf_size;
  /*!
     Resource address
    */
  u32 rsc_data_addr;
  /*!
     Flash base.
    */
  u32 flash_base;
  /*!
     Actual rectangle style number
    */
  u16 rstyle_cnt;
  /*!
     Rectangle style table
    */
  rsc_rstyle_t *p_rstyle_tab;
  /*!
     Actual font style number
    */
  u8 fstyle_cnt;
  /*!
     Font style table
    */
  rsc_fstyle_t *p_fstyle_tab;
}rsc_config_t;


/*!
   Common information of all the resource.
  */
typedef struct
{
  /*!
     Resource id.
    */
  u16 id;

  /*!
     Unzip buffer addr.
    */
  u32 unzip_buf_addr;
  /*!
     Unzip buffer size.
    */
  u32 unzip_buf_size;
  /*!
     Resource head address.
    */
  u32 rsc_hdr_addr;
}rsc_comm_info_t;

/*!
   Font resource info.
  */
typedef struct
{
  /*!
     Common info.
    */
  rsc_comm_info_t comm;
  /*!
     Font head.
    */
  rsc_font_t hdr_font;
}rsc_font_info_t;

/*!
   Palette info.
  */
typedef struct
{
  /*!
     Common info.
    */
  rsc_comm_info_t comm;
  /*!
     Palette head.
    */
  rsc_palette_t hdr_pal;
}rsc_pal_info_t;

/*!
   Bitmap info.
  */
typedef struct
{
  /*!
     Common info.
    */
  rsc_comm_info_t comm;
  /*!
     Bitmap head.
    */
  rsc_bitmap_t hdr_bmp;
}rsc_bmp_info_t;

/*!
   Script info.
  */
typedef struct
{
  /*!
     Common info.
    */
  rsc_comm_info_t comm;
  /*!
     Script data head.
    */
  rsc_script_t hdr_spt;
}rsc_script_info_t;

/*!
   String table info.
  */
typedef struct
{
  /*!
     Common info.
    */
  rsc_comm_info_t comm;
  /*!
     String table data head.
    */
  rsc_string_t hdr_strtab;
}rsc_strtab_info_t;

/*!
  char info
  */
typedef struct
{
  /*!
    xoffset
    */
  s16 xoffset;
  /*!
    yoffset
    */
  s16 yoffset;
  /*!
    width
    */
  u16 width;
  /*!
    height
    */
  u16 height;
  /*!
    color key
    */
  u32 ckey;
  /*!
    char buffer
    */
  u8 *p_char;
  /*!
    alpha map
    */
  u8 *p_alpha;
  /*!
    pitch.
    */
  u16 pitch;    
  /*!
    alpha map pitch
    */
  u16 alpha_pitch;
  /*!
    x step.
    */
  u16 x_step;   
  /*!
    step width
    */
  u16 step_width;
  /*!
    step height
    */
  u16 step_height;    
  /*!
    is use alpha blending(just for vfont).
    */
  BOOL is_alpha_spt;  
  /*!
    strok alpha
    */
  void *p_strok_alpha;
  /*!
    strok char
    */
  void *p_strok_char;
  /*!
     Bits per piexel.
    */
  u8 bpp;  
}rsc_char_info_t;

/*!
  get char attribute.
  */
typedef BOOL (*get_attr_t)(void *p_priv,
  u16 char_code, rsc_fstyle_t *p_fstyle, u16 *p_width, u16 *p_height);

/*!
  get char data.
  */
typedef BOOL (*get_char_t)(void *p_priv,
  u16 char_code, rsc_fstyle_t *p_fstyle, rsc_char_info_t *p_info);

/*! 
  Structure of all resource information.
  */
typedef struct
{
  /*!
    Resource start address
    */
  u8 *p_rsc_data_addr;
  /*!
    Memory heap for resource module
    */
  lib_memp_t heap;
  /*!
    Memory heap for resource module
    */
  void *p_heap_addr;
  /*!
    Max common data buffer size, for read data
    */
  u32 comm_buf_size;
  /*!
    Common data buffer
    */
  u8 *p_comm_buf_addr;
  /*!
    Current bitmap info(If the bitmap id equals to current bitmap id,
    then we can read data from bitmap buffer directly for saving time!)
    */
  rsc_bmp_info_t bmp_info;
  /*!
    Current palette info(If the palette id equals to current palette id,
    then we can read data from palette buffer directly for saving time!)
    */
  rsc_pal_info_t palette_info;
  /*!
    Current string table info(If the string table id equals to current string table id,
    then we can read data from string table buffer directly for saving time!)
    */
  rsc_strtab_info_t strtab_info;
  /*!
    Current common font info(If the font id equals to current local font id,
    then we can read data from local font buffer directly for saving time!)
    */
  rsc_font_info_t font_info;
  /*!
    Current script info(If the script id equals to current script id,
    then we can read data from script buffer directly for saving time!)
    */
  rsc_script_info_t script_info;

  /*!
    Language and font info
    */
  u16 curn_language_id;

  /*!
    Palette & style 
    */
  u16 curn_palette_id;

  /*!
    Actual rectangle style number
    */
  u16 rstyle_cnt;
  /*!
    Rectangle style table
    */
  rsc_rstyle_t *p_rstyle_tab;
  /*!
    Actual font style number
    */
  u8 fstyle_cnt;
  /*!
    Font style table
    */
  rsc_fstyle_t *p_fstyle_tab;
  /*!
    Flash base.
    */
  u32 flash_base;
  /*!
    charsto device.
    */
  void *p_charsto_dev;
  /*!
    vfont handle.
    */
  handle_t vf_handle;
  /*!
    get attr
    */ 
  get_attr_t get_attr;
  /*!
    get char
    */
  get_char_t get_char;
  /*!
    priv data for vf & af
    */
  void *p_priv;      
}rsc_main_t;

/*!
   Get resource header

   \param[in] rsc_handle : resource handle.
   \param[in] type : resource type
   \param[in] id : resource id
   \return : point to resource head
  */
u32 rsc_get_hdr_addr(handle_t rsc_handle, u8 type, u16 id);

/*!
   Get bitmap data.

   \param[in] rsc_handle : resource handle.
   \param[in] bmp_id : bitmap id
   \param[in] p_hdr_bmp : bitmap head.
   \param[out] p_data : data buffer address.
   \return : TRUE for success, otherwise FALSE.
  */
BOOL rsc_get_bmp(handle_t rsc_handle, u16 bmp_id, rsc_bitmap_t *p_hdr_bmp, u8 **p_data);

/*!
  get font
  */
BOOL rsc_get_font(handle_t rsc_handle, u16 font_id, rsc_font_t *p_hdr_font, u8 **p_data);

/*!
   Get char data.

   \param[in] rsc_handle : resource handle.
   \param[in] p_hdr_font : font head
   \param[in] p_font_data : font data
   \param[in] p_style : style
   \param[in] char_code : char code
   \param[out] p_info : char info
   \param[in] bpp : bpp.
   
   \return : Ture or False.
  */
BOOL rsc_get_char(handle_t rsc_handle,
  rsc_fstyle_t *p_style, u16 char_code, rsc_char_info_t *p_info);

/*!
   Get string data.

   \param[in] rsc_handle : resource handle.
   \param[in] strtab_id : string table id
   \param[in] string_id : string id
   \param[out] p_data : data buffer address.
   \return : TRUE for success, otherwise FALSE.
  */
BOOL rsc_get_string(handle_t rsc_handle, u16 strtab_id, u16 string_id, u8 **p_data);

/*!
   Get palette data.

   \param[in] rsc_handle : resource handle.
   \param[in] palette_id : palette id
   \param[in] p_hdr_pal : palette head.
   \param[out] p_data : data buffer address.
   \return : TRUE for success, otherwise FALSE.
  */
BOOL rsc_get_palette(handle_t rsc_handle, u16 palette_id, rsc_palette_t *p_hdr_pal, u8 **p_data);


/*!
   Get script data.

   \param[in] rsc_handle : resource handle.   
   \param[in] spt_id : script id
   \param[in] p_hdr_spt : script head.
   \param[out] p_data : data buffer address.
   \return : TRUE for success, otherwise FALSE.
  */
BOOL rsc_get_script(handle_t rsc_handle, u16 spt_id, rsc_script_t *p_hdr_spt, u8 **p_data);


/*!
   Set current language.

   \param[in] rsc_handle : resource handle.   
   \param[in] language : language id

   \return : NULL
  */
void rsc_set_curn_language(handle_t rsc_handle, u16 language);

/*!
   Get current language.

   \param[in] rsc_handle : resource handle.   

   \return : language id
  */
u16 rsc_get_curn_language(handle_t rsc_handle);

/*!
   Set current language.

   \param[in] rsc_handle : resource handle.   
   \param[in] palette : palette id

   \return : NULL
  */
void rsc_set_curn_palette(handle_t rsc_handle, u16 palette);

/*!
   Get current language.
   
   \param[in] rsc_handle : resource handle.   

   \return : palette id
  */
u16 rsc_get_curn_palette(handle_t rsc_handle);


/*!
   Init osd resource

   \param[in] p_config : config data
   
   \return : resource handle.
  */
handle_t rsc_init(rsc_config_t *p_config);

/*!
   Release osd resource.

   \param[in] rsc_handle : resource handle.

   \return : NULL
  */
void rsc_release(handle_t rsc_handle);

/*!
  reset fstyle.
  */
void rsc_set_fstyle(handle_t rsc_handle, u32 idx, rsc_fstyle_t *p_new_fstyle);

/*!
  reset rstyle.
  */
void rsc_set_rstyle(handle_t rsc_handle, u32 idx, rsc_rstyle_t *p_new_rstyle);

/*!
   Get common buffer address.

   \param[in] rsc_handle : resource handle.

   \return common buffer address.
  */
u8 *rsc_get_comm_buf_addr(handle_t rsc_handle);

/*!
   Get common buffer size.

   \param[in] rsc_handle : resource handle.   

   \return common buffer size.
  */
u32 rsc_get_comm_buf_size(handle_t rsc_handle);

/*!
   Get font style by index
   
   \param[in] rsc_handle : resource handle.   
   \param[in] idx : font style index
   
   \return : font style
  */
rsc_fstyle_t *rsc_get_fstyle(handle_t rsc_handle, u32 idx);

/*!
   Get rect style by index.

   \param[in] rsc_handle : resource handle.   
   \param[in] idx : rect style index
   
   \return : rect style
  */
rsc_rstyle_t *rsc_get_rstyle(handle_t rsc_handle, u32 idx);

/*!
  set font style.
  */
void rsc_set_fstyle(handle_t rsc_handle, u32 idx, rsc_fstyle_t *p_new_fstyle);

/*!
  set rect-style.
  */
void rsc_set_rstyle(handle_t rsc_handle, u32 idx, rsc_rstyle_t *p_new_rstyle);

/*!
    Determine a rectangle style has a background or not

   \param[in] p_rstyle		: a pointer point to the specified rstyle
   \return      : has background or not.
  */
BOOL rsc_is_rstyle_has_bg(rsc_rstyle_t *p_rstyle);

/*!
    Determine a rectangle style need copy its background or not

 \param[in] p_rstyle		: a pointer point to the specified rstyle
 \return 			: need or not.
 */
BOOL rsc_is_rstyle_cpy_bg(rsc_rstyle_t *p_rstyle);


/*!
   Get char width and height

   \param[in] rsc_handle : resource handle.   
   \param[in] char_code : unicode
   \param[in] p_style : font style
   \param[in] p_width : Output char width
   \param[in] p_height : Output char height

   \return : NULL
  */
void rsc_get_char_attr(handle_t rsc_handle,
  u16 char_code, rsc_fstyle_t *p_style, u16 *p_width, u16 *p_height);

/*!
   Get string width and height.

   \param[in] rsc_handle : resource handle.   
   \param[in] p_str : unicode string
   \param[in] p_style : font style
   \param[in] p_width : Output string width
   \param[in] p_height : Output string height
   
   \return : SUCCESS
  */
RET_CODE rsc_get_string_attr(handle_t rsc_handle,
  u16 *p_str, rsc_fstyle_t *p_style, u16 *p_width, u16 *p_height);

/*!
   Get string id width.

   \param[in] rsc_handle : resource handle.
   \param[in] string_id : string id
   \param[in] f_style_idx : font style index
   
   \return : string width.
  */
u16 rsc_get_strid_width(handle_t rsc_handle, u16 string_id, u32 f_style_idx);

/*!
   Get unicode string width.

   \param[in] rsc_handle : resource handle.
   \param[in] p_str : unicode string
   \param[in] f_style_idx : font style index.
   
   \return : unicode string width.
  */
u16 rsc_get_unistr_width(handle_t rsc_handle, u16 *p_str, u32 f_style_idx);


/*!
   Get data from ram or flash.

   \param[in] rsc_handle : resource handle.
   \param[in] offset : source address.
   \param[in] p_buf       : destination buffer.
   \param[in] len    : data length.
   
   \return      : NULL.
  */
void rsc_read_data(handle_t rsc_handle, u32 offset, void *p_buf, u32 len);
#ifdef __cplusplus
}
#endif

#endif
