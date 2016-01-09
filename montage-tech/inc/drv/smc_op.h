/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SMART_CARD_H__
#define __SMART_CARD_H__

/*! 
  define the max size of smc atr 
  */
#define SMC_ATR_MAX_LENGTH    33

/*! 
  define the min size of smc atr 
  */
#define SMC_ATR_MIN_LENGTH    2

/*!
  SMC ID
*/
typedef enum
{
  /*!
    smc dev 0
  */
  SMC0_ID = 0,
  /*!
    smc dev 1
  */
  SMC1_ID
} smc_id_t;

/*
  SMC task priority
*/
/*
typedef enum
{
    DRV_SMC0_TASK_PRIORITY = 5,
    DRV_SMC1_TASK_PRIORITY = 6
} smc_task_pri_t;
*/

/*! 
  define the protocal of smart card 
  */
typedef enum
{
  /*!
    protocol T0
    */
  SMC_PROTOCOL_T0 = 0,
  /*!
    Protocol T1
    */
  SMC_PROTOCOL_T1 = 1,
  /*!
    Protocol T14
    */
  SMC_PROTOCOL_T14 = 14,
}scard_protocol_t;

/*! 
  define the state of RW 
  */
typedef enum
{
  /*!
    Smart card read or write OK
    */
  SMC_RW_OK = 0,
  /*!
    Smart card receive procedure byte
    */
  SMC_RW_PROBYTE_IN,       
  /*!
    r/w a byte.
    */
  SMC_RW_DATABYTE_RW,
  /*!
    r/w all bytes. 
    */
  SMC_RW_DATABLOCK_RW,
  /*!
    Receiving status byte.
    */
  SMC_RW_SW_IN,
  /*!
    Invalid ack
    */
  SMC_RW_ERROR_ACK_INVALID,
  /*!
    Error command
    */
  SMC_RW_ERROR_CMD,
  /*!
    Error UNknown
    */
  SMC_RW_ERROR_UNKNOWN,
  /*!
    Error timeout
    */
  SMC_RW_ERROR_TIMEOUT,
  /*!
    Error parity
    */
  SMC_RW_ERROR_PARITY
}scard_rw_state_t;

/*! 
  state of a ATR session. 
  */
typedef enum
{
  /*!
    Send timeout
    */
  SMC_ATR_S_TIMEOUT,
  /*!
    Detect timeout
    */
  SMC_ATR_D_TIMEOUT = 16,
  /*!
    CH_TIMEOUT timout
    */
  SMC_ATR_CH_TIMEOUT,
  /*!
    parity error
    */
  SMC_ATR_PARITY_ERROR,
  /*!
    INTEGRITY_FAIL
    */
  SMC_ATR_INTEGRITY_FAIL,
  /*!
    INTEGRITY_OK
    */
  SMC_ATR_INTEGRITY_OK,
  /*!
    PARSE_ERROR
    */
  SMC_ATR_PARSE_ERROR,
  /*!
    PARSE_OK
    */
  SMC_ATR_PARSE_OK
}scard_atr_state_t;

/*! 
  state of a ATR session. 
  */
typedef enum
{
  /*!
    smart card insert 
    */
  SMC_CARD_INSERT = 0,
  /*!
    Smart card remove
    */
  SMC_CARD_REMOVE,
  /*!
    smart card reset ok 
    */
  SMC_CARD_RESET_OK,
  /*!
    Smart card init ok
    */
  SMC_CARD_INIT_OK,  
}scard_card_stat_t;

/*! 
  define scard atribution description 
  */
typedef struct
{
  /*! 
    Buffer for ATR. 
    */
  u8 *p_buf; 
  /*! 
    length of received ATR, including TS.
    */
  u8 atr_len; 
}scard_atr_desc_t;

/*! 
  define the struct of scard open state 
  */
