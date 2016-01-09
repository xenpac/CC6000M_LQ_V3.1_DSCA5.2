/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"

#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "lib_char.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_misc.h"

#include "mem_manager.h"
#include "class_factory.h"

#include "pti.h"
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "mt_time.h"

#include "dvb_svc.h"
#include "eit.h"
#include "ts_packet.h"
#include "epg_data4.h"
#include "epg_data_priv4.h"
#include "lib_unicode.h"
#include "err_check_def.h"

//#define EPG_DEBUG

#ifdef  EPG_DEBUG
#define EPG_PRINT OS_PRINTF(" %s , %d lines", __FUNCTION__, __LINE__);OS_PRINTF
#else
#define EPG_PRINT DUMMY_PRINTF
#endif

/*!
   Section number in each segment
  */
#define SEC_NUM_PER_SEG (0x8)
/*!
   Max section number in one table
  */
#define MAX_SEC_ONE_TABLE (0xFF)
/*!
   PF full flag
  */
#define PF_FULL (0x3)
/*!
  Segment step
  */
#define SEG_SETP  (32)

/*!
  Max table count need to be parsed
  */
#define TABLE_CNT(p_epg_var)  ((p_epg_var->evt_max_day - 1)/4 + 1)

/*!
  EPG_EVT_OF_ALL_TS
  */
#define MIN_NET_NODE_CNT  (2)
#define MIN_TS_NODE_CNT (5)
#define MIN_SVC_NODE_CNT  (15)
#define MIN_EVT_NODE_CNT  (120)

/*!
  EPG_EVT_OF_CUR_TS_ONLY
  */
#define MIN_TS_NODE_CNT1  (3)
#define MIN_SVC_NODE_CNT1 (11)
#define MIN_EVT_NODE_CNT1 (240)

/*!
  EPG_EVT_OF_CUR_SVCS_ONLY
  */
#define MIN_TS_NODE_CNT2  (3)
#define MIN_SVC_NODE_CNT2 (5)
#define MIN_EVT_NODE_CNT2 (200)

/*!
  EPG_EVT_NAME_AND_SHORT
  */
#define FIX_MEM_PER_TOTAL   (40)

/*!
  EPG_EVT_INFO_ALL
  */
#define FIX_MEM_PER_TOTAL1   (40)


#define NOT(x) ((x) == 0)
#define ISLEAP(y) (NOT((y) % 400) || (((y) % 100) && NOT((y) % 4)))
#define UTC_TIME_TO_SEC(x) (\
  ((((x)->day) * 24 + ((x)->hour)) * 60 + ((x)->minute)) * 60 + ((x)->second))
#define S_CMP(x, y) ((x) < (y) ? (-1) : 0)
#define B_CMP(x, y) ((x) > (y) ? 1 : (S_CMP(x, y)))
#define T_CMP(x, y) ((x) == 0 ? (y) : (x))

#define EVT_TCMP(x, y) T_CMP(\
    T_CMP(B_CMP((x)->year, (y)->year), B_CMP((x)->month, (y)->month)),\
    T_CMP(T_CMP(B_CMP((x)->day, (y)->day), B_CMP((x)->hour, (y)->hour)),\
      T_CMP(B_CMP((x)->minute, (y)->minute), B_CMP((x)->second, (y)->second))))

//static void print_svc_all_event(sev_node_t *p_svc_node);
//static void print_ts_all_event(ts_node_t *p_ts_node);

static void del_event_node(void *p_data, evt_node_t *p_evt_node, sev_node_t *p_svc_node);

/*!
  API for allocating memory in EPG database module
  \param[in] mem size memory size to be allocated with byte as unit
  \param[in] id partition id to be allocated
  */
static void *epg_malloc_nonfix(lib_memp_t *p_memp, u32 mem_size)
{
  return lib_memp_alloc(p_memp, mem_size);
}

/*!
  API for allocating memory in EPG database by fix mode
  */
static void *epg_malloc_fix(lib_memf_t *p_memf)
{
  return lib_memf_alloc(p_memf);
}

/*!
   API for freeing memory in EPG database by non fix mode
   \param[in] p_mem_addr memory address to be freed
   \param[in] paratition id where p_mem_addr located
  */
