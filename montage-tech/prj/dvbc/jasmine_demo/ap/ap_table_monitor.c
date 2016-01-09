/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_msg.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "ap_framework.h"

#include "dvb_protocol.h"
#include "mdl.h"
#include "drv_dev.h"
#include "nim.h"
#include "service.h"
#include "class_factory.h"
#include "nim_ctrl_svc.h"
#include "dvb_svc.h"
#include "nim_ctrl.h"
#include "dsmcc.h"
#include "monitor_service.h"
#include "mosaic.h"
#include "sdt.h"
#include "nit.h"
#include "pmt.h"
#include "pat.h"
#include "bat.h"
#include "user_parse_table.h"
#include "ap_table_monitor.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"

#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "db_dvbs.h"
#include "lib_util.h"
//#include "ap_ota.h"
//#include "sys_data_staic.h"
#include "sys_cfg.h"
#include "data_manager.h"
//#include "config_customer.h"
#include "smc_op.h"
#include "uio.h"
#include "lpower.h"
//#include "config_prj.h"
#include "ui_common.h"
#include "ap_scan.h"
#include "lib_char.h"

#define MAX_TABLE_NUM (3)
#define SECTION_LENGTH (4 * (KBYTES))
#define CRC_SIZE  4
#define MAX_SDT_SEC_LEN     1024  

#define SDT_DESC_HEAD_LEN   0x5
#define SDT_SEC_HEAD_LEN    12
#define DEFAULT_VOLUME (16)
#define BAT_REQUEST_BUFFER (8 * 1024)
#define MAX_BOU_ID_NUM (64)
#define MAX_ADD_PG (10)
/*!
  tm state definition
  */
typedef enum
{
  /*!
    IDLE state
    */
  TM_IDLE = 0,
  /*!
    tm pre scaning
    */
  TM_SCANING_BAT,
  TM_SCAN_WAIT_TP_LOCK,
} tm_state_t;

#if 0
/*!
  BAT section and bouquet info
  */
typedef struct
{
  /*!
    store BAT bouquet id
    */
  u16 bouquet_id;
  /*!
    store first recieved BAT section num of specify bouquet
    */
  u8 sec_num_first;
  /*!
    store repeat times of first recieved specify bouquet's section 
    */
  u8 sec_num_repeat;
}bat_sec_info_t;
#else
typedef struct
{
  /*!
    store BAT bouquet id
    */
  u16 bouquet_id;
  /*!
    store first recieved BAT section num of specify bouquet
    */
  u8 sec_num_array[16];
  /*!
    store repeat times of first recieved specify bouquet's section
    */
  u8 sec_repeat_array[16];
  /*!
    store repeat times of first recieved specify bouquet's section
    */
  u8 last_sec_num;
  /*!
    store repeat times of first recieved specify bouquet's section
    */
  BOOL b_sec_finish;
}bat_sec_info_t;
#endif
#ifdef QUICK_SEARCH_SUPPORT                
/*!
  sdt otr pg info
  */
typedef struct
{
  /*!
    Service id
    */
  u16 s_id;
  /*!
    Service scrambled flag, 
    1: scramble
    0: free
    */
  u8 is_scrambled;
  /*!
    store repeat times of first recieved specify bouquet's section
    */
  u16 orig_net_id;
  /*!
    CA system id
    */
  u16 ca_system_id;
  /*!
    Stream id
    */
  u16 ts_id;
  /*!
    service type
    */
  u8 service_type;
  /*!
    mosaic descriptor(service_type = 0x06) or
    nvod reference descriptor
    */
  mosaic_t mosaic;
  /*!
    Service name information
    */
  u8 pg_name[MAX_SVC_NAME_SUPPORTED];
}pg_info_t;
#endif

/*!
  table monitor private data
  */
typedef struct
{
  /*!
    Current tm state
    */
  tm_state_t state;
  /*!
    Current scan state
    */
  BOOL bBatAvailable;
  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;
  /*!
    DVB service handle
    */
  service_t *p_nc_svc;
  /*!
   add  pg_index
    */
  u32 add_pg_index;  
  /*!
   add  pg_num
    */
  u32 add_pg_num;
  /*!
   add  pg info
    */
  dvbs_prog_node_t add_pg[MAX_ADD_PG];
  /*!
    pg info
    */
  dvbs_prog_node_t pg;
  /*!
    pg name
    */
  u8 pg_name[MAX_ADD_PG][MAX_SVC_NAME_SUPPORTED];
  /*!
    ext buf list
    */
  filter_ext_buf_t ext_buf_list;
  /*!
    bat_multi_mode start tickets
    */
  u32 start_bat_tickets;
    /*!
    bat_multi_mode
    */
  BOOL bat_multi_mode;
      /*!
    store BAT section and bouquet info
    */
  bat_sec_info_t bat_sec_info[MAX_BOU_ID_NUM];
    /*!
    BAT first received
    */
  BOOL bat_is_received;
  /*!
    sdt start tickets
    */
  u32 start_sdt_tickets;
    
#ifdef QUICK_SEARCH_SUPPORT                    
  /*!
    service list number found in nit
    */
  u16 total_svc_found_list;
  /*!
    service list found in nit
    */
  tp_svc_list_t tp_svc_list[MAX_TP_NUM]; 
  /*!
    sdt actual ts id
    */
  u16 sdt_actual_ts_id;
  /*!
    sdt other table receive flag
    */
  BOOL is_sdt_otr_recv[MAX_TP_NUM];  
  /*!
    nit table receive flag
    */
  BOOL is_nit_recv[MAX_TP_NUM];  
  /*!
    start get sdt other ticks
    */
  u32 sdt_otr_start_ticks;
  /*!
    pg edit ticks
    */
  u32 pg_edit_ticks;  
  /*!
    pg list in sdt other table
    */
  pg_info_t total_pg_list[MAX_BROG_NUM_IN_SDT_OTR];
  /*!
    pg number in sdt other table
    */
  u32 pg_num;
  /*!
    flag nit all receive
    */
  BOOL bSdtOtrAvailable;
#endif  
} table_monitor_priv_t;

/*!
  table monitor handle 
  */
typedef struct
{
  /*!
    table monitor instance
    */
  app_t instance;
  /*
  data buffer
  */
  u8 *p_buffer;
  /*!
    table monitor private
    */
  table_monitor_priv_t priv;
} tm_handle_t;

u16 ui_table_mon_evtmap(u32 event);
extern u16 find_logic_number_by_sid(u16 sID);
extern u16 get_max_logic_number();
/*!
  Service name descriptor
  */
typedef struct
{
  /*!
    service type
    */
  u8 svc_type;
  /*!
    Length of service name
    */
  u8 size;
  /*!
    Provider name length
    */
  u8 provider_nm_len;
  /*!
    Service name length
    */
  u8 svc_nm_len;
  /*!
    Provider name length
    */
  u8 *p_provider_addr;
  /*!
    Service name length
    */
  u8 *p_svc_nm_addr;
}svc_des_t;

static void send_msg_to_ui(u32 id, u32 param1, u32 param2)
{
  event_t evt = {0};
  
  evt.id = id;
  evt.data1 = param1;
  evt.data2 = param2;
  ap_frm_send_evt_to_ui(APP_RESERVED2, &evt);
}

/*!
  \param[in] p_buf data buffer input
  */
static void parse_svc_des(u8 *p_buf, svc_des_t *p_svc_desc)
{
  u16 i = 0;

  MT_ASSERT(p_svc_desc != NULL);
  memset(p_svc_desc, 0, sizeof(svc_des_t));
  
  p_svc_desc->size  = p_buf[i++];
  p_svc_desc->svc_type = p_buf[i++];
  
  p_svc_desc->provider_nm_len = p_buf[i++];
  p_svc_desc->p_provider_addr = &p_buf[i];
  
  i += p_svc_desc->provider_nm_len;
  p_svc_desc->svc_nm_len = p_buf[i++];
  p_svc_desc->p_svc_nm_addr = &p_buf[i++];
}

void table_monitor_edit_logic_number(BOOL is_all_pg)
{
  u8 view_id;
  u16 pg_count;
  u16 i;
  u16 pg_id;
  dvbs_prog_node_t prog = {0};
  u16 logic_number = 0;
  BOOL edit_flag = FALSE;
  u16 un_india_ts_logic_num = 1;

  if(is_all_pg)
    view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  else
  view_id = ui_dbase_get_pg_view_id();
  pg_count = db_dvbs_get_count(view_id);

  OS_PRINTF("table_monitor_edit_logic_number:pg_count=%d\n",pg_count);
  for(i=0; i<pg_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);

    logic_number = find_logic_number_by_sid(prog.s_id);
    if(logic_number && prog.logical_num != logic_number)
    {
      prog.logical_num = logic_number;
      db_dvbs_edit_program(&prog);
      db_dvbs_save_pg_edit(&prog);
      edit_flag = TRUE;
    }
  }
  if(edit_flag == TRUE)
  {
    ui_cache_view();
    un_india_ts_logic_num = get_max_logic_number() + 1;
    view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
    pg_count = db_dvbs_get_count(view_id);
    for(i=0; i<pg_count; i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      db_dvbs_get_pg_by_id(pg_id, &prog);
      
      logic_number = find_logic_number_by_sid(prog.s_id);
      if(logic_number == 0)
      {
        un_india_ts_logic_num = sys_status_get_default_logic_num();
        if(prog.logical_num != un_india_ts_logic_num)
        {
          prog.logical_num = un_india_ts_logic_num;
          db_dvbs_edit_program(&prog);
          db_dvbs_save_pg_edit(&prog);
        }
        un_india_ts_logic_num++;
      }
    }
    sys_status_set_default_logic_num(un_india_ts_logic_num);
    ui_dbase_pg_sort(DB_DVBS_ALL_PG);
    sys_status_save();
    edit_flag = FALSE;
    ui_restore_view();
  }
}

