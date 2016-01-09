
/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MD5_SIGN_H__
/*!
  incoude file head
  */
#define __MD5_SIGN_H__

void md5_set_specific_value(unsigned int value);
void md5_get_date_value(unsigned char get_md5_value[16], /* 16 bytes */
				const unsigned char *input, unsigned int inputLen);

BOOL md5_check_data_value(unsigned char check_md5_value[16],
                                   const unsigned char *input, unsigned int inputLen);
#endif



