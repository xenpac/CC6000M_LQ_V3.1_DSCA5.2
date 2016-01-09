/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

#include "lib_usals.h"

/* radius of earth 6378 Km */
#define RADIUS_EARTH   6378.0
/* radius of geo-stationary orbit 42164.2 Km */
#define RADIUS_ORBIT   42164.2
/* circumference ratio */
#define PI             3.141592654

/*!
   calculate the angle to drive the motor.

   \param[in] sat_longitude satellite longitude, -180 to 100
   \param[in] local_longitude local longitude, -180 to 100
   \param[in] local_latitude local latitude, -90 to 90

   \return angle -65 to 65, otherwise is invalid value.
  */
double usals_calc_degree(double sat_longitude,
                            double local_longitude,
                            double local_latitude)
{
  double usals_value = 0;
  double ocb = 0, p_p1 = 0, c_p1 = 0;
  double p1_h = 0, ph = 0, ch = 0, bh = 0;
  double pb = 0, op = 0, ob = 0, dummy = 0, xx = 0;

  if(fabs(sat_longitude) > 180.0
    || fabs(local_longitude) > 180.0
    || fabs(local_latitude) > 90.0)
  {
    usals_value = 300.0;
    return usals_value;
  }

  ocb = fabs(sat_longitude - local_longitude);

  if(ocb > 180.0)
  {
    ocb = -(360.0 - ocb);
  }

  if(fabs(ocb) > 65.0)  //65.0	//The range DiSEqC can move was set at 37.5
  {
    // degree.
    usals_value = 300.0;
    return usals_value;
  }


  p_p1 = RADIUS_EARTH * sin(PI * local_latitude / 180.0);
  c_p1 = sqrt(pow(RADIUS_EARTH, 2) - pow(p_p1, 2));
  p1_h = c_p1 * sin(PI * ocb / 180.0);
  ph = sqrt(pow(p_p1, 2) + pow(p1_h, 2));
  ch = sqrt(pow(RADIUS_EARTH, 2) - pow(ph, 2));
  bh = RADIUS_ORBIT - ch;
  pb = sqrt(pow(bh, 2) + pow(ph, 2));
  op = sqrt(pow(RADIUS_ORBIT - c_p1, 2) + pow(p_p1, 2));
  /*FIXME: can't find sincos function in math. we remove it temporarily*/
#if 1
  ob = sqrt((2 * pow(RADIUS_ORBIT, 2)) - 
       (2 * pow(RADIUS_ORBIT, 2)) * sqrt(1 - pow(sin(PI * ocb / 180.0), 2)));
#endif
  dummy = (pow(pb, 2) + pow(op, 2) - pow(ob, 2)) / (2.0 * pb * op);
  dummy = acos(dummy);
  xx = dummy * 180.0 / PI;

  if(fabs(xx) > 65.0)
  {
    usals_value = 300.0;
    return usals_value;
  }

  xx = (float)((int)((xx + 0.05) * 10)) / 10.0;
  if(((sat_longitude < local_longitude) && (local_latitude >= 0.))
    || ((sat_longitude > local_longitude) && (local_latitude < 0.)))
  {
    /* do nothing */;
  }
  else
  {
    xx = 0 - xx;
  }

  if((sat_longitude < -89.9) && (local_longitude > 89.9))
  {
    xx = 0 - xx;
  }
  if((sat_longitude > 89.9) && (local_longitude < -89.9))
  {
    xx = 0 - xx;
  }

  usals_value = xx;

  return usals_value;
}


/*!
   convert degree to nim command's parameter

   \param[in] degree drive degree (double).
   \return command parameter.
  */
u16 usals_convert_degree(double degree)
{
  u16 degree_int = 0;
  u8 d = 0;
  float temp = 0;
  u16 result = 0;

  if(fabs(degree) >= 256)
  {
    result = 0xFFFF;
    return result;
  }

  if(degree < 0)
  {
    degree = 0 - degree;
    result = 0xe000;
  }
  else
  {
    result = 0xd000;
  }

  temp = (float)degree * 10;
  degree_int = (u16)temp;

  d = (u8)(degree_int / 160);
  result |= (d << 8);

  degree_int -= d * 160;
  d = (u8)(degree_int / 10);
  result |= (d << 4);

  degree_int -= d * 10;

  switch(degree_int)
  {
    case 0:
      result |= 0x00;
      break;
    case 1:
      result |= 0x02;
      break;
    case 2:
      result |= 0x03;
      break;
    case 3:
      result |= 0x05;
      break;
    case 4:
      result |= 0x06;
      break;
    case 5:
      result |= 0x08;
      break;
    case 6:
      result |= 0x0a;
      break;
    case 7:
      result |= 0x0b;
      break;
    case 8:
      result |= 0x0d;
      break;
    case 9:
      result |= 0x0e;
      break;
    default:
      /* do nothing */;
  }

  return result;
}
