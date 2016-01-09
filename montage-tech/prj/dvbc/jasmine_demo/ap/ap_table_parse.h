/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_TABLSE_PARE_H__
#define __AP_TABLSE_PARE_H__

#define REQUEST_CHANNEL_SEC_TIMEOUT 5000
#define REQUEST_NIT_STB_START_SEC_TIMEOUT 10000 

typedef struct  
{
  u32 table_id :8;
  u32 section_syntax_indicator :1;
  u32 reserved :3;
  u32 section_len :12;
  u32 table_id_ext  :16;
  u32 version  :8;
  u32 section_num :8;
  u32 last_section  :8;
}table_section_t;

/*!
  table parse implementation policy
  */
typedef struct
{
  /*!
    channel enable mark
    */
  u32 channel_bit;
  /*!
    pagebag pid
    */
  u16 pid;
  /*!
    table pid
    */
  u16 table_id;
  /*!
    Initialize channel list
    */
  void (*p_channel_init)(void);
   /*!
    Initialize channel process
    */
  void (*p_channel_process)(void);
   /*!
    Initialize channel release
    */
  void (*p_channel_release)(void);
}channel_table_parse_policy_t;
u8 get_channel_init_nit_ver(void);
void set_channel_init_nit_ver(u8 nit_version);
BOOL get_channel_init_channel_status(void);
void request_channel_sec(dvb_section_t *p_sec, u32 para1, u32 para2);
void parse_channel_sec(handle_t handle, dvb_section_t *p_sec);
void ap_table_parse_task_start(u32 freq, u32 sym, u16  modulation);
channel_table_parse_policy_t *construct_channel_table_parse_policy(void);
void destruct_channel_table_parse_policy(channel_table_parse_policy_t *p_table_parse_policy);
#endif

