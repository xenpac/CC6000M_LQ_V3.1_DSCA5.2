#ifndef __AF_H__
#define __AF_H__

/*!
  af config.
  */
typedef struct
{
  /*!
    cache size.
    */
  u32 cache_size;
  /*!
    cache count.
    */
  u32 cache_cnt;    
  /*!
    bpp
    */
  u8 bpp;   
}af_cfg_t;

/*!
  attach array font handle.
  */
void af_attach(handle_t res_handle, af_cfg_t *p_cfg);

/*!
  dettach array font handle.
  */
void af_detach(handle_t res_handle);

#endif
