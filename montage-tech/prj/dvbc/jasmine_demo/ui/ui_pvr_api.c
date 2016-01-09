/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_pvr_api.h"
#include "vfs.h"
#include "common.h"
#include "drv_dev.h"
#include "dmx.h"
#include "vdec.h"
#include "ts_sequence.h"
#ifdef ENABLE_CA
#include "cas_manager.h"
#endif

typedef enum
{
  PVR_REC_START = 0,
  PVR_REC_PAUSE,
  PVR_REC_STOP
} pvr_rec_mode_t;

typedef enum
{
  PVR_PREVIEW_START = 0,
  PVR_PREVIEW_STOP
} pvr_preview_mode_t;

typedef enum
{
  /*!
    disk full
    */
  UI_REC_EVT_DISK_FULL = ((APP_RECORDER << 16) + 0),
  /*!
    record real time
    */
  UI_REC_EVT_REC_UPDATE,
  /*!
    record stopped,please add record.
    */
  UI_REC_EVT_REC_STOPPED,
  /*!
    record error
    */
  UI_REC_EVT_REC_ERROR,
} ui_record_api_evt_t;

typedef enum
{
  /*!
    play end
    */
  UI_TS_PLAYER_EVT_PLAY_EOF = ((APP_TS_PLAYER << 16) + 0),
  /*!
    resotre normal play
    */
  UI_TS_PLAYER_EVT_PLAY_SOF,
  /*!
    play tome update.
    */
  UI_TS_PLAYER_EVT_PLAY_TIME_UPDATE,
  /*!
    play error.
    */
  UI_TS_PLAYER_EVT_PLAY_ERROR,
} ts_player_api_evt_t;

typedef struct tag_ui_pvr_lock_flag
{
  flist_t* p_pvr_rec;
  u8 lock_flag;
  u8 reserved;
  u16 index;
} ui_pvr_lock_flag_t;
#define UI_PVR_LOCK_FLAG_CNT (8) //(10)
#define INVAILD_LOCK_FLAG (0xFF)

//#if ENABLE_MUSIC_PICTURE

#define TS_FILE_TYPE ("|TS|ts|nodir")

static u32 g_rec_id = 0;
static u8 *sp_rec_buffer = NULL;
static pvr_rec_mode_t g_rec_mode = PVR_REC_STOP;
static u16 g_partition_letter = 0;
static rec_info_t s_rec_info;

static u32 g_play_id = 0;
static vfs_dev_info_t g_dev_info = {0};
static flist_t* sp_last_rec_fav = NULL;

static u32 s_total_cnt = 0;
static flist_dir_t s_flist_dir = NULL;
static ui_pvr_lock_flag_t s_pvr_lock[UI_PVR_LOCK_FLAG_CNT];

static BOOL preview_state = FALSE;
static u16 s_cur_audio_pid = 0;
BOOL is_key_error = FALSE;
static rec_info_t g_current_play_rec_info;

u32 ca_msg_time = 0;

BOOL g_is_PVR_play = FALSE;

u16 ui_recorder_evtmap(u32 event);
u16 ts_player_evtmap(u32 event);

u32 ui_pvr_get_play_id(void)
{
  return g_play_id;
}

void str_uni_printf(const u16 *p_src)
{
  OS_PRINTF("\n");
  while(*p_src)
  {
    OS_PRINTF("%c",*p_src);
    p_src ++;
  }
}

void ui_pvr_get_partition_info(vfs_dev_info_t *p_info)
{
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;

  if(p_info == NULL)
    return;
  memset(p_info, 0, sizeof(vfs_dev_info_t));
  partition_cnt = vfs_get_partitions(partition_letter, 10);
  if(partition_cnt)
  {
    vfs_get_dev_info(partition_letter[sys_status_get_usb_work_partition()],p_info);
  }
}

u16 ui_pvr_get_capacity(void)
{
  mul_pvr_rec_status_t status;
  if(!g_dev_info.total_size)
    return 0;

  if(g_rec_id)
  {
    //record state
    mul_pvr_rec_get_status(g_rec_id, &status);
    //OS_PRINTF("[%d,%d,%d]\n",g_dev_info.used_size,status.cur_write_point,g_dev_info.total_size);
    return (u16)((g_dev_info.used_size + status.cur_write_point)/ (g_dev_info.total_size / 100));
  }
  else
    return (u16)(g_dev_info.used_size / (g_dev_info.total_size / 100));
}

