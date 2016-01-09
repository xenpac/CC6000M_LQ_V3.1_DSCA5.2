/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_GBOX_H_
#define __AP_GBOX_H_

/*!
  Delcare all command for twin port
  */
typedef enum
{
  /*!
    Set service id
    */
  GBOX_CMD_SET_SID = SYNC_CMD,
  /*!
    turn off gbox
    */
   GBOX_CMD_TURN_OFF = ASYNC_CMD,
  /*!
    Set cw directly
    */
  GBOX_CMD_SET_CW,
 }gbox_cmd_t;

/*!
  Delcare event for twin port
  */
typedef enum
{
  /*!
    the first event of twin port module,
    if create new event, you must insert it between BEGIN and END
    */
  GBOX_EVT_BEGIN = ((APP_IS << 16) | ASYNC_EVT),
  /*!
    Set service id
    */
  GBOX_EVT_RECV_STEP2_DATA_ERR,
  /*!
    Set service id
    */
  GBOX_EVT_RECV_STEP4_DATA_ERR,
  /*!
    Set service id
    */
  GBOX_EVT_CW_SET,
  /*!
    Set service id
    */
  GBOX_EVT_SET_S_ID = ((APP_IS << 16) | SYNC_EVT),
  /*!
    Set service id
    */
  GBOX_EVT_TURN_OFF,
  /*!
    Set service id
    */
  GBOX_EVT_TURN_ON,
  /*!
    the last event of twin port module,
    if create new event, you must insert it between BEGIN and END
    */
  GBOX_EVT_END
}gbox_evt_t;


/*!
  structure for set sid when change channel.
  */
typedef struct
{
  /*!
    svc_id
    */
  u16 svc_id;

  /*!
    pg_id
    */
  u16 pg_id;

  /*!
      Downward Frequency
    */
  u16 freq;
  /*!
      Satellite degree
    */
  u16 sat_degree;
  /*!
      Current audio channel number
    */
  u16 cur_audio_pid;
  /*!
      Current video pid
    */
  u16 cur_video_pid;

  /*!
      Current video ch
    */
  u16 video_channel;

  /*!
      Current audio ch
    */
  u16 audio_channel;
}gbox_info_t;

/*!
  Implementation policy for twin port
  */
typedef struct 
{
  /*!
    Set current program information 
    */
  s16 (*on_set_cur_pg_info)(gbox_info_t *p_pg_info);
  /*!
    set CW by uart port
    */
  BOOL (*on_set_cw)(u8 *p_data,u32 data_len);
  /*!
    Send request by uart
    */
  BOOL (*on_req_cw)(void);
  /*!
    read ack by uart
    */
  BOOL (*on_req_ack)(u8 *p_data,u32 data_len,u32 *p_ret_len);
}gbox_policy_t;

/*!
  Get the implementation of twin port
  return twin port application instance
  \param[in] p_impl_policy implementation policy for twin port
  */
app_t * construct_ap_gbox(void);

#endif // End for __AP_GBOX_H_


