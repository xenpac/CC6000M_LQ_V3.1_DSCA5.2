/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_DEF_H__
#define __SYS_DEF_H__

/*!
  Routine Parameters Property Dummy indicators (for source review only)
  */
/*!
  Param as Input
  */
#define IN          
/*!
  Param as Output
  */
#define OUT          
/*!
  Param both as I/O
  */
#define INOUT          
/*!
  Routine type property
  */
#define CALLBACK        

/*!
  Basic type defines
  */
#define U8        unsigned char
/*!
  Basic type defines
  */
#define U16        unsigned short
/*!
  Basic type defines
  */
#define U32        unsigned long

/*!
  Basic type defines
  */
#define S8        char
/*!
  Basic type defines
  */
#define S16        short
/*!
  Basic type defines
  */
#define S32        int


/*!
  comments
  */
#define SYS_MAX(x, y)        ((x > y) ? x : y)
/*!
  comments
  */
#define SYS_MIN(x, y)        ((x < y) ? x : y)

/*!
  comments
  */
#define ERRORCODE_T   U32

/*!
  comments
  */
#define NO_ERROR     0
/*!
  comments
  */
#define ERROR_PARM     1
/*!
  comments
  */
#define ERROR_RESULT   2

/*!
  comments
  */
#define BOOL  BOOLEAN

/*!
  comments
  */
#define TRUE  1
      
/*!
  comments
  */
#define FALSE  0

#endif //__SYS_DEF_H__

