/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LRC_OUTPUT_PIN_INTRA_H_
#define __LRC_OUTPUT_PIN_INTRA_H_


/*!
  private data length
  */
typedef struct tag_lrc_out_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
}lrc_out_pin_private_t;

/*!
  the record output pin
  */
typedef struct tag_lrc_out_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data buffer
    */
  lrc_out_pin_private_t private_data;
}lrc_out_pin_t;


/*!
  create a lrc output pin
  \param[in] p_pin lrc output pin
  \param[in] p_owner lrc output pin's owner: lrc filter
  \return return the instance of rec_out_pin_t
  */
lrc_out_pin_t * lrc_out_pin_create(lrc_out_pin_t *p_pin, interface_t *p_owner);

#endif // End for __RECORD_OUTPUT_PIN_INTRA_H_

