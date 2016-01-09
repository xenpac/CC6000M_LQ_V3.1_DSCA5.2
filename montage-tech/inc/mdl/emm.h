/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EMM_H_
#define __EMM_H_

/*!
  EMM structure 
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
} emm_t;

/*!
  Parse EMM section

  \param[in] handle dvb service handle
  \param[in] p_sec section information 
  */
void parse_emm(handle_t handle, dvb_section_t *p_sec);

/*!
  Request a EMM section data
  \param[in] p_sec: EMM section information
  \param[in] para1: table id 
  \param[in] para2: pid
  */
void request_emm(dvb_section_t *p_sec, u32 para1, u32 para2);

/*!
  Request a EMM section data
  \param[in] p_sec: EMM section information
  \param[in] para1: table id 
  \param[in] para2: pid
  */
void request_cas_emm(dvb_section_t *p_sec, u32 para1, u32 para2);

#endif // End for __EMM_H_

