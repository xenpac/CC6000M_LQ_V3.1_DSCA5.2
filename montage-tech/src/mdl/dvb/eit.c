/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"
#include "string.h"
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
#include "err_check_def.h"

#include "eit.h"

#include "service.h"
/*!
  CRC length
  */
#define CRC_SIZE  (4)
/*!
  Length of EIT head
  */
#define EIT_SEC_HEAD_LEN  12
/*!
  Min length of EIT
  */
#define EIT_MIN_LEN 15


/*!
  Macro of debug`
  */
//#define EIT_DEBUG

#ifdef EIT_DEBUG
/*!
  Macro of debug`
  */
#define EIT_PRINTF    OS_PRINTF
#else
/*!
  Macro of debug
  */
#define EIT_PRINTF DUMMY_PRINTF
#endif

static RET_CODE parse_cont_desc(cont_desc_t *p_eit_cont, u16 event_id, 
u8 *p_buf)
{
  //Content level
  p_eit_cont->cont_level = p_buf[0];
  p_buf ++;

  //User nibble
  p_eit_cont->usr_nib    = p_buf[0];
  p_eit_cont->evt_id     = event_id;
  p_buf ++;

  return SUCCESS;
}

static RET_CODE parse_pf_sht_evt(sht_evt_desc_t *p_sht_desc, sht_evt_txt_t *p_txt,
u16 event_id, u8 *p_buf)
{
  u16 event_name_len = 0;
  //Event name length
  event_name_len = (u16)p_buf[0] ; 
  p_buf ++;
  //Event name information
  if(event_name_len > MAX_EVT_NAME_LEN)
  {
    p_txt->name_len = MAX_EVT_NAME_LEN;
  }
  else
  {
    p_txt->name_len = event_name_len;
  }  

  memcpy((u8 *)p_txt->evt_nm, p_buf, p_txt->name_len);
  p_buf += event_name_len;   
  return SUCCESS;
}





static RET_CODE parse_sht_evt(sht_evt_desc_t *p_sht_desc, sht_evt_txt_t *p_txt,
u16 event_id, u8 *p_buf)
{
  u16 event_name_len = 0;
  u16 txt_len = 0;
  
  //Event name length
  event_name_len = (u16)p_buf[0]; 
  p_buf ++;

  //Event name information
  if(event_name_len > MAX_EVT_NAME_LEN)
  {
    p_txt->name_len = MAX_EVT_NAME_LEN;
  }
  else
  {
    p_txt->name_len = event_name_len;
  }
  
  p_txt->evt_id = p_sht_desc->evt_id;
  memcpy((u8 *)p_txt->evt_nm, p_buf, p_txt->name_len);
  p_buf += event_name_len;

  //Event text length
  txt_len = p_buf[0];
  if(txt_len == 0)
  {
    return SUCCESS;
  }

  p_buf ++;

  if(txt_len > MAX_SHT_TEXT_LEN)
  {
    p_txt->txt_len = MAX_SHT_TEXT_LEN;
  }
  else
  {
    p_txt->txt_len = txt_len;
  }
  
  //Event txt
  memcpy(p_txt->txt, p_buf, p_txt->txt_len);
  p_buf += txt_len;

  //Set event id to text information
  p_txt->evt_id = event_id;
  
  //Add \0 to string end
  if(p_txt->name_len  > MAX_EVT_NAME_LEN)
  {
    p_txt->name_len = MAX_EVT_NAME_LEN;
  }
    
  return SUCCESS;
}

