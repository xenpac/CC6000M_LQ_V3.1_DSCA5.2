/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"

#include "mtos_msg.h"
#include "mtos_mem.h"

#include "class_factory.h"


#include "mdl.h"
#ifdef __LINUX__
#include "service.h"
#include "dvb_protocol.h"
#include "lib_util.h"
#include "dvb_svc.h"
#include "bat.h"
#include "cat.h"
#include "eit.h"
#include "nit.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"

/*!
  MDL extern pmt msg attatch buffer number
  */
#define MSG_PMT_MSG_DEPTH           (128)
/*!
  MDL extern other table msg attatch buffer number
  */
#define MSG_OTHER_MSG_DEPTH           (10)
#endif

typedef struct 
{
  u32 msgq_id;
} mdl_priv_t;
#ifndef WIN32
extern void print_mdl_ver(void); 
#endif
void mdl_init(void)
{
  mdl_priv_t *p_data = mtos_malloc(sizeof(mdl_priv_t));
  MT_ASSERT(p_data != NULL);
  class_register(MDL_CLASS_ID, p_data);
#ifndef WIN32
  print_mdl_ver(); 
#endif 
}
void mdl_set_msgq(handle_t handle, u32 id)
{
  mdl_priv_t *p_data = (mdl_priv_t *)handle;
#ifdef __LINUX__
  u8 *p_buffer = NULL;
  //attatch bat  msg buffer
  p_buffer = mtos_malloc(sizeof(bat_t) * MSG_OTHER_MSG_DEPTH);
  MT_ASSERT(NULL != p_buffer);
  memset(p_buffer, 0, sizeof(bat_t) * MSG_OTHER_MSG_DEPTH);
  mtos_messageq_attach(id, p_buffer, sizeof(bat_t), MSG_OTHER_MSG_DEPTH);

  //attatch cat  msg buffer
  p_buffer = mtos_malloc(sizeof(cat_cas_t) * MSG_OTHER_MSG_DEPTH);
  MT_ASSERT(NULL != p_buffer);
  memset(p_buffer, 0, sizeof(cat_cas_t) * MSG_OTHER_MSG_DEPTH);
  mtos_messageq_attach(id, p_buffer, sizeof(cat_cas_t), MSG_OTHER_MSG_DEPTH);

  //attatch eit  msg buffer
  p_buffer = mtos_malloc(sizeof(eit_t) * MSG_OTHER_MSG_DEPTH);
  MT_ASSERT(NULL != p_buffer);
  memset(p_buffer, 0, sizeof(eit_t) * MSG_OTHER_MSG_DEPTH);
  mtos_messageq_attach(id, p_buffer, sizeof(eit_t), MSG_OTHER_MSG_DEPTH);

  //attatch nit  msg buffer
  p_buffer = mtos_malloc(sizeof(nit_t) * MSG_OTHER_MSG_DEPTH);
  MT_ASSERT(NULL != p_buffer);
  memset(p_buffer, 0, sizeof(nit_t) * MSG_OTHER_MSG_DEPTH);
  mtos_messageq_attach(id, p_buffer, sizeof(nit_t), MSG_OTHER_MSG_DEPTH);

  //attatch pmt  msg buffer
  p_buffer = mtos_malloc(sizeof(pmt_t) * MSG_PMT_MSG_DEPTH);
  MT_ASSERT(NULL != p_buffer);
  memset(p_buffer, 0, sizeof(pmt_t) * MSG_PMT_MSG_DEPTH);
  mtos_messageq_attach(id, p_buffer, sizeof(pmt_t), MSG_PMT_MSG_DEPTH);

  //attatch sdt  msg buffer
  p_buffer = mtos_malloc(sizeof(sdt_t) * MSG_OTHER_MSG_DEPTH);
  MT_ASSERT(NULL != p_buffer);
  memset(p_buffer, 0, sizeof(sdt_t) * MSG_OTHER_MSG_DEPTH);
  mtos_messageq_attach(id, p_buffer, sizeof(sdt_t), MSG_OTHER_MSG_DEPTH);
#endif
  p_data->msgq_id = id;
}

void mdl_broadcast_msg(os_msg_t *p_msg)
{
  mdl_priv_t *p_data = class_get_handle_by_id(MDL_CLASS_ID);
  BOOL ret = FALSE;
  MT_ASSERT(p_msg != NULL);
  p_msg->is_brdcst = TRUE;
  p_msg->is_sync = FALSE;
  p_msg->extand_data = ~0;
  ret = mtos_messageq_send(p_data->msgq_id, p_msg);
  //OS_PRINTF("Leave mdl_broadcast_msg\n");
  if(!ret)
  {
    mtos_message_dump(p_data->msgq_id);
    MT_ASSERT(0);
  }
}

extern void ap_frm_send_mdl_msg(os_msg_t *p_msg);
void mdl_send_msg(os_msg_t *p_msg)
{
  //mdl_priv_t *p_data = class_get_handle_by_id(MDL_CLASS_ID);
  //BOOL ret = FALSE;
  MT_ASSERT(p_msg != NULL);
  p_msg->is_brdcst = FALSE;
  //p_msg->is_sync = FALSE;
  //OS_PRINTF("mdl send msg %x\n", p_msg->extand_data);
  ap_frm_send_mdl_msg(p_msg);
  //OS_PRINTF("mdl_send_msg  done \n");
//  ret = mtos_messageq_send(p_data->msgq_id, p_msg);
//  if(!ret)
  {
 //   mtos_message_dump(p_data->msgq_id);
//    MT_ASSERT(0);
  }
}

