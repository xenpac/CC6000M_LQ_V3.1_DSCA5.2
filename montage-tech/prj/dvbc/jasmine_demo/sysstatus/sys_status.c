/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "string.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "unzip.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "hal_uart.h"

#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "common.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"

// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"

#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "cat.h"
#include "pmt.h"
#include "sdt.h"
#include "ts_packet.h"
#include "eit.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"

#include "db_dvbs.h"
#include "mem_cfg.h"
//ap
#ifdef WIN32
//#include "../ap/ap_flounder_ota.h"
#else
//#include "ap_flounder_ota.h"
#endif
#include "sys_status.h"
#include "mt_time.h"
#include "customer_config.h"
#include "str_id.h"
//#include "ui_comm_dlg.h"
#include "ota_dm_api.h"
#include "stdlib.h"

/*!
   which block store sysstatus data
 */
//#define SS_VAULE_BLOCK_ID       (IW_VIEW_BLOCK_ID)

/*!
   which block store sysstatus default data
 */
//#define SS_DEFAULT_BLOCK_ID    (SS_DATA_BLOCK_ID)

/*!
   text encode list
 */
static char * g_text_encode[] =
{
  "UNICODE", 
  "GB2312",
  "ASCII",
  "UTF8",
  "CP1256",
  "8859_9",   
};


/*!
   language code list
 */
static char * g_lang_code_b[] =
{
  "eng",
  "zh",
};


/*!
   language code list
 */
static char * g_lang_code_t[] =
{
  "eng",
  "zh",
};

/************************************************************************
* NOTICE:
*   1. curn_group must be set as zero when initialize.
*   2. sys_status_check_group will cause recreate prog views.
*
************************************************************************/

static sys_status_t g_status;

static sys_bg_data_t g_bg_data;
mdi_class_t *p_bg_data_mdi;

static u32 USER_ID_BG_DATA = 1;

void bl_sys_status_init(void)
{
  static u8 is_initialized = FALSE;
  BOOL ret = TRUE;

  if (is_initialized == TRUE)
  {
    return;
  }

  ret = ss_ctrl_init(SS_VAULE_BLOCK_ID, (u8*)&g_status, sizeof(sys_status_t));
  if (!ret)
  {
    //db_dvbs_restore_to_factory(PRESET_BLOCK_ID, 0);
    sys_status_load();
    ss_ctrl_clr_checksum(class_get_handle_by_id(SC_CLASS_ID));
  }

  is_initialized = TRUE;
}

void sys_status_init(void)
{
	static u8 is_initialized = FALSE;
	BOOL ret = TRUE;
  if (is_initialized == TRUE)
  {
    return;
  }

  ret = ss_ctrl_init(SS_VAULE_BLOCK_ID, (u8*)&g_status, sizeof(sys_status_t));
  if(!ret)
  {
    sys_status_load();
  }

  sys_status_set_auto_sleep(0);

  sys_bg_data_load();

  sys_status_save();

  is_initialized = TRUE;
  
  g_customer.b_WaterMark = g_status.b_WaterMark;
  g_customer.x_WaterMark = g_status.x_WaterMark;
  g_customer.y_WaterMark = g_status.y_WaterMark;

  #ifdef MIS_ADS
  g_customer.b_WaterMark = FALSE;
  g_status.b_WaterMark = FALSE;
  #endif
  
}


void sys_status_load(void)
{
  class_handle_t handle = class_get_handle_by_id(DM_CLASS_ID);

  dm_read(handle, SS_DEFAULT_BLOCK_ID, 0, 0, sizeof(sys_status_t), (u8*)(&g_status));
  sys_status_save();  //need save
  ss_ctrl_set_checksum(class_get_handle_by_id(SC_CLASS_ID));
}

void sys_bg_data_load(void)
{
  class_handle_t handle = class_get_handle_by_id(DM_CLASS_ID);
  u32 read_len = 0;

  p_bg_data_mdi = NULL;
  p_bg_data_mdi = mdi_create_instance(SYS_BG_IW_VIEW_BLOCK_ID, USER_ID_BG_DATA, sizeof(sys_bg_data_t));
  read_len = p_bg_data_mdi->read(p_bg_data_mdi, sizeof(sys_bg_data_t), (u8*)(&g_bg_data));
  if(read_len == 0)
  {
    dm_read(handle, SYS_BG_DATA_BLOCK_ID, 0, 0, sizeof(sys_bg_data_t), (u8*)(&g_bg_data));

    p_bg_data_mdi->write(p_bg_data_mdi, sizeof(sys_bg_data_t), (u8*)(&g_bg_data));
  }
}

void sys_status_save(void)
{
  ss_ctrl_sync(class_get_handle_by_id(SC_CLASS_ID),
    (u8*)&g_status);
}

sys_status_t *sys_status_get(void)
{
#if 0
  OS_PRINTF("- SYS STATUS INFO - \n");
  OS_PRINTF("\t OSD PAL   = %d\n", g_status.osd_set.palette);
  OS_PRINTF("\t OSD TMOUT = %d\n", g_status.osd_set.timeout);
  OS_PRINTF("\t OSD TRANS = %d\n", g_status.osd_set.transparent);
  OS_PRINTF("\t LANGUAGE  = %d\n", g_status.lang_set.osd_text);
  OS_PRINTF("\t CURN MODE = %d\n", g_status.group_set.curn_mode);
  OS_PRINTF("\t CURN GROP = %d\n", g_status.group_set.curn_group);
  OS_PRINTF("\t TV MODE   = %d\n", g_status.av_set.tv_mode);
  OS_PRINTF("\t TV RATIO  = %d\n", g_status.av_set.tv_ratio);
#endif
  return &g_status;
}

