/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_BASE16_H_
#define __DATA_BASE16_H_

/*!
  \file data_base16.h

  This file provides the API to get the proc handle of data base 16.
  In this version database, basic item type in database is u16, the 
  max mark bits supported is 5 and the max virtual view supported is 3
  */

/*!
  Max mask bits from bit11 to 14 supported in this version
  */
#define MAX_MARK_NUM  (5)

/*!
  Max virtual view number
  */
#define MAX_VIRTUAL_VIEW_NUM  (3)

/*!
  Basic item type is u16
  */
typedef u16 item_type;

/*!
  Get the process handle of data base 16 supported in this version data base
  */
void *db_get_db_16_handle(void);

#endif //__DATA_BASE16_H_

