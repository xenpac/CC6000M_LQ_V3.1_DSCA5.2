/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PRESET_DATA_H__
#define __PRESET_DATA_H__

/*!
  max name length
  */
#define MAX_NAME_LENGTH 16

/*!
  node type
  */
typedef enum
{
  /*!
    sat node
    */
  PRESET_SAT = 0,
  /*!
    program node
    */
  PRESET_PRO,
  /*!
    tp node
    */
  PRESET_TP
}preset_node_type_t;


/*!
  dvbs satellite
  */
typedef struct
{
  /*!
    lnb high
    */
  u32 lnb_high;
  /*!
    lnb low
    */
  u32 lnb_low;
  /*!
    sat name
    */
  u16 sat_name[MAX_NAME_LENGTH + 1];
  /*!
    sat longitude
    */
  u16 longitude;
} preset_dvbs_sat_t;


/*!
  dvbs tp
  */
  typedef struct
{
  /*!
    tp pol
    */
  u32 pol;
  /*!
    tp frequency
    */
  u32 frq;
  /*!
    sym
    */
  u32 sym;
} preset_dvbs_tp_t;

/*!
  dvbs pg
  */
typedef struct
{
  /*!
    video pid
    */
  u32 video_pid;
  /*!
    audio pid
    */
  u32 audio_pid;
  /*!
    pcr pid
    */
  u32 pcr_pid;
  /*!
    audio track
    */
  u32 audio_track;
  /*!
    sid
    */
  u32 s_id;
  /*!
    network id
    */
  u32 net_id;
  /*!
    ts id
    */
  u32 ts_id;
  /*!
    pg name
    */
  u16 service_name[MAX_NAME_LENGTH + 1];
  /*!
    is scrambled
    */
  u16 is_scrambled;

} preset_dvbs_pg_t;

/*!
  mem config
  */
typedef struct
{
  /*!
    use extern buffer for unzip data
    */
  BOOL use_extern_buf;
  /*!
    extern buffer addr
    */
  u32 extern_buf_addr;
  /*!
    extern buffer size
    */
  u32 extern_buf_size;
}preset_mem_config_t;

/*!
  preset type
  */
typedef enum  
{
  /*!
    DVBS type
    */
  PRESET_DVBS,
  /*!
    DVBC type
    */
  PRESET_DVBC,
  /*!
    DVBT type
    */
  PRESET_DVBT
}preset_type_t;

/*!
  policy struct
  */
typedef struct
{
  /*!
    do some init before process 
    */
  RET_CODE (*pre_process)(void);
  /*!
    process custom data .for priv data
    */
  RET_CODE (*process_custom_data)(preset_node_type_t node_type, u8 pos, u8 *p_uni_str);
  /*!
    process sat struct . only for dvbs
    */
  RET_CODE (*process_sat)(void *p_sat);
  /*!
    process tp struct
    */
  RET_CODE (*process_tp)(void *p_tp);
  /*!
    process pg struct
    */
  RET_CODE (*process_pg)(void *p_pg);
  /*!
    process done
    */
  RET_CODE (*process_done)(void);
  /*!
    config preset module's memory
    */
  preset_mem_config_t mem_cfg;
}db_preset_policy_t;

/*!
  load preset data from flash block 
  functions 
  
  \param[in] blockid  block index of preset data
  \param[in] type see  preset_type_t
  \param[in] p_policy see  db_preset_policy_t
  */
void db_load_preset(u8 blockid, preset_type_t type,db_preset_policy_t *p_policy);

#endif
