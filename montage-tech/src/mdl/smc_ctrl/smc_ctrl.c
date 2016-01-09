/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std headers
#include "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_mem.h"
#include "mtos_printk.h"

#include "smc_ctrl.h"

#define SMC_MSG_CONTENT_LEN 512

typedef struct
{
  smc_status_t status;
  u8 slot;
  u8 card_type;
  u8 msg_content[SMC_MSG_CONTENT_LEN + 1];
  u32 msg_length;
} smcctrl_priv_t;

void smcctrl_init(void)
{
  smcctrl_priv_t *p_this = mtos_malloc(sizeof(smcctrl_priv_t));
  MT_ASSERT(p_this != NULL);

  memset(p_this, 0, sizeof(smcctrl_priv_t));
  p_this->status = SMC_UNKNOWN;
  p_this->slot = 0;
  p_this->card_type = SMC_UNKNOWN_CARD;
  class_register(SMART_CARD_CLASS_ID, p_this);
}

void smcctrl_status_notify(smc_status_t status, 
  u8 slot, u8 *p_msg, u32 msg_len, u8 card_type)
{
  smcctrl_priv_t *p_priv = (smcctrl_priv_t *)
    class_get_handle_by_id(SMART_CARD_CLASS_ID);
  p_priv->status = status;
  p_priv->slot = slot;
  p_priv->card_type = card_type;

  if (msg_len > SMC_MSG_CONTENT_LEN)
  {
    msg_len = SMC_MSG_CONTENT_LEN;
  }
  memcpy(p_priv->msg_content, p_msg, msg_len);
  p_priv->msg_content[msg_len] = '\0';

  OS_PRINTF("CA: status %d, slot %d, msg %s\n", status, slot, p_msg);
}

smc_status_t smcctrl_get_status(void)
{
  smcctrl_priv_t *p_priv = (smcctrl_priv_t *)
    class_get_handle_by_id(SMART_CARD_CLASS_ID);
  smc_status_t status = p_priv->status;
  p_priv->status = SMC_UNKNOWN;
  return status;
}

u8 smcctrl_get_cardtype(void)
{
  smcctrl_priv_t *p_priv = (smcctrl_priv_t *)
    class_get_handle_by_id(SMART_CARD_CLASS_ID);
  MT_ASSERT(p_priv != NULL);

  return p_priv->card_type;
}

