/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __NVOD_DATA_H_
#define  __NVOD_DATA_H_

/*!
  max time shifted svc cnt
  */
#define MAX_SHIFT_SVC (16)

/*!
  dvbc_tp_info
  */
typedef struct tag_dvbc_tp_info
{
  /*!
    Channel frequency in KHz
    */    
  u32 frequency;
  /*!
    symbol rate in Symbols per second, in KSs
    */
  u32 symbol_rate;
  /*!
    modulation type, see enum nim_modulation
    */    
  u32  modulation;
}dvbc_tp_info_t;

/*!
  time shifted svc event
  */
typedef struct time_shifted_svc_evt
{
  /*!
    Next event node
    */
  struct time_shifted_svc_evt *p_next_evt;
  /*!
    Event start time
    */
  utc_time_t start_time;
  /*!
    Event duration
    */
  utc_time_t drt_time;
  /*!
    Event id
    */
  u16 event_id;
  /*!
    time_shifted_svc_idx
    */
  u8 time_shifted_svc_idx;
  /*!
    evt status
    */
  u8 evt_status;
} time_shifted_svc_evt_t;

/*!
  nvod_reference_svc_evt
  */
typedef struct nvod_reference_svc_evt
{
  /*!
    Next event node
    */
  struct nvod_reference_svc_evt *p_next_evt;
  /*!
    Event id
    */
  u16 event_id;
  /*! 
    Event name
    */
  u16 event_name[MAX_EVT_NAME_LEN + 1];
  /*!
    Event description of event in unicode
    */
  u16 *p_sht_txt;
  /*! 
    The length of event short description
    */
  u16 sht_txt_len;
  /*!
    content nibble
    High 4 bits are level 1
    Low 4 bits are level 2
    */
  u8 cont_level;
  /*!
    User nibble
    High 4 bits are nibble 1
    Low 4 bits are nibble 2
    */
  u8 usr_nib;
  /*!
    Content valid or not
    */
  u8 cont_valid_flag;
  /*!
    reference svc index
    */
  u8 ref_svc_idx;
  /*!
    shift_evt_cnt
    */
  u16 shift_evt_cnt;
  /*!
    Language code by unicode
    */
  u16 lang_code[LANGUAGE_CODE_LEN + 1];
  /*!
    time_shifted_svc_evt_t
    */
  time_shifted_svc_evt_t *p_shift_evt;
} nvod_reference_svc_evt_t;

/*!
  novd time shifted svc
  */
typedef struct tag_time_shifted_svc
{
  /*!
    original network id
    */
  u16 orig_network_id;
  /*!
    stream id
    */
  u16 stream_id;
  /*!
    service id
    */
  u16 svc_id;
  /*!
    pmt pid
    */
  u16 pmt_pid;
  /*!
    video pid
    */
  u32 video_pid               : 13;
  /*!
    pcr pid
    */
  u32 pcr_pid                 : 13;
  /*!
    scrambled flag 0: not scrambled, 1: scrambled
    */
  u32 is_scrambled            : 1;
  /*!
    Audio channel existing
    */
  u32 audio_ch_num            : 5;
  /*!
    volume compensate
    */
  u32 vol_cmpt       : 8;
  /*!
    video_type
    */
  u32 v_type                : 8;
  /*!
    Audio pid
    */
  u16 audio_id  : 13;
  /*!
    Audio type:0, AC3 :1
    */
  u16 a_type  : 3;
}time_shifted_svc_t;

/*!
  novd time shifted svc
  */
typedef struct tag_nvod_reference_svc
{
  /*!
    Service id
    */
  u16 svc_id;
  /*!
    nvod reference time shifted svc cnt
    */
  u8 time_shifted_svc_cnt;
  /*!
  reference_evt cnt
  */
  u8 reference_evt_cnt;
  /*!
    Service name information
    */
  u16 name[MAX_SVC_NAME_LEN];
  /*!
    dvbc tp info
    */
  dvbc_tp_info_t tp;
  /*!
    nvod time shifted svc
    */
  time_shifted_svc_t time_shifted_svc[MAX_SHIFT_SVC];
}nvod_reference_svc_t;

/*!
  nvod database status
  */
