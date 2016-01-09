/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "data_base16v2.h"
#include "customer_config.h"
#include "user_parse_table.h"
/*
   CA_system_id values CA system specifier

   0x0000 Reserved
   0x0001 to 0x00FF Standardized systems
   0x0100 to 0x01FF Canal Plus
   0x0200 to 0x02FF CCETT
   0x0300 to 0x03FF Deutsche Telecom
   0x0400 to 0x04FF Eurodec
   0x0500 to 0x05FF France Telecom
   0x0600 to 0x06FF Irdeto
   0x0700 to 0x07FF Jerrold/GI
   0x0800 to 0x08FF Matra Communication
   0x0900 to 0x09FF News Datacom
   0x0A00 to 0x0AFF Nokia
   0x0B00 to 0x0BFF Norwegian Telekom
   0x0C00 to 0x0CFF NTL
   0x0D00 to 0x0DFF Philips
   0x0E00 to 0x0EFF Scientific Atlanta
   0x0F00 to 0x0FFF Sony
   0x1000 to 0x10FF Tandberg Television
   0x1100 to 0x11FF Thomson
   0x1200 to 0x12FF TV/Com
   0x1300 to 0x13FF HPT - Croatian Post and Telecommunications
   0x1400 to 0x14FF HRT - Croatian Radio and Television
   0x1500 to 0x15FF IBM
   0x1600 to 0x16FF Nera
   0x1700 to 0x17FF BetaTechnik
  */

static char *g_ca_system_desc[] =
{
  "",
  "Standardized systems",
  "Canal Plus",
  "CCETT",
  "Deutsche Telecom",
  "Eurodec",
  "France Telecom",
  "Irdeto",
  "Jerrold/GI",
  "Matra Communication",
  "News Datacom",
  "Nokia",
  "Norwegian Telekom",
  "NTL",
  "Philips",
  "Scientific Atlanta",
  "Sony",
  "Tandberg Television",
  "Thomson",
  "TV/Com",
  "HPT - Croatian Post and Telecommunications",
  "HRT - Croatian Radio and Television",
  "IBM",
  "Nera",
  "BetaTechnik",
};

#define CA_SYSTIME_ID_MAX_CNT ((sizeof(g_ca_system_desc)) / (sizeof(char *)))
/*!
 * program sort struct
 */
typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  service number in stream 
  */
  u16 s_id;
}prog_sort_t;

typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  service number in stream 
  */
  u16 name[DB_DVBS_MAX_NAME_LENGTH + 1];
}prog_sort_name_t;
/*!
 * logic number sort struct
 */
typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  logic number in stream
  */
  u16 logic_num;
}prog_logic_num_sort_t;

#define DB_MAX_TP_NUM 128
typedef struct
{
  u32 freq;
  u32 sym;
  u8 nim_modulate;
  dvbs_prog_node_t pg;
}prog_node_t;

typedef struct
{
  dvbs_tp_node_t tp_list[DB_MAX_TP_NUM];
  prog_node_t pg_list[DB_DVBS_MAX_PRO];
  u16 total_tp_num;
  u16  total_pg_num;
  u16 bouquet_name[32][32];
  u16 bouquet_id[32];
  u8 categories_count;
  u32 reserved[100];
}channel_data_t;

typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  service number in stream  
  */
  u16 s_id;
 /*!
  tp freq in stream 
  */
  u32 freq;
}prog_freq_service_sort_t;

typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
  /*
  service number in stream 
  */
  u16 s_id;
 /*!
  logic ch num
  */
  u16 logic_num;
}prog_logic_num_service_sort_t;

static u8 g_pg_view_id = 0;

static u8 *p_tp_view_buf = NULL;
static u8 *p_pg_view_buf = NULL;

//TKGS_SUPPORT START
static u16 old_view_type;
static u32 old_group;
static u32 old_group_context;
static u8 old_mode;
#if 1
partition_t *p_partition = NULL;
static u16 g_partition_letter = 0;

//macro
#define IW_DATA_FOLDER_NAME "DB_DATA"
#define IW_CHANNEL_DATA_FILE_NAME "preset_channel.bin"
#define IW_STBID_FILE_NAME "stb_id.txt"
#endif

#define MAX_DB_FILE_NAME_LENGTH 128
#define STBID_TXT_SIZE 64
extern BOOL have_logic_number(void);

void ui_cache_view(void)
{ 
  sys_status_get_curn_view_info(&old_view_type, &old_group_context);
  old_group = sys_status_get_curn_group();
  old_mode = sys_status_get_curn_prog_mode();
}
extern BOOL get_pg_type(void);
void ui_restore_view(void)
{ 
  if(old_view_type<DB_DVBS_INVALID_VIEW)
  {
    if(old_view_type == DB_DVBS_FAV_TV ||old_view_type == DB_DVBS_FAV_RADIO)
    {
      ui_dbase_set_pg_view_id(ui_dbase_create_view((dvbs_view_t)old_view_type, old_group - DB_DVBS_MAX_SAT - 1, NULL));
    }
    else
    {
      ui_dbase_set_pg_view_id(ui_dbase_create_view((dvbs_view_t)old_view_type, old_group_context, NULL));
    }
   #if 0
    if (0 == db_dvbs_get_count(ui_dbase_create_view(get_pg_type()?DB_DVBS_ALL_TV:DB_DVBS_ALL_RADIO, 0, NULL)))
    {
      if (0 == db_dvbs_get_count(ui_dbase_create_view(get_pg_type()?DB_DVBS_ALL_RADIO:DB_DVBS_ALL_TV, 0, NULL)))
      {
        old_view_type = DB_DVBS_INVALID_VIEW;
        old_group = 0;
        old_mode = CURN_MODE_NONE;
      }
      else
      {
        old_view_type = DB_DVBS_ALL_RADIO;
        old_group = 0;
        old_mode = CURN_MODE_RADIO;
      }
    }
   #endif
  }
  else
  {
    ui_dbase_set_pg_view_id(ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL));
  }
  sys_status_set_curn_group((u16)old_group);
  sys_status_set_curn_prog_mode(old_mode);
  sys_status_save();
}

