/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
//std
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdio.h"
#include "ctype.h"
#include "stdarg.h"
// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"


//util
#include "lib_rect.h"
#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"

#include "class_factory.h"
#include "common.h"
#include "drv_dev.h"
#include "charsto.h"
#include "fcrc.h"
//#include "lzma.h"
#include "lzmaif.h"
#include "md5_sign.h"
#include "mdl.h"
#include "nim.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"

#include "dvb_protocol.h"

#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"
#include "output_pin.h"

#include "transf_input_pin.h"
#include "transf_output_pin.h"

#include "ifilter.h"
#include "common_filter.h"
#include "transf_filter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"


#include "dmx.h"
#include "demux_interface.h"
#include "demux_filter.h"

#include "ota_public_def.h"
#include "ota_filter.h"
#include "ota_input_pin_intra.h"

#include "ota_output_pin_intra.h"
#include "ota_filter_intra.h"
#include "ota_filter_dsmcc.h"


/*!
  ota filter get private
  */
static ota_filter_priv_t *ota_filter_get_priv(handle_t _this)
{

  MT_ASSERT(_this != NULL);
  return &(((ota_filter_t *)_this)->m_priv_data);
}

/*!
  ota filter debug printf porting,the function run by project (intra).
  */
void ota_filter_debug_printf(ota_filter_priv_t *p_priv,
                                        ota_debug_level_t level,
                                        const char *p_fmt, ...)
{
  char buffer[MAX_PRINT_BUFFER_LEN] = {0};
  p_print_list p_args = NULL;
  if(p_priv->debug_level < OTA_DEBUG_ALL)
  {
    return;
  }
  if(level >= p_priv->debug_level)
  {

     if(p_priv->ota_filter_debug_printf != NULL)
      {
        PRINTF_VA_START(p_args, p_fmt);
        ck_vsnprintf(buffer,sizeof(buffer), p_fmt, p_args);
        PRINTF_VA_END(p_args);
        p_priv->ota_filter_debug_printf("%s", buffer);

      }
  }
  return;
}

/*!
  API for allocating memory in OTA module
  \param[in] mem size memory size to be allocated with byte as unit
  \param[in] id partition id to be allocated
  */
static void *ota_filter_malloc(ota_filter_priv_t *p_priv,u32 mem_size)
{
  void *p_malloc = NULL;
  if(p_priv->use_memp == TRUE)
  {
    //return lib_memp_alloc(p_priv->p_mem_heap, mem_size);
    p_malloc =  lib_memp_alloc(p_priv->p_mem_heap, mem_size);
  }
  else
  {
   // return mtos_malloc(mem_size);
    p_malloc = mtos_malloc(mem_size);
  }
 // OS_PRINTF("@@@ota filter malloc addr[0x%x],size:%d\n",(u32)p_malloc,mem_size);
  return p_malloc;
}
   
/*!
  API for allocating memory in ota
  \param[in] mem size memory size to be allocated with byte as unit
  \param[in] id partition id to be allocated
  */
static RET_CODE ota_filter_free(ota_filter_priv_t *p_priv,void *p_buf)
{
  //OS_PRINTF("@@@ota filter malloc addr[0x%x]\n",(u32)p_buf);
  if(p_buf == NULL)
  {
     return SUCCESS;
  }
  if(p_priv->use_memp == TRUE)
  {
    return lib_memp_free(p_priv->p_mem_heap, p_buf);
  }
  else
  {
    mtos_free(p_buf);
    return SUCCESS;
  }
}
/*!
  ota filter send message out cb(intra).
  */
static void ota_filter_send_message_cb(ota_filter_priv_t *p_priv,
                                                        ota_filter_evt_t evt,
                                                        u32 param1,
                                                        u32 param2,
                                                        u32 context)
{
   ipin_t *p_ipin = (ipin_t *)&p_priv->m_in_pin;
   os_msg_t        msg    = {0};
   msg.para1 = param1;
   msg.para2 = param2;
   msg.content = evt;
   msg.context = context;
   p_ipin->send_message_out(p_ipin, &msg);
}


/*!
  ota filter get ota type(intra).
  */
static BOOL  ota_filter_check_ota_finish(ota_filter_priv_t *p_priv)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->ota_data[i].exist_bit == TRUE)
        && (p_priv->ota_data[i].upg_bit == TRUE)
       && (p_priv->ota_data[i].burned_flash_bit != TRUE))
      { 
        return FALSE;
      }
  }
  return TRUE;
}

/*!
  ota filter get ota data block(intra).
  */
static BOOL ota_filter_get_ota_data_block(ota_filter_priv_t *p_priv,u8 block_id,u8 *pos)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->ota_data[i].exist_bit == TRUE)
       && (p_priv->ota_data[i].id == block_id))
      { 
        *pos = i;
        return TRUE;
      }
  }
  return FALSE;
}
/*!
  ota filter get ota data block to free block(intra).
  */
static u8 ota_filter_get_ota_block_free_block_index(ota_filter_priv_t *p_priv)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if(p_priv->ota_block[i].exist_bit != TRUE)
      { 
        return i;
      }
  }
  return 0xff;
}
/*!
  ota filter get ota data block to free block(intra).
  */
static u8 ota_filter_get_ota_data_free_block_index(ota_filter_priv_t *p_priv)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if(p_priv->ota_data[i].exist_bit != TRUE)
      { 
        return i;
      }
  }
  return 0xff;
}

/*!
  ota filter check block_id from block groupblock(intra).
  */
static BOOL ota_filter_check_block_id_from_block_group(\
                                                     ota_block_info_t *p_ota_block,
                                                     u8 block_id,u8 *index)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_ota_block[i].exist_bit == TRUE)
       && (p_ota_block[i].id == block_id))
      { 
        *index = i;
        return TRUE;
      }
  }
  return FALSE;
}

/*!
  ota filter check block_id from block groupblock(intra).
  */
static BOOL ota_filter_check_upg_exist_from_block_group(\
                                    ota_block_info_t *p_ota_block)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_ota_block[i].exist_bit == TRUE)
       && (p_ota_block[i].upg_bit == TRUE))
      { 
        return TRUE;
      }
  }
  return FALSE;
}

/*!
  ota filter send message out (intra).
  */
static void ota_filter_send_message_out(ota_filter_priv_t *p_priv,
                                                            ota_filter_evt_t evt,
                                                            u32 param1,u32 param2,
                                                            u32 context)
{
   if((evt == OTA_FILTER_EVENT_CHECK_FAIL)
       || (evt == OTA_FILTER_EVENT_REQEUST_TIMEOUT)
       ||(evt == OTA_FILTER_EVENT_BURNFLASH_FAIL)
       || (evt == OTA_FILTER_EVENT_DOWNLOAD_FAIL))
    {
      if((p_priv->tri_mode == OTA_TRI_MODE_AUTO) 
      && (p_priv->max_fail_times > 0)
      && (p_priv->destroy_flag == FALSE))
      {
        p_priv->fail_times ++;
        if(p_priv->fail_times >= p_priv->max_fail_times)
        {
          p_priv->fail_times = 0;
          p_priv->tri_mode = OTA_TRI_MODE_NONE;
           ota_filter_send_message_cb(p_priv,
                OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
                p_priv->maincode_block_id,0,0);
           ota_filter_send_message_cb(p_priv,
                      OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_STATUS,
                      p_priv->tri_mode,0,0);
        }
       ota_filter_send_message_cb(p_priv,
                    OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME,
                    p_priv->fail_times,0,0);
      }
    }
   
   if((evt == OTA_FILTER_EVENT_BURNFLASH_SUCCESS)
      ||(evt == OTA_FILTER_EVENT_BURNFLASH_REBOOT))
    {
       /********all block is upg over?************/
       if(TRUE == ota_filter_check_ota_finish(p_priv))
        {
           p_priv->fail_times = 0;
           p_priv->tri_mode = OTA_TRI_MODE_NONE;
           ota_filter_send_message_cb(p_priv,
                      OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_STATUS,
                      p_priv->tri_mode,0,0);
           ota_filter_send_message_cb(p_priv,
                      OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME,
                      p_priv->fail_times,0,0);
         
         ota_filter_send_message_cb(p_priv,
                      OTA_FILTER_EVENT_BURNFLASH_SAVE_UPG_VERSION,
                      p_priv->upg_check_info.upg_new_version,0,0);
         ota_filter_send_message_cb(p_priv,
                    OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
                    p_priv->maincode_block_id,0,0);
        }
       
    }
  if(evt == OTA_FILTER_EVENT_NO_BLOCK_UPG)
  {
     if(p_priv->tri_mode != OTA_TRI_MODE_FORC)
      {
         p_priv->fail_times = 0;
         p_priv->tri_mode = OTA_TRI_MODE_NONE;
         
         ota_filter_send_message_cb(p_priv,
                    OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_STATUS,
                    p_priv->tri_mode,0,0);
         ota_filter_send_message_cb(p_priv,
                    OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME,
                    p_priv->fail_times,0,0);
         ota_filter_send_message_cb(p_priv,
                    OTA_FILTER_EVENT_BURNFLASH_SAVE_UPG_VERSION,
                    p_priv->upg_check_info.upg_new_version,0,0);
        ota_filter_send_message_cb(p_priv,
                    OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
                    p_priv->maincode_block_id,0,0); 
      }
  }
   
  ota_filter_send_message_cb(p_priv,evt,param1,param2,context);
}

/*!
  ota filter lock control,the function run by project (intra).
  */
static void ota_filter_process_lock_result(ota_filter_priv_t *p_priv,u32 lpara)
{
  if(lpara == OTA_FILTER_LOCK_STATUS_LOCKED)
  {
    ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_LOCK,0,0,0);
    p_priv->loop_sm = OTA_FILTER_SM_LOCKED;
  }
  else
  { 
     p_priv->loop_sm = OTA_FILTER_SM_UNLOCK;
    }
 }
/*!
  ota filter lock control,the function run by project (intra).
  */
static void ota_filter_lock_control(ota_filter_priv_t *p_priv,BOOL is_lock,u32 lpara,u32 *param)
{ 
   u8 lock_mode = 0;
   
   p_priv->data_pid = (lpara >> 16) & 0xffff;
   if((p_priv->g_direct_dl == TRUE)
      && (p_priv->ipin_cfg.filter_param.m_flag == 0x01)
      && (p_priv->data_pid != p_priv->ipin_cfg.filter_param.pid))
    {
      p_priv->data_pid = p_priv->ipin_cfg.filter_param.pid;
    }
   
   lock_mode = lpara & 0xff;
   
  if(TRUE == is_lock)
  {
    if(p_priv->ota_do_lock != NULL)
    {
        p_priv->ota_do_lock(TRUE,lock_mode,param);
    }
  }
  else
  {
    if(p_priv->ota_do_lock != NULL)
    {
        p_priv->ota_do_lock(FALSE,lock_mode,NULL);
    }
  }
}


/*!
 ota api do check tp lock.
 */
static BOOL ota_api_on_check_tp(ota_filter_priv_t *p_priv)
{
  u32 param1 = 0;
  u32 *p_tp_info = NULL;
  param1 = (p_priv->tp_info.do_ota_tp.data_pid << 16) 
                   | (p_priv->tp_info.do_ota_tp.lock_mode);
  
  switch(p_priv->tp_info.do_ota_tp.lock_mode)
  {
    case SYS_DVBC:
    p_tp_info = (u32 *)&p_priv->tp_info.do_ota_tp.lockc;
    break;
   case SYS_DVBS:
    p_tp_info = (u32 *)&p_priv->tp_info.do_ota_tp.locks;
    break;
    case SYS_DTMB:
    case SYS_DVBT2:
    p_tp_info = (u32 *)&p_priv->tp_info.do_ota_tp.lockt;
    break;
    default:
    ota_filter_debug_printf(p_priv,OTA_DEBUG_ASSERT,"[%s]:lock mode is err\n",__FUNCTION__);  
    MT_ASSERT(0);
    break;
  }

   ota_filter_lock_control(p_priv,TRUE,param1,p_tp_info);
    return TRUE;
}

/*!
 ota filter do auto lock setup.
 */
static BOOL ota_api_on_auto_lock_tp(ota_filter_priv_t *p_priv,u8 mode)
{
  u32 param1 = 0;
  u32 *p_tp_info = NULL;
  param1 = (p_priv->tp_info.do_ota_tp.data_pid << 16) 
                   | (p_priv->tp_info.do_ota_tp.lock_mode);
  
  switch(p_priv->tp_info.do_ota_tp.lock_mode)
  {
    case SYS_DVBC:
    p_tp_info = (u32 *)&p_priv->tp_info.do_ota_tp.lockc;
    break;
   case SYS_DVBS:
    p_tp_info = (u32 *)&p_priv->tp_info.do_ota_tp.locks;
    break;
    case SYS_DTMB:
    case SYS_DVBT2:
    p_tp_info = (u32 *)&p_priv->tp_info.do_ota_tp.lockt;
    break;
    default:
    ota_filter_debug_printf(p_priv,OTA_DEBUG_ASSERT,"[%s]:lock mode is err\n",__FUNCTION__);  
    MT_ASSERT(0);
    break;
  }
  
  if((p_priv->lock_type == OTA_FILTER_LOCK_TP_NOTHING) && (mode == 0))
  {
    ota_filter_lock_control(p_priv,TRUE,param1,p_tp_info);   
    p_priv->lock_type = OTA_FILTER_LOCK_TP_DO_TP;
    return TRUE;
  }
  if(mode == 1)
  {
    if(p_priv->lock_type == OTA_FILTER_LOCK_TP_DO_TP)
    {
    if(((p_priv->tp_info.do_ota_tp.lockc.tp_freq != p_priv->tp_info.ota_set_tp.lockc.tp_freq)
    && (SYS_DVBC == p_priv->tp_info.do_ota_tp.lock_mode))
    ||((p_priv->tp_info.do_ota_tp.locks.tp_rcv.freq != p_priv->tp_info.ota_set_tp.locks.tp_rcv.freq)
    && (SYS_DVBS == p_priv->tp_info.do_ota_tp.lock_mode))
    ||((p_priv->tp_info.do_ota_tp.lockt.tp_freq != p_priv->tp_info.ota_set_tp.lockt.tp_freq)
    && ((SYS_DTMB == p_priv->tp_info.do_ota_tp.lock_mode)
          || (SYS_DVBT2 == p_priv->tp_info.do_ota_tp.lock_mode))))
        
  {
     switch(p_priv->tp_info.ota_set_tp.lock_mode)
    {
      case SYS_DVBC:
      p_tp_info = (u32 *)&p_priv->tp_info.ota_set_tp.lockc;
      break;
      case SYS_DVBS:
      p_tp_info = (u32 *)&p_priv->tp_info.ota_set_tp.locks;
      break;
      case SYS_DTMB:
      case SYS_DVBT2:
      p_tp_info = (u32 *)&p_priv->tp_info.ota_set_tp.lockt;
      break;
      default:
      ota_filter_debug_printf(p_priv,
                            OTA_DEBUG_ASSERT,
                            "[%s]:lock mode is err\n",
                            __FUNCTION__);  
      MT_ASSERT(0);
      break;
    }
   ota_filter_lock_control(p_priv,TRUE,param1,p_tp_info); 
    p_priv->lock_type = OTA_FILTER_LOCK_TP_SET_TP;
    return TRUE;
  }
    else
    {
      if(((p_priv->tp_info.ota_set_back_tp.lockc.tp_freq != 
                                                p_priv->tp_info.ota_set_tp.lockc.tp_freq)
        && (SYS_DVBC == p_priv->tp_info.do_ota_tp.lock_mode))
    ||((p_priv->tp_info.ota_set_back_tp.locks.tp_rcv.freq != 
                                            p_priv->tp_info.ota_set_tp.locks.tp_rcv.freq)
        && (SYS_DVBS == p_priv->tp_info.do_ota_tp.lock_mode))
    ||((p_priv->tp_info.ota_set_back_tp.lockt.tp_freq != 
                                                    p_priv->tp_info.ota_set_tp.lockt.tp_freq)
         && ((SYS_DTMB == p_priv->tp_info.do_ota_tp.lock_mode)
                || (SYS_DVBT2 == p_priv->tp_info.do_ota_tp.lock_mode))))
      {
          switch(p_priv->tp_info.ota_set_tp.lock_mode)
          {
            case SYS_DVBC:
            p_tp_info = (u32 *)&p_priv->tp_info.ota_set_back_tp.lockc;
            break;
            case SYS_DVBS:
            p_tp_info = (u32 *)&p_priv->tp_info.ota_set_back_tp.locks;
            break;
            case SYS_DTMB:
            case SYS_DVBT2:
            p_tp_info = (u32 *)&p_priv->tp_info.ota_set_back_tp.lockt;
            break;
            default:
            ota_filter_debug_printf(p_priv,
                          OTA_DEBUG_ASSERT,
                          "[%s]:lock mode is err\n",
                          __FUNCTION__);  
            MT_ASSERT(0);
            break;
          }
          ota_filter_lock_control(p_priv,TRUE,param1,p_tp_info); 
          p_priv->lock_type = OTA_FILTER_LOCK_TP_BACK_TP;
         return TRUE;
      }  
      p_priv->lock_type = OTA_FILTER_LOCK_TP_BACK_TP;
    }

    }
      else if(p_priv->lock_type == OTA_FILTER_LOCK_TP_SET_TP)
    {
      if(((p_priv->tp_info.do_ota_tp.lockc.tp_freq != 
                                  p_priv->tp_info.ota_set_back_tp.lockc.tp_freq)
          && (p_priv->tp_info.ota_set_back_tp.lockc.tp_freq != 
                                          p_priv->tp_info.ota_set_tp.lockc.tp_freq)
          && (SYS_DVBC == p_priv->tp_info.do_ota_tp.lock_mode))
      ||((p_priv->tp_info.do_ota_tp.locks.tp_rcv.freq != 
                              p_priv->tp_info.ota_set_back_tp.locks.tp_rcv.freq)
          && (p_priv->tp_info.ota_set_back_tp.locks.tp_rcv.freq != 
                                      p_priv->tp_info.ota_set_tp.locks.tp_rcv.freq)
          && (SYS_DVBS == p_priv->tp_info.do_ota_tp.lock_mode))
      ||((p_priv->tp_info.do_ota_tp.lockt.tp_freq != 
                                  p_priv->tp_info.ota_set_back_tp.lockt.tp_freq)
           &&(p_priv->tp_info.ota_set_back_tp.lockt.tp_freq != 
                                              p_priv->tp_info.ota_set_tp.lockt.tp_freq)
           && ((SYS_DTMB == p_priv->tp_info.do_ota_tp.lock_mode)
                   || (SYS_DVBT2 == p_priv->tp_info.do_ota_tp.lock_mode))))
      
    {
        switch(p_priv->tp_info.ota_set_tp.lock_mode)
        {
            case SYS_DVBC:
            p_tp_info = (u32 *)&p_priv->tp_info.ota_set_back_tp.lockc;
            break;
            case SYS_DVBS:
            p_tp_info = (u32 *)&p_priv->tp_info.ota_set_back_tp.locks;
            break;
            case SYS_DTMB:
            case SYS_DVBT2:
            p_tp_info = (u32 *)&p_priv->tp_info.ota_set_back_tp.lockt;
            break;
            default:
            ota_filter_debug_printf(p_priv,
                                  OTA_DEBUG_ASSERT,
                                  "[%s]:lock mode is err\n",
                                  __FUNCTION__);  
            MT_ASSERT(0);
            break;
        }
       ota_filter_lock_control(p_priv,TRUE,param1,p_tp_info); 
       p_priv->lock_type = OTA_FILTER_LOCK_TP_BACK_TP;
       return TRUE;
    }
     p_priv->lock_type = OTA_FILTER_LOCK_TP_BACK_TP;
    }

    }
    return FALSE;
  }

