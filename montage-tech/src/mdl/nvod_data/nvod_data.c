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
#include "mosaic.h"
#include "sdt.h"
#include "eit.h"
#include "pmt.h"
#include "ts_packet.h"
#include "nvod_data.h"
#include "lib_unicode.h"

/*!
   Section number in each segment
  */
#define SEC_NUM_PER_SEG (0x8)
/*!
  max nvod reference svc cnt
  */
#define MAX_NVOD_REFERENCE_SVC (10)
/*!
  max shift evt cnt
  */
#define MAX_SHIFT_EVT (600)
/*!
  max reference evt cnt
  */
#define MAX_REF_EVT (30)

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

/*!
  nvod_db
  */
typedef struct tag_nvod_db
{
  /*!
    nvod_reference_svc_t
    */
  nvod_reference_svc_t svc[MAX_NVOD_REFERENCE_SVC];
  /*!
    cur reference svc index
    */
  s8 cur_ref_svc_idx;
  /*!
    nvod_svc_cnt
    */
  u8 nvod_svc_cnt;
  /*!
    total_ref_evt_cnt
    */
  u16 total_ref_evt_cnt;
  /*!
    nvod reference svc evt
    */
  nvod_reference_svc_evt_t *p_ref_evt_head;
  /*!
    buffer for nvod data
    */
  u8 *p_buffer;
  /*!
    locked tp
    */
  dvbc_tp_info_t locked_tp;
  /*!
    memf for shift evt node with fixed size
    */
  lib_memf_t shift_evt_memf;
  /*!
    memp for ref evt memory 
    */
  lib_memp_t ref_evt_memp;
}nvod_db_t;

/*!
  evt_parent_t
  */
typedef struct tag_evt_parent
{
  /*!
    nvod reference service index
    */
  nvod_reference_svc_t *p_ref_svc;
  /*!
    time shifted service index
    */
  u32 shift_svc_idx;
}evt_parent_t;

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

static inline BOOL insert_shift_evt(nvod_reference_svc_evt_t *p_ref_evt, 
  time_shifted_svc_evt_t *p_new)
{
  time_shifted_svc_evt_t *p_prv_evt = NULL;
  time_shifted_svc_evt_t *p_tmp_evt = NULL;
  s8 cmp_reslt = 0;
  BOOL add_evt = FALSE;
  
  if (p_ref_evt->p_shift_evt == NULL)
  {
    p_ref_evt->p_shift_evt = p_new;
    return TRUE;
  }

  p_tmp_evt = p_ref_evt->p_shift_evt;
  while (p_tmp_evt)
  {
    cmp_reslt = EVT_TCMP(&p_new->start_time, &p_tmp_evt->start_time);
    if (cmp_reslt == 1)
    {
      p_prv_evt = p_tmp_evt;
      p_tmp_evt = p_tmp_evt->p_next_evt;
      if (p_tmp_evt == NULL)
      {
        p_prv_evt->p_next_evt = p_new;
        add_evt = TRUE;
        break;
      }
    }
    else if (cmp_reslt == -1)
    {
      if (p_prv_evt == NULL)
      {
        p_new->p_next_evt = p_tmp_evt;
        p_ref_evt->p_shift_evt = p_new;
      }
      else
      {
        p_new->p_next_evt = p_tmp_evt;
        p_prv_evt->p_next_evt = p_new;
      }
      add_evt = TRUE;
      break;
    }
    else
    {
      if (p_new->evt_status != p_tmp_evt->evt_status)
      {
        p_tmp_evt->evt_status = p_new->evt_status;
      }
      break;
    }
  }
  return add_evt;
}

