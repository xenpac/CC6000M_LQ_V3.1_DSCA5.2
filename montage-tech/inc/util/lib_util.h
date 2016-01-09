/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LIB_UTIL_H__
#define __LIB_UTIL_H__

/*! 
  MAX string length supported in string find API
  */
#define MAX_STRING_LEN_IN_FIND (32)

/*! 
  UTC format time definition
  */
typedef struct
{
  /*!
    Year
    */
  u16 year;
  /*!
    Month
    */
  u8 month;
  /*!
    Day
    */
  u8 day;
  /*!
    Hour
    */
  u8 hour;
  /*!
    Minute
    */
  u8 minute;
  /*!
    Sec
    */
  u8 second;  
  /*!
    researved.
    */
  u8 reserved;
} utc_time_t;

/*!
  Make input byte into u32
  \param[in] input bytes
  \param[out] u32 format data
  \return : u32 value.
  */
u32 make32(u8 *p_addr);

/*!
  Make input byte into u16
  \param[in] input bytes
  \param[out] u16 format data
  \return : u16 value.
  */
u16 make16(u8 *p_addr);

/*!
  Time point definition
  \param[in] p_in input time in time descriptor
  \param[out] UTC format time  
  \param[out] out : utc time for output
  \return : NULL
  */
void time_point_trans(u8 *p_in, utc_time_t *p_time_out);

/*!
  Translate input date into UTCtime format 
  \param[in] p_in time input
  \param[out] date output
  \param[out] out : utc time for output.
  \return : NULL
  */
RET_CODE date_trans(u8 *p_in, utc_time_t *p_date_out);

/*!
  Change data from BCD format to decimal format by each u32
  \param[in] input BCD  number
  \return : number in decimal type.
  */
u32 bcd_number_to_dec(u32 input);

/*!
  Change data from decimal format format to  BCD
  \param[in] input dec  number
  \param[out] BCD number out 
  \return : number in decimal type.
  */
u32 dec_number_to_bcd(u32 input);

/*!
  \param[in] p_src_string Source string where match sub string will be searched
  \param[in] p_match_string String to be searched for
  \param[in] nstart Starting point in source string where string will be 
    searched for
  \param[out] -1:  No match string existing u16: Starting position of matched 
    string
  \param[in] nstart : Starting point in source string where string will be 
  searched for
  \return : -1 for No match string existing; u16 for Starting position of 
    matched string
  */
s16 find_string(const u16 *p_src_string, const u16 *p_match_string, u16 nStart);

/*!
  Match string.
  
  \param[in] p_src_string:  		Source string where match sub string will be searched
  \param[in] p_match_string:  	String to be searched for
  \param[in] is_case_enabled: 	Case sensitive or not
  \param[out] TRUE sub_string existing, FALSE sub string not existing
  */
BOOL match_string(const u16 *p_src_string, const u16 *p_match_string, 
BOOL is_case_enabled);

/*!
  my_atof.
  
  \param[in] p_str:  		Source string conver to double
  */
double mt_atof(char *p_str);

/*!
   convert input  letter

   \return : SUCCESS/FAILURE
  */
RET_CODE input_letter_convert(u16 *p_base_str, u16 *p_convert_str, u16 input_code);

/*!
   convert input arabic letter

   \return : SUCCESS/FAILURE
  */
RET_CODE ara_input_letter_convert(u16 *p_base_str, u16 *p_convert_str, u16 input_code);

/*!
   convert arabic str

   \return : SUCCESS/FAILURE
  */
RET_CODE ara_str_convert(u16 *p_base_str, u16 *p_convert_str, u16 max_len);
/*!
   is arabic or not

   \return : is/not
  */
BOOL is_arabic_code(u16 uni_code);
/*!
   get arabic str len

   \return : len
  */
 u16 get_arabic_str_len(u16 *uni_code_str);
#endif

