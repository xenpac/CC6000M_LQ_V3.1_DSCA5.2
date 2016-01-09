/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/

/******************************************************************************/
#ifndef _FASTLOGO_DISPLAY_H
#define _FASTLOGO_DISPLAY_H

/*!
   Align bytes macro
  */
#define ALIGN_SHIFT    (3)

//yuanruilin shift display_ap_warriors_reg.h here
/*!
  the enum of DISP registers
  */
enum
{
    REG_DISP_DISPLAY_CTRL               = 0xbfd20000,
    REG_DISP_VSCALER_RATIO_HD           = 0xbfd20004,
    REG_DISP_VSCALER_RATIO_INIT_HD      = 0xbfd20008,
    REG_DISP_VID_DISP_FIELD             = 0xbfd2000c,
    REG_DISP_VID_WINDOW_CUT_SD          = 0xbfd20010,
    REG_DISP_VID_WINDOW_X_HD            = 0xbfd20014,
    REG_DISP_VID_WINDOW_Y_HD            = 0xbfd20018,
    REG_DISP_VID_WINDOW_CUT_HD          = 0xbfd2001c,
    REG_DISP_VSCALER_RATIO_SD           = 0xbfd20020,
    REG_DISP_VSCALER_RATIO_INIT_SD      = 0xbfd20024,
    REG_DISP_VID_WINDOW_X_SD            = 0xbfd20028,
    REG_DISP_VID_WINDOW_Y_SD            = 0xbfd2002c,
    REG_DISP_GRAPHIC_CTRL               = 0xbfd20030,
    REG_DISP_BG_COLOR                   = 0xbfd20034,
    REG_DISP_STILL_X_HD                 = 0xbfd20038,
    REG_DISP_STILL_Y_HD                 = 0xbfd2003c,
    REG_DISP_OSD0_CMD_HD                = 0xbfd20040,
    REG_DISP_SUB_CMD_HD                 = 0xbfd20044,
    REG_DISP_LAYER_ALPHA                = 0xbfd20048,
    REG_DISP_VID_INPUT_SIZE             = 0xbfd2004c,
    REG_DISP_VID_SD_DROP_LINE           = 0xbfd20050,
    REG_DISP_VID_CROP_MODE_EN           = 0xbfd20054,
    REG_DISP_VID_CROP_HORI              = 0xbfd20058,
    REG_DISP_VID_CROP_VERT              = 0xbfd2005c,
    REG_DISP_VID_PROCESS_MODE           = 0xbfd20060,
    REG_DISP_OSD1_CMD_HD                = 0xbfd2006c,
    REG_DISP_OSD1_CK_HD                 = 0xbfd20070,
    REG_DISP_STILL_X_SD                 = 0xbfd20074,
    REG_DISP_STILL_Y_SD                 = 0xbfd20078,
    REG_DISP_STILL_STRIDE_SD            = 0xbfd2007c,
    REG_DISP_RGB2Y_COEF                 = 0xbfd20080,
    REG_DISP_RGB2CB_COEF                = 0xbfd20084,
    REG_DISP_RGB2CR_COEF                = 0xbfd20088,
    REG_DISP_VID_DECOMP_CFG             = 0xbfd2008C,
    REG_DISP_OSD0_CK_HD                 = 0xbfd20090,
    REG_DISP_RGB2YUV_YOFFSET            = 0xbfd20094,
    REG_DISP_RGB2YUV_UVOFFSET           = 0xbfd20098,
    REG_DISP_VID_VERF_CFG               = 0xbfd200A8,
    REG_DISP_VID_HORF_CFG               = 0xbfd200AC,
    REG_DISP_LAYER_MIX_CFG              = 0xbfd200B0,
    REG_DISP_HD_SIZE_OUT                = 0xbfd200B8,
    REG_DISP_SD_SIZE_OUT                = 0xbfd200BC,
    REG_DISP_STILL_STRIDE_HD            = 0xbfd200C0,
    REG_DISP_HDTV_CFG                   = 0xbfd200C4,
    REG_DISP_HD_POST_CFG                = 0xbfd200CC,
    REG_DISP_SD_POST_CFG                = 0xbfd200D0,
    REG_DISP_EFFECT_COEF                = 0xbfd200D4,
    REG_DISP_GRA_FIFO_THRESHOLD         = 0xbfd200F4,
    REG_DISP_GRA_SCALER_CTRL            = 0xbfd200F8,
    REG_DISP_GRA_SCALER_HRATIO          = 0xbfd200FC,
    REG_DISP_GRA_SCALER_VRATIO          = 0xbfd20100,
    REG_DISP_GRA_SCALER_H_START_FRA     = 0xbfd20104,
    REG_DISP_GRA_SCALER_V_START_FRA     = 0xbfd20108,
    REG_DISP_GRA_CTL                    = 0xbfd2010C,
    REG_DISP_OSD_SCALE_HSIZE            = 0xbfd20110,
    REG_DISP_OSD_SCALE_RATIO            = 0xbfd20114,
    REG_DISP_OSD_ALPHA                  = 0xbfd20118,
    REG_DISP_SD_WRBACK_ADDR_ODD         = 0xbfd21000,
    REG_DISP_SD_BASE_ADDR_EVEN          = 0xbfd21004,
    REG_DISP_SD_WRBACK_CTRL             = 0xbfd21008,
    REG_DISP_STILL_START_ADDR_HD        = 0xbfd21020,
    REG_DISP_SUB_START_ADDR_HD          = 0xbfd21024,
    REG_DISP_OSD1_START_ADDR_HD         = 0xbfd21028,
    REG_DISP_STILL_START_ADDR_SD        = 0xbfd2102C,
    REG_DISP_OSD0_START_ADDR_HD         = 0xbfd21030,
    REG_DISP_VIDEO_DISPLAY_INFO         = 0xbfd21034,
    REG_DISP_MOTION_PRE_ADDR            = 0xbfd21040,
    REG_DISP_MOTION_CUR_ADDR            = 0xbfd21044,
    REG_DISP_LUMA_PRE_ADDR              = 0xbfd21048,
    REG_DISP_LUMA_CUR_ADDR              = 0xbfd2104C,
    REG_DISP_LUMA_NEXT_ADDR             = 0xbfd21050,
    REG_DISP_CHROMA_PPRE_ADDR           = 0xbfd21054,
    REG_DISP_CHROMA_PRE_ADDR            = 0xbfd21058,
    REG_DISP_CHROMA_CUR_ADDR            = 0xbfd2105C,
    REG_DISP_CHROMA_NEXT_ADDR           = 0xbfd21060,
    REG_DISP_MOTION_PRE_ADDR2           = 0xbfd21064,
    REG_DISP_MOTION_CUR_ADDR2           = 0xbfd21068,
    REG_DISP_LUMA_PRE_ADDR2             = 0xbfd2106C,
    REG_DISP_LUMA_CUR_ADDR2             = 0xbfd21070,
    REG_DISP_LUMA_NEXT_ADDR2            = 0xbfd21074,
    REG_DISP_CHROMA_PPRE_ADDR2          = 0xbfd21078,
    REG_DISP_CHROMA_PRE_ADDR2           = 0xbfd2107C,
    REG_DISP_CHROMA_CUR_ADDR2           = 0xbfd21080,
    REG_DISP_CHROMA_NEXT_ADDR2          = 0xbfd21084,
    REG_DISP_FIELD_PIC_FMT              = 0xbfd2108C,
    REG_DISP_VID_HD_VF_COEF_ADDR        = 0xbfd21098,
    REG_DISP_VID_HD_HF_COEF_ADDR        = 0xbfd2109C,
    REG_DISP_VID_SD_VF_COEF_ADDR        = 0xbfd210A0,
    REG_DISP_VID_SD_HF_COEF_ADDR        = 0xbfd210A4,
    REG_DISP_GRA_VF_COEF_ADDR           = 0xbfd210A8,
    REG_DISP_GRA_HF_COEF_ADDR           = 0xbfd210AC,
    REG_DISP_VSCALER_TABLE_SEL          = 0xbfd210B4,
    REG_DISP_OSD_HF_COEF_ADDR           = 0xbfd210C0,
    REG_DISP_DI_ENABLE                  = 0xbfd22000,
    REG_DISP_VIDEO_LINE_RD_CNT_MAX      = 0xbfd23008
};

