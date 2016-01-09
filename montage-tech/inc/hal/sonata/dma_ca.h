/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*! 
 *******************************************************************************
 * @defgroup  IP_AdvancedCA Advanced Conditional Access
 *  @{
 *  \note project:    sonata
 *  \note chip:     
 *  \file sonata/dma_ca.h
 *  \author     Bob Yang
 *  \date       2013/12/12
 *  \version    1.0
 *  \brief      Advanced Conditional Access Module Driver
 *******************************************************************************
 */ 


#ifndef __DMA_CA_H__
#define __DMA_CA_H__


/* ========================================================================== */
/* header files                     */
/* ========================================================================== */
#include  "sys_types.h"
#include  "hal_secure.h"


/* ========================================================================== */
/* function definitions                   */
/* ========================================================================== */
/**
 * @name Public Functions
 * @{
 */

/**
  \brief    This function attaches driver for Advanced Conditional Access Module.

  \return 0       Success
  \return Others        Exception
 ******************************************************************************/
RET_CODE  dma_ca_DriverAttach (
    ST_ADVANCEDCA_OPTIONINFO      * const pstOptionInfo
    );

/**
  \brief    This function attaches driver for Advanced Conditional Access Module.

  \return 0       Success
  \return Others        Exception
 ******************************************************************************/
RET_CODE  dma_ca_ChannelInfo (void);

/**
 * @}
 */


#endif /*  __HAL_DMA_CA_H__ END */


/**
 * @}
 */