static u8 eit_info_svc_type(void *p_data, eit_t *p_eit_info, u32 *p_shift_svc_idx)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_t *p_ref_svc = NULL;
  u8 loopi = 0;
  u8 loopj = 0;
  
  if (p_eit_info->table_id != DVB_TABLE_ID_EIT_ACTUAL
    && p_eit_info->table_id != DVB_TABLE_ID_EIT_OTHER
    && p_eit_info->table_id != DVB_TABLE_ID_EIT_SCH_ACTUAL
    && p_eit_info->table_id != DVB_TABLE_ID_EIT_SCH_ACTUAL_51)
  {
    return 0;
  }
  
  if (p_nvod->cur_ref_svc_idx != -1)
  {
    p_ref_svc = (nvod_reference_svc_t *)&p_nvod->svc[p_nvod->cur_ref_svc_idx];
    
    for (loopj = 0; loopj < p_ref_svc->time_shifted_svc_cnt; loopj ++)
    {
      if (p_eit_info->org_nw_id == p_ref_svc->time_shifted_svc[loopj].orig_network_id
        && p_eit_info->stream_id == p_ref_svc->time_shifted_svc[loopj].stream_id
        && p_eit_info->svc_id == p_ref_svc->time_shifted_svc[loopj].svc_id)
      {
        (*p_shift_svc_idx) = loopj;
        return DVB_NVOD_TIME_SHIFTED_SVC;
      }
    }
  }
  
  if (p_eit_info->table_id != DVB_TABLE_ID_EIT_ACTUAL)
  {
    return 0;
  }
  
  for (loopi = 0; loopi < p_nvod->nvod_svc_cnt; loopi ++)
  {
    if (p_eit_info->svc_id == p_nvod->svc[loopi].svc_id)
    {
      p_nvod->cur_ref_svc_idx = loopi;
      return DVB_NVOD_REF_SVC;
    }
  }

  return 0;
}

static u32 add_shift_svc_evt(void *p_data, eit_t *p_eit_info, u8 time_shifted_svc_index)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_t *p_svc = NULL;
  sht_evt_desc_t *p_evt_desc = NULL;
  utc_time_t sys_time = {0};
  utc_time_t end_time = {0};
  time_shifted_svc_evt_t *p_new = NULL;
  nvod_reference_svc_evt_t *p_svc_evt = NULL;
  BOOL add_new = FALSE;
  u8 evt_idx = 0;
  u32 evt_count = 0;
  RET_CODE ret = SUCCESS;
  u8 evt_status = 0;

  p_svc = (nvod_reference_svc_t *)(&p_nvod->svc[p_nvod->cur_ref_svc_idx]);
  time_get(&sys_time, TRUE);

  while (evt_idx < p_eit_info->tot_evt_num)
  {
    if (evt_idx >= MAX_EVT_PER_SEC)
    {
      break;
    }

    p_evt_desc = &p_eit_info->sht_evt_info[evt_idx];

    evt_status = 0;
    
    if (EVT_TCMP(&p_evt_desc->st_tm, &sys_time) < 0)
    {
      evt_status = 1;
    }
    
    if (p_eit_info->table_id >= DVB_TABLE_ID_EIT_SCH_ACTUAL)
    {
      end_time = p_evt_desc->st_tm;
      evt_tadd(&end_time, &p_evt_desc->drt_time);
      if (EVT_TCMP(&end_time, &sys_time) != 1)
      {
        evt_idx ++;
        continue;
      }
    }

    if (!p_evt_desc->time_shifted_evt_des_found)
    {
      evt_idx ++;
      OS_PRINTF("\n##debug: time_shifted_evt_des_found = 0!\n");
      continue;
    }
    
    add_new = FALSE;
    p_new = lib_memf_alloc(&p_nvod->shift_evt_memf);
    MT_ASSERT(p_new != NULL);
    memset(p_new, 0, sizeof(time_shifted_svc_evt_t));
    p_new->p_next_evt = NULL;
    p_new->start_time = p_evt_desc->st_tm;
    p_new->drt_time = p_evt_desc->drt_time;
    p_new->event_id = p_evt_desc->evt_id;
    p_new->evt_status = evt_status;
    p_new->time_shifted_svc_idx = time_shifted_svc_index;

    p_svc_evt = p_nvod->p_ref_evt_head;
    while (p_svc_evt)
    {
      if (p_svc_evt->event_id == p_evt_desc->time_shifted_evt_des.evt_id
        && p_svc_evt->ref_svc_idx == p_nvod->cur_ref_svc_idx)
      {
        break;
      }
      p_svc_evt = p_svc_evt->p_next_evt;
    }
    
    if (p_svc_evt)
    {
      add_new = insert_shift_evt(p_svc_evt, p_new);
    }
    else
    {
      OS_PRINTF("\n##debug: table id [0x%x] not add [0x%x][%x]\n", p_eit_info->table_id,
        p_evt_desc->time_shifted_evt_des.svc_id, p_evt_desc->time_shifted_evt_des.evt_id);
    }

    if (!add_new)
    {
      ret = lib_memf_free(&p_nvod->shift_evt_memf, p_new);
      MT_ASSERT(ret == SUCCESS);
      p_new = NULL;
    }
    else
    {
      p_svc_evt->shift_evt_cnt ++;
      evt_count ++;
    }
    
    evt_idx++;
  }

  return evt_count;
}

