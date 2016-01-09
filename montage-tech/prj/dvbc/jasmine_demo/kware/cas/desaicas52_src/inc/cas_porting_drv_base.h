/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __CAS_PORTING_DRV_BASE_H__
#define __CAS_PORTING_DRV_BASE_H__

void* cas_porting_drv_base_malloc(u32 size);

void cas_porting_drv_base_free(void* p);

void cas_porting_drv_printf(char *formart,...);

void cas_porting_lib_printf(char *formart,...);

void cas_porting_adt_printf(char *formart,...);

void cas_porting_debug_printf(char *formart,...);
#endif