static void pvr_lock_flag_add(flist_t *p_rec_fav, u16 index)
{
  u16 loopi = 0;

  loopi = index % UI_PVR_LOCK_FLAG_CNT;
  if (s_pvr_lock[loopi].index == index
      && s_pvr_lock[loopi].p_pvr_rec == p_rec_fav)
  {
    return;
  }

  s_pvr_lock[loopi].index = index;
  s_pvr_lock[loopi].p_pvr_rec = p_rec_fav;
  s_pvr_lock[loopi].lock_flag = INVAILD_LOCK_FLAG;
  /*
  for (loopi = 0; loopi < UI_PVR_LOCK_FLAG_CNT; loopi ++)
  {
    if (s_pvr_lock[loopi].p_pvr_rec == NULL)
    {
      break;
    }
    if (s_pvr_lock[loopi].index == index)
    {
      OS_PRINTF("\n##lock ready[add:%ld][%ld, 0x%x]", loopi, s_pvr_lock[loopi].index, s_pvr_lock[loopi].p_pvr_rec);
    }
    else
    {
      OS_PRINTF("\n##lock ready[%ld][%ld, 0x%x]", loopi, s_pvr_lock[loopi].index, s_pvr_lock[loopi].p_pvr_rec);
    }
  }*/
}

static ui_pvr_lock_flag_t * pvr_lock_flag_get(u16 index)
{
  return &s_pvr_lock[index % UI_PVR_LOCK_FLAG_CNT];
}

static void pvr_lock_flag_reset(void)
{
  u16 loopi = 0;

  for (loopi = 0; loopi < UI_PVR_LOCK_FLAG_CNT; loopi ++)
  {
    s_pvr_lock[loopi].index = 0;
    s_pvr_lock[loopi].p_pvr_rec = NULL;
    s_pvr_lock[loopi].lock_flag = INVAILD_LOCK_FLAG;
  }
}

static u32 ui_pvr_mk_rec_dir(void)
{
  u16 uni_tmp[16] = {0};
  partition_t *p_patition = NULL;

  file_list_get_partition(&p_patition);
  g_partition_letter = p_patition[sys_status_get_usb_work_partition()].letter[0];
  uni_tmp[0] = g_partition_letter;
  str_asc2uni((u8 *)":\\", uni_tmp + 1);

  str_asc2uni((u8 *)PVR_DIR, uni_tmp + 3);
  OS_PRINTF("sys_status_get_usb_work_partition(%d)\n",sys_status_get_usb_work_partition());
  if(vfs_mkdir(uni_tmp) != SUCCESS)
  {
    OS_PRINTF("%s, %s dir can't create or exist!\n", __FUNCTION__, PVR_DIR);
  }

  return 0;
}

u32 ui_pvr_load_rec_info(void)
{
  u16 dir_name[10] = {0};
  u16 dir_filter[20] = {0};
  flist_t *p_pvr_rec = NULL;
  u16 i=0,j=0;
  media_file_t *p_file = NULL;
  u16 idx_file[MAX_FILE_PATH] = {0};

  ui_pvr_mk_rec_dir();

  pvr_lock_flag_reset();
  //c:\PVRS
  dir_name[0] = g_partition_letter;
  str_asc2uni((u8 *)":\\", dir_name + 1);
  str_asc2uni((u8 *)PVR_DIR, dir_name + 3);

  str_asc2uni((u8 *)TS_FILE_TYPE, dir_filter);
  s_flist_dir = flist_open_dir(dir_filter, dir_name, &s_total_cnt);

  if (s_flist_dir == NULL)
  {
    return 0;
  }
  j=s_total_cnt;
  for(i=0; i<j; i++)
  {
    p_pvr_rec = flist_get_by_index(s_flist_dir, i);
    p_file = (media_file_t *)p_pvr_rec;
    if(p_file->size <= 0)
    {
      get_idx_info_file(flist_get_name(p_pvr_rec), idx_file, MAX_FILE_PATH);
      file_list_delete_file(idx_file);
      flist_del_by_index(s_flist_dir, i);
      s_total_cnt--;
      j--;
      if(i>0)
        i--;
    }
  }

  return s_total_cnt;
}

void ui_pvr_unload_rec_info(void)
{
  OS_PRINTF("\n##debug: ui_pvr_unload_rec_info !\n");
  if (s_flist_dir)
  {
    file_list_close_dir(s_flist_dir);
    s_flist_dir = NULL;
  }

  if (sp_rec_buffer)
  {
    mtos_free(sp_rec_buffer);
    sp_rec_buffer = NULL;
  }
  sp_last_rec_fav = NULL;
}

void ui_pvr_del_one_rec_byname(u16 index)
{
  u16 idx_file[MAX_FILE_PATH] = {0};
  flist_t* p_rec_fav = ui_pvr_get_rec_fav_by_index(index);

  if (s_flist_dir == NULL)
  {
    return ;
  }
  get_idx_info_file(flist_get_name(p_rec_fav), idx_file, MAX_FILE_PATH);
  file_list_delete_file(idx_file);

  flist_del_by_index(s_flist_dir, index);
  pvr_lock_flag_reset();
}

