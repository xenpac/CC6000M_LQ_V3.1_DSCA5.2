/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BAT_ELI_H_
#define __BAT_ELI_H_

/*!
  \file bat_eli.h
  This file provides the common data information for bat.h. These information include
  the data structure of bat table and how to request and parse bat information
  */
  
/*!
  Linkage type for bat desc
  */
#define DESC_LINKAGE  (0x4a)
/*!
  Upgrade bouquet id
  */
#define BOUQUET_UPGRADE 0xff00
/*!
  Fdt bouquet id
  */
#define FDT_BAT_ID  (0x7012)
/*!
  SSU linkage type
  */
#define LINKAGE_TYPE_SSU  (0xA0)
/*!
  FDT linkage type
  */
#define LINKAGE_TYPE_FDT  (0x80)
/*!
  Ts packet sync tag
  */
#define TS_PACKET_TAG (0x47)


/*!
  Data structure for logical channel desc
  */
typedef struct logical_chantag
{
  /*!
    service id
    */   
  u16 service_id;
  /*!
    logical channel number
    */   
  u16 logical_channel_id;
}logical_chan_t;

/*!
  Data structure for abs bat table information
  */
typedef struct tag_bat_table
{
  /*!
    Section number
    */
  u8 sec_number;
  /*!
    Last section number
    */
  u8 last_sec_number;
  /*!
    Network id
    */   
  u16 network_id;
  /*!
    Transport stream id
    */   
  u16 ts_id;
  /*!
    Service id to locate pmt lbn
    */   
  u16 service_id;
  /*!
    Bouquet of services id
    */   
  u16 bouquet_id;
  /*!
    Version number
    */      
  u8 version_num;
  /*!
    FDT table id
    */
  u8 fdt_table_id;
  /*!
    Si data packet PID
    */
  u16 fdt_pid;
  /*!
    si version number
    */
  u8  fdt_ver_num;
  /*!
    logical channel number
    */
  u8 log_ch_num;
  /*!
    logical channel info
    */
  logical_chan_t log_ch_info[DVB_MAX_SDT_SVC_NUM];
}bat_eli_t;


/*!
  parse bat table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE bat_eli_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 *p_output);

/*!
  bat table request param

  \param[in] p_param : bat table request static parameters
  */
void bat_eli_request(dvb_request_param_t *p_param);

/*!
  bat table request param

  \param[in] p_param : bat table request static parameters
  */
void bat_eli_request_multi(dvb_request_param_t *p_param);

#endif // End for __BAT_ELI_H_
