/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
#include "dvb_protocol.h"
#include "mt_time.h"
#include "epg_type.h"
#include "epg_database.h"
#include "epg_database_priv.h"
#include "lib_unicode.h"
#include "err_check_def.h"

//#define EPG_DEBUG

#ifdef  EPG_DEBUG
#define EB_PRINT     OS_PRINTF(" %s , %d lines", __FUNCTION__, __LINE__);OS_PRINTF
#define PRINT_TIME   printf_time
#else
#define EB_PRINT     DUMMY_PRINTF
#define PRINT_TIME   DUMMY_PRINTF
#endif


/*!
 define epg db waring
 */
#define EPG_DB_WARN(x)                                \
  if((x) != TRUE)                                     \
  {                                                   \
    OS_PRINTF("%s, %d\n", __FUNCTION__, __LINE__);  \
  }

/*!
 define epg db error
 */
#define EPG_DB_ERROR(x)                               \
  if((x) != TRUE)                                     \
  {                                                   \
    OS_PRINTF("%s, %d\n", __FUNCTION__, __LINE__);    \
  }



#define ONE_TABLE_SECTION_NUM   (256)

#define ONE_SEGMENT_SECTION_NUM (8)

#define ONE_TABLE_SEGMENT_NUM   (32)

/*!
 declaration
 */
static void _epg_db_delete_event(epg_event_t evt_type,
                                epg_event_note_t note_type,
                                u32 context);
static event_node_t *_abnormal_proc_event_node(epg_db_priv_t *p_priv,
                                                                              epg_eit_t *p_epg_eit);
static event_node_t *_new_event_node(epg_db_priv_t *p_priv);
static void _fixed_event_node_to_svc(epg_db_priv_t *p_priv,
                                                svc_node_t    *p_svc_node);
static svc_node_t *_get_svc_node(epg_db_priv_t *p_priv,
                                 u16            network_id,
                                 u16            ts_id,
                                 u16            svc_id);
event_id_info_t _check_pf_event_id(svc_node_t  *p_svc_node,
                              epg_event_note_t    event_note_type,
                              u16 evt_id,
                              event_node_t **p_pf_event_node);

event_id_info_t _check_schedule_event_id(svc_node_t   *p_svc_node, u16 evt_id,
                                                      event_node_t **p_sch_event_node);
static extern_event_t *_get_extern_text_event(epg_eit_t *p_epg_eit, u16 event_id);
static text_event_t *_get_text_event(epg_eit_t *p_epg_eit, u16 event_id);

/*!
  epg database filter lang code
  */
static void epg_db_filter_evt_lang_code(epg_eit_t *p_eit_info);

/*!
  epg databasetry lang code priority
  */
static BOOL epg_db_try_lang_code_priority(epg_eit_t *p_eit_info);
/*!
  epg database init lang code
  */
static void epg_db_init_lang_code(void);


/*!
 delete event node
 */
static void _del_event_node(epg_db_priv_t *p_priv, event_node_t *p_evt_node);
/*!
 epg database get private data.
 */
static epg_db_priv_t *_epg_db_get_priv(void)
{
  epg_db_priv_t *p_priv = (epg_db_priv_t *)class_get_handle_by_id(EPG_CLASS_ID);
  CHECK_FAIL_RET_NULL(p_priv != NULL);
  return p_priv;
}

/*!
  API for allocating memory in EPG database module
  \param[in] mem size memory size to be allocated with byte as unit
  \param[in] id partition id to be allocated
  */
static void *_malloc_nonfix(lib_memp_t *p_memp, u32 mem_size)
{
  return lib_memp_alloc(p_memp, mem_size);
}
   
/*!
3
  API for allocating memory in EPG database module
  \param[in] mem size memory size to be allocated with byte as unit
  \param[in] id partition id to be allocated
  */
static RET_CODE _free_nonfix(lib_memp_t *p_memp, void *p_buf)
{
  return lib_memp_free(p_memp, p_buf);
}

/*!
  API for allocating memory in EPG database by fix mode
  */
static void *_malloc_fix(lib_memf_t *p_memf)
{
  return lib_memf_alloc(p_memf);
}

/*!
  API for allocating memory in EPG database by fix mode
  */
static RET_CODE _free_fix(lib_memf_t *p_memf, void *p_buf)
{
  return lib_memf_free(p_memf, p_buf);
}

static BOOL _run_pf_policy_for_pf_event(epg_db_priv_t *p_priv,
                                                 epg_eit_t     *p_epg_eit)
{
   BOOL ret = FALSE;
  epg_event_note_t note_type = EPG_ALL_PF_EVENT;
  u32 context = 0;
  /***the policy is  will deleted EPG_ALL_PF_EVENT***/
  note_type = EPG_ALL_PF_EVENT;
  context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;
  
   if(p_priv->pf_policy == PF_EVENT_ALL)
    {
       ret = TRUE;
    }
   else if((p_priv->pf_policy == PF_EVENT_OF_NETWORK_ID) 
      && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id))
    {
       _epg_db_delete_event(EPG_NETWORK_ID_EVENT,note_type, context);
       ret = TRUE;
    }
   else if((p_priv->pf_policy == PF_EVENT_OF_TS_ID) 
         && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
         && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
    {
       _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT,note_type,context);
       ret = TRUE;
    }
  else if((p_priv->pf_policy == PF_EVENT_OF_SVC_ID) 
        && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
        _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT | EPG_SVC_ID_EVENT,
                                                   note_type,context);
       ret = TRUE;
    }
   return ret;
}
static event_node_t *_special_proc_pf_event_node(epg_db_priv_t *p_priv,epg_eit_t *p_epg_eit)
{
  event_node_t *p_evt_node = NULL;
  u8 i = 0;
  u32 context = 0;

  /*****delete other time sch*****/
  context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;
  
  _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT | EPG_SVC_ID_EVENT |
                                            EPG_SVC_ID_TIME_EVENT,EPG_SCHEDULE_EVENT, context);
  p_evt_node = _new_event_node(p_priv);
  if(NULL != p_evt_node)
  {
    return p_evt_node;
  }
    /*****delete all svc sch*****/
  _epg_db_delete_event(EPG_UNKNOWN_EVENT ,EPG_SCHEDULE_EVENT, context);
  p_evt_node = _new_event_node(p_priv);
  if(NULL != p_evt_node)
  {
    return p_evt_node;
  }

  /*****auto adjust pf event policy*************/
  for(i = PF_EVENT_ALL ;i <= PF_EVENT_OF_SVC_ID;i ++)
    {
       _run_pf_policy_for_pf_event(p_priv,p_epg_eit);
       p_evt_node = _new_event_node(p_priv);
       if(NULL != p_evt_node)
        {
          return p_evt_node;
        }
       if(p_priv->pf_policy == PF_EVENT_OF_SVC_ID)
        {
           break;
        }
        p_priv->pf_policy = p_priv->pf_policy + 1;
        if(p_priv->pf_policy >= PF_EVENT_OF_SVC_ID)
        {
            p_priv->pf_policy = PF_EVENT_OF_SVC_ID;
        }
    }

  if(p_evt_node == NULL)
  {
      EB_PRINT("!!!!!event node overflow, enter to special pf process!!!!"
               "max event node number %d, schedule policy is chang ts svc\n");
  }
  return p_evt_node;
}
/*!
 el_external_part_time_svc_node
 */
static BOOL _del_external_part_time_svc_node(epg_db_priv_t *p_priv,svc_node_t  *p_svc_node)
{
  event_node_t *p_curr_event = NULL;
  event_node_t *p_pre_event = NULL;
  event_node_t *p_temp_event = NULL;
  utc_time_t    end_time     = {0};
  u32 cnk = 0;
  if(p_svc_node == NULL)
  {
    return FALSE;
  }

  p_pre_event = p_svc_node->p_sch_evt_head;
  p_curr_event = p_svc_node->p_sch_evt_head;
  //delete schedule event node
  while(p_curr_event != NULL)
  {     
    end_time = p_curr_event->start_time;
    time_add(&end_time, &p_curr_event->drt_time);
    if((time_cmp(&p_priv->prog_info.start_time, &end_time, FALSE) >= 0)
        ||(time_cmp(&p_curr_event->start_time,&p_priv->prog_info.end_time, FALSE) >= 0))
    {
      if(p_curr_event == p_svc_node->p_sch_evt_head)
      {
        p_svc_node->p_sch_evt_head = p_curr_event->p_next_evt_node;
        p_pre_event = p_svc_node->p_sch_evt_head;
      }
      else
      {
         p_pre_event ->p_next_evt_node = p_curr_event->p_next_evt_node;
      }
      p_temp_event = p_curr_event;
      p_curr_event = p_curr_event->p_next_evt_node;
      _del_event_node(p_priv, p_temp_event);
    }
    else
    {
       p_pre_event = p_curr_event;
       p_curr_event = p_curr_event->p_next_evt_node;
    }

    cnk ++;
    CHECK_FAIL_RET_FALSE(cnk <= p_priv->evt_node_num);
  } 
  return TRUE;
}
/*
delete program svc during time last note
*/
static  BOOL _del_during_time_last_svc_node(epg_db_priv_t *p_priv,short_event_t *p_sht_evt)
{
   svc_node_t    *p_svc_node = NULL;
  event_node_t *p_cur_node = NULL;
  event_node_t *p_pre_node = NULL;
  event_node_t *p_next_node = NULL;
  s8 cmp_ret = 0;
  u32 cnk = 0;
EB_PRINT("last node start time is greate than new evt start time,it will delete last node\n");
/*!
  get svc node.
  */
  p_svc_node = _get_svc_node(p_priv,
                             p_priv->prog_info.network_id,
                             p_priv->prog_info.ts_id,
                             p_priv->prog_info.svc_id);
  if(p_svc_node == NULL)
  {
    return FALSE;
  }
  
  p_pre_node = p_svc_node->p_sch_evt_head;
   if(p_pre_node == NULL)  /***no node***/
  {
    return FALSE;
  }
   
  p_cur_node = p_pre_node->p_next_evt_node;
  if(p_cur_node == NULL) /****only one node,it is head****/
  {
     cmp_ret = time_cmp(&p_pre_node->start_time, 
                       &p_sht_evt->start_time,
                       FALSE);
     if(cmp_ret == 1)
    {
      _del_event_node(p_priv, p_pre_node);
      p_pre_node =  NULL;
      p_svc_node->p_sch_evt_head = NULL;
    }
     return TRUE;
  }

   p_next_node = p_cur_node->p_next_evt_node;
  while(p_next_node != NULL)  /*****find last head****/
  {
    p_pre_node = p_cur_node;
    p_cur_node = p_pre_node->p_next_evt_node;
    p_next_node = p_cur_node->p_next_evt_node;
    cnk ++;
    CHECK_FAIL_RET_FALSE(cnk <= p_priv->evt_node_num);
  }
  
  if(p_cur_node != NULL)
  {
      cmp_ret = time_cmp(&p_cur_node->start_time, 
                       &p_sht_evt->start_time,
                       FALSE);
     if(cmp_ret == 1)
    {
      _del_event_node(p_priv, p_cur_node);
      p_pre_node->p_next_evt_node = NULL;
    }
  }
  return TRUE;
}

static BOOL _run_sch_policy_for_sch_event(epg_db_priv_t *p_priv,
                                                              epg_eit_t     *p_epg_eit)
{
   BOOL ret = FALSE;
  epg_event_note_t note_type = EPG_ALL_EVENT;
  u32 context = 0;
  context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;
  
  if(p_priv->del_pf_able == TRUE)  
    {
       note_type = EPG_ALL_EVENT;
    }
  else
    {
      note_type = EPG_SCHEDULE_EVENT;
    }

   if(p_priv->sch_policy == SCH_EVENT_ALL)
    {
       ret = TRUE;
    }
   else if((p_priv->sch_policy == SCH_EVENT_OF_NETWORK_ID) 
      && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id))
    {
       _epg_db_delete_event(EPG_NETWORK_ID_EVENT,note_type, context);
       ret = TRUE;
    }
   else if((p_priv->sch_policy == SCH_EVENT_OF_TS_ID) 
         && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
         && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
    {
       _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT,note_type,context);
       ret = TRUE;
    }
  else if((p_priv->sch_policy == SCH_EVENT_OF_SVC_ID) 
        && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
        _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT | EPG_SVC_ID_EVENT,
                                                   note_type,context);
       ret = TRUE;
    }
  else if((p_priv->sch_policy == SCH_EVENT_OF_TIME_ID) 
        && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
         /***delete same svc other time sch or all event***/
    _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT | EPG_SVC_ID_EVENT |
                                            EPG_SVC_ID_TIME_EVENT,note_type,context);
       ret = TRUE;
    }
   return ret;
}
 static event_node_t *_special_proc_sch_event_node(epg_db_priv_t *p_priv,
                                                    epg_eit_t *p_epg_eit,
                                                    u8 index)
{
  event_node_t *p_evt_node = NULL;
  short_event_t *p_sht_evt  = NULL;
  u8 i = 0;
  u32 context = 0;
  context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;
  p_sht_evt = &p_epg_eit->sht_evt_list[index];
  
  for(i = SCH_EVENT_ALL ;i <= SCH_EVENT_OF_TIME_ID;i ++)
    {
       _run_sch_policy_for_sch_event(p_priv,p_epg_eit);
       p_evt_node = _new_event_node(p_priv);
       if(NULL != p_evt_node)
        {
          return p_evt_node;
        }
       if(p_priv->sch_policy == SCH_EVENT_OF_TIME_ID)
        {
           break;
        }
        p_priv->sch_policy = p_priv->sch_policy + 1;
        if(p_priv->sch_policy >= SCH_EVENT_OF_TIME_ID)
        {
            p_priv->sch_policy = SCH_EVENT_OF_TIME_ID;
        }
    }
  /*delete program svc during time last note*/
  if(p_evt_node == NULL)
  {
    _del_during_time_last_svc_node(p_priv,p_sht_evt);
    p_evt_node = _new_event_node(p_priv);
    if(NULL == p_evt_node)
      {
         if(p_epg_eit->have_pf_pin != TRUE)  /**no pf table status***/
        {
            /***if it no have pf table request,it will delete all pf event*******/
            _epg_db_delete_event(EPG_UNKNOWN_EVENT,EPG_ALL_PF_EVENT, context);
            p_evt_node = _new_event_node(p_priv);
        }
      }
  }
  return p_evt_node;
}


/*!
 new event node
 */
static event_node_t *_new_event_node(epg_db_priv_t *p_priv)
{
  event_node_t *p_evt_node = NULL;

  p_evt_node = _malloc_fix(&p_priv->evt_node_memf);
  if(p_evt_node == NULL)
  {
    //p_evt_node =_abnormal_proc_event_node(p_priv);
   // EPG_DB_WARN(p_evt_node != NULL)
   return NULL;
  }
  
  if(p_evt_node != NULL)
  {
    memset(p_evt_node, 0x0, sizeof(event_node_t));
  }

  return p_evt_node;
}

static event_node_t *_abnormal_proc_event_node(epg_db_priv_t *p_priv,epg_eit_t *p_epg_eit)
{
  event_node_t *p_evt_node = NULL;
  u32 context = 0;
  context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;
  _epg_db_delete_event(EPG_OVERDUE_EVENT,EPG_ALL_EVENT, context);
  _epg_db_delete_event(EPG_SVC_ID_FIXD_EVENT,EPG_SCHEDULE_EVENT,context);
  p_evt_node = _new_event_node(p_priv);
  if(p_evt_node == NULL)
  {
    EB_PRINT("\n!!!!!event node overflow,max event node number %d\n",p_priv->evt_node_num);
  }
  return p_evt_node;
}
/*!
 delete event node text
 */
static void _del_event_node_text(epg_db_priv_t *p_priv, event_node_t *p_evt_node)
{
  //has detail information

  if(p_evt_node->p_sht_text != NULL)
  {
    _free_nonfix(&p_priv->memp, p_evt_node->p_sht_text);
    p_evt_node->p_sht_text = NULL;
  }

  if(p_evt_node->p_ext_desc_list != NULL)
  {
    _free_nonfix(&p_priv->memp, p_evt_node->p_ext_desc_list);
    p_evt_node->p_ext_desc_list =  NULL;
    p_evt_node->count_ext_desc = 0;
  }
}

/*!
 delete event node
 */
static void _del_event_node(epg_db_priv_t *p_priv, event_node_t *p_evt_node)
{
  //has detail information

  if(p_evt_node->p_sht_text != NULL)
  {
    _free_nonfix(&p_priv->memp, p_evt_node->p_sht_text);
  }

  if(p_evt_node->p_ext_desc_list != NULL)
  {
    _free_nonfix(&p_priv->memp, p_evt_node->p_ext_desc_list);
  }

  //OS_PRINTF("\n db : event node id %d, name %s\n", p_evt_node->event_id, p_evt_node->event_name);
 
  _free_fix(&p_priv->evt_node_memf, p_evt_node);
  p_priv->evt_used_node_num --;
}

/*!
 new servcie node
 */
static svc_node_t *_new_svc_node(epg_db_priv_t *p_priv, u16 svc_id)
{
  svc_node_t *p_svc_node = NULL;

  p_svc_node = _malloc_fix(&p_priv->svc_node_memf);
  if(p_svc_node == NULL)
  {
    //p_svc_node = _abnormal_proc_svc_node(p_priv);
    //CHECK_FAIL_RET_NULL(p_svc_node != NULL);
    return  NULL;
  }

  //init parameter.
  p_svc_node->svc_id          = svc_id;
  p_svc_node->p_next_svc_node = NULL;
  p_svc_node->p_sch_evt_head  = NULL;
  p_svc_node->p_present_evt   = NULL;
  p_svc_node->p_follow_evt    = NULL; 
  return p_svc_node;
}

static void _free_svc_node(epg_db_priv_t *p_priv,svc_node_t  *p_svc_node)
{
 _free_fix(&p_priv->svc_node_memf, p_svc_node);
  p_svc_node = NULL;
}


