/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CHARSTO_PRIV_H__
#define __CHARSTO_PRIV_H__

/*!
    char storage driver low level structure as the private data("priv") of struct "charsto_device_t"
  */
typedef struct lld_charsto
{
  /*!
      The private data of low level driver.
    */
  void *p_priv;

  /*!
   *     The file handle 
   */
  int  file_handle;

  /*!
      The low level function to implement the high level interface "charsto_read".
    */  
  RET_CODE (*read)(struct lld_charsto *p_lld, u32 addr, u8 *p_buf, u32 len);
  /*!
      The low level function to implement the high level interface "charsto_writeonly".
    */   
  RET_CODE (*writeonly)(struct lld_charsto *p_lld, u32 addr, u8 *p_buf, u32 len);
  /*!
      The low level function to implement the high level interface "charsto_erase".
    */   
  RET_CODE (*erase)(struct lld_charsto *p_lld, u32 addr, u32 sec_cnt);
  /*!
      The low level function to implement the high level interface "charsto_protect_all".
    */   
  RET_CODE (*protect_all)(struct lld_charsto *p_lld);
  /*!
      The low level function to implement the high level interface "charsto_unprotect_all".
    */   
  RET_CODE (*unprotect_all)(struct lld_charsto *p_lld);
  /*!
    The low level function to implement the high level interface "get flash capacity".
    */   
  RET_CODE (*get_capacity)(struct lld_charsto *p_lld, u32 *p_cap);
  /*!
    The low level function to implement the high level interface "get flash unique id".
    */   
  RET_CODE (*get_unique_id)(struct lld_charsto *p_lld, u8 *p_buf, u32 len, u32 *act_len);
 /*!
      The low level function to implement the high level interface "charsto_otp_read".
    */  
  RET_CODE (*otp_read)(struct lld_charsto *p_lld, u32 addr, u8 *p_buf, u32 len);
  /*!
      The low level function to implement the high level interface "charsto_otp_write".
    */   
  RET_CODE (*otp_write)(struct lld_charsto *p_lld, u32 addr, u8 *p_buf, u32 len);
  /*!
      The low level function to implement the high level interface "charsto_otp_erase".
    */   
  RET_CODE (*otp_erase)(struct lld_charsto *p_lld, u32 addr);
}lld_charsto_t;

#endif //__CHARSTO_PRIV_H__

