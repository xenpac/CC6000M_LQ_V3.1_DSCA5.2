/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __OTA_INPUT_PIN_INTRA_H_
#define __OTA_INPUT_PIN_INTRA_H_

/*!
 ota input pin private data.
 */
typedef struct tag_ota_input_pin_priv
{
/*!
 ota input pin handle.  
 */  
 void *p_this;
/*!
 dmx interface pointer.
 */
 dmx_interface_t *p_dmx_inter;
/*!
  ota medium
  */
  ota_medium_t medium;
  /*!
    stream protocol type
    */
  ota_stream_protocol_t protocol_type;
  /*!
    stream  dsmcc multi reqest bit
    */
  u16 dsmcc_req_multi_flag;
  /*!
    stream  dsmcc req table id
    */
  u16 dsmcc_req_table_id;
    /*!
    lib memp
    */
  lib_memp_t *p_mem_heap;   
  /*!
    use memory heap
    */
  BOOL use_memp;  
  /*!
    dsmcc filter buffer
    */
 u8 *p_filter_buffer;
/*!
  ts pid
  */
 u16 data_pid;
  /*!
    use demux
    */
  BOOL dmx_buf_size;  
  /*!
    use psi reqest data
    */
  psi_request_data_t req_data;
  /*!
    use psi free data
    */
  psi_free_data_t free_data;
  /*!
  ota dsmc_msg_timeout
  */
  u32 dsmcc_msg_timeout;
  /*!
  ota download direct;
  */
  dmx_filter_param_t  filter_param;
}ota_input_pin_priv_t;
/*!
 ota in pint reqest 
 */
typedef enum
{
  /*!
    ota in pint reqest dsi
    */
   REQUES_TABLE_DSMCC_MSG_DSI = 0,
  /*!
    ota in pint reqest dii
    */
   REQUES_TABLE_DSMCC_MSG_DII,
  /*!
    ota in pint reqest dsmcc dmm
    */
   REQUES_TABLE_DSMCC_DMM,
     /*!
    ota in pint reqest dsi by multi
    */
   REQUES_TABLE_DSMCC_MSG_DSI_MULTI,
  /*!
    ota in pint reqest dii  by multi
    */
   REQUES_TABLE_DSMCC_MSG_DII_MULTI,
}ota_input_reqest_em_t;
/*!
 ota in pint free 
 */
typedef enum
{
  /*!
    ota in pint free dsi
    */
   FREE_TABLE_DSMCC_MSG_DSI = 0,
  /*!
    ota in pint reqest dii
    */
   FREE_TABLE_DSMCC_MSG_DII,
  /*!
    ota in pint reqest dmm
    */
   FREE_TABLE_DSMCC_DMM,
     /*!
    ota in pint free dsi by multi
    */
   FREE_TABLE_DSMCC_MSG_DSI_MULTI,
  /*!
    ota in pint reqest dii by multi
    */
   FREE_TABLE_DSMCC_MSG_DII_MULTI,
}ota_input_free_em_t;

/*!
  input pin event. 
  */
typedef enum tag_input_pin_evt
{
/*!
   input pin event: unkown. 
  */
  OTA_INPUT_PIN_EVT_UNKOWN = 0,
/*!
   input pin event:receive data
  */
  OTA_INPUT_PIN_EVT_RECEIVE_DATA,
}ota_input_pin_evt_t;

/*!
 ota pin 
 */
typedef struct tag_ota_input_pin
{
 #ifndef OTA_FILTER_USE_FLASH_FILTER
/*!
  base class.
  */ 
  FATHER sink_pin_t m_sink_pin;
#else
/*!
  base class.
  */ 
  FATHER transf_input_pin_t m_sink_pin;
#endif
 /*!
  private data.
  */
  ota_input_pin_priv_t    m_priv_data;
  /*!
  config filter.
  */ 
  void (*config)(handle_t _this, ota_ipin_cfg_t *p_cfg);
/*!
   input pin request.
  */ 
  void (*ota_input_pin_reques)(handle_t _this,ota_input_reqest_em_t req_para, u32 param);
/*!
   input pin request.
  */ 
  void (*ota_input_pin_free)(handle_t _this,ota_input_free_em_t free_para, u32 param); 
}ota_input_pin_t;


/*!
 create ota input pin

 \param[in] p_pin : ota input pin handle.
 \param[in] p_owner : ota pin's owner.
 \return ota_pin_t handle
 */
ota_input_pin_t *ota_input_pin_create(ota_input_pin_t *p_pin, interface_t *p_owner);

#endif //End for __OTA_INPUT_PIN_INTRA_H_
