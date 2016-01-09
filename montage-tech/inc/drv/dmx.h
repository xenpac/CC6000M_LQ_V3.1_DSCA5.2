/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DMX_H__
#define __DMX_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
    The filter 31 and slot 31 can not be employed due to HW design bug.
  */
/*!
    HW slot number
  */
#define DMX_HW_SLOT_SUM             31

/*!
    HW filter number
  */
#define DMX_HW_FILTER_SUM           31

/*!
    HW Descrambler number
  */
#define DMX_HW_DESC_SUM           8

/*!
    filter mask&match length
  */
#define DMX_SECTION_FILTER_SIZE          16

/*!
    INVALID channel ID
  */
#define DMX_INVALID_CHANNEL_ID          0xFFFF

/*!
     audio max number
  */
#define DMX_MAX_AUDIO_CHANNEL   16

/*!
  This structure defines an channel handle.
  */
typedef u16 dmx_chanid_t;

/*!
    The number of received TS packets to generate pti_intvec interrupt
  */
typedef enum
{
  /*!
    1 packet
    */
  DMX_ONE_MODE,        //0
  /*!
    2 packets
    */
  DMX_TWO_MODE,
  /*!
    4 packets
    */
  DMX_THREE_MODE,
  /*!
    8 packets
    */
  DMX_FOUR_MODE,
  /*!
    16 packets
    */
  DMX_FIVE_MODE,
  /*!
    32 packets
    */
  DMX_SIX_MODE,
  /*!
    64 packets
    */
  DMX_SEVEN_MODE,
  /*!
    128 packets
    */
  DMX_EIGHT_MODE
} dmx_ts_packet_number_t;

/*!
   TS input modes.
  */
typedef enum
{
  /*!
    Serial input
    */
  DMX_INPUT_SERIAL,
  /*!
    Parallel input
    */
  DMX_INPUT_PARALLEL
} dmx_input_mode_t;

/*!
   TS Record modes.
  */
typedef enum
{
  /*!
    Full TS Record with Null Packet
    */
  DMX_FULL_TS_WITH_NULL_PACKET,
  /*!
    Full TS Record without Null Packet
    */
  DMX_FULL_TS_WITHOUT_NULL_PACKET,
  /*!
    Partial TS record
    */
  DMX_PARTIAL_TS_PACKET,
  /*!
    Partial TS record before scramble
    */
  DMX_PARTIAL_TS_PACKET_SCRAMBLE,  
  /*!
    Partial TS record after descramble
    */
  DMX_PARTIAL_TS_PACKET_DESCRAMBLE,
   /*!
    Partial TS record after descramble
    */
  DMX_PARTIAL_TS_PACKET_PES_DESCRAMBLE
} dmx_rec_mode_t;

/*!
  This structure defines input port of TS Stream. Prepare for warriors
  */
typedef enum
{
  /*!
      TSI interface 1
      */
    DMX_INPUT_EXTERN0,
    /*!
      TSI interface 2
      */
    DMX_INPUT_EXTERN1,
    /*!
      TSI interface CI
      */
    DMX_INPUT_EXTERN_CI,
    /*!
      TSI interface DMA
      */
    DMX_INPUT_EXTERN_DMA,
    /*!
      TSI interface 3 serial
      */
    DMX_INPUT_EXTERN2_SERIAL,
    /*!
      TSI interface DMA
     */
    DMX_INPUT_EXTERN2_DMA,
    /*!
      disable
      */
    DMX_INPUT_DISABLE = 7,
} dmx_input_type_t;

/*!
  This structure defines record ts path. Prepare for warriors
  */
typedef enum
{
  /*!
      Record 0
      */
    DMX_REC_CONFIG0,
    /*!
      Record 1
      */
    DMX_REC_CONFIG1
} dmx_rec_path_t;

/*!
  This structure defines record ts data. Prepare for concerto
  */
typedef enum
{
  /*!
      Record 188 mode
      */
    DMX_REC_188,
    /*!
      Record 188 192
      */
    DMX_REC_192
} dmx_rec_data_t;


/*!
  This structure defines the type of the TS data to be parsed.
  */
