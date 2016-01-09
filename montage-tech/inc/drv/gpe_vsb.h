/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GPE_VSB_H__
#define __GPE_VSB_H__

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
*full version of rop3 ops in http://www.vckbase.com/document/viewdoc/?id=509
****************************************************************************/
        
/*!
  dest = source
  */
#define SRCCOPY             ((u8)0xCC)
/*!
  dest = source OR dest
  */
#define SRCPAINT            ((u8)0xEE)
/*!
  dest = source AND dest
  */
#define SRCAND              ((u8)0x88)
/*!
  dest = source XOR dest
  */
#define SRCINVERT           ((u8)0x66)
/*!
  dest = source AND (NOT dest)
  */
#define SRCERASE            ((u8)0x44)
/*!
  dest = (NOT source)
  */
#define NOTSRCCOPY          ((u8)0x33)
/*!
  dest = (NOT src) AND (NOT dest)
  */
#define NOTSRCERASE         ((u8)0x11)
/*!
  dest = (source AND pattern)
  */
#define MERGECOPY           ((u8)0xC0)
/*!
  dest = (NOT source) OR dest
  */
#define MERGEPAINT          ((u8)0xBB)
/*!
  dest = pattern
  */
#define PATCOPY             ((u8)0xF0)
/*!
  dest = DPSnoo    (NOT Src) OR Patten OR Dest
  */
#define PATPAINT            ((u8)0xFB)
/*!
  dest = pattern XOR dest
  */
#define PATINVERT           ((u8)0x5A)
/*!
  dest = (NOT dest)
  */
#define DSTINVERT           ((u8)0x55)
/*!
  dest = (dest)
  */
#define DSTCOPY           ((u8)0xAA)
/*!
  dest = BLACK
  */
#define BLACKNESS           ((u8)0x00)
/*!
  dest = WHITE
  */
#define WHITENESS           ((u8)0xFF)

/*!
  The command of alpha map
  */
#define GPE_CMD_ALPHA_MAP    0x00000010
/*!
  The command of raster operation
  rop3 operation, tatol support 15 kinds
  rop and alpha blending are not be uesed at the same time
  */
#define GPE_CMD_RASTER_OP      0x00000020
/*!
  The command of plane alpha blending
  this is normal alpha blending
  only valid when dst is rgb true color foramt
  */
#define GPE_CMD_ALPHA_BLEND    0x00000040
/*!
  The command of color key
  discourage to use in gpe vsb
  in the gpe_param_t can set the src ck and dst ck
  */
#define GPE_CMD_COLORKEY       0x00000080
/*!
  The command of clipping rectangle
  discourage to use in gpe vsb
  */
#define GPE_CMD_CLIP_RECT      0x00000100
/*!
  The command of color expand
  discourage to use in gpe vsb
  */
#define GPE_CMD_COLOR_EXPAND   0x00000200

/*!
  The command of scale
  valid after warriors chip
  */
#define GPE_CMD_SCALE          0x00000400


/*!
  The command of mix with region,region be the bottom layer
  valid after warriors chip
  */
#define GPE_CMD_MIXTOREGION          0x00000800
/*!
  The command of mix with region,region be the top layer
  valid after warriors chip
  */
#define GPE_CMD_MIXTOREGION_SWICH    0x00001000


/*!
  The command of mix with region
  valid after warriors chip
  */
typedef enum
{
  /*!
    comment
    */
  RGB_COLOR_SPACE,
  /*!
    comment
    */
  YUV_COLOR_SPACE,
  /*!
    comment
    */
  GRAY_COLOR_SPACE,

}color_space_t;


/*!
  The structure is defined to descript the paramter of GPE.
  */
typedef struct
{
  /*!
    The command
    */
  u32 cmd;
  /*!
    The raster operation type
    */
  u32 rop3;
  /*!
    The color expand palette pointer
    */
  void *p_palette;

  /*!
    The alpha map buffer pointer
    */
  void *p_alpha;

  /*!
    The alpha map buffer picth
    */
  u32  alpha_pitch;

  /*!
    xylc mode: 0:xy, 1:xyc, 2:xyl, 3:xylc
    */
  u32  xylc_mode;
  /*!
    xylc instruct number
    */
  u32 xylc_cmd_num;

  /*!
    The plane alpha value
    */
  BOOL  plane_alpha_en;
  /*!
    The plane alpha value
    */
  u32 plane_alpha;
  /*!
    Enable the src color key
    */
  u32 enable_colorkey;
  /*!
    The src color key
    */
  u32 colorkey;
  /*!
    Enable the dst color key
    */
  u32 enable_dst_ck;
  /*!
    The color key
    */
  u32 dst_colorkey;
  /*!
    is font
    */
  BOOL is_font;
  /*!
    pattern data for rop3 operation
    */
  u32 pattern;
  /*!
   src little_endian
   */
  BOOL src_little_endian;
  /*!
  dst little_endian
  */
  BOOL dst_little_endian;

  /*!
   src alpha channel disabled
   */
  BOOL src_alpha_ch_disenble;
  /*!
  dst alpha channel disabled
  */
  BOOL dst_alpha_ch_disenble;

} gpe_param_vsb_t;


/*!
  The structure is defined to descript the output paramter of dump image
  */
typedef struct
{
  /*!
    indicate the w of image
    */
  u32   w;
  /*!
    indicate the w of image
    */
  u32   h;
  /*!
    indicate the pitch of image
    */
  u32   pitch;
  /*!
    has palette or not
    */
  BOOL  b_pal;
  /*!
    if has palette, the palette entry cunt
    */
  u32   pal_cunt;
  /*!
    indicate the buf start byte is the valid data
    */
  u32   start_byte;
  /*!
    indicate used the image buf size
    */
  u32   buf_size;
  /*!
    indicate used the palette buf size
    */
  u32   pal_size;
 /*!
    dump alpha data to buf or not
    */
  BOOL  dump_alpha;
  /*!
    dump alpha buf dst address
    */
  u32   alpha_buf_addr;
  /*!
     dump alpha buf dst pitch
    */
  u32   alpha_pitch;

  /*!
    dump alpha pre mul en
    */
  BOOL alpha_pre_mul;
}gpe_dump_param_t;



/*!
  indicate the src1 lay take apart in the mix operation
  */
#define MIX_SRC1_FLAG        0x00000001

/*!
  indicate the src2 lay take apart in the mix operation
  */
#define MIX_SRC2_FLAG        0x00000002