void printf_svc_node_info(epg_db_priv_t *p_priv)
{
    network_node_t *p_cur_net_node = NULL;
    ts_node_t *p_cur_ts_node = NULL;
    svc_node_t *p_curr_svc_node = NULL;
    u32 cnk = 0;
    p_cur_net_node = p_priv->p_net_head;
    while(p_cur_net_node != NULL)
    {
        OS_PRINTF("***************net id = 0x%x\n",p_cur_net_node->network_id);
        p_cur_ts_node = p_cur_net_node->p_ts_head;
        while (p_cur_ts_node != NULL)
        {
          OS_PRINTF("*******ts_id = 0x%x\n",p_cur_ts_node->ts_id);
          p_curr_svc_node = p_cur_ts_node->p_svc_head;
          while(p_curr_svc_node != NULL)
          {
            OS_PRINTF("svc node info svc_id = 0x%x\n",p_curr_svc_node->svc_id);
            p_curr_svc_node = p_curr_svc_node->p_next_svc_node;
            cnk ++;
          }
          p_cur_ts_node = p_cur_ts_node->p_next_ts;
        }
        p_cur_net_node = p_cur_net_node->p_next_net;
    }
       
  OS_PRINTF("svc have cnk = %d\n",cnk);
}
static svc_node_t *_abnormal_proc_svc_node(epg_db_priv_t *p_priv,
                                                            network_node_t *p_net_node,
                                                            ts_node_t     *p_ts_node,
                                                            u16 svc_id)
{
  svc_node_t *p_new_node = NULL;
  svc_node_t *p_curr_node = NULL;
  svc_node_t *p_pre_node = NULL;
  u32 cnk = 0;
  u32 context = 0;

   if((p_net_node == NULL) || (p_ts_node == NULL))
    {
       return NULL;
    }
   
  if((p_priv->prog_info.network_id != p_net_node->network_id) || (p_priv->prog_info.network_id== EPG_IGNORE_ID))
    {
      return NULL;
    }

  if((p_priv->prog_info.ts_id != p_ts_node->ts_id) || (p_priv->prog_info.ts_id== EPG_IGNORE_ID))
    {
      return NULL;
    }

   if((p_priv->prog_info.svc_id != svc_id) || (p_priv->prog_info.svc_id == EPG_IGNORE_ID))
  {
    return NULL;
  }

 
  context = ((EPG_DB_KEEP_TYPE_TS << 16) & 0xFFFF0000) | p_ts_node->ts_id;
  
  _epg_db_delete_event(EPG_NETWORK_ID_EVENT,EPG_ALL_EVENT,  context);
  p_new_node = _new_svc_node(p_priv,svc_id);
  if(p_new_node == NULL)
  {
       _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT,EPG_ALL_EVENT,  context);
       p_new_node = _new_svc_node(p_priv,svc_id);
      if(p_new_node == NULL)
      {
          _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT|EPG_SVC_ID_EVENT,
                                                     EPG_ALL_EVENT,context);
          p_new_node = _new_svc_node(p_priv,svc_id);
      }
  }
  
  if(p_new_node != NULL)
  {
      p_curr_node = p_ts_node->p_svc_head;
      p_pre_node = p_ts_node->p_svc_head;
      while(p_curr_node != NULL)
      {
        p_pre_node = p_curr_node;
        p_curr_node = p_curr_node->p_next_svc_node;
        cnk ++;
        CHECK_FAIL_RET_NULL(cnk <= p_priv->svc_node_num);
      }

      if(p_ts_node->p_svc_head == NULL)
      {
        p_ts_node->p_svc_head = p_new_node;
      }
      else
      {
        p_pre_node->p_next_svc_node = p_new_node;
      }
  }
  else
  {
    EB_PRINT("max svc node number %d\n",p_priv->svc_node_num);
  }
  return p_new_node;
}
/*!
 delete service node text
 */
 static BOOL _del_svc_node_text(epg_db_priv_t *p_priv,
                          svc_node_t    *p_svc_node,
                          u8             event_type,
                          u32            context)
{
    event_node_t *p_curr_event = p_svc_node->p_sch_evt_head;
    event_node_t *p_temp_event = NULL;
    utc_time_t     event_end_time   = { 0 };
    u32 cnk =0;
    if(IS_SCHEDULE_EVENT(event_type))
    {
      while(p_curr_event != NULL)
      {
        p_temp_event = p_curr_event;
        p_curr_event = p_curr_event->p_next_evt_node;
        if(context == 0x01) /**p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_TIME_ID)**/
        {
          memcpy(&event_end_time, &p_temp_event->start_time, sizeof(utc_time_t));
          time_add(&event_end_time,&p_temp_event->drt_time);
          if((time_cmp(&p_temp_event->start_time, &p_priv->prog_info.end_time, FALSE) >= 0)
              ||(time_cmp(&p_priv->prog_info.start_time,&event_end_time ,FALSE) >= 0))
          {
             _del_event_node_text(p_priv, p_temp_event);
          }
        }
        else  /***all svc node text***/
        {
            _del_event_node_text(p_priv, p_temp_event);
        }
        cnk ++;
        CHECK_FAIL_RET_FALSE(cnk <= p_priv->evt_node_num);
      }
    }

    if(context == 0x01)
      {
         return FALSE;
      }
    //delete present event node
    if(IS_PRESENT_EVENT(event_type) 
      && (p_svc_node->p_present_evt != NULL))
    {
      OS_PRINTF("###debug delete policy pf event text\n");
      _del_event_node_text(p_priv, p_svc_node->p_present_evt);
    }

    //delete following event node.
    if((IS_FOLLOW_EVENT(event_type)) 
      && (p_svc_node->p_follow_evt != NULL))
    {
      _del_event_node_text(p_priv, p_svc_node->p_follow_evt);
    }
    return FALSE;
}
/*!
 delete service node
 */
static BOOL _del_svc_node(epg_db_priv_t *p_priv,
                          svc_node_t    *p_svc_node,
                          epg_event_t    event_type,
                          epg_event_note_t note_type, 
                          u32            context)
{
  event_node_t *p_curr_event = p_svc_node->p_sch_evt_head;
  event_node_t *p_temp_event = NULL;
  u32 cnk =0;
  u16 keep_type = (context >> 16) & 0xffff;
  u16 keep_id = context & 0xffff;
  
  if(IS_SVC_ID_TIME_EVENT(event_type)) /***the same svc,but is not work time****/
  { 
    if(IS_EPG_TEXT_EVENT(note_type))
      {
        _del_svc_node_text(p_priv,p_svc_node,note_type,1);
          return FALSE;
      }
     /*****it is delete same svc other time node,but it is not text*****/
     else if(IS_SCHEDULE_EVENT(note_type))

      {
        _del_external_part_time_svc_node(p_priv,p_svc_node);
      }


    if((keep_type == EPG_DB_KEEP_TYPE_SVC) && (keep_id == p_svc_node->svc_id))
      {
         return FALSE;
      }
    
    if(((p_svc_node->p_sch_evt_head == NULL) &&
      (p_svc_node->p_present_evt == NULL) &&
      (p_svc_node->p_follow_evt == NULL)))
      {
        _free_svc_node(p_priv,p_svc_node);
        return TRUE;
      }
      
    return FALSE;
  }
 else 
  {
    if(IS_EPG_TEXT_EVENT(note_type))
    {
        _del_svc_node_text(p_priv,p_svc_node,note_type,0);
        return FALSE;
    }
    //delete schedule event node
    if(IS_SCHEDULE_EVENT(note_type))
    {
      while(p_curr_event != NULL)
      {
        p_temp_event = p_curr_event;
        p_curr_event = p_curr_event->p_next_evt_node;
        _del_event_node(p_priv, p_temp_event);
        cnk ++;
        CHECK_FAIL_RET_FALSE(cnk <= p_priv->evt_node_num);
      }
      p_svc_node->p_sch_evt_head = NULL;
    }

    //delete present event node
    if(IS_PRESENT_EVENT(note_type) 
      && (p_svc_node->p_present_evt != NULL))
    {
      OS_PRINTF("###debug delete policy pf event\n");
      _del_event_node(p_priv, p_svc_node->p_present_evt);
      p_svc_node->p_present_evt = NULL;
    }

    //delete following event node.
    if((IS_FOLLOW_EVENT(note_type)) 
      && (p_svc_node->p_follow_evt != NULL))
    {
      _del_event_node(p_priv, p_svc_node->p_follow_evt);
      p_svc_node->p_follow_evt = NULL;
    }

    if((keep_type == EPG_DB_KEEP_TYPE_SVC) && (keep_id == p_svc_node->svc_id))
      {
         return FALSE;
      }


    if(((p_svc_node->p_sch_evt_head == NULL) &&
        (p_svc_node->p_present_evt == NULL) &&
        (p_svc_node->p_follow_evt == NULL)))
    {
      _free_svc_node(p_priv,p_svc_node);
      return TRUE;
    }
  }
  return FALSE;
}
/*!
 fixd other svc event node
 return TRUE, delete all event, and svc_node.
 return FALSE, only delete some event.
 */
static BOOL _fixd_other_svc_node(epg_db_priv_t *p_priv,
                                  svc_node_t    *p_svc_node,
                                  epg_event_t    event_type,
                                  epg_event_note_t note_type, 
                                  u32            context)
{
    u16 keep_type = (context >> 16) & 0xffff;
    u16 keep_id = context & 0xffff;
      
   _fixed_event_node_to_svc(p_priv,p_svc_node);

  if((keep_type == EPG_DB_KEEP_TYPE_SVC) && (keep_id == p_svc_node->svc_id))
    {
       return FALSE;
    }
  if((p_svc_node->p_sch_evt_head == NULL)
    && (p_svc_node->p_present_evt == NULL)
    && (p_svc_node->p_follow_evt == NULL))
  {
    _free_svc_node(p_priv,p_svc_node);
    return TRUE;
  }
  return FALSE;
}
/*!
 delete overdue event node
 return TRUE, delete all event, and svc_node.
 return FALSE, only delete some event.
 */
static BOOL _del_overdue_svc_node(epg_db_priv_t *p_priv,
                                                svc_node_t    *p_svc_node,
                                                epg_event_t    event_type,
                                                epg_event_note_t node_type,
                                                u32            context)
{
  event_node_t *p_curr_event = p_svc_node->p_sch_evt_head;
  event_node_t *p_temp_event = NULL;
  utc_time_t    sys_time     = {0};
  utc_time_t    end_time     = {0};
  u32 cnk =0;
  u16 keep_type = (context >> 16) & 0xffff;
  u16 keep_id = context & 0xffff;

  time_get(&sys_time, TRUE);

  //delete schedule event node
  while(p_curr_event != NULL)
  {
    end_time = p_curr_event->start_time;
    time_add(&end_time, &p_curr_event->drt_time);

    if(time_cmp(&sys_time, &end_time, FALSE) >= 0)
    {
      if(p_curr_event == p_svc_node->p_sch_evt_head)
      {
        p_svc_node->p_sch_evt_head = p_curr_event->p_next_evt_node;
      }
      else
      {
         CHECK_FAIL_RET_FALSE(0); /**the case link will broken***/
      }
      p_temp_event = p_curr_event;
      p_curr_event = p_curr_event->p_next_evt_node;
      _del_event_node(p_priv, p_temp_event);
    }
    else //Because the list is sorted in time order
    {
      break; 
    }
    cnk ++;
    CHECK_FAIL_RET_FALSE(cnk <= p_priv->evt_node_num);
  }

#if 0
/******in play status,it do it,may be time no update ts*******/
if((p_priv->prog_info.network_id != EPG_IGNORE_ID)
    && (p_priv->prog_info.ts_id != EPG_IGNORE_ID)
    && (p_priv->prog_info.svc_id != EPG_IGNORE_ID))
{
  //delete present event node
  if((p_svc_node->p_present_evt != NULL) && (p_priv->del_pf_able == TRUE))
  {
    end_time = p_svc_node->p_present_evt->start_time;
    time_add(&end_time, &p_svc_node->p_present_evt->drt_time);

    if(time_cmp(&sys_time, &end_time, FALSE) >= 0)
    {
      _del_event_node(p_priv, p_svc_node->p_present_evt);
      p_svc_node->p_present_evt = NULL;
    }
  }

  //delete following event node.
  if((p_svc_node->p_follow_evt != NULL) && (p_priv->del_pf_able == TRUE))
  {
    end_time = p_svc_node->p_follow_evt->start_time;
    time_add(&end_time, &p_svc_node->p_follow_evt->drt_time);

    if(time_cmp(&sys_time, &end_time, FALSE) >= 0)
    {
      _del_event_node(p_priv, p_svc_node->p_follow_evt);
      p_svc_node->p_follow_evt = NULL;
    }
  }
  
}
#endif

    if((keep_type == EPG_DB_KEEP_TYPE_SVC) && (keep_id == p_svc_node->svc_id))
      {
         return FALSE;
      }

  if((p_svc_node->p_sch_evt_head == NULL)
    && (p_svc_node->p_present_evt == NULL)
    && (p_svc_node->p_follow_evt == NULL))
  {
    _free_svc_node(p_priv,p_svc_node);
    return TRUE;
  }
  return FALSE;
}


/*!
 new transport stream node
 */
static ts_node_t *_new_ts_node(epg_db_priv_t *p_priv, u16 ts_id)
{
  ts_node_t *p_ts_node = NULL;

  p_ts_node = _malloc_fix(&p_priv->ts_node_memf);
  if(p_ts_node == NULL)
  {
    //p_ts_node = _abnormal_proc_ts_node(p_priv, ts_id);
    //CHECK_FAIL_RET_NULL(p_ts_node != NULL);
    return NULL;
  }

  p_ts_node->ts_id = ts_id;
  p_ts_node->p_svc_head = NULL;
  p_ts_node->p_next_ts  = NULL;
  return p_ts_node;
}

static ts_node_t *_abnormal_proc_ts_node(epg_db_priv_t *p_priv, u16 ts_id,network_node_t *p_net_node)
{
  ts_node_t *p_new_node = NULL;
  ts_node_t *p_cur_node = NULL;
  ts_node_t *p_pre_node = NULL;
  u32 cnk = 0;
  u32 context = 0;

  if(p_net_node == NULL)
    {
       return NULL;
    }
  
  if((p_priv->prog_info.network_id != p_net_node->network_id) || (p_priv->prog_info.network_id== EPG_IGNORE_ID))
    {
      return NULL;
    }


  if((p_priv->prog_info.ts_id != ts_id) || (p_priv->prog_info.ts_id== EPG_IGNORE_ID))
    {
      return NULL;
    }
  
   context = ((EPG_DB_KEEP_TYPE_NET << 16) & 0xFFFF0000) | p_net_node->network_id;
   
  /*!
   * in the PF_EVENT_OF_TS_ID/PF_EVENT_OF_SVC_ID
   * and in the SCH_EVENT_OF_TS_ID/SCH_EVENT_OF_SVC_ID,
   * delete others network id + ts_id node.
   */
   _epg_db_delete_event(EPG_NETWORK_ID_EVENT,EPG_ALL_EVENT,context);
  p_new_node =  _new_ts_node(p_priv, ts_id);
  if(p_new_node == NULL)
  {
     _epg_db_delete_event(EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT,EPG_ALL_EVENT,  context);
     p_new_node =  _new_ts_node(p_priv, ts_id);
  }
  if(p_new_node != NULL)
  {
      p_cur_node = p_net_node->p_ts_head;
      p_pre_node = p_net_node->p_ts_head;
      while(p_cur_node != NULL)
      {
         p_pre_node = p_cur_node;
         p_cur_node = p_cur_node->p_next_ts;
         cnk ++;
         CHECK_FAIL_RET_NULL(cnk <= p_priv->ts_node_num);
      }
      p_cur_node = p_new_node;
      if(p_net_node->p_ts_head == NULL)
        {
          p_net_node->p_ts_head = p_cur_node;
        }
      else
      {
         p_pre_node->p_next_ts = p_cur_node;
      }
      return p_cur_node;
  }
    //CHECK_FAIL_RET_NULL(p_ts_node != NULL);
   // return p_ts_node;
 
  EB_PRINT("max ts node number %d\n", p_priv->ts_node_num);
  return p_cur_node;
}

/*!
 delete transport stream node.
 return TRUE, delete all svc, and ts_node.
 return FALSE, only delete some svc_node.
 */
static BOOL _del_ts_node(epg_db_priv_t *p_priv,
                                                 ts_node_t     *p_ts_node,
                                                 epg_event_t    event_type,
                                                 epg_event_note_t note_type, 
                                                 u32            context)
{
  svc_node_t *p_curr_svc = p_ts_node->p_svc_head;
  svc_node_t *p_next_svc = NULL;
  svc_node_t *p_pre_svc = p_ts_node->p_svc_head;
  BOOL        b_act_del  = FALSE;
  BOOL        b_head_svc = FALSE;
  u16         svc_id     = p_priv->prog_info.svc_id;
  u32 cnk = 0;
  u16 keep_type = (context >> 16) & 0xffff;
  u16 keep_id = context & 0xffff;

  while(p_curr_svc != NULL)
  {
    b_head_svc = (p_curr_svc == p_ts_node->p_svc_head) ? TRUE : FALSE;
    b_act_del  = FALSE;
    p_next_svc = p_curr_svc->p_next_svc_node;

   /*!
    * case 1 : delete all overdue events.
    * case 2 : delete others svc_id event :
    *    (1) : not match current svc_id, please delete EPG_ALL_EVENT.
    *    (2) : not save all current svc_id event, for example :
    *         only save current svc id + P/F or SCHEDULE. please clear svc_id bit.
    * case 3 : delete all ts_id event.
    */
    if(IS_OVERDUE_EVENT(event_type))//case 1
    {
      b_act_del = _del_overdue_svc_node(p_priv, p_curr_svc, 
                                                          event_type,
                                                          note_type, 
                                                          context);
    }
    else if(IS_SVC_ID_EVENT(event_type))//case 2
    {
      if(p_curr_svc->svc_id != svc_id)//case 2 - (1)
      {
      /**EPG_ALL_EVENT***/
        b_act_del = _del_svc_node(p_priv, p_curr_svc,
                EPG_UNKNOWN_EVENT,note_type, context);
      }
      else if(!ONLY_SVC_ID_EVENT(event_type)) //case 2 - (2)
      {
        b_act_del = _del_svc_node(p_priv, p_curr_svc, 
          CLEAR_SVC_ID_BIT(event_type),note_type, context);
      }
    }
    else if(FIXD_SVC_ID_EVENT(event_type))
    {
       //if(p_curr_svc->svc_id != svc_id)
        {
          b_act_del = _fixd_other_svc_node(p_priv,p_curr_svc,event_type,note_type,context);
        }
    }
    else//case 3
    {
      b_act_del = _del_svc_node(p_priv, p_curr_svc, event_type,note_type, context);
    }

   /*!
    if actual del svc_node, please move to ts_node->svc_list.
    + the head_svc_node, please move the head_svc to next svc_node.
    */
    if(b_act_del)
    {
      if(b_head_svc)
      {
        p_ts_node->p_svc_head = p_next_svc;
        p_pre_svc = p_ts_node->p_svc_head;
      }
      else
      {
        p_pre_svc->p_next_svc_node = p_next_svc;
      }
    }
    else
    {
      p_pre_svc = p_curr_svc;
    }

    p_curr_svc = p_next_svc;

    cnk ++;
    CHECK_FAIL_RET_FALSE(cnk <= p_priv->svc_node_num);
  }

  if((keep_type == EPG_DB_KEEP_TYPE_TS) && (keep_id == p_ts_node->ts_id))
    {
       return FALSE;
    }

  if(p_ts_node->p_svc_head == NULL)
  {
    _free_fix(&p_priv->ts_node_memf, p_ts_node);
    p_ts_node = NULL;
    return TRUE;
  }
  return FALSE;
}


/*!
 new network node
 */
static network_node_t *_new_network_node(epg_db_priv_t *p_priv, u16 network_id)
{
  network_node_t *p_network_node = NULL;

  p_network_node = _malloc_fix(&p_priv->net_node_memf);
  if(p_network_node == NULL)
  {
    return NULL;
  }

  p_network_node->network_id = network_id;
  p_network_node->p_ts_head  = NULL;
  p_network_node->p_next_net = NULL;
  return p_network_node;
}
/*!
 * abnormal process network node.
 */