typedef enum
{
    /*!
      Section mode
      */
    DMX_CH_TYPE_SECTION,
    /*!
    TS Packet mode
    */
    DMX_CH_TYPE_TSPKT,
    /*!
      TS Packet with payload mode
      */
    DMX_CH_TYPE_TSPKT_PAYLOAD,
    /*!
      PES mode
      */
    DMX_CH_TYPE_PES,
    /*!
      Section mode
      */
    DMX_CH_TYPE_SECTION_SOFT,
    /*!
      Section mode(buffer protect)
      */
    DMX_CH_TYPE_SECTION_BUF_PROTECT,
    /*!
      REC mode
      */
    DMX_CH_TYPE_REC
} dmx_ch_type_t;

/*!
  Type of Program
  */
typedef enum
{
    /*!
      Video data
      */
    DMX_VIDEO_TYPE,
    /*!
      Audio data
      */
    DMX_AUDIO_TYPE,
    /*!
      PCR data
      */
    DMX_PCR_TYPE,
    /*!
      VBI data
      */
    DMX_VBI_TYPE
} dmx_media_t;

/*!
  Type of Program
  */
typedef enum
{
    /*!
      rec normal mode
      */
    DMX_REC_NORMAL,
    /*!
      rec special mode
      */
    DMX_REC_SPEC
}dmx_ts_play_rec_mode_t;

/*!
  Type of Descramble
  */
typedef enum
{
    /*!
      des normal ca mode
      */
    DMX_NORMAL_DESC,
    /*!
      des advance ca mode aes
      */
    DMX_ADVANCE_DESC_AES,
    /*!
      des advance ca mode des
      */
    DMX_ADVANCE_DESC_DES,
    /*!
      des advance ca mode 3des
      */
    DMX_ADVANCE_DESC_3DES
}dmx_descramble_mode_t;

/*!
  Type of Descramble
  */
typedef enum  
{
/*!
  block cipher mode ECB
  */
    DMX_BLOCKMODE_ECB    = 0,    
/*!
  block cipher mode CBC
  */
    DMX_BLOCKMODE_CBC    = 1,  
/*!
   block cipher mode PCBC
  */
    DMX_BLOCKMODE_PCBC   = 2, 
/*!
  block cipher mode CFB
  */
    DMX_BLOCKMODE_CFB    = 3,  
/*!
 block cipher mode OFB
  */
    DMX_BLOCKMODE_OFB    = 4, 
/*!
  block cipher mode CTR
  */
    DMX_BLOCKMODE_CTR    = 5,
}dmx_descramble_blockmode_t;

/**
  Type of Descramble layer mode
 */
typedef enum
{
    /*!
     layer 1
      */
  DMX_ADVANCEDCA_LAYER_1      = 1,
    /*!
      layer 2
      */
  DMX_ADVANCEDCA_LAYER_2      = 2,
    /*!
     layer 3
      */
  DMX_ADVANCEDCA_LAYER_3      = 3
}dmx_advanced_layer_t;


/**
  Type of Descramble key type 
 */
typedef enum
{
    /*!
     key type of sv1
      */
  DMA_ADVANCEDCA_KEY_SV1      = 0,
    /*!
     key type of sv2
      */
  DMA_ADVANCEDCA_KEY_SV2        ,
    /*!
     key type of sv2
      */
  DMA_ADVANCEDCA_KEY_SV3        ,
}dmx_advanced_keytype_t;



#pragma pack(4)

/*!
  TS input config
  */
typedef struct
{
  /*!
      1:parrel  0 :serial
    */
   BOOL input_way;
  /*!
    configure the TS data is active at the falling edge or rising edge!
    1: rising edge ; 0: falling edge
    */
   BOOL local_sel_edge;
  /*!
      0: do not check packet error indicator in TS
      1: check packet error indicator in TS, drop the packet if it is set.
    */
   BOOL error_indicator;
  /*!
      1 :mask 0x47
      0 : no
    */
   BOOL start_byte_mask;
}dmx_ts_input_t;

/*!
  Initialization parameters for a demux device.
  */
