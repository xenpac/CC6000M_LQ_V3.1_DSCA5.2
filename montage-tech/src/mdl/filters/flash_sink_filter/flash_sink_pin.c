/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "drv_dev.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"

#include "mtos_mutex.h"
#include "mtos_event.h"
#include "mt_time.h"
#include "vfs.h"
#include "drv_dev.h"
//#include "diskio.h"
#include "common.h"
#include "charsto.h"

//util
#include "lib_rect.h"
#include "class_factory.h"
#include "simple_queue.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "ifilter.h"
#include "sink_pin.h"
#include "ifilter.h"
#include "sink_filter.h"
#include "chain.h"
#include "common_filter.h"
#include "eva_filter_factory.h"
#include "flash_sink_pin_intra.h"
#include "flash_sink_filter.h"
#include "err_check_def.h"


/*static flash_sink_pin_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((flash_sink_pin_t *)_this)->private_data;
}*/

static RET_CODE on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE on_close(handle_t _this)
{
  return SUCCESS;
}


/*!
write flash
*/
static RET_CODE charsto_writeonly_net(charsto_device_t * p_dev, u32 addr, u8 * p_buf, u32 len)
{
  RET_CODE ret  = SUCCESS;
  spi_prot_block_type_t prot_type = PRT_UNPROT_ALL;
  
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_PROTECT_BLOCK_GET, (u32)(&prot_type));
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_PROTECT_BLOCK_SET, PRT_UNPROT_ALL);  
  //charsto_unprotect_all(p_dev);
  ret = charsto_writeonly(p_dev, addr, p_buf, len);
  //charsto_protect_all(p_dev);
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_PROTECT_BLOCK_SET, (u32)prot_type);

  return ret;
}

/*!
erase flash
*/
static RET_CODE charsto_erase_net(charsto_device_t * p_dev, u32 addr, u32 sec_cnt)
{
  RET_CODE ret = SUCCESS;
  spi_prot_block_type_t prot_type = PRT_UNPROT_ALL;
  
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_PROTECT_BLOCK_GET, (u32)(&prot_type));
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_PROTECT_BLOCK_SET, PRT_UNPROT_ALL);
  //charsto_unprotect_all(p_dev);
  ret = charsto_erase(p_dev, addr, sec_cnt);
  //charsto_protect_all(p_dev);
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_PROTECT_BLOCK_SET, (u32)prot_type);

  return ret;
}

static void on_receive(handle_t _this, media_sample_t *p_sample)
{
  RET_CODE ret = SUCCESS;
  handle_t dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  flash_burn_in_t *p_param = (flash_burn_in_t *)p_sample->p_user_data;
  u32 aligned_addr = 0;
  if (p_param == NULL)
  {
    return ;
  }
  aligned_addr = p_param->addr & (~ (u32)(UPG_BURN_PACKET_SIZE - 1));
  ret = charsto_erase_net(dev, aligned_addr, 1);
  if (ret != SUCCESS)
  {
    OS_PRINTF("\n##charsto_erase [0x%x] ret[%d]\n", aligned_addr, ret);
  }
  ret = charsto_writeonly_net(dev, p_param->addr, p_param->p_data, p_param->size);
  if (ret != SUCCESS)
  {
    OS_PRINTF("\n##charsto_writeonly [0x%x, %lu] ret[%d]\n", p_param->addr, p_param->size, ret);
  }
}

static BOOL notify_allocator(handle_t _this,
  imem_allocator_t *p_alloc, BOOL read_only)
{
  return TRUE;
}


flash_sink_pin_t * flash_sink_pin_create(flash_sink_pin_t *p_pin, interface_t *p_owner)
{
  flash_sink_pin_private_t *p_priv = NULL;
  base_input_pin_t *p_input_pin = NULL;
  sink_pin_para_t sink_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t media_format = {MT_FILE_DATA};
  
  //check input parameter
  CHECK_FAIL_RET_NULL(p_pin != NULL);
  CHECK_FAIL_RET_NULL(p_owner != NULL);

  //create base class
  sink_pin_para.p_filter = p_owner;
  sink_pin_para.p_name = "flash_pin";
  sink_pin_create(&p_pin->base_pin, &sink_pin_para);

  //init private date
  p_priv = &p_pin->private_data;
  memset(p_priv, 0, sizeof(flash_sink_pin_private_t));
  p_priv->p_this = p_pin; //this point

  //init member function

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->on_receive = on_receive;
  p_input_pin->notify_allocator = notify_allocator;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->on_open = on_open;
  p_ipin->on_close = on_close;
  p_ipin->add_supported_media_format(p_ipin, &media_format);

  return p_pin;
}