static network_node_t *_abnormal_proc_net_node(epg_db_priv_t *p_priv, u16 network_id)
{
  network_node_t *p_net_node = NULL;
  network_node_t *p_cur_node = NULL;
  network_node_t *p_new_node = NULL;
  network_node_t *p_pre_node = NULL;
  u32 cnk = 0;
  u32 context = 0;


   if((p_priv->prog_info.network_id != network_id) || (p_priv->prog_info.network_id == EPG_IGNORE_ID))
    {
      return NULL;
    }
   //context = ((EPG_DB_KEEP_TYPE_NET << 16) & 0xFFFF0000) | network_id;

  /*!
   *delete others network id, save cur network id
   */
  /*!
   *because _new_network_node now, so this network_id not has event.
   *can delete all event.
   */
   
   _epg_db_delete_event(EPG_NETWORK_ID_EVENT,EPG_ALL_EVENT,context);
   
   p_new_node =  _new_network_node(p_priv, network_id);
   if(p_new_node != NULL)
    {
      p_pre_node = p_priv->p_net_head;
      p_cur_node = p_priv->p_net_head;
      cnk = 0;
      while(p_cur_node != NULL)
      {
         p_pre_node = p_cur_node;
         p_cur_node = p_cur_node->p_next_net;
         cnk ++;
        CHECK_FAIL_RET_NULL(cnk <= p_priv->net_node_num);
      }
       p_cur_node = p_new_node;
       
      if(NULL == p_priv->p_net_head)
      {
          p_priv->p_net_head = p_cur_node;
      }
      else
      {
        p_pre_node->p_next_net = p_cur_node;
      }
      return p_cur_node;
    }
   
    EB_PRINT("network id number %d\n", p_priv->net_node_num);
    EB_PRINT("new network id %d\n", network_id);
    p_net_node = p_priv->p_net_head;
    cnk = 0;
    while(p_net_node != NULL)
    {
      EB_PRINT("old network id %d\n", p_net_node->network_id);
      p_net_node = p_net_node->p_next_net;
      cnk ++;
      CHECK_FAIL_RET_NULL(cnk <= p_priv->net_node_num);
    }
    return NULL;
}

/*!
 delete network node.
 */
static BOOL _del_network_node(epg_db_priv_t  *p_priv,
                              network_node_t *p_net_node,
                              epg_event_t     event_type,
                              epg_event_note_t note_type,
                              u32             context)
{
  ts_node_t *p_curr_ts = p_net_node->p_ts_head;
  ts_node_t *p_pre_ts = p_net_node->p_ts_head;
  ts_node_t *p_next_ts = NULL;
  BOOL       b_act_del = FALSE;
  BOOL       b_head_ts = FALSE;
  u16        ts_id     = p_priv->prog_info.ts_id;
  u32 cnk = 0;
  u16 keep_type = (context >> 16) & 0xffff;
  u16 keep_id = context & 0xffff;
  
  while(p_curr_ts != NULL)
  {
    b_head_ts = (p_curr_ts == p_net_node->p_ts_head) ? TRUE : FALSE;
    b_act_del  = FALSE;
    p_next_ts = p_curr_ts->p_next_ts;

   /*!
    * case 1: delete others ts_id event :
    *   (1) : not match current ts_id, please delete EPG_ALL_EVENT.
    *   (2) : not save all current ts_id event, for example :
    *         only save current ts id + svc id. please clear ts_id bit.
    * case 2: delete all ts_id event.
    */
    if(IS_TS_ID_EVENT(event_type))//case1
    {
      if(p_curr_ts->ts_id != ts_id)//case1 - (1)
      {
      /**EPG_ALL_EVENT***/
        b_act_del = _del_ts_node(p_priv, p_curr_ts,EPG_UNKNOWN_EVENT,note_type, context); 
      }
      else if(!ONLY_TS_ID_EVENT(event_type))//case1 - (2)
      {
        b_act_del = _del_ts_node(p_priv, p_curr_ts, CLEAR_TS_ID_BIT(event_type),note_type,context);
      }
    }
    else if(FIXD_SVC_ID_EVENT(event_type))
    {
       if(p_curr_ts->ts_id != ts_id)
        {
            b_act_del = _del_ts_node(p_priv, p_curr_ts, event_type,note_type, context);
        }
    }
    else//case 2
    {
      b_act_del = _del_ts_node(p_priv, p_curr_ts, event_type,note_type, context);
    }

   /*!
    if actual del svc_node, please move to ts_node->svc_list.
    + the head_svc_node, please move the head_svc to next svc_node.
    */
     if(b_act_del)/***if delete current ts***/
      {
         if(b_head_ts) /***it is head***/
          {
             p_net_node->p_ts_head = p_next_ts;
             p_pre_ts = p_net_node->p_ts_head ;
          }
         else  /***link next ts***/
          {
             p_pre_ts->p_next_ts = p_next_ts;
          }
      }
     else
      {
         p_pre_ts = p_curr_ts;
      }
    
    p_curr_ts = p_next_ts;

     cnk ++;
    CHECK_FAIL_RET_FALSE(cnk <= p_priv->ts_node_num);
  }

  if((keep_type == EPG_DB_KEEP_TYPE_NET) && (keep_id == p_net_node->network_id))
    {
       return FALSE;
    }

  if(p_net_node->p_ts_head == NULL)
  {
    _free_fix(&p_priv->net_node_memf, p_net_node);
    p_net_node = NULL;
    return TRUE;
  }
  return FALSE;
}

/*!
 fill short event to event node

 \param[in] p_priv : epg_db_priv_t;
 \param[in] p_sht_evt : short event pointer.
 \param[in] p_sec_map : section_map_t pointer.
 \param[in] p_evt_node : event node pointer.
 */
static void _fill_sht_evt_to_evt_node(epg_db_priv_t *p_priv,
                                                  epg_eit_t     *p_epg_eit,
                                                  short_event_t *p_sht_evt,
                                                  event_node_t  *p_evt_node)
{
  text_event_t *p_text_evt = NULL;
  
  CHECK_FAIL_RET_VOID(p_evt_node != NULL);
  p_evt_node->event_id   = p_sht_evt->event_id;
  p_evt_node->start_time = p_sht_evt->start_time;
  p_evt_node->drt_time   = p_sht_evt->drt_time;

  //content descriptor
  p_evt_node->cont_level = p_sht_evt->cont_level;
  p_evt_node->user_nibble = p_sht_evt->user_nibble;
  
  
  p_text_evt = _get_text_event(p_epg_eit, p_sht_evt->event_id);

 if((p_text_evt != NULL) && (p_text_evt->name_len >0))
    {
          CHECK_FAIL_RET_VOID(p_text_evt->name_len <= MAX_EVENT_NAME_LEN);
          dvb_to_unicode_epg(p_text_evt->evt_nm, p_text_evt->name_len,
                          p_evt_node->event_name, MAX_EVT_NAME_LENGTH + 1,
                          get_iso639_refcode(p_text_evt->lang_code));
          dvb_to_unicode_epg(p_text_evt->lang_code, LANGUAGE_CODE_LENGTH,
                          p_evt_node->lang_code, LANGUAGE_CODE_LENGTH + 1,
                          get_iso639_refcode(p_text_evt->lang_code));

          
   }

}
static text_event_t *_get_text_event(epg_eit_t *p_epg_eit, u16 event_id)
{
  u16 index = 0;

  for(index = 0; index < p_epg_eit->count_text_evt; index++)
  {
    if(p_epg_eit->text_evt_list[index].event_id == event_id)
    {
      return &p_epg_eit->text_evt_list[index];
    }
  }
  return NULL;
}
static extern_event_t *_get_extern_text_event(epg_eit_t *p_epg_eit,
                                                                              u16 event_id)
{
  u8 index = 0;
  
  for(index = 0; index < p_epg_eit->count_ext_evt; index++)
  {
    if(p_epg_eit->ext_evt_list[index].event_id == event_id)
    {
      return &p_epg_eit->ext_evt_list[index];
      }
    }
    
    return NULL;
}
#if 0
static void _add_eit_text_to_sch_event(epg_db_priv_t *p_priv,
                                        epg_eit_t     *p_epg_eit,
                                        u8 index,
                                        event_node_t  *p_evt_node)