typedef struct
{
  /*!
    av sync flag
    */
  BOOL av_sync;
  /*!
    TS input port select
    */
  BOOL input_port_used[4];
  /*!
    configure the TS data is active at the falling edge or rising edge!
    1: rising edge ; 0: falling edge
    */
  BOOL ts_direction[4];
  /*!
    TS input config
    */
  dmx_ts_input_t ts_input_cfg[4];
  /*!
    pool size
    */
  u32  pool_size;
  /*!
    pool mode
    */
  u32  pool_mode;
  /*!
    mutex lock type
    */
  u32  lock_type;
  /*!
    pti id 
    */
  u32  pti_id;
}dmx_config_t;

/*!
  REQ mode of dmx(for jazz)
  */
typedef enum
{
  /*!
    single section mode
  */
  DMX_REQ_MODE_SINGLE_SECTION,
  /*!
    continuous section mode
  */
  DMX_REQ_MODE_CONTINUOUS_SECTION,
  /*!
    loop section mode
  */
  DMX_REQ_MODE_FILTER_LOOPBUFF,
  /*!
    single ts mode
  */
  DMX_REQ_MODE_SINGLE_TS,
  /*!
    continuous ts mode
  */
  DMX_REQ_MODE_CONTINUOUS_TS,
  /*!
    single ts with filter mode(software group section package)
  */
  DMX_REQ_MODE_FTS_SINGLE,
  /*!
    multi ts with filter mode(software group section package)
  */
  DMX_REQ_MODE_FTS_MUL,
  /*!
    ts without filter mode(software group section package)
  */
  DMX_REQ_MODE_NFTS,
  /*!
    double buffer mode, constructed with multi-table(no package loss)
  */
  DMX_REQ_MODE_SECTION_DOUBLE,
  /*!
     ts without filter loop mode(no package loss now reserve)
  */
  DMX_REQ_MODE_SECTION_NFTS,
  /*!
    reserve
  */
  DMX_REQ_MODE_UNKNOWN
} dmx_req_mode_t;

/*!
  REQ mode of dmx(for concerto)
  */
typedef enum
{
  /*!
    对于正常的TS流建议填0
  */
    DMX_SLOT_MUL_SEC_DIS_ENABLE = 0x00,
  /*!
    对于TS填充不为0xff的TS流需要填1
  */
    DMX_SLOT_MUL_SEC_DIS_DISABLE,
  /*!
    comments
  */
    DMX_SLOT_MUL_SEC_DIS_END,
} dmx_mulsecdis_t;

/*!
  This structure defines the parameter of a filter.
  */
typedef struct
{
  /*!
    CRC enable/disable
    */
  BOOL            en_crc;
  /*!
    Contunuous mode enable/disable
    */
  BOOL            continuous;
  /*!
    TS mode filter
    */
  dmx_ts_packet_number_t ts_packet_mode;
  /*!
    Filter match value
    */
  u8  value[DMX_SECTION_FILTER_SIZE];
  /*!
    Filter mask value
    */
  u8  mask[DMX_SECTION_FILTER_SIZE];
  /*!
    REQ mode
    */
  dmx_req_mode_t req_mode;
} dmx_filter_setting_t;


/*!
  This structure defines the parameter of a slot.
  */
typedef struct
{
   /*!
    soft filter flag
    */
   BOOL soft_filter_flag;
   /*!
      TSI input type
      */
    dmx_input_type_t     in;
    /*!
      TS type
      */
    dmx_ch_type_t   type;
    /*!
      slot pid
      */
    u16             pid;
    /*!
      section end filled with 0xff
      */
    dmx_mulsecdis_t muldisp;
    /*!
      back point to struct dvb_demux_feed
    */
    void *feed;
} dmx_slot_setting_t;

/*!
 dma config parameter
  */
typedef struct
{
    /*!
      memory address
      */
    u32      data_length;
    /*!
      TS clk interval DMA
      */
    u32      ts_clk;
    /*!
      memory address
      */
    u32      mem_address;
}dmx_dma_config_t;

/*!
  This structure defines the parameter of a rec.
  */