flist_t* ui_pvr_get_rec_fav_by_index(u16 index)
{
  flist_t *p_pvr_rec = NULL;

  if (s_flist_dir == NULL)
  {
    return NULL;
  }

  p_pvr_rec = flist_get_by_index(s_flist_dir, index);

  pvr_lock_flag_add(p_pvr_rec, index);

  return p_pvr_rec;
}

rec_info_t *ui_pvr_get_rec_info(flist_t* p_rec_fav)
{
  u16 idx_name[MAX_FILE_PATH] = {0};

  if (sp_last_rec_fav != p_rec_fav)
  {
    get_idx_info_file(flist_get_name(p_rec_fav), idx_name, MAX_FILE_PATH);

    if (!parse_record_info_idx(&s_rec_info, idx_name))
    {
      memset(&s_rec_info, 0, sizeof(rec_info_t));
      sp_last_rec_fav = NULL;
    }
    else
    {
      sp_last_rec_fav = p_rec_fav;
    }
  }

  return &s_rec_info;
}

u8 ui_pvr_get_lock_flag(u16 index)
{
  u16 idx_name[MAX_FILE_PATH] = {0};
  flist_t *p_pvr_rec = NULL;
  ui_pvr_lock_flag_t *p_pvr_lock = NULL;

  p_pvr_lock = pvr_lock_flag_get(index);

  if (p_pvr_lock->lock_flag == INVAILD_LOCK_FLAG
      || p_pvr_lock->p_pvr_rec == NULL)
  {
    p_pvr_rec = ui_pvr_get_rec_fav_by_index(index);
    get_idx_info_file(flist_get_name(p_pvr_rec), idx_name, MAX_FILE_PATH);

    if (!parse_record_info_idx(&s_rec_info, idx_name))
    {
      memset(&s_rec_info, 0, sizeof(rec_info_t));
      p_pvr_lock->lock_flag = 0;
      sp_last_rec_fav = NULL;
    }
    else
    {
      sp_last_rec_fav = p_pvr_rec;
      p_pvr_lock->lock_flag = (u8)s_rec_info.lock_flag;
    }
  }
  return p_pvr_lock->lock_flag;
}

void ui_pvr_change_lock_flag(u16 index)
{
  u16 idx_name[MAX_FILE_PATH] = {0};
  flist_t *p_pvr_rec = NULL;
  ui_pvr_lock_flag_t *p_pvr_lock = NULL;

  p_pvr_rec = flist_get_by_index(s_flist_dir, index);
  get_idx_info_file(flist_get_name(p_pvr_rec), idx_name, MAX_FILE_PATH);
  record_info_idx_change_lock_flag(idx_name);
  p_pvr_lock = pvr_lock_flag_get(index);
  p_pvr_lock->lock_flag = !p_pvr_lock->lock_flag;
  s_rec_info.lock_flag = !s_rec_info.lock_flag;
}

BOOL ui_pvr_is_scrambled(void)
{
  u16 i = 0;
  for(i=0; i<g_current_play_rec_info.ext_pid_cnt; i++)
  {
    if(g_current_play_rec_info.ext_pid[i].pid == DVB_CAT_PID)
      return TRUE;
  }

  return FALSE;
}

BOOL ui_get_pvr_scrambled_flag(rec_info_t *p_rec_info)
{
  u16 i = 0;
  for(i=0; i<p_rec_info->ext_pid_cnt; i++)
  {
    if(p_rec_info->ext_pid[i].pid == DVB_CAT_PID)
      return TRUE;
  }

  return FALSE;
}

RET_CODE ui_recorder_init(void)
{
  ui_pvr_mk_rec_dir();
  g_rec_mode = PVR_REC_STOP;
  ui_pvr_get_partition_info(&g_dev_info);

  fw_register_ap_evtmap(APP_RECORDER, ui_recorder_evtmap);
  fw_register_ap_msghost(APP_RECORDER, ROOT_ID_PVR_REC_BAR);
  fw_register_ap_msghost(APP_RECORDER, ROOT_ID_TS_RECORD);
  return SUCCESS;
}

RET_CODE ui_recorder_release(void)
{
  RET_CODE ret;
  ret = mul_pvr_rec_destroy_chn(g_rec_id);
  g_rec_mode = PVR_REC_STOP;
  g_rec_id = 0;

  fw_unregister_ap_evtmap(APP_RECORDER);
  fw_unregister_ap_msghost(APP_RECORDER, ROOT_ID_PVR_REC_BAR);
  fw_unregister_ap_msghost(APP_RECORDER, ROOT_ID_TS_RECORD);
  return ret;
}