typedef struct
{
  /*!
    smc id
    */
  smc_id_t smc_id;
  /*!
    if use the public sharing driver service, set the handle here 
    */
  void *p_drvsvc; 
  /*!
    if not use the public driver service, set this, smc op task priority
    */
  u32 smc_op_pri;
  /*!
    if not use the public driver service, set this, smc op task stack size
    */
  u32 smc_op_stksize;
  /*!
    The size of ring buffer used to reading data
    */
  u32 ring_buf_size : 16;   
  /*!
    Detect pin polarity, 0:low active, 1:high active
    */
  u32 detect_pin_pol : 1;  
  /*!
    If Auto-detect convention, 
    1:automatically update CONV_REG on TS reception, 
    0:do not update SMC_CONV_REG on TS reception
    */
  u32 convention : 1;
    /*!
    If Auto-detect convention, 
    0:decoded by inversed convention,
    1:decoded by direct convention
    */
  u32 convt_value : 1;
  /*!
    Vcc enable polarity, 0: high enable 1:low enable
    */
  u32 vcc_enable_pol : 1;
  /*!
    Physical clock selection
    0: APB
    1: 12MHz
    */
  u32 phyclk : 1;
  /*!
    if enable VCC voltage(3.3V/5V) selection on board
    0: disable, use default voltage
    1: enable
    */
  u32 vol_sel_enable : 1;     
  /*!
    set the VCC voltage control pin's current level here
    0: low
    1: high
    */
  u32 vol_sel_pin_cur_level : 1;  
  /*!
    if enable VCC voltage(3.3V/5V) selection, set the control gpio pin number here
    */
  u32 vol_sel_pin : 8;  
  /*!
    to indicate if use sharing driver service
    */  
  u32 is_sharing_drvsvc : 1;
  /*!
    IO pin mode 0: output high
    IO pin mode 1: pull-up high
    */  
  u32 iopin_mode : 1;
  /*!
    RST pin mode 0: output high
    RST pin mode 1: pull-up high
    */  
  u32 rstpin_mode : 1;
  /*!
    CLK pin mode 0: output high
    CLK pin mode 1: pull-up high
    */  
  u32 clkpin_mode : 1;
  /*!
    scard_read(..) timeout setting (ms)
    */  
  u32 read_timeout : 16;
}scard_open_t;

/*! 
  define scard operation description struct 
  */
typedef struct
{
  /*! 
    The buffer address of data from interface to smart card. 
    */
  u8 *p_buf_out;
  /*! 
    The size of data from interface to smart card. 
    */
  u32 bytes_out;
  /*! 
    The buffer address of data from smart card to interface. 
    */
  u8 *p_buf_in;
  /*! 
    The target size of data from smart card to interface. 
    */
  u32 bytes_in_target;
  /*!
    The size of actual received data from smart card.
    */
  u32 bytes_in_actual;
}scard_opt_desc_t;

/*! 
  define scard configuration struct 
  */
typedef struct
{
  /*!
    char extra waiting time
    */
  u8  ch_guard;
  /*!
    blk_guard
    */
  u8  blk_guard;
  /*!
    bit_d_rate
    */
  u8  bit_d_rate;
  /*!
    clock reference divider factor
    */
  u16 clk_ref; 
  /*!
    clk_f_rate
    */
  u16 clk_f_rate; 
  /*!
    Check time
    */
  u32 ch_time;
  /*!
    block time
    */
  u32 blk_time;
  /*!
    parity check, 0:none, 1:odd, 2:even
    */
  u32  parity_check;
  /*!
    conv_set
    */
  BOOL  conv_set;
  /*!
    read_timeout
    */
  u16  read_timeout;
}scard_config_t;

/*! 
  define the struct of scard device 
  */
typedef struct scard_device
{
  /*!
    Base handle
    */
  void *p_base;
  /*!
    Private handle
    */
  void *p_priv;
}scard_device_t;


/*! 
  scard state notify definition 
  */
typedef struct
{
  /*!
    Smart card slot index
    */
  u32 slot;
  /*!
    Smart card status
    */
  scard_card_stat_t card_stat; 
  /*!
    Smart card protocol, for Topreal CAS support
    */
  scard_protocol_t m_protocol;
  /*!
    Smart card attribute, for Topreal CAS support
  */
  scard_atr_desc_t *p_atropt;  
}scard_notify_t;

/*!
  define the scard term check struct
  */
typedef struct
{
  /*!
    protocal
    */
  u32 protocol;
  /*!
    bytes_in_target
    */
  u32 bytes_in_target;
  /*!
    bytes_in_actual
    */
  u32 bytes_in_actual;
  /*!
    p_buf_in
    */
  u8 *p_buf_in;
}scard_term_check_arg_t;

/*!
  slot working configurations
  */
typedef struct
{
  /*!
    slot index
    */
  u32 slot;
  /*!
    working reference base clock in Hz
    */
  u32 base_clk;
}scard_slot_config_t;

/*! 
    IO control command
  */