typedef struct
{
    /*!
      slot pid
      */
    u16             pid;
    /*!
      record select
      */
    dmx_rec_path_t    rec_in;
    /*!
      TS type
      */
    dmx_rec_mode_t   type_mode;
    /*!
      TSI input type
      */
    dmx_input_type_t     stream_in;
    /*!
      TSI data type
      */
    dmx_rec_data_t     type_data; 
} dmx_rec_setting_t;

/*!
  This structure defines the parameter of play channel.
  */
typedef struct
{
    /*!
      slot pid
      */
    u16             pid;
    /*!
     stream type
      */
    dmx_media_t  type;
    /*!
      TSI input type
      */
    dmx_input_type_t     stream_in;
} dmx_play_setting_t;

/*!
  This structure defines the network play parameter.
  */
typedef struct
{
    /*!
      slot pid
      */
    u16             pid[10];
    /*!
      slot pid
      */
    s32             pid_count;
    /*!
      rec buf address
      */
    u8             *p_buf;
    /*!
      rec buf size
      */
    u32             size;
    /*!
      TSI input type
      */
    dmx_input_type_t     stream_in;
} dmx_network_play_setting_t;

/*!
    DMX device
  */
typedef struct dmx_device
{
    /*!
        base
      */
    void *p_base;
    /*!
        priv
      */
    void *p_priv;
} dmx_device_t;

/*!
    DMX infomation
*/
typedef struct dmx_info
{
    /*!
        video flag
      */
    BOOL has_video;
    /*!
        audio flag
      */
    BOOL has_audio;
    /*!
        rec unit size
      */
    u32 rec_unit_size;
}dmx_info_t;

/*!
    DMX ts play infomation
*/
typedef struct dmx_ts_play_info
{
    /*!
        ts play flag
      */
    BOOL ts_play_falg;
    /*!
        one cw can play time
      */
    u16 ts_cw_interval;
    /*!
        ts play address
      */
    u8 *p_ts_addr;
    /*!
        ts play buffer size
      */
    u32 buf_size;
    /*!
        ts play video pid
      */
    u16 v_pid;
    /*!
        ts play audio pid
      */
    u16 audio_num;
    /*!
        ts play cur audio pid
      */
    u16 audio_cur_pid;
    /*!
        ts play audio pid
      */
    u16 audio_pid[DMX_MAX_AUDIO_CHANNEL];
    /*!
        ts play pcr pid
      */
    u16 pcr_pid;
    /*!
        ts play rec mode
      */
    dmx_ts_play_rec_mode_t mode;
    /*!
        ts play ts input
      */
    dmx_input_type_t ts_in;
    /*!
        ts play rec config
      */
    dmx_rec_path_t rec_output;
    /*!
        ts play video channel id
      */
    dmx_chanid_t vid_chanid;
    /*!
        ts play audio channel id
      */
    dmx_chanid_t aud_chanid[DMX_MAX_AUDIO_CHANNEL];
    /*!
        ts play pcr channel id
      */
    dmx_chanid_t pcr_chanid;
}dmx_ts_play_info_t;

/*!
  This structure defines the type of the TS data to be parsed.
  */
typedef enum
{
    /*! 
    section mode
    */
    CH_TYPE_PSI_SI,
    /*! 
    record mode
    */
    CH_TYPE_REC,    
    /*! 
    video mode
    */
    CH_TYPE_VID,
    /*! 
    audio mode
    */
    CH_TYPE_AUD,
    /*! 
    pcr mode
    */
    CH_TYPE_PCR,
    /*! 
    other mode
    */
    CH_TYPE_UNKNOW
} dmx_chan_type_t;

/*!
  dmx advanced configure
  */
