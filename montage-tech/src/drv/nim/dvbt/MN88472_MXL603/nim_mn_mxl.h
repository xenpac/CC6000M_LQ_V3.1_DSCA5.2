/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIM_MN_MXL_H__
#define __NIM_MN_MXL_H__





/*!
  The NIM mn_mxl handle
  */
typedef struct nim_atbm_handle
{
   nim_config_t *p_nim_cfg; 
   void *mn88472_priv;
   void *mxl603_priv;
}nim_mn_mxl_handle_t;


typedef struct nim_mn_mxl_priv
{
  u8 bs_stop : 1;
  u8 diseqc_2x : 1;
  u8 onoff_22k : 1;
  u8 lnb_onoff : 1;
  u8 reserved : 4;
  u8 diseqc_tx_buf[8];
  nim_diseqc_cmd_t cur_diseqc;
  nim_channel_info_t cur_channel;
  nim_mn_mxl_handle_t mn_mxl_hanlde;
}nim_mn_mxl_priv_t;




#endif

