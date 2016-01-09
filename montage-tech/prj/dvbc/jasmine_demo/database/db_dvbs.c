/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "lib_char.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "class_factory.h"
#include "mdl.h"
#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"
#include "uio.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "db_dvbs.h"
#include "db_preset.h"
#include "data_manager.h"
#include "data_base.h"
#include "data_base16v2.h"
#include "mem_manager.h"
#include "iso_639_2.h"
#include "ui_common.h"
#include "customer_config.h"

#define  MDL_DBG
#ifdef   MDL_DBG
#define  mdl_dbg    OS_PRINTF
#else
#ifndef WIN32
#define  mdl_dbg(x)    do {} while(0)
#else
#define  mdl_dbg
#endif
#endif
#ifdef MOSAIC_ENABLE
#define PG_MAX_LEN 2 * ((sizeof(pg_data_t) \
                  + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16) \
                  + sizeof(audio_t) * DB_DVBS_MAX_AUDIO_CHANNEL \
                  + DB_DVBS_MAX_NAME_LENGTH \
                  + sizeof(mosaic_t) \
                  + sizeof(cas_desc_t) * MAX_ECM_DESC_NUM \
                  + 4))
#else
#define PG_MAX_LEN (sizeof(pg_data_t) \
                  + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16) \
                  + sizeof(audio_t) * DB_DVBS_MAX_AUDIO_CHANNEL \
                  + DB_DVBS_MAX_NAME_LENGTH \
                  + sizeof(mosaic_t) \
                  + sizeof(cas_desc_t) * MAX_ECM_DESC_NUM \
                  + 4)
#endif
#define TABLE_BLOCK_ID    (IW_TABLE_BLOCK_ID)
#define VIEW_BLOCK_ID     (IW_VIEW_BLOCK_ID)
#define PG_NODE_VALID_CACHE_DATA_LEN (MAX_TABLE_CACHE_SIZE)//(12)

#define DATA_CACHE_UNIT_SIZE (40)
#define VIEW_CACHE_UNIT_SIZE (sizeof(item_type_t))

static u8 g_tp_table_id = 0;
static u8 g_pg_table_id = 0;

#ifdef DB_EDITOR
  static BOOL is_initialized = FALSE;
#endif

static item_type_t g_all_tp_view_buf[DB_DVBS_MAX_TP];
static item_type_t g_pg_view_buf[DB_DVBS_MAX_PRO];


u8 *g_find_view_buf;
static db_filter_proc_t filter_array[DB_DVBS_INVALID_VIEW];
static db_filter_proc_new_t filter_new_array[DB_DVBS_INVALID_VIEW];

static pro_sort_t *p_sort_buf = NULL;
static pro_sort_id_t *p_sort_id_buf = NULL;
static pro_view_id_t g_name_view;
static pro_view_id_t g_free_view;
static pro_view_id_t g_lock_view;
static pro_view_id_t g_default_view;

static u32 default_order = 0;
static dvbs_view_t g_vv_id_name_map[MAX_VIRTUAL_VIEW_NUM];

typedef struct
{
  /*!Each bit of fav_grp_flag represents a single favorite group*/
  u32 fav_grp_flag      :32;

  u32 tv_flag                 : 1;
  u32 lck_flag                : 1;
  u32 skp_flag                : 1;
  /*! audio volume */
  u32 volume                  : 5;
  /*! audio channel option, 0: channel 1 \sa audio_pid1, 1: channel 2
     \sa audio_pid2 */
  u32 audio_channel           : 5;
  /*! scrambled flag 0: not scrambled, 1: scrambled */
  u32 is_scrambled            : 1;
  u32 audio_ch_num            : 5;
  /*! video pid */
  u32 video_pid               : 13;

  /*! pcr pid */
  u32 pcr_pid                 : 13;
  /*! audio track option, 0: stereo, 1: left, 2: right */
  u32 audio_track             : 2;
  u32 hide_flag                : 1;
  u32 service_type            : 8;
  u32 video_type              : 8;
  u32 sat_id                  : 16;
  u32 tp_id                   : 16;
  /*
     ---------------------------------------------------------
     above into data cache
    */
  u32 default_order   : 16;
  u32 orig_net_id     : 16;
  u32 ca_system_id            : 16;
  u32 is_audio_channel_modify  : 1;
  u32 name_length     : 5;
  u32 mosaic_flag     :8;
  u32 reserved_1        : 2;//8bits used for mosaic flag

  u32 s_id            : 16;
  u32 ts_id           : 16;

//TKGS_SUPPORT START
  u32 reserved_3        :10;
//used by aiset bouquet protocol
  u32 operator_flag       :1;
  u32 age_limit       :5;
  u32 logical_num     : 16;
//TKGS_SUPPORT END

  u32 pmt_pid         : 16;
  u32 ecm_num         : 6;
  u32 reserved_2      : 10;
  cas_desc_t *p_cas_ecm;  //[MAX_ECM_DESC_NUM];
#ifdef MOSAIC_ENABLE
  mosaic_t *p_mosaic;
#endif
} pg_data_t;

typedef BOOL (*db_dvbs_sort_func)(u8 *prev_node, u8 *cur_node,
                                  dvbs_sort_node_type_t node_type);

typedef BOOL (*db_dvbs_qsort_func)(pro_sort_t *prev_node, pro_sort_t *cur_node);

static db_dvbs_ret_t translate_return_value(db_ret_t db_value)
{
  if(db_value == DB_FAIL)
  {
    return DB_DVBS_FAILED;
  }
  if(db_value == DB_FULL)
  {
    return DB_DVBS_FULL;
  }
  if(db_value == DB_NOT_FOUND)
  {
    return DB_DVBS_NOT_FOUND;
  }
  return DB_DVBS_OK;
}


static BOOL all_radio_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->tv_flag == 0) && (pg_data->hide_flag != 1));
}


static u32 all_radio_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return pg_data->tv_flag == 0;
}


static BOOL all_radio_fta_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 0)
         && (pg_data->hide_flag != 1);
}



static u32 all_radio_fta_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 0);
}


static BOOL all_radio_cas_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 1)
         && (pg_data->hide_flag != 1);
}


static u32 all_radio_cas_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 1);
}


static BOOL all_radio_hd_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)
         && (pg_data->service_type == 0x11)
         && (pg_data->hide_flag != 1);
}


static u32 all_radio_hd_new_filter(u16 bit_mask, u32 context,
                                   u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)
         && (pg_data->service_type == 0x11);
}


static BOOL all_tv_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->tv_flag == 1) && (pg_data->hide_flag != 1));
}


static u32 all_tv_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return pg_data->tv_flag == 1;
}


static BOOL all_tv_fta_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 0)
         && (pg_data->hide_flag != 1);
}


static u32 all_tv_fta_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 0);
}


static BOOL all_tv_cas_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 1)
         && (pg_data->hide_flag != 1);
}


static u32 all_tv_cas_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 1);
}


static BOOL all_tv_hd_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)
         && (pg_data->service_type == 0x11)
         && (pg_data->hide_flag != 1);
}


static u32 all_tv_hd_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)
         && (pg_data->service_type == 0x11);
}


static BOOL fav_radio_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 0)
         && (pg_data->hide_flag != 1);
}


static u32 fav_radio_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 0))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
  //return (((pg_data->fav_grp_flag & (1<<context))!= 0)&&( pg_data->tv_flag == 0));
}


static BOOL fav_radio_fta_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 0)
         && (pg_data->hide_flag != 1);
}


static u32 fav_radio_fta_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 0)
      && (pg_data->is_scrambled == 0))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
}


static BOOL fav_radio_cas_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 1)
         && (pg_data->hide_flag != 1);
}


static u32 fav_radio_cas_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 0)
      && (pg_data->is_scrambled == 1))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
}


static BOOL fav_radio_hd_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 0)
         && (pg_data->service_type == 0x11)
         && (pg_data->hide_flag != 1);
}


static u32 fav_radio_hd_new_filter(u16 bit_mask, u32 context,
                                   u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 0)
      && (pg_data->service_type == 0x11))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
}


static BOOL fav_tv_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 1)
         && (pg_data->hide_flag != 1);
}


static u32 fav_tv_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 1))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
}


static BOOL fav_tv_fta_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 0)
         && (pg_data->hide_flag != 1);
}


static u32 fav_tv_fta_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 1)
      && (pg_data->is_scrambled == 0))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
}


static BOOL fav_tv_cas_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 1)
         && (pg_data->hide_flag != 1);
}


static u32 fav_tv_cas_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 1)
      && (pg_data->is_scrambled == 1))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
}


static BOOL fav_tv_hd_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->tv_flag == 1)
         && (pg_data->service_type == 0x11)
         && (pg_data->hide_flag != 1);
}


static u32 fav_tv_hd_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
      && (pg_data->tv_flag == 1)
      && (pg_data->service_type == 0x11))
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
}


static BOOL fav_all_filter(u16 bit_mask, u32 context,
                           u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & ((u32)1 << context)) != 0)
         && (pg_data->hide_flag != 1);
}


static u32 fav_all_new_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if((pg_data->fav_grp_flag & ((u32)1 << i)) != 0)
    {
      ret |= (u32)1 << i;
    }
  }
  return ret;
  //return((pg_data->fav_grp_flag & (1<<context)) != 0);
}

static BOOL tp_pg_filter(u16 bit_mask, u32 context,
                         u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  /*lint -e737*/
  return ((p->tp_id == context) && (p->hide_flag != 1));
  /*lint +e737*/
}

static BOOL tp_pg_filter_show(u16 bit_mask, u32 context,
                         u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  /*lint -e737*/
  return p->tp_id == context;
  /*lint +e737*/
}


static u32 tp_pg_new_filter(u16 bit_mask, u32 context,
                            u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  /*lint -e737*/
  return pg_data->tp_id == context;
  /*lint +e737*/
}
static BOOL mosaic_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return((((pg_data.service_type == SVC_TYPE_MOSAIC) && pg_data.mosaic_flag) && (pg_data.skp_flag == 0)) 
              && (pg_data.hide_flag == 0));
}

static u32 mosaic_new_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  pg_data_t pg_data = {0};
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);
  db_get_element(p_db_handle, block_id, node_id, (u8 *)&pg_data, 0,
                     sizeof(pg_data_t));
  return( ((pg_data.service_type == SVC_TYPE_MOSAIC) && pg_data.mosaic_flag) && (pg_data.skp_flag == 0) && 
              (pg_data.hide_flag == 0) );
}

