/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/

/******************************************************************************/
#include "sys_define.h"
#include "sys_types.h"
#include "sys_cfg.h"

#include "class_factory.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "string.h"

#include "drv_dev.h"
#include "data_manager.h"
#include "ap_framework.h"
#include "ap_cst_m.h"
#include "ap_uio.h"

#include "lib_memf.h"
#include "lib_memp.h"
#include "lib_rect.h"
//#include "surface.h"
//#include "gdi.h"
//#include "ctrl_base.h"
//#include "framework.h"
#if WIN32
#include "uart_win32.h"
#endif

extern void rtsp_server_reset(void);
/*!
  init upgrade basic information
  */
static void ap_cst_m_init()
{

}

/*!
  find hardware code for the exit/power key
  */
static RET_CODE ap_cst_m_save_ch_list(pg_list_t *p_pg_list)
{    
  return SUCCESS;
}

static void ap_cst_m_restart_rtsp(void)
{
  rtsp_server_reset();
}
/*!
  construct the usb upg policy
  */
cst_m_policy_t *construct_cst_m_policy(void)
{
  cst_m_policy_t *p_rc_http_policy = mtos_malloc(sizeof(cst_m_policy_t));
  MT_ASSERT(p_rc_http_policy != NULL);
  memset(p_rc_http_policy, 0, sizeof(cst_m_policy_t));

  p_rc_http_policy->on_init = ap_cst_m_init;
  p_rc_http_policy->save_ch_list = ap_cst_m_save_ch_list;
  p_rc_http_policy->restart_rtsp = ap_cst_m_restart_rtsp;
  return p_rc_http_policy;
}


