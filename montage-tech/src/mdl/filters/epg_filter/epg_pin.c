/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
//std
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_task.h"

//util
#include "lib_util.h"
#include "dvb_protocol.h"
#include "fcrc.h"

//driver
#include "dmx.h"


//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"

#include "ifilter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"

#include "demux_interface.h"
#include "demux_filter.h"
#include "epg_type.h"
#include "epg_filter.h"
#include "epg_database.h"
#include "epg_pin_intra.h"
#include "err_check_def.h"

//#ifdef PRINT_ON
#define EP_DEBUG
//#endif

#ifdef  EP_DEBUG
#define EP_PRINT  OS_PRINTF("%s, %d liens : ",__FUNCTION__, __LINE__); OS_PRINTF
#else
#define EP_PRINT  DUMMY_PRINTF
#endif

/*!
 header len + crc len
 */
#define EIT_MIN_LEN  (15)

static epg_pin_priv_t * epg_pin_get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(_this != NULL);
  return &(((epg_pin_t *)_this)->m_priv_data);
}

static void epg_pin_report(handle_t _this, void *p_state)
{

}

static void _epg_pin_request(epg_pin_priv_t *p_priv,
                             u8              table_id,
                             u16             network_id,
                             u16             ts_id,
                             u16             svc_id,
                             u8             *p_buffer)
{

  CHECK_FAIL_RET_VOID(p_buffer != NULL);

  memset(&p_priv->req_data,0,sizeof(psi_request_data_t));

  p_priv->req_data.media_type = MF_EIT;
  p_priv->req_data.table_id   = table_id;
  p_priv->req_data.req_mode   = DMX_DATA_MULTI;
  p_priv->req_data.pid        = DVB_EIT_PID;
  p_priv->req_data.timeout    = p_priv->eit_timeout; //EIT_TIMEOUT;
  p_priv->req_data.slot_type  = DMX_CH_TYPE_SECTION;
  p_priv->req_data.enable_crc = TRUE;
  p_priv->req_data.p_ext_data = p_buffer;
  p_priv->req_data.ext_data_size   = p_priv->spin_multi_buffer_size;
  p_priv->req_data.filter_value[0] = table_id;
  p_priv->req_data.filter_mask[0]  = 0xFF;

#ifdef WIN32
 /*!
  set service id.
  */
  if(svc_id != EPG_IGNORE_ID)
  {
    p_priv->req_data.filter_value[3] = (svc_id >> 8) & 0XFF;
    p_priv->req_data.filter_mask[3]  = 0XFF;
    p_priv->req_data.filter_value[4] = svc_id & 0XFF;
    p_priv->req_data.filter_mask[4]  = 0XFF;
  }

 /*!
  set transport stream id.
  */
  if(ts_id != EPG_IGNORE_ID)
  {
    p_priv->req_data.filter_value[8] = (ts_id >> 8) & 0XFF;
    p_priv->req_data.filter_mask[8]  = 0XFF;
    p_priv->req_data.filter_value[9] = (ts_id) & 0XFF;
    p_priv->req_data.filter_mask[9]  = 0XFF;
  }

 /*!
  set network id.
  */
  if(network_id != EPG_IGNORE_ID)
  {
    p_priv->req_data.filter_value[10] = (network_id >> 8) & 0XFF;
    p_priv->req_data.filter_mask[10]  = 0XFF;
    p_priv->req_data.filter_value[11] = (network_id) & 0XFF;
    p_priv->req_data.filter_mask[11]  = 0XFF;
  }
#else
 /*!
  set service id.
  */
  if(svc_id  != EPG_IGNORE_ID)
  {
    p_priv->req_data.filter_value[1] = (svc_id  >> 8) & 0XFF;
    p_priv->req_data.filter_mask[1]  = 0XFF;
    p_priv->req_data.filter_value[2] = svc_id  & 0XFF;
    p_priv->req_data.filter_mask[2]  = 0XFF;
  }

 /*!
  set transport stream id.
  */
  if(ts_id != EPG_IGNORE_ID)
  {
    p_priv->req_data.filter_value[6] = (ts_id >> 8) & 0XFF;
    p_priv->req_data.filter_mask[6]  = 0XFF;
    p_priv->req_data.filter_value[7] = (ts_id) & 0XFF;
    p_priv->req_data.filter_mask[7]  = 0XFF;
  }

  /*!
  set network id.
  */
  if(network_id != EPG_IGNORE_ID)
  {
    p_priv->req_data.filter_value[8] = (network_id >> 8) & 0XFF;
    p_priv->req_data.filter_mask[8]  = 0XFF;
    p_priv->req_data.filter_value[9] = (network_id) & 0XFF;
    p_priv->req_data.filter_mask[9]  = 0XFF;
  }
#endif

  p_priv->p_dmx_inter->i_request(p_priv->p_dmx_inter, &p_priv->req_data);
}

static void _epg_pin_free(epg_pin_priv_t *p_priv, u8 table_id)
{
  
  CHECK_FAIL_RET_VOID(p_priv != NULL);
  memset(&p_priv->free_data,0,sizeof(psi_free_data_t));
  //free dmx resource.
  p_priv->free_data.media_type = MF_EIT;
  p_priv->free_data.req_mode   = DMX_DATA_MULTI;
  p_priv->free_data.table_id   = table_id;
  p_priv->free_data.psi_pid    = DVB_EIT_PID;
 
  p_priv->p_dmx_inter->i_free(p_priv->p_dmx_inter,
                              &p_priv->free_data);
}

/*!
 request P/F eit table
 */
static void _epg_pin_request_pf_eit(epg_pin_priv_t *p_priv, epg_prog_info_t *p_info)
{
  u8   index      = 0;
  BOOL b_request  = FALSE;
  u16  network_id = EPG_IGNORE_ID;
  u16  ts_id      = EPG_IGNORE_ID;
  u16  svc_id     = EPG_IGNORE_ID;

  if(p_priv->pf_array_depth == 0)
    return;

  if(p_info != NULL)
  {
    switch(p_priv->pf_policy)
    {
    case PF_EVENT_ALL:
      break;

    case PF_EVENT_OF_NETWORK_ID :
      if(p_info->network_id != p_priv->prog_info.network_id)
      {
        network_id = p_info->network_id;
        b_request = TRUE;
      }
      break;

    case PF_EVENT_OF_TS_ID :
      if((p_info->network_id != p_priv->prog_info.network_id) 
        || (p_info->ts_id != p_priv->prog_info.ts_id))
      {
        network_id = p_info->network_id;
        ts_id      = p_info->ts_id;        
        b_request = TRUE;
      }
      break;

    case PF_EVENT_OF_SVC_ID :
      if((p_info->network_id != p_priv->prog_info.network_id) 
        || (p_info->ts_id != p_priv->prog_info.ts_id)
        || (p_info->svc_id != p_priv->prog_info.svc_id))
      {
        network_id = p_info->network_id;
        ts_id      = p_info->ts_id;
        svc_id     = p_info->svc_id;        
        b_request  = TRUE;
      }
      break;

    default :
      EP_PRINT("pf policy %d\n", p_priv->pf_policy);
      CHECK_FAIL_RET_VOID(0);
      break;
    }

    if(!b_request)
      return;  
      
    for(index = 0; index < p_priv->pf_array_depth; index++)
    {
     CHECK_FAIL_RET_VOID(p_priv->p_pf_array[index].free_flag == FALSE);
      _epg_pin_free(p_priv, p_priv->p_pf_array[index].table_id);
      p_priv->p_pf_array[index].free_flag = TRUE;
    }
  }
  else
  {
    switch(p_priv->pf_policy)
    {
    case PF_EVENT_ALL:
      break;
    case PF_EVENT_OF_NETWORK_ID :
        network_id = p_priv->prog_info.network_id;
      break;
    case PF_EVENT_OF_TS_ID :
        network_id = p_priv->prog_info.network_id;
        ts_id      = p_priv->prog_info.ts_id;        
      break;
    case PF_EVENT_OF_SVC_ID :
        network_id = p_priv->prog_info.network_id;
        ts_id      = p_priv->prog_info.ts_id;
        svc_id     = p_priv->prog_info.svc_id;        
      break;
    default :
      EP_PRINT("pf policy %d\n", p_priv->pf_policy);
      CHECK_FAIL_RET_VOID(0);
      break;
    }
  }

  for(index = 0; index < p_priv->pf_array_depth; index++)
  {
    CHECK_FAIL_RET_VOID(p_priv->p_pf_array[index].free_flag == TRUE);
    OS_PRINTF("###debug epg_pin_request_pf_eit[0x%x] filter:netid=0x%d,tsid=0x%d,sid=%d\n",
                            p_priv->p_pf_array[index].table_id,network_id,ts_id,svc_id);
    _epg_pin_request(p_priv,
                         p_priv->p_pf_array[index].table_id,
                         network_id,
                         ts_id,
                         svc_id,
                         p_priv->p_pf_array[index].p_buffer);
    p_priv->p_pf_array[index].free_flag = FALSE;
  }
 /*!
  *delete old P/F event.
  */
  //epg_db_delete(EPG_ALL_PF_EVENT);
}