/*!
  indicate the src3 lay take apart in the mix operation
  */
#define MIX_SRC3_FLAG        0x00000004

/*!
  indicate the alpha map lay take apart in the mix operation
  */
#define MIX_ALPHA_MAP_FLAG   0x00000008


/*!
  The alpha map channel
  */
typedef enum
{
  /*!
     no alpha map channel
    */
  NO_ALPHA_MAP_CHANNEL,

  /*!
    The alpha map channel 1
    */
  ALPHA_MAP_CHANNEL_1,

  /*!
    The alpha map channel 2
    */
  ALPHA_MAP_CHANNEL_2,

  /*!
    The alpha map channel 3
    */
  ALPHA_MAP_CHANNEL_3,

}alpha_map_ch_t;



/*!
  The structure is defined to descript the paramter of GPE scale
  */
typedef struct
{
  /*!
    When scaled, continue do the mix with dst
    */
  BOOL enable_mix;
  /*!
    The scale filter table id
    */
  u32  filter_table;
  /*!
    The scale use the matrix or not
    */
  BOOL enable_matrix;

  /*!
    When mix, the src mix start x
    */
  u32  src_mix_x;
  /*!
    When mix, the src mix start y
    */
  u32  src_mix_y;
  /*!
    When mix, the dst mix start x
    */
  u32  dst_mix_x;
  /*!
    When mix, the dst mix start y
    */
  u32  dst_mix_y;
  /*!
    When mix, the alpha map mix start x
    */
  u32 map_mix_x;
  /*!
    When mix, the alpha map mix start y
    */
  u32 map_mix_y;
  /*!
    When mix, enable the plane alpha of src layer
    */
  BOOL plane_alpha_en;
  /*!
    When mix, the src plane alpha value
    */
  u32  plane_alpha;
  /*!
    When mix, enable the color key of src layer
    */
  BOOL src_ck_en;
  /*!
    When mix,  the color key of src layer
    */
  u32  src_ck;
  /*!
    When mix, enable the color key of dst layer
    */
  BOOL dst_ck_en;
  /*!
    When mix,  the color key of dst layer
    */
  u32  dst_ck;
  /*!
    When mix,  the bg color value
    */
  u32  mix_bg_color;

  /*!
    The alpha_map_buf
    */
  u32 alpha_map_buf;

  /*!
    The alpha_map_buf pitch
    */
  u32 map_pitch;

  /*!
    The alpha_map w
    */
  u32 map_w;

  /*!
    The alpha_map h
    */
  u32 map_h;


}gpe_scale_param_t;


/*!
  The structure is defined to descript the paramter of GPE mixer
  */
typedef struct
{
  /*!
    The mix flag, MIX_SRC1_FLAG, MIX_SRC2_FLAG, MIX_SRC3_FLAG
    */
  u32 mix_flag;
  /*!
    The layer switch
    */
  u32 layer_switch;
  /*!
    The alpha_map_ch
    */
  u32 alpha_map_ch;

  /*!
    The alpha_map_buf
    */
  u32 alpha_map_buf;

  /*!
    The alpha_map_buf pitch
    */
  u32 map_pitch;

  /*!
    The alpha_map w
    */
  u32 map_w;

  /*!
    The alpha_map h
    */
  u32 map_h;

  /*!
    The bg_color
    */
  u32 bg_color;

  /*!
    The src1_mix_x pos
    */
  u32 src1_mix_x;

  /*!
    The src1_mix_y pos
    */
  u32 src1_mix_y;

  /*!
    The src2_mix_x pos
    */
  u32 src2_mix_x;

  /*!
    The src2_mix_y pos
    */
  u32 src2_mix_y;

  /*!
    The src3_mix_x pos
    */
  u32 src3_mix_x;

  /*!
    The src3_mix_y pos
    */
  u32 src3_mix_y;

  /*!
    The src1_alpha_en
    */
  BOOL src1_alpha_en;

  /*!
    The src1_plane_alpha
    */
  u8 src1_plane_alpha;

  /*!
    The src2_alpha_en
    */
  BOOL src2_alpha_en;

  /*!
    The src2_plane_alpha
    */
  u8 src2_plane_alpha;

  /*!
    The src3_alpha_en
    */
  BOOL src3_alpha_en;

  /*!
    The src3_plane_alpha
    */
  u8 src3_plane_alpha;

   /*!
    The plane_alpha_en
    */
  BOOL plane_alpha_en;

  /*!
    The plane_plane_alpha
    */
  u8 plane_alpha;

  /*!
    The src1_ck_en
    */
  BOOL src1_ck_en;

  /*!
    The src1_colorkey
    */
  u32 src1_colorkey;

  /*!
    The src2_ck_en
    */
  BOOL src2_ck_en;

  /*!
    The src2_colorkey
    */
  u32 src2_colorkey;

  /*!
    The src3_ck_en
    */
  BOOL src3_ck_en;

  /*!
    The src3_colorkey
    */
  u32 src3_colorkey;

}gpe_mix_param_t;


/*!
  The structure is defined for gpe config info
  */
typedef struct
{
  /*!
    The gpe align buffer is user config or auto config by gra engine
    */
  BOOL user_cfg;
  /*!
    The gpe align buffer address
    */
  u32 align_buf;

  /*!
    The gpe align buffer size
    */
  u32 buf_size;
}gpe_cfg_t;


/*!
  start batch.

  \param[in] pdev the gpe device.
  \param[in] p_region Points to the dest region
  */
RET_CODE gpe_start_batch_vsb(void *p_dev, void *p_region);


/*!
  end batch.

  \param[in] pdev the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] is_sync sync paint or not
  */
RET_CODE gpe_end_batch_vsb(void *p_dev, void *p_region, BOOL is_sync);