typedef enum
{
  /*!
    config smart card write function using mode 1
    */
   SMC_WRITE_FUNCTION_CFG_1,   
  /*!
    get card status, see param refer scard_card_stat_t
    */
   SMC_IO_CARD_STATUS_GET, 
  /*!
    get current card atr, param refer scard_atr_desc_t
    */
   SMC_IO_CARD_ATR_GET,    
  /*!
    get current slot interface working configurations, param see scard_slot_config_t
    */
   SMC_IO_SLOT_CONFIG_GET,   
  /*!
    change VCC voltage to another (3.3V or 5.5V)
    */
   SMC_IO_VCC_VOL_CHANGE,
  /*!
    check if support VCC voltage change control
    */
   SMC_IO_CHK_VCC_VOL_SEL,   
  /*!
    set card detect pin level
    */
   SMC_IO_SET_DETECT_VALID_LEVEL,    
  /*!
    set card convention type, parameter setting refer to follow:
    1: automatically update convention hw identification based on TS reception.
    0: manual setting mode
    */
   SMC_IO_SET_CONVENTION_MODE,
} scard_ioctrl_t;

/*! 
  scard state notify definition 
  */
typedef void (*smc_op_notify)(scard_notify_t *);

/*!
  smc term check function prototype
  */
typedef BOOL (*smc_op_term_check)(scard_term_check_arg_t *);

/*!
  smart card the 3nd part teminal checking configuration
  */
typedef struct
{
  /*!
    the 3rd part terminal checking function to be registered
    */
  smc_op_term_check func;
  /*!
    working protocol type, see scard_protocol_t
    */
  u32 protocol_type;
}scard_term_check_cfg_t;

/*!
   Active(power up) a present smart card and get the ATR.

   \param[in] p_dev the handle of device driver
   \param[in] p_atr ATR information received from smart card.

   \return Return SUCCESS for ATR reception; Others for failure.
 */
RET_CODE scard_active(scard_device_t *p_dev, scard_atr_desc_t *p_atr);

/*!
   Deactive(power down) a present smart card

   \param[in] p_dev the handle of device driver

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_deactive(scard_device_t *p_dev);

/*!
   Set configuration before operation.

   \param[in] p_dev the handle of device driver
   \param[in] p_cfg the configuration parameters to be set

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_set_config(scard_device_t *p_dev, scard_config_t *p_cfg);

/*!
   Get current configuration

   \param[in] p_dev the handle of device driver
   \param[in] p_cfg the configuration parameters to be gotten

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_get_config(scard_device_t *p_dev, scard_config_t *p_cfg);

/*!
   Reset the smart card controller.

   \param[in] p_dev the handle of device driver

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_reset(scard_device_t *p_dev);

/*!
   Transact data transfer between the smart card and the interface, 
   including transmission and reception.

   \param[in] p_dev the handle of device driver
   \param[in] p_rwopt Descript a data transfer task.

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_rw_transaction(scard_device_t *p_dev, scard_opt_desc_t *p_rwopt);

/*!
   Register the function to notify some events to outside. 

   \param[in] p_dev the handle of device driver
   \param[in] func the function to te registered

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_register_notify(scard_device_t *p_dev, smc_op_notify func);

/*!
   Register the function for outside to check if the operation is terminal 

   \param[in] p_dev the handle of device driver
   \param[in] p_config the configuration

   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_register_term_check(scard_device_t *p_dev, scard_term_check_cfg_t *p_config);

/*!
   Read data from smart card. 
   
   \param[in] p_dev the handle of device driver
   \param[in] p_buf the buffer to be read in
   \param[in] size the buffer size
   \param[out] p_actlen the actual length in reading
   
   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_read(scard_device_t *p_dev, u8 *p_buf, u32 size, u32 *p_actlen);

/*!
   Write data to smart card. 
   
   \param[in] p_dev the handle of device driver
   \param[in] p_buf the buffer to be write out
   \param[in] size the buffer size
   \param[out] p_actlen the actual length in writing
   
   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_write(scard_device_t *p_dev, u8 *p_buf, u32 size, u32 *p_actlen);

/*!
   Parse ATR from smart card. 
   
   \param[in] p_dev the handle of device driver
   \param[in] p_atr ATR data to be parsed
   \param[in] p_cfg configure parameters
   
   \return Return SUCCESS or Others for failure.
 */
RET_CODE scard_atr_parse(scard_device_t *p_dev, void *p_atr, void *p_cfg);

#endif //__SMART_CARD_H__
 
