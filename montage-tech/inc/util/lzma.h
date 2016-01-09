//============================================================================
//  File Name: lzma.h
//
// Copyright 2005 Magnum Semiconductor Inc.  All rights reserved.
//
//  Description:
//      lzma decode interface
//
//  Modification History:
//      $Id: lzma.h,v 1.1.1.1.12.1 2009/02/06 11:18:22 hwxiao Exp $
//============================================================================

#ifndef _LZMA_H
#define _LZMA_H
#ifdef __cplusplus
extern "C" {
#endif
typedef enum
{
	ZIP_GZIP = 0,
	ZIP_LZMA,
	ZIP_MAX
}ZIP_METHOD;

int unlzma(unsigned int dstAddr, unsigned int srcAddr, unsigned int inLen, int freeBuffer, unsigned int destLen);
//int unlzma1(Uint32 dstAddr, Uint32 srcAddr, Uint32 zLen, int freeBuffer, Uint32 *destLen);
int unlzma_get_uncompressed_size(unsigned int srcAddr, unsigned int zLen, unsigned int *destLen);
int IsGzip(unsigned int srcAddr);

int lzmacompress(unsigned char *dest, unsigned int  *destLen,
      const unsigned char *src, unsigned int  srcLen, unsigned char *p_cache,unsigned int cache_size);

#ifdef __cplusplus
}
#endif
      
#endif

