/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIM_ATBM_H__
#define __NIM_ATBM_H__





/*!
  The NIM atbm handle
  */
typedef struct nim_atbm_handle
{
   nim_config_t *p_nim_cfg; 
   void *atbm781_priv;
   void *mxl603_priv;
}nim_atbm_handle_t;


typedef struct nim_atbm_priv
{
  u8 bs_stop : 1;
  u8 diseqc_2x : 1;
  u8 onoff_22k : 1;
  u8 lnb_onoff : 1;
  u8 reserved : 4;
  u8 diseqc_tx_buf[8];
  nim_diseqc_cmd_t cur_diseqc;
  nim_channel_info_t cur_channel;
  nim_atbm_handle_t atbm_hanlde;
}nim_atbm_priv_t;




#endif

