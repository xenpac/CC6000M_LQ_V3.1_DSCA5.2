/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SHA1_H__
#define __SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

void SHA1_Perform(unsigned char *indata, unsigned long inlen, unsigned char *outdata);

#ifdef __cplusplus
}
#endif

#endif //__SHA1_H__
