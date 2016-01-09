/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_CHAR_H__
#define __LIB_CHAR_H__

/*! 
  MAX string length supported in string find API
  */
#define MAX_STR_LEN_IN_FIND (32)


/*!
   The type definition of the conversion function

   \param[in] p_dvb Points to the DVB SI string.
   \param[in] length Specifies the length of the iso6937 string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.

  */
typedef void (*dvb_txt_convertor_t)(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen);

/*!
   This function allows you to reassign a dvb text convertor which is used if 
   no character selection information is given in a text item. 
   (for those non-standard encoding text)

   \param[in] p_dvb Points to the DVB SI string.
   \param[in] length Specifies the length of the DVB SI string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.

  */
void dvb_set_def_convertor(dvb_txt_convertor_t convertor, BOOL is_bigendian);
 

/*!
   Convert DVB SI code to unicode, ignore what it doesn't support.

   \param[in] p_dvb Points to the DVB SI string.
   \param[in] length Specifies the length of the DVB SI string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.

   \Return					: the result of processing.
  */
RET_CODE dvb_to_unicode(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen);

/**
 * name		: get_iso639_refcode
 * description	: return reference code of ISO8859/ISO6937 according to iso639 code.
 * parameter	: 1
 *	IN	UINT8 *iso639:	the ISO-639 string, could be 2 or 3 letters.
 * return value	: UINT32
 *	0	: no reference code.
 *	1~15	: the ISO-8859 code page value.
 *	6937	: the ISO-6937 reference code.
 *	other	: reserved.
 */

u32 get_iso639_refcode(u8 *iso639);


/*!
   Convert DVB SI code to unicode, ignore what it doesn't support.

   \param[in] p_dvb Points to the DVB SI string.
   \param[in] length Specifies the length of the DVB SI string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.
   \param[in] refcode for use which iso charactor set.

   \Return					: the result of processing.
  */
RET_CODE dvb_to_unicode_epg(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen, u32 refcode);


/*!
   Convert window-1256 code to unicode.

   \param[in] Points to the SI string.
   \param[in] length Specifies the length of the DVB SI string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.

   \Return					: the result of processing.
  */
void cp1256_to_unicode(u8 *p_char1256, s32 length, u16 *p_unicode, s32 maxlen);

/*!
   convert input  letter

   \return : SUCCESS/FAILURE
  */
RET_CODE input_char_convert(u16 *p_base_str, u16 *p_convert_str, u16 input_code);

/*!
   convert input arabic letter

   \return : SUCCESS/FAILURE
  */
RET_CODE arabic_input_char_convert(u16 *p_base_str, u16 *p_convert_str, u16 input_code);

/*!
   convert arabic str

   \return : SUCCESS/FAILURE
  */
RET_CODE arabic_str_convert(u16 *p_base_str, u16 *p_convert_str, u16 max_len);
/*!
   is arabic or not

   \return : is/not
  */
BOOL is_arabic_uni_code(u16 uni_code);

/*!
  is hebrew unicode.
  */
BOOL is_hebrew_uni_code(u16 uni_code);
/*!
   get arabic str len

   \return : len
  */
 u16 get_arabic_uni_str_len(u16 *uni_code_str);

/*!
  match string.

  \param[in] p_src_string : source.
  \param[in] p_match_string : match.
  \param[in] is_case_enabled : case enable.

  \return : TRUE or FALSE.
  */
BOOL match_str(const u16 *p_src_string, const u16 *p_match_string, BOOL is_case_enabled);

/*!
  caculate crc8.
  */
u8 cacl_crc8(u8 *p_buf, u8 len);
#endif

