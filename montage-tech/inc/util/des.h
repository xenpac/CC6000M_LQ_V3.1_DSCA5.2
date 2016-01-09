/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DES_H__
#define __DES_H__

/*!
  DES encrypt function
  
  \param[out] cipher text, will be 8 bytes
  \param[in] clear text, must be 8 bytes
  \param[in] key, must be 8 bytes
  \return 0 if success, else fail
  */
int des_encrypt(char *out, char *in, const char *key);

/*!
  DES decrypt function
  
  \param[out] cipher text, need 8 bytes
  \param[in] clear text, must be 8 bytes
  \param[in] key, must be 8 bytes
  
  \return 0 if success, else fail
  */
int des_decrypt(char *out, char *in, const char *key);

/*!
  3DES decrypt function
  
  \param[out] clear text
  \param[in] cipher text
  \param[in] datalen, the data length of "in"
  \param[in] key, must be 16 bytes
  
  \return
  */
void triple_des_decrypt(char *out, char *in, long datalen, char *key);

/*!
  3DES encrypt function
  
  \param[out] cipher text
  \param[in] clear text
  \param[in] datalen, the data length of "in"
  \param[in] key, must be 16 bytes
  
  \return
  */
void triple_des_encrypt(char *out, char *in, long datalen, char *key);

#endif
