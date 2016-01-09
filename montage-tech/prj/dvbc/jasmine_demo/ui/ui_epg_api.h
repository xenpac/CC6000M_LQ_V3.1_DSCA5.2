/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _UI_EPG_API_H
#define _UI_EPG_API_H

enum epg_msg
{
  MSG_SCHE_READY = MSG_EXTERN_BEGIN + 50,
  MSG_STOP_EPG,
  MSG_PF_READY,
  MSG_EPG_DEL,
  MSG_EPG_CHANG_TABLE_POLICY,
  MSG_TDT_FOUND,
};

#define MAX_EXT_TXT_BUF_LEN     ((4)*(KBYTES))

#define EPG_DESC_MAX_NAME_LENGTH    256

void ui_epg_init(void);

void ui_epg_release(void);

void ui_epg_start(epg_table_selecte_policy selecte);

void ui_epg_stop(void);


void ui_epg_set_program_info(u16 pg_id);
#endif

