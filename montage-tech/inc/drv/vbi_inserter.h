/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __VBI_INSERTER_H__
#define __VBI_INSERTER_H__

#ifdef __cplusplus
extern "C" {
#endif



/*!
  bit field of Colse Caption
  */
#define VBI_INSERTION_CC                ((u32)1 << 0)
/*!
  bit field of WWS
  */
#define VBI_INSERTION_WSS               ((u32)1 << 1)
/*!
  bit field of Teletext
  */
#define VBI_INSERTION_TTX               ((u32)1 << 2)
/*!
  bit field of VPS
  */
#define VBI_INSERTION_VPS               ((u32)1 << 3)
/*!
  bit field of ATSC
  */
#define VBI_INSERTION_ATSC               ((u32)1 << 4)
/*!
  bit field of software closed caption
  */
#define VBI_INSERTION_CC_SW              ((u32)1<<5)
/*!
  bit field of software Teletext
  */
#define VBI_INSERTION_TTX_SW              ((u32)1<<6)

/*!
  bit field of all VBI function
  */
#define VBI_ALL_INSERTION               ((u32)0x7f)



/*!
    VBI inserter device
  */
typedef struct
{
    /*!
      Base
    */
    void *p_base;

    /*!
      Pointer to private data
    */
    void *p_priv;
} vbi_inserter_device_t;


/*!
    The structure of fifo instance
  */
typedef struct
{
    /*!
      Pointer to the start of fifo buffer
      */
    u8      *p_buf;
    /*!
      Pointer to the end of fifo buffer
      */
    u8      *p_buf_tail;
    /*!
      Size of fifo buffer
      */
    s32     buf_size;

    /*!
      Pointer to the first data in the fifo buffer
      */
    u8      *p_data;

    /*!
      Pointer to the free byte of fifo buffer
      */
    u8      *p_free;
    /*!
      Data put size in the fifo buffer    For magic
      */
    s32     data_cnt;

    /*!
      Data put size in the fifo buffer    For warriors
      */
    s32     data_put_cnt;
    /*!
      Data decrease size in the fifo buffer    For warriors
      */
    s32     data_decrease_cnt;
} vbi_fifo_t;

/*!
vbi cfg
*/
typedef struct
{
    /*!
      vbi buf addr, for concerto
      */
    u32 vbi_pes_buf_addr;
    /*!
      vbi buf size, for concerto
      */
    u32 vbi_pes_buf_size;
}vbi_cfg_t;

/*!
   Initialize the fifo buffer

   \param[in] p_fifo    The handle of FIFO
   \param[in] p_buf     Pointer to the buffer for FIFO
   \param[in] buf_size  The size of buffer

   \return
  */
void vbi_inserter_fifo_init(vbi_fifo_t *p_fifo, u8 *p_buf, u32 buf_size);

/*!
   Flush the fifo buffer

   \param[in] p_fifo    The handle of FIFO

   \return
  */
void vbi_inserter_fifo_flush(vbi_fifo_t *p_fifo);

/*!
   start VBI inserter

   \param[in] p_fifo    The handle of FIFO
   \param[in] p_data    Pointer to the data put into FIFO
   \param[in] buf_size  The size of data

   \return   TRUE for success
  */
BOOL vbi_inserter_fifo_put(vbi_fifo_t *p_fifo, u8 *p_data, u8 size);

/*!
   Get data from fifo buffer

   \param[in] p_fifo    The handle of FIFO
   \param[in] p_data_buf  Pointer to the data

   \return  TRUE for success
  */
BOOL vbi_inserter_fifo_get(vbi_fifo_t *p_fifo, u8 *p_data_buf);

/*!
   Delete the data witch put into the FIFO first

   \param[in] p_fifo    The handle of FIFO

   \return
  */
void vbi_inserter_fifo_decrease(vbi_fifo_t *p_fifo);

/*!
   Check if the fifo buffer is to be overflowed

   \param[in] p_fifo    The handle of FIFO

   \return
  */
BOOL vbi_inserter_fifo_is_critical(vbi_fifo_t *p_fifo);


/*!
   Start VBI inserter

   \param[in] dev	VBI Inserter
   \param[in] param     Specify which type of VBI data will be inserted. The data can be:
                        VBI_INSERTION_CC, VBI_INSERTION_WSS, VBI_INSERTION_TTX
			or VBI_ALL_INSERTION

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_inserter_dev_start(vbi_inserter_device_t *p_dev, u32 param);

/*!
   Stop VBI inserter

   \param[in] dev		VBI Inserter
   \param[in] param             Specify which type of VBI data can not be inserted.

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_inserter_dev_stop(vbi_inserter_device_t *p_dev, u32 param);

/*!
   Set video standard

   \param[in] dev		VBI Inserter
   \param[in] std		Video standard

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_inserter_dev_set_vid_std(vbi_inserter_device_t *p_dev
    , video_std_t std);

/*!
   Insert data

   \param[in] dev    VBI Inserter
   \param[in] pts    The PTS of this data
   \param[in] p_pes_data_filed	Pointer to the start of data filed (filed or field???) of PES ????
   \param[in] p_pes_last_byte   Pointer to the end of PES

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_inserter_dev_inserte_data(vbi_inserter_device_t *p_dev
    , u32 pts, u8 *p_pes_data_filed, u8 *p_pes_last_byte);

#ifdef __cplusplus
}
#endif

#endif

