/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "mem_manager.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "hal_misc.h"
#include "drv_dev.h"
#include "charsto.h"
#include "drv_dev.h"
#include "pti.h"
#include "nim.h"
#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"

#include "ss_ctrl.h"
#include "class_factory.h"
#include "mdl.h"
#include "err_check_def.h"
#include "service.h"
#include "dvb_svc.h"
#include "data_manager.h"
#include "dvb_protocol.h"

#include "class_factory.h"
#include "fcrc.h"
#include "data_manager.h"
#include "ap_framework.h"
#include "mosaic.h"
#include "sdt.h"
#include "pmt.h"
#include "ap_ts_ota.h"
#include "dsmcc.h"

#ifdef PRINT_ON
//#define OTA_DEBUG
#endif

#ifdef OTA_DEBUG
#define OTA_DBG OS_PRINTF
#else
#define OTA_DBG(x) do {} while(0)
#endif

#define TS_OTA_TS_PACKET_SIZE (2*128*188)

/*!
   ts ota private data
  */
typedef struct
{
  /*!
     ota state machine state
    */
  ts_ota_sm_t sm;
  /*!
    otai info.
    */
  ota_info_t otai; 
  /*!
    download info.
    */
  ts_download_info_t download_info;  
  /*!
     pointer to ota message queue mem start addr
    */
  u8 *p_ota_msgq_mem;
  /*!
     Message list information
    */
  u32 msg_list[MAX_TS_OTA_MSG_NUM];
  /*!
     Message number existing in message list
    */
  u8 msg_num;
  /*!
    the right service id
    */
  u16 svc_id;
  /*!
    nim device
    */
  nim_device_t *p_nim;  
  /*!
    DVB service handle
    */
  service_t *p_svc;

  pti_ext_buf_t ota_ext_buf;
} ts_ota_priv_data_t;

/*!
   dvbs ota handle
  */
typedef struct
{
  /*!
     ota instance
    */
  app_t instance;
  /*!
     policy
    */
  ts_ota_policy_t *p_policy;
  /*!
     private data
    */
  ts_ota_priv_data_t data;
} ts_ota_handle_t;


/*!
   send ota's evt out.

   \param[in] id: sent event id
  */
static void send_msg_to_ui(u32 id, u32 para1, u32 para2)
{
  event_t evt = {0};

  evt.id = id;
  evt.data1 = para1;
  evt.data2 = para2;
  ap_frm_send_evt_to_ui(APP_TS_OTA, &evt);
}

static void pat_request(handle_t handle)
{
  ts_ota_priv_data_t *p_this = (ts_ota_priv_data_t *)handle;
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_PAT;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = DVB_TABLE_ID_PAT;
  dvb_req.para2 = 0;
  dvb_req.context = 0;
  p_this->p_svc->do_cmd(p_this->p_svc, DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));

  OTA_DBG(("ts ota: pat request\n"));
}

static void sdt_request(handle_t handle, u32 sec_index)
{
  ts_ota_priv_data_t *p_this = (ts_ota_priv_data_t *)handle;  
  dvb_request_t dvb_req = {0};
  
  dvb_req.req_mode  = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.table_id  = DVB_TABLE_ID_SDT_ACTUAL;
  dvb_req.para1 = DVB_TABLE_ID_SDT_ACTUAL;
  dvb_req.para2 = sec_index;
  dvb_req.context = 0;
  
  p_this->p_svc->do_cmd(p_this->p_svc, DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));
  OTA_DBG(("ts ota: sdt request\n"));
}

static void pmt_request(handle_t handle, u32 param)
{
  ts_ota_priv_data_t *p_this = (ts_ota_priv_data_t *)handle;  
  dvb_request_t dvb_req = {0};
  
  dvb_req.req_mode  = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.table_id  = DVB_TABLE_ID_PMT;
  dvb_req.para1 = DVB_TABLE_ID_PMT;
  dvb_req.para2 = param;
  dvb_req.context = param;  
  
  p_this->p_svc->do_cmd(p_this->p_svc, DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));
  OTA_DBG(("ts ota: pmt request\n"));
}

static void request_ts_ota_packet(ts_ota_priv_data_t *p_priv, u8 *p_buffer)
{
  dvb_request_t dvb_req = {0};

  memset(&(p_priv->ota_ext_buf), 0x0, sizeof(pti_ext_buf_t));
  p_priv->ota_ext_buf.p_buf = p_buffer;
  p_priv->ota_ext_buf.size = TS_OTA_TS_PACKET_SIZE;

  dvb_req.table_id = DVB_TABLE_ID_OTA_TS;
  dvb_req.req_mode = DATA_MULTI;
  dvb_req.para1 = (u32)&p_priv->ota_ext_buf;
  dvb_req.para2 = p_priv->otai.download_data_pid;
  
  p_priv->p_svc->do_cmd(p_priv->p_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));  
}

