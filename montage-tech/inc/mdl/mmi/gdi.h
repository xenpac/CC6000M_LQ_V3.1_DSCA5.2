/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GDI_H__
#define __GDI_H__

/*!
  Macro for debug
  */
#define GDI_PRINTF DUMMY_PRINTF

/*!
  Handle to device context.
  */
typedef u32    hdc_t;

/*!
  Handle to compatible bitmap object.
  */
typedef u32    hbitmap_t;

/*!
  Clipping rectangle structure.
  */
typedef struct cliprc
{
  /*!
    The clipping rectangle itself.
    */
  rect_t    rc;
  /*!
    The next clipping rectangle.
    */
  struct cliprc *p_next;
  /*!
    The previous clipping rectangle.
    */
  struct cliprc *p_prev;
}cliprc_t;

/*!
  Clipping region structure, alos used for general regions.
  */
typedef struct
{
  /*!
    The bounding rect of the region.
    */
  rect_t       bound;
  /*!
    Head of the clipping rectangle list.
    */
  cliprc_t *p_head;
  /*!
    Tail of the clipping rectangle list.
    */
  cliprc_t *p_tail;
  /*!
    The id of private block data heap used to allocate clipping rectangles.
    */
  lib_memf_t *p_heap;
}cliprgn_t;

/*!
  Information about clip region.
  */
typedef struct
{
  /*!
    Clip region
    */
  cliprgn_t crgn;
}crgn_info_t;

/*!
  Initializes a clipping region.

  \param[in] rgn : Points to the region to be initialized.
  \param[in] p_heap : Memory heap for clip region.  
  \return : NULL
  */
void gdi_init_cliprgn(cliprgn_t *p_rgn, lib_memf_t *p_heap);

/*!
  Empties a clip region.

  \param[in] p_rgn : clip region to be emptied.
  \return : NULL
  */
void gdi_empty_cliprgn(cliprgn_t *p_rgn);

/*!
  Copies a clip region to another.

  \param[in] p_dst_rgn: destination region.
  \param[in] p_src_rgn: source region.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_copy_cliprgn(cliprgn_t *p_dst_rgn, const cliprgn_t *p_src_rgn);

/*!
  Intersects two regions.

  \param[out] p_dst_rgn : destination region, the result of intersected.
  \param[in] p_src_rgn1 : region to be intersected.
  \param[in] p_src_rgn2 : another region to be intersected.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_intersect_cliprgn(cliprgn_t *p_dst_rgn,
  const cliprgn_t *p_src_rgn1, const cliprgn_t *p_src_rgn2);

/*!
  Unions two regions.

  \param[out] p_dst_rgn : destionation region, result of union.
  \param[in] p_src_rgn1 : region to be union.
  \param[in] p_src_rgn2 : another region to be union.  
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_union_cliprgn(cliprgn_t *p_dst_rgn, const cliprgn_t *p_src_rgn1,
  const cliprgn_t *p_src_rgn2);

/*!
  Subtract a region from another.

  \param[out] p_difference_rgn : result region after subtract.
  \param[in] p_minuend_rgn : region to be subtracted.
  \param[in] p_subtrahend_rgn : subtract region.  
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_subtract_cliprgn(cliprgn_t *p_difference_rgn,
  const cliprgn_t *p_minuend_rgn, const cliprgn_t *p_subtrahend_rgn);

/*!
  Do XOR operation between two regions.

  \param[out] p_dst_rgn : result region after XOR.
  \param[in] p_src_rgn1 : one region for XOR operation.
  \param[in] p_src_rgn2 : another one for XOR operation.  
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_xor_cliprgn(cliprgn_t *p_dst_rgn, const cliprgn_t *p_src_rgn1,
  const cliprgn_t *p_src_rgn2);

/*!
  Gets the bounding rectangle of a region.

  \param[in] p_rgn : a region structure.
  \param[out] p_rc : rectangle that contains the coordinate of bound.
  \return : NULL
  */
void gdi_get_cliprgn_boundrect(cliprgn_t *p_rgn, rect_t *p_rc);

/*!
  To judge whether a region is empty.

  \param[in] p_rgn : region to be judged.
  \return : TRUE for empty, otherwise not .
  */
BOOL gdi_is_empty_cliprgn(const cliprgn_t *p_rgn);