void table_monitor_parse_sdt(u8 *p_in_buffer, sdt_t *p_sdt)
{
  u16 cnt = 0;
  u16 length = 0;
  u16 size = 0;
  u16 desloop = 0;
  u8  l = 0;
  svc_des_t  svc_desc = {0};
  u8 error_no = 0;
  cnt ++; 
  p_sdt->sec_length = p_in_buffer[cnt++] << 8 ;
  p_sdt->sec_length += p_in_buffer[cnt++] ;
  p_sdt->sec_length &= 0xFFF;
  if(p_sdt->sec_length < SDT_SEC_HEAD_LEN)
  {
    return;    
  }  
  MT_ASSERT(p_sdt->sec_length <= MAX_SDT_SEC_LEN);
  p_sdt->stream_id = p_in_buffer[cnt ++] << 8 ;
  p_sdt->stream_id += p_in_buffer[cnt ++] ;

  p_sdt->pf_flag = MASK_FIRST_BIT_OF_BYTE(p_in_buffer[cnt ++]);
  cnt --;

  p_sdt->version_num = (p_in_buffer[cnt ++] & 0x3E) >> 1;

  p_sdt->sec_number = p_in_buffer[cnt ++] ;
  p_sdt->last_sec_number = p_in_buffer[cnt ++] ;

  p_sdt->org_network_id = p_in_buffer[cnt ++] << 8 ;
  p_sdt->org_network_id += p_in_buffer[cnt ++] ;  
  cnt ++;
  length = (p_sdt->sec_length - 8 - CRC_SIZE) ;
  while(length > 0)
  {     
    if(length < SDT_DESC_HEAD_LEN)
    {
      break;
    }  

    if(p_sdt->svc_count >= DVB_MAX_SDT_SVC_NUM)
    {
      break;
    }
    
    (p_sdt->svc_des[p_sdt->svc_count]).svc_id = p_in_buffer[cnt ++] << 8;
    (p_sdt->svc_des[p_sdt->svc_count]).svc_id += p_in_buffer[cnt ++] ;
    (p_sdt->svc_des[p_sdt->svc_count]).eit_sch_flag = 
      MASK_SECOND_BIT_OF_BYTE(p_in_buffer[cnt]) >> 1 ;
    
    (p_sdt->svc_des[p_sdt->svc_count]).eit_pf_flag = 
      MASK_FIRST_BIT_OF_BYTE(p_in_buffer[cnt ++])  ;
    (p_sdt->svc_des[p_sdt->svc_count]).run_sts = ((p_in_buffer[cnt] & 0xE0) >> 5);
      
    p_sdt->svc_des[p_sdt->svc_count].is_scrambled = (p_in_buffer[cnt] & 0x10) >> 4;
    desloop = SYS_GET_LOW_HALF_BYTE(p_in_buffer[cnt++]) << 8 ;
    desloop += p_in_buffer[cnt ++] ;
    size = desloop;
    
    if (size > (length - SDT_DESC_HEAD_LEN))
    {
      break;
    }
    while(desloop > 0)
    {
      switch (p_in_buffer[cnt ++])
      {
      case DVB_DESC_SERVICE:
        {
          parse_svc_des(&p_in_buffer[cnt], &svc_desc);
          l = (svc_desc.svc_nm_len <= MAX_SVC_NAME_LEN)?
          (svc_desc.svc_nm_len) : (MAX_SVC_NAME_LEN);
          memcpy((u8 *)&(p_sdt->svc_des[p_sdt->svc_count]).name, 
            svc_desc.p_svc_nm_addr, l);
          p_sdt->svc_des[p_sdt->svc_count].service_type = svc_desc.svc_type;
          if(p_in_buffer[cnt] > (desloop - 2))
          {
            desloop = 0;
            memset((u8 *)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
                sizeof(sdt_svc_descriptor_t));
                
            p_sdt->svc_count --;
            error_no = 1;
          }
        }
        break;
      case DVB_DESC_CHNL_VLM_CMPST:
        {
           if(p_in_buffer[cnt] != 1)
           {
             desloop = 0;
             memset((u8 *)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
                sizeof(sdt_svc_descriptor_t));
             p_sdt->svc_count --;
             error_no = 1;
             break;
           }
           (p_sdt->svc_des[p_sdt->svc_count]).chnl_vlm_cmpt = p_in_buffer[cnt + 1];
        }
        break;
      case DVB_CA_IDENTIFIER:
        {
          p_sdt->svc_des[p_sdt->svc_count].ca_system_id = 
                  MAKE_WORD2(p_in_buffer[cnt + 1], p_in_buffer[cnt + 2]);
        }
        break;
        
      default:
        {
          if(p_in_buffer[cnt] > (desloop - 2))
          {
            desloop = 0;
            memset((u8 *)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
              sizeof(sdt_svc_descriptor_t));             
            p_sdt->svc_count --;
            error_no = 1;
          } 
        }
        break;
      }
      if (error_no)
      {
        break;
      }
      /*
      if(DVB_DESC_SERVICE == p_in_buffer[cnt])
      {
        cnt++;
        parse_svc_des(&p_in_buffer[cnt], &svc_desc);
        l = (svc_desc.svc_nm_len <= MAX_SVC_NAME_LEN)?
        (svc_desc.svc_nm_len):(MAX_SVC_NAME_LEN);
        memcpy((u8*)&(p_sdt->svc_des[p_sdt->svc_count]).name, 
          svc_desc.p_svc_nm_addr, l);
          
        if(p_in_buffer[cnt]>(desloop - 2))
        {
          desloop = 0;
          memset((u8*)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
              sizeof(sdt_svc_descriptor_t));
              
          p_sdt->svc_count--;
          break;          
        }     
      }
      else if(DVB_DESC_CHNL_VLM_CMPST == p_in_buffer[cnt])
      {
         cnt++;
         if(p_in_buffer[cnt]!= 1)
         {
           desloop = 0;
           memset((u8*)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
              sizeof(sdt_svc_descriptor_t));           
           p_sdt->svc_count--;
           break;                    
         }
         (p_sdt->svc_des[p_sdt->svc_count]).chnl_vlm_cmpt = p_in_buffer[cnt+1];
      }
      else if(DVB_CA_IDENTIFIER == p_in_buffer[cnt])
      {
        //Skip tag
        cnt++;

        p_sdt->svc_des[p_sdt->svc_count].ca_system_id = 
                MAKE_WORD2(p_in_buffer[cnt + 1], p_in_buffer[cnt + 2]);
      }
      else
      {
        cnt++;
        if(p_in_buffer[cnt]>(desloop-2))
        {
          desloop = 0;
          memset((u8*)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
            sizeof(sdt_svc_descriptor_t));             
          p_sdt->svc_count--;
          break;          
        }        
      }*/
      desloop -= (p_in_buffer[cnt] + 2);
      cnt += (p_in_buffer[cnt] + 1) ;
    }
    desloop = 0;
    p_sdt->svc_count ++ ;
    length -= (SDT_DESC_HEAD_LEN + size);
  }
}

static BOOL is_need_free_bat(tm_handle_t *p_priv, bat_t *p_bat)
#if 1
{
  u8 i = 0, j = 0;
  BOOL ret = FALSE;

  if(p_priv->priv.bat_sec_info[i].sec_repeat_array[p_bat->sec_number] >= 5)
  {
    for(i = 0; i < MAX_BOU_ID_NUM; i++)
    {
      if(p_priv->priv.bat_sec_info[i].bouquet_id != 0
        && !p_priv->priv.bat_sec_info[i].b_sec_finish)
      {
        OS_PRINTF("is_need_free_bat break\n");
        break;
      }
    }

    if(i == MAX_BOU_ID_NUM)
    {
      ret = TRUE;
      OS_PRINTF("is_need_free_bat FREE\n");
    }
  }
  else
  {
    OS_PRINTF("\n=============\n");

    for(i = 0; i < MAX_BOU_ID_NUM; i++)
    {
      if(p_priv->priv.bat_sec_info[i].bouquet_id != 0)
      {
        OS_PRINTF("BID=%d\n", p_priv->priv.bat_sec_info[i].bouquet_id);
        for(j = 0; j <= p_priv->priv.bat_sec_info[i].last_sec_num; j++)
        {
          OS_PRINTF("sec_num=%d,repeat=%d\n",
            p_priv->priv.bat_sec_info[i].sec_num_array[j],
            p_priv->priv.bat_sec_info[i].sec_repeat_array[j]
            );
        }
      }
    }
    
    OS_PRINTF("\n=============\n");
  }

  return ret;
}
#else
{
  u8 i = 0;
  BOOL ret = FALSE;
  for(i = 0; i < MAX_BOU_ID_NUM; i++)
  {
    if(p_priv->priv.bat_sec_info[i].bouquet_id == p_bat->bouquet_id)
    {
      if(p_priv->priv.bat_sec_info[i].sec_num_first == p_bat->sec_number)
      {
        p_priv->priv.bat_sec_info[i].sec_num_repeat++;
      }

      if(p_priv->priv.bat_sec_info[i].sec_num_repeat == 10) //repeat times 3?
      {
        ret = TRUE;
      }

      break;
    }
  }
  
  return ret;
}
#endif

static void add_bouquet_id(tm_handle_t *p_priv, bat_t *p_bat)
#if 1
{
  u8 i = 0, j = 0;
  BOOL b = TRUE;

  for(i = 0; i < MAX_BOU_ID_NUM; i++)
  {
    if(p_priv->priv.bat_sec_info[i].bouquet_id == p_bat->bouquet_id)
    {
      p_priv->priv.bat_sec_info[i].sec_repeat_array[p_bat->sec_number]++;

	    for(j=0; j<p_bat->last_sec_number; j++)
      {
        if(p_priv->priv.bat_sec_info[i].sec_repeat_array[j] == 0)
        {
          b = FALSE;
		      break;
        }
      }

      if(b)
      {
	      p_priv->priv.bat_sec_info[i].b_sec_finish = TRUE;
      }

      break;
    }
    else
    {
      if(p_priv->priv.bat_sec_info[i].bouquet_id == 0)
      {
        #if 0
        memset(p_priv->priv.bat_sec_info[i].sec_num_array,
        			0,
        			sizeof(p_priv->priv.bat_sec_info[i].sec_num_array)/sizeof(u8));
        memset(p_priv->priv.bat_sec_info[i].sec_repeat_array,
        			0,
        			sizeof(p_priv->priv.bat_sec_info[i].sec_repeat_array)/sizeof(u8));
    		#endif

    		p_priv->priv.bat_sec_info[i].bouquet_id = p_bat->bouquet_id;
    		p_priv->priv.bat_sec_info[i].last_sec_num = p_bat->last_sec_number;

        if(p_bat->last_sec_number == 0)
        {
          p_priv->priv.bat_sec_info[i].b_sec_finish = TRUE;
        }

        for(j=0; j<p_bat->last_sec_number; j++)
        {
          p_priv->priv.bat_sec_info[i].sec_num_array[j] = j;
        }

    		p_priv->priv.bat_sec_info[i].sec_repeat_array[p_bat->sec_number]++;

        break;
      }
    }
  }
  return;
}
#else
{
  u8 i = 0;
  for(i = 0; i < MAX_BOU_ID_NUM; i++)
  {
    if(p_priv->priv.bat_sec_info[i].bouquet_id == p_bat->bouquet_id)
    {
      break;
    }
    else
    {
      if(p_priv->priv.bat_sec_info[i].bouquet_id == 0)
      {
        p_priv->priv.bat_sec_info[i].bouquet_id = p_bat->bouquet_id;
        p_priv->priv.bat_sec_info[i].sec_num_first = p_bat->sec_number;

        break;
      }
    }
  }
  return;
}
#endif