/*!
  the union of register reg_disp_display_ctrl
  */
typedef union reg_disp_display_ctrl
{
    u32 all;
    struct
    {
        u32 hd_hf_sel                   : 1;
        u32                             : 1;
        u32 hd_cut_en                   : 1;
        u32 sd_cut_en                   : 1;
        u32 hd_vf_sel                   : 1;
        u32                             : 15;
        u32 vf_in_frame                 : 1;
        u32                             : 3;
        u32 vid_sel                     : 1;
        u32                             : 7;
    } bitc;
} reg_disp_display_ctrl_t;

/*!
  the union of register reg_disp_vscaler_ratio_hd
  */
typedef union reg_disp_vscaler_ratio_hd
{
    u32 all;
    struct
    {
        u32 hratio_int_hd               : 4;
        u32 hratio_fra_hd               : 12;
        u32 vratio_int_hd               : 4;
        u32 vratio_fra_hd               : 12;
    } bitc;
} reg_disp_vscaler_ratio_hd_t;

/*!
  the union of register reg_disp_vscaler_ratio_init_hd
  */
typedef union reg_disp_vscaler_ratio_init_hd
{
    u32 all;
    struct
    {
        u32 top_int_hd                  : 4;
        u32 top_fra_hd                  : 12;
        u32 bot_int_hd                  : 4;
        u32 bot_fra_hd                  : 12;
    } bitc;
} reg_disp_vscaler_ratio_init_hd_t;

/*!
  the union of register reg_disp_vid_disp_field
  */
typedef union reg_disp_vid_disp_field
{
    u32 all;
    struct
    {
        u32                             : 28;
        u32 vid_field                   : 2;
        u32                             : 2;
    } bitc;
} reg_disp_vid_disp_field_t;

/*!
  the union of register reg_disp_vid_window_cut_sd
  */
typedef union reg_disp_vid_window_cut_sd
{
    u32 all;
    struct
    {
        u32 left_cut                    : 8;
        u32 right_cut                   : 8;
        u32 top_cut                     : 8;
        u32 bot_cut                     : 8;
    } bitc;
} reg_disp_vid_window_cut_sd_t;

/*!
  the union of register reg_disp_vid_window_x_hd
  */
typedef union reg_disp_vid_window_x_hd
{
    u32 all;
    struct
    {
        u32 vid_x_left_hd               : 11;
        u32                             : 5;
        u32 vid_x_right_hd              : 11;
        u32                             : 5;
    } bitc;
} reg_disp_vid_window_x_hd_t;

