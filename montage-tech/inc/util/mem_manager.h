/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __MEM_MANAGER_H_
#define __MEM_MANAGER_H_

/*!
  \file mem_manager.h
  Memory management header file, this file defines interfaces and structures
  of a special memory management method.
  */
/*!
  A cryptonym for mem_mgr_create_partition
  */
#define MEM_CREATE_PARTITION(x) mem_mgr_create_partition(x)
/*!
  A cryptonym for mem_mgr_create_sub_partition
  */
#define MEM_CREATE_SUB_PARTITION(x) mem_mgr_create_sub_partition(x)
/*!
  A cryptonym for mem_mgr_release_partition
  */
#define MEM_RELEASE_PARTITION(x) mem_mgr_release_partition(x)
/*!
  A cryptonym for mem_mgr_alloc
  */
#define MEM_ALLOC(x) mem_mgr_alloc(x)
/*!
  A cryptonym for mem_mgr_free
  */
#define FREE_MEM(x) mem_mgr_free(x)
/*!
  A default align size in mem partition
  */
#define MEM_DEFAULT_ALIGN_SIZE 8
/*!
  The maximum block num in mem partition
  */
#define MAX_BLOCK_NUM 20
/*!
  This structure defines mem partition type id
  */
typedef enum
{
  /*!
    Partition id for video rendering
    */
  MEM_VIDEO_PARTITION = 0,
  /*!
    Partition id for system (including stacks)
    */
  MEM_SYS_PARTITION,
   /*!
    Partition id for nd malloc (including stacks)
    */
  MEM_DMA_PARTITION,
  /*!
    Partition id for ads malloc (including stacks)
    */
  MEM_ADS_PARTITION,
  /*!
    The last partition id 
    */
  MEM_LAST_PARTITION
} mem_mgr_partition_id_t;
/*!
  This structure defines the method id for mem mgr
  */
typedef enum
{
  /*!
    manage with normal flags, it's applied to
    sys partition mainly
    */
  MEM_METHOD_NORMAL = 0,
  /*!
    manage with special flags, it's applied to
    video partition especially
    */
  MEM_METHOD_SPECIAL
}mem_mgr_method_t;
/*!
  This structure defines the whether the mem
  block is shared or not
  */
typedef enum 
{
  /*!
    not allow to be used by other modules
    */
  MEM_BLOCK_NOT_SHARED = 0,
  /*!
    allow to be used by other modules
    */
  MEM_BLOCK_SHARED
}mem_mgr_block_attr_t;

/*!
  This structure defines the attributes of video partition block
  */
typedef struct mem_mgr_video_partition_block_tag
{
  /*!
    block id
    */
  u32 block_id;
  /*!
    block start address
    */
  void *p_addr;
  /*!
    block size
    */
  u32 size;
  /*!
    used status
    */
   BOOL b_used;
  /*! 
    the id of the user who is using this partition  
    */
   u16 user_id;    
  /*!
    block is shared or not. 1 for shared, 0 for not shared
    */
   u16 block_attr;
}mem_mgr_video_partition_block_t;

/*!
  This structure defines the attributes of mem partition
  */
typedef struct 
{
  /*!
    Total partition size
    */
  u32 total_size;
  /*!
    this size is the min size to be allocated in this partition
    */
  u32 atom_size;
  /*!
    flags to record which block is free or not
    */
  u32 *p_free_flag;
  /*!
    user id: the id of certain user who wants to make use of certain partition
    while the partition may have already been mastered by other users or not
    the initialization value of this variable is set to IDLE
    */
  u16 user_id;
  /*!
    parent partition id
    */
  u8 owner;
  /*!
    data start address
    */
  u8 *p_addr;
  /*!
    is fixed partition or not
    */
  u8 is_fixed;
  /*! 
    the method of managing this partition  
    */
   mem_mgr_method_t method_id;    
}mem_mgr_partition_t;
/*!
  This structure defines params on creating a mem partition
  */
