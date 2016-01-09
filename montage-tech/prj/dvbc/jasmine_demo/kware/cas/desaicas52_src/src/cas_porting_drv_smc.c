/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"


#include "smc_op.h"
#include "smc_pro.h"

RET_CODE cas_porting_drv_smc_transaction(u8 *SendComm, 
                                        u8 CommLen, 
                                        u8 *RecvResp,
                                        u16 *NumberRead)
{
  scard_opt_desc_t opt_desc = {0};
  RET_CODE ret = ERR_FAILURE;

  opt_desc.p_buf_out = SendComm;
  opt_desc.bytes_out = CommLen;
  opt_desc.p_buf_in = RecvResp;
  opt_desc.bytes_in_actual = 0;

  ret = scard_pro_rw_transaction(&opt_desc);
  *NumberRead = (u16)(opt_desc.bytes_in_actual);
  return ret;
}


RET_CODE cas_porting_drv_smc_reset(u8 *atr, u8 *len)
{
  RET_CODE ret = ERR_FAILURE;
  scard_atr_desc_t atr_desc;

  atr_desc.p_buf = atr;
  ret = scard_pro_active(&atr_desc);
  *len = atr_desc.atr_len;
  return ret; 
}


RET_CODE cas_porting_drv_smc_close(void)
{
  RET_CODE ret = ERR_FAILURE;
  
  ret = scard_pro_deactive();

  return ret;  
}