static void _epg_pin_request_sch_eit(epg_pin_priv_t *p_priv, epg_prog_info_t *p_info)
{
  u8   index      = 0;
  BOOL b_request  = FALSE;
  u16  network_id = EPG_IGNORE_ID;
  u16  ts_id      = EPG_IGNORE_ID;
  u16  svc_id     = EPG_IGNORE_ID;

  if(p_priv->sch_array_depth == 0)
    return;
  if(p_info != NULL)
  {
    switch(p_priv->sch_policy)
    {
    case SCH_EVENT_ALL:
      break;

    case SCH_EVENT_OF_NETWORK_ID :
      if(p_info->network_id != p_priv->prog_info.network_id)
      {
        network_id = p_info->network_id;        
        b_request  = TRUE;
      }
      break;

    case SCH_EVENT_OF_TS_ID :
      if((p_info->network_id != p_priv->prog_info.network_id) 
        || (p_info->ts_id != p_priv->prog_info.ts_id))
      {
        network_id = p_info->network_id;
        ts_id      = p_info->ts_id;        
        b_request  = TRUE;
      }
      break;

    case SCH_EVENT_OF_SVC_ID :
      if((p_info->network_id != p_priv->prog_info.network_id) 
        || (p_info->ts_id != p_priv->prog_info.ts_id)
        || (p_info->svc_id != p_priv->prog_info.svc_id))
      {
        network_id = p_info->network_id;
        ts_id      = p_info->ts_id;
        svc_id     = p_info->svc_id;
        b_request  = TRUE;
      }
      break;

    default :
      EP_PRINT("schedule policy %d\n", p_priv->sch_policy);
      CHECK_FAIL_RET_VOID(0);
      break;
    }
    
    if(!b_request)
      return;
    for(index = 0; index < p_priv->sch_array_depth; index++)
    {
      CHECK_FAIL_RET_VOID(p_priv->p_sch_array[index].free_flag == FALSE);
      _epg_pin_free(p_priv, p_priv->p_sch_array[index].table_id);
      p_priv->p_sch_array[index].free_flag = TRUE;
    }
  }
else
  {
    switch(p_priv->sch_policy)
    {
      case SCH_EVENT_ALL:
        break;
      case SCH_EVENT_OF_NETWORK_ID :
          network_id = p_priv->prog_info.network_id;
        break;
      case SCH_EVENT_OF_TS_ID :
          network_id = p_priv->prog_info.network_id;
          ts_id      = p_priv->prog_info.ts_id;        
        break;
      case SCH_EVENT_OF_SVC_ID :
          network_id = p_priv->prog_info.network_id;
          ts_id      = p_priv->prog_info.ts_id;
          svc_id     = p_priv->prog_info.svc_id;        
        break;
      default :
        EP_PRINT("pf policy %d\n", p_priv->pf_policy);
        CHECK_FAIL_RET_VOID(0);
      break;
    }
  }

  /*!
   *request all schedule eit table
   */
  for(index = 0; index < p_priv->sch_array_depth; index++)
  {
     CHECK_FAIL_RET_VOID(p_priv->p_sch_array[index].free_flag == TRUE);
     OS_PRINTF("###debug epg_pin_request_sch_eit[0x%x] filter:netid=0x%d,tsid=0x%d,sid=%d\n",
                            p_priv->p_sch_array[index].table_id,network_id,ts_id,svc_id);
    _epg_pin_request(p_priv,
                         p_priv->p_sch_array[index].table_id,
                         network_id,
                         ts_id,
                         svc_id,
                         p_priv->p_sch_array[index].p_buffer);
    p_priv->p_sch_array[index].free_flag = FALSE;
  }

 /*!
  *delete old schedule event.
  */
 // epg_db_delete(EPG_SCHEDULE_EVENT);
}
/*
static void _epg_pin_free_eit(epg_pin_priv_t *p_priv)
{
  u8 index = 0;
}
*/
#if 0
static u32 test_ticks = 0;
static u32 test_pf_ac_times = 0;
static u32 test_pf_ot_times = 0;
static u32 test_other_times = 0;
void test_table_times(u8 table_id)
{
   if((mtos_ticks_get() - test_ticks) > 5 * 100)
  {
    OS_PRINTF("@#$ test pf table pf atcue:%d *1s,pf other: %d *1s,sch: %d *1s \n",
                  test_pf_ac_times / 5,
                  test_pf_ot_times / 5,
                  test_other_times / 5);
    test_ticks = mtos_ticks_get();
    test_pf_ac_times = 0;
    test_pf_ot_times = 0;
    test_other_times = 0;
  }
   if(table_id == EIT_TABLE_ID_PF_ACTUAL)
  {
   test_pf_ac_times ++;
  }
  else  if(table_id == EIT_TABLE_ID_PF_OTHER)
  {
     test_pf_ot_times ++;
  }
  else
  {
    test_other_times ++;
  }
   
}
#endif
static BOOL _epg_pin_parse_sec_header(epg_pin_priv_t *p_priv,
                                      u8             *p_buf,
                                      u32             buf_size,
                                      s32            *p_sec_len)
{
  epg_eit_t *p_eit = &p_priv->epg_eit;
  u32 sec_len = 0;
  u32 crc_check = 0;

  CHECK_FAIL_RET_FALSE(p_buf     != NULL);
  CHECK_FAIL_RET_FALSE(p_eit != NULL);
  CHECK_FAIL_RET_FALSE(p_sec_len != NULL);

  p_eit->table_id = p_buf[0];
  
  if((p_eit->table_id == EIT_TABLE_ID_PF_ACTUAL)
  || (p_eit->table_id == EIT_TABLE_ID_PF_OTHER))
  {
    p_eit->pf_table = 1;
  }
  else
  {
    p_eit->pf_table = 0;
  }
  
 /*!
  The section_syntax_indicator is a 1-bit field which shall be set to "1".
  current_next_indicator : must set to "1".
  */
  if(((p_buf[1] & 0x80) == 0) ||
    ((p_buf[5] & 0x01) == 0))
  {
    return FALSE;
  }

  //starting immediately following the section_length field and including the CRC.
  sec_len = MAKE_WORD(p_buf[2], (p_buf[1] & 0x0f));
  if(((sec_len) > 4093) || ((sec_len) < EIT_MIN_LEN))
  {
    return FALSE;
  }
  *p_sec_len = sec_len;

  p_eit->svc_id = MAKE_WORD2(p_buf[3], p_buf[4]);
  p_eit->version_number = ((p_buf[5] >> 1) & 0x1F);
  p_eit->section_number  = p_buf[6];
  p_eit->last_section_number = p_buf[7];

  p_eit->ts_id = MAKE_WORD2(p_buf[8],  p_buf[9]);
  p_eit->org_nw_id = MAKE_WORD2(p_buf[10], p_buf[11]);
  
  p_eit->seg_last_sec_number = p_buf[12];

  p_eit->last_table_id = p_buf[13];

  /****buf[3+sec_len-4];sec_len is from buff[3] to buff[sec_len]*****/
  p_eit->crc_32 =  (p_buf[sec_len -1] << 24)  |
                              (p_buf[sec_len ] << 16)     |
                              (p_buf[sec_len + 1] << 8)  |
                              (p_buf[sec_len + 2]);
  
  crc_check = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,p_buf,sec_len - 1);
  if(crc_check != p_eit->crc_32)
  {
    return FALSE;
  }
