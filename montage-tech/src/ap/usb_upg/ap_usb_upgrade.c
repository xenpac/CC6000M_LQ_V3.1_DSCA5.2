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
#include "lib_util.h"
#include "lib_unicode.h"
#include "vfs.h"
#include "data_manager.h"
#include "ap_usb_upgrade.h"
#include "ap_usb_upg_priv.h"
    
#include "string.h"
#include "drv_dev.h"
#include "charsto.h"
#include "err_check_def.h"

#include "uart_win32.h"
#ifndef WIN32
#include "hal_misc.h"
#else
#endif

//#define AP_USB_UPG_DEBUG

#define DMH_START_ADDR 0xB0000
#define USB_UPG_FILE_HEADER_SIZE 12

#define ALIGN4(x)    (((x) + 3) & (~3))

#ifdef AP_USB_UPG_DEBUG
static void _debug_param(usb_upg_priv_t *p_priv)
{
  if(!p_priv)
  {
    return;
  }

  OS_PRINTF("[UPG]---------- USB UPG Start ----------\n");
  OS_PRINTF("[UPG]          mode: %d\n", p_priv->usb_upg_data.upg_mode);
  OS_PRINTF("[UPG]          ext_buf: 0x%x\n", p_priv->ext_buf);
  OS_PRINTF("[UPG]          ext_size: 0x%x\n", p_priv->ext_size);
  OS_PRINTF("[UPG]          flash_offset_addr: 0x%x\n", p_priv->flash_offset_addr);
  OS_PRINTF("[UPG]          burn_flash_size: 0x%x\n", p_priv->burn_flash_size);
  //OS_PRINTF("[UPG]          using_size: 0x%x\n", p_priv->using_size);
  OS_PRINTF("[UPG]          unzip_buffer: 0x%x\n", p_priv->usb_upg_data.cfg.p_unzip_buffer);
  OS_PRINTF("[UPG]          unzip_size: 0x%x\n", p_priv->usb_upg_data.cfg.unzip_size);
  OS_PRINTF("[UPG]          head_size: 0x%x\n", p_priv->usb_upg_data.head_size);
  OS_PRINTF("[UPG]------------------------------------\n");
  return;
}
#endif

/*!
  Send current status to UI
 */
void usb_upg_update_status(usb_upg_priv_t *p_priv, u8 status, int progress, 
                        u8 *p_description)
{
  event_t evt = {USB_UPG_EVT_UPDATE_STATUS,(u32)&p_priv->usb_upg_data.status, 0};

  p_priv->usb_upg_data.status.sm = status;
  p_priv->usb_upg_data.status.progress = progress;
  p_priv->usb_upg_data.status.p_description = p_description;
  ap_frm_send_evt_to_ui(APP_USB_UPG, &evt);
}
static void usb_dump_add_head_info(usb_upg_priv_t *p_priv)
{
  usb_upg_data_t *p_data = &p_priv->usb_upg_data;
  void *p_addr = NULL;

  if(0 == p_data->head_size)
  {
      #ifdef AP_USB_UPG_DEBUG
      OS_PRINTF("[UPG]%s: no head info\n", __FUNCTION__);
      #endif
      return;
  }
  else if(USB_UPG_MAX_HEAD_SIZE < p_data->head_size)
  {
      p_data->head_size = USB_UPG_MAX_HEAD_SIZE;
  }
  
  p_addr = (u8 *)p_priv->ext_buf;
  switch(p_data->upg_mode)
  {
      case USB_UPGRD_ALL:
        p_data->head_size = 0;
        break;
      case USB_UPGRD_USER_DB: 
        //add dm header
        memcpy(p_addr, p_data->head_data, p_data->head_size);
        p_data->burnt_size += p_data->head_size;
        break;
      default:
        break;
  }

}
static void * get_attach_buffer(usb_upg_priv_t *p_priv, u32 size)
{
  void *p_addr = NULL;
  
    size = ALIGN4(size);
    p_addr = (u8 *)p_priv->ext_buf;
    p_priv->ext_buf = p_priv->ext_buf + size;
    p_priv->using_size += size;
    CHECK_FAIL_RET_NULL(p_priv->using_size <= p_priv->ext_size);
    return p_addr;
}

