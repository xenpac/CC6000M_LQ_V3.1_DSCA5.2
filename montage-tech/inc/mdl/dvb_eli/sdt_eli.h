/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SDT_ELI_H_
#define __SDT_ELI_H_

/*!
  \file: sdt_eli.h
  This file defines the common data information available in SDT table. These 
  information cover all the descriptors existing in SDT table.
  
  Development policy:
  SDT part should be developed together with DVB module. And by requesting SDT API,
  filter SDT information operation will be performed and the ts packet information
  with SDT data will be fed into parse sdt api to get the sdt section data
  */
/*!
  Max service name length
  */
#define MAX_SERVICE_NAME_LEN (32)

/*!
  SDT table service descriptor definition
  */
typedef struct
{
  /*!
    Service id
    */
  u16 svc_id ;
  /*!
    service type
    */
  u8 service_type;
  /*!
    EIT schedule flag
    */
  u8 eit_sch_flag ;
  /*!
    EIT PF flag
    */
  u8 eit_pf_flag ;
  /*!
    Channel volum compensation
    */
  u8 chnl_vlm_cmpt;  
  /*!
    Running status
    */
  u8 run_sts ;
  /*!
    Service scrambled flag, 
    1: scramble
    0: free
    */
  u8 is_scrambled : 3;
  /*!
    mosaic descriptor found
    */
  u8 mosaic_des_found : 1;
  /*!
    logic screen descriptor found
    */
  u8 logic_screen_des_found : 1;
  /*!
    linkage descriptor found
    */
  u8 linkage_des_found : 1;
  /*!
    linkage descriptor found
    */
  u8 nvod_reference_des_found : 1;
  /*!
    linkage descriptor found
    */
  u8 time_shifted_svc_des_found : 1;
  /*!
    nvod reference des cnt or time shifted svc reference svc id
    */
  u16 nvod;
  /*!
    CA system id
    */
  u16 ca_system_id;
  /*!
    Service name information
    */
  u8 name[MAX_SERVICE_NAME_LEN];
  /*!
    mosaic descriptor(service_type = 0x06) or
    nvod reference descriptor
    */
  void *p_nvod_mosaic_des;
}sdt_svc_desc_t;

/*!
  SDT table structure
  */
typedef struct
{
  /*!
    Stream id
    */
  u16 stream_id;
  /*!
    SDT section length
    */
  u16 sec_length;
  /*!
    Section number of SDT
    */
  u8 sec_number;
  /*!
    Last section number
    */
  u8 last_sec_number;
  /*!
    Original network id
    */
  u16 org_network_id;
  /*!
    Module id for deal sdt request conflicts
    */
  u8 module_id;
  /*!
    PF flag
    */
  u8 pf_flag;
  /*!
    Service number
    */
  u16 svc_count;
  /*!
    Service descriptor information
    */
  sdt_svc_desc_t svc_des[DVB_MAX_SDT_SVC_NUM];
}sdt_eli_t;


/*!
  parse dvbs' sdt table
  
  \param[in] p_parse : dvb parse param pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE sdt_eli_parse_dvbs(dvb_parse_param_t *p_parse, u8 *p_input, u8 *p_output);

/*!
  sdt table request param

  \param[in] p_request : pmt table request static parameters
  */
void sdt_eli_request(dvb_request_param_t * p_request);


#endif // End for __SDT_ELI_H_

