/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PIC_H__
#define __PIC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  image format
  */
typedef enum 
{
  /*!
    error/unspecified type
    */
  IMAGE_FORMAT_UNKNOWN = 0,
  /*!
    jpeg file
    */
  IMAGE_FORMAT_JPEG  ,
  /*!
    gif file
    */
  IMAGE_FORMAT_GIF ,
  /*!
    png file
    */
  IMAGE_FORMAT_PNG,
  /*!
    bitmap file
    */
  IMAGE_FORMAT_BMP,    
}pdec_image_format_t;


/*!
  color space
  */
typedef enum 
{
  /*!
    error/unspecified
    */
  JPEG_CS_UNKNOWN,
  /*!
    monochrome
    */
  JPEG_CS_GRAYSCALE,
  /*!
    red/green/blue 
    */
  JPEG_CS_RGB,
  /*!
    Y/Cb/Cr (also known as YUV) 
    */
  JPEG_CS_YCBCR,
  /*!
    C/M/Y/K
    */
  JPEG_CS_CMYK,
  /*!
    Y/Cb/Cr/K
    */
  JPEG_CS_YCCK
}pdec_color_space_t;


/*!
  pic out mode
  */
typedef enum 
{
  /*!
    frame mode
    */
  PIC_FRAME_MODE = 0,  
  /*!
    field mode
    */
  PIC_FIELD_MODE ,
 
}pic_outmode_t;


/*!
  decoder mode
  */
typedef enum 
{
  /*!
    frame mode
    */
  DEC_ALL_MODE = 0, 
  /*!
    frame mode
    */
  DEC_FRAME_MODE = 1,  
  /*!
    line mode
    */
  DEC_LINE_MODE 
 
}dec_mode_t;


/*!
  picture decoder err state
  */
typedef enum 
{
  /*!
    src data err
    */
  SRC_DATA_ERR  =  1,  
  /*!
    memory lacked
    */
  MEM_LACK_ERR  =  2,
  /*!
    src data lacked
    */
  SRC_DATA_LACK  =  3,
  /*!
    unsupport color space
    */
  UNSOPPORT_PICTURE  =  4,
  /*!
   forced stop
  */
  FORCED_STOP = 5,
  
}pic_errstate_t;


/*!
  jpeg out_put format
  */
typedef enum 
{
  /*!
    JPEG_OUTFMT_YUV422
    */
  JPEG_OUTFMT_YUV422 = 0,  
  /*!
    JPEG_OUTFMT_YUV444
    */
  JPEG_OUTFMT_YUV444,
  /*!
    JPEG_OUTFMT_ARGB8888
    */
  JPEG_OUTFMT_ARGB8888,
  /*!
    JPEG_OUTFMT_ARGB1555
    */
  JPEG_OUTFMT_ARGB1555,

}jpeg_outfmt_t;


/*!
  Read header get inf
  */
typedef struct
{
  /*!
    picture width
    */
  u32 src_width;
  /*!
    picture height
    */
  u32 src_height;
  /*!
    component 
    */
  u32 comps;
  /*!
    output Color space
    */
  pdec_color_space_t color_space;
  /*!
    image format
    */
  pdec_image_format_t image_format;
  /*!
    frame numbers
    */
  u32 frame_nums;
  /*!
    daley time
    */
  u32 daleytime;
  /*!
    bits count per pixel --when GIF
    */
  u32 bbp;
  /*!
    size of per frame :for GIF
    */
  u32 dst_frm_size;  
  /*!
    dec mem size
    */
  u32 dec_size;   
} pic_info_t;

/*!
  flip & rotate.
  */
typedef enum 
{
  /*!
    no flip, no rotate.
    */
  PIC_NO_F_NO_R = 0,
  /*!
    rotate 90
    */
  PIC_R_90,
  /*!
    rotate 180
    */
  PIC_R_180,
  /*!
    rotate 270
    */
  PIC_R_270,
  /*!
    flip hori
    */
  PIC_F_H,
  /*!
    flip h & rotate 90
    */
  PIC_F_H_R_90,
  /*!
    flip verti
    */  
  PIC_F_V,
  /*!
    flip v & rotate 90
    */  
  PIC_F_V_R_90,
}pic_flip_rotate_type_t;


/*!
  Request from ui
  */