#if 0
 if((p_priv->prog_info.network_id == p_eit->org_nw_id) 
        && (p_priv->prog_info.ts_id == p_eit->ts_id) 
        && (p_priv->prog_info.svc_id == p_eit->svc_id))
  {
  OS_PRINTF("\n$$$$$ table[0x%x]epg_pin_parse: net_id [0x%x],ts_id [0x%x],svc_id [0x%x]\n",
                  p_eit->table_id,p_eit->org_nw_id,p_eit->ts_id,p_eit->svc_id);
  OS_PRINTF("\n$$$$$ table[0x%x] pi_net_id [0x%x],pi_ts_id [0x%x],pi_svc_id [0x%x]\n",
                  p_priv->prog_info.network_id,p_priv->prog_info.ts_id,p_priv->prog_info.svc_id);
 
  }
#endif
  return TRUE;
}

/*!
  parse extend event
  */
static RET_CODE _epg_pin_parse_extend_event(u8             *p_data,
                                            s16             des_len,
                                            extern_event_t *p_ext_evt)
{
  u32 offset       = 0;
  s16 len_of_items = 0;
  s16 item_len_1   = 0;
  s16 item_len_2   = 0;
  u8  item_number  = 0;

  //Set total event description number
  p_ext_evt->tot_ext_desc = SYS_GET_LOW_HALF_BYTE(p_data[offset]);
  p_ext_evt->index = SYS_GET_HIGH_HALF_BYTE(p_data[offset]);
  offset++;

  //Get language code
  memcpy(p_ext_evt->lang_code, &p_data[offset], LANGUAGE_CODE_LEN * sizeof(u8));
  offset += LANGUAGE_CODE_LEN;

  //length of items
  len_of_items = p_data[offset];
  offset++;

 //Parse item content loop
  while(len_of_items > 0)
  {
    //"item_description_char"
    item_len_1 = p_data[offset];
    len_of_items--;
    offset++;
    item_len_2 = item_len_1 > MAX_ITEM_DESC_LENGTH ? 
                 MAX_ITEM_DESC_LENGTH : item_len_1;
    if(item_number < MAX_ITEM_NUMBER)
    {
      memcpy(p_ext_evt->item[item_number].item_desc, &p_data[offset], item_len_2);
    }
    len_of_items -= item_len_1;
    offset += item_len_1;  

    //"item_char"
    item_len_1 = p_data[offset];
    len_of_items--;
    offset++;
    item_len_2 = item_len_1 > MAX_ITEM_CHAR_LENGTH ? 
                 MAX_ITEM_CHAR_LENGTH : item_len_1;
    //Item cont information
    if(item_number < MAX_ITEM_NUMBER)
    {
      memcpy(p_ext_evt->item[item_number].item_char, &p_data[offset], item_len_2);
      item_number++;
    }
    len_of_items -= item_len_1;
    offset += item_len_1;  
  }
  p_ext_evt->item_num = item_number;

  //Set text information
  p_ext_evt->ext_txt_len = p_data[offset];
  offset++;

  if(p_ext_evt->ext_txt_len > MAX_EXT_TXT_LEN)
  {
    p_ext_evt->ext_txt_len = MAX_EXT_TXT_LEN;
  }
  memcpy(p_ext_evt->ext_txt, &p_data[offset], p_ext_evt->ext_txt_len * sizeof(u8));
  return SUCCESS;
}

/*!
 parse short event

 \param[in]  p_data : si data.
 \param[in]  des_len : this descriptor length
 \param[in]  event_id : event id.
 \param[out] p_eit  : eit data.
 */
static void _epg_pin_parse_short_event(u8            *p_data,
                                       s16            des_len,
                                       short_event_t *p_sht_evt,
                                       epg_eit_t     *p_eit)
{
  u32           offset     = 0;
  u16           name_len   = 0;
  u16           text_len   = 0;
  text_event_t *p_text_evt = NULL;
  
  CHECK_FAIL_RET_VOID(p_data    != NULL);
  CHECK_FAIL_RET_VOID(p_sht_evt != NULL);
  CHECK_FAIL_RET_VOID(p_eit     != NULL);

  CHECK_FAIL_RET_VOID(p_eit->count_text_evt < MAX_TEXT_EVT_PER_SEC);
  p_text_evt = &p_eit->text_evt_list[p_eit->count_text_evt];
/*!
  get one new text event element.
  */ 
  p_text_evt->event_id = p_sht_evt->event_id;
  p_eit->count_text_evt++;

 /*!
  Set language code
  */
  memcpy(p_text_evt->lang_code, &p_data[offset], LANGUAGE_CODE_LEN);
  offset += LANGUAGE_CODE_LEN;
  des_len -= LANGUAGE_CODE_LEN;
  

 /*!
  Event name.
  */
  name_len = p_data[offset];
  offset++;
  des_len -= (1 + name_len);

  if(name_len > 0)
  {
   /*!    
    copy text to container.
    */
    p_text_evt->name_len = (name_len > MAX_EVENT_NAME_LEN) ? 
                          MAX_EVENT_NAME_LEN : name_len;
    memcpy((u8 *)p_text_evt->evt_nm, &p_data[offset], p_text_evt->name_len);
    offset += name_len;
  }
  
 /*!
  Event text length
  */
  if(des_len <= 0)
  {
     return;
  }

  text_len = p_data[offset];
  offset++;
  if(text_len > 0)
  {
   /*!
    copy text to container.
    */ 
    p_text_evt->text_len = (text_len > MAX_SHT_TEXT_LEN) ?
                           MAX_SHT_TEXT_LEN : text_len;    
    memcpy(p_text_evt->text, &p_data[offset], p_text_evt->text_len);
  }
}

static BOOL _epg_pin_check_time(utc_time_t *p_start_time)
{
  if((p_start_time->month > 12) 
  || (p_start_time->day > 31)
  || (p_start_time->hour > 23) 
  || (p_start_time->minute > 60) 
  || (p_start_time->second > 60))
  {
    return FALSE;
  }
  return TRUE;
}

/*!
  epg check P/F section record.
  */
