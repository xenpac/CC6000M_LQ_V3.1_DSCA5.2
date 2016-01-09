/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_misc.h"

#include "class_factory.h"
#include "mdl.h"
#include "mt_time.h"

#ifdef PRINT_ON
  #define TIME_DEBUG
#endif

#ifdef TIME_DEBUG
#define TI_DBG(x) (OS_PRINTF x)
#else
#define TI_DBG(x) do{} while(0);
#endif

#define NOT(x) ((x) == 0)
#define ISLEAP(y) (NOT(y % 400) || ((y % 100) && NOT(y % 4)))
#define LEAPS_NUM(y) (((y) / 400) - ((y) / 100) + ((y) / 4))

#define DAY_PER_YEAR 365
#define MONTH_PER_YEAR 12
#define DAYS_OF_DEC 31
#define HOUR_PER_DAY 24
#define MINUTE_PER_HOUR 60
#define SECOND_PER_MINUTE 60
#define TICK_PER_SECOND 100

/*!
  wait semphore
  */
#define LOCK(x)      mtos_sem_take(&((x)->sem), 0)

/*!
  give semphore
  */
#define UNLOCK(x)    mtos_sem_give(&((x)->sem))

typedef struct tag_time_info
{
  /*!
    semphore to protect api call
    */
  os_sem_t sem;
  /*!
    the last setting GMT time, it's exclusive time in system
    */
  utc_time_t gmt_time;
  /*!
    the tick when setting time
    */
  u32 sync_ticks;
  /*!
    summer time flag
    */
  BOOL is_summer_time;
  /*!
    time zone
    */
  s8 time_zone;
}time_info_t;

u8 date_to_weekday(utc_time_t *p_time)
{
  u8 weekdays = 0;
  utc_time_t tmp = {0};

  memcpy((void *)&tmp, (void *)p_time, sizeof(utc_time_t));

  if((tmp.month == 1)||(tmp.month == 2))
  {
    tmp.month += 12;
    tmp.year--;
  }

  weekdays = (tmp.day + 2 * tmp.month + 3 * (tmp.month + 1) / 5 +
    tmp.year + tmp.year / 4 - tmp.year / 100 + tmp.year / 400) % 7;

  return weekdays;
}

/*!
  Get how many days current month
  \param[in] p_time some time
  */
static u8 get_month_day(utc_time_t *p_time)
{
  //from 2000.1.1
  u32 year = p_time->year;
  u8 mon_lengths[2][12] =
  {
    /* Normal years.  */
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    /* Leap years.  */
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
  };

  return mon_lengths[ISLEAP(year)][p_time->month - 1];
}

/*!
  Get how many days from 2000.1.1 to p_time date
  \param[in] p_time some time
  */
static u32 date_to_day(utc_time_t *p_time)
{
  //from BC 1.1.1
  u32 year = p_time->year;  
  //here not think leap, but no problem in this system
  u32 day = (p_time->year - 1) * DAY_PER_YEAR;  

  u32 mon_yday[2][13] =
  {
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
  };

  MT_ASSERT(p_time->year != 0);

  //add leap years
  day += LEAPS_NUM(year - 1);
  //add pre-month day
  day += mon_yday[ISLEAP(year)][p_time->month - 1];
  //add current month day
  day += p_time->day - 1; //day from 1st
  return day;
}

static void add_day(utc_time_t *p_time, u32 d)
{
  utc_time_t temp = *p_time;
  u32 month_day = 0;

  while(d > 0)
  {
    month_day = get_month_day(p_time);
    if(temp.day + d > month_day) //overrun current month
    {
      d -= (month_day + 1 - temp.day);  //need some day to next month
      p_time->day = 1; //goto next month.
      temp.day = 1; 
      
      //month plus
      if(p_time->month < MONTH_PER_YEAR)
      {
        p_time->month++;
      }
      else
      {
        p_time->year++;
        p_time->month = 1;
      }
    }
    else
    {
      p_time->day = temp.day + d;
      d = 0;
    }
  }
}

/*!
  reduce one day only, 
  */