void ui_recorder_up_devinfo(void)
{
  ui_pvr_get_partition_info(&g_dev_info);
}

static BOOL ui_is_all_mpeg_in_cur_tp(u16 cur_pg_id)
{
  BOOL ret = TRUE;
  u8  view_id = 0;
  u16 view_cnt = 0;
  u16 pg_cnt = 0;
  u16 pg_id = 0;
  dvbs_prog_node_t pg_node = {0};
  db_dvbs_ret_t db_ret = DB_DVBS_OK;

  db_ret = db_dvbs_get_pg_by_id(cur_pg_id, &pg_node);
  if (db_ret != DB_DVBS_OK)
  {
    return FALSE;
  }

  view_id = db_dvbs_create_view(DB_DVBS_TP_PG, (u16)pg_node.tp_id, NULL);
  view_cnt = db_dvbs_get_count(view_id);

  for (pg_cnt = 0; pg_cnt < view_cnt; pg_cnt++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, pg_cnt);
    MT_ASSERT(pg_id != DB_UNKNOWN_ID);
    db_dvbs_get_pg_by_id(pg_id, &pg_node);
    if (pg_node.video_type != VIDEO_MPEG)
    {
      OS_PRINTF("\n##not all VIDEO_MPEG in cur TP!\n\n\n");
      ret = FALSE;
      break;
    }
  }

  return ret;
}

void ui_pvr_calc_rec_addr_size(u8 **p_addr, u32 *size)
{
  u16 pg_id = 0;
  u32 _p_addr = 0;

  _p_addr = mem_mgr_require_block(BLOCK_REC_BUFFER, 0);
  mem_mgr_release_block(BLOCK_REC_BUFFER);
  OS_PRINTF("get addr in ui_tshift_rec_cfg 0x%x\n", _p_addr);
  MT_ASSERT(_p_addr != 0);

  pg_id = sys_status_get_curn_group_curn_prog_id();

  if (ui_is_all_mpeg_in_cur_tp(pg_id))
  {
    *p_addr = (u8 *)(_p_addr - VIDEO_FW_MPEG_LESS_H264);
    *size = mem_mgr_get_block_size(BLOCK_REC_BUFFER) + VIDEO_FW_MPEG_LESS_H264;
  }
  else
  {
    *p_addr = (u8 *)_p_addr;
    *size = mem_mgr_get_block_size(BLOCK_REC_BUFFER);
  }
  (*size) = (*size) - 10 * KBYTES;
}

static void ui_record_callback(u32 chnid, mul_pvr_event_t event_type, s32 event_value, void *p_args)
{
  event_t evt = {0};

  evt.data1 = (u32)event_value;
  evt.data2 = 0;
  switch(event_type)
  {
    case MUL_PVR_EVENT_REC_DISKFULL:
      if (!event_value)
      {
        evt.id = UI_REC_EVT_DISK_FULL;
      }
      else
      {
        evt.id = UI_REC_EVT_REC_ERROR;
      }
      break;
    case MUL_PVR_EVENT_REC_TIME_UPDATE:
      evt.id = UI_REC_EVT_REC_UPDATE;
      break;
    case MUL_PVR_EVENT_REC_RESV:
      evt.id = UI_REC_EVT_REC_STOPPED;
      OS_PRINTF("%s,%d,event_value=%d\n",__FUNCTION__,__LINE__,
                event_value);
      break;
    default:
      break;
  }

  ap_frm_send_evt_to_ui(APP_RECORDER, &evt);
}

RET_CODE ui_recorder_start(mul_pvr_rec_attr_t *p_rec_attr)
{
  RET_CODE ret = SUCCESS;

  if(g_rec_mode != PVR_REC_STOP)
  {
    OS_PRINTF("%s,record mode=%d,not start\n",__FUNCTION__,g_rec_mode);
    return ERR_FAILURE;
  }
#ifdef EXTERN_CA_PVR
    if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
      || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    ui_pvr_extern_open(p_rec_attr->file_name, UI_PVR_EXTERN_WRITE);
#endif
  g_rec_mode = PVR_REC_START;
  p_rec_attr->b_rewind = FALSE;

  p_rec_attr->demux_index = 0;
  p_rec_attr->stk_size = RECORD_CHAIN_TASK_STK_SIZE;
  p_rec_attr->user_data_size = 0;
  ui_pvr_calc_rec_addr_size(&p_rec_attr->p_rec_buffer, &p_rec_attr->rec_buffer_size);

  ret = mul_pvr_rec_create_chn(&g_rec_id, p_rec_attr);
  if(ret != SUCCESS)
  {
    OS_PRINTF("Error %d: %s call mul_pvr_rec_create_chn() failed!\n",
              ret, __FUNCTION__);
    return ERR_FAILURE;
  }

  ret = mul_pvr_register_event(g_rec_id, ui_record_callback, NULL);

  if(ret != SUCCESS)
  {
    OS_PRINTF("Error %d: %s call mul_pvr_register_event() failed!\n",
              ret, __FUNCTION__);
    return ERR_FAILURE;
  }

  ret = mul_pvr_rec_start_chn(g_rec_id);
  if(ret != SUCCESS)
  {
    OS_PRINTF("Error %d: %s call mul_pvr_rec_start_chn() failed!\n", ret, __FUNCTION__);
    return ERR_FAILURE;
  }
  if(g_customer.cus_bouquet_set  != NULL)
  {
    g_customer.cus_bouquet_set ((void *)1);
  }
  return ret;
}

