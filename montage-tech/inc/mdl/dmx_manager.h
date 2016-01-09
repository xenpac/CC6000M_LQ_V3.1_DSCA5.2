/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DMX_MANAGER_H_
#define __DMX_MANAGER_H_

/*!
  \file: dmx_manager.h
  1. This file provides the interface for all operation to dmx manager.
  2. dmx manager must supports multi-task, 
     so open and close function must are atomic operation.
  */


#if 0

/*!
  defines dmm invalid road id.
  */
#define DMM_INVALID_ROAD_ID    0xFFFFFFFF

/*!
  defines dmm road id.
  */
typedef u32 dmm_rd_id_t;

/*!
  This structure defines the chan type of the dmx manager 
  */
typedef enum tag_dmm_ret
{
/*! 
 dmx manager success
 */
 DMM_SUCCESS = 0x00,
/*! 
 dmx manager failure
 */
 DMM_FAILURE = 0x01,
/*!
 dmx manager full
 */
 DMM_FULL    = 0x02,
/*!
 dmx mananger unknown
 */
 DMM_UNKNOWN = 0x03,
}dmm_ret_t;

/*!
  This structure defines the road type of the dmx manager 
  */
typedef enum tag_dmm_road_type
{
 /*! 
  recode type of dmx manager
  */
  DMM_REC_TYPE = 0x00,
 /*! 
  PSI type of dmx manager
  */
  DMM_PSI_TYPE = 0x01,
 /*!
  PES type of dmx manager
  */
  DMM_PES_TYPE = 0x02,
 /*!
  end type of dmx manager
  */
  DMM_END_TYPE,
}dmm_road_type_t;

/*!
  This structure defines the chan paramter of the dmx manager
  */
typedef struct tag_dmm_open_param
{
 /*! 
  slot pid
  */
  u16 pid;
 /*! 
  TSI input type
  */
  dmx_input_type_t stream_in;
 /*! 
  TS type : only uses psi or pes
  */
  dmx_ch_type_t ch_type;
 /*! 
  record select : only uses recode
  */
  dmx_rec_path_t rec_path;
 /*! 
  TS type : only uses recode.
  */
  dmx_rec_mode_t rec_mode;
}dmm_open_param_t;

/*!
  This structure defines the chan paramter of the dmx manager
  */
typedef struct tag_dmm_set_si_param
{
 /*!
  is extern buffer.(use for DMM_PSI_TYPE)
  */
  BOOL is_ext_buf;
 /*!
  the buffer pointer
  */
  u8 *p_buf; 
 /*!
  the buffer size
  */
  u32 size;
 /*!
  filter setting : only uses psi
  */
  dmx_filter_setting_t filter_set;
}dmm_set_si_param_t;

/*!
 defines dmx manager base class 
 */
typedef struct tag_dmm_handle
{
 /*!
  dmx manager private data.
  */
  void *p_priv_data;

 /*!
  open channel 

  \param[in] _this : dmx manager handle.
  \param[in] road_type : dmx manager road type.
  \param[in] p_para : channel data.
  \param[out] p_rd_id : return dmm element id.
  */
  dmm_ret_t (*open_road)(handle_t _this, dmm_road_type_t road_type, dmm_open_param_t *p_para, 
    dmm_rd_id_t *p_rd_id);

 /*!
  start channel 

  \param[in] _this : dmx manager handle.
  \param[in] road_type : dmx manager road type
  \param[in] rd_id the road id.
  */
  void (*start_road)(handle_t _this, dmm_road_type_t road_type, dmm_rd_id_t rd_id);

 /*!
  stop channel 

  \param[in] _this : p_handle dmx manager handle
  \param[in] road_type : dmx manager road type
  \param[in] rd_id the road id.
  */
  void (*stop_road)(handle_t _this, dmm_road_type_t road_type, dmm_rd_id_t rd_id);

 /*!
  close channel 

  \param[in] _this : p_handle dmx manager handle
  \param[in] road_type : dmx manager road type
  \param[in] rd_id the road id.
  */
  void (*close_road)(handle_t _this, dmm_road_type_t road_type, dmm_rd_id_t rd_id);

 /*!
  set recode channel
 
  \param[in] _this : p_handle dmx manager handle
  \param[in] rec_path : recode path
  \param[in] p_buff : the buffer pointer
  \param[in] buf_size : the buffer size
  */
  RET_CODE (*set_rec_road)(handle_t _this, dmx_rec_path_t rec_path, u8 *p_buff, u32 buf_size);
 
 /*!
   get recode channel
   
  \param[in] _this : dmx manager handle
  \param[in] rec_path : dmx recoad path
  \param[out] p_bf_size : buffer size.
  */
  RET_CODE (*get_rec_buff_size)(handle_t _this, dmx_rec_path_t rec_path, u64 *p_bf_size);

/*!
  set channel(pes or psi)

  \param[in] _this : dmx manager handle
  \param[in] road_type : dmx mamanger chan type
  \param[in] rd_id : road id
  \param[out] p_para : the si parameter pointer
  */
  RET_CODE (*set_si_road)(handle_t _this, dmm_road_type_t road_type, dmm_rd_id_t rd_id, 
    dmm_set_si_param_t *p_para);

/*!
  get si channel buff(pes and psi) 
  
  \param[in] _this : dmx manager handle
  \param[in] road_type : channel type
  \param[in] rd_id : road id
  \param[out] pp_buf : p_buffer address.
  \param[out] p_size : data size.
  */
  RET_CODE (*get_si_data)(handle_t _this, dmm_road_type_t road_type, dmm_rd_id_t rd_id,
    u8 **pp_buf, u32 *p_size);
}dmm_handle_t;

#endif

/*!
  dmx manager initialize parameter
  */
typedef struct tag_dmm_init_para
{
/*!
  how many dmx channel uses for recode , psi or pes?    
  */
  u16 channel_num;
}dmm_init_para_t;


/*!
  dmx manager initialize
  \param[in] p_para pointer dmx manager parameter.
  */
void dmm_init(dmm_init_para_t *p_para);
  
#endif //_DMX_MANAGER_H_