RET_CODE epg_check_pf_section_record(epg_pin_priv_t *p_priv)
{
  //epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  epg_eit_t *p_eit = &p_priv->epg_eit;
  u32 i = 0,j = 0;
  BOOL add_flag = TRUE;
  BOOL finish_flag = TRUE;
  u32 tmp_number = 0;

  tmp_number = 0;
  if(p_eit->last_section_number >= EIT_DVBC_MAX_PF_SECTION_NUMBER) 
  {
     return ERR_FAILURE;
  }
   if(p_priv->pf_sec_record.service_total_number >= EIT_DVBC_MAX_PF_SERVICE_NUMBER) 
  {
     return ERR_FAILURE;
  }
   
  for(i = 0;i < p_priv->pf_sec_record.service_total_number;i ++)
  {
    if((p_priv->pf_sec_record.service_rec[i].ts_id == p_eit->ts_id)
       && (p_priv->pf_sec_record.service_rec[i].svc_id == p_eit->svc_id))
    {
     
     if((p_priv->pf_sec_record.service_rec[i].last_section_number == 
                                                                  p_eit->last_section_number)
         &&(p_priv->pf_sec_record.service_rec[i].version_number == 
                                                                        p_eit->version_number))
    {
          
    for(j = 0; j <= p_priv->pf_sec_record.service_rec[i].last_section_number ; j ++)
    {
       if(p_priv->pf_sec_record.service_rec[i].section_rec[j].exist_bit != TRUE)
        {
           p_priv->pf_sec_record.service_rec[i].section_rec[j].section_number = 
                                                                            p_eit->section_number;
           p_priv->pf_sec_record.service_rec[i].section_rec[j].exist_bit = TRUE;
        }
       
      if((p_priv->pf_sec_record.service_rec[i].section_rec[j].section_number == 
                                                                        p_eit->section_number)
          && (p_priv->pf_sec_record.service_rec[i].section_rec[j].exist_bit == TRUE))
        {
            if(p_priv->pf_sec_record.service_rec[i].section_rec[j].find_number 
                   <= EIT_DVBC_MAX_PF_SAME_SEC_FIND_NUM)
            {
              p_priv->pf_sec_record.service_rec[i].section_rec[j].find_number ++;
            }
        }
    }
    }
     else
      {
          p_priv->pf_sec_record.finish_flag = FALSE;
          p_priv->pf_sec_record.service_rec[i].last_section_number = 
                                                          p_eit->last_section_number;
          p_priv->pf_sec_record.service_rec[i].version_number = p_eit->version_number;
          memset(&p_priv->pf_sec_record.service_rec[i].section_rec[0],0,
                           sizeof(epg_record_section_t) * EIT_DVBC_MAX_PF_SECTION_NUMBER);
          p_priv->pf_sec_record.service_rec[i].section_rec[0].section_number = 
                                                                p_eit->section_number;
          p_priv->pf_sec_record.service_rec[i].section_rec[0].exist_bit = TRUE;
      }
     add_flag = FALSE;
    /*****if it is exist,it don't add to service record******/ 
    }
    /****else comtinue;*****/
    
  }

  /***if section record table no same svc id, it will add to table ******/
  if(add_flag ==  TRUE)
 {
      p_priv->pf_sec_record.finish_flag = FALSE;
      p_priv->pf_sec_record.service_rec[i].ts_id = p_eit->ts_id ;
      p_priv->pf_sec_record.service_rec[i].svc_id = p_eit->svc_id;
      p_priv->pf_sec_record.service_rec[i].last_section_number = p_eit->last_section_number;
      p_priv->pf_sec_record.service_rec[i].version_number = p_eit->version_number;
      memset(&p_priv->pf_sec_record.service_rec[i].section_rec[0],0,
                       sizeof(epg_record_section_t) * EIT_DVBC_MAX_PF_SECTION_NUMBER);
      p_priv->pf_sec_record.service_rec[i].section_rec[0].section_number = p_eit->section_number;
      p_priv->pf_sec_record.service_rec[i].section_rec[0].exist_bit = TRUE;
      p_priv->pf_sec_record.service_total_number ++;
  }

#if 1
  for(i = 0;i < p_priv->pf_sec_record.service_total_number;i ++)
  {
      for(j = 0; j <= p_priv->pf_sec_record.service_rec[i].last_section_number ; j ++)
     {
        if(p_priv->pf_sec_record.service_rec[i].section_rec[j].find_number 
                                             < EIT_DVBC_MAX_PF_SAME_SEC_FIND_NUM)
          {
            finish_flag = FALSE;
          }
     }
  }
  if(i < 2) /***if only one or table sent same service id section contimue****/
  {
      finish_flag = FALSE;
  }
  #else
for(j = 0; j <= p_priv->pf_sec_record.service_rec[0].last_section_number ; j ++)
{
   if(p_priv->pf_sec_record.service_rec[0].section_rec[j].find_number < 1)
      {
        finish_flag = FALSE;
      }
}
tmp_number = p_priv->pf_sec_record.service_total_number;
for(j = 0; 
j <= p_priv->pf_sec_record.service_rec[tmp_number - 1].last_section_number ; 
j ++)
{
 if(p_priv->pf_sec_record.service_rec[tmp_number - 1].section_rec[j].find_number < 1)
    {
      finish_flag = FALSE;
    }
}
/***if only one or table sent same service id section contimue****/
if(p_priv->pf_sec_record.service_total_number < 2) 
{
  finish_flag = FALSE;
}
#endif

  if(TRUE == finish_flag)
  {
    p_priv->pf_sec_record.finish_flag = TRUE;
    //OS_PRINTF("#####PF section record finish:total service number:%d\n",
    //p_priv->pf_sec_record.service_total_number);
  }
  else
  {
    p_priv->pf_sec_record.finish_flag = FALSE;
  }
  
  return SUCCESS;
}

/*!
  epg check P/F section record.
  */