static void epg_free_nonfix(lib_memp_t *p_memp, void *p_mem_addr)
{
  RET_CODE ret = SUCCESS;

  CHECK_FAIL_RET_VOID(p_mem_addr != NULL);
  ret = lib_memp_free(p_memp, p_mem_addr);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

static void epg_free_fix(lib_memf_t *p_memf, void *p_buf)
{
  RET_CODE ret = SUCCESS;
  ret = lib_memf_free(p_memf, p_buf);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

static inline void evt_tadd(utc_time_t *p_time, utc_time_t *p_add)
{
  utc_time_t temp = *p_time;
  utc_time_t temp1 = {0};
  u32 month_day = 0;
  u32 sec = 0;
  u8 mon_lengths[2][12] =
  {
    /* Normal years.  */
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    /* Leap years.  */
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
  };

  sec = UTC_TIME_TO_SEC(p_add);

  //add second
  if(sec > 0)
  {
    p_time->second = (temp.second + sec) % 60;
    sec = (temp.second + sec) / 60; //to minute
  }

  //add minute
  if(sec > 0)
  {
    p_time->minute = (temp.minute + sec) % 60;
    sec = (temp.minute + sec) / 60; //to hour
  }

  //add hour
  if(sec > 0)
  {
    p_time->hour = (temp.hour + sec) % 24;
    sec = (temp.hour + sec) / 24;  //to day
    //add_day(p_time, sec);
    temp1 = *p_time;
    while(sec > 0)
    {
      //month_day = get_month_day(p_time);
      month_day = mon_lengths[ISLEAP(p_time->year)][p_time->month - 1];
      if(temp1.day + sec > month_day) //overrun current month
      {
        sec -= (month_day + 1 - temp1.day);  //need some day to next month
        p_time->day = 1; //goto next month.
        temp1.day = 1;

        //month plus
        if(p_time->month < 12)
        {
          p_time->month++;
        }
        else
        {
          p_time->year++;
          p_time->month = 1;
        }
      }
      else
      {
        p_time->day = temp1.day + sec;
        sec = 0;
      }
    }
  }
}

/*!
   Create new network node with id equal input orginal_network_id
   \param[in] orig_network_id orginal network id
   \param[out] new orginal network node with input network id
  */
static orig_net_node_t *create_net_node(void *p_data, u16 orig_network_id)
{
  epg_db_t *p_epg_db = (epg_db_t *)p_data;
  orig_net_node_t *p_net_node = NULL;

  p_net_node = epg_malloc_fix(&p_epg_db->net_node_memf);
  if (p_net_node == NULL)
  {
    EPG_PRINT("\n##debug: No net node mem!!\n");
    return NULL;
  }
  //EPG_PRINT("\n##debug: create_net_node = 0x%x\n", p_net_node);
  p_net_node->orig_network_id = orig_network_id;
  p_net_node->p_next_net_node = NULL;
  p_net_node->p_ts_node_head = NULL;
  return p_net_node;
}

/*!
   if the network id already exist return exist net node,
   otherwise add the new node to net node list, return new net node
   \param[in] orig_network_id network id
   \param[out] original network node with the input network id
  */
static orig_net_node_t *add_net_node(void *p_data, u16 orig_network_id)
{
  epg_db_t *p_epg_db = (epg_db_t *)p_data;
  orig_net_node_t *p_cur_net_node = NULL;
  orig_net_node_t *p_pre_net_node = NULL;

  p_cur_net_node = p_epg_db->p_net_node_head;
  p_pre_net_node = p_epg_db->p_net_node_head;

  //first net node
 if (p_epg_db->p_net_node_head == NULL)
 {
   p_epg_db->p_net_node_head = create_net_node(p_data, orig_network_id);
   return p_epg_db->p_net_node_head;
 }

  while (p_cur_net_node != NULL)
  {
    if (p_cur_net_node->orig_network_id == orig_network_id)
    {
      break;
    }
    p_pre_net_node = p_cur_net_node;
    p_cur_net_node = p_cur_net_node->p_next_net_node;
  }

  if (p_cur_net_node == NULL)
  {
    p_cur_net_node = create_net_node(p_data, orig_network_id);
    p_pre_net_node->p_next_net_node = p_cur_net_node;
  }

  return p_cur_net_node;
}

static orig_net_node_t *get_net_node(void *p_data, u16 orig_network_id)
{
  epg_db_t *p_epg_db = (epg_db_t *)p_data;
  orig_net_node_t *p_cur_net_node = NULL;

  p_cur_net_node = p_epg_db->p_net_node_head;
  while (p_cur_net_node != NULL)
  {
    if (p_cur_net_node->orig_network_id == orig_network_id)
    {
      break;
    }
    p_cur_net_node = p_cur_net_node->p_next_net_node;
  }
  //EPG_PRINT("\nmatch_net_node = 0x%x\n", p_cur_net_node);
  return p_cur_net_node;
}

/*!
   Create new ts node with input ts id
   \param[in] ts_id
  */
static ts_node_t *create_ts_node(void *p_data, u16 ts_id)
{
  ts_node_t *p_ts_node = NULL;
  epg_db_t *p_epg_db = (epg_db_t *)p_data;

  p_ts_node =  epg_malloc_fix(&p_epg_db->ts_node_memf);
  if (p_ts_node == NULL)
  {
    EPG_PRINT("\n##debug: No TS node mem!!\n");
    return NULL;
  }

  p_ts_node->ts_id = ts_id;
  p_ts_node->is_pf_ready = FALSE;
  p_ts_node->pf_count = 0;
  p_ts_node->p_svc_head = NULL;

  p_ts_node->pf_cap_svc_num = 0;
  p_ts_node->pf_total_svc_num = 0;

  p_ts_node->p_next_ts_node = NULL;
  if (p_epg_db->evt_priority != EPG_EVT_OF_CUR_SVCS_ONLY)
  {
    p_ts_node->p_svc_info_per_tab = (tab_svc_info_t *)((u32)p_ts_node + sizeof(ts_node_t));
    memset(p_ts_node->p_svc_info_per_tab, 0, sizeof(tab_svc_info_t) * TABLE_CNT(p_epg_db));
  }

  return p_ts_node;
}


/*!
   if the ts id already exist in the net node return the exist ts node,
   otherwise add the new ts node to ts node list
   \param[in] ts_id: ts id to be matched
   \param[in] net node: ts node will be searched in this network node
   \param[out] ts node with the ts id and this ts node may be existing or newly
      added by  input ts id
  */
static ts_node_t *add_ts_node_to_net(void *p_data, u16 ts_id, orig_net_node_t *p_net_node)
{
  ts_node_t *p_cur_ts_node = p_net_node->p_ts_node_head;
  ts_node_t *p_pre_ts_node = p_net_node->p_ts_node_head;

  //first ts node
  if (p_net_node->p_ts_node_head == NULL)
  {
    p_net_node->p_ts_node_head = create_ts_node(p_data, ts_id);
    return p_net_node->p_ts_node_head;
  }

  while (p_cur_ts_node != NULL)
  {
    if (p_cur_ts_node->ts_id == ts_id)
    {
      break;
    }
    p_pre_ts_node = p_cur_ts_node;
    p_cur_ts_node = p_cur_ts_node->p_next_ts_node;
  }

  if (p_cur_ts_node == NULL)
  {
    p_cur_ts_node = create_ts_node(p_data, ts_id);
    p_pre_ts_node->p_next_ts_node = p_cur_ts_node;
  }

  return p_cur_ts_node;
}

static ts_node_t *get_ts_node(void *p_data, u16 ts_id, u16 orig_network_id)
{
  orig_net_node_t *p_net_node = NULL;
  ts_node_t *p_cur_ts_node = NULL;

  p_net_node = get_net_node(p_data, orig_network_id);

  if (p_net_node == NULL)
  {
    return NULL;
  }

  p_cur_ts_node = p_net_node->p_ts_node_head;
  while (p_cur_ts_node != NULL)
  {
    if (p_cur_ts_node->ts_id == ts_id)
    {
      break;
    }
    p_cur_ts_node = p_cur_ts_node->p_next_ts_node;
  }
  //EPG_PRINT("\nmatch_ts_node = 0x%x\n", p_cur_ts_node);
  return p_cur_ts_node;
}

/*!
   Create new service node
   \param[in] service id
   \param[in] ver_num for service node
  */
static sev_node_t *create_svc_node(void *p_data, eit_t *p_eit)
{
  sev_node_t *p_svc_node = NULL;
  epg_db_t *p_epg_db = (epg_db_t *)p_data;

  p_svc_node = epg_malloc_fix(&p_epg_db->svc_node_memf);
  if (p_svc_node == NULL)
  {
//    EPG_PRINT("\n##debug: No service node mem %d!!\n", p_eit->svc_id);
    return NULL;
  }

  //EPG_PRINT("\n##debug: svc id = %d create_svc_node = 0x%x\n", svc_id, p_svc_node);
  p_svc_node->pf_flag = 0;
  p_svc_node->pf_full = FALSE;
  p_svc_node->service_id = p_eit->svc_id;
  p_svc_node->new_ver_to_notify = FALSE;
  p_svc_node->section_present_got = FALSE;
  if(p_eit->table_id == DVB_TABLE_ID_EIT_ACTUAL
      && p_eit->section_number == 0)
    {
      p_svc_node->version = p_eit->version;
      p_svc_node->new_ver_to_notify = TRUE;
      p_svc_node->section_present_got = TRUE;
    }
  p_svc_node->p_next_s_node = NULL;
  p_svc_node->p_recent_evt_node = NULL;
  p_svc_node->p_evt_node_head = NULL;
  p_svc_node->saved_evt_day = 0;

  if (p_epg_db->evt_priority != EPG_EVT_OF_CUR_SVCS_ONLY)
  {
    p_svc_node->p_seg_info_in_table = (seg_info_each_tab_t *)((u32)p_svc_node + sizeof(sev_node_t));
    memset(p_svc_node->p_seg_info_in_table, 0, sizeof(seg_info_each_tab_t) * TABLE_CNT(p_epg_db));

    p_svc_node->pp_evt_day_head = (evt_node_t **)((u32)p_svc_node->p_seg_info_in_table
      + sizeof(seg_info_each_tab_t) * TABLE_CNT(p_epg_db));
  }
  else
  {
    p_svc_node->p_seg_info_in_table = NULL;
    p_svc_node->pp_evt_day_head = (evt_node_t **)((u32)p_svc_node + sizeof(sev_node_t));
  }
  memset(p_svc_node->pp_evt_day_head, 0, sizeof(evt_node_t *) * p_epg_db->evt_max_day);

  return p_svc_node;
}

/*!
   if the svc id already exist in the ts node, return the exist svc node,
   otherwise add the new svc node to svc node list
   \param[in] service id
   \param[in] ts node where the service node locating
  */
static sev_node_t *add_svc_node_to_ts(void *p_data, eit_t *p_eit,
                                                            ts_node_t *p_ts_node)
{
  sev_node_t *p_cur_svc_node = p_ts_node->p_svc_head;
  sev_node_t *p_pre_svc_node = p_ts_node->p_svc_head;

  //first svc node
  if (p_ts_node->p_svc_head == NULL)
  {
    p_ts_node->p_svc_head = create_svc_node(p_data, p_eit);
    return p_ts_node->p_svc_head;
  }

  while (p_cur_svc_node != NULL)
  {
    if (p_cur_svc_node->service_id == p_eit->svc_id)
    {
      break;
    }
    p_pre_svc_node = p_cur_svc_node;
    p_cur_svc_node = p_cur_svc_node->p_next_s_node;
  }

  if (p_cur_svc_node == NULL)
  {
    p_cur_svc_node = create_svc_node(p_data, p_eit);
    p_pre_svc_node->p_next_s_node = p_cur_svc_node;
  }

  return p_cur_svc_node;
}

static sev_node_t *get_svc_node(void *p_data, u16 svc_id, u16 ts_id, u16 orig_nw_id)
{
  orig_net_node_t *p_net_node = NULL;
  ts_node_t       *p_ts_node  = NULL;
  sev_node_t      *p_svc_node = NULL;
  epg_db_t        *p_epg_var  = (epg_db_t *)p_data;

 /*!
  1. found svc_node : network id + transport id + service id.
  */
  p_ts_node = get_ts_node(p_data, ts_id, orig_nw_id);
  if(p_ts_node != NULL)
  {
    //get svc node head, and ergodic all svc node.
    p_svc_node = p_ts_node->p_svc_head;
    while(p_svc_node != NULL)
    {
      if(p_svc_node->service_id == svc_id)
      {
        return p_svc_node;
      }
      //get next svc node.
      p_svc_node = p_svc_node->p_next_s_node;
    }
  }
  //else failed. and found next.

 /*!
  2. found svc_node : network id + service id.
  */
  p_net_node = get_net_node(p_data, orig_nw_id);
  if(p_net_node != NULL)
  {
    //get ts node head, and ergodic all ts node.
    p_ts_node = p_net_node->p_ts_node_head;
    while(p_ts_node != NULL)
    {
      //get svc node head, and ergodic all svc node.
      p_svc_node = p_ts_node->p_svc_head;
      while(p_svc_node != NULL)
      {
        if(p_svc_node->service_id == svc_id)
        {
          return p_svc_node;
        }
        //get next svc node.
        p_svc_node = p_svc_node->p_next_s_node;
      }
      //get next ts node.
      p_ts_node = p_ts_node->p_next_ts_node;
    }
  }
  //else failed. and found next.

 /*!
  3. found svc_node : transport id + service id.
  */
  //get net node head, and ergodic all net node.
  p_net_node = p_epg_var->p_net_node_head;
  while(p_net_node != NULL)
  {
    //get ts node head, and ergodic all ts node.
    p_ts_node = p_net_node->p_ts_node_head;
    while (p_ts_node != NULL)
    {
      if(p_ts_node->ts_id == ts_id)
      {
        //get svc node head, and ergodic all svc node.
        p_svc_node = p_ts_node->p_svc_head;
        while(p_svc_node != NULL)
        {
          if(p_svc_node->service_id == svc_id)
          {
            return p_svc_node;
          }
          //get next svc node.
          p_svc_node = p_svc_node->p_next_s_node;
        }
      }
      //get next ts node.
      p_ts_node = p_ts_node->p_next_ts_node;
    }

    //get next net node.
    p_net_node = p_net_node->p_next_net_node;
  }

  return NULL;
}

/*!
  save svc pf info, if all svc pf full return TRUE, or return FALSE
  */
static BOOL save_svc_pf_info(eit_t *p_eit_info, sev_node_t *p_svc_node, ts_node_t *p_ts_node)
{
  /*!
     Setting section flag
    */
  if (p_eit_info->section_number > 1)
  {
    return FALSE;
  }

  if (p_svc_node->pf_flag == 0)
  {
    p_ts_node->pf_total_svc_num++;
  }

  p_svc_node->pf_flag |= (1 << p_eit_info->section_number);

  /*!
     When new service is found, increase captured service number
    */
  if (p_svc_node->pf_flag == PF_FULL
    && p_svc_node->pf_full == FALSE)
  {
    p_ts_node->pf_cap_svc_num++;
    p_svc_node->pf_full = TRUE;
    return FALSE;
  }

  if (p_ts_node->pf_cap_svc_num == p_ts_node->pf_total_svc_num
    && p_ts_node->is_pf_ready == FALSE
    && p_ts_node->pf_cap_svc_num != 0)
  {
    //pf ready
    p_ts_node->is_pf_ready = TRUE;
    EPG_PRINT("\n##debug:EPG_DB_PF_READY!!\n");
    //print_ts_all_event(p_ts_node);
    return TRUE;
  }

  return FALSE;
}

static BOOL save_svc_seg_info(void *p_data, eit_t *p_eit_info,
  sev_node_t *p_svc_node, ts_node_t *p_ts_node)
{
  u8 seg_num = 0;
  u8 sec_num = 0;
  s8 cmp_reslt = 0;
  u8 tab_cnt = 0;
  seg_status_t ret = SEG_NOT_READY;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  tab_svc_info_t *p_cur_tab = NULL;
  seg_info_each_tab_t *p_seg_info = NULL;
  u32 max_seg_num = 0;

  //Check whether table id is existing
  for (tab_cnt = 0; tab_cnt < TABLE_CNT(p_epg_var); tab_cnt++)
  {
    p_cur_tab = &p_ts_node->p_svc_info_per_tab[tab_cnt];
    if (p_cur_tab->table_id == 0
      || p_cur_tab->table_id == p_eit_info->table_id
      || (p_cur_tab->table_id == p_eit_info->table_id + 0x10)
      || (p_cur_tab->table_id + 0x10 == p_eit_info->table_id))
    {
      break;
    }
  }

  CHECK_FAIL_RET_FALSE(tab_cnt < TABLE_CNT(p_epg_var));

  //Initialize table information when a new table id is found
  if (p_cur_tab->table_id != p_eit_info->table_id)
  {
    if (p_cur_tab->table_id == 0)
    {
      p_cur_tab->capt_serv_num = 0;
      p_cur_tab->total_serv_num = 0;
      p_cur_tab->is_sch_full = FALSE;
    }
    p_cur_tab->table_id = p_eit_info->table_id;
  }

  p_seg_info = &p_svc_node->p_seg_info_in_table[tab_cnt];
  if (p_seg_info->tab_id == 0
    || (p_seg_info->tab_id == (p_eit_info->table_id + 0x10))
    || ((p_seg_info->tab_id + 0x10) == p_eit_info->table_id))
  {
    if (p_seg_info->tab_id == 0)
    {
      p_ts_node->p_svc_info_per_tab[tab_cnt].total_serv_num++;
      memset(p_seg_info->p_targ_seg_info, MAX_SEC_ONE_TABLE, MAX_SEG_NUM_EACH_TABLE);
      memset(p_seg_info->p_capt_seg_info, 0, MAX_SEG_NUM_EACH_TABLE);
    }
    p_seg_info->tab_id = p_eit_info->table_id;
  }

  seg_num = p_eit_info->section_number / SEC_NUM_PER_SEG;
  /*!
     Set section info to each segment when got the first section of the segment
    */
  if ((p_eit_info->section_number % SEC_NUM_PER_SEG == 0)
    && (p_eit_info->last_section_number >= SEC_NUM_PER_SEG))
  {
    sec_num = p_eit_info->seg_last_sec_number - p_eit_info->section_number + 1;
    p_seg_info->p_targ_seg_info[seg_num] = (MAX_SEC_ONE_TABLE) >> (SEC_NUM_PER_SEG - sec_num);
  }

  /*!
     Captured section infor per each segment
    */
  if (p_eit_info->last_section_number >= SEC_NUM_PER_SEG)
  {
    //p_seg_info = &p_svc_node->p_seg_info_in_table[tab_cnt];
    max_seg_num = p_eit_info->last_section_number / 8 + 1;

    sec_num = p_eit_info->seg_last_sec_number - p_eit_info->section_number;

    p_seg_info->p_capt_seg_info[seg_num] |= (1 << sec_num);

    cmp_reslt = memcmp(p_seg_info->p_targ_seg_info, p_seg_info->p_capt_seg_info, max_seg_num);

    /*!
       All section is full
      */
    if (cmp_reslt == 0 && p_seg_info->is_all_evt_saved == FALSE)
    {
      //Fix me: make sure that EPG one service id ready message should be sent
      p_ts_node->p_svc_info_per_tab[tab_cnt].capt_serv_num ++;
      p_seg_info->is_all_evt_saved = TRUE;
      ret = SEG_TAB_FULL;
    }
  }

  if (ret != SEG_TAB_FULL)
  {
    return FALSE;
  }

  if ((p_cur_tab->capt_serv_num == p_cur_tab->total_serv_num)
         && (p_cur_tab->is_sch_full == FALSE)
         && (p_cur_tab->total_serv_num != 0)
         && (p_ts_node->is_pf_ready == TRUE))
  {
    //schedule ready
    p_cur_tab->is_sch_full = TRUE;
    EPG_PRINT("\n##debug:EPG_DB_ONE_TAB_READY 0x%x!\n", p_eit_info->table_id);
    //print_ts_all_event(p_ts_node);
    return TRUE;
  }

  return FALSE;
}

static RET_CODE ext_desc_to_unicode(ext_evt_desc_uni_t *p_uni_ext_desc,
ext_evt_desc_t *p_ext_desc)
{
  u16 item_cnt = 0;

  p_uni_ext_desc->evt_id = p_ext_desc->evt_id;
  p_uni_ext_desc->index  = p_ext_desc->index;
  p_uni_ext_desc->item_num = p_ext_desc->item_num;
  p_uni_ext_desc->tot_ext_desc = p_ext_desc->tot_ext_desc;

  //Language code to unicode
  //dvb_to_unicode(p_ext_desc->lang_code, LANGUAGE_CODE_LEN,
  //    p_uni_ext_desc->lang_code, LANGUAGE_CODE_LEN + 1);

  //Item content to unicode
  for (item_cnt = 0; item_cnt < p_ext_desc->item_num; item_cnt ++)
  {
    if (item_cnt >= MAX_ITEM_NUM)
    {
      break;
    }
    dvb_to_unicode_epg(p_ext_desc->item[item_cnt].item_cont, MAX_ITEM_CONT_LEN,
      p_uni_ext_desc->item[item_cnt].item_cont, MAX_ITEM_CONT_LEN + 1,
      get_iso639_refcode(p_ext_desc->lang_code));

    dvb_to_unicode_epg(p_ext_desc->item[item_cnt].item_name, MAX_ITEM_CONT_LEN,
      p_uni_ext_desc->item[item_cnt].item_name, MAX_ITEM_CONT_LEN + 1, 
      get_iso639_refcode(p_ext_desc->lang_code));
  }

  return SUCCESS;
}

/*static void test_time(utc_time_t *p_time, char *p_name)
{
    EPG_PRINT("%s[%.4d-%.2d-%.2d-%.2d:%.2d:%.2d]",
      p_name, p_time->year,
      p_time->month,
      p_time->day,
      p_time->hour,
      p_time->minute,
      p_time->second);
}
static void print_eit_info(eit_t *p_eit_info, u8 evt_idx)
{
  EPG_PRINT("\n");

  EPG_PRINT("[%d]", p_eit_info->org_nw_id);
  EPG_PRINT("[%d]", p_eit_info->stream_id);
  EPG_PRINT("[%d]", p_eit_info->table_id);
  EPG_PRINT("[%d]", p_eit_info->version);
  EPG_PRINT("[%d]", p_eit_info->sec_length);
  EPG_PRINT("[%d]", p_eit_info->svc_id);
  EPG_PRINT("[%d]", p_eit_info->pf_flag);
  EPG_PRINT("[%d]", p_eit_info->section_number);
  EPG_PRINT("[%d]", p_eit_info->last_section_number);
  EPG_PRINT("[%d]", p_eit_info->seg_last_sec_number);
  EPG_PRINT("[%d]", p_eit_info->last_table_id);
  EPG_PRINT("[%d]", p_eit_info->tot_evt_num);
  EPG_PRINT("[%d]", p_eit_info->tot_ext_info_num);
  EPG_PRINT("[%d]", p_eit_info->sht_evt_info[evt_idx].evt_id);
  test_time(&p_eit_info->sht_evt_info[evt_idx].st_tm, "");
  test_time(&p_eit_info->sht_evt_info[evt_idx].drt_time, "");

  EPG_PRINT("\n");
}
*/
/*!
  For EPG to add new eit information into system memory
  */
static BOOL cpy_eit_info_to_evt(eit_t *p_eit_info, u8 evt_idx, evt_node_t *p_evt_node)
{
  sht_evt_desc_t *p_eit_desc = &p_eit_info->sht_evt_info[evt_idx];
  u16 cnt = 0;
  ext_evt_desc_uni_t *p_ext_desc = NULL;
  u16 ext_cnt = 0;

  //print_eit_info(p_eit_info, evt_idx);
  //check content desc
  for (cnt = 0; cnt < p_eit_info->tot_cont_num; cnt++)
  {
    if (cnt >= MAX_EVT_PER_SEC)
    {
      break;
    }
    if (p_eit_info->cont_desc[cnt].evt_id == p_eit_desc->evt_id)
    {
      break;
    }
  }

  if ((cnt >= MAX_EVT_PER_SEC) || (cnt >= p_eit_info->tot_cont_num))
  {
    p_evt_node->cont_valid_flag = 0;
  }
  else
  {
    //Set content information
    p_evt_node->cont_valid_flag = 1;
    p_evt_node->cont_level  = p_eit_info->cont_desc[cnt].cont_level;
    p_evt_node->usr_nib   = p_eit_info->cont_desc[cnt].usr_nib;
  }

  //Copy event description information
  if (p_eit_info->sec_length > MIN_EIT_LENGTH)
  {
    //short event description
    p_evt_node->event_id = p_eit_desc->evt_id;
    //memcpy(&p_evt_node->start_time, &p_eit_desc->st_tm, sizeof(utc_time_t));
    //memcpy(&p_evt_node->drt_time, &p_eit_desc->drt_time, sizeof(utc_time_t));
    p_evt_node->start_time = p_eit_desc->st_tm;
    p_evt_node->drt_time = p_eit_desc->drt_time;
//EPG_PRINT("\n##debug: event_id = %d\n", p_evt_node->event_id);
    //Event name to unicode
#if 0
    if (p_eit_desc->name_len > 0)
    {
      dvb_to_unicode(p_eit_desc->evt_nm, p_eit_desc->name_len,
        p_evt_node->event_name, MAX_EVT_NAME_LEN + 1);
    }
    else
    {
      p_evt_node->event_name[0] = 32;
      p_evt_node->event_name[1] = 0;
    }
    //Language code to unicode
    //The real length of language code is 3 and the last is 0
    dvb_to_unicode(p_eit_desc->lang_code, LANGUAGE_CODE_LEN ,
      p_evt_node->lang_code, LANGUAGE_CODE_LEN + 1);
#endif
    p_evt_node->parental_rating = p_eit_desc->parental_rating.rating;
    p_ext_desc = p_evt_node->p_ext_desc;

    ext_cnt = 0;
    while (p_ext_desc != NULL)
    {
      //Extend event description
      for (cnt = 0; cnt < p_eit_info->tot_ext_info_num; cnt ++)
      {
        if (cnt >= MAX_EXT_DESC_NUM)
        {
          break;
        }
        if (p_eit_info->ext_evt_info[cnt].evt_id == p_eit_desc->evt_id
          && p_eit_info->ext_evt_info[cnt].index == ext_cnt)
        {
          ext_cnt ++;
          ext_desc_to_unicode(p_ext_desc, &p_eit_info->ext_evt_info[cnt]);
          break;
        }
      }

      p_ext_desc = p_ext_desc->p_nxt_ext;
    }
  }
  else
  {
    memset(p_evt_node->event_name, 0, sizeof(u16) * (MAX_EVT_NAME_LEN + 1));
    p_evt_node->event_id = 0;
  }
  return TRUE;
}

/*!
   Create new event node
   \param[in] p_eit_info eit information for creating new event node
   \param[in] event indx in eit description
  */
static evt_node_t *create_evt_node(void *p_data, eit_t *p_eit_info, u8 evt_idx, u32 *p_ret)
{
  evt_node_t *p_new_e_node = NULL;
  sht_evt_desc_t *p_eit_desc = &p_eit_info->sht_evt_info[evt_idx];
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  u16 idx = 0, tmp = 0;
  u8 ext_desc_num = 0;
  ext_evt_desc_uni_t *p_ext_desc = NULL;
  u32 ext_txt_len  = 0;
  u8 *p_total_buffer = NULL;
  u32 buffer_addr = 0;
  u32 ext_total_len = 0;

  *p_ret = 0;
  p_new_e_node = epg_malloc_fix(&p_epg_var->evt_node_memf);
  if (p_new_e_node == NULL)
  {
    EPG_PRINT("N0 event node memory!\n");
    return NULL;
  }

  p_new_e_node->p_next_evt_node = NULL;
  p_new_e_node->p_sht_txt = NULL;
  p_new_e_node->p_ext_desc = NULL;
  p_new_e_node->p_ext_txt = NULL;
  p_new_e_node->ext_desc_num = 0;
  p_new_e_node->sht_txt_len  = 0;
  p_new_e_node->shrt_ext_len = 0;

  if (p_epg_var->evt_info_priority == EPG_EVT_NAME_ONLY)
  {
    cpy_eit_info_to_evt(p_eit_info, evt_idx, p_new_e_node);

    return p_new_e_node;
  }

  //Check short txt information
  for (idx = 0; idx < p_eit_info->tot_sht_txt_num; idx ++)
  {
    if (idx >= MAX_SHT_TXT_NUM)
    {
      break;
    }
    
    if(p_eit_info->sht_txt[idx].evt_id == p_eit_desc->evt_id)
    {
      if(p_eit_info->sht_txt[idx].name_len)
      {
        dvb_to_unicode_epg(p_eit_info->sht_txt[idx].evt_nm,
          p_eit_info->sht_txt[idx].name_len,
          p_new_e_node->event_name, MAX_EVT_NAME_LEN + 1, 
          get_iso639_refcode(p_eit_info->sht_txt[idx].lang_code));
      }
      else
      {
        p_new_e_node->event_name[0] = 32;
        p_new_e_node->event_name[1] = 0;
      }
      
      //dvb_to_unicode(p_eit_info->sht_txt[idx].lang_code, LANGUAGE_CODE_LEN ,
      //    p_new_e_node->lang_code, LANGUAGE_CODE_LEN + 1);
          
      if(p_eit_info->sht_txt[idx].txt_len != 0)
      {
        p_new_e_node->sht_txt_len += p_eit_info->sht_txt[idx].txt_len + 1;
        
        ext_total_len += p_new_e_node->sht_txt_len;
      }
    }
  }

  if (p_epg_var->evt_info_priority == EPG_EVT_INFO_ALL)
  {
    ext_txt_len = 0;

    //Check extend text information
    for (idx = 0; idx < p_eit_info->tot_ext_txt_num; idx ++)
    {
      if (idx >= MAX_EXT_TXT_NUM)
      {
        break;
      }

      if (p_eit_info->ext_txt[idx].evt_id == p_eit_desc->evt_id
          && p_eit_info->ext_txt[idx].txt_len != 0)
      {
        ext_txt_len +=  p_eit_info->ext_txt[idx].txt_len;
      }
    }

    if (ext_txt_len > 0)
    {
      ext_total_len += ext_txt_len;
    }

    //Check extend desc
    for (idx = 0; idx < p_eit_info->tot_ext_info_num; idx ++)
    {
      if (idx >= MAX_EXT_DESC_NUM)
      {
        break;
      }

      if (p_eit_info->ext_evt_info[idx].evt_id == p_eit_desc->evt_id)
      {
        ext_desc_num ++;
      }
    }
  }
  else
  {
    ext_txt_len = 0;
    ext_desc_num = 0;
  }

  if (ext_total_len > 0)
  {
    //malloc buffer for short txt & ext txt.
    tmp = ext_desc_num * sizeof(ext_evt_desc_uni_t) + ext_total_len * sizeof(u16) + 8;
    p_total_buffer = epg_malloc_nonfix(&p_epg_var->memp, tmp);
    if (p_total_buffer == NULL)
    {
      epg_free_fix(&p_epg_var->evt_node_memf, p_new_e_node);
      p_new_e_node = NULL;
      
      return NULL;
    }

    memset(p_total_buffer, 0, ext_total_len * sizeof(u16) + 4);

    p_new_e_node->shrt_ext_len = ext_total_len;

    buffer_addr = (u32)p_total_buffer;

    //save short txt.
    p_new_e_node->p_sht_txt = (u16 *)buffer_addr;

    for (idx = 0; idx < p_eit_info->tot_sht_txt_num; idx ++)
    {   
      if((p_eit_info->sht_txt[idx].evt_id == p_eit_desc->evt_id) &&
        (p_eit_info->sht_txt[idx].txt_len != 0) && 
        (idx < MAX_SHT_TXT_NUM))
      {
        tmp = uni_strlen(p_new_e_node->p_sht_txt);
        
        dvb_to_unicode_epg(p_eit_info->sht_txt[idx].txt, p_eit_info->sht_txt[idx].txt_len,
          p_new_e_node->p_sht_txt + tmp, p_new_e_node->sht_txt_len - tmp, 
          get_iso639_refcode(p_eit_info->sht_txt[idx].lang_code));
      }
    }

    //save ext txt.
    buffer_addr += (p_new_e_node->sht_txt_len + 1) * sizeof(u16);

    if(ext_txt_len == 0)
    {
      p_new_e_node->p_ext_txt = NULL;
    }
    else
    {
      p_new_e_node->p_ext_txt = (u16 *)buffer_addr;

      for (idx = 0; idx < p_eit_info->tot_ext_txt_num; idx ++)
      {
        if((p_eit_info->ext_txt[idx].evt_id == p_eit_desc->evt_id) &&
          (p_eit_info->ext_txt[idx].txt_len != 0) && 
          (idx < MAX_EXT_TXT_NUM))
        {
          tmp = uni_strlen(p_new_e_node->p_ext_txt);
          
          dvb_to_unicode_epg(p_eit_info->ext_txt[idx].ext_txt, p_eit_info->ext_txt[idx].txt_len,
            p_new_e_node->p_ext_txt + tmp, ext_txt_len - tmp,
            get_iso639_refcode(p_eit_info->ext_txt[idx].lang_code));
        }
      }

      buffer_addr += (ext_txt_len + 2) * sizeof(u16);        
    }

    //Alloc head for extend event description
    //sometime the buffer_addr is not align by 4bytes,
    //because (sht_txt_len + ext_txt_len + 1) is odd number, we can't
    //use the buffer directly. So we need 4 betys more.
    //align to 4bytes
    buffer_addr = (buffer_addr + 0x3) & (~0x3);
    if (ext_desc_num > 0)
    {
      p_new_e_node->p_ext_desc = (ext_evt_desc_uni_t *)buffer_addr;
      buffer_addr += sizeof(ext_evt_desc_uni_t);

      p_new_e_node->event_id = 0;
      p_new_e_node->p_ext_desc->item_num = 0;
      p_new_e_node->p_ext_desc->tot_ext_desc = 0;
      p_new_e_node->p_ext_desc->p_nxt_ext = NULL;
      p_new_e_node->ext_desc_num = ext_desc_num;
      p_ext_desc = p_new_e_node->p_ext_desc;
      ext_desc_num --;
    }

    while (ext_desc_num > 0)
    {
      p_ext_desc->p_nxt_ext = (ext_evt_desc_uni_t *)buffer_addr;
      buffer_addr += sizeof(ext_evt_desc_uni_t);

      p_ext_desc->p_nxt_ext->p_nxt_ext = NULL;
      p_ext_desc->p_nxt_ext->item_num  = 0;
      p_ext_desc->p_nxt_ext->tot_ext_desc = 0;
      p_ext_desc->p_nxt_ext->evt_id = 0;
      p_ext_desc = p_ext_desc->p_nxt_ext;
      ext_desc_num --;
    }

    CHECK_FAIL_RET_NULL(ext_desc_num == 0);
  }

  cpy_eit_info_to_evt(p_eit_info, evt_idx, p_new_e_node);

  return p_new_e_node;
}

static inline void set_evt_day_head(void *p_data, sev_node_t *p_svc_node, evt_node_t *p_evt_node)
{
  evt_node_t *p_tmp = NULL;
  s8 day_idx = 0;
  s8 loopi = 0;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  if (p_svc_node->saved_evt_day == 0)
  {
    p_svc_node->pp_evt_day_head[0] = p_evt_node;
    p_svc_node->saved_evt_day ++;
    return ;
  }

  //replace evt day head if the evt time more early
  for (day_idx = p_svc_node->saved_evt_day - 1; day_idx >= 0; day_idx --)
  {
    p_tmp = p_svc_node->pp_evt_day_head[day_idx];
    if (p_tmp->start_time.day == p_evt_node->start_time.day)
    {
      if (EVT_TCMP(&p_evt_node->start_time, &p_tmp->start_time) == -1)
      {
        p_svc_node->pp_evt_day_head[day_idx] = p_evt_node;
      }
      return ;
    }
  }

  if (p_epg_var->evt_max_day == p_svc_node->saved_evt_day)
  {
    return ;
  }

  //insert evt day head to table head
  for (day_idx = 0; day_idx < p_svc_node->saved_evt_day; day_idx ++)
  {
    p_tmp = p_svc_node->pp_evt_day_head[day_idx];
    if (EVT_TCMP(&p_evt_node->start_time, &p_tmp->start_time) == -1)
    {
      for (loopi = p_svc_node->saved_evt_day; loopi > day_idx; loopi --)
      {
        p_svc_node->pp_evt_day_head[loopi] = p_svc_node->pp_evt_day_head[loopi -1];
      }
      p_svc_node->pp_evt_day_head[day_idx] = p_evt_node;
      p_svc_node->saved_evt_day ++;
      return ;
    }
  }

  //add evt day head to table end
  p_svc_node->pp_evt_day_head[p_svc_node->saved_evt_day] = p_evt_node;
  p_svc_node->saved_evt_day ++;
}

static void reset_evt_day_head(void *p_data, sev_node_t *p_svc_node)
{
  evt_node_t *p_evt_node = NULL;
  evt_node_t *p_tmp = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  p_svc_node->saved_evt_day = 0;
  p_evt_node = p_svc_node->p_evt_node_head;

  if (p_evt_node == NULL)
  {
    return ;
  }

  p_svc_node->pp_evt_day_head[0] = p_evt_node;
  p_tmp = p_evt_node;
  p_svc_node->saved_evt_day ++;
  p_evt_node = p_evt_node->p_next_evt_node;
  while (p_evt_node != NULL)
  {
    if (p_evt_node->start_time.day != p_tmp->start_time.day)
    {
      p_svc_node->pp_evt_day_head[p_svc_node->saved_evt_day] = p_evt_node;
      p_tmp = p_evt_node;
      p_svc_node->saved_evt_day ++;
      if (p_epg_var->evt_max_day == p_svc_node->saved_evt_day)
      {
        return ;
      }
    }
    p_evt_node = p_evt_node->p_next_evt_node;
  }
}

static inline BOOL is_evt_in_svc(sev_node_t *p_svc_node, sht_evt_desc_t *p_evt_desc)
{
  u8 day_idx = 0;
  evt_node_t *p_evt_temp = NULL;

  for (day_idx = 0; day_idx < p_svc_node->saved_evt_day; day_idx ++)
  {
    p_evt_temp = p_svc_node->pp_evt_day_head[day_idx];
    if (p_evt_temp->start_time.day == p_evt_desc->st_tm.day)
    {
      break;
    }
  }

  if (day_idx == p_svc_node->saved_evt_day)
  {
    return FALSE;
  }

  while (p_evt_temp)
  {
    if ((EVT_TCMP(&p_evt_temp->start_time, &p_evt_desc->st_tm) == 1)
      || (p_evt_temp->start_time.day != p_evt_desc->st_tm.day))
    {
      break;
    }
    else if (p_evt_temp->event_id == p_evt_desc->evt_id)
    {
      return TRUE;
    }

    p_evt_temp = p_evt_temp->p_next_evt_node;
  }

  return FALSE;
}

static inline BOOL insert_evt_to_svc_by_order(sev_node_t *p_svc_node, evt_node_t *p_new_evt)
{
  BOOL is_node_added = FALSE;
  s8 cmp_reslt = 0;
  s8 day_idx = 0;
  evt_node_t *p_prv_evt = NULL;
  evt_node_t *p_tmp_evt = NULL;

  //new evt insert postion
  if (p_svc_node->p_recent_evt_node &&
    (EVT_TCMP(&p_svc_node->p_recent_evt_node->start_time, &p_new_evt->start_time) == -1))
  {
    p_prv_evt = p_svc_node->p_recent_evt_node;
    p_tmp_evt = p_prv_evt->p_next_evt_node;
  }
  else
  {
    for (day_idx = 0; day_idx < p_svc_node->saved_evt_day; day_idx ++)
    {
      p_prv_evt = p_svc_node->pp_evt_day_head[day_idx];
      cmp_reslt = EVT_TCMP(&p_prv_evt->start_time, &p_new_evt->start_time);

      if (cmp_reslt == 1)
      {
        if (day_idx)
        {
          p_prv_evt = p_svc_node->pp_evt_day_head[day_idx - 1];
        }
        else
        {
          is_node_added = TRUE;
          p_new_evt->p_next_evt_node = p_prv_evt;
          p_svc_node->p_evt_node_head = p_new_evt;
        }
        break;
      }
      else if (cmp_reslt == 0)
      {
        p_tmp_evt = p_prv_evt;
        p_prv_evt = NULL;
        break;
      }
    }

    if (is_node_added == FALSE && p_prv_evt)
    {
      p_tmp_evt = p_prv_evt->p_next_evt_node;
    }
  }

  //insert by start time
  if (is_node_added == FALSE)
  {
    //The first event node
    if (p_svc_node->p_evt_node_head == NULL)
    {
      p_svc_node->p_evt_node_head = p_new_evt;
      is_node_added = TRUE;
    }
    else if (p_tmp_evt == NULL)
    {
      p_prv_evt->p_next_evt_node = p_new_evt;
      is_node_added = TRUE;
    }
    else
    {
      while (p_tmp_evt)
      {
        if (p_tmp_evt->event_id == p_new_evt->event_id)
        {
          break;
        }
        cmp_reslt = EVT_TCMP(&p_new_evt->start_time, &p_tmp_evt->start_time);

        if (cmp_reslt == 1)
        {
          p_prv_evt = p_tmp_evt;
          p_tmp_evt = p_tmp_evt->p_next_evt_node;
          if (p_tmp_evt == NULL)
          {
            p_prv_evt->p_next_evt_node = p_new_evt;
            is_node_added = TRUE;
            break;
          }
        }
        else if (cmp_reslt == -1)
        {
          if (p_prv_evt == NULL)
          {
            p_new_evt->p_next_evt_node = p_tmp_evt;
            p_svc_node->p_evt_node_head = p_new_evt;
          }
          else
          {
            p_new_evt->p_next_evt_node = p_tmp_evt;
            p_prv_evt->p_next_evt_node = p_new_evt;
          }
          is_node_added = TRUE;
          break;
        }
        else
          {
           break;
          }
      }
    }
  }

  return is_node_added;
}

/*!
   Add all event information in EIT into service node
   \param[in] p_eit_info input EIT information
   \param[in] service node where EIT information will be added
   \param[out] EPG database status
  */
static epg_db_status_t add_evt_node_to_svc(void *p_data,
  eit_t *p_eit_info, sev_node_t *p_svc_node)
{
  u8 evt_idx = 0;
  utc_time_t end_time = {0};
  utc_time_t sys_time = {0};
  evt_node_t *p_new_evt = NULL;
  epg_db_status_t ret = EPG_DB_NORM;
  u32 create_ret = 0;
  sht_evt_desc_t *p_evt_desc = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  time_get(&sys_time, TRUE);

  while (evt_idx < p_eit_info->tot_evt_num)
  {
    if (evt_idx >= MAX_EVT_PER_SEC)
    {
      break;
    }

    p_evt_desc = &p_eit_info->sht_evt_info[evt_idx];
    //memcpy(&end_time, &p_evt_desc->st_tm, sizeof(utc_time_t));
    end_time = p_evt_desc->st_tm;
    evt_tadd(&end_time, &p_evt_desc->drt_time);

    //overdue evt no need to save
    if ((p_eit_info->table_id == DVB_TABLE_ID_EIT_SCH_ACTUAL
      || p_eit_info->table_id == DVB_TABLE_ID_EIT_SCH_OTHER)
      && (EVT_TCMP(&end_time, &sys_time) != 1))
    {
      evt_idx ++;
      continue;
    }

    //evt time out dynamic policy no need save
    if ((p_epg_var->evt_priority == EPG_EVT_OF_CUR_SVCS_ONLY)
      && (p_eit_info->table_id > DVB_TABLE_ID_EIT_OTHER)
      && p_epg_var->p_dy_policy_intra->start.year
      && ((EVT_TCMP(&end_time, &p_epg_var->p_dy_policy_intra->start) <= 0)
      || (EVT_TCMP(&p_evt_desc->st_tm, &p_epg_var->p_dy_policy_intra->end) >= 0)))
    {
      evt_idx ++;
      continue;
    }

    if ((p_epg_var->evt_priority == EPG_EVT_OF_CUR_SVCS_ONLY
      || p_eit_info->table_id <= DVB_TABLE_ID_EIT_OTHER)
      && is_evt_in_svc(p_svc_node, p_evt_desc))
    {
      evt_idx ++;
      continue;
    }

    p_new_evt = create_evt_node(p_data, p_eit_info, evt_idx, &create_ret);
    if (p_new_evt == NULL)
    {
      if (create_ret == 1)
      {
        evt_idx ++;
        continue;
      }
      return EPG_DB_OVERFLOW;
    }

    if (insert_evt_to_svc_by_order(p_svc_node, p_new_evt))
    {
      p_svc_node->p_recent_evt_node = p_new_evt;
      ret = EPG_DB_NEW_SEC_FOUND;
      set_evt_day_head(p_data, p_svc_node, p_new_evt);
    }
    else
    {
      del_event_node(p_data, p_new_evt, p_svc_node);
    }

    evt_idx++;
  }

  return ret;
}

/*!
   find evt parents exist otherwise add evt parents node
   \param[in] p_eit_info: EIT table input with network id, ts id and service id
   as input
   \param[out] p_svc_node: service node if service id matching
      or newly created service node with  the service id in EIT head
   \param[out] p_ts_node:	ts node if ts id matching or newly created ts node
   with
      same stream id value in EIT head
  */
static epg_db_status_t add_evt_parents_node(void *p_data,
                  eit_t *p_eit_info, evt_parents_node_t *p_evt_parent)
{
  ts_node_t *p_ts_node = NULL;

  p_evt_parent->p_svc_node = NULL;
  p_evt_parent->p_ts_node = NULL;

  p_evt_parent->p_net_node = add_net_node(p_data, p_eit_info->org_nw_id);
  if (p_evt_parent->p_net_node == NULL)
  {
//    EPG_PRINT("NO net node memory\n");
    return EPG_DB_OVERFLOW;
  }

  p_evt_parent->p_ts_node = add_ts_node_to_net(p_data,
    p_eit_info->stream_id, p_evt_parent->p_net_node);
  if (p_evt_parent->p_ts_node == NULL)
  {
//    EPG_PRINT("NO ts node memory\n");
    return EPG_DB_OVERFLOW;
  }

  p_ts_node = p_evt_parent->p_ts_node;

  p_evt_parent->p_svc_node = add_svc_node_to_ts(p_data,
    p_eit_info, p_ts_node);

  if (p_evt_parent->p_svc_node == NULL)
  {
 //   EPG_PRINT("NO svc node memory\n");
    return EPG_DB_OVERFLOW;
  }

  return EPG_DB_NORM;
}

static BOOL need_save_eit_info(void *p_data, eit_t *p_eit_info)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  u16 idx = 0;

  if ((p_eit_info->table_id >= (DVB_TABLE_ID_EIT_SCH_ACTUAL + TABLE_CNT(p_epg_var))
    && p_eit_info->table_id <= DVB_TABLE_ID_EIT_SCH_ACTUAL_MAX)
    || (p_eit_info->table_id >= (DVB_TABLE_ID_EIT_SCH_OTHER + TABLE_CNT(p_epg_var))))
  {
    return FALSE;
  }

  switch (p_epg_var->evt_priority)
  {
  case EPG_EVT_OF_CUR_SVCS_ONLY:
    {
      /*if (p_eit_info->table_id >= DVB_TABLE_ID_EIT_SCH_OTHER
        || p_eit_info->table_id == DVB_TABLE_ID_EIT_OTHER)
      {
        return FALSE;
      }*/

      if (p_epg_var->p_dy_policy_intra != NULL)
      {
        for (idx = 0; idx < p_epg_var->dy_policy_cnt; idx ++)
        {
          if (p_eit_info->svc_id == p_epg_var->p_dy_policy_intra[idx].cur_svc_id)
          {
            break;
          }
          else
          {
            if (idx == p_epg_var->dy_policy_cnt - 1)
            {
              return FALSE;
            }
          }
        }
      }
    }
    break;

  case EPG_EVT_OF_CUR_TS_ONLY:
    {
      if (p_eit_info->table_id >= DVB_TABLE_ID_EIT_SCH_OTHER
        || p_eit_info->table_id == DVB_TABLE_ID_EIT_OTHER)
      {
        return FALSE;
      }
    }
    break;

  default:
    break;
  }

  //NO EPG information existing
  if (p_eit_info->sec_length == MIN_EIT_LENGTH
    && p_eit_info->last_section_number < SEC_NUM_PER_SEG)
  {
    return FALSE;
  }

  return TRUE;
}

static inline s8 is_evt_in_given_period(evt_node_t *p_evt_node,
  utc_time_t *p_start_time, utc_time_t *p_end_time)
{
  utc_time_t evt_end_time = {0};

  CHECK_FAIL_RET((p_evt_node != NULL), -1);

  memcpy(&evt_end_time, &p_evt_node->start_time, sizeof(utc_time_t));
  evt_tadd(&evt_end_time, &p_evt_node->drt_time);

  if (EVT_TCMP(&evt_end_time, p_start_time) <= 0)
  {
    return -1;
  }

  if (EVT_TCMP(&p_evt_node->start_time, p_end_time) >= 0
    && p_end_time->year != 0)
  {
    return 1;
  }

  return 0;
}

static inline BOOL is_evt_in_given_content(evt_node_t *p_evt_node, u8 content_level)
{
    if (content_level > 0)
    {
      if (((content_level << 4) == 0xF0
        && ((content_level >> 4) == (p_evt_node->cont_level >> 4)))
        || (content_level == p_evt_node->cont_level))
      {
        return TRUE;
      }

      return FALSE;
    }

    return TRUE;
}

static void reset_seg_info(void *p_data, sev_node_t *p_svc_node, ts_node_t *p_ts_node)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  tab_svc_info_t *p_ts_seg_info = NULL;
  seg_info_each_tab_t *p_svc_seg_info = NULL;
  u8 tab_index = 0;

  if (p_svc_node == NULL
    || p_ts_node == NULL)
  {
    return ;
  }

  for (tab_index = 0; tab_index < TABLE_CNT(p_epg_var); tab_index ++)
  {
    p_ts_seg_info = &p_ts_node->p_svc_info_per_tab[tab_index];
    p_svc_seg_info = &p_svc_node->p_seg_info_in_table[tab_index];
    if (p_svc_seg_info->is_all_evt_saved == TRUE)
    {
      p_ts_seg_info->capt_serv_num --;
      p_ts_seg_info->is_sch_full = FALSE;
      p_svc_seg_info->is_all_evt_saved = FALSE;
      memset(p_svc_seg_info->p_capt_seg_info, 0, sizeof(u8) * MAX_SEG_NUM_EACH_TABLE);
    }
  }
}

