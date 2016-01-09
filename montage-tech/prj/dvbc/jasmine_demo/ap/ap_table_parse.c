/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "sys_regs_jazz.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lib_util.h"
#include "lib_rect.h"
#include "hal_gpio.h"
#include "hal_misc.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "common.h"
#include "drv_dev.h"
#include "nim.h"
#include "uio.h"
#include "hal_watchdog.h"
#include "hal_misc.h"
#include "display.h"

#include "vdec.h"
#include "aud_vsb.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "nit.h"
#include "cat.h"
#include "data_manager.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "db_dvbs.h"
#include "mt_time.h"

#include "ss_ctrl.h"
#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_signal_monitor.h"
//#include "ap_epg.h"
//#include "ap_satcodx.h"
//#include "ap_ota.h"
#include "sys_status.h"
#include "hal_base.h"
#include "smc_op.h"
#include "lpower.h"
//#include "customer_def.h"
//#include "config_customer.h"
//#include "config_prj.h"
#include "db_dvbs.h"
#include "ui_dbase_api.h"
#include "ap_table_parse.h"

typedef enum
{
  INIT_IDLE,
  INIT_NIT_REQUEST_TABLE,
  INIT_REQUEST_TABLE,
  INIT_WAIT_TABLE,
  INIT_DONE_CHANNEL_PARSE,
  INIT_FINISH

}table_parse_state_t;



typedef struct
{
  service_t *p_dvb_svc;
  /*!
  NC service handle
  */
  service_t *p_nc_svc;
  table_parse_state_t status;
  u16 pid;
  u16 table_id;
  u32 msg_id;
  u8 nit_finish;
  u8 channel_finish;
  u8 cur_sec_num;
  u8 pending_job;
  u32 version;
  BOOL table_found_bit;
  channel_table_parse_policy_t *channel_parse;
}table_parse_priv_t;

static table_parse_priv_t g_priv = {0};

static u8 startup_nit_version = 0xff;
static BOOL startup_nit_channel_status = FALSE;
u8 get_channel_init_nit_ver(void)
{
    return startup_nit_version;
}

void set_channel_init_nit_ver(u8 nit_version)
{
    startup_nit_version = nit_version;
}

BOOL get_channel_init_channel_status(void)
{
    return startup_nit_channel_status;
}

static void nit_request_start(void)
{
  dvb_request_t dvb_req = {0};
  dvb_req.req_mode  = DATA_SINGLE;
  dvb_req.table_id  = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para1     = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para2     = 0xFFFF;

  g_priv.p_dvb_svc->do_cmd(g_priv.p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));

}

static void channel_request(void)
{
    dvb_request_t param = {0};
    param.req_mode  = DATA_SINGLE;
    param.table_id  = DVB_TABLE_ID_RUN_STATUS; //just for dvb register
    param.para1     = g_priv.pid; // pid
    param.para2     = g_priv.table_id << 16 | g_priv.cur_sec_num; //table_id
    g_priv.p_dvb_svc->do_cmd(g_priv.p_dvb_svc, DVB_REQUEST,(u32)&param, sizeof(dvb_request_t));
}
BOOL ap_table_parse_setup(os_msg_t *p_msg)
{
  if(p_msg != NULL)
  {
    switch(p_msg->content & (~SYS_EVT_TO_UI_MSG_MASK))
    {
    case NC_EVT_LOCKED:
      g_priv.status = INIT_NIT_REQUEST_TABLE; // to request
      break;
    case NC_EVT_UNLOCKED:
      OS_PRINTF("unlock  in channel init\n");
      g_priv.nit_finish = 1;
      g_priv.channel_finish = 1;
      g_priv.status = INIT_FINISH;
      break;
    case DVB_NIT_FOUND:
      {
         nit_t *p_nit = NULL;
         p_nit = (nit_t *)p_msg->para1;
         startup_nit_version = p_nit->version_num;
         g_priv.nit_finish = 1;
         if(g_priv.channel_parse->channel_bit ==0)
          {
            g_priv.status = INIT_FINISH;
          }
          else if((g_priv.nit_finish ==1) && (g_priv.channel_finish ==1))
            {
               g_priv.status = INIT_FINISH;
            }
          else
            {
              /***no change to status***/
            }
          
      }
     break;
    case DVB_DTM_FOUND:
      // process nit
      {
        table_section_t *p_section = NULL;
        u32 channel_version = 0;
        p_section = (table_section_t *)(p_msg->para1);

        g_priv.table_found_bit = TRUE;
        g_priv.version = p_section->version;
		#ifdef CHANNEL_FOLLOWING_TABLE
        channel_version =  (u32)sys_status_get_channel_version();
		#endif
        if(g_priv.version == channel_version)
        {
          g_priv.status = INIT_DONE_CHANNEL_PARSE;
          g_priv.channel_finish = 1;
          break;
        }
    //    
        if(p_section->section_num != p_section->last_section)
        {
          g_priv.status = INIT_REQUEST_TABLE;//request again
          g_priv.cur_sec_num = (u8)p_section->section_num + 1;
        }
        else
        {
          g_priv.status = INIT_DONE_CHANNEL_PARSE;
          g_priv.channel_finish = 1;
        }
      }
      break;
    case DVB_TABLE_TIMED_OUT:
      OS_PRINTF("table 0x%x timeout\n",p_msg->para1);
       if(p_msg->para1 == DVB_TABLE_ID_NIT_ACTUAL)
        {
           g_priv.nit_finish = 1;
        }
        if(p_msg->para1 == DVB_TABLE_ID_RUN_STATUS)
        {
           g_priv.channel_finish = 1;
           
        }
       
        if(g_priv.channel_parse->channel_bit ==0)
        {
          g_priv.status = INIT_FINISH;
        }
       else
        {
           if((g_priv.nit_finish ==1) && (g_priv.channel_finish ==1))
            {
              g_priv.status = INIT_FINISH;
            }
        } 
      break;
    }
  }
  return FALSE;
}