/*!
  Send UPG event to UI
 */
static void usb_upg_send_evt_to_ui(usb_upg_evt_t id, u32 para1, u32 para2)
{
  event_t evt = {0};

  evt.id = id;
  evt.data1 = para1;

  ap_frm_send_evt_to_ui(APP_USB_UPG, &evt);
}

/*!
  UPG init
 */
static void usb_upg_init(class_handle_t handle)
{
  usb_upg_priv_t *p_priv = (usb_upg_priv_t *)handle;
  u32 msg_id_list[] = {USB_UPG_CMD_START_BURN,USB_UPG_CMD_EXIT};
  
  u8 msg_num = sizeof(msg_id_list) / sizeof(u32);
  CHECK_FAIL_RET_VOID(msg_num <= USB_UPG_CMD_CNT);

  p_priv->p_usb_upg_impl->p_init(&p_priv->usb_upg_data.cfg);
  
  p_priv->usb_upg_data.sm = USB_UPG_SM_IDLE;
  
  memcpy(p_priv->usb_upg_data.msg_list, msg_id_list, sizeof(msg_id_list));
  p_priv->usb_upg_data.msg_num = msg_num;
  return;
}

/*!
write flash
*/
static RET_CODE charsto_writeonly_usb(charsto_device_t * p_dev, u32 addr, u8 * p_buf, u32 len)
{
  RET_CODE ret  = SUCCESS;
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  ret = charsto_writeonly(p_dev, addr, p_buf, len);
  
  //restore
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

  return ret;
}

/*!
erase flash
*/
static RET_CODE charsto_erase_usb(charsto_device_t * p_dev, u32 addr, u32 sec_cnt)
{
  RET_CODE ret = SUCCESS;
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);

  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  ret = charsto_erase(p_dev, addr, sec_cnt);
  
  //restore
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

  return ret;
}

/*!
  Get the slave burn progress
 */
