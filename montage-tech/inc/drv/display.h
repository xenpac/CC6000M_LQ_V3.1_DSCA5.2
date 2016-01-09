/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  This structure defines the postprocessing mode
  */
typedef enum
{
  /*!
    Sharpening mode
    */
  PP_MODE_SHARPENING,
  /*!
    Adaptive mode
    */
  PP_MODE_ADAPTIVE,
  /*!
    Soft mode.
    */
  PP_MODE_SOFT,
  /*!
    NEW standard pass AE freq response
    */
  PP_MODE_STANDARD,
  /*!
    NEW default--montage prefered,look like mstar
    */
  PP_MODE_DEFAULT,
  /*!
    NEW vivid--more like ali
    */
  PP_MODE_VIVID,  
}disp_pp_mode_t;

/*!
  This structure defines the osd1 osd0 sub mix mode
  */
typedef enum
{
  /*!
    sub osd0 osd1
    */
  LAYER_ORDER_SUB_OSD1_OSD0,
  /*!
    sub osd1 osd0
    */
  LAYER_ORDER_SUB_OSD0_OSD1,
  /*!
     osd0 sub osd1
    */
  LAYER_ORDER_OSD0_OSD1_SUB,
   /*!
     osd0 osd1 sub
    */
  LAYER_ORDER_OSD0_SUB_OSD1,
  /*!
     osd1 sub osd0 
    */
  LAYER_ORDER_OSD1_OSD0_SUB,
  /*!
     osd1 osd0 sub 
    */
  LAYER_ORDER_OSD1_SUB_OSD0
  
}disp_layer_order_t;


/*!
  This structure defines the types of aspect ratio
  */
typedef enum
{
  /*!
    PanScan mode
    */
  VID_ASPECT_MODE_PANSCAN,
  /*!
    LetterBox mode
    */
  VID_ASPECT_MODE_LETTERBOX,
  /*!
    Keep the orignal video aspect ratio, centerlized.
    */
  VID_ASPECT_MODE_ORIG,
  /*!
    Auto mode
    */
  VID_ASPECT_MODE_AUTO
}disp_vid_aspect_mode_t;

/*!
  The output channels
  */
typedef enum
{
  /*!
    SD channel
    */
  DISP_CHANNEL_SD,
  /*!
    HD channel
    */
  DISP_CHANNEL_HD,
  /*!
    MAX channel
    */
  DISP_CHANNEL_MAX
}disp_channel_t;

/*!
  The supported gamma correction mode
  */
typedef enum
{
  /*!
    arc like
    */
  DISP_GAMMA_POLE_ARC,
  /*!
    S like
    */
  DISP_GAMMA_POLE_S
}disp_gamma_pole_t;

/*!
  This structure defines the layer
  */
typedef enum
{
  /*!
    background layer
    */
  DISP_LAYER_ID_BACKGROUND = 0,
  /*!
    still layer SD 1
    */
  DISP_LAYER_ID_STILL_SD,  
  /*!
    still layer HD 2
    */
  DISP_LAYER_ID_STILL_HD,  
  /*!
    video layer SD 3
    */
  DISP_LAYER_ID_VIDEO_SD,
  /*!
    video layer HD 4
    */
  DISP_LAYER_ID_VIDEO_HD,
  /*!
    osd layer 0  5
    */
  DISP_LAYER_ID_OSD0,
  /*!
    osd layer 1  6
    */
  DISP_LAYER_ID_OSD1,
  /*!
    sub layer 7
    */
  DISP_LAYER_ID_SUBTITL,
  /*!
    dummy sub layer for UI, fake layer 8
    */
  DISP_LAYER_ID_DUMMY_SUB,    
  /*!
    dummy osd layer for UI, fake layer 9
    */
  DISP_LAYER_ID_DUMMY_OSD,
  /*!
    osd0 vscaler layer, fake layer 10
    */
  DISP_LAYER_ID_OSD0_VS,
  /*!
    osd1 vscaler layer, fake layer
    */
  DISP_LAYER_ID_OSD1_VS,
  /*!
    sub vscaler layer, fake layer
    */
  DISP_LAYER_ID_SUBTITL_VS,
  /*!
    max 
    */
  DISP_LAYER_ID_MAX
} disp_layer_id_t;