/*!
  Draws a pixel with the specifies color.

  \param[in] pdev the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to  the x-coordinate and the y-coordinate.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_pixel_vsb(void *p_dev, void *p_region, point_t *p_pos, u32 c);

/*!
  Draws a horizontal line with the specifies color.

  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to the x-coordinate and the y-coordinate.
  \param[in] w Specifies the width.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_h_line_vsb(void *p_dev, void *p_region, point_t *p_pos, u32 w, u32 c);

/*!
  Draws a vertical line with the specifies color.

  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_pos Points to the x-coordinate and the y-coordinate.
  \param[in] h Specifies the height.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_v_line_vsb(void *p_dev, void *p_region, point_t *p_pos, u32 h, u32 c);

/*!
  Draws a rectangle with the specifies color.

  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_rect Points to the rectangle position and size.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_rectangle_vsb(void *p_dev, void *p_region, rect_t *p_rect, u32 c);

/*!
  Draws a image on a region.

  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest region
  \param[in] p_rect pointer to the image position and size in dest region
  \param[in] p_buf Specifies the image buf pointer.Only frame.
  \param[in] p_palette Specifies the image palette pointer if null use region palette.
  \param[in] entries_num The number of palette entries.
  \param[in] pitch The input buffer pitch.
  \param[in] in_size The input buffer size in bytes.
  \param[in] fmt Specifies the image pixel format.
  \param[in] p_param Points to the parameter package, colorkey and alpha is valid in this function.
  \param[in] src rect of image region
  */
RET_CODE gpe_draw_image_vsb(void *p_dev,
                        void *p_region,
                        rect_t *p_rect,
                        void *p_buf,
                        void *p_palette,
                        u32 entries_num,
                        u32 pitch,
                        u32 in_size,
                        pix_fmt_t fmt,
                        gpe_param_vsb_t *p_param,
                        rect_t *p_fill_rect);
/*!
  Dump a region image to a buffer.

  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the src region
  \param[in] p_rect pointer to  the image position and size.
  \param[in] p_buf Specifies the dst buf pointer. Only frame
  \param[in] p_palette Specifies the dst palette pointer.
  \param[out] p_entries_num Points to the number of palette entries.
  \param[in] fmt Specifies the dst buffer pixel format.
  */
RET_CODE gpe_dump_image_vsb(void *p_dev,
                        void *p_region,
                        rect_t *p_rect,
                        void *p_buf,
                        void *p_palette,
                        u32 *p_entries_num,
                        pix_fmt_t fmt);


/*!
  Dump a region image to a buffer.

  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the src region
  \param[in] p_rect pointer to the dump rect in the region.
  \param[in] p_buf Specifies the dst buf pointer. Only frame
  \param[in] buf_size Specifies the dst buf size.
  \param[in] p_palette Specifies the dst palette pointer.
  \param[in] pal_buf_size Specifies the palette buf size.
  \param[in] fmt Specifies the dst buffer pixel format.
  \param[out] p_dump Points to the number of palette entries.
  */
RET_CODE gpe_dump_image_vsb_v2(void *p_dev,
                        void *p_region,
                        rect_t *p_rect,
                        void *p_buf,
                        u32 buf_size,
                        void *p_pal_buf,
                        u32 pal_buf_size,
                        pix_fmt_t fmt,
                        gpe_dump_param_t *p_dump);

/*!
  Performs a bit-block transfer of the color data corresponding to a rectangle
  of pixels from the specified block into a destination region

  \param[in] p_dev Points to the gpe device.
  \param[in] p_dst Points to the destination region
  \param[in] p_dst_rc Points to the destination rectangle.
  \param[in] p_ref Points to the reference region
  \param[in] p_ref_rc Points to the reference rectangle.
  \param[in] p_src Points to the source region
  \param[in] p_src_rc Points to the source rectangle.
  \param[in] p_param Points to the parameter package.
  */
RET_CODE gpe_bitblt_vsb(void *p_dev,
                void *p_dst,
                rect_t *p_dst_rc,
                void *p_ref,
                rect_t *p_ref_rc,
                void *p_src,
                rect_t *p_src_rc,
                gpe_param_vsb_t *p_param);



/*!
  Performs 3 regions mix,withc color key and alpah

  \param[in] p_dev Points to the gpe device.
  \param[in] p_dst Points to the destination region
  \param[in] p_dst_rc Points to the destination rectangle.
  \param[in] p_src1 Points to the reference region
  \param[in] p_src1_rc Points to the reference rectangle.
  \param[in] p_src2 Points to the source region
  \param[in] p_src2_rc Points to the source rectangle.
  \param[in] p_src3 Points to the source region
  \param[in] p_src3_rc Points to the source rectangle.
  \param[in] p_param Points to the parameter package.
  */
RET_CODE gpe_mix_vsb(void *p_dev,
                void *p_dst,
                rect_t *p_dst_rc,
                void *p_src1,
                rect_t *p_src1_rc,
                void *p_src2,
                rect_t *p_src2_rc,
                void *p_src3,
                rect_t *p_src3_rc,
                gpe_mix_param_t *p_param);

/*!
   do scale  the src image_t into a destination image_t

  \param[in] p_dev Points to the gpe device.
  \param[in] p_src_rgn Points to the source image_t
  \param[in] p_src_rect Points to the rect of src image_t to scale
  \param[in] p_dst_rgn Points to the destination image_t.
  \param[in] p_dst_rect Points to the rect of dst image_t to scale
  \param[in] table_id: scale coeff table id
  \param[in] matrix indicate use the matrix or not
  */

RET_CODE gpe_buf_scale(void *p_dev,
                         void *p_src_image,
                         rect_t *p_src_rect,
                         void *p_dst_image,
                         rect_t *p_dst_rect,
                         u32 table_id,
                         BOOL matrix);


/*!
   do scale  the src region into a destination region

  \param[in] p_dev Points to the gpe device.
  \param[in] p_src_rgn Points to the source region
  \param[in] p_src_rect Points to the rect of src region to scale
  \param[in] p_dst_rgn Points to the destination region.
  \param[in] p_dst_rect Points to the rect of dst region to scale
  \param[in] table_id: scale coeff table id
  \param[in] matrix indicate use the matrix or not
  */
RET_CODE gpe_share_scale(void *p_dev,
                         void *p_src_rgn,
                         rect_t *p_src_rect,
                         void *p_dst_rgn,
                         rect_t *p_dst_rect,
                         u32 table_id,
                         BOOL matrix);


/*!
   do scale the src region and then mix into a destination region

  \param[in] p_dev Points to the gpe device.
  \param[in] p_src_rgn Points to the source region
  \param[in] p_src_rect Points to the rect of src region to scale
  \param[in] p_dst_rgn Points to the destination region.
  \param[in] p_dst_rect Points to the rect of dst region to scale
  \param[in] p_param: scale param
  */
RET_CODE gpe_share_scale_mix(void *p_dev,
                         void *p_src_rgn,
                         rect_t *p_src_rect,
                         void *p_dst_rgn,
                         rect_t *p_dst_rect,
                         gpe_scale_param_t *p_param);

