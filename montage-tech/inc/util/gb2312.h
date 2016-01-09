/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GB2312_H__
#define __GB2312_H__

/*!
   Links a gb2312 to unicode mapping table.

   \param[in] p_map Points to the table.
   \param[in] cnt Specifies the length of the table.
  */
void link_gb2312_maptab(char_map_t *p_map, u32 cnt);

/*!
   Unlinks mapping table(Release resources).
  */
void unlink_gb2312_maptab(void);


/*!
   Determines whether the string is non-standard gb2312 string.

   \param[in] str Points to the gb2312 string.
   \param[in] length Specifies the length of the gb2312 string.

   \return Returns TRUE if the string is non-standard gb2312 string,
         Otherwise return FALSE.
  */
BOOL is_gb2313_non_standard(u8 *p_str, u32 length);

/*!
   Converts a given gb2312 string to an unicode string.

   \param[in] gb2312 Points to the gb2312 string.
   \param[in] length Specifies the length of the gb2312 string.
   \param[in] unicode Points to the unicode string.
   \param[in] maxlen Specifies the maxiam length of the unicode string.

   \return The length of the generated unicode string.
  */
u16 convert_gb2312(u16 gb2312);

/*!
   Converts a given gb2312 string to an unicode string.

   \param[in] gb2312 Points to the gb2312 string.
   \param[in] length Specifies the length of the gb2312 string.
   \param[in] unicode Points to the unicode string.
   \param[in] maxlen Specifies the maxiam length of the unicode string.

  */
void gb2312_to_unicode(u8 *p_gb2312, s32 length, u16 *p_unicode, s32 maxlen);

/*!
   Converts a given unicode string to an gb2312 string.

   \param[in] unicode Points to the unicode string.
   \param[in] length	Specifies the length of the unicode string.
    \param[in] gb2312	Points to the gb2312 string.
   \param[in] maxlen Specifies the maxiam length of the gb2312 string.

  */
void unicode_to_gb2312(u16 *p_unicode, s32 length, u8 *p_gb2312, s32 maxlen);

#endif
