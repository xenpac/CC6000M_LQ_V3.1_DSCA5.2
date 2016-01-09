/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TOT_ELI_H_
#define __TOT_ELI_H_
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
}tot_eli_t;

/*!
  parse TOT table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE tot_table_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 * p_output);

/*!
  pat table request param

  \param[in] p_param : pat table request static parameters
  */
void tot_table_request(dvb_request_param_t *p_param);



#endif // End for __TOT_ELI_H_