/*!
  Move region by the specified coordinates.

  \param[in,out] p_rgn : source region, and also the destionation region after move operation.
  \param[in] x : offset on x axes.
  \param[in] y : offset on y axes.
  \return : NULL
  */
void gdi_offset_cliprgn(cliprgn_t *p_rgn, s16 x, s16 y);


/*!
  Set a region by a specified rectangle.

  \param[out] p_rgn : region after set.
  \param[in] p_rc : specified rectangle.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_set_cliprgn(cliprgn_t *p_rgn, const rect_t *p_rc);

/*!
  Add a rectangle to a exsiting clip region.

  \param[in, out] p_rgn : the exsited clip region, and it's also the result of add operation.
  \param[in] p_rc : the specified rectangle.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_add_cliprect(cliprgn_t *p_rgn, const rect_t *p_rc);

/*!
  Intersects a rectangle with a exsiting region.

  \param[in, out] p_rgn : the exsited clip region, and it's also the result of intersect operation.
  \param[in] p_rc : the specified rectangle.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_intersect_cliprect(cliprgn_t *p_rgn, const rect_t *p_rc);

/*!
  Subtarcts a rectangle from a exsiting region.

  \param[in, out] p_rgn : the exsited clip region, and it's also the result of subtracts operation.
  \param[in] p_rc : the specified rectangle.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_subtract_cliprect(cliprgn_t *p_rgn, const rect_t *p_rc);


/*!
  To judge whether a point is in a region.

  \param[in] p_rgn : a specified region for the judgement.
  \param[in] x : the x-coordinate of the specified point.
  \param[in] y : the y-coordinate of the specified point.
  \return : TRUE for yes, otherwise no.
  */
BOOL gdi_is_pt_in_cliprgn(const cliprgn_t *p_rgn, s16 x, s16 y);

/*!
  To judge whether a rectangle is in a region.

  \param[in] p_rgn : a specified region for the judgement.
  \param[in] p_rc : the specified rectangle.
  \return : TRUE for yes, otherwise no.
  */
BOOL gdi_is_rect_in_cliprgn(const cliprgn_t *p_rgn, const rect_t *p_rc);


/*!
  Initializes a region to be an enclosed polygon.

  \param[in] p_rgn : clip region.
  \param[in] p_pts : points.
  \param[in] vertices : vertices.
  \return : True for success, otherwise failed.
  */
//BOOL gdi_init_polygon_rgn (cliprgn_t *p_rgn, 
//  const point_t *p_pts, int vertices);

/*!
  Handle to the device context of the whole screen. 
  This DC is a special one. uses it to draw popup menus and
  other global objects. You can also use this DC to draw lines or text on
  the screen directly, and there is no need to get or release it.

  If you do not want to create any main window, but you want to draw on
  the screen, you can use this DC.
  */
#define HDC_SCREEN         (~0)

/*!
  Indicates an invalid handle to device context.
  */
#define HDC_INVALID        0

/*!
  Capabilitys of graphic device: color number.
  */
#define GDCAP_COLORNUM     0
/*!
  Capabilitys of graphic device: horizontal-pixel.
  */
#define GDCAP_HPIXEL       1
/*!
  Capabilitys of graphic device: vertical-pixel.
  */
#define GDCAP_VPIXEL       2
/*!
  Capabilitys of graphic device: maxx.
  */
#define GDCAP_MAXX         3
/*!
  Capabilitys of graphic device: maxy.
  */
#define GDCAP_MAXY         4
/*!
  Capabilitys of graphic device: depth.
  */
#define GDCAP_DEPTH        5
/*!
  Capabilitys of graphic device: bit per pixel.
  */
#define GDCAP_BPP          6
/*!
  Capabilitys of graphic device: bitspp.
  */
#define GDCAP_BITSPP       7
/*!
  Capabilitys of graphic device: pixel type.
  */
#define GDCAP_PIXELTYPE    8

/*!
  Raster operation : set.
  */
#define ROP_SET            0

/*!
  Raster operation : and
  */
#define ROP_AND            1

/*!
  Raster operation : or
  */
#define ROP_OR             2

/*!
  Raster operation : xor
  */
#define ROP_XOR            3

/*!
  Raster operation : nr
  */