void sys_bg_data_save(void)
{
  p_bg_data_mdi->write(p_bg_data_mdi, sizeof(sys_bg_data_t), (u8*)(&g_bg_data));
}

sys_bg_data_t *sys_bg_data_get(void)
{
  return &g_bg_data;
}


void sys_status_get_sw_changeset(u8 *changeset)
{
  sys_status_t *status;

  status = sys_status_get();
  memcpy(changeset, status->changeset, sizeof(u8) * MAX_CHANGESET_LEN);
}

u32 sys_status_get_sw_version(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return status->sw_version;
}

void sys_status_set_sw_version(u32 new_version)
{
  sys_status_t *status;

  status = sys_status_get();

  status->sw_version = new_version;

  sys_status_save();
}


void sys_status_add_book_node(u8 index, book_pg_t *node)
{
  sys_bg_data_t *p_bg_data;
  book_pg_t *pg_info;

  if(index < MAX_BOOK_PG)
  {
    p_bg_data = sys_bg_data_get();

    pg_info = &p_bg_data->book_info.pg_info[index];

    memcpy(pg_info, node, sizeof(book_pg_t));

    sys_bg_data_save();
  }
  else
  {
    MT_ASSERT(0);
  }
}


void sys_status_delete_book_node(u8 index)
{
  sys_bg_data_t *p_bg_data;
  book_pg_t *pg_info;

  if(index < MAX_BOOK_PG)
  {
    p_bg_data = sys_bg_data_get();

    pg_info = &p_bg_data->book_info.pg_info[index];

    memset(pg_info, 0, sizeof(book_pg_t));

    pg_info->start_time.year = 2012;
    pg_info->start_time.month = 1;
    pg_info->start_time.day = 1;

    sys_bg_data_save();
  }
  else
  {
    MT_ASSERT(0);
  }
}

void sys_status_get_book_node(u8 index, book_pg_t *node)
{
  sys_bg_data_t *p_bg_data;

  if(index < MAX_BOOK_PG)
  {
    p_bg_data = sys_bg_data_get();

    memcpy(node, &(p_bg_data->book_info.pg_info[index]), sizeof(book_pg_t));
  }
  else
  {
    MT_ASSERT(0);
  }
}


BOOL sys_status_get_status(u8 type, BOOL *p_status)
{
  sys_status_t *status;

  if (type < BS_MAX_CNT)
  {
    status = sys_status_get();

    *p_status = (BOOL)SYS_GET_BIT(status->bit_status, type);
    return TRUE;
  }

  return FALSE;
}


BOOL sys_status_set_status(u8 type, BOOL enable)
{
  sys_status_t *status;

  //lint -e701 -e502
  if (type < BS_MAX_CNT)
  {
    status = sys_status_get();
    if (enable)
    {
      SYS_SET_BIT(status->bit_status, type);
    }
    else
    {
      SYS_CLR_BIT(status->bit_status, type);
    }
  //lint +e701 +e502
    return TRUE;
  }

  return FALSE;
}

void sys_status_set_pwd_set(pwd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->pwd_set = *p_set;
}

void sys_status_get_pwd_set(pwd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->pwd_set;
}


void sys_status_set_time(time_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->time_set = *p_set;
}

void sys_status_get_time(time_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->time_set;
}

void sys_status_set_play_set(play_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->play_set = *p_set;
}

void sys_status_get_play_set(play_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->play_set;
}

void sys_status_set_av_set(av_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->av_set = *p_set;
}

void sys_status_get_av_set(av_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->av_set;
}

void sys_status_set_audio_set(audio_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->audio_set = *p_set;
}

void sys_status_get_audio_set(audio_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->audio_set;
}

void sys_status_set_video_quality_set(u8 *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->video_quality = *p_set;
}

void sys_status_get_video_quality_set(u8 *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->video_quality;
}

void sys_status_set_osd_set(osd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->osd_set = *p_set;
}

void sys_status_get_osd_set(osd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->osd_set;
}

void sys_status_set_adver_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->ad_tp = *p_set;
}

void sys_status_get_adver_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->ad_tp;
}

void sys_status_set_upgrade_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->upgrade_tp = *p_set;
}

void sys_status_get_upgrade_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->upgrade_tp;
}

void sys_status_set_main_tp1(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->main_tp1 = *p_set;
}

void sys_status_get_main_tp1(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->main_tp1;
}

void sys_status_set_main_tp2(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->main_tp2 = *p_set;
}

void sys_status_get_main_tp2(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->main_tp2;
}

void sys_status_set_nvod_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->nvod_tp = *p_set;
}

void sys_status_get_nvod_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->nvod_tp;
}

void sys_status_set_range_begin_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->range_tp_begin = *p_set;
}

void sys_status_get_range_begin_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->range_tp_begin;
}

void sys_status_set_range_end_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->range_tp_end = *p_set;
}

void sys_status_get_range_end_tp(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->range_tp_end;
}

void sys_status_set_lang_set(language_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->lang_set = *p_set;
}

void sys_status_get_lang_set(language_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->lang_set;
}

