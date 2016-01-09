/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifndef __DB_DVBS_H_
#define __DB_DVBS_H_

/*!
 \file db_dvbs.h

   This file defined data structures for Satellite, TP and Program management.
   And interfaces for high level modules and some middle-ware modules. Module
   DB is depend on OS, flash driver.

   Development policy:
   Modules who will call DB's interfaces should try it best to do data
   modifcation in memory first and reduce calling function \e DB_DVBS_WriteData
   times. This policy can protect flash and software will get more efficiency.
 */
#define INVALIDID                    DB_UNKNOWN_ID
#define INVALIDPOS                   DB_UNKNOWN_ID

/*!
   Max number of programs in flash and view depth of available tp node
 */
#define DB_DVBS_MAX_PRO              5000

/*!
   view depth of available tp node and Max number of TP node in Flash.
 */
#define DB_DVBS_MAX_TP               3000

/*!
   name length for Satellite and program
 */
#define DB_DVBS_MAX_NAME_LENGTH      16

#define DB_DVBS_MAX_AUDIO_CHANNEL   (16)

#define MAX_FAV_GRP                 (32)

#define DB_DVBS_PARAM_ACTIVE_FLAG   (0x8000)

#define MAX_VIEW_BUFFER_SIZE        (sizeof(item_type_t) * DB_DVBS_MAX_PRO)//(200*KBYTES)//
/*!
  Length of "no name"
  */
#define LEN_OF_NO_NAME              (8)
/*!
   Module return types
 */
typedef enum
{
  /*!
     DB error code. Means operation successed.
   */
  DB_DVBS_OK = 0,
  /*!
     DB error code. Means operation failed.
   */
  DB_DVBS_FAILED,
  /*!
     DB error code. Means database was full or already create 3 views.
   */
  DB_DVBS_FULL,
  /*!
     DB error code. Means the given data was duplicated with the data already
     existing in database.
   */
  DB_DVBS_DUPLICATED,
  /*!
     DB error code. Means database can't get a valid node from the given index.
   */
  DB_DVBS_NOT_FOUND
} db_dvbs_ret_t;


typedef enum
{
 /*! mark bit declares locked.*/
 DB_DVBS_MARK_SEL = 0,
 DB_DVBS_MARK_DEL = 1
}dvbs_view_mark_digit;

#define MAX_TABLE_CACHE_SIZE (12) // 12bytes at structure head
#define MAX_VIEW_CACHE_SIZE  (sizeof(item_type_t))

#endif // End for __DB_DVBS_H

