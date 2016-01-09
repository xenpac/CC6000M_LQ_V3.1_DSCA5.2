/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DSP_H__
#define __DSP_H__


/*!
  This the mode of dsp
  */
typedef enum
{
  /*!
    This the real data mode of dsp
    */
  DSP_REAL_MODE,

  /*!
    This the imaginary data mode of dsp
    */
  DSP_IMAGINARY_MODE,


  /*!
    This Mode Max Value
    */
  DSP_MODE_MAX
}dsp_mode_t;


/*!
  This the operation cmd of dsp
  */
typedef enum
{
  /*!
     A + B * C >> M
    */
  DSP_ADD_CMD,     

  /*!
      SUM (B[n] * C[n])
    */
  DSP_ACC_CMD,

  /*!
     A - B * C >> M
    */
  DSP_SUB_CMD,


}dsp_cmd_t;


/*!
  The calc group of dsp
  */
typedef enum
{
  /*!
    The group 0 of dsp
    */
  DSP_GROUP_0,

  /*!
    The group 1 of dsp
    */
  DSP_GROUP_1,

  /*!
    The group MAX
    */
  DSP_GROUP_MAX,
}dsp_group_t;



/*!
  The calc parameter of dsp
  */
typedef struct
{
  /*!
    The calc parameter of dsp: a real part
    */
  s32 real_a;

  /*!
    The calc parameter of dsp: a imaginary part
    */
  s32 img_a;

  /*!
    The calc parameter of dsp: b real part
    */
  s32 real_b;

  /*!
    The calc parameter of dsp: b imaginary part
    */
  s32 img_b;

  /*!
    The calc parameter of dsp: c real part
    */
  s32 real_c;

  /*!
    The calc parameter of dsp: c imaginary part
    */
  s32 img_c;

  /*!
  The calc parameter of dsp: max value
  */
  s32 max_value;

  /*!
    The calc parameter of dsp: min value
    */
  s32 min_value;


  /*!
    The calc parameter of dsp: b data buf start addr
    */
  u32 b_buf;

  /*!
    The calc parameter of dsp: b data step 
    */
  u32 b_step;

  /*!
    The calc parameter of dsp: b data step ascending order or descending order
    */
  u32 b_increase;

  /*!
  The calc parameter of dsp: c data buf start addr
  */
  u32 c_buf;

  /*!
    The calc parameter of dsp: c data step 
    */
  u32 c_step;

  /*!
    The calc parameter of dsp: c data step ascending order or descending order
    */
  u32 c_increase;

  /*!
    The calc parameter of dsp: c data base buffer
    */
  u32 c_base_buf;

  /*!
    The calc parameter of dsp: c data base buffer size
    */
  u32 c_buf_size;

  /*!
    The calc parameter of dsp: right shift number
    */
  u32 r_shift;

  /*!
    The calc parameter of dsp: muitl cunt
    */
  u32 mult_cunt;

  /*!
    The calc parameter of dsp: indicate the c is conjugate or not
    */
  BOOL c_conjugate; 
  
}dsp_param_t;


/*!
  The calc result of dsp
  */
typedef struct
{
  /*!
    The calc parameter of dsp: the result data real part low 32 bit
    */
  u32 real_lo;

  /*!
    The calc parameter of dsp: the result data real part high 32 bit
    */
  u32 real_hi;

  /*!
    The calc parameter of dsp: the result data imaginary part low 32 bit
    */
  u32 img_lo;

  /*!
     The calc parameter of dsp: the result data imaginary part high 32 bit
    */
  u32 img_hi;
  
}dsp_out_t;


/*!
   use the dsp to do a operation

  \param[in] p_dev: the dsp_device_t object
  \param[in] mod: indicate the dsp mode
  \param[in] cmd: indicate the dsp operation
  \param[in] grp: indicate the reg group of dsp
  \param[in] p_param:  parameters for dsp operation
  \param[out] p_out: output the result
  */
BOOL dsp_operation(void *p_dev,
                   dsp_mode_t mod, 
                   dsp_cmd_t cmd, 
                   dsp_group_t grp, 
                   dsp_param_t *p_param, 
                   dsp_out_t *p_out);


#endif  // __DSP_H__





