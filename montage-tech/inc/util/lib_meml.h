/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_MEML_H__
#define __LIB_MEML_H__

/*!
  Declaration for meml_buf 
  */
struct meml_buf;

/*!
  Type for meml_buf free callback
  */
typedef void (*meml_buf_free_t)(struct meml_buf *p_lbuf);

/*!
  Memory link buffer type
  */
typedef struct meml_buf 
{
  /*!
    Next meml_buf point
    */
  struct meml_buf *p_next;
  /*!
    Buffer start address
    */  
  u32 addr; 
  /*!
    Buffer size
    */  
  u32 size;
  /*!
    Active data offset in buffer
    */  
  u32 data_offset;
  /*!
    Active data length in buffer
    */  
  u32 data_length;
  /*!
    meml_buf free callback
    */  
  meml_buf_free_t free_cb;
  /*!
    Indidate the source handle where the buffer from
    */  
  void *p_src;
  /*!
    Private field 0 for meml_buf, recommend for flags
    */  
  u32 priv0;
  /*!
    Private field 1 for meml_buf, recommend for pointer
    */  
  u32 priv1;
}meml_buf_t;


/*!
  Create one buffer pool according to "address" and "size", return "id" for created buffer pool

  \param[in] address The buffer start address.
  \param[in] size The buffer size.
  \param[out] p_id The meml id created

  \return 0 if success, other if failed
  */
extern RET_CODE lib_meml_create(u32 address, u32 size, u8 *p_id);

/*!
  Delete buffer pool according to "id"

  \param[in] id The meml id

  \return 0 if success, other if failed  
  */
extern RET_CODE lib_meml_delete(u8 id);

/*!
  Allocate two types of instance. num * sizeof(struct meml_buf) from the 
  global meml_buf buffer pool + num*size from buffer pool "id".
  Set the p_next/addr/size in p_lbuf and return p_lbuf.
  If size==0;only alloc num*sizeof(struct meml_buf) and set addr = size=0 and return p_lbuf

  \param[in] id The meml_buf_t id
  \param[in] size The buffer size one meml
  \param[in] num The number of buffer in meml list
  \param[out] p_lbuf The meml_buf_t pointer 

  \return 0 if success, other if failed  
  */
extern RET_CODE lib_meml_alloc(u32 id, u32 size, u32 num, meml_buf_t **p_lbuf);

/*!
  Free meml and buffer address. 
  if buf_free_callback!=NULL, only free lbuf to meml buffer pool and free addr by calling buf_free_callback

  \param[in] p_lbuf The meml_buf_t pointer 

  \return 0 if success, other if failed  
  */
extern RET_CODE lib_meml_free(meml_buf_t *p_lbuf);

/*!
  Chain the header of lbuf_new to the tail of plbuf_old

  \param[in] p_lbuf_old The meml_buf_t pointer where the new meml to be linked at the tail
  \param[in] p_lbuf_new The meml_buf_t new pointer to be linked   

  \return 0 if success, other if failed  
  */
extern RET_CODE lib_meml_chain(meml_buf_t *p_lbuf_old, meml_buf_t *p_lbuf_new);

/*!
  Dischain the next of plbuf_old as the header of lbuf_new

  \param[in] p_lbuf_old The meml pointer from which the next meml_buf_t will be unlinked
  \param[out] p_lbuf_new The meml_buf_t pointer unlinked   

  \return void  
  */
extern void lib_meml_unchain(meml_buf_t *p_lbuf_old, meml_buf_t **p_lbuf_new);

/*!
  Cut the buffer(buffer size-size) as new meml_buf_t and chain it to the next of p_lbuf

  \param[in] p_lbuf The meml_buf_t pointer which buffer to be splited
  \param[in] size The meml pointer unlinked   

  \return 0 if success, other if failed  
  */
extern RET_CODE lib_meml_split(meml_buf_t *p_lbuf, u32 size);

/*!
  Allocate buffer according to the "size "from buffer pool specified by "id",return the address of allocated buffer

  \param[in] p_lbuf The meml_buf_t id
  \param[in] size The size to be allocated   

  \return the buffer pointer  
  */
extern void *lib_meml_buf_alloc(u8 id, u32 size);

/*!
  Free buffer according to the "size " and d"address"

  \param[in] address The buffer address to be freed
  \param[in] size The buffer size to be freed   

  \return 0 if success, other if failed
  */
extern RET_CODE lib_meml_buf_free(void *p_address, u32 size);

/*!
  Find the tail of the meml buffer chain

  \param[in] p_lbuf The meml_buf_t pointer from which to find   

  \return The tail pointer of the meml_buf_t chain
  */
extern meml_buf_t *lib_meml_tail(meml_buf_t *p_lbuf);

/*!
  Get total free buffer size in the whole meml_buf_t buffer chain

  \param[in] id The meml_buf_t id  

  \return The spare buffer size 
  */
extern u32 lib_meml_spare_size(u8 id);

#endif //__LIB_MEML_H__

