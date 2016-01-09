/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ECM_H_
#define __ECM_H_

/*!
  ECM structure
  */
typedef struct
{
  /*!
    Section buffer
    */
  u8 buf[MAX_SECTION_LENGTH];
  /*!
    Section buffer length
    */
  u32 len;
} ecm_t;

/*!
  Parse ECMt section

  \param[in] p_sec: Section data filtered by PTI driver
  */
void parse_ecm(handle_t handle, dvb_section_t *p_sec);

/*!
  Request a ECM data
  \param[in] p_sec: section information
  \param[in] para1: para1
  \param[in] para2: para2
  */
void request_ecm(dvb_section_t *p_sec, u32 para1, u32 para2);

/*!
  Request a ECM data
  \param[in] p_sec: section information
  \param[in] para1: para1
  \param[in] para2: para2
  */
void request_cas_ecm(dvb_section_t *p_sec, u32 para1, u32 para2);

#endif // End for __ECM_H_
