/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SATCODX_H_
#define __AP_SATCODX_H_

/*!
  \file ap_satcodx.h
  Declare satcodx application.
  This is a base class for all satcodx applications.
*/

/*!
    Max message number supported in Satcodx APP
    */
#define AP_SATCODX_MSG_MAX_NUM 32

/*!
    Command id list
    */
typedef enum
{
  /*!
    Satcodx app start 
    */
  SATCODX_CMD_START = ASYNC_CMD,
  /*!
    Satcodx app stop 
    */
  SATCODX_CMD_STOP = SYNC_CMD,
  /*!
    flag MAX cmd
    */
  SATCODX_CMD_END
}satcodx_cmd_t;

/*!
  event id list
  the event notify ui frame work, something done
  */
typedef enum
{
  /*!
    the first event of SATCODX module,
    if create new event, you must insert it between BEGIN and END
    */
  SATCODX_EVT_BEGIN = (APP_SATCODX << 16 | ASYNC_EVT),
  /*!
    SATCODX ap update current status and tv/radio channel list
    */
  SATCODX_EVT_UPDATE_STATUS,
  /*!
    SATCODX ap exit successfully
    */
  SATCODX_EVT_QUIT,
  /*!
    the last event of satcodx module,
    if create new event, you must insert it between BEGIN and END
    */
  SATCODX_EVT_END = (APP_SATCODX << 16 | SYNC_EVT)
}satcodx_evt_t;

/*!
  Implementation policy definition 
  */
typedef struct satcodx_policy_tag
{
  /*!
    Implementation policy for application Satcodx initialization
    */
  void (*on_satcodx_init)(void *p_data);
  /*!
    Implementation policy for application Satcodx deinitialization
    */
  void (*on_satcodx_deinit)(void *p_data);
  /*!
    Implementation policy for start Satcodx
    */
  BOOL (*on_satcodx_start)(void *p_data, u32 mem_block_id);
  /*!
    Implementation policy for start Satcodx
    */
  BOOL (*on_satcodx_stop)(void *p_data);    
  /*!
    Private data for Satcodx policy
    */
  void *p_data;
}satcodx_policy_t;

/*!
  Get the instance of application Satcodx
  \param[in] p_satcodx_policy implementation policy to be set
  */
app_t *construct_ap_satcodx(satcodx_policy_t *p_satcodx_policy);

#endif // End for __AP_SATCODX_H_