#if 0
/*!
  ota filter request dsmcc (intra).
  */
static BOOL  ota_filter_request_dsmcc_msg_dii(ota_filter_priv_t *p_priv)
{
   ota_input_pin_t *p_ipin = &p_priv->m_in_pin;
   u32 param = 0;
   u8 i = 0;
   
   for(i = 0;i < OTA_MAX_BLOCK_NUMBER;i++)
    {
     if((p_priv->ota_data[i].exist_bit == TRUE) 
          && (p_priv->ota_data[i].dl_group_bit == FALSE))
      {
         break;
      }
    }
  if(i >= OTA_MAX_BLOCK_NUMBER)
  {
       return FALSE;
  }
 dsmcc_parse_clear_dii_info();
 p_priv->group_id = p_priv->ota_data[i].group_id;
 param = (p_priv->data_pid << 16) |p_priv->group_id;
 p_ipin->ota_input_pin_reques(p_ipin,REQUES_TABLE_DSMCC_MSG_DII,param);
 return TRUE;
}
#endif
/*!
  ota filter request dsmcc  date(intra).
  */
static BOOL  ota_filter_request_all_dsmcc_data(ota_filter_priv_t *p_priv,upg_check_info_t *p_info)
{
  ota_input_pin_t *p_ipin = &p_priv->m_in_pin;
  u32 param = 0;

  param = (p_priv->data_pid << 16) |p_info->dsi_group_id;
  p_priv->group_id = p_info->dsi_group_id;

  p_priv->save_size = p_info->upg_file_size;
  OS_PRINTF("[%s] save size = %d\n",__FUNCTION__,p_priv->save_size);
  OS_PRINTF("[%s] info size size = %d\n",__FUNCTION__,p_info->upg_file_size);
  ota_filter_debug_printf(p_priv,
                        OTA_DEBUG_INFO,
                        "[%s]:save buffer size = %d\n",
                        __FUNCTION__,
                        p_priv->save_size);  
  p_priv->p_save_buf = (u8 *)ota_filter_malloc(p_priv,p_priv->save_size);
  
  if(p_priv->p_save_buf == NULL)
  {
     return FALSE;
  }
  memset(p_priv->p_save_buf,0,p_priv->save_size);
  
  dsmcc_download_init(p_priv->p_save_buf,p_priv->save_size);
  parse_dsmcc_reset_db_dl_info();
  p_ipin->ota_input_pin_reques(p_ipin,REQUES_TABLE_DSMCC_DMM,param);
  return TRUE;
}

/*!
  ota filter get lzma file size
  */
u32 ota_filter_get_lzma_file_size(u8 *p_buffer)
{
   /****LZMA HEAD INFO: LZMA parament (5 byte),data size (8 BYTE)****/
   u32 datasize = 0;
   u8 byte[4] = {0};
   if(NULL != p_buffer)
    {
       memcpy(byte,p_buffer + 5,4);
       datasize = byte[0] + (byte[1] << 8) + (byte[2] << 16) + (byte[3] << 24);
    }
   return datasize;
}

/*!
  dsmcc para process download data
  */
static BOOL ota_filter_md5_sign_data(ota_filter_priv_t *p_priv,
                              ota_md5_data_sign_t md5_function)
{
  u8 *p_md5_buf = NULL;
  u32 md5_mark = 0;
  u32 md5_distance = 0;
  BOOL ret = FALSE;
  u32 data_crc = 1234;
  u32 i = 0;
  md5_ex_head_info_t md5_head_info;
  ota_special_info_t ota_info;
  
  memset(&md5_head_info,0,sizeof(md5_ex_head_info_t));
  memset(&ota_info,0,sizeof(ota_special_info_t));

  p_md5_buf = p_priv->p_save_buf + p_priv->save_size - 1024;
  for(i = 1024;i >= 3;i --)
  {
     md5_mark = ((p_md5_buf[i] << 24) | (p_md5_buf[i - 1] << 16)
                         | (p_md5_buf[i - 2] << 8) | (p_md5_buf[i - 3]));
     if(md5_mark == MD5_HEAD_MARK)
      {
        p_md5_buf = (u8 *)&(p_md5_buf[i - 3]);
        md5_distance = 1024 - i + 3;
        p_md5_buf = (p_priv->p_save_buf + p_priv->save_size 
                               - md5_distance);
        memcpy((u8 *)&md5_head_info,p_md5_buf,sizeof(md5_ex_head_info_t));
        p_md5_buf = (u8 *)p_md5_buf + sizeof(u32) * 4;
	     data_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                 p_md5_buf,
                                 md5_head_info.header_info_size - sizeof(u32) * 4);
        memcpy(ota_info.sw_disp_version,md5_head_info.sw_disp_version,sizeof(u8) * 32);
        ota_info.g_ota_auto = md5_head_info.g_ota_auto;
        break;
      }
  }
  /********download data have md5 mark********/
  if((md5_head_info.mark == MD5_HEAD_MARK) 
    && (data_crc == md5_head_info.header_info_crc))
    {
       p_priv->save_size = p_priv->save_size - md5_distance;

       if(NULL != md5_function)
        {
           ret = md5_function(md5_head_info.md5_value,
                                p_priv->p_save_buf,p_priv->save_size,&ota_info);
        }
       else /***default***/
        {
           md5_set_specific_value(0xd76aa478);
           ret = md5_check_data_value(md5_head_info.md5_value,
                                p_priv->p_save_buf,p_priv->save_size);
        }
    }
    else if(NULL != md5_function) /****custom md5 sign***/
      {
          ret = md5_function(md5_head_info.md5_value,
                                  p_priv->p_save_buf,p_priv->save_size,&ota_info);
      }
    else /*****not md5 check data******/
      {
         ret = TRUE;
      }
    
      if(ret == FALSE)
      {
          ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                                  "[%s]:md5 check fail!\n",
                                  __FUNCTION__);  
          return FALSE;
      }
      else
      {
          return TRUE;
      }

      //return FALSE;
}

/*!
  dsmcc para process download data
  */
BOOL ota_filter_process_download_data(ota_filter_priv_t *p_priv,BOOL is_zipped,
                              ota_md5_data_sign_t md5_function)
{
  u8 byte[4] = {0};
  u32 data_crc = 0;
  u32 crc_check = 0;
  u32 data_size = 0;
  u8 *p_fake_buf = NULL;
  u32 fake_size = 0;
  u32 unzip_ret  = 0;
  u32 unzip_size = 0;

  if(TRUE != ota_filter_md5_sign_data(p_priv,md5_function))
    {
        return FALSE;
    }

  if(is_zipped == TRUE)
    {
       p_priv->burn_size = ota_filter_get_lzma_file_size(p_priv->p_save_buf);
       if(p_priv->burn_size <= 0)
        {
           return FALSE;
        }
       p_priv->burn_size += 1024;
       
       p_priv->p_burn_buf = (u8 *)ota_filter_malloc(p_priv,p_priv->burn_size);
       if(p_priv->p_burn_buf == NULL)
      {
          ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                    "[%s]:no enough memory to unzip!\n",
                    __FUNCTION__);  
        return FALSE;
       }
    }
   else
    {
       if(p_priv->p_burn_buf != NULL)
        {
           ota_filter_free(p_priv,p_priv->p_burn_buf);
           p_priv->p_burn_buf = NULL;
        }
    }

  if(p_priv->p_save_buf == NULL)
  {
    ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                                      "[%s]:save buffer is NULL!\n",
                                      __FUNCTION__);  
      return FALSE;
  }
 if((is_zipped == TRUE)
     && (p_priv->p_burn_buf == NULL))
  {
   ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                                    "[%s]:save buffer is NULL!\n",
                                    __FUNCTION__); 
     return FALSE;
  }

   if((is_zipped  != TRUE)
     && (p_priv->p_burn_buf != NULL))
  {
    ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                             "[%s]:burn buf  buffer is no NULL ,it will same save buffer!\n",
                             __FUNCTION__); 
     return FALSE;
  }
  
  //check the upg file's crc.
  byte[0] = *((u8 *)(p_priv->p_save_buf + p_priv->save_size - 4));
  byte[1] = *((u8 *)(p_priv->p_save_buf + p_priv->save_size - 3));
  byte[2] = *((u8 *)(p_priv->p_save_buf + p_priv->save_size - 2));
  byte[3] = *((u8 *)(p_priv->p_save_buf + p_priv->save_size - 1));


  data_size = p_priv->save_size - 4;
  p_priv->save_size = data_size;

  data_crc = ((byte[0]) << 24 | (byte[1] << 16) | (byte[2] << 8) | (byte[3]));   
  crc_check = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,p_priv->p_save_buf,data_size);
  OS_PRINTF("download date crc check :data crc = 0x%x,check crc = 0x%x\n",data_crc,crc_check);
  if(crc_check != data_crc)
  {
     OS_PRINTF("[%s]:download date crc is err!data crc = 0x%x,check crc =0x%x\n",
                         __FUNCTION__,data_crc,crc_check); 
     
     return FALSE;
  }
  
  if(is_zipped == TRUE)
  {
      fake_size = 400 * KBYTES;
      p_fake_buf = (u8 *)ota_filter_malloc(p_priv,fake_size);
      if(p_fake_buf == NULL)
      {
         ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                         "[%s]:no enough memory to unzip!\n",
                         __FUNCTION__); 
         return FALSE;
      }
      init_fake_mem_lzma((void *)p_fake_buf,fake_size);
      
      unzip_size = p_priv->burn_size;
      unzip_ret = lzma_decompress((void *)(p_priv->p_burn_buf),&unzip_size, 
                                                      (void *)p_priv->p_save_buf,data_size);
      if(unzip_ret != 0)
      {
          ota_filter_free(p_priv,p_fake_buf);
          p_fake_buf = NULL;
          ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                   "[%s]:do zip is err err:0x%x!\n",
                   __FUNCTION__,unzip_ret); 
          return FALSE;
      }
      p_priv->burn_size = unzip_size;
      ota_filter_free(p_priv,p_fake_buf);
      p_fake_buf = NULL;
      ota_filter_free(p_priv,p_priv->p_save_buf);
      p_priv->p_save_buf = NULL;
  }
  else
  {
    p_priv->p_burn_buf  = p_priv->p_save_buf;
    p_priv->p_save_buf = NULL;
    p_priv->burn_size = data_size;
  }

  #ifdef WIN32
{
    FILE *fp;
    fp = fopen("upg.bin", "wb");
    if(NULL != fp)
    {
        fwrite(p_priv->p_burn_buf , 1, p_priv->burn_size, fp);
        fclose(fp);
    }
}
#endif

  
  return TRUE;
}

/*!
  ota filter check dsi private info
  */
BOOL  ota_filter_check_dsi_private_info(ota_filter_priv_t *p_priv)
{
     BOOL g_block_lack = FALSE;
     u8 pos = 0;
     ota_filter_dl_check_t result;
     BOOL ret = FALSE;
     
     g_block_lack = FALSE;
     if((TRUE ==  p_priv->destroy_flag) &&
        ((p_priv->destroy_block_id == p_priv->ota_back_block_id)
        || (p_priv->destroy_block_id == p_priv->ota_curr_block_id)))
    {

      if((TRUE != ota_filter_get_ota_data_block(p_priv,p_priv->ota_back_block_id,&pos))
        && (TRUE != ota_filter_get_ota_data_block(p_priv,p_priv->ota_curr_block_id,&pos)))
        {
           g_block_lack = TRUE;
        }
      }
     else if((TRUE ==  p_priv->destroy_flag) /***other block****/
      && (TRUE != ota_filter_get_ota_data_block(p_priv,p_priv->destroy_block_id,&pos))
      && (TRUE != ota_filter_get_ota_data_block(p_priv,OTA_ALL_MAIN_CODE_BLOCK_ID,&pos)))
      {
          
        g_block_lack = TRUE;
      }

     if(g_block_lack == TRUE)  /***not ota block date in download file****/
      {
          result = OTA_FILTER_DL_CHECK_RESULT_ERR;
      }
    else /*****check exist block version****/
      {
         ret = dsmcc_parse_check_data_for_download(p_priv->upg_check_info.focre_bit,
                                p_priv->sys_block,p_priv->ota_block,
                                p_priv->ota_version_check,
                                FALSE);
         if(TRUE == ret)
          {
             result = OTA_FILTER_DL_CHECK_RESULT_SUCCESS;
          }
         else
          {
            result = OTA_FILTER_DL_CHECK_RESULT_NO_BLOCK;
          }
         
         if(OTA_FILTER_DL_CHECK_RESULT_SUCCESS == result)
          {
                if(((TRUE == p_priv->allow_upg_ota_bit)
                && (p_priv->load_block_id != p_priv->maincode_block_id)
                /***ota don't allow upg ota block****/
                && (TRUE == p_priv->ota_in_maincode_bit)) 
                ||(FALSE == p_priv->allow_upg_ota_bit)) /**no ota function****/
              {
                if(TRUE == ota_filter_check_block_id_from_block_group(p_priv->ota_block,
                                    p_priv->ota_curr_block_id,&pos))
                {
                   p_priv->ota_block[pos].exist_bit = FALSE;
                   p_priv->ota_block[pos].upg_bit = FALSE;
                   if(TRUE == ota_filter_check_upg_exist_from_block_group(p_priv->ota_block))
                    {
                     result = OTA_FILTER_DL_CHECK_RESULT_SUCCESS;
                    }
                   else
                    {
                      /******it will check ota version next maincode check upg*************/
                      result = OTA_FILTER_DL_CHECK_RESULT_NO_BLOCK;
                    }
                 }
                
              } 
          }
       }

       if(result == OTA_FILTER_DL_CHECK_RESULT_SUCCESS)
        {
          return TRUE;
        }
       else
        {
          return FALSE;
        }
        //return FALSE;
}

/*!
  ota filter process dsmcc protocol(intra).
  */