/*!
  the union of register reg_disp_vid_window_y_hd
  */
typedef union reg_disp_vid_window_y_hd
{
    u32 all;
    struct
    {
        u32 vid_y_start_hd              : 11;
        u32                             : 5;
        u32 vid_y_end_hd                : 11;
        u32                             : 5;
    } bitc;
} reg_disp_vid_window_y_hd_t;

/*!
  the union of register reg_disp_vid_window_cut_hd
  */
typedef union reg_disp_vid_window_cut_hd
{
    u32 all;
    struct
    {
        u32 left_cut                    : 8;
        u32 right_cut                   : 8;
        u32 top_cut                     : 8;
        u32 bot_cut                     : 8;
    } bitc;
} reg_disp_vid_window_cut_hd_t;

/*!
  the union of register reg_disp_vscaler_ratio_sd
  */
typedef union reg_disp_vscaler_ratio_sd
{
    u32 all;
    struct
    {
        u32 hratio_int_sd               : 4;
        u32 hratio_fra_sd               : 12;
        u32 vratio_int_sd               : 4;
        u32 vratio_fra_sd               : 12;
    } bitc;
} reg_disp_vscaler_ratio_sd_t;

/*!
  the union of register reg_disp_vscaler_ratio_init_sd
  */
typedef union reg_disp_vscaler_ratio_init_sd
{
    u32 all;
    struct
    {
        u32 top_int_sd                  : 4;
        u32 top_fra_sd                  : 12;
        u32 bot_int_sd                  : 4;
        u32 bot_fra_sd                  : 12;
    } bitc;
} reg_disp_vscaler_ratio_init_sd_t;

/*!
  the union of register reg_disp_vid_window_x_sd
  */
typedef union reg_disp_vid_window_x_sd
{
    u32 all;
    struct
    {
        u32 vid_x_left_sd               : 11;
        u32                             : 5;
        u32 vid_x_right_sd              : 11;
        u32                             : 5;
    } bitc;
} reg_disp_vid_window_x_sd_t;

/*!
  the union of register reg_disp_vid_window_y_sd
  */
typedef union reg_disp_vid_window_y_sd
{
    u32 all;
    struct
    {
        u32 vid_y_start_sd              : 11;
        u32                             : 5;
        u32 vid_y_end_sd                : 11;
        u32                             : 5;
    } bitc;
} reg_disp_vid_window_y_sd_t;

/*!
  the union of register reg_disp_graphic_ctrl
  */
typedef union reg_disp_graphic_ctrl
{
    u32 all;
    struct
    {
        u32                             : 8;
        u32 back_sel                    : 1;
        u32 still_sel_sd                : 1;
        u32 still_sd_format_444         : 1;
        u32                             : 1;
        u32 still_sel_hd                : 1;
        u32                             : 2;
        u32 still_hd_format_444         : 1;
        u32                             : 16;
    } bitc;
} reg_disp_graphic_ctrl_t;

/*!
  the union of register reg_disp_bg_color
  */
typedef union reg_disp_bg_color
{
    u32 all;
    struct
    {
        u32 bg_cr                       : 8;
        u32 bg_cb                       : 8;
        u32 bg_y                        : 8;
        u32                             : 8;
    } bitc;
} reg_disp_bg_color_t;

/*!
  the union of register reg_disp_still_x_hd
  */
typedef union reg_disp_still_x_hd
{
    u32 all;
    struct
    {
        u32 hd_still_startx             : 11;
        u32                             : 5;
        u32 hd_still_endx               : 11;
        u32                             : 5;
    } bitc;
} reg_disp_still_x_hd_t;

/*!
  the union of register reg_disp_still_y_hd
  */
typedef union reg_disp_still_y_hd
{
    u32 all;
    struct
    {
        u32 hd_still_starty             : 11;
        u32                             : 5;
        u32 hd_still_endy               : 11;
        u32                             : 5;
    } bitc;
} reg_disp_still_y_hd_t;

/*!
  the union of register reg_disp_osd0_cmd_hd
  */
typedef union reg_disp_osd0_cmd_hd
{
    u32 all;
    struct
    {
        u32 osd0_sel                    : 1;
        u32                             : 7;
        u32 plane_alpha_en              : 1;
        u32                             : 7;
        u32 plane_alpha                 : 8;
        u32                             : 4;
        u32 osd0_big_endian             : 1;
        u32                             : 3;
    } bitc;
} reg_disp_osd0_cmd_hd_t;

/*!
  the union of register reg_disp_sub_cmd_hd
  */
typedef union reg_disp_sub_cmd_hd
{
    u32 all;
    struct
    {
        u32 sub_sel                     : 1;
        u32                             : 7;
        u32 plane_alpha_en              : 1;
        u32                             : 7;
        u32 plane_alpha                 : 8;
        u32                             : 4;
        u32 sub_big_endian              : 1;
        u32                             : 3;
    } bitc;
} reg_disp_sub_cmd_hd_t;

/*!
  the union of register reg_disp_layer_alpha
  */
typedef union reg_disp_layer_alpha
{
    u32 all;
    struct
    {
        u32                             : 16;
        u32 vid_plane_alpha             : 8;
        u32 still_plane_alpha           : 8;
    } bitc;
} reg_disp_layer_alpha_t;

/*!
  the union of register reg_disp_vid_input_size
  */
