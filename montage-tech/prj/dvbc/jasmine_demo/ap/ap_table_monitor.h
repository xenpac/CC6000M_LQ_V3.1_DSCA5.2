/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __AP_TABLE_MONITOR_H_
#define __AP_TABLE_MONITOR_H_

/*!
  Application table monitor evt enum
  */
typedef enum
{
  /*!
    file play end
    */
  TM_EVT_DRAW_END = ((APP_RESERVED2 << 16)|ASYNC_EVT),
  /*!
    picture play start
    */
  TM_EVT_START = ((APP_RESERVED2 << 16)|SYNC_EVT),
  /*!
    picture play stoped
    */
  TM_BAT_STOPED,
  /*!
    picture play stoped
    */
  TM_BAT_ABLED,
  /*!
    picture play stoped
    */
  TM_SDT_OTR_STOPED,
  /*!
    picture play stoped
    */
  TM_SDT_OTR_ABLED,
  /*!
    sdt running status
    */
  TM_SDT_RUN_STATUS,
}tm_evt_t;

typedef enum
{
  /*!
    player start
    */
  TM_CMD_START,
  /*!
    player stop
    */
  TM_CMD_STOP_BAT,
  /*!
    player stop
    */
  TM_CMD_ABLE_BAT,
  /*!
    player sync start
    */
  TM_CMD_STOP_SDT_OTR,
  /*!
    player stop
    */
  TM_CMD_ABLE_SDT_OTR,
  /*!
    player sync start
    */
  TM_CMD_SYNC_START = SYNC_CMD,
  /*!
    player sync stop
    */
  TM_CMD_SYNC_STOP_BAT,
  /*!
    player stop
    */
  TM_CMD_SYNC_ABLE_BAT,
  /*!
    player sync stop
    */
  TM_CMD_SYNC_STOP_SDT_OTR,
  /*!
    player stop
    */
  TM_CMD_SYNC_ABLE_SDT_OTR,
  /*!
    player stop
    */
  TM_CMD_START_SCAN,
  
}tm_cmd_t;

typedef enum
{
  MSG_TM_RUN_STATUS = 0x6000 + 1320,//MSG_EXTERN_BEGIN + 1320
}tm_msg_t;

#define MAX_PG_PRE_TP          16;
app_t *construct_ap_table_monitor();

void ui_table_monitor_stop_bat(BOOL b_able, BOOL b_sync);
void ui_table_monitor_able_bat(BOOL b_able, BOOL b_sync);
void ui_table_monitor_stop_sdt_otr(BOOL b_able, BOOL b_sync);
void ui_table_monitor_able_sdt_otr(BOOL b_able, BOOL b_sync);
void ui_table_monitor_init(void);
void ui_table_monitor_release(void);

#endif // End for __AP_TABLE_MONITOR_H_