extern BOOL add_logic_info(u16 sID, u16 logic_num);
#if 0
void table_monitor_parse_bat(u8 *p_buf, bat_t *p_bat)
{
  u16 bouquet_id = 0;
  u16 offset = 0;
  s16 bouquet_len = 0;
  u8 bouquet_name[64] = {0};
  u16 bouquet_name_unistr[64] = {0};
  BOOL hasBouquetName = FALSE;
  BOOL BouquetID_repeat = FALSE;
  u16 repeat_bouq_index = 0;
  s16 section_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  u16 dl_loop_len = 0;
  u16 i = 0, j = 0;
  u8 tmp_len = 0;
  u16 cur_bouquet_service_list_id[256];
  linkage_update_desc_t update_info = {0};
  u16 cur_bouquet_service_count = 0;

  //load_categories_info();
  
  //MT_ASSERT(p_dvb_handle != NULL);

  //if(p_buf[0] != p_sec->table_id)
  //{
   // return;
  //}

  if ((p_buf[5] & 0x01) == 0)
  {
    return;
  }

  bouquet_id = MAKE_WORD2(p_buf[3], p_buf[4]);
  
  /* Section length */
  section_len = MAKE_WORD2((p_buf[1] & 0x0f), p_buf[2]);
  section_len += 3;
  
  p_bat->bouquet_id = bouquet_id;
  p_bat->version_num = (p_buf[5] & 0x3E) >> 1;
  p_bat->sec_number = p_buf[6];
  p_bat->last_sec_number = p_buf[7];

  bouquet_len = MAKE_WORD2(p_buf[8] & 0x0F, p_buf[9]);
  offset = 10;
  
  UI_PRINTF("[parsebat] ...sec %d, last sec %d, bouquet id 0x%x\n",p_bat->sec_number, p_bat->last_sec_number, bouquet_id);
  UI_PRINTF("[parsebat]==========start===========\n");
  
  UI_PRINTF("[parsebat]bat bouquet_id: %d 0x%x\n", bouquet_id,bouquet_id);
  while(bouquet_len > 0)
  {
    tmp_len = p_buf[offset + 1];
    switch(p_buf[offset])
    {
      case DVB_DESC_LINKAGE:
        {
          p_bat->ts_id = update_info.ts_id = MAKE_WORD2(p_buf[offset + 2], 
                                                                p_buf[offset + 3]);
          p_bat->network_id = update_info.network_id = MAKE_WORD2(p_buf[offset + 4], 
                                                                                p_buf[offset + 5]);
          p_bat->service_id = update_info.service_id = MAKE_WORD2(p_buf[offset + 6], 
                                                                                p_buf[offset + 7]);
          if(p_buf[offset + 8] == 0xA0)  // linkage type 0xA0
          {
            update_info.operator_num = p_buf[offset + 9];
            dl_loop_len = p_buf[offset + 10];
            for(i = 0; i < dl_loop_len;)
            {
              update_info.manufacture_id = p_buf[offset + 11 + i];
              update_info.hardware_id = MAKE_WORD2(p_buf[offset + 12 + i], p_buf[offset + 13 + i]);
              update_info.model_id = p_buf[offset + 14 + i];
              update_info.software_ver = MAKE_WORD2(p_buf[offset + 15], p_buf[offset + 16 + i]);

              update_info.force_flag = p_buf[offset + 18 + i] & 0x03;
              update_info.start_STB_id = make32(&p_buf[offset + 19 + i]);
              update_info.end_STB_id = make32(&p_buf[offset + 23 + i]);
              i += 16;
            }
          }
          else if(p_buf[offset + 8] == 0x80)
          {
            p_bat->fdt_table_id = p_buf[offset + 11];
            if(DVB_TABLE_ID_FDT == p_bat->fdt_table_id)
            {
              p_bat->fdt_pid = MAKE_WORD2(p_buf[offset + 9]&0x1F, p_buf[offset + 10 ]);
              p_bat->fdt_ver_num = p_buf[offset + 12]&0x1F;
            }
          }
        }
        break;
      
      case DVB_DESC_BOUQUET_NAME://bouquet_name_descriptor
        {
          UI_PRINTF("[parsebat]bat descriptor name: %s \n", &p_buf[offset + 2]);
          memcpy(bouquet_name, &p_buf[offset + 2], p_buf[offset + 1]);
          hasBouquetName = TRUE;
        }
        break;

      default:
        break;
    }
    
    offset += (tmp_len + 2);  
    bouquet_len -= (tmp_len + 2);  
  }

  // TODO ts loop len while
  ts_loop_len = MAKE_WORD2(p_buf[offset] & 0xF, p_buf[offset + 1]);
  offset += 2;
  while(ts_loop_len > 0)
  {
    offset += 4;
    desc_len = MAKE_WORD2(p_buf[offset] & 0xF, p_buf[offset + 1]);
    offset += 2;

    ts_loop_len -= (desc_len + 6);  
    while(desc_len > 0)
    {
      tmp_len = p_buf[offset + 1];

      switch(p_buf[offset])
      {
        case DVB_DESC_SERVICE_LIST:
          {
            u16 service_id = 0;
            u8 service_type = 0;
            u8 *p_log_des = p_buf + offset + 2;  //skip tag and len
            u8 des_len = tmp_len;
            
            while(des_len)
            {
              service_id = MAKE_WORD2(p_log_des[0], p_log_des[1]);
              service_type = p_log_des[2];

              UI_PRINTF("[parsebat]service_id: %d 0x%x\n", service_id,service_id);

              cur_bouquet_service_list_id[cur_bouquet_service_count] = service_id;
              cur_bouquet_service_count++;
              
              p_log_des += 3;
              if(des_len < 3) //for check error
              {
                des_len = 0;
              }
              else
              {
                des_len -= 3;
              }
            }
          }
          break;
          
        case 0x92:
          {
          }
          break;

        case 0x83:
          {
            logical_channel_t *p_log_ch = p_bat->log_ch_info + p_bat->log_ch_num;
            u8 *p_log_des = p_buf + offset + 2;  //skip tag and len
            u8 des_len = tmp_len;
            
            while(des_len)
            {
              if(p_bat->log_ch_num >= DVB_MAX_SDT_SVC_NUM) //check input
              {
                break;
              }
              p_log_ch->service_id = MAKE_WORD2(p_log_des[0], p_log_des[1]);
              p_log_ch->logical_channel_id = (((p_log_des[2]&0x03)<<8) | p_log_des[3]);
              
              add_logic_info(p_log_ch->service_id, p_log_ch->logical_channel_id);

              UI_PRINTF("[parsebat]s_id = %d, logic =%d\n", p_log_ch->service_id, p_log_ch->logical_channel_id);

              p_log_ch++;
              p_bat->log_ch_num++;
              p_log_des += 4;
              if(des_len < 4) //for check error
              {
                des_len = 0;
              }
              else
              {
                des_len -= 4;
              }
              
            }
          }
          break;
      }
      offset += (tmp_len + 2);
      desc_len -= (tmp_len + 2);
    }
  }

  if(g_bouquet_grp.bouquet_count != 0)
  {
    for(i=0; i<g_bouquet_grp.bouquet_count; i++)
    {
      if(bouquet_id == g_bouquet_grp.bouquet_id[i])
      {
        BouquetID_repeat = TRUE;
        repeat_bouq_index = i;
        break;
      }
    }
  }
  
  if(BouquetID_repeat)
  {
     UI_PRINTF("BouquetID_repeat\n");
    if(hasBouquetName)
    {
      str_asc2uni(bouquet_name, bouquet_name_unistr);
      memcpy(g_bouquet_grp.bouquet_name[repeat_bouq_index], bouquet_name_unistr, MAX_BOUQUET_NAMELEN);
    }

    for(i=0; i<cur_bouquet_service_count; i++)
    {
      if(g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]!=0)
      {
        for(j=0; j<g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]; j++)
        {
          if(cur_bouquet_service_list_id[i]==g_bouquet_grp.boq_SID_list[repeat_bouq_index][j])
          {
            break;
          }
        }

        if(j==g_bouquet_grp.boq_SID_list_count[repeat_bouq_index])
        {
          g_bouquet_grp.boq_SID_list[repeat_bouq_index][g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]] = cur_bouquet_service_list_id[i];
          g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]++;
        }
      }
      else
      {
        g_bouquet_grp.boq_SID_list[repeat_bouq_index][0] = cur_bouquet_service_list_id[i];
        g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]++;
      }
    }
  }
  else if((MAX_BOUQUET_GROUP - 8) <= g_bouquet_grp.bouquet_count)
  {
    UI_PRINTF("[error]==========MAX_BOUQUET_GROUP - 5 <= g_bouquet_grp.bouquet_coun ===========\n");
  }
  else
  {
         UI_PRINTF("g_bouquet_grp.bouquet_count = %d\n",g_bouquet_grp.bouquet_count);
    if(hasBouquetName)
    {
      str_asc2uni(bouquet_name, bouquet_name_unistr);
    }
    else
    {
      str_asc2uni((u8*)"unknow group", bouquet_name_unistr);
    }
    memcpy(g_bouquet_grp.bouquet_name[g_bouquet_grp.bouquet_count], bouquet_name_unistr, MAX_BOUQUET_NAMELEN);
    g_bouquet_grp.bouquet_id[g_bouquet_grp.bouquet_count] = bouquet_id;

    for(i=0; i<cur_bouquet_service_count; i++)
    {
      if(g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]!=0)
      {
        for(j=0; j<g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]; j++)
        {
          if(cur_bouquet_service_list_id[i]==g_bouquet_grp.boq_SID_list[g_bouquet_grp.bouquet_count][j])
          {
            break;
          }
        }

        if(j==g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count])
        {
          g_bouquet_grp.boq_SID_list[g_bouquet_grp.bouquet_count][g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]] = cur_bouquet_service_list_id[i];
          g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]++;
        }
      }
      else
      {
        g_bouquet_grp.boq_SID_list[g_bouquet_grp.bouquet_count][0] = cur_bouquet_service_list_id[i];
        g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]++;
      }
    }
    g_bouquet_grp.bouquet_count++;
  }

  UI_PRINTF("[parsebat]==========end===========\n");
  return;
}
#endif
void table_monitor_parse_lcn(u8 *p_buf, nit_t *p_nit)
{
  s16 network_desc_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  //u16 net_name_des_len = 0;
  u16 ts_id = 0;
  u16 offset = 0;
  u8  tp_cnt = 0 ;
  u8  svc_cnt = 0;
#ifdef QUICK_SEARCH_SUPPORT                
  s16 svc_list_des_length = 0;
#endif
  u16 nxt_ts_start = 0;

  if(p_buf[0] != DVB_TABLE_ID_NIT_ACTUAL &&
    p_buf[0] != DVB_TABLE_ID_NIT_OTHER)
  {
    UI_PRINTF("MDL: not nit\n");
    return;
  }

  if((p_buf[5] & 0x01)== 0)
  {
    UI_PRINTF("MDL: this nit is not usable\n");
    return;
  }
  p_nit->network_id = MAKE_WORD(p_buf[4], p_buf[3]);
  p_nit->version_num =(p_buf[5] & 0x3E) >> 1;
  p_nit->sec_number = p_buf[6];

  p_nit->last_sec_number = p_buf[7];
  
  network_desc_len = MAKE_WORD(p_buf[9], (p_buf[8] & 0x0F));
  offset = 10;
  //UI_PRINTF("network_desc_len[%d]\n", network_desc_len);
  
  while(network_desc_len > 0)
  {
    //net_name_des_len = p_buf[offset + 1];
    if(p_buf[offset] == DVB_DESC_NETWORK_NAME)
    {
      //Skip tag
      offset ++;
      network_desc_len --;
      
      //Fetch desc length
      desc_len = p_buf[offset];

      //Skip desc length
      offset ++;
      network_desc_len --;
  
      //get network name & skip it.
      //UI_PRINTF("nit network name: %s, length %d\n", &p_nit->network_name, desc_len);

      offset += desc_len;
      network_desc_len -= desc_len;
    }
    else if(p_buf[offset] == DVB_DESC_SERVICE_UPDATE)
    {
      //Skip tag
      offset += 1;
      network_desc_len -=1;

      //skip length
      offset += 1;
      network_desc_len -=1;
      
      //

      //Skip flag
      offset += 1;
      network_desc_len -= 1;

      // length is fixed as 1
      //UI_PRINTF("service update len: %d\n", p_buf[offset + 2]);
    }
    else if(p_buf[offset] == DVB_DESC_LOGIC_CHANNEL)
    {
      desc_len = p_buf[offset + 1];
      network_desc_len -= 2 + desc_len;
      offset += 2 + desc_len;
      // parse logical channel list here
    }
    else if(p_buf[offset] == DVB_DESC_LINKAGE)
    {
      desc_len = p_buf[offset + 1];
      network_desc_len -= 2 + desc_len;
      offset += 2 + desc_len;
    }
    else
    {
     offset ++;
     network_desc_len --;
    }
  }

  ts_loop_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
  //UI_PRINTF("ts_loop_len = %d\n", ts_loop_len);
  
  offset += 2;

  while(ts_loop_len > 0)
  {
    //UI_PRINTF("ts_loop_len[%d]\n", ts_loop_len);

    /*! ts id*/
    ts_id = MAKE_WORD(p_buf[offset + 1], p_buf[offset]);
    p_nit->tp_svc_list[tp_cnt].ts_id = ts_id;

    offset += 4;

    desc_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
    /*! Skip offset length*/
    offset += 2;
    
    ts_loop_len -= (desc_len + 6);
    /*! Saving next start position for next ts id*/
    nxt_ts_start = offset + desc_len; 
    svc_cnt   = 0 ;
    while(desc_len > 0)
    {
      //UI_PRINTF("desc_len[%d]\n", desc_len);
      switch(p_buf[offset])
      { 
        case 0x83:
        case 0x82:
          {
            u16 service_id = 0;
            u16 logical_channel_id = 0;
            u8 *p_log_des = p_buf + offset + 2;  //skip tag and len
            u8 des_len = 0;
            offset ++;
            desc_len --;
            
            //if(p_buf[offset] != 0x34)
            {
              desc_len -=  (s16)p_buf[offset];
              des_len = p_buf[offset];
              offset   +=  p_buf[offset];

              while(des_len)
              {
                service_id = MAKE_WORD2(p_log_des[0], p_log_des[1]);
                logical_channel_id = (((p_log_des[2]&0x03)<<8) | p_log_des[3]);
                
                add_logic_info(service_id, logical_channel_id);

                //UI_PRINTF("[parsebat]s_id = %d, logic =%d\n", service_id, logical_channel_id);
                
                p_log_des += 4;
                if(des_len < 4) //for check error
                {
                  des_len = 0;
                }
                else
                {
                  des_len -= 4;
                }
              }
            }
            //else      //skip
            {
              
            }
            offset ++;
            desc_len -- ;
          }
          break;
#ifdef QUICK_SEARCH_SUPPORT
        case DVB_DESC_SERVICE_LIST:
        /*!Jump to descriptor length*/
        offset += 1; 
        desc_len --;
        
        svc_list_des_length = p_buf[offset];
        desc_len -= svc_list_des_length;

        /*!Jump to service id */
        offset += 1;
        desc_len --;
        while(svc_list_des_length > 0)
        {
          UI_PRINTF("svc_list_des_length[%d]\n", svc_list_des_length);
          p_nit->tp_svc_list[tp_cnt].svc_id[svc_cnt] 
                  = MAKE_WORD(p_buf[offset+1], p_buf[offset]);
          /*!Skip service id and service type */
          offset += 3;
          svc_cnt ++;
          svc_list_des_length -=3;
          if(svc_cnt > MAX_PROG_NUM)
          {
            break;
          }
        }
        p_nit->tp_svc_list[tp_cnt].total_svc_num = svc_cnt;
          break;
        case DVB_DESC_SATELLITE_DELIVERY:
        {
          sat_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_cnt].dvbs_tp_info;

          p_nit->tp_svc_list[tp_cnt].tp_type = NIT_DVBS_TP; 
          /*! Skip tag*/
          offset += 1;
          desc_len -=1;
          
          /*! Fetch desc length*/
          desc_len -=  p_buf[offset];

          /*! Skip length*/
          offset += 1;
          desc_len -=1;

          p_tp_info->frequency  = make32(&p_buf[offset]);

          offset += 4;
          p_tp_info->orbital_position 
                                    = MAKE_WORD(p_buf[offset+1], p_buf[offset]);

          offset += 2;
          p_tp_info->west_east_flag = (p_buf[offset]&0x80)>>7;
          p_tp_info->polarization = (p_buf[offset] & 0x60)>> 5;
          //circule left 
          if(p_tp_info->polarization >= 2)
          {
            p_tp_info->polarization -= 2;
          }
          
          p_tp_info->modulation_type = (p_buf[offset] & 0x1F);

          offset += 1;
          p_tp_info->symbol_rate = make32(&p_buf[offset])>>4;  
          offset += 4;
        }
          break;
        case DVB_DESC_CABLE_DELIVERY:
          {
            cable_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_cnt].dvbc_tp_info;
            
            p_nit->tp_svc_list[tp_cnt].tp_type = NIT_DVBC_TP;
            offset += 1;
            desc_len -=1;
            desc_len -=  p_buf[offset];
            offset += 1;
            desc_len -=1;
            p_tp_info->frequency = make32(&p_buf[offset]);
            offset += 4;
            offset += 1;
            p_tp_info->fec_outer = p_buf[offset] & 0xf;
            offset += 1;
            p_tp_info->modulation = p_buf[offset];
            offset += 1;
            p_tp_info->symbol_rate = MT_MAKE_DWORD(
                      MAKE_WORD((p_buf[offset + 3] & 0xf0), p_buf[offset + 2]),
                      MAKE_WORD(p_buf[offset + 1], p_buf[offset]));
            p_tp_info->symbol_rate >>= 4;
            
            offset += 3;
            p_tp_info->fec_inner = p_buf[offset] & 0xf;
            offset += 1;
          }
          break;
#endif
        default:
          /*! 
            Jump to unknown desc length
           */
          offset ++;
          desc_len --;

          //UI_PRINTF("MDL: parse nit tmp_desc_length %d\n",p_buf[offset]);
          //UI_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          //Skip content of unkown descriptor
          //if(desc_len > tmp_desc_length)
          {
            desc_len -=  (s16)p_buf[offset];
            offset   +=  p_buf[offset];   
          }

          //UI_PRINTF("MDL: parse nit offset %d\n",  offset);
          //UI_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          offset ++;
          desc_len -- ;
          break;
        }

    }
    
    offset = nxt_ts_start ;
    tp_cnt ++;
    if(tp_cnt >= MAX_TP_NUM)
    {
      break;
    }
  }
  
  //All the tasks receiving this message must have higher task priority   //UI_PRINTF(("MDL: End parse nit\n"));
  return;  
}