static void day_reduce(utc_time_t *p_time)
{
  if(p_time->day > 1)
  {
    p_time->day--;
  }
  else //goto the previous month
  {
    if(p_time->month > 1)
    {
      p_time->month--;
      p_time->day = get_month_day(p_time);
    }
    else //goto the previous year
    {
      /*FIXME(Simon): for storm only*/
      if(p_time->year <= 0)
      {
        p_time->year = 2011;
      }
      MT_ASSERT(p_time->year > 0);
      p_time->year--;
      p_time->month = MONTH_PER_YEAR;
      p_time->day = DAYS_OF_DEC;
    }
  }
}

void time_up(utc_time_t *p_time, u32 sec)
{
  utc_time_t temp = *p_time;

  //add second
  if(sec > 0)
  {
    p_time->second = (temp.second + sec) % SECOND_PER_MINUTE;
    sec = (temp.second + sec) / SECOND_PER_MINUTE; //to minute
  }

  //add minute
  if(sec > 0)
  {
    p_time->minute = (temp.minute + sec) % MINUTE_PER_HOUR;
    sec = (temp.minute + sec) / MINUTE_PER_HOUR; //to hour
  }
  
  //add hour
  if(sec > 0)
  {
    p_time->hour = (temp.hour + sec) % HOUR_PER_DAY;
    sec = (temp.hour + sec) / HOUR_PER_DAY;  //to day
    add_day(p_time, sec);
  }
}

/*!
   Init time module.

 \return Return TRUE if succeed. or not FALSE.
 */
BOOL time_init(utc_time_t *p_time)
{
  class_handle_t p_handle = class_get_handle_by_id(TIME_CLASS_ID);

  //The first initialization
  if(NULL == p_handle)
  {
    BOOL ret = TRUE;
    time_info_t *p_info = NULL;
    p_info = mtos_malloc(sizeof(time_info_t));
    MT_ASSERT(p_info != NULL);
    memset(p_info, 0, sizeof(time_info_t));
    
    //create sem 
    ret = mtos_sem_create(&(p_info->sem), TRUE);
    MT_ASSERT(ret == TRUE);
#ifdef SEM_DEBUG
    strcpy(p_info->sem.m_name, "time");
#endif
    //set default time
    //p_info->gmt_time.value = p_time->value;
    memcpy(&(p_info->gmt_time), p_time, sizeof(utc_time_t));

    //printf_time(p_time, "@#$%^&*time init ");

    class_register(TIME_CLASS_ID, (class_handle_t)p_info);
  }

  return TRUE;
}

/*!
  Set time zone
  \param[in] zone to be set
  */
void time_set_zone(s8 zone)
{
  time_info_t *p_info = (time_info_t *)class_get_handle_by_id(TIME_CLASS_ID);
  p_info->time_zone = zone;
}

/*!
  Set summer time
  \param[in] ture or false
  */
void time_set_summer_time(BOOL is_summer)
{
  time_info_t *p_info = (time_info_t *)class_get_handle_by_id(TIME_CLASS_ID);
  p_info->is_summer_time = is_summer;
}

/*!
   Get current time
  \param[out] the current time
  \param[in] get GMT time ornot
  \return TRUE for get succeed, FALSE  failed
 */
BOOL time_get(utc_time_t *p_time, BOOL b_gmt_time)
{
  utc_time_t gmt_time = {0};
  time_info_t *p_info = (time_info_t *)class_get_handle_by_id(TIME_CLASS_ID);
  //LOCK(p_info);
  gmt_time = p_info->gmt_time;
  time_up(&gmt_time, 
    (mtos_ticks_get() - p_info->sync_ticks) / TICK_PER_SECOND);
  if(b_gmt_time)
  {
    *p_time = gmt_time;
  }
  else
  {
    time_to_local(&gmt_time, p_time);
  }
  //UNLOCK(p_info);

  return TRUE;
}

/*!
   Init system status module.

  \param[in] new time
 */