RET_CODE ui_recorder_stop(void)
{
  RET_CODE ret = SUCCESS;

  if(g_rec_mode == PVR_REC_STOP)
  {
    OS_PRINTF("%s,already stop\n",__FUNCTION__);
    return ret;
  }
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    ui_pvr_extern_close();
#endif
  g_rec_mode = PVR_REC_STOP;
  ret = mul_pvr_rec_stop_chn(g_rec_id);
  if(ret == SUCCESS)
  {
    if(g_customer.cus_bouquet_set  != NULL)
    {
      g_customer.cus_bouquet_set ((void *)2);
    }
  }

  return ret;
}

RET_CODE ui_recorder_pause(void)
{
  RET_CODE ret = SUCCESS;

  if(g_rec_mode != PVR_REC_START)
  {
    OS_PRINTF("%s,not pause\n",__FUNCTION__);
    return ret ;
  }
  g_rec_mode = PVR_REC_PAUSE;

  ret = mul_pvr_rec_pause_chn(g_rec_id);
  if(ret == SUCCESS)
  {
    if(g_customer.cus_bouquet_set  != NULL)
    {
      g_customer.cus_bouquet_set ((void *)2);
    }
  }
  return ret;
}

RET_CODE ui_recorder_resume(void)
{
  RET_CODE ret = SUCCESS;

  if(g_rec_mode != PVR_REC_PAUSE)
  {
    OS_PRINTF("%s,not resume\n",__FUNCTION__);
    return ret;
  }
  g_rec_mode = PVR_REC_START;
  ret =  mul_pvr_rec_resume_chn(g_rec_id);
  if(ret == SUCCESS)
  {
    if(g_customer.cus_bouquet_set  != NULL)
    {
      g_customer.cus_bouquet_set ((void *)1);
    }
  }
  return ret;
}

BOOL ui_recorder_isrecording(void)
{
  return (g_rec_mode != PVR_REC_STOP);
}

void ui_recorder_end(void)
{
  mul_pvr_register_event(g_rec_id, NULL, NULL);;
  if(g_customer.cus_bouquet_set  != NULL)
  {
    g_customer.cus_bouquet_set ((void *)2);
  }
}

void ui_recorder_change_pid(mul_rec_media_t *p_media)
{
  if(g_rec_mode == PVR_REC_STOP)
  {
    OS_PRINTF("%s,rec stopped, cannot change pid\n",__FUNCTION__);
    return ;
  }

  mul_pvr_rec_change_pid(g_rec_id, p_media);
}

RET_CODE ts_player_init(void)
{
  ui_pvr_get_partition_info(&g_dev_info);

  fw_register_ap_evtmap(APP_TS_PLAYER, ts_player_evtmap);
  fw_register_ap_msghost(APP_TS_PLAYER, ROOT_ID_RECORD_MANAGER);
  fw_register_ap_msghost(APP_TS_PLAYER, ROOT_ID_PVR_PLAY_BAR);
  return SUCCESS;
}

RET_CODE ts_player_release(void)
{
  ts_player_stop();

  fw_unregister_ap_evtmap(APP_TS_PLAYER);
  fw_unregister_ap_msghost(APP_TS_PLAYER, ROOT_ID_PVR_PLAY_BAR);
  fw_unregister_ap_msghost(APP_TS_PLAYER, ROOT_ID_RECORD_MANAGER);
  return SUCCESS;
}