void epg_reset_pf_section_record(epg_pin_priv_t *p_priv)
{
  memset(&p_priv->pf_sec_record,0,sizeof(epg_record_sec_t));
}
static RET_CODE _epg_pin_parse_eit(epg_pin_priv_t *p_priv,
                                   u8             *p_data,
                                   u32             data_size)
{
  epg_svc_t      *p_epg_svc  = &p_priv->epg_svc;
  epg_eit_t      *p_eit      = &p_priv->epg_eit;
  short_event_t  *p_sht_evt  = NULL;
  extern_event_t *p_ext_evt  = NULL;
  u16             event_id   = 0;
  s32             sec_len    = 0;
  u32             offset     = 0;
  s16             des_lp_len = 0;
  u8              des_tag    = 0;
  s16             des_length = 0;
  event_id_info_t id_info = EVT_ID_EXIST_NO;

  memset(p_eit, 0x0, sizeof(epg_eit_t));
  
  p_eit->evt_fixed_able = p_priv->evt_fixed_able;
  p_eit->have_pf_pin = p_priv->have_pf_pin;
  if(_epg_pin_parse_sec_header(p_priv, p_data, data_size, &sec_len) == FALSE)
  {
    //EP_PRINT("_epg_pin_parse_sec_header fail!!!!\n");
    return ERR_FAILURE;
  }
 /****check pf section record,it check pf receiver fininsh,only request pf table*****/
 if((p_priv->epg_eit.pf_table) 
     && (p_priv->pf_sec_record.finish_flag == FALSE))
  {
      epg_check_pf_section_record(p_priv);
  }

 epg_db_process_eit_edit_id_call_back(p_eit);

if(epg_db_add_svc(p_eit, p_epg_svc) != SUCCESS)
{
  EP_PRINT("epg_db_add_svc fail!!!!\n");
  return ERR_FAILURE;
}

 /*!
  calc section length and move data pointer.
  data size:sec len - eit header(size date later 11bit) -crc size
  */
  sec_len = sec_len - (EIT_HEADER_SIZE - 3) - 4;
  
  p_data += EIT_HEADER_SIZE;

  while(sec_len > EIT_SEC_HEAD_LEN)
  {
    event_id = MAKE_WORD2(p_data[offset], p_data[offset + 1]);
    id_info = epg_db_check_event_id(p_epg_svc, event_id);
    if(id_info == EVT_ID_EXIST_FULL) //fixed me??
    {
      #if 0  /***if the section have event nember is larger than max sht,it will add***/
      //if first event already exist, return it.
      if(p_eit->count_sht_evt == 0)
        {
        EP_PRINT("count_sht_evt fail!!!!\n");
        return ERR_FAILURE;
        }
       #endif 
      //all descriptor.
      des_lp_len = MAKE_WORD2(SYS_GET_LOW_HALF_BYTE(p_data[offset + 10]),
                            p_data[offset + 11]);
      //offset += 12;   /**cgf fix:why?***/
      offset += (12 + des_lp_len);
      
      sec_len -= (12 + des_lp_len);
      continue;
    }

   /*!
    get one short_event_t.
    */
    if(p_eit->count_sht_evt >= MAX_SHT_EVT_PER_SEC)
    {
      return SUCCESS;
    }
    CHECK_FAIL_RET_CODE(p_eit->count_sht_evt < MAX_SHT_EVT_PER_SEC);
    p_sht_evt = &p_eit->sht_evt_list[p_eit->count_sht_evt];
    p_eit->count_sht_evt ++;

    p_sht_evt->event_id = event_id;
    offset += 2;
    sec_len -= 2;

    //Start time
   date_trans(&p_data[offset], &p_sht_evt->start_time);
    offset += 5;
    sec_len -= 5;

    //Drt time
    time_point_trans(&p_data[offset], &p_sht_evt->drt_time);
    offset += 3;
    sec_len -= 3;

    //running status
    p_sht_evt->running_status = ((p_data[offset] & 0xE0) >> 5);

    //all descriptor.
    des_lp_len = MAKE_WORD2(SYS_GET_LOW_HALF_BYTE(p_data[offset]), p_data[offset + 1]);
    offset += 2;
    sec_len -= (2 + des_lp_len);

        //check start time.
    if((!_epg_pin_check_time(&p_sht_evt->start_time))
         ||((p_priv->epg_eit.pf_table == 0) 
                  && (SUCCESS != epg_db_check_max_evt_days_event(p_sht_evt,p_eit))))
    {
       /**fix by cgf***/
      p_eit->count_sht_evt --;
      //return ERR_FAILURE;
      offset += des_lp_len;
     continue;
    }
    while(des_lp_len > 0)
    {
      /*!parse descriptor tag and length*/
      des_tag    = p_data[offset];
      des_length = p_data[offset + 1];
      //move
      offset += 2;
      des_lp_len -= 2;

      switch(des_tag)
      {
      case DVB_DESC_SHORT_EVENT:
         if(p_eit->count_text_evt >= MAX_TEXT_EVT_PER_SEC)
          {
            p_eit->count_sht_evt --; /****don't process this event id*****/
            return SUCCESS;
          }
        _epg_pin_parse_short_event(&p_data[offset], des_length, p_sht_evt, p_eit);
#if 0
         if((p_eit->org_nw_id == p_priv->prog_info.network_id)
         && ((p_eit->ts_id == p_priv->prog_info.ts_id) 
         &&(p_eit->svc_id == p_priv->prog_info.svc_id)) && (p_eit->pf_table ==1))
        {
          extern void time_add(utc_time_t *p_time_src, utc_time_t *p_time_add);
          u8 asc_str[32];
          utc_time_t event_end_time = {0};
          
          memcpy(&event_end_time,&p_sht_evt->start_time,sizeof(utc_time_t));
          
          time_add(&event_end_time,&p_sht_evt->drt_time);
          sprintf((char *)asc_str, "%.2d:%.2d~%d:%d",
                        p_sht_evt->start_time.hour,p_sht_evt->start_time.minute,
                        event_end_time.hour,event_end_time.minute);
          OS_PRINTF("add event n[0x%x], t[0x%x], s[0x%x]\n",
                            p_eit->org_nw_id,
                            p_eit->ts_id,
                            p_eit->svc_id);
          OS_PRINTF("add event :%s  %s\n",asc_str,p_sht_evt->evt_nm);
      }
#endif  
        break;

      case DVB_DESC_EXT_EVENT:
        if(p_eit->count_ext_evt >= MAX_EXT_EVT_PER_EVT)
        {
            p_eit->count_sht_evt --; /****don't process this event id*****/
            return SUCCESS;
        }
        CHECK_FAIL_RET_CODE(p_eit->count_ext_evt < MAX_EXT_EVT_PER_EVT);
        p_ext_evt = &p_eit->ext_evt_list[p_eit->count_ext_evt];
        p_eit->count_ext_evt++;
        p_ext_evt->event_id = p_sht_evt->event_id;
        _epg_pin_parse_extend_event(&p_data[offset], des_length, p_ext_evt);
        break;

      case DVB_DESC_CONTENT:
        p_sht_evt->cont_level = p_data[offset];
        p_sht_evt->user_nibble = p_data[offset + 1];
        break;

      case DVB_DESC_PARENTAL_RATING:
        memcpy(p_sht_evt->parental_rating.country_code, &p_data[offset], LANGUAGE_CODE_LEN);
        p_sht_evt->parental_rating.rating = p_data[offset + 4];
        break;

      default:
        break;
      }
      //MOVE offset.
      offset += des_length;
      des_lp_len -= des_length;
    }
  }
  return (p_eit->count_sht_evt > 0) ? SUCCESS : ERR_FAILURE;
}

/*!
  epg pin pop msg list
  */
static void epg_pin_pop_msg_list_by_monitor(handle_t _this)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  ipin_t         *p_ipin = (ipin_t *)p_priv->p_this;
  u32 total_msg = 0;
  u8 i =0;
  os_msg_t *p_msg_serial =  NULL;

  CHECK_FAIL_RET_VOID(p_priv->p_msg_list != NULL);
  total_msg = (EPG_FILTE_EIT_MAX_EVENT - EPG_FILTER_UNKNOW_EVENT);
  for(i = 0;i < total_msg;i ++)
  {
     p_msg_serial = &p_priv->p_msg_list[i];
     if((p_msg_serial->content != EPG_FILTER_UNKNOW_EVENT)
        && (p_msg_serial->is_brdcst == 1))
      {
        memset(&p_priv->msg,0,sizeof(os_msg_t));
        p_priv->msg.content = p_msg_serial->content;
        p_priv->msg.para1 = p_msg_serial->para1;
        p_priv->msg.para2 = p_msg_serial->para2;
        p_priv->msg.context = p_msg_serial->context;
        p_ipin->send_message_out(p_ipin, &p_priv->msg);
        memset(p_msg_serial,0,sizeof(os_msg_t));

        /****clear msg list****/
        p_msg_serial->content = EPG_FILTER_UNKNOW_EVENT;
        p_msg_serial->is_brdcst = 0;
      }
  }
  
}

/*!
  epg pin push msg list
  */