static BOOL string_find_filter(u16 bit_mask, u32 context,
                               u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *p_pg_node = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u8 *p_pg_name = NULL;
  const u16 *string_to_be_matched = NULL;

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 PG_MAX_LEN/4);
  /*lint -e737*/
  p_pg_name = (u8 *)p_pg_node + sizeof(pg_data_t)
              + p_pg_node->audio_ch_num * sizeof(audio_t);
  /*lint +e737*/
  string_to_be_matched = (u16 *)context;


  MT_ASSERT(context != 0);

  //Case is not sensitive
  return match_str((u16 *)p_pg_name, string_to_be_matched, FALSE) == TRUE;
}


/*!
   \param[in] prev_node: previous node to be sorted
   \param[in] cur_node:  current node to be sorted
   \param[in] node type: node type of input ( No meaning in this api)
   return : TRUE >exchange pre_node with current node
           FALSE >continue
  */
static BOOL fta_first_sort(u8 *prev_node, u8 *cur_node, dvbs_sort_node_type_t node_type)
{
  BOOL ret = FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
    {
      dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
      dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;

      /*!Fix me about is scrambled and found scrambled*/
      ret = (p_pre_pg_node->is_scrambled == FALSE
            && p_cur_pg_node->is_scrambled == TRUE);
    }
    break;
    case DB_DVBS_SAT_NODE:
      MT_ASSERT(0);
      break;
    case DB_DVBS_TP_NODE:
    {
      dvbs_tp_node_t *p_pre_tp_node = (dvbs_tp_node_t *)prev_node;
      dvbs_tp_node_t *p_cur_tp_node = (dvbs_tp_node_t *)cur_node;

      /*!Fix me about is scrambled and found scrambled*/
      ret = (p_pre_tp_node->is_scramble == FALSE
            && p_cur_tp_node->is_scramble == TRUE);
    }
    break;
  }

  return ret;
}


static BOOL name_a_z_sort(u8 *prev_node, u8 *cur_node, dvbs_sort_node_type_t node_type)
{
  BOOL ret = FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
    {
      dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
      dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
      if(p_cur_pg_node->name[0] == 0
        || p_pre_pg_node->name[0] == 0)
      {
        ret = TRUE;
      }
      else
      {
        // ret = uni_strcmp(p_cur_pg_node->name, p_pre_pg_node->name) > 0 ;
        ret = uni_strcmp(p_pre_pg_node->name, p_cur_pg_node->name) > 0;
      }
    }
    break;
    case  DB_DVBS_SAT_NODE:
    {
      sat_node_t *p_cur_sat_node = (sat_node_t *)cur_node;
      sat_node_t *p_pre_sat_node = (sat_node_t *)prev_node;
      if(p_cur_sat_node->name[0] == 0
        || p_pre_sat_node->name[0] == 0)
      {
        ret = TRUE;
      }
      else
      {
        ret = uni_strcmp(p_pre_sat_node->name, p_cur_sat_node->name) > 0;
      }
    }
    break;
    case  DB_DVBS_TP_NODE:
      MT_ASSERT(0); //No tp name existing
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return ret;
}


static BOOL name_z_a_sort(u8 *prev_node, u8 *cur_node,
                          dvbs_sort_node_type_t node_type)
{
  BOOL ret = FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
    {
      dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
      dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
      ret = uni_strcmp(p_pre_pg_node->name, p_cur_pg_node->name) < 0;
    }
    break;
    case  DB_DVBS_SAT_NODE:
    {
      sat_node_t *p_cur_sat_node = (sat_node_t *)cur_node;
      sat_node_t *p_pre_sat_node = (sat_node_t *)prev_node;
      ret = uni_strcmp(p_pre_sat_node->name, p_cur_sat_node->name) < 0;
    }
    break;
    case  DB_DVBS_TP_NODE:
      MT_ASSERT(0); //No tp name existing
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return ret;
}


static BOOL lock_first_sort(u8 *prev_node, u8 *cur_node,
                            dvbs_sort_node_type_t node_type)
{
  dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
  dvbs_prog_node_t *p_prev_pg_node = (dvbs_prog_node_t *)prev_node;

  return p_cur_pg_node->lck_flag == 1
         && p_prev_pg_node->lck_flag == 0;
}


static BOOL default_order_sort(u8 *prev_node, u8 *cur_node,
                               dvbs_sort_node_type_t node_type)
{
  dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
  dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;

  return p_cur_pg_node->default_order > p_pre_pg_node->default_order;
}

static void sp_proc_tv(check_group_info_t *p, u16 node_id)
{
    if(p->tv_id == INVALIDID) //low priority
    {
      p->tv_id = node_id;
      p->tv_pos = 0;
    }
    if(p->orig_tv == node_id)  //found the orig id // first priority
    {
      p->tv_found = TRUE;
      p->tv_pos = p->tv_cur;
      p->tv_id = node_id;
    }
    if((p->orig_tv_pos == p->tv_cur) && (!p->tv_found))//found pos // sec priority
    {
      p->tv_found = TRUE;
      p->tv_pos = p->tv_cur;
      p->tv_id = node_id;
    }
    p->tv_cur++;
}

static void sp_proc_rd(check_group_info_t *p, u16 node_id)
{
    if(p->rd_id == INVALIDID) //low priority
    {
      p->rd_id = node_id;
      p->rd_pos = 0;
    }

    if(p->orig_rd == node_id)  //found the orig id // first priority
    {
      p->rd_found = TRUE;
      p->rd_pos = p->rd_cur;
      p->rd_id = node_id;
    }

    if((p->orig_rd_pos == p->rd_cur) && (!p->rd_found))//found pos // sec priority
    {
      p->rd_found = TRUE;
      p->rd_pos = p->rd_cur;
      p->rd_id = node_id;
    }
    p->rd_cur++;
}

static void special_check_all_group(spec_check_group_t *p_group, u16 node_id, pg_data_t *pg_info)
{
  check_group_info_t *p = &p_group->all_group;

  if(pg_info->video_pid != 0) //tv
  {
    sp_proc_tv(p, node_id);
  }
  else  //radio
  {
    sp_proc_rd(p, node_id);
  }
}

static void special_check_fav_group(spec_check_group_t *p_group, u16 node_id, pg_data_t *pg_info)
{
  u16 i = 0;
  check_group_info_t *p = NULL;

  for(i = 0; i < MAX_FAV_GRP; i++)
  {
    p = p_group->fav_group + i;

    if(pg_info->fav_grp_flag & ((u32)1<<p->context))
    {
      if(pg_info->video_pid != 0) //tv
      {
        sp_proc_tv(p, node_id);
      }
      else  //radio
      {
        sp_proc_rd(p, node_id);
      }
    }
  }
}

static BOOL special_check_group_filter(u16 bit_mask, u32 context,
  u8 block_id, u16 node_id)
{
  u8 element_buf[PG_MAX_LEN/4];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle  = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0, sizeof(pg_data_t));

  /*
   * When del pg due to TkgsIssue, only disable HideFlag, so we also can
   * filter the pg which has already del (actually its process hideflag 
   * not del), so we should discard it.
   */
  if(pg_data->hide_flag)
  {
    return FALSE;
  }

  special_check_fav_group((spec_check_group_t *)context, node_id, pg_data);
  special_check_all_group((spec_check_group_t *)context, node_id, pg_data);

  //FALSE represents need save to buffer.
  return FALSE;
}

static void sp_group_init(check_group_info_t *p)
{
  p->tv_found = FALSE;
  p->rd_found = FALSE;
  p->tv_id = INVALIDID;
  p->rd_id = INVALIDID;
  p->tv_pos = INVALIDPOS;
  p->rd_pos = INVALIDPOS;
  p->tv_cur = 0;
  p->rd_cur = 0;
}

void special_check_group(spec_check_group_t * p_group, u8 * buf_ex)
{
  int i = 0;
  u8 vv_id;
  item_type_t *p_view_buf = g_pg_view_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  MT_ASSERT(p_db_handle != NULL);

  //initialize parameter
  for(i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
    sp_group_init(p_group->sat_group + i);
  }

  for(i = 0; i < MAX_FAV_GRP; i++)
  {
    sp_group_init(p_group->fav_group + i);
  }

  sp_group_init(&p_group->all_group);

  if(buf_ex != NULL)
  {
    p_view_buf = (item_type_t *)buf_ex;
  }
  else
  {
    p_view_buf = g_pg_view_buf;
  }

  db_create_virtual_view(p_db_handle, g_pg_table_id, special_check_group_filter, FALSE,
    (u8 *)p_view_buf, (u32)p_group, &vv_id);
}