typedef struct pic_param
{
  /*!
    pic out mode
    */
  pic_outmode_t pic_out_mode;
  /*!
    decoder mode: frame ,or line, or all(all_mode just for gif)
    */
  dec_mode_t dec_mode;
  /*!
    Output format: mostly setted by decoder except jpeg
    */
  u8 output_format;
  /*!
    displayed on osd or still
    0 : on osd   1 : still
    */
  u8 disp_on_still;
  /*!
    src addr
    */
  void *p_src_buf;
  /*!
    size of src_rect
    */
  u32 src_size;
  /*!
    dest addr
    */
  void *p_dst_buf;
  /*!
    Up and down flip
    */
  pic_flip_rotate_type_t flip; 
  /*!
    numerator of width scale  
    */
  u32 scale_w_num;
  /*!
    denominator of width scale  
    */
  u32 scale_w_demo; 
  /*!
    numerator of height scale  
    */
  u32 scale_h_num;
  /*!
    denominator of height scale  
    */
  u32 scale_h_demo;
  /*!
    aligned dest addr: setted by decoder
    */
  u32 align_addr;
  /*!
    width when display: setted by decoder,in pixels
    */
  u32 disp_width;
  /*!
    height when display: setted by decoder,in pixels
    */
  u32 disp_height;
  /*!
    pitch or stride when display: setted by decoder,in pixels
    */
  u32 disp_stride;
  /*!
  for jpeg flip 
  */
  u32 disp_x;
  /*!
  for jpeg flip
  */
  u32 disp_y;


} pic_param_t;


/*!
  gif max frm num
  */
#define GIF_MAX_FRM_NUM 100


/*!
  gif_out_put_struct
  */
typedef struct gif_output_struct
{
  /*!
    pic out mode
    */
  pic_outmode_t pic_out_mode;
  /*!
    decoder mode: frame ,or line, or all(all_mode just for gif)
    */
  dec_mode_t dec_mode;
  /*!
    gif frame num
    */
  u32 frmnum;
  /*!
    gif frame size:include pdib header,pallete,data
    */
  u32 frmsize;
  /*!
    gif image height
    */
  u32 biHeight;
  /*!
    gif image width
    */
  u32 biWidth;
  /*!
    gif dec mode: if oncedecnum == n,then dec n frames once next time
    */
  u32 oncedecnum;
  /*!
    real decoded num: if realdecnum == n,then dec n frames last time
    */
  u32 realdecnum;
  /*!
    intimate decoder whether or not end
    */
  u32 b_DecEnd;
  /*!
    gif display height
    */
  u32 dispHeight;
  /*!
    gif display width
    */
  u32 dispWidth;
  /*!
    flip type
    */
  u8 flip;
  /*!
    gif iamge data 
    */
  u8 * frmdata[GIF_MAX_FRM_NUM];
  /*!
    for 8 byte align
    */
  u32 reserved;

}gif_output_t;

/*!
  pdec_ins_t
  */
typedef struct
{
  /*!
    image fmt.
    */
  pdec_image_format_t image_fmt;
  /*!
    memp
    */
  lib_memp_t *p_heap;
  /*!
    priv data
    */
  void *p_priv;
}pdec_ins_t;

/*!
  Get picture infomation

  \param[in] p_dev The handle of picture decoder (pdec) module.
  \param[in] p_data p_data The pointer to the image data.
  \param[in] data_size The image data size.
  \param[out] p_cur_info The pointer to picture information struct.
  \param[out] p_ins : instance.

  \return.
  */
RET_CODE pdec_getinfo(drv_dev_t *p_dev, unsigned char *p_data,
  unsigned int data_size, pic_info_t *p_cur_info, pdec_ins_t *p_instance);


/*!
  Set the decoder parameter or configuration. The decoder target address
  must be 8-byte aligned.
  
  \param[in] p_dev The handle of picture decoder (pdec) module.
  \param[in] p_cur_pic The pointer to decoder parameter struct.
  \param[out] p_ins : instance.

  \return.
  */
RET_CODE pdec_setinfo(drv_dev_t *p_dev, pic_param_t *p_cur_pic, pdec_ins_t *p_ins);


/*!
   Start to decode picture. Before start the decoder,pdec_getinfo and pdec_setinfo
   functions must be called.

  \param[in] p_dev The handle of picture decoder (pdec) module.  
  \param[out] p_ins : instance.

  \return.
  */
RET_CODE pdec_start(drv_dev_t *p_dev, pdec_ins_t *p_ins);


/*!
  End deocde

  \param[in] p_dev The handle of picture decoder (pdec) module.   
  \param[out] p_ins : instance.

  \return.
  */
RET_CODE pdec_stop(drv_dev_t *p_dev, pdec_ins_t *p_ins);

/*!
  End deocde

  \param[in] p_dev The handle of picture decoder (pdec) module.   
  \param[out] p_ins : instance.

  \return.
  */
RET_CODE pdec_forced_stop(drv_dev_t *p_dev, pdec_ins_t *p_ins);

/*!
  Get frame number

  \param[in] p_dev The handle of picture decoder (pdec) module.   
  \param[out] p_ins : instance.

  \return.
  */
u32 pdec_get_frmnum(drv_dev_t *p_dev, pdec_ins_t *p_ins);


/*!
  Get line data.

  \param[in] p_dev The handle of picture decoder (pdec) module.   
  \param[in] p_dstline The pointer to the target line buf. 
  \param[in] p_line_num The pointer to the number of the required num and the real decoded num. 
  \param[out] p_ins : instance.

  \return.
  */
RET_CODE pdec_get_line(drv_dev_t *p_dev,unsigned char *p_dstline,
  unsigned int *p_line_num, pdec_ins_t *p_ins);


#ifdef __cplusplus
}
#endif

#endif 