void time_set(utc_time_t *p_time)
{
  time_info_t *p_info = (time_info_t *)class_get_handle_by_id(TIME_CLASS_ID);
  LOCK(p_info);
  p_info->gmt_time = *p_time;
  p_info->sync_ticks = mtos_ticks_get();
  UNLOCK(p_info);
}


/*!
   compare times,
  \return compare result, -1 means time1 less time2 , 0 equal, 1 time1 more than time2 
 */
s8 time_cmp(utc_time_t *p_time_1, utc_time_t *p_time_2, BOOL ignore_sec)
{
  s8 ret = 0;
  utc_time_t time1 = *p_time_1;
  utc_time_t time2 = *p_time_2;
  
  if(ignore_sec == TRUE)
  {
    time1.second = 0;
    time2.second = 0;
  }
  
  /*if(*(u32*)(&time1) < *(u32*)(&time2))
  {
    ret = -1;
  }
  else if(*(u32*)(&time1) > *(u32*)(&time2))
  {
    ret = 1;
  }
  else if(*((u32*)(&time1) + 1) < *((u32*)(&time2) + 1))
  {
    ret = -1;
  }
  else if(*((u32*)(&time1) + 1) > *((u32*)(&time2) + 1))
  {
    ret = 1;
  }*/

  if(time1.year > time2.year)
  {
    ret = 1;
  }
  else if(time1.year < time2.year)
  {
    ret = -1;
  }
  else if(time1.month > time2.month)
  {
    ret = 1;
  }
  else if(time1.month < time2.month)
  {
    ret = -1;
  }  
  else if(time1.day > time2.day)
  {
    ret = 1;
  }
  else if(time1.day < time2.day)
  {
    ret = -1;
  }    
  else if(time1.hour > time2.hour)
  {
    ret = 1;
  }
  else if(time1.hour < time2.hour)
  {
    ret = -1;
  }
  else if(time1.minute > time2.minute)
  {
    ret = 1;
  }
  else if(time1.minute < time2.minute)
  {
    ret = -1;
  }
  else if(time1.second > time2.second)
  {
    ret = 1;
  }
  else if(time1.second < time2.second)
  {
    ret = -1;
  }
  
  return ret;
}

/*!
   compare times,
  \param[in/out] src
  \param[in] add
 */
void time_add(utc_time_t *p_time_src, utc_time_t *p_time_add)
{
  u32 s = 0;
  s = (((p_time_add->day * HOUR_PER_DAY)
        + p_time_add->hour) * MINUTE_PER_HOUR 
        + p_time_add->minute)* SECOND_PER_MINUTE 
        + p_time_add->second;
  time_up(p_time_src, s);
}

u32 time_conver(utc_time_t *p_dur_time)
{
  u32 sec = 0;
  
  MT_ASSERT(0 == p_dur_time->year && 0 == p_dur_time->month);
  sec = ((p_dur_time->day * HOUR_PER_DAY + p_dur_time->hour)
          * MINUTE_PER_HOUR + p_dur_time->minute)
            *SECOND_PER_MINUTE + p_dur_time->second;
  return sec;
}

