/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SMC_CTL_H__
#define __SMC_CTL_H__

/*!
  commments
  */
#define SMC_FREQUENCY_DIV_DEFAULT  9
/*!
  commments
  */
#define SMC_FREQUENCY_DIV_APB SMC_FREQUENCY_DIV_DEFAULT
/*!
  commments
  */
#define SMC_FREQUENCY_DIV_12MHZ 2
/*!
  commments
  */
#define SMC_FREQUENCY_DIV_T14    5

/*!
  620 for IS1521, 590 for IS1526
  */
#define SMC_ETU_T14    620
/*!
  commments
  */
#define SMC_ETU_DEFAULT  372

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
}smc_id_t;

/*!
  SMC CONFIG VALUE
*/
typedef enum
{
  /*!
    commments
  */
  SMC_DISABLE = 0,
  /*!
    commments
  */
  SMC_ENABLE
}smc_cfg;

/*!
  SMC PHYSICAL CLOCK SEL
*/
typedef enum
{
  /*!
    commments
  */
  SMC_PHYCLK_APB = 0,
  /*!
    commments
  */
  SMC_PHYCLK_12MHZ
}smc_clk_cfg_e;

/*!
  SMC STOP BIT NUMBER
*/
typedef enum
{
  /*!
    commments
  */
  SMC_STOP_TWO = 0,
  /*!
    commments
  */
  SMC_STOP_ONE
}smc_bitstop_t;

/*!
  智能卡插座类型
*/
typedef enum
{
  /*!
    commments
  */
  SMC_SOCKET_CLOSED,
  /*!
    commments
  */
  SMC_SOCKET_OPEN
}smc_sock_types;

/*!
  VCC 模式
*/
typedef enum
{
  /*!
    commments
  */
  SMC_VCC_INTERNAL,
  /*!
    commments
  */
  SMC_VCC_EXTERNAL
}smc_vcc_mod;

/*!
  comment
  */
BOOL smc_get_phyclk_sel(smc_id_t smc_id);

/*!
  sc get status
  */
BOOL smc_get_status(smc_id_t smc_id);

/*!
  sc get rx status
  */
BOOL smc_rx_notempty(smc_id_t smc_id);

/*!
  sc get one reply
  */
u8 smc_rx_get(smc_id_t smc_id);

/*!
  sc set cmd length
  */
void smc_tx_cmdlen(smc_id_t smc_id, u16 len);

/*!
  sc set cmd data
  */
void smc_tx_cmd(smc_id_t smc_id, u8 data);

/*!
  sc buffer fifo count
  */
u16 smc_fifo_cnt(smc_id_t smc_id);

/*!
  comment
  */
void smc_init(smc_id_t smc_id, void (*smc_interrupt_handle)(void), u8 phyclk, u8 convt_en,
  u8 convt_value, u8 detect_pin_pol, u8 vcc_enable_pol,
  u8 iopin_mode, u8 rstpin_mode, u8 clkpin_mode);
/*!
  comment
  */
void smc_set_etu(smc_id_t smc_id, u32 ETU);

/*!
  comment
  */
void smc_reset(smc_id_t smc_id, u16 delays);


/*!
  comment
  */
void  smc_set_bitcoding(smc_id_t smc_id, BOOL bT14);

/*!
  comment
  */
void smc_flush(smc_id_t smc_id);

#endif


