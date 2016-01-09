/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __AV_MATCH_PTI_H_
#define __AV_MATCH_PTI_H_

/*!
    Max program on a TP
  */
#define PG_PER_TP 64
/*!
    Max PID number
  */
#define MAX_PID_NUM     300
/*!
    Use the max filter number
  */
#define MAX_USED_FILT_SUM 90
/*!
    Pes data size
  */
#define PES_SIZE            (3*188)

/*!
    Channel buffer size
  */
#define CHANNEL_BUF_SIZE    (8*188)

/*!
    Max number of programs in flash and view depth of available tp node
  */
#define DB_DVBS_MAX_PRO_KWAR  50

/*!
    Radio program number
  */
#define PG_RADIO_NUM 16

/*!
  audio and video pid.
  */
typedef struct
{
  /*!
  video pid.
  */
  u16 v_pid;
  /*!
  audio pid 0.
  */
  u16 a_pid0;
  /*!
  audio pid 1.
  */
  u16 a_pid1;
  /*!
  audio pid 2.
  */
  u16 a_pid2;
  /*!
  audio pid 3.
  */
  u16 a_pid3;
  /*!
  audio pid 4.
  */
  u16 a_pid4;
}avpid_t;

/*!
  pes packet type.
  */
typedef enum
{
  /*!
  undef packet.
  */
  PES_STREAM_ID_UNDEF = 0,
  /*!
  error data packet.
  */
  PES_STREAM_ID_ERROR,
  /*!
  video packet.
  */
  PES_STREAM_ID_VIDEO,
  /*!
  audio packet.
  */
  PES_STREAM_ID_AUDIO
} pes_stream_id_t;


/*!
  pes information.
  */
typedef struct
{
  /*!
  video or audio pid
  */
  u32             pid;
  /*!
  filter index
  */
  s32             filter_index;
  /*!
  the filter is used
  */
  BOOL            b_used;
  /*!
  stream id
  */
  pes_stream_id_t stream_id;
  /*!
  pes size
  */
  u8              pes_data[PES_SIZE];
  /*!
  pes buffer size
  */
  u32             buf_size;
  /*!
  data size
  */
  u32             data_size;
  /*!
  cont cnt
  */
  u8              cont_cnt;
  /*!
  ts packet data
  */
  u8              ts_packet[CHANNEL_BUF_SIZE + 188];
  /*!
  pes buffer size
  */
  u32             point;
  /*!
  start time
  */
  u32             tick_start;
  /*!
  data arrive
  */
  BOOL            data_arrived;
  /*!
  wnp
  */
  BOOL            wnp;
  /*!
  scan cnt
  */
  u32             scan_cnt;
  /*!
  audio or video pts
  */
  u32             pts;
  /*!
  pts delta
  */
  u32             pts_delta[3];
  /*!
  audio pid index
  */
  u8              a_pid_inx;
} pes_t;


/*!
  PID scan information
  */
typedef struct
{
  /*!
  PTI point
  */
  dmx_device_t *p_pti;
  /*!
  Scan step
  */
  u16 step;
  /*!
  Cur try pid
  */
  u16 cur_try_pid;
  /*!
  Useing filter number
  */
  u16 using_filter_num;
  /*!
  PID total
  */
  u16 pid_total;
  /*!
  AV list
  */
  avpid_t av_list[PG_PER_TP];
  /*!
  Video pid list
  */
  u16 v_pid_list[PG_PER_TP];
  /*!
  Video pid total
  */
  u16 v_pid_total;
  /*!
  Audio pid list
  */
  u16 a_pid_list[PG_PER_TP];
  /*!
  Audio pid total
  */
  u16 a_pid_total;
  /*!
  Used pid list
  */
  u16 u_pid_list[MAX_PID_NUM];
  /*!
  Used pid total
  */
  u16 u_pid_total;
  /*!
  PES list
  */
  pes_t pes_list[MAX_USED_FILT_SUM];
  /*!
   Radio pg list
  */
  u16 radio[PG_RADIO_NUM];
}scan_pid_var_t;

/*!
  program struct
 */
typedef struct
{
  /*! Program node index in range of 0 - 599 */
  u32 id            : 16;
  /*! sat index */
  u32 sat_id        : 16;
  /*! TP node index */
  u32 tp_id         : 16;
  /*! service number in stream */
  u32 s_id          : 16;
  /*! video pid */
  u32 video_pid     : 13;
  /*! pcr pid */
  u32 pcr_pid       : 13;
  /*! is scrambled 0: not scrambled, 1: scrambled */
  u32 is_scrambled  : 1;
  /*! audio volume */
  u32 volume        : 5;
  /*! audio track option, 0: stereo, 1: left, 2: right */
  u32 audio_track   : 3;
  /*! audio channel option, 0: channel 1 \sa audio_pid1, 1: channel 2
   \sa audio_pid2 */
  u32 audio_channel : 5;
  /*! audio channel number*/
  u32 audio_ch_num : 5;
  /*! find scrameble flage*/
  u32 found_scramble :1;
  /*! avs free air program*/
  u32 abs_free_air :1;
  /*! reserve*/
  u32 r2            : 17;
  /*! video pid 1 */
  u16 audio_pid1;
  /*! video pid 2 */
  u16 audio_pid2;
  /*! video pid 3 */
  u16 audio_pid3;
  /*! video pid 4 */
  u16 audio_pid4;
  /*! video pid 5 */
  u16 audio_pid5;
  /*! program information*/
  scan_pid_var_t *p_var;
} dvbs_prog_node_info_t;


/*!
   Scan pid init
  */
void scan_pid_init(dvbs_prog_node_info_t *p_pg_list);


/*!
   Blind scan audio and video
   \param[in] p_pg_list The pointer program list.
  */
BOOL blind_scan_pid(dvbs_prog_node_info_t *p_pg_list);


/*!
   Get PTS data
   \param[in] p_pts_pes The pointer PTS data.
   \return Return SUCCESS if get PTS data
  */
void scan_pid_uninit(dvbs_prog_node_info_t *p_pg_list);

#endif  //__AV_MATCH_PTI_H_

