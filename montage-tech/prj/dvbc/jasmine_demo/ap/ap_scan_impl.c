/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "stdio.h"

#include "sys_types.h"
#include "sys_define.h"

#include "lib_char.h"
#include "lib_unicode.h"
#include "lib_util.h"

#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "nim.h"
#include "uio.h"

#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "cat.h"
#include "nit.h"
#include "pmt.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "data_base.h"
#include "db_dvbs.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "mem_manager.h"
#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_dvbs_scan.h"
#include "ss_ctrl.h"
//#include "ap_ota.h"
#include "sys_status.h"
#include "hal_misc.h"
#include "customer_config.h"
/*!
    Max pg number supported in one tp
    */
#define MAX_PG_NUM_PER_SAT (512)

/*!
  Frequency deviation tolerance
  */
//#define TOLERANCE(x)  (((x) > 20000) ? (5) : ((x/5000)+1))
/*!
  Maximum symbol rate offset
  */
#define MAX_SYM_OFFSET(x)  (((x) << 3)/100)

/*!
  default volume
  */
#define DEFAULT_VOLUME (16)

#define ALIGN4(x)    (((x) + 3) & (~3))

/*!
  Tp scan private data
  */
typedef struct
{
  /*!
    ALL pg view id
    */
  u8 pg_view_id;
  /*!
    All tp view id
    */
  u8 tp_view_id;
  /*!
    Current satelite id
    */
  u32 cur_sat_id;
  /*!
    Current satelite id
    */
  u32 cur_tp_id;
  /*!
    tp total in tp_list
    */
  u16 tp_depth;
  /*!
    pg total in tp_list
    */
  u16 pg_depth;
  /*!
    program list
    */
  dvbs_prog_node_t pg_list[MAX_PG_NUM_PER_SAT];
  /*!
    tp list
    */
  dvbs_tp_node_t tp_list[MAX_TP_NUM_PER_SAT];
#ifdef QUICK_SEARCH_SUPPORT 
    /*!
    total service list cnt in nit
    */
  u16 total_svc_cnt;
  /*!
    total service list in nit
    */
  tp_svc_list_t tp_svc_list[MAX_TP_NUM]; 
    /*
   ts_id list for sdt other receive
  */
  tp_rcv_para_t recv_tp_info[MAX_TP_NUM];
#endif
}scan_impl_data_t;

/*!
  Tp scan private data
  */
typedef struct
{
  BOOL using_attach_block;
  u32 block_id;
  void *p_block_addr;
  u32 total_size;
  u32 using_size;
}attach_block_info_t;

typedef struct
{
  u32 freq;
  u32 sym;
}nit_freq;

static nit_freq freq_nit[100] = {{0,0}};
static u8 nit_freq_flag = 0;
/*!
  Scan input parameter
  */
scan_input_param_t *g_p_input_para = NULL;
/*!
  Global variable for saving tp scan private data
  */
scan_impl_data_t *g_p_scan_priv = NULL;
/*!
  attach buff
  */
attach_block_info_t g_attach_block = {0};

static dvbs_tp_node_t cur_tp = {0};
/*!
  Create tp array
  */
static void create_tp_array(void)
{
//  u16 view_cnt = 0;
  u8  view_id  = 0;
  //u16 tp_cnt   = 0;
 // u16 tp_id    = 0;
  dvbs_tp_node_t *p_tp_list = g_p_scan_priv->tp_list;
 // db_dvbs_ret_t db_ret = DB_DVBS_OK;

  //Reset tp array
  memset(p_tp_list, 0xFF, sizeof(dvbs_tp_node_t)*MAX_TP_NUM_PER_SAT);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  g_p_scan_priv->tp_view_id = view_id;
}

/*!
  Create pg array
  */
static u32 create_pg_array(u32 tp_id)
{
  u16 view_cnt = 0;
  u8  view_id = 0;
  u16 pg_cnt = 0;
  u16 pg_id = 0;
  dvbs_prog_node_t *p_pg_list = g_p_scan_priv->pg_list;
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  
  //Memset pg array to 0xFF
  memset(p_pg_list, 0xFF, sizeof(dvbs_prog_node_t)*MAX_PG_NUM_PER_SAT);
  view_id = db_dvbs_create_view(DB_DVBS_TP_PG_IGNORE_SKIP_FLAG, tp_id, NULL);
  view_cnt = db_dvbs_get_count(view_id);
  
  for(pg_cnt = 0; pg_cnt < view_cnt; pg_cnt++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, pg_cnt);
    MT_ASSERT(pg_id != DB_UNKNOWN_ID);
    db_ret = db_dvbs_get_pg_by_id(pg_id, p_pg_list + pg_cnt);
    MT_ASSERT(db_ret == DB_DVBS_OK);
  }
  g_p_scan_priv->pg_depth = view_cnt;
  g_p_scan_priv->pg_view_id = view_id;
  //lint -e438 -e550
  return SUCCESS;
}
//lint +e438 +e550

/*!
  Check whether input tp information is a new tp
  \param[in] p_tp_info input tp information
  */
static BOOL is_new_tp(dvbs_tp_node_t *p_new_tp, u16 *p_sim_idx)
{
  u16 index = 0;
  dvbs_tp_node_t old_tp = {0};
  //db_dvbs_ret_t ret = DB_DVBS_OK;
  //u8 tp_view = 0;
  u16 cnt = 0;
  u16 pos = 0;

  cnt = db_dvbs_get_count(g_p_scan_priv->tp_view_id);

  for(index = 0; index < cnt; index++)
  {
    pos = db_dvbs_get_id_by_view_pos(g_p_scan_priv->tp_view_id, index);
    db_dvbs_get_tp_by_id(pos, &old_tp);
    if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
    {
      if(old_tp.freq == p_new_tp->freq)
      {
        *p_sim_idx = pos;
        return FALSE;
      }
    }
    else
    {
      if((old_tp.freq == p_new_tp->freq) && 
          (old_tp.nim_modulate == p_new_tp->nim_modulate) &&
          (old_tp.sym == p_new_tp->sym))
      {
        *p_sim_idx = pos;
        return FALSE;
      }
    }
  }
  return TRUE;
}