static void table_monitor_pat_request(tm_handle_t *p_priv)
{
  service_t *p_dvb_svc = p_priv->priv.p_dvb_svc;
  dvb_request_t  pat_req = {0};
  
  pat_req.req_mode  = DATA_SINGLE;
  pat_req.table_id  = DVB_TABLE_ID_PAT;
  pat_req.para1     = DVB_TABLE_ID_PAT;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&pat_req, sizeof(dvb_request_t));
}

void table_monitor_edit_pg_info_save(dvbs_prog_node_t * p_pg, sdt_svc_descriptor_t *sdt_desc)
{
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
  //u8 temp_name[DB_DVBS_MAX_NAME_LENGTH + 1] = {0,};
  BOOL is_modified = FALSE;
  dvb_to_unicode(sdt_desc->name, DB_DVBS_MAX_NAME_LENGTH, name, DB_DVBS_MAX_NAME_LENGTH);
  
  //unicode_to_gb2312(p_pg->name, DB_DVBS_MAX_NAME_LENGTH, temp_name, DB_DVBS_MAX_NAME_LENGTH);

  if(uni_strcmp(p_pg->name, name) != 0)
  {
    //OS_PRINTF("enter table_monitor_edit_pg_info_save,p_sdt->name1 [%s], p_sdt->name2 [%s]\n", temp_name, sdt_desc->name);
    memcpy(p_pg->name, name, DB_DVBS_MAX_NAME_LENGTH);
    OS_PRINTF("program name changed\n");
    is_modified = TRUE;
    //send_evt_to_ui(PB_EVT_UPDATE_PG_NAME, (u32)p_pg->id, 0);
  }

  if(p_pg->is_scrambled != sdt_desc->is_scrambled)
  {
    p_pg->is_scrambled = sdt_desc->is_scrambled;
    is_modified = TRUE;
  }
  
  if(p_pg->run_status != sdt_desc->run_sts)
  {
    OS_PRINTF("enter table_monitor_edit_pg_info_save,pg.run_status [%d], p_sdt->svc_des[i].run_sts [%d]\n", p_pg->run_status, sdt_desc->run_sts);
    p_pg->run_status = sdt_desc->run_sts;
    if(p_pg->id == sys_status_get_curn_group_curn_prog_id())
    {
      send_msg_to_ui(TM_SDT_RUN_STATUS, p_pg->run_status, 0);
    }
    is_modified = TRUE;
  }

  if(is_modified)
  {
    db_dvbs_edit_program(p_pg);
    db_dvbs_save_pg_edit(p_pg);
  }
}

void table_monitor_edit_pg(tm_handle_t *p_priv, sdt_t *p_sdt, u32 tp_id)
{  
  u8 view_id;
  u16 pg_count;
  u16 i,j;
  u16 pg_id;
  dvbs_prog_node_t prog = {0};
  BOOL bFinded = FALSE;
  BOOL bEdited = FALSE;
  u16 rid;
  u8 cnt = 0;
  dvbs_prog_node_t *pg = NULL;
  view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  pg_count = db_dvbs_get_count(view_id);
  for(i=0; i<pg_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);
    bFinded = FALSE;
    //OS_PRINTF("[table_monitor_edit_pg] : tp id [%d], prog.tp_id [%d] \n", tp_id, prog.tp_id);
    if(tp_id == prog.tp_id)
    {
      for(j=0; j<p_sdt->svc_count; j++)
      {
        if(p_sdt->svc_des[j].svc_id == prog.s_id)
        {
          table_monitor_edit_pg_info_save(&prog, &(p_sdt->svc_des[j]));
          bFinded = TRUE;
          break;
        }
      }
      if(bFinded == FALSE)
      {
        //OS_PRINTF("[table_monitor_edit_pg] : DB_DVBS_DEL_FLAG : pg_id [%d] \n", pg_id);
        if(prog.id == sys_status_get_curn_group_curn_prog_id())
        {
          if ((rid =
                 db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                            0)) != INVALIDID)
          {
            ui_play_prog(rid, FALSE);
          }
        }
        db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
        bEdited = TRUE;
      }
    }
  }

  if(bEdited)
  {
    db_dvbs_save(view_id);
    bEdited = FALSE;
  }
  
  bFinded = FALSE;
    
  for(i=0; i<p_sdt->svc_count; i++)
  {
    bFinded = FALSE;
    for(j=0; j<pg_count; j++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, j);
      db_dvbs_get_pg_by_id(pg_id, &prog);
      if(tp_id == prog.tp_id)
      {
        if(p_sdt->svc_des[i].svc_id == prog.s_id)
        {
          bFinded = TRUE;
          break;
        }
      }
    }
    if(bFinded == FALSE)
    {
      pg = &(p_priv->priv.add_pg[cnt]);
      memset(pg, 0, sizeof(dvbs_prog_node_t));
      sprintf((char *)p_priv->priv.pg_name[cnt],"%s",(u8 *)p_sdt->svc_des[i].name);
      pg->service_type = p_sdt->svc_des[i].service_type;
      pg->s_id = p_sdt->svc_des[i].svc_id;
      pg->ts_id = p_sdt->stream_id;
      pg->orig_net_id = p_sdt->org_network_id;
      pg->is_scrambled = p_sdt->svc_des[i].is_scrambled;
      pg->tp_id = tp_id;
      pg->volume = DEFAULT_VOLUME;
      pg->audio_track = 1;
      pg->skp_flag = 0;
      pg->hide_flag = 0;
      /*
      pg->run_status = p_sdt->svc_des[i].run_sts;
      OS_PRINTF("table_monitor_edit_pg : new pg run_sts : %d\n", pg->run_status);
      OS_PRINTF("table_monitor_edit_pg : new pg service_type : %d\n", pg->service_type);
      */
      //OS_PRINTF("######pg->s_id [%d] \n", pg->s_id);
      if (p_sdt->svc_des[i].mosaic_des_found
        || p_sdt->svc_des[i].logic_screen_des_found
        || p_sdt->svc_des[i].linkage_des_found)
      {
        memcpy(&pg->mosaic, (mosaic_t *)p_sdt->svc_des[i].p_nvod_mosaic_des, sizeof(mosaic_t));
        pg->mosaic_flag = pg->mosaic.lg_cell_cnt;
        p_sdt->svc_des[i].mosaic_des_found = 0;
        p_sdt->svc_des[i].logic_screen_des_found = 0;
        p_sdt->svc_des[i].linkage_des_found = 0;
      }
      cnt ++;
      if(cnt == MAX_ADD_PG)
        break;
    }  
  }
  p_priv->priv.add_pg_num = cnt;
  p_priv->priv.add_pg_index = 0;
  table_monitor_pat_request(p_priv);
}

static void table_monitor_pmt_request(tm_handle_t *p_priv, os_msg_t *p_msg)
{
  service_t *p_dvb_svc = p_priv->priv.p_dvb_svc;
  u32 param = ((p_msg->para1 << 16) | p_msg->para2);
  dvb_request_t req_param = {0};
  if(p_priv->priv.add_pg[p_priv->priv.add_pg_index].s_id == p_msg->para2)
  {
    req_param.req_mode = DATA_SINGLE;
    req_param.table_id = DVB_TABLE_ID_PMT;
    req_param.para1 = DVB_TABLE_ID_PMT;
    req_param.para2 = param;
    p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&req_param, sizeof(dvb_request_t));
  }
}

static void table_monitor_nit_request(tm_handle_t *p_priv , u32 para2)
{
  service_t *p_dvb_svc = p_priv->priv.p_dvb_svc;
  dvb_request_t  nit_req = {0};

  nit_req.req_mode = DATA_SINGLE;
  nit_req.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  nit_req.para1 = DVB_TABLE_ID_NIT_ACTUAL;
  nit_req.para2 = para2;
  
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST,(u32)&nit_req, sizeof(dvb_request_t));
}

static void table_monitor_bat_request(tm_handle_t *p_priv)
{
  service_t *p_dvb_svc = p_priv->priv.p_dvb_svc;
  
  dvb_request_t param = {0};
  //scan_data_t *p_scan_data = p_scan_priv->p_priv_data;
 // scan_policy_t *p_policy = p_scan_priv->p_policy;
  filter_ext_buf_t *p_ext_buf_list = &(p_priv->priv.ext_buf_list);
  
//  p_ext_buf_list = mtos_malloc(sizeof(filter_ext_buf_t));
  //p_ext_buf_list = 
   // p_policy->require_runingtime_buffer(p_policy->p_priv_data, sizeof(
//filter_ext_buf_t));
  if(p_ext_buf_list->p_buf == NULL)
  {
    p_ext_buf_list->p_buf = mtos_malloc(BAT_REQUEST_BUFFER);
    MT_ASSERT(p_ext_buf_list->p_buf != NULL);
    p_ext_buf_list->size = BAT_REQUEST_BUFFER;
  }

 //   p_policy->require_runingtime_buffer(p_policy->p_priv_data, 
///BAT_REQUEST_BUFFER);
//p_ext_buf_list->size = BAT_REQUEST_BUFFER;

  param.req_mode  = DATA_MULTI;
  param.table_id  = DVB_TABLE_ID_BAT;
  param.para1     = DVB_TABLE_ID_BAT;
  param.para2     = (u32)p_ext_buf_list;
  
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST,
  (u32)&param, sizeof(dvb_request_t));
  //p_scan_data->p_rt_data->job_dbg_info.bat_num++;
  //p_scan_data->p_rt_data->pending_job++;

  p_priv->priv.bat_multi_mode = TRUE;
  p_priv->priv.start_bat_tickets = mtos_ticks_get();

}