BOOL sys_status_get_fav_name(u8 index, u16 *name)
{
  sys_bg_data_t *p_bg_data;

  if(index >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();

  memcpy(name, p_bg_data->fav_set.fav_group[index].fav_name, sizeof(u16) * MAX_FAV_NAME_LEN);

  return TRUE;
}

BOOL sys_status_set_fav_name(u8 index, u16 *name)
{
  sys_bg_data_t *p_bg_data;

  if(index >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();

  memcpy(p_bg_data->fav_set.fav_group[index].fav_name, name, sizeof(u16) * MAX_FAV_NAME_LEN);

  return TRUE;
}
void sys_status_set_local_set(local_set_t *p_set)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  p_bg_data->local_set = *p_set;

}

void sys_status_get_scan_param(scan_param_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->scan_param;
}

void sys_status_set_scan_param(scan_param_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->scan_param = *p_set;

}

void sys_status_get_local_set(local_set_t *p_set)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  *p_set = p_bg_data->local_set;
}

void sys_status_get_sw_info(sw_info_t *p_sw)
{
  sys_bg_data_t *p_bg_data;
  
  p_bg_data = sys_bg_data_get();

  *p_sw = p_bg_data->ver_info;
}

void sys_status_set_sw_info(sw_info_t *p_sw)
{
  sys_bg_data_t *p_bg_data;
  
  p_bg_data = sys_bg_data_get();

  p_bg_data->ver_info = *p_sw;
}


void sys_status_set_time_zone(void)
{
  time_set_t time_set_tmp;
  s8 zone;

  sys_status_get_time(&time_set_tmp);

  zone = (s8)(time_set_tmp.gmt_offset - 23);

  time_set_zone(zone);
  time_set_summer_time((BOOL)time_set_tmp.summer_time);
}

u32 sys_status_get_standby_time_out()
{
  return sys_status_get()->time_out;
}

void sys_status_set_standby_time_out(u32 time_out)
{
  sys_status_get()->time_out = time_out;

  sys_status_save();
}

void sys_status_get_utc_time(utc_time_t *p_time)
{
  time_set_t t_set;

  sys_status_get_time(&t_set);

  memcpy(p_time, &(t_set.sys_time), sizeof(utc_time_t));

  return;
}

void sys_status_set_utc_time(utc_time_t *p_time)
{
  time_set_t t_set;

  sys_status_get_time(&t_set);

  memcpy(&(t_set.sys_time), p_time, sizeof(utc_time_t));

  sys_status_set_time(&t_set);

  sys_status_save();
}

void sys_status_get_sleep_time(utc_time_t *p_time)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  memcpy(p_time, &(p_status->sleep_time), sizeof(utc_time_t));

  return;
}

void sys_status_set_sleep_time(utc_time_t *p_time)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  memcpy(&(p_status->sleep_time), p_time, sizeof(utc_time_t));

  sys_status_save();
}

void sys_status_get_pic_showmode(pic_showmode_t *p_showmode, BOOL is_net)
{
  sys_status_t *status;

  status = sys_status_get();

#ifdef ENABLE_NETWORK
  if(is_net)
  {
    memcpy(p_showmode, &status->pic_showmode_net, sizeof(pic_showmode_t));
  }
  else
#endif
  {
    memcpy(p_showmode, &status->pic_showmode, sizeof(pic_showmode_t));
  }
}

void sys_status_set_pic_showmode(pic_showmode_t *p_showmode)
{
  sys_status_t *status;

  status = sys_status_get();

  status->pic_showmode = *p_showmode;
}

char **sys_status_get_text_encode(void)
{
  return g_text_encode;
}

char **sys_status_get_lang_code(BOOL is_2_b)
{
  return is_2_b ? g_lang_code_b : g_lang_code_t;
}

u16 sys_status_get_force_key(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return (u16)status->force_key;
}

ota_info_t *sys_status_get_ota_info(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return &status->ota_info;
}

void sys_status_set_ota_info(ota_info_t *p_otai)
{
  sys_status_t *status;

  if(p_otai != NULL)
  {
    status = sys_status_get();

    memcpy((void *)&status->ota_info, (void *)p_otai, sizeof(ota_info_t));
  }
  else
  {
    MT_ASSERT(0);
  }
  return;
}

u32 sys_status_get_auto_sleep(void)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  return status->auto_sleep;
}

void sys_status_set_auto_sleep(u32 auto_sleep)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  status->auto_sleep = auto_sleep;  
}

void sys_status_set_usb_work_partition(u8 partition)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  status->usb_work_partition = partition;  
}

u8 sys_status_get_usb_work_partition(void)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  return status->usb_work_partition;
}

void sys_status_set_timeshift_switch(BOOL is_on)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  status->timeshift_switch = (u8)is_on;  
}

BOOL sys_status_get_timeshift_switch(void)
{
#ifdef ENABLE_TIMESHIFT_CONFIG
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  return (BOOL)status->timeshift_switch;
#else
  return FALSE;
#endif
}