u32 time_dec(utc_time_t *p_time_src, utc_time_t *p_time_dec)
{
  u32 day_src = 0;
  u32 day_dec = 0;
  utc_time_t dur_time = {0};
  u32 day_result = 0;
  u32 temp = 0;

  MT_ASSERT(time_cmp(p_time_src, p_time_dec, FALSE) >= 0);
  //fix bug 33547
  //MT_ASSERT(p_time_src->year - p_time_dec->year <= 100);

  //calc day  
  day_src = date_to_day(p_time_src);
  day_dec = date_to_day(p_time_dec);
  day_result = (day_src - day_dec);

  //calc hour
  if(p_time_src->hour >= p_time_dec->hour)
  {
    dur_time.hour = p_time_src->hour - p_time_dec->hour;
  }
  else
  {
    dur_time.hour = p_time_src->hour - p_time_dec->hour + HOUR_PER_DAY;
    
    MT_ASSERT(day_result > 0);
    day_result--;
  }

  //calc min
  if(p_time_src->minute >= p_time_dec->minute)
  {
    dur_time.minute = p_time_src->minute - p_time_dec->minute;
  }
  else
  {
    dur_time.minute =
      p_time_src->minute - p_time_dec->minute + MINUTE_PER_HOUR;
    if(dur_time.hour > 0)
    {
      dur_time.hour--;
    }
    else
    {
      MT_ASSERT(day_result > 0);
      day_result--;
      dur_time.hour = (HOUR_PER_DAY - 1);
    }
  }
  
  //calc sec
  if(p_time_src->second >= p_time_dec->second)
  {
    dur_time.second = p_time_src->second - p_time_dec->second;
  }
  else
  {
    dur_time.second =
      p_time_src->second - p_time_dec->second + SECOND_PER_MINUTE;
    if(dur_time.minute > 0)
    {
      dur_time.minute--;
    }
    else
    {
      if(dur_time.hour > 0)
      {
        dur_time.hour--;
        dur_time.minute = (MINUTE_PER_HOUR - 1);
      }
      else
      {
        MT_ASSERT(day_result > 0);
        day_result--;
        dur_time.hour = (HOUR_PER_DAY - 1);
        dur_time.minute = (MINUTE_PER_HOUR - 1);
      }
    }
  }
  
  temp = time_conver(&dur_time);

  return (temp + (day_result * HOUR_PER_DAY * MINUTE_PER_HOUR * SECOND_PER_MINUTE));
}

void time_to_local(utc_time_t *p_gmt_time, utc_time_t *p_loc_time)
{
  time_info_t *p_info = (time_info_t *)class_get_handle_by_id(TIME_CLASS_ID);
  s32 offset = 0;
  s32 sec_of_zone = MINUTE_PER_HOUR * SECOND_PER_MINUTE / 2;
  
  //p_loc_time->value = p_gmt_time->value;
  memcpy(p_loc_time, p_gmt_time, sizeof(utc_time_t));
  
  //calculate summer time
  if(p_info->is_summer_time)
  {
    offset += MINUTE_PER_HOUR * SECOND_PER_MINUTE;
  }

  //calculate time zone
  offset += sec_of_zone * p_info->time_zone;

  while(offset < 0)
  {
    day_reduce(p_loc_time);
    offset += HOUR_PER_DAY * MINUTE_PER_HOUR * SECOND_PER_MINUTE;
  }

  time_up(p_loc_time, offset);
}

void time_to_gmt(utc_time_t *p_loc_time, utc_time_t *p_gmt_time)
{
  time_info_t *p_info = (time_info_t *)class_get_handle_by_id(TIME_CLASS_ID);
  s32 offset = 0;
  s32 sec_of_zone = MINUTE_PER_HOUR * SECOND_PER_MINUTE / 2;
  
  //p_gmt_time->value = p_loc_time->value;
  memcpy(p_gmt_time, p_loc_time, sizeof(utc_time_t));

  //calculate summer time
  if(p_info->is_summer_time)
  {
    offset -= MINUTE_PER_HOUR * SECOND_PER_MINUTE;
  }

  //calculate time zone
  offset -= sec_of_zone * p_info->time_zone;
  
  while(offset < 0)
  {
    day_reduce(p_gmt_time);
    offset += HOUR_PER_DAY * MINUTE_PER_HOUR * SECOND_PER_MINUTE;
  }

  time_up(p_gmt_time, offset);
}

/*!
  Printf time
  \param[in] p_time input time to be printed
  \param[in] string information to be printed
  */
void printf_time(utc_time_t *p_time, char *p_str)
{
  OS_PRINTF("%s:  %d-%d-%d, %.2d:%.2d:%.2d\n", p_str,
    p_time->year, 
    p_time->month, 
    p_time->day, 
    p_time->hour,
    p_time->minute, 
    p_time->second);
}

/*!
  to judge is time value is 0
  \param[in] p_time input time to be judged
  \return TRUE for yes, otherwise no.
  */
BOOL is_time_zero(utc_time_t *p_time)
{
  utc_time_t tmp = {0};

  return (memcmp(&tmp, p_time, sizeof(utc_time_t)) == 0)?TRUE:FALSE;
}

