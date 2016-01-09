/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SURFACE_H__
#define __SURFACE_H__
/*!
  matrix
  */
typedef struct
{
  /*!
    xx
    */
  double xx;
  /*!
    xy
    */
  double xy;
  /*!
    yx
    */
  double yx;
  /*!
    yy
    */
  double yy;
  /*!
    x0
    */
  double x0;
  /*!
    y0
    */
  double y0;
}matrix_t;

/*!
  surface trape param.
  */
typedef struct
{
  /*!
    top left pos
    */
  u32 top_start_x;
  /*!
    bottom left pos
    */
  u32 bottom_start_x;
  /*!
    top  w
    */
  u32 top_len;
  /*!
    bottom w
    */
  u32 bottom_len;
  /*!
  direction, 
  0:              1: 
  |\              /|
  | |            ||
  |/              \|
  */
  u32 direction;
  /*!
  dert factor denominator
  */
  u32 dert_deno;
   /*!
  dert factor numerator
  */
  u32 dert_num;      
}trape_t;

/*!
  transformations.
  */
typedef struct
{
  /*!
    matrix for trans.
    */
  matrix_t matrix;
  /*!
    alpha for trans.
    */
  u32 alpha;
  /*!
    step
    */
  u32 step;    
}transformation_t;

/*!
   Color definition in red/green/blue/alpha format
  */
typedef struct
{
  /*!
     Red
    */
  u8 r;
  /*!
     Green
    */
  u8 g;
  /*!
     Blue
    */
  u8 b;
  /*!
     Alpha: 0 for transparent, and 255 for solid
    */
  u8 a;
} color_t;

/*!
   Defines raster operation type
  */
typedef enum
{
  /*!
     Set to the new value, erase the original.
    */
  SURFACE_ROP_SET = 0x01,
  /*!
     AND'd the new value with the original.
    */
  SURFACE_ROP_AND,
  /*!
     OR'd the new value with the original.
    */
  SURFACE_ROP_OR,
  /*!
     XOR'd the new value with the original.
    */
  SURFACE_ROP_XOR,
  /*!
    copy with pattern.
    */
  SURFACE_ROP_PATCOPY,
  /*!
    alpha blending by dst alpha
    */
  SURFACE_ROP_BLEND_DST,
  /*!
    alpha blending by src alpha.
    */
  SURFACE_ROP_BLEND_SRC,    
} surface_rop_type_t;

/*!
   The structure is defined to descript a palette.
  */
typedef struct
{
  /*!
     The number of palette entries.
    */
  u16 cnt;
  /*!
     the entries of palette.
    */
  color_t *p_entry;
} palette_t;

/*!
   The structure is defined to descript a bitmap.
  */
typedef struct
{
  /*!
     The pixel format of the bitmap.
    */
  u8 format;
  /*!
     The flag for use color key.
    */
  u8 enable_ckey;
  /*!
     Bits per piexel.
    */
  u8 bpp;
  /*!
     The palette of the bitmap.
    */
  palette_t pal;
  /*!
     The width of the bitmap.
    */
  u16 width;
  /*!
     The height of the bitmap.
    */
  u16 height;
  /*!
     The bits of the bitmap.
    */
  u8 *p_bits;
  /*!
     The pitch of the bitmap.
    */
  u32 pitch;
  /*!
    The alpha map.
    */
  u8 *p_alpha; 
  /*!
    alpha pitch
    */
  u32 alpha_pitch;    
  /*!
    color
    */
  u32 color;    
  /*!
     The color key value.
    */
  u32 colorkey;
  /*!
    is font
    */
  BOOL is_font;  
  /*!
    strok alpha
    */
  u8 *p_strok_alpha;
  /*!
    stroke char
    */
  u8 *p_strok_char;    
} bitmap_t;


/*!
   Pixel format from rsc tool
  */
typedef enum
{
  /*!
     RGB 8 bit.
    */
  COLORFORMAT_RGB8BIT = 0,
  /*!
     RGB 4 bit.
    */
  COLORFORMAT_RGB4BIT,
  /*!
     YUV 655.
    */
  COLORFORMAT_YUV655,
  /*!
     RGB 1 bit.
    */
  COLORFORMAT_RGB1BIT,
  /*!
     RGB 2 bit.
    */
  COLORFORMAT_RGB2BIT,
  /*!
     RGB 555.
    */
  COLORFORMAT_RGB555,
  /*!
     RGB 565.
    */
  COLORFORMAT_RGB565,
  /*!
     RGB 888.
    */
  COLORFORMAT_RGB888,
  /*!
     RGBA 8888.
    */
  COLORFORMAT_RGBA8888,
  /*!
     ARGB 8888.
    */
  COLORFORMAT_ARGB8888,
  /*!
     RGBA 5551.
    */
  COLORFORMAT_RGBA5551,
  /*!
     ARGB 1555.
    */
  COLORFORMAT_ARGB1555,
}pix_type_t;

