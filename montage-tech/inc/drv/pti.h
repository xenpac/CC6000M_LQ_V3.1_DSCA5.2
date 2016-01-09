/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __PTI_H__
#define __PTI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
    The filter 31 and slot 31 can not be employed due to HW design bug.
  */
/*!
    HW slot number
  */
#define PTI_HW_SLOT_SUM             32

/*!
    HW filter number
  */
#define PTI_HW_FILTER_SUM           32

/*!
    The number of slots that can be used in normal mode
  */
#define PTI_SLOT_SUM_NORMAL         31

/*!
    The number of slots that can be used in dynamic mode
  */
#define PTI_SLOT_SUM_DYNAMIC        28

/*!
    The number of filters that can be used
  */
#define PTI_FILTER_SUM              31

/*!
    The size of filter mask codes
  */
#define MAX_PTI_FLTBYTESUM          12

/*!
    The number of descrable key
  */
#define PTI_DESC_KEY_SUM            8


/*!
    The handle of channel
  */
typedef void * h_pti_channel_t;


/*!
  Initialization parameters for a pti device.
  */
typedef struct 
{
  /*! 
    configure the TS data is active at the falling edge or rising edge!
    1: rising edge ; 0: falling edge
    */
  BOOL ts_direction;
}pti_config_t;

/*!
   TS input modes.
  */
typedef enum
{
    /*!
        Serial input
      */
    PTI_INPUT_SERIAL,
    /*!
        Parallel input
      */
    PTI_INPUT_PARALLEL
} pti_input_t;

/*!
   TS input source.
  */
typedef enum
{
    /*!
        Input from external(demodulator)
      */
    PTI_SOURCE_EXTERNAL,
    /*!
        Input from CPU
      */
    PTI_SOURCE_CPU
} pti_source_t;

/*!
   Type of PTI data
  */
typedef enum
{
    /*!
      TS packet data
    */
    PTI_TYPE_TSPKT,
    /*!
        Section data
       */
    PTI_TYPE_SECTION,
    /*!
        Video data
      */
    PTI_TYPE_VIDEO,
    /*!
        Audio data
       */
    PTI_TYPE_AUDIO,
    /*!
        PCR data
       */
    PTI_TYPE_PCR
} pti_type_t;


/*!
   Type of buffer mode
  */
typedef enum
{
    /*!
        Linear
      */
    PTI_BUF_MODE_LINEAR,
    /*!
        Circle
      */
    PTI_BUF_MODE_CIRCLE,
     /*!
        Double slot
      */
    PTI_BUF_MODE_DOUBLE,
   /*!
        Extern mode of linear
      */
    PTI_BUF_MODE_LINEAR_EXT,
    /*!
        Extern mode of circle
      */
    PTI_BUF_MODE_CIRCLE_EXT
} pti_buf_mode_t;

/*!
    For PTI extern mode.  
    Defines the external buffer used in PTI extern mode.
  */
typedef struct pti_ext_buf
{
    /*!
        Pointer to buffer
      */
    u8                  *p_buf;
    /*!
        Size of buffer
      */
    u32                 size;
    /*!
        Pointer to next buffer
      */
    struct pti_ext_buf  *p_next;
    /*!
        priv
     */
    u32 data_size;
    /*!
        priv
    */
    u8  *p_data;
    /*!
        priv
    */
    u8  *p_cache;
    /*!
        priv
      */
    u8  *p_r;
} pti_ext_buf_t;

/*!
    The number of received TS packets to generate pti_intvec interrupt
  */
typedef enum
{
  /*!
    1 packet
    */
  PTI_ONE_MODE,        
  /*!
    2 packets
    */
  PTI_TWO_MODE,      
  /*!
    4 packets
    */
  PTI_THREE_MODE,   
  /*!
    8 packets
    */
  PTI_FOUR_MODE,
  /*!
    16 packets
    */
  PTI_FIVE_MODE,
  /*!
    32 packets
    */
  PTI_SIX_MODE,
  /*!
    64 packets
    */
  PTI_SEVEN_MODE,
  /*!
    128 packets
    */
  PTI_EIGHT_MODE
} pti_ts_packet_number_t;

/*!
   This structure defines a bundle of parameters for allocating a channel.
  */
typedef struct
{
    /*!
        Data type
      */
    pti_type_t      type;
    /*!
        PID
      */
    u16             pid;
    /*!
        Filtering code
      */
    u8              code[MAX_PTI_FLTBYTESUM];
    /*!
        Mask
      */
    u8              mask[MAX_PTI_FLTBYTESUM];
    /*!
        Enable/disable CRC check on section data
      */
    BOOL            crc_enable;

    /*!
        Buffer mode
      */
    pti_buf_mode_t  buf_mode;
    /*!
        Pointer to the list of external buffer
      */
    pti_ext_buf_t   *p_ext_buf_list;
    /*! 
    TS receive mode
    */
    pti_ts_packet_number_t ts_packet_mode;
} pti_alloc_param_t;


/*!
   This structure is used for getting the data after filter. 
  */