///TODO: DAC need to remove to platform related .h
/*!
  The indext video DACs
  */
typedef enum
{
  /*!
    Video DAC 0
    */
  DAC_0 = 0,
  /*!
    Video DAC 1
    */
  DAC_1,
  /*!
    Video DAC 2
    */
  DAC_2,
  /*!
    Video DAC 3
    */
  DAC_3
}dac_index_t;

/*!
  This structure defines the supported output formats of video DAC.
  */
typedef enum
{
  /*!
    Red component
    */
  DAC_R = 0,
  /*!
    Green component
    */
  DAC_G,
  /*!
    Blue component
    */
  DAC_B,
  /*!
    Y component
    */
  DAC_Y,
  /*!
    U component
    */
  DAC_U,
  /*!
    V component
    */
  DAC_V,
  /*!
    Composite video
    */
  DAC_CVBS,
  /*!
    Luminance component
    */
  DAC_LUMA,
  /*!
    Chrominace componet
    */
  DAC_CHROMA
}dac_fmt_t;


/*!
  Define cvbs dac group
  */
typedef enum
{
  /*!
    CVBS Group 0 use dac 0
    */
  CVBS_GRP0 = 0,
  /*!
    CVBS Group 1 use dac 1
    */
  CVBS_GRP1,
  /*!
    CVBS Group 2 use dac 2
    */
  CVBS_GRP2,
  /*!
    CVBS Group 3 use dac 3
    */
  CVBS_GRP3,
    /*!
    CVBS Group MAX
    */
  CVBS_GRP_MAX
}cvbs_dacgrp_t;

/*!
  Define component dac group
  */
typedef enum
{
  /*!
    Component Group 0 SD use dac 1,2,3, yuv,gbr
    */
  COMPONENT_GRP0 = 0,
  /*!
    Component Group 1 HD use dac 1,2,3,yuv
    */
  COMPONENT_GRP1,
  /*!
    Component Group 2 HD use dac 1,0,2,yuv, fpga
    */
  COMPONENT_GRP2,
  /*!
    Component Group 3 HD use dac 1,3,2,yuv
    */
  COMPONENT_GRP3,
   /*!
    Component Group 4 HD use dac 1,2,0,yuv
    */
  COMPONENT_GRP4,
   /*!
    Component Group 5 HD use dac 1,0,3,yuv
    */
  COMPONENT_GRP5,
   /*!
    Component Group 6 HD use dac 1,3,0,yuv
    */
  COMPONENT_GRP6,
   /*!
    Component Group 7 HD use dac 0,1,2,yuv
    */
  COMPONENT_GRP7,
   /*!
    Component Group 8 HD use dac 0,1,3,yuv
    */
  COMPONENT_GRP8,
   /*!
    Component Group 9 HD use dac 0,2,1,yuv
    */
  COMPONENT_GRP9,
   /*!
    Component Group 10 HD use dac 0,2,3,yuv
    */
  COMPONENT_GRP10,
   /*!
    Component Group 11 HD use dac 0,3,1,yuv
    */
  COMPONENT_GRP11,
   /*!
    Component Group 12 HD use dac 0,3,2,yuv
    */
  COMPONENT_GRP12,
   /*!
    Component Group 13 HD use dac 2,0,1,yuv
    */
  COMPONENT_GRP13,
   /*!
    Component Group 14 HD use dac 2,0,3,yuv
    */
  COMPONENT_GRP14, 
  /*!
    Component Group 15 HD use dac 2,1,0,yuv
    */
  COMPONENT_GRP15,
   /*!
    Component Group 16 HD use dac 2,1,3,yuv
    */
  COMPONENT_GRP16,
   /*!
    Component Group 17 HD use dac 2,3,0,yuv
    */
  COMPONENT_GRP17,
   /*!
    Component Group 18 HD use dac 2,3,1,yuv
    */
  COMPONENT_GRP18,
   /*!
    Component Group 19 HD use dac 3,0,1,yuv
    */
  COMPONENT_GRP19,
   /*!
    Component Group 20 HD use dac 3,0,2,yuv
    */
  COMPONENT_GRP20,
   /*!
    Component Group 21 HD use dac 3,1,0,yuv
    */
  COMPONENT_GRP21,
   /*!
    Component Group 22 HD use dac 3,1,2,yuv
    */
  COMPONENT_GRP22,
   /*!
    Component Group 23 HD use dac 3,2,0,yuv
    */
  COMPONENT_GRP23,
   /*!
    Component Group 24 HD use dac 3,2,1,yuv
    */
  COMPONENT_GRP24,
  /*!
    Component Group 25 SD use dac 1,0,2, yuv,gbr
    */
  COMPONENT_GRP25,    
  /*!
    Component Group MAX 
    */
  COMPONENT_GRP_MAX
}component_dacgrp_t;