static void del_event_node(void *p_data, evt_node_t *p_evt_node, sev_node_t *p_svc_node)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  CHECK_FAIL_RET_VOID(p_evt_node != NULL);
  if (p_svc_node->p_recent_evt_node == p_evt_node)
  {
    p_svc_node->p_recent_evt_node = NULL;
  }

  //EPG_PRINT("\ndelete evt id = %d", p_evt_node->event_id);
  if (p_epg_var->evt_info_priority != EPG_EVT_NAME_ONLY)
  {
    //Release memory of short text
    if (p_evt_node->p_sht_txt != NULL)
    {
      //memset(p_evt_node->p_sht_txt,  0, p_evt_node->ext_total_len);
      epg_free_nonfix(&p_epg_var->memp, p_evt_node->p_sht_txt);
      p_evt_node->p_sht_txt = NULL;
    }
    else if (p_evt_node->p_ext_txt != NULL)
    {
      //memset(p_evt_node->p_ext_txt,  0, p_evt_node->ext_total_len);
      epg_free_nonfix(&p_epg_var->memp, p_evt_node->p_ext_txt);
      p_evt_node->p_ext_txt = NULL;
    }
    else if (p_evt_node->p_ext_desc != NULL)
    {
      //memset(p_evt_node->p_ext_desc,  0, p_evt_node->ext_total_len);
      epg_free_nonfix(&p_epg_var->memp, p_evt_node->p_ext_desc);
      p_evt_node->p_ext_desc = NULL;
    }
  }


  //memset(p_evt_node, 0, sizeof(evt_node_t));
  //CHECK_FAIL_RET_VOID(p_evt_node != NULL);
  epg_free_fix(&p_epg_var->evt_node_memf, p_evt_node);
  p_evt_node = NULL;
}

