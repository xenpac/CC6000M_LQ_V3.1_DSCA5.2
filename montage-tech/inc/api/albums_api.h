/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ALBUMS_API_H_
#define __ALBUMS_API_H_

/*!
  albums_call_back
  */
typedef void(*albums_call_back)(albums_pub_evt_t event, void *p_data, u16 context);

/*!
  mul_albums_attr_t
  */
typedef struct 
{
  /*!
    stack size.
    */
  u32 stk_size;
  /*!
    site
    */
  albums_site_t site;
  /*!
    cb see vod_call_back
    */
  albums_call_back cb;
}mul_albums_attr_t;

/*!
  albums api init.
  */
RET_CODE mul_albums_init(void);

/*!
  mul_vod_chn_create
  */
RET_CODE mul_albums_chn_create(mul_albums_attr_t *p_attr);

/*!
  mul_vod_chn_destory
  */
RET_CODE mul_albums_chn_destroy(u32 chanid);

/*!
  mul_vod_start
  */
RET_CODE mul_albums_start(void);

/*!
  mul_vod_stop
  */
RET_CODE mul_albums_stop(void);

/*!
  mul_albums_get_albums
  */
RET_CODE mul_albums_get_albums(albums_req_albums_t *p_albums, u16 context);

/*!
  mul_albums_get_photos
  */
RET_CODE mul_albums_get_photos(albums_req_photos_t *p_photo, u16 context);

/*!
  mul_albums_get_tags
  */
RET_CODE mul_albums_get_tags(albums_req_tags_t *p_tags, u16 context);

/*!
  mul_albums_get_comments
  */
RET_CODE mul_albums_get_comments(albums_req_comments_t *p_comments, u16 context);

/*!
  cancel google map last request
  */
RET_CODE mul_albums_cancel(void);

#endif