static void ts_player_callback(u32 chnid, mul_pvr_event_t event_type, s32 event_value, void *p_args)
{
  event_t evt = {0};

  evt.data1 = (u32)event_value;
  evt.data2 = 0;
  switch(event_type)
  {
    case MUL_PVR_EVENT_PLAY_EOF:
      evt.id = UI_TS_PLAYER_EVT_PLAY_EOF;
      OS_PRINTF("%s,%d,event_value=%d\n",__FUNCTION__,__LINE__,
                event_value);
      break;
    case MUL_PVR_EVENT_PLAY_SOF:
      evt.id = UI_TS_PLAYER_EVT_PLAY_SOF;
      break;
    case MUL_PVR_EVENT_PLAY_TIME_UPDATE:
      evt.id = UI_TS_PLAYER_EVT_PLAY_TIME_UPDATE;
      break;
    case MUL_PVR_EVENT_PLAY_ERROR:
      evt.id = UI_TS_PLAYER_EVT_PLAY_ERROR;
      break;
    default:
      break;
  }

  ap_frm_send_evt_to_ui(APP_TS_PLAYER, &evt);
}

RET_CODE ts_player_start(u16 *p_file_name, void *p_data)
{
  RET_CODE  ret;
  rec_info_t *p_rec_info = (rec_info_t *)p_data;
  mul_pvr_play_attr_t play_attr;
  av_set_t av_set = {0};
  ca_msg_time = 0xffffffff;

  g_is_PVR_play = TRUE;
  
  MT_ASSERT(p_data != NULL);

  memcpy(&g_current_play_rec_info, (rec_info_t *)p_data, sizeof(rec_info_t));

  if(g_play_id)
    return SUCCESS;
  is_key_error = FALSE;

  if (p_rec_info->v_pid == 0)
  {
    OS_PRINTF("\n## video pid error!\n");
    return ERR_FAILURE;
  }
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    ui_pvr_extern_open(p_file_name, UI_PVR_EXTERN_READ);
#endif
  memset(&play_attr, 0, sizeof(mul_pvr_play_attr_t));

  play_attr.stk_size = PLAY_CHAIN_TASK_STK_SIZE;

  play_attr.p_play_buffer = (u8 *)mem_mgr_require_block(BLOCK_PLAY_BUFFER, 0);
  mem_mgr_release_block(BLOCK_PLAY_BUFFER);
  play_attr.play_buffer_size = mem_mgr_get_block_size(BLOCK_PLAY_BUFFER) - TS_PLAY_BUFFER_LEN;

  play_attr.p_parse_buf = (u8 *)((u32)play_attr.p_play_buffer + play_attr.play_buffer_size);
  play_attr.parse_buf_size = TS_PLAY_BUFFER_LEN;

  sys_status_get_av_set(&av_set);

  play_attr.v_pid = p_rec_info->v_pid;
  play_attr.a_pid = p_rec_info->a_pid;
  play_attr.v_type = p_rec_info->video_fmt;
  play_attr.a_type = p_rec_info->audio_fmt;
  s_cur_audio_pid = p_rec_info->a_pid;

  //1 means bs out
  if((1 == av_set.digital_audio_output)
     && ((AUDIO_AC3_VSB == play_attr.a_type)
         ||(AUDIO_EAC3 == play_attr.a_type)))
  {
    play_attr.a_type = AUDIO_SPDIF;
  }
  play_attr.pcr_pid = p_rec_info->pcr_pid;
  play_attr.stream_type = MUL_PVR_STREAM_TYPE_TS;
  play_attr.is_clear_stream = !p_rec_info->encrypt_flag;
  uni_strncpy(play_attr.file_name, p_file_name, PVR_MAX_FILENAME_LEN - 1);
  play_attr.file_name_len = uni_strlen(play_attr.file_name);

  g_play_id = 0;
  ret = mul_pvr_play_create_chn(&g_play_id, &play_attr);
  if (ret != SUCCESS)
  {
    OS_PRINTF("Error %d: %s call mul_pvr_play_create_chn() failed!\n",
              ret, __FUNCTION__);
    return ERR_FAILURE;
  }

  ret = mul_pvr_register_event(g_play_id, ts_player_callback, NULL);
  if(ret != SUCCESS)
  {
    OS_PRINTF("Error %d: %s call mul_pvr_register_event() failed!\n",
              ret, __FUNCTION__);
    return ERR_FAILURE;
  }

  ret = mul_pvr_play_start_chn(g_play_id);
  if (ret != SUCCESS)
  {
    OS_PRINTF("Error %d: %s call mul_pvr_play_start_chn() failed!\n",
              ret, __FUNCTION__);
    //if(ret == MUL_ERR_PVR_CIPHER_KEY_ERROR)
    {
      is_key_error = TRUE;
      //if(CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
	  	//ui_comm_showdlg_open(NULL, IDS_ENCRYPETED_VIDEO, NULL, 3000);//IDS_ENCRYPETED_VIDEO
    }
    return ERR_FAILURE;
  }

  return ret;
}

