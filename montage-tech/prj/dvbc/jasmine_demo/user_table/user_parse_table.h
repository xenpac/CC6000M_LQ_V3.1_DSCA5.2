/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __USER_PARSE_TABLE_H__
#define __USER_PARSE_TABLE_H__

#define MAX_BOUQUET_GROUP    (32)
#define MAX_BOUQUET_NAMELEN  (32)
#define MAX_LOGIC_CHANNEL    (512)
#define HK_BOUQUET_INFO_DESCRIPTOR (0xB0)
#define HK_BOUQUET_LCN_DESCRIPTOR (0xB1)
typedef struct
{
  u8 bouquet_count;
  u8 reserve;
  u16 bouquet_id[MAX_BOUQUET_GROUP];
  u16 bouquet_name[MAX_BOUQUET_GROUP][MAX_BOUQUET_NAMELEN];
  u16 boq_SID_list_count[MAX_BOUQUET_GROUP];
  u16 boq_SID_list[MAX_BOUQUET_GROUP][MAX_LOGIC_CHANNEL];
}bouquet_group_t;

/*!
  Data structure for logical channel desc
  */
typedef struct
{
  u16 bouquet_id;
  u8 show_flag;
  u8 sound_channel;
  u8 sound_volume;
  u16 service_id;
  u16 logical_channel_id;
}lcn_channel_t;

typedef struct
{
  u16 log_ch_num;
  lcn_channel_t log_ch_info[MAX_LOGIC_CHANNEL];
}logic_info_t;

typedef enum {
  UI_OTA_NORMAL = 1,
  UI_OTA_FORCE = 2,
}UI_OTA_TYPE;

typedef struct
{
  u32 oui;
  u16 symbol;
  u32 freq;
  u8  qam_mode;
  u16  data_pid;
  u8  ota_type;
  u32 hwVersion;
  u32 swVersion;
  u32 plat_id;
  u8  Serial_number_start[16];
  u8  Serial_number_end[16];
}update_t;

void parse_bat_india(handle_t handle, dvb_section_t *p_sec);

void create_categories(void);

void request_nit_extend_filter(dvb_section_t *p_sec, u32 table_id, u32 para);

void parse_nit_india(handle_t handle, dvb_section_t *p_sec);

u16 get_network_id(void);

bouquet_group_t *get_bouquet_group(void);

bouquet_group_t * get_bouquet_group_tmp(void);

void sort_tmpgrp_by_bouquet_id(void);

void free_bouquet_group_tmp(void);

void request_bat_section_india(dvb_section_t *p_sec, u32 table_id, u32 para);
void categories_set_loaded_flag(BOOL b);
void request_eit_multi_mode_user(dvb_section_t *p_sec, u32 table_id, u32 para2);
#endif


