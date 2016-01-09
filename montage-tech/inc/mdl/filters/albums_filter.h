/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __ALBUMS_FILTER_H__
#define __ALBUMS_FILTER_H__

/*!
   Define the type of process function for a message
  */
typedef void (*albums_request_sample_t)(handle_t _this, void *p_url, u32 context);

/*!
  albums dbg handle
  */
#define ALBUMS_DBG   OS_PRINTF

/*!
  albums id length  
  */
#define ALBUMS_ID_LEN 32

/*!
  albums filter command define
  */
typedef enum
{
  /*!
    albums set site.
    */
  ALBUMS_SET_SITE,
  /*!
    get albums list.
    */
  ALBUMS_GET_ALBUMS_LIST,
  /*!
    create new albums.
    */
  //ALBUMS_CREATE_ALBUM,
  /*!
    modify album properity.
    */
  //ALBUMS_MODIFY_ALBUM,
  /*!
    del album.
    */
  //ALBUMS_DEL_ALBUM,
  /*!
    get photo list.
    */
  ALBUMS_GET_PHOTOS_LIST,
  /*!
    get a photo.
    */
  //ALBUMS_GET_PHOTO,
  /*!
    post a new photo.
    */
  //ALBUMS_POST_PHOTO,
  /*!
    post a new video.
    */
  //ALBUMS_POST_VIDEO,
  /*!
    update photo.
    */
  //ALBUMS_UPDATE_PHOTO,
  /*!
    del photo.
    */
  //ALBUMS_DEL_PHOTO,
  /*!
    get tag list.
    */
  ALBUMS_GET_TAGS_LIST,
  /*!
    search photo by tags.
    */
  //ALBUMS_SEARCH_PHOTO_BY_TAGS,
  /*!
    add tag to a photo.
    */
  //ALBUMS_ADD_TAG,
  /*!
    del tag from photo.
    */
  //ALBUMS_DEL_TAG,
  /*!
    get comments list
    */
  ALBUMS_GET_COMMENTS_LIST,
  /*!
    add comments to a photo.
    */
  //ALBUMS_ADD_COMMENTS,
  /*!
    del comment from a photo.
    */
  //ALBUMS_DEL_COMMENTS,
  /*!
    cancel request HTTP.
    */
  ALBUMS_CANCEL,
}albums_filter_cmd_t;


/*!
  albums_site_t
  */
typedef enum
{
  /*!
    pisaca
    */
  ALBUMS_PICASA,
  /*!
    flickr
    */
  ALBUMS_FLICKR,
  /*!
    yupoo
    */
  ALBUMS_YUPOO,
}albums_site_t;

/*!
  photo request type.
  */
typedef enum
{
  /*!
    by album.
    */
  ALBUM_BY_USER_NAME,
  /*!
    by recently upload.
    */
  ALBUM_BY_USER_ID,
}albums_request_t;

/*!
  albums_req_albums_t
  */
typedef struct
{
  /*!
    albums request type.
    */
  albums_request_t request_type;
  /*!
    user id.
    */
  u8 user_id[ALBUMS_ID_LEN];
  /*!
    user name.
    */
  u8 user_name[ALBUMS_ID_LEN];
}albums_req_albums_t;

/*!
  photo request type.
  */
typedef enum
{
  /*!
    by album.
    */
  PHOTO_BY_ALBUM,
  /*!
    by recently upload.
    */
  PHOTO_BY_RECENTLY,
  /*!
    by community search.
    */
  PHOTO_BY_SEARCH,
  /*!
    by feature.
    */
  PHOTO_BY_FEATURE,
}photo_request_t;

/*!
  albums_filter_photo_list_t
  */
typedef struct
{
  /*!
    request type.
    */
  photo_request_t request_type;
  /*!
    user id.
    */
  u8 user_id[ALBUMS_ID_LEN];
  /*!
    user name.(just for album type)
    */
  u8 album_id[ALBUMS_ID_LEN]; 
  /*!
    community.(just for community search)
    */
  u8 community[ALBUMS_ID_LEN];
  /*!
    the page of results to return. (base on 1)
    */
  u16 page;
  /*!
    max number of photos to return per page.
    */
  u16 per_page;
}albums_req_photos_t;

/*!
  tag request type.
  */
typedef enum
{
  /*!
    by user id.
    */
  TAG_BY_USER,
  /*!
    by album id.
    */
  TAG_BY_ALBUM,
  /*!
    by photo id.
    */
  TAG_BY_PHOTO,
}tag_request_t;

/*!
  albums_filter_tag_list_t
  */
typedef struct
{
  /*!
    request type.
    */
  tag_request_t request_type;
  /*!
    user id.(for search by user, by album & by photo.)
    */
  u8 user_id[ALBUMS_ID_LEN];
  /*!
    album id.(for serach by album & by photo.)
    */
  u8 album_id[ALBUMS_ID_LEN]; 
  /*!
    photo id (for search by photo).
    */
  u8 photo_id[ALBUMS_ID_LEN];
  /*!
    the page of results to return. (base on 1)
    */
  u16 page;
  /*!
    max number of tags to return per page.
    */
  u16 per_page;
}albums_req_tags_t;

/*!
  comments request type.
  */
typedef enum
{
  /*!
    by user id.
    */
  COMMENT_BY_ALBUM,
  /*!
    by album id.
    */
  COMMENT_BY_PHOTO,
}comments_request_t;

/*!
  albums_filter_comments_list_t
  */
typedef struct
{
  /*!
    request type.
    */
  comments_request_t request_type;
  /*!
    user id.(for search by user & by photo.)
    */
  u8 user_id[ALBUMS_ID_LEN];
  /*!
    album id.(for serach by photo.)
    */
  u8 album_id[ALBUMS_ID_LEN];
  /*!
    photo id (for search by photo).
    */
  u8 photo_id[ALBUMS_ID_LEN];
  /*!
    the page of results to return. (base on 1)
    */
  u16 page;
  /*!
    max number of comments to return per page.
    */
  u16 per_page;
}albums_req_comments_t;

/*!
  albums_filter_create
  */
ifilter_t * albums_filter_create(void *p_para);

#endif

