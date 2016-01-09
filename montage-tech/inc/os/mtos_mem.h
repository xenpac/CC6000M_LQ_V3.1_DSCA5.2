/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MTOS_MEM_H__
#define __MTOS_MEM_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
  switch for debug
  */
//#define MTOS_MEM_DEBUG

/*!
  mem debug info
  */
typedef struct
{
     /*!
       module name
    */
    char mem_module_name[30];
     /*!
       mode,malloc or free
    */
    BOOL mem_mode;
    /*!
      module all addr
    */
    u32 addr[50];
    /*!
       malloc or free size
   */
    u32 mem_size;
    /*!
       total malloc size
   */
    u32 mem_total_used_size;
}mem_debug_info_t;

/*!
  mem user debug info
  */
typedef struct
{
  /*!
     current alloced size
  */
  u32 alloced;
  /*!
     allocated peak size
  */
  u32 alloced_peak;
  /*!
     the rest size can be allocated
  */
  u32 rest_size;
}mem_user_dbg_info_t;

typedef enum
{
  MEM_ALLOC_NORMAL = 0,
  MEM_ALLOC_DMA,
  MEM_ALLOC_DMA_ALIGN,
}mem_alloc_mode_t;

/*!
  memory leak and bound overflow debug config
  */
typedef struct
{
  /*!
    how many allocated logs you want to record
  */
  u32 alloc_log_cnt;
  /*!
    the priority of the background task used to real time check.
  */
  u32 task_prio;
  /*!
    the task slice in ms.
  */
  u32 task_slice_ms;  
}mem_leak_overflow_dbg_cfg_t;

/*!
  malloc function pointer
  */
typedef  void * (*pmalloc)(u32 size);
/*!
  free function pointer
  */
typedef  void  (*pfree)(void *p_addr);

/*!
  Initiate the memory

  \param[in] m function pointer to malloc functino
  \param[in] f function pointer to free functino
  */
void  mtos_mem_init(pmalloc m, pfree f);

/*!
  mtos_mem_user_debug

  \param[out] p_dbg_info user debug information
  */
void mtos_mem_user_debug(mem_user_dbg_info_t *p_dbg_info);

/*!
  mtos_mem_bound_check

  */
void mtos_mem_bound_check();

/*!
  enable log recording for all malloc and free
  */
void mtos_mem_log_enable(BOOL enable);

/*!
  enable a backgroud process to moniter memory leak and overflow 
  */
RET_CODE mtos_mem_leak_overflow_dbg_enable(mem_leak_overflow_dbg_cfg_t *p_cfg);

/*!
  disable the backgroud process which was used to moniter memory leak and overflow 
  */
RET_CODE mtos_mem_leak_overflow_dbg_disable();

/*!
  mtos_malloc_actual_addr_get

  \param[in] alloc_addr allocated address
  \param[in] alloc_mode allocation mode, see mem_alloc_mode_t
  \param[out] p_act_addr actual address returned
  */
void mtos_malloc_actual_addr_get(u32 alloc_addr, u32 alloc_mode, u32 *p_act_addr);

#ifdef MTOS_MEM_DEBUG
/*!
  malloc space from system partition
  */
#define mtos_malloc(x) mtos_malloc_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  free memory
  */
#define mtos_free(x) mtos_free_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  mtos calloc

  \param[in] n_elements : elements
  \param[in] elem_size : size

  \return buffer address.
  */
#define mtos_calloc(x, y) mtos_calloc_dump(x, y, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  mtos re-alloc

  \param[in] p_readdr : addr
  \param[in] size : size

  \return buffer address.
  */
#define mtos_realloc(x, y) mtos_realloc_dump(x, y, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  malloc space from system partition

  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
#define mtos_dma_malloc(x) mtos_dma_malloc_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  free memory

  \param[in] addr the memory address to free
  */
#define mtos_dma_free(x) mtos_dma_free_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  Mem allocate in system partition within cache line alignment mode
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
#define mtos_cache_align_malloc(x) mtos_cache_align_malloc_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  Free a previously 'mtos_cache_line_malloc' allocated block within cache line alignment mode.
  Does not free NULL
  references.
  Returned value : None.
  */
#define mtos_cache_align_free(x) mtos_cache_align_free_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
#define mtos_align_malloc(x, y) mtos_align_malloc_dump(x, y, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
#define mtos_align_free(x) mtos_align_free_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align byte count, note that it is a multiple of 4 byte
  return Allocated memory start address
  */
#define mtos_align_dma_malloc(x, y) mtos_align_dma_malloc_dump(x, y, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
#define mtos_align_dma_free(x) mtos_align_dma_free_dump(x, __FILE__, (char *)__FUNCTION__, __LINE__)

/*!
  malloc space from system partition

  \param[in] size the size to malloc
  \param[in] p_file file name
  \param[in] p_func function name
  \param[in] line line number
  */
void* mtos_malloc_dump(u32 size, char *p_file, char *p_func, int line);

/*!
  free memory

  \param[in] addr the memory address to free
  \param[in] p_file file name
  \param[in] p_func function name
  \param[in] line line number

  */
void  mtos_free_dump(void *p_addr, char *p_file, char *p_func, int line);

/*!
  mtos calloc

  \param[in] n_elements : elements
  \param[in] elem_size : size

  \return buffer address.
  */
void *mtos_calloc_dump(u32 n_elements, u32 elem_size, char *p_file, char *p_func, int line);

/*!
  mtos re-alloc

  \param[in] p_readdr : addr
  \param[in] size : size

  \return buffer address.
  */