BOOL sys_get_serial_num(u8 *p_sn, u8 length)
{
  u32 read_len = 0;
  u32 SN_LEN = 64;
  BOOL bRet = FALSE;
  u8 distance = 0;
  u8 cfg_tmp[64] = {0};
  u8 id_tmp[64] = {0};
  u8 cnk = 0;
  u32 id_len = 0;
  
  distance = 0; /***note  it is distance from 0x7fc00 for tcon  tools***/
  memset(cfg_tmp,0,sizeof(cfg_tmp));
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                            IDENTITY_BLOCK_ID, 0, 0,
                            SN_LEN,
                            (u8 *)cfg_tmp);
  if(read_len == 0)
  {
    bRet = FALSE;
    return bRet;
  }
  
  for(cnk = 0;cnk < SN_LEN;cnk ++)
  {
    if(cfg_tmp[distance + cnk] == 0xff)
    {
      cfg_tmp[distance + cnk] = '\0';
      break;
    }
  }
 
  memset(id_tmp,0,sizeof(id_tmp));
  memcpy(id_tmp,&cfg_tmp[distance],SN_LEN - distance);

  id_len = strlen((char *)id_tmp);

  if(id_len == 0)
  {
    return FALSE;
  }
  if(id_len > length)
  {
    id_len = length;
  }
  strncpy((char *)p_sn,(char *)id_tmp,id_len);

  bRet = TRUE;

  return bRet;
}

u8 sys_status_get_category_num(void)
{
  sys_bg_data_t *p_bg_data;
  
  p_bg_data = sys_bg_data_get();

  return p_bg_data->categories_count;
}

BOOL sys_status_set_category_num(u8 index)
{
  sys_bg_data_t *p_bg_data;

  if(index > MAX_CATEGORY_CNT)
  {
    return FALSE;
  }  

  p_bg_data = sys_bg_data_get();

  p_bg_data->categories_count = index;
  
  return TRUE;  
}

u16 sys_status_get_default_logic_num(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return status->default_logic_number;
}

void sys_status_set_default_logic_num(u16 num)
{
  sys_status_t *status;

  status = sys_status_get();

  status->default_logic_number = num;

  sys_status_save();
}

u16 sys_status_get_bouquet_id(u16 index)
{
  sys_bg_data_t *p_bg_data;
  
  p_bg_data = sys_bg_data_get();

  return p_bg_data->bouquet_id[index];
}

void sys_status_set_bouquet_id(u16 index, u16 bouquet_id)
{
  sys_bg_data_t *p_bg_data;
  
  p_bg_data = sys_bg_data_get();

  p_bg_data->bouquet_id[index] = bouquet_id;

  sys_bg_data_save();
}

u8 sys_status_get_categories_count(void)
{
  sys_bg_data_t *p_bg_data;
  
  p_bg_data = sys_bg_data_get();

  return p_bg_data->categories_count;
}

void sys_status_set_categories_count(u8 count)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  p_bg_data->categories_count = count;

  sys_bg_data_save();
}

void sys_status_get_nit_version(u32 *p_nit_ver)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  *p_nit_ver = p_bg_data->nit_version;
}

void sys_status_set_nit_version(u32 nit_ver)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  p_bg_data->nit_version = (u8)nit_ver;

  sys_bg_data_save();
}

void sys_status_reset_channel_and_nit_version(void)
{
  sys_status_set_nit_version(0xff);
#ifdef CHANNEL_FOLLOWING_TABLE
   sys_status_set_channel_version(0xff);
#endif
}

void sys_group_reset(void)
{
  sys_bg_data_t bg_data;
  class_handle_t handle = class_get_handle_by_id(DM_CLASS_ID);
  dm_read(handle, SYS_BG_DATA_BLOCK_ID/*SS_DEFAULT_BLOCK_ID*/, 0, 0, sizeof(sys_bg_data_t), (u8*)(&bg_data));

  memcpy(&g_bg_data.fav_set, &bg_data.fav_set, sizeof(fav_set_t));
  memcpy(&g_bg_data.book_info, &bg_data.book_info, sizeof(book_info_t));
  memcpy(&g_bg_data.group_set, &bg_data.group_set, sizeof(group_set_t));
  sys_bg_data_save();  //need save
}

utc_time_t sys_status_get_card_upg_time(void)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  return p_bg_data->card_upg_time;
}

void sys_status_set_card_upg_time(utc_time_t card_upg_time)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  memcpy(&(p_bg_data->card_upg_time), &card_upg_time, sizeof(utc_time_t));

  p_bg_data->card_upg_time.reserved = 1;

  sys_bg_data_save();

}

u32 sys_status_get_card_upg_state(void)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  return p_bg_data->card_upg_state;
}

void sys_status_set_card_upg_state(u32 card_upg_state)
{
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();

  p_bg_data->card_upg_state = card_upg_state;

  sys_bg_data_save();
}
BOOL sys_status_get_ota_preset(void *p_nim_param)
{
  sys_status_t *p_status = NULL;
  nim_para_t *p_ota_tp = (nim_para_t *)p_nim_param;
   //lint -e731
  MT_ASSERT(NULL != p_ota_tp);
  //lint +e731
  p_status = sys_status_get();

  p_ota_tp->lock_mode = p_status->ota_info.sys_mode;
  p_ota_tp->data_pid = p_status->ota_info.download_data_pid;
  
  switch(p_ota_tp->lock_mode)
  {
    case SYS_DVBS:
    {
      memcpy(&(p_ota_tp->locks), 
             &(p_status->ota_info.locks), 
             sizeof(dvbs_lock_info_t));
      break;
    }
    case SYS_DVBC:
    {
      memcpy(&(p_ota_tp->lockc), 
             &(p_status->ota_info.lockc), 
             sizeof(dvbc_lock_info_t));
      break;
    }
    case SYS_DTMB:
    case SYS_DVBT2:
    {
      memcpy(&(p_ota_tp->lockt), 
             &(p_status->ota_info.lockt), 
             sizeof(dvbt_lock_info_t));
      break;
    }
    default:
    {
      return FALSE;
    }
  }

  return TRUE;
}