#endif
static BOOL _abnormal_proc_sch_text_event_policy(epg_db_priv_t *p_priv,
                                                               epg_eit_t     *p_epg_eit,
                                                               event_node_t  *p_evt_node)
{
     epg_event_t   event_type = EPG_UNKNOWN_EVENT;
     BOOL ret = FALSE;
    u32 context = 0;
    context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;
    if(p_evt_node ==  NULL)
    {
      return ret;
    }
   if(((p_priv->text_able == TRUE) && (p_evt_node->p_sht_text == NULL))
     ||((p_priv->ext_able == TRUE) && (p_evt_node->p_ext_desc_list == NULL)))
    {
       if(p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_NET_ID) 
        {
          /****delete other net  event text*******/
           event_type = EPG_NETWORK_ID_EVENT;
          /***delect all sch text*****/
           _epg_db_delete_event(event_type,EPG_TEXT_EVENT | EPG_SCHEDULE_EVENT,context); 
           ret = TRUE;
       }
       else if((p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_TS_ID) 
             && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
             && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
        {
          /****delete other ts  event text*******/
          event_type = EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT;
          /***delect all sch text*****/
           _epg_db_delete_event(event_type,EPG_TEXT_EVENT | EPG_SCHEDULE_EVENT, context); 
           ret = TRUE;
       }
      else if((p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_SVC_ID) 
            && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
            && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
            && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
         /****delete other svc  event text*******/
          event_type = EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT | EPG_SVC_ID_EVENT;
          /***delect all sch text*****/
           _epg_db_delete_event(event_type, EPG_TEXT_EVENT | EPG_SCHEDULE_EVENT,context);
           ret = TRUE;
      }
      else if((p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_TIME_ID) 
            && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
            && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
            && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
           /****delete this svc  other time event text*******/
            event_type = EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT | 
                                   EPG_SVC_ID_EVENT | EPG_SVC_ID_TIME_EVENT;
           /***delect all sch text*****/
           _epg_db_delete_event(event_type,EPG_TEXT_EVENT | EPG_SCHEDULE_EVENT,context); 
            ret = TRUE;
      }    
    }
   return ret;
 }

static BOOL _abnormal_proc_pf_text_event_policy(epg_db_priv_t *p_priv,
                                                                     epg_eit_t     *p_epg_eit,
                                                                     event_node_t  *p_evt_node)
{
     epg_event_t   event_type = EPG_UNKNOWN_EVENT;
     BOOL ret = FALSE;
     u32 context = 0;
     context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;
    if(p_evt_node ==  NULL)
    {
      return ret;
    }
   if((p_priv->text_able == TRUE) && (p_evt_node->p_sht_text == NULL))
    {
       if(p_priv->fixed_pf_text_policy == PF_FIXED_TEXT_OF_NET_ID) 
        {
          /****delete other net  event text*******/
           event_type = EPG_NETWORK_ID_EVENT;
          /***delect all pf text*****/
           _epg_db_delete_event(event_type,EPG_TEXT_EVENT | EPG_ALL_PF_EVENT,context); 
           ret = TRUE;
       }
       else if((p_priv->fixed_pf_text_policy == PF_FIXED_TEXT_OF_TS_ID) 
             && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
             && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
        {
          /****delete other ts  event text*******/
          event_type = EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT;
          /***delect all pf text*****/
           _epg_db_delete_event(event_type,EPG_TEXT_EVENT | EPG_ALL_PF_EVENT, context); 
           ret = TRUE;
       }
      else if((p_priv->fixed_pf_text_policy == PF_FIXED_TEXT_OF_SVC_ID) 
            && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
            && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
            && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
         /****delete other svc  event text*******/
          event_type = EPG_NETWORK_ID_EVENT | EPG_TS_ID_EVENT | EPG_SVC_ID_EVENT;
         /***delect all pf text*****/
           _epg_db_delete_event(event_type, EPG_TEXT_EVENT | EPG_ALL_PF_EVENT,context); 
           ret = TRUE;
      }   
    }
   return ret;
 }
static void _fill_text_evt_to_evt_node(epg_db_priv_t *p_priv,
                                       epg_eit_t     *p_epg_eit,
                                       u16            event_id,
                                       event_node_t  *p_evt_node)
{
  text_event_t *p_text_evt = NULL;
  u32 sht_text_len = 0;
  BOOL ret = FALSE;
  utc_time_t     event_end_time   = { 0 };
  u8 i = 0;
  u32 context = 0;
  context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;

  CHECK_FAIL_RET_VOID(p_evt_node != NULL);
  if(p_epg_eit->have_pf_pin == TRUE) /**pf table status***/
  {
    if(!p_epg_eit->pf_table)  /**if it is sch eit ,do not process it**/
    {
         if((p_priv->prog_info.network_id != p_epg_eit->org_nw_id)
        || (p_priv->prog_info.ts_id != p_epg_eit->ts_id)
        || (p_priv->prog_info.svc_id != p_epg_eit->svc_id))
        {
           return;
        }
        if((p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
          memcpy(&event_end_time, &p_evt_node->start_time, sizeof(utc_time_t));
          time_add(&event_end_time,&p_evt_node->drt_time);
          if((time_cmp(&p_evt_node->start_time, &p_priv->prog_info.end_time, FALSE) >= 0)
              || (time_cmp(&p_priv->prog_info.start_time,&event_end_time , FALSE) >= 0))
           {
             return;
            }
        }
        if(p_priv->fixed_pf_text_policy  >= PF_FIXED_TEXT_OF_TS_ID)
        {
          return;
        }
    }
  }
 /*!
  get text event
  */
  p_text_evt = _get_text_event(p_epg_eit, event_id);
  if((p_text_evt != NULL) && (p_text_evt->text_len > 0))
  {
    CHECK_FAIL_RET_VOID(p_text_evt->text_len > 0);
    CHECK_FAIL_RET_VOID(p_text_evt->text_len <= MAX_SHT_TEXT_LEN);
    //unicode.
    sht_text_len = (p_text_evt->text_len + UNICODE_BUFFER_OFFSET_LEN) * sizeof(u16);
    p_evt_node->p_sht_text = (u16 *)_malloc_nonfix(&p_priv->memp,sht_text_len);
    if(p_evt_node->p_sht_text == NULL)
    {
       if(p_epg_eit->have_pf_pin == TRUE)  /**pf table status***/
        {
          if(!p_epg_eit->pf_table)  /**if it is sch eit ,do not process it**/
          {
              return;
          }
          /****delete all sch event text*******/
           _epg_db_delete_event(EPG_UNKNOWN_EVENT,EPG_TEXT_EVENT |EPG_SCHEDULE_EVENT,context);
           p_evt_node->p_sht_text = (u16 *)_malloc_nonfix(&p_priv->memp,sht_text_len);
           if(p_evt_node->p_sht_text == NULL)
            {
             /*****auto adjust fixed pf text policy*******/
             for(i = PF_FIXED_TEXT_NO_ID;i <= PF_FIXED_TEXT_OF_SVC_ID;i ++)
              {
                  ret = _abnormal_proc_pf_text_event_policy(p_priv,p_epg_eit,p_evt_node);
                  p_evt_node->p_sht_text = (u16 *)_malloc_nonfix(&p_priv->memp,sht_text_len);
                  if(p_evt_node->p_sht_text != NULL)
                  {
                    break;
                  }
                  if(p_priv->fixed_pf_text_policy  == PF_FIXED_TEXT_OF_SVC_ID)
                    {
                       break;
                    }
                  /****if pf policy don't result,it will auto adjust policy*****/
                  p_priv->fixed_pf_text_policy = p_priv->fixed_pf_text_policy + 1;
                  if(p_priv->fixed_pf_text_policy  >= PF_FIXED_TEXT_OF_SVC_ID)
                  {
                      p_priv->fixed_pf_text_policy = PF_FIXED_TEXT_OF_SVC_ID;
                  }

              }
             if(p_evt_node->p_sht_text == NULL)
              {
                 return;
              }
             
            }

        }
       else  /***only sch status***/
        {
           if(p_epg_eit->pf_table)  /**if it is pf eit ,do not process it**/
            {
               return;
            }
           /****delete all pf text****/
            _epg_db_delete_event(EPG_UNKNOWN_EVENT,EPG_TEXT_EVENT |EPG_ALL_PF_EVENT, context);
           p_evt_node->p_sht_text = (u16 *)_malloc_nonfix(&p_priv->memp,sht_text_len);
           if(p_evt_node->p_sht_text == NULL)
            {
               for(i = SCH_FIXED_TEXT_NO_ID;i <= SCH_FIXED_TEXT_OF_TIME_ID;i++)
                {
                   /****do sch text policy****/
                   ret = _abnormal_proc_sch_text_event_policy(p_priv,p_epg_eit,p_evt_node);
                   p_evt_node->p_sht_text = (u16 *)_malloc_nonfix(&p_priv->memp,sht_text_len);
                   if(p_evt_node->p_sht_text != NULL)
                    {
                       break;
                    }
                   if(p_priv->fixed_sch_text_policy  == SCH_FIXED_TEXT_OF_TIME_ID)
                    {
                       break;
                    }
                    /****if sch policy don't result,it will auto adjust policy*****/
                    p_priv->fixed_sch_text_policy = p_priv->fixed_sch_text_policy + 1;
                    if(p_priv->fixed_sch_text_policy  >= SCH_FIXED_TEXT_OF_TIME_ID)
                    {
                        p_priv->fixed_sch_text_policy = SCH_FIXED_TEXT_OF_TIME_ID;
                    }
                }
               if(p_evt_node->p_sht_text == NULL)
                {
                  return;
                }
           }

        }
    }
    if(p_evt_node->p_sht_text != NULL)
    {
      p_evt_node->sht_text_len = p_text_evt->text_len; 
      dvb_to_unicode_epg(p_text_evt->text,
                     p_text_evt->text_len,
                     p_evt_node->p_sht_text,
                     p_evt_node->sht_text_len + 1,
                     get_iso639_refcode(p_text_evt->lang_code));
    }
  }
}


static void _fill_ext_evt_to_evt_node(epg_db_priv_t *p_priv,
                                      epg_eit_t     *p_epg_eit,
                                      u16            event_id,
                                      event_node_t  *p_evt_node)
{
  extern_event_t *p_ext_evt = NULL;
  ext_desc_t     *p_ext_des = NULL;
  BOOL ret = FALSE;
  u8             *p_buf     = NULL;
  u8              item_idx  = 0;
  u32             buf_len   = 0;
  u8              index     = 0;
  u8              count     = 0;
  u8 i = 0;
  utc_time_t     event_end_time   = { 0 };
  u32 context = 0;
  context = ((EPG_DB_KEEP_TYPE_SVC << 16) & 0xFFFF0000) | p_epg_eit->svc_id;

  CHECK_FAIL_RET_VOID(p_evt_node != NULL);
  if(p_epg_eit->have_pf_pin == TRUE) /**pf table status***/
  {
    if(!p_epg_eit->pf_table)  /**if it is sch eit ,do not process it**/
    {
         if((p_priv->prog_info.network_id != p_epg_eit->org_nw_id)
        || (p_priv->prog_info.ts_id != p_epg_eit->ts_id)
        || (p_priv->prog_info.svc_id != p_epg_eit->svc_id))
        {
           return;
        }
        if((p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
          memcpy(&event_end_time, &p_evt_node->start_time, sizeof(utc_time_t));
          time_add(&event_end_time,&p_evt_node->drt_time);
          if((time_cmp(&p_evt_node->start_time, &p_priv->prog_info.end_time, FALSE) >= 0)
              || (time_cmp(&p_priv->prog_info.start_time,&event_end_time , FALSE) >= 0))
           {
             return;
            }
        }
        if(p_priv->fixed_pf_text_policy  >= PF_FIXED_TEXT_OF_TS_ID)
        {
          return;
        }
    }
  }

 /*!
  calc extern text length.
  */
  for(index = 0; index < p_epg_eit->count_ext_evt; index++)
  {
    if(p_epg_eit->ext_evt_list[index].event_id == event_id)
    {
      count++;
      buf_len += ((p_epg_eit->ext_evt_list[index].ext_txt_len + 
                         UNICODE_BUFFER_OFFSET_LEN)* sizeof(u16));
    }
  }

 /*!
  buffer length equal to 0, not have ext desc.
  */
  if(buf_len == 0)
    return;

  buf_len += (count * sizeof(ext_desc_t));

 /*!
  alloc buffer.
  */
  p_buf = (u8 *)_malloc_nonfix(&p_priv->memp, buf_len);

  if(p_buf == NULL)
  {
      if(p_epg_eit->have_pf_pin != TRUE)  /*****only sch status***/
      {
         if(p_epg_eit->pf_table)  /**if it is pf eit ,do not process it**/
          {
             return;
          }
         /****delete all pf text****/
             _epg_db_delete_event(EPG_UNKNOWN_EVENT,EPG_TEXT_EVENT |EPG_ALL_PF_EVENT, context);
        p_buf = (u8 *)_malloc_nonfix(&p_priv->memp, buf_len);
         if(p_buf == NULL)
          {
              for(i = SCH_FIXED_TEXT_NO_ID;i <= SCH_FIXED_TEXT_OF_TIME_ID;i++)
              {
                 /****do sch text policy****/
                  ret = _abnormal_proc_sch_text_event_policy(p_priv,p_epg_eit,p_evt_node);
                 p_buf = (u8 *)_malloc_nonfix(&p_priv->memp, buf_len);
                 if(p_buf != NULL)
                  {
                     break;
                  }
                  if(p_priv->fixed_sch_text_policy  == SCH_FIXED_TEXT_OF_TIME_ID)
                  {
                     break;
                  }
                  /****if sch policy don't result,it will auto adjust policy*****/
                  p_priv->fixed_sch_text_policy = p_priv->fixed_sch_text_policy + 1;
                  if(p_priv->fixed_sch_text_policy  >= SCH_FIXED_TEXT_OF_TIME_ID)
                  {
                      p_priv->fixed_sch_text_policy = SCH_FIXED_TEXT_OF_TIME_ID;
                  }
              }
              if(p_buf == NULL)
              {
                  return;
              }
         }

      }
  }
 
  if(p_buf == NULL)
    return;

  p_evt_node->p_ext_desc_list = (ext_desc_t *)p_buf;
  p_evt_node->count_ext_desc = count;

 /*!
  move buffer
  */
  p_buf += (count * sizeof(ext_desc_t));
  count = 0;

 /*!
  copy
  */
  for(index = 0; index < p_epg_eit->count_ext_evt; index++)
  {
    if(p_epg_eit->ext_evt_list[index].event_id == event_id)
    {
      p_ext_des = &p_evt_node->p_ext_desc_list[count++];
      p_ext_evt = &p_epg_eit->ext_evt_list[index];

      CHECK_FAIL_RET_VOID(p_ext_evt->item_num <= MAX_ITEM_NUMBER);
      
      
      for(item_idx = 0; item_idx < p_ext_evt->item_num; item_idx++)
      {
        dvb_to_unicode_epg(p_ext_evt->item[item_idx].item_desc,
                       MAX_ITEM_DESC_LENGTH,
                       p_ext_des->item[item_idx].item_desc,
                       MAX_ITEM_DESC_LENGTH + 1,
                       get_iso639_refcode(p_ext_evt->lang_code));
        
        dvb_to_unicode_epg(p_ext_evt->item[item_idx].item_char,
                       MAX_ITEM_CHAR_LENGTH,
                       p_ext_des->item[item_idx].item_char,
                       MAX_ITEM_CHAR_LENGTH + 1,
                       get_iso639_refcode(p_ext_evt->lang_code));
      }

      if(p_ext_evt->ext_txt_len > 0)
      {
        p_ext_des->p_ext_text = (u16 *)p_buf;
        
        p_ext_des->ext_text_len = p_ext_evt->ext_txt_len;

        dvb_to_unicode_epg(p_ext_evt->ext_txt,
                       p_ext_evt->ext_txt_len,
                       p_ext_des->p_ext_text,
                       p_ext_des->ext_text_len + 1,
                       get_iso639_refcode(p_ext_evt->lang_code));
        //move buffer address
        p_buf += (p_ext_des->ext_text_len + UNICODE_BUFFER_OFFSET_LEN) * sizeof(u16);
      }
    }
  }
}

static void _fixed_event_node_to_svc(epg_db_priv_t *p_priv,
                                                        svc_node_t    *p_svc_node)
{
    event_node_t *p_cur_node = NULL;
    event_node_t *p_pre_node = NULL;
    event_node_t *p_delete_node = NULL;
    event_node_t *p_tmp_node = NULL;
    u32 cnk = 0;
     
    if(p_priv->fixed_svc_node_num == 0)
    {
       return;
    }
    p_pre_node = p_svc_node->p_sch_evt_head;
    if(p_pre_node == NULL)
    {
      return;
    }
    p_cur_node = p_pre_node->p_next_evt_node;
    cnk = 0;
    while(p_cur_node != NULL)
    {
      p_pre_node = p_cur_node;
      p_cur_node = p_cur_node->p_next_evt_node;
      cnk ++;
      if(cnk > p_priv->fixed_svc_node_num)
      {
        p_pre_node->p_next_evt_node = NULL;
        p_delete_node = p_cur_node;
        //OS_PRINTF("###debug _fixed_event_node_to_svc start@@@\n");
        break;
      }
      CHECK_FAIL_RET_VOID(cnk <= p_priv->evt_node_num);
    }

    cnk = 0;
    while(p_delete_node != NULL)
    {
       p_tmp_node = p_delete_node;
       p_delete_node = p_delete_node->p_next_evt_node;
       _del_event_node(p_priv, p_tmp_node);
       cnk ++;
       CHECK_FAIL_RET_VOID(cnk <= p_priv->evt_node_num);
    }
    //OS_PRINTF("###debug _fixed_event_node_to_svc delete [%d]event@@@\n",cnk);
}
static BOOL check_fixed_event_time(epg_db_priv_t *p_priv,
                                             svc_node_t    *p_svc_node,
                                             utc_time_t evt_start_time)
{
    event_node_t *p_cur_node = NULL;
    u32 cnk = 0;
   
    if(p_priv->fixed_svc_node_num == 0)
    {
       return TRUE;
    }
    p_cur_node = p_svc_node->p_sch_evt_head;
    cnk = 0;
    while(p_cur_node != NULL)
    {
      cnk ++;
      if(cnk > p_priv->fixed_svc_node_num)
      {
        break;
      }
      CHECK_FAIL_RET_FALSE(cnk <= p_priv->evt_node_num);
      p_cur_node = p_cur_node->p_next_evt_node;
    }
    
    if(cnk < p_priv->fixed_svc_node_num)
    {
      return TRUE;
    }
    else
    {

        if(p_cur_node != NULL)
        {
             if(time_cmp(&p_cur_node->start_time,&evt_start_time,  FALSE) >= 0)
              {
                 return TRUE;
              }
        }
    }
   
   return  FALSE;
}
void check_svc_event_node(epg_db_priv_t *p_priv, svc_node_t    *p_svc_node,u8 pos)
{
    event_node_t *p_cur_node = p_svc_node->p_sch_evt_head;
    u32 cnk = 0;
    while(p_cur_node != NULL)
    {
       p_cur_node = p_cur_node->p_next_evt_node;
       cnk ++;
       //CHECK_FAIL_RET_VOID(cnk <= p_priv->evt_node_num);
       if(cnk > p_priv->evt_node_num)
      {
         OS_PRINTF("@@err[%d] total node:%d\n",pos,p_priv->evt_node_num);
         p_cur_node = p_svc_node->p_sch_evt_head;
         cnk = 0;
         while(p_cur_node != NULL)
          {
            OS_PRINTF("@@err node:0x%x\n",p_cur_node);
            p_cur_node = p_cur_node->p_next_evt_node;
            cnk ++;
            CHECK_FAIL_RET_VOID(cnk <= p_priv->evt_node_num);
          }
      }
    }
}
static void _insert_event_node_to_svc(epg_db_priv_t *p_priv,
                                      event_node_t  *p_insert_node,
                                      svc_node_t    *p_svc_node)
{
  event_node_t *p_cur_node = NULL;
  event_node_t *p_pre_node = NULL;
  s8            cmp_ret    = 0;
  u32 cnk = 0; 
 /*!
  insert to the head
  */
  if(p_svc_node->p_sch_evt_head == NULL)
  {
    p_svc_node->p_sch_evt_head = p_insert_node;
    p_svc_node->p_sch_evt_head->p_next_evt_node = NULL;
    return;
  }

 /*!
  if less than head event, replace head event.
  */ 
  cmp_ret = time_cmp(&p_svc_node->p_sch_evt_head->start_time,
                     &p_insert_node->start_time,
                     FALSE);
  if(cmp_ret == 1)
  {
    p_insert_node->p_next_evt_node = p_svc_node->p_sch_evt_head;
    p_svc_node->p_sch_evt_head = p_insert_node;
    return;
  }

 /*!
  insert to the middle area
  */
  p_pre_node = p_svc_node->p_sch_evt_head;
  p_cur_node = p_svc_node->p_sch_evt_head->p_next_evt_node;
  
  while(p_cur_node != NULL)
  {
    cmp_ret = time_cmp(&p_cur_node->start_time, 
                       &p_insert_node->start_time,
                       FALSE);
   /*!
    = 1; insert it.
    = 0; delete old event node, and insert new.
    = -1; get next event node.
    */
    if(cmp_ret == 1)
    {
      p_pre_node->p_next_evt_node = p_insert_node;
      p_insert_node->p_next_evt_node = p_cur_node;
      return;
    }
    else if(cmp_ret == 0)
    {
      p_pre_node->p_next_evt_node = p_insert_node;
      p_insert_node->p_next_evt_node = p_cur_node->p_next_evt_node;

      _del_event_node(p_priv, p_cur_node);
      return;
    }
 
    p_pre_node = p_cur_node;
    p_cur_node = p_cur_node->p_next_evt_node;

    cnk ++;
    CHECK_FAIL_RET_VOID(cnk <= p_priv->evt_node_num);
  }

 /*!
  insert to the tail.
  */
  p_pre_node->p_next_evt_node = p_insert_node;
  p_insert_node->p_next_evt_node = NULL;
}


static void _fill_sch_text_to_event(epg_db_priv_t *p_priv,
                                                                      epg_eit_t     *p_epg_eit,
                                                                      u16 event_id,
                                                                      event_node_t  *p_evt_node)
{
    if((p_priv->text_able == TRUE) && (p_evt_node->p_sht_text == NULL))
    {
      _fill_text_evt_to_evt_node(p_priv, p_epg_eit, event_id, p_evt_node);
    }

    if((p_priv->ext_able == TRUE) && (p_evt_node->p_ext_desc_list == NULL))
    {
      _fill_ext_evt_to_evt_node(p_priv, p_epg_eit,event_id, p_evt_node);
    }
}
static void _add_eit_text_to_sch_event(epg_db_priv_t *p_priv,
                                                  epg_eit_t     *p_epg_eit,
                                                  u8 index,
                                                  event_node_t  *p_evt_node)
{
   utc_time_t     event_end_time   = { 0 };
   short_event_t *p_sht_evt  = NULL;
   
   p_sht_evt = &p_epg_eit->sht_evt_list[index];
   if(p_evt_node ==  NULL)
    {
       return;
    }
   if(((p_priv->text_able == TRUE) && (p_evt_node->p_sht_text == NULL))
     ||((p_priv->ext_able == TRUE) && (p_evt_node->p_ext_desc_list == NULL)))
    {
       if((p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_NET_ID) 
          && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id))
        {
           /***fill text***/
           _fill_sch_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
        }
       else if((p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_TS_ID) 
             && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
             && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
        {
          /***fill text***/
          _fill_sch_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
        }
      else if((p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_SVC_ID) 
            && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
            && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
            && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
          /***fill text***/
          _fill_sch_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
        }
      else if((p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_OF_TIME_ID) 
            && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
            && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
            && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
          memcpy(&event_end_time, &p_sht_evt->start_time, sizeof(utc_time_t));
          time_add(&event_end_time,&p_sht_evt->drt_time);
          if((time_cmp(&event_end_time, &p_priv->prog_info.start_time, FALSE) >= 0)
              &&(time_cmp(&p_priv->prog_info.end_time,&p_sht_evt->start_time, FALSE) >= 0))
          {
          /***fill text***/
          _fill_sch_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
          }
        }
      else if(p_priv->fixed_sch_text_policy == SCH_FIXED_TEXT_NO_ID) /***if no policy****/
      {
         _fill_sch_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
      }
    }
    }

static BOOL _check_sch_policy_for_sch_event(epg_db_priv_t *p_priv,
                                                              epg_eit_t     *p_epg_eit,
                                                              u8 index)
{
   utc_time_t     event_end_time   = { 0 };
   short_event_t *p_sht_evt  = NULL;
   BOOL ret = FALSE;
   p_sht_evt = &p_epg_eit->sht_evt_list[index];
   
   if(p_priv->sch_policy == SCH_EVENT_ALL)
    {
       ret = TRUE;
    }
   else if((p_priv->sch_policy == SCH_EVENT_OF_NETWORK_ID) 
      && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id))
    {
       ret = TRUE;
    }
   else if((p_priv->sch_policy == SCH_EVENT_OF_TS_ID) 
         && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
         && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
    {
       ret = TRUE;
    }
  else if((p_priv->sch_policy == SCH_EVENT_OF_SVC_ID) 
        && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
       ret = TRUE;
    }
  else if((p_priv->sch_policy == SCH_EVENT_OF_TIME_ID) 
        && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
      memcpy(&event_end_time, &p_sht_evt->start_time, sizeof(utc_time_t));
      time_add(&event_end_time,&p_sht_evt->drt_time);
      if((time_cmp(&event_end_time, &p_priv->prog_info.start_time, FALSE) >= 0)
          &&(time_cmp(&p_priv->prog_info.end_time,&p_sht_evt->start_time, FALSE) >= 0))
      {
       ret = TRUE;
       }
    }
   return ret;
}
static BOOL _check_sch_event_text_exist(epg_db_priv_t *p_priv,
                                                                        epg_eit_t *p_epg_eit,
                                                                        u8 index)
{
    short_event_t *p_sht_evt  = NULL;
    text_event_t *p_text_evt = NULL;
    extern_event_t *p_ext_text_evt = NULL;
    BOOL text_exist = FALSE;
    
    p_sht_evt = &p_epg_eit->sht_evt_list[index];

    if(p_priv->text_able == TRUE)
    {
      p_text_evt = _get_text_event(p_epg_eit, p_sht_evt->event_id);
      if(p_text_evt != NULL)
      {
         if(p_text_evt->text != NULL)
         {
            text_exist = TRUE;
         }
       }
    }
    if(p_priv->ext_able == TRUE)
    {
      p_ext_text_evt = _get_extern_text_event(p_epg_eit, p_sht_evt->event_id);
      if(p_ext_text_evt != NULL)
      {
         if(p_ext_text_evt->ext_txt != NULL)
         {
            text_exist = TRUE;
         }
       }
    }

    return text_exist ;
}
static void _updata_sch_event_message(epg_db_priv_t *p_priv,
                                              epg_eit_t     *p_epg_eit,u8 index)
{
    short_event_t *p_sht_evt  = NULL;
    utc_time_t     event_end_time   = { 0 };
    p_sht_evt = &p_epg_eit->sht_evt_list[index];
    
   if((p_priv->prog_info.network_id == p_epg_eit->org_nw_id) 
    && (p_priv->prog_info.ts_id == p_epg_eit->ts_id) 
    && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
#if  0
        {
          u8 asc_str[32];
          utc_time_t event_end_time = {0};
          
          memcpy(&event_end_time,&p_sht_evt->start_time,sizeof(utc_time_t));
          
          time_add(&event_end_time,&p_sht_evt->drt_time);
          sprintf((char *)asc_str, "%.2d:%.2d~%d:%d",
                        p_sht_evt->start_time.hour,p_sht_evt->start_time.minute,
                        event_end_time.hour,event_end_time.minute);
            OS_PRINTF("\n add[0x%x] sch event :%s  %s \n",
              p_sht_evt->event_id ,
              asc_str,p_sht_evt->evt_nm);
         }
#endif

      //OS_PRINTF("###debug eit is current program\n");
      memcpy(&event_end_time, &p_sht_evt->start_time, sizeof(utc_time_t));
      time_add(&event_end_time,&p_sht_evt->drt_time);
      if((time_cmp(&event_end_time, &p_priv->prog_info.start_time, FALSE) >= 0)
        &&(time_cmp(&p_priv->prog_info.end_time,&p_sht_evt->start_time , FALSE) >= 0))
        {
          //OS_PRINTF("###debug  program have sch evt updata\n");
          p_epg_eit->message_flag = TRUE;
        }
    }
}
static RET_CODE _add_sch_evt_to_svc(epg_db_priv_t *p_priv,
                                epg_eit_t     *p_epg_eit,
                                svc_node_t    *p_svc_node)
{
  u8             index      = 0;
  short_event_t *p_sht_evt  = NULL;
  event_node_t  *p_evt_node = NULL;
  utc_time_t     sys_time   = { 0 };
  utc_time_t     event_end_time   = { 0 };
  event_id_info_t id_info = EVT_ID_EXIST_NO;
  event_node_t *p_sch_event_node = NULL;
  BOOL ret = 0;

 /*!
  get current system time
  */
  time_get(&sys_time, TRUE);
 
  for(index = 0; index < p_epg_eit->count_sht_evt; index++)
  {
   CHECK_FAIL_RET_CODE(index < MAX_SHT_EVT_PER_SEC);
    p_sht_evt = &p_epg_eit->sht_evt_list[index];

    id_info = _check_schedule_event_id(p_svc_node, p_sht_evt->event_id,&p_sch_event_node);
    if(id_info == EVT_ID_EXIST_FULL)
    {
      continue;
    }
    else if(id_info == EVT_ID_EXIST_LACK)
    {
      if(_check_sch_event_text_exist(p_priv,p_epg_eit,index) == TRUE)
      {
       _add_eit_text_to_sch_event(p_priv,p_epg_eit,index,p_sch_event_node);
       _updata_sch_event_message(p_priv,p_epg_eit,index);
      }
      continue;
    }

    /*!
      p_sht_evt->start_time >= sys_time
      */
    event_end_time = p_sht_evt->start_time;
    time_add(&event_end_time, &p_sht_evt->drt_time);

     if((p_priv->prog_info.network_id != p_epg_eit->org_nw_id) 
      || (p_priv->prog_info.ts_id != p_epg_eit->ts_id)
      ||(p_epg_eit->evt_fixed_able == TRUE))
      {
        if(FALSE == check_fixed_event_time(p_priv,p_svc_node,p_sht_evt->start_time))
          {
            continue;
          }
      }

    /**if it is have pf table request,but the event node no enough on ts***/
    if((p_epg_eit->have_pf_pin == TRUE) && (p_priv->pf_policy >= PF_EVENT_OF_TS_ID))  
    {
      return ERR_FAILURE;
    }
    if(time_cmp(&event_end_time, &sys_time, FALSE) >= 0)
    {
      p_evt_node = _new_event_node(p_priv);    
      //EPG_DB_WARN(p_evt_node != NULL)
      if(NULL == p_evt_node)
      {
          p_evt_node = _abnormal_proc_event_node(p_priv,p_epg_eit);
      }
      if(NULL == p_evt_node)
      {
          ret = _check_sch_policy_for_sch_event(p_priv,p_epg_eit,index);
          if(ret == FALSE)
          {
              continue;
          }

          /***memory is no enough ,it switch policy,dele other net ts svc id or delete last node**/
          EB_PRINT("\n!!!!!event node overflow, _special_proc_sch_event_node!!!!\n");
          p_evt_node = _special_proc_sch_event_node(p_priv,p_epg_eit,index);
           if(p_evt_node != NULL)
          {
            //EB_PRINT("\n _special_proc_sch_event_node success!\n");
            OS_PRINTF("\n _special_proc_sch_event_node success!\n");
          }
          else
          {
              //EB_PRINT("\n _special_proc_sch_event_node fail!\n");
              OS_PRINTF("\n _special_proc_sch_event_node fail!\n");
              //return ERR_FAILURE;
               continue;
          }
          
      }

      p_priv->evt_used_node_num ++;
      _updata_sch_event_message(p_priv,p_epg_eit,index);
      
      _fill_sht_evt_to_evt_node(p_priv,p_epg_eit, p_sht_evt, p_evt_node);
      _add_eit_text_to_sch_event(p_priv,p_epg_eit,index,p_evt_node);
      _insert_event_node_to_svc(p_priv, p_evt_node, p_svc_node);
    }
    
  }
  
 if((p_priv->prog_info.network_id != p_epg_eit->org_nw_id) 
  || (p_priv->prog_info.ts_id != p_epg_eit->ts_id)
  || (p_epg_eit->evt_fixed_able == TRUE))
  //|| (p_priv->prog_info.svc_id != p_epg_eit->svc_id))
  {
    _fixed_event_node_to_svc(p_priv,p_svc_node);
  }

  return SUCCESS;
}

static void _fill_pf_text_to_event(epg_db_priv_t *p_priv,
                                                                      epg_eit_t     *p_epg_eit,
                                                                      u16 event_id,
                                                                      event_node_t  *p_evt_node)
{
    if((p_priv->text_able == TRUE) && (p_evt_node->p_sht_text == NULL))
    {
      _fill_text_evt_to_evt_node(p_priv, p_epg_eit, event_id, p_evt_node);
    }

    if((p_priv->ext_able == TRUE) && (p_evt_node->p_ext_desc_list == NULL))
    {
      _fill_ext_evt_to_evt_node(p_priv, p_epg_eit,event_id, p_evt_node);
    }
}

static void _add_eit_text_to_pf_event(epg_db_priv_t *p_priv,
                                              epg_eit_t     *p_epg_eit,
                                              u8 index,
                                              event_node_t  *p_evt_node)
{
   short_event_t *p_sht_evt  = NULL;
   
   p_sht_evt = &p_epg_eit->sht_evt_list[index];
   if(p_evt_node ==  NULL)
    {
       return;
    }

    if(((p_priv->text_able == TRUE) && (p_evt_node->p_sht_text == NULL))
     ||((p_priv->ext_able == TRUE) && (p_evt_node->p_ext_desc_list == NULL)))
    {
       if((p_priv->fixed_pf_text_policy == PF_FIXED_TEXT_OF_NET_ID) 
          && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id))
        {
           /***fill text***/
           _fill_pf_text_to_event(p_priv, p_epg_eit, p_sht_evt->event_id, p_evt_node);
        }
       else if((p_priv->fixed_pf_text_policy == PF_FIXED_TEXT_OF_TS_ID) 
             && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
             && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
        {
          /***fill text***/
          _fill_pf_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
        }
      else if((p_priv->fixed_pf_text_policy == PF_FIXED_TEXT_OF_SVC_ID) 
            && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
            && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
            && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
        {
          /***fill text***/
          _fill_pf_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
        }
      else if(p_priv->fixed_pf_text_policy == PF_FIXED_TEXT_NO_ID) /***if no policy****/
      {
         _fill_pf_text_to_event(p_priv,p_epg_eit,p_sht_evt->event_id,p_evt_node);
      }
    }
    }

static BOOL _check_pf_policy_for_pf_event(epg_db_priv_t *p_priv,
                                                    epg_eit_t     *p_epg_eit)
{
   BOOL ret = FALSE;
   
   if(p_priv->pf_policy == PF_EVENT_ALL)
    {
       ret = TRUE;
    }
   else if((p_priv->pf_policy == PF_EVENT_OF_NETWORK_ID) 
      && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id))
    {
       ret = TRUE;
    }
   else if((p_priv->pf_policy == PF_EVENT_OF_TS_ID) 
         && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
         && (p_priv->prog_info.ts_id == p_epg_eit->ts_id))
    {
       ret = TRUE;
    }
  else if((p_priv->pf_policy == PF_EVENT_OF_SVC_ID) 
        && (p_priv->prog_info.network_id == p_epg_eit->org_nw_id)
        && (p_priv->prog_info.ts_id == p_epg_eit->ts_id)
        && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
       ret = TRUE;
    }
   return ret;
}

static BOOL _check_pf_event_text_exist(epg_db_priv_t *p_priv,epg_eit_t *p_epg_eit,u8 index)
{
    short_event_t *p_sht_evt  = NULL;
    text_event_t *p_text_evt = NULL;
    extern_event_t *p_ext_text_evt = NULL;
    BOOL text_exist = FALSE;
    
    p_sht_evt = &p_epg_eit->sht_evt_list[index];
    if(p_priv->text_able == TRUE)
    {
      p_text_evt = _get_text_event(p_epg_eit, p_sht_evt->event_id);
      if(p_text_evt != NULL)
      {
         if(p_text_evt->text != NULL)
         {
            text_exist = TRUE;
         }
       }
    }
    if(p_priv->ext_able == TRUE)
    {
      p_ext_text_evt = _get_extern_text_event(p_epg_eit, p_sht_evt->event_id);
      if(p_ext_text_evt != NULL)
      {
         if(p_ext_text_evt->ext_txt != NULL)
         {
            text_exist = TRUE;
         }
      }
    }
    
    return text_exist ;
}
static RET_CODE _add_pf_evt_to_svc(epg_db_priv_t *p_priv,
                                   epg_eit_t     *p_epg_eit,
                                   epg_event_note_t    event_note_type,
                                   svc_node_t    *p_svc_node)
{
  short_event_t *p_sht_evt = NULL;
  event_node_t  *p_evt_node = NULL;
  event_id_info_t id_info = EVT_ID_EXIST_NO;
  event_node_t  *p_pf_evt_node = NULL;
  BOOL ret =  FALSE;
 
  if(p_epg_eit->count_sht_evt <= 0)
  {
     return ERR_FAILURE;
  }
  CHECK_FAIL_RET_CODE(p_epg_eit->count_sht_evt > 0);
  
  p_sht_evt = &p_epg_eit->sht_evt_list[0];

  /***up pf text****/
  id_info = _check_pf_event_id(p_svc_node,event_note_type, p_sht_evt->event_id,&p_pf_evt_node);
  if(id_info == EVT_ID_EXIST_FULL)
  {
    return ERR_FAILURE;
  }
  else if(id_info == EVT_ID_EXIST_LACK)
  {
    if(_check_pf_event_text_exist(p_priv,p_epg_eit,0) == TRUE)
    {
       _add_eit_text_to_pf_event(p_priv,p_epg_eit,0,p_pf_evt_node);
       return SUCCESS;
     }
    else
    {
      return ERR_FAILURE;
    }
    
  }
 /*!
  get preset event or follow event.
  */ 
  p_evt_node = IS_PRESENT_EVENT(event_note_type) ? 
              p_svc_node->p_present_evt : p_svc_node->p_follow_evt;
 /*!
  if event node == NULL, new it, 
  and if new fail, return ERR_FAILURE
  */ 
  if(p_evt_node == NULL)
  {
    p_evt_node = _new_event_node(p_priv);
    if(NULL == p_evt_node)
    {
        p_evt_node = _abnormal_proc_event_node(p_priv,p_epg_eit);
    }
    if(NULL == p_evt_node)
    {
       ret = _check_pf_policy_for_pf_event(p_priv,p_epg_eit);
       if(ret == FALSE)
        {
           /********if it is curr policy,don't process it***********/
           EB_PRINT("\nit isn't current program date,it will losed\n");
           return ERR_FAILURE;
        }
        /***memory is no enough ,it switch policy,dele other net ts svc id**/
       EB_PRINT("\n!!!!!event node overflow, enter to special pf process!!!!\n");
       p_evt_node = _special_proc_pf_event_node(p_priv,p_epg_eit);
       if(p_evt_node != NULL)
      {
        EB_PRINT("\n!!!!!enter to special pf process seccess!!!!\n");
      }
      else
      {
          EB_PRINT("\n!!!!!enter to special pf process fail!!!!\n");
          return ERR_FAILURE;
      }
    }
    p_priv->evt_used_node_num ++;
  }  
  
#if 0
{
  u8 asc_str[32];
  utc_time_t event_end_time = {0};
  
  memcpy(&event_end_time,&p_sht_evt->start_time,sizeof(utc_time_t));
  
  time_add(&event_end_time,&p_sht_evt->drt_time);
  sprintf((char *)asc_str, "%.2d:%.2d~%d:%d",
                p_sht_evt->start_time.hour,p_sht_evt->start_time.minute,
                event_end_time.hour,event_end_time.minute);
  //OS_PRINTF("add PF event n[0x%x], t[0x%x], s[0x%x]\n",
  //                  p_epg_eit->org_nw_id,
   //                 p_epg_eit->ts_id,
    //                p_epg_eit->svc_id);
   if((p_priv->prog_info.network_id == p_epg_eit->org_nw_id) 
    && (p_priv->prog_info.ts_id == p_epg_eit->ts_id) 
    && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
   {
    OS_PRINTF("add curr PF event :%s  %s\n",asc_str,p_sht_evt->evt_nm);
  }
 }
#endif

  _fill_sht_evt_to_evt_node(p_priv,
                            p_epg_eit,
                            p_sht_evt,
                            p_evt_node);

 _add_eit_text_to_pf_event(p_priv,p_epg_eit,0,p_evt_node);
 /*!
  attach to P/F
  */
  if(IS_PRESENT_EVENT(event_note_type))
  {
    p_svc_node->p_present_evt = p_evt_node;
  }
  else
  {
    p_svc_node->p_follow_evt = p_evt_node;
  }
  
  return SUCCESS;
}

static svc_node_t *_add_svc_node_to_ts(epg_db_priv_t *p_priv,
                                       u16           svc_id,
                                       network_node_t *p_net_node,
                                       ts_node_t     *p_ts_node)
{
  svc_node_t *p_cur_node = NULL;
  svc_node_t *p_pre_node = NULL;
  u32 cnk = 0;

   if((p_net_node == NULL) ||(p_ts_node == NULL))
    {
      return NULL;
    }
   p_cur_node = p_ts_node->p_svc_head;
   p_pre_node = p_ts_node->p_svc_head;
   
  //first
  if(p_cur_node == NULL)
  {
    p_cur_node = _new_svc_node(p_priv, svc_id);
    if(p_cur_node == NULL)
      {
         p_cur_node = _abnormal_proc_svc_node(p_priv,p_net_node,p_ts_node,svc_id);
         if(NULL == p_cur_node)
          {
            return NULL;
          }
      }
    
    p_cur_node->is_new_node = 1;
    p_ts_node->p_svc_head = p_cur_node;
    return p_ts_node->p_svc_head;
  }
  //exist.
  while(p_cur_node != NULL)
  {
    if(p_cur_node->svc_id == svc_id)
    {      
      p_cur_node->is_new_node = 0;
      break;
    }
    p_pre_node = p_cur_node;
    p_cur_node = p_cur_node->p_next_svc_node;
    cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->svc_node_num);
  }

  if(p_cur_node == NULL)
  {
    p_cur_node = _new_svc_node(p_priv, svc_id);
    if(p_cur_node != NULL)
    {
        p_cur_node->is_new_node = 1;
        p_pre_node->p_next_svc_node = p_cur_node;
    }
    else
    {
      /*!
       *because delete all event.the link is breaken,it dn't need to link pre node,it will build link by oneself.
       */
      p_cur_node = _abnormal_proc_svc_node(p_priv,p_net_node,p_ts_node,svc_id);
      if(p_cur_node != NULL)
      {
         p_cur_node->is_new_node = 1;
      }
    }
  }
  
  return p_cur_node;
}

static ts_node_t *_add_ts_node_to_network(epg_db_priv_t  *p_priv,
                                          u16             ts_id,
                                          network_node_t *p_net_node)
{
  ts_node_t *p_cur_node = NULL;
  ts_node_t *p_pre_node = NULL;
  u32 cnk = 0;

  if(p_net_node == NULL)
    {
     return NULL;
    }
  //first
  if(p_net_node->p_ts_head == NULL)
  {
    p_net_node->p_ts_head = _new_ts_node(p_priv, ts_id);
    if(NULL == p_net_node->p_ts_head)
    {
       p_net_node->p_ts_head = _abnormal_proc_ts_node(p_priv, ts_id,p_net_node);
    }
    return p_net_node->p_ts_head;
  }

  //exist.
  p_cur_node = p_net_node->p_ts_head;
  p_pre_node = p_net_node->p_ts_head;
  while (p_cur_node != NULL)
  {
    if (p_cur_node->ts_id == ts_id)
    {
      break;
    }
    p_pre_node = p_cur_node;
    p_cur_node = p_cur_node->p_next_ts;

    cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->ts_node_num);
  }

  if (p_cur_node == NULL)
  {
    p_cur_node = _new_ts_node(p_priv, ts_id);
    if(p_cur_node != NULL)
    {
      p_pre_node->p_next_ts = p_cur_node;
    }
    else
      {
          /*!
         *because delete all event.the link is breaken,it dn't need to link pre node,it will build link by oneself.
         */
          p_cur_node = _abnormal_proc_ts_node(p_priv, ts_id,p_net_node);
      }
  }

  return p_cur_node;
}

static network_node_t *_add_network_node(epg_db_priv_t *p_priv, u16 network_id)
{
  network_node_t *p_pre_node = NULL;
  network_node_t *p_cur_node = NULL;
  u32 cnk = 0; 
  //first
  if(p_priv->p_net_head == NULL)
  {
    p_priv->p_net_head = _new_network_node(p_priv, network_id);
    CHECK_FAIL_RET_NULL(p_priv->p_net_head != NULL);
    return p_priv->p_net_head;
  }

  //exist.
  p_cur_node = p_priv->p_net_head;
  p_pre_node = p_priv->p_net_head;
  while(p_cur_node)
  {

    if(p_cur_node->network_id == network_id)
    {
      break;   
    }
    else
    {
     //OS_PRINTF("new net id:0x%x;cur net id:0x%x  node:0x%x;next node:0x%x\n",
                                          // network_id,
                                           //p_cur_node->network_id
     //                                   ,p_cur_node,p_cur_node->p_next_net);
    }
    p_pre_node = p_cur_node;
    p_cur_node = p_cur_node->p_next_net;
    cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->net_node_num);
  }
  //create new.
  if(p_cur_node == NULL)
  {
    p_cur_node = _new_network_node(p_priv, network_id);
    if(p_cur_node != NULL)
    {
       p_pre_node->p_next_net = p_cur_node;
    } 
    else
    {
      /*!
       *because delete all event.the link is breaken,it dn't need to link pre node,it will build link by oneself.
       */
       p_cur_node = _abnormal_proc_net_node(p_priv, network_id);
    }
  }
  return p_cur_node;
}

static network_node_t *_get_network_node(epg_db_priv_t *p_priv, u16 network_id)
{
  network_node_t *p_cur_node = NULL;
  u32 cnk = 0;
  p_cur_node = p_priv->p_net_head;
  while(p_cur_node != NULL)
  {
    if(p_cur_node->network_id == network_id)
    {
      return p_cur_node;
    }
    p_cur_node = p_cur_node->p_next_net;
        cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->net_node_num);
  }
  return NULL;
}

static ts_node_t *_get_ts_node(epg_db_priv_t *p_priv, u16 network_id, u16 ts_id)
{
  network_node_t *p_net_node = NULL;
  ts_node_t      *p_ts_node  = NULL;
  u32 cnk = 0;
  p_net_node = _get_network_node(p_priv, network_id);
  if(p_net_node != NULL)
  {
    p_ts_node = p_net_node->p_ts_head;
    while(p_ts_node != NULL)
    {
      if(p_ts_node->ts_id == ts_id)
      {
        return p_ts_node;
      }
      p_ts_node = p_ts_node->p_next_ts;
     cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->ts_node_num);
    }
  }
  return NULL;
}