/*!
  Check whether input pg is new program
  \param[in] program information
  */
static BOOL is_new_pg(scan_pg_info_t *p_pg_info, u16 *p_sim_idx)
{
  u16 index = 0;

  for(index = 0; index < g_p_scan_priv->pg_depth; index++)
  {
    dvbs_prog_node_t *p_pg = g_p_scan_priv->pg_list + index;
    if((p_pg->s_id == p_pg_info->s_id) &&
//      (p_pg->ts_id == p_pg_info->ts_id) &&
      //lint -e571
      ((u32)p_pg->tp_id == g_p_scan_priv->cur_tp_id ))
      //lint +e571
    {
      //ts_id from sdt,maybe lost because of poor signal
      *p_sim_idx = index;
      return FALSE;
    }
  }
  return TRUE;
}
u32 check_service_type(scan_pg_info_t *p_pg_info)
{
  //it's error flag, maybe the stream not has the info
  if((p_pg_info->service_type == 0) || (p_pg_info->service_type == SVC_TYPE_TV))
  {
  #ifdef QUICK_SEARCH_SUPPORT     
  if(g_p_input_para->scan_data.nit_scan_type == NIT_SCAN_AND_STD_OTR)
    {
            if(p_pg_info->video_pid != 0)
        return SVC_TYPE_TV;
      else if(p_pg_info->audio_ch_num != 0)
        return SVC_TYPE_RADIO;
      else return SVC_TYPE_TV;
    }
  else
#endif
    {
      return (p_pg_info->video_pid == 0) ? SVC_TYPE_RADIO : SVC_TYPE_TV;
    }
  }
  else if((p_pg_info->service_type == SVC_TYPE_DATA) || (p_pg_info->service_type == SVC_TYPE_DATA))
    {
         return  (p_pg_info->audio[0].p_id != 0) ? SVC_TYPE_RADIO : SVC_TYPE_TRESERVED;
    }
  else
  {
    return p_pg_info->service_type;
  }
}

/*!
  Add program information into database
  \param[in] scan_pg_info_t
  */
extern u16 find_logic_number_by_sid(u16 sID);
#ifdef AISET_BOUQUET_SUPPORT
extern u8 find_show_flag_by_sid(u16 sID);
#endif
#ifdef LOGIC_NUM_SUPPORT
extern BOOL have_logic_number();
#endif
static BOOL add_pg_into_db(scan_pg_info_t *p_pg_info)
{
  //Add program into database
  event_t evt = {0};
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  dvbs_prog_node_t *p_pg_node = g_p_scan_priv->pg_list
    + g_p_scan_priv->pg_depth;  //add into the end

  //lint -e571
  memset(p_pg_node, 0, sizeof(dvbs_prog_node_t));
  p_pg_node->s_id = (u32)p_pg_info->s_id;
  p_pg_node->ts_id = p_pg_info->ts_id;
  p_pg_node->orig_net_id = (u32)p_pg_info->orig_net_id;
  p_pg_node->tp_id = g_p_scan_priv->cur_tp_id;
  p_pg_node->pcr_pid = (u32)p_pg_info->pcr_pid;
  p_pg_node->video_pid = (u32)p_pg_info->video_pid;
  p_pg_node->volume = DEFAULT_VOLUME;
  p_pg_node->is_scrambled = p_pg_info->is_scrambled;
//  p_pg_node->volume_compensate = p_pg_info->volume_compensate;
  p_pg_node->ca_system_id = p_pg_info->ca_system_id;
  p_pg_node->service_type = check_service_type(p_pg_info);
  p_pg_node->pmt_pid = p_pg_info->pmt_pid;
  p_pg_node->ecm_num = (u32)p_pg_info->ecm_num;
  p_pg_node->video_type = (u32)p_pg_info->video_type;
  //p_pg_node->aud_modify_flg = FALSE;

  #ifdef DALIAN_ZHUANGGUANG
  {
    dvbs_tp_node_t tp = {0,};
    if (db_dvbs_get_tp_by_id(p_pg_node->tp_id, &tp) == DB_DVBS_OK)
    {
      if (tp.freq == 235000)
      {
        OS_PRINTF("[DIVI_add]change audio trak to right!\n");
        p_pg_node->audio_track = 2;//235MHz频点上均为右声道
      }
    }
  }
  #endif

  
#ifdef LOGIC_NUM_SUPPORT
//lint -e746
  if(have_logic_number())
  {
  //lint +e746
    //p_pg_node->logical_num = p_pg_info->logical_num;
    p_pg_info->logical_num = find_logic_number_by_sid(p_pg_info->s_id);
    if(p_pg_info->logical_num)
    {
      p_pg_node->logical_num = p_pg_info->logical_num;
    }
    else
    {
      u16 un_india_ts_logic_num = sys_status_get_default_logic_num();
      p_pg_info->logical_num = un_india_ts_logic_num;
      p_pg_node->logical_num = p_pg_info->logical_num;
      un_india_ts_logic_num++;
      sys_status_set_default_logic_num(un_india_ts_logic_num);
    }
  }
  else
  {
    p_pg_node->logical_num = 0;
  }
#endif
  //p_pg_node->bouquet_id = p_pg_info->bouquet_id;

  //make a name for no name for some pgs
  if(strlen((char *)p_pg_info->name) == 0)
  {
    u8 p_name[MAX_SVC_NAME_SUPPORTED] = {0};
    u8 name_size = 0;
    if(p_pg_info->video_pid != 0)
    {
      sprintf((char *)p_name,"%s%d","TV CH",p_pg_info->s_id);
    }
    else
    {
      sprintf((char *)p_name,"%s%d","RADIO CH",p_pg_info->s_id);
    }
    name_size = (u8)strlen((char *)p_name);
    if(name_size > MAX_SVC_NAME_SUPPORTED)
      {
        name_size = MAX_SVC_NAME_SUPPORTED;
      }
    strncpy((char *)p_pg_info->name,(char *)p_name,name_size);
  }
  //Transfer name to unicode
  dvb_to_unicode(p_pg_info->name, 
        sizeof(p_pg_info->name), p_pg_node->name, DB_DVBS_MAX_NAME_LENGTH+1);

#ifdef AISET_BOUQUET_SUPPORT
  switch(find_show_flag_by_sid(p_pg_info->s_id))
  {
    case 0x01: //the service is always showing on service list
      p_pg_node->hide_flag = 0;
      break;    
    case 0x02://the service is always hiding on service list
      p_pg_node->hide_flag = 1;
      break;    
    case 0x00://if service is not authorized by operator then the service is hided on service list
      p_pg_node->hide_flag = 0;
      p_pg_node->operator_flag = 1;
      break;    
    default:
      break;
  }
#else
  p_pg_node->hide_flag = 0;      
#endif
  p_pg_node->tv_flag = (p_pg_node->video_pid != 0) ? 1 : 0;

  #ifdef LCN_SWITCH_DS_JHC
  if (p_pg_node->tv_flag == 0)  //radio节目的呒道号默认为最大值
  { 
    p_pg_node->logical_num = 0xff;
    p_pg_node->age_limit = 0x1f;
  }
  #endif

  p_pg_node->skp_flag = 0;
  p_pg_node->audio_ch_num = (u32)p_pg_info->audio_ch_num;
  if (p_pg_node->service_type != DVB_NVOD_REF_SVC)
  {
    memcpy(p_pg_node->audio, p_pg_info->audio,
      sizeof(audio_t)*(u32)p_pg_node->audio_ch_num);
  }

  p_pg_node->mosaic_flag = 0;
  if (p_pg_info->mosaic_flag)
  {
    if (p_pg_node->service_type == DVB_MOSAIC_SVC)
    {
      p_pg_node->mosaic_flag = p_pg_info->mosaic_flag;
      memcpy(&p_pg_node->mosaic, p_pg_info->p_mosaic, sizeof(mosaic_t));
    }

    p_pg_info->mosaic_flag = 0;
  }
  
  if (p_pg_node->ecm_num)
  {
    memcpy(p_pg_node->cas_ecm, p_pg_info->ca_ecm, sizeof(cas_desc_t) * (u32)p_pg_node->ecm_num);
  }
  //lint +e571

  db_ret = db_dvbs_add_program(g_p_scan_priv->pg_view_id, p_pg_node);

  if(DB_DVBS_OK == db_ret)
  {
    evt.id = SCAN_EVT_PG_FOUND;
    evt.data1 = (u32)p_pg_node;
    g_p_scan_priv->pg_depth++;
  }
  else 
  {
    evt.id = SCAN_EVT_NO_MEMORY;
  }
  
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  return (DB_DVBS_OK == db_ret);
}

