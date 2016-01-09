/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TOT_H_
#define __TOT_H_
/*!
  Max time region supported in TOT table
  */
#define MAX_TIME_REGION_NUM (24)
/*!
  Local time offeset descriptor
  */
typedef struct
{
  /*!
    Region id
    */    
  u8 region_id;
  /*!
    Local time offset polarity
    */
  u8 local_time_offset_polar ;
  /*!
    Local time offset
    */
  utc_time_t local_time_offset  ;
  /*!
    Time of change
    */
  utc_time_t tmofchge;  
  /*!
    Next time offset
    */
  utc_time_t next_time_offset ;
}local_tm_oft_des_t;

/*!
  TOT table definition
  */
typedef struct
{
  /*!
    Offset description number
    */  
  u8 oft_num;
  /*!
    UTC time 
    */
  utc_time_t tm;
  /*!
    Max time region supported
    */
  local_tm_oft_des_t tmoft[MAX_TIME_REGION_NUM] ;
}tot_t;

/*!
  Parse tot section.
  \param[in] p_sec: section data filter by PTI driver.
  */
void parse_tot(handle_t handle, dvb_section_t *p_sec);

/*!
  Request a TOT section data.

  \param[in] p_sec: tot section information for PTI to get TOT table
  \param[in] param: request parameter information
  */
void request_tot_section(dvb_section_t *p_sec, u32 param);


#endif // End for __TOT_H_

