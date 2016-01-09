/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __REGION_H__
#define __REGION_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  Palette update.
  */
#define REGION_PALETTE_UPDATE   0x01
/*!
  Alpha update.
  */
#define REGION_ALPHA_UPDATE     0x02
/*!
  On off update.
  */
#define REGION_ONOFF_UPDATE     0x04
/*!
  Alpha on off update.
  */
#define REGION_ALPHA_ONOFF_UPDATE       0x08
/*!
  Palette entry update.
  */
#define REGION_PALETTE_ENTRY_UPDATE     0x10
/*!
  Pre multiply.
  */
#define REGION_PREMULTIPLY_UPDATE     0x20

/*!
  memory storing formats
  */
typedef enum
{
  /*!
    y and cbcr stored in seperate.
    */
  Y_CBCR = 0,
  /*!
    y,cb and cr stored in seperate.
    */
  Y_CB_CR,
 /*!
    cbcr and y stored in seperate.
    */
  CBCR_Y,
 /*!
    cb,cr and y stored in seperate.
    */
  CB_CR_Y,
 /*!
    y, cbcr stored in pic9 mode still layer.
    */
  Y_CBCR_PIC9,
 /*!
    y, cb, cr stored in pic9 mode video layer.
    */
  Y_CB_CR_PIC9,
 /*!
    max.
    */
  YCBCR_MAX
}mem_fmt_t;

/*!
  memory buffer source
  */
typedef enum
{
  /*!
    buffer from configed area.
    */
  BUFFER_SRC_CONFIG = 0,
  /*!
    buffer from malloced by upper user.
    */
  BUFFER_SRC_USER,
  /*!
    buffer from malloced by inner.
    */
  BUFFER_SRC_INNER,
 /*!
    max.
    */
  BUFFER_SRC_MAX
}buf_src_t;

/*!
  This structure defines region structure.
  */
typedef struct
{
  /*!
    size:x,y is based on the plane
    */
  rect_vsb_t          rect;
  /*!
    pitch in bytes
    */
  u32                 pitch;
  /*!
    pixel format
    */
  pix_fmt_t           pix_fmt;
  /*!
    pixel format
    */
  colorspace_t        color_sp;
  /*!
    pixel format
    */
  u8                  bpp;
  /*!
    palette exist flag
    */
  BOOL                b_palette;
  /*!
    palette pointer
    */
  u32                 *p_palette;
  /*!
    the attached plane 
    */
  u8                  layerId;
  /*!
    alpha value 
    */
  u8                  alpha;
  /*!
    alpha enable 
    */
  BOOL                b_alpha;
  /*!
    display flag 
    */
  BOOL                b_show;
  /*!
    odd buffer pointer
    */
  void                *p_buf_odd;
  /*!
    even buffer pointer,, NULL for frame buffer use  
    */
  void                *p_buf_even;
  /*!
    graphic vertical scaler buffer for gaphic engine
    */
  void                *p_buf_vs;
  /*!
    The semaphore for the region operation
    */
  os_sem_t            sem;
  /*!
    Update flag
    */
  u8                  flag;
  /*!
    memory storing format
    */
  mem_fmt_t           mem_fmt;
  /*!
    palette entry changed
    */
  u32                 entry_changed;
  /*!
    buffer src
    */
  buf_src_t           buf_src;
    /*!
    buffer src for vs buf
    */
  buf_src_t           buf_vs_src;
  /*!
    context(only used for zoran project.)
    */
  void *p_context;    
  /*!
    for linux use
    */
  void *p_rgn_buf;
  /*!
    premultiply alpha 
    */
  u8                  premultiply;

  /*!
    little endian
    */
  BOOL          little_endian;
} region_t;


/*!
  This structure defines region structure.
  */
typedef struct
{
  /*!
    pitch in bytes
    */
  u32                 pitch;

  /*!
   width
    */
  u32                 width;

  /*!
   width
    */
  u32                 height;
  /*!
    pixel format
    */
  pix_fmt_t           pix_fmt;
  
  /*!
    palette exist flag
    */
  BOOL                b_palette;
  /*!
    palette pointer
    */
  u32                 *p_palette;

 /*!
    palette buffer size
    */
  u32                pal_buf_sz;

 /*!
    entry count
    */
  u32                 entry_cnt;
 
  /*!
    alpha value 
    */
  u8                  alpha;
  /*!
    alpha enable 
    */
  BOOL                b_alpha;
  
  /*!
    buffer pointer
    */
  void                *p_buf_addr;
   /*!
    data buffer size
    */
  u32                 buf_sz;

  /*!
    little endian
    */
  BOOL          little_endian;



} image_t;


/*!
  Create a region
  
  \param[in] p_rect_size     the pointer to the rectangle.
  \param[in] pix_fmt          pixel format
  return region handle
  */
void * region_create(rect_size_t *p_rect_size, pix_fmt_t pix_fmt);

/*!
  delete a region, should remove region on attached layer first.
  
  \param[in] p_region        handle of region
  */
RET_CODE region_delete(void *p_region);

/*!
  set palette
  
  \param[in] p_region        handle of region
  \param[in] p_palette       point to the palette
  \param[in] entries           the number of palette entry
  */
RET_CODE region_set_palette(void *p_region, u32 *p_palette, u32 entries);

/*!
  set palette entry
  
  \param[in] p_region        handle of region
  \param[in] index             index of entry in palette
  \param[in] color              color for entry
  */
RET_CODE region_set_palette_entry(void *p_region, u32 index, u32 color);

/*!
  Get palette entry
  
  \param[in] p_region        handle of region
  \param[in] index             index of entry in palette
  \param[out] p_color         the pointer to color of entry
  */
RET_CODE region_get_palette_entry(void *p_region, u32 index, u32 *p_color);

/*!
  show a region
  
  \param[in] p_region        handle of region
  \param[in] b_show          display on/off flag
  */
RET_CODE region_show(void *p_region, BOOL b_show);

/*!
  region display state
  
  \param[in] p_region        handle of region
  */
BOOL region_is_show(void *p_region);

/*!
  enable the alpha of region
  
  \param[in] p_region        handle of region
  \param[in] b_on             region alpha enable
  */
RET_CODE region_alpha_onoff(void *p_region, BOOL b_on);

/*!
  set the alpha of region
  
  \param[in] p_region        handle of region
  \param[in] alpha             region alpha value
  */
RET_CODE region_set_alpha(void *p_region, u8 alpha);

/*!
  get the alpha of region
  
  \param[in] p_region      handle of region
  \param[out] p_alpha      region alpha value pointer
  */
RET_CODE region_get_alpha(void *p_region, u8 *p_alpha);

/*!
  get the rectsize of region
  
  \param[in] p_region      handle of region
  \param[out] p_rectsize  region rectsize value pointer
  */
RET_CODE region_get_rectsize(void *p_region, rect_size_t *p_rectsize);

/*!
  set the region premultiply 
  
  \param[in] p_region        handle of region
  \param[in] premultiply         region premultiply
  */
RET_CODE region_set_premultiply(void *p_region, u8 premultiply);

/*!
  set little endian attribute of region
  
  \param[in] p_region      handle of region
  \param[in] 
  */
RET_CODE region_set_little_endian(void *p_region, BOOL is_little_endian);

#ifdef __cplusplus
}
#endif

#endif

