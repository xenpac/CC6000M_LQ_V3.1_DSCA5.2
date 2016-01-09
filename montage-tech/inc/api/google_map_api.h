/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __GOOGLE_MAP_API_H_
#define __GOOGLE_MAP_API_H_



/*!
  MUL_ERR_PIC_MODULE
  */
#define MUL_ERR_GOOGLE_MAP_MODULE (0x8040)

/*!
  pic module not initialed.
  */
#define MUL_ERR_GOOGLE_MAP_NOT_INIT                 ((MUL_ERR_GOOGLE_MAP_MODULE << 16) + 0001)

/*!
  pic module not initialed.
  */
#define MUL_ERR_GOOGLE_MAP_DUP_INIT                 ((MUL_ERR_GOOGLE_MAP_MODULE << 16) + 0002)

/*!
  pic module no memory
  */
#define MUL_ERR_GOOGLE_MAP_NO_MEM                   ((MUL_ERR_GOOGLE_MAP_MODULE << 16) + 0003)

/*!
  pic module error parameter
  */
#define MUL_ERR_GOOGLE_MAP_ERR_PARAM                ((MUL_ERR_GOOGLE_MAP_MODULE << 16) + 0004)

/*!
  pic module add filter failed.
  */
#define MUL_ERR_GOOGLE_MAP_ADD_FILTER_FAILED        ((MUL_ERR_GOOGLE_MAP_MODULE << 16) + 0005)

/*!
  pic module connect filter failed.
  */
#define MUL_ERR_GOOGLE_MAP_FILTER_CONNECT_FAILED   ((MUL_ERR_GOOGLE_MAP_MODULE << 16) + 0006)

/*!
  pic module config filter failed.
  */
#define MUL_ERR_GOOGLE_MAP_FILTER_CFG_FAILED       ((MUL_ERR_GOOGLE_MAP_MODULE << 16) + 0006)

/*!
  event from google map module.
  */
typedef enum
{
  /*!
    draw end.
    */
  MUL_MAP_EVT_DRAW_END = 0,
  /*!
    picture is too large.
    */
  MUL_MAP_EVT_TOO_LARGE,
  /*!
    picture is too large.
    */
  MUL_MAP_EVT_UNSUPPORT_FMT,

}mul_map_evt_t;


/*!
  zoom style.
  */
typedef enum
{
  /*!
    zoom in.
    */
  MAP_ZOOM_IN,
  /*!
    zoom out.
    */  
  MAP_ZOOM_OUT,
  
}map_zoom_style_t;

/*!
  move style.
  */
typedef enum
{
  /*!
    move up.
    */
  MAP_MOVE_UP,
  /*!
    move down.
    */  
  MAP_MOVE_DOWN,
  /*!
    move left.
    */  
  MAP_MOVE_LEFT,
  /*!
    move right.
    */  
  MAP_MOVE_RIGHT,
  
}map_move_style_t;


/*!
  parameter for create chain.
  */
typedef struct
{
  /*!
    stack size for chain task.
    */
  u32 task_stk_size;
  /*!
    animation task priority(if no animation, then set as 0).
    */
  u32 anim_pri;
  /*!
    animation task stack size.
    */
  u32 anim_stk_size; 
}mul_google_map_chan_t;


/*!
  fix me
  */
typedef struct tag_mul_google_map_param_t
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
    map rect
    */  
  rect_t map_rect;
  /*!
    maptype
    */  
  maptype_t maptype;
  /*!
    bSensor
    */  
  BOOL bSensor;

  
}mul_google_map_param_t;


/*!
  event call back
  */
typedef void(*map_evt_callback)(handle_t map_handle, u32 content, u32 para1, u32 para2);

/*!
  create chain
  */
RET_CODE mul_google_map_create_chain(handle_t *p_map_handle,
                                                 const mul_google_map_chan_t *p_chan); 
 
/*!
  destory chain
  */
RET_CODE mul_google_map_destroy_chain(handle_t map_handle); 

/*!
  start chain
  */
RET_CODE mul_google_map_start(handle_t map_handle, mul_google_map_param_t *p_map_cfg); 

/*!
  stop chain
  */
RET_CODE mul_google_map_stop(handle_t map_handle);

/*!
  zoom google map
  */
RET_CODE mul_google_map_zoom(handle_t map_handle, map_zoom_style_t zoom_style);

/*!
  move google map
  */
RET_CODE mul_google_map_move(handle_t map_handle, map_move_style_t move_style);

/*!
  cancel google map last request
  */
RET_CODE mul_google_map_cancel_request(handle_t map_handle);

/*!
  set map type
  */
RET_CODE mul_google_map_set_type();

/*!
  register event handle
  */
RET_CODE mul_google_map_register_evt(handle_t map_handle, map_evt_callback cb);  

/*!
  unregister event handle
  */
RET_CODE mul_google_map_unregister_evt(handle_t map_handle); 

/*!
  set render region. By default is NULL.
  */
RET_CODE mul_google_map_set_rend_rgn(handle_t map_handle, void *p_rgn); 

/*!
  attach buffer, for memory share. By default, pic module will the malloc buffer.
  */
RET_CODE mul_google_map_attach_buffer(handle_t map_handle, void *p_buffer, 
                                                 u32 buffer_size); 

/*!
  detach buffer.
  */
RET_CODE mul_google_map_detach_buffer(handle_t map_handle);

/*!
  google map clear region.
  */
RET_CODE mul_google_map_clear(handle_t map_handle, u32 color);


#endif //__GOOGLE_MAP_API_H_


