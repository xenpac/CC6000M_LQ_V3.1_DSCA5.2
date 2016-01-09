/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __IPIN_H_
#define __IPIN_H_

/*!
  private data length
  */
#define IPIN_PRIVAT_DATA (192)

/*!
  pin dir
  */
typedef enum
{
  /*!
    error
    */
  UNKNOWN_PIN,
  /*!
    input dir
    */
  INPUT_PIN,
  /*!
    output dir
    */
  OUTPUT_PIN,
}pin_dir_t;

/*!
  define working mode
  */
typedef enum tag_stream_mode
{
  /*!
    unknown
    */
  STREAM_MODE_UNKNOWN = 0,
  /*!
    push mode
    */
  STREAM_MODE_PUSH,
  /*!
    pull mode
    */
  STREAM_MODE_PULL,
}stream_mode_t;


/*!
  the base pin integrate feature of interface and class
  */
typedef struct tag_ipin
{
  /*!
    public base class, must be the first member
    */
  FATHER interface_t _interface;
  
  /*!
    private data buffer
    */
  u8 private_data[IPIN_PRIVAT_DATA];
  
  //DECLARE_INTERFACE(ImtFilter, IF_PIN);

  /*!
    Determines whether the pin is connected to another pin
    */
  BOOL (*is_connected)(handle_t _this);
  
  /*!
    Determines whether the pin is active. active include running and stop
    */
  BOOL (*is_active)(handle_t _this);
  
  /*!
    Retrieves the pin that is connected to this pin. 
    */
  interface_t * (*get_connected)(handle_t _this);
  
  /*!
    Retrieves the pin's name. 
    */
  u8 * (*get_name)(handle_t _this);

  /*!
    Retrieves the pin's name. 
    */
  pin_dir_t (*get_dir)(handle_t _this);

  /*!
    check media format is received.
    */
  BOOL (*is_supported_format)(handle_t _this, media_format_t *p_format);

  /*!
    Set the pin's media format. 
    */
  RET_CODE (*add_supported_media_format)(handle_t _this, media_format_t *p_format);

  /*!
    Retrieves the media format enum by supported.
    */
  u32 (*list_media_format)(handle_t _this, media_format_t **p_list);
  
  /*!
    Retrieves the belong to filter. 
    */
  interface_t * (*get_filter)(handle_t _this);

  /*!
    Retrieves the pin's state. 
    */
  PRIVATE u32 (*get_state)(handle_t _this);
  
  /*!
    connect with another pin.
    */
  PRIVATE RET_CODE (*connect)(handle_t _this, interface_t *p_peer, media_format_t *p_sample);
  
  /*!
    disconnect. 
    */
  PRIVATE RET_CODE (*disconnect)(handle_t _this);

  /*!
    purge. 
    */
  PRIVATE void (*purge)(handle_t _this);

  /*!
    open pin.
    */
  PRIVATE RET_CODE (*open)(handle_t _this);

  /*!
    start pin.
    */
  PRIVATE RET_CODE (*start)(handle_t _this);

  /*!
    stop pin.
    */
  PRIVATE RET_CODE (*stop)(handle_t _this);

  /*!
    close pin.
    */
  PRIVATE RET_CODE (*close)(handle_t _this);

  /*!
    The begin_flush method begins a flush operation
    applications should not call this method.
    */
  PRIVATE RET_CODE (*begin_flush)(handle_t _this);

  /*!
    The end_flush method ends a flush operation
    applications should not call this method.
    */
  PRIVATE RET_CODE (*end_flush)(handle_t _this);

  /*!
    The send_message_out method send a message to user as sync mode.
    */
  RET_CODE (*send_message_out)(handle_t _this, os_msg_t *p_msg);

  /*!
    The send_message_to_filter method send a message to it's filter as sync mode.
    */
  RET_CODE (*send_message_to_filter)(handle_t _this, os_msg_t *p_msg);

  /*!
    The attach interface method use attach child provide interface.
    */
  RET_CODE (*attach_interface)(handle_t _this, char *p_name, void *p_interface);  

  /*!
    Retrieves the belong to filter. 
    */
  RET_CODE (*get_interface)(handle_t _this, char *p_name, void **p_interface);
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************

  /*!
    call it after connected. Virtual. 
    */
  VIRTUAL RET_CODE (*on_connect)(handle_t _this,
                                  interface_t *p_peer, media_format_t *p_sample);
  
  /*!
    call it after disconnected. Virtual. 
    */
  VIRTUAL RET_CODE (*on_disconnect)(handle_t _this);

  /*!
    Notifies the pin that the filter is opend now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_open)(handle_t _this);

  /*!
    Notifies the pin that the filter is runing now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_start)(handle_t _this);

  /*!
    Notifies the pin that the filter is stop now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_stop)(handle_t _this);

  /*!
    Notifies the pin that the filter is closed now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_close)(handle_t _this);
}ipin_t;

#endif // End for __IPIN_H_