/*!
   check the region color
   this function just used for halping warirors to do the color check
  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the region
  \param[in] p_rect the checked rect in the region
  \param[in] color a color.
  */
RET_CODE gpe_check_color(void *p_dev, void *p_region, rect_t *p_rect, u32 color);


/*!
   set enable the cmdfifo
  \param[in] p_dev Points to the gpe device.
  \param[in] is_cmd indicate cmd fifo enable or not
  */
RET_CODE gpe_cmdfifo_set(void *p_dev, BOOL is_cmd);


/*!
   start run the cmd fifo
  \param[in] p_dev Points to the gpe device.
  */
RET_CODE gpe_cmdfifo_start(void *p_dev);


/*!
  Performs a bit-block transfer of the color data corresponding to a rectangle
  of pixels from the specified block of the src buf into a destination buf

  \param[in] p_dev Points to the gpe device.
  \param[in] p_dst Points to the destination image buf (image_t type)
  \param[in] p_dst_rc Points to the destination rectangle of the dst buf.
  \param[in] p_src Points to the source image buf (image_t type)
  \param[in] p_src_rc Points to the source rectangle.
  \param[in] p_param Points to the parameter package.
  */
RET_CODE gpe_buf_bitblt(void *p_dev,
                void *p_dst_image,
                rect_t *p_dst_rc,
                void *p_src_image,
                rect_t *p_src_rc,
                gpe_param_vsb_t *p_param);
/*!
   do scale  the src image_t into a destination image_t
  
  \param[in] p_dev Points to the gpe device.
  \param[in] p_src_image Points to the source image_t
  \param[in] p_src_rect Points to the rect of src image_t to scale
  \param[in] p_region Points to the destination region.
  \param[in] p_dst_rect Points to the rect of dst image_t to scale
  */
RET_CODE gpe_draw_image_scale(void *pdev,
                         void *p_src_image,
                         rect_t *p_src_rect,
                         void *p_region,
                         rect_t *p_dst_rect);
/*!
  Draws a rectangle with the specifies color into dst buf.

  \param[in] p_dev Points to the gpe device.
  \param[in] p_region Points to the dest image buf (image_t type)
  \param[in] p_rect Points to the rectangle position and size.
  \param[in] c Specifies the color.
  */
RET_CODE gpe_draw_buf_rectangle(void *p_dev, void *p_image, rect_t *p_rect, u32 c);

 
//==================================
// the following is added for concerto gpe
//==================================
/*!
rop modes
*/
typedef enum
{
    /*!
    Blackness
    */
    ROP_BLACK       = 0x00,     
    /*!
    ~(S | D)
    */
    ROP_NOTMERGEPEN = 0x11,     
    /*!
    ~S&D
    */
    ROP_MASKNOTPEN  = 0x22,    
    /*!
    ~S
    */
    ROP_NOTCOPYPEN  = 0x33,    
    /*!
    S&~D
    */
    ROP_MASKPENNOT  = 0x44,     
    /*!
    ~D
    */
    ROP_NOT         = 0x55,     
    /*!
    S^D
    */
    ROP_XORPEN      = 0x66,     
    /*!
    ~(S & D)
    */
    ROP_NOTMASKPEN  = 0x77,    
    /*!
    S&D
    */
    ROP_MASKPEN     = 0x88,    
    /*!
    ~(S^D)
    */
    ROP_NOTXORPEN   = 0x99,     
    /*!
    D
    */
    ROP_NOP         = 0xaa,     
    /*!
    ~S|D
    */
    ROP_MERGENOTPEN = 0xbb,    
    /*!
    S
    */
    ROP_COPYPEN     = 0xcc,     
    /*!
    S|~D
    */
    ROP_MERGEPENNOT = 0xdd,     
    /*!
    S|D
    */
    ROP_MERGEPEN    = 0xee,     
    /*!
    Whiteness
    */
    ROP_WHITE       = 0xff,     
    /*!
    p ^ d
    */
    ROP_PATINVERT   = 0x5a,     
    /*!
    s & p
    */
    ROP_MERGEPAINT  = 0xc0,     
    /*!
    p
    */
    ROP_PATCOPY     = 0xf0,   
    /*!
    ~s | p | d
    */
    ROP_PATPAINT    = 0xfb     
  }rop_mod_t;
/*!
  comments
  */
typedef enum
{
    GL_ZERO = 0,
    GL_ONE = 1,
    GL_SRC_COLOR = 2,
    GL_DST_COLOR = 3,
    GL_ONE_MINUS_SRC_COLOR = 4,
    GL_ONE_MINUS_DST_COLOR = 5,
    GL_DST_ALPHA = 6,
    GL_ONE_MINUS_DST_ALPHA = 7,
    GL_SRC_ALPHA = 8,
    GL_ONE_MINUS_SRC_ALPHA = 9,
    GL_SRC_ALPHA_SATURATE = 10,
}bld_fact_t;

/*!
  comments
  */
  typedef enum {
    VG_CLEAR_MASK,
    VG_FILL_MASK,
    VG_SET_MASK,
    VG_UNION_MASK,
    VG_INTERSECT_MASK,
    VG_SUBTRACT_MASK,

    VG_MASK_OPERATION_FORCE_SIZE,
  } vg_mask_operation_t;
/*!
  comments
  */
  typedef enum {
    VG_COLOR_RAMP_SPREAD_PAD,
    VG_COLOR_RAMP_SPREAD_REPEAT,
    VG_COLOR_RAMP_SPREAD_REFLECT,

    VG_COLOR_RAMP_SPREAD_MODE_FORCE_SIZE,
  } vg_color_ramp_spread_mode_t;
/*!
  comments
  */
  typedef enum {
    VG_TILE_FILL,
    VG_TILE_PAD,
    VG_TILE_REPEAT,
    VG_TILE_REFLECT,

    VG_TILING_MODE_FORCE_SIZE,
  } vg_tiling_mode_t;
/*!
  comments
  */
  typedef enum {
    VG_PAINT_TYPE_COLOR,
    VG_PAINT_TYPE_LINEAR_GRADIENT,
    VG_PAINT_TYPE_RADIAL_GRADIENT,
    VG_PAINT_TYPE_PATTERN,

    VG_PAINT_TYPE_FORCE_SIZE,
  } vg_paint_type_t;