static void del_svc_node(void *p_data, sev_node_t *p_svc_node, ts_node_t *p_ts_node)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  evt_node_t *p_evt_head = NULL;
  //u32 loopi = 1;

  CHECK_FAIL_RET_VOID(p_svc_node != NULL);

  p_evt_head = p_svc_node->p_evt_node_head;
  while (p_evt_head != NULL)
  {
    //Record current event node address for free operation
    p_svc_node->p_evt_node_head = p_evt_head->p_next_evt_node;
    //EPG_PRINT("\n %.3d del evt node id = %d", loopi, p_evt_head->event_id);
    del_event_node(p_data, p_evt_head, p_svc_node);
    p_evt_head = p_svc_node->p_evt_node_head;
    //loopi ++;
  }
  p_svc_node->p_recent_evt_node = NULL;

  //memset(p_svc_node, 0, sizeof(sev_node_t));
  epg_free_fix(&p_epg_var->svc_node_memf, p_svc_node);
  p_svc_node = NULL;
}

static void del_ts_node(void *p_data, ts_node_t *p_ts_node)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  sev_node_t *p_svc_node = NULL;
  //u32 loopi = 1;

  CHECK_FAIL_RET_VOID(p_ts_node != NULL);

  p_svc_node = p_ts_node->p_svc_head;
  while (p_svc_node != NULL)
  {
    p_ts_node->p_svc_head = p_svc_node->p_next_s_node;
    //EPG_PRINT("\n\n%.2d, delete svc node id = %d\n", loopi, p_svc_node->service_id);
    del_svc_node(p_data, p_svc_node, p_ts_node);
    p_svc_node = p_ts_node->p_svc_head;
    //loopi ++;
  }

  //EPG_PRINT("\ndelete ts node id = %d\n\n", p_ts_node->ts_id);
  //memset(p_ts_node, 0, sizeof(ts_node_t));
  epg_free_fix(&p_epg_var->ts_node_memf, p_ts_node);
  p_ts_node = NULL;
}

static void del_net_node(void *p_data, orig_net_node_t *p_net_node)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  ts_node_t *p_ts_node = NULL;

  CHECK_FAIL_RET_VOID(p_net_node != NULL);

  p_ts_node = p_net_node->p_ts_node_head;
  while (p_ts_node != NULL)
  {
    p_net_node->p_ts_node_head = p_ts_node->p_next_ts_node;
    del_ts_node(p_data, p_ts_node);
    p_ts_node = p_net_node->p_ts_node_head;
  }

  //EPG_PRINT("\n##delete net node id = %d\n\n\n", p_net_node->orig_network_id);
  //memset(p_net_node, 0, sizeof(orig_net_node_t));
  epg_free_fix(&p_epg_var->net_node_memf, p_net_node);
  p_net_node = NULL;
}

static void del_svc_filter_evt(void *p_data,
  sev_node_t *p_svc_node,  ts_node_t *p_ts_node, epg_filter_t *p_filter)
{
  evt_node_t *p_evt_node = NULL;
  evt_node_t *p_evt_prv = NULL;
  evt_node_t *p_evt_temp = NULL;
  BOOL b_del_evt = FALSE;
  s8 result = 0;
  u8 index = 0;

  CHECK_FAIL_RET_VOID(p_svc_node != NULL);

  if (p_filter->start_time.day)
  {
    for (index = 0; index < p_svc_node->saved_evt_day; index ++)
    {
      p_evt_node = p_svc_node->pp_evt_day_head[index];
      if (p_evt_node->start_time.day == p_filter->start_time.day)
      {
        break;
      }
    }
  }
  else
  {
    p_evt_node = p_svc_node->p_evt_node_head;
  }

  while (p_evt_node != NULL)
  {
    result = is_evt_in_given_period(p_evt_node,
      &p_filter->start_time, &p_filter->end_time);
    if (result == 0)
    {
      p_evt_temp = p_evt_node;
      if (p_evt_prv == NULL)
      {
        p_svc_node->p_evt_node_head = p_evt_node->p_next_evt_node;
        p_evt_node = p_svc_node->p_evt_node_head;
      }
      else
      {
        p_evt_prv->p_next_evt_node = p_evt_node->p_next_evt_node;
        p_evt_node = p_evt_prv->p_next_evt_node;
      }
      b_del_evt = TRUE;
      del_event_node(p_data, p_evt_temp, p_svc_node);
    }
    else
    {
      p_evt_prv = p_evt_node;
      p_evt_node = p_evt_node->p_next_evt_node;
    }
  }

  if (b_del_evt == TRUE)
  {
    reset_seg_info(p_data, p_svc_node, p_ts_node);
    reset_evt_day_head(p_data, p_svc_node);
  }
}

static void del_ts_filter_evt(void *p_data, ts_node_t *p_ts_node, epg_filter_t *p_filter)
{
  sev_node_t *p_svc_node = NULL;

  CHECK_FAIL_RET_VOID(p_ts_node != NULL);

  p_svc_node = p_ts_node->p_svc_head;
  while (p_svc_node != NULL)
  {
    del_svc_filter_evt(p_data, p_svc_node, p_ts_node, p_filter);
    p_svc_node = p_svc_node->p_next_s_node;
  }
}

static void del_net_filter_evt(void *p_data, orig_net_node_t *p_net_node, epg_filter_t *p_filter)
{
  ts_node_t *p_ts_node = NULL;

  CHECK_FAIL_RET_VOID(p_net_node != NULL);

  p_ts_node = p_net_node->p_ts_node_head;
  while (p_ts_node != NULL)
  {
    del_ts_filter_evt(p_data, p_ts_node, p_filter);
    p_ts_node = p_ts_node->p_next_ts_node;
  }
}

static void del_svc_exp_filter_evt(void *p_data,
  sev_node_t *p_svc_node,  ts_node_t *p_ts_node, epg_filter_t *p_filter)
{
  evt_node_t *p_evt_node = NULL;
  evt_node_t *p_evt_prv = NULL;
  BOOL b_del_evt = FALSE;
  s8 result = 0;

  CHECK_FAIL_RET_VOID(p_svc_node != NULL);

  p_evt_node = p_svc_node->p_evt_node_head;

  while (p_evt_node != NULL)
  {
    result = is_evt_in_given_period(p_evt_node,
      &p_filter->start_time, &p_filter->end_time);
    if (result != 0)
    {
      if (p_evt_prv == NULL)
      {
        p_svc_node->p_evt_node_head = p_evt_node->p_next_evt_node;
      }
      else
      {
        p_evt_prv->p_next_evt_node = p_evt_node->p_next_evt_node;
      }

      b_del_evt = TRUE;
      del_event_node(p_data, p_evt_node, p_svc_node);
      if (p_evt_prv == NULL)
      {
        p_evt_node = p_svc_node->p_evt_node_head;
      }
      else
      {
        p_evt_node = p_evt_prv->p_next_evt_node;
      }
    }
    else
    {
      p_evt_prv = p_evt_node;
      p_evt_node = p_evt_node->p_next_evt_node;
    }
  }

  if (b_del_evt == TRUE)
  {
    reset_seg_info(p_data, p_svc_node, p_ts_node);
    reset_evt_day_head(p_data, p_svc_node);
  }
}

static void del_svc_exp_filter_evt_but_hold_pf(void *p_data,
  sev_node_t *p_svc_node,  ts_node_t *p_ts_node, epg_filter_t *p_filter)
{
  evt_node_t *p_evt_node = NULL;
  evt_node_t *p_evt_prv = NULL;
  BOOL b_del_evt = FALSE;
  s8 result = 0;
  u8 loopi = 0;

  CHECK_FAIL_RET_VOID(p_svc_node != NULL);

  p_evt_node = p_svc_node->p_evt_node_head;

  //hold pf evt
  while (p_evt_node)
  {
    p_evt_node = p_evt_node->p_next_evt_node;
    loopi ++;
    if (loopi >= 2)
    {
      break;
    }
  }

  while (p_evt_node != NULL)
  {
    result = is_evt_in_given_period(p_evt_node,
      &p_filter->start_time, &p_filter->end_time);
    if (result != 0)
    {
      if (p_evt_prv == NULL)
      {
        p_svc_node->p_evt_node_head = p_evt_node->p_next_evt_node;
      }
      else
      {
        p_evt_prv->p_next_evt_node = p_evt_node->p_next_evt_node;
      }

      b_del_evt = TRUE;
      del_event_node(p_data, p_evt_node, p_svc_node);
      if (p_evt_prv == NULL)
      {
        p_evt_node = p_svc_node->p_evt_node_head;
      }
      else
      {
        p_evt_node = p_evt_prv->p_next_evt_node;
      }
    }
    else
    {
      p_evt_prv = p_evt_node;
      p_evt_node = p_evt_node->p_next_evt_node;
    }
  }

  if (b_del_evt == TRUE)
  {
    reset_evt_day_head(p_data, p_svc_node);
  }
}

static void del_ts_exp_filter_evt(void *p_data, ts_node_t *p_ts_node, epg_filter_t *p_filter)
{
  sev_node_t *p_svc_node = NULL;

  CHECK_FAIL_RET_VOID(p_ts_node != NULL);

  p_svc_node = p_ts_node->p_svc_head;
  while (p_svc_node != NULL)
  {
    del_svc_exp_filter_evt(p_data, p_svc_node, p_ts_node, p_filter);
    p_svc_node = p_svc_node->p_next_s_node;
  }
}

static void del_net_exp_filter_evt(void *p_data,
  orig_net_node_t *p_net_node, epg_filter_t *p_filter)
{
  ts_node_t *p_ts_node = NULL;

  CHECK_FAIL_RET_VOID(p_net_node != NULL);

  p_ts_node = p_net_node->p_ts_node_head;
  while (p_ts_node != NULL)
  {
    del_ts_exp_filter_evt(p_data, p_ts_node, p_filter);
    p_ts_node = p_ts_node->p_next_ts_node;
  }
}

static void del_overdue_evt_by_svc_node(void *p_data, sev_node_t *p_svc_node, ts_node_t *p_ts_node)
{
  evt_node_t *p_evt_head = NULL;
  utc_time_t sys_time = {0};
  utc_time_t end_time = {0};

  CHECK_FAIL_RET_VOID(p_svc_node != NULL);

  time_get(&sys_time, TRUE);

  if (p_svc_node->p_evt_node_head != NULL)
  {
    /*!Get first evt node*/
    p_evt_head = p_svc_node->p_evt_node_head;
    memcpy(&end_time, &p_evt_head->start_time, sizeof(utc_time_t));
    evt_tadd(&end_time, &p_evt_head->drt_time);

    /*!Compare end time of system time*/
    if (EVT_TCMP(&end_time, &sys_time) != 1)
    {
      /*!Delete overtime event*/
      p_svc_node->p_evt_node_head = p_evt_head->p_next_evt_node;

      reset_evt_day_head(p_data, p_svc_node);
      del_event_node(p_data, p_evt_head, p_svc_node);
      //reset_seg_info(p_data, p_svc_node, p_ts_node);
    }
  }
}

static void del_overdue_event_by_ts_node(void *p_data, ts_node_t *p_ts_node)
{
  sev_node_t *p_svc_node = NULL;

  CHECK_FAIL_RET_VOID(p_ts_node != NULL);

  p_svc_node = p_ts_node->p_svc_head;

  while (p_svc_node != NULL)
  {
    del_overdue_evt_by_svc_node(p_data, p_svc_node, p_ts_node);
    p_svc_node = p_svc_node->p_next_s_node;
  }
}

static void del_overdue_evt_by_net_node(void *p_data, orig_net_node_t *p_net_node)
{
  ts_node_t *p_ts_node = NULL;

  CHECK_FAIL_RET_VOID(p_net_node != NULL);

  p_ts_node = p_net_node->p_ts_node_head;

  while (p_ts_node != NULL)
  {
    del_overdue_event_by_ts_node(p_data, p_ts_node);
    p_ts_node = p_ts_node->p_next_ts_node;
  }
}