void table_monitor_scan_free_bat(tm_handle_t *p_priv)
{
  service_t *p_dvb_svc = p_priv->priv.p_dvb_svc;
  dvb_request_t param = {0};
  
  param.req_mode  = DATA_MULTI;
  param.table_id  = DVB_TABLE_ID_BAT;
  param.para1     = DVB_TABLE_ID_BAT;
  param.para2     = 0;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_FREE,(u32)&param, sizeof(dvb_request_t));
  
  p_priv->priv.bat_multi_mode = FALSE;
  p_priv->priv.start_bat_tickets = 0;
  memset(p_priv->priv.bat_sec_info, 0, sizeof(bat_sec_info_t) * MAX_BOU_ID_NUM);
  free_bouquet_group_tmp();
}
#ifdef QUICK_SEARCH_SUPPORT
static void table_monitor_free_sdt_otr(tm_handle_t *p_priv)
{
  p_priv->priv.total_svc_found_list = 0;
  p_priv->priv.sdt_otr_start_ticks = 0;
  p_priv->priv.pg_num = 0;
  memset(p_priv->priv.is_nit_recv, 0 , MAX_TP_NUM * sizeof(BOOL));
  memset(p_priv->priv.tp_svc_list, 0, sizeof(tp_svc_list_t) *MAX_TP_NUM );
  memset(p_priv->priv.is_sdt_otr_recv, 0 , MAX_TP_NUM * sizeof(BOOL));
  memset(p_priv->priv.total_pg_list, 0 , sizeof(pg_info_t) * MAX_BROG_NUM_IN_SDT_OTR);
  p_priv->priv.bSdtOtrAvailable = FALSE;
}
#endif
#ifdef LOGIC_NUM_SUPPORT
extern BOOL have_logic_number();
#endif
static void table_monitor_pmt_found(tm_handle_t *p_priv, os_msg_t *p_msg)
{
  pmt_t *p_pmt = (pmt_t *)p_msg->para1;
  dvbs_prog_node_t *p_pg = &(p_priv->priv.add_pg[p_priv->priv.add_pg_index]);
  u8 loopi = 0;
  u8 view_id;
#ifdef LOGIC_NUM_SUPPORT
  u16 un_india_ts_logic_num = 1;
#endif
  u8 name_size = 0;
  u8 p_name[MAX_SVC_NAME_SUPPORTED] = {0};
  u16  pg_name_uni[DB_DVBS_MAX_NAME_LENGTH+1] = {0};
  bouquet_group_t *bouquet_grp = get_bouquet_group();
  u16 pg_count;
  u16 i,j,k;
  u16 pg_id;
  dvbs_prog_node_t prog = {0};
  //OS_PRINTF("table_monitor_pmt_found : PG RUN STATUS: %d\n", p_pg->run_status);
  ui_cache_view();
  view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  pg_count = db_dvbs_get_count(view_id);
  if(p_pmt->prog_num == p_pg ->s_id)
  {
    //Add program information
    if(p_pmt->video_pid == 0
        && p_pmt->audio_count == 0)
    {
      //PMT is for data stream
      ui_restore_view();
      return;
    }
    for(i = 0; i < pg_count; i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
      db_dvbs_get_pg_by_id(pg_id, &prog);
      if((p_pg->s_id == prog.s_id) &&
        (p_pg->tp_id == prog.tp_id))
      {
        ui_restore_view();
        return;
      }
    }

    p_pg->audio_ch_num = (p_pmt->audio_count > SCAN_MAX_AUDIO_CHANNEL)?
      SCAN_MAX_AUDIO_CHANNEL : p_pmt->audio_count;
    p_pg->pcr_pid   = p_pmt->pcr_pid;
    p_pg->video_pid = p_pmt->video_pid;
    p_pg->video_type = p_pmt->video_type;
    p_pg->pmt_pid = p_pmt->pmt_pid;
    p_pg->ecm_num = p_pmt->ecm_cnt;
    
    p_pg->tv_flag = (p_pg->video_pid != 0) ? 1 : 0;

    for(loopi = 0; loopi < p_pg->audio_ch_num; loopi++)
    {
      p_pg->audio[loopi] = p_pmt->audio[loopi];
    }
    if((p_pg->service_type == 0) || (p_pg->service_type == SVC_TYPE_TV))
    {
      if(p_pg->video_pid == 0)
      {
        p_pg->service_type = SVC_TYPE_RADIO;
      }
      else
      {
        p_pg->service_type = SVC_TYPE_TV;
      }
    }
    else if((p_pg->service_type == SVC_TYPE_DATA) || (p_pg->service_type == SVC_TYPE_DATA))
    {
      if(p_pg->audio[0].p_id != 0) 
      {
        p_pg->service_type = SVC_TYPE_RADIO;
      }
      else
      {
        p_pg->service_type = SVC_TYPE_TRESERVED;
      }
    }

    if (p_pmt->mosaic_des_found
      || p_pmt->stream_identifier_des_found
      || p_pmt->logic_screen_des_found)
    {
      memcpy(&p_pg->mosaic, (mosaic_t *)p_pmt->p_mosaic_des, sizeof(mosaic_t));
      p_pg->mosaic_flag = p_pg->mosaic.lg_cell_cnt;
      p_pmt->mosaic_des_found = 0;
      p_pmt->stream_identifier_des_found = 0;
      p_pmt->logic_screen_des_found = 0;
    }
#ifdef LOGIC_NUM_SUPPORT
    if(have_logic_number())
    {
      p_pg->logical_num = find_logic_number_by_sid(p_pg->s_id);
      if(!p_pg->logical_num)
      {
        un_india_ts_logic_num = sys_status_get_default_logic_num();
        p_pg->logical_num = un_india_ts_logic_num;
        un_india_ts_logic_num++;
        sys_status_set_default_logic_num(un_india_ts_logic_num);
      }
    }
    else
      p_pg->logical_num = 0;
#endif
    
    //make a name for no name for some pgs
    if(strlen((char *)p_priv->priv.pg_name[p_priv->priv.add_pg_index]) == 0)
    {
      if(p_pg->video_pid != 0)
      {
        sprintf((char *)p_name,"%s%d","TV CH",p_pg->s_id);
      }
      else
      {
        sprintf((char *)p_name,"%s%d","RADIO CH",p_pg->s_id);
      }
      name_size = strlen((char *)p_priv->priv.pg_name[p_priv->priv.add_pg_index]);
      if(name_size > MAX_SVC_NAME_SUPPORTED)
      {
        name_size = MAX_SVC_NAME_SUPPORTED;
      }
      strncpy((char *)p_priv->priv.pg_name[p_priv->priv.add_pg_index],(char *)p_name,name_size);
    }
    dvb_to_unicode(p_priv->priv.pg_name[p_priv->priv.add_pg_index], 
            MAX_SVC_NAME_SUPPORTED, pg_name_uni, DB_DVBS_MAX_NAME_LENGTH+1);
    uni_strncpy(p_pg->name, pg_name_uni, DB_DVBS_MAX_NAME_LENGTH);

    if(bouquet_grp->bouquet_count != 0)
    {
      for(j=0; j<bouquet_grp->bouquet_count; j++)
      {
        for(k=0; k<bouquet_grp->boq_SID_list_count[j]; k++)
        {
          if(p_pg->s_id == bouquet_grp->boq_SID_list[j][k])
          {
            p_pg->bouquet_id = bouquet_grp->bouquet_id[j];
            p_pg->fav_grp_flag |= (1<<j);          
            break;
          }
        }
      }
    }
    
    OS_PRINTF("before add pg service_type : %d, p_pg->video_pid : %d\n", p_pg->service_type, p_pg->video_pid);
    db_dvbs_add_program(view_id, p_pg);
    db_dvbs_save(view_id);
    p_priv->priv.add_pg_index++;
  }

  if(p_priv->priv.add_pg_index >= p_priv->priv.add_pg_num)
  {
    p_priv->priv.add_pg_index = 0;
    p_priv->priv.add_pg_num = 0;
    memset(&(p_priv->priv.add_pg[0]), 0, sizeof(dvbs_prog_node_t) * MAX_ADD_PG);
    ui_dbase_pg_sort(DB_DVBS_ALL_PG);
  }
  else
  {
    table_monitor_pat_request(p_priv);
  }
  ui_restore_view();
}
#ifdef QUICK_SEARCH_SUPPORT  
static BOOL is_svc_list_exist(tm_handle_t *p_priv, tp_svc_list_t *p_tp_svc_list)
{
  tp_svc_list_t *p_cur_svc_list = p_priv->priv.tp_svc_list;
  u16 svc_list_cnt = 0;

  for(svc_list_cnt = 0; svc_list_cnt < p_priv->priv.total_svc_found_list; svc_list_cnt++)
  {
    p_cur_svc_list = p_priv->priv.tp_svc_list + svc_list_cnt;

    if(p_cur_svc_list->ts_id == p_tp_svc_list->ts_id
        || p_tp_svc_list->total_svc_num == 0
        || p_tp_svc_list->tp_type != NIT_DVBC_TP)
    {
      return TRUE;
    }
  }
  return FALSE;
}
static void add_svc_list(tm_handle_t *p_priv, nit_t *p_nit)
{
  u8 i;
  for(i = 0; i < MAX_TP_NUM;i++)
  {
    if(is_svc_list_exist(p_priv, &(p_nit->tp_svc_list[i])) == FALSE
      && p_nit->tp_svc_list->tp_type == NIT_DVBC_TP)
    {
      memcpy(&(p_priv->priv.tp_svc_list[p_priv->priv.total_svc_found_list]), &(p_nit->tp_svc_list[i]), sizeof(tp_svc_list_t));
      p_priv->priv.total_svc_found_list ++;
    }
  }
  i = 0;
}
#endif
static void table_monitor_nit_found(tm_handle_t *p_priv, os_msg_t *p_msg)
{
  tm_handle_t *p_this = (tm_handle_t *)p_priv;
  nit_t *p_nit = (nit_t *)p_msg->para1;
  u32 para2 = 0;
  OS_PRINTF("table_monitor_nit_found\n");
#ifdef QUICK_SEARCH_SUPPORT  
  p_priv->priv.is_nit_recv[p_nit->sec_number] = TRUE;
  add_svc_list(p_priv, p_nit);
#endif
  if (p_nit->sec_number < p_nit->last_sec_number)
  {
    para2 = ((p_nit->sec_number + 1) << 16) | 0xFFFF;
    table_monitor_nit_request(p_this, para2);
  }
#ifdef QUICK_SEARCH_SUPPORT
  else if(p_nit->sec_number == p_nit->last_sec_number)
  {
    dvb_request_t param = {0};
    service_t *p_dvb_svc = p_priv->priv.p_dvb_svc;
    u8 i = 0;
    BOOL bNit = TRUE;
    for(i = 0; i <= p_nit->last_sec_number; i++)
    {
      if(p_priv->priv.is_nit_recv[i] == FALSE)
      {
        bNit = FALSE;
        break;
      }
    }
    if(bNit == TRUE
      &&( mtos_ticks_get() - p_priv->priv.pg_edit_ticks > 12000 || p_priv->priv.pg_edit_ticks == 0))
    {
        // Request SDT other
      param.req_mode  = DATA_SINGLE;
      param.table_id  = DVB_TABLE_ID_SDT_OTHER;
      param.para1     = DVB_TABLE_ID_SDT_OTHER;
      p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST,
        (u32)&param, sizeof(dvb_request_t));
      p_priv->priv.sdt_otr_start_ticks = mtos_ticks_get();
      p_priv->priv.pg_num = 0;
      memset(p_priv->priv.total_pg_list, 0 , sizeof(pg_info_t) * MAX_BROG_NUM_IN_SDT_OTR);
      memset(p_priv->priv.is_sdt_otr_recv, 0 , MAX_TP_NUM * sizeof(BOOL));
      memset(p_this->priv.is_nit_recv, 0 , MAX_TP_NUM * sizeof(BOOL));
      p_this->priv.bSdtOtrAvailable = TRUE;
    }
    else
    {
      table_monitor_free_sdt_otr(p_priv);
    }
  }
#endif  
}