static void epg_pin_push_msg_list(handle_t _this,os_msg_t *p_msg)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  u32 serial_id = 0;
  u32 total_msg = 0;
  os_msg_t *p_msg_serial =  NULL;
  CHECK_FAIL_RET_VOID(p_priv->p_msg_list != NULL);
  total_msg = (EPG_FILTE_EIT_MAX_EVENT - EPG_FILTER_UNKNOW_EVENT);
  serial_id = p_msg->content - EPG_FILTER_UNKNOW_EVENT;
  CHECK_FAIL_RET_VOID(serial_id <= total_msg);
  p_msg_serial = &p_priv->p_msg_list[serial_id];
  if((p_msg_serial->content == EPG_FILTER_UNKNOW_EVENT)
      && (p_msg_serial->is_brdcst == 0))
  {
      memset(p_msg_serial,0,sizeof(os_msg_t));
      p_msg_serial->content = p_msg->content;
      p_msg_serial->para1 = p_msg->para1;
      p_msg_serial->para2 = p_msg->para2;
      p_msg_serial->context = p_msg->context;
      p_msg_serial->is_brdcst = 1;
  }
}

/*!
  epg pin send message out (intra).
  */
static void epg_pin_send_message(handle_t _this,
                                                        epg_filter_evt_t evt,
                                                        u32 param1,
                                                        u32 param2,
                                                        u32 context)
{
    epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
    ipin_t         *p_ipin = (ipin_t *)p_priv->p_this;
 
    memset(&p_priv->msg,0,sizeof(os_msg_t));
    p_priv->msg.content = evt;
    p_priv->msg.para1 = param1;
    p_priv->msg.para2 = param2;
    p_priv->msg.context = context;
    
    if(p_priv->msg_send_cycle == 0)
    {
        p_ipin->send_message_out(p_ipin, &p_priv->msg);
    }
    else
    {
       /***add msg to msg list****/
       epg_pin_push_msg_list(_this,&p_priv->msg);
    }
    
}

static void epg_pin_on_receive(handle_t _this, media_sample_t *p_sample)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  epg_filter_evt_t pin_evt;
  event_attr_t event_progran = EVENT_ATTR_OTHERS_PROG;

  if(p_sample->state == SAMP_STATE_GOOD)
  {
    if((p_priv->p_parse_buffer == NULL) 
        || (p_sample->payload > EIT_SAMPLE_SECTION_SIZE))
      {
         OS_PRINTF("epg pin receive mallock is fail!\n");
         return;
      }
    memset(p_priv->p_parse_buffer,0,EIT_SAMPLE_SECTION_SIZE);
    memcpy(p_priv->p_parse_buffer,p_sample->p_data,p_sample->payload);
    //EP_PRINT("######epg_pin_on_receive GOOD!!########\n");
    if(SUCCESS == _epg_pin_parse_eit(p_priv,
                                     p_priv->p_parse_buffer,
                                     p_sample->payload))
    {
      if(epg_db_add_event(&p_priv->epg_eit, &p_priv->epg_svc) == SUCCESS)
      { 
      #if 0
          EP_PRINT("add event n[0x%x], t[0x%x], s[0x%x] sec_num[0x%x] ver_num[0x%x]\n",
                    p_priv->epg_eit.org_nw_id,
                    p_priv->epg_eit.ts_id,
                    p_priv->epg_eit.svc_id,
                    p_priv->epg_eit.section_number,
                    p_priv->epg_eit.version_number);
        #endif
       /*!
        check event is belong current program .
        */
        if((p_priv->epg_eit.org_nw_id == p_priv->prog_info.network_id)
         && ((p_priv->epg_eit.ts_id == p_priv->prog_info.ts_id) &&
            (p_priv->epg_eit.svc_id == p_priv->prog_info.svc_id)))
          {
              event_progran = EVENT_ATTR_CURRENT_PROG;
          }
          else
          {
              event_progran = EVENT_ATTR_OTHERS_PROG;
          }
          
          if(p_priv->epg_eit.pf_table)
          {
            if(event_progran == EVENT_ATTR_CURRENT_PROG)
            {
               pin_evt = EPG_FILTER_PF_EVENT;
            }
            else
            {
               pin_evt = EPG_FILTER_PF_EVENT_OTHER;
            }
            
            if((pin_evt == EPG_FILTER_PF_EVENT)
              ||(p_priv->msg_send_cycle > 0))
            {
                p_priv->epg_eit.message_flag = TRUE;
            }

          }
          else
          {
            if(event_progran == EVENT_ATTR_CURRENT_PROG)
            {
               pin_evt = EPG_FILTER_SCHE_EVENT;
            }
            else
            {
               pin_evt = EPG_FILTER_SCHE_EVENT_OTHER;
            }
            
            if(p_priv->msg_send_cycle > 0)
            {
               /****allow send msg when receive other program*****/
               p_priv->epg_eit.message_flag = TRUE;
            }
          } 
          
          if(p_priv->epg_eit.message_flag == TRUE) /**if it is current prog info time**/
          {
            epg_pin_send_message(_this,pin_evt,
                                                      0,0,0);
            p_priv->epg_eit.message_flag = FALSE;
          }
        
        p_priv->monitor_ticks = mtos_ticks_get();
      }
    }

    
    if((p_priv->epg_eit.pf_table) 
         && (TRUE == p_priv->pf_sec_record.record_able))
    {
      if((((mtos_ticks_get() - p_priv->timeout_pf_ticks) * 10) > p_priv->pf_sec_record.pf_timeout)
        && (FALSE == p_priv->pf_sec_record.finish_flag))
        {
          
           p_priv->timeout_pf_ticks = mtos_ticks_get();
           epg_pin_send_message(_this,
                                        EPG_FILTER_PF_TIMEOUT_EVENT,
                                        0,0,0);
        }
      
      if((p_priv->pf_sec_record.finish_last_flag != p_priv->pf_sec_record.finish_flag)
          && (TRUE == p_priv->pf_sec_record.finish_flag))
        {
          // OS_PRINTF("#####PF section record finish:total service number:%d\n",
          //p_priv->pf_sec_record.service_total_number);
          p_priv->pf_sec_record.finish_last_flag = p_priv->pf_sec_record.finish_flag;
          epg_pin_send_message(_this,
                                        EPG_FILTER_PF_FINISH_EVENT,
                                        0,0,0);
          
        }
    }

     p_priv->timeout_ticks = mtos_ticks_get(); 

  }
  else if(p_sample->state == SAMP_STATE_TIMEOUT)
  {
    OS_PRINTF("EIT section table[0x%x] time out:EIT_TIMEOUT_EVENT\n",
          (p_sample->context & 0xffff));
        epg_pin_send_message(_this,
                              EPG_FILTE_EIT_TIMEOUT_EVENT,
                              p_sample->context & 0xffff,0,0);
        p_priv->timeout_ticks = mtos_ticks_get();
  }
}

static void epg_pin_set_info(handle_t _this, epg_prog_info_t *p_info)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);

  CHECK_FAIL_RET_VOID(p_info != NULL);

  if(p_priv->is_requested == FALSE)
  {
    EP_PRINT("please set info after start\n");
    CHECK_FAIL_RET_VOID(0);
  }
 OS_PRINTF("epg_pin_set_info: [0x%x], t[0x%x], s[0x%x];pf table:%d,sch table:%d\n",
                            p_info->network_id,
                            p_info->ts_id,
                            p_info->svc_id,
                            p_priv->pf_array_depth,
                            p_priv->sch_array_depth);
 
  _epg_pin_request_pf_eit(p_priv, p_info);
  _epg_pin_request_sch_eit(p_priv, p_info);

  memcpy(&p_priv->prog_info, p_info, sizeof(epg_prog_info_t));
}

