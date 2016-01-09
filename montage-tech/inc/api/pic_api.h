/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __PIC_API_H_
#define __PIC_API_H_



/*!
  MUL_ERR_PIC_MODULE
  */
#define MUL_ERR_PIC_MODULE (0x8040)

/*!
  pic module not initialed.
  */
#define MUL_ERR_PIC_NOT_INIT                ((MUL_ERR_PIC_MODULE << 16) + 0001)

/*!
  pic module not initialed.
  */
#define MUL_ERR_PIC_DUP_INIT                ((MUL_ERR_PIC_MODULE << 16) + 0002)

/*!
  pic module no memory
  */
#define MUL_ERR_PIC_NO_MEM                    ((MUL_ERR_PIC_MODULE << 16) + 0003)

/*!
  pic module error parameter
  */
#define MUL_ERR_PIC_ERR_PARAM                 ((MUL_ERR_PIC_MODULE << 16) + 0004)

/*!
  pic module add filter failed.
  */
#define MUL_ERR_PIC_ADD_FILTER_FAILED          ((MUL_ERR_PIC_MODULE << 16) + 0005)

/*!
  pic module connect filter failed.
  */
#define MUL_ERR_PIC_FILTER_CONNECT_FAILED       ((MUL_ERR_PIC_MODULE << 16) + 0006)

/*!
  pic module config filter failed.
  */
#define MUL_ERR_PIC_FILTER_CFG_FAILED           ((MUL_ERR_PIC_MODULE << 16) + 0006)

/*!
  event from pic module.
  */
typedef enum
{
  /*!
    draw end.
    */
  MUL_PIC_EVT_DRAW_END = 0,  
/*!
   the Dynamic picture first frame
*/
  MUL_PIC_EVT_DRAW_START, 
  /*!
    picture is too large.
    */
  MUL_PIC_EVT_TOO_LARGE, 
  /*!
    picture is too large.
    */
  MUL_PIC_EVT_UNSUPPORT_FMT,  
  /*!
    picture size update.
    */
  MUL_PIC_EVT_SIZE_UPDATE, 
  /*!
    picture data error.
    */
  MUL_PIC_EVT_DATA_ERROR,
}mul_pic_evt_t;

/*!
  pic source get from
  */
typedef enum
{
  /*!
    get from file.
    */
  SOURCE_FILE,
  /*!
    get from net.
    */
  SOURCE_NET,
  /*!
    pic from buffer
    */
  SOURCE_BUFFER,
}source_get_t;


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
}mul_pic_chan_t;

/*!
  parameter for start chain.
  */
typedef struct
{
  /*!
    pic source from file.
    */
  u16 file_name[MAX_FILE_PATH];
  /*!
    buffer get
    */
  u32 buf_get;  
  /*!
    is pic source from file? TRUE for file, FALSE for memory.
    */
  BOOL is_file;
  /*!
    file size
    */
  u32 file_size;
  /*!
    flip mode
    */
  pic_flip_rotate_type_t flip;    
  /*!
    anim style.
    */
  rend_anim_t anim;
  /*!
    rend style.
    */
  rend_style_t style;
  /*!
    win rect
    */
  rect_t win_rect;    
  /*!
   handle
    */
  u32 handle;   
  /*!
    is thumbnail, for gif, we will only decoder one frame in thumbnail mode.
    */
  BOOL is_thumb;
}mul_pic_param_t;

/*!
  event call back
  */
typedef void(*evt_call_back)(handle_t pic_handle, u32 content, u32 para1, u32 para2);

/*!
  create chain
  */
RET_CODE mul_pic_create_chain(handle_t *p_pic_handle, const mul_pic_chan_t *p_chan); 

/*!
  create chain for pic data in mem
  */
RET_CODE mul_pic_create_buffer_chain(handle_t *p_pic_handle, const mul_pic_chan_t *p_chan);

/*!
  create chain for pic data in mem
  */
RET_CODE mul_pic_create_buffer_chain(handle_t *p_pic_handle, const mul_pic_chan_t *p_chan);

/*!
  create chain for net pic
  */
RET_CODE mul_pic_create_net_chain(handle_t *p_pic_handle, const mul_pic_chan_t *p_chan);
 
/*!
  destory chain
  */
RET_CODE mul_pic_destroy_chain(handle_t pic_handle); 

/*!
  start chain
  */
RET_CODE mul_pic_start(handle_t pic_handle, mul_pic_param_t *p_param); 

/*!
  stop chain
  */
RET_CODE mul_pic_stop(handle_t pic_handle);

/*!
  register event handle
  */
RET_CODE mul_pic_register_evt(handle_t pic_handle, evt_call_back cb);  

/*!
  unregister event handle
  */
RET_CODE mul_pic_unregister_evt(handle_t pic_handle); 

/*!
  set render region. By default is NULL.
  */
RET_CODE mul_pic_set_rend_rgn(handle_t pic_handle, void *p_rgn); 

/*!
  set dec mode. By default is frame mode.
  */
RET_CODE mul_pic_set_dec_mode(handle_t pic_handle, dec_mode_t dec_mode); 

/*!
  set scale mode. By default is smart mode.
  */
RET_CODE mul_pic_set_scale_mode(handle_t pic_handle, scale_mode_t scale_mode); 

/*!
  set animation parameters, just work when anima mode is set. By default is 20 & 10.
  */
RET_CODE mul_pic_set_anim_param(handle_t pic_handle, u32 frames, u32 frame_rate); 

/*!
  attach buffer, for memory share. By default, pic module will the malloc buffer.
  */
RET_CODE mul_pic_attach_buffer(handle_t pic_handle, void *p_buffer, u32 buffer_size); 

/*!
  detach buffer.
  */
RET_CODE mul_pic_detach_buffer(handle_t pic_handle);

/*!
  clear win rect with color.
  */
RET_CODE mul_pic_clear(handle_t pic_handle, u32 color);

/*!
  clear the specified rectangle with color.
  */
RET_CODE mul_pic_clear_rect(handle_t pic_handle, rect_t *p_rect, u32 color);

/*!
  set pic url, when get from internet
  */
RET_CODE mul_pic_set_url(handle_t pic_handle, u8 *p_url);

/*!
  animation resume.
  */
RET_CODE mul_pic_anim_resume(handle_t pic_handle);

/*!
  animation pause.
  */
RET_CODE mul_pic_anim_pause(handle_t pic_handle);

#endif //__PIC_API_H_