void do_request_ota_packet(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  dvb_t *p_dvb = NULL;
  p_sec->pid      = (u16)para2;
  p_sec->table_id = DVB_TABLE_ID_OTA_TS;
  p_sec->timeout  = 10000;
  
  p_sec->filter_mode = PTI_TYPE_TSPKT; 
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_PTI_FLTBYTESUM);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_PTI_FLTBYTESUM);
  p_sec->filter_mask_size = 1;
  
  //Disable CRC check
  p_sec->p_ext_buf_list = (filter_ext_buf_t *)para1;

  p_sec->crc_enable = 0;
  p_sec->direct_data = TRUE;
  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  p_dvb->alloc_section(p_dvb, p_sec);
}

static void on_ts_ota_sdt_found(ts_ota_handle_t *p_handle, os_msg_t *p_msg)
{
  ts_ota_priv_data_t *p_data = &(p_handle->data);
  sdt_t sdt;
  u16 i = 0;
  u8 temp[MAX_SVC_NAME_LEN * 2] = "Kathrein Download";

  memcpy(&sdt, (void *)p_msg->para1, sizeof(sdt_t));

  OS_PRINTF("sdt found, cnt %d, section number %d, last_number %d\n", 
    sdt.svc_count, sdt.sec_number, sdt.last_sec_number);

  for(i = 0; i < sdt.svc_count; i++)
  {
    OS_PRINTF("sdt found, name %s\n", sdt.svc_des[i].name);
    if(memcmp(sdt.svc_des[i].name, temp, MAX_SVC_NAME_LEN - 1) == 0)
    {
      p_data->svc_id = sdt.svc_des[i].svc_id;

      break;
    }
  }

  if(p_data->svc_id != TS_OTA_INVALID_SID)
  {
    send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"service name matched, start request pat...", 0);
    pat_request(p_data);
  }
  else
  {
    if(sdt.sec_number < sdt.last_sec_number)
    {
      sdt_request(p_data, (sdt.sec_number + 1));
      send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"service name don't match, sec_index++", 0);
    }
    else
    {
      send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"service name don't match, ota stopped", 0);
    }
  }      

  return;
}

static void on_ts_ota_sdt_time_out(ts_ota_handle_t *p_handle, os_msg_t *p_msg)
{
  ts_ota_priv_data_t *p_data = &(p_handle->data);
  sdt_t sdt;

  memcpy(&sdt, (void *)p_msg->para1, sizeof(sdt_t));

  sdt_request(p_data, sdt.sec_number);
  
  return;
}

static void on_ts_ota_pmt_info_found(ts_ota_handle_t *p_handle, os_msg_t *p_msg)
{
  ts_ota_priv_data_t *p_priv = &(p_handle->data);
  u32 param = ((p_msg->para1 << 16) | p_msg->para2);
      
  if(p_priv->svc_id == p_msg->para2)
  {
      send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"request pmt...", 0);
      pmt_request(p_priv, param);
  }
}

static void on_ts_ota_pmt_found(ts_ota_handle_t *p_handle, os_msg_t *p_msg)
{
  ts_ota_priv_data_t *p_priv = &(p_handle->data);  
  ts_ota_policy_t *p_policy = p_handle->p_policy;  
  pmt_t pmt;

  memcpy(&pmt, (void *)p_msg->para1, sizeof(pmt));

  p_priv->otai.download_data_pid = pmt.es_id;

  p_policy->write_otai(&(p_priv->otai));
  return;
}

/*!
   process the message methdd, it will be called by ota task.

   \param[in] handle ota handle
   \param[in] p_msg new message
  */