typedef union reg_disp_vid_input_size
{
    u32 all;
    struct
    {
        u32 vid_frame_h                 : 11;
        u32                             : 5;
        u32 vid_frame_w                 : 11;
        u32                             : 5;
    } bitc;
} reg_disp_vid_input_size_t;

/*!
  the union of register reg_disp_vid_sd_drop_line
  */
typedef union reg_disp_vid_sd_drop_line
{
    u32 all;
    struct
    {
        u32 sd_top_field_drop           : 4;
        u32                             : 12;
        u32 sd_bot_field_drop           : 4;
        u32                             : 12;
    } bitc;
} reg_disp_vid_sd_drop_line_t;

/*!
  the union of register reg_disp_vid_crop_mode_en
  */
typedef union reg_disp_vid_crop_mode_en
{
    u32 all;
    struct
    {
        u32 vid_crop_en                 : 1;
        u32                             : 31;
    } bitc;
} reg_disp_vid_crop_mode_en_t;

/*!
  the union of register reg_disp_vid_crop_hori
  */
typedef union reg_disp_vid_crop_hori
{
    u32 all;
    struct
    {
        u32 vid_crop_endx               : 11;
        u32                             : 5;
        u32 vid_crop_startx             : 11;
        u32                             : 5;
    } bitc;
} reg_disp_vid_crop_hori_t;

/*!
  the union of register reg_disp_vid_crop_vert
  */
typedef union reg_disp_vid_crop_vert
{
    u32 all;
    struct
    {
        u32 vid_crop_endy               : 11;
        u32                             : 5;
        u32 vid_crop_starty             : 11;
        u32                             : 5;
    } bitc;
} reg_disp_vid_crop_vert_t;

/*!
  the union of register reg_disp_vid_process_mode
  */
typedef union reg_disp_vid_process_mode
{
    u32 all;
    struct
    {
        u32 di_process                  : 1;
        u32                             : 31;
    } bitc;
} reg_disp_vid_process_mode_t;

/*!
  the union of register reg_disp_osd1_cmd_hd
  */
typedef union reg_disp_osd1_cmd_hd
{
    u32 all;
    struct
    {
        u32 osd1_sel                    : 1;
        u32                             : 7;
        u32 plane_alpha_en              : 1;
        u32                             : 7;
        u32 plane_alpha                 : 8;
        u32                             : 4;
        u32 osd1_big_endian             : 1;
        u32                             : 3;
    } bitc;
} reg_disp_osd1_cmd_hd_t;

/*!
  the union of register reg_disp_osd1_ck_hd
  */
typedef union reg_disp_osd1_ck_hd
{
    u32 all;
    struct
    {
        u32 ck_yuv                      : 24;
        u32 ck_en                       : 1;
        u32                             : 7;
    } bitc;
} reg_disp_osd1_ck_hd_t;

/*!
  the union of register reg_disp_still_x_sd
  */
typedef union reg_disp_still_x_sd
{
    u32 all;
    struct
    {
        u32 sd_still_startx             : 11;
        u32                             : 5;
        u32 sd_still_endx               : 11;
        u32                             : 5;
    } bitc;
} reg_disp_still_x_sd_t;

/*!
  the union of register reg_disp_still_y_sd
  */
typedef union reg_disp_still_y_sd
{
    u32 all;
    struct
    {
        u32 sd_still_starty             : 11;
        u32                             : 5;
        u32 sd_still_endy               : 11;
        u32                             : 5;
    } bitc;
} reg_disp_still_y_sd_t;

/*!
  the union of register reg_disp_still_stride_sd
  */
typedef union reg_disp_still_stride_sd
{
    u32 all;
    struct
    {
        u32 sd_still_stride             : 10;
        u32                             : 6;
        u32 cr_byte_sel                 : 2;
        u32                             : 2;
        u32 cb_byte_sel                 : 2;
        u32                             : 2;
        u32 y_byte_sel                  : 2;
        u32                             : 6;
    } bitc;
} reg_disp_still_stride_sd_t;

/*!
  the union of register reg_disp_rgb2y_coef
  */
typedef union reg_disp_rgb2y_coef
{
    u32 all;
    struct
    {
        u32 r2y_coef                    : 9;
        u32                             : 3;
        u32 g2y_coef                    : 10;
        u32                             : 2;
        u32 b2y_coef                    : 7;
        u32                             : 1;
    } bitc;
} reg_disp_rgb2y_coef_t;

/*!
  the union of register reg_disp_rgb2cb_coef
  */
typedef union reg_disp_rgb2cb_coef
{
    u32 all;
    struct
    {
        u32 b2cb_coef                   : 10;
        u32                             : 2;
        u32 g2cb_coef                   : 9;
        u32                             : 3;
        u32 r2cb_coef                   : 8;
    } bitc;
} reg_disp_rgb2cb_coef_t;

/*!
  the union of register reg_disp_rgb2cr_coef
  */
typedef union reg_disp_rgb2cr_coef
{
    u32 all;
    struct
    {
        u32 r2cr_coef                   : 10;
        u32                             : 2;
        u32 g2cr_coef                   : 9;
        u32                             : 3;
        u32 b2cr_coef                   : 7;
        u32                             : 1;
    } bitc;
} reg_disp_rgb2cr_coef_t;

/*!
  the union of register reg_disp_vid_decomp_cfg
  */
typedef union reg_disp_vid_decomp_cfg
{
    u32 all;
    struct
    {
        u32 config                      : 32;
    } bitc;
} reg_disp_vid_decomp_cfg_t;