#ifdef QUICK_SEARCH_SUPPORT 
static BOOL find_pg_in_svc_list(u32 sid, u16 svc_index)
{
  u16 i;
  for(i = 0; i < g_p_scan_priv->tp_svc_list[svc_index].total_svc_num ; i++)
  {
    if(g_p_scan_priv->tp_svc_list[svc_index].svc_id[i] == sid)
    {
      return TRUE;
    }
  }
  return FALSE;
}


static BOOL add_pg_with_tp_id_into_db(scan_pg_info_t *p_pg_info)
{
  //Add program into database
  event_t evt = {0};
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  dvbs_prog_node_t *p_pg_node = g_p_scan_priv->pg_list
    + g_p_scan_priv->pg_depth;  //add into the end
  RET_CODE ret = SUCCESS;
  dvbs_tp_node_t tp = {0};
  u16 i = 0;
  u16 similar_tp_idx = 0;

  memset(p_pg_node, 0, sizeof(dvbs_prog_node_t));
  p_pg_node->s_id = p_pg_info->s_id;
  p_pg_node->ts_id = p_pg_info->ts_id;
  p_pg_node->orig_net_id = p_pg_info->orig_net_id;

  if(g_p_scan_priv->total_svc_cnt > 0)
  {
    for(i = 0; i < g_p_scan_priv->total_svc_cnt; i++)
    {
      if(find_pg_in_svc_list(p_pg_info->s_id, i) == TRUE)
      {
        tp.freq = bcd_number_to_dec(g_p_scan_priv->tp_svc_list[i].dvbc_tp_info.frequency)/10;
        tp.sym = bcd_number_to_dec(g_p_scan_priv->tp_svc_list[i].dvbc_tp_info.symbol_rate)/10;
        switch(g_p_scan_priv->tp_svc_list[i].dvbc_tp_info.modulation)
        {
          case 1:
          tp.nim_modulate = NIM_MODULA_QAM16;
          break;

        case 2:
          tp.nim_modulate = NIM_MODULA_QAM32;
          break;

        case 3:
          tp.nim_modulate = NIM_MODULA_QAM64;
          break;

        case 4:
          tp.nim_modulate = NIM_MODULA_QAM128;
          break;

        case 5:
          tp.nim_modulate = NIM_MODULA_QAM256;
          break;
        /*
        case 6:
          p_last_tp_para->nim_modulate = 0xFF;//reserved for future use
          break; */
        default:
          tp.nim_modulate = NIM_MODULA_QAM64;
        }

        //it's may be the new tp , need add it to 
        {
          if(is_new_tp(&tp, &similar_tp_idx))
          {
            ret = db_dvbs_add_tp(g_p_scan_priv->tp_view_id, &tp);
            //add to list
            p_pg_node->tp_id = tp.id;
          }
          else
          {
            p_pg_node->tp_id = similar_tp_idx;
          }
        }
      }
    }
  }
  p_pg_node->pcr_pid = p_pg_info->pcr_pid;
  p_pg_node->video_pid = p_pg_info->video_pid;
  p_pg_node->volume = DEFAULT_VOLUME;
  p_pg_node->is_scrambled = p_pg_info->is_scrambled;
//  p_pg_node->volume_compensate = p_pg_info->volume_compensate;
  p_pg_node->ca_system_id = p_pg_info->ca_system_id;
  p_pg_node->service_type = check_service_type(p_pg_info);
  p_pg_node->pmt_pid = p_pg_info->pmt_pid;
  p_pg_node->ecm_num = p_pg_info->ecm_num;
  p_pg_node->video_type = p_pg_info->video_type;
  //p_pg_node->aud_modify_flg = FALSE;
#ifdef LOGIC_NUM_SUPPORT
  //p_pg_node->logical_num = p_pg_info->logical_num;
  p_pg_info->logical_num = find_logic_number_by_sid(p_pg_info->s_id);
  if(p_pg_info->logical_num)
  {
    p_pg_node->logical_num = p_pg_info->logical_num;
  }
  else
  {
    u16 un_india_ts_logic_num = sys_status_get_default_logic_num();
    p_pg_info->logical_num = un_india_ts_logic_num;
    p_pg_node->logical_num = p_pg_info->logical_num;
    un_india_ts_logic_num++;
    sys_status_set_default_logic_num(un_india_ts_logic_num);
  }
#endif
  //p_pg_node->bouquet_id = p_pg_info->bouquet_id;

  //make a name for no name for some pgs
  if(strlen((char *)p_pg_info->name) == 0)
  {
    u8 p_name[MAX_SVC_NAME_SUPPORTED] = {0};
    u8 name_size = 0;
    if(g_p_input_para->scan_data.nit_scan_type == NIT_SCAN_AND_STD_OTR)
    {
      if(p_pg_info->video_pid != 0)
      {
        sprintf((char *)p_name,"%s%d","TV CH",p_pg_info->s_id);
      }
      else if(p_pg_info->audio_ch_num!= 0)
      {
        sprintf((char *)p_name,"%s%d","RADIO CH",p_pg_info->s_id);
      }
      else
      {
        sprintf((char *)p_name,"%s%d","CH",p_pg_info->s_id);
      }
    }
    else
    {
      if(p_pg_info->video_pid != 0)
      {
        sprintf((char *)p_name,"%s%d","TV CH",p_pg_info->s_id);
      }
      else
      {
        sprintf((char *)p_name,"%s%d","RADIO CH",p_pg_info->s_id);
      }
    }
    name_size = strlen((char *)p_name);
    if(name_size > MAX_SVC_NAME_SUPPORTED)
      {
        name_size = MAX_SVC_NAME_SUPPORTED;
      }
    strncpy((char *)p_pg_info->name,(char *)p_name,name_size);
  }

  //Transfer name to unicode
  ret = dvb_to_unicode(p_pg_info->name, 
        sizeof(p_pg_info->name), p_pg_node->name, DB_DVBS_MAX_NAME_LENGTH);

  p_pg_node->hide_flag = 0;      
  if(g_p_input_para->scan_data.nit_scan_type == NIT_SCAN_AND_STD_OTR)
  {
    if(p_pg_node->video_pid != 0)
      p_pg_node->tv_flag = 1;
    else if(p_pg_node->audio_ch_num != 0)
      p_pg_node->tv_flag = 0;
    else if(p_pg_node->service_type == SVC_TYPE_RADIO)
      p_pg_node->tv_flag = 0;
    else
      p_pg_node->tv_flag = 1;
  }
  else
  {
    p_pg_node->tv_flag = (p_pg_node->video_pid != 0) ? 1 : 0;
  }
  p_pg_node->skp_flag = 0;
  p_pg_node->audio_ch_num = p_pg_info->audio_ch_num;
  if (p_pg_node->service_type != DVB_NVOD_REF_SVC)
  {
    memcpy(p_pg_node->audio, p_pg_info->audio,
      sizeof(audio_t)*p_pg_node->audio_ch_num);
  }

  p_pg_node->mosaic_flag = 0;
  if (p_pg_info->mosaic_flag)
  {
    if (p_pg_node->service_type == DVB_MOSAIC_SVC)
    {
      p_pg_node->mosaic_flag = p_pg_info->mosaic_flag;
      memcpy(&p_pg_node->mosaic, p_pg_info->p_mosaic, sizeof(mosaic_t));
    }

    p_pg_info->mosaic_flag = 0;
  }
  
  if (p_pg_node->ecm_num)
  {
    memcpy(&p_pg_node->cas_ecm, p_pg_info->ca_ecm, sizeof(cas_desc_t) * p_pg_node->ecm_num);
  }
  
  db_ret = db_dvbs_add_program(g_p_scan_priv->pg_view_id, p_pg_node);

  if(DB_DVBS_OK == db_ret)
  {
    evt.id = SCAN_EVT_PG_FOUND;
    evt.data1 = (u32)p_pg_node;
    g_p_scan_priv->pg_depth++;
  }
  else 
  {
    evt.id = SCAN_EVT_NO_MEMORY;
  }
  
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  return (DB_DVBS_OK == db_ret);
}
#endif

