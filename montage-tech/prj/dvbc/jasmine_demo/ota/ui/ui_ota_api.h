/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 ****************************************************************************/
#ifndef __UI_OTA_API_H__
#define __UI_OTA_API_H__

enum ota_msg
{
  MSG_OTA_PROGRESS = MSG_EXTERN_BEGIN + 200, //0x60c8
  MSG_OTA_LOCK,
  MSG_OTA_UNLOCK,
  MSG_OTA_START_DL,
  MSG_OTA_CHECKING,
  MSG_OTA_BURNING,
  MSG_OTA_FINISH,
  MSG_OTA_UPGRADE,
  MSG_OTA_UNZIP_OK,
  MSG_OTA_TABLE_TIMEOUT,
  MSG_OTA_LOADER_MON_END, //OTA loader monitor check end
  MSG_OTA_FOUND,
};

void ui_ota_relock(ota_info_t *p_info);
  
void ui_enable_ota(BOOL is_enable);

void ui_ota_start(u32 para);

void ui_ota_burn_flash(BOOL is_burn);

void ui_enable_uio(BOOL is_enable);
#endif
