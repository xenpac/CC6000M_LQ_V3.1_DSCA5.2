/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GOOGLE_MAP_FILTER_H_
#define __GOOGLE_MAP_FILTER_H_

/*!
  google_map filter command define
  */
typedef enum tag_google_map_filter_cmd
{
  /*!
    Map cfg
    */
  GOOGLE_MAP_CFG,
  /*!
    Map up
    */
  GOOGLE_MAP_UP,
  /*!
    Map down
    */
  GOOGLE_MAP_DOWN,
  /*!
    Map left
    */
  GOOGLE_MAP_LEFT,
  /*!
    Map right
    */
  GOOGLE_MAP_RIGHT,  
  /*!
    Map zoom in
    */
  GOOGLE_MAP_ZOOM_IN,
  /*!
    Map zoom out
    */
  GOOGLE_MAP_ZOOM_OUT,
  /*!
    Map show type
    */
  GOOGLE_MAP_SHOW_TYPE,
  /*!
    Map show type
    */
  GOOGLE_MAP_CANCEL_REQUEST,  
}google_map_filter_cmd_t;

/*!
  google_map filter event define
  */
typedef enum tag_google_map_filter_evt
{
  /*!
    msg done
    */
  GOOGLE_MAP_MSG_DONE = GOOGLE_MAP_FILTER << 16,

}google_map_filter_evt_t;

/*!
  map width, height
  */
typedef struct
{
  /*!
    width
    */
  u32 width;
  /*!
    height
    */
  u32 height;
}map_size_t;

/*!
  map type
  */
typedef enum
{
  /*!
    roadmap
    */
  MAPTYPE_ROADMAP,
  /*!
    satellite
    */
  MAPTYPE_SATELLITE,
  /*!
    hybrid
    */
  MAPTYPE_HYBRID,
  /*!
    terrain
    */
  MAPTYPE_TERRAIN,
  /*!
    max
    */
  MAPTYPE_MAX,
}maptype_t;

/*!
  map image format
  */
typedef enum
{
  /*!
    img format
    */
  FORMAT_JPG,
  
}img_format_t;

/*!
  google map config
  */
typedef struct tag_google_map_cfg_t
{
  /*!
    addr
    */
  u8 addr[256];
  /*!
    zoom
    */
  u8 zoom;
  /*!
    format
    */
  img_format_t format;
  /*!
    size
    */
  map_size_t size;
  /*!
    maptype
    */
  maptype_t maptype;
  /*!
    bSensor
    */
  BOOL bSensor;
}google_map_cfg_t;

/*!
  subt_filter_create
  */
ifilter_t * google_map_filter_create(void *p_para);

#endif