/*!
  comments
  */
  typedef enum
  {
    /*!
      Asrc3 ? ROP/BLD(src1,src2):src2
      */
    GPE_CCT_ALPHA_MAP_LOGICAL,
    /*!
      [ARGB]src1.*[AAAA]src3
      */
    GPE_CCT_ALPHA_MAP_MIX_NORMAL,
    /*!
      [ARGB]src1.*[A111]src3
      */
    GPE_CCT_ALPHA_MAP_MIX_EX,

    ALPHA_MAP_MOD_MAX,
  }alpha_map_mod_t;

  /*!
indicate concerto rotator operation
*/
  typedef enum
  {
    GPE_CCT_NO_OP = 0,
    GPE_CCT_TRANS = 1,
    GPE_CCT_HORI_MIRROR = 2,
    GPE_CCT_HORI_MIRROR_TRANS = 3,
    GPE_CCT_VERT_MIRROR = 4,
    GPE_CCT_VERT_MIRROR_TRANS = 5,
    GPE_CCT_HORI_VERT_MIRROR = 6,
    GPE_CCT_HORI_VERT_MIRROR_TRANS = 7,
  }rotator_op_t;
/*!
  comments
  */
  typedef struct
  {
    /*!
      comments
      */
    u32 argb;
    /*!
      offset= offset_org <<12,offset_org: 0~1
      */
    u32 offset;
  }gradt_stop_t;
/*!
  comments
  */
  typedef struct
  {
    /*!
      comments
      */
    pos_t begin;
    /*!
      comments
      */
    pos_t end;
    /*!
      comments
      */
    gradt_stop_t stop0;
    /*!
      comments
      */
    gradt_stop_t stop1;
    /*!
      comments
      */
    gradt_stop_t stop2;
    /*!
      comments
      */
    gradt_stop_t stop3;
    /*!
      VGColorRampSpreadMode
      */
    u32 spread_mod;
  }paint_liner_gradt_cfg_t;
/*!
  comments
  */
  typedef struct
  {
    /*!
      comments
      */
    pos_t center;
    /*!
      comments
      */
    pos_t focus;
    /*!
      comments
      */
    u32 radius;
    /*!
      comments
      */
    gradt_stop_t stop0;
    /*!
      comments
      */
    gradt_stop_t stop1;
    /*!
      comments
      */
    gradt_stop_t stop2;
    /*!
      comments
      */
    gradt_stop_t stop3;
    /*!
      VGColorRampSpreadMode
      */
    u32 spread_mod;
  }paint_radial_gradt_cfg_t;
/*!
  comments
  */
  typedef struct
  {
    /*!
      comments
      */
    pos_t pat_beg;
    /*!
      comments
      */
    u32 fill_color;
    /*!
      VGTilingMode
      */
    u32 tiling_mod;
  }paint_pattern_cfg_t;
/*!
  comments
  */
  typedef struct
  {
    /*!
      VGPaintType
      */
    u32 paint_type;
    /*!
      paint color config
      */
    u32 paint_color;
    /*!
      paint liner gradient config
      */
    paint_liner_gradt_cfg_t paint_liner_gradt;
    /*!
      paint radial gradient config
      */
    paint_radial_gradt_cfg_t paint_radial_gradt;

    /*!
      paint pattern config
      */
    paint_pattern_cfg_t paint_pattern;

  }paint_info_t;
/*!
  comments
  */
  typedef struct
  {
    /*!
      comments
      */
    u32 buf;
    /*!
      unit:byte
      */
    u32 pitch;
    /*!
      unit:pixel
      */
    u32 width;
    /*!
      unit:pixel
      */
    u32 height;
    /*!
      comments
      */
    BOOL negative_stride;
    /*!
      comments
      */
    rect_vsb_t rect;
    /*!
      comments
      */
    pix_fmt_t pix_format;
    /*!
      alpha
      */
    BOOL alpha_pre_multed;
    /*!
      comments
      */
    BOOL alpha_ch_en;
    /*!
      comments
      */
    BOOL plane_alpha_en;
    /*!
      comments
      */
    u32 plane_alpha;
    /*!
      color key
      */
    BOOL ck_en;
    /*!
      comments
      */
    u32 key_color;
    /*!
      tile
      */
    u8 field_flag;
    /*!
      comments
      */
    u32 chroma_addr;
    /*!
      comments
      */
    u32 luma_addr;
    /*!
      xylc
      */
    u32 xylc_num;
    /*!
      comments
      */
    u32 xylc_color;
  }image_info_t;

/*!
  comments
  */
typedef struct
{
      /*!
      comments
      */
    u32 dert; //for 3d scaler
    /*!
      comments
      */
    u32 sub_dert; //for 3d scaler
    /*!
      comments
      */
    u32 ratio_offset; //for 3d scaler
    /*!
      comments
      */
    u32 sub_ratio_offset; //for 3d scaler
    /*!
      comments
      */
    u32 dst_fix_width; //for 3d scaler
    /*!
      comments
      */
    u32 fix_en; //for 3d scaler
}dert_t;
/*!
  for 3d scaler
  */
typedef struct
{
    /*!
      comments
      */
    u32 left_edge_adjust;
    /*!
      comments
      */
    u32 right_edge_adjust;
}edge_smooth_cfg_t;
/*!
  comments
*/
typedef struct
{
  /*!
     alpha
      */
    rop_mod_t  rop_a_id;
    /*!
     color
      */
    rop_mod_t  rop_c_id;
    /*!
      comments
      */
    u32 rop_pattern;
}rop_cfg_t;

/*!
  This structure defines blend factor
  */
typedef struct
{
  /*!
      comments
      */
    bld_fact_t  src_blend_fact;
    /*!
      comments
      */
    bld_fact_t  dst_blend_fact;
}blend_cfg_t;


/*!
  This structure defines the trapez for driver usage
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
}
trapez_t;

/*!
   2d or 3d scaler
 */
typedef struct
{
  /*!
     2d or 3d scaler
      */
    BOOL scale_2d_flag;
        /*!
      for 2d: 0=bypass, 1=filter, 2=matrix;
      for 3d: 0=bypass, 1=filter
      */
    u32 scaler_way;
    /*!
      comments
      */
    u32 scaler_tab_id;
    
    // the following are only used in 3d scaler
    /*!
     dst trape info
     */
    trapez_t dst_trape;
    /*!
      dert
      */
    BOOL dert_en;
    /*!
      dert cfg
      */
    dert_t dert_cfg;
    /*!
      edge smooth
      */
    BOOL edge_smooth_en;
    /*!
      edge smooth cfg
      */
    edge_smooth_cfg_t edge_smooth_cfg;

}scale_2d_3d_cfg_t;
/*!
    rotator
   */