static void ota_filter_process_dsmcc_protocol(ota_filter_priv_t *p_priv,media_sample_t *p_sample)
{
  ota_input_pin_t *p_ipin = &p_priv->m_in_pin;

  u16 table_id = 0;
  u16 dsmcc_type = 0;
  u32 data_buffer_size = 0;
  BOOL ret = FALSE;
  dsmcc_pare_db_t status = {0};
  
  if(p_sample->state == SAMP_STATE_GOOD)
  {
    if(p_priv->p_receiver_buffer == NULL)
    {
       ota_filter_debug_printf(p_priv,
                                               OTA_DEBUG_ERROR,
                                               "ota filter:receive buffer err!\n");
       return;
    }
    data_buffer_size = p_sample->payload;
    if(data_buffer_size > EIT_SECTION_BUFFER_SIZE)
    {
       ota_filter_debug_printf(p_priv,
                                               OTA_DEBUG_ERROR,
                                               "ota filter:receive size err!\n");
       return;
    }
    memset(p_priv->p_receiver_buffer,0,EIT_SECTION_BUFFER_SIZE);
    memcpy(p_priv->p_receiver_buffer,p_sample->p_data,data_buffer_size);
    
    table_id = p_sample->context & 0xffff;
    if(table_id == DVB_TABLE_ID_DSMCC_MSG)
      {
        dsmcc_type =  parse_dsmcc_dsi_dii_section_v2(p_priv->p_receiver_buffer,data_buffer_size);
        if(dsmcc_type == DSMCC_MSG_DSI)
        {
           ota_filter_debug_printf(p_priv,
                                                    OTA_DEBUG_INFO,
                                                    "ota filter:receive dsi info\n");
           memset(&p_priv->upg_check_info,0,sizeof(upg_check_info_t));
           if((p_priv->run_select == OTA_WORK_SELECT_AUTO)
                && (p_priv->g_direct_dl == TRUE))
            {
              ret = dsmcc_parse_get_dsi_upg_info(p_priv->ipin_cfg.filter_param.group_id
                                ,&p_priv->upg_check_info);
            }
           else
            {
              ret = dsmcc_parse_check_dsi_upg_info(p_priv->hw_tdi,p_priv->upg_version,
                    &p_priv->upg_check_info,
                    p_priv->ota_version_check,
                    p_priv->ota_dsi_info_check);
            }
           
           if((p_priv->upg_check_info.upg_bit == FALSE) 
                || (p_priv->upg_check_info.upg_file_size <= 0)
                || (ret == FALSE))
          {
             ota_filter_debug_printf(p_priv,
                      OTA_DEBUG_INFO,
                      "ota filter:no upg or upg file size is err!\n");
             p_priv->loop_sm = OTA_FILTER_SM_CHECK_FAIL;
          }
        else
          {
            
            if(p_priv->upg_check_info.dsi_priv_bit == TRUE)
              {
                  ret = ota_filter_check_dsi_private_info(p_priv);
                  if(FALSE == ret)
                  {
                    ota_filter_debug_printf(p_priv,
                    OTA_DEBUG_INFO,
                    "ota filter:dis private info no upg info!\n");
                    p_priv->loop_sm = OTA_FILTER_SM_CHECK_FAIL;
                  }
                  else
                  {
                    ota_filter_debug_printf(p_priv,
                    OTA_DEBUG_INFO,
                    "ota filter:check success!\n");
                    p_priv->loop_sm = OTA_FILTER_SM_CHECK_SUCCESS;
                  }
              }
            else
            {
              ota_filter_debug_printf(p_priv,
              OTA_DEBUG_INFO,
              "ota filter:check success!\n");
              p_priv->loop_sm = OTA_FILTER_SM_CHECK_SUCCESS;
            }
          }
        }
        else if((p_priv->dsi_multi_timeout <= mtos_ticks_get())
                  && (p_priv->g_check_timeout == TRUE))
        {
         ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:dsi multi time out\n");
         p_priv->loop_sm = OTA_FILTER_SM_CHECK_FAIL;
        }
      }
    else if(table_id == DVB_TABLE_ID_DSMCC_DDM)
    {
      ret = parse_dsmcc_db_section_v2(p_priv->p_receiver_buffer,
                  data_buffer_size,
                  &status,p_priv->group_id,
                  p_priv->p_save_buf);
      if(TRUE == ret)
      {
        if(status.finish_bit == TRUE)
          {
            ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_ING,100,0,0); 
            dsmcc_pare_download_finish();
            p_ipin->ota_input_pin_free(p_ipin,FREE_TABLE_DSMCC_DMM,p_priv->data_pid);
            p_priv->loop_sm = OTA_FILTER_SM_CHECK_DOWNLOAD_DATA;
          }
    else
      {
        ota_filter_send_message_out(p_priv,
          OTA_FILTER_EVENT_DOWNLOAD_ING,
          status.progress_all,0,0);
      }
    }
    } 
  }
  else if(p_sample->state == SAMP_STATE_TIMEOUT)
  {
      table_id = p_sample->context & 0xffff;
      OS_PRINTF("#####section table[0x%x] time out\n",table_id);
      p_priv->loop_sm = OTA_FILTER_SM_REQUEST_TIMEOUT;
      p_priv->sm_param = p_sample->context;
      
  }
}



/*!
  ota filter process download data(intra).
  */
static void ota_filter_check_download_data(ota_filter_priv_t *p_priv)
{
  BOOL dl_fail_bit = TRUE;
  BOOL ret = FALSE;
  u8 pos = 0;
  ota_filter_dl_check_t result;
  BOOL g_block_lack = FALSE;
  
  if(TRUE != ota_filter_process_download_data(p_priv,
    p_priv->upg_check_info.zipped_bit,
    p_priv->ota_md5_data_sign))
  {
  ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                                          "[%s]:process download err!\n",
                                          __FUNCTION__); 
  dl_fail_bit = FALSE;
  }
 else
 {
   if(dsmcc_parse_add_ddm_info_to_ota_block(p_priv->p_burn_buf,
              p_priv->upg_check_info.focre_bit,
              TRUE,p_priv->ota_block_data_verify) == TRUE)
  {
  /**previous ota block is destroy ,need check this ota block id exist;else don't run ota***/
  
  /****the ts maybe have ota self block only,but it have to ota block *****/
  g_block_lack = FALSE;
   if((TRUE ==  p_priv->destroy_flag) &&
      ((p_priv->destroy_block_id == p_priv->ota_back_block_id)
      || (p_priv->destroy_block_id == p_priv->ota_curr_block_id)))
  {

    if((TRUE != ota_filter_get_ota_data_block(p_priv,p_priv->ota_back_block_id,&pos))
      && (TRUE != ota_filter_get_ota_data_block(p_priv,p_priv->ota_curr_block_id,&pos)))
      {
         g_block_lack = TRUE;
      }
    }
   else if((TRUE ==  p_priv->destroy_flag) /***other block****/
    &&(TRUE != ota_filter_get_ota_data_block(p_priv,p_priv->destroy_block_id,&pos))
    && (TRUE != ota_filter_get_ota_data_block(p_priv,OTA_ALL_MAIN_CODE_BLOCK_ID,&pos)))
    {
        
      g_block_lack = TRUE;
    }
   
   
   if(g_block_lack == TRUE)  /***not ota block date in download file****/
    {
        
        result = OTA_FILTER_DL_CHECK_RESULT_ERR;
        p_priv->download_try_times = 3;
    }
   else /***normal ota ****/
    {
     ret = dsmcc_parse_check_data_for_download(p_priv->upg_check_info.focre_bit,
                            p_priv->sys_block,p_priv->ota_block,
                            p_priv->ota_version_check,
                            TRUE);
     if(TRUE == ret)
      {
         result = OTA_FILTER_DL_CHECK_RESULT_SUCCESS;
      }
     else
      {
        result = OTA_FILTER_DL_CHECK_RESULT_NO_BLOCK;
      }
     
     if(OTA_FILTER_DL_CHECK_RESULT_SUCCESS == result)
      {
            if(((TRUE == p_priv->allow_upg_ota_bit)
            && (p_priv->load_block_id != p_priv->maincode_block_id)
            /***ota don't allow upg ota block****/
            && (TRUE == p_priv->ota_in_maincode_bit)) 
            ||(FALSE == p_priv->allow_upg_ota_bit)) /**no ota function****/
          {
            if(TRUE == ota_filter_check_block_id_from_block_group(p_priv->ota_block,
                                p_priv->ota_curr_block_id,&pos))
            {
               // result = OTA_FILTER_DL_CHECK_RESULT_ERR;
               // p_priv->download_try_times = 3;
               p_priv->ota_block[pos].exist_bit = FALSE;
               p_priv->ota_block[pos].upg_bit = FALSE;
               if(TRUE == ota_filter_check_upg_exist_from_block_group(p_priv->ota_block))
                {
                 result = OTA_FILTER_DL_CHECK_RESULT_SUCCESS;
                }
               else
                {
                  /******it will check ota version next maincode check upg*************/
                  result = OTA_FILTER_DL_CHECK_RESULT_NO_BLOCK;
                }
               
             }
          }
      }

    }
   
   if(OTA_FILTER_DL_CHECK_RESULT_SUCCESS == result)
    {
        ota_filter_send_message_out(p_priv,
          OTA_FILTER_EVENT_DOWNLOAD_SUCCESS,
            (u32)((u32 *)p_priv->ota_block),0,0);
        p_priv->loop_sm = OTA_FILTER_SM_DOWNLOAD_SUCCESS;
        memset(p_priv->ota_intra_block,0,
            sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
        memcpy(p_priv->ota_intra_block,
          p_priv->ota_block,
          sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
        p_priv->sm_param = (u32)((u32 *)p_priv->ota_intra_block);
        return;
    }
   /***do nothing,save upg version and reboot***/
   else if(OTA_FILTER_DL_CHECK_RESULT_NO_BLOCK == result) 
    {
       ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,
                                                            "[%s]:no block upg!\n",
                                                            __FUNCTION__); 
       if(p_priv->p_save_buf != NULL)
        {
          ota_filter_free(p_priv,p_priv->p_save_buf);
          p_priv->p_save_buf =  NULL;
        }
      if(p_priv->p_burn_buf != NULL)
        {
          ota_filter_free(p_priv,p_priv->p_burn_buf);
          p_priv->p_burn_buf = NULL;
        }
       ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_NO_BLOCK_UPG,0,0,0);
       //ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_REBOOT,0,0,0);
       p_priv->loop_sm = OTA_FILTER_SM_IDLE;
       return;
    }
    else /***OTA_FILTER_DL_CHECK_RESULT_ERR***/
    {
       ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                            "[%s]:check block upg version in ddm header fail!\n",
                           __FUNCTION__); 
      dl_fail_bit = FALSE;
    }

  }
   else
    {
      ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,
                                              "[%s]:parse_add_ddm_info_to_ota_block fail!\n",
                                               __FUNCTION__); 
       dl_fail_bit = FALSE;
    }
  }

  if(dl_fail_bit == FALSE)
  {
     if(p_priv->p_save_buf != NULL)
      {
        ota_filter_free(p_priv,p_priv->p_save_buf);
        p_priv->p_save_buf =  NULL;
      }
    if(p_priv->p_burn_buf != NULL)
      {
        ota_filter_free(p_priv,p_priv->p_burn_buf);
        p_priv->p_burn_buf =  NULL;
      }

     if(p_priv->run_select == OTA_WORK_SELECT_AUTO)
      {
          if(p_priv->download_try_times < 3)
            {
            
                 p_priv->download_try_times ++;
                memcpy(&p_priv->upg_check_intra_info,
                  &p_priv->upg_check_info,
                  sizeof(upg_check_info_t));
                p_priv->sm_param = (u32)((u32 *)&p_priv->upg_check_intra_info);
                p_priv->loop_sm = OTA_FILTER_SM_CHECK_SUCCESS;
                ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,
                                                                    "[%s]:download again!\n",
                                                                    __FUNCTION__);
                return;
            }
          else
            {
              ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_FAIL,0,0,0);
            }
      }
     else
      {
         ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_FAIL,0,0,0);
      }
  }
  p_priv->loop_sm = OTA_FILTER_SM_IDLE;
}

/*!
  ota filter get dm info no use table(intra).
  */
static u8 ota_filter_get_dm_info_no_use_index(ota_filter_priv_t *p_priv)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if(p_priv->dm_info[i].exist_bit != TRUE)
      { 
        return i;
      }
  }
  return 0xff;
}

/*!
  ota filter get dm info exist table(intra).
  */
static u8 ota_filter_get_dm_info_exist_index(ota_filter_priv_t *p_priv,u8 block_id)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->dm_info[i].exist_bit == TRUE)
        && (p_priv->dm_info[i].id == block_id))
      { 
        return i;
      }
  }
  return 0xff;
}

/*!
  ota filter get sys block table(intra).
  */
static u8 ota_filter_get_sys_block_index(ota_filter_priv_t *p_priv,u8 block_id)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->sys_block[i].exist_bit == TRUE)
        && (p_priv->sys_block[i].id == block_id))
      { 
        return i;
      }
  }
  return 0xff;
}

/*!
  ota filter add dmh  table(intra).
  */
static void ota_filter_add_dmh_info(ota_filter_priv_t *p_priv,ota_dm_info_t *p_dmh_info)
{
    u8 index = 0;
    index = ota_filter_get_dm_info_no_use_index(p_priv);
    if(index == 0xff)
    {
      return;
    }
    memcpy(&p_priv->dm_info[index],p_dmh_info,sizeof(ota_dm_info_t));
    p_priv->dm_info[index].exist_bit = TRUE;
    return;
}

/*!
  get dm header info .
  */
BOOL ota_filter_find_dmh_info(u8 *p_buf,u32 buf_size,
               ota_dmh_info_t *p_dmh_info,
               u32 dm_offset)
{
   u32 i = 0,j = 0,k = 0;
   u32 col_setup = 0x10000;
   u8 dmh_mark[DMH_MARK_SIZE + 1] = "*^_^*DM(^o^)";
   u32 col_total = buf_size / col_setup;
   u32 row_total = 1024; /**1KBYTE**/ 
   u8 *p_header_start = NULL;
   u8 *p_block_start = NULL;
   u8 group_total = 0;
   
   
  for(i = 0; i < col_total; i ++)
  {
    for(j = 0;j < row_total;j ++)
    {
      if(0 == memcmp(p_buf +(col_setup * i)+ j, dmh_mark, DMH_MARK_SIZE))
       {
         /***dmh tag is 12 + 4****/
         p_header_start = p_buf +(col_setup * i)+ j + DMH_TAG_TOTAL_SIZE;

         p_dmh_info->header_group[group_total].dmh_start_pos = (col_setup * i)+ j;
         p_dmh_info->header_group[group_total].sdram_size = 
          MT_MAKE_DWORD(MAKE_WORD(p_header_start[0], p_header_start[1]), \
                                            MAKE_WORD(p_header_start[2], p_header_start[3]));
         p_dmh_info->header_group[group_total].flash_size = 
          MT_MAKE_DWORD(MAKE_WORD(p_header_start[4], p_header_start[5]), \
          MAKE_WORD(p_header_start[6], p_header_start[7]));
         p_dmh_info->header_group[group_total].block_num = 
          MAKE_WORD(p_header_start[8], p_header_start[9]);
         p_dmh_info->header_group[group_total].bh_size = 
          MAKE_WORD(p_header_start[10], p_header_start[11]);

         if(p_dmh_info->header_group[group_total].block_num > DMH_GROUP_MAX_BLOCK_NUMBER)
          {
            return FALSE;
          }
         
         for(k = 0; k < p_dmh_info->header_group[group_total].block_num;k++)
          {
             p_block_start = p_header_start 
                                     + DMH_BASE_INFO_LEN 
                                     + (k * p_dmh_info->header_group[group_total].bh_size);
             
                p_dmh_info->header_group[group_total].dm_block[k].id = p_block_start[0];
                p_dmh_info->header_group[group_total].dm_block[k].type = p_block_start[1];
                p_dmh_info->header_group[group_total].dm_block[k].base_addr = 
                   MT_MAKE_DWORD(MAKE_WORD(p_block_start[4], p_block_start[5]), \
                                                  MAKE_WORD(p_block_start[6], p_block_start[7]));
                p_dmh_info->header_group[group_total].dm_block[k].size = 
                  MT_MAKE_DWORD(MAKE_WORD(p_block_start[8], p_block_start[9]), \
                                                 MAKE_WORD(p_block_start[10], p_block_start[11]));
                p_dmh_info->header_group[group_total].dm_block[k].crc = 
                  MT_MAKE_DWORD(MAKE_WORD(p_block_start[12], p_block_start[13]), \
                                                 MAKE_WORD(p_block_start[14], p_block_start[15]));

               p_dmh_info->header_group[group_total].dm_block[k].dm_offset = 
                p_dmh_info->header_group[group_total].dmh_start_pos + dm_offset;
               p_dmh_info->header_group[group_total].dm_block[k].base_addr += 
                p_dmh_info->header_group[group_total].dm_block[k].dm_offset;
               
          }
         group_total ++;
         if(group_total > DMH_MAX_GROUP_NUMBER)
          {
             return FALSE;
          }
         p_dmh_info->header_num = group_total;
         break;
       }
    }
  }

  if(p_dmh_info->header_num == 0)
    {
      return FALSE;
    }
  
  return TRUE;
}

