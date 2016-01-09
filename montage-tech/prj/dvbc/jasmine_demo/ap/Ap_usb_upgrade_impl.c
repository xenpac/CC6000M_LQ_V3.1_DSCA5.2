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
#include "ap_usb_upgrade.h"

#include "lib_rect.h"
#include "lib_memp.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "lib_memf.h"
#include "flinger.h"
#include "gdi.h"
#include "ctrl_base.h"
#include "gui_resource.h"
#include "gui_paint.h"
#include "framework.h"
#include "mdl.h"
#include "uio_key_def.h"

#include "ap_uio.h"
#ifdef WIN32
#include "uart_win32.h"
#endif

/*!
  init upgrade basic information
  */
usb_upg_ret_t ap_usb_upg_init(usb_upg_config_t *p_cfg)
{

  return USB_UPG_RET_SUCCESS;
}

/*!
  check if the block id exist in stb side
  */
usb_upg_ret_t ap_usb_upg_block_process(u8 block_num, usb_upg_block_t *p_block, 
                          stb_info_t *p_info)
{
  u8 i = 0;
  //u8 j = 0;
  BOOL flag = FALSE;

 // for(j = 0; j < block_num; j++)
  {
    flag = FALSE;
    for(i = 0; i < p_info->block_num; i++)  
    {
      //find if the block ID exists in stb side
      if(p_block->usb_block_id == p_info->blocks[i].id)
      {
        p_block->stb_block_id = p_info->blocks[i].id;
        flag = TRUE;
        break;
      }
    }
    if(FALSE == flag)
      return USB_UPG_RET_ERROR;
    
  }
  return USB_UPG_RET_SUCCESS;
}

/*!
  clean the enviroment
  */
void ap_usb_upg_pre_start(usb_upg_config_t *p_cfg)
{
/*
  video_device_t *p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                      SYS_DEV_TYPE_VIDEO);
  pti_device_t *p_pti_dev  = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                  SYS_DEV_TYPE_PTI);
  
  mtos_task_lock();
  dev_io_ctrl(p_video_dev, VIDEO_CMD_VIDEO_DISPLAY, FALSE);
  dev_io_ctrl(p_video_dev, VIDEO_CMD_STOP, FALSE);
  pti_dev_av_enable(p_pti_dev, FALSE);
  mtos_task_unlock();
  mtos_task_delay_ms(500);
*/
}

/*!
  post exit handle
  */
void ap_usb_upg_post_exit(usb_upg_config_t *p_cfg)
{
}

extern u8 get_key_value(u8 irda, u8 vkey);

/*!
  find hardware code for the exit/power key
  */
u16 ap_usb_upg_get_code(u32 index, u16 *code)
{
  u8 v_key = V_KEY_CANCEL;
  u16 cnt = 2;
  
  switch(index)
  {
    case USB_UPGRD_CODE_EXIT:
      v_key = V_KEY_CANCEL;
      break;
    case USB_UPGRD_CODE_POWER:
      v_key = V_KEY_POWER;
      break;
    case USB_UPGRD_CODE_MENU:
      v_key = V_KEY_MENU;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  
  cnt = 2;
  //code[0] = get_key_value(0, v_key);
  code[1] = get_key_value(1, v_key);

    
  return cnt;
}

/*!
  construct the usb upg policy
  */
usb_upg_policy_t *construct_usb_upg_policy(void)
{
  usb_upg_policy_t *p_usb_upg_policy = mtos_malloc(sizeof(usb_upg_policy_t));
  MT_ASSERT(p_usb_upg_policy != NULL);
  //lint -e668
  memset(p_usb_upg_policy, 0, sizeof(usb_upg_policy_t));
  //lint +e668
  
  p_usb_upg_policy->p_init = ap_usb_upg_init;
  p_usb_upg_policy->p_block_process = ap_usb_upg_block_process;
  p_usb_upg_policy->p_pre_start = ap_usb_upg_pre_start;
  p_usb_upg_policy->p_post_exit = ap_usb_upg_post_exit;
  p_usb_upg_policy->p_get_code = ap_usb_upg_get_code;

  return p_usb_upg_policy;
}