/*!
  Define Svideo dac group
  */
typedef enum
{
  /*!
    Svideo Group 0 SD use dac 2,3
    */
  SVIDEO_GRP0 = 0,
  /*!
    Svideo Group 1 SD use dac 1,2
    */
  SVIDEO_GRP1,
  /*!
    Svideo Group 2
    */
  SVIDEO_GRP2,
  /*!
    Svideo Group 3
    */
  SVIDEO_GRP3,
  /*!
    Svideo Group MAX
    */
  SVIDEO_GRP_MAX
}svideo_dacgrp_t;

/*!
define the sigal type of DAC output
*/
typedef enum 
{
    DISP_DAC_CVBS_RGB = 0,           //CVBS + RGB输出,dac0:cvbs, dac1:sd G, dac2:sd B, dac3:sd R
    DISP_DAC_SING_CVBS,           //!<@~english dac0:cvbs, dac1~dac3:off @~chinese dac0:cvbs, dac1~dac3:关闭
    DISP_DAC_SING_CVBS_LOW_POWER,           //!<@~english dac3:cvbs, dac0~dac2:off @~chinese dac3:cvbs, dac0~dac2:关闭
    DISP_DAC_DULE_CVBS,        //!<@~english dac1:cvbs, dac0~dac2:off, dac3:cvbs @~chinese dac1:cvbs, dac0~dac2:off, dac3:cvbs
    DISP_DAC_CVBS_SVIDEO,  //CVBS + S_VIDEO, dac0:cvbs, dac1~2:s-video, dac3:off
    DISP_DAC_SVIDEO_CVBS,  //S_VIDEO + CVBS, dac0:off, dac1~2:s-video, dac3:cvbs
    DISP_DAC_DULE_CVBS_SVIDEO,  //S_VIDEO + 2CVBS, dac0:cvbs, dac1~2:s-video, dac3:cvbs
    DISP_DAC_CVBS_YPBPR_SD,  //CVBS + sd YPBPR输出，dac0:cvbs, dac1:sd y, dac2:sd pb, dac3:sd pr
    DISP_DAC_CVBS_YPBPR_HD,  //CVBS + hd YPBPR输出，dac0:cvbs, dac1:hd y, dac2:hd pb, dac3:hd pr
    DISP_DAC_TYPE_BUTT
}disp_vdac_type_t;

/*!
  Initialization parameters for an layer. 
  */
typedef struct 
{
  /*! 
    The start address of idle layer buffer for odd field data.
    */
  u32 odd_mem_start;
  /*! 
    The start address of idle layer buffer for even field data.
    */
  u32 even_mem_start;
  /*! 
    The end address of idle layer buffer for odd field data.
    */
  u32 odd_mem_end;
  /*! 
    The end address of idle layer buffer for even field data.
    */
  u32 even_mem_end;
  /*! 
    The osd filter_param, anti-flicking parameters, from 1~7.
    */
  u32 antiflicker_level;
}layer_cfg_t;

