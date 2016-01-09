/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"
#include "err_check_def.h"

#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "sdt.h"



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

/*!
  \param[in] p_buf data buffer input
  */
static void parse_svc_des(u8 *p_buf, svc_des_t *p_svc_desc)
{
  u16 i = 0;

  CHECK_FAIL_RET_VOID(p_svc_desc != NULL);
  memset(p_svc_desc, 0, sizeof(svc_des_t));
  
  p_svc_desc->size  = p_buf[i++];
  p_svc_desc->svc_type = p_buf[i++];
  
  p_svc_desc->provider_nm_len = p_buf[i++];
  p_svc_desc->p_provider_addr = &p_buf[i];
  
  i += p_svc_desc->provider_nm_len;
  p_svc_desc->svc_nm_len = p_buf[i++];
  p_svc_desc->p_svc_nm_addr = &p_buf[i++];
}

static void parse_nvod_reference_des(u8 *p_buf, sdt_t *p_sdt)
{
  u8 loopi = 0;
  u8 descriptor_len = p_buf[0] / 6;
  u8 *p_data = NULL;
  nvod_reference_descr_t *p_nvod = NULL;
  void *p_nvod_mosaic = class_get_handle_by_id(NVOD_MOSAIC_CLASS_ID);

  p_sdt->svc_des[p_sdt->svc_count].nvod = descriptor_len;
  p_nvod = (nvod_reference_descr_t *)alloc_nvod_buf(p_nvod_mosaic, descriptor_len);
  if (p_nvod == NULL)
  {
    return ;
  }
  p_sdt->svc_des[p_sdt->svc_count].p_nvod_mosaic_des = (void *)p_nvod;

  for (loopi = 0; loopi < descriptor_len; loopi ++)
  {
    p_data = p_buf + 1 + loopi * 6;
    p_nvod[loopi].stream_id = MAKE_WORD2(p_data[0], p_data[1]);
    p_nvod[loopi].orig_network_id = MAKE_WORD2(p_data[2], p_data[3]);
    p_nvod[loopi].svc_id = MAKE_WORD2(p_data[4], p_data[5]);
  }
}

void parse_sdt(handle_t handle, dvb_section_t *p_sec)
{  
  /*destination buffer for savint decoded data.*/
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_svc = handle;
  u8  l = 0;
  u16 cnt = 0 ;
  u16 length = 0;
  u16 size = 0 ;
  u16 desloop = 0 ;
  u8  *p_in_buffer = NULL;  
  svc_des_t  svc_desc = {0};
  sdt_t _sdt = {0};
  sdt_t *p_sdt = &_sdt;
  os_msg_t msg = {0};
  u8 error_no = 0;
  
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  p_in_buffer = p_sec->p_buffer;  
  if(p_in_buffer[0] != p_sec->table_id)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;
  }
  cnt ++; 
  p_sdt->sec_length = p_in_buffer[cnt++] << 8 ;
  p_sdt->sec_length += p_in_buffer[cnt++] ;
  p_sdt->sec_length &= 0xFFF;
  if(p_sdt->sec_length < SDT_SEC_HEAD_LEN)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;    
  }  
  CHECK_FAIL_RET_VOID(p_sdt->sec_length <= MAX_SDT_SEC_LEN);

  p_sdt->p_origion_data = p_sec->p_buffer;
  
  p_sdt->stream_id = p_in_buffer[cnt ++] << 8 ;
  p_sdt->stream_id += p_in_buffer[cnt ++] ;

  p_sdt->pf_flag = MASK_FIRST_BIT_OF_BYTE(p_in_buffer[cnt ++]);
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
    (p_sdt->svc_des[p_sdt->svc_count]).run_sts = 
      ((p_in_buffer[cnt] & 0xE0) >> 5);
      
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
          memcpy((u8 *)(p_sdt->svc_des[p_sdt->svc_count]).name, 
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

  if (p_sec->table_id == DVB_TABLE_ID_SDT_ACTUAL)
  {
    msg.content = DVB_SDT_FOUND;
  }
  if (p_sec->table_id == DVB_TABLE_ID_SDT_OTHER)
  {
    msg.content = DVB_SDT_OTR_FOUND;
  }
  
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.para1 = (u32)p_sdt;
  msg.para2 = sizeof(sdt_t);
#ifdef __LINUX__
  msg.is_ext = 1;
#endif
  p_svc->notify(p_svc, &msg);
}

