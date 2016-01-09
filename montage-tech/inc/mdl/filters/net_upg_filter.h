/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NET_UPG_FILTER_H__
#define __NET_UPG_FILTER_H__

/*!
  net protocol
  */
typedef enum tag_net_upg_protocol
{
  /*!
    http
    */
  NET_UPG_BY_HTTP,
  /*!
    ftp
    */
  NET_UPG_BY_FTP,
}net_upg_protocol_t;


/*!
  net upg filter config struct
  */
typedef struct
{
/*!
  customer_id
  */
  u32 customer_id;
/*!
  serial no
  */
  u32 serial_num;
  /*!
    customer string id
    */  
  u8 customer[16];
  /*!
    hardware version
    */
  u32 hw;
  /*!
    hardware version
    */
  u32 sw;
  /*!
    net upg protocol, net_upg_protocol_t
    */
  u8 type;
  /*!
    user name for login ftp
    */
  u8 user_name[16];
  /*!
    password for login ftp
    */
  u8 pwd[16];
  /*!
    ftp path
    */
  u8 ftp[256];
  /*!
    xml path
    */
  u8 xml[16];
  
  u8 post_check;

  u16 post_size;

  u8 post_data[1024];
}net_upg_filter_config_t;


/*!
  net_upg_item_t
  */
typedef struct
{
/*!
  customer_id
  */
  u32 customer_id;
/*!
  serial_start
  */
  u32 serial_start;
/*!
  serial_end
  */
  u32 serial_end;
/*!
  hw
  */
  u32 hw;
/*!
  sw
  */
  u32 sw;
/*!
  url
  */
  u8 url[256];
/*!
  size
  */
  u32 size;
  /*!
  type
  */
  u32 type : 8;
  /*!
    reserved
    */
  u32 reserved : 24;
/*!
  time
  */
  u32 time;
/*!
  version description
  */
  u8 version_des[256];
}net_upg_item_t;

/*!
  net_upg_xml_info_t
  */
typedef struct
{
/*!
  cnt
  */
  u16 item_cnt;
  /*!
    reserved
    */
  u16 reserved;
/*!
  items
  */
  net_upg_item_t item[64];
}net_upg_xml_info_t;

/*!
  net_upg_update_cfg_t
  */
typedef struct tag_net_upg_update_cfg
{
/*!
  sw
  */
  u32 sw;
  /*!
    flash_addr_offset
    */
  u32 flash_addr_offset;
  /*!
    flash_burn_size
    */
  u32 flash_burn_size;
  /*!
    main code offset addr
    */
  u32 main_code_offset_addr;
  /*!
    p_flash_buf
    */
  u8 *p_flash_buf;
  /*!
    flash_buf_size
    */
  u32 flash_buf_size;
  /*!
     flash_url
  */
  u8 p_url[512];
  /*!
  flash_is_skip_some_addr
  */
  u8  flash_is_skip;
  /*!
  flash_skip_block_number
 */
  u8  flash_skip_block_num;
  /*!
  flash_is_check_header_addr
 */
  u8  flash_is_check_header_addr;
  /*!
  flash_skip_number
 */
  u32  flash_skip_addr;
  /*!
  flash_check_header_addr
 */
  u32  flash_check_header_addr;
}net_upg_update_cfg_t;

/*!
  net_upg_filter_evt_t
  */
typedef enum
{
  /*!
    check evt
    */
  NET_UPG_FILTER_EVT_CHECK,
  /*!
    net upg progress
    */
  NET_UPG_FILTER_EVT_PROGRESS,
  /*!
    net upg fail
    */
  NET_UPG_FILTER_EVT_FAIL,
}net_upg_filter_evt_t;

/*!
  vod filter command define
  */
typedef enum tag_net_upg_filter_cmd
{
  /*!
    config params see p_cmd->p_para = net_upg_config_t
    */
  NET_UPG_FILTER_CMD_CFG_PARAMS,
  /*!
    check wether have new version on ftp
    */
  NET_UPG_NEW_VERSION_CHCECK,
  /*!
    start to update, set a url version
    */
  NET_UPG_UPDATE_START_DOWNLOAD,
  /*!
    start burn flash
    */
  NET_UPG_UPDATE_START_BURN,
  /*!
    start upg
    */
  NET_UPG_STOP,
}net_upg_filter_cmd_t;



/*!
  vod_filter_create
  */
ifilter_t * net_upg_filter_create(void *p_para);

#endif