/*!
  Initialization misc using buffer. 
  */
typedef struct 
{
  /*! 
    The di address.
    */
  u32 di_addr;
  /*! 
    The di size.
    */
  u32 di_size;
  /*! 
    The write back address.
    */
  u32 sd_wb_addr;
  /*! 
    The write back size.
    */
  u32 sd_wb_size;
  /*! 
    The write back field num.
    */
  u32 sd_wb_field_no;
}misc_buf_cfg_t;

/*!
  Initialization video pp cfg. 
  */
typedef struct 
{
  /*! 
    The bright,satua,etc.
    */
  u32 pp_value;
  /*! 
    The hd shoot.
    */
  u32 shoot_value;
  /*! 
    pp mode.
    */
  disp_pp_mode_t pp_mode;
}pp_cfg_t;


/*!
  Initialization parameters for the still picture display layer
  */
typedef struct
{
  /*!
    The buffer config for still layer.
    */    
  layer_cfg_t *p_cfg;
  /*! 
    The display mode for still picture: TRUE for frame and FALSE for field
    */
  BOOL b_frame;
  /*!
    The byte order for data storage of sill picture
    */
  BOOL b_endian;
  /*!
    The horizontal intervals of still picture hw value between 0~15
    */
  u16 int_h;
  /*!
    The vertical intervals of still picture  hw value between 0~15
    */
  u16 int_v;  
}still_cfg_t;

/*!
  This structure defines a display plane for 9-picture.
  */
typedef struct
{
  /*! 
    The left x coordinate of 9-picture plane. 
    */
  u16 x;
  /*! 
    The top y coordinate of 9-picture plane
    */
  u16 y;
  /*! 
    The width of 9-picture each unit. It should not exceed 180.
    */
  u8  width;
  /*! 
    The height of 9-picture each unit. It should not exceed 144 
    */
  u8  height;
  /*! 
    The width of horizontal intervals between 9 pic units
    */
  u16 interval_h;
  /*! 
    The width of vertical intervals between 9 pic units
    */
  u16 interval_v;
}disp_pic_9_t;

/*!
  This structure defines a frame of video scaled in 9 picture mode.
  */
typedef struct
{
  /*! 
    The width after scaling.
    */
  u16 width;
  /*! 
    The height after scaling.
    */
  u16 height;
  /*! 
    The offset to the width of a 9 picture unit. 
    */
  u16 x_delta;
  /*! 
    The offset to the height of a 9 picture unit. 
    */  
  u16 y_delta;
}multipic_unit_scale_t;

/*!
  Initialization parameters for the display layers
  */
