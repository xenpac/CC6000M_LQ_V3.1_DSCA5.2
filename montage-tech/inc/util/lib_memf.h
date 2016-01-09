/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_MEMF_H__
#define __LIB_MEMF_H__
/*!
   The structure is defined for memf link information.
  */
struct memf_link;

/*!
   The structure is defined for memf link information.
  */
typedef struct memf_link
{
  /*!
     The next node.
    */
  struct memf_link *p_next;
}memf_link_t;

/*!
   The structure is defined for memp internal information.
  */
typedef struct
{
  /*!
     Points to the unallocated buffer.
    */
  void *p_chunk;
  /*!
     Points to the end of the  buffer.
    */
  void *p_end;
  /*!
     The piece.
    */
  memf_link_t *p_piece;
  /*!
     The slice size.
    */
  u32 slice;
}memf_cb_t;

/*!
   The macro is defined for calculate the size of header of memp.
  */
#define MEMF_HEADER_SIZE    sizeof(memf_cb_t)
/*!
   The macro is MEMF object size.
  */
#define LIB_MEMF_SIZE              (MEMF_HEADER_SIZE / sizeof(u32))

/*!
   Define memory fix-size partition control data with all internal
   information hidden.
  */
typedef struct
{
  /*!
     The words of object to hide the internal information
    */
  u32 words[LIB_MEMF_SIZE];
}lib_memf_t;


/*!
   Initialize a memory fix-size partition

   \param[out] p_memf Points to the MEMF object.
   \param[in] address Specifies the address of the partition buffer.
   \param[in] size Specifies the size of the partition buffer.
   \param[in] slice Specifies the slice size of MEMF object.

   \return if successful, it return SUCCESS, Otherwise returns ERR_PARAM.

   \remark the minimal slice size is 4
  */
RET_CODE lib_memf_create(lib_memf_t *p_memf, u32 address, u32 size, u32 slice);

/*!
   Destroy a memory fix-size partition

   \param[in] p_memf Points to the MEMF object.

   \return if successful, it return SUCCESS, Otherwise returns ERR_PARAM.
  */
RET_CODE lib_memf_destroy(lib_memf_t *p_memf);

/*!
   Allocate buffer from memory fix-size partition

   \param[in] p_memf Points to the MEMF object.

   \return if successful, it return a pointer to allocated buffer,
           Otherwise returns NULL.
  */
void *lib_memf_alloc(lib_memf_t *p_memf);

/*!
   Free buffer to memory fix-size partition

   \param[in] p_memf Points to the MEMF object.
   \param[in] p_buf Points to the buffer to be freed.

   \return if successful, it return SUCCESS, Otherwise returns ERR_PARAM.
  */
RET_CODE lib_memf_free(lib_memf_t *p_memf, void *p_buf);

#endif
