/*-
 * Copyright (c) 2001-2003 Allan Saddi <allan@saddi.com>
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
 *
 * THIS SOFTWARE IS PROVIDED BY ALLAN SADDI AND HIS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL ALLAN SADDI OR HIS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: sha256.h 348 2003-02-23 22:12:06Z asaddi $
 */

#ifndef _SHA256_H
#define _SHA256_H

#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define SHA256_HASH_SIZE 32

/* Hash size in 32-bit words */
#define SHA256_HASH_WORDS 8

/*!
  _SHA256Context
  */
typedef struct _SHA256Context {
  /*!
    total length
    */
  uint64_t totalLength;
  /*!
    hash table
    */  
  uint32_t hash[SHA256_HASH_WORDS];
  /*!
    buffer length
    */  
  uint32_t bufferLength;
  /*!
    buffer
    */  
  union {
    /*!
      buffer in unit of words
      */    
    uint32_t words[16];
    /*!
      buffer in unit of bytes
      */    
    uint8_t bytes[64];
  } buffer;
#ifdef RUNTIME_ENDIAN
  /*!
    the flag to indicate cpu endian mode 
    */
  int littleEndian;
#endif /* RUNTIME_ENDIAN */
}SHA256Context;


#ifdef __cplusplus
extern "C" {
#endif

/*!
  SHA256Init
  */
void SHA256Init (SHA256Context *sc);

/*!
  SHA256Update
  */
void SHA256Update (SHA256Context *sc, const void *data, uint32_t len);

/*!
  SHA256Final
  */
void SHA256Final (SHA256Context *sc, uint8_t hash[SHA256_HASH_SIZE]);

#ifdef __cplusplus
}
#endif

#endif /* !_SHA256_H */
