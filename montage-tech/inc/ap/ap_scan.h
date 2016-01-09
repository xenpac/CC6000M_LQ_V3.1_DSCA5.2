/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SCAN_H_
#define __AP_SCAN_H_

/*!
  \file ap_scan.h

  Declare scan application and scan.h an base class for all kinds of scan
  process like dvbs scan, dvbs tp scan, dvbs blind scan and considering
  dvbc, dvbt it should support also.
  */

/*!
  Max message number received in App. Scan
  */
#define SCAN_MSG_NUM (32)

/*!
  Max tp number supported in TP Scan
  */
#define MAX_TP_NUM_PER_SAT (300*2)//twice tp scan

/*!
  Max number of programs in one TP
  */
#define SCAN_MAX_PG_PER_TP (256)

/*!
  Max audio channel number
  */
#define SCAN_MAX_AUDIO_CHANNEL (16)

/*!
  Max service name length supported
  */
#define MAX_SVC_NAME_SUPPORTED (32)

/*!
  Delcare all command for scan
  */
typedef enum
{
  /*!
    Start scan, with one parameter @see scan_parameter
    */
  SCAN_CMD_START_SCAN = 0, 
  /*!
    Pause scan, with one parameter @see scan_parameter
    */
  SCAN_CMD_PAUSE_SCAN,
  /*!
    Resume scan, with one parameter @see scan_parameter
    */
  SCAN_CMD_RESUME_SCAN, 
  /*!
    Cancel scan
    */
  SCAN_CMD_CANCEL_SCAN = SYNC_CMD
}scan_cmd_t;

/*!
  Delcare get scramble origin
  */
typedef enum
{
  /*!
    Scan from ts packet
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
}scan_scramble_origin_t;
/*!
  Declare all events which will be notified to high level modules like
  UI
  */
typedef enum
{
  /*!
    Notify current scaning progress and current signal information
    */
  SCAN_EVT_PROGRESS = (APP_SCAN << 16),
  /*!
    Notify one program was found
    */
  SCAN_EVT_PG_FOUND,
  /*!
    Notify one TP is found
    */
  SCAN_EVT_TP_FOUND,
  /*!
    Notify one NIT table is found
    */
  SCAN_EVT_NIT_FOUND,
  /*!
    Notify one tp pg list done
    */
  SCAN_EVT_TP_PG_LIST_DONE,
  /*!
    Notify current satellite information
    */
  SCAN_EVT_SAT_SWITCH,
  /*!
    Notify break scan
    */
  SCAN_EVT_NO_MEMORY,
  /*!
    Notify tp locked
    */
  SCAN_EVT_TP_LOCKED,
  /*!
    Notify tp unlocked
    */
  SCAN_EVT_TP_UNLOCKED,
  /*!
    The end of async events and all the sync events defined in 
    ap_tp_scan.c or ap_blind_scan.c should be starting from here
    */
  SCAN_EVT_ASYNC_END,
  /*!
    Scan finished
    */
  SCAN_EVT_FINISHED = ((APP_SCAN << 16) | SYNC_EVT),
  /*!
    SYNC event end and all the sync events defined in 
    ap_tp_scan.c or ap_blind_scan.c should be starting from here
    */
  SCAN_EVT_SYNC_END
} scan_evt_t;

/*!
  define handle
  */
typedef void * policy_handle_t;

/*!
  Program infomation
  */
typedef struct
{
  /*!
    service number in stream
    */
  u32 s_id                    : 16;
  /*!
    Original network id
    */
  u32 orig_net_id             : 16; 
  /*!
    video pid
    */
  u32 video_pid               : 13;
  /*!
    pcr pid
    */
  u32 pcr_pid                 : 13;
  /*!
    scrambled flag 0: not scrambled, 1: scrambled
    */
  u32 is_scrambled            : 1;
  /*!
    Audio channel existing
    */
  u32 audio_ch_num            : 5;
  /*!
    logical channel num
    */
  u32 logical_num             : 16;
  /*!
    volume compensate
    */
  u32 volume_compensate       : 8;
  /*!
    video_type
    */
  u32 video_type                : 8;
  /*!
    bouquet id
    */
  u32 bouquet_id              : 16;
  /*!
    reserve
    */
  u32 reserve                 : 10;
  /*!
    program ecm number
    */
  u32 ecm_num                :6;
  /*!
    ecm desc
    */
  cas_desc_t ca_ecm[MAX_ECM_DESC_NUM];
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
    plp pid
    */
  u8 plp_id;
    /*!
    plp pid index of plp array
    */
  u8 plp_index;

  /*!
    mosaic descriptor(service_type = 0x06)
    */
  mosaic_t *p_mosaic;
 /*!
  running status
  */
  u8 running_status;
} scan_pg_info_t;

/*!
  Transpond information, compatiable with DVBS/DVBC/DVBT
  */
typedef struct
{
  /*!
    Nim control channel infomation @see nc_channel_info_t
    */
  nc_channel_info_t tp_info;
  /*!
    Program num
    */
  u32 pg_num;

  /*!
    Program list
    */
  scan_pg_info_t *p_pg_list;
} scan_tp_info_t;

/*!
  Singleton patern interface which can get a scan application's
  instance
  
  \return instance address
  */
app_t *construct_ap_scan(void);

#endif // End for __AP_SCAN_H_
