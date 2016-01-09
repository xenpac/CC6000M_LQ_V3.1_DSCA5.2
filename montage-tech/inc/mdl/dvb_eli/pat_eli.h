/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PAT_ELI_H_
#define __PAT_ELI_H_

/*!
  Program info in PAT table.
  */
typedef struct tag_pat_eli_prog
{
 /*!
  LNB number
  */
  u32 lbn : 16;
 /*!
  PMT pid
  */
  u32 pmt_pid : 13;
 /*!
  mode : 0 is dvbs mode, 1 is abs mode. 
  */
  u32 mode : 1;
 /*!
  reserve
  */
  u32 reserve : 2;
 /*!
  program number.
  */
  u16 prog_num; 
}pat_eli_prog_t;

/*!
  PAT struct
  */
typedef struct tag_pat_eli
{
/*!
  network id
  */  
  u16 nit_pid;
/*!
  transport stream id.
  */ 
  u16 ts_id;  
 /*!
  Program number in PAT and this value equal to service id
  */
  u16 total_porgs;
 /*!
  Programs info in PAT
  */
  pat_eli_prog_t progs[DVB_MAX_PAT_PROG_NUM];
}pat_eli_t;


/*!
  parse pat table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE pat_eli_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 * p_output);

/*!
  pat table request param

  \param[in] p_param : pat table request static parameters
  */
void pat_eli_request(dvb_request_param_t *p_param);

#endif // End for __PAT_ELI_H_

