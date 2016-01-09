/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LZMAIF_H__
#define __LZMAIF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  Init the internal inflate memory used by the LZMA algorithm
  
  \param[in] p_sys the memory pointer to the inflate buffer
  \param[in] mem_size the inflate buffer size, 
              >100K inflate buffer size is recommended
  */
void init_fake_mem_lzma(void *p_sys, int mem_size);

/*!
  LZMA unzip function
  
  \param[in] p_dst the unzip data address
  \param[in/out] p_ulDstLen input the max size of available size, and output the 
                  actual size
  \param[in] p_src the zipped data address                  
  \param[in] ulsrcLen the size of zipped data

  \return 0 if success, else fail
  */
unsigned long lzma_decompress(void *p_dst, unsigned long *p_uldstLen, 
                               void *p_src, unsigned long ulsrcLen);

#ifdef __cplusplus
}
#endif

#endif //end of __LZMAIF_H__
