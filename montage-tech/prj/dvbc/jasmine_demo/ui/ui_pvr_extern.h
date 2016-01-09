/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_PVR_EXTERN_H_
#define _UI_PVR_EXTERN_H_

#if ENABLE_MUSIC_PICTURE
typedef enum tag_ui_pvr_extern_mode
{
  UI_PVR_EXTERN_READ = 0,
  UI_PVR_EXTERN_WRITE,
  UI_PVR_EXTERN_APPEND_RW,
}ui_pvr_extern_mode_t;

typedef enum tag_ui_pvr_extern_type
{
  UI_PVR_FINGER_PRINT = 0,
  UI_PVR_ECM_FINGER_PRINT,
  UI_PVR_HIDE_ECM_FINGER_PRINT,
  UI_PVR_OSD,
  UI_PVR_OSD_HIDE,
  UI_PVR_SUPER_OSD_HIDE,
}ui_pvr_extern_type_t;

typedef struct tag_ui_pvr_extern
{
  u32 rec_time;/*second*/
  ui_pvr_extern_type_t type;
  u32 extern_data_len;
  u8* p_extern_data;
}ui_pvr_extern_t;

RET_CODE ui_pvr_extern_open(u16 *p_rec_file, ui_pvr_extern_mode_t mode);
RET_CODE ui_pvr_extern_write(ui_pvr_extern_t *p_pvr_extern);
ui_pvr_extern_t *ui_pvr_extern_read(u32 rec_time, u8 index);
u8 ui_pvr_extern_get_msg_num(u32 rec_time);
void ui_pvr_extern_close(void);
void ui_pvr_extern_rename(u16 *p_rec_file, u16 *p_new_rec);
#endif

#endif

//end of file

