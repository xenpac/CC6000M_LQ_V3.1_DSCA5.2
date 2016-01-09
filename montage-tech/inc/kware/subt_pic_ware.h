/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SUBT_PIC_WARE_H__
#define __SUBT_PIC_WARE_H__

/*!
  Subtitle picture module id
  */
typedef enum
{
  /*!
    TopReal
    */
  SUBT_PIC_ID_ADT_TR = 0,
  /*!
    The maximal number  of Subtitle picture
    */
  SUBT_PIC_ID_ADT_MAX_NUM,
    /*!
    Unkown module
    */
  SUBT_PIC_UNKNOWN,
}subt_pic_module_id_t;

/*!
  Subtitle picture module cfg
  */
typedef struct
{
    /*!    
    task priority start, if is 0, means no task needed
    */
    u8 task_prio_start;
    /*!    
    task priorityend, if is 0, means no task needed
    */
    u8 task_prio_end;
    /*!    
    Is scroll, if is 1 ,means scroll
    */
    RET_CODE (*subt_is_scroll)(BOOL *flag);
    /*!    
    Is display ok, if is 1 ,display ok
    */
    RET_CODE (*osd_display_is_ok)(BOOL *flag);
    /*!    
    set current channel info
    */
    RET_CODE (*channel_info_set)(u16 *network_id, u16 *ts_id, u16 *service_id);
    /*!    
    scroll step pixel
    */
    RET_CODE (*scroll_step_pixel_set)(u8 * p_pixel_num);
    /*!    
    draw rectangle
    */
    RET_CODE (*draw_rectangle)(u32 x, u32 y, u32 width, u32 height, u32 color);
    /*!    
    draw picture
    */
    RET_CODE (*draw_picture)(u32 x, u32 y, u32 width, u32 height, 
                                       u8 *source1,u8 *source2,u32 bmpwidth,u8 scroll_type);
}subt_pic_module_cfg_t;

/*!
  Subtitle picture module priv
  */
typedef struct
{
    /*!    
    module id
    */
    u8 id;
    /*!    
    task priority start, if is 0, means no task needed
    */
    u8 task_prio_start;
    /*!    
    task priorityend, if is 0, means no task needed
    */
    u8 task_prio_end;
    /*!    
    Is scroll, if is 1 ,means scroll
    */
    RET_CODE (*subt_is_scroll)(BOOL *flag);
    /*!    
    Is display ok, if is 1 ,display ok
    */
    RET_CODE (*osd_display_is_ok)(BOOL *flag);
    /*!    
    set current channel info
    */
    RET_CODE (*channel_info_set)(u16 *network_id, u16 *ts_id, u16 *service_id);
    /*!    
    scroll step pixel
    */
    RET_CODE (*scroll_step_pixel_set)(u8 * p_pixel_num);
    /*!    
    draw rectangle
    */
    RET_CODE (*draw_rectangle)(u32 x, u32 y, u32 width, u32 height, u32 color);
    /*!    
    draw picture
    */
    RET_CODE (*draw_picture)(u32 x, u32 y, u32 width, u32 height, 
                                        u8 *source1,u8 *source2,u32 bmpwidth,u8 scroll_type);
    
}subt_pic_module_priv_t;

/*!
  Attach the subtitle module: Topreal
  \param[out] p_cam_id The attached subtitle handle
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subt_pic_adt_tr_attach(subt_pic_module_cfg_t * p_cfg, 
                                                                        u32 *p_adm_id);

/*!
  Detach  the subtitle module: Topreal
  \param[in] adm_id subtitle module id  
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subt_pic_detach(u32 adm_id);

/*!
  Initialize a specific SUBT pic module
  \param[in] adm_id SUBT module id
  \param[in] p_cfg The configuation of SUBT adapter
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subt_pic_init(u32 adm_id, subt_pic_module_cfg_t *p_cfg);

/*!
  De-Initialize a specific SUBT module
  \param[in] adm_id SUBT module id
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subtitle_pic_deinit(u32 adm_id);

/*!
  De-Initialize a specific SUBT module
  \param[in] adm_id SUBT module id
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subt_pic_open(u32 adm_id);

/*!
  De-Initialize a specific SUBT module
  \param[in] adm_id SUBT module id
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subt_pic_close(u32 adm_id);

/*!
  De-Initialize a specific SUBT module
  \param[in] adm_id SUBT module id
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subt_pic_stop(u32 adm_id);

/*!
  De-Initialize a specific SUBT module
  \param[in] adm_id SUBT module id
  \param[in] cmd 
  \param[in] p_param
  \return Return 0 for success and others for failure.
  */
extern RET_CODE subt_pic_io_ctrl(u32 adm_id, u32 cmd, void *p_param);

#endif //__ADS_WARE_H__
