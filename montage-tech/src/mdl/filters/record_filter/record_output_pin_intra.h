/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RECORD_OUTPUT_PIN_INTRA_H_
#define __RECORD_OUTPUT_PIN_INTRA_H_

/*!
  record output pin private data structure
  */
typedef struct tag_rec_out_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
}rec_out_pin_private_t;

/*!
  the record output pin
  */
typedef struct tag_rec_out_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data
    */
  rec_out_pin_private_t private_data;
}rec_out_pin_t;

/*!
  create a record output pin
  \param[in] p_pin record output pin
  \param[in] p_owner record output pin's owner: record filter
  \return return the instance of rec_out_pin_t
  */
rec_out_pin_t * record_out_pin_create(rec_out_pin_t *p_pin, interface_t *p_owner);

#endif // End for __RECORD_OUTPUT_PIN_INTRA_H_

