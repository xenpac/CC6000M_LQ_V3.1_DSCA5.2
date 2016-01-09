/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NET_MUSIC_FILTER_H_
#define __NET_MUSIC_FILTER_H_
/*!
  net_music filter event define
  */
//#define NET_MUSIC_GET_ALBUMS 0x0020
/*!
  net_music filter event define
  */
//#define NET_MUSIC_GET_TRACKS 0x0030
/*!
  net_music filter event define
  */
typedef enum tag_net_music_filter_evt
{
  /*!
    parse done
    */
  NET_MUSIC_MSG_PARSE_DONE = NET_MUSIC_FILTER << 16, //NET_MUSIC_FILTER << 16,
  /*!
    parse ERROR
    */
  NET_MUSIC_MSG_PARSE_ERROR,

}net_music_filter_evt_t;


/*!
  net_music filter command define
  */
typedef enum
{
  /*!
    net_music set site.
    */
  NET_MUSIC_CONFIG_WEB_CMD = 0,    
  /*!
    get net_music info.
    */
  NET_MUSIC_GET_INFO_CMD,
}net_music_filter_cmd_t;

/*!
net music type
*/
typedef enum
{
  /*!
  albums
  */
  TYPE_ALBUMS = 0x0020,
    /*!
  artists
  */
  TYPE_ARTISTS,
  /*!
  tracks
  */
  TYPE_TRACKS,
}net_music_type_t;
/*!
  get net_music info.
  */
typedef enum
{
  /*!
  get net_music info.
  */
  NET_MUSIC_JAMENDO = 0,
}net_music_site_t;


/*!
  image size
  */
typedef enum
{
  /*!
  25
  */
  IMG_SIZE_25 = 25, 
  /*!
  35
  */
  IMG_SIZE_35 = 35, 
  /*!
  50
  */
  IMG_SIZE_50 = 50, 
  /*!
  55
  */
  IMG_SIZE_55 = 55, 
  /*!
  60
  */
  IMG_SIZE_60 = 60, 
  /*!
  70
  */
  IMG_SIZE_70 = 70, 
  /*!
  85
  */
  IMG_SIZE_85 = 85, 
  /*!
  100
  */
  IMG_SIZE_100 = 100, 
  /*!
  130
  */
  IMG_SIZE_130 = 130, 
  /*!
  150
  */
  IMG_SIZE_150 = 150, 
  /*!
  200
  */
  IMG_SIZE_200 = 200, 
  /*!
  300
  */
  IMG_SIZE_300 = 300, 
  /*!
  400
  */
  IMG_SIZE_400 = 400, 
  /*!
  500
  */
  IMG_SIZE_500 = 500,
  /*!
  600
  */
  IMG_SIZE_600 = 600,  
}net_music_imagesize_t;


/*!
  news_type
  */
typedef enum
{
  /*!
  NAME
  */
  ALBUMS_NAME = 0,
  /*!
  ID
  */
  ALBUMS_ID,
  /*!
  RELEASEDATE
  */
  ALBUMS_RELEASEDATE, 
  /*!
  ARTIST_ID
  */
  ALBUMS_ARTIST_ID, 
  /*!
  ARTIST_NAME
  */
  ALBUMS_ARTIST_NAME, 
  /*!
  POPULARITY_TOTAL
  */
  ALBUMS_POPULARITY_TOTAL, 
  /*!
  POPULARITY_MONTH
  */
  ALBUMS_POPULARITY_MONTH, 
  /*!
  POPULARITY_WEEK
  */
  ALBUMS_POPULARITY_WEEK,

}net_music_albums_order_t;

/*!
  news_type
  */
