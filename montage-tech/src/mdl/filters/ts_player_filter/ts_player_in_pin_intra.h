/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TS_PLAYER_IN_PIN_INTRA_H_
#define __TS_PLAYER_IN_PIN_INTRA_H_

/*!
  ts player input pin private data
  */
typedef struct tag_ts_player_in_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    mem alloc interface
    */
  iasync_reader_t *p_reader;
}ts_player_in_pin_private_t;

/*!
  the ts player input pin
  */
typedef struct tag_ts_player_in_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_input_pin_t m_pin;
  /*!
    private data
    */
  ts_player_in_pin_private_t private_data;
  /*!
    request sample
    */
  void (*request_sample)(handle_t _this, media_format_t *p_format,
                              u32 fill_len, s64 position, u32 seek_op);
}ts_player_in_pin_t;

/*!
  create a ts player input pin
  \param[in] p_pin ts player input pin
  \param[in] p_owner ts player input pin's owner: ts player filter
  \return return the instance of ts_player_in_pin_t
  */
ts_player_in_pin_t * ts_player_in_pin_create(ts_player_in_pin_t *p_pin, interface_t *p_owner);

#endif // End for __TS_PLAYER_IN_PIN_INTRA_H_

