/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EIT_H_
#define __EIT_H_

/*!
  \file eit.h
  EIT information and descriptor structure is defined in this header file in 
  order to provide a full set of EIT descriptors.
  
  Development policy:
  In order to get EIT information from ts stream, the request api should be 
  invoked firstly and the ts packet of EIT will be sent back to parse EIT api 
  to get the EIT data section. 
  */
/*!
  MAX service number in one EIT section
  */
#define MAX_EVT_PER_SEC  (64)
/*!
  MIN length of EIT information
  */
#define MIN_EIT_TABLE_LENGTH  (15)
/*!
  Max segment number supported in EIT with the same table id
  */
#define MAX_SEG_NUM_EACH_TABLE  (32)
/*!
  Max section number supported in EIT with the same table id
  */
#define MAX_SEC_NUM_EACH_TABLE  (255)
/*!
  Max service name length
  */
#define MAX_EVT_NAME_LEN    (31)
/*!
  Max short text length in EIT table
  */
#define MAX_SHT_TEXT_LEN    (256)

/*!
  Min length of EIT table
  */
#define MIN_EIT_LENGTH    (0xF)

/*!
  Language code length
  */
#define LANGUAGE_CODE_LEN (3)

/*!
  MAX length to item description
  */
#define MAX_ITEM_DESC_LEN (15)

/*!
  MAX length to item content
  */
#define MAX_ITEM_CONT_LEN (15)

/*!
  MAX item number
  */
#define MAX_ITEM_NUM      (3)

/*!
  MAX txt length
  */
#define MAX_EXT_TXT_LEN   (256)

/*!
  Max number of extend event descriptors
  */
#define MAX_EXT_DESC_NUM  (16) 

/*!
  Max extend event txt number
  */
#define MAX_EXT_TXT_NUM  (16)

/*!
  Max Short event txt number
  */
#define MAX_SHT_TXT_NUM  (32)

/*!
  time shifted event descriptor
  */
typedef struct tag_time_shifted_evt_descr
{
    /*!
      reference service id
      */
    u16 svc_id;
    /*!
      reference event id
      */
    u16 evt_id;
}time_shifted_evt_descr_t;

/*!
  Item info in extend event descriptor
  */
typedef struct
{
  /*!
    Item description, such as cast list etc.
    */
  u8 item_name[MAX_ITEM_DESC_LEN];
  /*!
    Item content, such as cast name etc.
    */
  u8 item_cont[MAX_ITEM_CONT_LEN];
}item_info_t;

/*!
  EIT extend event descriptor
  */
typedef struct
{
  /*!
    Extend ext event index in extend event description series
    */
  u8 index;
  /*!
    Language code
    */
  u8 lang_code[LANGUAGE_CODE_LEN];
  /*!
    MAX item information
    */
  item_info_t item[MAX_ITEM_NUM];
  /*!
    item number
    */
  u8  item_num;
  /*!
    Total desc number
    */
  u8  tot_ext_desc;
  /*!
    Event id
    */
  u16 evt_id;
}ext_evt_desc_t;

/*!
  Content descriptor
  */
typedef struct
{
  /*!
    content nibble
    High 4 bits are level 1
    Low 4 bits are level 2
    */
  u8 cont_level;
  /*!
    User nibble
    High 4 bits are nibble 1
    Low 4 bits are nibble 2
    */
  u8 usr_nib;
  /*!
    Event id
    */
  u16 evt_id;
}cont_desc_t;

/*!
  Short event text info
  */
typedef struct
{
  /*!
    Text of short event 
    */
  u8 txt[MAX_SHT_TEXT_LEN];
  /*!
    Event id
    */
  u16 evt_id;
  /*!
    Length of short event text
    */
  u16 txt_len;  
  /*!
     Length of event name 
     */
   u8 name_len;
   /*!
     Event name of cerain event
     */
   u8 evt_nm[MAX_EVT_NAME_LEN];
    /*!
    Language code
    */
  u8 lang_code[LANGUAGE_CODE_LEN];   
  
  
}sht_evt_txt_t;

/*!
  parental rating descriptor
  */
typedef struct parental_rating_desc
{
    /*!
      country_code
      */
    u8 country_code[3];
    /*!
      event rating
      */
    u8 rating;
}parental_rating_desc_t;


/*!
  Extend event text information
  */
typedef struct
{
  /*!
    TXT info in extend event description
    */
  u8  ext_txt[MAX_EXT_TXT_LEN];
  /*!
    Extend text length
    */
  u8 txt_len;
  /*!
    language code 
     */
  u8 lang_code[LANGUAGE_CODE_LEN];   
  /*!
    Event id
    */
  u16 evt_id;
}ext_evt_txt_t;

/*!
  EIT event descriptor for one event
  */
typedef struct
{
  /*!
    Event id
    */
  u16 evt_id;
  /*!
    evt running status
    */
  u8 running_status : 3;
  /*!
    Duration of event time 
    */
  utc_time_t drt_time;
  /*!
    Start time of event time
    */
  utc_time_t st_tm ;  
  /*!
    time shifted event descriptor found
    */
  u8 time_shifted_evt_des_found;
  /*!
    time shifted event descriptor
    */  
  time_shifted_evt_descr_t time_shifted_evt_des;
  /*!
     parental rating
        */ 
  parental_rating_desc_t  parental_rating;  
}sht_evt_desc_t;

