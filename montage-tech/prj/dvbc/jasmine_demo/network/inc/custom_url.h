/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __CUSTOM_URL_H__
#define __CUSTOM_URL_H__

#define CUS_NAME_LEN (64)

#define CUS_URL_LEN (512)

#define MAX_CUSTOM_URL (1024)

typedef struct
{
  u8 name[CUS_NAME_LEN];
  u8 url[CUS_URL_LEN];
}custom_url_t;

/*
 * return value: url count, 0 means no url
 */
u16 load_custom_url(custom_url_t** pp_url_array,u8 *buf,custom_url_t *customs_url_tbl);

u32 read_custom_url(u8 *buf, u32 buf_size);

void save_custom_url(u8* p_data, u32 len);

#endif

