/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_PING_TEST_H__
#define __UI_PING_TEST_H__

#define PING_TEST_ITEM_CNT 3
#define PING_TEST__URL_TYPE_CNT 2
#define PING_TEST__DLG_ITEM_CNT 4

#define PING_TIMES_MIN 1
#define PING_TIMES_MAX 100

typedef struct
{ 
  int len;
  int  ok_times;
} ping_test_result_t;

typedef enum 
{
  PING_TYPE_NUMBER,
  PINGT_TYPE_URL,
}ping_addr_type_t;

RET_CODE open_ping_test(u32 para1, u32 para2);


#endif
