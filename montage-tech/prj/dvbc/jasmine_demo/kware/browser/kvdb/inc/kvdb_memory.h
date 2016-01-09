/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _KVdb_memory_H__
#define _KVdb_memory_H__

void *kvdb_porting_malloc(int size);

void kvdb_porting_free(void *ptr);

int kvdb_porting_dprintf(const char *fmt, ...);

unsigned int kvdb_porting_time_ms(void);

void *kvdb_porting_memcpy(void *dest, void *src, unsigned int len);

void kvdb_porting_memset(void * dest, int c, unsigned int len);

int kvdb_porting_memcmp( void *buf1, void *buf2,unsigned int  len);

#endif

