
/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _KVDB_MIDDLEWARE_API_H_
#define _KVDB_MIDDLEWARE_API_H_

#ifdef __cplusplus
extern "C" {
#endif


 int KVDB_CREATE_BROWSER();  

void KVDB_DESTROY_BROWSER();

 unsigned short KVDB_EVENT_PROC( int event[3]);

 void KVDB_DEMUX_PROC(int masseges[3]);


#ifdef __cplusplus
}
#endif

#endif

