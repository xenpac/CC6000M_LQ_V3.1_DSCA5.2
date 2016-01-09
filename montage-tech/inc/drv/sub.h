/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SUB_H__
 #define __SUB_H__


/*!
  This structure defines sub commands by dev_io_ctrl.
  */
typedef enum
{
  /*!
    Enable/disable sub layer display.
    */
  SUB_CMD_SET_DISPLAY = DEV_IOCTRL_TYPE_UNLOCK + 0, 
  /*!
    Check the display state of sub layer.
    */
  SUB_CMD_IS_DISPLAY
}sub_cmd_t;

/*!
  The supported color modes of sub layer.
  */
typedef enum
{
  /*! 
    RGB 8BIT 
    */
  SUB_COLORFORMAT_RGB8BIT,
  /*! 
    RGB 4BIT 
    */
  SUB_COLORFORMAT_RGB4BIT,
  /*! 
    RGB 2BIT 
    */
  SUB_COLORFORMAT_RGB2BIT,
  /*! 
    YUV 2bit 
    */
  SUB_COLORFORMAT_YUV2BIT,
  /*! 
    YUV 4bit 
    */
  SUB_COLORFORMAT_YUV4BIT,
  /*! 
    YUV 8BIT 
    */
  SUB_COLORFORMAT_YUV8BIT
} sub_color_mode_t;


/*!
  This structure describes a sub region.
  */
typedef struct
{
  /*! 
    The start x of the region
    */
  u16 x;
  /*! 
    The start y of the region
    */
  u16 y;
  /*! 
    The width of the region
    */
  u16 width;
  /*! 
    The height of the reigon
    */
  u16 height;
  /*! 
    The bit count of color cormat
    */
  u8 bit_count;
  /*! 
    color mode 
    */
  sub_color_mode_t mode;
  /*! 
    Storage mode: frame or field 
    */
  BOOL is_frame;
} sub_region_desc_t;

/*!
  Initialization parameters for a sub device. 
  */
typedef struct 
{
  /*!
    The buffer address for odd field color data.
    */
  u32 odd_addr;
  /*!
    The buffer address for even field color data.
    */
  u32 even_addr;
  /*!
    The buffer size of odd field color data
    */
  u32 odd_size;
  /*!
    The buffer size of even field color data
    */
  u32 even_size;
}sub_cfg_t;

/*!
  This structure defines a sub device.
  */
typedef struct 
{
  /*!
    Pointer to device head
    */
  void *p_base;
  /*!
    Pointer to private data
    */
  void *p_priv;
}sub_device_t;


/*!
  Create a sub region. 

  \param[in] p_dev The pointer to the sub device.
  \param[in] p_desc The pointer to the region description, including
  rectangle and color format information.
  \param[in] p_palette_in The pointer to the palette data.
  \param[in] p_handle The pointer to the region handle.
   
  \return Return SUCCESS for region creation done. Otherwise, region creation fails.
  */
RET_CODE sub_create_region(sub_device_t *p_dev, 
           const sub_region_desc_t *p_desc, u32 *p_palette_in, u8 *p_handle);

/*! 
  Delete a sub region. 
  The regions should be deleted one by one from the last to the first.

  \param[in] p_dev The pointer to the sub device.

  \return Return SUCCESS for deletion done.
  */
RET_CODE sub_delete_region(sub_device_t *p_dev);

/*!
  Move a sub region. NOTE: Two regions cannot share one display line. 
  Otherwise, the sub layer can not display correctly.

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[in] x Left x coordinate of the region.
  \param[in] y Top y coordinate of the region.

  \return Return SUCCESS for movement done.
  */
RET_CODE sub_move_region(sub_device_t *p_dev, u32 handle, u16 x, u16 y);

/*! 
  Get the region info. 

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[out] p_region_desc The pointer to the region description data,
  including rectangle and color format information.

  \return Return SUCCESS if the region description is achieved.
  */
RET_CODE sub_get_region_desc(sub_device_t *p_dev, 
                          u32 handle, sub_region_desc_t *p_region_desc);

/*!
  Get the start address of the color data. 

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[out] p_odd_addr The pointer to address of odd field data.
  \param[out] p_even_addr The pointer to the address of even field data.

  \return Return SUCCESS if the region buffer address is achieved.
  */
RET_CODE sub_get_region_buffer(sub_device_t *p_dev, 
                         u32 handle, u32 *p_odd_addr, u32 *p_even_addr);

/*! 
  Display/Hide a sub region. 

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[in] is_show TRUE to display the sub region, otherwise to hide the sub region.  
  */
void sub_set_region_display(sub_device_t *p_dev, u32 handle, BOOL is_show);

/*!
  Set alpha mode for a sub region. 

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[in] is_plane_enable If enable the plane alpha mode.
  \param[in] is_region_enable If enable the region alpha mode.

  \return Return SUCCESS for alpha mode setting done.
  */
RET_CODE sub_set_region_alphamode(sub_device_t *p_dev, 
                 u32 handle, BOOL is_plane_enable, BOOL is_region_enable);

/*! 
  Set the alpha value for a sub region. 

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[in] alpha Alpha value for the region.

  \return Return SUCCESS for region alpha setting done.
  */
RET_CODE sub_set_region_alpha(sub_device_t *p_dev, u32 handle, u8 alpha);

/*!
  Set transparent color in a region. (ONLY for color modes with palettes.) 

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[in] index The palette index of the transparent color.
  \param[in] alpha The alpha level of transparency.

  \return Return SUCCESS for transparency setting done.
  */
RET_CODE sub_set_region_trans(sub_device_t *p_dev, 
                                    u32 handle, u32 index, u8 alpha);

/*!
  Set the palette of a sub region

  \param[in] p_dev The pointer to the sub device.
  \param[in] handle The handle of the region.
  \param[in] p_palette The pointer to the palette data.

  \return Return SUCCESS for palette setting done.
  */
RET_CODE sub_set_region_palette(sub_device_t *p_dev, 
                                     u32 handle, u32 *p_palette);

/*!
  Set the alpha level of the sub layer. 

  \param[in] p_dev The pointer to the sub device.
  \param[in] alpha sub layer alpha level.
  */
void sub_set_plane_alpha(sub_device_t *p_dev, u8 alpha);

#endif //__SUB_H__