static void table_monitor_bat_found(tm_handle_t *p_priv, os_msg_t *p_msg)
{
  tm_handle_t *p_this = (tm_handle_t *)p_priv;
  bat_t *p_bat = (bat_t *)p_msg->para1;
  bouquet_group_t *bouquet_grp_tmp = get_bouquet_group_tmp();
  bouquet_group_t *bouquet_grp = get_bouquet_group();
  u8 view_id;
  u16 pg_count;
  u16 i,j;
  u16 pg_id, k;
  BOOL edit_flag = FALSE;
  dvbs_prog_node_t prog = {0};
  u8 fav_name[MAX_BOUQUET_NAMELEN] = {0};
  u16 fav_name_unistr[MAX_BOUQUET_NAMELEN] = {0};
  u32 fav_temp = 0;
  OS_PRINTF("table_monitor_bat_found\n");
  p_this->priv.start_bat_tickets = mtos_ticks_get();

  add_bouquet_id(p_this, p_bat);
  if(is_need_free_bat(p_this, p_bat))
  {
    sort_tmpgrp_by_bouquet_id();
    if(memcmp(bouquet_grp, bouquet_grp_tmp, sizeof(bouquet_group_t)) != 0 &&   p_this->priv.bat_is_received == TRUE)
    {
      OS_PRINTF("update categories!!!!!!\n");
      ui_cache_view();
      view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
      pg_count = db_dvbs_get_count(view_id);
      for(i=0; i<bouquet_grp_tmp->bouquet_count; i++)
      {
        sys_status_set_fav_name((u8)i, bouquet_grp_tmp->bouquet_name[i]);
        sys_status_set_bouquet_id(i, bouquet_grp_tmp->bouquet_id[i]);
      }
      sys_status_set_categories_count(bouquet_grp_tmp->bouquet_count+FAV_LIST_COUNT);
      
      for(i=0; i < FAV_LIST_COUNT; i++)
      {
        sprintf((char *)fav_name, "Fav List%d", i+1);
        str_asc2uni(fav_name, fav_name_unistr);
        sys_status_set_fav_name(bouquet_grp_tmp->bouquet_count+i, fav_name_unistr);
      }
      for(i=0; i<pg_count; i++)
      {
        edit_flag = FALSE;
        pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
        db_dvbs_get_pg_by_id(pg_id, &prog);
        fav_temp = prog.fav_grp_flag;
        for(j = 0; j < FAV_LIST_COUNT; j++)
        {
          if((fav_temp & (1<<(bouquet_grp->bouquet_count + j))) != 0)
          {
            edit_flag = TRUE;
            prog.fav_grp_flag &= ~(1<<(bouquet_grp->bouquet_count + j));
            prog.fav_grp_flag |= (1<<(bouquet_grp_tmp->bouquet_count + j));
          }
        }
        if(edit_flag)
        {
          db_dvbs_edit_program(&prog);
        }
      }

      for(i=0; i<pg_count; i++)
      {
        pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
        db_dvbs_get_pg_by_id(pg_id, &prog);
        for(j=0; j<bouquet_grp_tmp->bouquet_count; j++)
        {
          prog.fav_grp_flag &= ~(1<<j);
        }
        db_dvbs_edit_program(&prog);
      }
      
      for(i=0; i<pg_count; i++)
      {
        edit_flag = FALSE;
        pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
        db_dvbs_get_pg_by_id(pg_id, &prog);
        
        for(j=0; j<bouquet_grp_tmp->bouquet_count; j++)
        {
          for(k=0; k<bouquet_grp_tmp->boq_SID_list_count[j]; k++)
          {
            if(prog.s_id == bouquet_grp_tmp->boq_SID_list[j][k])
            {
              edit_flag = TRUE;
              prog.bouquet_id = bouquet_grp_tmp->bouquet_id[j];
              prog.fav_grp_flag |= (1<<j);
              break;
            }
          }
        }
        
        if(edit_flag)
        {
          db_dvbs_edit_program(&prog);
        }
      }

      table_monitor_edit_logic_number(FALSE);
      
      sys_status_save();
      ui_restore_view();
      memcpy(bouquet_grp, bouquet_grp_tmp, sizeof(bouquet_group_t));
    }
    table_monitor_scan_free_bat(p_this);
  }
  p_this->priv.bat_is_received = TRUE;
}
#ifdef QUICK_SEARCH_SUPPORT
static BOOL dvbc_is_need_sdt_otr(tm_handle_t *p_priv)
{  
  tp_svc_list_t *p_cur_svc_list;
  u16 svc_list_cnt = 0;

  if(mtos_ticks_get() - p_priv->priv.sdt_otr_start_ticks >= 6000)
    return FALSE;

  p_cur_svc_list = p_priv->priv.tp_svc_list;
  for(svc_list_cnt = 0; svc_list_cnt < p_priv->priv.total_svc_found_list; svc_list_cnt++)
  {
    p_cur_svc_list = p_priv->priv.tp_svc_list + svc_list_cnt;

    if(p_priv->priv.is_sdt_otr_recv[svc_list_cnt] == FALSE)
    {
      return TRUE;
    }
  }
  
  return FALSE;
}