void get_sys_stbid(u8 idsize,u8 *idbuf,u32 size)
{
    u32 read_len = 0;
    u32 id_len = 0;
    u8 cfg_tmp[64] = {0};
    u8 id_tmp[64] = {0};
    u8 cnk = 0;
    u8 distance = 0;

    distance = 12; /***note  it is distance from 0x7fc00 for tcon  tools***/
    memset(cfg_tmp,0,sizeof(cfg_tmp));
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
              IDENTITY_BLOCK_ID, 0, 0,
              64,
              (u8 *)cfg_tmp);
    if(read_len == 0)
    {
      strcpy((char *)&cfg_tmp[distance],(char *)"100001");
    }

    for(cnk = 0;cnk < idsize;cnk ++)
    {
      if((cfg_tmp[distance + cnk] < 0x30) ||(cfg_tmp[distance + cnk] > 0x39))
        {
          memset(cfg_tmp,0,sizeof(cfg_tmp));
          strcpy((char *)&cfg_tmp[distance],(char *)"100001");
          break;
        }
    }
    memset(id_tmp,0,sizeof(id_tmp));
    memcpy(id_tmp,&cfg_tmp[distance],idsize);

    id_len = strlen((char *)id_tmp);
    if(id_len > size)
      {
        id_len = size;
      }
    strncpy((char *)idbuf,(char *)id_tmp,id_len);
}

void sys_status_get_country_set(t2_search_info *p_set)
{
  p_set->country_style = 0;
}

#ifdef ENABLE_NETWORK

void sys_status_get_albums_friends_info(albums_friends_t *p_friends_info)
{
  *p_friends_info = g_bg_data.albums_friends;
}

void sys_status_set_albums_friends_info(albums_friends_t *p_friends_info)
{
   memcpy(&g_bg_data.albums_friends,p_friends_info,sizeof(albums_friends_t));
   sys_status_save();
}

void sys_set_3GinfoDefault(void)
{
	g3_conn_info_t	conn_info = {{0}, }; 
	strcpy(conn_info.apn,  "3gnet");
	strcpy(conn_info.dial_num,  "*99#");
	sys_status_set_3g_info(&conn_info);
}
void sys_set_wifiDefault(void)
{
  int i ;
  sys_bg_data_t* status = sys_bg_data_get();
  
  for(i=0; i<MAX_WIFI_NUM;i ++)
  {
    memset(&(status->wifi_set[i]),0,sizeof(wifi_info_t));
  }
}

void sys_status_set_ipaddress(ip_address_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->ipaddress_set = *p_set;
}

void sys_status_get_ipaddress(ip_address_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->ipaddress_set;
}

BOOL sys_status_set_ip_path_set(u8 index,u16 *name)
{
 sys_bg_data_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(status->ip_path_set.ip_group[index].ip_name, name, sizeof(u16) * 32);

  return TRUE;
}

BOOL sys_status_get_ip_path_set(u8 index,u16 *name)
{
  sys_bg_data_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(name, status->ip_path_set.ip_group[index].ip_name, sizeof(u16) * 32);

  return TRUE;
}

BOOL sys_status_set_ip_account(u8 index,u8 *account)
{
 sys_bg_data_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(status->ip_path_set.ip_group[index].ip_account, account, sizeof(u8) * 32);
  sys_status_save();

  return TRUE;
}

BOOL sys_status_get_ip_account(u8 index,u8 *account)
{
  sys_bg_data_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(account, status->ip_path_set.ip_group[index].ip_account, sizeof(u8) * 32);

  return TRUE;
}

BOOL sys_status_set_ip_passwd(u8 index,u8 *passwd)
{
 sys_bg_data_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(status->ip_path_set.ip_group[index].ip_password, passwd, sizeof(u8) * 32);
  sys_status_save();

  return TRUE;
}

BOOL sys_status_get_ip_passwd(u8 index,u8 *passwd)
{
  sys_bg_data_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(passwd, status->ip_path_set.ip_group[index].ip_password, sizeof(u8) * 32);

  return TRUE;
}

void sys_status_set_ip_path_cnt(u8 ip_path_cnt)
{
  sys_bg_data_t *status = NULL;

  status = sys_bg_data_get();  
  
  status->ip_path_cnt= ip_path_cnt;  
}

u8 sys_status_get_ip_path_cnt(void)
{
  sys_bg_data_t *status = NULL;

  status = sys_bg_data_get();

  return status->ip_path_cnt;
}

BOOL sys_status_set_city(u8 index,u16 *name)
{
 sys_bg_data_t *status;

  if(index > MAX_CITY_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(status->city_path_set.city_group[index].city_name, name, sizeof(u16) * 32);

  return TRUE;
}

BOOL sys_status_get_city(u8 index,u16 *name)
{
  sys_bg_data_t *status;

  if(index > MAX_CITY_CNT)
  {
    return FALSE;
  }

  status = sys_bg_data_get();

  memcpy(name, status->city_path_set.city_group[index].city_name, sizeof(u16) * 32);

  return TRUE;
}

void sys_status_set_city_cnt(u8 city_cnt)
{
  sys_bg_data_t *status = NULL;

  status = sys_bg_data_get();  
  
  status->city_cnt= city_cnt;  
}

u8 sys_status_get_city_cnt(void)
{
  sys_bg_data_t *status = NULL;

  status = sys_bg_data_get();

  return status->city_cnt;
}

BOOL sys_status_get_mac(u8 index, char *name)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();
 switch(index)
 {
  case 0:
    memcpy(name, status->mac_set.mac_one, 2);
    break;
  case 1:
    memcpy(name, status->mac_set.mac_two, 2);
    break;
  case 2:
    memcpy(name, status->mac_set.mac_three, 2);
    break;
  case 3:
    memcpy(name, status->mac_set.mac_four, 2);
    break;
 case 4:
    memcpy(name, status->mac_set.mac_five, 2);
    break;
 case 5:
    memcpy(name, status->mac_set.mac_six, 2);
    break;
 default:
    break;
 }
  return TRUE;
}

