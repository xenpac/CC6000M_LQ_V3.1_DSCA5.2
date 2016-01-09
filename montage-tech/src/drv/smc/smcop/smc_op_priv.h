/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SMART_CARD_PRIV_H__
#define __SMART_CARD_PRIV_H__

/*! 
definition of lld_smc 
   */
typedef struct lld_smc
{
  void *p_priv;

  /*! 
  Active a present smart card.
     */
  RET_CODE (*active)(struct lld_smc *p_lld, scard_atr_desc_t *p_atr);
  /*! 
  Deactive a preset smart card.
     */
  RET_CODE (*deactive)(struct lld_smc *p_lld);
  /*! 
  Config 7816. 
     */
  RET_CODE (*set_config)(struct lld_smc *p_lld, scard_config_t *p_cfg);
  /*! 
  Get Config. 
     */
  RET_CODE (*get_config)(struct lld_smc *p_lld, scard_config_t *p_cfg);
  /*! 
  Reset the smart card controller.
     */
  RET_CODE (*reset)(struct lld_smc *p_lld);
  /*! 
  Transact data transfer between the smart card and the interface. 
     */
  RET_CODE (*rw_transaction)(struct lld_smc *p_lld, scard_opt_desc_t *p_rwopt);
  /*! 
  Tell notify function. 
     */
  RET_CODE (*register_notify)(struct lld_smc *p_lld, smc_op_notify func);
  /*!
  Register term check func. 
  */
  RET_CODE (*register_term_check)(struct lld_smc *p_lld, scard_term_check_cfg_t *p_config);
  /*!
  Read data from smart card. 
  */
  RET_CODE (*read)(struct lld_smc *p_lld, u8 *p_buf, u32 size, u32 *p_actlen);
  /*!
  Write data to smart card. 
  */
  RET_CODE (*write)(struct lld_smc *p_lld, u8 *p_buf, u32 size, u32 *p_actlen);
  /*!
  Parse ATR from smart card. 
  */
  RET_CODE (* atr_parse)(struct lld_smc *p_lld, void *p_atr, void *p_cfg);
}lld_smc_t;

#endif //__SMART_CARD_PRIV_H__
