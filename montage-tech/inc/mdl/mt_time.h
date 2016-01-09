/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MT_TIME_H_
#define __MT_TIME_H_

/*!
  Init time module.
  
  \param[out] Return TRUE if succeed. or not FALSE.
  */
BOOL time_init(utc_time_t *p_time);

/*!
   Get current time
  \param[out] the current time
  \param[in] get GMT time ornot
  \return TRUE for get succeed, FALSE  failed
  */
BOOL time_get(utc_time_t *p_time, BOOL b_gmt_time);

/*!
  Init system status module.
  
  \param[in] time new time
  */
void time_set(utc_time_t *p_time);

/*!
  Compare different
  \param[in] p_time_1 time 1 for comparison
  \param[in] p_time_2 time 2 for comparison
  \param[in] ignore_sec   TRUE ignore second else compare second
  \return compare result, -1 means time1 less time2 , 0 equal, 1 time1 more than time2 
  */
s8 time_cmp(utc_time_t *p_time_1, utc_time_t *p_time_2, BOOL ignore_sec);

/*!
  Update time by seconds
  \param[in/out] p_time src
  \param[in] sec seconds
  */
void time_up(utc_time_t *p_time, u32 sec);


/*!
  Add time to source time,
  \param[in/out] p_time_src
  \param[in] p_time_add
  */
void time_add(utc_time_t *p_time_src, utc_time_t *p_time_add);

/*!
  Convert time to minute, form 2000.1.1
  \param[in/out] p_time
  \return time value(caculate by second)
  */
u32 time_conver(utc_time_t *p_time);

/*!
  Minus time from source time
  \param[in/out] p_time_src
  \param[in] p_time_add
  */
u32 time_dec(utc_time_t *p_time_src, utc_time_t *p_time_dec);

/*!
  convert input GMT time to local time
  \param[in] p_gmt_time
  \param[out] p_loc_time
  */
void time_to_local(utc_time_t *p_gmt_time, utc_time_t *p_loc_time);

/*!
  Convert local time to GMT time
  \param[in] p_loc_time local time 
  \param[in] p_gmt_time gmt time 
  */
void time_to_gmt(utc_time_t *p_loc_time, utc_time_t *p_gmt_time);

/*!
  Set time zone
  \param[in] zone to be set
  */
void time_set_zone(s8 zone);

/*!
  Set summer time
  \param[in] ture or false
  */
void time_set_summer_time(BOOL is_summer);

/*!
  Printf time
  \param[in] p_time input time to be printed
  \param[in] string information to be printed
  */
void printf_time(utc_time_t *p_time, char *p_str);

/*!
  to judge is time value is 0
  \param[in] p_time input time to be judged
  \return TRUE for yes, otherwise no.
  */
BOOL is_time_zero(utc_time_t *p_time);

/*!
  to get week day by date.
  \param[in] p_time : date
  \return : week day
  */
u8 date_to_weekday(utc_time_t *p_time);

#endif // End of __MT_TIME_H_