usb_upg_ret_t usb_upg_burn(usb_upg_priv_t *p_usb_upg_priv)
{
  u8 *p_start_addr = NULL;
  u32 total_size = p_usb_upg_priv->burn_flash_size;
  u8 percent = 0;
  charsto_device_t *p_charsto = NULL;
  u32 burn_addr = 0;
  u32 aligned_addr = 0;
  static u8 *p_temp_buf = NULL;
  u32 left_backup_size = 0;
  s32 right_backup_size = 0;
  u32 remain_size = 0;
  
  if(p_temp_buf == NULL)
    {
      p_temp_buf = mtos_malloc(USB_UPG_BURN_PACKET_SIZE);
      if(p_temp_buf == NULL)
        {
          return USB_UPG_RET_ERROR;
        }
    }
  
  p_charsto = (charsto_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, \
                                                      SYS_DEV_TYPE_CHARSTO);
  if(NULL == p_charsto)
  {
    return USB_UPG_RET_ERROR;
  }
  remain_size = total_size - p_usb_upg_priv->usb_upg_data.burnt_size;
  p_start_addr = p_usb_upg_priv->usb_upg_data.cfg.p_unzip_buffer + \
                          p_usb_upg_priv->usb_upg_data.burnt_size;
  burn_addr = p_usb_upg_priv->flash_offset_addr + \
                      p_usb_upg_priv->usb_upg_data.burnt_size;
  aligned_addr = burn_addr & (~ (u32)(USB_UPG_BURN_PACKET_SIZE - 1));
  left_backup_size = burn_addr % USB_UPG_BURN_PACKET_SIZE;
  right_backup_size = USB_UPG_BURN_PACKET_SIZE - left_backup_size - remain_size;

  #ifdef AP_USB_UPG_DEBUG
  OS_PRINTF("[UPG]---------- USB UPG Burn ----------\n");
  OS_PRINTF("[UPG]          remain_size: 0x%x\n", remain_size);
  OS_PRINTF("[UPG]          mem_addr: 0x%x\n", p_start_addr);
  OS_PRINTF("[UPG]          burn_addr: 0x%x(aligned: 0x%x)\n", burn_addr, aligned_addr);
  if(left_backup_size != 0)
  {
    OS_PRINTF("[UPG]          left_backup_size: 0x%x\n", left_backup_size);
  }
  if(right_backup_size > 0)
  {
    OS_PRINTF("[UPG]          right_backup_size: 0x%x\n", right_backup_size);
  }
  #endif
  
//backup left and right
  if(left_backup_size != 0)
    {
      charsto_read(p_charsto, aligned_addr, p_temp_buf, left_backup_size);
    }
  if(right_backup_size > 0)
    {
      charsto_read(p_charsto, burn_addr + remain_size, \
                           p_temp_buf + left_backup_size, right_backup_size);
    }
//backup end
  if(SUCCESS != charsto_erase_usb(p_charsto, aligned_addr, 1))
    {
      #ifdef AP_USB_UPG_DEBUG
      OS_PRINTF("[UPG]Flash erase error!\n");
      #endif
      return USB_UPG_RET_ERROR;
    }

  if(left_backup_size != 0)
   {
     charsto_writeonly_usb(p_charsto, aligned_addr, \
                                            p_temp_buf, left_backup_size);
   }

  if(right_backup_size <= 0)
    {
      charsto_writeonly_usb(p_charsto, burn_addr, p_start_addr, \
                                            USB_UPG_BURN_PACKET_SIZE - left_backup_size);
      p_usb_upg_priv->usb_upg_data.burnt_size += \
        (USB_UPG_BURN_PACKET_SIZE - left_backup_size);      
    }
  else
    {
      charsto_writeonly_usb(p_charsto, burn_addr, p_start_addr, remain_size);
      p_usb_upg_priv->usb_upg_data.burnt_size += remain_size;
      charsto_writeonly_usb(p_charsto, burn_addr + remain_size, \
                                            p_temp_buf + left_backup_size , right_backup_size);
    }
#ifdef WIN32
  mtos_task_delay_ms(50);
#endif 

  percent =  (p_usb_upg_priv->usb_upg_data.burnt_size * 100) / total_size;
  usb_upg_update_status(p_usb_upg_priv, \
                                          USB_UPG_GET_STS(p_usb_upg_priv),percent,NULL);

  #ifdef AP_USB_UPG_DEBUG
  OS_PRINTF("[UPG]          burnt_size: 0x%x\n", p_usb_upg_priv->usb_upg_data.burnt_size);
  #endif
  
  if(p_usb_upg_priv->usb_upg_data.burnt_size == total_size)
  {
    p_usb_upg_priv->usb_upg_data.burn_end = TRUE;
    mtos_free(p_temp_buf);
    p_temp_buf = NULL;
    #ifdef AP_USB_UPG_DEBUG
    OS_PRINTF("[UPG]          END\n");
    #endif
  }
  #ifdef AP_USB_UPG_DEBUG
  OS_PRINTF("[UPG]---------------------------------\n");
  #endif
  return USB_UPG_RET_SUCCESS;
}


static usb_upg_ret_t usb_dump_read_flash(usb_upg_priv_t *p_usb_upg_priv)
{
  u8 *p_buf_addr = NULL;
  u32 size = 0;
  u32 total_size = p_usb_upg_priv->usb_upg_data.cfg.unzip_size;
  u8 percent = 0;
  charsto_device_t *p_charsto = NULL;
  u32 flash_addr = 0;
  
  p_charsto = (charsto_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE,
                                                       SYS_DEV_TYPE_CHARSTO);
  if(NULL == p_charsto)
  {
    return USB_UPG_RET_ERROR;
  }

  size = ((p_usb_upg_priv->usb_upg_data.burnt_size + 
              USB_UPG_BURN_PACKET_SIZE) <= total_size) ? USB_UPG_BURN_PACKET_SIZE : 
              (total_size - p_usb_upg_priv->usb_upg_data.burnt_size);

  p_buf_addr = p_usb_upg_priv->usb_upg_data.cfg.p_unzip_buffer + 
                       p_usb_upg_priv->usb_upg_data.burnt_size;

  
  //#ifndef WIN32
  flash_addr = p_usb_upg_priv->flash_offset_addr +
                    p_usb_upg_priv->usb_upg_data.burnt_size - 
                    p_usb_upg_priv->usb_upg_data.head_size;

  #ifdef AP_USB_UPG_DEBUG
  OS_PRINTF("[UPG]read flash addr[%x] size[0x%x] \n", flash_addr, size);
  #endif
  
  if(SUCCESS != charsto_read(p_charsto, flash_addr, p_buf_addr,size))
  {
    #ifdef AP_USB_UPG_DEBUG
    OS_PRINTF("[UPG]read flash error!addr[%x] size[0x%x]\n",flash_addr, size);
    #endif
    return USB_UPG_RET_ERROR;
  }
  //#else
  #ifdef WIN32
  mtos_task_delay_ms(50);//analog of burning on simulator
  #endif
  //#endif
  
  p_usb_upg_priv->usb_upg_data.burnt_size += size;

  percent =  (p_usb_upg_priv->usb_upg_data.burnt_size * 100) / total_size;

  usb_upg_update_status(p_usb_upg_priv, \
                                          USB_UPG_GET_STS(p_usb_upg_priv),percent,NULL);

  if(p_usb_upg_priv->usb_upg_data.burnt_size == total_size)
  {
    p_usb_upg_priv->usb_upg_data.burn_end = TRUE;
  }
  return USB_UPG_RET_SUCCESS;
}