u8 db_dvbs_create_view(dvbs_view_t name, u32 context, u8 *buf_ex)
{
  u8 vv_id;
  BOOL bit_only = FALSE;
  u8 table_id = g_pg_table_id;
  item_type_t *p_view_buf = g_pg_view_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(name)
  {
    case DB_DVBS_ALL_RADIO:
	case DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_ALL_RADIO_FTA:
    case DB_DVBS_ALL_RADIO_CAS:
    case DB_DVBS_ALL_RADIO_HD:
    case DB_DVBS_ALL_TV:
	case DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG:
    case DB_DVBS_ALL_TV_FTA:
    case DB_DVBS_ALL_TV_CAS:
    case DB_DVBS_ALL_TV_HD:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_RADIO_FTA:
    case DB_DVBS_FAV_RADIO_CAS:
    case DB_DVBS_FAV_RADIO_HD:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_ALL_TV:
    case DB_DVBS_FAV_TV_FTA:
    case DB_DVBS_FAV_TV_CAS:
    case DB_DVBS_FAV_TV_HD:
    case DB_DVBS_ALL_PG:
	case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TV_RADIO:
    case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_MOSAIC:
    case DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG:

      break;
    case DB_DVBS_TP_PG:
    case DB_DVBS_TP_PG_SHOW:
	case DB_DVBS_TP_PG_IGNORE_SKIP_FLAG:
      bit_only = FALSE;
      break;
    case DB_DVBS_ALL_TP:
      table_id = g_tp_table_id;
      p_view_buf = g_all_tp_view_buf;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  if(buf_ex != NULL)
  {
    p_view_buf = (item_type_t *)buf_ex;
  }

  db_create_virtual_view(p_db_handle, table_id, filter_array[name], bit_only,
                         (u8 *)p_view_buf, context, &vv_id);
  g_vv_id_name_map[vv_id] = name;

  return vv_id;
}


u8 db_dvbs_create_view_all(dvbs_view_t name, u16 *p_context, u16 fav_count, u8 *buf_ex)
{
  u8 vv_id;
  BOOL bit_only = FALSE;
  u8 table_id = g_pg_table_id;
  item_type_t *p_view_buf = g_pg_view_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_RADIO_FTA:
    case DB_DVBS_ALL_RADIO_CAS:
    case DB_DVBS_ALL_RADIO_HD:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_FTA:
    case DB_DVBS_ALL_TV_CAS:
    case DB_DVBS_ALL_TV_HD:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_RADIO_FTA:
    case DB_DVBS_FAV_RADIO_CAS:
    case DB_DVBS_FAV_RADIO_HD:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_TV_FTA:
    case DB_DVBS_FAV_TV_CAS:
    case DB_DVBS_FAV_TV_HD:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_MOSAIC:		
      break;
    case DB_DVBS_TP_PG:
      bit_only = FALSE;
      break;
    case DB_DVBS_ALL_TP:
      table_id = g_tp_table_id;
      p_view_buf = g_all_tp_view_buf;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  if(buf_ex != NULL)
  {
    p_view_buf = (item_type_t *)buf_ex;
  }

  db_create_virtual_view_all(p_db_handle, table_id, filter_new_array[name], bit_only,
                             (u8 *)p_view_buf, p_context, fav_count, &vv_id);
  g_vv_id_name_map[vv_id] = name;

  return vv_id;
}


void db_dvbs_destroy_view(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_destroy_virtual_view(p_db_handle, view_id);
}


BOOL db_dvbs_get_view_del_flag(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  return db_get_view_del_flag(p_db_handle, view_id);
}

dvbs_view_t db_dvbs_get_view_type(u8 vv_id)
{
  return g_vv_id_name_map[vv_id];
}

db_dvbs_ret_t db_dvbs_init(void)
{
#ifndef DB_EDITOR
  static BOOL is_initialized = FALSE;
#endif
  u8 vv_cnt = 0;
  void *p_db_handle = NULL;

  if(is_initialized == FALSE)
  {

    //Initialize database 16v2
    db_init_database_16v2();

    //Get database handle
    p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
    MT_ASSERT(p_db_handle != NULL);

    g_tp_table_id = db_create_table(p_db_handle, TABLE_BLOCK_ID,
                                    DB_DVBS_MAX_TP);

    db_create_view(p_db_handle, VIEW_BLOCK_ID, g_tp_table_id);
    g_pg_table_id = db_create_table(p_db_handle, TABLE_BLOCK_ID,
                                    DB_DVBS_MAX_PRO);

    db_create_view(p_db_handle, VIEW_BLOCK_ID, g_pg_table_id);

    //Initaialize find view buffer
    g_find_view_buf = mtos_malloc(MAX_VIEW_BUFFER_SIZE);
    MT_ASSERT(g_find_view_buf != NULL);
    memset(g_find_view_buf, 0, MAX_VIEW_BUFFER_SIZE);

    //Initialize virtual view filter
    filter_array[DB_DVBS_ALL_RADIO] = all_radio_filter;
    filter_array[DB_DVBS_ALL_RADIO_FTA] = all_radio_fta_filter;
    filter_array[DB_DVBS_ALL_RADIO_CAS] = all_radio_cas_filter;
    filter_array[DB_DVBS_ALL_RADIO_HD] = all_radio_hd_filter;
    filter_array[DB_DVBS_ALL_TV] = all_tv_filter;
    filter_array[DB_DVBS_ALL_TV_FTA] = all_tv_fta_filter;
    filter_array[DB_DVBS_ALL_TV_CAS] = all_tv_cas_filter;
    filter_array[DB_DVBS_ALL_TV_HD] = all_tv_hd_filter;
    filter_array[DB_DVBS_FAV_RADIO] = fav_radio_filter;
    filter_array[DB_DVBS_FAV_RADIO_FTA] = fav_radio_fta_filter;
    filter_array[DB_DVBS_FAV_RADIO_CAS] = fav_radio_cas_filter;
    filter_array[DB_DVBS_FAV_RADIO_HD] = fav_radio_hd_filter;
    filter_array[DB_DVBS_FAV_TV] = fav_tv_filter;
    filter_array[DB_DVBS_FAV_TV_FTA] = fav_tv_fta_filter;
    filter_array[DB_DVBS_FAV_TV_CAS] = fav_tv_cas_filter;
    filter_array[DB_DVBS_FAV_TV_HD] = fav_tv_hd_filter;
    filter_array[DB_DVBS_FAV_ALL] = fav_all_filter;
    filter_array[DB_DVBS_TP_PG] = tp_pg_filter;
    filter_array[DB_DVBS_TP_PG_SHOW] = tp_pg_filter_show;
    filter_array[DB_DVBS_MOSAIC] = mosaic_filter;	
    filter_array[DB_DVBS_ALL_PG] = NULL;
    filter_array[DB_DVBS_ALL_TP] = NULL;


    filter_new_array[DB_DVBS_ALL_RADIO] = all_radio_new_filter;
    filter_new_array[DB_DVBS_ALL_RADIO_FTA] = all_radio_fta_new_filter;
    filter_new_array[DB_DVBS_ALL_RADIO_CAS] = all_radio_cas_new_filter;
    filter_new_array[DB_DVBS_ALL_RADIO_HD] = all_radio_hd_new_filter;
    filter_new_array[DB_DVBS_ALL_TV] = all_tv_new_filter;
    filter_new_array[DB_DVBS_ALL_TV_FTA] = all_tv_fta_new_filter;
    filter_new_array[DB_DVBS_ALL_TV_CAS] = all_tv_cas_new_filter;
    filter_new_array[DB_DVBS_ALL_TV_HD] = all_tv_hd_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO] = fav_radio_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO_FTA] = fav_radio_fta_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO_CAS] = fav_radio_cas_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO_HD] = fav_radio_hd_new_filter;
    filter_new_array[DB_DVBS_FAV_TV] = fav_tv_new_filter;
    filter_new_array[DB_DVBS_FAV_TV_FTA] = fav_tv_fta_new_filter;
    filter_new_array[DB_DVBS_FAV_TV_CAS] = fav_tv_cas_new_filter;
    filter_new_array[DB_DVBS_FAV_TV_HD] = fav_tv_hd_new_filter;
    filter_new_array[DB_DVBS_FAV_ALL] = fav_all_new_filter;
    filter_new_array[DB_DVBS_TP_PG] = tp_pg_new_filter;
    filter_new_array[DB_DVBS_MOSAIC] = mosaic_new_filter;	
    filter_new_array[DB_DVBS_ALL_PG] = NULL;
    filter_new_array[DB_DVBS_ALL_TP] = NULL;

    //initialize virtual view id and name map
    for(vv_cnt = 0; vv_cnt < MAX_VIRTUAL_VIEW_NUM; vv_cnt++)
    {
      g_vv_id_name_map[vv_cnt] = DB_DVBS_INVALID_VIEW;
    }

    is_initialized = TRUE;
  }
  return DB_DVBS_OK;
}


void db_dvbs_del_tp(u8 view_id, u16 pos)
{
  u8 pg_view_id = 0;
  u16 pg_total = 0;
  u16 i = 0;
  u16 tp_id = 0XFFFF;
  u16 param = DB_DVBS_PARAM_ACTIVE_FLAG;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  tp_id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  pg_view_id = db_dvbs_create_view(DB_DVBS_TP_PG, tp_id, NULL);
  pg_total = db_dvbs_get_count(pg_view_id);

  for(i = 0; i < pg_total; i++)
  {
    db_dvbs_change_mark_status(pg_view_id, i, DB_DVBS_DEL_FLAG, param);
    //db_dvbs_view_mark(pg_view_id, i, DB_DVBS_MARK_DEL, TRUE);
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, pg_view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, pg_view_id);

  db_del_view_item(p_db_handle, view_id, pos);
}


db_dvbs_ret_t db_dvbs_edit_program(dvbs_prog_node_t *p_node)
{
  u16 i, len;
  db_ret_t ret;
  u8 element_buf[PG_MAX_LEN/4] = {0};
  pg_data_t *p_pg   = (pg_data_t *)element_buf;
  audio_t *p_audio  = (audio_t *)(p_pg + 1);
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u16 *p_pg_name = NULL;
#ifdef MOSAIC_ENABLE  
  u16 ext_len = 0;
#endif
  MT_ASSERT(p_db_handle != NULL);
  OS_PRINTF("edit id %d, name %s\n", p_node->id, p_node->name);
  p_pg->tv_flag = p_node->tv_flag;
  p_pg->lck_flag = p_node->lck_flag;
  p_pg->skp_flag = p_node->skp_flag;
  /*lint -e732*/
  p_pg->default_order = p_node->default_order;
  p_pg->fav_grp_flag = p_node->fav_grp_flag;
  p_pg->audio_channel = p_node->audio_channel;
  p_pg->is_audio_channel_modify = p_node->is_audio_channel_modify;

  p_pg->audio_ch_num = p_node->audio_ch_num;
  p_pg->audio_track = p_node->audio_track;

  p_pg->is_scrambled = p_node->is_scrambled;
  p_pg->ca_system_id = p_node->ca_system_id;
  p_pg->pcr_pid = p_node->pcr_pid;
  p_pg->service_type = p_node->service_type;
  p_pg->sat_id = p_node->sat_id;
  p_pg->orig_net_id = p_node->orig_net_id;
  p_pg->s_id = p_node->s_id;
  p_pg->tp_id = p_node->tp_id;
  p_pg->pmt_pid = p_node->pmt_pid;
  p_pg->ecm_num = p_node->ecm_num;
  if(p_pg->ecm_num > MAX_ECM_DESC_NUM)
  {
    p_pg->ecm_num = MAX_ECM_DESC_NUM;
  }
  p_pg->video_pid = p_node->video_pid;
  p_pg->video_type = p_node->video_type;
//  OS_PRINTF("video type  : %d  %d line %d\n", p_pg->video_type, p_node->video_type, __LINE__);
  p_pg->volume = p_node->volume;
  p_pg->ts_id = p_node->ts_id;

  p_pg->hide_flag = p_node->hide_flag;


    p_pg->age_limit= p_node->age_limit;
    p_pg->mosaic_flag = p_node->mosaic_flag;	
    p_pg->logical_num = p_node->logical_num;
    p_pg->operator_flag = p_node->operator_flag;


  len = (u16)uni_strlen(p_node->name);
  p_pg->name_length = len > DB_DVBS_MAX_NAME_LENGTH
                      ? DB_DVBS_MAX_NAME_LENGTH : len;

  /*!Modify audio type*/
  for(i = 0; i < p_pg->audio_ch_num; i++, p_audio++)
  {
    memcpy(p_audio, p_node->audio + i, sizeof(audio_t));
  }

  p_pg_name = (u16 *)p_audio;
  if(p_pg->name_length > 0)
  {
    uni_strncpy(p_pg_name, p_node->name, p_pg->name_length);
    p_pg_name += p_pg->name_length + 1;
  }
  /*lint +e732*/
  /*lint -e737 -e571*/
  len = (u16)(sizeof(pg_data_t) + (p_pg->name_length + 1) * sizeof(u16)
              + p_pg->audio_ch_num * sizeof(audio_t));
#ifdef MOSAIC_ENABLE  
  if (p_pg->mosaic_flag)
  {
    p_pg->p_mosaic = (mosaic_t *)p_pg_name;
    ext_len = (u16)(sizeof(mosaic_t) - (MOSAIC_CELL_MAX - p_pg->mosaic_flag) * sizeof(mosaic_logic_cell_t));
    memcpy(p_pg->p_mosaic, &p_node->mosaic, ext_len);
    p_pg_name = (u16 *)((u8*)p_pg_name + ext_len + 1);

    len += ext_len + 1;
  }
 #endif 
#if 1 //ecm_num support
  if(p_pg->ecm_num)
  {
    p_pg->p_cas_ecm = (cas_desc_t *)p_pg_name;
    memcpy(p_pg->p_cas_ecm, p_node->cas_ecm, p_pg->ecm_num * sizeof(cas_desc_t));
    len += (u16)p_pg->ecm_num * sizeof(cas_desc_t);
  }
#endif
  /*lint +e737 +e571*/
  if(len >= PG_MAX_LEN/4)
 {
     len = PG_MAX_LEN/4;
 }
  ret = db_edit_element(p_db_handle, g_pg_table_id, (u16)p_node->id, element_buf, len);

  return translate_return_value(ret);
}