void ui_dbase_init(void)
{
  p_tp_view_buf = mtos_malloc(DB_DVBS_MAX_TP * sizeof(item_type_t));
  MT_ASSERT(p_tp_view_buf != NULL);
  memset(p_tp_view_buf, 0, DB_DVBS_MAX_TP * sizeof(item_type_t));

  p_pg_view_buf = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(item_type_t));
  MT_ASSERT(p_pg_view_buf != NULL);
  memset(p_pg_view_buf, 0, DB_DVBS_MAX_PRO * sizeof(item_type_t));
}


u8 ui_dbase_create_view(dvbs_view_t name, u32 context, u8 *p_ex_buf)
{
  u8 *p_view_buf = NULL;

  if(p_ex_buf != NULL)
  {
    p_view_buf = p_ex_buf;
  }
  else
  {
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
      case DB_DVBS_SAT_PG:
      case DB_DVBS_TP_PG:
      case DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG:
      case DB_DVBS_MOSAIC:	  	
        p_view_buf = p_pg_view_buf;
        break;
      case DB_DVBS_ALL_TP:
        p_view_buf = p_tp_view_buf;
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }

  return db_dvbs_create_view(name, context, p_view_buf);
}


dvbs_view_t ui_dbase_get_prog_view(dvbs_prog_node_t *p_node)
{
  return (dvbs_view_t)(p_node->video_pid ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO);
}

u16 ui_dbase_get_tp_by_pos(u8 view_id, dvbs_tp_node_t *p_node, u16 pos)
{
  u16 tp_id;

  tp_id = db_dvbs_get_id_by_view_pos(view_id, pos);
  db_dvbs_get_tp_by_id(tp_id, p_node);

  return tp_id;
}


void ui_dbase_get_full_prog_name(dvbs_prog_node_t *p_node, u16 *str, u16 max_length)
{
  u32 len;

  str_asc2uni(p_node->is_scrambled ? (u8 *)"$ " : (u8 *)"", str);
  len = uni_strlen(str);

  if(len < max_length)
  {
    uni_strncpy(str + len, p_node->name, max_length - len);
  }
}


BOOL ui_dbase_pg_is_scambled(u16 rid)
{
  dvbs_prog_node_t pg;

  if(rid == INVALIDID)
  {
    return FALSE;
  }

  if(db_dvbs_get_pg_by_id(rid, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }

  return (BOOL)(pg.is_scrambled);
}


BOOL ui_dbase_pg_is_full(void)
{
  //u8 view = (u8)ui_sys_get_data(SS_CUR_VIEW);
  BOOL is_full = is_pg_full();

  ui_dbase_reset_pg_view();
  return is_full;
}

//lint -e732
u32 ui_dbase_get_pg_curn_audio_pid(dvbs_prog_node_t *p_node)
{
  return p_node->audio[p_node->audio_channel].p_id;
}
//lint +e732


void ui_dbase_set_pg_curn_audio_pid(dvbs_prog_node_t *p_node, u32 pid)
{
  p_node->audio[p_node->audio_channel].p_id = (u16)pid;
}


void ui_dbase_set_pg_view_id(u8 view_id)
{
  g_pg_view_id = view_id;
}


u8 ui_dbase_get_pg_view_id(void)
{
  return g_pg_view_id;
}


void ui_dbase_reset_pg_view(void)
{
  //u16 view = ui_sys_get_data(SS_CUR_VIEW);
  u16 view;
  u32 context;

  sys_status_get_curn_view_info(&view, &context);

  if(view != DB_DVBS_INVALID_VIEW)
  {
    /* create view for new view */
    ui_dbase_set_pg_view_id(ui_dbase_create_view((dvbs_view_t)view, context, NULL));
  }
}


void ui_dbase_delete_all_pg(void)
{
  u8 view_id;
  u16 i, cnt;
  u16 param = DB_DVBS_PARAM_ACTIVE_FLAG;
  BOOL is_db_full = FALSE;

  view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  cnt = db_dvbs_get_count(view_id);

  for(i = 0; i < cnt; i++)
  {
    db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, param);
  }

  sys_status_get_status(BS_DB_FULL, &is_db_full);
  if(is_db_full == TRUE)
  {
    sys_status_set_status(BS_DB_FULL, FALSE);
  }
  db_dvbs_save(view_id);
}


BOOL ui_dbase_pg_is_fav(u8 view_id, u16 pos)
{
  u8 dig;

  for (dig = (u8)sys_status_get_fav1_index(); dig < MAX_FAV_GRP; dig++)
  {
    if(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, dig) == TRUE)
    {
      //if(db_dvbs_get_mark_value(view_id, pos, dig))
      return TRUE;
    }
  }

  return FALSE;
}


BOOL ui_dbase_check_tvradio(BOOL is_tv)
{
  u8 mode;
  u16 group;
  u16 view_type;
  u32 group_context;
  BOOL ret = FALSE;

  mode = sys_status_get_curn_prog_mode();
  group = sys_status_get_curn_group();

  sys_status_get_view_info(group, mode, &view_type, &group_context);

  if(db_dvbs_get_count(ui_dbase_create_view(
                         is_tv ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, 0, NULL)))
  {
    ret = TRUE;
  }

  //reset current view
  ui_dbase_set_pg_view_id(ui_dbase_create_view((dvbs_view_t)view_type, group_context, NULL));

  return ret;
}


static u32 freq_val[] =
{
  5150, 5750, 5950, 9750, 10000, 10050,
  10450, 10600, 10700, 10750, 11250, 11300
};

#define FREQ_VAL_CNT    (sizeof(freq_val) / sizeof(u32))

