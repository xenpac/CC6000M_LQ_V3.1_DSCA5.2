/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_UIO_H_
#define __AP_UIO_H_


/*!
  \file
  
  Application UIO translate hot keys from driver layer into 
  virtual keys and Notify the virtual key to UI layer
  */

/*!
  Max message number supported in AP UIO
  */
#define MAX_UIO_MSG_NUM (8)

/*!
  Invalid key
  */
#define INVALID_KEY 0xFFFF
/*!
  All supported virtual keys
  */

/*!
  UIO commands definition
  */
typedef enum
{
  /*!
    UIO task start command
    */
  AP_UIO_CMD_START = 0,
  /*!
    Front pannel display commands
    */
  AP_UIO_CMD_FP_DISPLAY,
  /*!
    fix me
    */
  AP_UIO_CMD_DISABLE_IR = SYNC_CMD,
  /*!
    fix me
    */  
  AP_UIO_CMD_ENABLE_IR,  
  /*!
    UIO end command
    */
  AP_UIO_CMD_END
} ap_uio_cmd_t;

/*!
  UIO event defintion
  */
typedef enum
{
  /*!
    UIO task start running
    */
  UIO_EVT_BEGIN = (APP_UIO << 16),
  /*!
    UIO key notify
    */
  UIO_EVT_KEY,
 /*!
    fix me
    */
  UIO_EVT_DISABLE_IR = ((APP_UIO << 16) | SYNC_EVT),
  /*!
    fix me
    */  
  UIO_EVT_ENABLE_IR
} uio_evt_t;

/*!
  Key type definition
  */
typedef enum
{
  /*!
    IRDA key
    */
  IRDA_KEY = 0,
  /*!
    Front pannel key
    */
  FP_KEY,
  /*!
    Both FP key and IRDA input is valid  
    */
  ALL_KEY_TYPE
} key_type_t;

/*!
  Parameter define for virtual key 
  */
typedef struct
{
  /*!
    Remote id
    */
  u8  usr;
  /*!
    Virtual key value
    */
  u16 v_key;
  /*!
    Virtual key value 2
    */
  u16 v_key_2;
  /*!
    Virtual key value 3
    */
  u16 v_key_3;
  /*!
    Virtual key value 4
    */
  u16 v_key_4;  
  /*!
    Key type
    */
  key_type_t type;
} v_key_t;

/*!
  Key map
  */
typedef struct
{
  /*!
    Hot key
    */
  u8 h_key;
  /*!
    Virtual key
    */
  u16 v_key;
} key_map_t;


/*!
  UIO implementation policy
  */
typedef struct
{
  /*!
    Initialize key map
    */
  void (*p_init_kep_map)(void);
  /*!
    Translate key and return key translation complete status
    \param[in] key parameter for translation 
    */
  BOOL (*p_translate_key)(v_key_t *p_key);
  /*!
    set irda led when ir push
    */
  void (*p_set_irda_led)(void);
}ap_uio_policy_t;

/*!
  Get UIO instance and return UIO instance
  \param[in] p_uio_policy UIO application implementation policy
  */
app_t *construct_ap_uio(ap_uio_policy_t *p_uio_policy);

#endif // End for __AP_UIO_H_