static RET_CODE parse_ext_desc(ext_evt_desc_t *p_ext_desc, 
ext_evt_txt_t *p_ext_txt, u16 event_id, u8 *p_buf)
{
  u8 item_desc_len = 0;
  u8 item_len = 0;
  u8  item_num  = 0;
//  u32 item_cont_len = 0;
  u32 txt_len = 0;
  u8 len_of_items = 0;
  
  //Set total event description number
  p_ext_desc->tot_ext_desc = SYS_GET_LOW_HALF_BYTE(p_buf[0]);
  p_ext_desc->index = SYS_GET_HIGH_HALF_BYTE(p_buf[0]) >> 4;
  p_ext_desc->evt_id = event_id;
  p_buf ++;

  if(p_ext_desc->index > p_ext_desc->tot_ext_desc)
  {
    return ERR_FAILURE;
  }
  //Get language code
  memcpy(p_ext_desc->lang_code, p_buf, LANGUAGE_CODE_LEN * sizeof(u8));
  memcpy(p_ext_txt->lang_code, p_buf, LANGUAGE_CODE_LEN * sizeof(u8));
  p_buf += LANGUAGE_CODE_LEN * sizeof(u8);
  
  len_of_items = p_buf[0];
  p_buf ++;

  
 //Parse item content loop
  while(len_of_items > 0)
  {
    //Item name length
    item_desc_len = p_buf[0];
    len_of_items -- ;
    p_buf ++;

    if(item_desc_len > len_of_items)
    {
      return ERR_FAILURE;
    }
  
    //Item name information
    if(item_desc_len > 0)
    {
      if(item_num < MAX_ITEM_NUM)
      {
        if(item_desc_len < MAX_ITEM_DESC_LEN)
        {
          memcpy(p_ext_desc->item[item_num].item_name, p_buf, item_desc_len);
        }
        else
        {
          memcpy(p_ext_desc->item[item_num].item_name, p_buf, MAX_ITEM_DESC_LEN);
        }
      }
      p_buf += item_desc_len;

//      CHECK_FAIL_RET_CODE(len_of_items >= item_desc_len);
      len_of_items -= item_desc_len;
    }

    if(len_of_items == 0)
    {
      return ERR_FAILURE;
    }
    //Item cont len
    item_len = p_buf[0];
    len_of_items --;
    p_buf ++;

    if(item_len > len_of_items)
    {
      return ERR_FAILURE;
    }

    if(item_len > 0)
    {
      //Item cont information
      if(item_num < MAX_ITEM_NUM)
      {
        if(item_len < MAX_ITEM_CONT_LEN)
        {
          memcpy(p_ext_desc->item[item_num].item_cont, p_buf, item_len);
        }
        else
        {
          memcpy(p_ext_desc->item[item_num].item_cont, p_buf, MAX_ITEM_CONT_LEN);
        }
        item_num ++;   
      }
      
      p_buf += item_len;
      CHECK_FAIL_RET_CODE(len_of_items >= item_len);
      len_of_items -= item_len;
    }
  }
  
  CHECK_FAIL_RET_CODE(len_of_items == 0);
  
  //Set total item number in current extend event desc
  p_ext_desc->item_num = item_num;
  
  //Set text information
  txt_len = p_buf[0];
  if(txt_len == 0)
  {
    return ERR_FAILURE;
  }
  
  p_buf ++;

  if(txt_len > MAX_EXT_TXT_LEN)
  {
    txt_len = MAX_EXT_TXT_LEN;
  }

  //if(txt_len > MAX_EXT_TXT_LEN)
  {
    p_ext_txt->txt_len = txt_len;
  }
  
  //Set 0 to the end of txt string
  memcpy(p_ext_txt->ext_txt, p_buf, txt_len * sizeof(u8));
  p_ext_txt->evt_id =  event_id;
  return SUCCESS;
}

static void parse_time_shifted_evt_des(u8 *p_buf, time_shifted_evt_descr_t *p_des)
{
  p_des->svc_id = MAKE_WORD2(p_buf[1], p_buf[2]);
  p_des->evt_id = MAKE_WORD2(p_buf[3], p_buf[4]);
}

