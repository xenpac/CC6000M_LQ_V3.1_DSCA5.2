/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MODEM_H__
#define __MODEM_H__

  /*!
    MODEM_MAX_NAME_LENGTH
    */
#define MODEM_MAX_NAME_LENGTH               64

/*!
  modem device status
  */
typedef enum
{
  /*!
    MODEM_DEVICE_HARDWARE_INITIALIZE
    */
  MODEM_DEVICE_HARDWARE_INITIALIZE,
  /*!
    MODEM_DEVICE_COMMUNICATE_ERROR
    */
  MODEM_DEVICE_COMMUNICATE_ERROR,
  /*!
    MODEM_DEVICE_NO_SIM_CARD
    */
  MODEM_DEVICE_NO_SIM_CARD,
  /*!
    MODEM_DEVICE_NO_SIGNAL,
    */
  MODEM_DEVICE_NO_SIGNAL,
  /*!
    MODEM_DEVICE_NETWORK_UNREGISTERED
    */
  MODEM_DEVICE_NETWORK_UNREGISTERED,
  /*!
    MODEM_DEVICE_NETWORK_UNCONNECTED
    */
  MODEM_DEVICE_NETWORK_UNCONNECTED,
  /*!
    MODEM_DEVICE_NETWORK_START_DIAL
    */
  MODEM_DEVICE_NETWORK_START_DIAL,
  /*!
    MODEM_DEVICE_NETWORK_DIALING
    */
  MODEM_DEVICE_NETWORK_DIALING,
  /*!
    MODEM_DEVICE_NETWORK_CONNECTED
    */
  MODEM_DEVICE_NETWORK_CONNECTED  
}modem_device_status_t;

/*!
  modem driver configuration
  */
typedef struct
{
  /*!
        The locking mode of function call, see dev_lock_mode
      */
  u32 lock_mode;
  /*!
        Communicate device id, if use UART, 0 for UART0, 1 for UART;
      */
  u32 comm_device;
  /*!
    pppos send/recv task priority
    */
  u32 ppp_task_prio;
  /*!
    modem  task priority
    */
  u32 modem_task_prio;
}modem_cfg_t;

/*!
  modem apn information
  */
typedef struct
{
  /*!
    MCC + MNC
    */
  u32 plmn;
  /*!
    Access Point Name
    */
  u8 apn[MODEM_MAX_NAME_LENGTH];
  /*!
    User Name
    */
  u8 user[MODEM_MAX_NAME_LENGTH];
  /*!
    Password
    */
  u8 password[MODEM_MAX_NAME_LENGTH];
}modem_apn_info_t;

/*!
  modem connect config
  */
typedef struct
{
  /*!
    Access Point Name
    */
  char apn[MODEM_MAX_NAME_LENGTH];
  /*!
    dial  string
    */
  char dial[MODEM_MAX_NAME_LENGTH];
  /*!
    User Name
    */
  u8 user[MODEM_MAX_NAME_LENGTH];
  /*!
    Password
    */
  u8 password[MODEM_MAX_NAME_LENGTH];
}modem_connect_config_t;
/*!
  information customer requested
  */
typedef struct {
   /*!
      Modem model
      */
    u8 model[32];
    /*!
       International Mobile Subscriber Identification Number
       */
    u8 imsi[32];
    /*!
    Access Point Name
    */
    u8 apn[32];
    /*!
      Dial Number
      */
    u8 dial_number[32];
    /*!
     Error Message
     */
    u8 msg[32];
}modem_spec_info_t;

/*!
  Modem get current status

  \param[in] p_dev  The Modem device handle
  \param[out] status  The Modem device current status, see modem_device_status_t

  \return Return 0 for success and others for failure.
  */
RET_CODE modem_get_status(drv_dev_t *p_dev, u32 *status);

/*!
  Modem get current status

  \param[in] p_dev  The Modem device handle
  \param[out] status  The Modem device signal strength, 0-100%

  \return Return 0 for success and others for failure.
  */
RET_CODE modem_get_signal_strength(drv_dev_t *p_dev, u32 *strength);

/*!
  Modem get current status

  \param[in] p_dev  The Modem device handle
  \param[out] operator  The operator name of current SIM card registered, 
                                                  buffer length sould be MODEM_MAX_NAME_LENGTH

  \return Return 0 for success and others for failure.
  */
RET_CODE modem_get_operator_name(drv_dev_t *p_dev, s8 *name);
  
/*!
  Modem get current status

  \param[in] p_dev  The Modem device handle
  \param[in] status  The Modem connect config, see modem_connect_config_t

  \return Return 0 for success and others for failure.
  */
RET_CODE modem_connect(drv_dev_t *p_dev, modem_connect_config_t *config);

/*!
  Modem get current status

  \param[in] p_dev  The Modem device handle

  \return Return 0 for success and others for failure.
  */
RET_CODE modem_disconnect(drv_dev_t *p_dev);

/*!
  Modem set apn information

  \param[in] p_dev  The Modem device handle
  \param[in] p_info  apn information array 
  \param[in] info_count  apn information count 

  \return Return 0 for success and others for failure.
  */
RET_CODE modem_set_apn_info(drv_dev_t *p_dev, modem_apn_info_t *p_info, 
                                                                      u32 info_count);
/*!
  Modem set apn information

  \param[in] p_dev  The Modem device handle
  \param[in] p_info  specified information array 
  \param[in] info_count  apn information count 

  \return Return 0 for success and others for failure.
  */
RET_CODE modem_get_spec_info(drv_dev_t *p_dev, modem_spec_info_t *p_info, 
                                                                      u32 *info_count);

#endif //__MODEM_H__


