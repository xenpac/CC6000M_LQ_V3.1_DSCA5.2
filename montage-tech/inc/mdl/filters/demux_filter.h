/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DEMUX_FILTER_H_
#define __DEMUX_FILTER_H_

/*!
  pes filter ts number : dmx driver register uses 17 bits to save buffer length.
  so buffer length must less than 2^17(128 * KBYTES). so configuration to dmx buffer 
  length = 128K / 188 = 697; so ts_packet_number must less than or equal to 697.
  */
#define PES_TS_PACKET_NUMBER      576

/*!
  define pes ts packet buffer total size.
  */
#define PES_TS_PACKET_BUF_SIZE   (188 * (PES_TS_PACKET_NUMBER))

/*!
 dmx request table mode
 */
typedef enum tag_demux_req_mode
{
 /*!
  dmx request table with single tabel, auto free
  */
  DMX_DATA_SINGLE = 0x00,
 /*!
  dmx request table with multi-section, manual free
  */
  DMX_DATA_MULTI = 0x01,
 /*!
  dmx request unknown
  */
  DMX_DATA_UNKNOWN,
}demux_req_mode_t;


/*!
  defines psi request parameter
  */
typedef struct tag_psi_request_table
{
 /*!
  media type
  */
  media_type_t media_type;
 /*!
  dmx request mode 
  */ 
  demux_req_mode_t req_mode;
 /*!
  table id.
  */
  u16 table_id;
 /*!
  PSI pid
  */
  u16 pid;
 /*!
  time out 
  */ 
  u32 timeout;
 /*! 
  TS type
  */
  dmx_ch_type_t slot_type;
 /*! 
  CRC enable/disable
  */
  BOOL enable_crc;
 /*! 
  Filter match value
  */
  u8 filter_value[DMX_SECTION_FILTER_SIZE];
 /*! 
  Filter mask value
  */
  u8 filter_mask[DMX_SECTION_FILTER_SIZE];
 /*!
  psi extern data address, general use to multi mode.
  if p_ext_data == NULL, user intra buffer.
  */
  u8 *p_ext_data;
 /*!
  extern data size
  */
  u32 ext_data_size;
}psi_request_data_t;

/*!
 defines psi free parameter
 */
typedef struct tag_psi_free_table_t
{
 /*!
  media type
  */
  media_type_t media_type;
 /*!
  dmx request mode 
  */ 
  demux_req_mode_t req_mode;
/*!  
  table id.
  */
  u16 table_id;
 /*!
  psi pid.
  */
  u32 psi_pid;
}psi_free_data_t;

/*!
  the psi pin config parameter
  */
typedef struct tag_psi_pin_cfg
{
 /*!
  how many table number do you want request?
  */
  u16 max_table_num;
 /*!
   see dmx_input_type_t
   */
 u16 demux_index;
}psi_pin_cfg_t;

/*!
 defines rec request parameter
 */
typedef struct tag_rec_request_data
{
 /*!
  pid
  */
  u16 pid;
 /*!
   rec_mode
   */
 u16 rec_mode;
 /*!
   see dmx_input_type_t
   */
 u16 demux_index;
 /*!
   see dmx_rec_path_t
   */
 u16 rec_in;
}rec_request_data_t;

/*!
 defines rec reset request parameter
 */
typedef struct tag_rec_reset_pid_data
{
 /*!
  pid
  */
  u16 new_v_pid;
 /*!
  pid
  */
  u16 old_v_pid;
  /*!
  pid
  */
  u16 new_a_pid;
 /*!
  pid
  */
  u16 old_a_pid;
 /*!
  pid
  */
  u16 new_pcr_pid;
 /*!
  pid
  */
  u16 old_pcr_pid;
  /*!
    see dmx_input_type_t
    */
 u16 demux_index;
 /*!
   rec_mode, see dmx_rec_mode_t
   */
 u16 rec_mode;
}rec_reset_pid_data_t;

/*!
  the record buffer config parameter
  */
typedef struct tag_rec_buf_cfg
{
 /*!
  record buffer
  */
  u8 *p_rec_buffer;
 /*!
  total buffer size
  */
  u32 total_buf_len;
 /*!
  atom size, depends on hardware
  */
  u32 atom_size;
 /*!
  fill expect size, min fill size of expection
  */
  u32 fill_expect_size;
 /*!
  fill unit size, fill size must multiple of fill unit size
  */
  u32 fill_unit_size;
 /*!
  rec mode see dmx_rec_mode_t
  */
  u16 rec_mode;
 /*!
   dmx_rec_path_t
   */
 u16 rec_in;
}rec_buf_cfg_t;

/*!
 defines pes request parameter
 */
typedef struct tag_pes_request_data
{
 /*!
  pid
  */
  u16 pid;
}pes_request_data_t;


/*!
  demux pin type define
  */
typedef enum tag_dmx_pin_type
{
 /*!
  default type, invalid
  */
  DMX_PIN_UNKNOWN_TYPE,
 /*!
  psi type(section)
  */
  DMX_PIN_PSI_TYPE,
 /*!
  record type
  */
  DMX_PIN_REC_TYPE,
 /*!
  pes type(ts packet)
  */
  DMX_PIN_PES_TYPE,  
}dmx_pin_type_t;

/*!
  demux filter command define
  */
typedef enum tag_dmx_filter_cmd
{
 /*!
  config pin type
  */
  DMX_SET_PIN_TYPE,
 /*!
  set pid
  */
  DMX_SET_PID,
 /*!
  clear pid
  */
  DMX_CLEAR,
 /*!
  config parameter
  */
  DMX_CFG_PARA,
 /*!
  reset pid
  */
  DMX_RESET_PID,
 /*!
  pause resume
  */
  DMX_PAUSE_RESUME,
 /*!
  dmx cmd unknown
  */
  DMX_CMD_UNKNOWN,
}dmx_filter_cmd_t;


/*!
  create a demux filter instance
  filter ID : DEMUX_FILTER
  \return return the instance of demux_filter
  */
ifilter_t * dmx_filter_create(void *p_para);


#endif // End for __DEMUX_FILTER_H_