/*! This API is added by Chandler */
u16 eit_section_processing(u8 *p_buf_addr, eit_t *p_eit_info)
{
  u16    cnt     = 0 ;
  u16    lenth   = 0 ;
  u16    desloop = 0 ;
  u16    size    = 0 ;
  u8     *p_inbuf  = NULL;
  
  u8     tot_evt_num = 0;
  u8     tot_ext_info_num = 0;
  u8     tot_cont_num = 0;
  u8     tot_sht_txt_num = 0;
  u8     tot_ext_txt_num = 0;

  u16    event_id = 0;
  sht_evt_desc_t *p_eit_desc = NULL;
  time_shifted_evt_descr_t *p_time_shifted_evt_des = NULL;
  BOOL is_err = FALSE;
  
  CHECK_FAIL_RET_ZERO(p_buf_addr != NULL);
  p_inbuf = p_buf_addr;

  /*!
    Begin to parse eit schedule section
    */
  CHECK_FAIL_RET_ZERO(p_eit_info != NULL);
  p_eit_info->tot_evt_num = 0;

  //Table id
  p_eit_info->table_id = p_inbuf[cnt];
  cnt++;
  
  //Section length
  p_eit_info->sec_length = SYS_GET_LOW_HALF_BYTE(p_inbuf[cnt++])<<8;
  p_eit_info->sec_length += p_inbuf[cnt++] ;
  p_eit_info->sec_length &= 0xFFF ;
  
  p_eit_info->tot_evt_num = 0;
  p_eit_info->tot_ext_txt_num = 0;
  p_eit_info->tot_sht_txt_num = 0;
  p_eit_info->tot_ext_info_num = 0;


  p_eit_info->svc_id  = p_inbuf[cnt++]<<8 ;
  p_eit_info->svc_id += p_inbuf[cnt++];

  p_eit_info->version = (p_inbuf[cnt] >> 1) & 0x1f;
  p_eit_info->pf_flag = MASK_FIRST_BIT_OF_BYTE(p_inbuf[cnt++]) ;

  p_eit_info->section_number = p_inbuf[cnt++] ;
  p_eit_info->last_section_number = p_inbuf[cnt++] ;

  p_eit_info->stream_id = p_inbuf[cnt++]<<8 ;
  p_eit_info->stream_id += p_inbuf[cnt++];

  p_eit_info->org_nw_id = p_inbuf[cnt++] << 8 ;
  p_eit_info->org_nw_id += p_inbuf[cnt++];  

  //Segment last section number
  p_eit_info->seg_last_sec_number = p_inbuf[cnt];
  cnt++;

  p_eit_info->last_table_id       = p_inbuf[cnt];
  cnt++;

 if(p_eit_info->sec_length < EIT_MIN_LEN)  
  {
    /*!
     Section length is too short
     */
    //mdl_dbg(("Invalid section\n"));
    return 0;    
  }

  lenth = (p_eit_info->sec_length - 11 - CRC_SIZE);

  while(lenth > 0)
  {
  	is_err = FALSE;
    if(tot_evt_num > MAX_EVT_PER_SEC)
    {
      break;
    }
    //Get current short event desc
    p_eit_desc = &p_eit_info->sht_evt_info[tot_evt_num];
    
    if(lenth < EIT_SEC_HEAD_LEN)
    {
      break;
    }

    //Get event id
    p_eit_desc->evt_id  = (p_inbuf[cnt++] << 8);
    p_eit_desc->evt_id +=  p_inbuf[cnt++];
    event_id = p_eit_desc->evt_id;
    
    //Start time
    date_trans(&p_inbuf[cnt], &p_eit_desc->st_tm);
    cnt += 5;  
    
    //Drt time
    time_point_trans(&p_inbuf[cnt], &p_eit_desc->drt_time);
    cnt += 3;
    
    //running status
    p_eit_desc->running_status = ((p_inbuf[cnt] & 0xE0) >> 5);
    
    desloop = SYS_GET_LOW_HALF_BYTE(p_inbuf[cnt++]) << 8;
    desloop += p_inbuf[cnt++];
    desloop &= 0xFFF;
    size = desloop ;
    
    if(size > (lenth - EIT_SEC_HEAD_LEN))
    {
      break;
    }    

    while(desloop > 0)
    {
      switch(p_inbuf[cnt])
      {  
        case DVB_DESC_CONTENT:
          {
            cnt++;       
            
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              memset((u8 *)p_eit_desc, 0, sizeof(sht_evt_desc_t));
              //p_eit_info->tot_evt_num  --;
              break;
            } 

            //CHECK_FAIL_RET_ZERO(tot_cont_num < MAX_EVT_PER_SEC);
            if(tot_cont_num < MAX_EVT_PER_SEC)
            {
              parse_cont_desc(&p_eit_info->cont_desc[tot_cont_num], 
                      event_id, &p_inbuf[cnt + 1]);                    
            }
            else
            {
              break;
            }
            tot_cont_num ++;
          }
          break;
        case DVB_DESC_SHORT_EVENT:
          {
            sht_evt_txt_t *p_cur_txt = NULL;
            cnt++;
            
            //CHECK_FAIL_RET_ZERO(tot_sht_txt_num < MAX_SHT_TXT_NUM);
            
            if(tot_sht_txt_num >= MAX_SHT_TXT_NUM)
            {
              break;
            }
            p_cur_txt = &p_eit_info->sht_txt[tot_sht_txt_num];
            
            if(p_inbuf[cnt] > (desloop - 2))
            {       
              desloop = 0;
              //p_eit_info->evt_cnt--;
              break;
            }  

            //Set language code       
            memcpy(p_cur_txt->lang_code, &p_inbuf[cnt + 1], 
                                             LANGUAGE_CODE_LEN * sizeof(u8));
                                             
            if(parse_sht_evt(p_eit_desc, p_cur_txt, event_id, &p_inbuf[cnt + 4])
              == SUCCESS)
            {
              tot_sht_txt_num ++;
            }
          }
          break;
        case DVB_DESC_COMPONENT_DESC:
          {
            cnt++;
            
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              memset((u8 *)p_eit_desc, 0, sizeof(sht_evt_desc_t));
              //p_eit_info->evt_cnt --;
              break;
            }        
            
            //Copy language code out
            //memcpy(p_eit_desc->lang_code, &p_inbuf[cnt + 4], 
            //LANGUAGE_CODE_LEN * sizeof(u8));
          }
          break;
        case DVB_DESC_EXT_EVENT:
          {
            ext_evt_desc_t *p_cur_desc = NULL;
            ext_evt_txt_t *p_cur_txt = NULL;
            
            /*!
              Skip tag
              */
            cnt++;

            //CHECK_FAIL_RET_ZERO(tot_ext_txt_num < MAX_EXT_TXT_NUM);
            if(tot_ext_txt_num >= MAX_EXT_TXT_NUM)
            {
              break;
            }     
            p_cur_txt = &p_eit_info->ext_txt[tot_ext_txt_num];

            //CHECK_FAIL_RET_ZERO(tot_ext_info_num < MAX_EXT_DESC_NUM); 
            if(tot_ext_info_num >= MAX_EXT_DESC_NUM)  
            {
              break;
            }
            p_cur_desc  = &p_eit_info->ext_evt_info[tot_ext_info_num];
            
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              memset((u8 *)p_eit_desc, 0, sizeof(sht_evt_desc_t));
              //p_eit_info->evt_cnt --;
              break;
            }   
            
            parse_ext_desc(p_cur_desc, p_cur_txt, event_id, &p_inbuf[cnt + 1]);
            if(p_cur_desc->item_num > 0)
            {
              tot_ext_info_num ++;
            }

            if(p_cur_txt->txt_len > 1)
            {
              tot_ext_txt_num ++;
            }
          }
          break;
        case DVB_DESC_TIME_SHIFTED_EVT:
          {
            cnt ++;
            p_eit_desc->time_shifted_evt_des_found = 1;
            p_time_shifted_evt_des = &p_eit_desc->time_shifted_evt_des;
            parse_time_shifted_evt_des(&p_inbuf[cnt], p_time_shifted_evt_des);
          }
          break;
#if 1
        case DVB_DESC_PARENTAL_RATING:
          {
            cnt++;
             if(p_inbuf[cnt] > (desloop - 2))
              {
                desloop = 0;
                //memset((u8*)p_eit_desc, 0, sizeof(sht_evt_desc_t));
                break;
              }    
            memcpy(p_eit_desc->parental_rating.country_code, &p_inbuf[cnt + 1], 
                                             LANGUAGE_CODE_LEN * sizeof(u8));
            p_eit_desc->parental_rating.rating = p_inbuf[cnt + 4];
          }
          break;    
#endif          
        default:
          {
            //Skip tag
            cnt++;
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              //p_eit_info->evt_cnt--;
              break;
            }         
          }
          break;
      }
	  if(0 == desloop)
	 {
	 	is_err = TRUE;
		break;
	}
      desloop -= (p_inbuf[cnt] + 2);
      cnt += (p_inbuf[cnt]+ 1);
    }
    
    desloop = 0;
    lenth -= (12 + size);
	if(TRUE != is_err)
	{
  		tot_evt_num++;
	}
  }
  p_eit_info->tot_cont_num = tot_cont_num;
  p_eit_info->tot_evt_num = tot_evt_num;
  p_eit_info->tot_ext_txt_num = tot_ext_txt_num;
  p_eit_info->tot_sht_txt_num = tot_sht_txt_num;
  p_eit_info->tot_ext_info_num = tot_ext_info_num;

  return p_eit_info->sec_length;
}

