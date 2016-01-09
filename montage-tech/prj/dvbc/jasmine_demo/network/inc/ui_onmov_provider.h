/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_ONMOV_PROVIDER_H__
#define __UI_ONMOV_PROVIDER_H__



#define MAX_COMM_NAME_LENGTH (32)
#define MAX_DESC_LENGTH (256)
#define MAX_URL_LENGTH (512)

typedef enum
{
  ONMOV_TAB_TYPE_TAB = 0,
  ONMOV_TAB_TYPE_WEBSITE,
  ONMOV_TAB_TYPE_CATEGORY,
  ONMOV_TAB_TYPE_MOVIE
} onmov_tab_type_t;

typedef struct
{ 
  u16 name[MAX_COMM_NAME_LENGTH+1];
  
  u8 picture[MAX_URL_LENGTH+1];
  u16 director[MAX_COMM_NAME_LENGTH+1];
  u16 actor[MAX_COMM_NAME_LENGTH+1];
  u16 description[MAX_DESC_LENGTH+1];
  
  u16 url[MAX_URL_LENGTH+1];
  u16 count;
} onmov_movie_item;

typedef struct
{ 
  u16 name[MAX_COMM_NAME_LENGTH+1];
  u16 movie_count;
  onmov_movie_item *p_movie;
} onmov_cate_item;

typedef struct
{
  u16 name[MAX_COMM_NAME_LENGTH+1];
  u8 picture[MAX_URL_LENGTH+1];
  u16 cate_count;
  onmov_cate_item *p_cate;
} onmov_website_item;



BOOL ui_load_onmov_file(void);

BOOL ui_release_onmov_data(void);

onmov_website_item *omp_get_websites(void);

u16 omp_get_websites_cnt(void);

#endif