void ui_dbase_pos2freq(u16 pos, u32 *p_lnbtype, u32 *p_freq1, u32 *p_freq2,
                       u8 *p_is_unicable, u8 *p_unicable_type)
{

  {
    if(pos < LNB_FREQ_CNT)
    {
      if(pos < FREQ_VAL_CNT)
      {
        *p_lnbtype = 0; // standard
        *p_freq1 = *p_freq2 = freq_val[pos];
      }
      else if(pos < FREQ_VAL_CNT + 2)
      {
        *p_lnbtype = 1; // user
        if(pos == FREQ_VAL_CNT)
        {
          *p_freq1 = 5150;
          *p_freq2 = 5750;
        }
        else
        {
          *p_freq1 = 5750;
          *p_freq2 = 5150;
        }
      }
      else
      {
        *p_lnbtype = 2; // universal
        *p_freq1 = 9750;
        *p_freq2 = 10600;
      }
    }
    else
    {
      UI_PRINTF("pos2feq: ERROR, invaild pos (%d)\n", pos);
      MT_ASSERT(0);
      *p_lnbtype = *p_freq1 = *p_freq2 = 0;
    }
  }
}


static u16 get_freq_index(u32 freq)
{
  u16 i;

  for(i = 0; i < FREQ_VAL_CNT; i++)
  {
    if(freq_val[i] == freq)
    {
      return i;
    }
  }

  UI_PRINTF("get_freq_index: ERROR, freq(%d) is NOT matched\n", freq);
  return 0;
}


u16 ui_dbase_freq2pos(u32 lnbtype, u32 freq1, u32 freq2, u8 p_is_unicable, u8 p_unicable_type)
{
  u16 pos;


  {
    switch(lnbtype)
    {
      case 0: // standard
//      MT_ASSERT(freq1 == freq2);
        pos = get_freq_index(freq1);
        break;
      case 1: // user
//      MT_ASSERT(freq1 != freq2);
        pos = freq1 < freq2 ? FREQ_VAL_CNT : FREQ_VAL_CNT + 1;
        break;
      case 2: // universal
        pos = FREQ_VAL_CNT + 2;
        break;
      default:
        MT_ASSERT(0);
        pos = 0;
    }
  }
  return pos;
}


BOOL ui_dbase_reset_last_prog(BOOL focus_first)
{
  u8 mode, view_id;
  BOOL ret = FALSE;
  u16 view_type = 0;
  u32 context = 0;

  sys_status_check_group();

  mode = sys_status_get_curn_prog_mode();

  if(mode != CURN_MODE_NONE)
  {
    // set curn group to group_all
    //sys_status_set_curn_group(0);
    //view_id = ui_dbase_create_view((mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, 0, NULL);
    sys_status_get_view_info(sys_status_get_curn_group(), mode, &view_type, &context);
    view_id = ui_dbase_create_view((dvbs_view_t)view_type, context, NULL);
    ui_dbase_set_pg_view_id(view_id);

    //if it's first scan, focus on first pg.
    if(focus_first)
    {
      sys_status_set_curn_group_info(db_dvbs_get_id_by_view_pos(view_id, 0), 0);
    }

    ret = TRUE;
  }
  // save
  sys_status_save();

  return ret;
}

//lint -e702
char *ui_dbase_get_ca_system_desc(dvbs_prog_node_t *p_pg)
{
  u32 index = 0;

  MT_ASSERT(p_pg != NULL);

  if(p_pg->ca_system_id == 0
    || !p_pg->is_scrambled)
  {
    index = 0; // reserved
  }
  else
  {
    index = SYS_GET_BYTE1(p_pg->ca_system_id) + 1 /* skip reserved */;
    if(index > CA_SYSTIME_ID_MAX_CNT)
    {
      OS_PRINTF("ERROR! ca_system_id = 0x%x, force change to 0\n", p_pg->ca_system_id);
      index = 0;
    }
  }

  if(index >= sizeof(g_ca_system_desc) / sizeof(char *))
  {
    index = 0; // reserved
  }
  return g_ca_system_desc[index];
}
//lint +e702
int pg_cmp_name_a_z(const void *node1, const void *node2)
{
  prog_sort_name_t *pre_node = (prog_sort_name_t *) node1;
  prog_sort_name_t *cur_node = (prog_sort_name_t *) node2;
  
  return (pre_node->name[0] - cur_node->name[0]);
}

int pg_cmp_name_z_a(const void *node1, const void *node2)
{
  prog_sort_name_t *pre_node = (prog_sort_name_t *) node1;
  prog_sort_name_t *cur_node = (prog_sort_name_t *) node2;
  
  return (cur_node->name[0] - pre_node->name[0]);
}

int pg_cmp_logic_num(const void *node1, const void *node2)
{
  prog_logic_num_sort_t *pre_node = (prog_logic_num_sort_t *) node1;
  prog_logic_num_sort_t *cur_node = (prog_logic_num_sort_t *) node2;
  
  return (pre_node->logic_num- cur_node->logic_num);
}

int pg_cmp_sev_id(const void *node1, const void *node2)
{
  prog_sort_t *pre_node = (prog_sort_t *) node1;
  prog_sort_t *cur_node = (prog_sort_t *) node2;
  
  return (pre_node->s_id - cur_node->s_id);
}
int pg_cmp_freq(const void *node1, const void *node2)
{
  prog_freq_service_sort_t *pre_node = (prog_freq_service_sort_t *) node1;
  prog_freq_service_sort_t *cur_node = (prog_freq_service_sort_t *) node2;
  
  return (pre_node->freq - cur_node->freq);
}
int pg_cmp_sid(const void *node1, const void *node2)
{
  prog_freq_service_sort_t *pre_node = (prog_freq_service_sort_t *) node1;
  prog_freq_service_sort_t *cur_node = (prog_freq_service_sort_t *) node2;
  
  return (pre_node->s_id - cur_node->s_id);
}