/*!
  ota filter add header info to dm table(intra).
  */
static void ota_filter_add_dmh_info_to_dm(ota_filter_priv_t *p_priv,
                            u8 *p_buf,
                            u32 buf_size,
                            u32 dm_offset)
{
 ota_dmh_info_t dmh_info;
 ota_dm_info_t block_dm_info;
 u8 i = 0, j = 0;
 u8 sys_block_index = 0;
 memset(p_priv->dm_info,0,sizeof(ota_dm_info_t) * OTA_MAX_BLOCK_NUMBER);
 memset(&dmh_info,0,sizeof(ota_dmh_info_t));
 if(TRUE == ota_filter_find_dmh_info(p_buf,buf_size,&dmh_info,dm_offset))
{
 for(i = 0;i < dmh_info.header_num;i ++)
{
 for(j = 0;j < dmh_info.header_group[i].block_num ;j ++)
{
   if(OTA_ALL_MAIN_CODE_BLOCK_ID == dmh_info.header_group[i].dm_block[j].id)
    {
      continue;
    }
   sys_block_index = ota_filter_get_sys_block_index(p_priv,dmh_info.header_group[i].dm_block[j].id);
   if(sys_block_index == 0xff)
    {
      continue;
    }
   
 if((dmh_info.header_group[i].dm_block[j].type == 
                              p_priv->sys_block [sys_block_index].type)
    && (dmh_info.header_group[i].dm_block[j].base_addr == 
                      p_priv->sys_block [sys_block_index].base_addr)
    &&  (dmh_info.header_group[i].dm_block[j].size == 
                                p_priv->sys_block [sys_block_index].size)
    &&((p_priv->sys_block[i].base_addr % CHARSTO_SECTOR_SIZE) == 0)
    && ((p_priv->sys_block[i].size % CHARSTO_SECTOR_SIZE) == 0))
    {
       memset(&block_dm_info,0,sizeof(ota_dm_info_t));
       block_dm_info.id = p_priv->sys_block [sys_block_index].id;
       block_dm_info.type = p_priv->sys_block [sys_block_index].type;
       block_dm_info.size = p_priv->sys_block [sys_block_index].size;
       block_dm_info.base_addr = p_priv->sys_block [sys_block_index].base_addr;
       block_dm_info.crc = dmh_info.header_group[i].dm_block[j].crc;
       block_dm_info.dm_offset = dmh_info.header_group[i].dm_block[j].dm_offset;
       ota_filter_add_dmh_info(p_priv,&block_dm_info);
    }
}
}
}
}

/*!
  ota filter reset ota data table(intra).
  */
static void ota_filter_reset_ota_data(ota_filter_priv_t *p_priv)
{
    u8 i = 0;
    for(i = 0;i < OTA_MAX_BLOCK_NUMBER;i++)
    {
        if(p_priv->ota_data[i].exist_bit == TRUE)
        {
          if((p_priv->ota_data[i].p_burn_buf != NULL) 
            && (p_priv->ota_data[i].is_new_buf == TRUE))
          {
             ota_filter_free(p_priv,p_priv->ota_data[i].p_burn_buf);
             p_priv->ota_data[i].p_burn_buf = NULL;
             p_priv->ota_data[i].is_new_buf = FALSE;
          }
        }
        memset(&p_priv->ota_data[i],0,sizeof(ota_data_info_t));
        p_priv->ota_data[i].exist_bit = FALSE;
    }
}

/*!
  ota filter break all flash file to block file(intra).
  */
static void ota_filter_break_all_to_block(ota_filter_priv_t *p_priv)
{
  u8 i = 0;
  u8 j = 0; 
  u8 pos = 0;
  u8 index = 0,index1 = 0,index2 = 0;
  u8 index_ota_block = 0;
  ota_data_info_t  ota_all_code_info;
  BOOL main_code_bit = FALSE;
  u32 dm_offset = 0;
  u8 index_intra_block = 0;
    
  for(i = 0;i < OTA_MAX_BLOCK_NUMBER;i++)
  {
     if((p_priv->ota_data[i].exist_bit == TRUE)
         && (p_priv->ota_data[i].id == OTA_ALL_MAIN_CODE_BLOCK_ID)
         &&(p_priv->ota_data[pos].burn_size > 0)
         && (p_priv->ota_data[pos].is_factory_force == FALSE)
         && (p_priv->ota_data[pos].is_data_cut == TRUE))
      {
           break;
      }
  }

  if(i >= OTA_MAX_BLOCK_NUMBER)
  {
    return;
  }
  
  if((p_priv->ota_data[pos].burn_size) 
             == (p_priv->flash_size - p_priv->bootload_size)) /***it is maincode***/
    {
      p_priv->ota_data[pos].file_type = OTA_BURN_FILE_TYPE_MAIN_CODE;
      /**it have ota upg? yes pare bootload dm head,no pare maincode dm***/
      ota_filter_add_dmh_info_to_dm(p_priv,
                  p_priv->ota_data[pos].p_burn_buf,
                  p_priv->ota_data[pos].burn_size,
                  p_priv->bootload_size);
    }         
  else if(p_priv->ota_data[pos].burn_size == p_priv->flash_size)
    {
      p_priv->ota_data[pos].file_type = OTA_BURN_FILE_TYPE_ALL_CODE;
      /**pare maincode dm***/
      ota_filter_add_dmh_info_to_dm(p_priv,
          p_priv->ota_data[pos].p_burn_buf,
          p_priv->ota_data[pos].burn_size,0);
    }


  memset(&ota_all_code_info,0,sizeof(ota_data_info_t));
  memcpy(&ota_all_code_info,&p_priv->ota_data[pos],sizeof(ota_data_info_t));
  p_priv->ota_data[pos].p_burn_buf = NULL;

  memset(p_priv->ota_intra_block,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
  memcpy(p_priv->ota_intra_block,
    p_priv->ota_block,
    sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
  
  memset(p_priv->ota_block,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
  ota_filter_reset_ota_data(p_priv);
  for(j = 0;j < ota_all_code_info.break_block.block_num;j++)
  {

   if((ota_all_code_info.break_block.block_id[j] == OTA_ALL_MAIN_CODE_BLOCK_ID) 
       &&(ota_all_code_info.file_type == OTA_BURN_FILE_TYPE_ALL_CODE))
       /****main code****/
    {
       main_code_bit = TRUE;
    }
   else /**other block**/
    {
       index = ota_filter_get_dm_info_exist_index(p_priv,
                        ota_all_code_info.break_block.block_id[j]);
       if(index == 0xff)
        {
          continue;
        }
       main_code_bit = FALSE;
    }
   index1 = ota_filter_get_sys_block_index(p_priv,ota_all_code_info.break_block.block_id[j]);
   if(index1 == 0xff)
    {
      continue;
    }

   if(((p_priv->sys_block [index1].id == p_priv->dm_info[index].id)
       && (p_priv->sys_block [index1].type == p_priv->dm_info[index].type)
      && (p_priv->sys_block [index1].size == p_priv->dm_info[index].size)
      && (p_priv->sys_block [index1].base_addr == p_priv->dm_info[index].base_addr))
      || (main_code_bit == TRUE))
    {
      if(TRUE != ota_filter_check_block_id_from_block_group(p_priv->ota_intra_block,
                          p_priv->sys_block [index1].id,
                          &index_intra_block))
        {
           continue;
        }
      if(TRUE != p_priv->ota_intra_block[index_intra_block].upg_bit)
        {
           continue;
        }
    
      index2 = ota_filter_get_ota_data_free_block_index(p_priv);
      index_ota_block = ota_filter_get_ota_block_free_block_index(p_priv);
      if((index2 != 0xff) && (index_ota_block != 0xff))
      {
         p_priv->ota_data[index2].id = p_priv->sys_block [index1].id;
         p_priv->ota_data[index2].is_factory_force = FALSE;
         p_priv->ota_data[index2].ota_new_ver = ota_all_code_info.ota_new_ver;
         p_priv->ota_data[index2].g_user_db_data_clear = ota_all_code_info.g_user_db_data_clear;
         p_priv->ota_data[index2].g_user_sys_data_clear = ota_all_code_info.g_user_sys_data_clear;
         p_priv->ota_data[index2].g_ca_data_clear = ota_all_code_info.g_ca_data_clear;
         p_priv->ota_data[index2].g_ads_data_clear = ota_all_code_info.g_ads_data_clear;
        
         
         if(main_code_bit == TRUE)
          {
            p_priv->ota_data[index2].burn_size = p_priv->flash_size - p_priv->bootload_size;
          }
         else
          {
            p_priv->ota_data[index2].burn_size = p_priv->sys_block [index1].size;
          }
        
        p_priv->ota_data[index2].p_burn_buf = 
                   (u8 *)ota_filter_malloc(p_priv,p_priv->ota_data[index2].burn_size);
        p_priv->ota_data[index2].is_new_buf = TRUE;
        
       if(p_priv->ota_data[index2].p_burn_buf != NULL)
        {
           if((p_priv->ota_data[index2].id == OTA_ALL_MAIN_CODE_BLOCK_ID)
               && (main_code_bit == TRUE))
              {
                  dm_offset =  p_priv->bootload_size;
              }
            else
              {
                if(ota_all_code_info.file_type == OTA_BURN_FILE_TYPE_MAIN_CODE)
                  {
                    dm_offset = p_priv->dm_info[index].base_addr - p_priv->bootload_size;
                  }
                 else /**all code***/
                  {
                    dm_offset = p_priv->dm_info[index].base_addr;
                  }
              }
            
          memcpy(p_priv->ota_data[index2].p_burn_buf,ota_all_code_info.p_burn_buf + dm_offset,
          p_priv->ota_data[index2].burn_size);

          if((p_priv->ota_data[index2].id == OTA_ALL_MAIN_CODE_BLOCK_ID)
            && (main_code_bit == TRUE))
          {
             /*******do nothing**********/
          }
           else
          {
             p_priv->ota_data[index2].crc = crc_fast_calculate(CRC32_ARITHMETIC,
                                        0xFFFFFFFF,
                                        p_priv->ota_data[index2].p_burn_buf,
                                        p_priv->ota_data[index2].burn_size);
              if(p_priv->dm_info[index].crc != p_priv->ota_data[index2].crc)
              {
                 ota_filter_free(p_priv,p_priv->ota_data[index2].p_burn_buf);
                 p_priv->ota_data[index2].is_new_buf = FALSE;
                 continue;
              }
          }

         p_priv->ota_data[index2].crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,
                                        0xFFFFFFFF,
                                        p_priv->ota_data[index2].p_burn_buf,
                                        p_priv->ota_data[index2].burn_size);
        
         p_priv->ota_data[index2].exist_bit = TRUE;
         p_priv->ota_data[index2].upg_bit = TRUE;
         p_priv->ota_data[index2].burned_flash_bit = FALSE;

          p_priv->ota_block[index_ota_block].id = p_priv->ota_data[index2].id;
          p_priv->ota_block[index_ota_block].exist_bit = TRUE;
          p_priv->ota_block[index_ota_block].upg_bit = TRUE;
          
      }
       else
        {
          p_priv->ota_data[index2].exist_bit = FALSE;
          p_priv->ota_data[index2].upg_bit =  FALSE;
        }
      }
    }
  }

  if(p_priv->p_burn_buf != NULL)
  {
     ota_filter_free(p_priv,p_priv->p_burn_buf);
     p_priv->p_burn_buf = NULL;
  }
  
}

/*!
  ota filter make block header(intra).
  */
static BOOL ota_filter_make_block_header(ota_filter_priv_t *p_priv,u8 block_id)
{
  block_header_info_t block_info = {0};
  u16 bisize = sizeof(block_header_info_t);
  u8 blh_indicator[12] = "@~_~BI~_~@";
  u8 add_buffer[64] = {0}; /***block info at block end****/
  u8 pos = 0;
  u32 add_len = 0;
  add_len = sizeof(u8)*64;
  if(ota_filter_get_ota_data_block(p_priv,block_id,&pos) !=  TRUE)
  {
    return FALSE;
  }

  if((p_priv->ota_data[pos].size - p_priv->ota_data[pos].burn_size) < add_len)
  {
    return FALSE;
  }

  memcpy(add_buffer,blh_indicator,sizeof(u8)*12);
  memset(&block_info,0,bisize);
  block_info.id = p_priv->ota_data[pos].id;
  block_info.type = p_priv->ota_data[pos].type;
  
  block_info.base_addr = p_priv->ota_data[pos].base_addr \
                                      - p_priv->ota_data[pos].dm_offset;
  
  block_info.size = p_priv->ota_data[pos].burn_size;
  block_info.crc = p_priv->ota_data[pos].crc;
 memcpy(add_buffer + add_len - bisize,&block_info,bisize);
 memcpy(p_priv->ota_data[pos].p_burn_buf + p_priv->ota_data[pos].size - add_len,
                 add_buffer,add_len);
 p_priv->ota_data[pos].burn_size = p_priv->ota_data[pos].size;
 p_priv->ota_data[pos].crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                p_priv->ota_data[pos].p_burn_buf,
                                p_priv->ota_data[pos].burn_size);
return TRUE;
}

/*!
  ota filter check block header(intra).
  */
static BOOL ota_filter_check_block_header(ota_filter_priv_t *p_priv,u8 block_id)
{
  block_header_info_t block_info = {0};
  u16 bisize = sizeof(block_header_info_t);
  u8 blh_indicator[12] = "@~_~BI~_~@";
  u8 read_buffer[64] = {0}; /***block info at block end****/
  u8 pos = 0;
  u32 read_len = 0;
  u32 bass_addr = 0;
  read_len = sizeof(u8)*64;
  
  if(ota_filter_get_ota_data_block(p_priv,block_id,&pos) !=  TRUE)
  {
    return FALSE;
  }

  if(p_priv->ota_data[pos].size != p_priv->ota_data[pos].burn_size)
  {
   // OS_PRINTF("###debug check block head size fail block: sys = 0x%x:0x%x\n",
    //              p_priv->ota_data[pos].size,p_priv->ota_data[pos].burn_size);
    return FALSE;
  }
  memcpy(read_buffer,
                p_priv->ota_data[pos].p_burn_buf + p_priv->ota_data[pos].size - read_len,
                read_len);
 
 if(0 != memcmp(read_buffer, blh_indicator,12))
  {
    //OS_PRINTF("###debug check block head mark fail\n");
   return FALSE;
  }

 memcpy(&block_info,read_buffer + read_len - bisize,bisize);

bass_addr = p_priv->ota_data[pos].base_addr \
                    - p_priv->ota_data[pos].dm_offset;

 if((block_info.id != p_priv->ota_data[pos].id)
    || (block_info.type != p_priv->ota_data[pos].type)
    || (block_info.base_addr != bass_addr))
  {
    return FALSE;
  }

return TRUE;
}

/*!
  ota filter get ota type(intra).
  */
static u8 ota_filter_get_ota_type(ota_filter_priv_t *p_priv,ota_burn_file_t type)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->ota_data[i].exist_bit == TRUE)
        && (p_priv->ota_data[i].upg_bit == TRUE)
        && (p_priv->ota_data[i].file_type == type)
       && (p_priv->ota_data[i].burned_flash_bit == FALSE))
      { 
        return i;
      }
  }
  return 0xff;
}

/*!
  ota filter get jump block(intra).
  */
static u32 ota_filter_get_jump_block(ota_filter_priv_t * p_priv,u8 index,u32 section)
{
  u8 i = 0;
  u8 jump_num = 0;
  u8 sys_block_index = 0;
  u8 dm_block_index = 0;
  jump_num = p_priv->ota_data[index].jump_block.block_num;
  if(jump_num == 0)
  {
    return 0;
  }

  for(i = 0;i < jump_num;i ++)
  {
    sys_block_index = 
      ota_filter_get_sys_block_index(p_priv,p_priv->ota_data[index].jump_block.block_id[i]);
   dm_block_index = 
    ota_filter_get_dm_info_exist_index(p_priv,p_priv->ota_data[index].jump_block.block_id[i]);
   if((sys_block_index == 0xff) || (dm_block_index == 0xff))
    {
      continue;
    }
   if((p_priv->dm_info[dm_block_index].size != p_priv->sys_block[sys_block_index].size)
    || (p_priv->dm_info[dm_block_index].base_addr != p_priv->sys_block[sys_block_index].base_addr))
    {
      continue;
    } 
  if((p_priv->dm_info[dm_block_index].size % OTA_SECTOR_SIZE != 0)
    || (p_priv->dm_info[dm_block_index].base_addr % OTA_SECTOR_SIZE != 0))
    {
      continue;
    } 
  if(section == (p_priv->dm_info[dm_block_index].base_addr / OTA_SECTOR_SIZE))
    {
      ota_filter_debug_printf(p_priv,
                                OTA_DEBUG_INFO,
                                "ota jump block id[0x%x] addr[0x%x] size[0x%x]\n",
                                p_priv->dm_info[dm_block_index].id,
                                p_priv->dm_info[dm_block_index].base_addr,
                                p_priv->dm_info[dm_block_index].size);
      return (p_priv->dm_info[dm_block_index].size / OTA_SECTOR_SIZE);
    }
  }
  return 0;
}