RET_CODE ts_player_stop(void)
{
  RET_CODE ret = SUCCESS;

  g_is_PVR_play = FALSE;
#ifdef ENABLE_CA
  if(CAS_ID == CONFIG_CAS_ID_ABV
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    cas_manager_stop_ca_play();
#endif

  memset(&g_current_play_rec_info, 0, sizeof(rec_info_t));
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    ui_pvr_extern_close();
#endif
  if(g_play_id)
  {
    mul_av_play_stop_opt_t op = {0};

    op.enMode = MUL_AVPLAY_STOP_MODE_BLACK;
    ret = mul_pvr_play_stop_chn(g_play_id, &op);
    ret = mul_pvr_play_destroy_chn(g_play_id);
    g_play_id= 0;
  }

  return ret;
}

RET_CODE ts_player_pause(void)
{
  if(!g_play_id)
    return SUCCESS;

  return mul_pvr_play_pause_chn(g_play_id);
}

RET_CODE ts_player_resume(void)
{
  if(!g_play_id)
    return SUCCESS;
  return mul_pvr_play_resume_chn(g_play_id);
}

RET_CODE ts_player_play_mode(mul_pvr_play_speed_t speed)
{
  mul_pvr_play_mode_t trick;

  if(!g_play_id)
    return SUCCESS;
  trick.speed = speed;
  return mul_pvr_play_trick_play(g_play_id, &trick);
}

RET_CODE ts_player_seek(ts_player_seek_param_t *p_seek)
{
  mul_pvr_play_position_t seek;

  if(!g_play_id || is_key_error == TRUE)
    return SUCCESS;
  switch (p_seek->seek_type)
  {
    case TS_PLAYER_TIME_SEEK:
      seek.pos_type = MUL_PVR_PLAY_POS_TYPE_TIME;
      break;
    case TS_PLAYER_POSITION_SEEK:
      seek.pos_type = MUL_PVR_PLAY_POS_TYPE_SIZE;
      break;
    case TS_PLAYER_FRAME_SEEK:
      seek.pos_type = MUL_PVR_PLAY_POS_TYPE_FRAME;
      break;
    default:
      return ERR_FAILURE;
  }
  seek.offset = (s32)(p_seek->offset * 1000);
  switch(p_seek->seek_op)
  {
    case TS_PLAYER_SEEK_SET:
      seek.whence = MUL_PVR_SEEK_SET;
      break;
    case TS_PLAYER_SEEK_CUR:
      seek.whence = MUL_PVR_SEEK_CUR;
      break;
    case TS_PLAYER_SEEK_END:
      seek.whence = MUL_PVR_SEEK_END;
      break;
    default:
      return ERR_FAILURE;
  }

  return mul_pvr_play_seek(g_play_id, &seek);
}

BOOL ts_player_isplaying(void)
{
  if(g_play_id)
    return TRUE;

  return FALSE;
}

void ts_player_change_audio(u16 a_pid, u8 a_fmt)
{
  s_cur_audio_pid = a_pid;
  OS_PRINTF("\n##ts_player_change_audio [%d]\n", a_pid);
  mul_pvr_play_change_audio(g_play_id, a_pid, a_fmt);
}

u16 ts_player_get_cur_audio(void)
{
  return s_cur_audio_pid;
}

//lint -e438 -e550 -e830
RET_CODE ts_player_start_preview(u16 *p_file_name, u16 v_pid, u8 video_fmt)
{
  RET_CODE ret = SUCCESS;
  void *p_video_dev = NULL;
  void *p_disp_dev = NULL;
  hfile_t file = NULL;
  u32 read_size = 0;
  video_codec_type_t type = MPEG_TYPE_CODE;
  u32 file_buf_size = 0;
  u32 out_buf_size = 0;
  u8 *p_file_buffer = NULL;
  u8 *p_out_buf = NULL;
  u8 *p_first_kframe_addr = NULL;
  u8  i = 0;

  if (v_pid == 0)//fixed vpid error
  {
    OS_PRINTF("\n##preview vpid[0] error!\n");
    return ERR_FAILURE;
  }

  p_file_buffer = (u8 *)mem_mgr_require_block(BLOCK_REC_BUFFER, 0);
  p_out_buf = (u8 *)mem_mgr_require_block(BLOCK_PLAY_BUFFER, 0);

  file_buf_size = mem_mgr_get_block_size(BLOCK_REC_BUFFER);
  file_buf_size -= (file_buf_size % (188 * 512));
  out_buf_size = mem_mgr_get_block_size(BLOCK_PLAY_BUFFER);
  mem_mgr_release_block(BLOCK_REC_BUFFER);
  mem_mgr_release_block(BLOCK_PLAY_BUFFER);

  p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(p_video_dev != NULL);
  p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp_dev != NULL);

  file = vfs_open(p_file_name, VFS_READ);
  if(file == NULL)
  {
    OS_PRINTF("open file failed\n");
    return ERR_FAILURE;
  }

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);

  //memset(p_file_buffer, 0, file_buf_size);
  read_size = vfs_read(p_file_buffer, 1, file_buf_size, file);
  if (!read_size)
  {
    vfs_close(file);
    return ERR_FAILURE;
  }
  OS_PRINTF("\n##debug: ts_player_start_preview read size [%d] 111 !!\n", read_size);
  switch (video_fmt)
  {
    case VIDEO_MPEG:
      type = MPEG_TYPE_CODE;
      break;
    case VIDEO_H264:
      type = H264_TYPE_CODE;
      break;
    case VIDEO_AVS:
      type = AVS_TYPE_CODE;
      break;
    default:
      break;
  }
  memset(p_out_buf, 0, out_buf_size);
  OS_PRINTF("\n##debug: ts_player_start_preview v_pid[%d]!!\n", v_pid);
  {
    s64 seek_pos = 0;
    unsigned int tmp_size;
    for(i = 0; i< 3; i++)
    {
      tmp_size = read_size;
      if(ts_seq_get_vkey_frame((unsigned char *)p_file_buffer, (unsigned char *)p_out_buf,
                               (unsigned int *)&tmp_size, (unsigned int)out_buf_size, (int)v_pid, type , &p_first_kframe_addr))
        break;
      if(p_first_kframe_addr != NULL)
      {
        seek_pos = (s64)((s32)read_size - ((s32)p_first_kframe_addr - (s32)p_file_buffer));
        OS_PRINTF("\n##debug: preview seek pos[%lld]* 188\n", seek_pos / 188);
        vfs_seek(file, -seek_pos, VFS_SEEK_CUR);
      }
      read_size = vfs_read(p_file_buffer, 1, file_buf_size, file);
      OS_PRINTF("\n##debug: read_size[%d,%d]\n", read_size, file_buf_size);
      if (!read_size)
      {
        vfs_close(file);
        return ERR_FAILURE;
      }
    }
  }
  OS_PRINTF("\n##debug: ts_player_start_preview vframe size [%d] 222 !!\n", read_size);
  mtos_task_delay_ms(5);
  vdec_start(p_video_dev, (vdec_src_fmt_t)video_fmt, VID_UNBLANK_USER);
  mtos_task_delay_ms(5);
  ret = vdec_dec_one_frame(p_video_dev, p_out_buf, read_size);
  MT_ASSERT(SUCCESS == ret);
  {
    vdec_info_t vstate;
    do
    {
      vdec_get_info(p_video_dev,&vstate);
      mtos_task_sleep(20);
      i++;
    }
    while(vstate.decoder_frame_count < 10 && i < 20);
  }
  vdec_freeze(p_video_dev);
  mtos_task_delay_ms(5);

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
  vfs_close(file);
  preview_state = TRUE;

  return SUCCESS;
}
//lint +e438 +e550 +e830