typedef struct
{
  /*!
      tan(A/2) * 2048
  */
  u32 alpha;  
  /*!
      sin(A) * 2048
  */
  u32 beta;  
}rotator_angle_t;

/*!
    output mask
   */
typedef struct
{
  /*!
  buffer for output mask data
  */
  u32 mbuf_addr;
  /*!
  8bytes aligned
  */
  u32 mbuf_pitch;
  /*!
  0: store mask to mask buf, 
  1: store mask to alpha, no mask buf needed, 
  2: store src alpha to mask buf
  */
  u32 mask2alpha; 
}dst_mask_cfg_t;

/*!
    gpe operation
   */
typedef enum {
     GPE_OP_NONE                = 0x00000000,  
     GPE_OP_ROP                  = 0x000000001,
     GPE_OP_BLEND              = 0x000000002,
     GPE_OP_ALPHAMAP       = 0x000000004,
     GPE_OP_SCALE               = 0x000000008,
     GPE_OP_ROTATE            = 0x000000010,
     GPE_OP_PAINT               = 0x000000020,
     GPE_OP_MASK                = 0x000000040,
     GPE_OP_DMULT              = 0x000000080,
     GPE_OP_DSTEN              = 0x000000100,
} gpe_ops_t;

/*!
  for test
  */
#define IC_VERIFACTION

/*!
  struct for common use
  */
  typedef struct
  {
//#ifdef IC_VERIFACTION 
    /*!
      when color key match, dst out or 0xffffffff out
      */
    BOOL src2_key_msk;
    /*!
      1:write 0xffffffff to dst when key match
      */
    BOOL proc_out_key_set;
//#endif     
    /*!
      comments
      */
    BOOL no_dithering;
    /*!
      comments
      */
    BOOL src_img_en;
    /*!
      comments
      */
    BOOL ex_img_en;
    /*!
      comments
      */
    image_info_t src_img;
    /*!
      src2(background) image
      */
    image_info_t dst_img;
    /*!
      comments
      */
    image_info_t ex_img;
    /*!
     palette
      */
    u32 palette_base;
    /*!
      comments
      */
    u32 palette_size;
   /*!
      comments
   */
    gpe_ops_t gpe_op;
    /*!
      comments
      */
    paint_info_t paint;
    /*!
      comments
      */
    alpha_map_mod_t alpha_map_mod;
    /*!
      blend cfg
      */
    blend_cfg_t blend;
    /*!
     rop cfg
      */
    rop_cfg_t rop;
    /*!
     scaler cfg
      */
    scale_2d_3d_cfg_t scale;
    /*!
     if mask enable, src1 and src2 must be GRAY-8??
      */
    u32 mask_mod;
    /*!
      comments
      */
    rotator_op_t rotator_op;
   /*!
     comments
   */
   BOOL src_with_mask;
   /*!
     comments
   */
   BOOL dst_with_mask; // true in 3d scaler 
   /*!
     comments
   */
   dst_mask_cfg_t dst_mask;
   /*!
   background image for scroll blit
   background is the same as dst image except buf addr, fmt and rect
   */
   BOOL bg_en;
   /*!
      comments
      */
   u32 bg_buf;
   /*!
      comments
      */
   rect_vsb_t bg_rect;
   /*!
      comments
      */
   u32 bg_pitch;
   /*!
      comments
      */
   pix_fmt_t bg_fmt;
   /*!
      comments
      */
   BOOL bg_alpha_en;
  }cct_param_t;

//*******************************************************
//concerto for hisi use
//*******************************************************
/*!
  rotate struct for hisi interface
  */
typedef enum
{
    /*!
      no Rotate
      */
    LLD_GFX_ROTATE_NONE = 0,
    /*!
      Rotate 90° clockwise
      */
    LLD_GFX_ROTATE_90,    
    /*!
      Rotate 180° clockwise
      */
    LLD_GFX_ROTATE_180,   
    /*!
      Rotate 270° clockwise
      */
    LLD_GFX_ROTATE_270,   
    /*!
      comments
      */
    LLD_GFX_ROTATE_BUTT
} lld_gfx_rotator_t;

/*!
  mirror struct for hisi interface
  */
typedef enum
{
    /*!
      no mirror
      */
    LLD_GFX_MIRROR_NONE = 0,
    /*!
      Mirror the left and the right
      */
    LLD_GFX_MIRROR_LR,     
    /*!
      Mirror the top and the bottom
      */
    LLD_GFX_MIRROR_TB,     
    /*!
      comments
      */
    LLD_GFX_MIRROR_BUTT
} lld_gfx_mirror_t;

/*!
  3d scaler config struct for hisi interface
  */
typedef struct
{  
    // the following are only used in 3d scaler
    /*!
     dst trape info
     */
    trapez_t dst_trape;
    /*!
      dert
      */
    BOOL dert_en;
    /*!
      dert cfg
      */
    dert_t dert_cfg;
    /*!
      edge smooth
      */
    BOOL edge_smooth_en;
    /*!
      edge smooth cfg
      */
    edge_smooth_cfg_t edge_smooth_cfg;
}scaler3d_cfg_t;

/*!
  fill rect command struct for hisi interface
  */
typedef struct
{
    /*!
      rop enable
      */
    BOOL enableRop;       
    /*!
      blending enable
      */
    BOOL enableBld;
    /*!
      dst color key enable
      */
    BOOL enableDstCkey;    
    /*!
      rop config
      */
    rop_cfg_t ropCfg;
    /*!
      blending config
      */
    blend_cfg_t blendCfg; 
}cct_gpe_fillopt_t;

/*!
  two src blt command struct for hisi interface
  */
