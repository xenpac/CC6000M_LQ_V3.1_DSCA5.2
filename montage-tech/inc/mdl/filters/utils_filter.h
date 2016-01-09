/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UTILS_FILTER_H_
#define __UTILS_FILTER_H_

/*!
  subt filter command define
  */
typedef enum tag_utils_filter_cmd
{
  /*!
    start.
    \param[in] 
    */
  UTILS_CMD_GET_CITY_BY_IP,

}utils_filter_cmd_t;

/*!
  utils filter event define
  */
typedef enum tag_utils_filter_evt
{
  /*!
    parse done
    */
  UTILS_MSG_PARSE_DONE = UTILS_FILTER << 16,
  /*!
    parse ERROR
    */
  UTILS_MSG_PARSE_ERROR,

}utils_filter_evt_t;

/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  BOOL b_city_chinese;
  /*!
  utf-8
  */
  char u8_city[50];
  /*!
  Unistr
  */
  u16 u16_city[50];
  /*!
  parse done
  */
  BOOL b_region_chinese;
  /*!
  utf-8
  */
  char u8_region[50];
  /*!
  Unistr
  */
  u16 u16_region[50];
  /*!
  parse done
  */
  BOOL b_country_chinese;
  /*!
  utf-8
  */
  char u8_country[50];
    /*!
  Unistr
  */
  u16 u16_country[50];
}utils_location_t;


/*!
  subt_filter_create
  */
ifilter_t * utils_filter_create(void *p_para);

#endif