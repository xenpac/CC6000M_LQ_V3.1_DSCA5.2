/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_LIVE_TV_DESKTOP_H__
#define __UI_LIVE_TV_DESKTOP_H__

typedef struct
{
  control_t *p_desktop;
  BOOL write_db_info_done;
  BOOL category_init;
  BOOL chan_zip_init;
  BOOL chan_list_init;
  BOOL up_livetv_timing;
  u8 livetv_init;
  void *p_chan_zip;  
}desk_sts_mch_t;   //desktop state machine 

BOOL ui_get_livetv_db_flag(void);

void ui_set_livetv_db_flag(BOOL is_done);

u8 ui_get_init_livetv_status(void);

void ui_set_init_livetv_status(u8 status);


#endif