typedef struct
{
  /*!
      mode
    */
    dmx_descramble_mode_t mode;
  /*!
      block mode
    */
    dmx_descramble_blockmode_t blockmode;
  /*!
      current layer
    */
    dmx_advanced_layer_t cur_layer;
  /*!
     total layer
    */
    dmx_advanced_layer_t total_layer;
  /*!
     key type for key ladder
    */
    dmx_advanced_keytype_t  key_type;
  /*!
     temp key value
    */
    u8 *p_key;
  /*!
      1:parrel  0 :serial
    */
    u8 key_len;
  /*!
     temp key value
    */
    u8 *p_odd_key;
  /*!
      1:parrel  0 :serial
    */
    u8 odd_key_len;
  /*!
     temp key value
    */
    u8 *p_even_key;
  /*!
      1:parrel  0 :serial
    */
    u8 even_key_len;
  /*!
     u8 IV
    */
    u8 *p_u8IV;
}dmx_advanced_config_t;

/*!
    DMX channel infomation
*/
typedef struct dmx_chantype_info
{
    /*!
        channel pid
      */
    u16 pid;
    /*!
        channel type
      */
    dmx_chan_type_t chan_type;
}dmx_chantype_info_t;

#pragma pack()
/*!
  open a demux channel
  the same slot&diff filter,is the diff channel.

  \param[in] p_dmx The pointer to the demux device.
  \param[in] p_slot The pointer to the slot setting parameter.
  \param[out] p_channel  The pointer to the channel handle.
  */
RET_CODE dmx_si_chan_open(void *p_dev, dmx_slot_setting_t *p_slot, dmx_chanid_t *p_channel);

/*!
  set hardware buffer for saving data
  the buffer is controlled by hardware

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] p_buf  The buffer pointer.
  \param[in] size   The buffer size.
  */
RET_CODE dmx_si_chan_set_buffer(void *p_dev, dmx_chanid_t channel, u8 *p_buf, u32 size);

/*!
  set a filter for demux channel

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] p_filter  The pointer of filter parameter.
  */
RET_CODE dmx_si_chan_set_filter(void *p_dev,dmx_chanid_t channel, dmx_filter_setting_t *p_param);

/*!
  get data from demux channel

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[out] pp_data  The pointer of data pointer.
  \param[out] p_size  The pointer of data size.
  */
RET_CODE dmx_si_chan_get_data(void *p_dev, dmx_chanid_t channel, u8 **pp_data, u32 *p_size);

/*!
  allocate a demux channel for play programe.

  \param[in] p_dmx The pointer to the demux device.
  \param[in] type   The type of program.
  \param[in] pid    The program pid value.
  \param[out] p_channel    The pointer of channel handle.
  */
RET_CODE dmx_play_chan_open(void *p_dev, dmx_play_setting_t *p_play_t, dmx_chanid_t *p_channel);

/*!
  allocate a demux channel for record TS

  \param[in] p_dmx The pointer to the demux device.
  \param[in] pid    The program pid value.
  \param[in] mode    The record mode.
  \param[out] p_channel    The pointer of channel handle.
  */
RET_CODE dmx_rec_chan_open(void *p_dev, dmx_rec_setting_t *p_rec_t,dmx_chanid_t *p_channel);

/*!
  set DDR buffer for user transfor data.
  the buffer is controlled by hardware

  \param[in] p_dmx The pointer to the demux device.
  \param[in] p_buf  The buffer pointer.
  \param[in] size   The buffer size.
  */
RET_CODE dmx_rec_chan_set_buffer(void *p_dev, u8 index,u8 *p_buf, u32 size);

/*!
  get data from record memory

  \param[in] p_dmx The pointer to the demux device.
  \param[out] p_size  The pointer of data size.
  */
RET_CODE dmx_rec_get_data(void *p_dev, u8 index, u64 *p_size);

/*!
  free a demux channel

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel handle.
  */
RET_CODE dmx_chan_close(void *p_dev, dmx_chanid_t channel);

/*!
  start a demux channel

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel handle.
  */
RET_CODE dmx_chan_start(void *p_dev, dmx_chanid_t channel);

/*!
  stop a demux channel

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel handle.
  */
RET_CODE dmx_chan_stop(void *p_dev, dmx_chanid_t channel);

/*!
  control the descrambler module

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] enable    the module open&close,true for open.
  */
RET_CODE dmx_descrambler_onoff(void *p_dev, dmx_chanid_t channel, BOOL enable);

/*!
  control the descrambler module

  \param[in] p_dmx The pointer to the demux device.
  \param[in] p_adv_conf   The struct advanced ca configure.
  */