void db_dvbs_save_pg_edit(dvbs_prog_node_t *p_pg_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_save_element_edit(p_db_handle, g_pg_table_id, (u16)p_pg_node->id);
}


void db_dvbs_undo_pg_edit(dvbs_prog_node_t *p_pg_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_undo_element_edit(p_db_handle, g_pg_table_id, (u16)p_pg_node->id);
}

db_dvbs_ret_t db_dvbs_edit_tp(dvbs_tp_node_t *p_node)
{
  db_ret_t ret = DB_FAIL;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_edit_element(p_db_handle, g_tp_table_id, (u16)p_node->id, (u8 *)p_node,
                  sizeof(dvbs_tp_node_t));

  return translate_return_value(ret);
}


void db_dvbs_save_tp_edit(dvbs_tp_node_t *p_tp_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_save_element_edit(p_db_handle, g_tp_table_id, (u16)p_tp_node->id);
}


void db_dvbs_undo_tp_edit(dvbs_tp_node_t *p_tp_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_undo_element_edit(p_db_handle, g_tp_table_id, (u16)p_tp_node->id);
}


u16 db_dvbs_get_count(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  return db_get_virtual_view_count(p_db_handle, view_id);
}

void db_previosly_program_for_ae(void)
{
  u8 vv_id_tp = 0 ,vv_id_pro = 0;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  u32 cur_tp_id = 0;
  u32 i = 0;
  u32 j = 0;
  u32 frqtable_cnk = 0;
  u8 pgname[32] = {0};
  u32 FreqTable[] = {
    51000,  59000,  67000,  75000,  83000,  91000,  99000, 
    107000, 115000, 123000, 
    131000, 139000, 147000, 155000, 163000, 171000, 179000, 187000, 
    195000, 203000, 211000, 219000, 227000, 235000, 243000, 251000,
    259000, 267000, 275000, 283000, 291000, 299000, 307000, 315000,
    323000, 331000, 339000, 347000, 355000, 363000, 371000, 379000, 
    387000, 395000, 403000, 411000, 419000, 427000, 435000, 443000, 
    451000, 459000, 467000, 474000, 482000, 490000, 498000, 506000,
    514000, 522000, 530000, 538000, 546000, 554000, 562000, 570000,
    578000, 586000, 594000, 602000, 610000, 618000, 626000, 634000,
    642000, 650000, 658000, 666000, 674000, 682000, 690000, 698000,
    706000, 714000, 722000, 730000, 738000, 746000, 754000, 762000,
    770000, 778000, 786000, 794000, 802000, 810000, 818000, 826000,
    834000, 842000, 850000, 858000, 866000, 874000, 882000, 890000,
    };

  vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);

  frqtable_cnk = sizeof(FreqTable) /sizeof(u32);
  for(j = 0;j < 2;j ++)
  {
    if(j == 0)
    {
      tp.nim_modulate = NIM_MODULA_QAM64;
    }
    else
    {
      tp.nim_modulate = NIM_MODULA_QAM256;
    }
    
    for(i = 0;i < frqtable_cnk;i ++)
    {
      tp.freq = FreqTable[i];
      tp.sym = 6900;
      
      if(tp.nim_modulate == NIM_MODULA_QAM64)
      {
        sprintf((char *)pgname,"%ld_%s",tp.freq/1000,(u8 *)"QAM64");
      }
      else
      {
        sprintf((char *)pgname,"%ld_%s",tp.freq/1000,(u8 *)"QAM256");
      }
      
      db_dvbs_add_tp(vv_id_tp,&tp) ;
      /*lint -e732*/
      cur_tp_id = tp.id;
      /*lint +e732*/
      pg.s_id = 1;
      pg.ts_id = 2241;
      pg.orig_net_id = 1999;
      pg.pcr_pid = 256;
      pg.video_pid = 256;
      pg.is_scrambled = 0;
      pg.ca_system_id = 0;
      pg.pmt_pid = 32;
      pg.audio_ch_num = 1;
      pg.audio[0].p_id = 272;
      pg.audio[0].type = 2; 
      pg.tp_id = cur_tp_id;
      pg.volume = 16;
      pg.service_type = SVC_TYPE_TV;
      pg.audio_track = 1;
      pg.tv_flag = (pg.video_pid != 0) ? 1 : 0;
      pg.skp_flag = 0;
      pg.mosaic_flag = 0;
      pg.logical_num = i+1 + frqtable_cnk*j;
      dvb_to_unicode(pgname, sizeof(pgname),pg.name, DB_DVBS_MAX_NAME_LENGTH+1);
      db_dvbs_add_program(vv_id_pro, &pg) ;   
    }
  }

  db_dvbs_save(vv_id_tp);
  db_dvbs_save(vv_id_pro);
}

db_dvbs_ret_t db_dvbs_restore_to_factory(u8 blk_id, u8 ext_mem_blk_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  void *p_sc_handle = class_get_handle_by_id(SC_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  MT_ASSERT(p_sc_handle != NULL);

  //mtos_task_lock();
  //ss_set_status(SS_CHECKOUT, 0xFF); //write it to invalid
  ss_ctrl_clr_checksum(p_sc_handle);
  OS_PRINTF("load default start...\n");
  db_dvbs_init();
  db_clean(p_db_handle);

  //ss_set_status(SS_CHECKOUT, SS_CHECKOUT_VALUE); //write it back
  ss_ctrl_set_checksum(p_sc_handle);
  OS_PRINTF("load default finish.\n");
  //stk_info_rep();

  //mtos_task_unlock();
  return DB_DVBS_OK;
}

db_dvbs_ret_t db_dvbs_get_pg_by_id(u16 id, dvbs_prog_node_t *p_pg)
{
  u16 i = 0;
  audio_t *p_audio = NULL;
  u8 element_buf[PG_MAX_LEN/4] = {0};
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u16 *p_pg_name = NULL;
#ifdef MOSAIC_ENABLE  
  u16 ext_len = 0;
#endif
  MT_ASSERT(p_db_handle != NULL);
/*
  if(db_get_element(p_db_handle, g_pg_table_id, id, element_buf, 0,
                    sizeof(pg_data_t)
                    + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16)
                    + sizeof(audio_t) * DB_DVBS_MAX_AUDIO_CHANNEL)+sizeof(mosaic_t)
					+ sizeof(cas_desc_t) * MAX_ECM_DESC_NUM) */
  if(db_get_element(p_db_handle, g_pg_table_id, id, element_buf, 0,PG_MAX_LEN/4 - 1))
  {
    pg_data_t *p_node = (pg_data_t *)element_buf;
    p_audio = (audio_t *)(p_node + 1);

    memset(p_pg, 0x00, sizeof(dvbs_prog_node_t));
    p_pg->id = id;
    /*lint -e732*/
    p_pg->audio_channel = p_node->audio_channel;
    p_pg->is_audio_channel_modify = p_node->is_audio_channel_modify;
    p_pg->audio_track = p_node->audio_track;
    p_pg->is_scrambled = p_node->is_scrambled;
    p_pg->ca_system_id = p_node->ca_system_id;
    p_pg->pcr_pid = p_node->pcr_pid;
    p_pg->service_type = p_node->service_type;
    p_pg->sat_id = p_node->sat_id;
    p_pg->s_id = p_node->s_id;
    p_pg->tp_id = p_node->tp_id;
    p_pg->video_pid = p_node->video_pid;
    p_pg->video_type = p_node->video_type;
    p_pg->volume = p_node->volume;
    p_pg->audio_ch_num = (p_node->audio_ch_num < DB_DVBS_MAX_AUDIO_CHANNEL)
                         ? p_node->audio_ch_num : DB_DVBS_MAX_AUDIO_CHANNEL;
    p_pg->ts_id = p_node->ts_id;
    p_pg->orig_net_id = p_node->orig_net_id;
    p_pg->pmt_pid = p_node->pmt_pid;
    p_pg->ecm_num = p_node->ecm_num;
    if(p_pg->ecm_num > MAX_ECM_DESC_NUM)
    {
      p_pg->ecm_num = MAX_ECM_DESC_NUM;
    }
    p_pg->lck_flag = p_node->lck_flag;
    p_pg->skp_flag = p_node->skp_flag;
    p_pg->tv_flag = p_node->tv_flag;


    p_pg->age_limit= p_node->age_limit;
    p_pg->mosaic_flag = p_node->mosaic_flag;	
    p_pg->logical_num = p_node->logical_num;
    p_pg->operator_flag = p_node->operator_flag;

    p_pg->fav_grp_flag = p_node->fav_grp_flag;
    p_pg->default_order = p_node->default_order;
    /*lint +e732*/
    for(i = 0; i < p_pg->audio_ch_num; i++)
    {
      memcpy((p_pg->audio + i), p_audio, sizeof(audio_t));
      p_audio++;
    }
#if 0
    if(p_node->name_length > 0)
    {
      memset(p_pg->name, 0, DB_DVBS_MAX_NAME_LENGTH);
      strncpy((char *)p_pg->name, (char *)p_audio, p_node->name_length);
      if(p_node->name_length >= DB_DVBS_MAX_NAME_LENGTH)
      {
        p_pg->name[p_node->name_length - 1] = 0;
      }
      else
      {
        p_pg->name[p_node->name_length] = 0;
      }
    }
    else
    {
      strcpy((char *)p_pg->name, "noname");
    }
#else
    p_pg_name = (u16 *)p_audio;
    if(p_node->name_length > 0)
    {
      uni_strncpy(p_pg->name, p_pg_name, DB_DVBS_MAX_NAME_LENGTH);
      p_pg_name += p_node->name_length + 1;
    }
    else
    {
      //strcpy((char*)p_pg->name, "noname");
      str_nasc2uni((const u8 *)"noname", p_pg->name, DB_DVBS_MAX_NAME_LENGTH);
    }
#endif
#ifdef MOSAIC_ENABLE
  if (p_pg->mosaic_flag)
  {
    p_node->p_mosaic = (mosaic_t *)p_pg_name;
    ext_len = (u16)(sizeof(mosaic_t) - (MOSAIC_CELL_MAX - p_pg->mosaic_flag) * sizeof(mosaic_logic_cell_t));
    memcpy(&p_pg->mosaic, p_node->p_mosaic, ext_len);
    p_pg_name = (u16 *)((u8*)p_pg_name + ext_len + 1);
  }
#endif 

#if 1 //ecm_num support
    if(p_pg->ecm_num)
    {
      p_node->p_cas_ecm = (cas_desc_t *)p_pg_name;
      //lint -e737
      memcpy(p_pg->cas_ecm, p_node->p_cas_ecm, p_pg->ecm_num * sizeof(cas_desc_t));
      //lint +e737
    }
#endif
    return DB_DVBS_OK;
  }
  return DB_DVBS_FAILED;
}


db_dvbs_ret_t db_dvbs_get_tp_by_id(u16 id, dvbs_tp_node_t *p_tp)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  if(db_get_element(p_db_handle, g_tp_table_id, id, (u8 *)p_tp, 0, sizeof(dvbs_tp_node_t)))
  {
    p_tp->id = id;
    return DB_DVBS_OK;
  }
  return DB_DVBS_FAILED;
}

