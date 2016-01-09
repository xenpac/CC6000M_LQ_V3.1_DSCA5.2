/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DDS_H_
#define __DDS_H_

/*!
  Max section length for one DDS section   
  */
#define MAX_DDS_SEC_LEN (4093)

/*!
  Max payload length
  */
#define MAX_PAYLOAD_LEN (4096)

/*!
  Max signature length
  */
#define MAX_SIGN_LEN  (4096)  

/*!
  Download software description
  */
typedef struct dl_sw_des_tag
{
  /*!
    Software version
    */
  u16 sw_version;
  /*!
    reserved
    */
  u16 resv;
  /*!
    software size
    */
  u32 sw_size;
  /*!
    CRC information for software
    */
  u32 sw_crc;
}dl_sw_des_t;

/*!
  Download data section
  */
typedef struct dds_tag
{
  /*!
    table id
    */
  u8 table_id;

  /*!
    sub-table id
    */
  u8 sub_tab_id;
  
  /*!
    Section number
    */
  u8 sec_num;

  /*!
    Last section number
    */
  u8 last_sec_num;

  /*!
    Download software descriptor
    */
  dl_sw_des_t sw_des;

  /*!
    Payload length
    */
  u16 payload_len;
  
  /*!
    Signature length and if signature length is 0 means no signature existing
    */
  u16 sign_len;
  
  /*!
    Payload content
    */
  u8 payload[MAX_PAYLOAD_LEN];
  
  /*!
    Signature content
    */
  u8 sign[MAX_SIGN_LEN];
}dds_t;

/*!
  Parse DDS section

  \param[in] p_buf Section data filtered by PTI driver
  */
void parse_dds(handle_t handle, dvb_section_t *p_sec);
/*!
  Request a DDS section data

  \param[in] p_sec: DDS section information
  \param[in] para1:  
  \param[in] para2: 
  */
void request_dds(dvb_section_t *p_sec, u32 para1, u32 para2);

#endif // End for __DDS_H_