static BOOL check_ref_svc_evt_exist(void *p_data, u16 new_evt_id, s8 ref_svc_idx)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_evt_t *p_next_evt = NULL;
//  u8 same_ref_svc = 0;

  p_next_evt = p_nvod->p_ref_evt_head;
  while (p_next_evt)
  {
    if (p_next_evt->event_id == new_evt_id
      && p_next_evt->ref_svc_idx == ref_svc_idx)
    {
      return TRUE;
    }
    
    p_next_evt = p_next_evt->p_next_evt;
  }

  return FALSE;
}

static void insert_ref_svc_evt(void *p_data, nvod_reference_svc_evt_t *p_new)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_evt_t *p_prev_evt = NULL;
  nvod_reference_svc_evt_t *p_next_evt = NULL;
  u8 ref_svc = 0;

  if (p_nvod->p_ref_evt_head == NULL)
  {
    p_nvod->p_ref_evt_head = p_new;
    return ;
  }

  p_next_evt = p_nvod->p_ref_evt_head;
  while (p_next_evt)
  {
    if (p_next_evt->ref_svc_idx == p_new->ref_svc_idx)
    {
      ref_svc = 1;
    }
    else
    {
      if (ref_svc)
      {
        p_new->p_next_evt = p_next_evt;
        p_prev_evt->p_next_evt = p_new;
        return ;
      }
    }
    p_prev_evt = p_next_evt;
    p_next_evt = p_next_evt->p_next_evt;
  }
  p_prev_evt->p_next_evt = p_new;
  p_new->p_next_evt = NULL;
}