#if 1
BOOL evt_in_time_for_days(evt_node_t *p_evt_node,
              utc_time_t *p_start_day, utc_time_t *p_end_day,
              utc_time_t *p_start_day_start_time, utc_time_t *p_start_day_end_time,
              utc_time_t *p_end_day_end_time)

{
 utc_time_t evt_end_time = {0};
 utc_time_t one_day = {0};
 utc_time_t each_day_start_time = {0};
 utc_time_t each_day_end_time = {0};
 //u32 day = 0;

 memcpy(&evt_end_time, &(p_evt_node->start_time), sizeof(utc_time_t));
 evt_tadd(&evt_end_time, &p_evt_node->drt_time);
 if(EVT_TCMP(&evt_end_time, p_start_day) <= 0)
 return FALSE;
 if(EVT_TCMP(&p_evt_node->start_time, p_end_day) >= 0)
 return FALSE;
 //continue to match....
 memcpy(&each_day_start_time, p_start_day_start_time, sizeof(utc_time_t));
 memcpy(&each_day_end_time, p_start_day_end_time, sizeof(utc_time_t));
 one_day.day = 1;

 while(EVT_TCMP(&each_day_end_time, p_end_day_end_time) <= 0)
  {
    if(EVT_TCMP(&p_evt_node->start_time, &each_day_end_time) >= 0 ||
        EVT_TCMP(&evt_end_time, &each_day_start_time) <= 0)
        {
          evt_tadd(&each_day_start_time, &one_day);
          evt_tadd(&each_day_end_time, &one_day);
          continue;
        }
    #if 1
    EPG_PRINT("!!! %d:%d:%d  %d:%d:%d---%d:%d:%d  %d:%d:%d",
                     //(u8*)p_evt_node->event_name,
                     p_evt_node->start_time.year,
                     p_evt_node->start_time.month, p_evt_node->start_time.day,
                     p_evt_node->start_time.hour, p_evt_node->start_time.minute,
                     p_evt_node->start_time.second, evt_end_time.year, evt_end_time.month,
                     evt_end_time.day, evt_end_time.hour, evt_end_time.minute,
                     evt_end_time.second);
    #endif
    return TRUE;
  }

 return FALSE;
}


u32 epg_create_list(void *p_data, evt_node_t **pp_evt,
          char * p_str, u8 cont_level, utc_time_t *p_start_day,
         utc_time_t *p_end_day, utc_time_t *p_start_time, utc_time_t *p_end_time)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  orig_net_node_t *p_net_node = p_epg_var->p_net_node_head;
  ts_node_t *p_ts_node = NULL;
  sev_node_t *p_svc_node = NULL;
  evt_node_t *p_evt_node = NULL;
  u32 evt_cnt = 0;
  u8 evt_name[17] = {0};

  utc_time_t start_day_start_time = {0};
  utc_time_t start_day_end_time = {0};
  utc_time_t end_day_end_time = {0};
  utc_time_t one_day = {0};

  start_day_start_time.year = p_start_day->year;
  start_day_start_time.month = p_start_day->month;
  start_day_start_time.day = p_start_day->day;
  start_day_start_time.hour = p_start_time->hour;
  start_day_start_time.minute = p_start_time->minute;
  start_day_start_time.second = p_start_time->second;

  start_day_end_time.year = p_start_day->year;
  start_day_end_time.month = p_start_day->month;
  start_day_end_time.day = p_start_day->day;
  start_day_end_time.hour = p_end_time->hour;
  start_day_end_time.minute = p_end_time->minute;
  start_day_end_time.second = p_end_time->second;

  end_day_end_time.year = p_end_day->year;
  end_day_end_time.month = p_end_day->month;
  end_day_end_time.day = p_end_day->day;
  end_day_end_time.hour = p_end_time->hour;
  end_day_end_time.minute = p_end_time->minute;
  end_day_end_time.second = p_end_time->second;

  one_day.day = 1;

  evt_tadd(p_end_day, &one_day);

  while(p_net_node)
    {
      p_ts_node = p_net_node->p_ts_node_head;
      while(p_ts_node)
        {
          p_svc_node = p_ts_node->p_svc_head;
          while(p_svc_node)
            {
              p_evt_node = p_svc_node->p_evt_node_head;
              while(p_evt_node)
                {

                 str_uni2asc((u8 *)evt_name, (u16 *)(p_evt_node->event_name));
                 //match name
                 if(
                    !strstr((char *)evt_name,p_str) ||
                    cont_level != p_evt_node->cont_level ||
                    !evt_in_time_for_days(p_evt_node, p_start_day, p_end_day,
                    &start_day_start_time, &start_day_end_time,&end_day_end_time))
                    {
                      p_evt_node = p_evt_node->p_next_evt_node;
                      continue;
                    }
                  EPG_PRINT(" %s\n", &evt_name);
                  pp_evt[evt_cnt] = p_evt_node;
                  evt_cnt ++;
                 // *(pp_evt++) = p_evt_node;
                  p_evt_node = p_evt_node->p_next_evt_node;
                }
             p_svc_node = p_svc_node->p_next_s_node;
            }
         p_ts_node = p_ts_node->p_next_ts_node;
        }
     p_net_node = p_net_node->p_next_net_node;
    }
return evt_cnt;
}


u32 epg_get_all_evt_num(void *p_data)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  orig_net_node_t *p_net_node = p_epg_var->p_net_node_head;
  ts_node_t *p_ts_node = NULL;
  sev_node_t *p_svc_node = NULL;
  evt_node_t *p_evt_node = NULL;
  u32 evt_cnt = 0;

  while(p_net_node)
    {
      p_ts_node = p_net_node->p_ts_node_head;
      while(p_ts_node)
        {
          p_svc_node = p_ts_node->p_svc_head;
          while(p_svc_node)
            {
              p_evt_node = p_svc_node->p_evt_node_head;
              while(p_evt_node)
                {
                  evt_cnt ++;
                  p_evt_node = p_evt_node->p_next_evt_node;
                }
              p_svc_node = p_svc_node->p_next_s_node;
            }
         p_ts_node = p_ts_node->p_next_ts_node;
        }
     p_net_node = p_net_node->p_next_net_node;
    }
  return evt_cnt;
}


void epg_list_sort_by_time(evt_node_t **pp_evt, u32 item_num)
{
  u32 i = 0;
  u32 j = 0;
  evt_node_t *p_temp = NULL;
  for(j = 0; j < (item_num - 1); j++)
  for(i = 0; i < (item_num - j - 1); i++)
    {
      if(EVT_TCMP(&(pp_evt[i]->start_time), &(pp_evt[i + 1]->start_time)) > 0)
        {
          p_temp = pp_evt[i];
          pp_evt[i] =  pp_evt[i + 1];
          pp_evt[i + 1] = p_temp;
        }
    }
}

void epg_list_sort_by_name(evt_node_t **pp_evt, u32 item_num)
{
  u32 i = 0;
  u32 j = 0;
  evt_node_t *p_temp = NULL;
  u8 evt_name1[17] = {0};
  u8 evt_name2[17] = {0};
  for(j = 0; j < (item_num - 1); j++)
  for(i = 0; i < (item_num - j - 1) ; i++)
    {
      str_uni2asc((u8 *)evt_name1, (u16 *)(pp_evt[i]->event_name));
      str_uni2asc((u8 *)evt_name2, (u16 *)(pp_evt[i + 1]->event_name));
      if(strcmp((char *)evt_name1, (char *)evt_name2) > 0)
        {
          p_temp = pp_evt[i];
          pp_evt[i] =  pp_evt[i + 1];
          pp_evt[i + 1] = p_temp;
        }
    }
}


RET_CODE epg_query_by_nibble(void *p_data,
  epg_query_by_nibble_t *p_query)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  orig_net_node_t *p_net_node = p_epg_var->p_net_node_head;
  ts_node_t *p_ts_node = NULL;
  sev_node_t *p_svc_node = NULL;
  evt_node_t *p_evt_node = NULL;
  u32 evt_cnt = 0;
  u32 i = 0;
  s8 result = 0;
  if(p_query->p_result != NULL)
    {
      mtos_free(p_query->p_result);
      p_query->p_result = NULL;
    }
  while(p_net_node)
   {
    if(p_query->orig_network_id == p_net_node->orig_network_id)
      break;
    p_net_node = p_net_node->p_next_net_node;
   }
  if(!p_net_node)
    return ERR_FAILURE;

  p_ts_node = p_net_node->p_ts_node_head;
  while(p_ts_node)
   {
    if(p_query->stream_id  == p_ts_node->ts_id)
      break;
    p_ts_node = p_ts_node->p_next_ts_node;
   }
  if(!p_ts_node)
    return ERR_FAILURE;

  p_svc_node = p_ts_node->p_svc_head;
  while(p_svc_node)
   {
    if(p_query->service_id  == p_svc_node->service_id)
      break;
    p_svc_node = p_svc_node->p_next_s_node;
   }
  if(!p_svc_node)
    return ERR_FAILURE;

  p_evt_node = p_svc_node->p_evt_node_head;
  while(p_evt_node)
   {
    result = is_evt_in_given_period(p_evt_node, &p_query->start_time, &p_query->end_time);
    #if 1
    if(((p_query->cont_level  == p_evt_node->cont_level)
      || ((p_query->cont_level & 0xF0) == (p_evt_node->cont_level & 0xF0)
          && ((p_query->cont_level & 0x0F)==0x0F))) && (result == 0))
    #else
    if(p_query->cont_level  == p_evt_node->cont_level)
    #endif
    {
      evt_cnt ++;
    }
    p_evt_node = p_evt_node->p_next_evt_node;
   }
   if(evt_cnt <= 0)
   {
   return ERR_FAILURE;
   }
  p_query->p_result = mtos_malloc(evt_cnt * sizeof(void *));
  if(p_query->p_result == NULL)
    return ERR_FAILURE;
  p_query->cnt = evt_cnt;
  p_evt_node = p_svc_node->p_evt_node_head;
  while(p_evt_node)
   {
    if(i == evt_cnt)   //this shouldn't happen
      break;
    result = is_evt_in_given_period(p_evt_node, &p_query->start_time, &p_query->end_time);
    #if 1
    if(((p_query->cont_level  == p_evt_node->cont_level)
      || ((p_query->cont_level & 0xF0)==(p_evt_node->cont_level & 0xF0)
        && ((p_query->cont_level & 0x0F) == 0x0F))) && (result == 0))
    #else
    if(p_query->cont_level  == p_evt_node->cont_level)
    #endif
      p_query->p_result[i ++] = (void *)p_evt_node;
    p_evt_node = p_evt_node->p_next_evt_node;
   }
  return SUCCESS;
}

RET_CODE epg_query_by_nibble_list(void *p_data,
  epg_query_by_nibble_t *p_query, u32 query_cnt)
{
  u32 i = 0;
  RET_CODE ret = ERR_FAILURE;
  for(i = 0; i < query_cnt; i++)
    {
      epg_query_by_nibble(p_data, &p_query[i]);
      if(p_query[i].cnt > 0)
        ret = SUCCESS;
    }
  return ret;
}
#endif




static BOOL del_all_node(void *p_data)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  orig_net_node_t *p_net_node = NULL;

  while (p_epg_var->p_net_node_head != NULL)
  {
    p_net_node = p_epg_var->p_net_node_head;
    p_epg_var->p_net_node_head = p_net_node->p_next_net_node;
    del_net_node(p_data, p_net_node);
  }
  EPG_PRINT("\n##debug: del_all_node!\n");

  return TRUE;
}

static BOOL del_overdue_evt_node(void *p_data)
{
  orig_net_node_t *p_net_node = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  p_net_node = p_epg_var->p_net_node_head;
  while (p_net_node != NULL)
  {
    del_overdue_evt_by_net_node(p_data, p_net_node);
    p_net_node = p_net_node->p_next_net_node;
  }

  return TRUE;
}

static BOOL del_exp_filter(void *p_data, epg_filter_t *p_filter)
{
  orig_net_node_t *p_net_node = NULL;
  orig_net_node_t *p_net_temp = NULL;
  orig_net_node_t *p_net_head = NULL;
  ts_node_t *p_ts_node = NULL;
  ts_node_t *p_ts_temp = NULL;
  ts_node_t *p_ts_head = NULL;
  sev_node_t *p_svc_node = NULL;
  sev_node_t *p_svc_temp = NULL;
  sev_node_t *p_svc_head = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  BOOL b_delete = FALSE;

  if (p_filter == NULL)
  {
    return FALSE;
  }
  EPG_PRINT("\n##debug: del_exp_filter!\n");

  //delete exp time
  if (p_filter->orig_network_id == IGNORE_ID
    && p_filter->start_time.year != 0)
  {
    p_net_node = p_epg_var->p_net_node_head;
    while (p_net_node != NULL)
    {
      p_ts_node = p_net_node->p_ts_node_head;
      while (p_ts_node != NULL)
      {
        p_svc_node = p_ts_node->p_svc_head;
        while (p_svc_node != NULL)
        {
          del_svc_exp_filter_evt(p_data, p_svc_node, p_ts_node, p_filter);
          p_svc_node = p_svc_node->p_next_s_node;
        }
      }
    }
    return TRUE;
  }

  //del all net except net id
  if (p_filter->orig_network_id != IGNORE_ID)
  {
    p_net_node = p_epg_var->p_net_node_head;
    while (p_net_node != NULL)
    {
      p_net_temp = p_net_node;
      p_net_node = p_net_node->p_next_net_node;

      if (p_net_temp->orig_network_id != p_filter->orig_network_id)
      {
        del_net_node(p_data, p_net_temp);
        b_delete = TRUE;
      }
      else
      {
        p_net_head = p_net_temp;
      }
    }

    p_epg_var->p_net_node_head = p_net_head;
    p_net_node = p_epg_var->p_net_node_head;
    if (p_net_node != NULL)
    {
      p_net_node->p_next_net_node = NULL;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    return FALSE;
  }

  //del all ts exp ts id
  if (p_filter->stream_id != IGNORE_ID)
  {
    p_ts_node = p_net_node->p_ts_node_head;
    while (p_ts_node != NULL)
    {
      p_ts_temp = p_ts_node;
      p_ts_node = p_ts_node->p_next_ts_node;
      if (p_ts_temp->ts_id == p_filter->stream_id)
      {
        p_ts_head = p_ts_temp;
      }
      else
      {
        del_ts_node(p_data, p_ts_temp);
        b_delete = TRUE;
      }
    }

    p_net_node->p_ts_node_head = p_ts_head;
    p_ts_node = p_net_node->p_ts_node_head;
    if (p_ts_node != NULL)
    {
      p_ts_node->p_next_ts_node = NULL;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    if (p_filter->start_time.year != 0)
    {
      del_net_exp_filter_evt(p_data, p_net_node, p_filter);
      b_delete = TRUE;
    }

    return b_delete;
  }

  //del all svc exp svc id
  if (p_filter->service_id != IGNORE_ID)
  {
    p_svc_node = p_ts_node->p_svc_head;
    while (p_svc_node != NULL)
    {
      p_svc_temp = p_svc_node;
      p_svc_node = p_svc_node->p_next_s_node;
      if (p_svc_temp->service_id != p_filter->service_id)
      {
        del_svc_node(p_data, p_svc_temp, p_ts_node);
      }
      else
      {
        p_svc_head = p_svc_temp;
      }
    }

    p_ts_node->p_svc_head = p_svc_head;
    p_svc_node = p_ts_node->p_svc_head;
    if (p_svc_node != NULL)
    {
      p_svc_node->p_next_s_node = NULL;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    if (p_filter->start_time.year != 0)
    {
      del_ts_exp_filter_evt(p_data, p_ts_node, p_filter);
    }

    return TRUE;
  }

  if (p_filter->start_time.year != 0)
  {
    del_svc_exp_filter_evt(p_data, p_svc_node, p_ts_node, p_filter);
  }

  return TRUE;
}

static BOOL is_net_in_dy_policy(void *p_data, u16 orig_network_id)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  BOOL ret = FALSE;
  u16 index = 0;

  for (index = 0; index < p_epg_var->dy_policy_cnt; index ++)
  {
    if (p_epg_var->p_dy_policy_intra[index].cur_net_id == orig_network_id)
    {
      ret = TRUE;
      break;
    }
  }

  return ret;
}

static BOOL is_ts_in_dy_policy(void *p_data, u16 ts_id)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  BOOL ret = FALSE;
  u16 index = 0;

  for (index = 0; index < p_epg_var->dy_policy_cnt; index ++)
  {
    if (p_epg_var->p_dy_policy_intra[index].cur_ts_id == ts_id)
    {
      ret = TRUE;
      break;
    }
  }

  return ret;
}

static BOOL is_svc_in_dy_policy(void *p_data, u16 svc_id)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  BOOL ret = FALSE;
  u16 index = 0;

  for (index = 0; index < p_epg_var->dy_policy_cnt; index ++)
  {
    if (p_epg_var->p_dy_policy_intra[index].cur_svc_id == svc_id)
    {
      ret = TRUE;
      break;
    }
  }

  return ret;
}