typedef struct 
{
  /*!
    Partition id
    */
  mem_mgr_partition_id_t id;
  /*!
    block start address
    */
  void *p_addr;
  /*!
    Partition size
    */
  u32 size;
  /*!
    this size is the min size to be allocated in this partition
    */
  u32 atom_size;
  /*! 
    representing the id of the user who wants to use this partition  
    */
   u16 user_id;    
  /*! 
    the method of managing this partition  
    */
   mem_mgr_method_t method_id;    
} mem_mgr_partition_param_t;
/*!
  This structure defines params on creating a sub mem partition 
  */
typedef struct 
{
  /*!
    sub partition size
    */
  u32 size;      
  /*! 
    each block size in sub partition
    */
  u32 atom_size; 
  /*!
    This pointer points to parent partition id 
    and returns created sub partition id.
    When this module has been called, 
    this p_id will point at a sub partition id,
    not a partition id 
    */
  u8 *p_id;  
  /*! 
    when this variable is set to TRUE,
    which means the size of each allocated 
    blocks are set to atom size.
    However, when this variable is set to FALSE, 
    which means the size of each allocated 
    blocks can be random
    */
  u8 is_fixed;
  /*!
    the id for certain user who wants to share certain partition
    */
  u16 user_id;
} mem_mgr_sub_partition_param_t;
/*!
  This structure defines memory allocation input structure 
  */
typedef struct 
{
  /*!
    partition id, It can be partition or sub partition.
    */
  u8 id;
  /*!
    the memory size will be allocated in partition or sub partition
    */
  u32 size;
  /*!
    the id of the user who wants to perform allocation operations
    */
  u16 user_id ;
}mem_mgr_alloc_param_t;
/*!
  This structure defines memory free input structure
  */
typedef struct 
{
  /*!
    Partition id. It can be system partition id  or sub partition id
    */
 u8 id;
  /*!
    Memory starting address of allocated memory space
    */
 void *p_addr; 
  /*!
    The id of the user who wants to perform free operations 
    */
 u16 user_id;
} mem_mgr_free_param_t;
/*!
  This structure defines memory release partition structure
  */
typedef struct 
{
  /*!
    this id is fixed in the partition definitions
    */
  u8 id;
  /*!
    the user who wants to perform release partition operations
    */
  u16 user_id;
}mem_mgr_release_param_t;

/*!
  This structure defines partition block configs
  */
typedef struct partition_block_configs_tag
{
  /*!
    block id
    */
  u32 block_id;
  /*!
    block start address
    */
  u32 addr;
  /*!
    Partition size
    */
  u32 size;
  /*!
    block's attributes: 1 for shared, 0 for not shared
    */
  u16 block_attr;
}partition_block_configs_t;

/*!
  This structure defines the method id for mem mgr
  */
typedef enum
{
/*!
  align 4 mode
  */
  ALIGN_FOUR = 4,
/*!
  align 8 mode
  */
  ALIGN_EIGHT = 8,
/*!
align 16 mode
  */
  ALIGN_SIXTEEN = 16,
/*!
  align 32 mode
  */
 ALIGN_THIRTY_TWO = 32,

 /*!
   align 32 mode
   */
  ALIGN_SIXTY_FOUR = 64
  
}align_mode_t;

/*!
  Initialize memory module
  \param[in] p_address: System memory start address
  \param[in] size: Size of system memory
  */
void mem_mgr_init(u8 *p_address, u32 size);

/*!
  Mem allocate in system partition
  \param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  \return Allocated memory start address
  */
void *mem_mgr_alloc(mem_mgr_alloc_param_t *p_alloc_t);

/*!
 realloc memory, do not chang preview data.
  */
void *mem_mgr_realloc_malloc(mem_mgr_free_param_t *p_realloc_t, u32 mem_size);

/*!
  Mem free in system partition
  \param[in] p_free_t: input parameter is a structure pointer defined before
  */
void mem_mgr_free(mem_mgr_free_param_t *p_free_t);

/*!
  Mem allocate in system partition
  param[in] p_alloc_t: the input parameters of this function is a structure poiter,
  param[in] alignment: align mode
  return Allocated memory start address
  */