/*!
   The macro indicates enable display or NOT.
  */
#define SURFACE_ATTR_EN_DISPLAY     0x01
/*!
   The macro indicates enable clipping or NOT.
  */
#define SURFACE_ATTR_EN_CLIP        0x02
/*!
   The macro indicates enable palette or NOT.
  */
#define SURFACE_ATTR_EN_PALETTE     0x04
/*!
   The macro indicates enable colorkey or NOT.
  */
#define SURFACE_ATTR_EN_COLORKEY    0x08

/*!
   Invalid color key for all format.
  */
#define SURFACE_INVALID_COLORKEY    (~1)



/*!
   Specifis the capabilities of descriptor.
  */
#define SURFACE_DESC_CAPS          0x01
/*!
   Specifis the fixel format of descriptor.
  */
#define SURFACE_DESC_FORMAT        0x02
/*!
   Specifis the width of descriptor.
  */
#define SURFACE_DESC_WIDTH         0x04
/*!
   Specifis the height of descriptor.
  */
#define SURFACE_DESC_HEIGHT        0x08
/*!
   Specifis the pitch of descriptor.
  */
#define SURFACE_DESC_PITCH         0x10

/*!
   x.
  */
#define SURFACE_DESC_X        0x20
/*!
   y.
  */
#define SURFACE_DESC_Y         0x40

/*!
   Assign the display buffer directly.
  */
#define SURFACE_CAPS_MEM_ASSIGN    0x01
/*!
   Allocate the display buffer from system memory.
  */
#define SURFACE_CAPS_MEM_SYSTEM    0x02


/*!
  screen layer.
  */
typedef enum
{
  /*!
    screen on osd0 layer.
    */
  SURFACE_OSD0,
  /*!
    screen on sod1 layer.
    */
  SURFACE_OSD1,
  /*!
    screen on sub layer.
    */
  SURFACE_SUB,
}surface_layer_t;

/*!
   The structure is defined to descript a surface descriptor.
  */
typedef struct
{
  /*!
    surface layer.
    */
  surface_layer_t layer;    
  /*!
     The flag of create surface
    */
  u8 flag;
  /*!
     The capability
    */
  u8 caps;
  /*!
     The pixel format
    */
  pix_type_t format;
  /*!
    x
    */
  u16 x;
  /*!
    y
    */
  u16 y;    
  /*!
     The width
    */
  u16 width;
  /*!
     The height
    */
  u16 height;
  /*!
     The pitch
    */
  u32 pitch;
  /*!
     The palette
    */
  palette_t *p_pal;
  /*!
     The display buffer odd & even
    */
  u8 *p_vmem[2];
} surface_desc_t;

/*!
   Enable display for all surface.

   \param[in] is_enable Eanble or NOT.
  */
void surface_enable(void * p_surf, BOOL is_enable);