static BOOL find_pg_in_svc_list(tm_handle_t *p_priv, u32 sid, u16 svc_index)
{
  u16 i;
  for(i = 0; i < p_priv->priv.tp_svc_list[svc_index].total_svc_num ; i++)
  {
    if(p_priv->priv.tp_svc_list[svc_index].svc_id[i] == sid)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*!
  Check whether input tp information is a new tp
  \param[in] p_tp_info input tp information
  */
static BOOL is_new_tp(u8 tp_view_id, dvbs_tp_node_t *p_new_tp, u16 *p_sim_idx)
{
  u16 index = 0;
  dvbs_tp_node_t old_tp = {0};
  //db_dvbs_ret_t ret = DB_DVBS_OK;
  //u8 tp_view = 0;
  u16 cnt = 0;
  u16 pos = 0;

  cnt = db_dvbs_get_count(tp_view_id);

  for(index = 0; index < cnt; index++)
  {
    pos = db_dvbs_get_id_by_view_pos(tp_view_id, index);
    db_dvbs_get_tp_by_id(pos, &old_tp);
    if(old_tp.freq == p_new_tp->freq)
    {
      *p_sim_idx = pos;
      return FALSE;
    }
  }
  return TRUE;
}

static void parse_svc_tp(cable_tp_info_t *p_tp_info, dvbs_tp_node_t *p_tp)
{
  p_tp->freq = bcd_number_to_dec(p_tp_info->frequency)/10;
  p_tp->sym = bcd_number_to_dec(p_tp_info->symbol_rate)/10;
  switch(p_tp_info->modulation)
  {
    case 1:
    p_tp->nim_modulate = NIM_MODULA_QAM16;
    break;

    case 2:
      p_tp->nim_modulate = NIM_MODULA_QAM32;
      break;

    case 3:
      p_tp->nim_modulate = NIM_MODULA_QAM64;
      break;

    case 4:
      p_tp->nim_modulate = NIM_MODULA_QAM128;
      break;

    case 5:
      p_tp->nim_modulate = NIM_MODULA_QAM256;
      break;
    /*
    case 6:
      p_last_tp_para->nim_modulate = 0xFF;//reserved for future use
      break; */
    default:
      p_tp->nim_modulate = NIM_MODULA_QAM64;
  }
}

static RET_CODE add_pg_list(tm_handle_t *p_priv, sdt_t *p_sdt)
{
  u32 i = 0, j = 0;
  BOOL already_exist = FALSE;
  RET_CODE ret = SUCCESS;

  for(i = 0; i < p_sdt->svc_count; i++)
  {
    for(j = 0; j < p_priv->priv.pg_num; j ++)
    {
      if(p_priv->priv.total_pg_list[j].s_id == p_sdt->svc_des[i].svc_id)
      {
        already_exist = TRUE;
      }
    }
    if(already_exist == TRUE)
    {
      already_exist = FALSE;
      continue;
    }
    if(p_sdt->stream_id == 0)
    {
      continue;
    }
    
    if (p_sdt->svc_des[i].service_type == DVB_NVOD_TIME_SHIFTED_SVC
      || p_sdt->svc_des[i].service_type == 0x19)
    {
      continue;
    }

    p_priv->priv.total_pg_list[p_priv->priv.pg_num].s_id = p_sdt->svc_des[i].svc_id;
    p_priv->priv.total_pg_list[p_priv->priv.pg_num].orig_net_id = p_sdt->org_network_id;
    p_priv->priv.total_pg_list[p_priv->priv.pg_num].is_scrambled = p_sdt->svc_des[i].is_scrambled;
    p_priv->priv.total_pg_list[p_priv->priv.pg_num].ca_system_id = p_sdt->svc_des[i].ca_system_id;
    p_priv->priv.total_pg_list[p_priv->priv.pg_num].ts_id = p_sdt->stream_id;
    p_priv->priv.total_pg_list[p_priv->priv.pg_num].service_type = p_sdt->svc_des[i].service_type;
    if (p_sdt->svc_des[i].mosaic_des_found
  || p_sdt->svc_des[i].logic_screen_des_found
  || p_sdt->svc_des[i].linkage_des_found)
    {
      memcpy(&p_priv->priv.total_pg_list[p_priv->priv.pg_num].mosaic, (mosaic_t *)p_sdt->svc_des[i].p_nvod_mosaic_des, sizeof(mosaic_t));
    }
    else
    {
      p_priv->priv.total_pg_list[p_priv->priv.pg_num].mosaic.lg_cell_cnt = 0;
    }
    memcpy(p_priv->priv.total_pg_list[p_priv->priv.pg_num].pg_name, p_sdt->svc_des[i].name, MAX_SVC_NAME_LEN);
    p_priv->priv.pg_num ++;
  }
  return ret;
}

static void table_monitor_sdt_add_remove_pg(tm_handle_t *p_priv)
{
  u8 pg_view_id, tp_view_id;
  u16 pg_count;
  u16 i,j,k;
  u16 pg_id;
  dvbs_prog_node_t prog = {0};
  BOOL bFinded = FALSE;
  //dvbs_tp_node_t prog_tp = {0};
  dvbs_tp_node_t tp = {0};
  u16 similar_tp_idx = 0;
  RET_CODE ret = SUCCESS;
  dvbs_prog_node_t pg = {0};
  bouquet_group_t *bouquet_grp = get_bouquet_group();  
  BOOL is_pg_edit = FALSE;
  u16 all_channel_sid[DB_DVBS_MAX_PRO];
  BOOL edit_flag = FALSE;

  memset(all_channel_sid, 0 , DB_DVBS_MAX_PRO * sizeof(u16));
  ui_cache_view();

  tp_view_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  pg_view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  pg_count = db_dvbs_get_count(pg_view_id);
  for(i = 0; i < pg_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(pg_view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);
    all_channel_sid[i] = prog.s_id;
  }
  for(i=0; i<p_priv->priv.pg_num; i++)
  {
    if(p_priv->priv.bSdtOtrAvailable == FALSE)
    {
      table_monitor_free_sdt_otr(p_priv);
      break;
    }
    bFinded = FALSE;
    for(j = 0; j < pg_count; j++)
    {
      if(p_priv->priv.total_pg_list[i].s_id == all_channel_sid[j])
      {
         bFinded = TRUE;
        break;
      }
    }
    if(bFinded == FALSE)
    {
      if(p_priv->priv.total_svc_found_list > 0)
      {
        for(k = 0; k < p_priv->priv.total_svc_found_list; k++)
        {
          if(find_pg_in_svc_list(p_priv,p_priv->priv.total_pg_list[i].s_id, k) == TRUE)
          {
            parse_svc_tp(&(p_priv->priv.tp_svc_list[k].dvbc_tp_info), &tp);

            //it's may be the new tp , need add it to 
            {
              if(is_new_tp(tp_view_id, &tp, &similar_tp_idx))
              {
                ret = db_dvbs_add_tp(tp_view_id, &tp);
                
                //add to list
                pg.tp_id = tp.id;
                db_dvbs_save(tp_view_id);
              }
              else
              {
                pg.tp_id = similar_tp_idx;
              }
            }
          }
        }
      }
      pg.orig_net_id = p_priv->priv.total_pg_list[i].orig_net_id;
      pg.is_scrambled = p_priv->priv.total_pg_list[i].is_scrambled;
      pg.s_id = p_priv->priv.total_pg_list[i].s_id;
      pg.ca_system_id = p_priv->priv.total_pg_list[i].ca_system_id;
      pg.ts_id = p_priv->priv.total_pg_list[i].ts_id;
      pg.service_type = p_priv->priv.total_pg_list[i].service_type;
      
      if(pg.service_type == SVC_TYPE_RADIO)
        pg.tv_flag = 0;
      else
        pg.tv_flag = 1;
      
      if (p_priv->priv.total_pg_list[i].mosaic.lg_cell_cnt)
      {
        if (pg.service_type == DVB_MOSAIC_SVC)
        {
          pg.mosaic_flag = p_priv->priv.total_pg_list[i].mosaic.lg_cell_cnt;
          memcpy(&pg.mosaic, &(p_priv->priv.total_pg_list[i].mosaic), sizeof(mosaic_t));
        }
      }
      //
      //The service name length in sdt is less than 16
      MT_ASSERT(MAX_SVC_NAME_SUPPORTED >= MAX_SVC_NAME_LEN);
      if(strlen((char *)p_priv->priv.total_pg_list[i].pg_name) == 0)
      {
        u8 p_name[MAX_SVC_NAME_SUPPORTED] = {0};
        u8 name_size = 0;
        sprintf((char *)p_name,"%s%d","CH",pg.s_id);
        name_size = strlen((char *)p_name);
        if(name_size > MAX_SVC_NAME_SUPPORTED)
          {
            name_size = MAX_SVC_NAME_SUPPORTED;
          }
        strncpy((char *)p_priv->priv.total_pg_list[i].pg_name,(char *)p_name,name_size);
      }

      //Transfer name to unicode
      dvb_to_unicode(p_priv->priv.total_pg_list[i].pg_name, 
              sizeof(p_priv->priv.total_pg_list[i].pg_name), pg.name, DB_DVBS_MAX_NAME_LENGTH+1);
      db_dvbs_add_program(pg_view_id, &pg);
      is_pg_edit = TRUE;
    }
  }
  if(is_pg_edit == TRUE)
  {
     db_dvbs_save(pg_view_id);
  }
  for(i = 0; i < pg_count; i++)
  {
    if(p_priv->priv.bSdtOtrAvailable == FALSE)
    {
      table_monitor_free_sdt_otr(p_priv);
      break;
    }
    bFinded = FALSE;
    for(j = 0; j < p_priv->priv.pg_num; j++)
    {
      if(all_channel_sid[i] == p_priv->priv.total_pg_list[j].s_id)
      {
        bFinded = TRUE;
        break;
      }
    }
    if(bFinded == FALSE)
    {
      db_dvbs_change_mark_status(pg_view_id, i, DB_DVBS_DEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
      db_dvbs_save(pg_view_id);
      is_pg_edit = TRUE;
    }
  }
  if(is_pg_edit == TRUE)
  {
    pg_view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
    pg_count = db_dvbs_get_count(pg_view_id);
    for(i = 0; i < pg_count; i++)
    {
      pg_id = db_dvbs_get_id_by_view_pos(pg_view_id, i);
      db_dvbs_get_pg_by_id(pg_id, &prog);
      if(bouquet_grp->bouquet_count != 0)
      {
        for(j=0; j<bouquet_grp->bouquet_count; j++)
        {
          for(k=0; k<bouquet_grp->boq_SID_list_count[j]; k++)
          {
            if(prog.s_id == bouquet_grp->boq_SID_list[j][k])
            {
              edit_flag = TRUE;
              prog.bouquet_id = bouquet_grp->bouquet_id[j];
              prog.fav_grp_flag |= (1<<j);        
              break;
            }
          }
        }
      }
      if(edit_flag)
      {
        db_dvbs_edit_program(&prog);
        db_dvbs_save_pg_edit(&prog);
      }
    }
    table_monitor_edit_logic_number(TRUE);
    p_priv->priv.pg_edit_ticks = mtos_ticks_get();
  }
  ui_restore_view();
}

static void table_monitor_sdt_found(tm_handle_t *p_priv, os_msg_t *p_msg)
{
  sdt_t *p_sdt = (sdt_t *)p_msg->para1;
  u32 i = 0, j = 0;
  BOOL already_exist = FALSE;

  if(p_priv->priv.bSdtOtrAvailable == TRUE)
  {
    for(i = 0; i < p_sdt->svc_count; i++)
    {
      for(j = 0; j < p_priv->priv.pg_num; j ++)
      {
        if(p_priv->priv.total_pg_list[j].s_id == p_sdt->svc_des[i].svc_id)
        {
          already_exist = TRUE;
        }
      }
      if(already_exist == TRUE)
      {
        already_exist = FALSE;
        continue;
      }
      if(p_sdt->stream_id == 0)
      {
        continue;
      }
      
      if (p_sdt->svc_des[i].service_type == DVB_NVOD_TIME_SHIFTED_SVC
        || p_sdt->svc_des[i].service_type == 0x19)
      {
        continue;
      }

      p_priv->priv.total_pg_list[p_priv->priv.pg_num].s_id = p_sdt->svc_des[i].svc_id;
      p_priv->priv.total_pg_list[p_priv->priv.pg_num].orig_net_id = p_sdt->org_network_id;
      p_priv->priv.total_pg_list[p_priv->priv.pg_num].is_scrambled = p_sdt->svc_des[i].is_scrambled;
      p_priv->priv.total_pg_list[p_priv->priv.pg_num].ca_system_id = p_sdt->svc_des[i].ca_system_id;
      p_priv->priv.total_pg_list[p_priv->priv.pg_num].ts_id = p_sdt->stream_id;
      p_priv->priv.total_pg_list[p_priv->priv.pg_num].service_type = p_sdt->svc_des[i].service_type;
      if (p_sdt->svc_des[i].mosaic_des_found
    || p_sdt->svc_des[i].logic_screen_des_found
    || p_sdt->svc_des[i].linkage_des_found)
      {
        memcpy(&p_priv->priv.total_pg_list[p_priv->priv.pg_num].mosaic, (mosaic_t *)p_sdt->svc_des[i].p_nvod_mosaic_des, sizeof(mosaic_t));
      }
      else
      {
        p_priv->priv.total_pg_list[p_priv->priv.pg_num].mosaic.lg_cell_cnt = 0;
      }
      memcpy(p_priv->priv.total_pg_list[p_priv->priv.pg_num].pg_name, p_sdt->svc_des[i].name, MAX_SVC_NAME_LEN);
      p_priv->priv.pg_num ++;
    }
    if(p_priv->priv.bSdtOtrAvailable == TRUE)
    {
      table_monitor_sdt_add_remove_pg(p_priv);
    }
  }
  table_monitor_free_sdt_otr(p_priv);
}

static void table_monitor_sdt_otr_found(tm_handle_t *p_priv, os_msg_t *p_msg)
{
  sdt_t *p_sdt = (sdt_t *)p_msg->para1;
  tp_svc_list_t *p_cur_svc_list = p_priv->priv.tp_svc_list;
  u16 svc_list_cnt = 0;
  dvb_request_t param = {0};
  service_t *p_dvb_svc = p_priv->priv.p_dvb_svc;

  if(p_priv->priv.bSdtOtrAvailable == TRUE)
  {
    for(svc_list_cnt = 0; svc_list_cnt < p_priv->priv.total_svc_found_list; svc_list_cnt++)
    {
      p_cur_svc_list = p_priv->priv.tp_svc_list + svc_list_cnt;
      if(p_priv->priv.sdt_actual_ts_id != 0)
      {
        if(p_cur_svc_list->ts_id == p_priv->priv.sdt_actual_ts_id && p_priv->priv.is_sdt_otr_recv[svc_list_cnt] == FALSE)
        {
          p_priv->priv.is_sdt_otr_recv[svc_list_cnt] = TRUE;
        }
      }
      if(p_cur_svc_list->ts_id == p_sdt->stream_id && p_priv->priv.is_sdt_otr_recv[svc_list_cnt] == FALSE)
      {
        p_priv->priv.is_sdt_otr_recv[svc_list_cnt] = TRUE;
        add_pg_list(p_priv, p_sdt);
      }
    }
    if(dvbc_is_need_sdt_otr(p_priv) == TRUE)
    {
      // Request SDT other
      param.req_mode  = DATA_SINGLE;
      param.table_id  = DVB_TABLE_ID_SDT_OTHER;
      param.para1     = DVB_TABLE_ID_SDT_OTHER;
      p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST,
        (u32)&param, sizeof(dvb_request_t));
    }
    else
    {
      // Request SDT actual
      param.req_mode  = DATA_SINGLE;
      param.table_id  = DVB_TABLE_ID_SDT_ACTUAL;
      param.para1     = DVB_TABLE_ID_SDT_ACTUAL;
      p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST,
        (u32)&param, sizeof(dvb_request_t));
    }
  }
  else
  {
    table_monitor_free_sdt_otr(p_priv);
  }
}
#endif
static void table_monitor_timedout(tm_handle_t *p_priv, os_msg_t *p_msg)
{
  tm_handle_t *p_this = (tm_handle_t *)p_priv;
  OS_PRINTF("time out table 0x%x, module %d\n",
  p_msg->para1, p_msg->para2);
  switch(p_msg->para1)
  {
    case DVB_TABLE_ID_NIT_ACTUAL:
      OS_PRINTF("DVB_TABLE_ID_NIT_ACTUAL\n");
      break;
    case DVB_TABLE_ID_SDT_ACTUAL:
      OS_PRINTF("DVB_TABLE_ID_SDT_ACTUAL\n");
      break;
    case DVB_TABLE_ID_BAT:
      OS_PRINTF("DVB_TABLE_ID_BAT\n");
	  table_monitor_scan_free_bat(p_this);
      break;
    case DVB_SDT_OTR_FOUND:
      OS_PRINTF("DVB_SDT_OTR_FOUND\n");
      table_monitor_scan_free_bat(p_this);
    default:
      break;
  }
}

extern void clean_lcn_info(void);
static void process_reset_data(handle_t handle, m_table_id_t t_id, m_svc_event_t event_tyoe, m_data_type_t type)
{
  tm_handle_t *p_this = (tm_handle_t *)handle;
  m_svc_t *p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);
  m_data_param_t param = {0};
  sdt_t sdt = {0};
  bat_t bat = {0};
  nit_t nit = {0};
  dvbs_tp_node_t tp_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  u16 cur_pg_id = 0;
  u32 para2 = 0;
  param.ap_id = APP_RESERVED2;
  param.t_id = t_id;
  param.type = type;
  
  memset(&sdt, 0, sizeof(sdt_t));
  memset(&bat, 0, sizeof(bat_t));
  memset(&nit, 0, sizeof(nit_t));
  
  switch(t_id)
  {
    case M_PMT_TABLE:
      OS_PRINTF("M_PMT_TABLE\n");
      break;
    case M_SDT_TABLE:
      OS_PRINTF("M_SDT_TABLE\n");
      p_this->priv.start_sdt_tickets = mtos_ticks_get();
      if(event_tyoe == M_SVC_VER_CHANGED)
      {
        param.p_buffer = p_this->p_buffer;
        if (dvb_monitor_data_get(p_svc, &param))
        {
          table_monitor_parse_sdt(param.p_buffer, &sdt);
          cur_pg_id = sys_status_get_curn_group_curn_prog_id();
          ret = db_dvbs_get_pg_by_id(cur_pg_id, &pg);
          if(DB_DVBS_OK != ret)
          {
            break;
          }
          ret = db_dvbs_get_tp_by_id(pg.tp_id, &tp_node);
          
          if(DB_DVBS_OK != ret)
          {
            break;
          }
          else
          {
            //OS_PRINTF("1111tp_node.sdt_version [%d], sdt.version_num [%d] \n", tp_node.sdt_version, sdt.version_num);
            if(tp_node.sdt_version != sdt.version_num && pg.ts_id == sdt.stream_id)
            {
              ui_cache_view();
              table_monitor_edit_pg(p_this, &sdt, pg.tp_id);
              tp_node.sdt_version = sdt.version_num;
              db_dvbs_edit_tp(&tp_node);
              db_dvbs_save_tp_edit(&tp_node);
              ui_restore_view();
            }
          }
#ifdef QUICK_SEARCH_SUPPORT
          p_this->priv.sdt_actual_ts_id = sdt.stream_id;
#endif
        }
      }
      else
      {
        param.p_buffer = p_this->p_buffer;
        if (dvb_monitor_data_get(p_svc, &param))
        {
          table_monitor_parse_sdt(param.p_buffer, &sdt);
          cur_pg_id = sys_status_get_curn_group_curn_prog_id();
          ret = db_dvbs_get_pg_by_id(cur_pg_id, &pg);
          if(DB_DVBS_OK != ret)
          {
            break;
          }
          ret = db_dvbs_get_tp_by_id(pg.tp_id, &tp_node);
          
          //OS_PRINTF("2222tp_node.sdt_version [%d], sdt.version_num [%d] \n", tp_node.sdt_version, sdt.version_num);
          
          if(DB_DVBS_OK != ret)
          {
            break;
          }
          else if(tp_node.sdt_version != sdt.version_num && pg.ts_id == sdt.stream_id)
          {
            ui_cache_view();
            table_monitor_edit_pg(p_this, &sdt, pg.tp_id);
            tp_node.sdt_version = sdt.version_num;
            db_dvbs_edit_tp(&tp_node);
            db_dvbs_save_tp_edit(&tp_node);
            ui_restore_view();
          }
#ifdef QUICK_SEARCH_SUPPORT
          p_this->priv.sdt_actual_ts_id = sdt.stream_id;
#endif
        }
        table_monitor_edit_logic_number(FALSE);
      }
      break;
    case M_PAT_TABLE:
      OS_PRINTF("M_PAT_TABLE\n");
      break;
    case M_NIT_TABLE:
      OS_PRINTF("M_NIT_TABLE\n");
      param.p_buffer = p_this->p_buffer + SECTION_LENGTH;
      if(event_tyoe == M_SVC_VER_CHANGED)
        clean_lcn_info();
      if (dvb_monitor_data_get(p_svc, &param))
      {
        table_monitor_parse_lcn(param.p_buffer, &nit);
      }
#ifdef QUICK_SEARCH_SUPPORT
      {
        memset(p_this->priv.is_nit_recv, 0 , MAX_TP_NUM * sizeof(BOOL));
        p_this->priv.total_svc_found_list = 0;
        memset(p_this->priv.tp_svc_list, 0, sizeof(tp_svc_list_t) *MAX_TP_NUM );
        add_svc_list(p_this, &nit);
        p_this->priv.bSdtOtrAvailable = FALSE;
        p_this->priv.is_nit_recv[nit.sec_number] = TRUE;
      }
#endif
      if (nit.sec_number < nit.last_sec_number)
      {
        para2 = ((nit.sec_number + 1) << 16) | 0xFFFF;
        table_monitor_nit_request(p_this, para2);
      }
#ifdef QUICK_SEARCH_SUPPORT
      else if(nit.sec_number == nit.last_sec_number)
      {
        dvb_request_t param = {0};
        service_t *p_dvb_svc = p_this->priv.p_dvb_svc;
        u8 i = 0;
        BOOL bNit = TRUE;
        for(i = 0; i <= nit.last_sec_number; i++)
        {
          if(p_this->priv.is_nit_recv[i] == FALSE)
          {
            bNit = FALSE;
            break;
          }
        }
        if(bNit == TRUE
          &&( mtos_ticks_get() - p_this->priv.pg_edit_ticks > 12000 || p_this->priv.pg_edit_ticks == 0))
        {
            // Request SDT other
          param.req_mode  = DATA_SINGLE;
          param.table_id  = DVB_TABLE_ID_SDT_OTHER;
          param.para1     = DVB_TABLE_ID_SDT_OTHER;
          p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST,
            (u32)&param, sizeof(dvb_request_t));
          p_this->priv.sdt_otr_start_ticks = mtos_ticks_get();
          p_this->priv.pg_num = 0;
          memset(p_this->priv.total_pg_list, 0 , sizeof(pg_info_t) * MAX_BROG_NUM_IN_SDT_OTR);
          memset(p_this->priv.is_sdt_otr_recv, 0 , MAX_TP_NUM * sizeof(BOOL));
          memset(p_this->priv.is_nit_recv, 0 , MAX_TP_NUM * sizeof(BOOL));
          p_this->priv.bSdtOtrAvailable = TRUE;
        }
        else
        {
          table_monitor_free_sdt_otr(p_this);
        }
      }
#endif
      break;
    case M_BAT_TABLE:
      OS_PRINTF("M_BAT_TABLE\n");
#if 0
      param.p_buffer = p_this->p_buffer + 4 * SECTION_LENGTH;
      if (dvb_monitor_data_get(p_svc, &param))
      {
        table_monitor_parse_bat(param.p_buffer,&bat);
      }
#endif

      if(event_tyoe == M_SVC_VER_CHANGED && p_this->priv.bat_multi_mode == FALSE)
      {
        //table_monitor_bat_request(p_this);
      }
      break;
    default:
      break;
  }
}

