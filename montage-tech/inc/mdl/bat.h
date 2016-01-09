/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BAT_H_
#define __BAT_H_

/*!
  \file bat.h
  This file provides the common data information for bat.h. These information include
  the data structure of bat table and how to request and parse bat information
  
  Development policy:
  In order to get bat information under certain circumstance, this module should work
  together with DVB module and PTI module
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
  Data structure for linkage update desc
  */
typedef struct linkage_update_desc_tag
{
  /*!
    Operator num
    */   
  u8 operator_num;
  /*!
    Manufacture id
    */   
  u8 manufacture_id;
  /*!
    Model id
    */   
  u8 model_id;
  /*!
    Hardware id
    */   
  u16 hardware_id;
  /*!
    Software version
    */   
  u16 software_ver;
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
    Force download flag
    */   
  u8 force_flag;
  /*!
    Start STB id
    */   
  u32 start_STB_id;
  /*!
    End STB id
    */   
  u32 end_STB_id;
}linkage_update_desc_t;

/*!
  Data structure for logical channel desc
  */
typedef struct logical_channel_tag
{
  /*!
    service id
    */   
  u16 service_id;
  /*!
    logical channel number
    */   
  u16 logical_channel_id;
}logical_channel_t;

/*!
  Data structure for abs bat table information
  */
typedef struct abs_bat_tag
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
  logical_channel_t log_ch_info[DVB_MAX_BAT_SVC_NUM];
}bat_t;

/*!
  Parse BAT information
  \param[in] handle: Service handle
  \param[in] p_sec: bat section information from PTI driver
  */
void parse_bat(handle_t handle, dvb_section_t *p_sec);
/*!
  Request BAT section
  \param[in] p_sec: section information for request
  \param[in] table_id: BAT table id for requesting
  \param[in] para: param for requesting 
  */
void request_bat_section(dvb_section_t *p_sec, u32 table_id, u32 para);
/*!
  Request BAT section
  \param[in] p_sec: section information for request
  \param[in] table_id: BAT table id for requesting
  \param[in] para2: param for requesting 
  */
void request_bat_multi_mode(dvb_section_t *p_sec, u32 table_id, u32 para2);

#endif // End for __ABS_BAT_H_