RET_CODE ts_player_stop_preview(void)
{
  void *p_video_dev = NULL;
  void *p_disp_dev = NULL;
  void *p_dmx_dev = dev_find_identifier(NULL,
                                        DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(p_disp_dev != NULL);

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);

  if (preview_state)
  {
    preview_state = FALSE;
    p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
    MT_ASSERT(p_video_dev != NULL);
    vdec_stop(p_video_dev);
    dmx_av_reset(p_dmx_dev);
  }
  return SUCCESS;
}

BEGIN_AP_EVTMAP(ui_recorder_evtmap)
CONVERT_EVENT(UI_REC_EVT_DISK_FULL, MSG_PVR_REC_MEM_NOT_ENOUGH)
CONVERT_EVENT(UI_REC_EVT_REC_UPDATE, MSG_PVR_REC_UPDATED)
CONVERT_EVENT(UI_REC_EVT_REC_STOPPED, MSG_PVR_REC_STOPPED)
CONVERT_EVENT(UI_REC_EVT_REC_ERROR, MSG_WRITE_ERROR)
END_AP_EVTMAP(ui_recorder_evtmap)

BEGIN_AP_EVTMAP(ts_player_evtmap)
CONVERT_EVENT(UI_TS_PLAYER_EVT_PLAY_EOF, MSG_PLAY_END)
CONVERT_EVENT(UI_TS_PLAYER_EVT_PLAY_SOF, MSG_TS_PLAY_NORMAL)
CONVERT_EVENT(UI_TS_PLAYER_EVT_PLAY_TIME_UPDATE, MSG_PLAYED)
CONVERT_EVENT(UI_TS_PLAYER_EVT_PLAY_ERROR, MSG_CANNOT_PLAY)
END_AP_EVTMAP(ts_player_evtmap)

//#endif

//end of file
