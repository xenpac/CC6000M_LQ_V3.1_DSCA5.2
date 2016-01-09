/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TDT_H_
#define __TDT_H_

/*!
  Parse tdt section.
  \param[in] p_sec: section data filter by PTI driver.
  */
void parse_tdt(handle_t handle, dvb_section_t *p_sec);

/*!
  Request a TDT section data.

  \param[in] p_sec: Section information for request tdt section
  \param[in] table_id for TDT
  \param[in] para2: 
  */
void request_tdt_section(dvb_section_t *p_sec, u32 table_id, u32 para2);


#endif // End for __TDT_H_