/*!
  ota filter get sys block table(intra).
  */
static u8 ota_filter_find_sys_block_index_from_addr(ota_filter_priv_t *p_priv,u32 address)
{
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->sys_block[i].exist_bit == TRUE)
        && (p_priv->sys_block[i].base_addr == address))
      { 
        return i;
      }
  }
  return 0xff;
}

BOOL ota_filter_burn_flash_charsto_write(ota_filter_priv_t *p_priv,
                                         u32 addr, u8 * p_buf, u32 len)
{
  RET_CODE ret = SUCCESS;
  u32 sec_cnt = 0;
  u32 w_crc =0;
  u32 r_crc =0;
  u8 *p_read_buf = NULL;
  handle_t dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  w_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                p_buf,
                                len);
  p_read_buf = ota_filter_malloc(p_priv,len);
  if(p_read_buf == NULL)
  {
     return FALSE;
  }
  
  if((len % OTA_SECTOR_SIZE != 0)
  || (addr % OTA_SECTOR_SIZE != 0))
  {
    ota_filter_free(p_priv,p_read_buf);
    return FALSE;
  }
  sec_cnt = len / OTA_SECTOR_SIZE;
  

  dev_io_ctrl(dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);
  
  ret = charsto_erase(dev, addr, sec_cnt);
  if(ret == SUCCESS)
  {
      ret = charsto_writeonly(dev, addr, p_buf, len);
  }
  
  //restore
  dev_io_ctrl(dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
  
  if(ret == SUCCESS)
  {
    memset(p_read_buf,0,len);
    ret = charsto_read(dev,addr, p_read_buf,len);
  }

  if(ret == SUCCESS)
  {
    r_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                p_read_buf,
                                len);
    if(w_crc == r_crc)
    {
       ota_filter_free(p_priv,p_read_buf);
       return TRUE;
    }
  }
  ota_filter_free(p_priv,p_read_buf);
  return FALSE;
}

/*!
  ota filter burn flash burn(intra).
  */
static BOOL ota_filter_burn_flash_burn(ota_filter_priv_t * p_priv,u8 index,
          u32 sec_start,u32 sec_cnt,u8 *p_buf)
{ 
#ifdef OTA_FILTER_USE_FLASH_FILTER
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_sample = NULL;
#endif 
  flash_burn_in_t  param = {0};

  u32 i = 0;
  u8 progress = 0;
  u32 jump_sec = 0;
  u32 burn_size = 0;
  BOOL is_all_version =  FALSE;
  u8 sys_block_index = 0;
  u8 dm_info_index = 0;
  ota_block_info_t ota_save_bock = {0};
  BOOL is_block_burn =  FALSE;
  u32 block_burn_size = 0;
  ota_tri_mode_t block_tri_mode = 0;
  BOOL ret = FALSE;
  

  burn_size = sec_cnt * OTA_SECTOR_SIZE;
  if(((p_priv->flash_size - p_priv->bootload_size) == burn_size)
     ||(burn_size >= p_priv->flash_size)) /**factory force upg ***/
    {
       is_all_version = TRUE;
    }

  p_priv->destroy_flag = TRUE;
  block_tri_mode = OTA_TRI_MODE_FORC;

  ota_filter_send_message_out(p_priv,
    OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_STATUS,
    block_tri_mode,0,0);
  ota_filter_send_message_out(p_priv,
    OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_DSTROY_FLAGE,
    p_priv->destroy_flag,p_priv->ota_data[index].id,0);

  
  for(i = sec_start; i < (sec_cnt + sec_start);i ++)
  {
    jump_sec = ota_filter_get_jump_block(p_priv,index,i);
    if(jump_sec > 0)
    {
      i += jump_sec;
      i --; /**continue ++**/
      continue;
    }

    param.addr = i * OTA_SECTOR_SIZE;
    param.size = OTA_SECTOR_SIZE;
    param.p_data = p_buf + ((i - sec_start)  * OTA_SECTOR_SIZE);
    
    sys_block_index = ota_filter_find_sys_block_index_from_addr(p_priv,param.addr);
    if((sys_block_index != 0xFF) 
       && (p_priv->sys_block[sys_block_index].id != OTA_ALL_MAIN_CODE_BLOCK_ID)
       && (is_all_version == TRUE))
    {
        memset(&ota_save_bock,0,sizeof(ota_block_info_t));
       dm_info_index = 
        ota_filter_get_dm_info_exist_index(p_priv,p_priv->sys_block[sys_block_index].id);
       if(dm_info_index != 0xFF)
        {
          ota_save_bock.id = p_priv->dm_info[dm_info_index].id;
          ota_save_bock.type = p_priv->dm_info[dm_info_index].type;
          ota_save_bock.crc = p_priv->dm_info[dm_info_index].crc;
          ota_save_bock.ota_new_ver = p_priv->ota_data[index].ota_new_ver;
          ota_save_bock.size = p_priv->dm_info[dm_info_index].size;
          ota_save_bock.base_addr = p_priv->dm_info[dm_info_index].base_addr;
          ota_save_bock.exist_bit = TRUE;
          ota_save_bock.upg_bit = TRUE;
          block_burn_size = 0;
          is_block_burn = TRUE;
        }      
      }

    if(is_block_burn == TRUE)
    {
       block_burn_size += param.size;

    }
    
    #ifdef OTA_FILTER_USE_FLASH_FILTER
    p_out_pin->query_sample(p_out_pin, &p_sample);
    p_sample->p_user_data = (void *)&param;
    p_sample->state             = SAMP_STATE_GOOD;
    p_sample->format.stream_type = MT_FILE_DATA;
    p_out_pin->push_down(p_out_pin, p_sample);
    #else
    ret = ota_filter_burn_flash_charsto_write(p_priv,param.addr,param.p_data,param.size);
    if(ret == FALSE)
    {
       return FALSE;
    }
    #endif
    progress = (u8)(((i + 1 - sec_start) * 100) / (sec_cnt));
    ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_ING,progress,0,0);

    if((is_block_burn == TRUE) && (block_burn_size >= ota_save_bock.size))
      {
         /***save  version block id **/
         ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_SAVE_BLOCK_VERSION_INFO,
            (u32)((u32 *)&ota_save_bock),0,0);
          block_burn_size = 0;
          is_block_burn = FALSE;
      }
  }


  if((TRUE == p_priv->ota_data[index].g_user_sys_data_clear) 
      && (FALSE == p_priv->sys_data_reset_flag))
    {
       ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_RESET_SYS_DATA_BLOCK,
        0,0,0);
       p_priv->sys_data_reset_flag = TRUE;
    }
  if((TRUE == p_priv->ota_data[index].g_user_db_data_clear)
      && (FALSE == p_priv->db_data_reset_flag))
    {
      p_priv->db_data_reset_flag = TRUE;
      ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_RESET_DB_DATA_BLOCK,0,0,0);
    }
    if((TRUE == p_priv->ota_data[index].g_ca_data_clear)
      && (FALSE == p_priv->ca_data_reset_flag))
    {
      p_priv->ca_data_reset_flag = TRUE;
      ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_RESET_CA_DATA_BLOCK,0,0,0);
    }
    if((TRUE == p_priv->ota_data[index].g_ads_data_clear)
    && (FALSE == p_priv->ads_data_reset_flag))
    {
      p_priv->ads_data_reset_flag = TRUE;
      ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_RESET_ADS_DATA_BLOCK,0,0,0);
    }

  

  p_priv->destroy_flag = FALSE;
  ota_filter_send_message_out(p_priv,
    OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_DSTROY_FLAGE,
    p_priv->destroy_flag,0,0);
  block_tri_mode = p_priv->tri_mode;
  ota_filter_send_message_out(p_priv,
    OTA_FILTER_EVENT_BURNFLASH_SAVE_OTA_STATUS,
    block_tri_mode,0,0);
  

  /***save all version block id = OTA_ALL_MAIN_CODE_BLOCK_ID**/
   {
      ota_save_bock.id = p_priv->ota_data[index].id;
      ota_save_bock.type = p_priv->ota_data[index].type;
      ota_save_bock.crc = p_priv->ota_data[index].crc;
      ota_save_bock.ota_new_ver = p_priv->ota_data[index].ota_new_ver;
      ota_save_bock.size = p_priv->ota_data[index].size;
      ota_save_bock.base_addr = p_priv->ota_data[index].base_addr;
      ota_save_bock.exist_bit = TRUE;
      ota_save_bock.upg_bit = TRUE;
  }
  
  ota_filter_send_message_out(p_priv,
    OTA_FILTER_EVENT_BURNFLASH_SAVE_BLOCK_VERSION_INFO,
    (u32)((u32 *)&ota_save_bock),0,0);

  progress = 100;
  ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_ING,progress,0,0);
  return TRUE;
}

/*!
  ota filter burn block flash(intra).
  */
static BOOL ota_filter_burn_flash_by_other_block(ota_filter_priv_t *p_priv)
{
   u8 index = 0;
   u8 i = 0;
   u32 sec_start = 0;
   u32 sec_cnt = 0;
   BOOL upg_bit = FALSE;
   u32 check_crc = 0;
   BOOL ret = FALSE;


   ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
        p_priv->ota_curr_block_id,0,0);
   for(i = 0;i < OTA_MAX_BLOCK_NUMBER;i ++)
    {
       index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_ONLY_BLOCK);
       if(index == 0xff)
        {
          break;
        }
      if((p_priv->ota_data[index].burn_size != p_priv->ota_data[index].size)
      ||(p_priv->ota_data[index].burn_size % OTA_SECTOR_SIZE != 0)
      || (p_priv->ota_data[index].base_addr % OTA_SECTOR_SIZE != 0))
      {
        continue;
      }
      
      check_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                p_priv->ota_data[index].p_burn_buf,
                                p_priv->ota_data[index].burn_size);
      
      if(check_crc != p_priv->ota_data[index].crc)
      {
        continue;
      }

      
      upg_bit = TRUE;
      
      ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_BLOCK,
        p_priv->ota_data[index].id,0,0);
      sec_start = p_priv->ota_data[index].base_addr / OTA_SECTOR_SIZE;
      sec_cnt = p_priv->ota_data[index].burn_size / OTA_SECTOR_SIZE;
      ret = ota_filter_burn_flash_burn(p_priv,index,sec_start,
                            sec_cnt,p_priv->ota_data[index].p_burn_buf);
      if(ret == FALSE)
      {
        return FALSE;
      }
        
   if((p_priv->ota_data[index].p_burn_buf != NULL)
    && (p_priv->ota_data[index].is_new_buf == TRUE))
    {
      ota_filter_free(p_priv,p_priv->ota_data[index].p_burn_buf);
      p_priv->ota_data[index].p_burn_buf = NULL;
      p_priv->ota_data[index].is_new_buf = FALSE;
    }
      p_priv->ota_data[index].burned_flash_bit = TRUE;
    }
   
   ota_filter_send_message_out(p_priv,
          OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
          p_priv->maincode_block_id,0,0);
   return upg_bit;
}
/*!
  ota filter burn ota block flash(intra).
  */
static BOOL ota_filter_burn_flash_by_ota_block(ota_filter_priv_t *p_priv)
{
    block_header_info_t block_info = {0};
    u16 bisize = sizeof(block_header_info_t);
    u8 blh_indicator[12] = "@~_~BI~_~@";
    u8 read_buffer[64] = {0}; /***block info at block end****/
    u32 read_len = 0;
    BOOL rebuild_head_bit = FALSE;
    BOOL ret = FALSE;

    u8 index = 0;
    u8 back_block_index = 0;
    u8 curr_block_index = 0;
    u32 sec_start = 0;
    u32 sec_cnt = 0;
    u32 check_crc = 0;
   
   index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_OTA);
   if(index == 0xff)
    {
      return FALSE;
    }

  else if(TRUE == p_priv->ota_in_maincode_bit)
    {
         if(p_priv->load_block_id != p_priv->maincode_block_id)
          {
            return FALSE;
          }
        curr_block_index = ota_filter_get_sys_block_index(p_priv,p_priv->ota_curr_block_id);
       if((curr_block_index == 0xff)
          || ((p_priv->ota_data[index].burn_size % OTA_SECTOR_SIZE) != 0)
          || (p_priv->ota_data[index].burn_size  != p_priv->sys_block[curr_block_index].size)
          || ((p_priv->sys_block[curr_block_index].base_addr % OTA_SECTOR_SIZE) != 0))
        {
           return FALSE;
        }


      check_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                p_priv->ota_data[index].p_burn_buf,
                                p_priv->ota_data[index].burn_size);
      
      if(check_crc != p_priv->ota_data[index].crc)
      {
        return FALSE;
      }

      ota_filter_send_message_out(p_priv,
              OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
              p_priv->maincode_block_id,0,0);
      
      ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_BLOCK,
            p_priv->ota_curr_block_id,0,0);
      p_priv->ota_data[index].id = p_priv->sys_block[curr_block_index].id;
      p_priv->ota_data[index].base_addr = p_priv->sys_block[curr_block_index].base_addr;
      sec_start = (p_priv->sys_block[curr_block_index].base_addr) / OTA_SECTOR_SIZE;
      sec_cnt = p_priv->ota_data[index].burn_size / OTA_SECTOR_SIZE;
      ret = ota_filter_burn_flash_burn(p_priv,index,sec_start,sec_cnt,
                                                p_priv->ota_data[index].p_burn_buf);
      if(ret == FALSE)
      {
        return FALSE;
      }
      /****set ota_curr_block id = ota_curr_block_id and save*****/
      ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_SAVE_CURR_OTA_ID,
            p_priv->ota_curr_block_id,
            p_priv->ota_curr_block_id,0);
    
     ota_filter_send_message_out(p_priv,
          OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
          p_priv->maincode_block_id,0,0);
      
    if((p_priv->ota_data[index].p_burn_buf != NULL)
    && (p_priv->ota_data[index].is_new_buf == TRUE))
    {
      ota_filter_free(p_priv,p_priv->ota_data[index].p_burn_buf);
      p_priv->ota_data[index].p_burn_buf = NULL;
      p_priv->ota_data[index].is_new_buf = FALSE;
    }
     p_priv->ota_data[index].burned_flash_bit = TRUE;
      
      
    }
  else if((p_priv->ota_data[index].id == p_priv->ota_curr_block_id) 
      ||(p_priv->ota_data[index].id == p_priv->ota_back_block_id))
    {
       if(p_priv->ota_number < 2)
          /****only one ota don't upg*****/
        {
           return FALSE;
        }
       back_block_index = ota_filter_get_sys_block_index(p_priv,p_priv->ota_back_block_id);
       curr_block_index = ota_filter_get_sys_block_index(p_priv,p_priv->ota_curr_block_id);
       if((back_block_index == 0xff)
          || (curr_block_index == 0xff)
          || ((p_priv->sys_block[back_block_index].base_addr % OTA_SECTOR_SIZE) != 0)
          || ((p_priv->ota_data[index].burn_size % OTA_SECTOR_SIZE) != 0)
          || (p_priv->ota_data[index].burn_size  != p_priv->sys_block[back_block_index].size)
          || (p_priv->sys_block[back_block_index].size  != p_priv->sys_block[curr_block_index].size)
          || ((p_priv->sys_block[curr_block_index].base_addr % OTA_SECTOR_SIZE) != 0))
        {
           return FALSE;
        }

     check_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                p_priv->ota_data[index].p_burn_buf,
                                p_priv->ota_data[index].burn_size);
      
      if(check_crc != p_priv->ota_data[index].crc)
      {
        return FALSE;
      }

      read_len = sizeof(u8)*64;
      memcpy(read_buffer,
                    p_priv->ota_data[index].p_burn_buf + p_priv->ota_data[index].size - read_len,
                    read_len);
      if(0 == memcmp(read_buffer, blh_indicator,12))
      {
        memcpy(&block_info,read_buffer + read_len - bisize,bisize);
        rebuild_head_bit = TRUE;
      }

      ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
        p_priv->ota_curr_block_id,0,0);
      
      ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_BLOCK,
        p_priv->ota_back_block_id,0,0);
      p_priv->ota_data[index].id = p_priv->sys_block[back_block_index].id;
      p_priv->ota_data[index].base_addr = p_priv->sys_block[back_block_index].base_addr;

      if(rebuild_head_bit == TRUE)
      {
          block_info.id = p_priv->ota_data[index].id;
          block_info.base_addr = p_priv->ota_data[index].base_addr - 
                                              p_priv->ota_data[index].dm_offset;
          memcpy(read_buffer + read_len - bisize,&block_info,bisize);
          memcpy(p_priv->ota_data[index].p_burn_buf + p_priv->ota_data[index].size - read_len,
                         read_buffer,read_len);
      }
      sec_start = (p_priv->sys_block[back_block_index].base_addr) / OTA_SECTOR_SIZE;
      sec_cnt = p_priv->ota_data[index].burn_size / OTA_SECTOR_SIZE;
      ret = ota_filter_burn_flash_burn(p_priv,index,sec_start,
                              sec_cnt,p_priv->ota_data[index].p_burn_buf);
      if(ret == FALSE)
      {
        return FALSE;
      }
      
      /****set ota_curr_block id = ota_back_block_id and save*****/
      ota_filter_send_message_out(p_priv,
              OTA_FILTER_EVENT_BURNFLASH_SAVE_CURR_OTA_ID,
              p_priv->ota_back_block_id,
              p_priv->ota_curr_block_id,0);

      ota_filter_send_message_out(p_priv,
              OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
              p_priv->ota_back_block_id,0,0);  
      
      ota_filter_send_message_out(p_priv,
              OTA_FILTER_EVENT_BURNFLASH_BLOCK,
              p_priv->ota_curr_block_id,0,0);
      p_priv->ota_data[index].id = p_priv->sys_block[curr_block_index].id;
      p_priv->ota_data[index].base_addr = p_priv->sys_block[curr_block_index].base_addr;
      if(rebuild_head_bit == TRUE)
      {
          block_info.id = p_priv->ota_data[index].id;
          block_info.base_addr = p_priv->ota_data[index].base_addr - 
                                              p_priv->ota_data[index].dm_offset;
          memcpy(read_buffer + read_len - bisize,&block_info,bisize);
          memcpy(p_priv->ota_data[index].p_burn_buf + p_priv->ota_data[index].size - read_len,
                         read_buffer,read_len);
      }
      sec_start = (p_priv->sys_block[curr_block_index].base_addr) / OTA_SECTOR_SIZE;
      sec_cnt = p_priv->ota_data[index].burn_size / OTA_SECTOR_SIZE;
      ret = ota_filter_burn_flash_burn(p_priv,index,sec_start,
                              sec_cnt,p_priv->ota_data[index].p_burn_buf);
      if(ret == FALSE)
      {
        return FALSE;
      }
      /****set ota_curr_block id = ota_curr_block_id and save*****/
      ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_SAVE_CURR_OTA_ID,
            p_priv->ota_curr_block_id,p_priv->ota_back_block_id,0);
      
      ota_filter_send_message_out(p_priv,
              OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
              p_priv->maincode_block_id,0,0);
      
   if((p_priv->ota_data[index].p_burn_buf != NULL)
    && (p_priv->ota_data[index].is_new_buf == TRUE))
    {
      ota_filter_free(p_priv,p_priv->ota_data[index].p_burn_buf);
      p_priv->ota_data[index].p_burn_buf = NULL;
      p_priv->ota_data[index].is_new_buf = FALSE;
    }
      p_priv->ota_data[index].burned_flash_bit = TRUE;
 }
  
  return TRUE;
}
/*!
  ota filter burn maincode block flash(intra).
  */
