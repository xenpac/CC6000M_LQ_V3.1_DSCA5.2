/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DRV_DEV_PRIV_H__
#define __DRV_DEV_PRIV_H__

/*!
  Device link item structure, this structure descript one of media data output.
  */  
typedef struct dev_stream_link_item
{
  /*!
      Next media output item
    */
  struct dev_stream_link_item *p_next;
  /*!
      Destin device handle
    */
  void *p_dev;
  /*!
      Destin device push data callback
    */  
  dev_string_push_cb_t push_cb;
  /*!
      Output meida type
    */  
  u8 media;
  /*!
      This item's output pin id
    */  
  u8 opin_id  : 4;
  /*!
      Destin device input pin id
    */  
  u8 ipin_id  : 4;
}dev_stream_link_item_t;


/*!
  Device media information
  */    
typedef struct  
{
  /*!
      Device meida capability
    */ 
  dev_media_cap_t media_cap;
  /*!
      Input pin id allocate flags
    */   
  u16 ipid_flags;
  /*!
      Output pin id allocate flags
    */   
  u16 opid_flags;
  /*!
      Device link (output) list head
    */   
  dev_stream_link_item_t  *p_list;
  /*!
      This device's push data callback
    */   
  dev_string_push_cb_t push_cb;
}dev_media_t;


/*!
    Device base structure. Each type of device base structure should follow
  */
typedef struct device_base
{
    /*!
        Next device (indicate in-seat order)
      */
    struct device_base *p_next;
    /*!
        Device handle index
      */    
    u8 indx;
    /*!
        Device reference counter
      */    
    u8 refc : 4;
    /*!
        Device status
      */    
    u8 stat : 4;
    /*!
        Device type
      */    
    u16 type;
    /*!
        Device locking mode, see dev_lock_mode
      */    
    u8 lock_mode;    
    /*!
        The flag to indicate if this device support stream media attribute
      */
    u8 stream_support;    
    /*!
        Resered for future using
      */
    u8 reserved[2];
    /*!
        Device name
      */    
    char *p_name;
    /*!
        Device media information
      */
    dev_media_t media_info;
    /*!
         Device operation protect mutex
      */    
    os_sem_t mutex;

    /*!
         Device operation protect mutex pointer
      */    
    void * p_drv_mutex;

    /*!
        Driver lld open function
      */    
    signed long (*open)(void *p_lld, void *p_param);
    /*!
        Driver lld close function
      */    
    RET_CODE (*close)(void *p_lld);
    /*!
        Driver lld I/O control function
      */    
    signed long (*io_ctrl)(void *p_lld, unsigned long cmd, unsigned long param);
    /*!
        Driver lld detach function
      */    
    void (*detach)(void *p_lld);

    /*!
        Resered for future using
      */
    u32 data[1];
} device_base_t;

/*!
    Macro used to find device handle by lld handler
  */
#define DEV_FIND_BY_LLD(TYPE, LLD_PTR) \
  (TYPE *)((unsigned int)LLD_PTR - sizeof(device_base_t) - sizeof(drv_dev_t))

/*!
    Macro used to find device id by lld handler
  */
#define DEV_ID_FIND_BY_LLD(LLD_PTR) \
  (((u32)(((device_base_t *)((unsigned int)LLD_PTR - sizeof(device_base_t)))->type)) << 16) \
  | ((u32)(((device_base_t *)((unsigned int)LLD_PTR - sizeof(device_base_t)))->indx))



#endif //__DRV_DEV_PRIV_H__

