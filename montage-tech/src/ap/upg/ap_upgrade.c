/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "hal_uart.h"
#include "ap_framework.h"
#include "class_factory.h"
#include "data_manager.h"
#include "ap_upgrade.h"
#include "ap_upg_priv.h"
#include "uart_win32.h"

/*!
  Send current status to UI
 */
void upg_update_status(upg_priv_t *p_priv, u8 status, int progress, 
                        char *p_description)
{
  event_t evt = {UPG_EVT_UPDATE_STATUS,(u32)&p_priv->upg_data.status, 0};
#ifdef _P2P_DEBUG
  OS_PRINTF("\n##upg_update_status[%s]!!\n", p_description);
#endif
  p_priv->upg_data.status.sm = status;
  p_priv->upg_data.status.progress = progress;
  p_priv->upg_data.status.p_description = (u8 *)p_description;
  ap_frm_send_evt_to_ui(APP_UPG, &evt);
}

/*!
  Send UPG event to UI
 */
static void upg_send_evt_to_ui(upg_evt_t id, u32 para1, u32 para2)
{
  event_t evt = {0};

  evt.id = id;
  evt.data1 = para1;
  
  ap_frm_send_evt_to_ui(APP_UPG, &evt);
}

/*!
  UPG init
 */
static void upg_init(class_handle_t handle)
{
  upg_priv_t *p_priv = (upg_priv_t *)handle;
  u32 msg_id_list[] = {UPG_CMD_START, UPG_CMD_EXIT, UPG_CMD_RESTART,};
  
  u8 msg_num = sizeof(msg_id_list) / sizeof(u32);

  p_priv->p_upg_impl->p_init(&p_priv->upg_data.cfg);
  
  p_priv->upg_data.sm = UPG_SM_IDLE;
  
  memcpy(p_priv->upg_data.msg_list, msg_id_list, sizeof(msg_id_list));
  p_priv->upg_data.msg_num = msg_num;
  p_priv->upg_data.key = UPGRD_KEY_IGNORE;

  return;
}

/*!
  UPG task
 */