usb_upg_ret_t create_dump_file(usb_upg_priv_t *p_usb_upg_priv, u8 zipped)
{
  usb_upg_data_t *p_data = &p_usb_upg_priv->usb_upg_data;
  hfile_t file = NULL;

  p_data->p_file = vfs_open(p_data->upg_file_path, VFS_NEW);
  file = p_data->p_file;  
  if (NULL == file)
  {
    return USB_OPEN_UPGRADE_FILE_ERROR;
  }
  #ifdef AP_USB_UPG_DEBUG
  OS_PRINTF("[UPG]File is opened successfully...\n");
  #endif
  return USB_UPG_RET_SUCCESS;
}


usb_upg_ret_t save_dump_file(usb_upg_priv_t *p_usb_upg_priv, u8 zipped)
{
  u32 size = 0;
  u32 total_size = 0;
  u8 *p_buff = NULL;
  u8 percent = 0;
  usb_upg_data_t *p_data = &p_usb_upg_priv->usb_upg_data;
  hfile_t file = p_data->p_file;

  total_size = p_data->cfg.unzip_size;
  
  p_buff = p_data->cfg.p_unzip_buffer + p_data->dnloaded_size;
  
  size = ((p_usb_upg_priv->usb_upg_data.dnloaded_size + \
            USB_DUMP_WRITE_PACKET_SIZE) <= total_size) ? USB_DUMP_WRITE_PACKET_SIZE \
            : (total_size - p_usb_upg_priv->usb_upg_data.dnloaded_size);
  #ifdef AP_USB_UPG_DEBUG
  OS_PRINTF("[UPG]before write addr:%x  size:%d totsize:%d dnloadedsize:%d\n", 
                      p_data->cfg.p_unzip_buffer, size, total_size, p_data->dnloaded_size);
  #endif
  if(size != vfs_write(p_buff, size, 1, file))
  {
    #ifdef AP_USB_UPG_DEBUG
    OS_PRINTF("[UPG]Failed to read the upg file...\n");
    #endif
    return USB_READ_UPGFILE_DATA_RET_ERROR;
  }
  p_data->dnloaded_size += size;
  
  percent =  (p_usb_upg_priv->usb_upg_data.dnloaded_size * 100) / total_size;

  usb_upg_update_status(p_usb_upg_priv, USB_UPG_GET_STS(p_usb_upg_priv),percent, NULL);

  if(p_usb_upg_priv->usb_upg_data.dnloaded_size == total_size)
  {
    vfs_close(file);
    p_usb_upg_priv->usb_upg_data.dnload_end = TRUE;
  }
  
  return USB_UPG_RET_SUCCESS;
}

static BOOL find_dmh_indicator_from_upgfile(usb_upg_priv_t *p_usb_upg_priv)
{
  u32 offset = 0;
  const u32 sec_size = 64*1024;
  usb_upg_data_t *p_data = &p_usb_upg_priv->usb_upg_data;
  u8 *p_upg_buf = p_data->cfg.p_unzip_buffer;
  u8 dmh_indicator[USB_UPG_DMH_INDC_SIZE + 1] = "*^_^*DM(^o^)";

  while(offset <= (p_data->cfg.unzip_size - sec_size))
  {
    if(memcmp(p_upg_buf + offset, dmh_indicator, USB_UPG_DMH_INDC_SIZE) == 0)
    {
      OS_PRINTF("get dmh indicator at offset 0x%x\n", offset);
      return TRUE;
    }

    offset += sec_size;
  }

  return FALSE;
}