static u32 add_ref_svc_evt(void *p_data, eit_t *p_eit_info)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_t *p_svc = NULL;
  sht_evt_desc_t *p_evt_desc = NULL;
  nvod_reference_svc_evt_t *p_new_evt = NULL;
  u8 evt_idx = 0;
  u8 cont_idx = 0;
  u16 sht_idx = 0;
  u16 short_txt_len = 0;
  u32 evt_count = 0;

  p_svc = (nvod_reference_svc_t *)(&p_nvod->svc[p_nvod->cur_ref_svc_idx]);
  while (evt_idx < p_eit_info->tot_evt_num)
  {
    if (evt_idx >= MAX_EVT_PER_SEC)
    {
      break;
    }

    p_evt_desc = &p_eit_info->sht_evt_info[evt_idx];
    short_txt_len = 0;

    if (check_ref_svc_evt_exist(p_data, p_evt_desc->evt_id, p_nvod->cur_ref_svc_idx))
    {
      evt_idx ++;
      continue;
    }
    
    for (sht_idx = 0; sht_idx < p_eit_info->tot_sht_txt_num; sht_idx ++)
    {
      if (sht_idx >= MAX_SHT_TXT_NUM)
      {
        return evt_count;
      }
      
      if (p_eit_info->sht_txt[sht_idx].evt_id == p_evt_desc->evt_id
        && p_eit_info->sht_txt[sht_idx].txt_len != 0)
      {
        short_txt_len = p_eit_info->sht_txt[sht_idx].txt_len + 1;
        break;
      }
    }
    
    p_new_evt = lib_memp_alloc(&p_nvod->ref_evt_memp,
      sizeof(nvod_reference_svc_evt_t) + short_txt_len * sizeof(u16));
    MT_ASSERT(p_new_evt != NULL);
    memset(p_new_evt, 0, sizeof(nvod_reference_svc_evt_t) + short_txt_len * sizeof(u16));
    p_new_evt->shift_evt_cnt = 0;
    p_new_evt->p_next_evt = NULL;
    p_new_evt->p_shift_evt = NULL;
    p_new_evt->sht_txt_len = short_txt_len;
    p_new_evt->event_id = p_evt_desc->evt_id;
    p_new_evt->ref_svc_idx = p_nvod->cur_ref_svc_idx;
    if (sht_idx == p_eit_info->tot_sht_txt_num)
    {
      p_new_evt->p_sht_txt = NULL;
    }
    else
    {
      p_new_evt->p_sht_txt = (u16 *)(p_new_evt + sizeof(nvod_reference_svc_evt_t));
      dvb_to_unicode(p_eit_info->sht_txt[sht_idx].txt,
        p_eit_info->sht_txt[sht_idx].txt_len,
        p_new_evt->p_sht_txt, 
        p_new_evt->sht_txt_len);
    }

    if (p_eit_info->sht_txt[sht_idx].name_len >0)
    {
      dvb_to_unicode(p_eit_info->sht_txt[sht_idx].evt_nm, 
      p_eit_info->sht_txt[sht_idx].name_len,
        p_new_evt->event_name, MAX_EVT_NAME_LEN + 1);
    }
    else
    {
      p_new_evt->event_name[0] = 32;
      p_new_evt->event_name[1] = 0;
    }
    
    dvb_to_unicode(p_eit_info->sht_txt[sht_idx].lang_code, LANGUAGE_CODE_LEN , 
                              p_new_evt->lang_code, LANGUAGE_CODE_LEN + 1);
    
    //check content desc
    for (cont_idx = 0; cont_idx < p_eit_info->tot_cont_num; cont_idx ++)
    {
      if (cont_idx >= MAX_EVT_PER_SEC)
      {
        break;
      }
      if (p_eit_info->cont_desc[cont_idx].evt_id == p_evt_desc->evt_id)
      {
        break;
      }
    }

    if ((cont_idx >= MAX_EVT_PER_SEC) || (cont_idx >= p_eit_info->tot_cont_num))
    {
      p_new_evt->cont_valid_flag = 0;
    }
    else
    {
      p_new_evt->cont_valid_flag = 1;
      p_new_evt->cont_level  = p_eit_info->cont_desc[cont_idx].cont_level;
      p_new_evt->usr_nib   = p_eit_info->cont_desc[cont_idx].usr_nib;
    }

    insert_ref_svc_evt(p_data, p_new_evt);

    p_nvod->total_ref_evt_cnt ++;
    p_svc->reference_evt_cnt ++;
    evt_count = p_svc->reference_evt_cnt;
    evt_idx++;
  }

  return evt_count;
}

static void nvod_mem_alloc(void *p_data)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  RET_CODE ret_mem = SUCCESS;
  u32 non_fix_size = 0;
  u8 *p_buffer = NULL;

  non_fix_size = (MAX_SHT_TEXT_LEN + sizeof(nvod_reference_svc_evt_t)) * MAX_REF_EVT;
  p_buffer = mtos_malloc(MAX_SHIFT_EVT * sizeof(time_shifted_svc_evt_t) + non_fix_size);
  MT_ASSERT(p_buffer != NULL);
  memset(p_buffer, 0, MAX_SHIFT_EVT * sizeof(time_shifted_svc_evt_t) + non_fix_size);
  p_nvod->p_buffer = p_buffer;
  ret_mem = lib_memf_create(&p_nvod->shift_evt_memf, (u32)p_buffer, 
      MAX_SHIFT_EVT * sizeof(time_shifted_svc_evt_t), sizeof(time_shifted_svc_evt_t));
  MT_ASSERT(ret_mem == SUCCESS);

  p_buffer += MAX_SHIFT_EVT * sizeof(time_shifted_svc_evt_t);
  ret_mem = lib_memp_create(&p_nvod->ref_evt_memp, (u32)p_buffer, non_fix_size);
  MT_ASSERT(ret_mem == SUCCESS);
}