/*! This api has been modified by Chandler*/
void parse_eit(handle_t handle, dvb_section_t *p_sec)
{  
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_svc = (service_t *)handle;
  u16 cnt = 0 ;
  u8 *p_inbuf = NULL;
  eit_t eit = {0};
  os_msg_t msg = {0};

  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL); 
  p_inbuf = p_sec->p_buffer ;
  if(p_inbuf[cnt]!= p_sec->table_id)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;    
  }    
  eit_section_processing(p_inbuf, &eit);
  CHECK_FAIL_RET_VOID(eit.tot_sht_txt_num <= MAX_SHT_TXT_NUM);
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.content = DVB_ONE_SEC_FOUND;
  msg.para1   = (u32)&eit;
  msg.para2   = sizeof(eit_t);
  //mdl_broadcast_msg(&msg);
 // OS_PRINTF("send eit msg\n");
#ifdef __LINUX__
  msg.is_ext = 1;
#endif
  p_svc->notify(p_svc, &msg);
}




/*! This API is added by Chandler */
u16 eit_pf_section_processing(u8 *p_buf_addr, eit_pf_t *p_eit_info)
{
  u16    cnt     = 0 ;
  u16    lenth   = 0 ;
  u16    desloop = 0 ;
  u16    size    = 0 ;
  u8     *p_inbuf  = NULL;
  
  u8     tot_evt_num = 0;
  u8     tot_ext_info_num = 0;
  u8     tot_cont_num = 0;
  u8     tot_sht_txt_num = 0;
  u8     tot_ext_txt_num = 0;

  u16    event_id = 0;
  sht_evt_desc_t *p_eit_desc = NULL;
  time_shifted_evt_descr_t *p_time_shifted_evt_des = NULL;
  BOOL is_err = FALSE;
  
  CHECK_FAIL_RET_ZERO(p_buf_addr != NULL);
  p_inbuf = p_buf_addr;

  /*!
    Begin to parse eit schedule section
    */
  CHECK_FAIL_RET_ZERO(p_eit_info != NULL);
//  p_eit_info->tot_evt_num = 0;

  //Table id
  p_eit_info->table_id = p_inbuf[cnt];
  cnt++;
  
  //Section length
  p_eit_info->sec_length = SYS_GET_LOW_HALF_BYTE(p_inbuf[cnt++])<<8;
  p_eit_info->sec_length += p_inbuf[cnt++] ;
  p_eit_info->sec_length &= 0xFFF ;
  
//  p_eit_info->tot_evt_num = 0;
//  p_eit_info->tot_ext_txt_num = 0;
//  p_eit_info->tot_sht_txt_num = 0;
//  p_eit_info->tot_ext_info_num = 0;


  p_eit_info->svc_id  = p_inbuf[cnt++]<<8 ;
  p_eit_info->svc_id += p_inbuf[cnt++];

  p_eit_info->pf_flag = MASK_FIRST_BIT_OF_BYTE(p_inbuf[cnt++]) ;

  p_eit_info->section_number = p_inbuf[cnt++] ;
  p_eit_info->last_section_number = p_inbuf[cnt++] ;

  p_eit_info->stream_id = p_inbuf[cnt++]<<8 ;
  p_eit_info->stream_id += p_inbuf[cnt++];

  p_eit_info->org_nw_id = p_inbuf[cnt++] << 8 ;
  p_eit_info->org_nw_id += p_inbuf[cnt++];  

  //Segment last section number
  p_eit_info->seg_last_sec_number = p_inbuf[cnt];
  cnt++;

  p_eit_info->last_table_id       = p_inbuf[cnt];
  cnt++;

 if(p_eit_info->sec_length < EIT_MIN_LEN)  
  {
    /*!
     Section length is too short
     */
    //mdl_dbg(("Invalid section\n"));
    return 0;    
  }

  lenth = (p_eit_info->sec_length - 11 - CRC_SIZE);

  while(lenth > 0)
  {
  	is_err = FALSE;
    if(tot_evt_num > MAX_EVT_PER_SEC)
    {
      break;
    }
    //Get current short event desc
    p_eit_desc = &p_eit_info->sht_evt_info;
    
    if(lenth < EIT_SEC_HEAD_LEN)
    {
      break;
    }

    //Get event id
    p_eit_desc->evt_id  = (p_inbuf[cnt++] << 8);
    p_eit_desc->evt_id +=  p_inbuf[cnt++];
    event_id = p_eit_desc->evt_id;
    
    //Start time
    date_trans(&p_inbuf[cnt], &p_eit_desc->st_tm);
    cnt += 5;  
    
    //Drt time
    time_point_trans(&p_inbuf[cnt], &p_eit_desc->drt_time);
    cnt += 3;

    p_eit_desc->running_status = ((p_inbuf[cnt] & 0xE0) >> 5);
    
    desloop = SYS_GET_LOW_HALF_BYTE(p_inbuf[cnt++]) << 8;
    desloop += p_inbuf[cnt++];
    desloop &= 0xFFF;
    size = desloop ;
    
    if(size > (lenth - EIT_SEC_HEAD_LEN))
    {
      break;
    }    

    while(desloop > 0)
    {
      switch(p_inbuf[cnt])
      {  
        case DVB_DESC_CONTENT:
          {
            cnt++;       
            
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              memset((u8 *)p_eit_desc, 0, sizeof(sht_evt_desc_t));
              //p_eit_info->tot_evt_num  --;
              break;
            } 

            //CHECK_FAIL_RET_ZERO(tot_cont_num < MAX_EVT_PER_SEC);
            if(tot_cont_num < MAX_EVT_PER_SEC)
            {
//              parse_cont_desc(&p_eit_info->cont_desc[tot_cont_num], 
//                      event_id, &p_inbuf[cnt + 1]);                    
            }
            else
            {
              break;
            }
            tot_cont_num ++;
          }
          break;
        case DVB_DESC_SHORT_EVENT:
          {
            sht_evt_txt_t *p_cur_txt = NULL;
            cnt++;
            
            //CHECK_FAIL_RET_ZERO(tot_sht_txt_num < MAX_SHT_TXT_NUM);
            
            if(tot_sht_txt_num >= MAX_SHT_TXT_NUM)
            {
              break;
            }
            p_cur_txt = &p_eit_info->sht_txt;
            
            if(p_inbuf[cnt] > (desloop - 2))
            {       
              desloop = 0;
              //p_eit_info->evt_cnt--;
              break;
            }  

            //Set language code       
            memcpy(p_cur_txt->lang_code, &p_inbuf[cnt + 1], LANGUAGE_CODE_LEN);
                                             
            if(parse_pf_sht_evt(p_eit_desc, p_cur_txt, event_id, &p_inbuf[cnt + 4])
              == SUCCESS)
            {
              tot_sht_txt_num ++;
            }
          }
          break;
        case DVB_DESC_COMPONENT_DESC:
          {
            cnt++;
            
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              memset((u8 *)p_eit_desc, 0, sizeof(sht_evt_desc_t));
              //p_eit_info->evt_cnt --;
              break;
            }        
            
            //Copy language code out
            //memcpy(p_eit_desc->lang_code, &p_inbuf[cnt + 4], 
            //LANGUAGE_CODE_LEN * sizeof(u8));
          }
          break;
        case DVB_DESC_EXT_EVENT:
          {
            /*!
              Skip tag
              */
            cnt++;
            //CHECK_FAIL_RET_ZERO(tot_ext_txt_num < MAX_EXT_TXT_NUM);
            if(tot_ext_txt_num >= MAX_EXT_TXT_NUM)
            {
              break;
            }     
//            p_cur_txt = &p_eit_info->ext_txt[tot_ext_txt_num];

            //CHECK_FAIL_RET_ZERO(tot_ext_info_num < MAX_EXT_DESC_NUM); 
            if(tot_ext_info_num >= MAX_EXT_DESC_NUM)  
            {
              break;
            }
//            p_cur_desc  = &p_eit_info->ext_evt_info[tot_ext_info_num];
            
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              memset((u8 *)p_eit_desc, 0, sizeof(sht_evt_desc_t));
              //p_eit_info->evt_cnt --;
              break;
            } 
          }
          break;
        case DVB_DESC_TIME_SHIFTED_EVT:
          {
            cnt ++;
            p_eit_desc->time_shifted_evt_des_found = 1;
            p_time_shifted_evt_des = &p_eit_desc->time_shifted_evt_des;
            parse_time_shifted_evt_des(&p_inbuf[cnt], p_time_shifted_evt_des);
          }
          break;
        default:
          {
            //Skip tag
            cnt++;
            if(p_inbuf[cnt] > (desloop - 2))
            {
              desloop = 0;
              //p_eit_info->evt_cnt--;
              break;
            }         
          }
          break;
      }

	 if(0 == desloop)
	 {
	 	is_err = TRUE;
		break;
	}

      desloop -= (p_inbuf[cnt] + 2);
      cnt += (p_inbuf[cnt]+ 1);
    }
    
    desloop = 0;
    lenth -= (12 + size);
	if(TRUE != is_err)
	{
  		tot_evt_num++;
	}
  }

