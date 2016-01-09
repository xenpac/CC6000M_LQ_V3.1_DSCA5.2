/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_FILTER_DSMCC_H_
#define __OTA_FILTER_DSMCC_H_

/*!
   the buffer size of DSMCC DDM
  */
#define DSMCC_DDM_BUF_SIZE  (5 * 1024)
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
  md5 head mark
  */
#define MD5_HEAD_MARK 0x4D443548

typedef struct tag_md5_ex_info
{

  /*!
    the header mark
    */
  u32 mark;
  /*!
    the head info size
    */
  u32 header_info_size;
  /*!
    the head info size
    */
  u32 header_info_crc;
    /*!
    the head reserve
    */
  u32 reserve1;
  /*!
    the md5 value
    */
  u8 md5_value[16];
  /*!
    the ota display version value
    */
  u8 sw_disp_version[32];
  /*!
    the ota auto switch
    */     
  BOOL g_ota_auto;
}md5_ex_head_info_t;


/*!
  dsmcc db pare status
  */
  typedef struct
{
  /*!
  dsmcc db pare progress
  */
  u8 progress_all;
/*!
  dsmcc db pare finish bot
  */
  BOOL finish_bit;
}dsmcc_pare_db_t;

/*!
  dsmcc para private init
  */
RET_CODE dsmcc_parse_init(ota_data_info_t *p_block,
                                                          ota_debug_level_t debug_level,
                                                          ota_printf_t printf);
/*!
  dsmcc para private destory init
  */
void dsmcc_parse_deinit(void);
/*!
  dsmcc para clear private dii info
  */
void dsmcc_parse_clear_dii_info(void);
/*!
  dsmcc para clear private dsi info
  */
void dsmcc_parse_clear_dsi_info(void);

/*!
  dsmcc para process download finish
  */
BOOL dsmcc_pare_download_finish(void);
/*!
  dsmcc para process download init
  */
void dsmcc_download_init(u8 *p_save_buf,u32 total_szie);

/*!
  dsmcc para ddm head info to ota block
  */
BOOL dsmcc_parse_add_ddm_info_to_ota_block(u8 *p_buf,
                                      BOOL is_force,
                                      BOOL is_data,
                                      ota_block_data_verify_t function);
/*!
  dsmcc para check data for donwnload
  */
BOOL dsmcc_parse_check_data_for_download(BOOL is_force,
                                      ota_block_info_t *p_sys_block,
                                      ota_block_info_t *p_re_block,
                                      ota_version_check_t ota_version_check,
                                      BOOL g_dmh_check);
/*!
 dsmcc parse dsmcc get dsi upg info.
 */
BOOL dsmcc_parse_get_dsi_upg_info(u8 group_id,
                                                      upg_check_info_t *p_upg_info);
/*!
  dsmcc para check parse check dii info
  */
BOOL dsmcc_parse_check_dii_info(ota_block_info_t *p_sys_block,
                                      ota_block_info_t *p_re_block);
/*!
 dsmcc parse dsmcc check dsi upg info.
 */
BOOL dsmcc_parse_check_dsi_upg_info(ota_hw_tdi_t hw_tdi,
                                        u32 old_version, 
                                        upg_check_info_t *p_upg_info,
                                        ota_version_check_t ota_version_check,
                                        ota_dsi_info_check_t ota_dsi_info_check);

/*!
  dsmcc para add dii info
  */
BOOL dsmcc_parse_add_dii_info(u16 group_id);

/*!
  request dsmcc data base multi section
  */
void request_dsmcc_db_multi_section_v2(psi_request_data_t *p_req_data,
                                      u32 para1,
                                      u32 para2);

/*!
  request dsmcc dsi or dii section
  */
void request_dsmcc_dsi_dii_section_v2(psi_request_data_t *p_req_data,
                                        u32 para1,
                                        u32 para2);
/*!
  request dsmcc dsi or dii section
  */
void request_dsmcc_dsi_dii_multi_section_v2(psi_request_data_t *p_req_data,
                                        u32 para1,
                                        u32 para2);
/*!
 dsmcc parse dsmcc table info.
 */
BOOL parse_dsmcc_db_section_v2(u8 *p_pare_buf,u32 size,
                                      dsmcc_pare_db_t *p_status,
                                      u32 group_id,u8 *p_save_buf);
/*
 dsmcc parse dsmcc table info.
 */
void parse_dsmcc_reset_db_dl_info(void);


/*!
 dsmcc parse dsmcc dsi or dii.
 */
u16 parse_dsmcc_dsi_dii_section_v2(u8 *p_data,u32 data_size);
/*!
 dsmcc parse dsmcc check dsi info.
 */
BOOL dsmcc_parse_check_dsi_info(ota_hw_tdi_t hw_tdi);

#endif // End for __DSMCC_H_
