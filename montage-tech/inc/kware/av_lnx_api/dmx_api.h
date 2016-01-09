/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LNX_DMX_API_H_

#define __LNX_DMX_API_H_

/*!
  xxxxxxxx
  */
typedef enum
{
/*!
  xxxxxxxx
  */
    MT_DEMUX_CHANNELTYPE_NULL,
    /*!
  xxxxxxxx
  */
    MT_DEMUX_CHANNELTYPE_TS,
    /*!
  xxxxxxxx
  */
    MT_DEMUX_CHANNELTYPE_PES,
    /*!
  xxxxxxxx
  */
    MT_DEMUX_CHANNELTYPE_SECTION,
    /*!
  xxxxxxxx
  */
    MT_DEMUX_CHANNELTYPE_PCR,
    /*!
  xxxxxxxx
  */
    MT_DEMUX_CHANNELTYPE_VIDEO,
    /*!
  xxxxxxxx
  */
    MT_DEMUX_CHANNELTYPE_AUDIO
}dmx_channel_type;

/*!
  xxxxxxxx
  */
typedef struct
{
/*!
  xxxxxxxx
  */
    dmx_input_type_t     in;
/*!
  xxxxxxxx
  */
    dmx_ch_type_t   type;
    
/*!
  xxxxxxxx
  */
    dmx_media_t  type;
}dmx_channel_config_t;
/*!
  xxxxxxxx
  */
typedef struct
{
/*!
  xxxxxxxx
  */
    BOOL allocated;
/*!
  xxxxxxxx
  */
    dmx_filter_setting_t filter_setting;
/*!
  xxxxxxxx
  */
    void *buffer;
/*!
  xxxxxxxx
  */
    u32 buffer_size;
/*!
  xxxxxxxx
  */
    list_head_t list;
/*!
  xxxxxxxx
  */
    void *pchannel;

}dmx_sectionfilter_t;
/*!
  xxxxxxxx
  */
typedef struct
{
/*!
  xxxxxxxx
  */    
    BOOL allocated;

}dmx_descrambler_t;
/*!
  xxxxxxxx
  */
typedef struct
{

/*!
  xxxxxxxx
  */ 
  u16   pid;
    /*!
  xxxxxxxx
  */
    BOOL allocated;
/*!
  xxxxxxxx
  */
    u32 buffer_size;
/*!
  xxxxxxxx
  */
    void *buffer;
/*!
  xxxxxxxx
  */
    dmx_chanid_t hw_channel_handle;
/*!
  xxxxxxxx
  */
    dmx_channel_type channel_type;
/*!
  xxxxxxxx
  */
    dmx_channel_config_t channel_config;
/*!
  xxxxxxxx
  */
    void *p_dmx_dev;
/*!
  xxxxxxxx
  */
    list_head_t sectionfilter_list_head;
}dmx_channel_t;

/*!
  xxxxxxxx
  */
typedef struct
{
 /*!
  xxxxxxxx
  */   
    u8* playback_data;
 /*!
  xxxxxxxx
  */
    u32 playback_data_len;
/*!
  xxxxxxxx
  */
    u8* record_data;
/*!
  xxxxxxxx
  */
    u32 record_data_len;
/*!
  xxxxxxxx
  */
    int dmx_fd;
/*!
  xxxxxxxx
  */
    pthread_mutex_t *pmutex;
/*!
  xxxxxxxx
  */
    pthread_t  section_filter_thread;
/*!
  xxxxxxxx
  */
    u32 channel_num;
   /*!
  xxxxxxxx
  */ 
    dmx_channel_t *p_channels;
/*!
  xxxxxxxx
  */
    u32 filter_num;
/*!
  xxxxxxxx
  */
    dmx_sectionfilter_t *p_filters;
/*!
  xxxxxxxx
  */
    u32 descrambler_num;
/*!
  xxxxxxxx
  */
    dmx_descrambler_t *p_descramblers;
}dmx_info_t;

#endif