//  p_eit_info->tot_cont_num = tot_cont_num;
//  p_eit_info->tot_evt_num = tot_evt_num;
//  p_eit_info->tot_ext_txt_num = tot_ext_txt_num;
//  p_eit_info->tot_sht_txt_num = tot_sht_txt_num;
//  p_eit_info->tot_ext_info_num = tot_ext_info_num;

  return p_eit_info->sec_length;
}

void parse_eit_pf(handle_t handle, dvb_section_t *p_sec)
{  
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_svc = (service_t *)handle;
  u16 cnt = 0 ;
  u8 *p_inbuf = NULL;
  eit_pf_t eit = {0};
  os_msg_t msg = {0};

  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL); 
  p_inbuf = p_sec->p_buffer ;
  if(p_inbuf[cnt]!= p_sec->table_id)
  {
    p_dvb_handle->filter_data_error(p_svc, p_sec);
    return;    
  }    
  eit_pf_section_processing(p_inbuf, &eit);
//  CHECK_FAIL_RET_VOID(eit.tot_sht_txt_num <= MAX_SHT_TXT_NUM);
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.content = DVB_ONE_SEC_FOUND;
  msg.para1   = (u32)&eit;
  msg.para2   = sizeof(eit_t);
  //mdl_broadcast_msg(&msg);
  p_svc->notify(p_svc, &msg);
}