/*!
  the union of register reg_disp_osd0_ck_hd
  */
typedef union reg_disp_osd0_ck_hd
{
    u32 all;
    struct
    {
        u32 ck_yuv                      : 24;
        u32 ck_en                       : 1;
        u32                             : 7;
    } bitc;
} reg_disp_osd0_ck_hd_t;

/*!
  the union of register reg_disp_rgb2yuv_yoffset
  */
typedef union reg_disp_rgb2yuv_yoffset
{
    u32 all;
    struct
    {
        u32 yoffset                     : 15;
        u32                             : 17;
    } bitc;
} reg_disp_rgb2yuv_yoffset_t;

/*!
  the union of register reg_disp_rgb2yuv_uvoffset
  */
typedef union reg_disp_rgb2yuv_uvoffset
{
    u32 all;
    struct
    {
        u32 uvoffset                    : 18;
        u32                             : 14;
    } bitc;
} reg_disp_rgb2yuv_uvoffset_t;

/*!
  the union of register reg_disp_vid_verf_cfg
  */
typedef union reg_disp_vid_verf_cfg
{
    u32 all;
    struct
    {
        u32                             : 28;
        u32 hd_dce_en                   : 1;
        u32 sd_dce_en                   : 1;
        u32                             : 2;
    } bitc;
} reg_disp_vid_verf_cfg_t;

/*!
  the union of register reg_disp_vid_horf_cfg
  */
typedef union reg_disp_vid_horf_cfg
{
    u32 all;
    struct
    {
        u32 sd_infl_thr                 : 8;
        u32 hd_infl_thr                 : 8;
        u32                             : 16;
    } bitc;
} reg_disp_vid_horf_cfg_t;

/*!
  the union of register reg_disp_layer_mix_cfg
  */
typedef union reg_disp_layer_mix_cfg
{
    u32 all;
    struct
    {
        u32 bypass_sd_vid_scaler        : 1;
        u32                             : 7;
        u32 hd_mix_alpha                : 8;
        u32                             : 4;
        u32 vid_alpha_op                : 1;
        u32                             : 3;
        u32 hd_mix_op                   : 1;
        u32                             : 3;
        u32 sd_wr_back_forbidden        : 1;
        u32                             : 3;
    } bitc;
} reg_disp_layer_mix_cfg_t;

/*!
  the union of register reg_disp_hd_size_out
  */
typedef union reg_disp_hd_size_out
{
    u32 all;
    struct
    {
        u32 hd_width_out                : 11;
        u32                             : 5;
        u32 hd_height_out               : 11;
        u32                             : 5;
    } bitc;
} reg_disp_hd_size_out_t;

/*!
  the union of register reg_disp_sd_size_out
  */
typedef union reg_disp_sd_size_out
{
    u32 all;
    struct
    {
        u32 sd_width_out                : 11;
        u32                             : 5;
        u32 sd_height_out               : 11;
        u32                             : 5;
    } bitc;
} reg_disp_sd_size_out_t;

/*!
  the union of register reg_disp_still_stride_hd
  */
typedef union reg_disp_still_stride_hd
{
    u32 all;
    struct
    {
        u32 hd_still_stride             : 11;
        u32                             : 5;
        u32 cr_byte_sel                 : 2;
        u32                             : 2;
        u32 cb_byte_sel                 : 2;
        u32                             : 2;
        u32 y_byte_sel                  : 2;
        u32                             : 6;
    } bitc;
} reg_disp_still_stride_hd_t;

/*!
  the union of register reg_disp_hdtv_cfg
  */
typedef union reg_disp_hdtv_cfg
{
    u32 all;
    struct
    {
        u32                             : 8;
        u32 hd_vid_fmt                  : 4;
        u32                             : 20;
    } bitc;
} reg_disp_hdtv_cfg_t;

/*!
  the union of register reg_disp_hd_post_cfg
  */
typedef union reg_disp_hd_post_cfg
{
    u32 all;
    struct
    {
        u32 hd_leverage                 : 8;
        u32 hd_hp_enha                  : 8;
        u32 hd_hori_enha                : 8;
        u32 hd_shoot_chg                : 8;
    } bitc;
} reg_disp_hd_post_cfg_t;

/*!
  the union of register reg_disp_sd_post_cfg
  */
typedef union reg_disp_sd_post_cfg
{
    u32 all;
    struct
    {
        u32 sd_leverage                 : 8;
        u32 sd_hp_enha                  : 8;
        u32 sd_hori_enha                : 8;
        u32 sd_shoot_chg                : 8;
    } bitc;
} reg_disp_sd_post_cfg_t;

/*!
  the union of register reg_disp_effect_coef
  */
typedef union reg_disp_effect_coef
{
    u32 all;
    struct
    {
        u32 bright_coef                 : 8;
        u32 contrast_coef               : 8;
        u32 saturation_coef             : 8;
        u32                             : 8;
    } bitc;
} reg_disp_effect_coef_t;

/*!
  the union of register reg_disp_gra_fifo_threshold
  */
typedef union reg_disp_gra_fifo_threshold
{
    u32 all;
    struct
    {
        u32 fifo64_lo                   : 6;
        u32                             : 2;
        u32 fifo64_hi                   : 6;
        u32                             : 2;
        u32 fifo128_lo                  : 6;
        u32                             : 2;
        u32 fifo128_hi                  : 6;
        u32                             : 2;
    } bitc;
} reg_disp_gra_fifo_threshold_t;