BOOL sys_status_set_mac(u8 index, char *name)
{
  sys_bg_data_t *status;


  status = sys_bg_data_get();

  switch(index)
 {
  case 0:
    memcpy(status->mac_set.mac_one, name, 2);
    break;
  case 1:
    memcpy(status->mac_set.mac_two, name, 2);
    break;
  case 2:
    memcpy( status->mac_set.mac_three, name, 2);
    break;
  case 3:
    memcpy( status->mac_set.mac_four, name, 2);
    break;
 case 4:
    memcpy(status->mac_set.mac_five, name, 2);
    break;
 case 5:
    memcpy(status->mac_set.mac_six, name, 2);
    break;
 default:
    break;
 }

  return TRUE;
}


char  sys_status_get_mac_by_index(u8 index)
{
  sys_bg_data_t *status;
  char hwaddr = '\0',*pstr;
  char name[3] = "0";

  status = sys_bg_data_get();
 switch(index)
 {
  case 0:
    memcpy(name, status->mac_set.mac_one, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 1:
    memcpy(name, status->mac_set.mac_two, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 2:
    memcpy(name, status->mac_set.mac_three, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 3:
    memcpy(name, status->mac_set.mac_four, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
 case 4:
    memcpy(name, status->mac_set.mac_five, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
 case 5:
    memcpy(name, status->mac_set.mac_six, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
 default:
    break;
 }
  return hwaddr;
}

void sys_status_set_net_upg_config_info(net_upg_info_t *p_info)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(&status->netupg_set, p_info, sizeof(net_upg_info_t));

  sys_status_save();
}

void sys_status_get_net_upg_config_info(net_upg_info_t *p_info)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(p_info, &status->netupg_set, sizeof(net_upg_info_t));
}


void sys_status_set_wifi_info(wifi_info_t *p_set)
{
  sys_bg_data_t *status;
  int index;
  u8 flag = 0;

  status = sys_bg_data_get();
  
  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((0 == flag) && strcmp(p_set->essid, status->wifi_set[index].essid) == 0)
      {
         status->wifi_set[index] = *p_set;
         status->wifi_set[index].latest = 1;
         flag = 1;
      }
      else
      {
        status->wifi_set[index].latest = 0;
      }
  }
  if(1 == flag)
  {
      return;
  }
  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((0 == flag) && (strlen(status->wifi_set[index].essid) == 0))
      {
         status->wifi_set[index] = *p_set;
         status->wifi_set[index].latest = 1;
         flag = 1;       
      }
      else
      {
        status->wifi_set[index].latest = 0;
      }
  }
  return;  
}

void sys_status_get_wifi_info(wifi_info_t *p_set,signed char* name)
{
  sys_bg_data_t *status;
  int index;
  
  status = sys_bg_data_get();

  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((name == NULL) && (status->wifi_set[index].essid != NULL))
        {
          if(1 == status->wifi_set[index].latest)
          {
            *p_set = status->wifi_set[index];
            return ;
          }
        }
      if((name != NULL) && (strcmp(name, status->wifi_set[index].essid) == 0))
      {
        *p_set = status->wifi_set[index];
         return ;
      }
  }

}

void sys_status_set_net_config_info(net_config_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  status->net_config_set = *p_set;
}

void sys_status_get_net_config_info(net_config_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  *p_set = status->net_config_set;
}

void sys_status_set_pppoe_config_info(pppoe_info_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  status->pppoe_set = *p_set;
}

void sys_status_get_pppoe_config_info(pppoe_info_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  *p_set = status->pppoe_set;
}

void sys_status_set_youtube_config_info(youtube_config_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  status->youtube_config_set = *p_set;
}

void sys_status_get_youtube_config_info(youtube_config_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  *p_set = status->youtube_config_set;
}

void sys_status_set_nm_simple_config_info(nm_simple_config_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  status->nm_simple_config_set = *p_set;
}

void sys_status_get_nm_simple_config_info(nm_simple_config_t *p_set)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  *p_set = status->nm_simple_config_set;
}


void sys_status_get_dlna_device_name(char* dlna_name)
{
	sys_bg_data_t *status;

	status = sys_bg_data_get();
	
	memcpy(dlna_name, status->dlna_device_name, 64);

}

void sys_status_set_dlna_device_name(char* dlna_name)
{
    sys_bg_data_t *status;

    status = sys_bg_data_get();

	  strcpy(status->dlna_device_name, dlna_name);

   sys_status_save();

}

void sys_status_get_fav_livetv(live_tv_fav *fav_livetv)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(fav_livetv, &(status->fav_live_pg), sizeof(live_tv_fav));
}

void sys_status_set_fav_livetv(live_tv_fav *fav_livetv)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(&(status->fav_live_pg),fav_livetv, sizeof(live_tv_fav));
}


