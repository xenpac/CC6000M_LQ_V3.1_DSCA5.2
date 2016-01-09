/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __IFILTER_H_
#define __IFILTER_H_

/*
The IFilter class is an abstract class for implementing filters.
To implement a filter using this class, you must perform at least
the following steps: 

Derive a new class from IFilter. 
Include member variables that define the pins on the filter.The pins must 
  inherit from the oPin class. 
Override the pure virtual method. 
*/

/*!
  private data length
  */
#define IFILTER_PRIVAT_DATA (256)

/*!
  command define
  */
typedef struct tag_icmd
{
  /*!
    command
    */
  u32 cmd;
  /*!
    long type parameter
    */
  u32 lpara;
  /*!
    point type parameter
    */
  void *p_para;
}icmd_t;

/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_ifilter
{
  /*!
    public base class, must be the first member
    */
  FATHER interface_t _interface;
  /*!
    private data buffer
    */
  u8 private_data[IFILTER_PRIVAT_DATA];
  
  /*!
    join chain ,call after chain add filter
    */
  PRIVATE RET_CODE (*join_chain)(handle_t _this, interface_t *p_chain);

  /*!
    leave chain ,call after chain remove filter
    */
  PRIVATE RET_CODE (*leave_chain)(handle_t _this);
  
  /*!
    filter step. 
    */
  PRIVATE void (*step)(handle_t _this);
  
  /*!
    purge. 
    */
  PRIVATE void (*purge)(handle_t _this);

  /*!
    open filter
    */
  PRIVATE RET_CODE (*open)(handle_t _this);

  /*!
    start filter
    */
  PRIVATE RET_CODE (*start)(handle_t _this);
  
  /*!
    stop filter
    */
  PRIVATE RET_CODE (*stop)(handle_t _this);
  
  /*!
    close filter
    */
  PRIVATE RET_CODE (*close)(handle_t _this);

  /*!
    dispatch evt
    */
  PRIVATE RET_CODE (*dispatch_evt)(handle_t _this, void *p_ievt);

  /*!
    Enumerates the pins on this filter. Retrieves the number of pins
    */
  u32 (*enum_pins)(handle_t _this, pin_dir_t dir, ipin_t **pp_pin_list[]);
  
  /*!
    Retrieves the number of pins.
    */
  u32 (*get_pin_count)(handle_t _this);

  /*!
    Retrieves a pointer to the filter chain manager. 
    */
  RET_CODE (*get_chain)(handle_t _this, interface_t **pp_chain);

  /*!
    Retrieves the reference clock that the filter is using. 
    */
  u32 (*get_sync_counter)(handle_t _this);
  
  /*!
    Sets a reference clock for the filter. 
    */
  RET_CODE (*set_sync_counter)(handle_t _this, u32 clock);

  /*!
    get an unconnect pin.
    */
  RET_CODE (*get_unconnect_pin)(handle_t _this, pin_dir_t dir, ipin_t **pp_pin); 

  /*!
    if you need the filter has an active feature, you can use this function to
    regist your loop enter
    */
  RET_CODE (*set_active_enter)(handle_t _this, void (*act_loop)(handle_t _this));

  /*!
    do some cmd
    */
  RET_CODE (*do_command)(handle_t _this, icmd_t *p_cmd);

  /*!
    The send_message_out method send a message to user as sync mode.
    */
  RET_CODE (*send_message_out)(handle_t _this, os_msg_t *p_msg);

  /*!
    The method send a message to it's upriver filter as sync mode.
    */
  RET_CODE (*send_message_upriver)(handle_t _this, os_msg_t *p_msg);

  /*!
    The method send a message to it's downriver filter as sync mode.
    */
  RET_CODE (*send_message_downriver)(handle_t _this, os_msg_t *p_msg);

  /*!
    The method forward a message to it's thread as sync mode.
    it be called by filter only!!!!!!!!!
    */
  RET_CODE (*forward_command)(handle_t _this, icmd_t *p_cmd);
  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************

  /*!
    Virtual. call it when do open
    */
  VIRTUAL RET_CODE (*on_open)(handle_t _this);

  /*!
    Virtual. call it when do start
    */
  VIRTUAL RET_CODE (*on_start)(handle_t _this);

  /*!
    Notifies the chain that it is paused now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_pause)(handle_t _this);

  /*!
    Notifies the chain that it is resume now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_resume)(handle_t _this);
  
  /*!
    Virtual. call it when do stop
    */
  VIRTUAL RET_CODE (*on_stop)(handle_t _this);

  /*!
    Virtual. call it when do close
    */
  VIRTUAL RET_CODE (*on_close)(handle_t _this);

  /*!
    Virtual. call it when do cmd
    */
  VIRTUAL RET_CODE (*on_command)(handle_t _this, icmd_t *p_cmd);
  
  /*!
   Virtual. call it when receive event
    */
  VIRTUAL RET_CODE (*on_process_evt)(handle_t _this, os_msg_t *p_msg);

  /*!
   Virtual. Completes a pin connection.
    */
  VIRTUAL void (*on_complete_connect)(handle_t _this, ipin_t *p_pin);

  /*!
   Virtual. for some query
    */
  PURE VIRTUAL RET_CODE (*query)(handle_t _this, void *p_data);
}ifilter_t;

#endif // End for __IFILTER_H_