static svc_node_t *_get_svc_node_by_net_and_ts(epg_db_priv_t *p_priv,
                                               u16            network_id,
                                               u16            ts_id,
                                               u16            svc_id)
{
  ts_node_t  *p_ts_node  = NULL;
  svc_node_t *p_svc_node = NULL;
  u32 cnk = 0;
  p_ts_node = _get_ts_node(p_priv, network_id, ts_id);
  if(p_ts_node != NULL)
  {
    p_svc_node = p_ts_node->p_svc_head;
    while(p_svc_node != NULL)
    {
      if(p_svc_node->svc_id == svc_id)
      {
        return p_svc_node;
      }
      p_svc_node = p_svc_node->p_next_svc_node;
          cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->svc_node_num);
    }
  }
  return NULL;
}

static svc_node_t *_get_svc_node_by_network(epg_db_priv_t *p_priv,
                                            u16            network_id,
                                            u16            svc_id)
{
  network_node_t *p_net_node = NULL;
  ts_node_t      *p_ts_node  = NULL;
  svc_node_t     *p_svc_node = NULL;
  u32 cnk = 0,cnk1 = 0;
  //get network node.
  p_net_node = _get_network_node(p_priv, network_id);
  if(p_net_node != NULL)
  {
    //get ts node head, and ergodic all ts node.
    p_ts_node = p_net_node->p_ts_head;
    while(p_ts_node != NULL)
    {
      //get svc node head, and ergodic all svc node.
      p_svc_node = p_ts_node->p_svc_head;
      while(p_svc_node != NULL)
      {
        if(p_svc_node->svc_id == svc_id)
        {
          return p_svc_node;
        }
        //get next svc_node.
        p_svc_node = p_svc_node->p_next_svc_node;
            cnk ++;
        CHECK_FAIL_RET_NULL(cnk <= p_priv->svc_node_num);
      }
      //get next ts_node.
      p_ts_node = p_ts_node->p_next_ts;

          cnk1 ++;
      CHECK_FAIL_RET_NULL(cnk1 <= p_priv->ts_node_num);
    }
  }
  return NULL;
}
static svc_node_t *_get_svc_node_by_svc(epg_db_priv_t *p_priv,
                                                                              u16 svc_id)
{
  network_node_t *p_net_node = NULL;
  ts_node_t      *p_ts_node  = NULL;
  svc_node_t     *p_svc_node = NULL;
  u32 cnk = 0,cnk1 = 0,cnk2 = 0;
 /*!
  get net node head, and ergodic all net node.
  */
  p_net_node = p_priv->p_net_head;
  while(p_net_node != NULL)
  {
   /*!
    get ts node head, and ergodic all ts node.
    */
    p_ts_node = p_net_node->p_ts_head;
    while (p_ts_node != NULL)
    {
       /*!
        get svc node head, and ergodic all svc node.
        */
        p_svc_node = p_ts_node->p_svc_head;
        while(p_svc_node != NULL)
        {
          if(p_svc_node->svc_id == svc_id)
          {
            return p_svc_node;
          }
         /*!
          get next svc node.
          */
          p_svc_node = p_svc_node->p_next_svc_node;
             cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->svc_node_num);
        }
     /*!
      get next ts node.
      */
      p_ts_node = p_ts_node->p_next_ts;
      cnk1 ++;
    CHECK_FAIL_RET_NULL(cnk1 <= p_priv->ts_node_num);
    }

   /*!
    get next net node.
    */
    p_net_node = p_net_node->p_next_net;
                cnk2 ++;
    CHECK_FAIL_RET_NULL(cnk2 <= p_priv->net_node_num);
  }
  return NULL;
}

