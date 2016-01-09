/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIT_ELI_H_
#define __NIT_ELI_H_

/*!
  \file nit_eli.h
  Description:
  This file defines the structure of NIT information with all the description definitions
  And the NIT information can be got by sending request to engine and the ts packet will be
  parsed into NIT section. In addition, although all the descritor existing in NIT is defined,
  not all the descriptor can be found in one NIT table

  */


/*!
  Max tp number supported
  */
#define MAX_TP_NUMBER  64
/*!
  Max program number
  */
#define MAX_PROG_NUM  40

/*!
  Max provider name length
  */
#define MAX_NETWORK_NAME_LEN 256

/*!
  invalid network id
  */
#define NETWORK_ID_INVALID   (0xFFFF)

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
  u8 modulation     : 5;
  /*!
    reserve
    */
  u8 reserve;
}nit_sat_tp_info_t;

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
}nit_cable_tp_info_t;


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
}nit_terrestrial_tp_info_t;

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
}svc_up_desc_t;


/*!
  tp type
  */
typedef enum
{
/*!
  dvbs type
  */
  NIT_ELI_DVBS_TP,
/*!
  dvbc type
  */
  NIT_ELI_DVBC_TP,
/*!
  dvbt type
  */
  NIT_ELI_DVBT_TP
}nit_eli_tp_type_t;

/*!
  Service list on each tp 
  */
typedef struct tag_nit_tp_svc_list
{
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
  nit_eli_tp_type_t tp_type;
  /*!
    Specific tp info
    */   
  union
  {
  /*!
    dvbs tp info
    */
    nit_sat_tp_info_t dvbs_tp_info;
  /*!
    dvbc tp info
    */
    nit_cable_tp_info_t dvbc_tp_info;
  /*!
    fix me !!
    */
    nit_terrestrial_tp_info_t dvbt_tp_info;
  };
}nit_tp_svc_list_t;

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
  nit_tp_svc_list_t tp_svc_list[MAX_TP_NUMBER]; 
  /*!
    Service update descriptor supported in ABS project
    */
  svc_up_desc_t svc_update;
}nit_eli_t;

/*!
  parse nit table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE nit_eli_parse(dvb_parse_param_t *p_param, u8  *p_input, u8 *p_output);

/*!
  nit table request param

  \param[in] p_param : bat table request static parameters
  */
void nit_eli_request(dvb_request_param_t *p_param);


#endif // End for __NIT_ELI_H_