void request_eit_single_mode_2(dvb_section_t *p_sec, u32 para1, 
u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  //u32 sec_num  = para2;
  u16 svc_id = para1 & 0xffff;
  /*!
    para1 and para2 should be used and one of them should be table id
    */
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);
  
  p_sec->pid = DVB_EIT_PID ;
  p_sec->timeout = (u32)EIT_TIMEOUT;
  p_sec->table_id = para1 >> 24;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,sizeof(u8) * MAX_FILTER_MASK_BYTES);  
  
  p_sec->filter_mode = FILTER_TYPE_SECTION; 
  p_sec->filter_code[0] = p_sec->table_id;
  p_sec->filter_mask[0] = 0xFF;
  
#ifndef WIN32  
  //Two byte have been kicked out by hardware PTI
  p_sec->filter_code[1] = svc_id >> 8;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = svc_id & 0xff;
  p_sec->filter_mask[2] = 0xFF;

  p_sec->filter_code[4] =  para2 & 0xff;  //P/F  0/1
  p_sec->filter_mask[4] = 0xFF;

  p_sec->filter_mask_size = 5;
#else
  p_sec->filter_code[3] = svc_id >> 8;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = svc_id & 0xff;
  p_sec->filter_mask[4] = 0xFF;

  p_sec->filter_code[6] =  para2 & 0xff;  //P/F  0/1
  p_sec->filter_mask[6] = 0xFF;
  
  p_sec->filter_mask_size = 7;  