static BOOL del_exp_dy_policy(void *p_data)
{
  orig_net_node_t *p_net_node = NULL;
  orig_net_node_t *p_net_temp = NULL;
  orig_net_node_t *p_net_prev = NULL;
  ts_node_t *p_ts_node = NULL;
  ts_node_t *p_ts_temp = NULL;
  ts_node_t *p_ts_prev = NULL;
  sev_node_t *p_svc_node = NULL;
  sev_node_t *p_svc_temp = NULL;
  sev_node_t *p_svc_prev = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  epg_filter_t del_filter;

  if (p_epg_var->p_dy_policy_intra == NULL)
  {
    return FALSE;
  }

  //del net exp dynamic policy
  p_net_node = p_epg_var->p_net_node_head;
  p_net_prev = NULL;
  memset(&del_filter, 0, sizeof(epg_filter_t));

  memcpy(&del_filter.start_time, &p_epg_var->p_dy_policy_intra[0].start, sizeof(utc_time_t));
  memcpy(&del_filter.end_time, &p_epg_var->p_dy_policy_intra[0].end, sizeof(utc_time_t));
  while (p_net_node != NULL)
  {
    p_net_temp = p_net_node;
    p_net_node = p_net_node->p_next_net_node;
    if (is_net_in_dy_policy(p_data, p_net_temp->orig_network_id) == FALSE)
    {
      del_net_node(p_data, p_net_temp);
      if (p_net_prev == NULL)
      {
        p_epg_var->p_net_node_head = p_net_node;
      }
      else
      {
        p_net_prev->p_next_net_node = p_net_node;
      }
    }
    else
    {
      p_net_prev = p_net_temp;
    }
  }

  //del ts exp dynamic policy
  p_net_node = p_epg_var->p_net_node_head;
  while (p_net_node != NULL)
  {
    p_ts_node = p_net_node->p_ts_node_head;
    p_ts_prev = NULL;
    while (p_ts_node != NULL)
    {
      p_ts_temp = p_ts_node;
      p_ts_node = p_ts_node->p_next_ts_node;
      if (is_ts_in_dy_policy(p_data, p_ts_temp->ts_id) == FALSE)
      {
        del_ts_node(p_data, p_ts_temp);
        if (p_ts_prev == NULL)
        {
          p_net_node->p_ts_node_head = p_ts_node;
        }
        else
        {
          p_ts_prev->p_next_ts_node = p_ts_node;
        }
      }
      else
      {
        p_ts_prev = p_ts_temp;
      }
    }
    p_net_node = p_net_node->p_next_net_node;
  }

  //del svc exp dynamic policy
  p_net_node = p_epg_var->p_net_node_head;
  while (p_net_node != NULL)
  {
    p_ts_node = p_net_node->p_ts_node_head;
    while (p_ts_node != NULL)
    {
      p_svc_node = p_ts_node->p_svc_head;
      p_svc_prev = NULL;

      while (p_svc_node != NULL)
      {
        p_svc_temp = p_svc_node;
        p_svc_node = p_svc_node->p_next_s_node;
        if (is_svc_in_dy_policy(p_data, p_svc_temp->service_id) == FALSE)
        {
          del_svc_node(p_data, p_svc_temp, p_ts_node);
          if (p_svc_prev == NULL)
          {
            p_ts_node->p_svc_head = p_svc_node;
          }
          else
          {
            p_svc_prev->p_next_s_node = p_svc_node;
          }
        }
        else
        {
          p_svc_prev = p_svc_temp;
          //del svc evt exp start-end time
          del_svc_exp_filter_evt_but_hold_pf(p_data, p_svc_temp, p_ts_node, &del_filter);
        }
      }

      p_ts_node = p_ts_node->p_next_ts_node;
    }
    p_net_node = p_net_node->p_next_net_node;
  }

  return TRUE;
}

static BOOL del_by_filter(void *p_data, epg_filter_t *p_filter)
{
  orig_net_node_t *p_net_node = NULL;
  orig_net_node_t *p_net_prv = NULL;
  ts_node_t *p_ts_node = NULL;
  ts_node_t *p_ts_prv = NULL;
  sev_node_t *p_svc_node = NULL;
  sev_node_t *p_svc_prv = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  BOOL found = FALSE;

  if (p_filter == NULL)
  {
    return FALSE;
  }

  if (p_filter->orig_network_id == IGNORE_ID
    && p_filter->stream_id == IGNORE_ID
    && p_filter->service_id != IGNORE_ID)
  {
    p_net_node =p_epg_var->p_net_node_head;
    while (p_net_node != NULL)
    {
      p_ts_node = p_net_node->p_ts_node_head;
      while (p_ts_node != NULL)
      {
        p_svc_node = p_ts_node->p_svc_head;
        p_svc_prv = NULL;
        while (p_svc_node != NULL)
        {
          if (p_svc_node->service_id == p_filter->service_id)
          {
            //reset_seg_info(p_data, p_svc_node, p_ts_node);
            if (p_filter->start_time.year == 0)
            {
              if (p_svc_prv == NULL)
              {
                p_ts_node->p_svc_head = p_svc_node->p_next_s_node;
              }
              else
              {
                p_svc_prv->p_next_s_node = p_svc_node->p_next_s_node;
              }
              del_svc_node(p_data, p_svc_node, p_ts_node);
            }
            else
            {
              del_svc_filter_evt(p_data, p_svc_node, p_ts_node, p_filter);
            }

            return TRUE;
          }

          p_svc_prv = p_svc_node;
          p_svc_node = p_svc_node->p_next_s_node;
        }
        p_ts_node = p_ts_node->p_next_ts_node;
      }
      p_net_node = p_net_node->p_next_net_node;
    }
    return FALSE;
  }
  else if (p_filter->orig_network_id == IGNORE_ID
    && p_filter->stream_id != IGNORE_ID)
  {
    p_net_node =p_epg_var->p_net_node_head;
    while (p_net_node != NULL)
    {
      p_ts_node = p_net_node->p_ts_node_head;
      p_ts_prv = NULL;
      while (p_ts_node != NULL)
      {
        if (p_ts_node->ts_id == p_filter->stream_id)
        {
          found = TRUE;
          break;
        }
        p_ts_prv = p_ts_node;
        p_ts_node = p_ts_node->p_next_ts_node;
      }

      if (found == TRUE)
      {
        break;
      }
      p_net_node = p_net_node->p_next_net_node;
    }

    if (p_ts_node == NULL)
    {
      return FALSE;
    }

    if (p_filter->service_id != IGNORE_ID)
    {
      p_svc_node = p_ts_node->p_svc_head;
      p_svc_prv = NULL;
      while (p_svc_node != NULL)
      {
        if (p_svc_node->service_id == p_filter->service_id)
        {
          //reset_seg_info(p_data, p_svc_node, p_ts_node);
          if (p_filter->start_time.year == 0)
          {
            if (p_svc_prv == NULL)
            {
              p_ts_node->p_svc_head = p_svc_node->p_next_s_node;
            }
            else
            {
              p_svc_prv->p_next_s_node = p_svc_node->p_next_s_node;
            }
            del_svc_node(p_data, p_svc_node, p_ts_node);
          }
          else
          {
            del_svc_filter_evt(p_data, p_svc_node, p_ts_node, p_filter);
          }

          return TRUE;
        }

        p_svc_prv = p_svc_node;
        p_svc_node = p_svc_node->p_next_s_node;
      }
    }
    else
    {
      if (p_filter->start_time.year == 0)
      {
        if (p_ts_prv == NULL)
        {
          p_net_node->p_ts_node_head = p_ts_node->p_next_ts_node;
        }
        else
        {
          p_ts_prv->p_next_ts_node = p_ts_node->p_next_ts_node;
        }
        del_ts_node(p_data, p_ts_node);
      }
      else
      {
        del_ts_filter_evt(p_data, p_ts_node, p_filter);
      }
    }
  }
  //del net node by filter net id
  else if (p_filter->orig_network_id != IGNORE_ID
    && p_filter->stream_id == IGNORE_ID)
  {
    p_net_node = p_epg_var->p_net_node_head;
    p_net_prv = NULL;
    while (p_net_node != NULL)
    {
      if (p_net_node->orig_network_id == p_filter->orig_network_id)
      {
        break;
      }

      p_net_prv = p_net_node;
      p_net_node = p_net_node->p_next_net_node;
    }

    if (p_net_node == NULL)
    {
      return FALSE;
    }

    if (p_filter->start_time.year == 0)
    {
      if (p_net_prv == NULL)
      {
        p_epg_var->p_net_node_head = p_net_node->p_next_net_node;
      }
      else
      {
        p_net_prv->p_next_net_node = p_net_node->p_next_net_node;
      }

      del_net_node(p_data, p_net_node);
    }
    else
    {
      del_net_filter_evt(p_data, p_net_node, p_filter);
    }
  }
  //del ts node by filter ts id
  else if (p_filter->stream_id != IGNORE_ID
    && p_filter->service_id == IGNORE_ID)
  {
    p_net_node = get_net_node(p_data, p_filter->orig_network_id);
    if (p_net_node != NULL)
    {
      p_ts_node = p_net_node->p_ts_node_head;
      p_ts_prv = NULL;
      while (p_ts_node != NULL)
      {
        if (p_ts_node->ts_id == p_filter->stream_id)
        {
          break;
        }

        p_ts_prv = p_ts_node;
        p_ts_node = p_ts_node->p_next_ts_node;
      }
    }

    if (p_ts_node == NULL)
    {
      return FALSE;
    }

    if (p_filter->start_time.year == 0)
    {
      if (p_ts_prv == NULL)
      {
        p_net_node->p_ts_node_head = p_ts_node->p_next_ts_node;
      }
      else
      {
        p_ts_prv->p_next_ts_node = p_ts_node->p_next_ts_node;
      }

      del_ts_node(p_data, p_ts_node);
    }
    else
    {
      del_ts_filter_evt(p_data, p_ts_node, p_filter);
    }
  }
  //del svc node by filter svc id
  else if (p_filter->service_id != IGNORE_ID)
  {
    p_ts_node = get_ts_node(p_data, p_filter->stream_id, p_filter->orig_network_id);

    if (p_ts_node != NULL)
    {
      p_svc_node = p_ts_node->p_svc_head;
      p_svc_prv = NULL;
      while (p_svc_node != NULL)
      {
        if (p_svc_node->service_id == p_filter->service_id)
        {
          break;
        }

        p_svc_prv = p_svc_node;
        p_svc_node = p_svc_node->p_next_s_node;
      }
    }

    if (p_svc_node == NULL)
    {
      return FALSE;
    }

    //reset_seg_info(p_data, p_svc_node, p_ts_node);

    if (p_filter->start_time.year == 0)
    {
      if (p_svc_prv == NULL)
      {
        p_ts_node->p_svc_head = p_svc_node->p_next_s_node;
      }
      else
      {
        p_svc_prv->p_next_s_node = p_svc_node->p_next_s_node;
      }

      del_svc_node(p_data, p_svc_node, p_ts_node);
    }
    else
    {
      del_svc_filter_evt(p_data, p_svc_node, p_ts_node, p_filter);
    }
  }

  return TRUE;
}

void epg_data_init4(void)
{
  epg_db_t *p_epg_var = NULL;

  p_epg_var = class_get_handle_by_id(EPG_CLASS_ID);
  if (p_epg_var == NULL)
  {
    //Create EPG db handle
    p_epg_var = mtos_malloc(sizeof(epg_db_t));
    memset(p_epg_var, 0, sizeof(epg_db_t));
    CHECK_FAIL_RET_VOID(p_epg_var != NULL);
    //Register handle
    class_register(EPG_CLASS_ID, p_epg_var);
  }
  epg_data_init_lang_code(p_epg_var);

  p_epg_var->p_net_node_head = NULL;
}

void epg_data_deinit(void *p_data)
{
  /*epg_db_t *p_epg_var = (epg_db_t *)p_data;
  CHECK_FAIL_RET_VOID(p_epg_var != NULL);

  //epg_data_mem_release(p_data);
  OS_PRINTF("epg_data_deinit\n");*/
}

BOOL epg_data_mem_alloc(void *p_data, epg_db_policy_t *p_epg_db_policy,
                        u16 max_net, u16 max_ts, u16 max_svc, u16 max_evt)
{
  u32 net_node_cnt = 0;
  u32 ts_node_cnt = 0;
  u32 svc_node_cnt = 0;
  u32 evt_node_cnt = 0;
  //u32 fix_mem_size = 0;
  u32 non_fix_size = 0;
  u32 net_node_size = 0;
  u32 ts_node_size = 0;
  u32 svc_node_size = 0;
  u32 evt_node_size = 0;
  RET_CODE ret_mem = SUCCESS;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  u8 *p_buffer = NULL;

  if (p_epg_var == NULL || p_epg_db_policy == NULL)
  {
    return FALSE;
  }

  p_epg_var->evt_priority = p_epg_db_policy->evt_priority;
  p_epg_var->evt_info_priority = p_epg_db_policy->evt_info_priority;
  p_epg_var->evt_max_day = p_epg_db_policy->evt_max_day;
  p_buffer = (u8 *)p_epg_db_policy->p_mem_addr;
  CHECK_FAIL_RET_FALSE(p_buffer != NULL);
  //memset(p_buffer, 0, p_epg_db_policy->mem_size);

  net_node_size = sizeof(orig_net_node_t);
  ts_node_size = sizeof(ts_node_t);
  if (p_epg_var->evt_priority != EPG_EVT_OF_CUR_SVCS_ONLY)
  {
    ts_node_size += sizeof(tab_svc_info_t) * TABLE_CNT(p_epg_var);
  }
  svc_node_size = sizeof(sev_node_t);
  if (p_epg_var->evt_priority != EPG_EVT_OF_CUR_SVCS_ONLY)
  {
    svc_node_size += sizeof(seg_info_each_tab_t) * TABLE_CNT(p_epg_var);
  }
  svc_node_size += sizeof(evt_node_t *) * p_epg_var->evt_max_day;
  evt_node_size = sizeof(evt_node_t);
  //EPG_PRINT("\n##debug: evt_node_size = %d\n", evt_node_size);
  #if 0
  switch (p_epg_var->evt_priority)
  {
  case EPG_EVT_OF_CUR_TS_ONLY:
    {
      net_node_cnt = MIN_NET_NODE_CNT;
      ts_node_cnt = MIN_TS_NODE_CNT1;
      svc_node_cnt = MIN_SVC_NODE_CNT1;
      evt_node_cnt = MIN_EVT_NODE_CNT1 * p_epg_var->evt_max_day;
    }
    break;

  case EPG_EVT_OF_CUR_SVCS_ONLY:
    {
      net_node_cnt = MIN_NET_NODE_CNT;
      ts_node_cnt = MIN_TS_NODE_CNT2;
      svc_node_cnt = MIN_SVC_NODE_CNT2;
      evt_node_cnt = MIN_EVT_NODE_CNT2 * p_epg_var->evt_max_day;
    }
    break;

  default:
    {
      net_node_cnt = MIN_NET_NODE_CNT;
      ts_node_cnt = MIN_TS_NODE_CNT;
      svc_node_cnt = MIN_SVC_NODE_CNT;
      evt_node_cnt = MIN_EVT_NODE_CNT * p_epg_var->evt_max_day;
    }
    break;
  }

  fix_mem_size = net_node_cnt * net_node_size
    + ts_node_cnt * ts_node_size
    + svc_node_cnt * svc_node_size
    + evt_node_cnt * evt_node_size;

  switch (p_epg_var->evt_info_priority)
  {
  case EPG_EVT_NAME_ONLY:
    {
      net_node_cnt = p_epg_db_policy->mem_size / fix_mem_size;
      fix_mem_size = net_node_cnt * fix_mem_size;
      non_fix_size = p_epg_db_policy->mem_size - fix_mem_size;
      ts_node_cnt *= net_node_cnt;
      svc_node_cnt *= net_node_cnt;
      evt_node_cnt *= net_node_cnt;
      evt_node_cnt += non_fix_size / evt_node_size;
      non_fix_size = 0;
    }
    break;
  case EPG_EVT_NAME_AND_SHORT:
    {
      non_fix_size = p_epg_db_policy->mem_size * FIX_MEM_PER_TOTAL / 100;
      net_node_cnt = non_fix_size / fix_mem_size;
      ts_node_cnt *= net_node_cnt;
      svc_node_cnt *= net_node_cnt;
      evt_node_cnt *= net_node_cnt;
      fix_mem_size = net_node_cnt * fix_mem_size;
      non_fix_size = non_fix_size - fix_mem_size;
      //evt_node_cnt += (non_fix_size / 2) / evt_node_size;
      //non_fix_size /= 2;
      non_fix_size += p_epg_db_policy->mem_size * (100 - FIX_MEM_PER_TOTAL) / 100;
    }
    break;
  default:
    {
      non_fix_size = p_epg_db_policy->mem_size * FIX_MEM_PER_TOTAL1 / 100;
      net_node_cnt = non_fix_size / fix_mem_size;
      ts_node_cnt *= net_node_cnt;
      svc_node_cnt *= net_node_cnt;
      evt_node_cnt *= net_node_cnt;
      fix_mem_size = net_node_cnt * fix_mem_size;
      non_fix_size = non_fix_size - fix_mem_size;
      //evt_node_cnt += (non_fix_size / 2) / evt_node_size;
      //non_fix_size /= 2;
      non_fix_size += p_epg_db_policy->mem_size * (100 - FIX_MEM_PER_TOTAL) / 100;
    }
    break;
  }

  if (net_node_cnt == 0)
  {
    //return FALSE;
  }
  net_node_cnt *= MIN_NET_NODE_CNT;
#endif
#if 1//wubin add temporarily
  net_node_cnt = max_net;
  ts_node_cnt = max_ts;
  svc_node_cnt = max_svc;
  evt_node_cnt = max_evt;
  non_fix_size = p_epg_db_policy->mem_size - net_node_cnt * net_node_size -
                 ts_node_cnt * ts_node_size - svc_node_cnt * svc_node_size -
                 evt_node_cnt * evt_node_size;
#endif//add end

  p_epg_var->db_report.net_node_total = net_node_cnt;
  p_epg_var->db_report.ts_node_total = ts_node_cnt;
  p_epg_var->db_report.svc_node_total = svc_node_cnt;
  p_epg_var->db_report.evt_node_total = evt_node_cnt;
  p_epg_var->db_report.nonfix_total = non_fix_size;

  ret_mem = lib_memf_create(&p_epg_var->net_node_memf, (u32)p_buffer,
      net_node_cnt * net_node_size, net_node_size);
  CHECK_FAIL_RET_FALSE(ret_mem == SUCCESS);
  p_buffer += net_node_cnt * net_node_size;

  //Create ts node memf
  ret_mem = lib_memf_create(&p_epg_var->ts_node_memf, (u32)p_buffer,
      ts_node_cnt * ts_node_size, ts_node_size);
  CHECK_FAIL_RET_FALSE(ret_mem == SUCCESS);
  p_buffer += ts_node_cnt * ts_node_size;

  //Create service node memf
  ret_mem = lib_memf_create(&p_epg_var->svc_node_memf, (u32)p_buffer,
      svc_node_cnt * svc_node_size, svc_node_size);
  CHECK_FAIL_RET_FALSE(ret_mem == SUCCESS);
  p_buffer += svc_node_cnt * svc_node_size;

  //Create event node memf
  //Create event head memf
  ret_mem = lib_memf_create(&p_epg_var->evt_node_memf, (u32)p_buffer,
      evt_node_cnt * evt_node_size, evt_node_size);
  CHECK_FAIL_RET_FALSE(ret_mem == SUCCESS);
  p_buffer += evt_node_cnt * evt_node_size;

  //Set non-fixed partition for saving txt and desc information
  if (non_fix_size > 0)
  {
    ret_mem = lib_memp_create(&p_epg_var->memp, (u32)p_buffer, non_fix_size);
    CHECK_FAIL_RET_FALSE(ret_mem == SUCCESS);
  }

  return TRUE;
}