static void ts_ota_single_step(class_handle_t p_handler, os_msg_t *p_msg)
{
  ts_ota_handle_t *p_handle = (ts_ota_handle_t *)p_handler;
  ts_ota_policy_t *p_policy = p_handle->p_policy;
  ts_ota_priv_data_t *p_priv = &p_handle->data;
  dvb_section_t *p_sec = NULL;
  u16 data_len = 0;
  void *p_buffer = NULL;
  
  if(p_msg != NULL)
  {
    switch(p_msg->content)
    {
      case TS_OTA_CMD_RELOCK:
        memcpy((void *)(&p_priv->download_info), 
          (void *)p_msg->para1, sizeof(ts_download_info_t));

        p_priv->otai.ota_tri = OTA_TRI_FORC;
        p_priv->sm = TS_OTA_SM_LOCKING;        
        break;
      case TS_OTA_CMD_START:
        p_policy->read_otai(&p_priv->otai);
        if(p_priv->otai.ota_tri == OTA_TRI_AUTO)
        {
          //auto ota.
          p_priv->download_info.polar = p_priv->otai.locks.tp_rcv.polarity;

          p_priv->download_info.channel_info.frequency = 
            dvbs_calc_mid_freq(&(p_priv->otai.locks.tp_rcv), &(p_priv->otai.locks.sat_rcv));

          p_priv->download_info.channel_info.param.dvbs.symbol_rate = 
            p_priv->otai.locks.tp_rcv.sym;
          p_priv->download_info.channel_info.param.dvbs.fec_inner = NIM_CR_AUTO;
          p_priv->download_info.pid = p_priv->otai.download_data_pid;
          
          OTA_DBG(("Auto mode: f[%d], s[%d], data_pid[%d]\n", 
            p_priv->download_info.channel_info.frequency,
            p_priv->download_info.channel_info.param.dvbs.symbol_rate,
            p_priv->download_info.pid));
          p_priv->sm = TS_OTA_SM_LOCKING;
        }
        else if(p_priv->otai.ota_tri == OTA_TRI_FORC)
        {
          OTA_DBG(("force ota, to open user input menu.\n"));            
          send_msg_to_ui(TS_OTA_EVT_UNLOCKED, 0, 0);
          p_priv->sm = TS_OTA_SM_IDLE;    
        }      
        break;

      case TS_OTA_CMD_STOP:
        OTA_DBG(("stop OTA and restart\n"));
        p_priv->otai.ota_tri = OTA_TRI_NONE;
        p_policy->write_otai(&p_priv->otai);
        
        #ifndef WIN32
        mtos_task_delay_ms(1000); 
        //hal_pm_reset();
        hal_watchdog_init(0);
  		hal_watchdog_enable();
        #endif
        
        p_priv->sm = TS_OTA_SM_IDLE;
        break;
        
      case TS_OTA_CMD_START_SEARCH:
        send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"start request sdt...", 0);

        memcpy(&p_priv->otai, (ota_info_t *)p_msg->para1,
               sizeof(ota_info_t));

        p_priv->svc_id = TS_OTA_INVALID_SID;
        
        sdt_request(p_priv, 0);
 
        p_priv->sm = TS_OTA_SM_IDLE;        
        break;
        
      case TS_OTA_CMD_STOP_SEARCH:        
        if(p_msg->para1)
        {
          p_priv->otai.ota_tri = OTA_TRI_AUTO;
          p_policy->write_otai(&(p_priv->otai));
          send_msg_to_ui(TS_OTA_EVT_STOPPED, 0, 0);
          send_msg_to_ui(TS_OTA_EVT_RESET, 0, 0);
        }
        else
        {
          send_msg_to_ui(TS_OTA_EVT_STOPPED, 0, 0);
        }
        p_priv->sm = TS_OTA_SM_IDLE;
        break;
        
      case DVB_PAT_FOUND:
        OTA_DBG(("ts ota: pat found\n"));
        break;
        
      case DVB_SDT_FOUND:
        send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"sdt found", 0);
        on_ts_ota_sdt_found(p_handle, p_msg);
        break;
      case DVB_PAT_PMT_INFO:
        OTA_DBG(("ts ota: pat pmt info\n"));
        on_ts_ota_pmt_info_found(p_handle, p_msg);
        break;
      case DVB_PMT_FOUND:
        send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"pmt found", 0);
        on_ts_ota_pmt_found(p_handle, p_msg);

        if(p_priv->otai.ota_tri == OTA_TRI_FORC)
        {
          send_msg_to_ui(TS_OTA_EVT_STATUS, (u32)"request ts paket...", 0);
          p_buffer = mtos_malloc(TS_OTA_TS_PACKET_SIZE);
          CHECK_FAIL_RET_VOID(p_buffer != NULL);
          
          request_ts_ota_packet(p_priv, p_buffer);
          p_priv->sm = TS_OTA_SM_DOWLOAD;
        }
        else
        {
          send_msg_to_ui(TS_OTA_EVT_FOUND, 0, 0);
          p_priv->sm = TS_OTA_SM_IDLE;
        }
        break;

      case DVB_ONE_SEC_FOUND:
        OTA_DBG(("DVB_ONE_SEC_FOUND\n"));

        data_len = (u16)(p_msg->para2 & 0xFFFF);
        p_sec = (dvb_section_t *)p_msg->para1;
        
        if(DVB_TABLE_ID_OTA_TS == p_sec->table_id)
        {
          //notify ts for the data.
          //ts_ota_Section_Notify(p_sec->p_buffer);
        }        
        break;
        
      case DVB_TABLE_TIMED_OUT:
        switch(p_msg->para1)
        {
          case DVB_TABLE_ID_PAT:
            OTA_DBG(("ts ota: pat timeout\n"));
            pat_request(p_priv);
            break;
          case DVB_TABLE_ID_SDT_ACTUAL:
            OTA_DBG(("ts ota: sdt timeout, retry\n"));
            on_ts_ota_sdt_time_out(p_handle, p_msg);
            break;
          case DVB_TABLE_ID_PMT:
            OTA_DBG(("ts ota: pmt timeout\n"));
            pmt_request(p_priv, p_msg->context);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  else
  {
    switch(p_priv->sm)
    {
      case TS_OTA_SM_IDLE:
        //mtos_task_sleep(100);
        break;
      case TS_OTA_SM_LOCKING:
        dev_io_ctrl(p_priv->p_nim, NIM_IOCTRL_SET_PORLAR, p_priv->download_info.polar);
        nim_channel_connect(p_priv->p_nim, &(p_priv->download_info.channel_info), FALSE);
        
        if(p_priv->download_info.channel_info.lock)
        {
          send_msg_to_ui(TS_OTA_EVT_LOCKED, 0, 0);

          if(p_priv->otai.ota_tri == OTA_TRI_AUTO)
          {
            p_priv->sm = TS_OTA_SM_DOWLOAD;
            p_buffer = mtos_malloc(TS_OTA_TS_PACKET_SIZE);
            CHECK_FAIL_RET_VOID(p_buffer != NULL);
            
            request_ts_ota_packet(p_priv, p_buffer);
            
          }
          else
          {
            p_priv->sm = TS_OTA_SM_SEARCHING;
          }
        }
        else
        {
          send_msg_to_ui(TS_OTA_EVT_UNLOCKED, 0, 0);
          p_priv->sm = TS_OTA_SM_IDLE;
        }        
        break;        
      case TS_OTA_SM_SEARCHING:
        p_priv->svc_id = TS_OTA_INVALID_SID;
        
        sdt_request(p_priv, 0);
 
        p_priv->sm = TS_OTA_SM_IDLE;    
        break;
      case TS_OTA_SM_DOWLOAD:
        break;
        
      default:
        break;
    }
  }
}