/*!
  Add program information into database
  \param[in]
  */
static BOOL edit_pg_into_db(scan_pg_info_t *p_pg_info, u16 similar_idx)
{
  //Edit program into database
  event_t evt = {SCAN_EVT_PG_FOUND};
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  dvbs_prog_node_t *p_pg_node = g_p_scan_priv->pg_list + similar_idx;
  u16  pg_name_uni[DB_DVBS_MAX_NAME_LENGTH+1] = {0};
  
  //the below is read only for user
  MT_ASSERT(p_pg_node->s_id == p_pg_info->s_id);

  //lint -e571
  dvb_to_unicode(p_pg_info->name, MAX_SVC_NAME_SUPPORTED,
    pg_name_uni, DB_DVBS_MAX_NAME_LENGTH+1);
  
  if((p_pg_node->hide_flag == 1)
    ||(p_pg_node->video_pid != p_pg_info->video_pid)
    || (p_pg_node->audio_ch_num != p_pg_info->audio_ch_num)
    || (memcmp(p_pg_node->audio, p_pg_info->audio,
      sizeof(audio_t) * (u32)p_pg_info->audio_ch_num)) 
    || (p_pg_node->pcr_pid != p_pg_info->pcr_pid) 
    || (p_pg_node->skp_flag != 0)
    || (p_pg_node->is_scrambled != p_pg_info->is_scrambled) 
    || (uni_strcmp(p_pg_node->name, pg_name_uni) != 0)
//    || (p_pg_node->volume_compensate != p_pg_info->volume_compensate) 
#ifdef LOGIC_NUM_SUPPORT
    || (p_pg_node->logical_num != p_pg_info->logical_num && p_pg_info->logical_num != 0)
#endif
    //|| (p_pg_node->bouquet_id != p_pg_info->bouquet_id)
    )
  {
#ifdef AISET_BOUQUET_SUPPORT
    switch(find_show_flag_by_sid(p_pg_info->s_id))
    {
      case 0x01://the service is always showing on service list
        p_pg_node->hide_flag = 0;
        break;    
      case 0x02://the service is always hiding on service list
        p_pg_node->hide_flag = 1;
        break;    
      case 0x00://if service is not authorized by operator then the service is hided on service list
        p_pg_node->hide_flag = 0;
        p_pg_node->operator_flag = 1;
        break;    
      default:
        break;
    }
#else
    p_pg_node->hide_flag = 0;
#endif
    p_pg_node->pcr_pid = (u32)p_pg_info->pcr_pid;
    p_pg_node->video_pid = (u32)p_pg_info->video_pid;
    uni_strncpy(p_pg_node->name, pg_name_uni, DB_DVBS_MAX_NAME_LENGTH);
    p_pg_node->tv_flag = (p_pg_node->video_pid != 0) ? 1 : 0;
    p_pg_node->skp_flag = 0;
    p_pg_node->audio_ch_num = (u32)p_pg_info->audio_ch_num;
    p_pg_node->is_scrambled = p_pg_info->is_scrambled;
//    p_pg_node->volume_compensate = p_pg_info->volume_compensate;
    p_pg_node->ca_system_id = p_pg_info->ca_system_id;
    p_pg_node->service_type = check_service_type(p_pg_info);
    p_pg_node->video_type = (u32)p_pg_info->video_type;

    #ifdef DALIAN_ZHUANGGUANG
    {
      dvbs_tp_node_t tp = {0,};
      if (db_dvbs_get_tp_by_id(p_pg_node->tp_id, &tp) == DB_DVBS_OK)
      {
        if (tp.freq == 235000)
        {OS_PRINTF("[DIVI_add]change audio trak to right!\n");
          p_pg_node->audio_track = 2;//235MHz频点上均为右声道
        }
      }
    }
    #endif

#ifdef LOGIC_NUM_SUPPORT
    if(have_logic_number())
    {
      //p_pg_node->logical_num = p_pg_info->logical_num;
      p_pg_info->logical_num = find_logic_number_by_sid(p_pg_info->s_id);
      if(p_pg_info->logical_num)
      {
        p_pg_node->logical_num = p_pg_info->logical_num;
      }
      else
      {
        u16 un_india_ts_logic_num = sys_status_get_default_logic_num();
        p_pg_info->logical_num = un_india_ts_logic_num;
        p_pg_node->logical_num = p_pg_info->logical_num;
        un_india_ts_logic_num++;
        sys_status_set_default_logic_num(un_india_ts_logic_num);
      }
    }
    else
    {
      p_pg_node->logical_num = 0;
    }
#endif
    //p_pg_node->bouquet_id = p_pg_info->bouquet_id;
    memcpy(p_pg_node->audio, p_pg_info->audio,
      sizeof(audio_t)*(u32)p_pg_node->audio_ch_num);
    db_ret = db_dvbs_edit_program(p_pg_node);
    MT_ASSERT(db_ret == DB_DVBS_OK);
  }
  //lint +e571

  evt.data1 = (u32)p_pg_node;
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  
  //lint -e438 -e550
  return TRUE;
}
//lint +e438 +e550

