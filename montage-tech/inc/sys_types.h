/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_TYPES_H_
#define __SYS_TYPES_H_

/*!
  Signed integer, weight is 8 bits. In most case it equal to char.
  */
typedef signed char s8;

/*!
  Unsigned integer, weight is 8 bits. In most case it equal to unsigned char.
  */
typedef unsigned char u8;

/*!
  Signed integer, weight is 16 bits. In most case it equal to short.
  */
typedef signed short s16;

/*!
  Unsigned integer, weight is 16 bits. In most case it equal to unsigned short.
  */
typedef unsigned short u16;

/*!
  Signed integer, weight is 32 bits. In most case it equal to long.
  */
typedef signed long s32;

/*!
  Unsigned integer, weight is 32 bits. In most case it equal to unsigned long.
  */
typedef unsigned long u32;
/*!
  Signed integer, weight is 64 bits
  */
typedef signed long long s64;
/*!
  Usigned integer, weight is 64 bits
  */
typedef unsigned long long u64;

/*!
  Boolean type, the value should be FALSE and TRUE. Recommand replace BOOL by
  RET_CODE as return value.
  */
typedef int BOOL;

/*!
  Instance handle type
  */
typedef void * handle_t;

/*!
  Boolean false
  */
#ifndef  FALSE
/*!
  Boolean false
  */
#define FALSE (0)             
#endif

/*!
  Boolean true
  */
#ifndef  TRUE
/*!
  Boolean true
  */
#define TRUE (1)
#endif

/*!
  Invalid value
  */
#define INVALID (~0)

#ifndef NULL
/*!
  Null pointer type
  */
#define NULL (0)
#endif

/*!
  Return code type. Please reference return code macro for the values.
  */
#define RET_CODE s32

/*!
  Success return
  */
#define SUCCESS ((s32)0)       
/*!
  Fail for common reason
  */
#define ERR_FAILURE ((s32)-1)  
/*!
  Fail for waiting timeout
  */     
#define ERR_TIMEOUT ((s32)-2)      
/*!
  Fail for function param invalid
  */ 
#define ERR_PARAM ((s32)-3) 
/*!
  Fail for module status invalid
  */      
#define ERR_STATUS ((s32)-4)  
/*!
  Fail for module busy
  */     
#define ERR_BUSY ((s32)-5)
/*!
  Fail for no enough memory
  */
#define ERR_NO_MEM ((s32)-6)  
/*!
  Fail for no enough resource
  */     
#define ERR_NO_RSRC ((s32)-7)
/*!
  Fail for hardware error
  */ 
#define ERR_HARDWARE ((s32)-8) 
/*!
  Fail for feature not support
  */      
#define ERR_NOFEATURE ((s32)-9)       


/*!
  For unicode fs 
  */
#define UFS_UNICODE 1

/*!
  for unicode char type
  */
typedef u16 tchar_t;
/*!
  for unicode char strings
  */
#define _T(x) L ## x
/*!
  for unicode char strings
  */
#define _TEXT(x) L ## x


#endif