/*!
   Creates a surface with a specifies descriptor.

   \param[out] pp_surf Points to a address of surface.
   \param[in] p_desc Specifies the descriptor.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_create(void **pp_surf, surface_desc_t *p_desc);

/*!
   Deletes a surface.

   \param[in] p_surf Points to a surface.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_delete(void *p_surf);

/*!
   Display a surface.

   \param[in] p_surf Points to a surface.
   \param[in] is_display Display the surface or NOT.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_display(void *p_surf, BOOL is_display);

/*!
   Sets the global alpha blend of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] alpha Specifies the alpha value.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_alpha(void *p_surf, u8 alpha);

/*!
   Sets the transparence of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] index Specifies the index of the transparent color.
   \param[in] alpha Specifies the alpha level of transparency.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_trans(void *p_surf, u32 index, u8 alpha);

/*!
   Initialize the palette of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_pal Points to the specifies palette.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_init_palette(void *p_surf, palette_t *p_pal);

/*!
   Sets the palette of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] start Specifies the index of beginning.
   \param[in] len Specifies the length of entrys be set.
   \param[in] p_entry Points to the entrys.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_palette(void *p_surf, u16 start, u16 len, color_t *p_entry);

/*!
   Gets the palette of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] start Specifies the index of beginning.
   \param[in] len Specifies the length of entrys be set.
   \param[in] p_entry Points to the entrys.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_palette(void *p_surf, u16 start, u16 len, color_t *p_entry);

/*!
   Sets the colorkey of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] ckey Specifies the colorkey.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_colorkey(void *p_surf, u32 ckey);

/*!
   Sets the transparent key of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] ckey Specifies the colorkey.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_trans_clr(void *p_surf, u32 trans_clr);

/*!
   Gets the transparence of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_ckey Points to the address of the colorkey.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_colorkey(void *p_surf, u32 *p_ckey);

/*!
   Sets the clipping rectangle of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to the rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_cliprect(void *p_surf, rect_t *p_rc);

/*!
   Gets the clipping rectangle of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to the rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_cliprect(void *p_surf, rect_t *p_rc);

/*!
   Sets the display rectangle of a surface.

   \param[in] p_surf Points to a surface.
   \param[in] p_rc Points to the rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_set_srcrect(void *p_surf, rect_t *p_rc);

/*!
   Gets the display rectangle of a surface.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_srcrect(void *p_surf, rect_t *p_rc);

/*!
   Fill a rectangle with the specified color.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_fill_rect(void *p_surf, rect_t *p_rc, u32 value);

/*!
   Fill a rectangle with the specified bitmap.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_fill_bmp(void *p_surf, rect_t *p_rc, bitmap_t *p_bmp, u16 x_off, u16 y_off);

/*!
   Gets the image from a surface with the specified rectangle.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_get_bits(void *p_surf, rect_t *p_rc, void *p_buf, u32 pitch);

/*!
   Bitblt, performs a bit-block transfer of the color data corresponding to
   a rectangle of pixels from the specified source surface into a
   destination surface.

   \return if sucessful, return SUCCESS. Otherwise return ERR_FAILTURE.
  */
RET_CODE surface_bitblt(void *p_src_surface,
  u16 sx, u16 sy, u16 sw, u16 sh,
  void *p_dst_surface, u16 dx, u16 dy, surface_rop_type_t rop, u32 rop_pat);

/*!
   Gets surface attr.

   \param[in] p_surf : Points to surf.

   \return attr.
  */
u8 surface_get_attr(void *p_surf);

/*!
   Gets surface handle.

   \param[in] handle : handle.
   \param[in] p_surf : Points to surf.

   \return handle.
  */
s32 surface_get_handle(void *p_surf);

/*!
   Gets surface bpp.

   \param[in] p_surf : Points to surf.

   \return bpp.
  */
u8 surface_get_bpp(void *p_surf);

/*!
   Gets surface format.

   \param[in] p_surf : Points to surf.

   \return format.
  */
u8 surface_get_format(void *p_surf);

/*!
   Gets palette address.

   \param[in] p_surf : Points to surf.

   \return palette.
  */
palette_t *surface_get_palette_addr(void *p_surf);

/*!
   surface start batch.

   \param[in] p_surf : Points to surf.

   \return NULL.
  */
void surface_start_batch(void *p_surf);

/*!
   surface start batch.

   \param[in] p_surf : Points to surf.
   \param[in] is_sync : sync paint or not

   \return NULL.
  */
void surface_end_batch(void *p_surf, BOOL is_sync, rect_t *p_rect);
/*!
  surface dert scale
  */
RET_CODE surface_dert_scale(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst);
/*!
  surface stretch blt
  */
RET_CODE surface_stretch_blt(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst);
/*!
  surface trape blt
  */
RET_CODE surface_trape_blt(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst, trape_t *p_trape);
/*!
  surface paint blt
  */
RET_CODE surface_paint_blt(void *p_dst_surface, rect_t *p_dst,
  cct_gpe_paint2opt_t *p_popt);

/*!
  rect stroke.
  */
RET_CODE surface_rect_stroke(void *p_surface, rect_t *p_rect, u16 depth, u32 color);
  
/*!
  surface rotate mirror
  */
RET_CODE surface_rotate_mirror(void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, u16 dx, u16 dy, 
  lld_gfx_rotator_t rotate_mod, 
  lld_gfx_mirror_t mirror_mod);
/*!
  surface blt 3src
  */
RET_CODE surface_blt_3src(
  void *p_src_surface, rect_t *p_src,
  void *p_dst_surface, rect_t *p_dst,
  void *p_ref_surface, rect_t *p_ref, cct_gpe_blt3opt_t *p_opt3);

/*!
  surface draw pie.
  */
void surface_draw_pie(void *p_surface,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u32 color);

/*!
  surface draw arc.
  */
void surface_draw_arc(void *p_surface,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u16 thick, u32 color);  
#endif
