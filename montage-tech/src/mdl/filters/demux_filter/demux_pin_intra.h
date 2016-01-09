/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DMX_PIN_INTRA_H_
#define __DMX_PIN_INTRA_H_

/*!
  defines demux pin(father class) private data
  */
typedef struct tag_demux_pin_private_data
{
/*!
 member of idemux_pin
 */
 dmx_interface_t m_dmx_inter; 
}demux_pin_private_t;


/*!
  the demux pin define
  */
typedef struct tag_dmx_pin
{
 /*!
  public base class, must be the first member
  */
  FATHER source_pin_t m_pin;
 
 /*!
  demux pin private data buffer(father class)
  */
  demux_pin_private_t dmx_pin_priv;
 
 /*!
  child pin private data buffer(derive class)
  */ 
  void *p_child_priv;

/*!
 ts in index
 */
 //u8 ts_in;
 
 /*!
  set pin type
  */
  BOOL (*set_pin_type)(handle_t _this, dmx_pin_type_t type);

 /*!
  config parameter
  */

  RET_CODE (*config)(handle_t _this, void *p_para);

 /*!
  request 
  */
  RET_CODE (*request)(handle_t _this, void *p_data);

 /*!
  free
  */
  RET_CODE (*free)(handle_t _this, void *p_data);
 /*!
  reset_pid 
  */
  RET_CODE (*reset_pid)(handle_t _this, void *p_data);
  /*!
    pause resume
    */
  RET_CODE (*pause_resume)(handle_t _this, BOOL b_resume);
}demux_pin_t;

/*!
 attach rec pin 

 \return return the instance of demux_pin_t 
 */
demux_pin_t * attach_rec_pin_instance(handle_t _this);

/*!
 attach psi pin 

 \return return the instance of demux_pin_t 
 */
demux_pin_t * attach_psi_pin_instance(handle_t _this);

/*!
 attach pes pin 

 \return return the instance of demux_pin_t 
 */
demux_pin_t * attach_pes_pin_instance(handle_t _this);

/*!
  create a pin

  \return return the instance of demux_pin_t
  */
demux_pin_t * demux_pin_create(demux_pin_t *p_dmx_pin, interface_t *p_owner);

#endif // End for __DMX_PIN_INTRA_H_

