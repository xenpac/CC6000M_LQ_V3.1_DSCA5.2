/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#ifndef _AP_NIT_H_ 
#define _AP_NIT_H_ 

app_t *construct_ap_nit(void);

void enable_ap_nit(void);

u8 get_init_nit_ver(void);

void set_init_nit_ver(u8 nit_version);

u16 get_init_nit_network_id(void);

void set_init_nit_network_id(u16 network_id);

#endif