#define NR_ROPS            4

/*!
  Gets a capability of a DC.

  \param[in] hdc : Handle to the device context.
  \param[in] type : Specifies the type of capability.
  \return : If the function succeeds,the return value is capability value.
  */
u32 gdi_get_capability(hdc_t hdc, s32 type);

/*!
  Information structrue for get DC.
  */
typedef struct
{
  /*!
    The rectangle of device.
    */
  rect_t       rc;
  /*!
    The clipping region.
    */
  crgn_info_t *p_crgn_info;
}cdc_info_t;

/*!
  osd config
  */
typedef struct
{
  /*!
    surface layer
    */
  surface_layer_t layer;    
  /*!
    Screen format.
    */
  u8 format;
  /*!
    Screen rect.
    */
  rect_t osd_rect;
  /*!
    Global palette.
    */
  palette_t *p_pal;
  /*!
    Screen default color.
    */
  u32 cdef;
  /*!
    Color key.
    */
  u32 ckey;
  /*!
    Osd odd memory address.
    */
  u32 *p_odd_addr;
  /*!
    Osd odd memory size.
    */
  u32 odd_size;
  /*!
    Osd even memeory address.
    */
  u32 *p_even_addr;
  /*!
    Osd even memory size.
    */
  u32 even_size;
}screen_cfg_t;

/*!
  gdi multi rgn config.
  */
typedef struct
{
  /*!
    screen rect, rgn rect is base on screen rect.
    */
  rect_t *p_screen_rect;
  /*!
    top osd config.
    */
  flinger_cfg_t *p_top;
  /*!
    bottom osd config.
    */
  flinger_cfg_t *p_bot;    
  /*!
    Screen format.
    */
  pix_type_t format;
  /*!
    Global palette.
    */
  palette_t *p_pal;
  /*!
    Screen default color.
    */
  u32 cdef;
  /*!
    Color key.
    */
  u32 ckey;  
  /*!
    Osd virtual buffer size.
    */
  u32 vsurf_buf_addr;
  /*!
    Osd virtual buffer size.
    */
  u32 vsurf_buf_size;
  /*!
    Maximum cliprect count.
    */
  u16 max_cliprect_cnt;
  /*!
    Maximum dc count.
    */
  u16 max_dc_cnt;
  /*!
    anim new buffer address.
    */
  u32 anim_addr;
  /*!
    anim new buffer size.
    */
  u32 anim_size;
}gdi_cfg_t;

/*!
  screen reset param(multi region mode.)
  */
typedef struct
{
  /*!
    screen rect, rgn rect is base on screen rect.
    */
  rect_t *p_screen_rect;
  /*!
    top osd config.
    */
  flinger_cfg_t *p_top;
  /*!
    bottom osd config.
    */
  flinger_cfg_t *p_bot;
  /*!
    Screen format.
    */
  pix_type_t format;  
  /*!
    Global palette.
    */
  palette_t *p_pal;
  /*!
    Screen default color.
    */
  u32 cdef;
  /*!
    Color key.
    */
  u32 ckey;
}screen_reset_t;

/*!
  GDI initialize.

  \param[in] info : parameter for initilize.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_init(gdi_cfg_t *p_info);

/*!
  GDI release.

  \return : NULL
  */
void gdi_release(void);

/*!
  reset screen multi.
  */
BOOL gdi_reset_screen(screen_reset_t *p_scr_reset);

/*!
  reset & scale.
  */
BOOL gdi_reset_screen_and_scale(screen_reset_t *p_scr_reset, void (*reset_cb)(void *));

/*!
  Gdi reset transparence as zero.

  \return : NULL
  */
void gdi_reset_trans(void);

/*!
  Get screen format

  \return : current screen format.
  */
pix_type_t gdi_get_screen_pixel_type(void);

/*!
  Get screen rectangle.

  \param[out] rc : screen rectangle.
  \return : NULL
  */
void gdi_get_screen_rect(rect_t *p_rc);

/*!
  Move screen according to a specified coordinates.

  \param[in] x : specified x-coordinate.
  \param[in] y : specified y-coordinate.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_offset_screen(s16 x, s16 y);

/*!
  Move left-top of the screen to a specified point.

  \param[in] x : x-coordinate of the specified point.
  \param[in] y : y-coordinate of the specified point.
  \return : TRUE for success, otherwise failed.
  */
