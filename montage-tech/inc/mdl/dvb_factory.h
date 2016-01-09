/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVB_FACTORY_H_
#define __DVB_FACTORY_H_

/*!
 defines all request table parameter
 */
typedef struct tag_dvb_request_param
{
 /*!
  table id
  */  
  u16 table_id;
 /*!
  pid
  */
  u16 pid;
 /*!
  data1, decide by the user.
  */
  u32 data1;
 /*!
  data2, decide by the user.
  */  
  u32 data2;
 /*!
  time out. 
  */ 
  u32 timeout;
 /*!
  dmx channel type.  
  */
  dmx_ch_type_t ch_type;
 /*!
  need check crc.
  */ 
  BOOL crc_enable;
 /*!
  valid filter content mask size 
  */
  u8 filter_mask_size;
 /*! 
  Filter match code
  */
  u8 filter_code[DMX_SECTION_FILTER_SIZE];
 /*! 
  Filter mask value
  */
  u8 filter_mask[DMX_SECTION_FILTER_SIZE];
}dvb_request_param_t;

/*!
  defines all table parse param
  */
typedef struct tag_dvb_parse_param
{
 /*!
  table id
  */
  u16 table_id;
 /*!
  packet id
  */
  u16 pid;
 /*!
  data1, decide by the user.
  */
  u32 data1;
 /*!
  data2, decide by the user.
  */  
  u32 data2;
}dvb_parse_param_t;

/*!
 the declaration of get request table param callback funtion

 \p_param[in] p_param : dvb request parameter.
 */
typedef void (*request_param_proc_t)(dvb_request_param_t * p_param);

/*!
 the declaration of parse table callback function.

 \param[in] p_param : parse paramter.
 \param[in] p_input : dmx psi data.
 \param[out] p_output : dvb parsed data.
  */
typedef RET_CODE (*parse_table_proc_t)(dvb_parse_param_t *p_param, u8 *p_in, u8 *p_out);

/*!
 dvb register parameter data structrue
 */
typedef struct tag_dvb_attach_table
{
 /*!
  the register media type.
  */
  media_type_t media_type;
 /*!
  get table request param process.  
  */
  request_param_proc_t request_param_proc;
 /*!
  parse function process. 
  */
  parse_table_proc_t parse_table_proc;
}dvb_attach_table_t;

/*!
 dvb table factory initialize parameter.
 */
typedef struct tag_dvb_factory_param
{
 /*!
  number of table.
  */
  u8 num_of_table;
 /*!
  table array include which table will be register.
  */
  dvb_attach_table_t *p_table_array;
}dvb_factory_param_t;

/*!
  dvb table factory process 
  */
typedef struct tag_dvb_factory_proc_t
{
/*!
 dvb factory private data.
 */
 void *p_priv;

/*!
 fill request table parameter.

 \param[in] _this dvb factory handle
 \param[in] str_type stream type 
 \param[out] p_table_info output dvb table info
 */
 void (*fill_request_param)(handle_t _this, 
                            media_type_t media_type,
                            dvb_request_param_t *p_param);

/*!
 parse dmx data.
 
 \param[in] _this dvb factory handle 
 \param[in] str_type stream type
 \param[in] p_param prase parameter
 \param[in] p_in input has dmx psi data.
 \param[out] p_out ouput has parse info.
 */
 RET_CODE (*parse_table)(handle_t _this, 
                         media_type_t media_type,
                         dvb_parse_param_t *p_param,
                         u8 *p_in,
                         u8 *p_out);
}dvb_factory_proc_t;

/*!
  dvb table factory initialize.

  \param[in] p_param dvb factory parameter. 
  */
void dvb_factory_init(dvb_factory_param_t *p_param);

#endif //End for __DVB_FACTORY_H_

