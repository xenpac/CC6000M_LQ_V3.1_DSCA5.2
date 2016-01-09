/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SCTRL_H__
#define __SCTRL_H__
/*!
  ***************************************************************
  **  parameters for sctrl 
  ***************************************************************
  */
/*!
  channel status define 
  */
typedef enum
{
  SCTRL_CH_STATE_IDLE     = 0,
  SCTRL_CH_STATE_BUSY        ,
}sctrl_ch_state_t;

/*!
  des input mode 
  */
typedef enum 
{
  SCTRL_DES_INPUT_MODE_SINGLE = 0 ,
  SCTRL_DES_INPUT_MODE_TWO        ,
  SCTRL_DES_INPUT_MODE_ERR        ,
}sctrl_des_inputmode_t;


/*!
  sctrl channel id define
  */
typedef enum
{
  SCTRL_TIMER0           = 0,
  SCTRL_TIMER1              ,
  SCTRL_CHANNEL2            ,
  SCTRL_CHANNEL3            ,
  SCTRL_CHANNEL4            ,
  SCTRL_CHANNEL5            ,
  SCTRL_CHANNEL6            ,
  SCTRL_CHANNEL7            ,
  SCTRL_CHANNEL8            ,
  SCTRL_CHANNEL9            ,
  SCTRL_CHANNELa            ,
  SCTRL_CHANNELb            ,
  SCTRL_CHANNELc            ,
  SCTRL_CHANNELd            ,
  SCTRL_CHANNELe            ,
  SCTRL_CHANNELf            ,
  SCTRL_CHANNELERROR        ,
}sctrl_chid_t;

/*!
  crypto key type 
  * !!!!!     pls pay attention : it can not be modify. it is related with SMCU !!!!
  */
typedef enum
{
  SCTRL_KEYTYPE_FLASHKEY    = 0 ,
  SCTRL_KEYTYPE_SVC_KLD         ,
  SCTRL_KEYTYPE_SV3_KLD         ,
  SCTRL_KEYTYPE_ESMK_KLD        ,
  SCTRL_KEYTYPE_ERROR           ,
}sctrl_keytype_t;
/*!
  The structure is used to configure the 2-wire bus driver.    
  */
typedef struct sctrl_cfg
{
    /*!
        The locking mode of function call, see dev_lock_mode
      */    
    u8 lock_mode;
    /*!
        The ID of sctrl controller
      */    
    u8 sctrl_id;
} sctrl_cfg_t;
/*!
    secure conctrol master device
  */
typedef struct sctrl
{
    /*!
        base
      */
    void *p_base;
    /*!
        priv
      */
    void *p_priv;
} sctrl_t;

/*!
  reset enable 
  */
RET_CODE sctrl_reset_enable(sctrl_t *p_dev);
/*!
  reset disable 
  */
RET_CODE sctrl_reset_disable(sctrl_t *p_dev);
/*!
  channel states query
  sub_ch  : sub channel id of sctrl
  */
sctrl_ch_state_t sctrl_chstate_query(sctrl_t *p_dev, sctrl_chid_t sub_ch);
/*!
  channel trigger
  sub_ch  : sub channel id of sctrl
  */
RET_CODE sctrl_chtrig(sctrl_t *p_dev, sctrl_chid_t sub_ch);
/*!
  channel trigger for cw
  sub_ch  : sub channel id of sctrl
  pti_id  : pti_id , 0-- pti0;1--pti1
  cw_id   : 0 -- even; 1 --  odd 
  des_id  : descramble channel num
  */
RET_CODE sctrl_setcw_chtrig(sctrl_t *p_dev, sctrl_chid_t sub_ch, u8  pti_id, u8  cw_id, u8 des_id
                            ,u8 length);
       
/*!
  channel trigger for set data 
  sub_ch      : sub channel id of sctrlsecure
  p_addr_dst  : destination address 
  p_addr_src  : source address 
  length      : data length (unit 4bytes)
  */                     
RET_CODE sctrl_setdata(sctrl_t *p_dev, sctrl_chid_t sub_ch, u32 *p_addr_dst, u32 *p_addr_src
                       , u32 length);

/*!
  channel trigger for set data 
  sub_ch      : sub channel id of sctrlsecure
  key_type    : key type need to set 
  id_crypto   : crypto id  
  length      : data length (unit bytes, it should be 4bytes aligned)
  */  
RET_CODE sctrl_crypto_keyset(sctrl_t *p_dev, sctrl_chid_t sub_ch, sctrl_keytype_t key_type
                        , u32 id_crypto,u8 length);
/*!
  sctrl_attach
  */
RET_CODE sctrl_attach(char *p_name);
#endif //__SCTRL_H__