BOOL gdi_move_screen(s16 x, s16 y);

/*!
  fill the whole screen with default color.(clear the screen)

  \return : NULL
  */
void gdi_clear_screen(void);

/*!
  gdi clear top screen.
  */
void gdi_clear_top_screen(void);

/*!
  Get the handle of the screen.

  \return : the handle.
  */
handle_t gdi_get_screen_handle(BOOL is_top_screen);

/*!
  Get the heap id of cliprects.

  \return : the index of the heap.
  */
lib_memf_t *gdi_get_cliprc_heap(void);

/*!
  Set the surface display On/Off.

  \param[in] is_enable : On/Off
  \return : NULL
  */
void gdi_set_enable(BOOL is_enable);

/*!
  Set screen transparence.

  \param[in] alpha : transparence to set.
  \return : NULL
  */
void gdi_set_global_alpha(u8 alpha);

/*!
  Gets a DC and can use the returned handle in subsequent GDI functions to draw.

  \param[in] is_top : is top.
  \param[in] p_src : Points to the rectangle of the screen.
  \param[in] p_vrc : Points to the rectangle of dc to get.
  \param[in] gcrgn_info : Points to the structure that contains clipping region and so on.
  \param[in] parent : parent dc.
  \return : the handle to the DC if success, otherwise returns HDC_INVALID.
  */
hdc_t gdi_get_dc(BOOL is_top, rect_t *p_src, rect_t *p_vrc,
  crgn_info_t *p_gcrgn_info, hdc_t parent);

/*!
  Releases a DC.
  
  \param[in] hdc : Handle to the device context.
  \return : NULL
  */
void gdi_release_dc(hdc_t hdc);

/*!
  locks a dc to get direct access to pixels in the DC.

  \param[in] hdc : handle of deveice context.
  \param[in] p_rw_rc : rectangle.
  \param[out] p_width : width.
  \param[out] p_height : height.
  \param[out] p_pitch : pitch.
  \return : 
  */
//u8 *gdi_lock_dc (hdc_t hdc, const rect_t *p_rw_rc, 
//  s16 *p_width, s16 *p_height, u16 *p_pitch);

/*!
  unlocks a locked DC.

  \param[in] hdc : handle of device context.
  \return : NULL.
  */
//void gdi_unlock_dc (hdc_t hdc);

/*!
  Match an RGBA value to a particular palette index.

  \param[in] p_pal : Points to the palette.
  \param[in] r : Specifies the red component of the entry.
  \param[in] g : Specifies the green component of the entry.
  \param[in] b : Specifies the blue component of the entry.
  \param[in] a : Specifies the alpha component of the entry.
  \return : If the function succeeds, the return value is a matched index of palette.
  */
u8 gdi_find_color(palette_t *p_pal, u8 r, u8 g, u8 b, u8 a);

/*!.
  Gets palette entries of a DC.

  \param[in] hdc : Handle to the device context.
  \param[in] pix_val : Specifies the pixel value.
  \param[out] p_r : Points to the red component of the entry.
  \param[out] p_g : Points to the green component of the entry.
  \param[out] p_b : Points to the blue component of the entry.
  \param[out] p_a : Points to the alpha component of the entry.
  \return : NULL
  */
void gdi_get_rgba(hdc_t hdc, u32 pix_val, u8 *p_r, u8 *p_g, u8 *p_b, u8 *p_a);

/*!
  Find the opaque pixel value corresponding to an RGBA triple

  \param[in] hdc : handle to the device context.
  \param[in] r : Specifies the red component of the entry.
  \param[in] g : Specifies the green component of the entry.
  \param[in] b : Specifies the blue component of the entry.
  \param[in] a : Specifies the alpha component of the entry.
  \return : the pixel value.
  */
u32 gdi_map_rgba(hdc_t hdc, u8 r, u8 g, u8 b, u8 a);

/*!
  Gets palette entries of a DC.

  \param[in] is_top_screen : is top screen.
  \param[in] start : Specifies the first entry in the palette to be retrieved.
  \param[in] len : Specifies the number of entries in the palette to be retrieved.
  \param[out] p_colors : Points to an array of color structures to receive the palette entries.
  \returns : TRUE if the function is successful. Otherwise returns FALSE.
  */
