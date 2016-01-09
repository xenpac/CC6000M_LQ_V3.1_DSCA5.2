/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PMT_ELI_H_
#define __PMT_ELI_H_

/*!
  Max audio pid supported when parse PMT information
  and the actual audio pid number can be more or less than this value
  */
#define DVB_MAX_AUDIO_PID_NUM 16
/*!
  Max subtitle description supported when parse PMT information
  */
#define DVB_MAX_SUBTILTE_DESC 16

/*!
  Max teletext description supported when parse PMT information
  */
#define DVB_MAX_TELETEXT_DESC 16

/*!
  Max teletext description supported when parse PMT information
  */
#define MAX_ECM_DESC_NUM (24)

/*!
  Language code length
  */
#define LANGUAGE_LEN (3)

/*!
  Subtitle descriptor
  */
typedef struct subtitle_descriptor
{
  /*!
    PID for get subtitle information
    */
  u16 pid;
  /*!
    Language code length of language code is 24
    */
  u8  language_code[3];
  /*!
    Subtitle type
    */
  u8  type;
  /*!
    Composition page id
    */
  u16 cmps_page_id;
  /*!
    Ancilliary page id
    */
  u16 ancl_page_id;
  /*!
    reserved
    */
  u16 reserved;
}subtitle_descriptor_t;

/*!
  Teletext descriptor
  */
typedef struct teletext_descriptor
{
  /*!
    Teletext  pid
    */
  u16 pid;
  /*!
    Language code
    */
  u8  language_code[3];
  /*!
    Teletext type
    */
  u8  type;
  /*!
    Magazine number
    */
  u8  magazien;
  /*!
    Page number
    */
  u8  page;
}teletext_descriptor_t;


/*!
  Data broadcast desc
  */
typedef struct
{
  /*!
    Manufacture id
    */
  u8 manufacture_id;
  /*!
    Product model id
    */
  u8 model_id;
  /*!
    Product hardware id
    */
  u16 hardware_id;
  /*!
    ABS spec defined timeout
    */
  u8 wait_timeout;
  /*!
    reserved
    */
  u32 reserved : 24;
}broadcast_data_desc_t;

/*!
  Audio description information
  */
typedef struct
{
  /*!
    Audio pid
    */
  u16 p_id:13;
  /*!
    Audio type:0, AC3 :1
    */
  u16 type:3;
  /*!
    language code
    */
  u16 language_index;
}audio_t;

/*!
  ABS software upgrade id descriptior loop definition
  ref GD/J 027-2009 4.3.3.2.1
  */
typedef struct
{
  /*!
    Module ID
    */
  u32 model_id : 8;
  /*!
    Hardware ID
    */
  u32 hardware_id : 8;
  /*!
    reserved 1
    */
  u32 reserved1 : 6;
  /*!
    Last subtable ID
    */
  u32 last_subtable_id : 4;
  /*!
    Section timeout
    */
  u32 wait_timeout : 6;
} abs_sw_upgrade_id_des_detail_t;

/*!
  ABS software upgrade id descriptior definition
  ref GD/J 027-2009 4.3.3.2.1
  */
typedef struct
{
  /*!
    Stream pid
    */
  u16 pid;
  /*!
    number of detail info
    */
  u16 count;
  /*!
    Manufacture ID
    */
  u32 manufacture_id : 8;
  /*!
    reserved
    */
  u32 reserved :24;
  /*!
    detail data
    */
  abs_sw_upgrade_id_des_detail_t *p_detail;
} abs_sw_upgrade_id_des_t;

/*!
  ecm descriptior definition
  ref GD/J 027-2009 4.3.3.2.1
  */
typedef struct 
{
  /*!
    CA system id
    */
  u16 ca_sys_id;
  /*!
    ECM pid
    */
  u16 ecm_pid;
  /*!
    es pid :if es_pid is 0, meas this info parse from program info descriptor
    */
  u16 es_pid;
  /*!
    reserved
    */
  u16 reserved;
}cas_descriptior_t;

/*!
  mosaic stream identifier descriptor
  */
typedef struct tag_stream_id_des
{
  /*!
    logical cell id
    */
  u16 lg_cell_id;
  /*!
    audio stream id
    */
  u16 stream_id;
}stream_id_des_t;

/*!
  private ad descriptor
  */
typedef struct tag_ad_des
{
  /*!
    program id
    */
  u8 program_id;
  /*!
    reserved
    */
  u8 reserved;
  /*!
    ad pid
    */
  u16 ad_pid;
}ad_des_t;

/*!
  PMT table struct
  */
typedef struct tag_pmt_eli
{
  /*!
    Program number
    */
  u16 prog_num;
  /*!
    PMT version
    */
  u8 version;
  /*!
    video type
    */
  u8 video_type;
  
  /*!
    pcr pid
    */
  u16 pcr_pid;
  /*!
    video pid
    */
  u16 video_pid;

  /*!
    pmt pid
    */
  u16 pmt_pid;
  /*!
    Upgrade data stream id
    */
  u16 es_id;

  /*!
    Audio channel count
    */
  u8 audio_count;
  /*!
    Subtitle description number
    */
  u8 subt_dr_cnt;
  /*!
    Teletext description number
    */
  u16 ttx_dr_cnt;

  /*!
    desc count
    */
  u16 ecm_cnt;
  /*!
    mosaic descriptor found
    */
  u16 mosaic_des_found : 1;
  /*!
    logical screen descriptor found
    */
  u16 logic_screen_des_found : 1;
  /*!
    stream identified descriptor found
    */
  u16 stream_identifier_des_found : 14;

  /*!
    audio pid & type
    */
  audio_t audio[DVB_MAX_AUDIO_PID_NUM];
  /*!
    Data broadcast id desc
    */
   broadcast_data_desc_t data_broadcast;
  /*!
    Subtitle description information
    */
  subtitle_descriptor_t  subt_descr[DVB_MAX_SUBTILTE_DESC];
  /*!
    Teletext description
    */  
  teletext_descriptor_t  ttx_descr[DVB_MAX_TELETEXT_DESC];
  /*!
    CAS description
    */  
  cas_descriptior_t cas_descr[MAX_ECM_DESC_NUM];
  /*!
    priv ad des
    */
  ad_des_t priv_ad_dec;
  /*!
    mosaic descriptor(for DVB_C)
    */
  mosaic_t *p_mosaic_des;
}pmt_eli_t;


/*!
  parse dvbs' pmt table
  
  \param[in] p_param  : dvb parse param pointer.
  \param[in] p_input  : dmx psi data
  \param[in] p_output : dvb parsed data
  */
RET_CODE pmt_eli_parse_dvbs(dvb_parse_param_t *p_parse, u8 *p_input, u8 *p_output);

/*!
  pmt table param

  \param[in] p_param : pmt table request static parameters
  */
void pmt_eli_request(dvb_request_param_t *p_request);


#endif // End for __PMT_ELI_H_