typedef struct
{
  /*! 
    pointer to the still sd cfg
    */
  still_cfg_t *p_still_sd_cfg;
  /*! 
    pointer to the still sd cfg
    */
  still_cfg_t *p_still_hd_cfg;
  /*! 
    pointer to the osd0 cfg
    */
  layer_cfg_t *p_osd0_cfg;
  /*! 
    pointer to the osd1 cfg
    */
  layer_cfg_t *p_osd1_cfg;
  /*! 
    pointer to the sub cfg
    */
  layer_cfg_t *p_sub_cfg;
  /*! 
    pointer to the dummy osd cfg
    */
  layer_cfg_t *p_dummy_osd_cfg;
  /*! 
    pointer to the osd0 vscaler cfg
    */
  layer_cfg_t *p_osd0_vscale_cfg;
  /*! 
    pointer to the osd1 vscaler cfg
    */
  layer_cfg_t *p_osd1_vscale_cfg;
  /*! 
    pointer to the sub vscaler cfg
    */
  layer_cfg_t *p_sub_vscale_cfg;  
  /*! 
    pointer to misc buffer cfg
    */
  misc_buf_cfg_t misc_buf_cfg;  
  /*! 
    use vscaler or not
    if set to false, b_vscale_osd0,b_vscale_osd1,b_vscale_sub will be considered as false;
    */
  BOOL b_vscale;
  /*! 
    osd 0 use vscaler or not
    */
  BOOL b_vscale_osd0;
  /*! 
    osd 1 use vscaler or not
    */
  BOOL b_vscale_osd1;
  /*! 
    sub use vscaler or not
    */
  BOOL b_vscale_sub;
  /*! 
    use osd hardware hscale or not
    */
  BOOL b_osd_hscle;
  /*! 
    use osd hardware vscale or not
    */
  BOOL b_osd_vscle;
  /*! 
    use di enable
    */
  BOOL b_di_en;
  /*! 
    ui default graphic size
    */
  rect_size_t *p_gra_default_size;
  /*!
    if use the public sharing driver service, set the handle here 
    */
  void *p_drvsvc;  
  /*!
    if not use the public sharing driver service, set this, hdmi notify task priority
    */
  u32 task_prio;
  /*!
    if not use the public sharing driver service, set this, hdmi notify task size
    */  
  u32 stack_size;
  /*!
    The video pp cfg
    */  
  pp_cfg_t pp_cfg;
  /*!
    The resource lock type
    */
  u32 lock_type;
  /*! 
    The start address of coef tab.
    */
  u32 p_coeff_start;
  /*!
    The background color
    */
  u32 background_color;
  /*!
    The hdcp onoff
    */
  BOOL b_hdcp_on;
  /*!
    The frequence_respond cfg
    */
  BOOL b_freq_resp;
  /*!
    sd write back mode, should set as 444 to pass frequence respond;
    can set as 422 when memory is limited
    */
  BOOL b_wrback_422;  
  /*!
    user close prescale, horizontal scale ratio can't be less than 1/7, 
    and vertical scale ratio can't be less than 1/4
    */
  BOOL b_unuse_prescale;  
  /*!
     shared memory used by av and ap cpu
  */
  u32 av_ap_shared_mem;
  /*!
     shared memory size
  */
  u32 shared_mem_size;
  /*!
    Uninitialized in uboot
  */
  BOOL b_uboot_uninit;
}disp_cfg_t;

/*!
  This structure defines display common info.
  */
typedef struct
{
  /*! 
    The video channel number
    */
  u32 ch_num;
  /*! 
    The vertical scaler buffer usage
    */
  BOOL use_vscale_buf;  
  /*! 
    The eid info
    */
  u32 *p_hdmi_eid;  
  /*! 
    The hdmi connect status
    */
  BOOL is_hdmi_connected;  
}disp_common_info_t;

/*!
  The vertical layer and true layer offset.
  */
#define DISP_VS_LAYER_OFFSET  (DISP_LAYER_ID_OSD0_VS - DISP_LAYER_ID_OSD0)

/*!
  Get video output channel numbers
  
  \param[in] p_dev The handle of display module.
  \param[out] p_ch_no The pointer to the total channel numbers
  */
RET_CODE disp_get_vout_ch_num(void *p_dev, u8 *p_ch_num);

/*!
  Set display graphic size
  
  \param[in] p_dev The handle of display module.
  \param[in] p_size The pointer to graphic defaut size
  */
RET_CODE disp_set_graphic_size(void *p_dev, rect_size_t *p_size);

/*!
  Set the video TV system
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] fmt The video TV system to set
  */
RET_CODE disp_set_tv_sys(void *p_dev, disp_channel_t ch, disp_sys_t fmt);

/*!
  Get current video TV system
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[out] p_fmt The pointer to  current video TV system
  */
RET_CODE disp_get_tv_sys(void *p_dev, disp_channel_t ch, disp_sys_t *p_fmt);

/*!
  Set the aspect ratio of screem
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] aspect The screem aspect ratio to set
  */
RET_CODE disp_set_aspect_ratio(void *p_dev, disp_channel_t ch, aspect_ratio_t aspect);

/*!
  Get current aspect setting of screem
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[out] p_aspect The pointer to current aspect setting
  */