typedef struct
{
    /*!
        Pointer to data
      */
    u8  *p_data;
    /*!
        Size of data
      */
    u32 len;

/*!
    priv
  */
    h_pti_channel_t handle;
} pti_data_block_t;

/*!
    PTI device
  */
typedef struct pti_device
{
    /*!
        base
      */
    void *p_base;
    /*!
        priv
      */
    void *p_priv;
} pti_device_t;


/*!
   Get the size of buffer for PTI management.

   \param[in] dynamic Enable dynamic allocation of filters.
   \param[in] filters The number of filters to be used
   \param[out] p_size The size of buffer requested for PTI management.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_get_mem_request(pti_device_t *p_dev
    , BOOL dynamic, s8 filters, u32 *p_size);

/*!
   Allocate memory for blind scan: the buffer for slot and filter management.
   Set the section buffer's address limit.

   \param[in] dynamic Enable dynamic allocation of filters.
   \param[in] filters The number of filters to be used.
   \param[in] p_buf The start address of the buffer for slot and filter management
   \param[in] buf_size The memory size for slot and filter management.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_parser_init(pti_device_t *p_dev
    , BOOL dynamic, s8 filters, u8 *p_buf, u32 buf_size);

/*!
   De-initialize the memory allocation parse. Also see pti_dev_parser_init.

   \param[in] dev PTI handle.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_parser_deinit(pti_device_t *p_dev);

/*!
   Select the TS source of PTI 

   \param[in] source PTI_SOURCE_EXTERNAL for external input TS(from demodulator),
             or PTI_SOURCE_CPU for CPU input TS.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_select_source(pti_device_t *p_dev, pti_source_t source);

/*!
   Get the source type of PTI

   \param[out] source PTI_SOURCE_EXTERNAL for external input TS (from demodulator),
             or PTI_SOURCE_CPU for CPU input TS.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_get_source(pti_device_t *p_dev, pti_source_t *p_source);

/*!
   Allocate a channel.

   \param[in] p_param A bundle of input parameters, including PID,
     section buffer, filter contents, etc.
   \param[out] p_channel Pointer to a channel handle buffer. 

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_channel_alloc(pti_device_t *p_dev
    , const pti_alloc_param_t *p_param, h_pti_channel_t *p_channel);

/*!
   Release a filter.

   \param[in] filter_idx The index of the filter to be released.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_channel_free(pti_device_t *p_dev, h_pti_channel_t channel);

/*!
   Get data from a specific filter.

   \param[in] p_block Pointer to the structure buffer to be filled with the information of data block.
   \param[in] channel The handle of channel.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_get_data(pti_device_t *p_dev
    , h_pti_channel_t channel, pti_data_block_t *p_block);

/*!
   Free the data got from a specific filter.

   \param[in] p_block Pointer to the structure buffer in which the information of data block is to be free.
   \param[in] channel The handle of channel.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_free_data(pti_device_t *p_dev
    , h_pti_channel_t channel, pti_data_block_t *p_block);

/*!
   Set the PID of video.

   \param[in] pid Video PID for given channel.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_set_video_pid(pti_device_t *p_dev, u16 pid);

/*!
   Set the PID of audio.

   \param[in] pid Audio PID for given channel.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_set_audio_pid(pti_device_t *p_dev, u16 pid);

/*!
   Set the PID of PCR.

   \param[in] pid PCR PID for given channel.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_set_pcr_pid(pti_device_t *p_dev, u16 pid);


/*!
   Get the preset video, audio and PCR PIDs.

   \param[out] v_pid Video PID of given channel.
   \param[out] a_pid Audio PID of given channel.
   \param[out] pcr_pid PCR PID of given channel.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_get_pid(pti_device_t *p_dev
    , u16 *p_v_pid, u16 *p_a_pid, u16 *p_pcr_pid);

/*!
   Reset PTI, ES buffer and A/V decoder.

   \param[in] dev The handle of PTI.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_av_reset(pti_device_t *p_dev);

/*!
   Enable/disable the video, audio and PCR slots.

   \param[in] is_en TRUE to enable the slot, FALSE to disable it.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_av_enable(pti_device_t *p_dev, BOOL enable);

/*!
   Enable/disable the video, audio and PCR slots.

   \param[in] is_en TRUE to enable the slot, FALSE to disable it.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_desc_clear_key(pti_device_t *p_dev, u16 pid);

/*!
   Enable/disable the video, audio and PCR slots.

   \param[in] is_en TRUE to enable the slot, FALSE to disable it.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_desc_set_odd_key(pti_device_t *p_dev, u16 pid, u8 *p_key);

/*!
   Enable/disable the video, audio and PCR slots.

   \param[in] is_en TRUE to enable the slot, FALSE to disable it.

   \return Return 0 for success and others for failure.
  */
RET_CODE pti_dev_desc_set_even_key(pti_device_t *p_dev, u16 pid, u8 *p_key);

#ifdef __cplusplus
}
#endif

#endif //__PTI_H__

