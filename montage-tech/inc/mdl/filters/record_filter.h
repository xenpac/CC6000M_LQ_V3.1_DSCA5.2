/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RECORD_FILTER_H_
#define __RECORD_FILTER_H_

/*!
  the min buffer used for ts record
  */
#define REC_BUFFER_UNIT_SIZE (800 * 188)

/*!
  file source filter command define
  */
typedef enum tag_record_filter_cmd
{
  /*!
    config program
    */
  RECORD_CFG_PROGRAM,
  /*!
    config pid
    */
  RECORD_CFG_PROGRAM_PID,
  /*!
    time shift
    */
  RECORD_FILTER_CMD_TIMESHIFT_CFG,
}record_filter_cmd_t;

/*!
  record filter event define
  */
typedef enum tag_record_filter_evt
{
  /*!
    RECORD time update
    */
  RECORD_MSG_RECORDED_TIME_UPDATE = RECORD_FILTER << 16,
}record_filter_evt_t;

/*!
  tag_record_filter_program_pid
  */
typedef struct tag_record_filter_program_pid
{
  /*!
    record mode
    */
  u32 record_mode;
  /*!
    video pid
    */
  u16 v_pid;
  /*!
    audio pid
    */
  u16 a_pid;
  /*!
    pcr pid
    */
  u16 pcr_pid;
  /*!
    vdec_src_fmt_t
    */
  u8 video_fmt;
  /*!
    adec_src_fmt_vsb_t
    */
  u8 audio_fmt;
  /*!
    ext pid count
    */
  u16 ext_pid_cnt;
  /*!
    ext pid
    */
  u16 ext_pid[REC_MAX_EXTERN_PID];
}rec_filter_pg_pid_t;

/*!
  create a record filter instance
  filter ID: RECORD_FILTER
  \return return the instance of record_filter_t
  */
ifilter_t * record_filter_create(void *p_para);

#endif // End for __RECORD_FILTER_H_