typedef enum
{
  /*!
    Normal return
    */
  NVOD_DB_NORM = 0,
  /*!
    NVOD database is full
    */
  NVOD_DB_OVERFLOW,
  /*!
    NVOD reference svc found
    */
  NVOD_DB_REF_SVC_FOUND,
  /*!
    NVOD shift svc found
    */
  NVOD_DB_SHIFT_SVC_FOUND,
  /*!
    NVOD reference evt update
    */
  NVOD_DB_REF_EVT_UPDATE,
  /*!
    NVOD shift evt update
    */
  NVOD_DB_SHIFT_EVT_UPDATE
}nvod_db_status_t;

/*!
  Initialize of nvod data 
  */
void nvod_data_init(void);

/*!
  nvod data delete init
  \param[in] p_data nvod data handle
  */
void nvod_data_deinit(void *p_data);

/*!
  nvod data set locked tp info
  \param[in] p_data nvod data handle
  \param[in] freq frequency
  \param[in] sym symbol_rate
  \param[in] mod modulation
  */
void nvod_data_set_locked_tp_info(void *p_data, u32 freq, u32 sym, u32 mod);

/*!
  nvod data set shift svc info
  \param[in] p_data nvod data handle
  \param[in] p_pmt pmt section
  */
nvod_db_status_t nvod_data_set_shift_svc(void *p_data, pmt_t *p_pmt);

/*!
  nvod data add svc
  \param[in] p_data nvod data handle
  \param[in] p_sdt sdt section
  */
nvod_db_status_t nvod_data_add_svc(void *p_data, sdt_t *p_sdt);

/*!
  Save EIT information into nvod data handle
  \param[in] p_data nvod data handle
  \param[in] p_eit_info eit information to be saved into memory
  */
nvod_db_status_t nvod_data_add_evt(void *p_data, eit_t *p_eit_info);

/*!
  Delete overdue shift evt of referent evt
  \param[in] p_data nvod data handle
  */
void nvod_data_delete_overdue(void *p_data);

/*!
  Delete all nvod data
  \param[in] p_data nvod data handle
  */
void nvod_data_delete_all(void *p_data);

/*!
  get locked tp info
  \param[in] p_data nvod data handle
  */
dvbc_tp_info_t *nvod_data_get_locked_tp_info(void *p_data);

/*!
  nvod data get nvod reference svc cnt
  \param[in] p_data nvod data handle
  */
u32 nvod_data_get_ref_svc_cnt(void *p_data);

/*!
  nvod data get nvod_reference_svc pointer
  \param[in] p_data nvod data handle
  \param[in] reference svc index
  */
nvod_reference_svc_t *nvod_data_get_ref_svc(void *p_data, u32 index);

/*!
  nvod data get reference svc evt
  \param[in] p_data nvod data handle
  \param[in] p_ref_svc ref svc, if NULL get all ref svc evt
  \param[in/out] p_cnt ref svc evt cnt
  */
nvod_reference_svc_evt_t *nvod_data_get_ref_evt(void *p_data,
  nvod_reference_svc_t *p_ref_svc, u32 *p_cnt);

/*!
  nvod data get reference svc evt next
  \param[in] p_data nvod data handle
  \param[in] p_evt ref svc evt
  */
nvod_reference_svc_evt_t *nvod_data_get_ref_evt_next(void *p_data,
  nvod_reference_svc_evt_t *p_evt);

/*!
  nvod data get shift evt of ref evt
  \param[in] p_data nvod data handle
  \param[in] p_ref_evt ref evt
  \param[in/out] p_shift_evt_cnt shift evt cnt
  */
time_shifted_svc_evt_t *nvod_data_get_shift_evt(void *p_data,
  nvod_reference_svc_evt_t *p_ref_evt, u32 *p_shift_evt_cnt);

/*!
  nvod data get shift evt next
  \param[in] p_data nvod data handle
  \param[in] p_evt shift evt
  */
time_shifted_svc_evt_t *nvod_data_get_shift_evt_next(void *p_data,
  time_shifted_svc_evt_t *p_evt);

/*!
  nvod data get real svc of shift evt
  \param[in] p_data nvod data handle
  \param[in] p_ref_evt ref svc evt
  \param[in] p_evt shift evt
  */
time_shifted_svc_t *nvod_data_get_shift_evt_svc(void *p_data,
  nvod_reference_svc_evt_t *p_ref_evt, time_shifted_svc_evt_t *p_evt);

#endif // End for __NVOD_DATA_H_


