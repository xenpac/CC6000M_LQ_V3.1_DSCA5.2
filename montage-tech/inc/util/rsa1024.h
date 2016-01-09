/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RSA_H__
#define __RSA_H__

#define BI_MAXLEN 80

/*!
  CBigInt
  */
struct CBigInt
{
    /*!
      Big int length in unit of unsigned long 
      */    
    unsigned long m_nLength;
    /*!
      Big int value in format of unsigned long array
      */
    unsigned long m_ulValue[BI_MAXLEN];
};

/*!
  RsaTrans
  */
void RsaTrans(struct CBigInt *in, struct CBigInt *A, struct CBigInt *B, struct CBigInt *out);

/*!
  BigInt_Mod
  */
void BigInt_Mod(struct CBigInt *A, struct CBigInt *B, struct CBigInt *out);

#endif //__RSA_H__