static BOOL ota_filter_burn_flash_by_main_code(ota_filter_priv_t *p_priv)
{
  u8 index = 0;
  u32 sec_start = 0;
  u32 sec_cnt = 0;
  u32 check_crc = 0;
  BOOL ret = FALSE;
  
   index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_MAIN_CODE);
   if(index == 0xff)
    {
      return FALSE;
    }
   if(p_priv->ota_data[index].burn_size != (p_priv->flash_size - p_priv->bootload_size))
    {
      return FALSE;
    }
   if((p_priv->bootload_size % OTA_SECTOR_SIZE != 0)
      || (p_priv->ota_data[index].burn_size % OTA_SECTOR_SIZE != 0))
    {
      return FALSE;
    }
    
    check_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                              p_priv->ota_data[index].p_burn_buf,
                              p_priv->ota_data[index].burn_size);
    
    if(check_crc != p_priv->ota_data[index].crc)
    {
      return FALSE;
    }
   ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
        p_priv->ota_curr_block_id,0,0);
   ota_filter_send_message_out(p_priv,
      OTA_FILTER_EVENT_BURNFLASH_BLOCK,
      p_priv->ota_data[index].id,0,0);
    sec_start = (p_priv->ota_data[index].base_addr + p_priv->bootload_size) / OTA_SECTOR_SIZE;
    sec_cnt = p_priv->ota_data[index].burn_size / OTA_SECTOR_SIZE;
    ret = ota_filter_burn_flash_burn(p_priv,index,sec_start,sec_cnt,
                                                  p_priv->ota_data[index].p_burn_buf);
    if(ret == FALSE)
      {
        return FALSE;
      }

    ota_filter_send_message_out(p_priv,
        OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
        p_priv->maincode_block_id,0,0);
     
   if((p_priv->ota_data[index].p_burn_buf != NULL)
    && (p_priv->ota_data[index].is_new_buf == TRUE))
    {
      ota_filter_free(p_priv,p_priv->ota_data[index].p_burn_buf);
      p_priv->ota_data[index].p_burn_buf = NULL;
      p_priv->ota_data[index].is_new_buf = FALSE;
    }
    
    p_priv->ota_data[index].burned_flash_bit = TRUE;
    return TRUE;
}
/*!
  ota filter burn all flash(intra).
  */   
static BOOL ota_filter_burn_flash_by_all_code(ota_filter_priv_t *p_priv,BOOL *force)
{
   u8 index = 0;
  u32 sec_start = 0;
  u32 sec_cnt = 0;
  u32 check_crc = 0;
  BOOL ret =  FALSE;
   index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_ALL_CODE);
   if(index == 0xff)
    {
      return FALSE;
    }
   if(((p_priv->ota_data[index].burn_size != p_priv->flash_size) &&
         (p_priv->ota_data[index].is_factory_force != TRUE))
      ||(p_priv->ota_data[index].burn_size % OTA_SECTOR_SIZE != 0)
      || (p_priv->ota_data[index].base_addr % OTA_SECTOR_SIZE != 0)
      || (p_priv->bootload_size % OTA_SECTOR_SIZE != 0))
    {
      return FALSE;
    }

       
    check_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                              p_priv->ota_data[index].p_burn_buf,
                              p_priv->ota_data[index].burn_size);
    
    if(check_crc != p_priv->ota_data[index].crc)
    {
      return FALSE;
    }
    
   if(p_priv->ota_data[index].is_factory_force == TRUE) /**all flash***/
    {
        /***may be it isn't this flash size***/
        *force = TRUE;
       p_priv->ota_data[index].size = p_priv->ota_data[index].burn_size;
       ota_filter_send_message_out(p_priv,
          OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
          p_priv->ota_curr_block_id,0,0);
       ota_filter_send_message_out(p_priv,
          OTA_FILTER_EVENT_BURNFLASH_BLOCK,
          p_priv->ota_data[index].id,TRUE,0);
       p_priv->ota_data[index].jump_block.block_num = 0;
       sec_start = p_priv->ota_data[index].base_addr / OTA_SECTOR_SIZE;
       sec_cnt = p_priv->ota_data[index].burn_size / OTA_SECTOR_SIZE;
       ret = ota_filter_burn_flash_burn(p_priv,index,sec_start,
                        sec_cnt,p_priv->ota_data[index].p_burn_buf);
       if(ret == FALSE)
      {
        return FALSE;
      }
       ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
            p_priv->maincode_block_id,0,0);
       
    }
   else   /****main code***/
  {
     /***if the all flash need upg ota,it should do it here,but now don't it***/
     *force = FALSE;
     ota_filter_send_message_out(p_priv,
             OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
             p_priv->ota_curr_block_id,0,0);
     ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_BLOCK,
            p_priv->ota_data[index].id,0,0);
     sec_start = (p_priv->ota_data[index].base_addr + p_priv->bootload_size) / OTA_SECTOR_SIZE;
     sec_cnt = (p_priv->ota_data[index].burn_size - p_priv->bootload_size) / OTA_SECTOR_SIZE;
     ret = ota_filter_burn_flash_burn(p_priv,index,sec_start,sec_cnt,
              (u8 *)(p_priv->ota_data[index].p_burn_buf + p_priv->bootload_size));
      if(ret == FALSE)
      {
        return FALSE;
      }
     ota_filter_send_message_out(p_priv,
              OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
              p_priv->maincode_block_id,0,0);
     
  }
   if((p_priv->ota_data[index].p_burn_buf != NULL)
    && (p_priv->ota_data[index].is_new_buf == TRUE))
    {
      ota_filter_free(p_priv,p_priv->ota_data[index].p_burn_buf);
      p_priv->ota_data[index].p_burn_buf = NULL;
      p_priv->ota_data[index].is_new_buf = FALSE;
    }
    p_priv->ota_data[index].burned_flash_bit = TRUE;
    return TRUE;
}

/*!
  ota filter burn flash(intra).
  */  
static void ota_filter_burn_flash_process(ota_filter_priv_t *p_priv)
{
  BOOL force_bit = FALSE;
  BOOL ret = FALSE;
  u8 index = 0;
  BOOL upg_bit = FALSE;

  
  p_priv->db_data_reset_flag = FALSE;
  p_priv->sys_data_reset_flag = FALSE;
  p_priv->ca_data_reset_flag = FALSE;
  p_priv->ads_data_reset_flag = FALSE;
  
  index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_ALL_CODE);
  if(index != 0xff) /***exist all code upg****/
   {
      ret = ota_filter_burn_flash_by_all_code(p_priv,&force_bit);
      if((ret == TRUE) && (force_bit == TRUE)) /***factory all upg***/
      {
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_SUCCESS,0,0,0);
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FACTORY,0,0,0);
      }
      else if ((ret == TRUE) && (force_bit == FALSE))/*** main code upg***/
      {
        #if 0 /***the all***/
         index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_OTA);
         if(index != 0xff) /*** exist ota block upg***/
          {
             ret = ota_filter_burn_flash_by_ota_block(p_priv);
             if(ret == TRUE)
              {
                ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_REBOOT,0,0,0);
              }
             else /***don't process other block***/
              {
                ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
              }
          }
         else
        #endif
          {
            ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_REBOOT,0,0,0);
          }
      }
      else
      {
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
      }
    return;
   }

  index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_MAIN_CODE); 
  if(index != 0xff) /****but ota ,other block don't processs***/
  {
     index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_OTA);
     if(index != 0xff)
      {
         ret = ota_filter_burn_flash_by_ota_block(p_priv);
         if(ret != TRUE)
          {
            ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
            return;
          }
      }
      ret = ota_filter_burn_flash_by_main_code(p_priv);
      if(ret == FALSE) /***upg maincode false!****/
      {
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
      }
      else
      {
          ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_REBOOT,0,0,0);
      }
      return;
  }

   index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_OTA);
   if(index != 0xff)  /***exist ota block upg***/
    {
       ret = ota_filter_burn_flash_by_ota_block(p_priv);
       if(ret != TRUE)
       {
          ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
          return;
        }
       else
        {
          upg_bit = TRUE;
        }
    }

   index = ota_filter_get_ota_type(p_priv,OTA_BURN_FILE_TYPE_ONLY_BLOCK);
   if(index != 0xff)  /***exist other block upg***/
    {
      ret = ota_filter_burn_flash_by_other_block(p_priv);
      if(ret != TRUE)
      {
         ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
         return;
      }
     else
      {
        upg_bit = TRUE;
      }
    }
   if(upg_bit == TRUE)
    {
       ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_REBOOT,0,0,0);
    }
   else
    {
      ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
    }
   return;
 
}

/*!
  ota filter burn flash(intra).
  */  
static void ota_filter_burn_flash(ota_filter_priv_t *p_priv, u32 *param)
{
  u8 i = 0,j = 0 ;
  u8 pos = 0;
  BOOL upg_bit = FALSE;
 u8 sys_block_index = 0;
 u8 dm_block_index = 0;
 u8 index_sys_block = 0;
 
  if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
      &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
  {
    memset(p_priv->ota_block,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
    memcpy(p_priv->ota_block,
        (ota_block_info_t *)param,
        sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
    memset(p_priv->dm_info,0,sizeof(ota_dm_info_t) * OTA_MAX_BLOCK_NUMBER);
    ota_filter_break_all_to_block(p_priv);
    for(i = 0;i < OTA_MAX_BLOCK_NUMBER;i++)
    {
       if((p_priv->ota_block[i].exist_bit == TRUE)
           && (p_priv->ota_block[i].upg_bit == TRUE))
      {
        if(ota_filter_get_ota_data_block(p_priv,p_priv->ota_block[i].id,&pos) ==  TRUE)
          {
              index_sys_block = ota_filter_get_sys_block_index(p_priv,p_priv->ota_block[i].id);
              if(index_sys_block == 0xFF)
              {
                continue;
              }

       if(((p_priv->sys_block[index_sys_block].size >= p_priv->ota_data[pos].burn_size)
                 ||((p_priv->ota_data[pos].is_factory_force == TRUE) 
                     && (p_priv->ota_data[pos].id == OTA_ALL_MAIN_CODE_BLOCK_ID)))
           && (p_priv->ota_data[pos].burn_size > 0)
           &&((p_priv->sys_block[index_sys_block].base_addr % CHARSTO_SECTOR_SIZE) == 0)
           && ((p_priv->sys_block[index_sys_block].size % CHARSTO_SECTOR_SIZE) == 0))
        {
           p_priv->ota_data[pos].type = p_priv->sys_block[index_sys_block].type;
           p_priv->ota_data[pos].base_addr = p_priv->sys_block[index_sys_block].base_addr;
           p_priv->ota_data[pos].size = p_priv->sys_block[index_sys_block].size;
           p_priv->ota_data[pos].dm_offset = p_priv->sys_block[index_sys_block].dm_offset;

           /*******************************************************************
          all flash or main code,don't upg other block,other block ota_version is maincode ota version
          ********************************************************************/
          if(p_priv->ota_data[pos].id == OTA_ALL_MAIN_CODE_BLOCK_ID) 
          {
             if(p_priv->ota_data[pos].burn_size 
                   == (p_priv->flash_size - p_priv->bootload_size)) /***it is maincode***/
              {
                p_priv->ota_data[pos].file_type = OTA_BURN_FILE_TYPE_MAIN_CODE;
                if(p_priv->ota_data[pos].is_factory_force == TRUE)
                  {
                      p_priv->ota_data[pos].is_factory_force = FALSE;
                  }
                ota_filter_add_dmh_info_to_dm(p_priv,
                              p_priv->ota_data[pos].p_burn_buf,
                              p_priv->ota_data[pos].burn_size,
                              p_priv->bootload_size);
              }
             else if((p_priv->ota_data[pos].burn_size == p_priv->flash_size)
                        || ((p_priv->ota_data[pos].is_factory_force == TRUE) 
                              && (p_priv->ota_data[pos].id == OTA_ALL_MAIN_CODE_BLOCK_ID)))
            {
               if(p_priv->ota_data[pos].is_factory_force == TRUE)
                {
                  p_priv->ota_data[pos].jump_block.block_num = 0;
                }
              p_priv->ota_data[pos].file_type = OTA_BURN_FILE_TYPE_ALL_CODE;
              ota_filter_add_dmh_info_to_dm(p_priv,
                        p_priv->ota_data[pos].p_burn_buf,
                        p_priv->ota_data[pos].burn_size,0);
            }
             else
              {
                  upg_bit = FALSE;
                  break;
              }
             
       

            if(p_priv->ota_data[pos].jump_block.block_num > 0) /***need jump same block****/
              {
                  for(j = 0;j < p_priv->ota_data[pos].jump_block.block_num;j ++)
                  {
                     sys_block_index = ota_filter_get_sys_block_index(p_priv,
                                                  p_priv->ota_data[pos].jump_block.block_id[j]);
                     dm_block_index = ota_filter_get_dm_info_exist_index(p_priv,
                                                  p_priv->ota_data[pos].jump_block.block_id[j]);
                     if((sys_block_index == 0xff) || (dm_block_index == 0xff))
                      {
                        upg_bit = FALSE;
                        break;
                      }
                     if((p_priv->dm_info[dm_block_index].size != 
                              p_priv->sys_block[sys_block_index].size)
                      || (p_priv->dm_info[dm_block_index].base_addr != 
                             p_priv->sys_block[sys_block_index].base_addr))
                      {
                        upg_bit = FALSE;
                        break;
                      }
                  }
              }
            
          }
       else if((p_priv->ota_data[pos].id == p_priv->ota_curr_block_id) 
              ||(p_priv->ota_data[pos].id == p_priv->ota_back_block_id))
          {
             if(TRUE == p_priv->ota_in_maincode_bit)  
              {
                 /***run on maicode****/
                 if(p_priv->load_block_id != p_priv->maincode_block_id) 
                  {
                    continue;
                  }
              }
             else
              {
                 /***only one ota****/
                 if(p_priv->ota_number < 2) 
                  {
                    continue;
                  }
              }
               
              if(p_priv->ota_data[pos].burn_size < p_priv->ota_data[pos].size)
              {
                /***make block head***/
                if(ota_filter_make_block_header(p_priv,p_priv->ota_data[pos].id) != TRUE)
                  {
                    upg_bit = FALSE;
                    break;
                  }
              }
             else
              {
                /*******check block head ******/
                if(ota_filter_check_block_header(p_priv,p_priv->ota_data[pos].id) != TRUE)
                  {
                    upg_bit = FALSE;
                    break;
                  }
              }

             p_priv->ota_data[pos].jump_block.block_num = 0;
             p_priv->ota_data[pos].file_type = OTA_BURN_FILE_TYPE_OTA;
          }
         else
          {
               if(p_priv->ota_data[pos].burn_size < p_priv->ota_data[pos].size)
              {
                /***make block head***/
                if(ota_filter_make_block_header(p_priv,p_priv->ota_data[pos].id) != TRUE)
                  {
                    upg_bit = FALSE;
                    break;
                  }
              }
             else
              {
                /*******check block head ******/
                if(ota_filter_check_block_header(p_priv,p_priv->ota_data[pos].id) != TRUE)
                  {
                    upg_bit = FALSE;
                    break;
                  }
              }
               p_priv->ota_data[pos].jump_block.block_num = 0;
               p_priv->ota_data[pos].file_type = OTA_BURN_FILE_TYPE_ONLY_BLOCK;
          }
           p_priv->ota_data[pos].upg_bit = TRUE;
           p_priv->ota_data[pos].burned_flash_bit = FALSE;
           upg_bit = TRUE;
        }
          }
      }
    }
    if(upg_bit != TRUE)
    {
        /**sent download fail message**/
        p_priv->g_burn_mark = FALSE;;
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_FAIL,0,0,0);
    }
    else
    {
      p_priv->g_burn_mark = TRUE;
      ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_BURNFLASH_START,0,0,0);
      ota_filter_burn_flash_process(p_priv);
      p_priv->g_burn_mark = FALSE;
    }
    
    ota_filter_reset_ota_data(p_priv);
    
    if(p_priv->p_save_buf != NULL)
      {
        ota_filter_free(p_priv,p_priv->p_save_buf);
        p_priv->p_save_buf = NULL;
      }
     if(p_priv->p_burn_buf != NULL)
      {
        ota_filter_free(p_priv,p_priv->p_burn_buf);
        p_priv->p_burn_buf = NULL;
      }
  }
}

