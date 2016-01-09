/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "class_factory.h"

#include "log.h"

#include "log_scan_imp.h"

#include "log_mgr.h"

static void do_log(handle_t handle, log_module_id_t mid, const char *p_file_name, 
  u32 line, u32 event,  u32 p1, u32 p2)
{
  log_mgr_t *p_mgr = handle;
  logger_mgr_t *p_priv = p_mgr->p_data;
  
  if(p_priv->p_logger[mid] != NULL)
  {
    p_priv->p_logger[mid]->log(p_priv->p_logger[mid], p_file_name, line, event, p1, p2);
  }
}

static void do_send_key(handle_t handle, log_module_id_t mid)
{
  log_mgr_t *p_mgr = handle;
  logger_mgr_t *p_priv = p_mgr->p_data;

  if(p_priv->p_logger[mid] != NULL)
  {
   p_priv->p_logger[mid]->send_one_key(p_priv->p_logger[mid]);
  }
}

void attach(handle_t handle, log_module_id_t mid, log_t *p_logger)
{
  log_mgr_t *p_mgr = handle;
  logger_mgr_t *p_priv = p_mgr->p_data;
  p_priv->p_logger[mid] = p_logger;
}

log_mgr_t * log_mgr_init(void)
{
  // initialize your data here
  log_mgr_t *p_mgr = mtos_malloc(sizeof(log_mgr_t));
  logger_mgr_t *p_priv = mtos_malloc(sizeof(logger_mgr_t));
  u32 i = 0;
  p_mgr->p_data = p_priv;
  p_mgr->do_log = do_log;
  p_mgr->do_send_key = do_send_key;
  p_mgr->attach = attach;
  for(i = 0; i < LOG_MODULE_ALL; i++)
  {
    p_priv->p_logger[i] = NULL;
  }
  // register your class in class factory here
  class_register(LOG_MGR_CLASS_ID, p_mgr);

  return p_mgr;
}




