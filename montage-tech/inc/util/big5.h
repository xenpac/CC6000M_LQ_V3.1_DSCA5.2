/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __BIG5_H__
#define __BIG5_H__

/*!
   Links a big5 to unicode mapping table.

   \param[in] p_map Points to the table.
   \param[in] cnt Specifies the length of the table.
  */
void link_big5_maptab(char_map_t *p_map, u32 cnt);

/*!
   Unlinks mapping table(Release resources).
  */
void unlink_big5_maptab(void);

/*!
   Converts a given gb2312 string to an big5 string.

   \param[in] gb2312 Points to the gb2312 string.
   \param[in] length Specifies the length of the gb2312 string.
   \param[in] p_big5 Points to the big5 string.
   \param[in] maxlen Specifies the maxiam length of the unicode string.

  */
void gb2312_to_big5(u8 *p_gb2312, s32 length, u8 *p_big5, s32 maxlen);

/*!
   Converts a given big5 string to an unicode string.

   \param[in] big5 Points to the big5 string.
   \param[in] length Specifies the length of the gb2312 string.
   \param[in] unicode Points to the unicode string.
   \param[in] maxlen Specifies the maxiam length of the unicode string.

  */
void big5_to_unicode(u8 *p_big5, s32 length, u16 *p_unicode, s32 maxlen);
#endif