/*!
  EIT data for processing EIT information  
  */
typedef struct
{
  /*!
    Ts stream id,concern with different TP
    */
  u16 stream_id;  
  /*!
    Table id 
    */
  u8  table_id;
  /*!
    Version number
    */
  u8  version; 
  /*!
    Section length
    */
  u16 sec_length;
  /*!
    Service id
    */
  u16 svc_id;
  /*!
    If the nit info is avalaible in current TS or other TS
    */
  BOOL pf_flag;
  /*!
    The counter of this section in max section length
    */
  u8 section_number;
  /*!
    Max section length.
    */
  u8 last_section_number;
  /*!
    Last section number in each section
    */
  u8 seg_last_sec_number;
  /*!
    Last table id of EIT with the same service id
    */
  u8 last_table_id;
  /*!
    Descriptor struct for des LOOP content
    */  
  sht_evt_desc_t sht_evt_info[MAX_EVT_PER_SEC] ;
  /*!
    Extend event info
    */
  ext_evt_desc_t ext_evt_info[MAX_EXT_DESC_NUM];
  /*!
    Content description array
    */
  cont_desc_t cont_desc[MAX_EVT_PER_SEC];
  /*!
    Short event txt array
    */
  sht_evt_txt_t sht_txt[MAX_SHT_TXT_NUM];  
  /*! 
    Extend event txt array
    */
  ext_evt_txt_t ext_txt[MAX_EXT_TXT_NUM];
  /*!
    Short event description number
    */
  u8 tot_evt_num;
  /*!
    Total extend event descriptor number
    */
  u8 tot_ext_info_num;
  /*!
    Content desc number
    */
  u8 tot_cont_num;
  /*!
    Short event txt number
    */
  u8 tot_sht_txt_num;
  /*!
    Extend event text number
    */
  u8 tot_ext_txt_num;
  /*!
    Id for rebroadcast other network's info
    */
  u16 org_nw_id;    
}eit_t;


/*!
  A simple PF eit structure 
  */
typedef struct
{
  /*!
    Ts stream id,concern with different TP
    */
  u16 stream_id;  
  /*!
    Table id 
    */
  u8  table_id;
  /*!
    Version number
    */
  u8  version; 
  /*!
    Section length
    */
  u16 sec_length;
  /*!
    Service id
    */
  u16 svc_id;
  /*!
    If the nit info is avalaible in current TS or other TS
    */
  BOOL pf_flag;
  /*!
    The counter of this section in max section length
    */
  u8 section_number;
  /*!
    Max section length.
    */
  u8 last_section_number;
  /*!
    Last section number in each section
    */
  u8 seg_last_sec_number;
  /*!
    Last table id of EIT with the same service id
    */
  u8 last_table_id;
  /*!
    Descriptor struct for des LOOP content
    */  
  sht_evt_desc_t sht_evt_info ;
   /*!
    Short event
    */
  sht_evt_txt_t sht_txt;  
  /*!
    net work id
    */ 
  u16 org_nw_id;    
}eit_pf_t;


/*! 
  This api can be applied to parse eit data stored in buffer
  
  \param[in] p_buf_addr: start address of eit buffer 
  \param[out] p_eit_info: address to the buffer for parsed eit sections
  return section length 
  */
u16  eit_section_processing(u8 *p_buf_addr, eit_t *p_eit_info);

/*!
  Parse eit section.

  \param[in] handle Service handle
  \param[in] p_sec: section data filter by PTI driver.
  */
void parse_eit(handle_t handle, dvb_section_t *p_sec);

/*!
  Parse only pf eit section.

  \param[in] handle Service handle
  \param[in] p_sec: section data filter by PTI driver.
  */
void parse_eit_pf(handle_t handle, dvb_section_t *p_sec);


/*!
  Request a EIT of PF in actual stream in single section mode

  \param[in] p_sec: EIT section number in TS.
  \param[in] table_id: table id
  \param[in] para2: section number
  */
void request_eit_single_mode(dvb_section_t *p_sec, u32 table_id, u32 para2);

/*!
  Request a EIT of PF in actual stream in single section mode

  \param[in] p_sec: EIT section number in TS.
  \param[in] para1: table_id and service_id  (table_id<<24|servic_id)
  \param[in] para2: section number(0/1, 0 for Present pg and 1 for Following pg)
  */
void request_eit_single_mode_2(dvb_section_t *p_sec, u32 para1, u32 para2);  


/*!
  Request a EIT of PF in other stream in multitude section mode

  \param[in] p_sec: Section information for request operation in DVB
  \param[in] table_id EIT table id
  \param[in] para2 external buffer pointer to external buffer list
  */
void request_eit_multi_mode(dvb_section_t *p_sec, u32 table_id, u32 para2);

/*!
  Request a EIT of PF in other stream in multitude section mode

  \param[in] p_sec: Section information for request operation in DVB
  \param[in] table_id EIT table id
  \param[in] para2 external buffer pointer to external buffer list
  */
void request_eit_multi_mode_2(dvb_section_t *p_sec, u32 table_id, u32 para2);

#endif // End for __EIT_H_