void nvod_data_init(void)
{
  nvod_db_t *p_nvod = NULL;

  p_nvod = class_get_handle_by_id(NVOD_CLASS_ID);
  if (p_nvod == NULL)
  {
    //Create EPG db handle
    p_nvod = mtos_malloc(sizeof(nvod_db_t));
    memset(p_nvod, 0, sizeof(nvod_db_t));
    MT_ASSERT(p_nvod != NULL);   
    //Register handle
    class_register(NVOD_CLASS_ID, p_nvod);
  }
  else
  {
    memset(p_nvod->svc, 0, MAX_NVOD_REFERENCE_SVC * sizeof(nvod_reference_svc_t));
  }
  p_nvod->cur_ref_svc_idx = -1;
  p_nvod->p_ref_evt_head = NULL;
  nvod_mem_alloc(p_nvod);
  p_nvod->nvod_svc_cnt = 0;
  p_nvod->total_ref_evt_cnt = 0;
}

void nvod_data_deinit(void *p_data)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  
  if (p_nvod == NULL)
  {
    return ;
  }

  if (p_nvod->p_buffer)
  {
    mtos_free(p_nvod->p_buffer);
    p_nvod->p_buffer = NULL;
  }
}

void nvod_data_set_locked_tp_info(void *p_data, u32 freq, u32 sym, u32 mod)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;

  MT_ASSERT(p_nvod != NULL);
  
  p_nvod->locked_tp.frequency = freq;
  p_nvod->locked_tp.symbol_rate = sym;
  p_nvod->locked_tp.modulation = mod;
}

nvod_db_status_t nvod_data_set_shift_svc(void *p_data, pmt_t *p_pmt)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  time_shifted_svc_t *p_shift_svc = NULL;
  u8 loopi = 0;
  u8 loopj = 0;

  MT_ASSERT(p_nvod != NULL);
  
  for (loopi = 0; loopi < p_nvod->nvod_svc_cnt; loopi ++)
  {
    p_shift_svc = (time_shifted_svc_t *)(p_nvod->svc[loopi].time_shifted_svc);
    for (loopj = 0; loopj < p_nvod->svc[loopi].time_shifted_svc_cnt; loopj ++)
    {
      if (p_shift_svc[loopj].svc_id == p_pmt->prog_num)
      {
        p_shift_svc[loopj].pmt_pid = p_pmt->pmt_pid;
        p_shift_svc[loopj].video_pid = p_pmt->video_pid;
        p_shift_svc[loopj].pcr_pid = p_pmt->pcr_pid;
        p_shift_svc[loopj].audio_ch_num = p_pmt->audio_count;
        p_shift_svc[loopj].v_type = p_pmt->video_type;
        p_shift_svc[loopj].audio_id = p_pmt->audio[0].p_id;
        p_shift_svc[loopj].a_type = p_pmt->audio[0].type;
        return NVOD_DB_SHIFT_SVC_FOUND;
      }
    }
  }
  
  return NVOD_DB_NORM;
}

nvod_db_status_t nvod_data_add_svc(void *p_data, sdt_t *p_sdt)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_t *p_svc = NULL;
  sdt_svc_descriptor_t *p_svc_des = NULL;
  nvod_reference_descr_t *p_nvod_des = NULL;
  u8 loopi = 0;
  u8 loopj = 0;
  
  MT_ASSERT(p_nvod != NULL);
  
  for (loopi = 0; loopi < p_sdt->svc_count; loopi ++)
  {
    p_svc_des = &p_sdt->svc_des[loopi];
    switch (p_svc_des->service_type)
    {
    case DVB_NVOD_REF_SVC:
      {
        if (p_nvod->nvod_svc_cnt >= MAX_NVOD_REFERENCE_SVC)
        {
          return NVOD_DB_OVERFLOW;
        }
        
        for (loopj = 0; loopj < p_nvod->nvod_svc_cnt; loopj ++)
        {
          if (p_nvod->svc[loopj].svc_id == p_svc_des->svc_id
            && p_svc_des->svc_id)
          {
            return NVOD_DB_NORM;
          }
        }

        p_svc = &p_nvod->svc[loopj];

        p_svc->tp.frequency = p_nvod->locked_tp.frequency;
        p_svc->tp.symbol_rate = p_nvod->locked_tp.symbol_rate;
        p_svc->tp.modulation = p_nvod->locked_tp.modulation;
        if (p_svc_des->nvod_reference_des_found)
        {
          p_svc->time_shifted_svc_cnt = p_svc_des->nvod;
          p_nvod_des = (nvod_reference_descr_t *)p_svc_des->p_nvod_mosaic_des;
          for (loopj = 0; loopj < p_svc_des->nvod; loopj ++)
          {
            p_svc->time_shifted_svc[loopj].orig_network_id = p_nvod_des[loopj].orig_network_id;
            p_svc->time_shifted_svc[loopj].stream_id = p_nvod_des[loopj].stream_id;
            p_svc->time_shifted_svc[loopj].svc_id = p_nvod_des[loopj].svc_id;
          }
        }
        dvb_to_unicode(p_svc_des->name, MAX_SVC_NAME_LEN - 1, p_svc->name, MAX_SVC_NAME_LEN);
        p_nvod->nvod_svc_cnt ++;
        p_svc->svc_id = p_svc_des->svc_id;
        return NVOD_DB_REF_SVC_FOUND;
      }
      
    case DVB_NVOD_TIME_SHIFTED_SVC:
      break;
      
    default:
      break;
    }
  }
  
  return NVOD_DB_NORM;
}