void sys_status_set_fav_livetv_info(u32 index,live_tv_fav_info *fav_livetv_info)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(&(status->fav_live_pg.live_fav_pg[index]), fav_livetv_info, sizeof(live_tv_fav_info));
}

void sys_status_get_fav_livetv_info(u32 index,live_tv_fav_info *fav_livetv_info)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(fav_livetv_info, &(status->fav_live_pg.live_fav_pg[index]), sizeof(live_tv_fav_info));
}


void sys_status_del_fav_livetv_info(u32 index)
{
  sys_bg_data_t *status;
  u32 cnt = 0;

  status = sys_bg_data_get();
  
  cnt = status->fav_live_pg.live_tv_total_num - (index + 1);
  status->fav_live_pg.live_tv_total_num --;

  memmove(&(status->fav_live_pg.live_fav_pg[index]),&(status->fav_live_pg.live_fav_pg[index + 1]),
           sizeof(live_tv_fav_info) * cnt);
}

void sys_status_set_fav_livetv_total_num(u32 total_num)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  status->fav_live_pg.live_tv_total_num = total_num;
}

void sys_status_get_fav_livetv_total_num(u32 *total_num)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();
    
  *total_num = status->fav_live_pg.live_tv_total_num;
}

void sys_status_get_fav_iptv(iptv_fav *fav_iptv)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(fav_iptv, &(status->fav_iptv_pg), sizeof(iptv_fav));
}

void sys_status_set_fav_iptv(iptv_fav *fav_iptv)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(&(status->fav_iptv_pg),fav_iptv, sizeof(iptv_fav));
}


void sys_status_set_fav_iptv_info(u32 index,iptv_fav_info *fav_iptv_info)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(&(status->fav_iptv_pg.iptv_fav_pg[index]), fav_iptv_info, sizeof(iptv_fav_info));
}

void sys_status_get_fav_iptv_info(u32 index,iptv_fav_info *ip_tv_info)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  memcpy(ip_tv_info, &(status->fav_iptv_pg.iptv_fav_pg[index]), sizeof(iptv_fav_info));
}

void sys_status_del_fav_iptv_info(u32 index)
{
  sys_bg_data_t *status;
  u32 cnt = 0;

  status = sys_bg_data_get();

  cnt = status->fav_iptv_pg.iptv_total_num - (index + 1);
  status->fav_iptv_pg.iptv_total_num --;
  memmove( &(status->fav_iptv_pg.iptv_fav_pg[index]),&(status->fav_iptv_pg.iptv_fav_pg[index + 1]), 
         sizeof(iptv_fav_info) * cnt);
}

void sys_status_set_fav_iptv_total_num(u32 total_num)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  status->fav_iptv_pg.iptv_total_num= total_num;
}

void sys_status_get_fav_iptv_total_num(u32 *total_num)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();
    
  *total_num = status->fav_iptv_pg.iptv_total_num;
}

db_conn_play_item *sys_status_get_conn_play_table(void)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  return status->conn_play_table;
}

void sys_status_read_conn_play_item(u16 pos, db_conn_play_item *item)
{
  sys_bg_data_t *status;

  MT_ASSERT(pos < MAX_CONN_PLAY_CNT);
  status = sys_bg_data_get();

  memcpy(item, status->conn_play_table + pos, sizeof(db_conn_play_item));
}

void sys_status_write_conn_play_item(u16 pos, db_conn_play_item *item)
{
  sys_bg_data_t *status;

  MT_ASSERT(pos < MAX_CONN_PLAY_CNT);
  status = sys_bg_data_get();

  memcpy(status->conn_play_table + pos, item, sizeof(db_conn_play_item));
}

s32 sys_status_find_free_conn_play_item(void)
{
  sys_bg_data_t *status;
  db_conn_play_item *p_table;
  u16 pos;

  status = sys_bg_data_get();
  p_table = status->conn_play_table;

  for (pos = 0; pos < MAX_CONN_PLAY_CNT; pos++)
  {
    if (p_table[pos].is_used == FALSE)
    {
        return pos;
    }
  }

  return -1;
}

db_play_hist_item *sys_status_get_play_hist_table(void)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  return status->play_hist_table;
}

void sys_status_read_play_hist_item(u16 pos, db_play_hist_item *item)
{
  sys_bg_data_t *status;

  MT_ASSERT(pos < MAX_PLAY_HIST_CNT);
  status = sys_bg_data_get();

  memcpy(item, status->play_hist_table + pos, sizeof(db_play_hist_item));
}

void sys_status_write_play_hist_item(u16 pos, db_play_hist_item *item)
{
  sys_bg_data_t *status;

  MT_ASSERT(pos < MAX_PLAY_HIST_CNT);
  status = sys_bg_data_get();

  memcpy(status->play_hist_table + pos, item, sizeof(db_play_hist_item));
}

s32 sys_status_find_free_play_hist_item(void)
{
  sys_bg_data_t *status;
  db_play_hist_item *p_table;
  u16 pos;

  status = sys_bg_data_get();
  p_table = status->play_hist_table;

  for (pos = 0; pos < MAX_PLAY_HIST_CNT; pos++)
  {
    if (p_table[pos].is_used == FALSE)
    {
        return pos;
    }
  }

  return -1;
}

gprs_info_t *sys_status_get_gprs_info(void)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  return &status->gprs_info;
}

void sys_status_set_gprs_info(gprs_info_t *p_gprs_info)
{
  sys_bg_data_t *status;

  MT_ASSERT(p_gprs_info != NULL);

  status = sys_bg_data_get();

  memcpy((void *)&status->gprs_info, (void *)p_gprs_info, sizeof(gprs_info_t));

  return;
}

