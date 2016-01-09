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

//util
#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "dvb_protocol.h"
#include "fcrc.h"

//driver
#include "dmx.h"
#include "class_factory.h"
#include "common.h"
#include "drv_dev.h"
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
#include "ipin.h"
#include "input_pin.h"
#include "sink_pin.h"

#include "transf_input_pin.h"

#include "ifilter.h"
#include "sink_filter.h"
#include "eva_filter_factory.h"

#include "demux_interface.h"
#include "demux_filter.h"

#include "ota_public_def.h"
#include "ota_filter.h"
#include "ota_filter_dsmcc.h"
#include "ota_input_pin_intra.h"


static ota_input_pin_priv_t * ota_input_pin_get_priv(handle_t _this)
{
  MT_ASSERT(_this != NULL);
  return &(((ota_input_pin_t *)_this)->m_priv_data);
}

/*!
  API for allocating memory in OTA module
  \param[in] mem size memory size to be allocated with byte as unit
  \param[in] id partition id to be allocated
  */
static void *ota_input_pin_malloc(ota_input_pin_priv_t *p_priv,u32 mem_size)
{
  if(p_priv->use_memp == TRUE)
  {
    return lib_memp_alloc(p_priv->p_mem_heap, mem_size);
  }
  else
  {
    return mtos_malloc(mem_size);
  }
}
   
/*!
3
  API for allocating memory in ota
  \param[in] mem size memory size to be allocated with byte as unit
  \param[in] id partition id to be allocated
  */
