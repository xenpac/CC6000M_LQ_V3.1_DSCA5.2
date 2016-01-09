/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SUBT_STATION_OUTPUT_PIN_H__
#define __SUBT_STATION_OUTPUT_PIN_H__


/*!
  the pes pin report structure.
  */
typedef struct tag_subt_station_out_report
{
  /*!
    dummy.
    */
  u32 dummy;
}subt_station_out_report_t;


/*!
  tag_subt_out_pin_private
  */
typedef struct tag_subt_station_out_pin_private
{
  /*!
    this point !!
    */
  u8 *p_this;
  /*!
    out sample 
    */
  media_sample_t *p_out_sample;
  /*!
    out buf 
    */
  u8 *p_out_buf;
  /*!
    out buf size
    */
  u32 buf_size;
  /*!
    define format
    */
  allocator_properties_t properties;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
  /*!
    report for debug
    */
  subt_station_out_report_t report;
  /*!
    last_start pts
    */
  u32 last_start;
  /*!
    last_start pts
    */
  u32 last_end;
  /*!
    align
    */
  str_align_t align;
}subt_station_out_pin_priv_t;



/*!
  the subt output pin define
  */
typedef struct tag_subt_station_out_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER transf_output_pin_t m_pin;
  /*!
    private data buffer
    */
  subt_station_out_pin_priv_t private_data;
}subt_station_out_pin_t;

/*!
  subt_satation_out_get_showed_time
  \param[out] 
  \param[in] 
  \return 
  */
u32 subt_satation_out_get_showed_time(handle_t _this, u32 *p_start, u32 *p_end);

/*!
  subt_station_out_show
  \param[out]
  \param[in] 
  \return 
  */
BOOL subt_station_out_show(handle_t _this, void *p_data);

/*!
  subt_station_out_hide
  \param[out] 
  \param[in] 
  \return 
  */
void subt_station_out_hide(handle_t _this, void *p_data);

/*!
  subt_station_out_config
  \param[out] 
  \param[in] 
  \return 
  */
void subt_station_out_config(handle_t _this, str_align_t align);
/*!
  create a pin
  \param[out] p_pin: the out pin handle.
  \param[in] p_owner: filter handle.
  \return return the instance of subt_out_pin_t
  */
subt_station_out_pin_t * subt_station_out_pin_create(
              subt_station_out_pin_t *p_pin, interface_t *p_owner);


#endif