static svc_node_t *_get_svc_node_by_ts(epg_db_priv_t *p_priv,
                                       u16            ts_id,
                                       u16            svc_id)
{
  network_node_t *p_net_node = NULL;
  ts_node_t      *p_ts_node  = NULL;
  svc_node_t     *p_svc_node = NULL;
  u32 cnk = 0,cnk1 = 0,cnk2 = 0;
 /*!
  get net node head, and ergodic all net node.
  */
  p_net_node = p_priv->p_net_head;
  while(p_net_node != NULL)
  {
   /*!
    get ts node head, and ergodic all ts node.
    */
    p_ts_node = p_net_node->p_ts_head;
    while (p_ts_node != NULL)
    {
      if(p_ts_node->ts_id == ts_id)
      {
       /*!
        get svc node head, and ergodic all svc node.
        */
        p_svc_node = p_ts_node->p_svc_head;
        while(p_svc_node != NULL)
        {
          if(p_svc_node->svc_id == svc_id)
          {
            return p_svc_node;
          }
         /*!
          get next svc node.
          */
          p_svc_node = p_svc_node->p_next_svc_node;
             cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->svc_node_num);
        }
      }
     /*!
      get next ts node.
      */
      p_ts_node = p_ts_node->p_next_ts;
      cnk1 ++;
    CHECK_FAIL_RET_NULL(cnk1 <= p_priv->ts_node_num);
    }

   /*!
    get next net node.
    */
    p_net_node = p_net_node->p_next_net;
                cnk2 ++;
    CHECK_FAIL_RET_NULL(cnk2 <= p_priv->net_node_num);
  }
  return NULL;
}

static svc_node_t *_get_svc_node(epg_db_priv_t *p_priv,
                                 u16            network_id,
                                 u16            ts_id,
                                 u16            svc_id)
{
  svc_node_t *p_svc_node = NULL;

 /*!
  supports ignore network_id, because DVB-C only has one network id.
  */
  if((network_id == EPG_IGNORE_ID) && (ts_id != EPG_IGNORE_ID))
  {
    p_svc_node = _get_svc_node_by_ts(p_priv, ts_id, svc_id);
  }
  else if((network_id == EPG_IGNORE_ID) 
              && (ts_id == EPG_IGNORE_ID)
              && (svc_id != EPG_IGNORE_ID))
  {
    p_svc_node = _get_svc_node_by_svc(p_priv, svc_id);
  }
  else
  {
   /*!
    1. found svc_node : network id + ts id + svc id.
    */
    p_svc_node = _get_svc_node_by_net_and_ts(p_priv, network_id, ts_id, svc_id);

    if(p_svc_node == NULL)
    {

     /*!
      2. found svc_node : network id + svc_id.
      */
      p_svc_node = _get_svc_node_by_network(p_priv, network_id, svc_id);

      if(p_svc_node == NULL)
      {
       /*!
        3. found svc_node : ts_id + svc_id.
        */
        p_svc_node = _get_svc_node_by_ts(p_priv, ts_id, svc_id);
        if(p_svc_node == NULL)
        {
         /*!
          4. found svc_node : only svc_id.
          */
          p_svc_node = _get_svc_node_by_svc(p_priv,svc_id);
        }
      }
    }
  }

  return p_svc_node;
}

/*!
  return 1  : is after this period of time
  return 0  : is in this period of time
  return -1 : is before this period of time
  */
static s8 _is_event_in_period(event_node_t *p_evt_node,
                              utc_time_t *p_start_time,
                              utc_time_t *p_end_time)
{
  utc_time_t evt_end_time = {0};

  CHECK_FAIL_RET_ZERO(p_evt_node != NULL);
  CHECK_FAIL_RET_ZERO(p_start_time != NULL);
  CHECK_FAIL_RET_ZERO(p_end_time != NULL);

 /*!
  Calc event end time
  */
  memcpy(&evt_end_time, &p_evt_node->start_time, sizeof(utc_time_t));
  time_add(&evt_end_time, &p_evt_node->drt_time);

 /*!
  event's end time <= start time
  */
  if(time_cmp(&evt_end_time, p_start_time, FALSE) <= 0)
  {
    return -1;
  }

 /*!
  event's start time >= end time
  */
  if(time_cmp(&p_evt_node->start_time, p_end_time, FALSE) >= 0)
  {
    return 1;
  }

  return 0;
}

static void _check_prog_info(epg_prog_info_t *p_prog_info)
{
  utc_time_t start_time = { 0 };

  if(time_cmp(&p_prog_info->start_time, &p_prog_info->end_time, FALSE) > 0)
  {
    PRINT_TIME(&p_prog_info->start_time, "start");
    PRINT_TIME(&p_prog_info->end_time, "end");
    CHECK_FAIL_RET_VOID(0);
  }

 /*!
  get current system time
  */
  time_get(&start_time, TRUE);

 /*!
  if system time bigger than p_info->start_time
  */
  if(time_cmp(&start_time, &p_prog_info->start_time, FALSE) >= 0)
  {
    p_prog_info->start_time = start_time;
  }
}


static void _epg_db_delete_event(epg_event_t evt_type,epg_event_note_t note_type,u32 context)
{
  epg_db_priv_t  *p_priv     = _epg_db_get_priv();
  network_node_t *p_curr_net = p_priv->p_net_head;
  network_node_t *p_prev_net = p_priv->p_net_head;
  network_node_t *p_next_net = NULL;
  BOOL            b_act_del  = FALSE;
  BOOL            b_head_net = FALSE;
  u16             network_id = p_priv->prog_info.network_id;
  u32 cnk = 0;
  while(p_curr_net != NULL)
  {
    b_head_net = (p_curr_net == p_priv->p_net_head) ? TRUE : FALSE;
    p_next_net = p_curr_net->p_next_net;
  
   /*!
    case 1: delete others network_id event :
       (1) : not match current network_id, please delete EPG_ALL_EVENT.
       (2) : not save all current network_id event, for example :
             only save current net id + ts id. please clear network_id bit.
    case 2: delete all network_id event.
    */
    if(IS_NET_ID_EVENT(evt_type))//case 1
    {
      if(p_curr_net->network_id != network_id)//case 1 - (1)
      {
        b_act_del = _del_network_node(p_priv, p_curr_net,
                                EPG_UNKNOWN_EVENT,
                                note_type, context);
      }
      else if(!ONLY_NET_ID_EVENT(evt_type))//case 1 - (2)
      {
        b_act_del = _del_network_node(p_priv, p_curr_net, 
          CLEAR_NET_ID_BIT(evt_type),note_type, context);
      }
    }
    else if(FIXD_SVC_ID_EVENT(evt_type))
    {
      b_act_del = _del_network_node(p_priv, p_curr_net, evt_type,note_type, context);
    }
    else//case 2
    {
      b_act_del = _del_network_node(p_priv, p_curr_net, evt_type,note_type, context);
    }

     
    /*!
    if actual del svc_node, please move to ts_node->svc_list.
    */
    if(b_act_del)
    {
      /*!
      if the head_svc_node, please move the head_svc to next svc_node.
      */
      if(b_head_net)
      {
        p_priv->p_net_head = p_next_net;
        p_prev_net = p_priv->p_net_head;
      }
      else
      {
        p_prev_net->p_next_net = p_next_net;
      }
    }
    else
    {
       p_prev_net = p_curr_net;
    }
    
    p_curr_net = p_next_net;

    cnk ++;
    CHECK_FAIL_RET_VOID(cnk <= p_priv->net_node_num);
  }
}

event_id_info_t _check_pf_event_id(svc_node_t  *p_svc_node, 
                               epg_event_note_t    event_note_type,
                              u16 evt_id,
                              event_node_t **p_pf_event_node)
{
  event_node_t *p_evt_node = NULL;
  epg_db_priv_t *p_priv     = _epg_db_get_priv();

  /*!
    Get event node
    */
  p_evt_node = IS_PRESENT_EVENT(event_note_type) ?
            p_svc_node->p_present_evt : p_svc_node->p_follow_evt;

  /*!
    check is new event node.
    */
  if(p_evt_node == NULL)
  {
    *p_pf_event_node =  NULL;
    return EVT_ID_EXIST_NO;
  }
  else if(p_evt_node->event_id != evt_id)/***new pf,delete old sht text**/
  {
  
     *p_pf_event_node =  p_evt_node;
     /**delete pre text***/
      if((p_evt_node->p_sht_text != NULL) && (p_priv->text_able == TRUE))
      {
          _free_nonfix(&p_priv->memp, p_evt_node->p_sht_text);
          p_evt_node->p_sht_text = NULL;
      }

      if((p_evt_node->p_ext_desc_list != NULL) && (p_priv->ext_able == TRUE))
      {
          _free_nonfix(&p_priv->memp, p_evt_node->p_ext_desc_list);
          p_evt_node->p_ext_desc_list = NULL;
      }
      return EVT_ID_EXIST_NO;
  }
  else if(p_evt_node->event_id == evt_id) 
  {
    if(((p_evt_node->p_sht_text == NULL) && (p_priv->text_able == TRUE))
      || ((p_evt_node->p_ext_desc_list == NULL) && (p_priv->ext_able == TRUE)))
       // && (p_priv->fixed_pf_text_policy > PF_FIXED_TEXT_NO_ID))
      {
          *p_pf_event_node =  p_evt_node;
          return EVT_ID_EXIST_LACK;
      }
  }
  *p_pf_event_node =  p_evt_node;
  return EVT_ID_EXIST_FULL;
}

event_id_info_t _check_schedule_event_id(svc_node_t   *p_svc_node,
                                        u16 evt_id,
                                        event_node_t **p_sch_event_node)
{
  event_node_t *p_evt_node = p_svc_node->p_sch_evt_head;
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  u32 cnk = 0;
  //check this event is in this day.
  while(p_evt_node != NULL)
  {
    //uniquely allocated within a service definition.
    if(p_evt_node->event_id == evt_id)
    {
      *p_sch_event_node = p_evt_node;

      if(p_priv->fixed_sch_text_policy > SCH_FIXED_TEXT_NO_ID) /**ha fixed sch text policy***/
      {
         if(((p_priv->text_able == TRUE) && (p_evt_node->p_sht_text == NULL))
           ||((p_priv->ext_able == TRUE) && (p_evt_node->p_ext_desc_list == NULL)))
          {
            return EVT_ID_EXIST_LACK;
          }
         else
          {
             return EVT_ID_EXIST_FULL;
          }
      }
      else
      {
          return EVT_ID_EXIST_FULL;
      }
    }
    p_evt_node = p_evt_node->p_next_evt_node;
    
    cnk ++;
    CHECK_FAIL_RET((cnk <= p_priv->evt_node_num), EVT_ID_EXIST_NO);
  }
  *p_sch_event_node = NULL;
  
  return EVT_ID_EXIST_NO;
}

event_id_info_t epg_db_check_event_id(epg_svc_t *p_svc, u16 evt_id)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  svc_node_t *p_svc_node = (svc_node_t *)p_svc->p_node;
  event_node_t *p_evt_node = NULL;
  event_id_info_t ret = EVT_ID_EXIST_NO;
  CHECK_FAIL_RET((p_svc_node != NULL), EVT_ID_EXIST_NO);
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
 // OS_PRINTF("### sem take :%s:%d\n",__FUNCTION__,__LINE__);
  if(p_svc_node->is_new_node)
  {
      ret = EVT_ID_EXIST_NO;
  }
  else
  {
      if(p_svc->pf_table)
      {
        ret =  _check_pf_event_id(p_svc_node,p_svc->evt_note_type ,evt_id,&p_evt_node);
      }
      else
      {
        ret =  _check_schedule_event_id(p_svc_node, evt_id,&p_evt_node);
      }
  }
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  return ret;
}

/*!
  epg db check max_evt_days.
  */
RET_CODE epg_db_check_max_evt_days_event(short_event_t  *p_sht_evt,epg_eit_t *p_epg_eit)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  utc_time_t     max_evt_days_time = { 0 };
  utc_time_t     sht_evt_end_time = { 0 };
  RET_CODE ret = ERR_FAILURE;
  /*!
    get current system time
    */
    mtos_sem_take(&p_priv->epg_data_base_sem, 0);
    //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
    time_get(&max_evt_days_time, TRUE);

   if((p_priv->prog_info.network_id == p_epg_eit->org_nw_id) 
      && (p_priv->prog_info.ts_id == p_epg_eit->ts_id) 
      && (p_priv->prog_info.svc_id == p_epg_eit->svc_id))
    {
      time_up(&max_evt_days_time, p_priv->max_actual_event_days * 24 * 60 * 60);
    }
   else
    {
       time_up(&max_evt_days_time, p_priv->max_other_event_days * 24 * 60 * 60);
    }
  
    memcpy(&sht_evt_end_time,&p_sht_evt->start_time,sizeof(utc_time_t));
    time_add(&sht_evt_end_time, &p_sht_evt->drt_time);
    
    /**filter max evt days time**/
    if(time_cmp(&sht_evt_end_time,&max_evt_days_time,FALSE) >= 0)
    {
       ret = ERR_FAILURE;
    }
    else
    {
        ret = SUCCESS;
    }
    //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
    mtos_sem_give(&p_priv->epg_data_base_sem);
    return ret;
}

//u32 add_evt_ticks = 0;
RET_CODE epg_db_add_event(epg_eit_t *p_eit, epg_svc_t *p_svc)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  svc_node_t    *p_svc_node = (svc_node_t *)p_svc->p_node;
  RET_CODE       ret        = ERR_FAILURE;
  BOOL lang_switch = FALSE;
  //add_evt_ticks = mtos_ticks_get();
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
 //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);

  epg_db_process_eit_edit_info_call_back(p_eit);

   /*****filter epg lang code*********/
  lang_switch = epg_db_try_lang_code_priority(p_eit);
  if(lang_switch == TRUE)
  {
       _epg_db_delete_event(EPG_UNKNOWN_EVENT,EPG_ALL_EVENT,0);
       p_eit->message_flag = TRUE;
       mtos_sem_give(&p_priv->epg_data_base_sem);
      return SUCCESS;
  }
  epg_db_filter_evt_lang_code(p_eit);
   /**********filter epg lang code end**/
   
  if(p_svc->pf_table)
  {
    ret = _add_pf_evt_to_svc(p_priv, p_eit, p_svc->evt_note_type, p_svc_node);    
  }
  else
  {
    ret = _add_sch_evt_to_svc(p_priv, p_eit, p_svc_node);
  }
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  //OS_PRINTF("**add evt time =%d\n",mtos_ticks_get()-add_evt_ticks);
  return ret;
}

RET_CODE epg_db_add_svc(epg_eit_t *p_eit, epg_svc_t *p_svc)
{
  epg_db_priv_t  *p_priv     = _epg_db_get_priv();
  network_node_t *p_net_node = NULL;
  ts_node_t      *p_ts_node  = NULL;
  svc_node_t     *p_svc_node = NULL; 
  RET_CODE ret = SUCCESS;
  
  CHECK_FAIL_RET_CODE(p_svc != NULL);
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
  //add network
  p_net_node = _add_network_node(p_priv, p_eit->org_nw_id);
  //EPG_DB_WARN(p_net_node != NULL);
  if(p_net_node == NULL)
  {
     ret = ERR_FAILURE;
     mtos_sem_give(&p_priv->epg_data_base_sem);
     return ret;
  }
  //add transport stream
  p_ts_node = _add_ts_node_to_network(p_priv, p_eit->ts_id, p_net_node);
  //EPG_DB_WARN(p_ts_node != NULL);
   if(p_ts_node == NULL)
  {
     ret = ERR_FAILURE;
     mtos_sem_give(&p_priv->epg_data_base_sem);
     return ret;
  }

  //add service
  p_svc_node = _add_svc_node_to_ts(p_priv, p_eit->svc_id, p_net_node,p_ts_node);
  //EPG_DB_WARN(p_svc_node != NULL);
  if(p_svc_node == NULL)
  {
     ret = ERR_FAILURE;
     mtos_sem_give(&p_priv->epg_data_base_sem);
     return ret;
  }
  
  p_svc->p_node   = (void *)p_svc_node;
  p_svc->pf_table = p_eit->pf_table;

  //add event node to service node. only one event in p/f table.
  if(p_svc->pf_table)
  {
    p_svc->evt_note_type = (p_eit->section_number == 0) ? 
                      EPG_PRESENT_EVENT : EPG_FOLLOW_EVENT; 
  }
  else
  {
    p_svc->evt_note_type = EPG_SCHEDULE_EVENT;
  }
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  
  return ret;
}  


void epg_db_delete(epg_event_t evt_type)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take :%s:%d\n",__FUNCTION__,__LINE__);
  _epg_db_delete_event(evt_type,EPG_ALL_EVENT,0);
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  
}

/*!
 epg db get pf event
 */
RET_CODE epg_db_get_pf_event(epg_prog_info_t *p_info,
                             event_node_t    **pp_present_evt,
                             event_node_t    **pp_follow_evt)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  svc_node_t    *p_svc_node = NULL;
  utc_time_t    sys_time     = {0};
  utc_time_t    end_time     = {0};

 /*!
  check filter parameter.
  */
  CHECK_FAIL_RET_CODE(p_info  != NULL);
  CHECK_FAIL_RET_CODE(pp_present_evt != NULL);
  CHECK_FAIL_RET_CODE(pp_follow_evt  != NULL);
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
  //OS_PRINTF("\n get pf event :net_id:0x%x;ts_id:0x%x;svc_id:0x%x \n",
  //p_info->network_id,p_info->ts_id,p_info->svc_id);

 /*!
  get svc node.
  */
  p_svc_node = _get_svc_node(p_priv,
                             p_info->network_id,
                             p_info->ts_id,
                             p_info->svc_id);
  if(p_svc_node == NULL)
  {
    //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
    mtos_sem_give(&p_priv->epg_data_base_sem);
    return ERR_FAILURE;
  }

 /*!
  assign to event_pf_t.
  */
  time_get(&sys_time, TRUE);
  //delete present event node
  if(p_svc_node->p_present_evt != NULL)
  {
    end_time = p_svc_node->p_present_evt->start_time;
    time_add(&end_time, &p_svc_node->p_present_evt->drt_time);
#if  0
          {
            u8 asc_str[32];
            utc_time_t event_end_time = {0};
            
            memcpy(&event_end_time,&p_svc_node->p_present_evt->start_time,sizeof(utc_time_t));
            
            time_add(&event_end_time,&p_svc_node->p_present_evt->drt_time);
            sprintf((char *)asc_str, "%.2d:%.2d::::%.2d:%.2d~%d:%d",
                          sys_time.hour,sys_time.minute,
                          p_svc_node->p_present_evt->start_time.hour,
                          p_svc_node->p_present_evt->start_time.minute,
                          event_end_time.hour,event_end_time.minute);
              OS_PRINTF("\n get pf event :%s  %s \n",asc_str,p_svc_node->p_present_evt->event_name);

             }
#endif

    if(time_cmp(&end_time,&sys_time,FALSE) >= 0)
    {
       *pp_present_evt = p_svc_node->p_present_evt;
    }
    else
      {
         *pp_present_evt = NULL;
         //OS_PRINTF("get pf it ist over time or no evter!\n");
      }
  }

  //delete following event node.
  if(p_svc_node->p_follow_evt != NULL)
  {
    end_time = p_svc_node->p_follow_evt->start_time;
    time_add(&end_time, &p_svc_node->p_follow_evt->drt_time);
    if(time_cmp(&end_time,&sys_time,FALSE) >= 0)
    {
      *pp_follow_evt  = p_svc_node->p_follow_evt;
    }
    else
    {
      *pp_follow_evt  = NULL;
    }
  }
  
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  return SUCCESS;
}