BOOL ap_table_parse_process(void)
{
  switch(g_priv.status)
  {
    case INIT_IDLE: //idle
      break;
    case INIT_NIT_REQUEST_TABLE:
      nit_request_start();
      if(g_priv.channel_parse->channel_bit !=0)
        {
          channel_request();
        }
      g_priv.status = INIT_WAIT_TABLE;
      break;
    case INIT_REQUEST_TABLE:// request
     channel_request();
      g_priv.status = INIT_WAIT_TABLE;
      break;
    case INIT_WAIT_TABLE: //running
      break;
    case INIT_DONE_CHANNEL_PARSE: //done
      //release resource
      {
        u8 vid = 0;
        u32 channel_version = 0;
		#ifdef CHANNEL_FOLLOWING_TABLE
        channel_version =  (u32)sys_status_get_channel_version();
        #endif
        if(g_priv.version != channel_version)
        {
          if(g_priv.channel_parse->p_channel_process != NULL)
            {
                g_priv.channel_parse->p_channel_process();
            }
        }

        vid = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
        if((g_priv.table_found_bit == TRUE) && (db_dvbs_get_count(vid) > 0))
          {
            startup_nit_channel_status = TRUE;
          }

       if((g_priv.nit_finish ==1) && (g_priv.channel_finish ==1))
        {
            g_priv.status = INIT_FINISH;
        }
       else
        {
            g_priv.status = INIT_WAIT_TABLE;
        }
        
      }
      break;
    case INIT_FINISH:
      {
          dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
          p_dvb->remove_svc_instance(p_dvb, g_priv.p_dvb_svc);
          nc_remove_svc_instance(class_get_handle_by_id(NC_CLASS_ID), g_priv.p_nc_svc);
      }

      return TRUE;
      break;
    default:
      break;
  }
  return FALSE;
}


 void ap_table_parse_task_start(u32 freq, u32 sym, u16  modulation)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  nc_channel_info_t search_info = {0};
  os_msg_t  msg = {0};
  u32 ret = 0;
  dvbc_lock_t main_tp = {0};
  sys_status_get_main_tp1(&main_tp);
  ap_frm_run();
  g_priv.channel_parse = construct_channel_table_parse_policy();
  
  g_priv.p_nc_svc = nc_get_svc_instance(nc_handle, APP_FRAMEWORK);
  g_priv.p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_FRAMEWORK);
  g_priv.cur_sec_num = 0;
  g_priv.pid = g_priv.channel_parse->pid;
  g_priv.table_id = g_priv.channel_parse->table_id;
  
  search_info.polarization = 0;
  search_info.channel_info.frequency = main_tp.tp_freq;
  search_info.channel_info.param.dvbc.symbol_rate = main_tp.tp_sym;
  search_info.channel_info.param.dvbc.modulation = main_tp.nim_modulate;
  
  g_priv.msg_id = mtos_messageq_create(40, (u8 *)"chan init");
  if(g_priv.channel_parse->p_channel_init != NULL)
  {
      g_priv.channel_parse->p_channel_init();
  }
  ap_frm_set_ui_msgq_id(g_priv.msg_id);
  nc_set_tp(nc_handle,0, g_priv.p_nc_svc, &search_info);

  while(1)
  {
    ret = mtos_messageq_receive(g_priv.msg_id, &msg, 10);
    if(ret)
    {
      ap_table_parse_setup(&msg);
    }

    if(ap_table_parse_process())
    {
        break;
    }
  }

 if(g_priv.channel_parse->p_channel_release!= NULL)
  {
      g_priv.channel_parse->p_channel_release();
  }
  
  if(g_priv.channel_parse != NULL)
    {
        destruct_channel_table_parse_policy(g_priv.channel_parse);
    }
}