RET_CODE dmx_descrambler_set_adkey(void *p_dmx
        , dmx_advanced_config_t *p_adv_conf);

/*!
  set the odd keys

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] p_key    The pointer to key buffer.
  \param[in] key_length    The key length.
  */
RET_CODE dmx_descrambler_set_odd_keys(void *p_dev, dmx_chanid_t channel, u8 *p_key, s32 key_length);

/*!
  set the even keys

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] p_key    The pointer to key buffer.
  \param[in] key_length    The key length.
  */
RET_CODE dmx_descrambler_set_even_keys(void *p_dev, dmx_chanid_t channel,
                                             u8 *p_key, s32 key_length);




/*!
  set the odd keys for advance cas

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] key_length    The key length.
  \param[in] ch_id    sctrl channel id.
  */

RET_CODE dmx_descrambler_set_odd_keys_adcas(void *p_dev, dmx_chanid_t channel
                                            , s32 key_length,u8 ch_id,void *p_dev_sctrl);
/*!
  set the even keys for advance cas

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] key_length    The key length.
  \param[in] ch_id    sctrl channel id.
  */
RET_CODE dmx_descrambler_set_even_keys_adcas(void *p_dev,dmx_chanid_t channel,
                                            s32 key_length,u8 ch_id,void *p_dev_sctrl);

/*!
  set the even and odd keys

  \param[in] p_dmx The pointer to the demux device.
  \param[in] channel   The channel ID handle.
  \param[in] p_advanced_engine   The pointer to the advanced engine setting parameter.
  */
RET_CODE dmx_descrambler_advanced_set_keys(void *p_dmx,
        dmx_chanid_t channel, dmx_advanced_config_t const * const p_adv_conf);

/*!
  set the dma config

  \param[in] p_dmx The pointer to the demux device.
  \param[in] p_dma_config   The parameter of dma
  */
RET_CODE dmx_set_dma_config(void *p_dev,dmx_dma_config_t *p_dma_config);

/*!
  Get the dmx dma state.

  \param[in] p_dmx The pointer to the demux device.
  */
BOOL  dmx_get_dma_state(void *p_dev);

/*!
  Get the channel id by pid

  \param[in] p_dev The pointer to the demux device.
  \param[in] pid   The parameter of dma
  \param[out] p_channel    The pointer of channel handle.
  */
RET_CODE dmx_get_chanid_bypid(void *p_dev, u16 pid, dmx_chanid_t *p_channel);

/*!
  Get the channel id by pid

  \param[in] p_dev The pointer to the demux device.
  \param[in] pid   The parameter of dma
  \param[out] p_channel    The pointer of channel handle.
  */
RET_CODE dmx_get_chanid(void *p_dev,dmx_chantype_info_t chan_info, dmx_chanid_t *p_channel);

/*!
  Player ts file by cpu transport
  
  \param[in] p_dev The pointer to the demux device.
  \param[in] p_data The pointer to ts data
  \param[in] size     The data size
  \param[in] v_pid   The video pid of  data
  \param[in] a_pid   The audio pid of  data
  \param[in] pcr_pid   The pcr pid of  data
  \param[in] mode   The type of data es,pes,ts
  */
RET_CODE dmx_cpu_to_ts(void *p_dev, u8 *p_data, u32 size,
                                                 u16 v_pid, u16 a_pid, u16 pcr_pid, u8 mode);

/*!
  Reset the channel for av switch and es buffer

  \param[in] p_dev The pointer to the demux device.
  */
RET_CODE dmx_av_reset(void *p_dev);

/*!
  get the dmx run infomation.

  \param[in] p_dev The pointer to the demux device.
  \param[out] p_info    The struct of dmx infomation.
  */
RET_CODE dmx_get_info(void *p_dev,dmx_info_t *p_info);

/*!
  select dmx device have section data or record data, only used in linux

  \param[in] p_dev The pointer to the demux device.
  */
RET_CODE dmx_data_select(void *p_dev);

#ifdef __cplusplus
}
#endif

#endif //__DMX_H__

