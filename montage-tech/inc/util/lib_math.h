/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_MATH_H__
#define __LIB_MATH_H__

/*!
   u_sin
   Calculate sines
   Paramters
   angle 	[in] the integer scaled angle.
    Angel range [0,360] is equably scaled in integer range [0,0x7fff].
    For example, integer 0 means angel 0, integer 0x4000 means angel
    90, integer 0x8000 means angel 180, and integer 0xC000 means angel
    270.
   Return Values:
   u_sin returns the integer scaled sine values of angle. The integer return value
   scales sine valeu from rang [-1,1] to range [-0x7FFF,0x7FFF]. For examples,
   return value -0x7FFF means real value 1, return value 0x7FFF means real value 1.
 */
s16 u_sin(s16 angle);

double f_sin(double angle_in_radians);


/*!
   u_cos
   Calculate cosine
   Paramters
   angle 	[in] the integer scaled angle.
    please refer to u_sin function.
   Return Values:
   u_sin returns the integer scaled sine values of angle.
   please refer to u_sin function.
 */
s16 u_cos(s16 angle);

double f_cos(double angle_in_radians);


/*!
   u_acos
   Calculate cosine
   Paramters
   angle 	[in] the integer scaled angle.
    please refer to u_sin function.
   Return Values:
   u_sin returns the integer scaled sine values of angle.
   please refer to u_sin function.
 */
s16 u_acos(s16 x);

double f_acos(double x);

/*!
   u_cos
   Calculate cosine
   Paramters
   angle 	[in] the integer scaled angle.
    please refer to u_sin function.
   Return Values:
   u_sin returns the integer scaled sine values of angle.
   please refer to u_sin function.
 */
s16 u_atan(s16 x, s16 y);


/*!
   u_cos
   Calculates the square root.
   Paramters
   angle 	[in] the integer scaled angle.
    please refer to u_sin function.
   Return Values:
   u_sin returns the integer scaled sine values of angle.
   please refer to u_sin function.
 */
s16 u_sqrt(s16 x);

#endif

