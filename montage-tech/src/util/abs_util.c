/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"

// util
#include "class_factory.h"
#include "fcrc.h"

// os
#include "mtos_msg.h"
#include "mtos_printk.h"

// drv
#include "drv_dev.h"
#include "charsto.h"

// middleware
#include "mdl.h"
#include "data_manager.h"

#include "abs_util.h"

#define ABS_FLASH_HEADER_OFFSET (64 * 1024 * 2)
#define TDI_OFFSET (64 * 1024 * 2 - sizeof(abs_tdi_t))

void abs_util_read_tdi(abs_tdi_t *p_tdi)
{
  handle_t handle = 
    dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);


  if(charsto_read(handle, TDI_OFFSET, (u8 *)p_tdi,
    sizeof(abs_tdi_t)) != 0)
  {
    return;
  }
  p_tdi->manufacture_id = 0x22;
  p_tdi->hardware_id = 0x02;
  p_tdi->model_id = 0x02;
}

BOOL abs_util_read_flash_header(abs_flash_header_t *p_header)
{
  handle_t handle = 
    dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  u32 crc = 0;

  
  if(charsto_read(handle, ABS_FLASH_HEADER_OFFSET, (u8 *)p_header,
    sizeof(abs_flash_header_t)) != 0)
  {
    return FALSE;
  }

  crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,
    0xFFFFFFFF, (u8 *)p_header, sizeof(abs_flash_header_t) - 4);
  if(crc != p_header->flash_header_crc)
  {
    return FALSE;
  }
  return TRUE;
}
