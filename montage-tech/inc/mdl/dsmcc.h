/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DSMCC_H_
#define __DSMCC_H_

/*!
  Marco for directory in dsmcc content
  */
#define DSMCC_CONTENT_DIR       0x01
/*!
  Marco for file in dsmcc content
  */
#define DSMCC_CONTENT_FILE      0x02
/*!
  dsmcc message type of DII
  */
#define DSMCC_MSG_DII                      0x1002
/*!
  dsmcc message type of DDB
  */
#define DSMCC_MSG_DDB                      0x1003
/*!
  dsmcc message type of DDR
  */
#define DSMCC_MSG_DDR                      0x1004
/*!
  dsmcc message type of DC
  */
#define DSMCC_MSG_DC                       0x1005
/*!
  dsmcc message type of DSI
  */
#define DSMCC_MSG_DSI                      0x1006
/*!
  max dsmcc module number
  */
#define DSMCC_MAX_MOD_NUM                  10
/*!
  biop header size of dsmcc message
  */
#define BIOP_HEADER_SIZE                44
/*!
  system hardware descriptor
  */
#define SYS_HARDWARE_DESC     1
/*!
  system software descriptor
  */
#define SYS_SOFTWARE_DESC      2
/*!
  upg software format descriptor
  */
#define SYS_SOFTWARE_FORMAT_DESC      3

/*!
  Maximum Group number 
  */
#define MAX_GROUP_NUMBER 32

/*!
  Maximum module  number 
  */
#define MAX_MODULE_NUM_PER_TYPE 10
/*!
  Module type number 
  */
#define MODULE_TYPE_NUM 3

/*!
  Module link tag 
  */
#define MODULE_LINK_TAG 0x04
/*!
  Module info tag 
  */
#define MODULE_INFO_TAG 0x80

/*!
  module type bootloader
  */
#define MODULE_TYPE_BL 0

/*!
  module type read only part
  */
#define MODULE_TYPE_RD 1

/*!
  module type read-write part
  */
#define MODULE_TYPE_RW 2

/*!
  module type block & all(no split)
  */
#define MODULE_TYPE_ALL 3

/*!
  dsmcc group info structure
  */
typedef struct
{
  /*!
    group indication 
    
    */
  u32 group_id;
  /*!
     oui id, it's allocated by the org
    */
  u32 oui;
  /*!
     hardware model id
    */
  u16 hw_mod_id;
  /*!
     software model id
    */
  u16 sw_mod_id;
  /*!
     hardware version
    */
  u16 hw_version;
  /*!
     software version
    */
  u16 sw_version;
  /*!
     software version
    */
  u8 sw_block_id;
  /*!
     software is zipped
    */
  u8 sw_is_zipped;
  /*!
    is split mode. 1 means split.
    */
  u8 sw_is_spilt;    
  /*!
     file total size
    */
  u32 file_total_size;
}dsmcc_dsi_gi_t;

/*!
  dsi info
  */
typedef struct
{
  /*!
    group number
    */
  u8 group_num;
  /*!
    group number
    */
  u8 is_force;
  /*!
    group information
    */
    dsmcc_dsi_gi_t group_info[MAX_GROUP_NUMBER];
}dsmcc_dsi_info_t;

/*!
  dsmcc module info structure
  */
typedef struct
{
  /*!
     module id
    */
  u16 mod_id;
  /*!
     module size
    */
  u32 mod_size;
  /*!
     module version
    */
  u8 mod_ver;

  ////dasfaf
}dsmcc_module_info_t;

/*!
  dii type info
  */
typedef struct
{
  /*!
    module type(BL,Readonly,RW)
    */
  u8 mod_type;
  /*!
    whether data is valid
    */
  BOOL is_data_valid;
  /*!
    start address of each type
    */
  u32 start_addr;
  /*!
    module number of each type
    */
    u8 module_num;
  /*!
    modules of each type
    */
  dsmcc_module_info_t mod_info[MAX_MODULE_NUM_PER_TYPE];
}dsmcc_dii_type_info_t;

/*!
  dii info
  */
typedef struct
{
  /*!
    total num.
    */
  u8 type_num;
  /*!
    dii type info
    */
  dsmcc_dii_type_info_t dii_type_info[MODULE_TYPE_NUM];
}dsmcc_dii_info_t;
    
/*!
  dsmcc ddb header structure
  */
typedef struct
{
  /*!
     module id
    */
  u16 mod_id;
  /*!
     module version
    */
  u8  mod_ver;
  /*!
     number of blocks
    */
  u16 block_num;
}dsmcc_ddb_hdr_t;

/*!
  dsmcc ddb structure
  */
typedef struct
{
  /*!
    section number
    */
  u8 sec_num;
  /*!
    Last section number
    */
  u8 last_sec_num;
  /*!
    dsmcc ddb header
    */
  dsmcc_ddb_hdr_t ddb_hdr;
  /*!
    content type
    */
  u8 content_type;  
  /*!
    content length
    */
  u16 content_len;
  /*!
    content data
    */
  u8 content[4096];
}dsmcc_ddb_t;

/*!
  dsmcc ddm structure
  */
typedef struct
{
  /*!
    stream es id
    */
  u16 es_id;
  /*!
    current section num
    */
  u16 section_num;
  /*!
    module id
    */
  u16 mod_id;
}ddm_param_t;
/*!
 new dsmcc ddm structure
  */
typedef struct
{
  /*!
    stream es id
    */
  u16 es_id;
  /*!
    group id
    */
  u32 group_id;
  /*!
    type
    */
  u8 type;
}dsmcc_db_req_t;
/*!
  Parse dsmcc db section
  */
void parse_dsmcc_db_section(handle_t handle, dvb_section_t *p_sec);
/*!
  Parse dsi or dii msg section
  */
void parse_dsmcc_dsi_dii_section(handle_t handle, dvb_section_t *p_sec);

/*!
  Request dsmcc dsi or dii section
  */
void request_dsmcc_dsi_dii_section(dvb_section_t *p_sec,u32 para1, u32 para2);
/*!
  Request dsmcc db section
  */
void request_dsmcc_db_multi_section(dvb_section_t *p_sec,u32 para1, u32 para2);


/*!
  Parse dsmcc ddb section

  \param[in] p_buf Section data filtered by PTI driver
  */
void parse_dsmcc_ddm(handle_t handle, dvb_section_t *p_sec);
/*!
  Parse dsmcc msg section

  \param[in] p_buf Section data filtered by PTI driver
  */
void parse_dsmcc_msg(handle_t handle, dvb_section_t *p_sec);

/*!
  Request dsmcc message section

  \param[in] p_sec section data pointer
  \param[in] param Section data filtered by PTI driver
  */
void request_dsmcc_msg_section(dvb_section_t *p_sec,u32 para1, u32 para2);
/*!
  Request dsmcc multi sections of ddm

  \param[in] p_sec section data pointer
  \param[in] para1 para1
  \param[in] para2
  */
void request_dsmcc_msg_multi_section(dvb_section_t *p_sec, u32 para1, u32 para2);
/*!
  Request dsmcc ddm section

  \param[in] p_sec section data pointer
  \param[in] param Section data filtered by PTI driver
  */
void request_dsmcc_ddm_section(dvb_section_t *p_sec, u32 para1, u32 para2);
/*!
  Request dsmcc multi sections of ddm

  \param[in] p_sec section data pointer
  \param[in] para1 para1
  \param[in] para2
  */
void request_dsmcc_ddm_multi_section(dvb_section_t *p_sec,
  u32 para1, u32 para2);

#endif // End for __DSMCC_H_
