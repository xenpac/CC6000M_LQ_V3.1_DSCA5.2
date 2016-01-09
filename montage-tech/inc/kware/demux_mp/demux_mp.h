/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DEMUX_MP_H__
#define __DEMUX_MP_H__
/*!
  This structure defines the postprocessing mode
  */
typedef struct {
/*!
  This structure defines the postprocessing mode
  */
    int progid;   
/*!
  pcr pid
  */
    int pcr_pid;
/*!
  This structure defines the postprocessing mode
  */
    int es_cnt;
/*!
  This structure defines the postprocessing mode
  */
    struct pmt_ts_es {
    /*!
  This structure defines the postprocessing mode
  */
    int pid;
    /*!
  This structure defines the postprocessing mode
  */
    int type; 
    /*!
  This structure defines the postprocessing mode
  */
    char lang[4];
     /*!
  This structure defines the eac3 mode
  */
   char audio_eac3_flag;
    } es[64];
/*!
  audio es stream count
  */
    int aud_stream_cnt;
/*!
  video es stream array
  */
    struct pmt_ts_es aud_stream[32];
/*!
  video es stream count
  */
    int vid_stream_cnt;
/*!
  video es stream array
  */
    struct pmt_ts_es vid_stream[32];
/*!
   subtitle count
  */
    int sub_title_cnt;
/*!
  subtitle array
  */
    struct pmt_ts_es sub_title[32];

    
} pmt_ts_t;
/*!
  This structure defines the postprocessing mode
  */
typedef struct {
   /*!
  This structure defines the postprocessing mode
  */
    pmt_ts_t *p_pmt;
   /*!
  This structure defines the postprocessing mode
  */
    int pmt_cnt; 
   
} ts_info_t;
/*!
 xxxxxxxxxxxxx
*/
void ts_get_pcm_info(demuxer_t * demuxer, int * is_big_endian,
                                 int * bits ,int * channel,int * sample_rate);
/*!
 xxxxxxxxxxxxx
*/
int ds_get_packet_subt(demux_stream_t *ds, unsigned char ** start, double * pts);
/*!
 xxxxxxxxxxxxx
*/
unsigned char  ds_check_packet_buffer_overflow(demux_stream_t * ds);
/*!
 xxxxxxxxxxxxx
*/
void   ds_get_video_codec_type(demux_stream_t *ds, 
   int * p_video_type, int * vpid, int * pcr_pid) ;
/*!
 xxxxxxxxxxxxx
*/
int ds_get_video_bps(demux_stream_t * ds);
/*!
 xxxxxxxxxxxxx
*/
demuxer_t * demux_mp_open(stream_t *vs, int file_format, int audio_id_mp,
                          int video_id_mp, int dvdsub_id_mp, char *filename);
/*!
 xxxxxxxxxxxxx
*/
ts_info_t * ds_ts_prog(demuxer_t * demuxer);
/*!
 xxxxxxxxxxxxx
*/
int ds_get_audio_count(demuxer_t * d) ;
/*!
 xxxxxxxxxxxxx
*/
int ds_get_packet_video(demux_stream_t *ds, unsigned char ** start, int8_t speed);
/*!
 xxxxxxxxxxxxx
*/
int ts_get_audio_track_pid(ts_info_t * p_ts_priv, int id);
/*!
 xxxxxxxxxxxxx
*/
 void set_ts_pmt_avs_info(ts_info_t * p_ts_priv);
/*!
 xxxxxxxxxxxxx
*/
 int ds_get_audio_bps(demux_stream_t * ds);
/*!
 xxxxxxxxxxxxx
*/
 void   ds_get_audio_codec_type(demux_stream_t *ds, int * p_audio_type, int *apid, 
                                               AUDIO_OUT_MODE aout_mode);
/*!
 xxxxxxxxxxxxx
*/
 int ds_get_packet_audio(demux_stream_t * ds, unsigned char ** start,
    uint8_t ** extra_buf, uint8_t **extra_size) ;
#endif