typedef enum
{
  /*!
  NAME
  */
  ARTISTS_NAME = 20,
  /*!
  ID
  */
  ARTISTS_ID,
  /*!
  RELEASEDATE
  */
  ARTISTS_RELEASEDATE, 
  /*!
  ARTIST_ID
  */
  ARTISTS_ARTIST_ID, 
  /*!
  ARTIST_NAME
  */
  ARTISTS_ARTIST_NAME, 
  /*!
  POPULARITY_TOTAL
  */
  ARTISTS_POPULARITY_TOTAL, 
  /*!
  POPULARITY_MONTH
  */
  ARTISTS_POPULARITY_MONTH, 
  /*!
  POPULARITY_WEEK
  */
  ARTISTS_POPULARITY_WEEK,
    /*!
  POPULARITY_TOTAL
  */
  ARTISTS_TRACK_ID, 
  /*!
  POPULARITY_MONTH
  */
  ARTISTS_TRACK_NAME, 
  /*!
  POPULARITY_WEEK
  */
  ARTISTS_TRACK_POSITION,

}net_music_artists_order_t;
/*!
net_music set site.
*/
typedef enum
{
  /*!
  net_music set site.
  */
  TRACKS_BUZZRATE = 50, 
  /*!
  net_music set site.
  */
  TRACKS_DOWNLOADS_WEEK, 
  /*!
  net_music set site.
  */
  TRACKS_DOWNLOADS_MONTH, 
   /*!
  net_music set site.
  */
  TRACKS_DOWNLOADS_TOTAL, 
  /*!
  net_music set site.
  */
  TRACKS_LISTENS_WEEK, 
  /*!
  net_music set site.
  */
  TRACKS_LISTENS_MONTH, 
  /*!
  net_music set site.
  */
  TRACKS_LISTENS_TOTAL, 
  /*!
  net_music set site.
  */
  TRACKS_POPULARITY_WEEK, 
  /*!
  net_music set site.
  */
  TRACKS_POPULARITY_MONTH, 
  /*!
  net_music set site.
  */
  TRACKS_POPULARITY_TOTAL, 
  /*!
  net_music set site.
  */
  TRACKS_NAME, 
  /*!
  net_music set site.
  */
  TRACKS_ALBUM_NAME, 
  /*!
  net_music set site.
  */
  TRACKS_ARTIST_NAME, 
  /*!
  net_music set site.
  */
  TRACKS_RELEASEDATE, 
  /*!
  net_music set site.
  */
  TRACKS_DURATION, 
  /*!
  net_music set site.
  */
  TRACKS_ID,
}net_music_tracks_order_t;

/*!
net_music set site.
*/
typedef enum
{
  /*!
  net_music set site.
  */
  A_INVAILD = 0,
  /*!
  net_music set site.
  */
  ACOUSTIC, 
  /*!
  net_music set site.
  */
  ELECTRIC, 
}track_acousticelectric_t;

/*!
net_music set site.
*/
typedef enum
{
  /*!
  net_music set site.
  */
  V_INVAILD = 0,
  /*!
  net_music set site.
  */
  VOCAL = 5, 
  /*!
  net_music set site.
  */
  INSTRUMENTAL, 
}track_vocalinstrumental_t;

/*!
net_music set site.
*/
typedef enum
{
  /*!
  net_music set site.
  */
  G_INVAILD = 0,
  /*!
  net_music set site.
  */
  MALE = 10, 
  /*!
  net_music set site.
  */
  FEMALE, 
}track_gender_t;

  /*!
  net_music set site.
  */
typedef enum
{
  /*!
  net_music set site.
  */
  E_INVAILD = 0,
  /*!
  net_music set site.
  */
  ALBUMSTOTRACK, 
  /*!
  net_music set site.
  */
  ARTISTSTOTRACK, 
}track_enter_t;
  
/*!
  albums_filter_photo_list_t
  */
typedef struct
{
  /*!
  music type.
  */
  net_music_type_t music_type;
  /*!
  Limit default is 10
  */
  int limit;
  /*!
  albums order
  */
  net_music_tracks_order_t tracks_order;
  /*!
  albums order
  */
  net_music_albums_order_t albums_order;
  /*!
  albums tracks order
  */
  net_music_artists_order_t artists_order;
  /*!
  Image Size
  */
  net_music_imagesize_t music_img_size;
  /*!
  net_music set site.
  */
  track_enter_t track_enter;
  //BOOL b_acousticelectric;
  /*!
  net_music set site.
  */
  track_acousticelectric_t ac_value;
  //BOOL b_vocalinstrumental;
  /*!
  net_music set site.
  */
  track_vocalinstrumental_t vo_value;
  //BOOL b_gender;
  /*!
  net_music set site.
  */
  track_gender_t gender_value;
  /*!
  net_music set site.
  */
  BOOL b_prolicensing;
  /*!
  net_music set site.
  */
  BOOL b_probackground;
  /*!
  net_music set site.
  */
  BOOL b_ccsa;
  /*!
  net_music set site.
  */
  BOOL b_ccnd;
  /*!
  net_music set site.
  */
  BOOL b_ccnc;
  /*!
  net_music set site.
  */  
  BOOL b_search;
  /*!
  net_music set site.
  */
  char key_word[50];
  /*!
  index_id
  */
  int index_id;
}net_music_param_t;

