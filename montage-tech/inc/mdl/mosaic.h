/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVB_MOSAIC_H_
#define __DVB_MOSAIC_H_

/*!
  mosaic cell link bouquet
  */
#define MOSAIC_LINK_BOUQUET    (0x01)
/*!
  mosaic cell link service
  */
#define MOSAIC_LINK_SVC           (0x02)
/*!
  mosaic cell link mosaic
  */
#define MOSAIC_LINK_MOSAIC    (0x03)
/*!
  mosaic cell link event
  */
#define MOSAIC_LINK_EVENT       (0x04)

/*!
  mosaic svc link up
  */
#define MOSAIC_LINK_SVC_UP       (0x82)
/*!
  mosaic svc link down
  */
#define MOSAIC_LINK_SVC_DOWN       (0x83)
/*!
  mosaic cell max
  */
#define MOSAIC_CELL_MAX       (64)

/*!
  mosaic logical cell
  */
typedef struct tag_mosaic_logic_cell
{
  /*!
    logical cell id
    */
  u8 logic_cell_id;
  /*!
    logical cell presentation_info
    */
  u8 logical_cell_info;
  /*!
    cell linkage info
    */
  u8 cell_link_info;
  /*!
    top left elementary cell id
    */
  u8 top_left_e_cell_id;
  /*!
    bottom right elementary cell id
    */
  u8 bottom_right_e_cell_id;
  /*!
    audio stream
    */
  u16 audio_stream;
  /*!
    reserved
    */
  u8 reserved;
  /*!
    logic cell link
    */
  union
  {
    /*!
      MOSAIC_LINK_BOUQUET
      */
    struct
    {
      /*!
        bouquet id
        */
      u16 bouquet_id;
    }bqt;
    /*!
      MOSAIC_LINK_SVC
      */
    struct
    {
      /*!
        original network id
        */
      u16 orig_network_id;
      /*!
        stream id
        */
      u16 stream_id;
      /*!
        service id
        */
      u16 svc_id;
    }svc;
    /*!
      MOSAIC_LINK_MOSAIC
      */
    struct
    {
      /*!
        original network id
        */
      u16 orig_network_id;
      /*!
        stream id
        */
      u16 stream_id;
      /*!
        service id
        */
      u16 svc_id;
    }msc;
    /*!
      MOSAIC_LINK_EVENT
      */
    struct
    {
      /*!
        original network id
        */
      u16 orig_network_id;
      /*!
        stream id
        */
      u16 stream_id;
      /*!
        service id
        */
      u16 svc_id;
      /*!
        event id
        */
      u16 evt_id;
    }evt;
  }lnk;
}mosaic_logic_cell_t;

/*!
  novd reference descriptor
  */
typedef struct tag_nvod_reference_descr
{
    /*!
      original network id
      */
    u16 orig_network_id;
    /*!
      stream id
      */
    u16 stream_id;
    /*!
      service id
      */
    u16 svc_id;
    /*!
      reserved
      */
    u16 reserved;
}nvod_reference_descr_t;

/*!
  mosaic
  */
typedef struct tag_mosaic
{
  /*!
    mosaic entry point
    */
  u8 mosaic_entry_point;
  /*!
    number of horizontal elementary cells 
    */
  u8 h_e_cells;
  /*!
    number of vertical elementary cells 
    */
  u8 v_e_cells;
  /*!
    logical cell count
    */
  u8 lg_cell_cnt;
  /*!
    previous service id
    */
  u16 pre_svc_id;
  /*!
    next service id
    */
  u16 nxt_svc_id;
  /*!
    screen left
    */
  u16 screen_left;
  /*!
    screen top
    */
  u16 screen_top;
  /*!
    screen right
    */
  u16 screen_right;
  /*!
    screen bottom
    */
  u16 screen_bottom;
  /*!
    logical cell
    */
  mosaic_logic_cell_t lg_cell[MOSAIC_CELL_MAX];
}mosaic_t;


/*!
  parse mosaic descriptor

  \param[in] p_buf: descriptor buf
  \param[in] p_des p_des descriptor pointer
  */
void parse_mosaic_des(u8 *p_buf, mosaic_t *p_des);

/*!
  parse mosaic logical screen descriptor

  \param[in] p_buf: descriptor buf
  \param[in] p_des p_des descriptor pointer
  */
void parse_mosaic_logic_screen_des(u8 *p_buf, mosaic_t *p_des);

/*!
  parse mosaic linkage descriptor

  \param[in] p_buf: descriptor buf
  \param[in] p_des p_des descriptor pointer
  */
void parse_mosaic_linkage_des(u8 *p_buf, mosaic_t *p_des);

/*!
  alloc_mosaic_buf
  \param[in] handle: nvod mosaic handle
  \param[in] svc_id desc svc id
  */
mosaic_t * alloc_mosaic_buf(class_handle_t handle, u16 svc_id);

/*!
  alloc_nvod_buf
  \param[in] handle: nvod mosaic handle
  \param[in] cnt: nvod desc cnt
  */
nvod_reference_descr_t *alloc_nvod_buf(class_handle_t handle, u8 cnt);

/*!
  nvod_mosaic_buf_reset
  \param[in] handle: nvod mosaic handle
  */
void nvod_mosaic_buf_reset(class_handle_t handle);

/*!
  nvod_mosaic_buf_init
  \param[in] buf_size: buf_size for nvod mosaic
  */
void nvod_mosaic_buf_init(u32 buf_size);

#endif // End for __DVB_MOSAIC_H_