static void process_normal_data(handle_t handle, m_table_id_t t_id, m_svc_event_t event_tyoe, m_data_type_t type)
{
  
}

static void tm_process_msg(void *p_handle, os_msg_t *p_msg)
{
  tm_handle_t *p_this = (tm_handle_t *)p_handle;
  //class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  switch(p_msg->content)
  {
    case M_SVC_DATA_RESET:
      OS_PRINTF("M_SVC_DATA_RESET\n");
      process_reset_data(p_this, p_msg->para1, p_msg->content, M_SECTION_DATA);
      break;
    case M_SVC_VER_CHANGED:
      OS_PRINTF("M_SVC_VER_CHANGED\n");
      process_reset_data(p_this, p_msg->para1, p_msg->content, M_SECTION_DATA);
      break;
    case M_SVC_DATA_READY:
      OS_PRINTF("M_SVC_DATA_READY\n");
      process_normal_data(p_this, p_msg->para1, p_msg->content, M_SECTION_DATA);
      break;
    case DVB_PAT_FOUND:
      OS_PRINTF("DVB_PAT_FOUND\n");
      break;
    case DVB_PAT_PMT_INFO:
      OS_PRINTF("DVB_PAT_PMT_INFO\n");
      table_monitor_pmt_request(p_this, p_msg);
      break;
    case DVB_PMT_FOUND:
      OS_PRINTF("DVB_PMT_FOUND\n");
      table_monitor_pmt_found(p_this, p_msg);
      break;
    case DVB_NIT_FOUND:
      OS_PRINTF("DVB_NIT_FOUND\n");
      table_monitor_nit_found(p_this, p_msg);
      break;
    case DVB_BAT_FOUND:
      OS_PRINTF("DVB_BAT_FOUND\n");
      table_monitor_bat_found(p_this, p_msg);
      break;
#ifdef QUICK_SEARCH_SUPPORT
    case DVB_SDT_FOUND:
      OS_PRINTF("DVB_SDT_OTR_FOUND\n");
      table_monitor_sdt_found(p_this, p_msg);
      break;      
    case DVB_SDT_OTR_FOUND:
      OS_PRINTF("DVB_SDT_OTR_FOUND\n");
      table_monitor_sdt_otr_found(p_this, p_msg);
      break;
#endif      
    case DVB_TABLE_TIMED_OUT:
      //OS_PRINTF("DVB_TABLE_TIMED_OUT\n");
      table_monitor_timedout(p_this, p_msg);
      break;
    case TM_CMD_STOP_BAT:
    case TM_CMD_SYNC_STOP_BAT:
      table_monitor_scan_free_bat(p_this);
      if(p_msg->content == TM_CMD_SYNC_STOP_BAT)
      {
        send_msg_to_ui(TM_BAT_STOPED, 0, 0);
      }
	    break;
    case TM_CMD_ABLE_BAT:
    case TM_CMD_SYNC_ABLE_BAT:
      {
        BOOL b = (BOOL)(p_msg->para1);
        if(!b)
        {
          OS_PRINTF("TM_CMD_ABLE_BAT disable\n");
          table_monitor_scan_free_bat(p_this);
          p_this->priv.bBatAvailable = FALSE;
        }
        else
        {
          OS_PRINTF("TM_CMD_ABLE_BAT enable\n");
          p_this->priv.bBatAvailable = TRUE;
        }

        if(p_msg->content == TM_CMD_SYNC_ABLE_BAT)
        {
          send_msg_to_ui(TM_BAT_ABLED, 0, 0);
        }
      }
	    break;
#ifdef QUICK_SEARCH_SUPPORT
    case TM_CMD_STOP_SDT_OTR:
    case TM_CMD_SYNC_STOP_SDT_OTR:
       p_this->priv.bSdtOtrAvailable = FALSE;
      if(p_msg->content == TM_CMD_SYNC_STOP_SDT_OTR)
      {
        send_msg_to_ui(TM_SDT_OTR_STOPED, 0, 0);
      }
	    break;
    case TM_CMD_ABLE_SDT_OTR:
    case TM_CMD_SYNC_ABLE_SDT_OTR:
      {
        BOOL b = (BOOL)(p_msg->para1);
        if(!b)
        {
          OS_PRINTF("TM_CMD_ABLE_SDT_OTR disable\n");
          p_this->priv.bSdtOtrAvailable = FALSE;
        }
        else
        {
          OS_PRINTF("TM_CMD_ABLE_SDT_OTR enable\n");
          p_this->priv.bSdtOtrAvailable = TRUE;
        }

        if(p_msg->content == TM_CMD_SYNC_ABLE_SDT_OTR)
        {
          send_msg_to_ui(TM_SDT_OTR_ABLED, 0, 0);
        }
      }
	    break;
#endif
    case TM_CMD_START_SCAN:
      #if 1
      {
        nc_channel_info_t search_info = {0};
        search_info.polarization = 0;
        search_info.channel_info.frequency = 3140000;
        search_info.channel_info.param.dvbc.symbol_rate = 6900;
        search_info.channel_info.param.dvbc.modulation = 6;
        //nc_set_tp(nc_handle, p_this->priv->p_nc_svc, &search_info);
        //p_this->priv->state = TM_SCAN_WAIT_TP_LOCK;
      }
      #endif
      break;
    default:
      break;
  }
}

static void tm_state_machine(void *p_handle)
{
  static u32 start_tickets = 0;
  tm_handle_t *p_priv = (tm_handle_t *)p_handle;

  switch(p_priv->priv.state)
  {
  case TM_IDLE:    
    if(p_priv->priv.bat_multi_mode == TRUE)
    {
      if(mtos_ticks_get() - p_priv->priv.start_bat_tickets
          > 100*8)
      {
        OS_PRINTF("tm_state_machine BAT timeout \n");

        table_monitor_scan_free_bat(p_priv);
      }
    }
    else
    {
      if(mtos_ticks_get() - start_tickets >= 12000
        && p_priv->priv.bBatAvailable) 
      {
        OS_PRINTF("tm_state_machine start BAT \n");
        table_monitor_bat_request(p_priv);
        start_tickets = mtos_ticks_get();
      }
    }

    if(mtos_ticks_get() - p_priv->priv.start_sdt_tickets >= 6000)
    {
      OS_PRINTF("tm_state_machine M_SVC_VER_CHANGED\n");
      process_reset_data(p_priv, M_SDT_TABLE, M_SVC_VER_CHANGED, M_SECTION_DATA);
    }
    break;
    
  default:
    break;
  }

}

static void register_monitor_table(void)
{
  m_register_t reg;
  m_svc_t *p_svc = NULL;

  p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);
  reg.app_id = APP_RESERVED2;
  reg.num = 0;
  #if 0
  reg.table[reg.num].t_id = M_PMT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;
  #endif
  reg.table[reg.num].t_id = M_SDT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;
  #if 0
  reg.table[reg.num].t_id = M_PAT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;
  #endif
  reg.table[reg.num].t_id = M_NIT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;
  #if 0
  reg.table[reg.num].t_id = M_BAT_TABLE;
  reg.table[reg.num].period = 500;
  //reg.table[reg.num].request_proc = NULL;
  reg.table[reg.num].parse_proc = NULL;
  reg.num++;
  #endif
  dvb_monitor_register_table(p_svc, &reg);

}

/*!
   The init operation will be invoked once when system power up
  */
static void init(void *p_handle)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  tm_handle_t *p_priv = (tm_handle_t *)p_handle;
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  register_monitor_table();
  MT_ASSERT(p_priv != NULL);
  p_priv->priv.p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_RESERVED2);
  p_priv->p_buffer = mtos_malloc(MAX_TABLE_NUM * SECTION_LENGTH);
  MT_ASSERT(NULL != p_priv->p_buffer);
  p_priv->priv.bBatAvailable = TRUE;
  p_priv->priv.p_nc_svc = nc_get_svc_instance(nc_handle, APP_RESERVED2);
}

app_t *construct_ap_table_monitor()
{
  tm_handle_t *p_priv = mtos_malloc(sizeof(tm_handle_t));
  MT_ASSERT(NULL != p_priv);
  memset(p_priv, 0, sizeof(tm_handle_t));
  memset(&(p_priv->instance), 0, sizeof(app_t));
  
  p_priv->instance.init = init;
  p_priv->instance.process_msg = tm_process_msg;
  p_priv->instance.state_machine = tm_state_machine;
  p_priv->instance.p_data = (void *)p_priv;
  return &p_priv->instance;
}

void ui_table_monitor_stop_bat(BOOL b_able, BOOL b_sync)
{
  static BOOL b_stoped_bat = TRUE;
  cmd_t cmd = {0};

  if(b_able == b_stoped_bat)
  {
    return;
  }
  else
  {
    b_stoped_bat = b_able;
  }
  
  cmd.id = b_sync? TM_CMD_SYNC_STOP_BAT : TM_CMD_STOP_BAT;
  cmd.data1 = b_able? 1 : 0;
  
  ap_frm_do_command(APP_RESERVED2, &cmd);
}

void ui_table_monitor_able_bat(BOOL b_able, BOOL b_sync)
{
  static BOOL b_able_bat = TRUE;
  cmd_t cmd = {0};

  if(b_able == b_able_bat)
  {
    return;
  }
  else
  {
    b_able_bat = b_able;
  }

  cmd.id = b_sync? TM_CMD_SYNC_ABLE_BAT : TM_CMD_ABLE_BAT;
  cmd.data1 = b_able? 1 : 0;
  
  ap_frm_do_command(APP_RESERVED2, &cmd);
}

void ui_table_monitor_stop_sdt_otr(BOOL b_able, BOOL b_sync)
{
  static BOOL b_stoped_sdt = TRUE;
  cmd_t cmd = {0};

  if(b_able == b_stoped_sdt)
  {
    return;
  }
  else
  {
    b_stoped_sdt = b_able;
  }

  cmd.id = b_sync? TM_CMD_SYNC_STOP_SDT_OTR : TM_CMD_STOP_SDT_OTR;
  cmd.data1 = b_able? 1 : 0;
  
  ap_frm_do_command(APP_RESERVED2, &cmd);
}

void ui_table_monitor_able_sdt_otr(BOOL b_able, BOOL b_sync)
{
  static BOOL b_able_sdt = TRUE;
  cmd_t cmd = {0};

  if(b_able == b_able_sdt)
  {
    return;
  }
  else
  {
    b_able_sdt = b_able;
  }

  cmd.id = b_sync? TM_CMD_SYNC_ABLE_SDT_OTR : TM_CMD_ABLE_SDT_OTR;
  cmd.data1 = b_able? 1 : 0;
  
  ap_frm_do_command(APP_RESERVED2, &cmd);
}

void ui_table_monitor_init(void)
{
  cmd_t cmd;
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_RESERVED2;
  cmd.data2 = 0;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  fw_register_ap_evtmap(APP_RESERVED2, ui_table_mon_evtmap);
  fw_register_ap_msghost(APP_RESERVED2, ROOT_ID_BACKGROUND);
}

void ui_table_monitor_release(void)
{
  cmd_t cmd = {0};
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_RESERVED2;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_unregister_ap_evtmap(APP_RESERVED2);
  fw_unregister_ap_msghost(APP_RESERVED2, ROOT_ID_BACKGROUND); 
}

BEGIN_AP_EVTMAP(ui_table_mon_evtmap)
END_AP_EVTMAP(ui_table_mon_evtmap)

//end of file