typedef struct
{
    /*!
      rop enable
      */
    BOOL enableRop;       
    /*!
      blending enable
      */
    BOOL enableBld;
    /*!
      2d scaler enable
      */
    BOOL enable2dScaler;
    /*!
      src color key enable
      */
    BOOL enableSrcCkey;
    /*!
      dst color key enable
      */
    BOOL enableDstCkey;
    /*!
      src global alpha enable
      */
    BOOL enableSrcGlobalAlpha;
    /*!
      src alpha premult enable
    */
    BOOL enableSrcPreMult;
    /*!
      disable src alpha
    */
    BOOL disableSrcAlpha;
    /*!
      disable dst alpha
    */
    BOOL disableDstAlpha;
    /*!
      flip src image data
    */
    BOOL srcFlip; //src data is from bottom to top

    /*!
      rop config
      */
    rop_cfg_t ropCfg;
    /*!
      blending config
      */
    blend_cfg_t blendCfg;  
    /*!
    default:0
    */
    BOOL disable_dithering;
}cct_gpe_blt2opt_t;
/*!
comments
*/
typedef struct
{
    /*!
    comments
    */
    BOOL enableRop;
    /*!
    comments
    */
    BOOL enableBld;
    /*!
    comments
    */
    BOOL enableDstCkey;
    /*!
    comments
    */
    BOOL disableDstAlpha;
    /*!
    comments
    */
    rop_cfg_t ropCfg;
    /*!
    comments
    */
    blend_cfg_t blendCfg;
    /*!
    comments
    */
    paint_info_t paint_cfg;
}cct_gpe_paint2opt_t;

/*!
comments
*/
typedef struct
{
    /*!
    comments
    */
    BOOL enableRop;   
    /*!
    comments
    */
    BOOL enableBld;
    /*!
    comments
    */
    BOOL enableAMapLogical;
    /*!
    comments
    */
    BOOL enableAMapMix;
    /*!
    comments
    */
    BOOL enableAMapMixEx;
    /*!
    comments
    */
    BOOL enableDrawMult;
    /*!
    comments
    */
    BOOL enableDrawSten;
    /*!
    comments
    */
    BOOL enableDstCkey;
    /*!
    comments
    */
    BOOL enableExCkey;
    /*!
    comments
    */
    BOOL enableExGlobalAlpha;
    /*!
    comments
    */
    BOOL enableExPreMult;
    /*!
    comments
    */
    BOOL disableExAlpha;
    /*!
    comments
    */
    BOOL disableDstAlpha;
    /*!
    comments
    */
    rop_cfg_t ropCfg;
    /*!
    comments
    */
    blend_cfg_t blendCfg; 
    /*!
    comments
    */
    paint_info_t paint_cfg;
}cct_gpe_paint3opt_t;
/*!
  three src blt command struct for hisi interface
  */
typedef struct
{
    /*!
    rop enable
    */
    BOOL enableRop;       
    /*!
    blending enable
    */
    BOOL enableBld;
    /*!
    2d scaler enable
    */
    BOOL enable2dScaler;

    /*!
    alpha map logical mode enable
    */
    BOOL enableAMapLogical;
    /*!
    alpha map mix mode enable
    */
    BOOL enableAMapMix;
    /*!
    comments
    */
    BOOL enableAMapMixEx;
    /*!
    draw image multiply enable
    */
    BOOL enableDrawMult;
    /*!
    draw image stencil enable
    */
    BOOL enableDrawSten;
    /*!
    src color key enable
    */
    BOOL enableSrcCkey;
    /*!
    dst color key enable
    */
    BOOL enableDstCkey;
    /*!
    dst color key enable
    */
    BOOL enableExCkey;
    /*!
    src global alpha enable
    */
    BOOL enableSrcGlobalAlpha;
    /*!
    ex global alpha enable
    */
    BOOL enableExGlobalAlpha;
    /*!
    src alpha premult enable
    */
    BOOL enableSrcPreMult;
    /*!
    ex alpha premult enable
    */
    BOOL enableExPreMult;
    /*!
    comments
    */
    BOOL disableSrcAlpha;
    /*!
    comments
    */
    BOOL disableExAlpha;
    /*!
    comments
    */
    BOOL disableDstAlpha;
    /*!
    comments
    */
    BOOL srcFlip;

    /*!
    rop config
    */
    rop_cfg_t ropCfg;
    /*!
    blending config
    */
    blend_cfg_t blendCfg;  
}cct_gpe_blt3opt_t;

/*!
comments
*/
typedef struct
{
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
}cct_gpe_dertopt_t;

/*!
comments
*/
typedef struct
{
    /*!
    comments
    */
    BOOL enableRop;
    /*!
    comments
    */
    BOOL enableBld;
    /*!
    comments
    */
    BOOL enableDstCkey;
    /*!
    comments
    */
    u32 dstColorkey;
    /*!
    comments
    */
    rop_cfg_t ropCfg;
    /*!
    comments
    */
    blend_cfg_t blendCfg;
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
}cct_gpe_trapezopt_t;

/*!
comments
*/
typedef struct
{
    /*!
    comments
    */
    BOOL enableRop;
    /*!
    comments
    */
    BOOL enableBld;
    /*!
    comments
    */
    BOOL enableDstCkey;
    /*!
    comments
    */
    u32 dstColorkey;
    /*!
    comments
    */
    rop_cfg_t ropCfg;
    /*!
    comments
    */
    blend_cfg_t blendCfg;
}cct_gpe_rotopt_t;

/*!
  comment
  */
typedef struct
{
    /*!
      plane alpha enable
      */
    BOOL plane_alpha_en;
    /*!
      plane alpha
      */
    u32 plane_alpha;
    /*!
      color key enable
      */
    BOOL keycolor_en;
    /*!
      color key
      */
    u32 key_color;
}surface_info_t;

/*!
set parameters for gpe, used in concerto gpe
  */
BOOL gpe_set_parameter(void *p_dev, cct_param_t *p_param);

/*!
start gpe, used in concerto gpe
  */
BOOL gpe_start(void *p_dev);

/*!
   two source bitblt, used in concerto gpe
  */
RET_CODE gpe_bitblt2src(void *p_dev,
                                       void *p_src_rgn,
                                       void *p_dst_rgn,
                                       rect_vsb_t *p_src_rect,
                                       rect_vsb_t *p_dst_rect,
                                       surface_info_t *p_src_info,
                                       surface_info_t *p_dst_info,
                                       cct_gpe_blt2opt_t *opt);
/*!
   three source bitblt, used in concerto gpe
  */
RET_CODE gpe_bitblt3src(void *p_dev,
                                       void *p_src_rgn,
                                       void *p_dst_rgn,
                                       void *p_ex_rgn,
                                       rect_vsb_t *p_src_rect,
                                       rect_vsb_t *p_dst_rect,
                                       rect_vsb_t *p_ex_rect,
                                       surface_info_t *p_src_info,
                                       surface_info_t *p_dst_info,
                                       surface_info_t *p_ex_info,
                                       cct_gpe_blt3opt_t *opt);
/*!
  fill rect ex, used in concerto gpe
  */
