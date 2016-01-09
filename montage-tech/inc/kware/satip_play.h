/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SATIP_PLAY_H__
#define __SATIP_PLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  commments
*/
typedef struct program_param_s
{
    /*!
      commments
    */
    u16 program_id;
    /*!
      commments
    */
    u8 program_type;
    /*!
      commments
    */
    u32 freq;
    /*!
      commments
    */
    u32 symb_rate;
    /*!
      commments
    */
    u8 onoff_22k;
    /*!
      commments
    */
    u8 polarization;
    /*!
      commments
    */
    u16 vidpid;
    /*!
      commments
    */
    u16 audpid;
    /*!
      commments
    */
    u16 pcrpid;
    /*!
      commments
    */
    u16 pmtpid;
    /*!
      [7:4]---dmx record mode
      [3:0]---stream type
    */
    u8 is_des;
}program_param_t;

/*
  call back function to inform stb to change program for TV
  /p_program_param: program information
 */
typedef void (*dmx_changeprogram_t)(program_param_t *p_program_param);

/*!
  Define the dmx socket call back function's prototype
  */
typedef void (*dmx_data_cb_t)(u8 *p_data, u32 length, u8 index, u8 ptype);


/*!
  DMX play init API.

  \param[in] stack_size : data task szie
  \param[in] task_piro : data task prio
  \param[in] conn_num : connection_num
  \param[in] nim_type : nim type
  \param[in] stream_type : dmx stream in type
  \param[in] recoder_buf_size : recoder buf size
  \param[out] Return TRUE success, FALSE failed
  */
BOOL dmx_network_play_init(int stack_size[], int task_piro[], u8 conn_num,
                    u8 nim_type, u8 stream_type, u32 recoder_buf_size);

/*!
  dmx start play API.
  Only support DVB-S DVB-S2 right now.

  \param[in] pointer : tv program param
  \param[in] freq : Input freq in MHz(950~2150)
  \param[in] symb_rate : Input symb_rate in KSs
  \param[in] onoff_22k : Input 22k on/off(1/0)
  \param[in] polarization : Input polarization, 0:H; 1:V; 2:L; 3:R
  \param[in] vidpid : video pid
  \param[in] audpid : audio pid
  \param[in] pcrpid : pcr pid
  \param[in] pmtpid : pmt pid
  \param[in] p_data_cb : data callback
  \param[in] is_des : [7:4] dmx record mode [3:0] stream type
  \param[out] Return TRUE success, FALSE failed
 */
BOOL dmx_network_start_filter_data(void *pointer);

/*!
  Satip start play API.
  Only support DVB-S DVB-S2 right now.

  \param[in] index: current play task index
  \param[in] protol_type: current play using protol
  \param[out] Return TRUE success, FALSE failed
  */
BOOL dmx_network_start_play(u8 index, u8 protol_type, dmx_data_cb_t p_data_cb);

/*!
  dmx stop filter data API.
  Only support DVB-S DVB-S2 right now.

  \param[out] Return TRUE success, FALSE failed
  */
BOOL dmx_network_stop_filter_data(void);

/*!
  dmx stop play API.
  \param[in] index: current play task index
  \param[out] Return TRUE success, FALSE failed
  */
BOOL dmx_network_stop_play(u8 index);

/*!
  dmx play exit API.

  \param[out] Return TRUE success, FALSE failed
  */
BOOL dmx_network_exit_play(void);

#ifdef __cplusplus
}
#endif

#endif //__SATIP_PLAY_H__

