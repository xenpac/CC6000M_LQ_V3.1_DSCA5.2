/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TS_PLAYER_OUT_PIN_INTRA_H_
#define __TS_PLAYER_OUT_PIN_INTRA_H_

/*!
  ts player video output pin private data structure
  */
typedef struct tag_ts_player_out_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
}ts_player_out_pin_private_t;

/*!
  the ts player video output pin
  */
typedef struct tag_ts_player_out_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data
    */
  ts_player_out_pin_private_t private_data;
}ts_player_out_pin_t;

/*!
  create a ts player output pin
  \param[in] p_pin record output pin
  \param[in] p_owner record output pin's owner: record filter
  \return return the instance of ts_player_out_pin_t
  */
ts_player_out_pin_t * ts_player_out_pin_create(ts_player_out_pin_t *p_pin, interface_t *p_owner);

#endif // End for __TS_PLAYER_OUT_PIN_INTRA_H_

