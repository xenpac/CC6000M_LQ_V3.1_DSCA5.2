/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVB_TRANSF_FILTER_H_
#define __DVB_TRANSF_FILTER_H_

/*!
  dvb transfer filter supports :
  1. do command has config, request and free CMD.
  2. downriver filter can get interface, and interface supports request and free.
  3. can attached user-defined table parse function. 
     Refer to the dvb_filter_cfg_t. this structure has p_table_list, and table_attach_list.
  */

/*!
  defines interface name macro
  */
#define DVB_TRANSF_INTERFACE_NAME    "dvb_transf_i"

/*!
 dvb filter config.
 */
typedef struct tag_dvb_filter_cfg
{
 /*!
  max request table at the same time.
  */
  u16                 max_request_table;
 /*!
  total attached table
  if you want to user-defined parse function, you need config.
  else total_attach_table = 0.
  */
  u16                     total_attach_table;
 /*!
  table array includes which table will be register.
  if you want to user-defined parse function, you need config.
  else p_attach_list = NULL.
  */
  dvb_attach_table_t       *p_table_list;
}dvb_filter_cfg_t;


/*!
  dvb filter request parameter. 
  */
typedef struct tag_dvb_filter_request
{
 /*!
  media type 
  */ 
  media_type_t     media_type;
 /*!
  dmx request mode.
  */
  demux_req_mode_t mode;
 /*!
  PSI pid.
  */
  u16 pid;
 /*!
  table id
  */
  u16 table_id;
 /*!
  custom data1, decide by the user. for example, pmt table sid.
  */
  u32 data1;
 /*!
  custom data2, decide by the user.
  */  
  u32 data2;
 /*!
  extern buffer address 
  */
  u8 *p_ext_buf;
 /*!
  extern buffer size
  */
  u32 buf_size;
}dvb_filter_request_t;


/*!
  dvb filter free para.
 */
typedef struct tag_dvb_filter_free
{
 /*!
  media type 
  */ 
  media_type_t     media_type;
 /*!  
  dmx request mode
  */
  demux_req_mode_t mode;
 /*!
  PSI pid.
  */
  u16              psi_pid;
 /*!  
  table id.
  */
  u16 table_id;
}dvb_filter_free_t;


/*!
  dvb transf interface
  */
typedef struct tag_dvb_transf_interface
{
  /*!
    point to the owner
    */
  PRIVATE interface_t *p_owner;

  /*!
    request table

    \param[in] _this dmx pin handle
    \param[in] p_req pointer request info 
    */
  RET_CODE (*request_table)(handle_t _this, dvb_filter_request_t *p_request);

  /*!
    free table

    \param[in] _this dmx pin handle
    \param[in] p_free pointer free info 
    */
  RET_CODE (*free_table)(handle_t _this,  dvb_filter_free_t *p_free);
}dvb_transf_interface_t;


/*!
 defines dvb transfer filter command type. 
 */
typedef enum 
{
 /*!
  config dvb filter. 
  */
  DVB_FILTER_CONFIG,
 /*!
  dvb filter request table.  
  */
  DVB_FILTER_REQUEST_TABLE,
 /*!
  dvb filter free table.
  */
  DVB_FILTER_FREE_TABLE,
 /*!
  dmx cmd unknown.
  */
  DVB_FILTER_CMD_UNKNOWN,
}dvb_transf_filter_cmd_t;

/*!
  create a dvb transf filter instance

  \return return the instance of filter
  */
ifilter_t *dvb_transf_filter_create(void *p_para);

#endif // End for __DVB_TRANSF_FILTER_H_