//lint -e438 -e550
void ui_dbase_pg_sort_by_sid(u8 view_id)
{
  u16 view_cnt  = 0;
  u16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_sort_t *p_sort_buf = NULL;
  prog_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, (u16)view_cnt * sizeof(u16));
  
  p_pro_item = (prog_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    p_pro_item->s_id = (u16)prog_node.s_id;
    p_pro_item++;
  }

//  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_sort_t), pg_cmp_sev_id);
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}

//lint -e732
void ui_dbase_pg_sort_by_logic_num(u8 view_id)
{
  u16 view_cnt  = 0;
  u16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_logic_num_sort_t *p_sort_buf = NULL;
  prog_logic_num_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(view_cnt * sizeof(prog_logic_num_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, view_cnt * sizeof(prog_logic_num_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_logic_num_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    p_pro_item->logic_num= prog_node.logical_num;
    p_pro_item++;
  }

  qsort(p_sort_buf, view_cnt, sizeof(prog_logic_num_sort_t), pg_cmp_logic_num);
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);
  
  db_dvbs_save(view_id);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}
//lint +e732

void ui_dbase_pg_sort_by_mode(dvbs_view_t view_id, u8 mode)
{
  u16 view_cnt  = 0;
  u16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_sort_name_t *p_sort_buf = NULL;
  prog_sort_name_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  u16 cnt = 0;;

  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_name_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_name_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_sort_name_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    memcpy(p_pro_item->name, prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
    p_pro_item++;
  }

//  shellsort(p_sort_buf, view_cnt);
  switch(mode)
  {
    case DB_DVBS_A_Z_MODE:
      qsort(p_sort_buf, view_cnt, sizeof(prog_sort_name_t), pg_cmp_name_z_a);
      for(i = 0; i < view_cnt; i++)
      {
        if((p_sort_buf[i].name[0] >= 0x41 && p_sort_buf[i].name[0] <= 0x5a)
          || (p_sort_buf[i].name[0] >= 0x61 && p_sort_buf[i].name[0] <= 0x7a))
        {
          cnt++;
        }
      }
      qsort(p_sort_buf, cnt, sizeof(prog_sort_name_t), pg_cmp_name_a_z);
      cnt = 0;
      break;
    case DB_DVBS_Z_A_MODE:
      qsort(p_sort_buf, view_cnt, sizeof(prog_sort_name_t), pg_cmp_name_z_a);
      break;    
    case DB_DVBS_0_9_MODE:
      qsort(p_sort_buf, view_cnt, sizeof(prog_sort_name_t), pg_cmp_name_a_z);
      break;
    case DB_DVBS_9_0_MODE:
      qsort(p_sort_buf, view_cnt, sizeof(prog_sort_name_t), pg_cmp_name_a_z);
      for(i = 0; i < view_cnt; i++)
      {
        if(p_sort_buf[i].name[0] >= 0x30 && p_sort_buf[i].name[0] <= 0x39)
        {
          cnt++;
        }
      }
      qsort(p_sort_buf, cnt, sizeof(prog_sort_name_t), pg_cmp_name_z_a);
      cnt = 0;
      break;
    case DB_DVBS_LOCAL_NUM:
    default:
      break;
  }
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}
#if 1
static BOOL ui_dbase_mk_db_dir(void)
{
  u16 uni_tmp[16] = {0};

  if(file_list_get_partition(&p_partition) > 0)
  {
    g_partition_letter = p_partition[sys_status_get_usb_work_partition()].letter[0];
    uni_tmp[0] = g_partition_letter;
    str_asc2uni((u8 *)":\\", uni_tmp + 1);
    
    str_asc2uni((u8 *)IW_DATA_FOLDER_NAME, uni_tmp + 3);
    OS_PRINTF("sys_status_get_usb_work_partition(%d)\n",sys_status_get_usb_work_partition());
    if(vfs_mkdir(uni_tmp) != SUCCESS)
    {
      OS_PRINTF("%s, %s dir can't create or exist!\n", __FUNCTION__, "DB_DATA");
    }
  }
  return 0;
}

static void ui_dbase_mk_db_filename(u16* p_unistr, u8* p_ascstr)
{
  u16 uni_tmp[MAX_DB_FILE_NAME_LENGTH] = {0};

  
  if(file_list_get_partition(&p_partition) > 0)
  {
    uni_strcat(p_unistr, p_partition[sys_status_get_usb_work_partition()].letter, MAX_DB_FILE_NAME_LENGTH);

    str_asc2uni((u8 *)"\\", uni_tmp);
    uni_strcat(p_unistr, uni_tmp, MAX_DB_FILE_NAME_LENGTH);

    str_asc2uni((u8 *)IW_DATA_FOLDER_NAME, uni_tmp);
    uni_strcat(p_unistr, uni_tmp, MAX_DB_FILE_NAME_LENGTH);

    str_asc2uni((u8 *)"\\", uni_tmp);
    uni_strcat(p_unistr, uni_tmp, MAX_DB_FILE_NAME_LENGTH);    

    str_asc2uni(p_ascstr, uni_tmp);
    uni_strcat(p_unistr, uni_tmp, MAX_DB_FILE_NAME_LENGTH);  
  }
}

static void ui_dbase_play_first_pg(u8 view_id)
{
  u16 pg_id = 0xFFFF, pg_pos = 0;
  dvbs_prog_node_t prog = {0};
  u8 curn_mode = CURN_MODE_NONE;
  
  ui_dbase_set_pg_view_id(view_id);
  
  if(db_dvbs_get_count(view_id)>0)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
    pg_pos = 0;
    
    db_dvbs_get_pg_by_id(pg_id, &prog);
    
    if((BOOL)prog.tv_flag)
    {
      curn_mode = CURN_MODE_TV;
    }
    else
    {
      curn_mode = CURN_MODE_RADIO;
    }

    sys_status_set_curn_prog_mode(curn_mode);
    sys_status_set_curn_group_info(pg_id, pg_pos);
  }
  //play current pg
  manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
}

