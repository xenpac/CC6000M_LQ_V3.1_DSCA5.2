/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __CAS_PORTING_DRV_SMC_H__
#define __CAS_PORTING_DRV_SMC_H__
RET_CODE cas_porting_drv_smc_transaction(u8 *SendComm, 
                                        u8 CommLen, 
                                        u8 *RecvResp,
                                        u16 *NumberRead);

RET_CODE cas_porting_drv_smc_reset(u8 *atr, u8 *len);

RET_CODE cas_porting_drv_smc_close(void);
#endif

                                        