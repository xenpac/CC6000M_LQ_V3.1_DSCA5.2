/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIM_PRIV_H__
#define __NIM_PRIV_H__

/*!
    NIM driver low level structure as the private data("priv") of struct "nim_device_t"
  */
typedef struct lld_nim
{
  /*!
      The private data of low level driver.
    */
  void *p_priv;
  /*!
      The low level function to implement the high level interface "nim_channel_scan".
    */
  RET_CODE (*channel_scan)(struct lld_nim *p_dev, 
              nim_scan_info_t *p_scan_info);
  /*!
      The low level function to implement the high level interface "nim_channel_connect".
    */
  signed long (*channel_connect)(struct lld_nim *p_dev, 
              nim_channel_info_t *p_channel_info, 
              BOOL for_scan);
  /*!
      The low level function to implement the high level interface "nim_channel_set".
    */
  signed long (*channel_set)(struct lld_nim *p_dev, 
               nim_channel_info_t *p_channel_info, 
               nim_channel_set_info_t *p_channel_set_info);
  /*!
      The low level function to implement the high level interface "nim_channel_perf".
    */
  signed long (*channel_perf)(struct lld_nim *p_dev, 
               nim_channel_perf_t *p_channel_perf);
  /*!
      The low level function to implement the high level interface "nim_diseqc_ctrl".
    */
  RET_CODE (*diseqc_ctrl)(struct lld_nim *p_dev, 
               nim_diseqc_cmd_t *p_diseqc_cmd);
  /*!
      The low level function to implement the high level interface "nim_blind_scan_start".
    */
  RET_CODE (*blind_scan_start)(struct lld_nim *p_dev, nim_scan_info_t *p_scan_info);
  /*!
    The low level function to implement the high level interface "nim_blind_scan_cancel".
  */
  RET_CODE (*blind_scan_cancel)(struct lld_nim *p_dev);
  /*!
    The low level function to implement the high level interface "nim_notify_register".
  */
  RET_CODE (*notify_register)(struct lld_nim *p_dev, nim_notify func);
}lld_nim_t;
 
#endif // __NIM_PRIV_H__

