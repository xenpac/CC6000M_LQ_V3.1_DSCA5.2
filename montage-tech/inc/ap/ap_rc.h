/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __AP_RC_H__
#define  __AP_RC_H__

/*!
  define max program per packet can trans
  */
#define MAX_PG_PER_PACKET  (20)
/*!
  Delcare all command for scan
  */
typedef enum
{
  /*!
    Start scan, with one parameter @see scan_parameter
    */
  RC_CMD_START_RUN = 0, 
  /*!
    RC_CMD_SET_TEXT_MODE
    */
  RC_CMD_SET_TEXT_MODE,
  /*!
    RC_CMD_EXIT_TEXT_MODE
    */
  RC_CMD_EXIT_TEXT_MODE,
  /*!
    RC_CMD_START_EPG
    */
  RC_CMD_START_EPG,
  /*!
    RC_CMD_STOP_EPG
    */
  RC_CMD_STOP_EPG,
  /*!
    stop
    */
  RC_CMD_STOP = SYNC_CMD,
  
}rc_cmd_t;


/*!
  Declare all events
  */
typedef enum
{
  /*!
    Notify RC_EVT_KEY
    */
  RC_EVT_KEY = (APP_RC << 16),
  /*!
    Notify RC_EVT_INPUT
    */
  RC_EVT_INPUT,
  /*!
    Notify RC_EVT_DVB_PLAY
    */
  RC_EVT_DVB_PLAY,  
  /*!
    Notify RC_EVT_DVB_PLAY
    */
  RC_EVT_MEDIA_IP_PLAY,    
  /*!
    RC_EVT_MEDIA_IP_DISCONNECT
    */
  RC_EVT_MEDIA_IP_DISCONNECT,
  /*!
    Notify RC_EVT_DVB_PLAY
    */
  RC_EVT_MEDIA_IP_PLAY_STOP,   
    /*!
    Notify RC_EVT_DVB_PLAY
    */
  RC_EVT_SERVER_CHANGE_CH,  
  /*!
    Notify RC_EVT_STOP_DONE
    */
  RC_EVT_STOP_DONE = ((APP_RC << 16) | SYNC_EVT),
  /*!
    Notify one program was found
    */
  RC_EVT_END
} rc_evt_t;

/*!
  Declare rc_init_param_t
  */
typedef struct
{
  /*!
    max_connect_num
    */
  u8 max_connect_num;

  BOOL enable_epg;
}rc_init_param_t;


/*!
  Playback instance policy interface declaration
  */
typedef struct
{
  /*!
    \see _init_play, set sdt,nit,do dynamic pid switch
    */
  void (*init)(rc_init_param_t *p_init_param);
  /*!
    get_channel_list
    */
  void (*get_channel_list)(u16 page_idx, rc_xml_pg_root_t *p_root);
  /*!
    get_channel_list
    */
  void (*get_channel_list_by_id)(u16 pg_start, u8 cnt, rc_xml_pg_root_by_id_t *p_root);
  /*!
    get_channel_list
    */
  void (*get_tp_channel_list)(u16 page_idx, u16 tp_id, rc_xml_pg_root_t *p_root);
  /*!
    get_channel_list
    */
  void (*get_tp_channel_list_by_id)(u16 pg_start, u8 cnt, u16 tp_id,
                                                    rc_xml_pg_root_by_id_t *p_root);
  /*!
    get_cur_play_info
    */
  BOOL (*get_cur_play_info)(rc_xml_pg_t *p_pg);
  /*!
    get_ext_buf_addr
    */
  u32 (*get_ext_buf_addr)(u8 idx);
   /*!
    set_auth
    */
  u32 (*set_auth)(rc_xml_auth_t *p_auth);
    /*!
    get_auth
    */
  u32 (*get_auth)(rc_xml_auth_t *p_auth);
  /*!
    get_channel_list
    */
  void (*custom_key_trans)(u16 *output_key, u16 input_key);  
  /*!
    policy private data
    */
  void *p_data;
} rc_policy_t;

/*!
  Singleton patern interface which can get a rc application's
  instance
  
  \return instance address
  */
app_t * construct_ap_rc(rc_policy_t *p_policy);
/*
   send rtsp ack
*/
void ap_rc_rtsp_callback(char *clientAddr, int code);
/*
   ap_rc_send_epg_data
*/
void ap_rc_send_epg_data(u8 *p_addr, u32 len);
#endif


