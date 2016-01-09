/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SOCKET_API_H__
#define __SOCKET_API_H__

#ifdef WIN32
#include <winsock2.h>

void socket_init(void);

#else
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define socket_init do{} while(0)

#endif

#endif /* __SOCKET_API_H__ */