static RET_CODE ota_input_pin_mem_free(ota_input_pin_priv_t *p_priv,void *p_buf)
{
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

static void ota_input_pin_on_receive(handle_t _this, media_sample_t *p_sample)
{
  #ifndef OTA_FILTER_USE_FLASH_FILTER
  ipin_t *p_base_pin = (ipin_t *)_this;
  os_msg_t msg = {0};
  msg.content = OTA_INPUT_PIN_EVT_RECEIVE_DATA;
  msg.para1 = (u32)p_sample;
  p_base_pin->send_message_to_filter(p_base_pin, &msg);
  #endif
}

static void ota_ipin_dsmcc_request_ddm_multi(ota_input_pin_priv_t *p_priv,u32 req_para1)
{
   //psi_request_data_t req_data = { 0 };
   u32 para1 = (req_para1 >> 16) & 0xffff ;
   u32 para2 = req_para1 & 0xffff ;
   u8 i = 0;
   
   if(p_priv->dsmcc_req_table_id != 0)
  {
    return;
  }
  memset(&p_priv->req_data,0,sizeof(psi_request_data_t));
  request_dsmcc_db_multi_section_v2(&p_priv->req_data,para1,para2);

  if(p_priv->filter_param.m_flag == 0x01)
  {
    if(p_priv->filter_param.p_filter_data != NULL)
    {
      for(i = 0;i < DMX_SECTION_FILTER_SIZE;i ++)
      { 
            if(p_priv->filter_param.p_filter_data[i] != 0)
              {
                p_priv->req_data.filter_value[i] = p_priv->filter_param.p_filter_data[i];
              }
        }
     }
    if(p_priv->filter_param.p_filter_mask != NULL)
    {
      for(i = 0;i < DMX_SECTION_FILTER_SIZE;i ++)
      {
        if(p_priv->filter_param.p_filter_mask[i] != 0)
          {
            p_priv->req_data.filter_mask[i] = p_priv->filter_param.p_filter_mask[i];
          }
      }
    }
  }

  OS_PRINTF("tanaka integration:%s(%d):filter param\n", __FUNCTION__, __LINE__);
  
  for(i = 0; i < DMX_SECTION_FILTER_SIZE; i++)
  {
    OS_PRINTF("%02x ", p_priv->req_data.filter_value[i]);
  }

  OS_PRINTF("\n");

  for(i = 0; i < DMX_SECTION_FILTER_SIZE; i++)
  {
    OS_PRINTF("%02x ", p_priv->req_data.filter_mask[i]);
  }
  

  OS_PRINTF("\n end \n");
  
  p_priv->p_filter_buffer = (u8 *)ota_input_pin_malloc(p_priv,p_priv->dmx_buf_size + 15);
  if(p_priv->p_filter_buffer == NULL)
  {
    OS_PRINTF("#####debug reqest ddm filter malloc fail!\n");
    return;
  }
  p_priv->req_data.p_ext_data = p_priv->p_filter_buffer;
  p_priv->req_data.ext_data_size   = p_priv->dmx_buf_size;
  p_priv->dsmcc_req_multi_flag = TRUE;
  
  p_priv->p_dmx_inter->i_request(p_priv->p_dmx_inter, &p_priv->req_data);
  p_priv->dsmcc_req_table_id = DVB_TABLE_ID_DSMCC_DDM;
   p_priv->data_pid = (u16)para1;

}

static void ota_ipin_dsmcc_ddm_multi_free(ota_input_pin_priv_t *p_priv,u16 pid)
{
  //psi_free_data_t free_data = {0};
   if(p_priv->dsmcc_req_table_id != DVB_TABLE_ID_DSMCC_DDM)
  {
    return;
  }
  memset(&p_priv->free_data,0,sizeof(psi_free_data_t));
  MT_ASSERT(p_priv != NULL);
  //free dmx resource.
  p_priv->free_data.media_type = MF_DSMCC;
  p_priv->free_data.req_mode   = DMX_DATA_MULTI;
  p_priv->free_data.table_id   = DVB_TABLE_ID_DSMCC_DDM;
  p_priv->free_data.psi_pid    = pid;
  
  p_priv->p_dmx_inter->i_free(p_priv->p_dmx_inter,
                              &p_priv->free_data);
  if(p_priv->p_filter_buffer != NULL)
  {
    ota_input_pin_mem_free(p_priv, (void *)p_priv->p_filter_buffer);
    p_priv->p_filter_buffer =  NULL;
    p_priv->dsmcc_req_multi_flag = FALSE;
  }
  p_priv->dsmcc_req_table_id  = 0;
   p_priv->data_pid = 0;
}

static void ota_ipin_dsmcc_request_dsi_dii_multi(ota_input_pin_priv_t *p_priv, u16 pid,u16 msg_type)
{
  //psi_request_data_t req_data = { 0 };
  u32 para1 = (u32)((pid << 16) | msg_type);/****DSMCC_MSG_DI ***/
  u32 para2 = 0;
  u8  i     = 0;
  
  if(p_priv->dsmcc_req_table_id != 0)
  {
    return;
  }
  memset(&p_priv->req_data,0,sizeof(psi_request_data_t));
  p_priv->req_data.p_ext_data = NULL;
  p_priv->req_data.ext_data_size   = 0;
  OS_PRINTF("#####debug reqest dis single pid= 0x%x\n",para1);
  request_dsmcc_dsi_dii_multi_section_v2(&p_priv->req_data,para1,para2);

  if(p_priv->filter_param.m_flag == 0x01)
  {
    if(p_priv->filter_param.p_filter_data != NULL)
    {
      for(i = 0; i < DMX_SECTION_FILTER_SIZE;i ++)
      { 
            if(p_priv->filter_param.p_filter_data[i] != 0)
              {
                p_priv->req_data.filter_value[i] = p_priv->filter_param.p_filter_data[i];
              }
        }
     }
    if(p_priv->filter_param.p_filter_mask != NULL)
    {
      for(i = 0;i < DMX_SECTION_FILTER_SIZE;i ++)
      {
        if(p_priv->filter_param.p_filter_mask[i] != 0)
          {
            p_priv->req_data.filter_mask[i] = p_priv->filter_param.p_filter_mask[i];
          }
      }
    }
  }

  OS_PRINTF("tanaka integration:%s(%d):filter param\n", __FUNCTION__, __LINE__);
  
  for(i = 0; i < DMX_SECTION_FILTER_SIZE; i++)
  {
    OS_PRINTF("%02x ", p_priv->req_data.filter_value[i]);
  }

  OS_PRINTF("\n");

  for(i = 0; i < DMX_SECTION_FILTER_SIZE; i++)
  {
    OS_PRINTF("%02x ", p_priv->req_data.filter_mask[i]);
  }
  

  OS_PRINTF("\n end \n");

  
  p_priv->req_data.timeout = p_priv->dsmcc_msg_timeout;
  p_priv->p_filter_buffer = (u8 *)ota_input_pin_malloc(p_priv,p_priv->dmx_buf_size + 15);
  if(p_priv->p_filter_buffer == NULL)
  {
    OS_PRINTF("#####debug reqest dis or dii filter malloc fail!\n");
    return;
  }
  p_priv->req_data.p_ext_data = p_priv->p_filter_buffer;
  p_priv->req_data.ext_data_size   = p_priv->dmx_buf_size;
  p_priv->dsmcc_req_multi_flag = TRUE;

  p_priv->p_dmx_inter->i_request(p_priv->p_dmx_inter, &p_priv->req_data);
  p_priv->dsmcc_req_table_id = DVB_TABLE_ID_DSMCC_MSG;
   p_priv->data_pid = pid;
}

static void ota_ipin_dsmcc_dsi_dii_multi_free(ota_input_pin_priv_t *p_priv,u16 pid)
{
  //psi_free_data_t free_data = {0};
  if(p_priv->dsmcc_req_table_id != DVB_TABLE_ID_DSMCC_MSG)
  {
    return;
  }
  memset(&p_priv->free_data,0,sizeof(psi_free_data_t));
  MT_ASSERT(p_priv != NULL);
  //free dmx resource.
  p_priv->free_data.media_type = MF_DSMCC;
  p_priv->free_data.req_mode   = DMX_DATA_MULTI;
  p_priv->free_data.table_id   = DVB_TABLE_ID_DSMCC_MSG;
  p_priv->free_data.psi_pid    = pid;
  
  p_priv->p_dmx_inter->i_free(p_priv->p_dmx_inter,
                              &p_priv->free_data);
  if(p_priv->p_filter_buffer != NULL)
  {
    ota_input_pin_mem_free(p_priv, (void *)p_priv->p_filter_buffer);
    p_priv->p_filter_buffer = NULL;
    p_priv->dsmcc_req_multi_flag = FALSE;
  }
  p_priv->dsmcc_req_table_id  = 0;
   p_priv->data_pid = 0;
}



static void ota_ipin_dsmcc_request_dii_single(ota_input_pin_priv_t *p_priv, u16 pid, u8 group_id)
{
  //psi_request_data_t req_data = { 0 };
  u32 para1 = (u32)((pid << 16) | 0x01); /****DSMCC_MSG_DII ***/
  u32 para2 = (u32)group_id;
  if(p_priv->dsmcc_req_table_id != 0)
  {
    return;
  }
  memset(&p_priv->req_data,0,sizeof(psi_request_data_t));
  p_priv->req_data.p_ext_data = NULL;
  p_priv->req_data.ext_data_size   = 0;
  request_dsmcc_dsi_dii_section_v2(&p_priv->req_data,para1,para2);
  p_priv->req_data.timeout = p_priv->dsmcc_msg_timeout;
  p_priv->p_dmx_inter->i_request(p_priv->p_dmx_inter, &p_priv->req_data);
  p_priv->dsmcc_req_table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_priv->dsmcc_req_multi_flag = FALSE;
   p_priv->data_pid = pid;
}
static void ota_ipin_dsmcc_request_dsi_single(ota_input_pin_priv_t *p_priv, u16 pid)
{
  //psi_request_data_t req_data = { 0 };
  u32 para1 = (u32)((pid << 16) | 0x00);/****DSMCC_MSG_DI ***/
  u32 para2 = 0;
  if(p_priv->dsmcc_req_table_id != 0)
  {
    return;
  }
  memset(&p_priv->req_data,0,sizeof(psi_request_data_t));
  p_priv->req_data.p_ext_data = NULL;
  p_priv->req_data.ext_data_size   = 0;
  OS_PRINTF("#####debug reqest dis single pid= 0x%x\n",para1);
  request_dsmcc_dsi_dii_section_v2(&p_priv->req_data,para1,para2);
  p_priv->req_data.timeout = p_priv->dsmcc_msg_timeout;
  p_priv->p_dmx_inter->i_request(p_priv->p_dmx_inter, &p_priv->req_data);
  p_priv->dsmcc_req_table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_priv->dsmcc_req_multi_flag = FALSE;
  p_priv->data_pid = pid;
}

static void ota_ipin_dsmcc_dii_dsi_single_free(ota_input_pin_priv_t *p_priv,u16 pid)
{
  //psi_free_data_t free_data = {0};
  if(p_priv->dsmcc_req_table_id != DVB_TABLE_ID_DSMCC_MSG)
  {
    return;
  }
  memset(&p_priv->free_data,0,sizeof(psi_free_data_t));
  MT_ASSERT(p_priv != NULL);
  //free dmx resource.
  p_priv->free_data.media_type = MF_DSMCC;
  p_priv->free_data.req_mode   = DMX_DATA_SINGLE;
  p_priv->free_data.table_id   = DVB_TABLE_ID_DSMCC_MSG;
  p_priv->free_data.psi_pid    = pid;
  p_priv->p_dmx_inter->i_free(p_priv->p_dmx_inter,
                              &p_priv->free_data);
  p_priv->dsmcc_req_table_id = 0;
  p_priv->dsmcc_req_multi_flag = FALSE;
  p_priv->data_pid = 0;
}


static void ota_input_pin_config(handle_t _this, ota_ipin_cfg_t *p_cfg)
{
  ota_input_pin_priv_t *p_priv         = ota_input_pin_get_priv(_this);
  p_priv->medium = p_cfg->medium;
  p_priv->protocol_type = p_cfg->protocol_type;
  p_priv->p_mem_heap = p_cfg->p_mem_heap;
  p_priv->use_memp = p_cfg->use_memp;
  p_priv->dmx_buf_size = p_cfg->dmx_buf_size;
  p_priv->dsmcc_msg_timeout = p_cfg->dsmcc_msg_timeout;
  memcpy(&p_priv->filter_param,
                  &p_cfg->filter_param,
                  sizeof(dmx_filter_param_t));
}

static RET_CODE ota_pin_on_open(handle_t _this)
{
  ota_input_pin_priv_t *p_priv         = ota_input_pin_get_priv(_this);
  ipin_t         *p_ipin         = (ipin_t *)_this;
  ipin_t         *p_connect_ipin = (ipin_t *)p_ipin->get_connected(p_ipin);
  
  //only get once.
  if(p_priv->p_dmx_inter == NULL)
  {
    p_connect_ipin->get_interface(p_connect_ipin,
                                  DMX_INTERFACE_NAME,
                                  (void **)&p_priv->p_dmx_inter);
    MT_ASSERT(p_priv->p_dmx_inter != NULL);
  }
  return SUCCESS;
}

static RET_CODE ota_pin_on_start(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE ota_pin_on_stop(handle_t _this)
{
    ota_input_pin_priv_t *p_priv = ota_input_pin_get_priv(_this);
  //free...
  if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
    &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC)
    && (p_priv->data_pid != 0))
    {
       if((p_priv->dsmcc_req_table_id == DVB_TABLE_ID_DSMCC_DDM)
          && (p_priv->dsmcc_req_multi_flag == TRUE))
        {
           ota_ipin_dsmcc_ddm_multi_free(p_priv,p_priv->data_pid);
        }
       
        if(p_priv->dsmcc_req_table_id == DVB_TABLE_ID_DSMCC_MSG)
        {
           if(p_priv->dsmcc_req_multi_flag == TRUE)
            {
              ota_ipin_dsmcc_dsi_dii_multi_free(p_priv,p_priv->data_pid);
            }
           else
            {
              ota_ipin_dsmcc_dii_dsi_single_free(p_priv,p_priv->data_pid);
            }
        }
    }
  return SUCCESS;
}