static void upg_single_step(void *p_handle, os_msg_t *p_msg)
{
  upg_ret_t ret = UPG_RET_SUCCESS;
  u32 i = 0;
  upg_priv_t *p_priv = (upg_priv_t *)p_handle;
  upg_data_t *p_data = &p_priv->upg_data;
  upg_policy_t *p_policy = p_priv->p_upg_impl;
  slave_info_t *p_info = p_priv->upg_data.cfg.p_slave_info;
  
  if(NULL != p_msg)
  {
    switch(p_msg->content)
    {
      case UPG_CMD_START:
        
        if(UPG_SM_IDLE != p_data->sm)
        {
          break;
        }
       
        p_policy->p_pre_start(&(p_data->cfg));
        
        p_data->upg_mode = (p_msg->para1) >> 16;
        p_data->blk_num = (p_msg->para1)& 0xFF;

        p_data->divides_mode = ((p_msg->para1 >> 8)& 0xFF);

        p_data->divides = p_data->blk_num;

        p_data->divides_ready = 0;

          
        memcpy((void *)(p_data->upg_block), 
          (void *)(p_msg->para2), p_data->blk_num * sizeof(upg_block_t));

        
       
        uart_ioctl(UART_CMD_EN_INTERRUPT,FALSE,0); 

        p_data->key = UPGRD_KEY_IGNORE;
        p_data->sm = UPG_SM_SYNCING;
        break;
        
      default:
        break;
    }
    return;
  }  
  
  switch(p_data->sm)
  {
    case UPG_SM_IDLE:
      break;

    case UPG_SM_SYNCING:
      ret = p2p_cmd_sync(p_priv);
      if(ret == UPG_RET_SUCCESS)
      {
        for(i = 0; i < 150; i++)
        {
          mtos_task_delay_ms(10);
          
          if(UPGRD_KEY_EXIT == p_data->key)
          {
            p_data->sm = UPG_SM_EXIT;  
            return;
          }
          else if(UPGRD_KEY_MENU == p_data->key)
          {
            p_data->sm = UPG_SM_EXIT;  
            return;
          }
        }
        p_data->sm = UPG_SM_NEGOTIATING;          
      }
      else if(ret == UPG_RET_ERROR)
      {
        p_data->key = UPGRD_KEY_MENU;
        p_data->sm = UPG_SM_EXIT;           
      }
      else if(ret == UPG_RET_EXIT)
      {
        p_data->key = UPGRD_KEY_EXIT;
        p_data->sm = UPG_SM_EXIT;          
      }
      else if(ret == UPG_RET_MENU)
      {
        p_data->key = UPGRD_KEY_MENU;
        p_data->sm = UPG_SM_EXIT;          
      }
      break;
      
    case UPG_SM_NEGOTIATING:
      if(UPG_RET_SUCCESS != p2p_cmd_test(p_priv))
      {
        p_data->key = UPGRD_KEY_MENU;
        p_data->sm = UPG_SM_EXIT;   
        break;
      }

      if((UPGRD_ALL_SINGLE == p_data->upg_mode)||(UPGRD_ALL_MULTI == p_data->upg_mode))
      {
        if(UPG_RET_SUCCESS != p2p_cmd_inform(p_priv, p_data->blk_num, p_data->upg_block))
        {
          p_data->key = UPGRD_KEY_MENU;
          p_data->sm = UPG_SM_EXIT;   
        }
        else
        {
          p_data->sm = UPG_SM_TRANSFERING;
        }
      }
      else
      {
        memset(p_info, 0, sizeof(slave_info_t));

        if(UPG_RET_SUCCESS != p2p_cmd_collect(p_priv,TRUE, (u8 *)p_info))
        {
          p_data->key = UPGRD_KEY_MENU;
          p_data->sm = UPG_SM_EXIT;   
        }
        else if(UPG_RET_SUCCESS != p_priv->p_upg_impl->p_block_process(
          p_data->blk_num, p_data->upg_block, p_info))
        {
          p_data->key = UPGRD_KEY_MENU;
          p_data->sm = UPG_SM_EXIT;   
        }
        else if(UPG_RET_SUCCESS != p2p_cmd_inform(p_priv, p_data->blk_num, p_data->upg_block))
        {
          p_data->key = UPGRD_KEY_MENU;
          p_data->sm = UPG_SM_EXIT;   
        }
        else
        {
          p_data->sm = UPG_SM_TRANSFERING;
        }
      }
      break;
      
    case UPG_SM_TRANSFERING:
      if(p_data->divides_mode)
      {
        if(UPG_RET_SUCCESS != p2p_cmd_transfer(p_priv, FALSE, p_data->upg_block[0].master_block_id))
        {
          if((UPGRD_ALL_MULTI == p_data->upg_mode)|| (UPGRD_BLOCKS_MULTI == p_data->upg_mode))
          {            
            p_priv->upg_data.key = UPGRD_KEY_IGNORE;
            p_data->sm = UPG_SM_SYNCING;
          }
          else
          {
            p_data->key = UPGRD_KEY_MENU;
            p_data->sm = UPG_SM_EXIT;   
          }          
          return;
        }

        p_data->sm = UPG_SM_BURNING;

      }
      else
      {
        for(i = 0; i < p_data->blk_num; i++)
        {
          if(UPG_RET_SUCCESS != 
            p2p_cmd_transfer(p_priv, FALSE, p_data->upg_block[i].master_block_id))
          {
            if((UPGRD_ALL_MULTI == p_data->upg_mode)|| (UPGRD_BLOCKS_MULTI == p_data->upg_mode))
            {            
              p_priv->upg_data.key = UPGRD_KEY_IGNORE;
              p_data->sm = UPG_SM_SYNCING;
            }
            else
            {
              p_data->key = UPGRD_KEY_MENU;
            p_data->sm = UPG_SM_EXIT;   
          }          
          return;
          }
        }
        p_data->sm = UPG_SM_BURNING;
      }
      break;
      
    case UPG_SM_BURNING:
      if(p_data->divides_mode)
      {
        ret = p2p_cmd_burn(p_priv);
        if(UPG_RET_SUCCESS != ret)
        {
          if((UPGRD_ALL_MULTI == p_data->upg_mode) || (UPGRD_BLOCKS_MULTI == p_data->upg_mode))
          {
            p_priv->upg_data.key = UPGRD_KEY_IGNORE;
            p_data->sm = UPG_SM_SYNCING;
          }
          else
          {
            p_data->key = UPGRD_KEY_MENU;
            p_data->sm = UPG_SM_EXIT;
          }
          p_data->divides_ready = 0;
        } 
        else
        {
          p_data->divides_ready++;
          
          if (p_data->divides_ready >= p_data->divides)
          {
            p_data->sm = UPG_SM_REBOOTING;
          }
          else
          {
            p_data->sm = UPG_SM_TRANSFERING;
          }
        }

      }
      else
      {
        ret = p2p_cmd_burn(p_priv);
        if(UPG_RET_SUCCESS != ret)
        {
          if((UPGRD_ALL_MULTI == p_data->upg_mode) || (UPGRD_BLOCKS_MULTI == p_data->upg_mode))
          {
            p_priv->upg_data.key = UPGRD_KEY_IGNORE;
            p_data->sm = UPG_SM_SYNCING;
          }
          else
          {
            p_data->key = UPGRD_KEY_MENU;
            p_data->sm = UPG_SM_EXIT;
          }
        } 
        else
        {
          p_data->sm = UPG_SM_REBOOTING;
        }
      }
      break;
      
    case UPG_SM_REBOOTING:
      p2p_cmd_reboot(p_priv);
        
      if((UPGRD_ALL_MULTI == p_data->upg_mode) || (UPGRD_BLOCKS_MULTI == p_data->upg_mode))
      {
        p_data->sm = UPG_SM_SYNCING;
      }
      else
      {
        p_data->sm = UPG_SM_IDLE;
        p_policy->p_post_exit(&(p_data->cfg));
        upg_send_evt_to_ui(UPG_EVT_SUCCESS, 0, 0);
      }
      break;
      
    case UPG_SM_EXIT:
      p_policy->p_post_exit(&(p_data->cfg));

      if(p_data->key == UPGRD_KEY_EXIT)
      {
        upg_send_evt_to_ui(UPG_EVT_QUIT_UPG, 1, 0);
      }
      else if(p_data->key == UPGRD_KEY_MENU)
      {
        upg_send_evt_to_ui(UPG_EVT_QUIT_UPG, 0, 0);
      }

      uart_ioctl(UART_CMD_EN_INTERRUPT,TRUE,0);

      p_data->key = UPGRD_KEY_IGNORE;
      p_data->sm = UPG_SM_IDLE;
      break;
      
    default:
      break;
  }
}

