/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TDT_ELI_H_
#define __TDT_ELI_H_

/*!
  parse pat table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE tdt_eli_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 * p_output);

/*!
  pat table request param

  \param[in] p_param : pat table request static parameters
  */
void tdt_eli_request(dvb_request_param_t *p_param);

#endif // End for __TDT_ELI_H_
