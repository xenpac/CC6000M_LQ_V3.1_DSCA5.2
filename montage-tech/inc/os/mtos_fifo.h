/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __MTOS_FIFO_H__
#define  __MTOS_FIFO_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
  MTOS fifo structure
  */
typedef struct tagFifo
{
  /*!
    pionter of fifo data
    */
  u16   *p_buffer;
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
}os_fifo_t;


/*!
  MTOS fifo with two data structure
  */
typedef struct tagFifo_2data
{
  /*!
    pionter of fifo data
    */
  u16   *p_buffer;
  /*!
    pionter of fifo data2
    */
  u16   *p_buffer2;
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
}os_fifo_2data_t;

/*!
  Clear fifo data

  \param[in] p_fifo pointer to fifo
  */
void mtos_fifo_flush(os_fifo_t *p_fifo);

/*!
  Put data to fifo

  \param[in] p_fifo pointer to fifo
  \param[in] data data to store
  */
void mtos_fifo_put(os_fifo_t *p_fifo, u16 data);

/*!
  Get fifo data

  \param[in] p_fifo pointer to fifo
  \param[out] p_data store the data got

  \return TRUE if success, FALSE if fail
  */
BOOL mtos_fifo_get(os_fifo_t *p_fifo, u16 *p_data);

/*!
  Clear fifo data

  \param[in] p_fifo pointer to fifo
  */
void mtos_fifo_flush_2param(os_fifo_2data_t *p_fifo);

/*!
  Put data to fifo

  \param[in] p_fifo pointer to fifo
  \param[in] data data to store
  \param[in] data2 data to store
  */
void mtos_fifo_put_2param(os_fifo_2data_t *p_fifo, u16 data, u16 data2);

/*!
  Get fifo data

  \param[in] p_fifo pointer to fifo
  \param[out] p_data store the data got
  \param[out] p_data2 store the data2 got

  \return TRUE if success, FALSE if fail
  */
BOOL mtos_fifo_get_2param(os_fifo_2data_t *p_fifo, u16 *p_data, u16 *p_data2);
#ifdef __cplusplus
}
#endif
#endif //__MTOS_FIFO_H__