usb_upg_ret_t usb_check_upgfile_validness(usb_upg_priv_t *p_usb_upg_priv)
{
  usb_upg_data_t *p_data = &p_usb_upg_priv->usb_upg_data;
  u8 *p_upg_buf = p_data->cfg.p_unzip_buffer;
  u8 dmh_indicator[USB_UPG_DMH_INDC_SIZE + 1] = "*^_^*DM(^o^)";
  u8 offset = 0;
  switch(p_data->upg_mode)
  {
      case USB_UPGRD_ALL:
      case USB_UPGRD_MAINCODE:
        if(0 == p_usb_upg_priv->flash_offset_addr)
        {
          offset = USB_UPG_FILE_HEADER_SIZE;
        }
        else if(DMH_START_ADDR == p_usb_upg_priv->flash_offset_addr)
        {
          offset = 0;
        }
        else{}
        
        if(memcmp(p_upg_buf + offset, dmh_indicator, USB_UPG_DMH_INDC_SIZE) != 0)
        {
          //try to find dmh indicator
          if (! find_dmh_indicator_from_upgfile(p_usb_upg_priv))
          {
            return USB_INVALID_UPG_FILE_ERROR;
          }
        }
        break;
      case USB_UPGRD_USER_DB:
        if(memcmp(p_upg_buf , dmh_indicator, USB_UPG_DMH_INDC_SIZE) != 0)
        {
          return USB_INVALID_UPG_FILE_ERROR;
        }
        else //exclude the head info
        {
            p_data->cfg.p_unzip_buffer += p_data->head_size;
            p_data->cfg.unzip_size -= p_data->head_size;
        }
        break; 
    case USB_UPGRD_CONSTANT_CW:
      break;
    default:
        return USB_INVALID_UPG_FILE_ERROR;
  }

  return USB_UPG_RET_SUCCESS;
}

usb_upg_ret_t usb_upg_reboot(usb_upg_priv_t *p_usb_upg_priv)
{
#ifndef WIN32
  #ifdef AP_USB_UPG_DEBUG
  OS_PRINTF("[UPG]%s:reset stb! \n", __FUNCTION__);
  #endif
  hal_pm_reset();
#endif
  return USB_UPG_RET_SUCCESS;
}

/*!
  UPG task
 */
