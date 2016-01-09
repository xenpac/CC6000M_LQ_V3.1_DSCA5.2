/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIT_H_
#define __NIT_H_

/*!
  \file nit.h
  Description:
  This file defines the structure of NIT information with all the description definitions
  And the NIT information can be got by sending request to engine and the ts packet will be
  parsed into NIT section. In addition, although all the descritor existing in NIT is defined,
  not all the descriptor can be found in one NIT table

  */


/*!
  Max tp number supported
  */
#define MAX_TP_NUM  64
/*!
  Max program number
  */
#define MAX_PROG_NUM  40

/*!
  Max provider name length
  */
#define MAX_NETWORK_NAME_LEN 256

/*!
  Max lcd number supported
  */
#define MAX_LCD_NUM  256

/*!
  Satellite transport stream specification
  */
typedef struct
{
  /*!
    TP frequency
    */   
  u32 frequency;
  /*!
    Symbol rate
    */   
  u32 symbol_rate;
  /*!
    Satellite orbital position
    */   
  u16 orbital_position;
  /*!
    West or east orbit area
    */   
  u8 west_east_flag : 1;
  /*!
    Polarization of transmitting signal
    */   
  u8 polarization   : 2;
  /*!
    Modultation type
    */   
  u8 reserved     : 2;
  /*!
    Modultation type
    */   
  u8 modulation_system     : 1;
  /*!
    Modultation type
    */   
  u8 modulation_type     : 2;
  /*!
    fec_inner
    */
  u8 fec_inner : 4;
  /*!
    reserve
    */
  u8 reserve : 4;
}sat_tp_info_t;

/*!
  cable stream specification
  */
typedef struct
{
  /*!
    TP frequency
    */   
  u32 frequency;
  /*!
    Symbol rate
    */   
  u32 symbol_rate;
  /*!
    Modultation type
    */   
  u32 modulation  :16;
  /*!
    FEC outer :0:undefine,  1:none fec outer,  2: RS(204 188) 3~.. reserved
    */   
  u32 fec_outer     :8;
  /*!
    FEC inner   0: undefined. 1: 1/1; 2: 2/3;  3: 3/4; 4: 5/6; 5:7/8; ...
    */   
  u32 fec_inner     :8;
}cable_tp_info_t;


/*!
  cable stream specification
  */
typedef struct
{
  /*!
    TP frequency
    */   
  u32 frequency;
  /*!
    Symbol rate
    */   
  u32 symbol_rate;
  /*!
    Modultation type
    */   
  u32 reserved;
}terrestrial_tp_info_t;

/*!
  cable stream specification
  */
typedef struct
{
  /*!
    TP frequency
    */   
  u32 centre_frequency;
  /*!
    band width
    */   
  u8 bandwidth;
  /*!
    PLP id
    */   
  u8 plp;
  /*!
    T2 system id
    */   
  u16 t2_sys_id;
}terrestrial_2_tp_info_t;

/*!
  Service update descriptor
  */
typedef struct
{
  /*!
    Update force flag 
    0 not force 
    1 force
    */
  u8 force_flag:1;
  /*!
    Version number
    */
  u8 ver_num:7;
  /*!
    reserve
    */
  u8 reserve;
  /*!
    reserve
    */
  u16 reserve1;
}svc_update_desc_t;


/*!
  tp type
  */
typedef enum
{
/*!
  dvbs type
  */
  NIT_DVBS_TP,
/*!
  dvbc type
  */
  NIT_DVBC_TP,
/*!
  dvbt type
  */
  NIT_DVBT_TP,
/*!
  dvbt2 type
  */
  NIT_DVBT2_TP
}nit_tp_type_t;

/*!
  Service list on each tp 
  */
typedef struct
{
  /*!
    Total service Id in one single tp
    */   
  u16 svc_id[MAX_PROG_NUM];
  /*!
    Total service number on current tp
    */
  u16 total_svc_num;
  /*!
    Stream id
    */
  u16 ts_id;
  /*!
    tp type
    */
  nit_tp_type_t tp_type;
  /*!
    Specific tp info
    */   
  union
  {
  /*!
    dvbs tp info
    */
    sat_tp_info_t dvbs_tp_info;
  /*!
    dvbc tp info
    */
    cable_tp_info_t dvbc_tp_info;
  /*!
    fix me !!
    */
    terrestrial_tp_info_t dvbt_tp_info;

    terrestrial_2_tp_info_t t2_tp_info;
  };
}tp_svc_list_t;

/*!
  logical channel descriptor 
  */
typedef struct
{
  /*!
    This is a 16-bit field which serves as a label to identify this service from
  any other service within the Transport Stream. The service_id is the same as the
  program_number in the corresponding program_map_section. Services shall be included
  irrespective of their running status.
    */
  u32 svc_id:16;
  /*!
    This 1-bit field when set to future бе indicates that the receiver shall
  make the service visible and selectable (subject to the service type being suitable etc.)
  via the receiver service list.
    */
  u32 visible_service_flag:1;
  /*!
    All reserved: bits shall be set to future.
    */
  u32 reserved:5;
  /*!
    This is a 10-bit field which indicates the broadcaster
  preference for ordering services.
    */
  u32 logical_channel_number:10;
}logical_channel_descriptor_t;

/*!
  NIT definition
  */
typedef struct
{
  /*!
    TABLE id
    */
  u16 table_id;
  /*!
    network id
    */
  u16 network_id;
  
  /*!
    Version number
    */
  u8  version_num;
  /*!
    Section number
    */
  u8 sec_number;
  /*!
    Total tp number
    */
  u8 total_tp_num;
  /*!
    Last section number
    */
  u8 last_sec_number;
  /*!
    provider name.
    */
  u8 network_name[MAX_NETWORK_NAME_LEN];    
  /*!
    Service list on each tp
    */
  tp_svc_list_t tp_svc_list[MAX_TP_NUM]; 
  /*!
    Service update descriptor supported in ABS project
    */
  svc_update_desc_t svc_update;
  /*!
    logical channel number.
    */
  logical_channel_descriptor_t lcd[MAX_LCD_NUM];  
  /*!
    logical channel count.
    */
  u16 lcd_cnt;
  /*!
    origion data
    */
  u8 *p_origion_data;
} nit_t;
/*!
  Parse NIT information
  \param[in] handle Service handle
  \param[in] p_sec: nit section information from PTI driver
  */
void parse_nit(handle_t handle, dvb_section_t *p_sec);
/*!
  Request NIT section
  \param[in] p_sec: section information for request
  \param[in] table_id: NIT table id for requesting
  \param[in] para param for requesting 
  */
void request_nit(dvb_section_t *p_sec, u32 table_id, u32 para);

#endif // End for __NIT_H_
