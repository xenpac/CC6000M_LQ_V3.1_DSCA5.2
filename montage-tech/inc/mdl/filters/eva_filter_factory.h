/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EVA_FILTER_FAC_H_
#define __EVA_FILTER_FAC_H_


/*!
  function type
  */
typedef ifilter_t * (* filter_create_f)(void *p_para);

/*!
  filter id define
  */
typedef enum tag_eva_filter_id
{
  /*!
    undefined filter id,
    */
  UNKNOWN_FILTER_ID = 0x200,  //follow the APP_LAST //see it ap_framwork.h
  /*!
    file source filter
    */
  FILE_SOURCE_FILTER,
  /*!
	  net source filter
	  */
  NET_SOURCE_FILTER,
  /*!
    file sink filter
    */
  FILE_SINK_FILTER,
  /*!
    recording filter
    */
  RECORD_FILTER,
  /*!
    file sink filter
    */
  MP3_DECODE_FILTER,
  /*!
    record filter
    */
  MP3_TRANSFER_FILTER,
  /*!
    demux filter
    */
  DEMUX_FILTER,
  /*!
    ts player filter
    */
  TS_PLAYER_FILTER,
  /*!
    video render filter
    */
  AV_RENDER_FILTER,
  /*!
    jpeg filter
    */
  JPEG_FILTER,
  /*!
    pic render filter
    */
  PIC_RENDER_FILTER,   
  /*!
    ttx render filter
    */
  TTX_FILTER,
  /*!
    subt render filter
    */
  SUBT_FILTER,
  /*!
    lrc filter
    */
  LRC_PARSE_FILTER,
  /*!
     char filter
    */
  CHAR_FILTER,
  /*!
    scan filter
    */
  SCAN_FILTER,
  /*!
    dvb transfer filter
    */
  DVB_TRANSF_FILTER,  
  /*!
    dvb transfer filter
    */
  SUBT_STATION_FILTER,
    /*!
    weather filter
    */
  WEATHER_FILTER,
      /*!
    weather filter
    */
  NEWS_FILTER,
     /*!
    weather filter
    */
  NET_MUSIC_FILTER,
  /*!
    dvb transfer filter
    */
  TEST_FILTER,    
  /*!
    string filter.
    */
  STR_FILTER,    
  /*!
    vod filter.
    */
  VOD_FILTER,  
  /*!
    albums filter
    */
  ALBUMS_FILTER,    
  /*!
    google map filter.
    */
  GOOGLE_MAP_FILTER,
    /*!
    util filter.
    */
  UTILS_FILTER,
    /*!
    net upg filter.
    */
  NET_UPG_FILTER, 
    /*!
    net upg filter.
    */
  FLASH_SINK_FILTER,
  /*!
    net sink filter.
    */
  NET_SINK_FILTER,
  /*!
    net transf filter.
    */
  NET_TRANSF_FILTER,
  /*!
    ota filter.
    */
  OTA_FILTER,
  /*!
   epg render filter
   */
  EPG_FILTER,
  /*!
    net extern filter.
    */
  NET_EXTERN_FILTER,
  /*!
    media ip filter.
    */
  MEDIA_IP_FILTER,
  /*!
    mplayer aud filter
    */
  MPLAYER_AUD_FILTER,
  /*!
    max filter id flag, when you need add a filter id, insert it above
    */
  FILTER_MAX_ID
}filter_id_t;

/*!
  register filter create function 

  \param[in] id filter id
  \param[out] pp_filter instance of filter
  \param[in] p_para para to filter

  \return return SUCCESS is right
  */
RET_CODE eva_add_filter_by_id(filter_id_t id, ifilter_t **pp_filter, void *p_para);

/*!
  register filter create function 

  \param[in] id filter id
  \param[in] f funtion of create filter

  \return return SUCCESS is right
  */
RET_CODE eva_register_filter(filter_id_t id, filter_create_f f);

/*!
  init filter factory
  \return return SUCCESS is right
  */
RET_CODE eva_filter_factory_init(void);

/*!
  deinit filter factory
  \return return SUCCESS is right
  */
RET_CODE eva_filter_factory_deinit(void);

#endif // End for __EVA_FILTER_FAC_H_

