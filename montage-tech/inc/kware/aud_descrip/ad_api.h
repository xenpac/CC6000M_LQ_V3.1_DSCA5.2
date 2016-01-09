/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AD_API_H__
#define __AD_API_H__


/*!
  ad_kernel_init

  \param[in] prio       task prio.
  \param[in] stack_size task stack size.
  \return.
  */
RET_CODE ad_kernel_init(u32 prio, u32 stack_size);
/*!
  aud_descrip_init

  \param[in] prio       task prio.
  \param[in] stack_size task stack size.
  \return.
  */
RET_CODE aud_descrip_init(u32 prio, u32 stack_size);

/*!
  aud_descrip_set_dmx_in_type

  \param[in] dmx_in_type
  \return.
  */
RET_CODE aud_descrip_set_dmx_in_type(u16 dmx_in_type);

/*!
  aud_descrip_set_pid_type

  \param[in] pid  ad stream pid
  \param[in] type audio format.
  \return.
  */
RET_CODE aud_descrip_set_pid_type(u16 pid, u16 type);

/*!
  aud_descrip_start

  \return.
  */
RET_CODE aud_descrip_start();

/*!
  aud_descrip_stop

  \return.
  */
RET_CODE aud_descrip_stop();

/*!
  aud_descrip set volume

  \return.
  */
RET_CODE aud_descrip_vol_set(u32 * voltype);


#endif