static void epg_pin_config(handle_t _this, epg_filter_cfg_t *p_cfg)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  u32             tot_size = 0;
  u8             *p_buf    = NULL;
  u8              i        = 0;
  u8              j        = 0;

  CHECK_FAIL_RET_VOID(p_cfg != NULL);

 /*!
  *attach eit buffer pool
  */
  tot_size = p_cfg->spin_multi_buffer_size * p_cfg->max_eit_table_num;

  CHECK_FAIL_RET_VOID(p_cfg->p_attach_buf != NULL);
  CHECK_FAIL_RET_VOID(p_cfg->buffer_size >= tot_size);
  p_priv->msg_send_cycle = p_cfg->msg_send_cycle;
  p_priv->spin_multi_buffer_size = p_cfg->spin_multi_buffer_size;
  p_priv->p_attach_buf    = p_cfg->p_attach_buf;
  p_priv->attach_buf_size = p_cfg->buffer_size;

 /*!
  *split attach buffer ==> |max_table_id * spin_multi_buffer_size|
  */
  p_buf = p_priv->p_attach_buf;

  /*!
   *alloc pf eit array.
   */
  p_priv->pf_array_depth = 0;
  p_priv->sch_array_depth = 0;
  for(i = 0; i < p_cfg->max_eit_table_num; i++)
  {
    if((p_cfg->p_tbl_id_list[i] == EIT_TABLE_ID_PF_ACTUAL)
      || (p_cfg->p_tbl_id_list[i] == EIT_TABLE_ID_PF_OTHER))
    {
      p_priv->pf_array_depth++;
    }
    else
    {
      p_priv->sch_array_depth++;
    }
  }

  if(p_priv->pf_array_depth > 0)
  {
     p_priv->have_pf_pin = TRUE;
  }
  else
  {
    p_priv->have_pf_pin = FALSE;
  }

  if(p_priv->pf_array_depth == p_cfg->max_eit_table_num)
  {
    p_priv->pf_sec_record.record_able = TRUE;
    p_priv->pf_sec_record.finish_last_flag = FALSE;
    p_priv->pf_sec_record.finish_flag = FALSE;
    p_priv->pf_sec_record.pf_timeout = p_cfg->pf_timeout;
    if(p_priv->pf_sec_record.pf_timeout == 0)
    {
      p_priv->pf_sec_record.pf_timeout  = PF_CONFIG_DEFAULT_TIME_OUT;
    }
  }
  else
  {
    p_priv->pf_sec_record.record_able = FALSE;
  }

  p_priv->eit_timeout = p_cfg->eit_timeout;
  
  if(p_priv->eit_timeout == 0)
  {
    p_priv->eit_timeout = EIT_TIMEOUT;
  }
   
   CHECK_FAIL_RET_VOID((p_priv->pf_array_depth + p_priv->sch_array_depth) <= p_cfg->max_eit_table_num);

  if(p_priv->pf_array_depth > 0)
  {
    p_priv->p_pf_array = mtos_malloc(p_priv->pf_array_depth * sizeof(eit_array_t));
    CHECK_FAIL_RET_VOID(p_priv->p_pf_array != NULL);
    memset(p_priv->p_pf_array, 0x0, p_priv->pf_array_depth * sizeof(eit_array_t));

    /*!
     *init pf eit array.
     */
    for(i = 0, j = 0; i < p_cfg->max_eit_table_num; i++)
    {
      if((p_cfg->p_tbl_id_list[i] == EIT_TABLE_ID_PF_ACTUAL)
        || (p_cfg->p_tbl_id_list[i] == EIT_TABLE_ID_PF_OTHER))
      {
        CHECK_FAIL_RET_VOID(j < p_priv->pf_array_depth);
        p_priv->p_pf_array[j].table_id = p_cfg->p_tbl_id_list[i];
        p_priv->p_pf_array[j].p_buffer = p_buf;
        p_priv->p_pf_array[j].free_flag = TRUE;
        CHECK_FAIL_RET_VOID(p_priv->p_pf_array[j].p_buffer != NULL);
        if(p_cfg->p_tbl_id_list[i] == EIT_TABLE_ID_PF_OTHER)
        {
           p_cfg->pf_policy = PF_EVENT_OF_NETWORK_ID;
        }
        //move
        p_buf += p_priv->spin_multi_buffer_size;
        j++;
      }
    }
  }

  /*!
   *alloc schedule eit array.
   */
 // p_priv->sch_array_depth = p_cfg->max_eit_table_num - p_priv->pf_array_depth;

  
    if(p_priv->sch_array_depth > 0)
    {
      p_priv->p_sch_array = mtos_malloc(p_priv->sch_array_depth * sizeof(eit_array_t));
      CHECK_FAIL_RET_VOID(p_priv->p_sch_array != NULL);
      memset(p_priv->p_sch_array, 0x0, p_priv->sch_array_depth * sizeof(eit_array_t));

      /*!
       *init schedule eit array.
       */
      for(i = 0, j = 0; i < p_cfg->max_eit_table_num; i++)
       {
         if((p_cfg->p_tbl_id_list[i] != EIT_TABLE_ID_PF_ACTUAL)
           && (p_cfg->p_tbl_id_list[i] != EIT_TABLE_ID_PF_OTHER))
         {
           CHECK_FAIL_RET_VOID(j < p_priv->sch_array_depth);
           p_priv->p_sch_array[j].table_id = p_cfg->p_tbl_id_list[i];
           p_priv->p_sch_array[j].p_buffer = p_buf;
           p_priv->p_sch_array[j].free_flag = TRUE;
           CHECK_FAIL_RET_VOID(p_priv->p_sch_array[j].p_buffer != NULL);
          if((p_cfg->p_tbl_id_list[i] == EIT_TABLE_ID_SCH_OTHER_60)
              ||(p_cfg->p_tbl_id_list[i] == EIT_TABLE_ID_SCH_OTHER_61))
            {
                p_cfg->sch_policy = SCH_EVENT_OF_NETWORK_ID;
            }
           //move
           p_buf += p_priv->spin_multi_buffer_size;
           j++;
         }
      }
    }

  /*!
   attach policy.
   */
  p_priv->net_policy = p_cfg->net_policy;
  if(p_priv->net_policy == NETWORK_ID_UNKNOWN)
  {
    p_priv->net_policy = NETWORK_ID_NORMAL;
  }
  p_priv->pf_policy  = p_cfg->pf_policy;
  if(p_priv->pf_policy  == PF_EVENT_UNKNOWN)
  {
   p_priv->pf_policy  = PF_EVENT_OF_NETWORK_ID;
  }
  p_priv->sch_policy = p_cfg->sch_policy;
  if(p_priv->sch_policy == SCH_EVENT_UNKNOWN)
  {
   p_priv->sch_policy = SCH_EVENT_OF_TS_ID;
  }
  memcpy(&p_priv->prog_info,&p_cfg->prog_info,sizeof(epg_prog_info_t)) ;
  p_priv->evt_fixed_able = p_cfg->evt_fixed_able;
  p_priv->task_release_time = p_cfg->task_release_time;
}