static RET_CODE ota_pin_on_close(handle_t _this)
{

  return SUCCESS;
}

/*!
   input pin request.
  */ 
 static void _ota_input_pin_reques(handle_t _this,ota_input_reqest_em_t req_para, u32 param)
{
   ota_input_pin_priv_t *p_priv = ota_input_pin_get_priv(_this);
  if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
      &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
  {
     switch(req_para)
      { 
        case REQUES_TABLE_DSMCC_MSG_DSI:
         ota_ipin_dsmcc_request_dsi_single(p_priv,(u16)param);
         break;
         case REQUES_TABLE_DSMCC_MSG_DII:
          ota_ipin_dsmcc_request_dii_single(p_priv,
                            (u16)((param >> 16) & 0xffff),
                            (u8)(param & 0xff));
          break;
         case REQUES_TABLE_DSMCC_DMM:
          p_priv->data_pid = (param >> 16) & 0xffff;
          ota_ipin_dsmcc_request_ddm_multi(p_priv,param);
         break;
         case REQUES_TABLE_DSMCC_MSG_DSI_MULTI:
          ota_ipin_dsmcc_request_dsi_dii_multi(p_priv,(u16)param,DSMCC_MSG_DSI);
          break;
          case REQUES_TABLE_DSMCC_MSG_DII_MULTI:
          ota_ipin_dsmcc_request_dsi_dii_multi(p_priv,(u16)param,DSMCC_MSG_DII);
          break;
         default:
          break;
      }
  }
}
/*!
   input pin request.
  */ 