/*!
   Initialization function.

   \param[in] handle ota handle
  */
static void init(class_handle_t p_handler)
{
  ts_ota_handle_t *p_handle = (ts_ota_handle_t *)p_handler;
  ts_ota_priv_data_t *p_priv = &(p_handle->data);
  ts_ota_policy_t *p_policy = p_handle->p_policy;
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  u32 msg_id_list[] =
  {
    TS_OTA_CMD_START_SEARCH,
    TS_OTA_CMD_STOP_SEARCH,
    TS_OTA_CMD_START,
    TS_OTA_CMD_STOP,
    DVB_TABLE_TIMED_OUT,
    DVB_PAT_FOUND,
    DVB_SDT_FOUND,
    DVB_PMT_FOUND,
    DVB_PAT_PMT_INFO,
  };
  u8 msg_num = sizeof(msg_id_list) / sizeof(u32);

  memcpy(p_handle->data.msg_list, msg_id_list, sizeof(msg_id_list));

  p_policy->ota_crc_init();
  
  p_priv->p_nim = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);  
  p_priv->msg_num = msg_num;
  p_priv->svc_id = TS_OTA_INVALID_SID;
  p_priv->p_svc = p_dvb->get_svc_instance(p_dvb, APP_TS_OTA);
}


/*!
   Call this method, load ota instance by the policy.

   \param[in] p_policy The policy of application ota
   \return Return app_t instance address
  */
app_t *construct_ap_ts_ota(ts_ota_policy_t *p_policy)
{
  ts_ota_handle_t *p_handle = mtos_malloc(sizeof(ts_ota_handle_t));

  CHECK_FAIL_RET_NULL((p_handle != NULL) && (p_policy != NULL));
  p_handle->p_policy = p_policy;
  memset(&(p_handle->data), 0, sizeof(ts_ota_priv_data_t));
  
  memset(&(p_handle->instance), 0, sizeof(app_t));
  p_handle->instance.init = init;
  p_handle->instance.task_single_step = ts_ota_single_step;
  p_handle->instance.get_msgq_timeout = NULL;
  p_handle->instance.p_data = (void *)p_handle;

  return &(p_handle->instance);
}