void epg_data_mem_release(void *p_data)
{
  RET_CODE ret = SUCCESS;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  if (p_epg_var == NULL)
  {
    return ;
  }
  //EPG_PRINT("\n##debug: epg_data_mem_release!!!!\n");
  p_epg_var->p_net_node_head = NULL;

  ret = lib_memf_destroy(&p_epg_var->net_node_memf);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  ret = lib_memf_destroy(&p_epg_var->ts_node_memf);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  ret = lib_memf_destroy(&p_epg_var->svc_node_memf);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  ret = lib_memf_destroy(&p_epg_var->evt_node_memf);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  if (p_epg_var->evt_info_priority != EPG_EVT_NAME_ONLY)
  {
    ret = lib_memp_destroy(&p_epg_var->memp);
    CHECK_FAIL_RET_VOID(ret == SUCCESS);
  }
}

void epg_data_init_dy_policy(void *p_data, u16 dy_policy_cnt)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  CHECK_FAIL_RET_VOID(p_epg_var != NULL);

  if (dy_policy_cnt == 0)
  {
    return ;
  }
    p_epg_var->dy_policy_cnt = dy_policy_cnt;
    p_epg_var->p_dy_policy = mtos_malloc(sizeof(epg_dy_policy_t) * dy_policy_cnt);
    memset(p_epg_var->p_dy_policy, 0, sizeof(epg_dy_policy_t) * dy_policy_cnt);
    CHECK_FAIL_RET_VOID(p_epg_var->p_dy_policy != NULL);

    p_epg_var->p_dy_policy_intra = mtos_malloc(sizeof(epg_dy_policy_t) * dy_policy_cnt);
    memset(p_epg_var->p_dy_policy_intra, 0, sizeof(epg_dy_policy_t) * dy_policy_cnt);
    CHECK_FAIL_RET_VOID(p_epg_var->p_dy_policy_intra != NULL);
 }

void epg_data_set_dy_policy(void *p_data, epg_dy_policy_t *p_dy_policy)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  CHECK_FAIL_RET_VOID(p_epg_var != NULL);

  if (p_epg_var->p_dy_policy == NULL)
  {
    return ;
  }

  if (p_dy_policy && (p_epg_var->p_dy_policy != p_dy_policy))
  {
    memcpy(p_epg_var->p_dy_policy, p_dy_policy,
           sizeof(epg_dy_policy_t) * p_epg_var->dy_policy_cnt);
  }

  if (memcmp(p_epg_var->p_dy_policy, p_epg_var->p_dy_policy_intra,
    sizeof(epg_dy_policy_t) * p_epg_var->dy_policy_cnt))
  {
    p_epg_var->p_dy_policy->reserved = 1;
  }
  else
  {
    p_epg_var->p_dy_policy->reserved = 0;
  }
}

epg_dy_policy_t *epg_data_get_dy_policy(void *p_data)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  CHECK_FAIL_RET_NULL(p_epg_var != NULL);

  return p_epg_var->p_dy_policy;
}

epg_db_status_t epg_data_add(void *p_data, eit_t *p_eit_info,
                                                      BOOL *p_pf_new_ver)
{
  sev_node_t *p_svc_node = NULL;
  ts_node_t *p_ts_node = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  epg_db_status_t db_status = EPG_DB_NORM;
  evt_parents_node_t evt_parent = {0};
  //evt_node_t *p_evt_head = NULL;
  u8 tab_cnt = 0;
  seg_info_each_tab_t *p_seg_info = NULL;
  u32 cur_seg = p_eit_info->section_number / SEC_NUM_PER_SEG;
  u8 sec_num = p_eit_info->seg_last_sec_number - p_eit_info->section_number;

  CHECK_FAIL_RET((p_epg_var != NULL), EPG_DB_UNKNOWN_ERROR);
  CHECK_FAIL_RET((p_eit_info != NULL), EPG_DB_UNKNOWN_ERROR);
  if (p_epg_var->p_dy_policy && p_epg_var->p_dy_policy->reserved)
  {
    mtos_task_lock();
    p_epg_var->p_dy_policy->reserved = 0;
    memcpy(p_epg_var->p_dy_policy_intra, p_epg_var->p_dy_policy,
      sizeof(epg_dy_policy_t) * p_epg_var->dy_policy_cnt);
    mtos_task_unlock();
  }

  if (!need_save_eit_info(p_data, p_eit_info))
  {
    return EPG_DB_NORM;
  }

  //add evt parents node
  db_status = add_evt_parents_node(p_data, p_eit_info, &evt_parent);

  if (db_status == EPG_DB_OVERFLOW)
  {
    return EPG_DB_OVERFLOW;
  }

  p_svc_node = evt_parent.p_svc_node;
  p_ts_node  = evt_parent.p_ts_node;

#if 0
  //del old version evt when new version found
  if (p_svc_node->version != p_eit_info->version)
  {
    if (p_epg_var->evt_priority != EPG_EVT_OF_CUR_SVCS_ONLY)
    {
      reset_seg_info(p_data, p_svc_node, p_ts_node);
    }

    p_evt_head = p_svc_node->p_evt_node_head;
    while (p_evt_head != NULL)
    {
      p_svc_node->p_evt_node_head = p_evt_head->p_next_evt_node;
      del_event_node(p_data, p_evt_head, p_svc_node);
      p_evt_head = p_svc_node->p_evt_node_head;
    }
    p_svc_node->p_recent_evt_node = NULL;
    p_svc_node->new_ver_fnd = TRUE;
    p_svc_node->version = p_eit_info->version;
  }
#endif
  if(p_eit_info->table_id == DVB_TABLE_ID_EIT_ACTUAL
      && p_eit_info->section_number == 0)
    {
      if(p_svc_node->section_present_got == TRUE)
        {
          if(p_svc_node->new_ver_to_notify != TRUE
               && p_svc_node->version != p_eit_info->version)
            {
              p_svc_node->version = p_eit_info->version;
              *p_pf_new_ver = TRUE;
            }
          if(p_svc_node->new_ver_to_notify == TRUE)
            {
              *p_pf_new_ver = TRUE;
              p_svc_node->new_ver_to_notify = FALSE;
            }
        }
      else
        {
          p_svc_node->version = p_eit_info->version;
          *p_pf_new_ver = TRUE;
          p_svc_node->new_ver_to_notify = FALSE;
          p_svc_node->section_present_got = TRUE;
        }
    }


  if (p_epg_var->evt_priority == EPG_EVT_OF_CUR_SVCS_ONLY)
  {
    db_status = add_evt_node_to_svc(p_data, p_eit_info, p_svc_node);
    return db_status;
  }

  //eit info already saved
  for (tab_cnt = 0; tab_cnt < TABLE_CNT(p_epg_var); tab_cnt ++)
  {
    p_seg_info = &p_svc_node->p_seg_info_in_table[tab_cnt];
    if (p_seg_info->tab_id == p_eit_info->table_id
      && (p_seg_info->p_capt_seg_info[cur_seg] & (1 << sec_num)))
    {
      //EPG_PRINT("\n##debug: eit info already saved!table id = 0x%x, %d, %d, svc id = %d\n",
        //p_eit_info->table_id, cur_seg, sec_num, p_eit_info->svc_id);
      return EPG_DB_NORM;
    }
  }

  //add evt to svc
  db_status = add_evt_node_to_svc(p_data, p_eit_info, p_svc_node);

  if (db_status == EPG_DB_OVERFLOW)
  {
    return EPG_DB_OVERFLOW;
  }

  //Check segment information or PF information
  if (p_eit_info->table_id >= DVB_TABLE_ID_EIT_SCH_ACTUAL)
  {
    if (save_svc_seg_info(p_data, p_eit_info, p_svc_node, p_ts_node) == TRUE)
    {
      return EPG_DB_ONE_TAB_READY;
    }
  }
  else
  {
    if (save_svc_pf_info(p_eit_info, p_svc_node, p_ts_node) == TRUE)
    {
      return EPG_DB_PF_READY;
    }
  }

  return db_status;
}

evt_node_t *epg_data_get_evt(void *p_data, epg_filter_t *p_filter, u32 *p_evt_cnt)
{
  sev_node_t *p_svc_node = NULL;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  utc_time_t start_time = {0};
  evt_node_t *p_evt_head = NULL;
  evt_node_t *p_tmp_evt = NULL;
  evt_node_t *p_prev_day_last_evt = NULL;
  evt_node_t *p_cur_day_first_evt = NULL;
  u8 index = 0;
  s8 result = 0;
  BOOL content = TRUE;

  if (p_epg_var == NULL || p_filter == NULL)
  {
    return NULL;
  }

  if (EVT_TCMP(&p_filter->end_time, &p_filter->start_time) < 0)
  {
    return NULL;
  }

  if (p_evt_cnt != NULL)
  {
    *p_evt_cnt = 0;
  }

  p_svc_node = get_svc_node(p_data,
    p_filter->service_id, p_filter->stream_id, p_filter->orig_network_id);

  if (p_svc_node == NULL)
  {
    EPG_PRINT("\nepg_data_get_evt [%d][%d][%d] p_svc_node == NULL\n",
      p_filter->orig_network_id, p_filter->stream_id, p_filter->service_id);
    return NULL;
  }

  time_get(&start_time, TRUE);

  if (EVT_TCMP(&start_time, &p_filter->start_time) != 1)
  {
    memcpy(&start_time, &(p_filter->start_time), sizeof(utc_time_t));
  }

  //evt start head
  for (index = 0; index < p_svc_node->saved_evt_day; index ++)
  {
    p_tmp_evt = p_svc_node->pp_evt_day_head[index];
    if (p_tmp_evt->start_time.day == start_time.day)
    {
      if (index)  //add case when a evt last overnight
      {
        p_tmp_evt = p_svc_node->pp_evt_day_head[index - 1];  //prev day first evt
        p_cur_day_first_evt = p_svc_node->pp_evt_day_head[index];
        while (p_tmp_evt != NULL && p_tmp_evt != p_cur_day_first_evt)
        {
          p_prev_day_last_evt = p_tmp_evt;
          p_tmp_evt = p_tmp_evt->p_next_evt_node;
        }
        p_tmp_evt = p_prev_day_last_evt;
      }

      break;
    }
  }

  while (p_tmp_evt != NULL)
  {
    result = is_evt_in_given_period(p_tmp_evt, &start_time, &p_filter->end_time);

    content = is_evt_in_given_content(p_tmp_evt, p_filter->cont_level);

    //evt start time > filter end time
    if (result == 1)
    {
      break;
    }

    //evt match the filter condition
    if ((p_tmp_evt->event_id != 0 || p_tmp_evt->start_time.year != 0)
      && (result == 0 && content == TRUE))
    {
      if (p_evt_cnt != NULL)
      {
        (*p_evt_cnt) ++;
      }
      else
      {
        return p_tmp_evt;
      }

      if (p_evt_head == NULL)
      {
        p_evt_head = p_tmp_evt;
      }
    }

    p_tmp_evt = p_tmp_evt->p_next_evt_node;
  }

  return p_evt_head;
}

evt_node_t *epg_data_get_pf(void *p_data, epg_filter_t *p_filter)
{
  evt_node_t *p_evt_node = NULL;
  sev_node_t *p_svc_node = NULL;
  p_svc_node = get_svc_node(p_data,
    p_filter->service_id, p_filter->stream_id, p_filter->orig_network_id);
  if(p_svc_node == NULL)
    {
      return NULL;
    }
  p_evt_node = p_svc_node->pp_evt_day_head[0];
  return p_evt_node;
}

evt_node_t *epg_data_get_next_evt(void *p_data, evt_node_t *p_last_evt, epg_filter_t *p_filter)
{
  utc_time_t start_time;
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  evt_node_t *p_tmp_evt = NULL;
  s8 result = 0;
  BOOL content = FALSE;

  CHECK_FAIL_RET_NULL(p_epg_var != NULL);

  if (p_last_evt == NULL || p_filter == NULL)
  {
    return NULL;
  }

  if (EVT_TCMP(&p_filter->end_time, &p_filter->start_time) < 0)
  {
    return NULL;
  }

  time_get(&start_time, TRUE);
  if (EVT_TCMP(&start_time, &p_filter->start_time) != 1)
  {
    memcpy(&start_time, &(p_filter->start_time), sizeof(utc_time_t));
  }

  p_tmp_evt = p_last_evt->p_next_evt_node;

  while (p_tmp_evt != NULL)
  {
    result = is_evt_in_given_period(p_tmp_evt, &start_time, &p_filter->end_time);

    content = is_evt_in_given_content(p_tmp_evt, p_filter->cont_level);

    //evt start time > filter end time
    if (result == 1)
    {
      return NULL;
    }

    //evt match the filter condition
    if ((p_tmp_evt->event_id != 0 || p_tmp_evt->start_time.year != 0)
      && (result == 0 && content == TRUE))
    {
      break;
    }

    p_tmp_evt = p_tmp_evt->p_next_evt_node;
  }

  return p_tmp_evt;
}

BOOL epg_data_delete(void *p_data, epg_del_opt_t del_op, epg_filter_t *p_filter)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;

  CHECK_FAIL_RET_FALSE(p_epg_var != NULL);

  switch (del_op)
  {
  case EPG_DEL_ALL_NODE:
    return del_all_node(p_data);

  case EPG_DEL_OVERDEU_EVT:
    return del_overdue_evt_node(p_data);

  case EPG_DEL_BY_FILTER:
    return del_by_filter(p_data, p_filter);

  case EPG_DEL_EXP_FILTER:
    return del_exp_filter(p_data, p_filter);

  case EPG_DEL_EXP_DY_POLICY:
    return del_exp_dy_policy(p_data);

  default:
    break;
  }

  return FALSE;
}

epg_db_report_t *epg_data_get_db_report(void *p_data)
{
  epg_db_t *p_epg_var = (epg_db_t *)p_data;
  orig_net_node_t *p_net_node = NULL;
  ts_node_t *p_ts_node = NULL;
  sev_node_t *p_svc_node = NULL;
  evt_node_t *p_evt_node = NULL;

  CHECK_FAIL_RET_NULL(p_epg_var != NULL);

  p_epg_var->db_report.net_node_used = 0;
  p_epg_var->db_report.ts_node_used = 0;
  p_epg_var->db_report.svc_node_used = 0;
  p_epg_var->db_report.evt_node_used = 0;
  p_epg_var->db_report.nonfix_used = 0;

  p_net_node =p_epg_var->p_net_node_head;
  while (p_net_node != NULL)
  {
    p_ts_node = p_net_node->p_ts_node_head;
    while (p_ts_node != NULL)
    {
      p_svc_node = p_ts_node->p_svc_head;
      while (p_svc_node != NULL)
      {
        p_evt_node = p_svc_node->p_evt_node_head;
        while (p_evt_node != NULL)
        {
          p_epg_var->db_report.nonfix_used += (p_evt_node->shrt_ext_len);
          p_epg_var->db_report.evt_node_used ++;
          p_evt_node = p_evt_node->p_next_evt_node;
        }
        p_epg_var->db_report.svc_node_used ++;
        p_svc_node = p_svc_node->p_next_s_node;
      }
      p_epg_var->db_report.ts_node_used ++;
      p_ts_node = p_ts_node->p_next_ts_node;
    }
    p_epg_var->db_report.net_node_used ++;
    p_net_node = p_net_node->p_next_net_node;
  }

  EPG_PRINT("\n##debug: net [%d/%d], ts [%d/%d], svc [%d/%d], evt [%d/%d], nonfix [%d/%d]\n",
     p_epg_var->db_report.net_node_total,
     p_epg_var->db_report.net_node_used,
     p_epg_var->db_report.ts_node_total,
     p_epg_var->db_report.ts_node_used,
     p_epg_var->db_report.svc_node_total,
     p_epg_var->db_report.svc_node_used,
     p_epg_var->db_report.evt_node_total,
     p_epg_var->db_report.evt_node_used,
     p_epg_var->db_report.nonfix_total,
     p_epg_var->db_report.nonfix_used);
  return &p_epg_var->db_report;
}