#endif

BOOL ui_find_usb_pgdata_file(void)
{
  hfile_t file = NULL;
  channel_data_t * p_channel_data = NULL;
  u32 channel_data_size = 0;
  u16 filename[MAX_DB_FILE_NAME_LENGTH] = {0};
  u32 len = 0;
  
  ui_dbase_mk_db_filename(filename, IW_CHANNEL_DATA_FILE_NAME);
  
  channel_data_size = sizeof(channel_data_t);
  p_channel_data = mtos_malloc(channel_data_size);
  if(NULL == p_channel_data)
  {
    OS_PRINTF("@@ %s, p_channel_data malloc failed, size = %d \n", __FUNCTION__, channel_data_size);
    return FALSE;
  }
  memset(p_channel_data, 0, channel_data_size);
  
  //open file --
  file = vfs_open(filename, VFS_READ);
  if(file != NULL)
  {
    len = vfs_read(p_channel_data, channel_data_size,  1, file);
    if(len != channel_data_size)
    {
      OS_PRINTF("@@ %s :%d \n", __FUNCTION__, __LINE__);  
      vfs_close(file);
      mtos_free(p_channel_data);
      return FALSE;
    }
  }
  else
  {
    OS_PRINTF("@@ %s :%d \n", __FUNCTION__, __LINE__);  
    vfs_close(file);
    mtos_free(p_channel_data);
    return FALSE;
  }
  
  if(p_channel_data->total_pg_num == 0 || p_channel_data->total_tp_num == 0)
  {
    mtos_free(p_channel_data);
    return FALSE;
  }
  mtos_free(p_channel_data);
  return TRUE;
}
void ui_dbase_pgdata_db_to_usb(void)
{
  hfile_t file = NULL;
  dvbs_tp_node_t tp;
  dvbs_prog_node_t  pg;
  channel_data_t * p_channel_data = NULL;
  u32 channel_data_size = 0;
  u8 pg_view_id = 0xFF, tp_view_id = 0xFF;
  u16 pg_total = 0, tp_total = 0, i = 0;
  u16 pg_id = 0xFFFF, tp_id = 0xFFFF; 
  u32 len = 0;
  u16 filename[MAX_DB_FILE_NAME_LENGTH] = {0};
  u16 bouquet_name_unistr[64] = {0};
  bouquet_group_t *g_bouquet_grp = NULL;
  u8 bouquet_name[64] = {0};
  //u16 bouquet_id[32];
  
  channel_data_size = sizeof(channel_data_t);
  p_channel_data = mtos_malloc(channel_data_size);
  if(NULL == p_channel_data)
  {
    OS_PRINTF("@@ %s, p_channel_data malloc failed, size = %d \n", __FUNCTION__, channel_data_size);
    return;
  }
  memset(p_channel_data, 0, channel_data_size);
  tp_view_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  tp_total = db_dvbs_get_count(tp_view_id);
  p_channel_data->total_tp_num = tp_total;
    
  for(i=0; i<tp_total; i++)
  {
    tp_id = db_dvbs_get_id_by_view_pos(tp_view_id, i);
    if (db_dvbs_get_tp_by_id(tp_id, &tp) == DB_DVBS_OK)
    {
      memcpy(&p_channel_data->tp_list[i], &tp, sizeof(dvbs_tp_node_t));
    }
  }
  
  pg_view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  pg_total = db_dvbs_get_count(pg_view_id); 
  p_channel_data->total_pg_num = pg_total;
  
  for(i=0; i<pg_total; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(pg_view_id, i);
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      memcpy(&(p_channel_data->pg_list[i].pg), &pg, sizeof(dvbs_prog_node_t));
      if (db_dvbs_get_tp_by_id(pg.tp_id, &tp) == DB_DVBS_OK)
      {
        p_channel_data->pg_list[i].freq = tp.freq;
        p_channel_data->pg_list[i].sym = tp.sym;
        p_channel_data->pg_list[i].nim_modulate= (u8)tp.nim_modulate;
      }
    }
  }
  g_bouquet_grp = get_bouquet_group();
  p_channel_data->categories_count = g_bouquet_grp->bouquet_count;
  for(i=0; i<g_bouquet_grp->bouquet_count; i++)
  {
    memset(bouquet_name, 0 ,sizeof(bouquet_name)/sizeof(u8));
    memset(bouquet_name_unistr, 0 ,sizeof(bouquet_name_unistr)/sizeof(u16));
    str_uni2asc(bouquet_name, g_bouquet_grp->bouquet_name[i]);
    gb2312_to_unicode((u8*)bouquet_name, MAX_BOUQUET_NAMELEN, bouquet_name_unistr, MAX_BOUQUET_NAMELEN+1);
    memcpy(p_channel_data->bouquet_name[i],bouquet_name_unistr,32);
    p_channel_data->bouquet_id[i] = g_bouquet_grp->bouquet_id[i];	
  }
  
  //creat file to save  channel_data
  ui_dbase_mk_db_dir();

  ui_dbase_mk_db_filename(filename, IW_CHANNEL_DATA_FILE_NAME);

    //creat file --
  file=vfs_open(filename, VFS_NEW);
  if(file == NULL)
  {
    OS_PRINTF("@@ %s file has been exist \n",filename);
    file = vfs_open(filename, VFS_WRITE);
  }
  if(file != NULL)
  {
    len = vfs_write(p_channel_data, sizeof(channel_data_t), 1, file);
    if(len == 0)
    {
      vfs_close(file);
      mtos_free(p_channel_data);
      return;
    }

    vfs_close(file);
  }
  else
  {
    OS_PRINTF("@@@ open file : %s failed \n",filename);
  }
  mtos_free(p_channel_data);

}

