/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DEMUX_MP_MISC_H__
#define __DEMUX_MP_MISC_H__
/*!
  Get current ip
  */
int get_local_ip(char *ip_addr);
/*!
 get ip's address
  */
int get_ip_country(char *ip_addr,char *country_id);    
#endif
