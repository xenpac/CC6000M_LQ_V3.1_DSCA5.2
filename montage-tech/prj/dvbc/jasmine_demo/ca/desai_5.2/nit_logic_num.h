/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIT_LCN_H_
#define __NIT_LCN_H_

/*!
  \file nit.h
  Description:
  This file defines the structure of NIT information with all the description definitions
  And the NIT information can be got by sending request to engine and the ts packet will be
  parsed into NIT section. In addition, although all the descritor existing in NIT is defined,
  not all the descriptor can be found in one NIT table

  */
/*!
  Define the max logic channel number
  */
#define MAX_LCN_NUM 255

/*!
  Define the private descriptor
  */
#define DVB_DESC_LOGICAL_CHANNEL_PRIV  0x83

/*!
  Define start logic number 
  */
#define DVB_LOGIC_START_NUMBER   1001



/*!
  Parse NIT information
  \param[in] handle Service handle
  \param[in] p_sec: nit section information from PTI driver
  */
void parse_nit_with_lcn(handle_t handle, dvb_section_t *p_sec);
/*!
  Request NIT section
  \param[in] p_sec: section information for request
  \param[in] table_id: NIT table id for requesting
  \param[in] para param for requesting 
  */
void request_nit_with_lcn(dvb_section_t *p_sec, u32 table_id, u32 para);

#endif // End for __NIT_H_