extern void categories_mem_reset();;
void db_reset_prog_before_copy(void)
{
   /**clear db date***/
  ui_dbase_delete_all_pg();
  //set default
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID, 0);
  //sys_status_load();
  sys_group_reset();
  categories_mem_reset();
  sys_status_check_group();
  sys_status_save();
}
extern void create_categories(void);
void ui_dbase_pgdata_usb_to_db(void)
{
  comm_dlg_data_t dlg_data_copy =
  {
      ROOT_ID_BACKGROUND,
      DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
      COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
      IDS_IMPORTING_PRESET_CHANNEL,
      0,
  };
  hfile_t file = NULL;
  channel_data_t * p_channel_data = NULL;
  u32 channel_data_size = 0;
  u8 pg_view_id = 0xFF, tp_view_id = 0xFF;
  u16 pg_total = 0, tp_total = 0, i = 0, j = 0;
  u32 len = 0;
  dvbs_tp_node_t* p_tp;
  prog_node_t * p_pg_node;
  u16 filename[MAX_DB_FILE_NAME_LENGTH] = {0};
  //u16 bouquet_name_unistr[64] = {0};
  bouquet_group_t *g_bouquet_grp = NULL;
  //u8 bouquet_name[64] = {0};
  //u16 bouquet_id[32];
  ui_close_all_mennus();
  
  ui_comm_dlg_open(&dlg_data_copy);
  
  ui_dbase_mk_db_filename(filename, IW_CHANNEL_DATA_FILE_NAME);
  
  channel_data_size = sizeof(channel_data_t);
  p_channel_data = mtos_malloc(channel_data_size);
  if(NULL == p_channel_data)
  {
    OS_PRINTF("@@ %s, p_channel_data malloc failed, size = %d \n", __FUNCTION__, channel_data_size);
    return;
  }
  memset(p_channel_data, 0, channel_data_size);
  
  //open file --
  file = vfs_open(filename, VFS_READ);
  if(file != NULL)
  {
    len = vfs_read(p_channel_data, channel_data_size,  1, file);
    if(len != channel_data_size)
    {
      OS_PRINTF("@@ %s :%d \n", __FUNCTION__, __LINE__);  
      vfs_close(file);
      mtos_free(p_channel_data);
      return;
    }
  }
  else
  {
    OS_PRINTF("@@ %s :%d \n", __FUNCTION__, __LINE__);  
    vfs_close(file);
    mtos_free(p_channel_data);
    return;
  }
  db_reset_prog_before_copy();
  
  tp_view_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  tp_total = p_channel_data->total_tp_num;

  pg_view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  pg_total = p_channel_data->total_pg_num;

  if(tp_total == 0 || pg_total == 0)
  {
    mtos_free(p_channel_data);
    return;
  }
  //add tp
  for(i = 0; i < tp_total; i++)
  {
    db_dvbs_add_tp(tp_view_id, &(p_channel_data->tp_list[i]));
  }
  
  //add tp
  for(i = 0; i < pg_total; i++)
  {
    p_pg_node = &(p_channel_data->pg_list[i]);
    
    for(j = 0; j < tp_total; j++)
    {
      p_tp = &(p_channel_data->tp_list[j]);
      
      if(p_pg_node->freq == p_tp->freq 
        && p_pg_node->sym == p_tp->sym
        && p_pg_node->nim_modulate== p_tp->nim_modulate)
      {
        p_pg_node->pg.tp_id = p_tp->id;
        break;
      }
    }
    db_dvbs_add_program(pg_view_id, &(p_pg_node->pg));
  }
  db_dvbs_save(tp_view_id);
  db_dvbs_save(pg_view_id);
  g_bouquet_grp = get_bouquet_group();
  g_bouquet_grp->bouquet_count = p_channel_data->categories_count;
  for(i=0; i<g_bouquet_grp->bouquet_count; i++)
  {
     memcpy(g_bouquet_grp->bouquet_name[i],p_channel_data->bouquet_name[i],32);
     g_bouquet_grp->bouquet_id[i] = p_channel_data->bouquet_id[i];	
  }
  categories_set_loaded_flag(TRUE);
  ui_cache_view();
  create_categories();
  ui_restore_view();
  
  //ui_dbase_pg_sort(DB_DVBS_ALL_PG);
  
  sys_status_set_curn_group(GROUP_T_ALL);

  ui_dbase_play_first_pg(pg_view_id);
 
  mtos_free(p_channel_data);
  ui_comm_dlg_close();
  return;
}
void ui_dbase_pg_sort_by_fre_service_pid(view_id)
{
  s16 view_cnt  = 0;
  s16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_freq_service_sort_t *p_sort_buf = NULL;
  prog_freq_service_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  dvbs_tp_node_t tp;
  s16 same_tp_num = 1, cnt = 0;;
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_freq_service_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_freq_service_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_freq_service_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->s_id = prog_node.s_id;

    ret = db_dvbs_get_tp_by_id((u16)prog_node.tp_id, &tp);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    p_pro_item->freq= tp.freq;
    p_pro_item++;
  }

//  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_freq_service_sort_t), pg_cmp_freq);
  for(i = 0; i < view_cnt - 1; i++)
  {
    if(p_sort_buf[i].freq == p_sort_buf[i + 1].freq)
    {
      same_tp_num ++;
    }
    else
    {
      qsort(&p_sort_buf[cnt], same_tp_num, sizeof(prog_freq_service_sort_t), pg_cmp_sid);
      cnt = cnt + same_tp_num;
      same_tp_num = 1;
    }
    if(i == view_cnt - 2)
    {
      qsort(&p_sort_buf[cnt], same_tp_num, sizeof(prog_freq_service_sort_t), pg_cmp_sid);
      cnt = cnt + same_tp_num;
      same_tp_num = 1;
    }
  }
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}

