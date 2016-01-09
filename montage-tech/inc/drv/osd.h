/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __OSD_H__
#define __OSD_H__



/*!
  This structure defines OSD commands by dev_io_ctrl.
  */
typedef enum
{
  /*!
    Enable/disable OSD layer display.
    */
  OSD_CMD_SET_DISPLAY = DEV_IOCTRL_TYPE_UNLOCK + 1, 
  /*!
    Check the display state of OSD layer.
    */
  OSD_CMD_IS_DISPLAY, 
  /*!
    Enable/disable OSD anti-flicker function.
    */
  OSD_CMD_ANTIFLICKER_EN, 
  /*!
    Set the level of OSD anti-flicker.
    */
  OSD_CMD_SET_FILTER_PARAM
}osd_cmd_t;

/*!
  The supported color modes of OSD.
  */
typedef enum
{
  /*! 
    RGB 8BIT 
    */
  OSD_COLORFORMAT_RGB8BIT,
  /*! 
    RGB 4BIT 
    */
  OSD_COLORFORMAT_RGB4BIT,
  /*! 
    RGB 2BIT 
    */
  OSD_COLORFORMAT_RGB2BIT,
  /*! 
    YUV 2bit 
    */
  OSD_COLORFORMAT_YUV2BIT,
  /*! 
    YUV 4bit 
    */
  OSD_COLORFORMAT_YUV4BIT,
  /*! 
    YUV 8bit 
    */
  OSD_COLORFORMAT_YUV8BIT,
  /*! 
    YUV 4:2:2 32bit for 2 pixel 
    */
  OSD_COLORFORMAT_YUV422,
  /*! 
    YUV 8bit 
    */
  OSD_COLORFORMAT_ALUT44,
  /*! 
    YUV 16bit 
    */
  OSD_COLORFORMAT_ALUT88,
  /*! 
    RGB 8bit 
    */
  OSD_COLORFORMAT_RGB233,
  /*! 
    ARGB 16bit 
    */
  OSD_COLORFORMAT_ARGB4444,
  /*! 
    RGB 16bit 
    */
  OSD_COLORFORMAT_RGB565,
  /*! 
    ARGB 16bit 
    */
  OSD_COLORFORMAT_ARGB1555,
  /*! 
    ARGB 32bit 
    */
  OSD_COLORFORMAT_ARGB8888
} osd_color_mode_t;

/*!
  This structure describes an OSD region.
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
  osd_color_mode_t mode;
  /*! 
    Storage mode: frame or field 
    */
  BOOL is_frame;
} osd_region_desc_t;

/*!
  Initialization parameters for an OSD device. 
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
}osd_cfg_t;

/*!
  This structure defines an OSD device.
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
}osd_device_t;


/*!
  Create an OSD region. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] p_desc The pointer to the region description, including
  rectangle and color format information.
  \param[in] p_palette_in The pointer to the palette data.
  \param[out] p_handle The pointer to the region handle.
   
  \return Return SUCCESS for region creation done. 
        Return ERR_NO_RSRC for no resource for a new region.
  */
RET_CODE osd_create_region(osd_device_t *p_dev, 
           const osd_region_desc_t *p_desc, u32 *p_palette_in, u8 *p_handle);

/*! 
  Delete an OSD region. 
  The regions should be deleted one by one from the last to the first.

  \param[in] p_dev The pointer to the OSD device.

  \return Return SUCCESS for deletion done. Return ERR_STATUS for no existing region.
  */
RET_CODE osd_delete_region(osd_device_t *p_dev);

/*!
  Move an OSD region. NOTE: Two regions cannot share the same display lines. 
  Otherwise, the OSD can not display correctly.

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[in] x Left x coordinate of the region.
  \param[in] y Top y coordinate of the region.

  \return Return SUCCESS for movement done. Return ERR_STATUS for no existing region.
  */
RET_CODE osd_move_region(osd_device_t *p_dev, u32 handle, u16 x, u16 y);

