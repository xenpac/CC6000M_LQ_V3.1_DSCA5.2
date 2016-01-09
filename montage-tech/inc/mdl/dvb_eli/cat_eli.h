/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CAT_ELI_H_
#define __CAT_ELI_H_

/*!
  CA descriptor number
  */
#define MAX_EMM_DESC_NUM (8)

/*!
  CA private data length
  */
#define CA_PRIVATE_DATA_LEN (252)

/*!
  CA descriptor
  */
typedef struct 
{
  /*!
    CA system id
    */
  u16 ca_sys_id;
  /*!
    EMM pid
    */
  u16 emm_pid;
}ca_eli_desc_t;

/*!
  CA structure define(not used)
  */
typedef struct
{
  /*!
    Table id
    */
  u8 tab_id;
  /*!
    Section number
    */
  u8 sec_num;
  /*!
    Last section number
    */
  u8 last_sec_num;
  /*!
    Version number
    */
  u8 ver_num;
  /*!
    origion data
    */
  u8 origion_data[MAX_SECTION_LENGTH];
  /*!
    desc num
    */
  u32 emm_cnt;    
  /*!
    CA descriptor information
    */
  ca_eli_desc_t ca_desc[MAX_EMM_DESC_NUM];
}cat_cas_eli_t;

/*!
  CA structure define
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
  u32 sec_len;
  /*!
    sid
    */
  u32 sid;
}cat_eli_t;


/*!
  parse cat table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE cat_eli_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 *p_output);

/*!
  parse cat cas table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE cat_cas_eli_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 *p_output);

  
/*!
  cat table request param

  \param[in] p_param : pat table request static parameters
  */
void cat_eli_request(dvb_request_param_t *p_param);


#endif // End for __CAT_ELI_H_

