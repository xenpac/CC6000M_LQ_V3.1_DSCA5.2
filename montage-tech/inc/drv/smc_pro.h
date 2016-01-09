/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SMART_PRO_CARD_H__
#define __SMART_PRO_CARD_H__


/*! 
  Maximum number of historical bytes
  */
#define ATR_MAX_HISTORICAL  15

/*!
  Maximun number of protocols
  */
#define ATR_MAX_PROTOCOLS 7

/*!
  Maximum number of interface bytes per protocol
  */
#define ATR_MAX_IB    4

/*!
  smart card woring parameters
  */
typedef struct
{
  /*!
    the reference represenation of Fi
    */
  u32 FI;
  /*!
    the reference represenation of Di
    */
  u32 DI;
  /*!
    Fi
    */
  u32 Fi;
  /*!
    Di
    */
  u32 Di;
  /*!
    Max working clock in Hz
    */
  u32 clk_max;
  /*!
    char wait integer, CWT = (2 ^ cwi + 11)etu
    */
  u32 cwi;
  /*!
    block wait integer, BWT=2 ^ bwi * 960 * 372 / f s + 11etu
    */
  u32 bwi;
  /*!
    the extra wait time between two char on the different deriction
    */
  u32 N;
  /*!
    working wait time integeter, between card and IFN, wwt is less than 960 * wi * (Fi/f) etu
    */
  u32 wi;
  /*!
    working wait time, is less than 960 * wi * (Fi/f) etu
    */
  u32 wwt;
  /*!
    the all protocols this card support, bit validation
    */
  u16 SupportedProtocolTypes;
  /*!
    the flag to state if support special mode
    */
  u8 SpecificMode;
  /*!
    the special protocol type
    */
  u8 SpecificType;
  /*!
    to state if the special type is changable
    */
  u8 SpecificTypeChangable;
  /*!
    the current working protocol type
    */
  u8 WorkingType;
  /*!
    Card's maximum block size for (T=1)
    */
  u8 IFSC;
  /*!
    "II" variables integer represenation
    */
  u8 IInt;
  /*!
    "PI1" variables integer represenation
    */
  u8 PInt1;
  /*!
    "PI2" variables integer represenation
    */
  u8 PInt2;
  /*!
    Redundancy Check used CRC=1 LRC=0 (T=1)
    */
  u8 RC;
  /*!
    parity, 0:not use, 1:odd, 2:even
    */
  u8 parity : 4;
  /*!
    convention setting, 0:direct convention, 1:inverse convention
    */
  u8 convention : 4;
}smc_working_param_t;

/*!
  smart card prococol configurations
  */
typedef struct
{
  /*!
    ATR length
    */
  u8 length;
  /*!
    TS
    */
  u8 TS;
  /*!
    T0
    */
  u8 T0;
  /*!
    interface byte descripter
    */
  struct
  {
    /*!
      the value of this interface byte
      */
    u8 value;
    /*!
      the flag to state if this interface byte is present
      */
    u8 present;
  }
  /*!
    interface byte
    */  
  ib[ATR_MAX_PROTOCOLS][ATR_MAX_IB], 
  /*!
    TCK byte
    */
  TCK;
  /*!
    the count of protocol should be supported
    */
  u8 pn;
  /*!
    the history byte
    */
  u8 hb[ATR_MAX_HISTORICAL];
  /*!
    the count of history byte
    */
  u8 hbn;
  /*!
    the card work parameters
    */
  smc_working_param_t work_param;
}scard_pro_cfg_t;


/*!
   Active a present smart card.


   \return Return SUCCESS for ATR reception; Others for failure.
 */
RET_CODE scard_pro_active(scard_atr_desc_t *p_atr);


/*!
   Parse ATR of the card.
 */
RET_CODE scard_pro_atr_parse(scard_atr_desc_t *p_atr, scard_pro_cfg_t *p_cfg);


/*!
   Cofigure the interface based on the parameters from ATR, including PTS.

   \param[in] p_cfg Parameters to configure the smart card device.
 */
RET_CODE scard_pro_interface_cfg(smc_working_param_t *p_param);


/*!
   Deactive a preset smart card.
 */
RET_CODE scard_pro_deactive(void);

/*!
   Transact data transfer between the smart card and the interface,
   including transmission and reception.

   \param[in] rwopt Descript a data transfer task.
   \return
 */
RET_CODE scard_pro_rw_transaction(scard_opt_desc_t *rwopt);

/*!
   Data transfer using protocol T = 0.

   \param[in] rwopt Descript a data transfer task.
   \param[in] status Two bytes of status.
   \return
 */
RET_CODE scard_pro_t0_transaction(scard_opt_desc_t *rwopt, u8 status[2]);

/*!
   Data transfer using protocol T = 1.

   \param[in] rwopt Descript a data transfer task.
   \param[in] status Two bytes of status.
   \return
 */
RET_CODE scard_pro_t1_transaction(scard_opt_desc_t *rwopt, u8 status[2]);

/*!
   Data transfer using protocol T = 14.

   \param[in] rwopt Descript a data transfer task.
   \param[in] status Two bytes of status.
   \return
 */
RET_CODE scard_pro_t14_transaction(scard_opt_desc_t *rwopt, u8 status[2]);

/*!
   Register the device handle of smart card operation layer

   \param[in] p_op_device The device handle of smart card operation layer.
   \return
 */
RET_CODE scard_pro_register_op(scard_device_t *p_op_device);


/*!
   Get smart card id.

   \param[out] the pointer of memory to store the card id
   \return
 */
RET_CODE scard_pro_get_card_type(u32 *card_type);


/*!
   PPS

   \param[in] the param for pps
   \return
 */RET_CODE smc7816_pps_req(scard_pro_cfg_t *p_cfg);

#endif //__SMART_PRO_CARD_H__