static void usb_upg_single_step(void *p_handle, os_msg_t *p_msg)
{
  usb_upg_ret_t ret = USB_UPG_RET_SUCCESS;
  usb_upg_priv_t *p_priv = (usb_upg_priv_t *)p_handle;
  usb_upg_data_t *p_data = &p_priv->usb_upg_data;
  usb_upg_policy_t *p_policy = p_priv->p_usb_upg_impl;
  usb_upg_start_params_t *p_start = NULL;

  CHECK_FAIL_RET_VOID(NULL != p_priv);
  
  if(NULL != p_msg)
  {
    switch(p_msg->content)
    {
      case USB_UPG_CMD_START_BURN:
        CHECK_FAIL_RET_VOID(p_msg->para1 != 0);
        p_start = (usb_upg_start_params_t *)p_msg->para1;

        p_priv->ext_buf = p_start->ext_buf_addr;
        p_priv->ext_size = p_start->ext_buf_size;
        p_priv->flash_offset_addr = p_start->flash_offset_addr;
        p_priv->burn_flash_size = p_start->burn_flash_size;
        p_priv->using_size = 0;
        p_priv->usb_upg_data.dnloaded_size = 0;
        p_priv->usb_upg_data.burnt_size = 0;
        p_priv->usb_upg_data.cfg.p_unzip_buffer = \
                      (u8 *)get_attach_buffer(p_priv, p_priv->ext_size);
        p_priv->usb_upg_data.burn_end = FALSE;
        p_priv->usb_upg_data.dnload_end = FALSE;
        p_priv->usb_upg_data.cfg.unzip_size = p_priv->ext_size;;
        p_priv->usb_upg_data.head_size = p_start->head_size;
        p_priv->usb_upg_data.upg_mode = p_start->mode;

        p_data->sm = USB_UPG_SM_CHECK_VALIDNESS;
        #ifdef AP_USB_UPG_DEBUG
        _debug_param(p_priv);
        #endif
        break;
      case USB_UPG_CMD_EXIT:
        p_policy->p_post_exit(&(p_data->cfg));
        p_data->sm = USB_UPG_SM_IDLE;
        break;
      case USB_DUMP_CMD_START_DUMP:
        CHECK_FAIL_RET_VOID(USB_UPG_SM_IDLE == p_data->sm);
        CHECK_FAIL_RET_VOID(p_msg->para1 != 0);
        p_start = (usb_upg_start_params_t *)p_msg->para1;
        p_data->upg_mode = p_start->mode;
        p_data->burn_end = FALSE;
        p_data->dnload_end = FALSE;

        p_data->head_size = p_start->head_size;
        memset(p_data->head_data, 0, sizeof(p_data->head_data));
        if(0 != p_data->head_size)
        {
          memcpy((void *)(p_data->head_data), p_start->head_data, sizeof(p_data->head_data));
        }
        
        memset(p_data->upg_file_path, 0, MAX_FILE_PATH * sizeof(u16));
        uni_strncpy(p_data->upg_file_path, p_start->file, uni_strlen(p_start->file));

        p_priv->ext_buf = p_start->ext_buf_addr;
        p_priv->ext_size = p_start->ext_buf_size;
        p_priv->flash_offset_addr = p_start->flash_offset_addr;
        p_priv->burn_flash_size = 0;
        p_priv->using_size = 0;
        p_priv->usb_upg_data.burnt_size = 0;
        p_priv->usb_upg_data.dnloaded_size = 0;
        usb_dump_add_head_info(p_priv);    //add user db head info
        p_priv->usb_upg_data.cfg.p_unzip_buffer = 
              (u8 *)get_attach_buffer(p_priv, p_priv->ext_size);
        p_priv->usb_upg_data.cfg.unzip_size = p_priv->ext_size;        
        p_data->sm = USB_DUMP_SM_READING_FLASH;
        #ifdef AP_USB_UPG_DEBUG
        OS_PRINTF("[UPG]dump start :upg_mode[%d],ext_buf addr[0x%x], ext_size[0x%x]\n", 
                          p_data->upg_mode, p_priv->ext_buf,p_priv->ext_size);
        _debug_param(p_priv);
        #endif
        break;
      default:
        break;
    }
    return;
  }  
  
  switch(p_data->sm)
  {
    case USB_UPG_SM_IDLE:
      break;
    case USB_UPG_SM_CHECK_VALIDNESS:
      ret = usb_check_upgfile_validness(p_priv);

      if(ret == USB_INVALID_UPG_FILE_ERROR)
      {
        usb_upg_send_evt_to_ui(USB_INVLID_UPGRADE_FILE, 0, 0);
        p_data->sm = USB_UPG_SM_EXIT;  
      }
      else
      {
        p_data->sm = USB_UPG_SM_BURNING;
      }
      break;    
    case USB_UPG_SM_BURNING:
      ret = usb_upg_burn(p_priv);

      if(USB_UPG_RET_SUCCESS != ret)
      {
          usb_upg_send_evt_to_ui(USB_UPG_BURN_FLASH_ERROR, 0, 0);
          p_data->sm = USB_UPG_SM_EXIT;
      } 
      else
      {
        if(p_data->burn_end == TRUE)
        {
          usb_upg_update_status(p_priv, USB_UPG_GET_STS(p_priv),100,NULL);
          p_data->sm = USB_UPG_SM_REBOOTING;
        }
        else
        {
          p_data->sm = USB_UPG_SM_BURNING;
        }
      }
      break;
    case USB_UPG_SM_REBOOTING:
      p_policy->p_post_exit(&(p_data->cfg));
      usb_upg_reboot(p_priv);
      p_data->sm = USB_UPG_SM_IDLE;
      usb_upg_send_evt_to_ui(USB_UPG_EVT_SUCCESS, 0, 0);
      break;    
    case USB_DUMP_SM_READING_FLASH:
      if(USB_UPG_RET_SUCCESS != usb_dump_read_flash(p_priv))
        {
          usb_upg_send_evt_to_ui(USB_DUMP_READ_FLASH_ERROR, 0, 0);
          p_data->sm = USB_UPG_SM_EXIT;
        }
      else
      {
        if(p_data->burn_end == TRUE)
        {            
          usb_upg_update_status(p_priv, USB_UPG_GET_STS(p_priv),100,NULL);
          usb_upg_send_evt_to_ui(USB_DUMP_READ_FLASH_SUCCESS, 0, 0);
          p_data->sm = USB_DUMP_SM_CREATING_FILE;
        }
        else
        {
          p_data->sm = USB_DUMP_SM_READING_FLASH;
        }
      }
      break;
    case USB_DUMP_SM_CREATING_FILE:
      if(USB_UPG_RET_SUCCESS == create_dump_file(p_priv, FALSE))
        {
          usb_upg_send_evt_to_ui(USB_DUMP_CREATE_FILE_SUCCESS, 0, 0);
          p_data->burn_end = FALSE;
          p_data->sm = USB_DUMP_SM_WRITING_FILE;
        }
      else
        {
          usb_upg_send_evt_to_ui(USB_DUMP_CREATE_FILE_ERROR, 0, 0);
          p_data->sm = USB_UPG_SM_EXIT;
        }
      break;
    case USB_DUMP_SM_WRITING_FILE:
      if(USB_UPG_RET_SUCCESS == save_dump_file(p_priv, FALSE))
      {
        if(p_data->dnload_end == TRUE)
          {
            usb_upg_send_evt_to_ui(USB_DUMP_WRITE_FILE_SUCCESS, 0, 0);
            usb_upg_update_status(p_priv, USB_UPG_GET_STS(p_priv),100,NULL);
            p_data->sm = USB_UPG_SM_EXIT; 
          }
        else
          {        
            p_data->sm = USB_DUMP_SM_WRITING_FILE;
          }
      }
      else
      {  
        usb_upg_send_evt_to_ui(USB_DUMP_WRITE_FILE_ERROR, 0, 0);        
        p_data->sm = USB_UPG_SM_EXIT; 
      }
      break;  
    case USB_UPG_SM_EXIT:
      p_policy->p_post_exit(&(p_data->cfg));
      p_data->sm = USB_UPG_SM_IDLE;
      break; 
    default:
      break;
  }
}

