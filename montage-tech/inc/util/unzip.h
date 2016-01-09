/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UNZIP_H__
#define __UNZIP_H__

//define size_t u32

/*!
  the memory copy function pointer
  */
typedef void *(*p_memcpy)(void *p_dest,const void *p_src,size_t n);

/*!
  Init the internal inflate memory used by the gzip algorithm
  
  \param[in] p_sys the memory pointer to the inflate buffer, 
            >100K inflate buffer size is recommended
  */
void init_fake_mem(void *p_sys);

/*!
  gnu unzip function
  
  \param[in] p_in_addr the zipped data address
  \param[in] p_out_addr the unzip data address
  \param[in] in_len the size of zipped data
  \param[in/out] p_out_len input the max size of available size, and output the 
                  actual size
  \param[in] p_func_ptr the memory copy function pointer

  \return 0 if success, else fail
  */
int gunzip(void *p_in_addr,void *p_out_addr,unsigned long in_len,
                unsigned long *p_out_len,p_memcpy p_func_ptr);


#endif // end of __UNZIP_H__
