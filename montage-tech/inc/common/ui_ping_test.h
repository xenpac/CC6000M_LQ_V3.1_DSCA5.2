/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_PING_TEST_H__
#define __UI_PING_TEST_H__
/*!
  fix me
  */
#define PING_TEST_ITEM_CNT 4
#define PING_TEST_URL_TYPE_CNT 2
#define PING_TEST_DLG_ITEM_CNT 4
/*!
  fix me
  */
#define PING_TIMES_MIN 1
/*!
  fix me
  */
#define PING_TIMES_MAX 100

/*!
  fix me
  */
typedef struct
{ 
/*!
  fix me
  */
  int len;
/*!
  fix me
  */
  int  ok_times;
/*!
  fix me
  */
  int  bad_times;
} ping_test_result_t;

RET_CODE open_ping_test(u32 para1, u32 para2);
#endif