/*!
 epg db get schedule event
 */
event_node_t *epg_db_get_sch_event(epg_prog_info_t *p_prog_info, u16 *p_event_num)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  svc_node_t    *p_svc_node = NULL;
  event_node_t  *p_evt_head = NULL;
  event_node_t  *p_temp_evt = NULL;
  s8             result     = 0;
  u32 cnk = 0;

 /*!
  check param
  */
  CHECK_FAIL_RET_NULL(p_prog_info != NULL);
  CHECK_FAIL_RET_NULL(p_event_num != NULL);
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
  _check_prog_info(p_prog_info);
  *p_event_num = 0;

 /*!
  get svc node.
  */
  p_svc_node = _get_svc_node(p_priv,
                             p_prog_info->network_id,
                             p_prog_info->ts_id,
                             p_prog_info->svc_id);
  if(p_svc_node == NULL)
  {
    //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
    mtos_sem_give(&p_priv->epg_data_base_sem);
    return NULL;
  }

 /*!
  found first time
  */
  p_temp_evt = p_svc_node->p_sch_evt_head;

  while(p_temp_evt != NULL)
  {
#if  0
{
  u8 asc_str[32];
  utc_time_t event_end_time = {0};
  
  memcpy(&event_end_time,&p_temp_evt->start_time,sizeof(utc_time_t));
  
  time_add(&event_end_time,&p_temp_evt->drt_time);
  sprintf((char *)asc_str, "%.2d:%.2d~%d:%d",
                p_temp_evt->start_time.hour,p_temp_evt->start_time.minute,
                event_end_time.hour,event_end_time.minute);
    OS_PRINTF("get sch event :%s  %s\n",asc_str,p_temp_evt->event_name);
 }
#endif

  result = _is_event_in_period(p_temp_evt,
                               &p_prog_info->start_time,
                               &p_prog_info->end_time);

   /*!
    in this period.
    */
    if(result == 0)
    {
      if(p_evt_head == NULL)
      {
        p_evt_head = p_temp_evt;
      }
      (*p_event_num)++;
    }
    else if(result == 1) //after this period.
    {
      break;
    }
    p_temp_evt = p_temp_evt->p_next_evt_node;

    cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->evt_node_num);
  }
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  return p_evt_head;
}

event_node_t *epg_db_get_sch_event_by_pos(epg_prog_info_t *p_prog_info,
                                          event_node_t    *p_evt_head,     
                                          u16              offset)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  event_node_t *p_next_evt = NULL;
  s8            result     = 0;
  u16           count      = 0;
  u32 cnk = 0;
 /*!
  check input parameter.
  */
  CHECK_FAIL_RET_NULL(p_evt_head != NULL);
  CHECK_FAIL_RET_NULL(p_prog_info != NULL);
  CHECK_FAIL_RET_NULL(offset > 0);
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
  _check_prog_info(p_prog_info);

 /*!
  found first time
  */
  p_next_evt = p_evt_head->p_next_evt_node;
  while(p_next_evt != NULL)
  {
    result = _is_event_in_period(p_next_evt,
                                 &p_prog_info->start_time,
                                 &p_prog_info->end_time);
   /*!
    in this period.
    */
    if(result == 0)
    {
     /*!
      if found it, return.
      */
      if(++count == offset)
      {
        //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
        mtos_sem_give(&p_priv->epg_data_base_sem);
        return p_next_evt;
      }
    }
    else if(result == 1) //after this period.
    {
      break;
    }
    p_next_evt = p_next_evt->p_next_evt_node;
    
    cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->evt_node_num);
    
  }
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  return NULL;
}


event_node_t *epg_db_get_event_by_nibble(epg_nibble_t *p_nibble,
                                         u16          *p_event_num)
{
  epg_db_priv_t *p_priv    = _epg_db_get_priv();
  svc_node_t   *p_svc_node = NULL;
  event_node_t *p_head_evt = NULL;
  event_node_t *p_temp_evt = NULL;
  s8            result     = 0;
  u8            con_level1 = (p_nibble->nibble_level & 0XF0);
  u32 cnk = 0;
  (*p_event_num) = 0;
 /*!
  check filter parameter.
  */
  CHECK_FAIL_RET_NULL(p_nibble != NULL);
  CHECK_FAIL_RET_NULL(p_event_num != NULL);
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take :%s:%d\n",__FUNCTION__,__LINE__);
  _check_prog_info(&p_nibble->prog_info);
  *p_event_num = 0;

 /*!
  get svc node.
  */
  p_svc_node = _get_svc_node(p_priv,
                             p_nibble->prog_info.network_id,
                             p_nibble->prog_info.ts_id,
                             p_nibble->prog_info.svc_id);
  if(p_svc_node == NULL)
  {
    //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
    mtos_sem_give(&p_priv->epg_data_base_sem);
    return NULL;
  }

 /*!
  found first time
  */
  p_temp_evt = p_svc_node->p_sch_evt_head;
  while(p_temp_evt != NULL)
  {
    result = _is_event_in_period(p_temp_evt,
                                 &p_nibble->prog_info.start_time,
                                 &p_nibble->prog_info.end_time);
   /*!
    in this period.
    */
    if((result == 0) && ((p_temp_evt->cont_level & 0xF0) == con_level1))
    {
      if(p_head_evt == NULL)
      {
        p_head_evt = p_temp_evt;
      }
      (*p_event_num) = 1;
    }
    else if(result == 1) //after this period.
    {
      break;
    }    
    p_temp_evt = p_temp_evt->p_next_evt_node;
    cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->evt_node_num);
  }
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  return p_head_evt;
}

/*!
 epg db get fp event by nibble
 */
event_node_t * epg_db_get_pf_event_by_nibble(epg_nibble_t *p_nibble,
                                                u16 *p_event_num)
{
  epg_db_priv_t *p_priv     = _epg_db_get_priv();
  svc_node_t    *p_svc_node = NULL;
  utc_time_t    end_time     = {0};
  event_node_t *p_head_evt = NULL;
  u8  con_level1 = (p_nibble->nibble_level & 0XF0);
  
 /*!
  check filter parameter.
  */
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
  //OS_PRINTF("\n get pf event :net_id:0x%x;ts_id:0x%x;svc_id:0x%x \n",
  //p_info->network_id,p_info->ts_id,p_info->svc_id);

 /*!
  get svc node.
  */
  (*p_event_num) = 0;
  p_svc_node = _get_svc_node(p_priv,
                             p_nibble->prog_info.network_id,
                             p_nibble->prog_info.ts_id,
                             p_nibble->prog_info.svc_id);
  if(p_svc_node == NULL)
  {
    //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
    mtos_sem_give(&p_priv->epg_data_base_sem);
    return p_head_evt;
  }

  if(p_svc_node->p_present_evt != NULL) 
  {
    end_time = p_svc_node->p_present_evt->start_time;
    time_add(&end_time, &p_svc_node->p_present_evt->drt_time);
#if  0
          {
            u8 asc_str[32];
            utc_time_t event_end_time = {0};
            
            memcpy(&event_end_time,&p_svc_node->p_present_evt->start_time,sizeof(utc_time_t));
            
            time_add(&event_end_time,&p_svc_node->p_present_evt->drt_time);
            sprintf((char *)asc_str, "%.2d:%.2d::::%.2d:%.2d~%d:%d",
                          sys_time.hour,sys_time.minute,
                          p_svc_node->p_present_evt->start_time.hour,
                          p_svc_node->p_present_evt->start_time.minute,
                          event_end_time.hour,event_end_time.minute);
              OS_PRINTF("\n get pf event :%s  %s \n",asc_str,p_svc_node->p_present_evt->event_name);

             }
#endif

    if((time_cmp(&end_time,&p_nibble->prog_info.start_time,FALSE) >= 0) 
        && (time_cmp(&p_nibble->prog_info.end_time,
                  &p_svc_node->p_present_evt->start_time,FALSE) >= 0)
        && ((p_svc_node->p_present_evt->cont_level & 0xF0) == con_level1))
    {
       p_head_evt = p_svc_node->p_present_evt;
       (*p_event_num) = 1;
    }
    else
      {
         p_head_evt = NULL;
         (*p_event_num) = 0;
         //OS_PRINTF("get pf it ist over time or no evter!\n");
      }
  }

  //delete following event node.
  
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  return p_head_evt;
}
event_node_t *epg_db_get_next_event_by_nibble(epg_nibble_t *p_nibble,
                                              event_node_t *p_evt_head)
{
  epg_db_priv_t *p_priv    = _epg_db_get_priv();
  event_node_t *p_next_evt = NULL;
  s8            result     = 0;
  u8            con_level1 = (p_nibble->nibble_level & 0XF0);
  u32 cnk = 0; 
  /*!
  check filter parameter.
  */
  CHECK_FAIL_RET_NULL(p_nibble != NULL);
  CHECK_FAIL_RET_NULL(p_evt_head != NULL);
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
  _check_prog_info(&p_nibble->prog_info);

 /*!
  found first time
  */
  p_next_evt = p_evt_head->p_next_evt_node;
  while(p_next_evt != NULL)
  {
    result = _is_event_in_period(p_next_evt,
                                 &p_nibble->prog_info.start_time,
                                 &p_nibble->prog_info.end_time);
   /*!
    in this period.
    */
    if((result == 0) && ((p_next_evt->cont_level & 0xF0) == con_level1))
    {
      //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
      mtos_sem_give(&p_priv->epg_data_base_sem);
      return p_next_evt;
    }
    else if(result == 1) //after this period.
    {
      break;
    }
    p_next_evt = p_next_evt->p_next_evt_node;
    cnk ++;
    CHECK_FAIL_RET_NULL(cnk <= p_priv->evt_node_num);
  }
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
  return NULL;
}


/*!
 epg set program info.

 \param[in] p_prog_info : program info.
 */
void epg_db_set_info(epg_prog_info_t *p_prog_info)
{
  epg_db_priv_t *p_priv = _epg_db_get_priv();
  mtos_sem_take(&p_priv->epg_data_base_sem, 0);
  //OS_PRINTF("### sem take:%s:%d\n",__FUNCTION__,__LINE__);
  p_priv->prog_info.network_id = p_prog_info->network_id;
  p_priv->prog_info.ts_id = p_prog_info->ts_id;
  p_priv->prog_info.svc_id = p_prog_info->svc_id;
  p_priv->prog_info.start_time = p_prog_info->start_time;
  p_priv->prog_info.end_time = p_prog_info->end_time; 
  //OS_PRINTF("### sem take end:%s:%d\n",__FUNCTION__,__LINE__);
  mtos_sem_give(&p_priv->epg_data_base_sem);
}



/*!
 epg get program info.

 \param[out] p_prog_info : program info.
 */
void epg_db_get_info(epg_prog_info_t *p_prog_info)
{
  epg_db_priv_t *p_priv = _epg_db_get_priv();

  CHECK_FAIL_RET_VOID(p_prog_info != NULL);

  p_prog_info->network_id = p_priv->prog_info.network_id;
  p_prog_info->ts_id = p_priv->prog_info.ts_id;
  p_prog_info->svc_id = p_priv->prog_info.svc_id;
  p_prog_info->start_time = p_priv->prog_info.start_time;
  p_prog_info->end_time = p_priv->prog_info.end_time;
}
/*!
 epg get db config help info.
 */
void epg_db_get_config_help_info(u16 net_node_num,
                                                               u16 ts_node_num,
                                                               u16 svc_node_num,
                                                               u16 evt_node_num,
                                                               u32 mem_size)
{
  u32            net_mem_size = 0;
  u32            ts_mem_size  = 0;
  u32            svc_mem_size = 0;
  u32            evt_mem_size = 0;
  u32            total_mem_size = 0;
  
  OS_PRINTF("\n***********db config info**************\n");
  OS_PRINTF("****node size: net[%d]B,ts[%d]B,svc[%d]B,evt[%d]B\n",
                                     sizeof(network_node_t),
                                     sizeof(ts_node_t),
                                     sizeof(svc_node_t),
                                     sizeof(event_node_t));
  net_mem_size = net_node_num * sizeof(network_node_t);
  ts_mem_size  = ts_node_num  * sizeof(ts_node_t);
  svc_mem_size = svc_node_num * sizeof(svc_node_t);
  evt_mem_size = evt_node_num * sizeof(event_node_t);
  total_mem_size = net_mem_size + ts_mem_size 
                         + svc_mem_size + evt_mem_size;
  OS_PRINTF("***total node size[%d]KB :net [%d]B;ts [%d]B;svc [%d]B;evt [%d]B\n",
                                        total_mem_size / 1024,
                                        net_mem_size,
                                        ts_mem_size,
                                        svc_mem_size,
                                        evt_mem_size);
  
  if(mem_size > total_mem_size)
  {
    OS_PRINTF("\n****epg free size:%dKB\n",(mem_size - total_mem_size) /1024);
   
   OS_PRINTF("\n**** it can adj the evt node to[%d]\n",
                       (mem_size - total_mem_size) / sizeof(event_node_t));
      
  }
  else
  {
    evt_node_num = (mem_size - net_mem_size - ts_mem_size - svc_mem_size) / 
                                                        sizeof(event_node_t);
    OS_PRINTF("\n****the evt node auto adjust :[%d]\n",evt_node_num);
  }
  OS_PRINTF("\n****the memory size don't include epg text fix size\n");
  OS_PRINTF("***********db config info end**************\n");
}

/*!
  epg database config.
  */
void epg_db_config(epg_db_cfg_t *p_cfg)
{
  epg_db_priv_t *p_priv       = _epg_db_get_priv();
  u8            *p_mem        = NULL;
  u8           *p_new_buffer = NULL;
  u32            net_mem_size = 0;
  u32            ts_mem_size  = 0;
  u32            svc_mem_size = 0;
  u32            evt_mem_size = 0;
  u32            non_fix_size = 0;
  u32            total_mem_size = 0;
  RET_CODE       ret          = ERR_FAILURE;

  CHECK_FAIL_RET_VOID(p_cfg != NULL);

/*!
because sizeof(network_node_t) == 12, but net_mem_size must >= sizeof(memf_cb_t)(16).
*/
if(p_cfg->net_node_num == 1)
{
  p_cfg->net_node_num = 2;
}

if((NULL == p_cfg->p_attach_buf) || (0 == p_cfg->buffer_size))
  {
    net_mem_size = p_cfg->net_node_num * sizeof(network_node_t);
    ts_mem_size  = p_cfg->ts_node_num  * sizeof(ts_node_t);
    svc_mem_size = p_cfg->svc_node_num * sizeof(svc_node_t);
    evt_mem_size = p_cfg->evt_node_num * sizeof(event_node_t);
    
     if((TRUE == p_cfg->text_able) || (TRUE == p_cfg->ext_able))
      {
        total_mem_size = net_mem_size + ts_mem_size 
                                     + svc_mem_size + evt_mem_size
                                     + p_cfg->text_buffer_size;
      }
     else
      {
          total_mem_size = net_mem_size + ts_mem_size 
                                     + svc_mem_size + evt_mem_size + (1 * KBYTES);
      }
    
    /**MAX_SHT_TEXT_LEN + MAX_EXT_TXT_LEN + ***/
    p_new_buffer = (u8 *)mtos_malloc(total_mem_size);
    CHECK_FAIL_RET_VOID(NULL != p_new_buffer);
    p_priv->p_db_cfg_buf = p_new_buffer;
    
    p_cfg->p_attach_buf = p_new_buffer;
    p_cfg->buffer_size = total_mem_size;
    
  }
  else
  {
    total_mem_size = p_cfg->buffer_size;
    net_mem_size = p_cfg->net_node_num * sizeof(network_node_t);
    ts_mem_size  = p_cfg->ts_node_num  * sizeof(ts_node_t);
    svc_mem_size = p_cfg->svc_node_num * sizeof(svc_node_t);
    if((TRUE == p_cfg->text_able) || (TRUE == p_cfg->ext_able))
    {
       evt_mem_size = (total_mem_size - net_mem_size - ts_mem_size - svc_mem_size
                                    - p_cfg->text_buffer_size);
       
    }
    else
    {
        evt_mem_size = (total_mem_size - net_mem_size - ts_mem_size - svc_mem_size -(1 * KBYTES));
    }
    p_cfg->evt_node_num = evt_mem_size / sizeof(event_node_t);
    
    OS_PRINTF("db config :total mem size:%d; net size:%d;ts size:%d;svc size:%d;evt size:%d\n",
                           total_mem_size,net_mem_size,ts_mem_size,svc_mem_size,evt_mem_size);
    p_priv->p_db_cfg_buf = NULL;
  }

  p_mem = p_cfg->p_attach_buf;
  CHECK_FAIL_RET_VOID(p_mem != NULL);
  CHECK_FAIL_RET_VOID(p_cfg->buffer_size > 0);

  p_priv->del_pf_able =  p_cfg->del_pf_able; 
  p_priv->ext_able = p_cfg->ext_able;
  p_priv->text_able = p_cfg->text_able;
  p_priv->lang_code_check_times = p_cfg->lang_code_check_times;
  if(p_priv->lang_code_check_times == 0)
  {
    p_priv->lang_code_check_times = 100;
  }
  p_priv->lang_code_forbit_swich = p_cfg->lang_code_forbit_swich;
  p_priv->lang_code_sync = p_cfg->lang_code_sync;
  p_priv->eit_edit_id_call_back = p_cfg->eit_edit_id_call_back;
  p_priv->eit_edit_info_call_back = p_cfg->eit_edit_info_call_back;
  

  p_priv->sch_policy = p_cfg->sch_policy;
  if(p_priv->sch_policy < SCH_EVENT_OF_NETWORK_ID)
  {
      p_priv->sch_policy = SCH_EVENT_OF_NETWORK_ID;
  }
  p_priv->pf_policy  = p_cfg->pf_policy;
  if(p_priv->pf_policy < PF_EVENT_OF_NETWORK_ID)
  {
      p_priv->pf_policy  = PF_EVENT_OF_NETWORK_ID;
  }
  p_priv->fixed_sch_text_policy = p_cfg->fixed_sch_text_policy;
  if(p_priv->fixed_sch_text_policy < SCH_FIXED_TEXT_OF_NET_ID)
  {
      p_priv->fixed_sch_text_policy = SCH_FIXED_TEXT_OF_NET_ID;
  }
  p_priv->fixed_pf_text_policy = p_cfg->fixed_pf_text_policy;
  if(p_priv->fixed_pf_text_policy <  PF_FIXED_TEXT_OF_NET_ID)
  {
      p_priv->fixed_pf_text_policy =  PF_FIXED_TEXT_OF_NET_ID;
  }
   
  p_priv->net_node_num = p_cfg->net_node_num;
  p_priv->ts_node_num  = p_cfg->ts_node_num;
  p_priv->svc_node_num = p_cfg->svc_node_num;
  p_priv->evt_node_num = p_cfg->evt_node_num;


  p_priv->max_actual_event_days = p_cfg->max_actual_event_days;
  p_priv->max_other_event_days = p_cfg->max_other_event_days;
  p_priv->fixed_svc_node_num = p_cfg->fixed_svc_node_num;


  OS_PRINTF("db config :net_node_num:%d; ts_node_num:%d\n",
                     p_priv->net_node_num,p_priv->ts_node_num);
  OS_PRINTF("db config :svc_node_num:%d;evt_node_num:%d\n",
                     p_priv->svc_node_num,p_priv->evt_node_num);
  
  net_mem_size = p_cfg->net_node_num * sizeof(network_node_t);
  ts_mem_size  = p_cfg->ts_node_num  * sizeof(ts_node_t);
  svc_mem_size = p_cfg->svc_node_num * sizeof(svc_node_t);
  evt_mem_size = p_cfg->evt_node_num * sizeof(event_node_t);
  non_fix_size = p_cfg->buffer_size - net_mem_size - ts_mem_size   \
                                    - svc_mem_size - evt_mem_size;
 OS_PRINTF("db config :total mem size:%d; net size:%d;ts size:%d;\n",
                           p_cfg->buffer_size,net_mem_size,ts_mem_size);
  OS_PRINTF("db config :svc size:%d;evt size:%d;non_fix size:%d\n",
                           svc_mem_size,evt_mem_size,non_fix_size);
  CHECK_FAIL_RET_VOID(non_fix_size > MEMP_PIECE_HEADER_SIZE + MEMP_MIN_SIZE);

  //create network node fixed memory.
  ret = lib_memf_create(&p_priv->net_node_memf, (u32)p_mem, net_mem_size, sizeof(network_node_t));
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  p_mem += net_mem_size;

  //create ts node fixed memory.
  ret = lib_memf_create(&p_priv->ts_node_memf, (u32)p_mem, ts_mem_size, sizeof(ts_node_t));
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  p_mem += ts_mem_size;

  //create svc node fixed memory.
  ret = lib_memf_create(&p_priv->svc_node_memf, (u32)p_mem, svc_mem_size, sizeof(svc_node_t));
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  p_mem += svc_mem_size;

  //create event node fixed memory.
  ret = lib_memf_create(&p_priv->evt_node_memf, (u32)p_mem, evt_mem_size, sizeof(event_node_t));
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  p_mem += evt_mem_size;

  //create no fixed memory.
  memset((u8 *)p_mem,0,non_fix_size);
  lib_memp_create(&p_priv->memp, (u32)p_mem, non_fix_size);
  OS_PRINTF("####debug memp headle[0x%x] add:0x%x  end addr:0x%x",
                &p_priv->memp,
                (u32)p_mem,
                p_mem + non_fix_size);
}
/*!
  free config buffer,it malloc myself
  */