/*! 
  Get the region info. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[out] p_region_desc The pointer to the region description data,
  including rectangle and color format information.

  \return Return SUCCESS if the region description is achieved.
        Return ERR_STATUS for no existing region.
  */
RET_CODE osd_get_region_desc(osd_device_t *p_dev, 
                          u32 handle, osd_region_desc_t *p_region_desc);

/*!
  Get the start address of the color data. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[out] p_odd_addr The pointer to address of odd field data.
  \param[out] p_even_addr The pointer to the address of even field data.

  \return Return SUCCESS if the region buffer address is achieved.
        Return ERR_STATUS for no existing region.
  */
RET_CODE osd_get_region_buffer(osd_device_t *p_dev, 
                         u32 handle, u32 *p_odd_addr, u32 *p_even_addr);

/*! 
  Display/Hide an OSD region. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[in] is_show TRUE to display the OSD region, otherwise to hide the OSD region.  

  \return Return SUCCESS for diplay state done. Return ERR_STATUS for no existing region.
  */
RET_CODE osd_set_region_display(osd_device_t *p_dev, u32 handle, BOOL is_show);

/*!
  Set alpha mode for an OSD region. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[in] is_plane_enable If enable the plane alpha mode.
  \param[in] is_region_enable If enable the region alpha mode.

  \return Return SUCCESS for alpha mode setting done.
        Return ERR_STATUS for no existing region.
  */
RET_CODE osd_set_region_alphamode(osd_device_t *p_dev, 
                 u32 handle, BOOL is_plane_enable, BOOL is_region_enable);

/*! 
  Set the alpha value for an OSD region. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[in] alpha Alpha value for the region.

  \return Return SUCCESS for region alpha setting done.
        Return ERR_STATUS for no existing region.
  */
RET_CODE osd_set_region_alpha(osd_device_t *p_dev, u32 handle, u8 alpha);

/*!
  Set transparent color in a region. (ONLY for color modes with palettes.) 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[in] index The palette index of the transparent color.
  \param[in] alpha The alpha level of transparency.

  \return Return SUCCESS for transparency setting done.
        Return ERR_STATUS for no existing region.
        Return ERR_NOFEATURE means current region contains no palette.
  */
RET_CODE osd_set_region_trans(osd_device_t *p_dev, 
                                    u32 handle, u32 index, u8 alpha);

/*!
  Set the palette of an OSD region

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[in] p_palette The pointer to the palette data.

  \return Return SUCCESS for palette setting done. 
        Return ERR_NOFEATURE means current region contains no palette.
  */
RET_CODE osd_set_region_palette(osd_device_t *p_dev, 
                                     u32 handle, u32 *p_palette);

/*!
  Set a palette entry of an OSD region.

  \param[in] p_dev The pointer to the OSD device.
  \param[in] handle The handle of the region.
  \param[in] p_palette The entry of the palette.
  \param[in] color The new color data for the entry.

  \return Return SUCCESS for entry setting done.
        Return ERR_NOFEATURE means current region contains no palette.
  */
RET_CODE osd_set_region_palette_entry(osd_device_t *p_dev, 
                                          u32 handle, u32 entry, u32 color);

/*!
  Set the edge weight of the OSD layer. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] top_weight Should within [0-255].
  \param[in] bot_weight Should within [0-255].
  */
void osd_set_edge_weight(osd_device_t *p_dev, u8 top_weight, u8 bot_weight);

/*!
  Set the alpha level of the OSD layer. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] alpha OSD layer alpha level.
  */
void osd_set_plane_alpha(osd_device_t *p_dev, u8 alpha);

/*!
  Enable color key. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] is_en TRUE to enalbe color key; FALSE to disable it.
  */
void osd_color_key_en(osd_device_t *p_dev, BOOL is_en);

/*!
  Set color key. 

  \param[in] p_dev The pointer to the OSD device.
  \param[in] r The R/Y component of the trasparent color.
  \param[in] g The G/Cb component of the trasparent color.
  \param[in] b The B/Cr component of the trasparent color.
  */
void osd_set_color_key(osd_device_t *p_dev, u8 r, u8 g, u8 b);

#endif //__OSD_H__

