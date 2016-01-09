/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SUBT_STATION_INPUT_PIN_H__
#define __SUBT_STATION_INPUT_PIN_H__

/*!
  LINE_MAX_SIZE
  */
#define LINE_MAX_SIZE (512)
/*!
  tag_subt_station_in_report.
  */
typedef struct tag_subt_station_in_report
{
  /*!
    dummy
    */
  u32 dummy;
}subt_station_in_report_t;

/*!
  subt_station_in_status_t
  */
typedef enum
{
  SST_IN_STATE_ON_CYCLE_DONE,
  /*!
    SST_IN_STATE_REQUEST
    */
  SST_IN_STATE_REQUEST,
  /*!
    SST_IN_STATE_GET_LINE
    */
  SST_IN_STATE_GET_LINE,
  /*!
    SST_IN_STATE_PARSE
    */
  SST_IN_STATE_PARSE,
  /*!
    SST_IN_STATE_END
    */
  SST_IN_STATE_END
}subt_station_in_status_t;


/*!
  subt_station_in_pin_priv_t
  */
typedef struct tag_subt_station_in_pin_private
{
  /*!
    this point !!
    */
  u8 *p_this;

  /*!
    private data buffer
    */
  subt_sys_type_t sst_type;
  /*!
    codec fmt
    */
  subt_codec_fmt_t codec_fmt;  
  /*!
    stream_mode
    */
  BOOL enable_pre_scan;
  /*!
    report for debug
    */
  subt_station_in_report_t report;
  /*!
    mem alloc interface
    */
  iasync_reader_t *p_reader;
  /*!
    parse_pos
    */
  u32 parse_pos;
  /*!
    this buffer used for outside subt pre scan 
    */
  u8 p_pre_scan_buf[LINE_MAX_SIZE];
  /*!
    scan_state
    */
  subt_station_in_status_t scan_state;
  /*!
    parse_state
    */
  subt_station_in_status_t parse_state;
  /*!
    last parse item start
    */
  u32 last_start;
  u32 last_end;
}subt_station_in_pin_priv_t;


/*!
  the sst_internal_msg_t
  */
typedef enum
{
  /*!
    pts update
    */  
  SST_INT_PTS_UPDATE,
  /*!
    show 
    */  
  SST_INT_SHOW_SUBT,
  /*!
    hide
    */  
  SST_INT_HIDE_SUBT
  
}sst_internal_msg_t;


/*!
  the pic input pin
  */
typedef struct tag_subt_station_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data buffer
    */
  subt_station_in_pin_priv_t private_data;
}subt_station_in_pin_t;


/*!
  subt_station_in_pin_create
  */
subt_station_in_pin_t *subt_station_in_pin_create(
                          subt_station_in_pin_t *p_pin, interface_t *p_owner);

/*!
  sst_in_pin_process_sample
  */
RET_CODE sst_in_pin_process_internal_subt(handle_t _this, media_sample_t *p_sample);

/*!
  sst_in_pin_process_sample
  */
RET_CODE sst_in_pin_pre_parse_in_subt(handle_t _this, media_sample_t *p_sample);

/*!
  sst_in_pin_request_data
  */
RET_CODE sst_in_pin_request_data(handle_t _this, u32 start, u32 pos, BOOL seek);

/*!
  sst_in_pin_pre_scan
  */
RET_CODE sst_in_pin_pre_scan(handle_t _this);
/*!
  sst_in_pin_config
  */
void sst_in_pin_config(handle_t _this, u8 type, u8 is_internal);


#endif

