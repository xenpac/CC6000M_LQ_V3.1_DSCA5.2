/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CHAIN_H_
#define __CHAIN_H_


/*!
  private data length
  */
#define CHAIN_PRIVAT_DATA (640)

/*!
  state
  */
typedef enum
{
  /*!
    error state
    */
  CHAIN_UNCREATED,
  /*!
    created
    */
  CHAIN_CREATED,
  /*!
    opened
    */
  CHAIN_OPENED,
  /*!
    running
    */
  CHAIN_RUNNING,
  /*!
    paused
    */
  CHAIN_PAUSED,
  /*!
    stoped
    */
  CHAIN_STOPPED,
  /*!
    closed
    */
  CHAIN_CLOSED,
}chain_state_t;

/*!
  the base chain class
  */
typedef struct tag_chain
{
  /*!
    public base class, must be the first member
    */
  FATHER interface_t _interface;

  /*!
    private data buffer
    */
  u8 private_data[CHAIN_PRIVAT_DATA];

  /*!
    get chain's prior
    */
  PRIVATE u32 (*get_priority)(handle_t _this);

  /*!
    dispatch some thing
    */
  PRIVATE RET_CODE (*dispatch)(handle_t _this, void *p_mail);
  
  /*!
    dispatch intra cmd
    */
  PRIVATE RET_CODE (*forward_intra_cmd)(handle_t _this, ifilter_t *p_des, icmd_t *p_cmd);
  
  /*!
    get chain's state
    */
  chain_state_t (*get_state)(handle_t _this);
  
  /*!
    add filter to chain
    */
  RET_CODE (*add_filter)(handle_t _this, ifilter_t *p_filter, char *p_name);
  
  /*!
    remove filter from chain
    */
  RET_CODE (*remove_filter)(handle_t _this, ifilter_t *p_filter);
 
  /*!
    connect filters
    */
  RET_CODE (*connect)(handle_t _this, ifilter_t *p_upriver,
                              ifilter_t *p_downriver, media_format_t *p_format);

  /*!
    connect pin
    */
  RET_CODE (*connect_pin)(handle_t _this, ipin_t *p_output,
                              ipin_t *p_input, media_format_t *p_format);

  /*!
    purge. 
    */
  void (*purge)(handle_t _this);

  /*!
    open chain.
    */
  RET_CODE (*open)(handle_t _this);

  /*!
    start chain.
    */
  RET_CODE (*start)(handle_t _this);

  /*!
    pause chain.
    */
  RET_CODE (*pause)(handle_t _this);

  /*!
    resume chain.
    */
  RET_CODE (*resume)(handle_t _this);

  /*!
    stop chain.
    */
  RET_CODE (*stop)(handle_t _this);

  /*!
    close chain.
    */
  RET_CODE (*close)(handle_t _this);
}chain_t;

/*!
  create base chain parameter define
  */
typedef struct tag_chain_para
{
  /*!
    Pointer to the controller that use it. 
    */
  interface_t *p_owner;
  
  /*!
    name 
    */
  char *p_name;

  /*!
    task size
    */
  u32 stk_size;
  
  /*!
    high prio, TURE means high
    */
  BOOL high_prio;  
}chain_para_t;

/*!
  create a chain instance

  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of chain
  */
chain_t * chain_create(chain_para_t *p_para);

#endif // End for __CHAIN_H_