BOOL gdi_get_palette(BOOL is_top_screen, u16 start, u16 len, color_t *p_colors);

/*!
  get palette address.
  */
palette_t *gdi_get_palette_addr(BOOL is_top_screen);

/*!
  Sets palette entries of a DC.

  \param[in] is_top_screen : is top screen.
  \param[in] start : Specifies the first palette entry to be set.
  \param[in] len : Specifies the number of palette entries to be set.
  \param[in] p_colors : Points to an array of color structures to set the palette entries.
  \returns : TRUE if the function is successful. Otherwise returns FALSE.
  */
BOOL gdi_set_palette(BOOL is_top_screen, u16 start, u16 len, color_t *p_colors);

/*!
  Gets the color key value.

  \param[in] is_top_screen : is top screen.
  \param[out] p_ckey : for get colorkey.

  \return : True for success, otherwise false.
  */
BOOL gdi_get_colorkey(BOOL is_top_screen, u32 *p_ckey);

/*!
  Sets the color key value.

  \param[in] is_top_screen : is top screen.
  \param[in] ckey : colorkey for set.
  \return : True for success, otherwise false.
  */
BOOL gdi_set_colorkey(BOOL is_top_screen, u32 ckey);

/*!
  Fills a rectangle.

  \param[in] hdc : Handle to a device context.
  \param[in] p_rc :   rectangle for fill.
  \param[in] color : color for fill.
  \return : NULL
  */
void gdi_fill_rect(hdc_t hdc, rect_t *p_rc, u32 color);

/*!
  Fills a round rectangle.

  \param[in] hdc : Handle to a device context.
  \param[in] p_rc :   rectangle for fill.
  \param[in] color : color for fill.
  \return : NULL
  */
void gdi_fill_round_rect(hdc_t hdc, rect_t *p_rc, u32 color);


/*!
  Excludes the specified rectangle from the current visible region of a DC.

  \param[in] hdc : Handle to the device context.
  \param[in] p_rc : Points to the rectangle.
  \return : NULL
  */
void gdi_exclude_cliprect(hdc_t hdc, const rect_t *p_rc);

/*!
  Includes the specified rectangle to the current visible region of a DC.

  \param[in] hdc : Handle to the device context.
  \param[in] p_rc : Points to rectangle to be included to the region.
  \return : NULL
  */
void gdi_include_cliprect(hdc_t hdc, const rect_t *p_rc);

/*!
  Sets the visible region of a DC to be a specified rectangle.

  \param[in] hdc : [in] Handle to the device context.
  \param[in] p_rc : [in] Points to the rectangle.
  \return : NULL
  */
void gdi_select_cliprect(hdc_t hdc, const rect_t *p_rc);

/*!
  Sets the visible region of a DC to be a specified region.

  \param[in] hdc : [in] Handle to the device context.
  \param[in] p_rgn : [in] Points to the region.
  \return : NULL
  */
void gdi_select_cliprgn(hdc_t hdc, const cliprgn_t *p_rgn);

/*!
  Retrieves the bounding rectangle of the current visible region of a DC.

  \param[in] hdc: Handle to the device context.
  \param[in] p_rc: Points to the rectangle that receives the enclosing bound.
  \return : NULL
  */
void gdi_get_boundrect(hdc_t hdc, rect_t *p_rc);


/*!
  Determines whether a point is visible.

  \param[in] hdc : Handle to the device context.
  \param[in] x : Specifies the x\-coordinate of the point.
  \param[in] y : Specifies the y\-coordinate of the point.
  \returns : The return value is TRUE if the point is visible. Otherwise returns FALSE.
  */
BOOL gdi_is_visible_pt(hdc_t hdc, s16 x, s16 y);

/*!
  Determines whether the specified rectangle is visible.

  \param[in] hdc : Handle to the device context.
  \param[in] p_rc : Points to the rectangle.
  \returns : The return value is TRUE if the rectangle is visible. Otherwise returns FALSE.
  */
BOOL gdi_is_visible_rect(hdc_t hdc, const rect_t *p_rc);