void parse_sdt_dvbc(handle_t handle, dvb_section_t *p_sec)
{  
  /*destination buffer for savint decoded data.*/
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  void *p_nvod_mosaic = class_get_handle_by_id(NVOD_MOSAIC_CLASS_ID);
  service_t *p_svc = handle;
  u8  l = 0;
  u16 cnt = 0 ;
  u16 length = 0;
  u16 size = 0 ;
  u16 desloop = 0 ;
  u8  *p_in_buffer = NULL;  
  svc_des_t  svc_desc = {0};
  sdt_t _sdt = {0};
  sdt_t *p_sdt = &_sdt;
  os_msg_t msg = {0};
  u8 error_no = 0;
  sdt_svc_descriptor_t *p_svc_des = NULL;
  u8 desc_tag = 0;
  u32 priv_data_spec = 0;
  
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  p_in_buffer = p_sec->p_buffer;  
  if (p_in_buffer[0] != p_sec->table_id)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;
  }
  cnt ++; 
  p_sdt->sec_length = p_in_buffer[cnt ++] << 8 ;
  p_sdt->sec_length += p_in_buffer[cnt ++] ;
  p_sdt->sec_length &= 0xFFF;
  if (p_sdt->sec_length < SDT_SEC_HEAD_LEN)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;    
  }  
  CHECK_FAIL_RET_VOID(p_sdt->sec_length <= MAX_SDT_SEC_LEN);
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
    if (length < SDT_DESC_HEAD_LEN)
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
    (p_sdt->svc_des[p_sdt->svc_count]).run_sts = 
      ((p_in_buffer[cnt] & 0xE0) >> 5);
      
    p_sdt->svc_des[p_sdt->svc_count].is_scrambled = (p_in_buffer[cnt] & 0x10) >> 4;
    desloop = SYS_GET_LOW_HALF_BYTE(p_in_buffer[cnt ++]) << 8 ;
    desloop += p_in_buffer[cnt ++] ;
    size = desloop;
    
    if (size > (length - SDT_DESC_HEAD_LEN))
    {
      break;
    }
    while(desloop >  0)
    {
      switch (p_in_buffer[cnt ++])
      {
      case DVB_DESC_SERVICE:
        {
          parse_svc_des(&p_in_buffer[cnt], &svc_desc);
          l = (svc_desc.svc_nm_len <= MAX_SVC_NAME_LEN)?
          (svc_desc.svc_nm_len) : (MAX_SVC_NAME_LEN);
          memcpy((u8 *)(p_sdt->svc_des[p_sdt->svc_count]).name, 
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
          /*if(p_in_buffer[cnt] != 1)
          {
            desloop = 0;
            memset((u8 *)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
            sizeof(sdt_svc_descriptor_t));
            p_sdt->svc_count --;
            error_no = 1;
            break;
          }*/
          if(p_in_buffer[cnt] == 1)
          {
            (p_sdt->svc_des[p_sdt->svc_count]).chnl_vlm_cmpt = p_in_buffer[cnt + 1];
          }
        }
        break;
      case DVB_CA_IDENTIFIER:
        {
          p_sdt->svc_des[p_sdt->svc_count].ca_system_id = 
                  MAKE_WORD2(p_in_buffer[cnt + 1], p_in_buffer[cnt + 2]);
        }
        break;
      case DVB_DESC_MOSAIC_DESC:
        {
          p_svc_des = &(p_sdt->svc_des[p_sdt->svc_count]);
          if (!p_svc_des->mosaic_des_found
            && !p_svc_des->logic_screen_des_found
            && !p_svc_des->linkage_des_found)
          {
            p_svc_des->p_nvod_mosaic_des = alloc_mosaic_buf(p_nvod_mosaic, p_svc_des->svc_id);
            if (p_svc_des->p_nvod_mosaic_des == NULL)
            {
              break;
            }
          }
          p_svc_des->mosaic_des_found = 1;
          parse_mosaic_des(&p_in_buffer[cnt], (mosaic_t *)p_svc_des->p_nvod_mosaic_des);
        }
        break;
      case DVB_DESC_PRIVATE_DATA:
        {
          desc_tag = DVB_DESC_PRIVATE_DATA;
          priv_data_spec = (((u32)p_in_buffer[cnt + 1]) << 24) |
            (((u32)p_in_buffer[cnt + 2]) << 16) |
            (((u32)p_in_buffer[cnt + 3]) << 8) | p_in_buffer[cnt + 4];
        }
        break;
      case DVB_DESC_LOGIC_SCREEN:
        {
          if (desc_tag == DVB_DESC_PRIVATE_DATA && priv_data_spec == 0x1F)
          {
            p_svc_des = &(p_sdt->svc_des[p_sdt->svc_count]);
            if (!p_svc_des->mosaic_des_found
              && !p_svc_des->logic_screen_des_found
              && !p_svc_des->linkage_des_found)
            {
              p_svc_des->p_nvod_mosaic_des = alloc_mosaic_buf(p_nvod_mosaic, p_svc_des->svc_id);
              if (p_svc_des->p_nvod_mosaic_des == NULL)
              {
                break;
              }
            }
            p_svc_des->logic_screen_des_found = 1;
            parse_mosaic_logic_screen_des(&p_in_buffer[cnt],
              (mosaic_t *)p_svc_des->p_nvod_mosaic_des);
          }
        }
        break;
        case DVB_DESC_LINKAGE:
        {
          if (desc_tag == DVB_DESC_PRIVATE_DATA)
          {
            p_svc_des = &(p_sdt->svc_des[p_sdt->svc_count]);
            if (!p_svc_des->mosaic_des_found
              && !p_svc_des->logic_screen_des_found
              && !p_svc_des->linkage_des_found)
            {
              p_svc_des->p_nvod_mosaic_des = alloc_mosaic_buf(p_nvod_mosaic, p_svc_des->svc_id);
              if (p_svc_des->p_nvod_mosaic_des == NULL)
              {
                break;
              }
            }
            p_svc_des->linkage_des_found = 1;
            parse_mosaic_linkage_des(&p_in_buffer[cnt], (mosaic_t *)p_svc_des->p_nvod_mosaic_des);
          }
        }
        break;
        
      case DVB_DESC_NVOD_REFERENCE:
        {
          p_sdt->svc_des[p_sdt->svc_count].nvod_reference_des_found = 1;
          parse_nvod_reference_des(&p_in_buffer[cnt], p_sdt);
        }
        break;
      case DVB_DESC_TIME_SHIFTED_SERVICE:
        {
          p_sdt->svc_des[p_sdt->svc_count].time_shifted_svc_des_found = 1;
          p_sdt->svc_des[p_sdt->svc_count].nvod =
            MAKE_WORD2(p_in_buffer[cnt + 1], p_in_buffer[cnt + 2]);
        }
        break;
        
      default:
        {
          if(p_in_buffer[cnt]>(desloop -2))
          {
            desloop = 0;
            memset((u8 *)&(p_sdt->svc_des[p_sdt->svc_count]), 0,
              sizeof(sdt_svc_descriptor_t));             
            p_sdt->svc_count--;
            error_no = 1;
          } 
        }
        break;
      }
      if (error_no)
      {
        break;
      }

      desloop -= (p_in_buffer[cnt] + 2);
      cnt += (p_in_buffer[cnt] + 1);
    }
    desloop = 0;
    p_sdt->svc_count ++;
    length -= (SDT_DESC_HEAD_LEN + size);
  }

  if (p_sec->table_id == DVB_TABLE_ID_SDT_ACTUAL)
  {
    msg.content = DVB_SDT_FOUND;
  }
  if (p_sec->table_id == DVB_TABLE_ID_SDT_OTHER)
  {
    msg.content = DVB_SDT_OTR_FOUND;
  }
  
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.para1 = (u32)p_sdt;
  msg.para2 = sizeof(sdt_t);
#ifdef __LINUX__
  msg.is_ext = 1;
#endif
  p_svc->notify(p_svc, &msg);  
}

void request_sdt(dvb_section_t * p_sec, u32 table_id, 
  u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  //para2 is section number
  u32 sec_number = para2; 
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  
  p_sec->pid      = DVB_SDT_PID;
  p_sec->table_id = table_id;
  p_sec->timeout = (u32)SDT_TIMEOUT;
  p_sec->direct_data = FALSE;
  p_sec->crc_enable = 1;
  //p_sec->use_soft_filter = TRUE;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_code[0] = p_sec->table_id;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[4] = (u8)sec_number;
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_mask_size = 5;
#else
  p_sec->filter_code[6] = (u8)sec_number;
  p_sec->filter_mask[6] = 0xFF;
  p_sec->filter_mask_size = 7;
#endif
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