RET_CODE gpe_fill_rect_ex(void *p_dev,
                                       void *p_region,
                                        rect_vsb_t *p_rect,
                                        u32 color,
                                        u32 colorkey,
                                        cct_gpe_fillopt_t *opt);

/*!
  paint blit, used in concerto gpe
  */
RET_CODE gpe_paint_blit(void *p_dev,
                                       void *p_dst_rgn,
                                       rect_vsb_t *p_dst_rect,
                                       surface_info_t *p_dst_info,
                                       cct_gpe_paint2opt_t *opt);

/*!
  3src paint blit, used in concerto gpe
  */
RET_CODE gpe_paint_blit_3src(void *p_dev,
                                       void *p_dst_rgn,
                                       void *p_ex_rgn,
                                       rect_vsb_t *p_dst_rect,
                                       rect_vsb_t *p_ex_rect,
                                       surface_info_t *p_dst_info,
                                       surface_info_t *p_ex_info,
                                       cct_gpe_paint3opt_t *opt);

/*!
  pattern paint, used in concerto gpe
  */
RET_CODE gpe_pattern_paint(void *p_dev,
                                       void *p_src_rgn,
                                       void *p_dst_rgn,
                                       rect_vsb_t *p_src_rect,
                                       rect_vsb_t *p_dst_rect,
                                       pos_t *pat_beg,
                                       vg_tiling_mode_t tiling_mod,
                                       u32 fill_color);

/*!
  rotate and mirror, used in concerto gpe
  */
RET_CODE gpe_rotate_mirror(void *p_dev,
                                       void *p_src_rgn,
                                       void *p_dst_rgn,
                                       rect_vsb_t *p_src_rect,
                                       pos_t *dst_pos,
                                       lld_gfx_rotator_t rotate_mod,
                                       lld_gfx_mirror_t mirror_mod);
/*!
  clip mask, used in concerto gpe
  */
RET_CODE gpe_clip_mask(void *p_dev,
                                       void *p_src_rgn,
                                       void *p_dst_rgn,
                                       rect_vsb_t *p_src_rect,
                                       rect_vsb_t *p_dst_rect,
                                       vg_mask_operation_t mask_opt);

/*!
  2src blit extended api, used in concerto gpe
  */
RET_CODE gpe_blit_ex(void *p_dev,
                                       void *p_src_rgn,
                                       void *p_bg_rgn,
                                       void *p_dst_rgn,
                                       rect_vsb_t *p_src_rect,
                                       rect_vsb_t *p_bg_rect,
                                       rect_vsb_t *p_dst_rect,
                                       surface_info_t *p_src_info,
                                       surface_info_t *p_dst_info,
                                       cct_gpe_blt2opt_t *opt);
/*!
  3src blit extended api, used in concerto gpe
  */
RET_CODE gpe_blit_3src_ex(void *p_dev,
                                       void *p_src_rgn,
                                       void *p_bg_rgn,
                                       void *p_dst_rgn,
                                       void *p_ex_rgn,
                                       rect_vsb_t *p_src_rect,
                                       rect_vsb_t *p_bg_rect,
                                       rect_vsb_t *p_dst_rect,
                                       rect_vsb_t *p_ex_rect,
                                       surface_info_t *p_src_info,
                                       surface_info_t *p_dst_info,
                                       surface_info_t *p_ex_info,
                                       cct_gpe_blt3opt_t *opt);
/*!
  2src paint blit extended api, used in concerto gpe
  */
RET_CODE gpe_paint_blit_ex(void *p_dev,
                                       void *p_bg_rgn,
                                       void *p_dst_rgn,
                                       rect_vsb_t *p_bg_rect,
                                       rect_vsb_t *p_dst_rect,
                                       surface_info_t *p_dst_info,
                                       cct_gpe_paint2opt_t *opt);

/*!
  3src paint blit extended api, used in concerto gpe
  */
RET_CODE gpe_paint_blit_3src_ex(void *p_dev,
                                        void *p_bg_rgn,
                                       void *p_dst_rgn,
                                       void *p_ex_rgn,
                                       rect_vsb_t *p_bg_rect,
                                       rect_vsb_t *p_dst_rect,
                                       rect_vsb_t *p_ex_rect,
                                       surface_info_t *p_dst_info,
                                       surface_info_t *p_ex_info,
                                       cct_gpe_paint3opt_t *opt);

/*!
  3src paint blit extended api, used in concerto gpe
  */
RET_CODE gpe_dert_scale(void *p_dev,
                                       void *p_src_rgn,
                                        void *p_dst_rgn,
                                        rect_vsb_t *p_src_rect,
                                       rect_vsb_t *p_dst_rect,
                                       cct_gpe_dertopt_t *opt);

/*!
  矩形到带远近效果的梯形，used in concerto gpe
*/
RET_CODE gpe_draw_3d_trapez(void *p_dev,
                                        void *p_src_rgn,
                                        void *p_dst_rgn,
                                        rect_vsb_t *p_src_rect,
                                        rect_vsb_t *p_dst_rect,
                                        trapez_t *p_dst_trape,
                                        cct_gpe_trapezopt_t *opt);

/*!
rotate arbitrary angle for concerto; if rotate 90/180/270, please use rotate_mirror
*/
RET_CODE gpe_rotate_arbitrary_angle(void *p_dev,
                                        void *p_src_rgn,
                                        void *p_dst_rgn,
                                        rect_vsb_t *p_src_rect,
                                        pos_t *p_dst_pos,
                                        rotator_angle_t *p_angle,
                                        cct_gpe_rotopt_t *opt);

/*!
capture video layer to dst region
*/
RET_CODE gpe_vid_capture(void *p_dev, void *p_dst_rgn);
#if 0   
//for ic verification                                   
RET_CODE gpe_draw_3d_test(void *p_dev,
                                        image_info_t *p_dst,
                                        trapez_t *p_dst_trape,
                                        image_info_t *p_src,
                                        void *p_palette,
                                        u32 entries,
                                        gpe_ops_t gpe_op,
                                        blend_cfg_t *p_blend,
                                        rop_cfg_t *p_rop);
RET_CODE gpe_rotate_test(void *p_dev,
                                        image_info_t *p_dst,
                                        image_info_t *p_src,
                                        void *p_palette,
                                        u32 entries,
                                        gpe_ops_t gpe_op,
                                        rotator_angle_t *p_angle,
                                        blend_cfg_t *p_blend,
                                        rop_cfg_t *p_rop);
#endif                                      
#ifdef __cplusplus
}
#endif

#endif //__GPE_H__

