/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_FILTER_DSMCC_INTRA_H_
#define __OTA_FILTER_DSMCC_INTRA_H_

/*!
  Marco for directory in dsmcc content
  */
#define DSMCC_CONTENT_DIR       0x01
/*!
  Marco for file in dsmcc content
  */
#define DSMCC_CONTENT_FILE      0x02
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
   DSMCC BIOP TAG
  */
#define DSMCC_TAG_BIOP      0x49534F06
/*!
   DSMCC connection binder TAG
  */
#define DSMCC_TAG_CONN_BINDER   0x49534F40
/*!
   DSMCC lite options TAG
  */
#define DSMCC_TAG_LITE_OPTIONS    0x49534F05
/*!
   DSMCC object location TAG
  */
#define DSMCC_TAG_OBJECT_LOCATION   0x49534F50

/*!
   DSMCC DII GROUP ID
  */
#define DSMCC_DII_GROUP_ID      0x0080
/*!
   section max number
  */
#define MAX_SECTIONS_PER_MOD 256
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
  /*!
  dsmcc dsi private info data buffer
  */
  u8 *p_info_buffer;
/*!
  dsmcc dsi private info data buffer size
  */
  u32 m_info_size;
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
  section info
  */
typedef struct
{
/*!
  section max number
  */
  BOOL is_received;
/*!
  section start addr
  */
  u32 start_addr;
/*!
  section len
  */
  u32 section_len;
}section_info_t;
/*!
  mod info
  */
typedef struct
{
/*!
  section info
  */
  BOOL invalid_mod;
/*!
  section table
  */
  section_info_t section_info[MAX_SECTIONS_PER_MOD];
}mod_info_t;

/*!
 dsmcc private data.
 */
typedef struct tag_dsmcc_priv
{
  /*!
    stream pid
    */
  u16 data_pid;
  /*!
    group id
    */
  u32 group_id;
  /*!
    ota force,when ota is force status
    */
  BOOL is_ota_force;
  /*!
    ota ts force; o:common;1:ota force into ota;2:factory force all flash ota;
    */
  BOOL is_factory_force;
  /*!
    upg data total length(include head data)
    */
  u32 total_len;
  /*!
    received data length
    */
  u32 received_len;
  /*!
    received data max mod download number
    */
  u32 max_mod_dl_number;
    /*!
    download save buffer
    */
  u8 *p_save_buf;
  /*!
    system all block
    */ 
  ota_data_info_t *p_dsmcc_block;//dsmcc_block[OTA_MAX_BLOCK_NUMBER];
  /*!
 dsmcc dsi info.
 */
  dsmcc_dsi_info_t dsi_info;
/*!
 dsmcc dsi info.
 */
  dsmcc_dii_info_t dii_info;
/*!
  serial stb id start
  */
  u8 stbid_start[128];
/*!
  serial stb id end
  */
  u8 stbid_end[128];
 /*!
   mod download info
   */
   mod_info_t *p_mod_dl_info;
  /*!
    ota filter debug printf
    */
  ota_debug_level_t debug_level;
  /*!
    ota filter debug printf
    */
 ota_printf_t ota_dsmcc_debug_printf;
}dsmcc_priv_t;


#endif // End for __DSMCC_H_