/*!
  Create a compatible bitmap object with the specified reference DC.

  \param[in] hdc : Handle to a device context.
  \param[in] p_bmp : Points to a bitmap structure.
  \returns : If the function succeeds, the return value is a handle to the compatible bitmap (DDB).Otherwise returns NULL.
  */
hbitmap_t gdi_create_compatible_bitmap(hdc_t hdc, bitmap_t *p_bmp);

/*!
  Delete a compatible bitmap object.

  \param[in] bmp : Handle to the bitmap object.
  \return : NULL
  */
void gdi_delete_compatible_bitmap(hbitmap_t bmp);


/*!
  Fill the rectangle with bitmap by tile mode.
  */
#define FILL_FLAG_TILE         0x10000000

/*!
  Fill the rectangle with bitmap by stretch mode.
  */
#define FILL_FLAG_STRETCH      0x20000000

/*!
  Fill the rectangle with bitmap by stretch mode.
  */
#define FILL_FLAG_CENTER      0x40000000

/*!
  Filling by horizontal direction.
  */
#define FILL_DIRECTION_HORI    0x00001000

/*!
  Filling by vertical direction.
  */
#define FILL_DIRECTION_VERT    0x00002000

/*!
  Fills a box with a BITMAP object.

  \param[in] hdc : Handle to a device context.
  \param[in] p_rc : Pointer to a RECT structure that contains the coordinates of the rectangle to be filled.
  \param[in] bmp : Handle to a bitmap object.
  \param[in] flags : Specifies the flag of method.
  \return : TRUE if the function is successful. Otherwise returns FALSE.
  */
BOOL gdi_fill_bitmap(hdc_t hdc, rect_t *p_rc, const hbitmap_t bmp, u32 flags);

/*!
  Gets a box with a rectangle.

  \param[in] hdc : Handle to a device context.
  \param[in] p_rc : Pointer to a RECT structure that contains the coordinates of the rectangle to be filled.
  \param[in] p_buf : Points to a buffer.
  \param[in] pitch : Specifies the pitch.
  \return : TRUE if the function is successful. Otherwise returns FALSE.
  */
BOOL gdi_get_bits(hdc_t hdc, rect_t *p_rc, void *p_buf, u32 pitch);


/*!
  Performs a bit-block transfer from a device context into another device context.

  \param[in] hsdc : Handle to the source device context.
  \param[in] sx : Specifies the x-coordinate of the upper-left corner of the source rectangle.
  \param[in] sy : Specifies the y-coordinate of the upper-left corner of the source rectangle.
  \param[in] sw : Specifies the width of the source rectangle.
  \param[in] sh : Specifies the height of the source rectangle.
  \param[in] hddc : Handle to the destination device context.
  \param[in] dx : Specifies the x-coordinate of the upper-left corner of the destination rectangle.
  \param[in] dy : Specifies the x-coordinate of the upper-left corner of the destination rectangle.
  \param[in] rop : Specifies a raster-operation code.
  \return : NULL
  */
void gdi_bitblt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc, u16 dx,
  u16 dy, u32 rop, u32 rop_pat);

/*!
  Copies a bit-block from a a device context into another device context, 
  stretching or compressing it if necessary.
  */
void gdi_stretch_blt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc,
  u16 dx, u16 dy, u16 dw, u16 dh);

/*!
  trape blt.
  */
void gdi_trape_blt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc,
  u16 dx, u16 dy, u16 dw, u16 dh, trape_t *p_trape);

/*!
  paint blt.
  */
void gdi_paint_blt(hdc_t hddc, u16 dx, u16 dy, u16 dw, u16 dh, cct_gpe_paint2opt_t *p_popt);

/*!
  rect stroke.
  */
void gdi_rect_stroke(hdc_t hddc, u16 dx, u16 dy, u16 dw, u16 dh, u16 depth, u32 color);


/*!
  rotate mirror blt.
  */
void gdi_rotate_mirror(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc,
  u16 dx, u16 dy, lld_gfx_rotator_t rotate_mod, lld_gfx_mirror_t mirror_mod);

/*!
  3src blt.
  */
void gdi_blt_3src(
  hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh,
  hdc_t hddc, u16 dx, u16 dy, u16 dw, u16 dh,
  hdc_t hrdc, u16 rx, u16 ry, u16 rw, u16 rh, cct_gpe_blt3opt_t *p_opt3);

/*!
  draw pie, clockwise.
  */