nvod_db_status_t nvod_data_add_evt(void *p_data, eit_t *p_eit_info)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  u32 shift_svc_idx = 0;
  
  if (!p_eit_info->tot_evt_num)
  {
    return NVOD_DB_NORM;
  }

  switch (eit_info_svc_type(p_data, p_eit_info, &shift_svc_idx))
  {
  case DVB_NVOD_REF_SVC:
    if (p_eit_info->table_id != DVB_TABLE_ID_EIT_ACTUAL
      && p_eit_info->table_id != DVB_TABLE_ID_EIT_OTHER)
    {
      return NVOD_DB_NORM;
    }
    if (add_ref_svc_evt(p_data, p_eit_info))
    {
      return NVOD_DB_REF_EVT_UPDATE;
    }
    break;
    
  case DVB_NVOD_TIME_SHIFTED_SVC:
    if (p_nvod->svc[p_nvod->cur_ref_svc_idx].reference_evt_cnt == 0)
    {
      return NVOD_DB_NORM;
    }
    
    if (add_shift_svc_evt(p_data, p_eit_info, shift_svc_idx))
    {
      return NVOD_DB_SHIFT_EVT_UPDATE;
    }
    break;
    
  default:
    break;
  }
  
  return NVOD_DB_NORM;
}

void nvod_data_delete_overdue(void *p_data)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_evt_t *p_ref_evt = NULL;
  time_shifted_svc_evt_t *p_shift_evt = NULL;
  utc_time_t sys_time = {0};
  utc_time_t end_time = {0};
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_nvod != NULL);

  time_get(&sys_time, TRUE);

  p_ref_evt = p_nvod->p_ref_evt_head;
  while (p_ref_evt)
  {
    if (p_ref_evt->shift_evt_cnt
      && p_ref_evt->ref_svc_idx == p_nvod->cur_ref_svc_idx)
    {
      p_shift_evt = p_ref_evt->p_shift_evt;
      MT_ASSERT(p_shift_evt != NULL);
      
      memcpy(&end_time, &p_shift_evt->start_time, sizeof(utc_time_t));
      evt_tadd(&end_time, &p_shift_evt->drt_time);

      /*!Compare end time of system time*/
      if (EVT_TCMP(&end_time, &sys_time) != 1)
      {
        p_ref_evt->p_shift_evt = p_shift_evt->p_next_evt;
        p_ref_evt->shift_evt_cnt --;
        ret = lib_memf_free(&p_nvod->shift_evt_memf, p_shift_evt);
        MT_ASSERT(ret == SUCCESS);
      }
    }
    
    p_ref_evt = p_ref_evt->p_next_evt;
  }
  
}

