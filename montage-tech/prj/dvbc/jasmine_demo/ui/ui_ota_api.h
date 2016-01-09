/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _UI_OTA_API_H
#define _UI_OTA_API_H

enum ota_msg
{
  MSG_OTA_FOUND = MSG_EXTERN_BEGIN + 150,
  MSG_OTA_TRIGGER_RESET,
  MSG_OTA_TMOUT,  
  MSG_TS_OTA_STATUS,
};


void ui_ota_api_init(void);
void ui_ota_api_stop(u32 para1, u32 para2);
void ui_ota_api_release(void);
void ui_ota_api_start_search(u32 para1, u32 para2);
void ui_ota_api_stop_search(u32 para1, u32 para2);

#endif