void * align_malloc(u32 size, align_mode_t alignment);

/*!
  Free a previously 'xvid_malloc' allocated block. Does not free NULL
  references.
  Returned value : None.
  */
void align_free(void *p_mem_ptr);

/*!
  Create system partition defined in mem_partition_id_t
  \param[in] *p_parti_t: input paramter is a structure pointer 
  \return BOOL 
  And if partition has been successfully created 
  return TRUE
  And if partition has not been successfully created 
  return FALSE
  */
BOOL mem_mgr_create_partition(mem_mgr_partition_param_t *p_parti_t);

/*!
  Release sub partition according to the input structure 
  \param[in] *p_release_t: input parameter is a structure pointer prevousely defined 
  */
void mem_mgr_release_partition(mem_mgr_release_param_t *p_release_t);

/*!
  Create sub partition in system partition
  \param[in] p_sub_pa_t input parameter is 
      a structure pointer previously defined 
  \return BOOL
  if sub partition has been successfully created return TRUE
  if sub partition has not been successfully created return FALSE 
  */
BOOL mem_mgr_create_sub_partition(mem_mgr_sub_partition_param_t *p_sub_pa_t);

/*!
  show memory partitions' using status
  */
void mem_show(void);

/*!
  require block in partition with special mgr method 
  \param[in] block_id block id that is required
  \param[in] user_id user id that wanna take the block 
  \return the address of the required block
  */
u32 mem_mgr_require_block(u32 block_id, u32 user_id);

/*!
  release block in partition with special mgr method 
  \param[in] block_id block id that is to be released
  \return TRUE for success, FALSE for failure.
  */
BOOL mem_mgr_release_block(u32 block_id);

/*!
  get block size.
  \param[in] block_id block id that is required
  \return the size of the required block
  */
u32 mem_mgr_get_block_size(u32 block_id);

/*!
  config blocks tab in video partition
  \param[in] p_table block table handler
  \param[in] block_num: number of blocks
  */
BOOL mem_mgr_config_blocks(partition_block_configs_t *p_table, u32 block_num);


/*!
   The macro is defined to enable debug print information or NOT.
  */
//#define MEM_DEBUG    0

/*!
   The structure is defined for mem link information.
  */
struct mem_link;

/*!
   The structure is defined for mem link information.
  */
typedef struct mem_link
{
  /*!
     The next node.
    */
  struct mem_link *p_next;
  /*!
     The size of this node.
    */
  u32 size;
#if MEM_DEBUG
  /*!
     The data of this node(additional debug info).
    */
  u32 data;
#endif
}mem_link_t;

/*!
   The structure is defined for mem internal information.
  */
typedef struct
{
  /*!
     The piece, points to mem_link.
    */
  mem_link_t *p_piece;
}mem_cb_t ;

/*!
   The macro is defined to specifies the minimum size of a piece.
  */
#define MEM_MIN_SIZE             (16)

/*!
   The macro is defined for calculate the size of header of mem.
  */
#define MEM_HEADER_SIZE          sizeof(mem_cb_t)

/*!
   The macro is defined for calculate the size of a picec header of mem.
  */
#define MEM_PIECE_HEADER_SIZE    sizeof(mem_link_t)

/*!
   The macro is MEM object size.
  */
#define LIB_MEM_SIZE           (MEM_HEADER_SIZE / sizeof(u32))
/*!
   The macro is MEM slice size for aligning.
  */
#define LIB_MEM_SLICE_ALIGN    (4)

/*!
   Define memory partition control data with all internal
   information hidden.
  */
typedef struct
{
  /*!
     The words of object to hide the internal information
    */
  u32 words[LIB_MEM_SIZE];
}lib_mem_t;


/*!
   Initialize a memory partition
  */
  RET_CODE malloc_mem_create(u8 *p_address, u32 size);

/*!
   Allocate buffer from memory partition
  */
void *lib_mem_alloc(u32 size);

/*!
   Free buffer to memory partition
  */
RET_CODE lib_mem_free(void *p_piece);

#endif 