RET_CODE disp_get_aspect_ratio(void *p_dev, disp_channel_t ch, aspect_ratio_t *p_aspect);


/*!
  Video brightness setup.
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] percent The percentage of full brightness to set
  */
RET_CODE disp_set_bright(void *p_dev, disp_channel_t ch, u8 percent);

/*!
  Video saturation setup.
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] percent The percentage of full saturation to set
  */
RET_CODE disp_set_sature(void *p_dev, disp_channel_t ch, u8 percent);

/*!
  Video contrast setup.
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] percent The percentage of full contrast to set
  */
RET_CODE disp_set_contrast(void *p_dev, disp_channel_t ch, u8 percent);

/*!
  display gamma correction setup.
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] pole The mode for gamma correction.
  \param[in] coef The gamma coefficient * 10.
  */
RET_CODE disp_set_gama(void *p_dev, disp_channel_t ch, disp_gamma_pole_t pole, u8 coef);

/*!
  display gamma correction onoff.
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] b_on gamma on/off.
  */
RET_CODE disp_gama_onoff(void *p_dev, disp_channel_t ch, BOOL b_on);
                                        
/*!
  Get region true size in seperate layers
  
  \param[in] p_dev  The handle of disp module.
  \param[in] layer   layerId
  \param[in] p_region   The pointer to the region 
  \param[out] p_align  The pointer to the required buffer alignment in bytes
  \param[out] p_size   The pointer to the required buffer size in bytes
  */
RET_CODE disp_calc_region_size(void *p_dev,
    disp_layer_id_t layer, void *p_region, u32 *p_align, u32 *p_size);

/*!
  Attach region to the plane.
  
  \param[in] p_dev  The handle of display module.
  \param[in] layer   layerId
  \param[in] p_region  The pointer to the region 
  \param[in] p_pos   The pointer to the dest postion in the plane
  \param[in] p_buf   The pointer to the region buf
  */
RET_CODE disp_layer_add_region(void *p_dev,
    disp_layer_id_t layer, void *p_region, point_t *p_pos, void *p_buf);

/*!
  Attach region to the plane.
  
  \param[in] p_dev  The handle of display module.
  \param[in] layer   layerId
  \param[in] p_region  The pointer to the region 
  \param[in] p_pos   The pointer to the dest postion in the plane
  \param[in] p_buf   The pointer to the region buf
  \param[in] p_buf_vs   The pointer to the region vs buf
  */
RET_CODE disp_layer_add_region_ex(void *p_dev,
    disp_layer_id_t layer, void *p_region, point_t *p_pos, void *p_buf, void *p_buf_vs);

/*!
  Move region in the plane.
  
  \param[in] p_dev  The handle of display module.
  \param[in] p_region  The pointer to the region 
  \param[in] p_pos   The pointer to the dest postion in the plane
  */
RET_CODE disp_layer_move_region(void *p_dev, void *p_region, point_t *p_pos);

/*!
  dettach the region from the plane.
  
  \param[in] p_dev  The handle of display module.
  \param[in] layer   layerId
  \param[in] p_region   The pointer to the region 
  */
RET_CODE disp_layer_remove_region(void *p_dev, disp_layer_id_t layer, void *p_region);

/*!
  update a region using graphic engine.
  
  \param[in] p_dev  The handle of display module.
  \param[in] p_region   The pointer to the region 
  \param[in] p_rect   The rect based on region 
  */
RET_CODE disp_layer_update_region(void *p_dev, 
                                      void *p_region,
                                      rect_t *p_rect);

/*!
  switch regions buffers from dummy osd layer to osd/sub layer.
  Only support src dummy osd, dst osd/sub, 
  Only support that the region buffers are malloced not configured
  
  \param[in] p_dev  The handle of display module.
  \param[in] p_src_rgn   The pointer to the src region 
  \param[in] p_dst_rgn   The pointer to the dst region 
  */
RET_CODE disp_layer_switch_region(void *p_dev, 
                                      void *p_src_rgn,
                                      void *p_dst_rgn);

