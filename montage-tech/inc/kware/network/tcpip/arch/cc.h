/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: cc.h,v 1.2 2005/01/09 13:58:06 millin Exp $
 */
#ifndef __CC_H__
#define __CC_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifndef WIN32
#include <sys/time.h>
#endif

typedef    unsigned char  u8_t;
typedef    signed char    s8_t;
typedef    unsigned short  u16_t;
typedef    signed short   s16_t;
typedef    unsigned int  u32_t;
typedef    signed int    s32_t;

typedef    u32_t    mem_ptr_t;

/* Define (sn)printf formatters for these lwIP types */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"

#ifndef BYTE_ORDER
#define BYTE_ORDER   LITTLE_ENDIAN 
#endif
 
#if 0
#define PACK_STRUCT_FIELD(x) x//; #pragma STRUCT_ALIGN(x,1)
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#else
#define PACK_STRUCT_FIELD(x)  x
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_BEGIN  //__packed
#define PACK_STRUCT_END
#endif

#ifdef __cplusplus
extern "C" {
#endif
 int mtos_printk(const char *p_fmt, ...);
#ifdef __cplusplus
}
#endif



#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x)  do { mtos_printk x; } while(0)
#endif

#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) do {mtos_printk("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); } while(0)
#endif

#define LWIP_PROVIDE_ERRNO
#ifdef __cplusplus
}
#endif

#endif /* __CC_H__ */