static void _ota_input_pin_free(handle_t _this,ota_input_free_em_t free_para, u32 param)
{
   ota_input_pin_priv_t *p_priv = ota_input_pin_get_priv(_this);
   
  if((p_priv->medium == OTA_MEDIUM_BY_TUNER)
      &&(p_priv->protocol_type == OTA_STREAM_PROTOCOL_TYPE_DSMCC))
  {
     switch(free_para)
      { 
        case FREE_TABLE_DSMCC_MSG_DSI:
        p_priv->data_pid = (u16)param;
        ota_ipin_dsmcc_dii_dsi_single_free(p_priv,p_priv->data_pid);
        break;
        case FREE_TABLE_DSMCC_MSG_DII:
         p_priv->data_pid = (u16)(param >> 16);
        ota_ipin_dsmcc_dii_dsi_single_free(p_priv,p_priv->data_pid);
        break;
        case FREE_TABLE_DSMCC_DMM:
        p_priv->data_pid = (u16)param;
        ota_ipin_dsmcc_ddm_multi_free(p_priv,p_priv->data_pid);
        break;
        case FREE_TABLE_DSMCC_MSG_DSI_MULTI:
        case FREE_TABLE_DSMCC_MSG_DII_MULTI:
        p_priv->data_pid = (u16)param;
        ota_ipin_dsmcc_dsi_dii_multi_free(p_priv,p_priv->data_pid);
        break;
        default:
        break;
      }
  }
}
ota_input_pin_t * ota_input_pin_create(ota_input_pin_t *p_pin, interface_t *p_owner)
{
  ota_input_pin_priv_t   *p_priv  = &p_pin->m_priv_data;
  ipin_t           *p_ipin  = NULL;
  base_input_pin_t *p_input = NULL;
  #ifdef OTA_FILTER_USE_FLASH_FILTER
  transf_input_pin_para_t transf_pin_para;
  #else
  sink_pin_para_t sink_pin_para = {0};
  #endif
  media_format_t    format  = { MT_PSI,};

  //check input parameter
  MT_ASSERT(p_priv  != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
#ifdef OTA_FILTER_USE_FLASH_FILTER
  memset(&transf_pin_para,0,sizeof(transf_input_pin_para_t));
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "ota_in_pin";
  transf_input_pin_create(&p_pin->m_sink_pin, &transf_pin_para);
#else
  //create base class
  memset(&sink_pin_para,0,sizeof(sink_pin_para_t));
  sink_pin_para.p_filter = p_owner;
  sink_pin_para.p_name = "ota_in_pin";
  sink_pin_create(&p_pin->m_sink_pin, &sink_pin_para);
#endif

  //init private date
 memset(p_priv, 0, sizeof(ota_input_pin_priv_t));
 p_priv->p_this = (u8 *)p_pin; //this point
 p_pin->config = ota_input_pin_config;
 p_pin->ota_input_pin_reques = _ota_input_pin_reques;
 p_pin->ota_input_pin_free = _ota_input_pin_free;

  //overload class base_input_pin_t virtual function
  p_input = (base_input_pin_t *)p_pin;
  MT_ASSERT(p_input != NULL);
  p_input->on_receive = ota_input_pin_on_receive;

  //overload class ipin_t virtual function
  p_ipin  = (ipin_t *)p_pin;
  MT_ASSERT(p_ipin  != NULL);
  p_ipin->add_supported_media_format(p_ipin, &format);
  p_ipin->on_open  = ota_pin_on_open;
  p_ipin->on_start = ota_pin_on_start;
  p_ipin->on_stop  = ota_pin_on_stop;
  p_ipin->on_close = ota_pin_on_close;

  return p_pin;
}