/*!
  set display layer show or hide
  
  \param[in] p_dev  The handle of display module.
  \param[in] layer   layerId
  \param[in] b_on    TRUE for show, FALSE for hide.
  */
RET_CODE disp_layer_show(void *p_dev, disp_layer_id_t layer, BOOL b_on);

/*!
  Query the display state of display layer
  
  \param[in] p_dev   The handle of display module.
  \param[in] layer   layerId
  */
BOOL disp_layer_is_show(void *p_dev, disp_layer_id_t layer);

/*!
  color key on or off
  
  \param[in] p_dev  The handle of disp module.
  \param[in] layer   layerId
  \param[in] b_on       enable or disable color key
  */
RET_CODE disp_layer_color_key_onoff(void *p_dev, disp_layer_id_t layer, BOOL b_on);

/*!
  set the color key
  
  \param[in] p_dev  The handle of disp module.
  \param[in] layer   layerId
  \param[in] color  colorkey value
  */
RET_CODE disp_layer_set_color_key(void *p_dev, disp_layer_id_t layer, u32 color);

/*!
  set the plane alpha onoff
  
  \param[in] p_dev  The handle of disp module.
  \param[in] layer   layerId
  \param[in] b_on   enable or disable color alpha
  */
RET_CODE disp_layer_alpha_onoff(void *p_dev, disp_layer_id_t layer, BOOL b_on);

/*!
  set the plane alpha
  
  \param[in] p_dev  The handle of disp module.
  \param[in] layer   layerId
  \param[in] alpha  plane alpha value
  */
RET_CODE disp_layer_set_alpha(void *p_dev, disp_layer_id_t layer, u32 alpha);

/*!
  get the plane alpha
  
  \param[in] p_dev  The handle of disp module.
  \param[in] layer   layerId
  \param[out] p_alpha  the pointer to the current plane alpha value
  */
RET_CODE disp_layer_get_alpha(void *p_dev, disp_layer_id_t layer, u32 *p_alpha);

/*!
  Clip the layer. Only valid when display scaling is active.
  
  \param[in] p_dev The handle of display module.
  \param[in] layer   layerId
  \param[in] p_clip  the pointer to the remain rectangle after clipped.
  */
RET_CODE disp_layer_set_clip(void *p_dev, disp_layer_id_t layer, rect_t *p_clip);

/*!
  Set the aspect ratio of video plane
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] aspect The video aspect ratio to set
  */
RET_CODE disp_vid_set_aspect_mode(void *p_dev, disp_channel_t ch, disp_vid_aspect_mode_t aspect);

/*!
  Get current aspect setting of video plane
  
  \param[in] p_dev     The handle of video module.
  \param[in] ch The channel of output.
  \param[out] p_aspect  The pointer to current aspect setting
  */
RET_CODE disp_vid_get_aspect_mode(void *p_dev, 
                                        disp_channel_t ch, 
                                        disp_vid_aspect_mode_t *p_aspect);

/*!
  Scale down the source video plane to the specific rectangle
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] p_rect_dst the pointer to rectangle that will be fill with video plane
  */
RET_CODE disp_vid_zoom_in(void *p_dev, disp_channel_t ch, rect_t *p_rect_dst);

/*!
  Scale up the specific rectangle of source video to full screen
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] p_rect_src the pointer to cut part of video plane
  */
RET_CODE disp_vid_zoom_out(void *p_dev, disp_channel_t ch, rect_t *p_rect_src);
/*!
  Scale up the specific rectangle of source video to a specific size
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  \param[in] p_rect_src the pointer to cut part of video plane
  \param[in] p_rect_dst the pointer to rectangle that will be fill with video plane
  */
RET_CODE disp_vid_cut_zoom(void *p_dev, disp_channel_t ch, rect_t *p_rect_src, rect_t *p_rect_dst);
/*!
  Exit from zoom out/in 
  
  \param[in] p_dev The handle of display module.
  \param[in] ch The channel of output.
  */