u16 db_dvbs_get_view_pos_by_id(u8 view_id, u16 id)
{
  u16 i = 0;
  u16 ret_id = 0;
  u16 count = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  count = db_get_virtual_view_count(p_db_handle, view_id);

  for(i = 0; i < count; i++)
  {
    ret_id = db_get_element_id_by_pos(p_db_handle, view_id, i);
    if(ret_id == id)
    {
      return i;
    }
  }
  return INVALIDID;
}


u16 db_dvbs_get_id_by_view_pos(u8 view_id, u16 pos)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  return db_get_element_id_by_pos(p_db_handle, view_id, pos);
}


db_dvbs_ret_t db_dvbs_move_item_in_view(u8 view_id, u16 pos_src, u16 pos_des)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_move_view_item(p_db_handle, view_id, pos_src, pos_des);
  return DB_DVBS_OK;
}


db_dvbs_ret_t db_dvbs_exchange_item_in_view(u8 view_id, u16 pos_src,
                                            u16 pos_des)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_exchange_view_item(p_db_handle, view_id, pos_src, pos_des);
  return DB_DVBS_OK;
}


db_dvbs_ret_t db_dvbs_dump_item_in_view(u8 view_id, void *p_mem, u16 len)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_dump_view_item(p_db_handle, view_id, p_mem, len);
  return DB_DVBS_OK;
}


void db_dvbs_save(u8 view_id)
{
  dvbs_view_t view_name = g_vv_id_name_map[view_id];
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(view_name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_ALL_TV:
	case DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG:
    case DB_DVBS_ALL_TV_FTA:
    case DB_DVBS_ALL_TV_CAS:
    case DB_DVBS_ALL_TV_HD:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_FAV_TV_FTA:
    case DB_DVBS_FAV_TV_CAS:
    case DB_DVBS_FAV_TV_HD:
    case DB_DVBS_ALL_PG:
	case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TV_RADIO:
    case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
    case DB_DVBS_MOSAIC:
    case DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG:
      break;

    case DB_DVBS_ALL_TP:
    case DB_DVBS_TP_PG:
	case DB_DVBS_TP_PG_IGNORE_SKIP_FLAG:
    case DB_DVBS_TP_PG_SHOW:
    {
      u16 tp_cnt = 0;
      u16 total_tp_cnt = db_dvbs_get_count(view_id);
      for(tp_cnt = 0; tp_cnt < total_tp_cnt; tp_cnt++)
      {
        if(db_dvbs_get_mark_status(view_id, tp_cnt, DB_DVBS_DEL_FLAG, 0) == 1)
        {
          db_dvbs_del_tp(view_id, tp_cnt);
        }
      }
    }
    break;
    default:
      MT_ASSERT(0);
      break;
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, view_id);
}

void db_dvbs_save_use_hide(u8 view_id)
{
  dvbs_view_t view_name = g_vv_id_name_map[view_id];
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(view_name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_FTA:
    case DB_DVBS_ALL_TV_CAS:
    case DB_DVBS_ALL_TV_HD:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_FAV_TV_FTA:
    case DB_DVBS_FAV_TV_CAS:
    case DB_DVBS_FAV_TV_HD:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_TP_PG:
    case DB_DVBS_TP_PG_SHOW:
      {
        u16 pg_cnt  = 0 ;
        u16 total_pg_cnt = 0;
        BOOL is_db_full = FALSE;
        total_pg_cnt = db_dvbs_get_count(view_id);
        sys_status_get_status(BS_DB_FULL, &is_db_full);
        for( pg_cnt = 0; pg_cnt < total_pg_cnt; pg_cnt++)
        {
          if(db_dvbs_get_mark_status(view_id, pg_cnt, DB_DVBS_DEL_FLAG, 0) == 1)
          {
            //db_dvbs_change_mark_status(view_id, pg_cnt, DB_DVBS_DEL_FLAG, 0);        

            //db_dvbs_change_mark_status(view_id, pg_cnt, DB_DVBS_HIDE_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
            if(is_db_full == TRUE)
            {
              sys_status_set_status(BS_DB_FULL, FALSE);
            }
           }
        }
      }      
      break;
      
    case DB_DVBS_ALL_TP:
    {
      u16 tp_cnt = 0;
      u16 total_tp_cnt = db_dvbs_get_count(view_id);
      for(tp_cnt = 0; tp_cnt < total_tp_cnt; tp_cnt++)
      {
        if(db_dvbs_get_mark_status(view_id, tp_cnt, DB_DVBS_DEL_FLAG, 0) == 1)
        {
          db_dvbs_del_tp(view_id, tp_cnt);
        }
      }
    }
    break;
    default:
      MT_ASSERT(0);
      break;
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, view_id);

  {
  db_dvbs_get_mark_status(view_id, 0, DB_DVBS_HIDE_FLAG, 0) ;
  }

}


void db_dvbs_undo(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_undo_view(p_db_handle, view_id);
  db_undo_element(p_db_handle, view_id);
}


BOOL is_tp_full(void)
{
  u8 view_id = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  if(db_get_real_view_count(p_db_handle, view_id) >= DB_DVBS_MAX_TP)
  {
    return TRUE;
  }
  return FALSE;
}


BOOL is_pg_full(void)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u8 view_id = 0;

  MT_ASSERT(p_db_handle != NULL);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  if(db_get_real_view_count(p_db_handle, view_id) >= DB_DVBS_MAX_PRO)
  {
    return TRUE;
  }
  return FALSE;
}


db_dvbs_ret_t db_dvbs_add_program(u8 view_id, dvbs_prog_node_t *p_node)
{
  u16 pos = 0;
  u16 i = 0;
  u16 len = 0;
  u16 *p_pg_name = NULL;
  db_ret_t ret = DB_SUC;
  pg_data_t *p_pg = NULL;
  audio_t *p_audio = NULL;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u8 element_buf[PG_MAX_LEN/4] = {0};
#ifdef MOSAIC_ENABLE  
  u16 ext_len = 0;
#endif
  MT_ASSERT(p_db_handle != NULL);

  p_pg = (pg_data_t *)element_buf;
  p_audio = (audio_t *)(p_pg + 1);

  default_order++;
  /*lint -e732*/
  p_pg->fav_grp_flag = p_node->fav_grp_flag;
  p_pg->audio_channel = p_node->audio_channel;
  p_pg->is_audio_channel_modify = p_node->is_audio_channel_modify;
  p_pg->audio_ch_num = p_node->audio_ch_num;
  p_pg->audio_track = p_node->audio_track;
  p_pg->is_scrambled = p_node->is_scrambled;
  p_pg->ca_system_id = p_node->ca_system_id;
  p_pg->pcr_pid = p_node->pcr_pid;
  p_pg->service_type = p_node->service_type;
  p_pg->sat_id = p_node->sat_id;
  p_pg->s_id = p_node->s_id;
  p_pg->tp_id = p_node->tp_id;
  p_pg->video_pid = p_node->video_pid;
  p_pg->video_type = p_node->video_type;
  OS_PRINTF("video type2 : %d  %d\n", p_pg->video_type, p_node->video_type);
  p_pg->volume = p_node->volume;
  p_pg->ts_id = p_node->ts_id;
  p_pg->orig_net_id = p_node->orig_net_id;
  p_pg->pmt_pid = p_node->pmt_pid;
  p_pg->ecm_num = p_node->ecm_num;
  /*lint +e732*/
  if(p_pg->ecm_num > MAX_ECM_DESC_NUM)
  {
    p_pg->ecm_num = MAX_ECM_DESC_NUM;
  }
 #ifdef DB_EDITOR
  p_pg->default_order = p_node->default_order;
 #else
  p_pg->default_order = default_order;
 #endif
  len = (u16)uni_strlen(p_node->name);
     OS_PRINTF("hello  hereuni_strlen(p_node->name)=%d\n",len); 

  p_pg->name_length = len > DB_DVBS_MAX_NAME_LENGTH
                      ? DB_DVBS_MAX_NAME_LENGTH : len;

  p_pg->tv_flag = p_node->tv_flag;
  p_pg->lck_flag = p_node->lck_flag;
  p_pg->skp_flag = p_node->skp_flag;
  p_pg->mosaic_flag = p_node->mosaic_flag;
  p_pg->hide_flag = p_node->hide_flag;
  /*lint -e732*/
    p_pg->age_limit= p_node->age_limit;
    p_pg->logical_num = p_node->logical_num;
    p_pg->operator_flag = p_node->operator_flag;
    p_pg->fav_grp_flag = p_node->fav_grp_flag;
  /*lint +e732*/
  if(p_pg->audio_ch_num > DB_DVBS_MAX_AUDIO_CHANNEL)
  {
    p_pg->audio_ch_num = DB_DVBS_MAX_AUDIO_CHANNEL;
  }
  for(i = 0; i < p_pg->audio_ch_num; i++)
  {
    memcpy(p_audio, (p_node->audio + i), sizeof(audio_t));
    //*p_audio++ = p_node->audio_pid[i];
    p_audio++;
  }

  p_pg_name = (u16 *)p_audio;
  if(p_pg->name_length > 0)
  {
  /*lint -e732*/
    uni_strncpy((u16 *)p_pg_name, p_node->name, p_pg->name_length);
  /*lint +e732*/
   p_pg_name += p_pg->name_length + 1;
  }
  else
  {
    u8 p_no_name[LEN_OF_NO_NAME] = {0};
    u8 service_id [6] = {0};

    sprintf((char *)p_no_name, "CH-");
    sprintf((char *)service_id, "%d", p_node->s_id);
    strcat((char *)p_no_name, (const char *)service_id);

    //Add the unicode of noname

    str_nasc2uni(p_no_name, p_pg_name, LEN_OF_NO_NAME);
    p_pg->name_length = uni_strlen(p_pg_name);
  }
  //lint -e737 -e571
  len = (u16)(sizeof(pg_data_t) + (p_pg->name_length + 1) * sizeof(u16)
              + p_pg->audio_ch_num * sizeof(audio_t));
#ifdef MOSAIC_ENABLE
  if (p_pg->mosaic_flag)
  {
    p_pg->p_mosaic = (mosaic_t *)p_pg_name;
    ext_len = (u16)(sizeof(mosaic_t) - (MOSAIC_CELL_MAX - p_pg->mosaic_flag) * sizeof(mosaic_logic_cell_t));
    memcpy(p_pg->p_mosaic, &p_node->mosaic, ext_len);
    p_pg_name = (u16 *)((u8*)p_pg_name + ext_len + 1);

    len += ext_len + 1;
  }
#endif

#if 1 //ecm_num support
  if(p_pg->ecm_num)
  {
    p_pg->p_cas_ecm = (cas_desc_t *)(p_pg_name);
    memcpy(p_pg->p_cas_ecm, p_node->cas_ecm, p_pg->ecm_num * sizeof(cas_desc_t));
    len += (u16)p_pg->ecm_num * sizeof(cas_desc_t);
  }
#endif
  //lint +e737 +e571
  if(len >= PG_MAX_LEN/4)
  {
    len = PG_MAX_LEN/4;
  }
  
  ret = db_add_view_item(p_db_handle, view_id, (u8 *)p_pg, len, &pos);
  p_node->id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  
  return translate_return_value(ret);
}