g3_conn_info_t *sys_status_get_3g_info(void)
{
  sys_bg_data_t *status;

  status = sys_bg_data_get();

  return &status->g3_conn_info;
}

void sys_status_set_3g_info(g3_conn_info_t *p_3g_info)
{
  sys_bg_data_t *status;

  MT_ASSERT(p_3g_info != NULL);

  status = sys_bg_data_get();

  memcpy((void *)&status->g3_conn_info, (void *)p_3g_info, sizeof(g3_conn_info_t));

  return;
}


void sys_status_get_epg_lang(epg_lang_t *e_lang)
{
  sys_status_t *status = NULL;

  if(NULL != e_lang)
  {
    status = sys_status_get(); 
  
    memcpy(e_lang, &status->e_lang_set, sizeof(epg_lang_t));
  }
}

void sys_status_set_epg_lang(epg_lang_t *e_lang)
{
  sys_status_t *status = NULL;

  if(NULL != e_lang)
  {
    status = sys_status_get(); 
  
    memcpy(&status->e_lang_set, e_lang, sizeof(epg_lang_t));
  }
}
#endif

#ifdef ONLY1_CA_VER
void sys_status_set_osd_content(char* osd_content,u16 len)
{
  sys_status_t *status;

  status = sys_status_get();
  memcpy(status->osd_data,osd_content,len);
  status->data_len = len;
}

void sys_status_get_osd_content(char* osd_content,u16* len)
{
  sys_status_t *status;

  status = sys_status_get();
  memcpy(osd_content,status->osd_data,MAXLEN_OSD);
  *len = status->data_len;
}

void sys_status_set_end_time(utc_time_t* current_time,u8 durtime)
{
  sys_status_t *status;

  status = sys_status_get();
  if((current_time->minute + durtime) < 60) 
  {
  	current_time->minute = current_time->minute + durtime;
  }
  else
  {
  	current_time->minute = current_time->minute + durtime -60;
  	current_time->hour++;
  }
  memcpy(&status->end_time,current_time,sizeof(utc_time_t));
 
}

void sys_status_get_end_time(utc_time_t* end_time)
{
  sys_status_t *status;

  status = sys_status_get();
  memcpy(end_time,&status->end_time,sizeof(utc_time_t));
}
u32 sys_status_get_osd_time(void)
{
  sys_status_t *status;

  status = sys_status_get();
  return status->osd_time;
}

void sys_status_set_osd_time(u32 second)
{
  sys_status_t *status;

  status = sys_status_get();
  status->osd_time = second;
}
#endif

#ifdef NIT_SETTING
u32 sys_status_get_nit_setting(void)
{
  sys_status_t *status;

  status = sys_status_get();
  return status->nit_setting;
}

void sys_status_set_nit_setting(u8 nit_setting)
{
  sys_status_t *status;

  status = sys_status_get();
  status->nit_setting = nit_setting;
}	

u32 sys_status_get_scankey_status(void)
{
  sys_status_t *status;

  status = sys_status_get();
  return status->scankey_status;
}

void sys_status_set_scankey_status(u8 scankey_status)
{
  sys_status_t *status;

  status = sys_status_get();
  status->scankey_status = scankey_status;
}	
#endif

#ifdef CHANNEL_FOLLOWING_TABLE
u8  sys_status_get_channel_version(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return status->channel_version;
}
void sys_status_set_channel_version(u8 new_version)
{
  sys_status_t *status;
  
  status = sys_status_get();

  status->channel_version = new_version;

  sys_status_save();
}
#endif

#ifdef MIS_ADS
void sys_status_get_MIS_ads_tp_info(dvbc_lock_t *p_set)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  *p_set = status->MIS_ads_tp_info;
}
void sys_status_set_MIS_ads_tp_info(dvbc_lock_t *p_set)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  status->MIS_ads_tp_info = *p_set;
  
  sys_status_save();
}

void sys_status_get_MIS_ads_dis_pid(u16 *p_dsi_pid)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  *p_dsi_pid = status->MIS_ads_dis_pid;
}
void sys_status_set_MIS_ads_dis_pid(u16 p_dsi_pid)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  status->MIS_ads_dis_pid = p_dsi_pid;
  
  sys_status_save();
}

void sys_status_get_MIS_ads_openApp_dis_pid(u16 *p_openApp_dsi_pid)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  *p_openApp_dsi_pid = status->MIS_ads_openApp_dis_pid;
}
void sys_status_set_MIS_ads_openApp_dis_pid(u16 p_openApp_dsi_pid)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  status->MIS_ads_openApp_dis_pid = p_openApp_dsi_pid;
  
  sys_status_save();
}
#endif

#ifdef PLAAS_DEXIN_STB_ID_COMBINE
void sys_status_get_stb_id_info(stb_id_com_t *p_set)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  *p_set = status->stb_id_com_info;
}
void sys_status_set_stb_id_info(stb_id_com_t *p_set)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  status->stb_id_com_info = *p_set;
  
  sys_status_save();
}
#endif

void sys_status_get_factory_test_result(factory_test_result_t *p_result)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  *p_result = status->factory_test_result;
}
void sys_status_set_factory_test_result(factory_test_result_t *p_result)
{
  sys_status_t *status;
  
  status = sys_status_get();
  
  status->factory_test_result = *p_result;
  
  sys_status_save();
}