/*!
  message Pre-process 
 */
static void usb_upg_pre_process_msg(void *handle, os_msg_t *p_msg)
{
  usb_upg_priv_t *p_priv = (usb_upg_priv_t *)handle;

  if(NULL != p_msg)
  {
    switch(p_msg->content)
    {
      case USB_UPG_CMD_EXIT:
        if(p_msg->para1)
        {
          p_priv->usb_upg_data.key = USB_UPGRD_KEY_EXIT;
        }
        else
        {
          p_priv->usb_upg_data.key = USB_UPGRD_KEY_MENU;
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
app_t * construct_ap_usb_upg(usb_upg_policy_t *p_usb_upg_policy)
{
  usb_upg_priv_t *p_usb_upg_priv = mtos_malloc(sizeof(usb_upg_priv_t));
  CHECK_FAIL_RET_NULL(NULL != p_usb_upg_priv);

  //clear private data
  memset(p_usb_upg_priv, 0, sizeof(usb_upg_priv_t));

  //Attach upg policy
  p_usb_upg_priv->p_usb_upg_impl = p_usb_upg_policy;
  
  //Attach scan applition
  p_usb_upg_priv->usb_upg_app.init = usb_upg_init;
  p_usb_upg_priv->usb_upg_app.pre_process_msg = usb_upg_pre_process_msg;
  p_usb_upg_priv->usb_upg_app.task_single_step = usb_upg_single_step;
  p_usb_upg_priv->usb_upg_app.get_msgq_timeout = NULL;
  p_usb_upg_priv->usb_upg_app.p_data = (void *)p_usb_upg_priv;

  return &p_usb_upg_priv->usb_upg_app;
}