void epg_db_config_buffer_free(void)
{
  epg_db_priv_t *p_priv = _epg_db_get_priv();
  if(NULL != p_priv->p_db_cfg_buf)
  {
      mtos_free(p_priv->p_db_cfg_buf);
  }
}
void epg_db_deinit(void)
{
  epg_db_priv_t *p_priv = _epg_db_get_priv();
  _epg_db_delete_event(EPG_UNKNOWN_EVENT,EPG_ALL_EVENT | EPG_TEXT_EVENT, 0);
  epg_db_config_buffer_free();
  mtos_free(p_priv);
  class_register(EPG_CLASS_ID, NULL);
}

RET_CODE epg_db_init(void)
{
  epg_db_priv_t *p_priv = NULL;

  if(class_get_handle_by_id(EPG_CLASS_ID) != NULL)
  {
    return SUCCESS;
  }

  p_priv = (epg_db_priv_t *)mtos_malloc(sizeof(epg_db_priv_t));
  CHECK_FAIL_RET_CODE(p_priv != NULL);
  memset(p_priv, 0X0, sizeof(epg_db_priv_t));

  p_priv->p_net_head = NULL;
  mtos_sem_create (&p_priv->epg_data_base_sem, TRUE);
    
  class_register(EPG_CLASS_ID, p_priv);
  epg_db_init_lang_code();
  return SUCCESS;
}


/*!
  epg database init lang code
  */
static void epg_db_init_lang_code(void)
{
  epg_db_priv_t *p_priv_data = _epg_db_get_priv();

  memset(p_priv_data->set_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8)
                                           * LANG_CODE_MAX_GROUP);
  memset(p_priv_data->first_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8)
                                           * LANG_CODE_MAX_GROUP);
  memset(p_priv_data->second_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  memset(p_priv_data->default_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  
  p_priv_data->lang_code_first_times = 0;
  p_priv_data->lang_code_second_times = 0;
  p_priv_data->lang_code_default_times = 0;
  p_priv_data->lang_code_other_times = 0;

}
/*!
  epg database set lang code
  */
void epg_db_set_lang_code(
  u8 first_lang[LANGUAGE_CODE_LEN],
  u8 second_lang[LANGUAGE_CODE_LEN],
  u8 default_lang[LANGUAGE_CODE_LEN])
{
  epg_db_priv_t *p_priv_data = _epg_db_get_priv();
  mtos_sem_take(&p_priv_data->epg_data_base_sem, 0);
  memset(p_priv_data->first_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8)
                                           * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->first_lang_code[0],first_lang, 
                                           LANGUAGE_CODE_LEN * sizeof(u8));
  memset(p_priv_data->second_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->second_lang_code[0],second_lang, 
                                         LANGUAGE_CODE_LEN * sizeof(u8));
  memset(p_priv_data->default_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->default_lang_code[0],default_lang, 
                                         LANGUAGE_CODE_LEN * sizeof(u8));
  memset(p_priv_data->set_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8)
                                           * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->set_lang_code[0],first_lang, 
                                          LANGUAGE_CODE_LEN * sizeof(u8));
mtos_sem_give(&p_priv_data->epg_data_base_sem);
}

/*!
  epg database set lang code grop
  */
void epg_db_set_lang_code_group(
  u8 first_lang[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN],
  u8 second_lang[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN],
  u8 default_lang[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN])
{
  epg_db_priv_t *p_priv_data = _epg_db_get_priv();
  mtos_sem_take(&p_priv_data->epg_data_base_sem, 0);
  memset(p_priv_data->first_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8)
                                           * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->first_lang_code,first_lang, 
                                           LANGUAGE_CODE_LEN * sizeof(u8)
                                           * LANG_CODE_MAX_GROUP);
  memset(p_priv_data->second_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->second_lang_code,second_lang, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  memset(p_priv_data->default_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->default_lang_code,default_lang, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
  memset(p_priv_data->set_lang_code,0, 
                                           LANGUAGE_CODE_LEN * sizeof(u8)
                                           * LANG_CODE_MAX_GROUP);
  memcpy(p_priv_data->set_lang_code,first_lang, 
                                          LANGUAGE_CODE_LEN * sizeof(u8)
                                          * LANG_CODE_MAX_GROUP);
mtos_sem_give(&p_priv_data->epg_data_base_sem);
}

/*!
  epg database check group valid
  */
static BOOL epg_db_check_group_valid(
                            u8 group[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN])
{
  u8 i = 0;
  u8 null_code[LANGUAGE_CODE_LEN] = {0};
  u8 null_idex = 0;
  memset(null_code,0,LANGUAGE_CODE_LEN * sizeof(u8));
  for(i = 0;i < LANG_CODE_MAX_GROUP;i ++)
  {
    if(memcmp(null_code,group[i],
                        LANGUAGE_CODE_LEN) != 0)
    {
      break;
    }
    else
    {
      null_idex = i;
    }
  }
   /***previous is invalid***/
  if((null_idex < i) && (i < LANG_CODE_MAX_GROUP))
  {
    return FALSE;
  }
  /***all is invalid***/
  if(i >= LANG_CODE_MAX_GROUP)
  {
    return FALSE;
  }

  return TRUE;
}

/*!
  epg database check lang code group
  */
static BOOL epg_db_check_epg_lang_code_group(
                            u8 epg_lang_code[LANGUAGE_CODE_LEN],
                            u8 group[LANG_CODE_MAX_GROUP][LANGUAGE_CODE_LEN])
{
  u8 i = 0;
  u8 null_code[LANGUAGE_CODE_LEN] = {0};
  if(TRUE != epg_db_check_group_valid(group))
  {
    return FALSE;
  }
  memset(null_code,0,LANGUAGE_CODE_LEN * sizeof(u8));
  for(i = 0;i < LANG_CODE_MAX_GROUP;i ++)
  {

      if(memcmp(null_code,group[i],
                        LANGUAGE_CODE_LEN) == 0)
      {
        return FALSE;
      }

      if(memcmp(epg_lang_code,group[i],
                        LANGUAGE_CODE_LEN) == 0)
      {
        return TRUE;
      }
  }
  return FALSE;
}

static BOOL epg_epg_db_lang_code_cmp(u8 lang_code[LANGUAGE_CODE_LEN])
{
    epg_db_priv_t *p_priv_data = _epg_db_get_priv();

    if(TRUE == epg_db_check_epg_lang_code_group(lang_code,
                          p_priv_data->first_lang_code))
    {
      p_priv_data->lang_code_second_times = 0;
      p_priv_data->lang_code_default_times = 0;
      p_priv_data->lang_code_other_times = 0;
      if(TRUE != epg_db_check_epg_lang_code_group(lang_code,
                      p_priv_data->set_lang_code))
      {
        p_priv_data->lang_code_first_times ++;
        if(p_priv_data->lang_code_first_times >= p_priv_data->lang_code_check_times)
          {
            memcpy(p_priv_data->set_lang_code,p_priv_data->first_lang_code, 
                                               LANGUAGE_CODE_LEN * sizeof(u8)
                                               * LANG_CODE_MAX_GROUP);
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
    else if(TRUE == epg_db_check_epg_lang_code_group(lang_code,
                          p_priv_data->second_lang_code))
    {
      p_priv_data->lang_code_default_times = 0;
      p_priv_data->lang_code_other_times = 0;
      if(TRUE != epg_db_check_epg_lang_code_group(lang_code,
              p_priv_data->set_lang_code))
       {
          p_priv_data->lang_code_second_times ++;
          if(p_priv_data->lang_code_second_times >= p_priv_data->lang_code_check_times)
            {
              memcpy(p_priv_data->set_lang_code,p_priv_data->second_lang_code, 
                                                 LANGUAGE_CODE_LEN * sizeof(u8)
                                                 * LANG_CODE_MAX_GROUP);
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
    else if(TRUE == epg_db_check_epg_lang_code_group(lang_code,
                          p_priv_data->default_lang_code))
    {
       p_priv_data->lang_code_other_times = 0;
       if(TRUE != epg_db_check_epg_lang_code_group(lang_code,
              p_priv_data->set_lang_code))
        {
           p_priv_data->lang_code_default_times ++;
          if(p_priv_data->lang_code_default_times >= p_priv_data->lang_code_check_times)
            {
              memcpy(p_priv_data->set_lang_code,p_priv_data->default_lang_code, 
                                                 LANGUAGE_CODE_LEN * sizeof(u8)
                                                 * LANG_CODE_MAX_GROUP);
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
        if(TRUE != epg_db_check_epg_lang_code_group(lang_code,
            p_priv_data->set_lang_code))
        {
           p_priv_data->lang_code_other_times ++;
           if(p_priv_data->lang_code_other_times >= p_priv_data->lang_code_check_times)
            {
                /********next input switch set lang code************/
                memset(p_priv_data->set_lang_code,0, 
                                             LANGUAGE_CODE_LEN * sizeof(u8)
                                             * LANG_CODE_MAX_GROUP);
                memcpy(p_priv_data->set_lang_code[0],lang_code, 
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


/*!
  epg databasetry lang code priority
  */
static BOOL epg_db_try_lang_code_priority(epg_eit_t *p_eit_info)
{
  epg_db_priv_t *p_priv_data = _epg_db_get_priv();
  u8 i = 0;
  u8 lang_code[LANGUAGE_CODE_LEN] = {0};
  BOOL ret = FALSE;

  /****default or not set lang code,it will don't set lang code******/
  if((TRUE != epg_db_check_group_valid( 
                              p_priv_data->first_lang_code))  
      && (TRUE != epg_db_check_group_valid( 
                              p_priv_data->second_lang_code))
      && (TRUE != epg_db_check_group_valid( 
                              p_priv_data->default_lang_code)))
    {
       memset(p_priv_data->set_lang_code,0, 
                                         LANGUAGE_CODE_LEN * sizeof(u8)
                                         * LANG_CODE_MAX_GROUP);
        return FALSE;
    }

  if(p_priv_data->lang_code_forbit_swich == TRUE)
  {
      return FALSE;
  }

  if((p_priv_data->prog_info.network_id != p_eit_info->org_nw_id)
      && (p_priv_data->lang_code_sync == LANG_CODE_SYNC_NET))
    {
       return FALSE;
    }
  if((p_priv_data->prog_info.ts_id != p_eit_info->ts_id)
      && (p_priv_data->lang_code_sync == LANG_CODE_SYNC_TS))
    {
       return FALSE;
    }
   if((p_priv_data->prog_info.svc_id != p_eit_info->svc_id)
      && (p_priv_data->lang_code_sync == LANG_CODE_SYNC_SVC))
    {
       return FALSE;
    }

  //Check short txt information
  for (i = 0; i  < p_eit_info->count_text_evt; i  ++)
  {
    if (i  >= MAX_SHT_EVT_PER_SEC)
    {
      break;
    }
    if((p_eit_info->text_evt_list[i].name_len > 0)
      ||(p_eit_info->text_evt_list[i].text_len > 0))
    {
      memcpy(lang_code,p_eit_info->text_evt_list[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
       ret = epg_epg_db_lang_code_cmp(lang_code);
       if(TRUE == ret)
        {
          return ret;
        }
     }
   }

  for (i = 0; i  < p_eit_info->count_ext_evt; i  ++)
  {
    if (i  >= MAX_EXT_EVT_PER_EVT)
    {
      break;
    }
    if(p_eit_info->ext_evt_list[i].ext_txt_len)
    {
      memcpy(lang_code,p_eit_info->ext_evt_list[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
      ret = epg_epg_db_lang_code_cmp(lang_code);
       if(TRUE == ret)
        {
          return ret;
        }
     }
   }

  return FALSE;
}

/*!
  epg database filter lang code
  */
static void epg_db_filter_evt_lang_code(epg_eit_t *p_eit_info)
{
  epg_db_priv_t *p_priv_data = _epg_db_get_priv();
  text_event_t *p_sht_txt_list = NULL;  
  extern_event_t *p_ext_txt_list = NULL;
  u8 tot_sht_txt_num = 0;
  u8 tot_ext_txt_num = 0;
  u8 i = 0;
  u8 lang_code[LANGUAGE_CODE_LEN] = {0};

  if(TRUE != epg_db_check_group_valid( p_priv_data->set_lang_code))
    {
      return;
    }

  
  p_sht_txt_list = (text_event_t *)mtos_malloc(sizeof(text_event_t) * MAX_TEXT_EVT_PER_SEC);
  if(p_sht_txt_list == NULL)
  {
      return;
  }
  p_ext_txt_list = (extern_event_t *)mtos_malloc(sizeof(extern_event_t) * MAX_EXT_EVT_PER_EVT);
  if(p_ext_txt_list == NULL)
  {
      mtos_free(p_sht_txt_list);
      return;
  }

  memset(p_sht_txt_list ,0,sizeof(text_event_t) * MAX_TEXT_EVT_PER_SEC);
  memset(p_ext_txt_list,0,sizeof(extern_event_t) * MAX_EXT_EVT_PER_EVT);
    //Check short txt information
  for (i = 0; i  < p_eit_info->count_text_evt; i  ++)
  {
    if (i  >= MAX_TEXT_EVT_PER_SEC)
    {
      break;
    }
    memset(lang_code,0,LANGUAGE_CODE_LEN * sizeof(u8));
    memcpy(lang_code,p_eit_info->text_evt_list[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
    if(TRUE == epg_db_check_epg_lang_code_group(lang_code,
                        p_priv_data->set_lang_code))
      {
          memcpy(&p_sht_txt_list[tot_sht_txt_num],
              &p_eit_info->text_evt_list[i],
              sizeof(text_event_t));
          tot_sht_txt_num ++;
      }
   }
  memset(&p_eit_info->text_evt_list[0],
                0,
                (sizeof(text_event_t) * MAX_TEXT_EVT_PER_SEC));
  memcpy(&p_eit_info->text_evt_list[0],
                &p_sht_txt_list[0],
                (sizeof(text_event_t) * tot_sht_txt_num));
  p_eit_info->count_text_evt = tot_sht_txt_num;
  
/********extern info***************/
 for (i = 0; i  < p_eit_info->count_ext_evt; i  ++)
  {
    if (i  >= MAX_EXT_EVT_PER_EVT)
    {
      break;
    }
    memset(lang_code,0,LANGUAGE_CODE_LEN * sizeof(u8));
    memcpy(lang_code,p_eit_info->ext_evt_list[i].lang_code, 
                                       LANGUAGE_CODE_LEN * sizeof(u8));
    if(TRUE == epg_db_check_epg_lang_code_group(lang_code,
                        p_priv_data->set_lang_code))
      {
        memcpy(&p_ext_txt_list[tot_ext_txt_num],
                    &p_eit_info->ext_evt_list[i],
                    sizeof(extern_event_t));
        tot_ext_txt_num ++;
      }
 }

 memset(&p_eit_info->ext_evt_list[0],
              0,
              (sizeof(extern_event_t) * MAX_EXT_EVT_PER_EVT));
 memcpy(&p_eit_info->ext_evt_list[0],
              &p_ext_txt_list[0],
              (sizeof(extern_event_t) * tot_ext_txt_num));
 p_eit_info->count_ext_evt = tot_ext_txt_num;
 
  mtos_free(p_sht_txt_list);
  mtos_free(p_ext_txt_list);
  return;
}

/*!
  epg database process eit edit id call back
  */
BOOL epg_db_process_eit_edit_id_call_back(epg_eit_t *p_eit)
{
  epg_db_priv_t *p_priv_data = _epg_db_get_priv();
  if(NULL != p_priv_data->eit_edit_id_call_back)
  {
    return p_priv_data->eit_edit_id_call_back(p_eit);
  }
  return FALSE;
}

/*!
  epg database process eit edit id call back
  */
BOOL epg_db_process_eit_edit_info_call_back(epg_eit_t *p_eit)
{
  epg_db_priv_t *p_priv_data = _epg_db_get_priv();
  if(NULL != p_priv_data->eit_edit_info_call_back)
  {
    return p_priv_data->eit_edit_info_call_back(p_eit);
  }
  return FALSE;
}

