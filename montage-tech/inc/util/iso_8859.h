/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ISO_8859_H__
#define __ISO_8859_H__

/*!
   Support ISO8859-1 to ISO8859-16.
  */
#define SUPPORT_ISO8859_CNT 17

/*!
   Convert a string from iso8859 to unicode.

   \param[in] p_iso8859 Points to the iso8859 string.
   \param[in] length Specifies the length of the iso8859 string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.

  */
void iso8859_to_unicode(u32 code,
                        u8 *p_iso8859,
                        s32 length,
                        u16 *p_unicode,
                        s32 maxlen);

#endif
