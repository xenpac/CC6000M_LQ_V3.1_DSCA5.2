/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_DLNA_API_H__
#define __UI_DLNA_API_H__

#define DLNA_URL_LEN   (2048)



typedef struct
{ 
  char *playSpeed;
  char *cur_media_duration;
  char *cur_track_duration;
  int *track_num;
  int *cur_track;
} dmr_play_para_t;


#define DLNA_N_ERROR  0
#define DLNA_Y_ERROR (-1)

void dlna_init(void);
int ui_dlna_start(void);
int ui_dlna_stop(void);

BOOL ui_get_dlna_init_status(void);

void set_dlna_promotion(BOOL enable);

void ui_cg_dlna_server_stop(void);
void ui_stop_mini_httpd(void);
#endif
