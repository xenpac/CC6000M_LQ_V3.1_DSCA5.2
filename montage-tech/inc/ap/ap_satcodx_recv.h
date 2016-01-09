/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SATCODX_RECV_H_
#define __AP_SATCODX_RECV_H_

/*!
  \file ap_satcodx_parse.h
  This is the declearation of receive module in Satcodx app.
  */

/*!
  Read uart
  \param[in] pointer to satcodx implementation data
  \param[out] pointer to data which is read out
  \param[in] time_out interval
  */
BOOL ap_satcodx_recv_read_uart(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       u8 *p_ch,
                       u32 time_out);

/*!
  Allocate buffer for satcodx receive module
  \param[out] pointer to satcodx implementation data
  */
BOOL ap_satcodx_recv_alloc_buf(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Free buffer for satcodx receive module
  \param[out] pointer to satcodx implementation data
  */
void  ap_satcodx_recv_free_buf(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Allocate buffer for satcodx uart
  \param[out] pointer to satcodx implementation data
  */
BOOL ap_satcodx_recv_alloc_uart(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Free buffer for uart
  \param[out] pointer to satcodx implementation data
  */
void  ap_satcodx_recv_free_uart(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Init satcodx implementation data before receive module starts
  \param[out] pointer to satcodx implementation data
  */
void ap_satcodx_recv_init(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Deinit satcodx implementation data after receive module finishes
  \param[out] pointer to satcodx implementation data
  */
void ap_satcodx_recv_deinit(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Start satcodx receive module
  \param[out] pointer to satcodx implementation data
  */
void ap_satcodx_recv_start(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Stop satcodx receive module
  \param[out] pointer to satcodx implementation data
  */
void ap_satcodx_recv_stop(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Check sync byte
  \param[out] pointer to satcodx implementation data  
  */
BOOL ap_satcodx_recv_chk_sync_byte(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Transfer data to satcodx implementation data
  \param[out] pointer to satcodx implementation data
  */
BOOL ap_satcodx_recv_transfer_data(
                       satcodx_impl_data_t *p_satcodx_impl_data);

#endif // End for __AP_SATCODX_RECV_H_