void epg_data_init_lang_code(void *p_data)
{
  epg_db_t *p_priv_data = (epg_db_t *)p_data;

  memset(p_priv_data->set_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8));
  memset(p_priv_data->first_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8));
  memset(p_priv_data->second_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8));
  memset(p_priv_data->default_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8));
  p_priv_data->lang_code_forbit_swich = FALSE;
  p_priv_data->lang_code_sync = LANG_CODE_SYNC_TS;
  p_priv_data->lang_code_check_times = 100;
  p_priv_data->lang_set_sync_id = FALSE;
  
  p_priv_data->lang_code_first_times = 0;
  p_priv_data->lang_code_second_times = 0;
  p_priv_data->lang_code_default_times = 0;
  p_priv_data->lang_code_other_times = 0;
}

void epg_data_set_lang_code(void *p_data,
  u8 first_lang[LANGUAGE_CODE_LEN],
  u8 second_lang[LANGUAGE_CODE_LEN],
  u8 default_lang[LANGUAGE_CODE_LEN])
{
  epg_db_t *p_priv_data = (epg_db_t *)p_data;
  
  memcpy(p_priv_data->first_lang_code,first_lang, 
                                           LANGUAGE_CODE_LEN * sizeof(u8));
  memcpy(p_priv_data->second_lang_code,second_lang, 
                                         LANGUAGE_CODE_LEN * sizeof(u8));
  memcpy(p_priv_data->default_lang_code,default_lang, 
                                         LANGUAGE_CODE_LEN * sizeof(u8));
  memcpy(p_priv_data->set_lang_code,first_lang, 
                                          LANGUAGE_CODE_LEN * sizeof(u8));

}

/*!
  set lang code policy,
  forbit_sw: true:don't swich lang priority but first lang code;
  pos: check event lang code position
  max_times:auto swich max different lang times
  */
void epg_data_set_lang_code_policy(void *p_data,
                                                                BOOL forbit_sw,
                                                                lang_code_sync_t pos,
                                                                u32 max_times)
{
   epg_db_t *p_priv_data = (epg_db_t *)p_data;
   p_priv_data->lang_code_forbit_swich = forbit_sw;
   p_priv_data->lang_code_sync = pos;
   p_priv_data->lang_code_check_times = max_times;
}

/*!
  set lang sync sync pg pararm,
  play_pg_pararm
  */
void epg_data_set_lang_sync_pg_pararm(void *p_data,
                                                                u16 pg_net_id,
                                                                u16 pg_ts_id,
                                                                u16 pg_svc_id)
{
  epg_db_t *p_priv_data = (epg_db_t *)p_data;
  if((pg_net_id == 0)
    && (pg_ts_id == 0)
    && (pg_svc_id == 0))
    {
      p_priv_data->lang_set_sync_id = FALSE;
    }
  else
    {
      p_priv_data->lang_net_id = pg_net_id;
      p_priv_data->lang_ts_id = pg_ts_id;
      p_priv_data->lang_svc_id = pg_svc_id;
      p_priv_data->lang_set_sync_id = TRUE;
    }
}

/*!
  epg database check group valid
  */
static BOOL epg_data_check_group_valid(
                            u8 lang_code[LANGUAGE_CODE_LEN])
{
  u8 null_code[LANGUAGE_CODE_LEN] = {0};
  memset(null_code,0,LANGUAGE_CODE_LEN * sizeof(u8));
  if(memcmp(null_code,lang_code,
                        LANGUAGE_CODE_LEN) == 0)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
  return TRUE;
}


static BOOL epg_data_check_epg_lang_code(
                            u8 epg_lang_code[LANGUAGE_CODE_LEN],
                            u8 cmp_lang[LANGUAGE_CODE_LEN])
{
  u8 null_code[LANGUAGE_CODE_LEN] = {0};
  memset(null_code,0,LANGUAGE_CODE_LEN * sizeof(u8));
  if(memcmp(null_code,cmp_lang,
                        LANGUAGE_CODE_LEN) == 0)
  {
    return FALSE;
  }
  if(memcmp(epg_lang_code,cmp_lang,
                        LANGUAGE_CODE_LEN) == 0)
  {
    return TRUE;
  }
  return FALSE;
}


static BOOL epg_epg_data_lang_code_cmp(void *p_data,u8 lang_code[LANGUAGE_CODE_LEN])
{
    epg_db_t *p_priv_data = (epg_db_t *)p_data;
    if(TRUE == epg_data_check_epg_lang_code(lang_code,
                          p_priv_data->first_lang_code))
    {
      p_priv_data->lang_code_second_times = 0;
      p_priv_data->lang_code_default_times = 0;
      p_priv_data->lang_code_other_times = 0;
      if(TRUE != epg_data_check_epg_lang_code(lang_code,
                      p_priv_data->set_lang_code))
      {
        p_priv_data->lang_code_first_times ++;
        if(p_priv_data->lang_code_first_times >= p_priv_data->lang_code_check_times)
          {
            memcpy(p_priv_data->set_lang_code,p_priv_data->first_lang_code, 
                                               LANGUAGE_CODE_LEN * sizeof(u8));
            p_priv_data->lang_code_first_times = 0;
            p_priv_data->lang_code_second_times = 0;
            p_priv_data->lang_code_default_times = 0;
            p_priv_data->lang_code_other_times = 0;
            return TRUE;
          }
      }
      else
      {
        p_priv_data->lang_code_first_times = 0;
      }
    }
    else if(TRUE == epg_data_check_epg_lang_code(lang_code,
                          p_priv_data->second_lang_code))
    {
      p_priv_data->lang_code_default_times = 0;
      p_priv_data->lang_code_other_times = 0;
      if(TRUE != epg_data_check_epg_lang_code(lang_code,
              p_priv_data->set_lang_code))
       {
          p_priv_data->lang_code_second_times ++;
          if(p_priv_data->lang_code_second_times >= p_priv_data->lang_code_check_times)
            {
              memcpy(p_priv_data->set_lang_code,p_priv_data->second_lang_code, 
                                                 LANGUAGE_CODE_LEN * sizeof(u8));
              p_priv_data->lang_code_first_times = 0;
              p_priv_data->lang_code_second_times = 0;
              p_priv_data->lang_code_default_times = 0;
              p_priv_data->lang_code_other_times = 0;
              return TRUE;
            }
        }
      else
        {
          p_priv_data->lang_code_second_times = 0;
        }
    }
    else if(TRUE == epg_data_check_epg_lang_code(lang_code,
                          p_priv_data->default_lang_code))
    {
       p_priv_data->lang_code_other_times = 0;
       if(TRUE != epg_data_check_epg_lang_code(lang_code,
              p_priv_data->set_lang_code))
        {
           p_priv_data->lang_code_default_times ++;
          if(p_priv_data->lang_code_default_times >= p_priv_data->lang_code_check_times)
            {
              memcpy(p_priv_data->set_lang_code,p_priv_data->default_lang_code, 
                                                 LANGUAGE_CODE_LEN * sizeof(u8));
              p_priv_data->lang_code_first_times = 0;
              p_priv_data->lang_code_second_times = 0;
              p_priv_data->lang_code_default_times = 0;
              p_priv_data->lang_code_other_times = 0;
              return TRUE;
            }
      }
      else
      {
        p_priv_data->lang_code_default_times = 0;
      }
    }
    /***it is other lang code***/
   else 
    {
        if(TRUE != epg_data_check_epg_lang_code(lang_code,
            p_priv_data->set_lang_code))
        {
           p_priv_data->lang_code_other_times ++;
           if(p_priv_data->lang_code_other_times >= p_priv_data->lang_code_check_times)
            {
                /********next input switch set lang code************/
                memset(p_priv_data->set_lang_code,0, 
                                             LANGUAGE_CODE_LEN * sizeof(u8));
                memcpy(p_priv_data->set_lang_code,lang_code, 
                                                   LANGUAGE_CODE_LEN * sizeof(u8));
                p_priv_data->lang_code_first_times = 0;
                p_priv_data->lang_code_second_times = 0;
                p_priv_data->lang_code_default_times = 0;
                p_priv_data->lang_code_other_times = 0;
                return TRUE;
            }
        }
        else
        {
          p_priv_data->lang_code_other_times = 0;
        }
    }

  return FALSE;
}


BOOL epg_data_try_lang_code_priority(void *p_data,eit_t *p_eit_info)
{
  epg_db_t *p_priv_data = (epg_db_t *)p_data;
  u8 i = 0;
  u8 lang_code[LANGUAGE_CODE_LEN] = {0};
  u16 cmp_net_id = 0;
  u16 cmp_ts_id = 0;
  u16 cmp_svc_id = 0;
  BOOL ret = FALSE;

  /****default or not set lang code,it will don't set lang code******/
  if((TRUE != epg_data_check_group_valid( 
                              p_priv_data->first_lang_code))  
      && (TRUE != epg_data_check_group_valid( 
                              p_priv_data->second_lang_code))
      && (TRUE != epg_data_check_group_valid( 
                              p_priv_data->default_lang_code)))
    {
         memset(p_priv_data->set_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8));
        return FALSE;
    }

   if(p_priv_data->lang_code_forbit_swich == TRUE)
  {
     return FALSE;
  }

   if(p_priv_data->lang_set_sync_id == TRUE)
    {
      cmp_net_id = p_priv_data->lang_net_id;
      cmp_ts_id = p_priv_data->lang_ts_id;
      cmp_svc_id = p_priv_data->lang_svc_id;
    }
   else
    {
      cmp_net_id = p_priv_data->p_dy_policy->cur_net_id;
      cmp_ts_id = p_priv_data->p_dy_policy->cur_ts_id;
      cmp_svc_id = p_priv_data->p_dy_policy->cur_svc_id;
    }

  if((cmp_net_id != p_eit_info->org_nw_id)
      && (p_priv_data->lang_code_sync == LANG_CODE_SYNC_NET))
    {
       return FALSE;
    }
  if((cmp_ts_id != p_eit_info->stream_id)
      && (p_priv_data->lang_code_sync == LANG_CODE_SYNC_TS))
    {
       return FALSE;
    }
   if((cmp_svc_id != p_eit_info->svc_id)
      && (p_priv_data->lang_code_sync == LANG_CODE_SYNC_SVC))
    {
       return FALSE;
    }



  //Check short txt information
  for (i = 0; i  < p_eit_info->tot_sht_txt_num; i  ++)
  {
    if (i  >= MAX_SHT_TXT_NUM)
    {
      break;
    }
    if((p_eit_info->sht_txt[i].name_len > 0)
      || (p_eit_info->sht_txt[i].txt_len > 0))
    {
      memcpy(lang_code,p_eit_info->sht_txt[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
      ret = epg_epg_data_lang_code_cmp(p_priv_data,lang_code);
       if(TRUE == ret)
        {
          return ret;
        }
     }
   }

  for (i = 0; i  < p_eit_info->tot_ext_txt_num; i  ++)
  {
    if (i  >= MAX_EXT_TXT_NUM)
    {
      break;
    }
    if(p_eit_info->ext_txt[i].txt_len)
    {
      memcpy(lang_code,p_eit_info->ext_txt[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
      ret = epg_epg_data_lang_code_cmp(p_priv_data,lang_code);
       if(TRUE == ret)
        {
          return ret;
        }
     }
   }

  return FALSE;
}

void epg_data_filter_evt_lang_code(void *p_data,eit_t *p_eit_info)
{
  epg_db_t *p_priv_data = (epg_db_t *)p_data;
  sht_evt_txt_t *p_sht_txt = NULL;  
  ext_evt_txt_t *p_ext_txt = NULL;
  ext_evt_desc_t *p_ext_evt_info = NULL;
  u8 tot_sht_txt_num = 0;
  u8 tot_ext_txt_num = 0;
  u8 tot_ext_info_num = 0;
  u8 i = 0;
  u8 lang_code[LANGUAGE_CODE_LEN] = {0};

  if(TRUE != epg_data_check_group_valid(p_priv_data->set_lang_code))
    {
      return;
    }
  
  p_sht_txt = (sht_evt_txt_t *)mtos_malloc(sizeof(sht_evt_txt_t) * MAX_SHT_TXT_NUM);
  if(p_sht_txt == NULL)
  {
      return;
  }
  p_ext_txt = (ext_evt_txt_t *)mtos_malloc(sizeof(ext_evt_txt_t) * MAX_EXT_TXT_NUM);
  if(p_ext_txt == NULL)
  {
      mtos_free(p_sht_txt);
      return;
  }

  p_ext_evt_info = (ext_evt_desc_t *)mtos_malloc(
                        sizeof(ext_evt_desc_t) * MAX_EXT_DESC_NUM);
  if(p_ext_evt_info == NULL)
  {
      mtos_free(p_sht_txt);
      mtos_free(p_ext_txt);
      return;
  }

  memset(p_sht_txt ,0,sizeof(sht_evt_txt_t) * MAX_SHT_TXT_NUM);
  memset(p_ext_txt,0,sizeof(ext_evt_txt_t) * MAX_EXT_TXT_NUM);
  memset(p_ext_evt_info,0,sizeof(ext_evt_desc_t) *MAX_EXT_DESC_NUM);

    //Check short txt information
  for (i = 0; i  < p_eit_info->tot_sht_txt_num; i  ++)
  {
    if (i  >= MAX_SHT_TXT_NUM)
    {
      break;
    }
    memcpy(lang_code,p_eit_info->sht_txt[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
    if(epg_data_check_epg_lang_code(lang_code,
                                  p_priv_data->set_lang_code) == TRUE)
      {
          memcpy(&p_sht_txt[tot_sht_txt_num],
              &p_eit_info->sht_txt[i],
              sizeof(sht_evt_txt_t));
          tot_sht_txt_num ++;
      }
   }
  memset(&p_eit_info->sht_txt[0],
                0,
                (sizeof(sht_evt_txt_t) * MAX_SHT_TXT_NUM));
  memcpy(&p_eit_info->sht_txt[0],
                &p_sht_txt[0],
                (sizeof(sht_evt_txt_t) * tot_sht_txt_num));
  p_eit_info->tot_sht_txt_num = tot_sht_txt_num;

 for (i = 0; i  < p_eit_info->tot_ext_txt_num; i  ++)
  {
    if (i  >= MAX_EXT_TXT_NUM)
    {
      break;
    }
    memcpy(lang_code,p_eit_info->ext_txt[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
    if(epg_data_check_epg_lang_code(lang_code,
                                   p_priv_data->set_lang_code) == TRUE)
      {
        memcpy(&p_ext_txt[tot_ext_txt_num],
                    &p_eit_info->ext_txt[i],
                    sizeof(ext_evt_txt_t));
        tot_ext_txt_num ++;
      }
 }

 memset(&p_eit_info->ext_txt[0],
              0,
              (sizeof(ext_evt_txt_t) * MAX_EXT_TXT_NUM));
 memcpy(&p_eit_info->ext_txt[0],
              &p_ext_txt[0],
              (sizeof(ext_evt_txt_t) * tot_ext_txt_num));
 p_eit_info->tot_ext_txt_num = tot_ext_txt_num;

  for (i = 0; i  < p_eit_info->tot_ext_info_num; i  ++)
  {
    if (i  >= MAX_EXT_DESC_NUM)
    {
      break;
    }
    memcpy(lang_code,p_eit_info->ext_evt_info[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
    if(epg_data_check_epg_lang_code(lang_code,
                                   p_priv_data->set_lang_code) == TRUE)
      {
        memcpy(&p_ext_evt_info[tot_ext_info_num],
          &p_eit_info->ext_evt_info[i],
          sizeof(ext_evt_desc_t));
        tot_ext_info_num ++;
      }
 }

  memset(&p_eit_info->ext_evt_info[0],
                0,
                sizeof(ext_evt_desc_t) * MAX_EXT_DESC_NUM);
  memcpy(&p_eit_info->ext_evt_info[0],
    &p_ext_evt_info[0],
    sizeof(ext_evt_desc_t) * tot_ext_info_num);
    p_eit_info->tot_ext_info_num = tot_ext_info_num;

  mtos_free(p_sht_txt);
  mtos_free(p_ext_txt);
  mtos_free(p_ext_evt_info);
  return;
}


/*!
  end of file
  */
