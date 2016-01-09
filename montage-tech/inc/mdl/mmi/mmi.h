/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MMI_H__
#define __MMI_H__

#ifdef __cplusplus
extern "C" {
#endif
/*!
   Initialize the global heap.

   \param[in] size Specifies the size.
  */
void mmi_init_heap(u32 size);

/*!
   Release the global heap.
  */
void mmi_release_heap(void);

/*!
   Allocate a buffer from the global heap.

   \param[in] size Specifies the size of the buffer.

   \return If successful return the address of buffer, Otherwise return NULL.
  */
void *mmi_alloc_buf(u32 size);

/*!
   Reallocate a buffer with new size.

   \param[in] p_addr Points to a buffer.
   \param[in] size Specifies the new size.

   \return If successful return the address of buffer, Otherwise return NULL.
  */
void *mmi_realloc_buf(void *p_addr, u32 size);

/*!
   Free a buffer.

   \param[in] p_addr Points to a buffer.
  */
void mmi_free_buf(void *p_addr);

/*!
   Create a memf object with the specified count and slice size.

   \param[out] p_memf Points to a memf object.
   \param[in] cnt Specifies the count.
   \param[in] cnt Specifies the size of a slice.

   \return If successful return the address of buffer, Otherwise return NULL.
  */
void *mmi_create_memf(lib_memf_t *p_memf, u32 cnt, u32 slice);

/*!
   Destroy a memf object.

   \param[in] p_memf Points to a memf object.
   \param[in] p_addr Specifies the buffer of the memef object.
  */
void mmi_destroy_memf(lib_memf_t *p_memf, void *p_addr);

/*!
   Create a memp object with the specified size.

   \param[in] p_memp Points to a memp object.
   \param[in] size Specifies the size.

   \return If successful return the address of buffer, Otherwise return NULL.
  */
void *mmi_create_memp(lib_memp_t *p_memp, u32 size);

/*!
   Destroy a memp object.

   \param[in] p_memp Points to a memp object.
   \param[in] p_addr Specifies the buffer of the memp object.
  */
void mmi_destroy_memp(lib_memp_t *p_memp, void *p_addr);

/*!
  mmi assert.
  */
void mmi_assert(BOOL is_true);
#ifdef __cplusplus
}
#endif

#endif