/*!
  ota filter download data(intra).
  */  
static void ota_filter_download_all_data(ota_filter_priv_t *p_priv, u32 *param)
{
  if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
      &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
  {
    OS_PRINTF("####[%s]debug check download filesize:%d\n",
                                      __FUNCTION__,
                                      p_priv->upg_check_info.upg_file_size);
    memset(&p_priv->upg_check_info,0,sizeof(upg_check_info_t));
    memcpy(&p_priv->upg_check_info,(upg_check_info_t *)param,sizeof(upg_check_info_t));
    OS_PRINTF("####[%s]debug2 check download filesize:%d\n",
                                __FUNCTION__,
                                p_priv->upg_check_info.upg_file_size);
    if(p_priv->upg_check_info.upg_bit != TRUE)
    {
        /**sent download fail message**/
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_FAIL,0,0,0);
        return;
    }
     ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_START,0,0,0);
      /***fist reques****/
     if(TRUE != ota_filter_request_all_dsmcc_data(p_priv,&p_priv->upg_check_info))
      {
        /**sent download fail message**/
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_FAIL,0,0,0);
    }
     
  }
}

/*!
  ota filter check data(intra).
  */  
static void ota_filter_ota_check(ota_filter_priv_t *p_priv)
{
   ota_input_pin_t         *p_pin  = &p_priv->m_in_pin;
   if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
  &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
  {
     dsmcc_parse_clear_dsi_info();
     memset(p_priv->ota_data,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
     //ota_ipin_dsmcc_request_dsi_single(p_priv,p_priv->data_pid);
     // p_pin->ota_input_pin_reques(p_pin,REQUES_TABLE_DSMCC_MSG_DSI,p_priv->data_pid);
     p_priv->dsi_multi_timeout = mtos_ticks_get() + (p_priv->dsmcc_msg_timeout / 10);
     p_priv->g_check_timeout =  TRUE;
     p_pin->ota_input_pin_reques(p_pin,REQUES_TABLE_DSMCC_MSG_DSI_MULTI,p_priv->data_pid);
  }
}
/*!
  ota filter check monitor status.
  */
static BOOL ota_filter_check_monitor_status(ota_filter_priv_t *p_priv)
{
      if((p_priv->run_select == OTA_WORK_SELECT_MONITOR)
       && (p_priv->monitor_enable_flag == TRUE))
      {
         p_priv->monitor_ticks = mtos_ticks_get();
         p_priv->loop_sm = OTA_FILTER_SM_IDLE;
         return TRUE;
      }
      else
        {
           return FALSE;
        }
      //return FALSE;
}
/*!
  ota filter ota filter config(intra).
  */  
static void ota_filter_config(ota_filter_priv_t *p_priv,u32 *p_para)
{
    ota_input_pin_t         *p_pin  = &p_priv->m_in_pin;
    ota_filter_cfg_t *p_cfg =  NULL;
    RET_CODE ret = ERR_FAILURE;
    
    p_cfg = (ota_filter_cfg_t *)p_para; 
    p_priv->medium = p_cfg->medium;
    p_priv->protocol_type = p_cfg->protocol_type;
   // p_priv->run_select = p_cfg->run_select;
    memcpy(p_priv->sys_block,p_cfg->filter_cfg.sys_block,
                 sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
   
    memcpy(&p_priv->hw_tdi,&p_cfg->filter_cfg.hw_tdi,sizeof(ota_hw_tdi_t));

    p_priv->p_attach_buffer = p_cfg->filter_cfg.p_attach_buffer;
    p_priv->buffer_size = p_cfg->filter_cfg.buffer_size;
    p_priv->flash_size = p_cfg->filter_cfg.flash_size;
    p_priv->bootload_size = p_cfg->filter_cfg.bootload_size;

    p_priv->upg_version = p_cfg->filter_cfg.upg_version;
    
    p_priv->fail_times = p_cfg->filter_cfg.fail_times;
    p_priv->destroy_flag = p_cfg->filter_cfg.destroy_flag;
    p_priv->destroy_block_id = p_cfg->filter_cfg.destroy_block_id;
    p_priv->tri_mode = p_cfg->filter_cfg.tri_mode;
    p_priv->max_fail_times = p_cfg->filter_cfg.max_fail_times;
     
    p_priv->ota_number  = p_cfg->filter_cfg.ota_number;
    p_priv->ota_curr_block_id = p_cfg->filter_cfg.ota_curr_block_id;
    p_priv->ota_back_block_id = p_cfg->filter_cfg.ota_back_block_id;
    p_priv->load_block_id = p_cfg->filter_cfg.load_block_id;
    p_priv->maincode_block_id = p_cfg->filter_cfg.maincode_block_id;
    p_priv->ota_in_maincode_bit = p_cfg->filter_cfg.ota_in_maincode_bit;

    if(p_priv->ota_curr_block_id == p_priv->ota_back_block_id)
      {
        p_priv->ota_number = 1;
      }
    
    if((p_priv->ota_number >= 2) ||(TRUE == p_priv->ota_in_maincode_bit))
    {
      p_priv->allow_upg_ota_bit = TRUE;
    }
    else
    {
      p_priv->allow_upg_ota_bit = FALSE;
    }
    
    
    p_priv->debug_level = p_cfg->filter_cfg.debug_level;
    p_priv->ota_filter_debug_printf = p_cfg->filter_cfg.ota_debug_printf;
    p_priv->ota_do_lock = p_cfg->filter_cfg.ota_do_lock;
    p_priv->dsmcc_msg_timeout = p_cfg->filter_cfg.dsmcc_msg_timeout;
    p_priv->ota_get_lock_status = p_cfg->filter_cfg.ota_get_lock_status;
    
    if(p_priv->dsmcc_msg_timeout  <= 0)
      {
        p_priv->dsmcc_msg_timeout  = DSMCC_MSG_TIMEOUT;
      }
    p_priv->ota_version_check = p_cfg->filter_cfg.ota_version_check;
    p_priv->ota_dsi_info_check = p_cfg->filter_cfg.ota_dsi_info_check;
    p_priv->ota_block_data_verify = p_cfg->filter_cfg.ota_block_data_verify;
    p_priv->ota_md5_data_sign = p_cfg->filter_cfg.ota_md5_data_sign;

  if((p_priv->p_attach_buffer != NULL) && (p_priv->buffer_size > 0))
    {
        ret = lib_memp_create(&p_priv->mem_heap, (u32)p_priv->p_attach_buffer, p_priv->buffer_size);
        if(ret == SUCCESS)
        {
          p_priv->use_memp = TRUE;
          p_priv->p_mem_heap = &p_priv->mem_heap;
        }
        else
        {
          p_priv->use_memp = FALSE;
          p_priv->p_mem_heap = NULL;
        }
    }
  else
    {
      p_priv->use_memp = FALSE;
      p_priv->p_mem_heap = NULL;
    }
  
    if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
    &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
    {
       memset(p_priv->ota_data,0,sizeof(ota_data_info_t) * OTA_MAX_BLOCK_NUMBER);
       dsmcc_parse_init(p_priv->ota_data,
        p_cfg->filter_cfg.debug_level,
        p_cfg->filter_cfg.ota_debug_printf);
    }
    p_priv->g_direct_dl = p_cfg->filter_cfg.g_direct_dl;
    
    memcpy(&p_priv->ipin_cfg.filter_param,
                    &p_cfg->filter_cfg.filter_param,
                    sizeof(dmx_filter_param_t));
      
    p_priv->ipin_cfg.medium = p_priv->medium;
    p_priv->ipin_cfg.protocol_type = p_priv->protocol_type;
    
    p_priv->ipin_cfg.p_mem_heap = p_priv->p_mem_heap;
    p_priv->ipin_cfg.use_memp =  p_priv->use_memp;
    
    p_priv->ipin_cfg.dmx_buf_size = p_cfg->filter_cfg.dmx_buf_size;
    p_priv->ipin_cfg.dsmcc_msg_timeout = p_priv->dsmcc_msg_timeout;
    p_pin->config(p_pin,(ota_ipin_cfg_t *)&p_priv->ipin_cfg);
}

/*!
  ota filter ota filter transform(intra).
  */ 
static RET_CODE ota_filter_transform(handle_t _this, media_sample_t *p_in)
{
  ota_filter_priv_t *p_priv = ota_filter_get_priv(_this);
  if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
    &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
    {
      ota_filter_process_dsmcc_protocol(p_priv,p_in);
    }
  return SUCCESS;
}

/*!
  ota filter do command(intra).
  */ 
static RET_CODE ota_filter_on_command(handle_t _this, icmd_t *p_cmd)
{
  ota_filter_priv_t *p_priv = ota_filter_get_priv(_this);
  ota_input_pin_t         *p_pin  = &p_priv->m_in_pin;
  ota_monitor_para_t monitor_info = {0};

  MT_ASSERT(p_cmd != NULL);
  switch(p_cmd->cmd)
  {
      case OTA_FILTER_CMD_CONFIG:
        ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:config\n");
        ota_filter_config(p_priv,(u32 *)p_cmd->p_para);
      break;
      case OTA_FILTER_CMD_LOCK_PROCESS:
        ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:do lock\n");
        ota_filter_process_lock_result(p_priv,p_cmd->lpara);
      break;
      case OTA_FILTER_CMD_DO_POLICY:
        ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:do policy\n");
        p_priv->run_select = p_cmd->lpara;
        switch(p_cmd->lpara)
        {
          case OTA_WORK_SELECT_AUTO:
          ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:do policy->auto\n");
          memcpy(&p_priv->tp_info,(ota_tp_info_t *)p_cmd->p_para,sizeof(ota_tp_info_t));
          p_priv->loop_sm = OTA_FILTER_SM_AUTO_START; 
          break;
          case OTA_WORK_SELECT_CHECK:
          ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:do policy->check\n");
          if(p_priv->medium == OTA_MEDIUM_BY_TUNER)
          {
            ota_tp_info_t *p_all_tp_info = NULL;
            p_all_tp_info = (ota_tp_info_t *)p_cmd->p_para;
            memcpy(&p_priv->tp_info.do_ota_tp,&p_all_tp_info->do_ota_tp,sizeof(nim_info_t));
          }
          p_priv->loop_sm = OTA_FILTER_SM_CHECK_START;
          break;
          case OTA_WORK_SELECT_DOWNLOAD:
          ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:do download\n");
          memcpy(&p_priv->upg_check_intra_info,
                      (upg_check_info_t *)p_cmd->p_para,
                      sizeof(upg_check_info_t));
          p_priv->sm_param = (u32)((u32 *)&p_priv->upg_check_intra_info);
          p_priv->loop_sm = OTA_FILTER_SM_DOWNLOAD_START;
          break;
          case OTA_WORK_SELECT_BURNFLASH:
          ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:do burnfalsh\n");
          memcpy(p_priv->ota_intra_block,
                        (ota_block_info_t *)&p_cmd->p_para,
                        sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
          p_priv->sm_param = (u32)((u32 *)p_priv->ota_intra_block);
          p_priv->loop_sm = OTA_FILTER_SM_BURNFLASH_START;

          break;
          case OTA_WORK_SELECT_UPGGRADE:
          ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:do upg\n");
          memcpy(&p_priv->upg_check_intra_info,
                      (upg_check_info_t *)p_cmd->p_para,
                      sizeof(upg_check_info_t));
          p_priv->sm_param = (u32)((u32 *)&p_priv->upg_check_intra_info);
          p_priv->loop_sm = OTA_FILTER_SM_DOWNLOAD_START;
          break;
          case OTA_WORK_SELECT_MONITOR:
          memcpy(&monitor_info,
                        (ota_monitor_para_t *)p_cmd->p_para,
                        sizeof(ota_monitor_para_t));
          if(monitor_info.cmd == OTA_MONITOR_CMD_START)
            {
                p_priv->monitor_enable_flag = TRUE;
                if(monitor_info.monitor_cycle_time < p_priv->dsmcc_msg_timeout)
                {
                   monitor_info.monitor_cycle_time = p_priv->dsmcc_msg_timeout + 100 ;
                }
                p_priv->monitor_timeout = monitor_info.monitor_cycle_time  / 10;
                p_priv->monitor_ticks = mtos_ticks_get();
                p_priv->data_pid = monitor_info.monitor_pid;
                p_priv->loop_sm = OTA_FILTER_SM_MONITOR_START;
                ota_filter_debug_printf(p_priv,
                                    OTA_DEBUG_INFO,
                                    "ota filter:monitor start: cycle time:%d ms\n",
                                    p_priv->monitor_timeout * 10);
            }
           else /***stop**/
            {
                p_priv->monitor_enable_flag = FALSE;
                p_priv->monitor_timeout = monitor_info.monitor_cycle_time;
                p_priv->monitor_ticks = mtos_ticks_get();
                p_priv->loop_sm = OTA_FILTER_SM_IDLE;
                p_pin->ota_input_pin_free(p_pin,FREE_TABLE_DSMCC_MSG_DSI_MULTI,p_priv->data_pid);
                ota_filter_debug_printf(p_priv,
                                    OTA_DEBUG_INFO,
                                    "ota filter:monitor stop\n");
            }
          break;
          case OTA_WORK_SELECT_INQUIRE_STOP:
          break;
          default:
          break;
        }

       /***other mode disable monitor ****/
      if((p_cmd->lpara != OTA_WORK_SELECT_MONITOR)
         && (p_priv->monitor_enable_flag == TRUE))
        {
          p_priv->monitor_enable_flag = FALSE;
        }        
      break;
      default:
      ota_filter_debug_printf(p_priv,OTA_DEBUG_ERROR,"[%s]:%d", __FUNCTION__,p_cmd->cmd);
      break;
      }

  return SUCCESS;
}

/*!
  ota filter do command(intra).
  */ 
static RET_CODE ota_filter_on_evt(handle_t _this, os_msg_t *p_msg)
{
#ifndef OTA_FILTER_USE_FLASH_FILTER
  media_sample_t *p_sample = (media_sample_t *)p_msg->para1;
  switch(p_msg->content)
  {
    case OTA_INPUT_PIN_EVT_RECEIVE_DATA:
    ota_filter_transform(_this,p_sample);
    break;
    default:
    break;
  }
#endif  

  return SUCCESS;
}


/*!
 state machine
 */
static void ota_filter_active_loop(handle_t _this)
{
  ota_filter_priv_t *p_priv = ota_filter_get_priv(_this);
  ota_input_pin_t *p_ipin = &p_priv->m_in_pin;
  u16 table_id = 0;
  BOOL g_locked_flg = FALSE;
  switch(p_priv->loop_sm)
  {
    case OTA_FILTER_SM_IDLE:
      if(p_priv->monitor_enable_flag == TRUE)
      {
         if((mtos_ticks_get() - p_priv->monitor_ticks) 
             > p_priv->monitor_timeout)
          {
             p_priv->monitor_ticks = mtos_ticks_get();
             p_priv->loop_sm = OTA_FILTER_SM_MONITOR_START;
          }
      }

      if(p_priv->g_check_timeout == TRUE)
      {
          if(p_priv->dsi_multi_timeout <= mtos_ticks_get())
          {
           p_priv->dsi_multi_timeout = mtos_ticks_get() 
                                    + (p_priv->dsmcc_msg_timeout / 10);
            p_priv->loop_sm = OTA_FILTER_SM_CHECK_FAIL;
            p_priv->g_check_timeout = FALSE;
          }
      }
      
    break;
    case OTA_FILTER_SM_AUTO_START:
      p_priv->download_start_flag = FALSE;
      if(p_priv->medium == OTA_MEDIUM_BY_TUNER)
      {
        p_priv->lock_type = OTA_FILTER_LOCK_TP_NOTHING;
        p_priv->download_try_times  = 0;
        ota_api_on_auto_lock_tp(p_priv,0);
      }
      p_priv->loop_sm = OTA_FILTER_SM_IDLE;
    break;
    case OTA_FILTER_SM_CHECK_START:
      p_priv->download_start_flag = FALSE;
      p_priv->g_check_timeout =  FALSE;
      if(p_priv->medium == OTA_MEDIUM_BY_TUNER)
      {
       ota_api_on_check_tp(p_priv);
      }
      else
      {
        ota_filter_ota_check(p_priv);
      }
      p_priv->loop_sm = OTA_FILTER_SM_IDLE;
    break;
    case OTA_FILTER_SM_LOCKED:
     if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
        && ((p_priv->run_select == OTA_WORK_SELECT_AUTO) 
        ||(p_priv->run_select == OTA_WORK_SELECT_CHECK)))
      {
        #if 0 
        if((p_priv->run_select == OTA_WORK_SELECT_AUTO)
          && (p_priv->g_direct_dl == TRUE))
        {
           ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,
                            "ota filter:direct downlod\n");
           memset(&p_priv->upg_check_intra_info,0,sizeof(upg_check_info_t));
           p_priv->upg_check_intra_info.dsi_group_id = 
                                              p_priv->ipin_cfg.filter_param.group_id;
           p_priv->data_pid = p_priv->ipin_cfg.filter_param.pid;
           
           p_priv->upg_check_intra_info.upg_bit = TRUE;
           p_priv->upg_check_intra_info.upg_file_size = 
                                                   p_priv->flash_size + (10 * KBYTES);
           p_priv->sm_param = (u32)((u32 *)&p_priv->upg_check_intra_info);
           ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:dowload data\n");
          p_priv->loop_sm = OTA_FILTER_SM_DOWNLOAD_START;
          break;
        }
        else
        #endif
        {
           ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:it will check\n");
           ota_filter_ota_check(p_priv);
        }
      }
     p_priv->loop_sm = OTA_FILTER_SM_IDLE;
    break;
    case OTA_FILTER_SM_UNLOCK:
    if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
        &&(OTA_WORK_SELECT_AUTO == p_priv->run_select))
      {
         if(TRUE != ota_api_on_auto_lock_tp(p_priv,1))
          {
            p_priv->lock_type = OTA_FILTER_LOCK_TP_NOTHING;
            if(p_priv->g_timeout_mark == TRUE)
            {
              p_ipin->ota_input_pin_free(p_ipin,FREE_TABLE_DSMCC_MSG_DSI_MULTI,p_priv->data_pid);
              ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_REQEUST_TIMEOUT,
                                                                  DVB_TABLE_ID_DSMCC_MSG,
                                                                  p_priv->data_pid,
                                                                  0);
              p_priv->g_timeout_mark = FALSE;
            }
            else
            {
              ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_UNLOCK,0,0,0);
               if(p_priv->download_start_flag == FALSE)
              {
                ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_CHECK_FAIL,0,0,0);
              }
            }
            
          }
      }  
    else if(p_priv->medium == OTA_MEDIUM_BY_TUNER)
      {
         ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_UNLOCK,0,0,0);
         if(p_priv->download_start_flag == FALSE)
          {
            ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_CHECK_FAIL,0,0,0);
          }
      }
     p_priv->loop_sm = OTA_FILTER_SM_IDLE;
    break;
    case OTA_FILTER_SM_REQUEST_TIMEOUT:
    p_priv->g_check_timeout = FALSE; 
    table_id = p_priv->sm_param & 0xFFFF;
    
    if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
        && (p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC)
        && (table_id == DVB_TABLE_ID_DSMCC_MSG))
         /*****only process dsmcc msg but ddm****/
      {
         if(p_priv->run_select == OTA_WORK_SELECT_AUTO)
        {
           p_priv->g_timeout_mark = TRUE;
           if(TRUE != ota_api_on_auto_lock_tp(p_priv,1))
            {
              p_priv->lock_type = OTA_FILTER_LOCK_TP_NOTHING;
              p_priv->g_timeout_mark =  FALSE; 
              p_ipin->ota_input_pin_free(p_ipin,FREE_TABLE_DSMCC_MSG_DSI_MULTI,p_priv->data_pid);
              ota_filter_send_message_out(p_priv,
                    OTA_FILTER_EVENT_REQEUST_TIMEOUT,
                   table_id,
                   (p_priv->sm_param >> 16) & 0xFFFF,
                   0);
              p_priv->loop_sm = OTA_FILTER_SM_IDLE;
              break;
            }
        }
       else
        {
          p_priv->g_timeout_mark =  FALSE; 
          p_ipin->ota_input_pin_free(p_ipin,FREE_TABLE_DSMCC_MSG_DSI_MULTI,p_priv->data_pid);
          if(TRUE == ota_filter_check_monitor_status(p_priv))
          {
            p_priv->loop_sm = OTA_FILTER_SM_IDLE;
            break;
            /***don't send timeout msg to outside****/
          }
          ota_filter_send_message_out(p_priv,
                OTA_FILTER_EVENT_REQEUST_TIMEOUT,
               table_id,
               (p_priv->sm_param >> 16) & 0xFFFF,
               0);
          p_priv->loop_sm = OTA_FILTER_SM_IDLE;
            
          break;
        }
    }
    else if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
        && (p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC)
        && (table_id == DVB_TABLE_ID_DSMCC_DDM))
      {
        /*****don't process*****/
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_TIMEOUT,0,0,0);
      }
   
    p_priv->loop_sm = OTA_FILTER_SM_IDLE;
    break;
    case OTA_FILTER_SM_CHECK_SUCCESS:
    p_priv->g_check_timeout = FALSE; 
    p_ipin->ota_input_pin_free(p_ipin,FREE_TABLE_DSMCC_MSG_DSI_MULTI,p_priv->data_pid);
    if(p_priv->run_select == OTA_WORK_SELECT_AUTO)
    {
       memcpy(&p_priv->upg_check_intra_info,&p_priv->upg_check_info,sizeof(upg_check_info_t));
       p_priv->sm_param = (u32)((u32 *)&p_priv->upg_check_intra_info);
      ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:dowload data\n");
      p_priv->loop_sm = OTA_FILTER_SM_DOWNLOAD_START; /****goto download****/
    }
    else
    {
         ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_CHECK_SUCCESS,
                                   (u32)((u32 *)&p_priv->upg_check_info),
                                   (u32)((u32 *)&p_priv->tp_info.do_ota_tp),
                                   (u32)p_priv->run_select);
       if(TRUE == ota_filter_check_monitor_status(p_priv))
      {
         p_priv->monitor_enable_flag = FALSE;
         /***stop monitor***/
      }
      if((TRUE == p_priv->ota_in_maincode_bit) 
          && (p_priv->load_block_id == p_priv->maincode_block_id))
        {
          ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
            p_priv->maincode_block_id,0,0);
        }
       else
        {
          ota_filter_send_message_out(p_priv,
            OTA_FILTER_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID,
            p_priv->ota_curr_block_id,0,0);
        }
        p_priv->loop_sm = OTA_FILTER_SM_IDLE;
    }
    break;
    case OTA_FILTER_SM_DOWNLOAD_SUCCESS:
      if((p_priv->run_select == OTA_WORK_SELECT_AUTO)
          ||(p_priv->run_select == OTA_WORK_SELECT_UPGGRADE))
      {
         ota_filter_debug_printf(p_priv,OTA_DEBUG_INFO,"ota filter:burn flash\n");
         p_priv->loop_sm = OTA_FILTER_SM_BURNFLASH_START;
      }
      else
      {
        p_priv->loop_sm = OTA_FILTER_SM_IDLE;
      }
    break;
    case OTA_FILTER_SM_DOWNLOAD_START:
     p_priv->download_start_flag = TRUE;
     ota_filter_download_all_data(p_priv,(u32 *)p_priv->sm_param);
     p_priv->loop_sm = OTA_FILTER_SM_IDLE; 
    break;
    case OTA_FILTER_SM_CHECK_DOWNLOAD_DATA:
     ota_filter_check_download_data(p_priv);
    /***don't change loop sm here****/
    break;
    case OTA_FILTER_SM_BURNFLASH_START:
      ota_filter_burn_flash(p_priv,(u32 *)p_priv->sm_param);
      p_priv->loop_sm = OTA_FILTER_SM_IDLE; 
     break;
    case OTA_FILTER_SM_INQUIRE_STOP:
    if(p_priv->g_burn_mark == FALSE)
    {
     ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_STOP_RESULT,
                                                         OTA_STOP_RESULT_ENABLE_STOP,0,0);
    }
    else
    {
      ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_STOP_RESULT,
                                                              OTA_STOP_RESULT_FORBID_STOP,0,0);
    }
    break;
    case OTA_FILTER_SM_MONITOR_START:
      g_locked_flg = FALSE;
      if(p_priv->ota_get_lock_status != NULL)
      {
          g_locked_flg = p_priv->ota_get_lock_status();
      }

      if(g_locked_flg)
        {
            ota_filter_debug_printf(p_priv,
                                                    OTA_DEBUG_INFO,
                                                    "ota filter:ota monitor do check!\n");
            ota_filter_ota_check(p_priv);
            p_priv->monitor_ticks = mtos_ticks_get();
            p_priv->loop_sm = OTA_FILTER_SM_IDLE;
        }
      else
        {
            p_priv->monitor_ticks = mtos_ticks_get();
            p_priv->loop_sm = OTA_FILTER_SM_IDLE;
        }
    break;
    case OTA_FILTER_SM_CHECK_FAIL:
      p_priv->g_check_timeout = FALSE; 
     p_ipin->ota_input_pin_free(p_ipin,FREE_TABLE_DSMCC_MSG_DSI_MULTI,p_priv->data_pid);
     if(TRUE == ota_filter_check_monitor_status(p_priv))
      {
        p_priv->loop_sm = OTA_FILTER_SM_IDLE;
        /***don't send timeout msg to outside****/
        break;
      }
      if((p_priv->run_select == OTA_WORK_SELECT_AUTO)
          && (p_priv->g_direct_dl == TRUE))
      {
        OS_PRINTF("###debug ota filter:download fail msg\n");
        ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_DOWNLOAD_FAIL,0,0,0);
      }
    else
      {
       OS_PRINTF("###debug ota filter:send check fail msg\n");
       ota_filter_send_message_out(p_priv,OTA_FILTER_EVENT_CHECK_FAIL,0,0,0);
      }
     p_priv->loop_sm = OTA_FILTER_SM_IDLE;

    break;
    default:
    break;
  }
  
}