#endif

  p_sec->interval_time = 0;
  p_sec->crc_enable = 1;
  p_sec->direct_data = TRUE;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

void request_eit_single_mode(dvb_section_t *p_sec, u32 table_id, 
u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  //u32 sec_num  = para2;
  /*!
    para1 and para2 should be used and one of them should be table id
    */
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);
  
  p_sec->pid = DVB_EIT_PID ;
  p_sec->timeout = (u32)EIT_TIMEOUT;
  p_sec->table_id = table_id;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,sizeof(u8) * MAX_FILTER_MASK_BYTES);  
  
  p_sec->filter_mode = FILTER_TYPE_SECTION; 
  p_sec->filter_code[0] = p_sec->table_id;
  p_sec->filter_mask[0] = 0xFF;
  
#ifndef WIN32  
  //Two byte have been kicked out by hardware PTI
  //p_sec->filter_code[4]   = (u8)(sec_num);
  //p_sec->filter_mask[4]   = 0xFF;  
  p_sec->filter_mask_size = 1;
#else
  //p_sec->filter_code[6]  = (u8)(sec_num);
  //p_sec->filter_mask[6]  = 0xFF;  
  p_sec->filter_mask_size = 1;  
#endif

  p_sec->interval_time = 0;
  p_sec->crc_enable = 1;
  p_sec->direct_data = TRUE;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}


void request_eit_multi_mode(dvb_section_t *p_sec, u32 table_id, u32 para2)
{
  filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t *)para2;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  
  //Set PID information
  p_sec->pid      = DVB_EIT_PID;
  p_sec->table_id = table_id;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = table_id;
  p_sec->filter_mask[0] = 0xFF;
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_mask_size = 1;
  p_sec->timeout = (u32)EIT_TIMEOUT;

  //Attach buffer list
  p_sec->p_ext_buf_list = p_ext_buf_list;
  p_sec->crc_enable = 1;
  p_sec->direct_data = TRUE;
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

void request_eit_multi_mode_2(dvb_section_t *p_sec, u32 table_id, u32 para2)
{
  filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t *)para2;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  
  //Set PID information
  p_sec->pid      = DVB_EIT_PID;
  p_sec->table_id = table_id;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = table_id;
  p_sec->filter_mask[0] = 0xFF;
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_mask_size = 1;
  p_sec->timeout = (u32)EIT_TIMEOUT;

  //Attach buffer list
  p_sec->p_ext_buf_list = p_ext_buf_list;
  p_sec->crc_enable = 1;
  p_sec->direct_data = TRUE;
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}