static BOOL match_pid(scan_pg_info_t *p_pg_info)
{
  u32 audio_cnt = 0; 
      
  if(g_p_input_para->pid_parm.pcr_pid == p_pg_info->pcr_pid
    && g_p_input_para->pid_parm.video_pid == p_pg_info->video_pid)
  {
    //lint -e571
    for(audio_cnt = 0; audio_cnt < (u32)p_pg_info->audio_ch_num; audio_cnt++)
    {
      //lint +e571
      if(p_pg_info->audio[audio_cnt].p_id == g_p_input_para->pid_parm.audio_pid)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*!
  Process tp information when one tp is done
  \param[in] sat_id satellite id
  \param[in] p_tp_list
  */
static RET_CODE impl_process_pg_list(scan_tp_info_i_t *p_tp)
{
  u32 pg_cnt = 0;
  scan_pg_info_t *p_pg = NULL;
  u16 similar_pg_idx = 0;

  BOOL db_done = TRUE;
  u32 loopi = 0;
  u32 prog_max = 0;
  
  #ifdef QUICK_SEARCH_SUPPORT 
  if(g_p_input_para->scan_data.nit_scan_type == NIT_SCAN_AND_STD_OTR)
  {
    prog_max = MAX_BROG_NUM_IN_SDT_OTR;
  }
  else
  #endif
  {
    prog_max = SCAN_MAX_PG_PER_TP;
  }

  
  if(g_customer.customer_id == CUSTOMER_CHANGHONG)
  {
     u32 tp_cont = 0;
     if(nit_freq_flag)
    {
      for(tp_cont = 0; tp_cont< 100;tp_cont++ )
     {
       if(freq_nit[tp_cont].freq  != 0 && freq_nit[tp_cont].freq ==  cur_tp.freq )
      {
        break;
      }
     }
    }
    if(tp_cont == 100)
   {
     OS_PRINTF("this freq is not in nit freq \n");
     return SUCCESS; 
   }
  }
  //Check if new pg
  for(loopi = 0; loopi < prog_max; loopi++)
  {
    //lint -e571
    if (pg_cnt == (u32)p_tp->pg_num)
    {
      //lint +e571
      break;
    }
    p_pg = p_tp->p_pg_list + loopi;
    
    OS_PRINTF("\tFound PG: s_id[%d], v_pid[%d], a_pid_0[%d],"
      " a_pid_1[%d],a_pid_2[%d], trace num[%d], name[%s]\n",
      p_pg->s_id, p_pg->video_pid, p_pg->audio[0].p_id,p_pg->audio[1].p_id,
      p_pg->audio[2].p_id, p_pg->audio_ch_num, p_pg->name);

    if(0 == p_pg->video_pid && 0 == p_pg->audio_ch_num
      && p_pg->service_type != DVB_NVOD_REF_SVC)
    {
    #ifdef QUICK_SEARCH_SUPPORT 
      if( g_p_input_para->scan_data.nit_scan_type == NIT_SCAN_AND_STD_OTR)
      {
        if(p_pg->ts_id == 0)
           continue; //it's dummy pg
      }
      else
    #endif
      {
        continue; //it's dummy pg
      }

    }
    pg_cnt ++;
    //Check scramble status
    if(g_p_input_para->is_free_only && p_pg->is_scrambled)
    {
      continue;
    }
    if(CHAN_TV == g_p_input_para->chan_type
      && 0 == p_pg->video_pid
      && p_pg->service_type != DVB_NVOD_REF_SVC)
    {
      continue;
    }
  #ifdef QUICK_SEARCH_SUPPORT 
    if (p_pg->service_type == DVB_NVOD_TIME_SHIFTED_SVC || p_pg->service_type == 0x0C)
  #else
    if (p_pg->service_type == DVB_NVOD_TIME_SHIFTED_SVC)
  #endif
    {
      continue;
    }
    else if(CHAN_RADIO == g_p_input_para->chan_type
      && p_pg->video_pid != 0)
    {
      continue;
    }
    
    //Check pid parameter
    if(g_p_input_para->pid_scan_enable && !match_pid(p_pg))
    {
      continue;
    }

    if(is_new_pg(p_pg, &similar_pg_idx))
    {
    #ifdef QUICK_SEARCH_SUPPORT 
      if(g_p_input_para->scan_data.nit_scan_type == NIT_SCAN_AND_STD_OTR
        && 0 == p_pg->video_pid
        && 0 == p_pg->audio_ch_num
        && p_pg->service_type != DVB_NVOD_REF_SVC
        && 0 == p_pg->pmt_pid)
      {
        if(g_p_scan_priv->total_svc_cnt == 0)
        {
          continue;
        }
        else
        {
          db_done = add_pg_with_tp_id_into_db(p_pg);
        }
      }
      else
    #endif
      {
      db_done = add_pg_into_db(p_pg);
      }
    }

    else
    {
      db_done = edit_pg_into_db(p_pg, similar_pg_idx);
    }

    if(!db_done)
    {
      break;
    }
  }

  //reset pg depth
  g_p_scan_priv->pg_depth = 0;
  db_dvbs_save(g_p_scan_priv->pg_view_id);
  db_dvbs_save(g_p_scan_priv->tp_view_id);

  return  db_done ? SUCCESS : ERR_NO_MEM;
}

static RET_CODE impl_process_tp(scan_tp_info_i_t *p_tp)
{
  u16 similar_tp_idx = 0;
  event_t evt = {0};
  tp_rcv_para_t *p_tp_para = &p_tp->tp_info;
  u32 iRet = 0;
  cur_tp.freq = p_tp_para->freq;
  cur_tp.sym  = p_tp_para->sym;
 
  //lint -e571
  cur_tp.nim_modulate = (u32)p_tp_para->nim_modulate;
  cur_tp.nim_type = p_tp_para->nim_type;
  
  
  //it's may be the new tp , need add it to 
  {
    if(is_new_tp(&cur_tp, &similar_tp_idx))
    {
      db_dvbs_add_tp(g_p_scan_priv->tp_view_id, &cur_tp);
      //add to list
      
      p_tp->id = (u32)cur_tp.id;
    }
    else
    {
  //    dvbs_tp_node_t *p_exist_tp = g_p_scan_priv->tp_list + similar_tp_idx;
      p_tp->id = similar_tp_idx;
      cur_tp.id = similar_tp_idx;
   //   p_exist_tp->freq = cur_tp.freq;
    //  p_exist_tp->sym = cur_tp.sym;

      db_dvbs_edit_tp(&cur_tp);
      db_dvbs_save_tp_edit(&cur_tp);
    }
  }
  memcpy(g_p_scan_priv->tp_list + g_p_scan_priv->tp_depth,
        &cur_tp, sizeof(dvbs_tp_node_t));
    g_p_scan_priv->tp_depth++;
      
  cur_tp.id = (u32)p_tp->id;
  g_p_scan_priv->cur_tp_id = (u32)p_tp->id;
  //if(DB_DVBS_OK == ret)
  {
    if(p_tp->can_locked)
    {
      iRet = create_pg_array((u32)p_tp->id);
    }
    if(iRet == SUCCESS)
    {
      evt.id = SCAN_EVT_TP_FOUND;
      evt.data1 = (u32)&cur_tp;
      evt.data2 = p_tp->can_locked;
    }
  }
  //else
  {
    //evt.id = SCAN_EVT_NO_MEMORY;
  }
  //lint +e571
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  return SUCCESS;
}

static dvbs_tp_node_t * impl_find_cur_tp(u32 cur_tp_id)
{
  u32 i = 0;
  dvbs_tp_node_t * p_cur = NULL;

  for(i = 0; i < g_p_scan_priv->tp_depth; i ++)
  {
    p_cur= g_p_scan_priv->tp_list + i;
    //lint -e571
    if((u32)p_cur->id == cur_tp_id)
    {
      //lint +e571
      return p_cur;
    }
  }
  return NULL;
}

static RET_CODE impl_process_cat(void *p_cat)
{
  dvbs_tp_node_t *p_cur_tp = NULL;
  cat_cas_t *p_cat_info = (cat_cas_t *)p_cat;

  p_cur_tp = impl_find_cur_tp(g_p_scan_priv->cur_tp_id);

  if (NULL == p_cur_tp)
  {
    OS_PRINTF("tp error\n");
    return ERR_NOFEATURE;
  }
  OS_PRINTF("[ap_scan_impl] p_cat_info->emm_cnt",p_cat_info->emm_cnt);
  p_cur_tp->emm_num = p_cat_info->emm_cnt;
  //lint -e571
  memcpy(p_cur_tp->emm_info, p_cat_info->ca_desc, (u32)p_cur_tp->emm_num * sizeof(ca_desc_t));
  //lint +e571

  db_dvbs_edit_tp(p_cur_tp);
  db_dvbs_save_tp_edit(p_cur_tp);
  return SUCCESS;
}

static void impl_process_nit(void *nit)
{
  u8 nit_tp_cnt  = 0;
  u8 k = 0;
  u32 sym;
  u32 ts_freq = 0;
  BOOL same_freq = FALSE;
  nit_t *p_nit = (nit_t *)nit;
  if(p_nit->total_tp_num== 0)
  {
    return ;
  }
  MT_ASSERT(p_nit->total_tp_num <= MAX_TP_NUM);
  nit_freq_flag = 1;
  for(nit_tp_cnt  = 0;nit_tp_cnt < p_nit->total_tp_num; ++ nit_tp_cnt)
  {
    cable_tp_info_t *p_nit_tp = NULL;
    if(p_nit->tp_svc_list[nit_tp_cnt].tp_type == NIT_DVBC_TP)
    {
       p_nit_tp = &p_nit->tp_svc_list[nit_tp_cnt].dvbc_tp_info;
	if((bcd_number_to_dec(p_nit_tp->frequency)/10) <= 256 || 
	    (bcd_number_to_dec(p_nit_tp->symbol_rate)/10) <= 20)
       {
           //Invalid tp information
	    continue;
	}
      ts_freq =  bcd_number_to_dec(p_nit_tp->frequency)/10;
      sym = bcd_number_to_dec(p_nit_tp->symbol_rate)/10;
      for(k = 0; k < 100; ++k)
     {
	 if(freq_nit[k].freq == 0)
	 {
	   break;
	 }
	 else if(freq_nit[k].freq == ts_freq)
	 {
	   same_freq = TRUE;
	   break;
	 }
     }
     if(k > 100)
     {
	OS_PRINTF("FREQ >100");
	return  ;
     }
     if(same_freq)
     {
	same_freq = FALSE;
	OS_PRINTF("the same freq = %d \n", ts_freq);
	continue;
     }
     freq_nit[k].freq= ts_freq;
     freq_nit[k].sym= sym;
   }
	OS_PRINTF(" k = %d, ADD nit freq  = %d \n", k, ts_freq); 
 }
	return;
}
#ifdef QUICK_SEARCH_SUPPORT 
static RET_CODE impl_process_tp_svc_list(void *p_data, u16 svc_cnt)
{
  g_p_scan_priv->total_svc_cnt = svc_cnt;
  memcpy(g_p_scan_priv->tp_svc_list, p_data, sizeof(tp_svc_list_t) * svc_cnt);
  return SUCCESS;
}
#endif
static RET_CODE impl_on_start(void)
{
  u16 i=0;
  //create TP list of current satellite
  create_tp_array();
  if(g_customer.customer_id == CUSTOMER_CHANGHONG)
  {
     nit_freq_flag =0;
     for(i=0;i<100;i++)
    {
      freq_nit[i].freq =0;
      freq_nit[i].sym=0;
     }
  }
  return SUCCESS;
}

static u32 impl_get_switch_disq_time(scan_sat_info_t *p_sat_info)
{
  if (!p_sat_info->diseqc_info.is_fixed)
  {
    return (20 * 1000);  //20 s
  }
  else
  {
    return (1 * 1000);
  }
}

static RET_CODE impl_check_tp(nc_channel_info_t *p_channel)
{
  u8 i = 0;
  dvbs_tp_node_t * p_cur = NULL;

#ifdef DTMB_PROJECT
  if(p_channel->channel_info.frequency < SEARCH_FREQ_MIN || p_channel->channel_info.frequency > SEARCH_FREQ_MAX)
    return ERR_FAILURE;
#endif
  for(i = 0; i < g_p_scan_priv->tp_depth; i ++)
  {
    p_cur= g_p_scan_priv->tp_list + i;
    //OS_PRINTF("#@#@ freq %d %d\n",p_cur->freq , p_channel->channel_info.frequency);
    if(p_cur->freq == p_channel->channel_info.frequency)
    {
      return ERR_FAILURE;
    }
  }
  return SUCCESS;
}

static RET_CODE impl_free_resource(void)
{
  //for some unlock TP with parse NIT
  db_dvbs_save(g_p_scan_priv->tp_view_id);

  if(g_attach_block.using_attach_block)
  {
    g_p_input_para = NULL;
    g_p_scan_priv = NULL;
  }
  else
  {
    // workround since mtos_free/alloc's bug
    //mtos_free(g_p_input_para);
    //g_p_input_para = NULL;
    mtos_free(g_p_scan_priv);
    g_p_scan_priv = NULL;
  }
  return SUCCESS;
}

static void * impl_get_attach_buffer(u32 size)
{
  void *p_addr = NULL;
  
  if(g_attach_block.using_attach_block)
  {
    size = ALIGN4(size);
    OS_PRINTF("scan alloc mem %d\n", size);
    p_addr = (u8*)g_attach_block.p_block_addr;
    g_attach_block.p_block_addr = (u8*)g_attach_block.p_block_addr + size;
    g_attach_block.using_size += size;
    MT_ASSERT(g_attach_block.using_size <= g_attach_block.total_size);
  }
  return p_addr;
}

#ifdef QUICK_SEARCH_SUPPORT 
static void impl_save_tp_info_from_sdt_other(tp_svc_list_t *p_cur_tp_svc)
{
  u8 i = 0;
  tp_rcv_para_t *p_recv_tp_info =  g_p_scan_priv->recv_tp_info;

  if(p_cur_tp_svc == NULL)
  {
    return;
  }
  
  for(i = 0; i < MAX_TP_NUM; i++)
  {
    if(p_recv_tp_info[i].freq == 0)
    {      
    //sat id is the same to current tp
    p_recv_tp_info[i].freq = bcd_number_to_dec(p_cur_tp_svc->dvbc_tp_info.frequency)/10;
    p_recv_tp_info[i].sym = bcd_number_to_dec(p_cur_tp_svc->dvbc_tp_info.symbol_rate)/10;

    //convert the QAM table 
    switch(p_cur_tp_svc->dvbc_tp_info.modulation)
    {
      case 1:
        p_recv_tp_info[i].nim_modulate = NIM_MODULA_QAM16;
      break;

      case 2:
        p_recv_tp_info[i].nim_modulate = NIM_MODULA_QAM32;
      break;

      case 3:
        p_recv_tp_info[i].nim_modulate = NIM_MODULA_QAM64;      
      break;

      case 4:
        p_recv_tp_info[i].nim_modulate = NIM_MODULA_QAM128;
      break;

      case 5:
        p_recv_tp_info[i].nim_modulate = NIM_MODULA_QAM256;
      break;

      default:
        p_recv_tp_info[i].nim_modulate = NIM_MODULA_QAM64;

    }
    OS_PRINTF("@@@ save tp info to array[%d] = %d! \n",i, p_recv_tp_info[i].freq);
    break;
    }
  }
}

static BOOL impl_check_tp_received(nc_channel_info_t *p_channel)
{  
  u8 i = 0;

  for(i = 0; i < MAX_TP_NUM; i++)
  {
    OS_PRINTF("!@@@ dvbc_check_pg_on_tp_received tp =%d i = %d, lck tp = %d \n", 
                        p_channel->channel_info.frequency, i, g_p_scan_priv->recv_tp_info[i].freq);
    if(g_p_scan_priv->recv_tp_info[i].freq == p_channel->channel_info.frequency 
      && g_p_scan_priv->recv_tp_info[i].sym == p_channel->channel_info.param.dvbc.symbol_rate
      && g_p_scan_priv->recv_tp_info[i].nim_modulate == p_channel->channel_info.param.dvbc.modulation)
    {
      return TRUE;
    }
  }

  return FALSE;
}

#endif
/*!
  Construct tp scan ploicy
  \param[in] block id
  */
void ap_scan_set_attach_block(BOOL use_attach_block, u32 block_id)
{
  g_attach_block.using_attach_block = use_attach_block;
  if(use_attach_block)
  {
    g_attach_block.block_id = block_id;
    g_attach_block.p_block_addr = (void *)
      mem_mgr_require_block(block_id, SYS_MODULE_SCAN);
    MT_ASSERT(g_attach_block.p_block_addr != NULL);
    MT_ASSERT(((u32)g_attach_block.p_block_addr % 4) == 0);
    g_attach_block.total_size = mem_mgr_get_block_size(block_id);
    mem_mgr_release_block(block_id);
    
    g_attach_block.using_size = 0;
  }
}

scan_input_param_t *ap_scan_param_buffer(void)
{
  u32 size = sizeof(scan_input_param_t);
  
  if(NULL == g_p_input_para)
  {
    if(g_attach_block.using_attach_block)
    {
      g_p_input_para = impl_get_attach_buffer(size);
    }
    else
    {
      g_p_input_para = mtos_malloc(size);
    }
    MT_ASSERT(g_p_input_para != NULL);
    //memset(g_p_input_para, 0, size);
  }
  return g_p_input_para;
}
policy_handle_t ap_scan_param_apply(void)
{
  u32 size = sizeof(scan_impl_data_t);
  scan_hook_t hook = {0};

  memset(&hook, 0, sizeof(scan_hook_t));
  hook.get_switch_disq_time = impl_get_switch_disq_time;
  hook.check_tp = impl_check_tp;
  hook.process_tp = impl_process_tp;
  hook.free_resource = impl_free_resource;
  hook.get_attach_buffer = impl_get_attach_buffer;
  hook.on_start = impl_on_start;
  hook.process_pg_list = impl_process_pg_list;
#ifdef QUICK_SEARCH_SUPPORT 
  hook.process_tp_svc_list = impl_process_tp_svc_list;
  hook.check_tp_received = impl_check_tp_received;
  hook.save_tp_info = impl_save_tp_info_from_sdt_other;
#endif
  hook.process_cat = impl_process_cat;
  if(g_customer.customer_id == CUSTOMER_CHANGHONG)
  {
     hook.process_nit_info =impl_process_nit;
  }
  else
  {
     hook.process_nit_info =NULL;
  }
  
  MT_ASSERT(NULL == g_p_scan_priv);
  if(g_attach_block.using_attach_block)
  {
    g_p_scan_priv = impl_get_attach_buffer(size);
  }
  else
  {
    g_p_scan_priv = mtos_malloc(size);
  }
  MT_ASSERT(g_p_scan_priv != NULL);
  memset(g_p_scan_priv, 0, size);

  if(BLIND_SCAN == g_p_input_para->scan_mode)
  {
    MT_ASSERT(0);
    return NULL;
  }
  else if(TP_SCAN == g_p_input_para->scan_mode)
  {
    MT_ASSERT(0);
    return NULL;
  }
  else if(DVBC_SCAN == g_p_input_para->scan_mode)
  {
    #ifdef TC_TUNER_RELOCK
      return construct_dvbc_scan_policy_warriors(&hook, &g_p_input_para->scan_data);
    #else
      return construct_dvbc_scan_policy(&hook, &g_p_input_para->scan_data);
    #endif
  }
  else if(DTMB_SCAN == g_p_input_para->scan_mode)
  {
    return construct_dtmb_scan_policy(&hook, &g_p_input_para->scan_data);
  }
  else
  {
    MT_ASSERT(0);
    return NULL;
  }
}



