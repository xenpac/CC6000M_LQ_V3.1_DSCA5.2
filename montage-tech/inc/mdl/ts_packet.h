/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TS_PACKET_H__
#define __TS_PACKET_H__

/*!
  \file ts_packet.h
  
  This file provides the common operation to parse ts packet into one section. 
  As the initialization function, process function and release function are provided, 
  the implementation should also follow this logic. 
  
  Development policy:
  This module should also work with DVB module and PTI driver module
  */
/*!
  PSI section information for parsing ts packet
  */
typedef struct
{
  /*!
    Max section supported when parse ts packet
    */
  u16 i_section_max_size;
  /*!
    Continuity counter
    */
  u8  i_continuity_counter;
  /*!
    section header already parsed or not
    */
  u8  b_complete_header;
  /*!
    Actual data length to be parsed in section
    */
  u16 i_need;
  /*!
    Current section buffer
    */
  u8 *p_cur_sec_buf;
  /*!
    Payload start position in ts packet buffer
    */
  u8 *p_payload_pos;
  /*!
    Section in current ts packet is used to transport information
    */
  BOOL b_this_sec_using;
}psi_sec_info_t;

/*!
  Initialize ts packet buffer and control flag for parsing ts packet 
  into section
  \param[in] p_psi_info PSI information with ts packet buffer and control flag 
  \param[in] user_id memory user id where ts packet buffer created
  \param[in] partition_id partition id where buffer is allocated
  \param[in] section_max_size max section length supported when parsing ts 
             packet
  API invoke this api should check whether p_ts_buf is NULL 
  */
void create_ts_packet_buf(psi_sec_info_t *p_psi_info,
u8 partition_id, u16 user_id, u16 section_max_size);

/*!
  Parse one input ts packet to section 
  \param[in] p_psi_info: psi buffer with ts packet information and control flag
  \param[in] p_data:  data buffer for saving section information parsed 
   from input ts packet
  */
void ts_packet_to_sec(psi_sec_info_t *p_psi_info,u8 *p_data);
/*!
  Release ts packet buffer
  \param[in] p_psi_info PSI information with ts packet buffer and control flag       
  \param[in] partition_id: partition id where ts packet buffer allocated
  \param[in] user_id:      user id of the partition where ts packet buffer 
  */
void release_ts_packet_buf(psi_sec_info_t *p_psi_info,
u8 partition_id, u16 user_id);

#endif // End for ts packet to section

