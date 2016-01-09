/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#include "ui_common.h"
#include "pti.h"
#include "dvb_svc.h"
//#include "ap_ota.h"
#include "ui_ota_api_v2.h"

RET_CODE ui_desktop_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE on_open_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 new_root = (u8)(msg & MSG_DATA_MASK);

	return manage_open_menu(new_root, 0, 0);
}

void ui_desktop_init(void)
{
	fw_config_t g_desktop_config =
	{
	  { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },

	  DST_IDLE_TMOUT,
	  RECEIVE_MSG_TMOUT,
	  POST_MSG_TMOUT,

	  ROOT_ID_BACKGROUND,
	  MAX_ROOT_CONT_CNT,
	  MAX_MESSAGE_CNT,
	  MAX_HOST_CNT,
	  RSI_TRANSPARENT,
	};
	
  fw_init(&g_desktop_config,
               NULL,
               ui_desktop_proc,
               ui_menu_manage);

}

void ui_desktop_main(void)
{
  fw_default_mainwin_loop(ROOT_ID_BACKGROUND);
}

static RET_CODE on_ota_upgrade_upgrade(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(fw_find_root_by_id(ROOT_ID_OTA_UPGRADE) == NULL)
  {
	  return manage_open_menu(ROOT_ID_OTA_UPGRADE, 0, 0);
  }

  return SUCCESS;
}


static RET_CODE on_ota_unlock(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  OS_PRINTF("\r\n %s:entry... ", __FUNCTION__);
  if(fw_find_root_by_id(ROOT_ID_OTA_USER_INPUT) == NULL)
  {
    manage_open_menu(ROOT_ID_OTA_USER_INPUT, 0, 0);
  }
  else
  {
    if(fw_find_root_by_id(ROOT_ID_OTA_UPGRADE) != NULL)
    {
      manage_close_menu(ROOT_ID_OTA_UPGRADE, 0, 0);
    }
  }

	return SUCCESS;
}

static RET_CODE on_ota_locked(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(fw_find_root_by_id(ROOT_ID_OTA_USER_INPUT) != NULL)
  {  
	  manage_close_menu(ROOT_ID_OTA_USER_INPUT, 0, 0);
  }
  
  if(fw_find_root_by_id(ROOT_ID_OTA_UPGRADE) == NULL)
  {
    manage_open_menu(ROOT_ID_OTA_UPGRADE, 0, 0);
  }
	return SUCCESS;
}

static RET_CODE on_ota_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  OS_PRINTF("\r\n %s:entry... ", __FUNCTION__);
  if(fw_find_root_by_id(ROOT_ID_OTA_USER_INPUT) == NULL)
  {
    manage_open_menu(ROOT_ID_OTA_USER_INPUT, 0, 0);
  }
  else
  {
    if(fw_find_root_by_id(ROOT_ID_OTA_UPGRADE) != NULL)
    {
      manage_close_menu(ROOT_ID_OTA_UPGRADE, 0, 0);
    }
  }

	return SUCCESS;
}

extern void reback_to_boot(void);



BEGIN_MSGPROC(ui_desktop_proc, cont_class_proc)
  ON_COMMAND(MSG_OTA_UPGRADE, on_ota_upgrade_upgrade)
  ON_COMMAND(MSG_OPEN_MENU_IN_TAB, on_open_menu)
  ON_COMMAND(MSG_OTA_UNLOCK, on_ota_unlock)
  ON_COMMAND(MSG_OTA_LOCK, on_ota_locked)  
  //ON_COMMAND(MSG_OTA_TABLE_TIMEOUT, on_ota_loader_monitor_end)
  //ON_COMMAND(MSG_OTA_LOADER_MON_END, on_ota_loader_monitor_end)
  ON_COMMAND(MSG_OTA_TMOUT, on_ota_timeout)
  ON_COMMAND(MSG_OTA_FOUND, on_ota_locked)
END_MSGPROC(ui_desktop_proc, cont_class_proc);
