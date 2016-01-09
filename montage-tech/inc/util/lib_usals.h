/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LIB_USALS_H__
#define __LIB_USALS_H__

/*!
   calculate the angle to drive the motor.

   \param[in] sat_longitude satellite longitude, -180 to 100
   \param[in] local_longitude local longitude, -180 to 100
   \param[in] local_latitude local latitude, -90 to 90

   \return angle -90 to 90, otherwise is invalid value.
  */
double usals_calc_degree(double sat_longitude,
                            double local_longitude,
                            double local_latitude);

/*!
   convert degree to nim command's parameter

   \param[in] degree drive degree (double).
   \return command parameter.
  */
u16 usals_convert_degree(double degree);

#endif