/*!
  the union of register reg_disp_gra_scaler_ctrl
  */
typedef union reg_disp_gra_scaler_ctrl
{
    u32 all;
    struct
    {
        u32 hori_filter_en              : 1;
        u32 vert_filter_en              : 1;
        u32 hori_phase_type             : 1;
        u32 vert_phase_type             : 1;
        u32 vert_table_num              : 2;
        u32                             : 2;
        u32 hori_table_num              : 3;
        u32                             : 1;
        u32 vert_start_line_odd         : 3;
        u32 vert_start_line_even        : 3;
        u32                             : 14;
    } bitc;
} reg_disp_gra_scaler_ctrl_t;

/*!
  the union of register reg_disp_gra_scaler_hratio
  */
typedef union reg_disp_gra_scaler_hratio
{
    u32 all;
    struct
    {
        u32 hratio_int                  : 3;
        u32                             : 5;
        u32 hratio_fra                  : 12;
        u32                             : 12;
    } bitc;
} reg_disp_gra_scaler_hratio_t;

/*!
  the union of register reg_disp_gra_scaler_vratio
  */
typedef union reg_disp_gra_scaler_vratio
{
    u32 all;
    struct
    {
        u32 vratio_int                  : 3;
        u32                             : 5;
        u32 vratio_fra                  : 12;
        u32                             : 12;
    } bitc;
} reg_disp_gra_scaler_vratio_t;

/*!
  the union of register reg_disp_gra_scaler_h_start_fra
  */
typedef union reg_disp_gra_scaler_h_start_fra
{
    u32 all;
    struct
    {
        u32 h_start_fra                 : 12;
        u32                             : 20;
    } bitc;
} reg_disp_gra_scaler_h_start_fra_t;

/*!
  the union of register reg_disp_gra_scaler_v_start_fra
  */
typedef union reg_disp_gra_scaler_v_start_fra
{
    u32 all;
    struct
    {
        u32 h_start_fra_odd             : 12;
        u32                             : 4;
        u32 h_start_fra_even            : 12;
        u32                             : 4;
    } bitc;
} reg_disp_gra_scaler_v_start_fra_t;

/*!
  the union of register reg_disp_gra_ctl
  */
typedef union reg_disp_gra_ctl
{
    u32 all;
    struct
    {
        u32 hd_prog_mode                : 1;
        u32                             : 3;
        u32 reduce_fr_osd0              : 1;
        u32 reduce_fr_osd1              : 1;
        u32 reduce_fr_sub               : 1;
        u32 reduce_fr_hd_still          : 1;
        u32                             : 24;
    } bitc;
} reg_disp_gra_ctl_t;

/*!
  the union of register reg_disp_osd_scale_hsize
  */
typedef union reg_disp_osd_scale_hsize
{
    u32 all;
    struct
    {
        u32 osd_dst_hsize               : 11;
        u32                             : 5;
        u32 osd_src_hsize               : 11;
        u32                             : 5;
    } bitc;
} reg_disp_osd_scale_hsize_t;

/*!
  the union of register reg_disp_osd_scale_ratio
  */
typedef union reg_disp_osd_scale_ratio
{
    u32 all;
    struct
    {
        u32 osd_ratio_fra               : 12;
        u32                             : 4;
        u32 osd_ratio_int               : 3;
        u32                             : 13;
    } bitc;
} reg_disp_osd_scale_ratio_t;

/*!
  the union of register reg_disp_osd_alpha
  */
typedef union reg_disp_osd_alpha
{
    u32 all;
    struct
    {
        u32                             : 8;
        u32 osd_alpha_filter            : 1;
        u32 border_cfg                  : 1;
        u32                             : 18;
        u32 osd_no_filter               : 1;
        u32                             : 2;
        u32 osd_sub_mix_first           : 1;
    } bitc;
} reg_disp_osd_alpha_t;

/*!
  the union of register reg_disp_sd_wrback_addr_odd
  */
typedef union reg_disp_sd_wrback_addr_odd
{
    u32 all;
    struct
    {
        u32 sd_wrback_addr_odd          : 25;
        u32                             : 7;
    } bitc;
} reg_disp_sd_wrback_addr_odd_t;

/*!
  the union of register reg_disp_sd_base_addr_even
  */
typedef union reg_disp_sd_base_addr_even
{
    u32 all;
    struct
    {
        u32 sd_base_addr_even           : 25;
        u32                             : 7;
    } bitc;
} reg_disp_sd_base_addr_even_t;

/*!
  the union of register reg_disp_sd_wrback_ctrl
  */
typedef union reg_disp_sd_wrback_ctrl
{
    u32 all;
    struct
    {
        u32                             : 24;
        u32 sd_buffer_num               : 1;
        u32                             : 6;
        u32 sd_softctrl_en              : 1;
    } bitc;
} reg_disp_sd_wrback_ctrl_t;

/*!
  the union of register reg_disp_still_start_addr_hd
  */
typedef union reg_disp_still_start_addr_hd
{
    u32 all;
    struct
    {
        u32 hd_still_start_addr         : 25;
        u32                             : 7;
    } bitc;
} reg_disp_still_start_addr_hd_t;

/*!
  the union of register reg_disp_sub_start_addr_hd
  */
typedef union reg_disp_sub_start_addr_hd
{
    u32 all;
    struct
    {
        u32 sub_header_addr             : 25;
        u32                             : 7;
    } bitc;
} reg_disp_sub_start_addr_hd_t;

