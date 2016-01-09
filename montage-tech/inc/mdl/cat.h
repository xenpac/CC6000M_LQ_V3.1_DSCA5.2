/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CAT_H_
#define __CAT_H_

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
  /*!
    CA private data
    */
  //u8 ca_prv_data[CA_PRIVATE_DATA_LEN];
}ca_desc_t;

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
  u8 *p_origion_data;
  /*!
    desc num
    */
  u32 emm_cnt;    
  /*!
    CA descriptor information
    */
  ca_desc_t ca_desc[MAX_EMM_DESC_NUM];
}cat_cas_t;

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
  u32 len;
} cat_t;


/*!
  Parse CAT table
  \param[in] handle DVB handle
  \param[in] p_sec DVB section information
  */
void parse_cat(handle_t handle, dvb_section_t *p_sec);

/*!
  Parse CAT table
  \param[in] handle DVB handle
  \param[in] p_sec DVB section information
  */
void parse_cas_cat(handle_t handle, dvb_section_t *p_sec);

/*!
  Request CAT table
  \param[in] p_sec section table
  \param[in] table_id table id
  \param[in] para2 parameter
  */
void request_cat(dvb_section_t *p_sec, u32 table_id, u32 para2);

#endif // End for __CAT_H_