/***************************************************************************
函数名：	pg_cmp_logic_id
函数描述：比较逻辑号
入口参数：node1,node2 

返回值：
******************************************************************************/
static int pg_cmp_logic_id(const void *node1, const void *node2)
{
  prog_logic_num_service_sort_t *pre_node = (prog_logic_num_service_sort_t *) node1;
  prog_logic_num_service_sort_t *cur_node = (prog_logic_num_service_sort_t *) node2;
  
  return (pre_node->logic_num - cur_node->logic_num);
}

int pg_cmp_logic_service_id(const void *node1, const void *node2)
{
  prog_logic_num_service_sort_t *pre_node = (prog_logic_num_service_sort_t *) node1;
  prog_logic_num_service_sort_t *cur_node = (prog_logic_num_service_sort_t *) node2;
  
  return (pre_node->s_id - cur_node->s_id);
}

void ui_dbase_pg_sort_by_logic_num_service_pid(view_id)
{
  s16 view_cnt  = 0;
  s16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_logic_num_service_sort_t *p_sort_buf = NULL;
  prog_logic_num_service_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  s16 same_logic_num = 1, cnt = 0;;
  view_cnt = db_dvbs_get_count(view_id);
  
  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_logic_num_service_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_logic_num_service_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_logic_num_service_sort_t *)p_sort_buf;

  OS_PRINTF("\n**********************************\n");
  OS_PRINTF("enter logic num and service id sequence\n");
  OS_PRINTF("\n**********************************\n");

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->s_id = prog_node.s_id;
    p_pro_item->node_id = element_id;
    p_pro_item->logic_num= prog_node.logical_num;
    OS_PRINTF("p_pro_item s_id %d,node_id %d,logic_num 0x%x\n",p_pro_item->s_id,p_pro_item->node_id,p_pro_item->logic_num);
    p_pro_item++;
  }

  //  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_logic_num_service_sort_t), pg_cmp_logic_id);
  for(i = 0; i < view_cnt - 1; i++)
  {
    if(p_sort_buf[i].logic_num== p_sort_buf[i + 1].logic_num)
    {
      same_logic_num ++;
    }
    else
    {
      qsort(&p_sort_buf[cnt], same_logic_num, sizeof(prog_logic_num_service_sort_t), pg_cmp_logic_service_id);
      cnt = cnt + same_logic_num;
      same_logic_num = 1;
    }
    if(i == view_cnt - 2)
    {
      qsort(&p_sort_buf[cnt], same_logic_num, sizeof(prog_logic_num_service_sort_t), pg_cmp_logic_service_id);
      cnt = cnt + same_logic_num;
      same_logic_num = 1;
    }
  }

  OS_PRINTF("view_cnt = %d\n",view_cnt);
  for(i = 0; i < view_cnt;i++)
  {
    OS_PRINTF("p_sort_buf[%d].node_id = %d\n",i,p_sort_buf[i].node_id);
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);

}

//lint +e438 +e550
void ui_dbase_pg_sort(dvbs_view_t name)
{
  u8 view_id;
  
  MT_ASSERT(name<DB_DVBS_INVALID_VIEW);
  
  view_id = db_dvbs_create_view(name, 0, NULL);
  if(db_dvbs_get_count(view_id) > 0)
  {
    if(g_customer.customer_id != CUSTOMER_HUANGSHI)
    {
      #ifdef LCN_SWITCH_DS_JHC
      OS_PRINTF("LCN && SERVICE_ID SORT!\n");
      ui_dbase_pg_sort_by_logic_num_service_pid(view_id);
      #else
      if(have_logic_number())
        ui_dbase_pg_sort_by_logic_num(view_id);
      else
        #ifdef FRE_SVC_SORT
        ui_dbase_pg_sort_by_fre_service_pid(view_id);
        #else
        ui_dbase_pg_sort_by_sid(view_id);
        #endif
      #endif
    }
    else
    {
      ui_dbase_pg_sort_by_sid(view_id);
    }
    db_dvbs_save(view_id);
  }
}

RET_CODE check_is_msg_serial_num(u8 *p_uni_str)
{
 charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);

  //dm_v2_init_para_t dm_para = { 0 };
  u32 indentity_block_addr;
  u32 rw_addr;
  u8 *p_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
  charsto_prot_status_t st_set = {0};
  charsto_prot_status_t st_old = {0};

  memset(p_buffer, 0, CHARSTO_SECTOR_SIZE);
  //unprotect
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  //if(charsto_unprotect_all(p_charsto_dev) == SUCCESS)
  {
    indentity_block_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), IDENTITY_BLOCK_ID) - get_flash_addr();
    rw_addr = indentity_block_addr / CHARSTO_SECTOR_SIZE * CHARSTO_SECTOR_SIZE;
    if(charsto_read(p_charsto_dev, rw_addr, p_buffer, CHARSTO_SECTOR_SIZE) == SUCCESS)
    {
      memcpy(p_buffer+ (indentity_block_addr - rw_addr), p_uni_str, 64);
      if(charsto_erase(p_charsto_dev, rw_addr, 1) == SUCCESS)
      {
        charsto_writeonly(p_charsto_dev, rw_addr, p_buffer, CHARSTO_SECTOR_SIZE);
      }
      else
      {
	  OS_PRINTF("charsto erase fail\n");
	  return ERR_FAILURE;
	}
    }
    else
    {
	OS_PRINTF("charsto_read fail\n");
	return ERR_FAILURE;
    }
  }
  //restore
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

  mtos_free(p_buffer);
  return SUCCESS;

}
static u64 stringtou32(u8 *buf,u16 size)
{
    u32 i = 0;
    u64 result = 0;	
    for(;i < size;i++ )
    {
	result = result * 10 + (buf[i] -'0');
    }
    return result;	
}

