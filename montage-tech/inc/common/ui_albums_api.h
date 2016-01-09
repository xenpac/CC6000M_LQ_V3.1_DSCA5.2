/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_ALBUMS_API_H__
#define __UI_ALBUMS_API_H__

typedef enum 
{
  MSG_ALBUMS_EVT_FAILED = MSG_EXTERN_BEGIN + 1330,
  
  MSG_ALBUMS_EVT_SUCCESS,
}albums_msg_t;

void ui_albums_init(albums_site_t site);

void ui_albums_release(void);

RET_CODE ui_albums_get_albums(albums_req_albums_t *p_albums);

RET_CODE ui_albums_get_photos(albums_req_photos_t *p_photos);

RET_CODE ui_albums_get_tags(albums_req_tags_t *p_tags);

RET_CODE ui_albums_get_comments(albums_req_comments_t *p_comments);

photos_t *ui_albums_get_photos_list(void);

albums_t *ui_albums_get_albums_list(void);

#endif