void epg_pin_active_loop(handle_t _this)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);

  if((mtos_ticks_get() - p_priv->monitor_ticks) > MONITOR_TICKS)
  {
    epg_db_delete(EPG_OVERDUE_EVENT);
    epg_pin_send_message(_this,
                            EPG_FILTER_DEL_EVENT,
                            0,0,0);

    p_priv->monitor_ticks = mtos_ticks_get();
  }

  if(p_priv->msg_send_cycle > 0)
  {
    if((mtos_ticks_get() - p_priv->msg_monitor_ticks) 
       >  (p_priv->msg_send_cycle / 10))
      {
        epg_pin_pop_msg_list_by_monitor(_this);
        p_priv->msg_monitor_ticks = mtos_ticks_get();
      }
  }
  
  /*!
  epg api task release time,value * ms;if it is 0,don't take effect!
  it  supply eva too fast,but ap is tool slow;it is used jazz plat only,
  it will solve by eva oneself;
  */
  if(p_priv->task_release_time > 0)
    {
      mtos_task_sleep(p_priv->task_release_time);    
    }
  #if 0
  if(((mtos_ticks_get() - p_priv->timeout_ticks) * 10) > p_priv->eit_timeout)
  {
     OS_PRINTF("#####EIT section time out:EPG_FILTE_EIT_TIMEOUT_EVENT\n");
    epg_pin_send_message(_this,
                            EPG_FILTE_EIT_TIMEOUT_EVENT,
                            0,0,0);
    p_priv->timeout_ticks = mtos_ticks_get();
  }
 #endif 
}

static RET_CODE epg_pin_on_open(handle_t _this)
{
  epg_pin_priv_t *p_priv         = epg_pin_get_priv(_this);
  ipin_t         *p_ipin         = (ipin_t *)_this;
  ipin_t         *p_connect_ipin = (ipin_t *)p_ipin->get_connected(p_ipin);
  
  //only get once.
  if(p_priv->p_dmx_inter == NULL)
  {
    p_connect_ipin->get_interface(p_connect_ipin,
                                  DMX_INTERFACE_NAME,
                                  (void **)&p_priv->p_dmx_inter);
    CHECK_FAIL_RET_CODE(p_priv->p_dmx_inter != NULL);
  }
  p_priv->is_requested = FALSE;
  return SUCCESS;
}

static RET_CODE epg_pin_on_start(handle_t _this)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  u32 depth_iq_msg = 0;
  u8 i =0;
                            
  p_priv->p_parse_buffer = mtos_malloc(EIT_SAMPLE_SECTION_SIZE);
  if(p_priv->p_parse_buffer == NULL)
  {
    return ERR_FAILURE;
  }

  depth_iq_msg = EPG_FILTE_EIT_MAX_EVENT - EPG_FILTER_UNKNOW_EVENT;
  p_priv->p_msg_list = (os_msg_t *)mtos_malloc(depth_iq_msg * sizeof(os_msg_t));
  if(p_priv->p_msg_list == NULL)
  {
    return ERR_FAILURE;
  }
  
  /****clear msg list****/
  memset(p_priv->p_msg_list,0,depth_iq_msg * sizeof(os_msg_t));
  for(i = 0;i < depth_iq_msg;i ++)
  {
     p_priv->p_msg_list[i].content = EPG_FILTER_UNKNOW_EVENT;
     p_priv->p_msg_list[i].is_brdcst = 0;
  }

  p_priv->is_requested = TRUE;
  p_priv->monitor_ticks = mtos_ticks_get();
  p_priv->timeout_pf_ticks = mtos_ticks_get();
  p_priv->timeout_ticks = mtos_ticks_get(); 
  p_priv->msg_monitor_ticks = mtos_ticks_get();
  //memset(&p_priv->prog_info, 0xFF, sizeof(epg_prog_info_t));
   OS_PRINTF("epg_pin_start_info: [0x%x], t[0x%x], s[0x%x];pf table:%d,sch table:%d\n",
                            p_priv->prog_info.network_id,
                            p_priv->prog_info.ts_id,
                            p_priv->prog_info.svc_id,
                            p_priv->pf_array_depth,
                            p_priv->sch_array_depth);
  _epg_pin_request_pf_eit(p_priv, NULL);
  _epg_pin_request_sch_eit(p_priv, NULL);
  return SUCCESS;
}

static RET_CODE epg_pin_on_stop(handle_t _this)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  u8 index = 0;
  for(index = 0; index < p_priv->pf_array_depth; index++)
  {
    CHECK_FAIL_RET_CODE(p_priv->p_pf_array[index].free_flag == FALSE);
    _epg_pin_free(p_priv, p_priv->p_pf_array[index].table_id);
    p_priv->p_pf_array[index].free_flag = TRUE;
  }

  for(index = 0; index < p_priv->sch_array_depth; index++)
  {
    CHECK_FAIL_RET_CODE(p_priv->p_sch_array[index].free_flag == FALSE);
    _epg_pin_free(p_priv, p_priv->p_sch_array[index].table_id);
    p_priv->p_sch_array[index].free_flag = TRUE;
  }
  
  p_priv->is_requested = FALSE;

   if(p_priv->p_msg_list != NULL)
  {
    mtos_free(p_priv->p_msg_list);
    p_priv->p_msg_list = NULL;
  }

  if(p_priv->p_parse_buffer != NULL)
  {
    mtos_free(p_priv->p_parse_buffer);
    p_priv->p_parse_buffer = NULL;
  }
  
  
  return SUCCESS;
}

static RET_CODE epg_pin_on_close(handle_t _this)
{
  epg_pin_priv_t *p_priv = epg_pin_get_priv(_this);
  //free...
  if(p_priv->p_pf_array != NULL)
  {
    mtos_free(p_priv->p_pf_array);
    p_priv->p_pf_array = NULL;
  }

  if(p_priv->p_sch_array != NULL)
  {
    mtos_free(p_priv->p_sch_array);
    p_priv->p_sch_array = NULL;
  }
  return SUCCESS;
}

epg_pin_t * epg_pin_create(epg_pin_t *p_epg_pin, interface_t *p_owner)
{
  epg_pin_priv_t   *p_priv  = &p_epg_pin->m_priv_data;
  interface_t      *p_inter = (interface_t *)p_epg_pin;
  ipin_t           *p_ipin  = (ipin_t *)p_epg_pin;
  base_input_pin_t *p_input = (base_input_pin_t *)p_epg_pin;
  sink_pin_para_t   sp_para = { 0, };
  media_format_t    format  = { MT_PSI,};

  //check input parameter
  CHECK_FAIL_RET_NULL(p_priv  != NULL);
  CHECK_FAIL_RET_NULL(p_input != NULL);
  CHECK_FAIL_RET_NULL(p_ipin  != NULL);
  CHECK_FAIL_RET_NULL(p_inter != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);
  epg_reset_pf_section_record(p_priv);

  //create base class
  sp_para.p_filter = p_owner;
  sp_para.p_name   = "epg_pin";
  sink_pin_create(&p_epg_pin->m_sink_pin, &sp_para);

  //init private date
  p_priv->p_this = p_epg_pin; //this point

  p_ipin->add_supported_media_format(p_ipin, &format);

  //init member function
  p_epg_pin->set_info    = epg_pin_set_info;
  p_epg_pin->active_loop = epg_pin_active_loop;
  p_epg_pin->config      = epg_pin_config;

  //overload class base_input_pin_t virtual function
  p_input->on_receive = epg_pin_on_receive;

  //overload class ipin_t virtual function
  p_ipin->on_open  = epg_pin_on_open;
  p_ipin->on_start = epg_pin_on_start;
  p_ipin->on_stop  = epg_pin_on_stop;
  p_ipin->on_close = epg_pin_on_close;

  //overload class interface_t virtual function
  p_inter->report = epg_pin_report;

  return p_epg_pin;
}