db_dvbs_ret_t db_dvbs_add_tp(u8 view_id, dvbs_tp_node_t *p_node)
{
  db_ret_t ret = {DB_FAIL};
  u16 pos = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  ret = db_add_view_item(p_db_handle, view_id, (u8 *)p_node,
                         sizeof(dvbs_tp_node_t), &pos);
  p_node->id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  return translate_return_value(ret);
}


BOOL db_dvbs_get_special_pg(u16 ts_id, u16 s_id, dvbs_prog_node_t *p_pg)
{
  BOOL ret = FALSE;
  u16 count = 0;
  u16 pg_id = 0;
  u16 i = 0;
  u8 view_id = 0;
  item_type_t *p_array = NULL;

  MT_ASSERT(p_pg != NULL);

  p_array = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(item_type_t));
  MT_ASSERT(p_array != NULL);
  memset(p_array, 0, DB_DVBS_MAX_PRO * sizeof(item_type_t));

  view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, (u8 *)p_array);
  count = db_dvbs_get_count(view_id);
  for(i = 0; i < count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, p_pg);
    if((p_pg->ts_id == ts_id)
      && (p_pg->s_id == s_id))
    {
      ret = TRUE;
      break;
    }
  }
  db_dvbs_destroy_view(view_id);
  mtos_free(p_array);
  return ret;
}


