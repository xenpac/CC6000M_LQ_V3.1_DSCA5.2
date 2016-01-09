/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SECURITY_CONCERTO_H__
#define __SECURITY_CONCERTO_H__
/*!
* header files                     
*/
#include  "sys_types.h"
#include  "hal_secure.h"
/*!
  type definitions							                                              
*/

/*!
 * @name Public Functions
 * @{
 */

/*!
  \brief    This function attaches driver for Advanced Conditional Access Module.

  \return 0       Success
  \return Others        Exception
 *****************************************************************************
 */
RET_CODE  dma_ca_DriverAttach (ST_ADVANCEDCA_OPTIONINFO      * const pstOptionInfo);

/*!
  \brief    This function attaches driver for Advanced Conditional Access Module.

  \return 0       Success
  \return Others        Exception
 *****************************************************************************
 */
RET_CODE  dma_ca_ChannelInfo (void);

/**
 * @}
 */
#endif