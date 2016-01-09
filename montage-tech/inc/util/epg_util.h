/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EPG_UTIL_H__
#define __EPG_UTIL_H__

/*!
  max length for epg type description.
  */
#define EPG_TYPE_MAX_LEN 64

/*!
  structure for epg type table
  */
typedef struct
{
  /*!
    epg type index.
    */
  u8 index;
  /*!
    epg type content.
    */
  u8 type[EPG_TYPE_MAX_LEN];
}epg_type_t;


/*!
  MTOS fifo structure
  */
typedef struct EPG_FIFO
{
  /*!
    pionter of fifo data
    */
  u32   *p_buffer; 
  /*!
    header of fifo
    */
  u32   m_head;
  /*!
    tail of fifo
    */
  u32   m_tail;
  /*!
    counter of fifo data
    */
  s32   m_cnt;
  /*!
    size of fifo buffer
    */
  u32   m_size;
  /*!
    overlay flag
    */
  BOOL  m_overlay;
}epg_fifo_t;

/*!
  interface for search the table.
  \param[in] index: type index.
  \return string content.
  */
u8 *epg_get_type(u8 index);

/*!
  Clear fifo data
  
  \param[in] p_fifo pointer to fifo
  */
void epg_fifo_flush(epg_fifo_t *p_fifo);

/*!
  Put data to fifo
  
  \param[in] p_fifo pointer to fifo
  \param[in] data data to store
  */
void epg_fifo_put(epg_fifo_t *p_fifo, u32 data);

/*!
  Get fifo data
  
  \param[in] p_fifo pointer to fifo
  \param[out] p_data store the data got

  \return TRUE if success, FALSE if fail
  */
BOOL epg_fifo_get(epg_fifo_t *p_fifo, u32 *p_data);


#endif

