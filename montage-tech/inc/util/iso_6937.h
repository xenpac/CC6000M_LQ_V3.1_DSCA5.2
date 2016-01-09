/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ISO_6937_H__
#define __ISO_6937_H__

/*!
   Convert a string from iso6937 to unicode.

   \param[in] p_iso6937 Points to the iso6937 string.
   \param[in] length Specifies the length of the iso6937 string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.

  */
void iso6937_to_unicode(u8 *p_iso6937, s32 length, u16 *p_unicode, s32 maxlen);

#endif
