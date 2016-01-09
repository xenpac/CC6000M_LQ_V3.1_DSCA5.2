/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __REGISTER_NET_STREAM_H__
#define __REGISTER_NET_STREAM_H__

/*!
*
*/
void register_http_stream();
/*!
*
*/
int register_http_stream_is();
/*!
*
*/
void register_rtsp_stream();
/*!
*
*/
void register_rtmp_stream();

/*!
*
*/
void register_asf_stream();

/*!
*
*/
void register_rtmp_protocol(int type);

/*!
*
*/
void register_fifo_stream();

#endif