void db_dvbs_change_mark_status(u8 view_id, u16 item_pos,
                                dvbs_element_mark_digit dig, u16 param)
{
  BOOL is_set = ((param & 0x8000) != 0) ? TRUE : FALSE;
  // Operation can be completed based on view id
  BOOL view_only = FALSE;
  BOOL is_del_operation = FALSE;
  // Parameter for view operation
  u8 mask_shift = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(dig)
  {
    case DB_DVBS_MARK_TV:
      break;
    case DB_DVBS_MARK_LCK:
      break;
    case DB_DVBS_MARK_SKP:
      break;
    case DB_DVBS_FAV_GRP:
      mask_shift = param & 0xFF;
      break;
    case DB_DVBS_DEL_FLAG:
      view_only = TRUE;
      is_del_operation = TRUE;
      mask_shift = DB_DVBS_MARK_DEL;
      break;
    case DB_DVBS_SEL_FLAG:
      view_only = TRUE;
      mask_shift = DB_DVBS_MARK_SEL;
      break;
    case DB_DVBS_HIDE_FLAG:
      break;
  }

  if(view_only == TRUE)
  {
    if(is_del_operation == TRUE)
    {
      if(is_set == TRUE)
      {
        db_del_view_item(p_db_handle, view_id, item_pos);
      }
      else
      {
        db_active_view_item(p_db_handle, view_id, item_pos);
      }
    }
    else
    {
      db_set_mask_view_item(p_db_handle, view_id, item_pos, mask_shift,
                            is_set);
    }
    //db_update_view(view_id);
  } //View operation
  else
  {
    //PG only
    pg_data_t pg_data = { 0 };
    u16 element_id = db_get_element_id_by_pos(p_db_handle, view_id, item_pos);
    u16 cache_data_len = PG_NODE_VALID_CACHE_DATA_LEN;
    BOOL b_ret = FALSE;

    b_ret = db_get_element(p_db_handle, g_pg_table_id, element_id, (u8 *)&pg_data, 0, sizeof(pg_data_t));
    MT_ASSERT((u32)(b_ret == TRUE));
  /*lint -e732*/
    switch(dig)
    {
      case DB_DVBS_MARK_TV:
        pg_data.tv_flag = is_set;
        break;
      case DB_DVBS_MARK_LCK:
        pg_data.lck_flag = is_set;
        break;
      case DB_DVBS_MARK_SKP:
        pg_data.skp_flag = is_set;
        break;
      case DB_DVBS_HIDE_FLAG:
        pg_data.hide_flag = is_set;
        break;
      case DB_DVBS_FAV_GRP:
        if(is_set == TRUE)
        {
          pg_data.fav_grp_flag |= (u32)1 << mask_shift;
        }
        else
        {
          pg_data.fav_grp_flag &= ~((u32)1 << mask_shift);
        }
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  /*lint +e732*/
    db_edit_element(p_db_handle, g_pg_table_id, element_id, (u8 *)&pg_data,
                    cache_data_len);
  }
}


BOOL db_dvbs_get_mark_status(u8 view_id, u16 item_pos,
                             dvbs_element_mark_digit dig, u16 param)
{
  // Operation can be completed based on view id
  BOOL view_only = FALSE;
  u8 mask_shift = 0;  // Parameter for view operation
  BOOL ret = FALSE;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  //Differenciate view only and not
  switch(dig)
  {
    case DB_DVBS_MARK_TV:
      break;
    case DB_DVBS_MARK_LCK:
      break;
    case DB_DVBS_MARK_SKP:
      break;
    case DB_DVBS_FAV_GRP:
      mask_shift = param & 0xFF;
      break;
    case DB_DVBS_DEL_FLAG:
      view_only = TRUE;
      mask_shift = DB_DVBS_MARK_DEL;
      break;
    case DB_DVBS_SEL_FLAG:
      view_only = TRUE;
      mask_shift = DB_DVBS_MARK_SEL;
      break;
    case DB_DVBS_HIDE_FLAG:
      break;      
  }

  if(view_only == TRUE)
  {
    //View only
    if(dig == DB_DVBS_DEL_FLAG)
    {
      ret = !(db_get_mask_view_item(p_db_handle, view_id, item_pos,
                                    mask_shift));
    }
    else
    {
      ret = db_get_mask_view_item(p_db_handle, view_id, item_pos, mask_shift);
    }
  }
  else
  {
    //PG only
    pg_data_t pg_data = { 0 };
    u16 element_id = db_get_element_id_by_pos(p_db_handle, view_id, item_pos);

    db_get_element(p_db_handle, g_pg_table_id, element_id, (u8 *)&pg_data, 0, sizeof(pg_data_t));

    switch(dig)
    {
      case DB_DVBS_MARK_TV:
        ret = pg_data.tv_flag;
        break;
      case DB_DVBS_MARK_LCK:
        ret = pg_data.lck_flag;
        break;
      case DB_DVBS_MARK_SKP:
        ret = pg_data.skp_flag;
        break;
      case DB_DVBS_FAV_GRP:
        ret = (pg_data.fav_grp_flag & ((u32)1 << mask_shift)) ? TRUE : FALSE;
        break;
      case DB_DVBS_HIDE_FLAG:
        ret = pg_data.hide_flag;
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }

  return ret;
}


/*!
   \param[in]parent_view_id: id of the view to be searched for the input
      string
   \param[in]p_string_in: string to be searched for in given view
   \param[in/out]p_sub_view_id: the sub-view id
   return    TRUE:  input string existing
            FALSE: input string not found
  */
BOOL db_dvbs_find(u8 parent_view_id, const u16 *p_string_in,
                  u8 *p_sub_view_id)
{
  u16 view_item_cnt = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  // str_uni2asc(asc_str, (const u16 *) p_string_in);
  memset(g_find_view_buf, 0, MAX_VIEW_BUFFER_SIZE);
  view_item_cnt = db_create_sub_virtual_view(p_db_handle,
                                             string_find_filter,
                                             FALSE,
                                             g_find_view_buf,
                                             (u32)p_string_in,
                                             p_sub_view_id,
                                             parent_view_id);

  if(view_item_cnt == 0)
  {
    //Invalid sub-view is found
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}


void db_dvbs_pg_sort_init(u8 view_id)
{
  u16 view_cnt = 0;
  u16 outer_cnt = 0;
  u16 prv_element_id = 0;
  dvbs_prog_node_t prv_prog_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;
  u16 *p_mem_tmp = NULL;
  pro_sort_t *p_pro_item = NULL;
  s16 i = 0, j = 0;
  u16 count = 0;

  p_sort_buf = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(pro_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, DB_DVBS_MAX_PRO * sizeof(pro_sort_t));

  p_sort_id_buf = mtos_malloc(31 * sizeof(pro_sort_id_t));
  MT_ASSERT(p_sort_id_buf != NULL);
  memset(p_sort_id_buf, 0, 31 * sizeof(pro_sort_id_t));

  g_name_view.sort_type = DB_DVBS_A_Z_MODE;
  g_name_view.view_init = 0;
  g_name_view.view_count = 0;
  g_name_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_name_view.p_mem != NULL);
  memset(g_name_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  g_lock_view.sort_type = DB_DVBS_LOCK_FIRST_MODE;
  g_lock_view.view_init = 0;
  g_lock_view.view_count = 0;
  g_lock_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_lock_view.p_mem != NULL);
  memset(g_lock_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  g_free_view.sort_type = DB_DVBS_FTA_FIRST_MODE;
  g_free_view.view_init = 0;
  g_free_view.view_count = 0;
  g_free_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_free_view.p_mem != NULL);
  memset(g_free_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  g_default_view.sort_type = DB_DVBS_DEFAULT_ORDER_MODE;
  g_default_view.view_init = 0;
  g_default_view.view_count = 0;
  g_default_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_default_view.p_mem != NULL);
  memset(g_default_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  view_cnt = db_dvbs_get_count(view_id);
  p_pro_item = (pro_sort_t *)p_sort_buf;

  count = 0;

  for(outer_cnt = 0; outer_cnt < view_cnt; outer_cnt++)
  {
    prv_element_id = db_dvbs_get_id_by_view_pos(view_id, outer_cnt);
    ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);

    //a-z
    for(i = 0; i < 26; i++)
    {
      if(prv_prog_node.name[0] == (0x41 + i)
        || prv_prog_node.name[0] == (0x61 + i))
      {
        if(p_sort_id_buf[i].id_init == 0)
        {
          p_sort_id_buf[i].id_init = 1;
          p_sort_id_buf[i].id_count = 1;
          p_sort_id_buf[i].id_offset = 0;
          p_sort_id_buf[i].id_start = 0;
          break;
        }
        else
        {
          p_sort_id_buf[i].id_count++;
          break;
        }
      }
    }

    //other char
    if(i == 26)
    {
      if(p_sort_id_buf[i].id_init == 0)
      {
        p_sort_id_buf[i].id_init = 1;
        p_sort_id_buf[i].id_count = 1;
        p_sort_id_buf[i].id_offset = 0;
        p_sort_id_buf[i].id_start = 0;
      }
      else
      {
        p_sort_id_buf[i].id_count++;
      }
    }

    //fta
    if(prv_prog_node.is_scrambled == FALSE)
    {
      if(p_sort_id_buf[27].id_init == 0)
      {
        p_sort_id_buf[27].id_init = 1;
        p_sort_id_buf[27].id_count = 1;
        p_sort_id_buf[27].id_offset = 0;
        p_sort_id_buf[27].id_start = 0;
      }
      else
      {
        p_sort_id_buf[27].id_count++;
      }
    }
    else
    {
      if(p_sort_id_buf[28].id_init == 0)
      {
        p_sort_id_buf[28].id_init = 1;
        p_sort_id_buf[28].id_count = 1;
        p_sort_id_buf[28].id_offset = 0;
        p_sort_id_buf[28].id_start = 0;
      }
      else
      {
        p_sort_id_buf[28].id_count++;
      }
    }

    //lock
    if(prv_prog_node.lck_flag == FALSE)
    {
      if(p_sort_id_buf[29].id_init == 0)
      {
        p_sort_id_buf[29].id_init = 1;
        p_sort_id_buf[29].id_count = 1;
        p_sort_id_buf[29].id_offset = 0;
        p_sort_id_buf[29].id_start = 0;
      }
      else
      {
        p_sort_id_buf[29].id_count++;
      }
    }
    else
    {
      if(p_sort_id_buf[30].id_init == 0)
      {
        p_sort_id_buf[30].id_init = 1;
        p_sort_id_buf[30].id_count = 1;
        p_sort_id_buf[30].id_offset = 0;
        p_sort_id_buf[30].id_start = 0;
      }
      else
      {
        p_sort_id_buf[30].id_count++;
      }
    }

    p_pro_item++;
  }

  //(a-z) + other char.
  for(i = 0; i < 27; i++)
  {
    count = 0;
    for(j = 0; j < i; j++)
    {
      if(p_sort_id_buf[j].id_init == 1)
      {
        count += p_sort_id_buf[j].id_count;
      }
    }

    if(p_sort_id_buf[i].id_init == 1)
    {
      p_sort_id_buf[i].id_start = count;
    }
  }

  //fta and locked and letters separate
  if(p_sort_id_buf[27].id_init == 1)
  {
    p_sort_id_buf[27].id_start = 0;
  }

  if(p_sort_id_buf[28].id_init == 1)
  {
    p_sort_id_buf[28].id_start += p_sort_id_buf[27].id_count;
  }

  if(p_sort_id_buf[29].id_init == 1)
  {
    p_sort_id_buf[29].id_start = 0;
  }

  if(p_sort_id_buf[30].id_init == 1)
  {
    p_sort_id_buf[30].id_start += p_sort_id_buf[29].id_count;
  }

  for(outer_cnt = 0; outer_cnt < view_cnt; outer_cnt++)
  {
    prv_element_id = db_dvbs_get_id_by_view_pos(view_id, outer_cnt);
    ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);

    for(i = 0; i < 26; i++)
    {
      if(prv_prog_node.name[0] == (0x41 + i) || prv_prog_node.name[0] == (0x61 + i))
      {
        if(p_sort_id_buf[i].id_init == 1)
        {
          p_pro_item = p_sort_buf + p_sort_id_buf[i].id_start + p_sort_id_buf[i].id_offset;
          p_pro_item->node_id = prv_element_id;
          memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
          p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
          p_pro_item->lck_flag = prv_prog_node.lck_flag;
          //lint -e732
          p_pro_item->default_order = prv_prog_node.default_order;
          //lint +e732
          p_sort_id_buf[i].id_offset++;
        }
        break;
      }
    }

    if(i == 26)
    {
      if(p_sort_id_buf[i].id_init == 1)
      {
        p_pro_item = p_sort_buf + p_sort_id_buf[i].id_start + p_sort_id_buf[i].id_offset;
        p_pro_item->node_id = prv_element_id;
        memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
        p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
        p_pro_item->lck_flag = prv_prog_node.lck_flag;
        //lint -e732
        p_pro_item->default_order = prv_prog_node.default_order;
        //lint +e732
        p_sort_id_buf[i].id_offset++;
      }
    }

    if(prv_prog_node.is_scrambled == FALSE)
    {
      if(p_sort_id_buf[27].id_init == 1)
      {
        p_mem_tmp = g_free_view.p_mem + p_sort_id_buf[27].id_start + p_sort_id_buf[27].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[27].id_offset++;
      }
    }
    else
    {
      if(p_sort_id_buf[28].id_init == 1)
      {
        p_mem_tmp = g_free_view.p_mem + p_sort_id_buf[28].id_start + p_sort_id_buf[28].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[28].id_offset++;
      }
    }

    if(prv_prog_node.lck_flag == FALSE)
    {
      if(p_sort_id_buf[29].id_init == 1)
      {
        p_mem_tmp = g_lock_view.p_mem + p_sort_id_buf[29].id_start + p_sort_id_buf[29].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[29].id_offset++;
      }
    }
    else
    {
      if(p_sort_id_buf[30].id_init == 1)
      {
        p_mem_tmp = g_lock_view.p_mem + p_sort_id_buf[30].id_start + p_sort_id_buf[30].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[30].id_offset++;
      }
    }

  }
}


static BOOL str_cmp_a_z(pro_sort_t *prev_node, pro_sort_t *cur_node)
{
  return uni_strcmp(prev_node->name, cur_node->name);
}


static BOOL str_cmp_z_a(pro_sort_t *prev_node, pro_sort_t *cur_node)
{
  return uni_strcmp(cur_node->name, prev_node->name);
}


static BOOL default_order_cmp(pro_sort_t *prev_node, pro_sort_t *cur_node)
{
  return prev_node->default_order < cur_node->default_order;
}


void pro_str_change(pro_sort_t *p_1, pro_sort_t *p_2)
{
  pro_sort_t temp;
  int size = sizeof(pro_sort_t);
//lint -e732
  memcpy(&temp, p_1, size);
  memcpy(p_1, p_2, size);
  memcpy(p_2, &temp, size);
//lint +e732
}


void shellsort(pro_sort_t *p_sort, u32 count, dvbs_sort_type_t sort_type)
{
  u32 i, j, k;
  pro_sort_t temp_prog = {0};
  db_dvbs_qsort_func sort_func = NULL;
  u16 size = sizeof(pro_sort_t);

  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = str_cmp_a_z;
      break;

    case DB_DVBS_Z_A_MODE:
      sort_func = str_cmp_z_a;
      break;

    case DB_DVBS_FTA_FIRST_MODE:
    case DB_DVBS_LOCK_FIRST_MODE:
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_cmp;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  for(i = 0; i < (count - 1); i++)
  {
    k = i;
//lint -e613
    for(j = i + 1; j< count; j++)
    {
        if(sort_func(&p_sort[j], &p_sort[k]) > 0)
            k = j;
    }
//lint +e613
    if(k != i)
    {
        memcpy(&temp_prog, &p_sort[i], size);
        memcpy(&p_sort[i], &p_sort[k], size);
        memcpy(&p_sort[k], &temp_prog, size);
    }
  }
}



/*!
   \param[in]parent_view_id: view to be sorted
   \param[in]sort_type:			sort type to be selected
   \param[in]p_sub_view_id:	the sub-view created based on sort result
   \param[in]view:						view to be sorted
  */
#ifndef DB_EDITOR
void db_dvbs_pg_sort(u8 view_id, dvbs_sort_type_t sort_type)
{
  u16 view_cnt = 0, i = 0, j = 0;
  u16 n = 0, temp = 0, group_num = 0, *p_mem = NULL, *p_mem1 = NULL;
  pro_sort_id_t all = {0};

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
    case DB_DVBS_Z_A_MODE:
      n = 27;
      break;

    case DB_DVBS_FTA_FIRST_MODE:
    case DB_DVBS_LOCK_FIRST_MODE:
    case DB_DVBS_DEFAULT_ORDER_MODE:
      break;

    default:
      OS_PRINTF("%s, sort_type : %d\n", __FUNCTION__, sort_type);
      MT_ASSERT(0);
      break;
  }

  view_cnt = db_dvbs_get_count(view_id);

  p_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_mem != NULL);
  memset(p_mem, 0, view_cnt * sizeof(u16));
  p_mem1 = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_mem1 != NULL);
  memset(p_mem1, 0, view_cnt * sizeof(u16));

  if(sort_type == DB_DVBS_A_Z_MODE  || sort_type == DB_DVBS_Z_A_MODE)
  {
    if(g_name_view.view_init == 0)
    {
      for(i = 0; i < n; i++)
      {
        if(p_sort_id_buf[i].id_init == 1)
        {
          shellsort(p_sort_buf + p_sort_id_buf[i].id_start, p_sort_id_buf[i].id_count, DB_DVBS_A_Z_MODE);
        }
      }

      for(i = 0; i < view_cnt; i++)
      {
        *(p_mem + i) = p_sort_buf[i].node_id | 0x8000;
      }

      if(sort_type != DB_DVBS_A_Z_MODE)
      {
        for(i = 0; i < view_cnt / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (view_cnt - 1 - i));
          *(p_mem + (view_cnt - 1 - i)) = temp;
        }
      }

      g_name_view.view_init = 1;
      g_name_view.view_count = view_cnt;
      g_name_view.sort_type = sort_type;

      for(i = 0; i < view_cnt; i++)
      {
        *(g_name_view.p_mem + i) = *(p_mem + i);
      }
    }
    else
    {
      if(g_name_view.sort_type == sort_type)
      {
        for(i = 0; i < g_name_view.view_count; i++)
        {
          *(p_mem + i) = *(g_name_view.p_mem + i);
        }
      }
      else
      {
        for(i = 0; i < g_name_view.view_count; i++)
        {
          *(p_mem + i) = *(g_name_view.p_mem + i);
        }
        for(i = 0; i < g_name_view.view_count / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_name_view.view_count - 1 - i));
          *(p_mem + (g_name_view.view_count - 1 - i)) = temp;
        }
      }
    }
  }
  else if(sort_type == DB_DVBS_FTA_FIRST_MODE)
  {
    if(g_free_view.view_init == 0)
    {
      g_free_view.view_init = 1;
      g_free_view.view_count = view_cnt;
      g_free_view.sort_type = sort_type;

      for(i = 0; i < g_free_view.view_count; i++)
      {
        *(p_mem + i) = *(g_free_view.p_mem + i);
      }
    }
    else
    {
      if(g_free_view.sort_type == sort_type)
      {
        for(i = 0; i < g_free_view.view_count; i++)
        {
          *(p_mem + i) = *(g_free_view.p_mem + i);
        }
        for(i = 0; i < g_free_view.view_count / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_free_view.view_count - 1 - i));
          *(p_mem + (g_free_view.view_count - 1 - i)) = temp;
        }
        for(i = 0; i < view_cnt; i++)
        {
          *(g_free_view.p_mem + i) = *(p_mem + i);
        }
      }
    }
  }
  else if(sort_type == DB_DVBS_LOCK_FIRST_MODE)
  {
    if(g_lock_view.view_init == 0)
    {
      g_lock_view.view_init = 1;
      g_lock_view.view_count = view_cnt;
      g_lock_view.sort_type = sort_type;

      for(i = 0; i < g_lock_view.view_count; i++)
      {
        *(p_mem + i) = *(g_lock_view.p_mem + i);
      }
    }
    else
    {
      if(g_lock_view.sort_type == sort_type)
      {
        for(i = 0; i < g_lock_view.view_count; i++)
        {
          *(p_mem + i) = *(g_lock_view.p_mem + i);
        }
        for(i = 0; i < g_lock_view.view_count / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_lock_view.view_count - 1 - i));
          *(p_mem + (g_lock_view.view_count - 1 - i)) = temp;
        }
        for(i = 0; i < view_cnt; i++)
        {
          *(g_lock_view.p_mem + i) = *(p_mem + i);
        }
      }
    }
  }
  if(sort_type == DB_DVBS_DEFAULT_ORDER_MODE)
  {
    if(g_default_view.view_init == 0)
    {
      all.id_count = view_cnt;
      all.id_offset = view_cnt;
      all.id_start = 0;
      all.id_init = 1;
      group_num = sys_status_get_all_group_num();

      if(all.id_init == 1)
      {
        shellsort(p_sort_buf + all.id_start, all.id_count, sort_type);
      }

      for(i = 0; i < view_cnt; i++)
      {
        *(p_mem1 + i) = p_sort_buf[i].node_id | 0x8000;
      }

      for(j = 0; j < group_num - 1; j++)
      {
        for(i = j; i < view_cnt; i = i + group_num - 1)
        {
          *(p_mem + i) = *(p_mem1 + i);
        }
      }

      g_default_view.view_init = 1;
      g_default_view.view_count = view_cnt;
      g_default_view.sort_type = sort_type;

      for(i = 0; i < view_cnt; i++)
      {
        *(g_default_view.p_mem + i) = *(p_mem + i);
      }
    }
    else
    {
      if(g_default_view.sort_type == sort_type)
      {
        for(i = 0; i < g_default_view.view_count; i++)
        {
          *(p_mem + i) = *(g_default_view.p_mem + i);
        }
      }
    }
  }

  db_dvbs_dump_item_in_view(view_id, p_mem, view_cnt);

  mtos_free(p_mem);
  mtos_free(p_mem1);
}
#endif

