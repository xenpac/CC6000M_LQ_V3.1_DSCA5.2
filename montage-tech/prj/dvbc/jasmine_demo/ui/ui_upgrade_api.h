/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef _UI_UPGRADE_API_H
#define _UI_UPGRADE_API_H

enum upg_msg
{
  MSG_UPG_UPDATE_STATUS = MSG_EXTERN_BEGIN + 400,
  MSG_UPG_QUIT,
  MSG_UPG_OK,
  MSG_UPG_EXIT,
  MSG_UPG_FAIL,
  MSG_DUMP_READ_FLASH_OK,
  MSG_DUMP_FAIL,
};

void ui_init_upg(app_id_t app_id, u8 root_id,ap_evtmap_t p_evtmap);

void ui_release_upg(app_id_t app_id, u8 root_id);

void ui_start_upgrade(upg_mode_t md, u8 block_num, upg_block_t *block);

void ui_exit_upgrade(app_id_t app_id, BOOL is_exit_all);

#endif