/*!
  message Pre-process 
 */
static void upg_pre_process_msg(void *handle, os_msg_t *p_msg)
{
  upg_priv_t *p_priv = (upg_priv_t *)handle;

  if(NULL != p_msg)
  {
    switch(p_msg->content)
    {
      case UPG_CMD_EXIT:
        if(p_msg->para1)
        {
          p_priv->upg_data.key = UPGRD_KEY_EXIT;
        }
        else
        {
          p_priv->upg_data.key = UPGRD_KEY_MENU;
        }
        break;
      default:
        break;
    }
  }  
}

/*!
  Construct UPG instance
 */
app_t * construct_ap_upg(upg_policy_t *p_upg_policy)
{
  upg_priv_t *p_upg_priv = mtos_malloc(sizeof(upg_priv_t));

  //clear private data
  memset(p_upg_priv, 0, sizeof(upg_priv_t));

  //Attach upg policy
  p_upg_priv->p_upg_impl = p_upg_policy;
  
  //Attach scan applition
  p_upg_priv->upg_app.init = upg_init;
  p_upg_priv->upg_app.pre_process_msg = upg_pre_process_msg;
  p_upg_priv->upg_app.task_single_step = upg_single_step;
  p_upg_priv->upg_app.get_msgq_timeout = NULL;
  p_upg_priv->upg_app.p_data = (void *)p_upg_priv;

  return &p_upg_priv->upg_app;
}