RET_CODE disp_vid_zoom_exit(void *p_dev, disp_channel_t ch);

/*!
  On or Off specific CVBS output.
  
  \param[in] p_dev  The handle of display module.
  \param[in] idx  the index of CVBS
  \param[in] b_on   TRUE for on, FALSE for off
  */
RET_CODE disp_cvbs_onoff(void *p_dev, cvbs_dacgrp_t idx,  BOOL b_on);

/*!
  On or Off dac output.
  
  \param[in] p_dev  The handle of display module.
  \param[in] idx  the index of dac
  \param[in] b_on   TRUE for on, FALSE for off
  */
RET_CODE disp_dac_onoff(void *p_dev, dac_index_t idx,  BOOL b_on);

/*!
  On or Off specific componen output.
  
  \param[in] p_dev  The handle of display module.
  \param[in] idx  the index of componen
  \param[in] b_on   TRUE for on, FALSE for off
  */
RET_CODE disp_component_onoff(void *p_dev, component_dacgrp_t idx, BOOL b_on);

/*!
  Set the colorspace of component output.
  
  \param[in] p_dev  The handle of display module.
  \param[in] idx  the index of componen
  \param[in] type the type of colorspace
  */
RET_CODE disp_component_set_type(void *p_dev, component_dacgrp_t idx, colorspace_t type);

/*!
  On or Off specific svideo output.
  
  \param[in] p_dev  The handle of display module.
  \param[in] idx  the index of svideo
  \param[in] b_on   TRUE for on, FALSE for off
  */
RET_CODE disp_svideo_onoff(void *p_dev, svideo_dacgrp_t idx, BOOL b_on);
/*!
  On or Off hdmi output.
  
  \param[in] p_dev  The handle of display module.
  \param[in] b_on   TRUE for on, FALSE for off
  */
RET_CODE disp_hdmi_onoff(void *p_dev, BOOL b_on);
/*!
  User di onoff.
  
  \param[in] p_dev  The handle of display module.
  \param[in] b_on   TRUE for on, FALSE for off
  */
RET_CODE disp_user_di_onoff(void *p_dev, BOOL b_on);
/*!
  Set pp mode.
  
  \param[in] p_dev  The handle of display module.
  \param[in] mode  pp mode
  */
RET_CODE disp_set_postprocess_mode(void *p_dev, disp_pp_mode_t mode);

/*!
   This function calculate the step to adjust the position of still picture unit
   in 9 picture.
   
   \param[in] p_disp The handle of display module.
   \param[out] unit_scale the pointer to rectangle that fill 9pic pg.
  */
RET_CODE disp_set_9pic_unit_scale(void *p_dev, multipic_unit_scale_t *multi_scale);

/*!
  Get display common info
  
  \param[in] p_dev The handle of display module.
  \param[out] p_info The pointer to the display common info
  */
RET_CODE disp_get_common_info(void *p_dev, disp_common_info_t *p_info);
/*!
  Used in concerto, create a region, and malloc memory, but without attaching on hw layer
  */
RET_CODE disp_create_virtual_region(void *p_dev, u32 *p_rgn_hdl, 
            u32 width, u32 height, pix_fmt_t fmt);
 /*!
  Used in concerto, delete a region, and free memory.
  */
RET_CODE disp_destroy_virtual_region(void *p_dev, u32 rgn_hdl);
/*!
  Set sub osd1 osd0 layer mix param
  
  \param[in] p_dev  The handle of display module.
  \param[in] layer_order layer order 
  */
RET_CODE disp_set_osd_layer_order(void *p_dev, disp_layer_order_t layer_order);
/*!
  set dac mode, used in concerto.
  */
RET_CODE disp_set_dacmode(void *p_dev, disp_vdac_type_t dacMode);
/*!
  venc pass frequency config check
  */
RET_CODE disp_venc_check(void *p_dev);
/*!
  usr close di
  */
RET_CODE disp_usr_close_di(void *p_dev, BOOL close_di);
#ifdef __cplusplus
}
#endif

#endif


