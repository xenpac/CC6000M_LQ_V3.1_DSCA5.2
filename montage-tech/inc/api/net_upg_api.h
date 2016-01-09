/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NET_UPG_API_H_
#define __NET_UPG_API_H_



/*!
  net_upg_api_evt_t
  */
typedef enum
{
/*!
  check done
  */
  NET_UPG_EVT_CHECK_DONE,
/*!
  progress
  */
  NET_UPG_EVT_PROGRESS,
/*!
  net upg fail
  */
  NET_UPG_EVT_FAIL,
}net_upg_api_evt_t;

/*!
  net_upg_item_t
  */
typedef struct
{
/*!
  cus id
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
}net_upg_api_item_t;

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
  net_upg_api_item_t item[64];

}net_upg_api_info_t;

/*!
  net upg config struct
  */
typedef struct
{
/*!
  cus id
  */
  u32 customer_id;
/*!
  serial no.
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
    hardware version
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
  
}net_upg_config_t;

/*!
  net_upg_api_up_cfg_t
  */
typedef struct tag_net_upg_api_up_cfg
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
  /*
   p_url
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
}net_upg_api_up_cfg_t;

/*!
  vod_call_back
  */
typedef void(*net_upg_call_back)(net_upg_api_evt_t event, void *p_data, u16 context);

/*!
  FIXME
  */
typedef struct 
{
/*!
  FIXME
  */
  u32 stk_size;
/*!
  cb see vod_call_back
  */
  net_upg_call_back cb;
}mul_net_upg_attr_t;

/*!
 * Create filter
 */
RET_CODE mul_net_upg_init(void);
/*!
 * mul_vod_chn_create
 */
RET_CODE mul_net_upg_chn_create(mul_net_upg_attr_t *p_attr);
/*!
 * mul_vod_chn_destory
 */
RET_CODE mul_net_upg_chn_destroy(u32 chanid);
/*!
 * mul_vod_start
 */
RET_CODE mul_net_upg_start_check(net_upg_config_t *p_cfg);
/*!
 * mul_vod_stop
 */
RET_CODE mul_net_upg_stop(void);

/*!
 * mul_net_upg_start_upgrade
 */
RET_CODE mul_net_upg_start_upgrade(net_upg_api_up_cfg_t *p_cfg);

/*!
 * mul_net_upg_start_burn_flash
 */
RET_CODE mul_net_upg_start_burn_flash(void);

#endif //