void db_dvbs_pg_sort_deinit()
{
  mtos_free(p_sort_buf);
  mtos_free(p_sort_id_buf);

  g_name_view.sort_type = DB_DVBS_A_Z_MODE;
  g_name_view.view_init = 0;
  g_name_view.view_count = 0;
  mtos_free(g_name_view.p_mem);
  g_name_view.p_mem = NULL;

  g_free_view.sort_type = DB_DVBS_FTA_FIRST_MODE;
  g_free_view.view_init = 0;
  g_free_view.view_count = 0;
  mtos_free(g_free_view.p_mem);
  g_free_view.p_mem = NULL;

  g_lock_view.sort_type = DB_DVBS_LOCK_FIRST_MODE;
  g_lock_view.view_init = 0;
  g_lock_view.view_count = 0;
  mtos_free(g_lock_view.p_mem);
  g_lock_view.p_mem = NULL;

  g_default_view.sort_type = DB_DVBS_DEFAULT_ORDER_MODE;
  g_default_view.view_init = 0;
  g_default_view.view_count = 0;
  mtos_free(g_default_view.p_mem);
  g_default_view.p_mem = NULL;
}

/*!
   \param[in]parent_view_id: view to be sorted
   \param[in]sort_type:			sort type to be selected
   \param[in]p_sub_view_id:	the sub-view created based on sort result
   \param[in]view:						view to be sorted
  */
void db_dvbs_tp_sort(u8 view_id, dvbs_sort_type_t sort_type)
{
  db_dvbs_sort_func sort_func = NULL;
  u16 view_cnt = 0;

  u16 inner_cnt = 0;
  u16 outer_cnt = 0;

  u16 cur_element_id = 0;
  u16 prv_element_id = 0;

  dvbs_tp_node_t cur_tp_node = {0};
  dvbs_tp_node_t prv_tp_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = name_a_z_sort;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = name_z_a_sort;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
      sort_func = fta_first_sort;
      break;
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = lock_first_sort;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_sort;
      break;
    case DB_DVBS_LOCAL_NUM:
      break;
    case DB_DVBS_0_9_MODE:
      break;
    case DB_DVBS_9_0_MODE:
      break;
  }

  //Create view
  view_cnt = db_dvbs_get_count(view_id);

  for(outer_cnt = view_cnt; outer_cnt > 0; outer_cnt--)
  {
    for(inner_cnt = 0; inner_cnt < outer_cnt; inner_cnt++)
    {
      if(inner_cnt == 0)
      {
        prv_element_id = db_dvbs_get_id_by_view_pos(view_id, inner_cnt);
        ret = db_dvbs_get_tp_by_id(prv_element_id, &prv_tp_node);
        MT_ASSERT(ret == DB_DVBS_OK);
      }

      cur_element_id = db_dvbs_get_id_by_view_pos(view_id, inner_cnt + 1);
      ret = db_dvbs_get_tp_by_id(cur_element_id, &cur_tp_node);
      MT_ASSERT(ret == DB_DVBS_OK);
      //lint -e613 -e419 -e420
      if(sort_func((u8 *)&cur_tp_node, (u8 *)&prv_tp_node, DB_DVBS_PG_NODE)
         == TRUE)
      {
        //Remain current node status unchanged
        db_dvbs_exchange_item_in_view(view_id, outer_cnt, inner_cnt);
      }
      else
      {
        //Set current node to previous
        memcpy(&prv_tp_node, &cur_tp_node, sizeof(dvbs_prog_node_t));
      }
      //lint +e613 +e419 +e420
    }
  }

  //db_dvbs_destroy_view(view_id);
}

#ifndef DB_EDITOR
u32 get_audio_channel(dvbs_prog_node_t *p_pg)
{
  language_set_t lang_set;
  char **p_lang_code_list_b = sys_status_get_lang_code(TRUE);
  char **p_lang_code_list_t = sys_status_get_lang_code(FALSE);
  char *p_cur_lang_code_b = NULL;
  char *p_cur_lang_code_t = NULL;
  u16 b_type = 0;
  u16 t_type = 0;
  BOOL found_trace = FALSE;
  u16 pg_lang = 0;
  u32 i = 0;

  //get current language code
  sys_status_get_lang_set(&lang_set);

  //search first audio trace
  p_cur_lang_code_b = p_lang_code_list_b[lang_set.first_audio];
  p_cur_lang_code_t = p_lang_code_list_t[lang_set.first_audio];
  b_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_b));
  t_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_t));
  //lint -e574 -e732 -e737
  for(i = 0; i < p_pg->audio_ch_num; i++)
  {
    //match language code
    //if((memcmp(p_cur_lang_code_b, p_pg->audio[i].language, LANGUAGE_LEN) == 0)
    //  ||(memcmp(p_cur_lang_code_t, p_pg->audio[i].language, LANGUAGE_LEN) == 0))
    pg_lang = iso_639_2_idx_to_type(p_pg->audio[i].language_index);

    if((pg_lang == b_type)
      || (pg_lang == t_type))
    {
      found_trace = TRUE;
      break;
    }
  }
  //lint +e574 +e732 +e737
  //search second audio trace
  if(!found_trace)
  {
    p_cur_lang_code_b = p_lang_code_list_b[lang_set.second_audio];
    p_cur_lang_code_t = p_lang_code_list_t[lang_set.second_audio];
    b_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_b));
    t_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_t));
    //lint -e574 -e732 -e737
    for(i = 0; i < p_pg->audio_ch_num; i++)
    {
      //match language code
      //if((memcmp(p_cur_lang_code_b, p_pg->audio[i].language, LANGUAGE_LEN) == 0)
      //  ||(memcmp(p_cur_lang_code_t, p_pg->audio[i].language, LANGUAGE_LEN) == 0))
      pg_lang = iso_639_2_idx_to_type(p_pg->audio[i].language_index);

      if((pg_lang == b_type)
        || (pg_lang == t_type))
      {
        found_trace = TRUE;
        break;
      }
    }
    //lint +e574 +e732 +e737
  }
  return found_trace ? i : 0;
}
#endif


#ifdef DB_EDITOR
void db_dvbs_release(void)
  {
    is_initialized =  FALSE;
  }
#endif
