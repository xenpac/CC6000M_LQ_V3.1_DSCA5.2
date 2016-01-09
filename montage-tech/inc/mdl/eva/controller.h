/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

/*!
  private data length
  */
#define CTRL_PRIVAT_DATA (64)

/*!
  the base controller class
  */
typedef struct tag_controller
{
  /*!
    public base class, must be the first member
    */
  FATHER interface_t _interface;
  /*!
    private data buffer
    */
  u8 private_data[CTRL_PRIVAT_DATA];

  /*!
    user handle
    */
  handle_t user_handle;
    
  /*!
    do some evt
    */
  PRIVATE RET_CODE (*process_evt)(handle_t _this, void *p_evt);

  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************
    
  /*!
   Virtual. call it when receive event.
   if you return TRUE, means this message was processed.
   if you return FALSE, means you hope send this message to app's process_msg handle
   if you don't implement this method, system send all of evt to app's process_msg handle
    */
  VIRTUAL BOOL (*on_process_evt)(handle_t _this, os_msg_t *p_msg);

}controller_t;


/*!
  create controller parameter define
  */
typedef struct tag_ctrl_para
{
  /*!
    fill the user id
    */
  u32 user_id;
  /*!
    user handle
    */
  handle_t user_handle;
}ctrl_para_t;

/*!
  create controller define
  */
controller_t * controller_create(controller_t *p_contrl,ctrl_para_t *p_para);

#endif // End for __CONTROLLER_H_

