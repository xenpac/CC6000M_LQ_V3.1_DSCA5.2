/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"

#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "service.h"
#include "mdl.h"
#include "class_factory.h"
#include "dvb_svc.h"

#include "ap_framework.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "dvbs_util.h"

#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"
#include "nit.h"
#include "cat.h"
#include "ap_playback.h"

#include "ui_common.h" // This file includes lots of heads.

#include "ap_nit.h"

/******************************************************************************/

/******************************************************************************/

static u8 startup_nit_version = 0xff;

static u16 startup_nit_network_id = 0xffff;
/*!
  Nit private data
  */
typedef struct
{
  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;
}nit_data_t;

/*!
  nit private information
  */
typedef struct
{
  /*!
    nvod private data
    */
  nit_data_t *p_priv_data;

  /*!
    Application nit information
    */
  app_t nit_app;
  
}nit_priv_t;

/******************************************************************************/

u8 get_init_nit_ver(void)
{
    return startup_nit_version;
}

void set_init_nit_ver(u8 nit_version)
{
    startup_nit_version = nit_version;
}

u16 get_init_nit_network_id(void)
{
    return startup_nit_network_id;
}

void set_init_nit_network_id(u16 network_id)
{
    startup_nit_network_id = network_id;
}

static void init(handle_t p_handle)
{
  nit_priv_t *p_priv = (nit_priv_t *)p_handle;

  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
    
  p_priv->p_priv_data->p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_NIT);
  
}

static void nit_single_step(void *p_handle, os_msg_t *p_cmd)
{
#if defined DESAI_56_CA || defined DESAI_52_CA || defined DESAI_40_CA
  u32 ret = 0;
#endif
  cmd_t cmd = {0};
  
  if(p_cmd != NULL)
  {
    switch(p_cmd->content)
    {
      case DVB_NIT_FOUND:
      {
        #if defined DESAI_56_CA || defined DESAI_52_CA || defined DESAI_40_CA
        cas_table_process(CAS_TID_NIT, (((nit_t *)p_cmd->para1)->p_origion_data),  &ret);
        #endif
        #if 0 //def PRINT_ON
          int i;
        u8 *p_buf = ((nit_t *)p_cmd->para1)->p_origion_data;
        for(i=0;i<32;i++)
          {
               OS_PRINTF("$###@@@@@@!!!!!!!!!0x%x\n",p_buf[i]);
          }
        #endif

        startup_nit_version = ((nit_t *)p_cmd->para1)->version_num;
        startup_nit_network_id = ((nit_t *)p_cmd->para1)->network_id;
        OS_PRINTF("@@@startup_nit_version = %d \n", startup_nit_version);
        //deactive ap_nit
        cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
        cmd.data1 = APP_NIT;
        ap_frm_do_command(APP_FRAMEWORK, &cmd);
      }
      break;
      default:
      break;
    }
  }
}

void enable_ap_nit(void)
{
  cmd_t cmd = {0};
  dvb_request_t dvb_req = {0};
  
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_tmp_svc = p_dvb->get_svc_instance(p_dvb, APP_NIT);

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_NIT;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  dvb_req.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.para1 = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para2 = 0xFFFF;
  p_tmp_svc->do_cmd(p_tmp_svc, DVB_REQUEST,
   (u32)&dvb_req, sizeof(dvb_request_t));
  OS_PRINTF("@@ enable_ap_nit \n");
}

app_t *construct_ap_nit(void)
{
  nit_priv_t *p_priv = mtos_malloc(sizeof(nit_priv_t));
  
  MT_ASSERT(p_priv != NULL);

  //Malloc private data for scan
  p_priv->p_priv_data = mtos_malloc(sizeof(nit_data_t));
  MT_ASSERT(p_priv->p_priv_data != NULL);
  memset(p_priv->p_priv_data, 0, sizeof(nit_data_t));
  memset(&(p_priv->nit_app), 0, sizeof(app_t));
  

  p_priv->nit_app.init = init;
  p_priv->nit_app.task_single_step = nit_single_step;
  p_priv->nit_app.p_data = (void *)p_priv;

  return &p_priv->nit_app;
}

