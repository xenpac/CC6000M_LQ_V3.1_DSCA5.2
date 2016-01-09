/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_UNICODE_H__
#define __LIB_UNICODE_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!

   Get the length of a unicode string.

   \param[in] p_str Points to a null-terminated string.

   \return The number of characters in string, excluding the terminal NULL.
  */
u32 uni_strlen(const u16 *p_str);

/*!
   Compares two unicode strings to determine if they are the same.

   \param[in] p_dst Pointer to the first null-terminated string to be compared.
   \param[in] p_src Pointer to the second null-terminated string to be compared.

   \return Returns zero if the strings are identical. 
           Returns a positive value if the string pointed to by p_dst is 
           greater than that pointed to by p_src. Returns a negative value
           if the string pointed to by p_dst is less than that pointed to by p_src.
  */
s32 uni_strcmp(const u16 *p_dst, const u16 *p_src);

/*!
   Compares two unicode strings to determine if they are the same.

   \param[in] p_dst Pointer to the first null-terminated string to be compared.
   \param[in] p_src Pointer to the second null-terminated string to be compared.
   \param[in] count Specifies the number of characters to be compared.
   
   \return Returns zero if the strings are identical. 
           Returns a positive value if the string pointed to by p_dst is 
           greater than that pointed to by p_src. Returns a negative value
           if the string pointed to by p_dst is less than that pointed to by p_src.
  */
s32 uni_strncmp(const u16 *p_dst, const u16 *p_src, u32 count);
/*!
   Copies one unicode string to another.

   \param[out] p_dst Pointer to the destination string.
   \param[in] p_src Pointer to the source string.
  */
void uni_strcpy(u16 *p_dst, const u16 *p_src);

/*!
   Copy characters of one unicode string to another.

   \param[out] p_dst Pointer to the destination string.
   \param[in] p_src Pointer to the source string.
   \param[in] count Specifies the number of characters to be copied.
  */
void uni_strncpy(u16 *p_dst, const u16 *p_src, u32 count);

/*!
   Initialize characters of a unicode string to a given format.
   \param[out] p_str Pointer to the string.
   \param[in] val Specifies the character setting.
   \param[in] count Specifies the number of characters to be set.
  */
void uni_strnset(u16 *p_str, const u16 val, u32 count);

/*!
   Appends one string to another.

   \param[out] p_dst Pointer to the destination string. This buffer must be
                   large enough to hold both strings and the terminating 
                   null character.
   \param[in] p_src Pointer to the source string to be appended to p_dst.

   \return Returns a pointer to psz1, which holds the combined strings.
  */
u16 *uni_strcat(u16 *p_dst, const u16 *p_src, u32 dst_len);

/*!
  unistr rchr.

  \param[in] p_str : p_str
  \param[in] code : code

  \return 
  */
u16 *uni_strrchr(u16 *p_str, u16 code);

/*!
  uni str compare i.

  \param[in] p_str1 : p_str
  \param[in] p_str2 : p_str

  \return 
  */
u32 uni_strcmpi(u16 *p_str1, u16 *p_str2);

/*!
  uni str str

  \param[in] p_str1 : p_str
  \param[in] p_str2 : p_str

  \return 
  */
u16 *uni_strstr(u16 *p_str1, u16 *p_str2);

/*!
   Converts a given ascii string to an unicode string.

   \param[in] p_ascstr Pointer to the ascii string.
   \param[out] p_unistr Pointer to the unicode string.

   \return Returns the number of converted characters.
  */
u32 str_asc2uni(const u8 *p_ascstr, u16 *p_unistr);

/*!
   Converts a given unicode string to an ascii string.

   \param[out] p_ascstr Pointer to the ascii string.
   \param[in] p_unistr Pointer to the unicode string.

   \return Returns the number of converted characters.
  */
u32 str_uni2asc(u8 *p_ascstr, const u16 *p_unistr);

/*!
   Converts a given ascii string to an unicode string with the specified count.

   \param[in] p_ascstr Pointer to the ascii string.
   \param[out] p_unistr Pointer to the unicode string.
   \param[in] count Specifies the number of characters to be converted.

   \return Returns the number of converted characters.
  */
u32 str_nasc2uni(const u8 *p_ascstr, u16 *p_unistr, u32 count);

/*!
   Converts a given unicode string to an ascii string with the specified count.

   \param[out] p_ascstr Pointer to the ascii string.
   \param[in] p_unistr Pointer to the unicode string.
   \param[in] count Specifies the number of characters to be converted.

   \return Returns the number of converted characters.
  */
u32 str_nuni2asc(u8 *p_ascstr, const u16 *p_unistr, u32 count);

/*!
   Converts a given ascii character to an unicode character.

   \param[in] achar Specifies the ascii character.

   \return Returns the converted character.
  */
u16 char_asc2uni(u8 achar);

/*!
   Converts a given unicode character to an ascii character.

   \param[in] uchar Specifies the ascii character.

   \return Returns the converted character.
  */
u8 char_uni2asc(u16 uchar);

/*!
   Swaps the endian of an unicode character.

   \param[in] uchar Specifies the unicode character.

   \return Returns the converted character.
  */
u16 swap_unichar_endian(u16 uchar);

/*!
   Swaps the endian of an unicode string.

   \param[in] p_str Pointer to the unicode string.
  */
void swap_unistr_endian(u16 *p_str);
#ifdef __cplusplus
}
#endif
#endif
