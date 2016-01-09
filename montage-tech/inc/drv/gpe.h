/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GPE_H__
#define __GPE_H__

/*!
   The most significant bit being left.
  */
#define BEING_LEFT             0
/*!
   The most significant bit being right.
  */
#define BEING_RIGHT            1

/*!
   The macro is defined to determines the most significant bit is
   being left or right.
  */
#define GPE_SIGNIFICANT_BIT    BEING_LEFT

/*!
   The command of directly filling
  */
#define GPE_CMD_DIRECT_FILL    0x00000010
/*!
   The command of raster operation
  */
#define GPE_CMD_RASTER_OP      0x00000020
/*!
   The command of alpha blending
  */
#define GPE_CMD_ALPHA_BLEND    0x00000040
/*!
   The command of color key
  */
#define GPE_CMD_COLORKEY       0x00000080
/*!
   The command of clipping rectangle
  */
#define GPE_CMD_CLIP_RECT      0x00000100


/*!
   Defines raster operation type
  */
typedef enum
{
  /*!
     Set to the new value, erase the original.
    */
  GPE_ROP_SET = 0x1,
  /*!
     AND'd the new value with the original.
    */
  GPE_ROP_AND,
  /*!
     OR'd the new value with the original.
    */
  GPE_ROP_OR,
  /*!
     XOR'd the new value with the original.
    */
  GPE_ROP_XOR
}gpe_raster_op_t;


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
  u8 rop;
  /*!
     The alpha value
    */
  u8 alpha;
  /*!
     The color key
    */
  u32 colorkey;
  /*!
     The clipping rectangle
    */
  rect_t *p_cliprc;
} gpe_param_t;

/*!
   The structure is defined to descript the characteristic of GPE
  */
typedef struct
{
  /*!
     Bits pre pixel
    */
  u8 bpp;
  /*!
     The width
    */
  u16 width;
  /*!
     The height
    */
  u16 height;
  /*!
     The pitch, bytes pre scanline.
    */
  u32 pitch;
  /*!
     The buffers, vmem[0] is odd field, vmem[1] is even field.
    */
  u8 *p_vmem[2];
} gpe_desc_t;

/*!
    gpe device structure
  */
typedef struct gpe_device
{
  /*!
      device base control
    */
  void *p_base;
  /*!
      device private handle
    */
  void *p_priv;
} gpe_device_t;

/*!
   Draws a pixel with the specifies color.

   \param[in] p_dev Points to the gpe device.
   \param[in] p_desc Points to a block descriptor
   \param[in] x Specifies the x-coordinate.
   \param[in] y Specifies the y-coordinate.
   \param[in] c Specifies the color.

   \return If successful return SUCCESS, Otherwise return ERR_NOFEATURE.
  */
RET_CODE gpe_draw_pixel(gpe_device_t *p_dev,
                        gpe_desc_t *p_desc,
                        u16 x,
                        u16 y,
                        u32 c);


/*!
   Draws a horizontal line with the specifies color.

   \param[in] p_dev Points to the gpe device.
   \param[in] p_desc Points to a block descriptor
   \param[in] x Specifies the x-coordinate.
   \param[in] y Specifies the y-coordinate.
   \param[in] w Specifies the width.
   \param[in] c Specifies the color.

   \return If successful return SUCCESS, Otherwise return ERR_NOFEATURE.
  */
RET_CODE gpe_draw_h_line(gpe_device_t *p_dev,
                         gpe_desc_t *p_desc,
                         u16 x,
                         u16 y,
                         u16 w,
                         u32 c);

/*!
   Draws a vertical line with the specifies color.

   \param[in] p_dev Points to the gpe device.
   \param[in] p_desc Points to a block descriptor
   \param[in] x Specifies the x-coordinate.
   \param[in] y Specifies the y-coordinate.
   \param[in] h Specifies the height.
   \param[in] c Specifies the color.

   \return If successful return SUCCESS, Otherwise return ERR_NOFEATURE.
  */
RET_CODE gpe_draw_v_line(gpe_device_t *p_dev,
                         gpe_desc_t *p_desc,
                         u16 x,
                         u16 y,
                         u16 h,
                         u32 c);

/*!
   Puts a image from the buffer.

   \param[in] p_dev Points to the gpe device.
   \param[in] p_desc Points to a block descriptor
   \param[in] p_rc Points to the destination rectangle.
   \param[in] p_pt Points to the source left-top point on the image.
   \param[in] p_buf Specifies the buffer of the image.
   \param[in] pitch Specifies the pitch of the image.

   \return If successful return SUCCESS, Otherwise return ERR_NOFEATURE.
  */
RET_CODE gpe_put_image(gpe_device_t *p_dev,
                       gpe_desc_t *p_desc,
                       rect_t *p_rc,
                       point_t *p_pt,
                       void *p_buf,
                       u32 pitch);


/*!
   Gets a image copy to the buffer.

   \param[in] p_dev Points to the gpe device.
   \param[in] p_desc Points to a block descriptor
   \param[in] p_rc Points to the source rectangle.
   \param[in] p_pt Points to the destination left-top point on the buffer.
   \param[in] p_buf Specifies the buffer of the image.
   \param[in] pitch Specifies the pitch of the image.

   \return If successful return SUCCESS, Otherwise return ERR_NOFEATURE.
  */
RET_CODE gpe_get_image(gpe_device_t *p_dev,
                       gpe_desc_t *p_desc,
                       rect_t *p_rc,
                       point_t *p_pt,
                       void *p_buf,
                       u32 pitch);


/*!
   Puts a image with a mask (color key).

   \param[in] p_dev Points to the gpe device.
   \param[in] p_desc Points to a block descriptor
   \param[in] p_rc Points to the destination rectangle.
   \param[in] p_pt Points to the source left-top point on the image.
   \param[in] p_buf Specifies the buffer of the image.
   \param[in] pitch Specifies the pitch of the image.
   \param[in] mask Specifies the mask.

   \return If successful return SUCCESS, Otherwise return ERR_NOFEATURE.
  */
RET_CODE gpe_put_image_mask(gpe_device_t *p_dev,
                            gpe_desc_t *p_desc,
                            rect_t *p_rc,
                            point_t *p_pt,
                            void *p_buf,
                            u32 pitch,
                            u32 mask);


/*!
   Performs a bit-block transfer of the color data corresponding to a rectangle
   of pixels from the specified block into a destination block descriptor

   \param[in] p_dev Points to the gpe device.
   \param[in] p_dst_desc Points to the destination block descriptor
   \param[in] p_dst_rc Points to the destination rectangle.
   \param[in] p_src_desc Points to the source block descriptor
   \param[in] p_src_rc Points to the source rectangle.
   \param[in] p_param Points to the parameter package.

   return If successful return SUCCESS, Otherwise return ERR_NOFEATURE.
  */
RET_CODE gpe_bitblt(gpe_device_t *p_dev,
                    gpe_desc_t *p_dst_desc,
                    rect_t *p_dst_rc,
                    gpe_desc_t *p_src_desc,
                    rect_t *p_src_rc,
                    gpe_param_t *p_param);

#endif //__GPE_H__