void nvod_data_delete_all(void *p_data)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_evt_t *p_ref_evt = NULL;
  time_shifted_svc_evt_t *p_shift_evt = NULL;
  RET_CODE ret = SUCCESS;
  
  MT_ASSERT(p_nvod != NULL);
  
  p_ref_evt = p_nvod->p_ref_evt_head;
  while (p_ref_evt)
  {
    while (p_ref_evt->shift_evt_cnt && p_ref_evt->p_shift_evt)
    {
      p_shift_evt = p_ref_evt->p_shift_evt;

      p_ref_evt->p_shift_evt = p_shift_evt->p_next_evt;
      p_ref_evt->shift_evt_cnt --;
      ret = lib_memf_free(&p_nvod->shift_evt_memf, p_shift_evt);
      MT_ASSERT(ret == SUCCESS);
    }
    p_nvod->p_ref_evt_head = p_ref_evt->p_next_evt;
    ret = lib_memp_free(&p_nvod->ref_evt_memp, p_ref_evt);
    MT_ASSERT(ret == SUCCESS);
    p_ref_evt = p_ref_evt->p_next_evt;
  }
  memset(p_nvod->svc, 0, MAX_NVOD_REFERENCE_SVC * sizeof(nvod_reference_svc_t));
  p_nvod->nvod_svc_cnt = 0;
  p_nvod->cur_ref_svc_idx = -1;
  p_nvod->total_ref_evt_cnt = 0;
  p_nvod->p_ref_evt_head = NULL;
}

dvbc_tp_info_t *nvod_data_get_locked_tp_info(void *p_data)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;

  MT_ASSERT(p_nvod != NULL);

  return &p_nvod->locked_tp;
}

u32 nvod_data_get_ref_svc_cnt(void *p_data)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;

  MT_ASSERT(p_nvod != NULL);
  
  return p_nvod->nvod_svc_cnt;
}

nvod_reference_svc_t *nvod_data_get_ref_svc(void *p_data, u32 index)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;

  MT_ASSERT(p_nvod != NULL);
  MT_ASSERT(index < p_nvod->nvod_svc_cnt);

  return &(p_nvod->svc[index]);
}

nvod_reference_svc_evt_t *nvod_data_get_ref_evt(void *p_data,
  nvod_reference_svc_t *p_ref_svc, u32 *p_cnt)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_evt_t *p_next = NULL;
  
  MT_ASSERT(p_cnt != NULL);
  
  if (p_ref_svc == NULL)
  {
    (*p_cnt) = p_nvod->total_ref_evt_cnt;
    return p_nvod->p_ref_evt_head;
  }
  
  (*p_cnt) = p_ref_svc->reference_evt_cnt;
  p_next = p_nvod->p_ref_evt_head;

  while (p_next)
  {
    if (p_nvod->svc[p_next->ref_svc_idx].svc_id == p_ref_svc->svc_id)
    {
      return p_next;
    }
    p_next = p_next->p_next_evt;
  }
  return NULL;
}

nvod_reference_svc_evt_t *nvod_data_get_ref_evt_next(void *p_data,
  nvod_reference_svc_evt_t *p_evt)
{
  MT_ASSERT(p_evt != NULL);

  return p_evt->p_next_evt;
}

time_shifted_svc_evt_t *nvod_data_get_shift_evt(void *p_data,
  nvod_reference_svc_evt_t *p_ref_evt, u32 *p_shift_evt_cnt)
{
  MT_ASSERT(p_ref_evt != NULL);
  MT_ASSERT(p_shift_evt_cnt != NULL);

  (*p_shift_evt_cnt) = p_ref_evt->shift_evt_cnt;
  return p_ref_evt->p_shift_evt;
}

time_shifted_svc_evt_t *nvod_data_get_shift_evt_next(void *p_data,
  time_shifted_svc_evt_t *p_evt)
{
  MT_ASSERT(p_evt != NULL);

  return p_evt->p_next_evt;
}

time_shifted_svc_t *nvod_data_get_shift_evt_svc(void *p_data,
  nvod_reference_svc_evt_t *p_ref_evt, time_shifted_svc_evt_t *p_evt)
{
  nvod_db_t *p_nvod = (nvod_db_t *)p_data;
  nvod_reference_svc_t *p_svc = NULL;
  
  MT_ASSERT(p_ref_evt != NULL);
  MT_ASSERT(p_evt != NULL);

  p_svc = (nvod_reference_svc_t *)(&p_nvod->svc[p_ref_evt->ref_svc_idx]);
  return &(p_svc->time_shifted_svc[p_evt->time_shifted_svc_idx]);
}

/*!
  end of file
  */