static void u32tostring(u8 *buf,u64 number)
{
    u32 i = 0;
    u64 result = number;
    while(result)
    {
	result /= 10;
	i++;
    }
    buf[i] = '\0';
	
    while(number)
    {
	buf[--i] = number % 10 + '0';
	number /= 10;
    }
}
static u64 get_string_from_buf(u8 *buf,u8 *destbuf,u16 *localtion)
{
   u16 j = 0,len = 0;
   u16 i = *localtion;
   OS_PRINTF("location1 is %d\n",*localtion);
   for(;i < STBID_TXT_SIZE;i++)
   { 
	if(buf[i] == '#')
	{
		destbuf[j] = '\0';
		i++;
		break;
	}
	destbuf[j] = buf[i];
	j++;
   }
   *localtion = i;
   len = strlen(destbuf);
   OS_PRINTF("destbuf is %s\n",destbuf);
   OS_PRINTF("param's len is %d,location2 is %d\n",len,*localtion);
   return stringtou32(destbuf,len);

}
static void connect_buf(u8 *buf,u8 *souce)
{
    u16 size = 0, i =0;
    size = strlen(souce);
    for(;i < size;i++)
    {
	buf[i] = souce[i];
    }
    buf[i] = '#';
}
static void connect_buf_for_stbid(u8 *buf,u8 *start,u8 *end,u8 *step,u8 *times,u8 *total_stbid)
{
    u16 len_start = 0,len_end = 0,len_step = 0,len_times = 0,len_stbid =0;
    len_start = strlen(start);
    len_end = strlen(end);
    len_step = strlen(step);
    len_times = strlen(times);
    len_stbid = strlen(total_stbid);	
    connect_buf(buf,start);
    connect_buf(buf+len_start+1,end);
    connect_buf(buf+len_start+len_end+2,step);
    connect_buf(buf+len_start+len_end+len_step+3,times);
    connect_buf(buf+len_start+len_end+len_step+len_times+4,total_stbid);	    	
}
static RET_CODE parse_stbid_txt_file(u8 *buf)
{
    u8 stbid_start[5] = {0};
    u8 stbid_end[5] = {0};
    u8 increase_step[5] = {0};
    u8 burn_times[10] = {0};	
    u8 total_stbid[20] = {0};
    u8 stbid[20] = {0};	
    u16 len = 0;
    u32 start = 0,end = 0,step = 0,times = 0;
    u64 result = 0;	
    u32 i = 0,j =0;
    RET_CODE ret = SUCCESS;
    start = get_string_from_buf(buf,stbid_start,&len);
    end = get_string_from_buf(buf,stbid_end,&len);
    step = get_string_from_buf(buf,increase_step,&len);
    times = get_string_from_buf(buf,burn_times,&len);
    result = get_string_from_buf(buf,total_stbid,&len);	
    OS_PRINTF("start %d,end %d,step %d,times %d,result is %lld\n",start,end,step,times,result);
    OS_PRINTF("len is %d\n",len);	
    if(start < 0 || end > 19)
    {
	OS_PRINTF("param error\n");
	return ERR_PARAM;
    }

    for(i = start;i<=end;i++)
    {
	stbid[j] = total_stbid[i];
	j++;
    }
    stbid[j] = '\0';
    OS_PRINTF("wait to burn stbid %s\n ",stbid);	
    len = strlen(stbid);
    result = stringtou32(stbid,len);	
    result += step * times;
    u32tostring(stbid,result);
    OS_PRINTF("stbid is %s\n",stbid);	
    ret = check_is_msg_serial_num(stbid);
    if(ret == SUCCESS)
    {
	times++;
	u32tostring(burn_times,(u64)times);
	connect_buf_for_stbid(buf,stbid_start,stbid_end,increase_step,burn_times,total_stbid);
    }
    return ret;
}
RET_CODE ui_dbase_burn_stbid_from_usb(void)
{
  comm_dlg_data_t dlg_data_copy =
  {
      ROOT_ID_BACKGROUND,
      DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
      COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
      IDS_BURN_FLASH,
      0,
  };
  hfile_t file = NULL;
  u8 p_channel_data[STBID_TXT_SIZE] = {0};
  RET_CODE ret = SUCCESS;
  u32 channel_data_size = 0;
  u16 filename[MAX_DB_FILE_NAME_LENGTH] = {0};
  u32 len = 0;

  //ui_close_all_mennus();
  
  ui_comm_dlg_open(&dlg_data_copy);
  
  ui_dbase_mk_db_filename(filename, IW_STBID_FILE_NAME);
  channel_data_size = sizeof(p_channel_data);
  //open file --
  file = vfs_open(filename, VFS_READ);
  if(file != NULL)
  {
    len = vfs_read(p_channel_data, channel_data_size,  1, file);
    if(len != channel_data_size)
    {
      OS_PRINTF("@@ %s :%d \n", __FUNCTION__, __LINE__);  
      vfs_close(file);
      return ERR_FAILURE;
    }
  }
  else
  {
    OS_PRINTF("@@ %s :%d \n", __FUNCTION__, __LINE__);  
    vfs_close(file);
    return ERR_FAILURE;
  }
  ret = parse_stbid_txt_file(p_channel_data);
  if(ret != SUCCESS)
  	return ret;
  ui_dbase_mk_db_dir();
  channel_data_size = strlen(p_channel_data);
  if(channel_data_size > STBID_TXT_SIZE)
  	channel_data_size = STBID_TXT_SIZE;
  file = vfs_open(filename, VFS_WRITE);
  if(file != NULL)
  {
    len = vfs_write(p_channel_data, channel_data_size, 1, file);
    if(len == 0)
    {
      vfs_close(file);
      return ERR_FAILURE;
    }
    vfs_close(file);
  }
  else
  {
    OS_PRINTF("@@@ open file : %s failed \n",filename);
  }
  ui_comm_dlg_close();
  return SUCCESS;
}