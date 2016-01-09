/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EMM_ELI_H_
#define __EMM_ELI_H_

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
}emm_eli_t;


/*!
  parse emm table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE emm_eli_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 * p_output);

/*!
  emm table request param

  \param[in] p_param : emm request static parameters
  */
void emm_eli_request(dvb_request_param_t *p_param);

/*!
  emm table request param

  \param[in] p_param : emm request static parameters
  */
void emm_cas_eli_request(dvb_request_param_t *p_param);


#endif // End for __EMM_ELI_H_

