/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>
#include "sys_define.h"
#include "sys_types.h"
#include "lib_util.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "tot.h"
#include "err_check_def.h"

#include "service.h"

#define ISO3166CRCODE_CHI                  0x0043484E

void parse_tot(handle_t handle, dvb_section_t *p_sec) 
{
  service_t *p_svc = handle;
  u8 i = 0 ;
  u8 *p_data_buf = NULL;
  u16 len = 0 ;
  tot_t *p_tot_sec = NULL;
  u32 prc = 0 ;
  u8 size = 0,loop = 0;
  local_tm_oft_des_t *p_time_offset_desc = NULL;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);
  
  p_data_buf = p_sec->p_buffer;
  if(p_data_buf[i]!= DVB_TABLE_ID_TOT)
  {
    //Fix me 
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;    
  }
  p_tot_sec  = mtos_malloc(sizeof(tot_t));
  CHECK_FAIL_RET_VOID(p_tot_sec != NULL);
  memset(p_tot_sec,0,sizeof(tot_t));
  
  i += 3;
  date_trans(&p_data_buf[i], &(p_tot_sec->tm));    
  i += 5;
  len += (p_data_buf[i++]&0xF<<8);
  len += p_data_buf[i++];
  len &= 0xFFF;
  //CHECK_FAIL_RET_VOID(len%13 == 0);  
  while(len > 0)
  {
    if(p_data_buf[i]!= DVB_DESC_LOALTIME_OFFSET)
    {
      i++;
      len -= (p_data_buf[i]+2);
      i += (p_data_buf[i]+1);
    }
    else
    {
      i++ ;
      size = p_data_buf[i++] ;
      loop = size;
      while(size > 0)
      {
        prc += ((u32)p_data_buf[i++])<<16;
        prc += ((u32)p_data_buf[i++])<<8;
        prc += ((u32)p_data_buf[i++]) ;
        p_time_offset_desc = &p_tot_sec->tmoft[p_tot_sec->oft_num];
        
        if(prc == ISO3166CRCODE_CHI)
        {
          p_time_offset_desc->region_id = (0xFC&p_data_buf[i])>>2 ;
           p_time_offset_desc->local_time_offset_polar = (0x1&p_data_buf[i++]);
          time_point_trans(&p_data_buf[i],
                                    &(p_time_offset_desc->local_time_offset)) ;
          i += 2;
          date_trans(&p_data_buf[i],&(p_time_offset_desc->tmofchge));
          i += 5;
          time_point_trans(&p_data_buf[i],
                                    &(p_time_offset_desc->next_time_offset)) ;
          i += 2;      
          p_tot_sec->oft_num++;
          size -= 13;
        }
        
        else
        {
          i += 13;
          /*skip time info belong to other countries.*/
          size -= 13;
        }        
      }

      len -= (loop + 2);
    }
  }  
  
  {
    os_msg_t msg;
    msg.is_ext = 1;
    msg.content = DVB_TOT_FOUND;
    msg.para1 = (u32)p_tot_sec;
    msg.para2 = sizeof(tot_t);
    p_svc->notify(p_svc, &msg);
  }      
  mtos_free(p_tot_sec);
}

void request_tot_section(dvb_section_t *p_sec, u32 param)
{
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,sizeof(u8) * MAX_FILTER_MASK_BYTES);    
  
  p_sec->pid = DVB_TOT_PID;
  p_sec->table_id = DVB_TABLE_ID_TOT;
  
  p_sec->filter_code[0] = p_sec->table_id;
  p_sec->filter_mask[0] = 0xFF;
  p_sec->filter_mask_size = 1;
  p_sec->timeout = (u32)TOT_TIMEOUT;  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  

  //Alloc fileter 
  //if(FALSE == dvb_engine_alloc_section(p_sec))
  //{
  //}
}