void gdi_draw_pie(hdc_t hdc, pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u32 color);

/*!
  draw arch, clockwise.
  */
void gdi_draw_arc(hdc_t hdc,
  pos_t cent, u16 radius, u16 from_degree, u16 to_degree, u16 thick, u32 color);

/*!
  rotate mirror blt.
  */
void gdi_rotate_mirror_blt(hdc_t hsdc, u16 sx, u16 sy, u16 sw, u16 sh, hdc_t hddc,
  u16 dx, u16 dy, lld_gfx_rotator_t rotate_mod, lld_gfx_mirror_t mirror_mod);
  
/*!
  Initial virtual surface.

  \param[in] size : virtual surface buffer size..

  \return : SUCCESS or ERR_FAILURE
  */
RET_CODE gdi_init_vsurf(u32 addr, u32 size);

/*!
  Release virtual surface.

  \return : NULL
  */
void gdi_release_vsurf(void);

/*!
  Create virtual surface.

  \param[in] hdc : handle for device context.
  \param[in] width : width of virtual surface.
  \param[in] height : height of virtual surface.
  \return : SUCCESS or ERR_FAILURE
  */
RET_CODE gdi_create_vsurf(hdc_t hdc, s16 width, s16 height);

/*!
  Delete virtual surface.

  \param[in] hdc : handle for device context.
  \return : SUCCESS or ERR_FAILURE
  */
RET_CODE gdi_delete_vsurf(hdc_t hdc);

/*!
  Get bit per pixel.

  \param[in] format : color format.
  \return : bpp.
  */
u8 gdi_get_bpp(pix_type_t format);

/*!
  Check inherit dc.
  
  \param[in] hdc : handle to device context.
  \return : True for yes, otherwise no.
  */
//BOOL gdi_check_inherit_dc(hdc_t hdc);

/*!
  Gdi get invert color
  \param[in] hdc : handle of dc
  \param[in] color : color for invert
  \return : invert color.
  */
u32 gdi_get_invert_color(hdc_t hdc, u32 color);

/*!
  Gdi start batch.

  \return NULL
  */
void gdi_start_batch(BOOL is_top_screen);

/*!
  Gdi end batch.

  \return NULL
  */
void gdi_end_batch(BOOL is_top_screen, BOOL is_sync, rect_t *p_rect);

/*!
  Gdi get roll dc

  \param[in] p_rc     rect
  \param[in] p_rsurf  roll surface

  \return dc
  */
hdc_t gdi_get_roll_dc(rect_t *p_rc, void *p_rsurf);

/*!
  Gdi release dc

  \param[in] hdc  dc

  \return NULL
  */
void gdi_release_roll_dc(hdc_t hdc);

/*!
  Gdi create roll surface

  \param[in] widht        width
  \param[in] height       height
  \paramp[out] p_addr   address

  \return surface handle.
  */
handle_t gdi_create_rsurf(BOOL is_top_screen, u16 width, u16 height, u32 *p_addr);

/*!
  Gdi delete roll surface

  \param[in] handle   surface handle
  \param[in] p_addr  context get from create rsurface

  \return NULL
  */
void gdi_delete_rsurf(handle_t handle, u32 addr);

/*!
  Gdi get roll dc

  \param[in] p_rc     rect
  \param[in] p_rsurf  roll surface

  \return dc
  */
hdc_t gdi_get_mem_dc(rect_t *p_rc, void *p_surf);

/*!
  Gdi release dc

  \param[in] hdc  dc

  \return NULL
  */
void gdi_release_mem_dc(hdc_t hdc);

/*!
  Gdi create roll surface

  \param[in] widht        width
  \param[in] height       height
  \paramp[out] p_addr   address

  \return surface handle.
  */
handle_t gdi_create_mem_surf(BOOL is_top_screen, u16 width, u16 height, u32 *p_addr);

/*!
  Gdi delete roll surface

  \param[in] handle   surface handle
  \param[in] p_addr  context get from create rsurface

  \return NULL
  */
void gdi_delete_mem_surf(handle_t handle, u32 addr);

/*!
  Releases a anim DC.

  \param[in] hdc : Handle to the device context.

  \return : NULL
  */
void gdi_release_anim_dc(hdc_t hdc);
#endif

