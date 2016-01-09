/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_MEMP_H__
#define __LIB_MEMP_H__

/*!
   The macro is defined to enable debug print information or NOT.
  */
#define MEMP_DEBUG    0

/*!
   The structure is defined for memp link information.
  */
struct memp_link;

/*!
   The structure is defined for memp link information.
  */
typedef struct memp_link
{
  /*!
     The next node.
    */
  struct memp_link *p_next;
  /*!
     The size of this node.
    */
  u32 size;
#if MEMP_DEBUG
  /*!
     The data of this node(additional debug info).
    */
  u32 data;
#endif
}memp_link_t;

/*!
   The structure is defined for memp internal information.
  */
typedef struct
{
  /*!
     The piece, points to memp_link.
    */
  memp_link_t *p_piece;
#if MEMP_DEBUG
  /*!
     The address of this piece.
    */
  u32 address;
  /*!
     The size of this piece.
    */
  u32 size;
#endif
}memp_cb_t;

/*!
   The macro is defined to specifies the minimum size of a piece.
  */
#define MEMP_MIN_SIZE             (16)

/*!
   The macro is defined for calculate the size of header of memp.
  */
#define MEMP_HEADER_SIZE          sizeof(memp_cb_t)

/*!
   The macro is defined for calculate the size of a picec header of memp.
  */
#define MEMP_PIECE_HEADER_SIZE    sizeof(memp_link_t)

/*!
   The macro is MEMP object size.
  */
#define LIB_MEMP_SIZE           (MEMP_HEADER_SIZE / sizeof(u32))
/*!
   The macro is MEMP slice size for aligning.
  */
#define LIB_MEMP_SLICE_ALIGN    (8)

/*!
   Define memory partition control data with all internal
   information hidden.
  */
typedef struct
{
  /*!
     The words of object to hide the internal information
    */
  u32 words[LIB_MEMP_SIZE];
}lib_memp_t;


/*!
   Initialize a memory partition

   \param[out] p_memp Points to the MEMF object.
   \param[in] address Specifies the address of the partition buffer.
   \param[in] size Specifies the size of the partition buffer.

   \return if successful, it return SUCCESS, Otherwise returns ERR_PARAM.
  */
RET_CODE lib_memp_create(lib_memp_t *p_memp, u32 address, u32 size);

/*!
   Destroy a memory partition

   \param[in] p_memp Points to the MEMF object.

   \return if successful, it return SUCCESS, Otherwise returns ERR_PARAM.
  */
RET_CODE lib_memp_destroy(lib_memp_t *p_memp);

/*!
   Allocate buffer from memory partition

   \param[in] p_memp Points to the MEMF object.
   \param[in] size Specifies the size of the buffer to be allocated.

   \return if successful, it return a pointer to allocated buffer,
           Otherwise returns NULL.
  */
void *lib_memp_alloc(lib_memp_t *p_memp, u32 size);

/*!
   Free buffer to memory partition

   \param[in] p_memp Points to the MEMF object.
   \param[in] buffer Points to the buffer to be freed.
  */
RET_CODE lib_memp_free(lib_memp_t *p_memp, void *p_piece);

/*!
   Resize allocated buffer to new size

   \param[in] p_memp Points to the MEMF object.
   \param[in] buffer Points to the buffer to be resized.
   \param[in] size Specifies the new size of the buffer.

   \return if successful, it return a pointer to allocated buffer,
           Otherwise returns NULL.
  */
void *lib_memp_resize(lib_memp_t *p_memp, void *p_piece, u32 size);

/*!
   Allocate buffer from memory partition with alignment

   \param[in] p_memp Points to the MEMF object.
   \param[in] size Specifies the size of the buffer to be allocated.
   \param[in] alignment : alignment.

   \return if successful, it return a pointer to allocated buffer,
           Otherwise returns NULL.
  */
void *lib_memp_align_alloc(lib_memp_t *p_memp, u32 size, u32 alignment);

/*!
   Free buffer to memory partition(alignment)

   \param[in] p_memp Points to the MEMF object.
   \param[in] buffer Points to the buffer to be freed.

   \return.
  */
RET_CODE lib_memp_align_free(lib_memp_t *p_memp, void *p_piece);

#endif
