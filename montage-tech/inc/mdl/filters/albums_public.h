/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ALBUMS_PUBLIC_H__
#define __ALBUMS_PUBLIC_H__

/*!
  albums_filter_evt_t
  */
typedef enum
{
  /*!
    ALBUMS_EVT_ALBUM_LIST
    */
  ALBUMS_EVT_ALBUMS_LIST = 0x00000010,
  /*!
    ALBUMS_EVT_PHOTO_LIST
    */
  ALBUMS_EVT_PHOTOS_LIST,
  /*!
    ALBUMS_EVT_TAG_LIST
    */
  ALBUMS_EVT_TAGS_LIST,
  /*!
    ALBUMS_EVT_COMMENTS_LIST
    */
  ALBUMS_EVT_COMMENTS_LIST,
}albums_pub_evt_t;

/*!
  albums access type.
  */
typedef enum
{
  /*!
    all
    */
  ACCESS_ALL,
  /*!
    public
    */
  ACCESS_PUBLIC,
  /*!
    private
    */
  ACCESS_PRIVATE,
  /*!
    visible
    */
  ACCESS_VISIBLE,
}albums_access_t;

/*!
  thumbnail info
  */
typedef struct
{
  /*!
    thumbnail width.
    */
  u16 width;
  /*!
    thumbnail height.
    */
  u16 height;
  /*!
    thumbnail url.
    */
  char *p_thumb_url;
}thumb_t;

/*!
  album info.
  */
typedef struct
{
  /*!
    create time
    */
  utc_time_t create_time;
  /*!
    photo count
    */
  u16 photos_num;
  /*!
    album name
    */
  char *p_album_name;
  /*!
    album id
    */
  char *p_album_id;
  /*!
    user id
    */
  char *p_user_id;
  /*!
    user name
    */
  char *p_user_name;
  /*!
    thumbnail
    */
  thumb_t thumb;
}album_info_t;

/*!
  albums description.
  */
typedef struct
{
  u16 context;
  /*!
    album count.
    */
  u16 album_total;
  /*!
    albums.
    */
  album_info_t *p_album;
}albums_t;

/*!
  photo info.
  */
typedef struct
{
  /*!
    width
    */
  u16 width;
  /*!
    height
    */
  u16 height;
  /*!
    size
    */
  u32 size;
  /*!
    create time
    */
  utc_time_t create_time;
  /*!
    thumbnail count
    */
  u16 thumb_num;
  /*!
    thumbnail
    */
  thumb_t *p_thumb;
  /*!
    photo name.
    */
  char *p_photo_name;
  /*!
    photo id.
    */
  char *p_photo_id;
  /*!
    album id
    */
  char *p_album_id;
  /*!
    user id
    */
  char *p_user_id;
  /*!
    user name
    */
  char *p_user_name;
  /*!
    photo url
    */
  char *p_photo_url;
  /*!
    photo description
    */
  char *p_photo_description;
}photo_info_t;

/*!
  albums description.
  */
typedef struct
{
  u16 context;
  /*!
    photo total.
    */
  u32 photo_total;
  /*!
    count.
    */
  u16 cnt;
  /*!
    the page of results to return. (base on 1)
    */
  u16 page;
  /*!
    max number of photos to return per page.
    */
  u16 per_page;
  /*!
    photos.
    */
  photo_info_t *p_photo;
}photos_t;

/*!
  comment info.
  */
typedef struct
{
  /*!
    comment data.
    */
  char *p_cmmt_data;
  /*!
    user id.
    */
  char *p_user_id;
  /*!
    user nickname.
    */
  char *p_user_name;
  /*!
    create time.
    */
  utc_time_t create_time;
  /*!
    author thumbnail.
    */
  char *p_author_thumb;
  /*!
    comment id.
    */
  char *p_cmmt_id;
  /*!
    photo id.
    */
  char *p_photo_id;
  /*!
    photoset id.
    */
  char *p_album_id;
}cmmt_info_t;

/*!
  comments.
  */
typedef struct
{
  /*!
    comment total
    */
  u16 cmmt_total;
  /*!
    comment count.
    */
  u16 cmmt_cnt;
  /*!
    the page of results to return. (base on 1)
    */
  u16 page;
  /*!
    max number of comments to return per page.
    */
  u16 per_page;
  /*!
    comments.
    */
  cmmt_info_t *p_cmmt;
}cmmts_t;

/*!
  tag info
  */
typedef struct
{
  /*!
    update time.
    */
  utc_time_t update_time;
  /*!
    tag.
    */
  char *p_tag_name;
}tag_info_t;

/*!
  tags
  */
typedef struct
{
  /*!
    comment total
    */
  u16 tag_total;
  /*!
    comment count.
    */
  u16 tag_cnt;
  /*!
    the page of results to return. (base on 1)
    */
  u16 page;
  /*!
    max number of tags to return per page.
    */
  u16 per_page;
  /*!
    comments.
    */
  tag_info_t *p_tag;
}tags_t;
#endif