static RET_CODE ota_filter_on_open(handle_t _this)
{
  ota_filter_priv_t *p_priv    = ota_filter_get_priv(_this);
  ifilter_t         *p_ifilter = (ifilter_t *)p_priv->p_this;

  p_ifilter->set_active_enter(p_ifilter, ota_filter_active_loop);
  return SUCCESS;
}

static RET_CODE ota_filter_on_close(handle_t _this)
{
  ota_filter_priv_t *p_priv = ota_filter_get_priv(_this);
  ifilter_t *p_ifilter      = (ifilter_t *)p_priv->p_this;

  p_ifilter->set_active_enter(p_ifilter, NULL);
  return SUCCESS;
}

static RET_CODE ota_filter_on_start(handle_t _this)
{
  ota_filter_priv_t *p_priv = ota_filter_get_priv(_this);
  p_priv->p_receiver_buffer = ota_filter_malloc(p_priv,EIT_SECTION_BUFFER_SIZE);
  if(p_priv->p_receiver_buffer == NULL)
  {
    ota_filter_debug_printf(p_priv,
                                           OTA_DEBUG_ERROR,
                                           "ota filter:malloc receiver buffer err\n");
    return ERR_FAILURE;
  }
  return SUCCESS;
}
static RET_CODE ota_filter_on_stop(handle_t _this)
{
  ota_filter_priv_t *p_priv = ota_filter_get_priv(_this);
  
  if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
  &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
  {
    ota_filter_reset_ota_data(p_priv);
    if(p_priv->p_save_buf != NULL)
      {
        ota_filter_free(p_priv,p_priv->p_save_buf);
        p_priv->p_save_buf = NULL;
      }
     if(p_priv->p_burn_buf != NULL)
      {
        ota_filter_free(p_priv,p_priv->p_burn_buf);
        p_priv->p_burn_buf = NULL;
      }
    dsmcc_parse_deinit();
  }
  
  if(p_priv->p_receiver_buffer  !=  NULL)
  {
     ota_filter_free(p_priv,p_priv->p_receiver_buffer);
     p_priv->p_receiver_buffer  =  NULL;
  }
  
  return SUCCESS;
}

static void ota_filter_on_destroy(handle_t _this)
{ 
  mtos_free(_this);
  _this = NULL;
}

/*!
 create ota filter.
 */
#ifdef OTA_FILTER_USE_FLASH_FILTER
ifilter_t *ota_filter_create(void *p_para)
{
  interface_t         *p_interface  = NULL;
  ifilter_t               *p_ifilter    = NULL;
  ota_filter_t        *p_ota_filter = NULL;
  ota_filter_priv_t *p_priv       = NULL;
  transf_filter_t *p_transf_filter = NULL;
  transf_filter_para_t transf_filter_para = {0};

  p_ota_filter = (ota_filter_t *)mtos_malloc(sizeof(ota_filter_t));
  MT_ASSERT(p_ota_filter != NULL);
  memset(p_ota_filter, 0x0, sizeof(ota_filter_t));

  p_priv = &p_ota_filter->m_priv_data;

  
  p_priv->p_this = (void *)p_ota_filter;

    //create base class
  transf_filter_create(&p_ota_filter->m_filter, &transf_filter_para);
  p_transf_filter = (transf_filter_t *)p_ota_filter;
  p_transf_filter->transform = ota_filter_transform;

  p_interface = (interface_t *)p_ota_filter;
  p_interface->_rename(p_interface, "ota_filter");
  p_interface->on_destroy = ota_filter_on_destroy;
  
  p_ifilter = (ifilter_t *)p_ota_filter;
  p_ifilter->on_open    = ota_filter_on_open;
  p_ifilter->on_close   = ota_filter_on_close;
  p_ifilter->on_process_evt = ota_filter_on_evt;
  p_ifilter->on_start = ota_filter_on_start;
  p_ifilter->on_stop = ota_filter_on_stop;
  p_ifilter->on_command = ota_filter_on_command;

  //p_ifilter->_interface.on_destroy = ota_filter_on_destroy;

  //create it's pin
  ota_input_pin_create(&p_priv->m_in_pin, (interface_t *)p_ota_filter);
  ota_output_pin_create(&p_priv->m_out_pin, (interface_t *)p_ota_filter);
  return p_ifilter;
}
#else
/*!
 base class
 */  
ifilter_t *ota_filter_create(void *p_para)
{
  interface_t         *p_interface  = NULL;
  ifilter_t               *p_ifilter    = NULL;
  ota_filter_t        *p_ota_filter = NULL;
  ota_filter_priv_t *p_priv       = NULL;


  p_ota_filter = (ota_filter_t *)mtos_malloc(sizeof(ota_filter_t));
  MT_ASSERT(p_ota_filter != NULL);
  memset(p_ota_filter, 0x0, sizeof(ota_filter_t));

  p_priv = &p_ota_filter->m_priv_data;

  
  p_priv->p_this = (void *)p_ota_filter;

    //create base class
  sink_filter_create(&p_ota_filter->m_filter, NULL);

  p_interface = (interface_t *)p_ota_filter;
  p_interface->_rename(p_interface, "ota_filter");
  p_interface->on_destroy = ota_filter_on_destroy;
  

  p_ifilter = (ifilter_t *)p_ota_filter;
  p_ifilter->on_open    = ota_filter_on_open;
  p_ifilter->on_close   = ota_filter_on_close;
  p_ifilter->on_process_evt = ota_filter_on_evt;
  p_ifilter->on_start = ota_filter_on_start;
  p_ifilter->on_stop = ota_filter_on_stop;
  p_ifilter->on_command = ota_filter_on_command;

  //p_ifilter->_interface.on_destroy = ota_filter_on_destroy;

  //create it's pin
  ota_input_pin_create(&p_priv->m_in_pin, (interface_t *)p_ota_filter);
  return p_ifilter;
}
#endif