void *mtos_realloc_dump(void *p_readdr, u32 size, char *p_file, char *p_func, int line);

/*!
  malloc space from system partition

  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
void* mtos_dma_malloc_dump(u32 size, char *p_file, char *p_func, int line);

/*!
  free memory

  \param[in] addr the memory address to free
  */
void  mtos_dma_free_dump(void *p_addr, char *p_file, char *p_func, int line);

/*!
  Mem allocate in system partition within cache line alignment mode
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
void *mtos_cache_align_malloc_dump(u32 size, char *p_file, char *p_func, int line);

/*!
  Free a previously 'mtos_cache_line_malloc' allocated block within cache line alignment mode.
  Does not free NULL
  references.
  Returned value : None.
  */
void mtos_cache_align_free_dump(void *p_ptr, char *p_file, char *p_func, int line);

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
void *mtos_align_malloc_dump(u32 size, u32 alignment, char *p_file, char *p_func, int line);

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void mtos_align_free_dump(void *p_mem_ptr, char *p_file, char *p_func, int line);

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align byte count, note that it is a multiple of 4 byte
  return Allocated memory start address
  */
void * mtos_align_dma_malloc_dump(u32 size, u32 alignment, char *p_file, char *p_func, int line);

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void mtos_align_dma_free_dump(void *p_mem_ptr, char *p_file, char *p_func, int line);

#else
/*!
  malloc space from system partition
  */
#define mtos_malloc(x) mtos_malloc_alias(x)

/*!
  free memory
  */
#define mtos_free(x) mtos_free_alias(x)

/*!
  mtos calloc

  \param[in] n_elements : elements
  \param[in] elem_size : size

  \return buffer address.
  */
#define mtos_calloc(x, y) mtos_calloc_alias(x, y)

/*!
  mtos re-alloc

  \param[in] p_readdr : addr
  \param[in] size : size

  \return buffer address.
  */
#define mtos_realloc(x, y) mtos_realloc_alias(x, y)

/*!
  malloc space from system partition

  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
#define mtos_dma_malloc(x) mtos_dma_malloc_alias(x)

/*!
  free memory

  \param[in] addr the memory address to free
  */
#define mtos_dma_free(x) mtos_dma_free_alias(x)

/*!
  Mem allocate in system partition within cache line alignment mode
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
#define mtos_cache_align_malloc(x) mtos_cache_align_malloc_alias(x)

/*!
  Free a previously 'mtos_cache_line_malloc' allocated block within cache line alignment mode.
  Does not free NULL
  references.
  Returned value : None.
  */
#define mtos_cache_align_free(x) mtos_cache_align_free_alias(x)

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
#define mtos_align_malloc(x, y) mtos_align_malloc_alias(x, y)

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
#define mtos_align_free(x) mtos_align_free_alias(x)

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align byte count, note that it is a multiple of 4 byte
  return Allocated memory start address
  */
#define mtos_align_dma_malloc(x, y) mtos_align_dma_malloc_alias(x, y)

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
#define mtos_align_dma_free(x) mtos_align_dma_free_alias(x)

/*!
  malloc space from system partition

  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
void* mtos_malloc_alias(u32 size);

/*!
  free memory

  \param[in] addr the memory address to free
  */
void  mtos_free_alias(void *p_addr);

/*!
  mtos calloc

  \param[in] n_elements : elements
  \param[in] elem_size : size

  \return buffer address.
  */
void *mtos_calloc_alias(u32 n_elements, u32 elem_size);

/*!
  mtos re-alloc

  \param[in] p_readdr : addr
  \param[in] size : size

  \return buffer address.
  */
void *mtos_realloc_alias(void *p_readdr, u32 size);

/*!
  malloc space from system partition

  \param[in] size the size to malloc
  \param[in] module the index of the current memory user
  */
void* mtos_dma_malloc_alias(u32 size);

/*!
  free memory

  \param[in] addr the memory address to free
  */
void  mtos_dma_free_alias(void *p_addr);

/*!
  Mem allocate in system partition within cache line alignment mode
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
void *mtos_cache_align_malloc_alias(u32 size);

/*!
  Free a previously 'mtos_cache_line_malloc' allocated block within cache line alignment mode.
  Does not free NULL
  references.
  Returned value : None.
  */
void mtos_cache_align_free_alias(void *p_ptr);

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
void *mtos_align_malloc_alias(u32 size, u32 alignment);

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void mtos_align_free_alias(void *p_mem_ptr);

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align byte count.
  return Allocated memory start address
  */
void * mtos_align_dma_malloc_alias(u32 size, u32 alignment);

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void mtos_align_dma_free_alias(void *p_mem_ptr);
#endif

/*!
  Mem allocate in system partition heap   dl_malloc/dl_free  fuction used
  param[in] base: the heap base pointer,
  param[in] size:  the heap length.
  */
void dlmem_init(void* base, int size);

/*!
  dl malloc

  \param[in] bytes : bytes needed

  \return buffer address.
  */

void *dl_malloc(u32 bytes);


/*!
  dl free

  \param[in] p_mem_ptr : elements

  */
void dl_free(void* p_mem_ptr);

/*!
  dl calloc

  \param[in] n_elements : elements
  \param[in] elem_size : size

  \return buffer address.
  */

void *dl_calloc(u32 n_elements, u32 elem_size);

/*!
  dl realloc

  \param[in] p_oldmem :  old mem pointer
  \param[in] bytes :  new size

  \return buffer address.
  */

void *dl_realloc(void *p_oldmem, u32 bytes);


#ifdef __cplusplus
}
#endif

#endif //__MTOS_MEM_H__
