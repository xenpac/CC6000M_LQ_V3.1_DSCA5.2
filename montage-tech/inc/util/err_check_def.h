/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ERR_CHECK_DEF_H_
#define __ERR_CHECK_DEF_H_

#define CHECK_FAIL_RET_VOID(condition)                                   \
  do                                                                        \
  {                                                                         \
    if (!(condition))                                                       \
    {                                                                       \
        OS_PRINTF("%s: line %d\n", __FILE__, __LINE__);                     \
        return;                                                             \
    }                                                                       \
  }                                                                         \
  while (0)

#define CHECK_FAIL_RET(condition, ret)                                   \
  do                                                                        \
  {                                                                         \
    if (!(condition))                                                       \
    {                                                                       \
        OS_PRINTF("%s: line %d\n", __FILE__, __LINE__);                     \
        return (ret);                                                       \
    }                                                                       \
  }                                                                         \
  while (0)

#define CHECK_FAIL_RET_ZERO(condition) CHECK_FAIL_RET(condition, 0) 
#define CHECK_FAIL_RET_NULL(condition) CHECK_FAIL_RET(condition, NULL) 
#define CHECK_FAIL_RET_CODE(condition) CHECK_FAIL_RET(condition, ERR_FAILURE) 
#define CHECK_FAIL_RET_FALSE(condition) CHECK_FAIL_RET(condition, FALSE) 
#endif //__ERR_CHECK_DEF_H_ 