/*!
  the union of register reg_disp_osd1_start_addr_hd
  */
typedef union reg_disp_osd1_start_addr_hd
{
    u32 all;
    struct
    {
        u32 osd1_header_addr            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_osd1_start_addr_hd_t;

/*!
  the union of register reg_disp_still_start_addr_sd
  */
typedef union reg_disp_still_start_addr_sd
{
    u32 all;
    struct
    {
        u32 sd_still_start_addr         : 25;
        u32                             : 7;
    } bitc;
} reg_disp_still_start_addr_sd_t;

/*!
  the union of register reg_disp_osd0_start_addr_hd
  */
typedef union reg_disp_osd0_start_addr_hd
{
    u32 all;
    struct
    {
        u32 osd0_header_addr            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_osd0_start_addr_hd_t;

/*!
  the union of register reg_disp_video_display_info
  */
typedef union reg_disp_video_display_info
{
    u32 all;
    struct
    {
        u32                             : 5;
        u32 bot_first_flag              : 1;
        u32 progressive_flag            : 1;
        u32 frame_field_flag            : 1;
        u32                             : 2;
        u32 height_low2bits             : 2;
        u32                             : 1;
        u32 height_mid3bits             : 3;
        u32                             : 1;
        u32 height_high3bits            : 3;
        u32                             : 2;
        u32 width_low2bits              : 2;
        u32                             : 1;
        u32 width_mid3bits              : 3;
        u32                             : 1;
        u32 width_high3bits             : 3;
    } bitc;
} reg_disp_video_display_info_t;

/*!
  the union of register reg_disp_motion_pre_addr
  */
typedef union reg_disp_motion_pre_addr
{
    u32 all;
    struct
    {
        u32 motion_pre_addr             : 25;
        u32                             : 7;
    } bitc;
} reg_disp_motion_pre_addr_t;

/*!
  the union of register reg_disp_motion_cur_addr
  */
typedef union reg_disp_motion_cur_addr
{
    u32 all;
    struct
    {
        u32 motion_cur_addr             : 25;
        u32                             : 7;
    } bitc;
} reg_disp_motion_cur_addr_t;

/*!
  the union of register reg_disp_luma_pre_addr
  */
typedef union reg_disp_luma_pre_addr
{
    u32 all;
    struct
    {
        u32 luma_pre_addr               : 25;
        u32                             : 7;
    } bitc;
} reg_disp_luma_pre_addr_t;

/*!
  the union of register reg_disp_luma_cur_addr
  */
typedef union reg_disp_luma_cur_addr
{
    u32 all;
    struct
    {
        u32 luma_cur_addr               : 25;
        u32                             : 7;
    } bitc;
} reg_disp_luma_cur_addr_t;

/*!
  the union of register reg_disp_luma_next_addr
  */
typedef union reg_disp_luma_next_addr
{
    u32 all;
    struct
    {
        u32 luma_next_addr              : 25;
        u32                             : 7;
    } bitc;
} reg_disp_luma_next_addr_t;

/*!
  the union of register reg_disp_chroma_ppre_addr
  */
typedef union reg_disp_chroma_ppre_addr
{
    u32 all;
    struct
    {
        u32 chroma_ppre_addr            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_ppre_addr_t;

/*!
  the union of register reg_disp_chroma_pre_addr
  */
typedef union reg_disp_chroma_pre_addr
{
    u32 all;
    struct
    {
        u32 chroma_pre_addr             : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_pre_addr_t;

/*!
  the union of register reg_disp_chroma_cur_addr
  */
typedef union reg_disp_chroma_cur_addr
{
    u32 all;
    struct
    {
        u32 chroma_cur_addr             : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_cur_addr_t;

/*!
  the union of register reg_disp_chroma_next_addr
  */
typedef union reg_disp_chroma_next_addr
{
    u32 all;
    struct
    {
        u32 chroma_next_addr            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_next_addr_t;

/*!
  the union of register reg_disp_motion_pre_addr2
  */
typedef union reg_disp_motion_pre_addr2
{
    u32 all;
    struct
    {
        u32 motion_pre_addr2            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_motion_pre_addr2_t;

/*!
  the union of register reg_disp_motion_cur_addr2
  */
typedef union reg_disp_motion_cur_addr2
{
    u32 all;
    struct
    {
        u32 motion_cur_addr2            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_motion_cur_addr2_t;

/*!
  the union of register reg_disp_luma_pre_addr2
  */
typedef union reg_disp_luma_pre_addr2
{
    u32 all;
    struct
    {
        u32 luma_pre_addr2              : 25;
        u32                             : 7;
    } bitc;
} reg_disp_luma_pre_addr2_t;

/*!
  the union of register reg_disp_luma_cur_addr2
  */
typedef union reg_disp_luma_cur_addr2
{
    u32 all;
    struct
    {
        u32 luma_cur_addr2              : 25;
        u32                             : 7;
    } bitc;
} reg_disp_luma_cur_addr2_t;

/*!
  the union of register reg_disp_luma_next_addr2
  */
typedef union reg_disp_luma_next_addr2
{
    u32 all;
    struct
    {
        u32 luma_next_addr2             : 25;
        u32                             : 7;
    } bitc;
} reg_disp_luma_next_addr2_t;

/*!
  the union of register reg_disp_chroma_ppre_addr2
  */
typedef union reg_disp_chroma_ppre_addr2
{
    u32 all;
    struct
    {
        u32 chroma_ppre_addr2           : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_ppre_addr2_t;

/*!
  the union of register reg_disp_chroma_pre_addr2
  */
typedef union reg_disp_chroma_pre_addr2
{
    u32 all;
    struct
    {
        u32 chroma_pre_addr2            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_pre_addr2_t;

/*!
  the union of register reg_disp_chroma_cur_addr2
  */
typedef union reg_disp_chroma_cur_addr2
{
    u32 all;
    struct
    {
        u32 chroma_cur_addr2            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_cur_addr2_t;

/*!
  the union of register reg_disp_chroma_next_addr2
  */
typedef union reg_disp_chroma_next_addr2
{
    u32 all;
    struct
    {
        u32 chroma_next_addr2           : 25;
        u32                             : 7;
    } bitc;
} reg_disp_chroma_next_addr2_t;

/*!
  the union of register reg_disp_field_pic_fmt
  */
typedef union reg_disp_field_pic_fmt
{
    u32 all;
    struct
    {
        u32 field_pic_fmt               : 1;
        u32                             : 31;
    } bitc;
} reg_disp_field_pic_fmt_t;

/*!
  the union of register reg_disp_vid_hd_vf_coef_addr
  */
typedef union reg_disp_vid_hd_vf_coef_addr
{
    u32 all;
    struct
    {
        u32 vid_hd_vf_coef_addr         : 25;
        u32                             : 7;
    } bitc;
} reg_disp_vid_hd_vf_coef_addr_t;

/*!
  the union of register reg_disp_vid_hd_hf_coef_addr
  */
typedef union reg_disp_vid_hd_hf_coef_addr
{
    u32 all;
    struct
    {
        u32 vid_hd_hf_coef_addr         : 25;
        u32                             : 7;
    } bitc;
} reg_disp_vid_hd_hf_coef_addr_t;

/*!
  the union of register reg_disp_vid_sd_vf_coef_addr
  */
typedef union reg_disp_vid_sd_vf_coef_addr
{
    u32 all;
    struct
    {
        u32 vid_sd_vf_coef_addr         : 25;
        u32                             : 7;
    } bitc;
} reg_disp_vid_sd_vf_coef_addr_t;

/*!
  the union of register reg_disp_vid_sd_hf_coef_addr
  */
typedef union reg_disp_vid_sd_hf_coef_addr
{
    u32 all;
    struct
    {
        u32 vid_sd_hf_coef_addr         : 25;
        u32                             : 7;
    } bitc;
} reg_disp_vid_sd_hf_coef_addr_t;

/*!
  the union of register reg_disp_gra_vf_coef_addr
  */
typedef union reg_disp_gra_vf_coef_addr
{
    u32 all;
    struct
    {
        u32 vf_coef_addr                : 25;
        u32                             : 7;
    } bitc;
} reg_disp_gra_vf_coef_addr_t;

/*!
  the union of register reg_disp_gra_hf_coef_addr
  */
typedef union reg_disp_gra_hf_coef_addr
{
    u32 all;
    struct
    {
        u32 hf_coef_addr                : 25;
        u32                             : 7;
    } bitc;
} reg_disp_gra_hf_coef_addr_t;

/*!
  the union of register reg_disp_vscaler_table_sel
  */
typedef union reg_disp_vscaler_table_sel
{
    u32 all;
    struct
    {
        u32 dce_table_sel               : 1;
        u32 osd_hori_table_sel          : 1;
        u32 gra_hori_table_sel          : 1;
        u32 gra_vert_table_sel          : 1;
        u32 sd_hori_table_sel           : 1;
        u32 sd_vert_table_sel           : 1;
        u32 hd_hori_table_sel           : 1;
        u32 hd_vert_table_sel           : 1;
        u32                             : 24;
    } bitc;
} reg_disp_vscaler_table_sel_t;

/*!
  the union of register reg_disp_osd_hf_coef_addr
  */
typedef union reg_disp_osd_hf_coef_addr
{
    u32 all;
    struct
    {
        u32 osd_hf_coef_addr            : 25;
        u32                             : 7;
    } bitc;
} reg_disp_osd_hf_coef_addr_t;

/*!
  the union of register reg_disp_di_enable
  */
typedef union reg_disp_di_enable
{
    u32 all;
    struct
    {
        u32 enable                      : 1;
        u32                             : 31;
    } bitc;
} reg_disp_di_enable_t;

/*!
  the union of register reg_disp_video_line_rd_cnt_max
  */
typedef union reg_disp_video_line_rd_cnt_max
{
    u32 all;
    struct
    {
        u32 line_cnt_max                : 8;
        u32                             : 24;
    } bitc;
} reg_disp_video_line_rd_cnt_max_t;


#ifdef __cplusplus
extern "C" {
#endif

void fl_disp_set_layer_onoff(disp_layer_id_t layer, BOOL b_on);
void fl_disp_st_layer_update(u32 rgn_addr,rect_vsb_t *p_rect,
                             disp_layer_id_t layer,pix_fmt_t pix_fmt,
                             BOOL b_update);
avc_video_mode_1_t fl_disp_get_sd_mode(u8 focus);
avc_video_mode_1_t fl_disp_get_hd_mode(u8 focus,u32 *p_w,u32 *p_h);
avc_video_aspect_1_t fl_disp_get_video_aspect(u8 focus);

#ifdef __cplusplus
}
#endif

#endif /* _DISP_AP_WARRIORS_REG_H */

