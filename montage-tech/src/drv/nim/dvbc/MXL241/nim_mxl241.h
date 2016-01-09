/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIM_MXL241_H__
#define __NIM_MXL241_H__


#define CONTAIN_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - (int)&((type *)0)->member)) 


typedef struct
{
    nim_channel_info_t cur_channel;
    unsigned char bandwidth;
    unsigned char j83b_j83a;//0=j83b, 1=j83a
}nim_mxl241_priv_t;

#endif
