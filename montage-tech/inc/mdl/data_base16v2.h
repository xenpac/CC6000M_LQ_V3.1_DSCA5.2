/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_BASE16V2_H_
#define __DATA_BASE16V2_H_

/*!
	\file data_base16.h

	This file provides the API to get the proc handle of data base 16.
	In this version database, basic item type in database is u16, the 
	max mark bits supported is 5 and the max virtual view supported is 3. 
	In addition, to support the data cache in data manager 32, this version 
	data base 16 provides good support to data cache
	*/

/*!
  Mask bits from bit 14 to 15 and starting from bit 0
  */
#define MAX_MARK_NUM    (3)

/*!
  Max virtual view number
  */
#define MAX_VIRTUAL_VIEW_NUM  (20)
/*!
  Basic item type is u16
  */
typedef u16 item_type_t;

/*!
  Initialize database 16v2 and Store data into class factory
  */
void db_init_database_16v2(void);

/*!
  destory database 16v2 and unregister to class factory.
  */
void db_destory_database_16v2(void);

#endif //__DATA_BASE16V2_H_


