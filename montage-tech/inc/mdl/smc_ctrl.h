/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SMC_CTRL_H_
#define __SMC_CTRL_H_

/*!
  Smart card status
  */
typedef enum
{
  /*!
    Smart card in
    */
  SMC_IN = 0,
  /*!
    Smart card out
    */
  SMC_OUT,
  /*!
    Smart card out
    */
  SMC_RESET_OK,
  /*!
    Smart card out
    */
  SMC_RESET_FAIL,
  /*!
    Smart card out
    */
  SMC_INIT_OK,
  /*!
    Smart card out
    */
  SMC_INIT_FAIL,
  /*!
    Smart card out
    */
  SMC_INFO,
  /*!
    Smart card unknown
    */
    SMC_UNKNOWN
} smc_status_t;

/*!
  Smart card slot
  */
typedef enum
{
  /*!
    TODO: ...
    */
  SMC_AUTODETECT = 0, //please dont change this one
  /*!
    TODO: ...
    */
  SMC_IRDETO_CARD,
  /*!
    TODO: ...
    */
  SMC_VIACCESS_CARD,
  /*!
    TODO: ...
    */
  SMC_NAGRA_CARD,
  /*!
    TODO: ...
    */
  SMC_SECA_CARD,
  /*!
    TODO: ...
    */
  SMC_CONAX_CARD,
  /*!
    TODO: ...
    */
  SMC_CRYPTOWORK_CARD,
  /*!
    TODO: ...
    */
  SMC_SHL_CARD,
  /*!
    TODO: ...
    */
  SMC_DREAMCRYPT_CARD,
  /*!
    TODO: ...
    */
  SMC_XCRYPTO_CARD,
  /*!
    TODO: ...
    */
  SMC_NDS_CARD,
  /*!
    TODO: ...
    */
  SMC_NAGRA2_CARD,
  /*!
    TODO: ...
    */
  SMC_TRICOLOR_CARD,
  /*!
    TODO: ...
    */
  SMC_TONGFANG_CARD,
  /*!
    TODO: ...
    */
  SMC_SUMAVISION_CARD,
  /*!
    TODO: ...
    */
  SMC_TIANBAI_CARD,
  /*!
    TODO: ...
    */
  SMC_UNKNOWN_CARD = 0xff //please dont change this one 
}smc_slot_t;

/*!
  Initialize smc ctrl module

  */
void smcctrl_init(void);

/*!
  Notify smart card status changed

  \param[in] status In or out
  \param[in] slot slot type
  \param[in] p_msg content
  \param[in] msg_len length
  */
void smcctrl_status_notify(smc_status_t status, 
  u8 slot, u8 *p_msg, u32 msg_len, u8 card_type);
/*!
  Get current smcctrl status
  */
smc_status_t smcctrl_get_status(void);

/*!
  Get current smcctrl card type
  */
u8 smcctrl_get_cardtype(void);


#endif // End for __SMC_CTRL_H_