/*!
net_music set site.
*/
typedef struct
{
  /*!
  net_music set site.
  */
  u32 album_id;
  /*!
  net_music set site.
  */
  u32 duration;
  /*!
  net_music set site.
  */
  u32 id;
  /*!
  net_music set site.
  */
  u32 artist_id;
  /*!
  net_music set site.
  */
  char artist_name[30];
  /*!
  net_music set site.
  */
  u32 position;
  /*!
  net_music set site.
  */
  char album_name[30];
  /*!
  net_music set site.
  */
  char name[30];
  /*!
  net_music set site.
  */
  char releasedate[11];
  /*!
  net_music set site.
  */
  char album_image[512];
  /*!
  net_music set site.
  */
  char audio[512];
  /*!
  net_music set site.
  */
  //char prourl[512];
}net_music_tracks_t;

/*!
net_music set site.
*/
typedef struct
{
  /*!
  net_music set site.
  */
  char status[10];
  /*!
  net_music set site.
  */
  u32 code;
  /*!
  net_music set site.
  */
  u32 tracks_count;
  /*!
  net_music set site.
  */
  net_music_tracks_t tracks_content[100];
}net_music_tracks_content_t;
  
/*!
net_music set site.
*/
typedef struct
{
  /*!
  net_music set site.
  */
  u32 id;
  /*!
  net_music set site.
  */
  char name[30];
  /*!
  net_music set site.
  */
  char releasedate[11];
  /*!
  net_music set site.
  */
  u32 artist_id;
  /*!
  net_music set site.
  */
  char artist_name[30];
  /*!
  net_music set site.
  */
  char img_url[512];
  /*!
  net_music set site.
  */
  //char zip_url[512];
  /*!
  net_music set site.
  */
  //u32 tracks_count;
  /*!
  net_music set site.
  */
  //net_music_tracks_t tracks[50];
}net_music_albums_t;
  
/*!
net_music set site.
*/
typedef struct
{
  /*!
  net_music set site.
  */
  char status[10];
  /*!
  net_music set site.
  */
  u32 code;
  /*!
  music type.
  */
  u32 albums_count;
  /*!
  albums order
  */
  net_music_albums_t albums_content[100];
  
}net_music_albums_content_t;
  
/*!
net_music set site.
*/
typedef struct
{
  /*!
  net_music set site.
  */
  u32 id;
   /*!
  net_music set site.
  */
  char name[30];
  /*!
  net_music set site.
  */
  char joindate[11];
  /*!
  net_music set site.
  */
  char img_url[512];
  /*!
  net_music set site.
  */
  //u32 tracks_count;
  /*!
  net_music set site.
  */
  //net_music_tracks_t tracks[50];
}net_music_artists_t;
  
/*!
net_music set site.
*/
typedef struct
{
  /*!
  net_music set site.
  */
  char status[10];
  /*!
  net_music set site.
  */
  u32 code;
  /*!
  music type.
  */
  u32 artists_count;
  /*!
  albums order
  */
  net_music_artists_t artists_content[100];
  
}net_music_artists_content_t;
/*!
net_music set site.
*/
typedef struct
{
  /*!
net_music set site.
*/
  net_music_tracks_content_t tracks_content;
/*!
net_music set site.
*/
  net_music_albums_content_t albums_content;
/*!
net_music set site.
*/
  net_music_artists_content_t artists_content;
}net_music_data_base_t;

/*!
  subt_filter_create
  */
ifilter_t * net_music_filter_create(void *p_para);

#endif
