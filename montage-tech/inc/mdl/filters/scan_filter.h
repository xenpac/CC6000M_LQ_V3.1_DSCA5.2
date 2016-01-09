/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SCAN_FILTER_H_
#define __SCAN_FILTER_H_

/*!
  Max number of programs in one TP, emqul to DVB_MAX_PAT_PROG_NUM(in the dvb_protocol.h)
  */
#define SCAN_MAX_PG_PER_TP          DVB_MAX_PAT_PROG_NUM

/*!
  Max audio channel number
  */
#define SCAN_MAX_AUDIO_CHANNEL      (16)

/*!
  Max service name length supported
  */
#define MAX_SVC_NAME_SUPPORTED (32)

/*!
  max sid number
  */
#define MAX_SVC_ID_NUM            (65536)

/*!
  invalid prog offset.
  */
#define INVALID_PROG_OFFSET       (0xFF)
           
/*!
  Max pid number supported, pid is a 13-bit number
  */
#define MAX_PID_NUMBER            (8 * 1024)

/*!
  Max satellite name supported
  */
#define MAX_BOUQUET_ID_IN_SCAN (64)



/*!
  nit scan mode
  */
typedef enum tag_scan_nit_type
{
 /*!
  scan without request nit
  */
  SCAN_NIT_WITHOUT = 0,
 /*!
  request nit on all tp
  */ 
  SCAN_NIT_ALL_TP,
 /*!
  request nit once
  */
  SCAN_NIT_ONCE,
}scan_nit_type_t;

/*!
  bat scan mode
  */
typedef enum tag_scan_bat_type
{
 /*!
  scan without request bat
  */
  SCAN_BAT_WITHOUT = 0,
 /*!
  request bat on all tp
  */
  SCAN_BAT_ALL_TP,
}scan_bat_type_t;

/*!
  cat scan mode
  */
typedef enum tag_scan_cat_type
{
 /*!
  scan without request cat
  */
  SCAN_CAT_WITHOUT = 0,
 /*!
  request cat on all tp
  */
  SCAN_CAT_ALL_TP,
}scan_cat_type_t;

/*!
  Delcare get scramble origin
  */
typedef enum tag_scan_scramble_type
{
 /*!
  scan from ts packet
  */
  SCAN_SCRAMBLE_FROM_TS, 
 /*!
  scan from pmt
  */
  SCAN_SCRAMBLE_FROM_PMT,
/*!
  scan from pmt first, if not found scramble flag, then request ts packet
  */
  SCAN_SCRAMBLE_PMT_THEN_TS,
}scan_scramble_type_t;


/*! 
  scan_s program info
  */
typedef struct tag_scan_pg_info
{
 /*!
  service number in stream
  */
  u32 s_id                : 16;
 /*!
  Original network id
  */
  u32 orig_net_id         : 16; 
 /*!
  video pid
  */
  u32 video_pid           : 13;
 /*!
  pcr pid
  */
  u32 pcr_pid             : 13;
 /*!
  scrambled flag 0: not scrambled, 1: scrambled
  */
  u32 is_scrambled        : 1;
 /*!
  Audio channel existing
  */
  u32 audio_ch_num        : 5;
 /*!
  logical channel num
  */
  u32 logical_num         : 16;
 /*!
  volume compensate
  */
  u32 volume_compensate   : 8;
 /*!
  video_type
  */
  u32 video_type          : 8;
 /*!
  bouquet id
  */
  u32 bouquet_id          : 16;
 /*!
  reserve
  */
  u32 reserve             : 10;
 /*!
  program ecm number
  */
  u32 ecm_num             :6;
 /*!
  ecm desc
  */
  cas_descriptior_t ca_ecm[MAX_ECM_DESC_NUM];
 /*!
  Audio channel list
  */
  audio_t audio[SCAN_MAX_AUDIO_CHANNEL];
 /*!
  program name
  */
  u8 name[MAX_SVC_NAME_SUPPORTED];
 /*!
  FLAG for recording whether program scramble state in video packet is found
  */
  BOOL is_scramble_found;
 /*!
  service type
  */
  u8 service_type;
 /*!
  mosaic flag found
  */
  u8 mosaic_flag;
 /*!
  reserved
  */
  u16 reserved;
 /*!
  stream id
  */
  u16 ts_id;
 /*!
  CA system id
  */
  u16 ca_system_id;
 /*!
  pmt pid
  */
  u16 pmt_pid;
 /*!
  mosaic descriptor(service_type = 0x06)
  */
  mosaic_t *p_mosaic;
 /*!
  running status
  */
  u8 running_status;
}scan_pg_info_t;

/*!
  scan_s running information. 
  */
typedef struct tag_scan_filter_result
{
 /*!
  program number. 
  */
  u32 pg_num;
 /*!
  scan_s program array
  */
  scan_pg_info_t pg_list[SCAN_MAX_PG_PER_TP];
}scan_filter_result_t;

/*!
  scan filter event define
  */
typedef enum tag_scan_filter_evt
{
 /*!
  scan finish packing, mutual information is scan_s_pg_list_t.
  */
  SCAN_FILTER_FINISH_PACKING = SCAN_FILTER << 16,
}scan_filter_evt_t;

/*!
 defines scan filter policy structure.
 */
typedef struct tag_scan_filter_policy
{
 /*!
  the extend handle
  */
  handle_t ext_this;
 /*!
  extend sample process callback function
  */
  RET_CODE (*extend_media_proc)(handle_t _this, media_type_t media_type, u8 *p_table);
 /*!
  request extend media enable
  */
  BOOL (*is_require_media)(handle_t _this, media_type_t media_type);
/*!
  bouque id list.
  */
  u8 (*get_bouquet_id_list)(handle_t _this, u16 *p_id_list);
 /*!
  get buffer in the runing time.
  */
  void *(*alloc_run_buf)(handle_t _this, u32 size, char *p_name);  
 /*!
  get scramble flag origin
  */
  scan_scramble_type_t (*get_scramble_origin)(handle_t _this);
}scan_filter_policy_t;

/*!
  scan filter command define
  */
typedef enum tag_scan_filter_cmd
{
 /*!
  scan filter attach policy callback 
  */ 
  SCAN_FILTER_ATTACH_POLICY,
 /*!
  unknown.
  */  
  SCAN_FILTER_UNKNOWN,    
}scan_s_filter_cmd_t;


/*!
  create a scan filter instance

  \return return the instance of filter
  */
ifilter_t * scan_filter_create(void *p_para);


#endif // End for __SCAN_FILTER_H_
