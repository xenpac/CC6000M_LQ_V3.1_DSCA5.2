/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"

#include "hal_misc.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"
#include "mtos_mutex.h"

#include "mem_manager.h"
#include "class_factory.h"
#include "drv_dev.h"
#include "charsto.h"
#include "mdl.h"
#include "gzip.h"

#include "data_manager.h"
#include "data_manager_priv.h"
#include "data_manager_v3.h"

/*!
  Notes. if your project is MP state, please open DM_MP_STATE.
  Because sometimes is hardware error, but we must assure system can work well.
  */
//#define DM_MP_STATE


//#define DM_DETAIL_TRACE
//#define DM_DEBUG_TRACE
//#define DM_ERROR_TRACE
//#define DM_VERIFY_TRACE
//#define DM_VERIFY_OPEN

#ifdef DM_DETAIL_TRACE
#define DM_DETAIL DUMMY_PRINTF
#else
#define DM_DETAIL DUMMY_PRINTF
#endif

#ifdef DM_DEBUG_TRACE
#define DM_PRINT DUMMY_PRINTF
#else
#define DM_PRINT DUMMY_PRINTF
#endif

#ifdef DM_ERROR_TRACE
#define DM_ERROR OS_PRINTF("DM_ERROR!! function %s, line %d!!!!", \
                            __FUNCTION__, __LINE__);OS_PRINTF
#else
#define DM_ERROR DUMMY_PRINTF
#endif

#ifdef DM_VERIFY_TRACE
#define DM_VERIFY DUMMY_PRINTF
#else
#define DM_VERIFY DUMMY_PRINTF
#endif

/*!
  Data manager node header structure
  */
typedef union tag_node_header_t
{
  /*!
    Separate fields of this union
    */
  struct
  {
   /*!
    ID
    */
    u32 id      : 16;
   /*!
    active flag
    */
    u32 active  : 1;
   /*!
    Node length
    */
    u32 length  : 15; 
  };
 /*!
  Merged variable
  */
  u32 value;
}node_header_t;

/*!
  Length of node header
  */
#define NODE_HEADER_SIZE (sizeof(node_header_t))

/*!
  align to 16 bytes
  */
#define ALIGN16(x) (((x) + 0xF) & (~0xF))

/*!
  Data manager atom size
  */
#define DM_ATOM_SIZE (16)

/*!
  Length of node flag, use one byte
  */
#define NODE_FLAG_SIZE (1)

/*!
  this flag will appear on end of node, means the node is intact
  */
#define NODE_INTACT_FLAG (0x5A)

/*!
  Min node length in data manager 32
  */
#define MIN_NODE_LEN (32)

/*!
  Max node lower limit length pagload in data manager_v3
  */
#define DM_MAX_NODE_LOWER_LIMIT_LEN (2 * (KBYTES))

/*!
  Max node high limit length pagload in data manager_v3
  */
#define DM_MAX_NODE_HIGH_LIMIT_LEN  (10 * (KBYTES))

/*!
  size of section
  */
#define FLASH_SECTION_SIZE (64 *1024)
/*!
  size of critical
  */
#define CRITICAL_SIZE (60*1024)
/*!
  min load data
  */
#define MIN_VALID_DATA (40*1024)
/*!
  min load data when abnormity
  */
#define ABN_MIN_VALID_DATA (60*1024)

/*!
  unknown id, means needs dispatch a id for it
  */
#define UNKNOWN_ID (0xFFFF)
/*!
  0xFFFFFFFF (u32)
  */
#define ERASE_DATA (~0)
/*!
  header offset
  */
#define DM_TAG_SIZE (16)
/*!
  header size
  */
#define DM_BASE_INFO_HEAD_LEN (12)

/*!
  max node buffer 
  */
#define DM_MAX_NODE_BUFFER     (5)

/*!
  node buffer structure.
  */
typedef struct tag_dm_node_buf
{
/*!
  is used.
  */
  BOOL is_used;
/*!
  node buffer.
  */
  u8 *p_node_buf;
}dm_node_buf_t;

/*!
  Cache state
  */
typedef enum cache_state_e
{
  /*!
    2bit is '00"
    */
  CACHE_EMPTY = 0x0,
  /*!
    2bit is '01"
    */
  CACHE_ASYNC = 0x1,
  /*!
    2bit is '11"
    */
  CACHE_SYNC = 0x3,
}cache_state_t;

/*!
  node mapping info, init to ZERO
  */
typedef struct tag_node_logic_info_t
{
  /*!
    malloc node_num space to store each node offset in flash
    */
  u32 node_iw_addr :21;   //iw block: base for iw addr; piece block for the ghost
  /*!
    unsync cache size in current node
    */
  u32 unsync_cache_size:8;
  /*!
    00b is empty, 01b is async,11b: is sync
    */
  u32 cache_state :2;
  /*!
    0: deleted 1: active
    */
  u32 active :1;
}node_logic_info_t;

/*!
  section status
  */
typedef enum tag_sec_status_t
{
  /*!
    section is clean
    */
  SEC_CLEAN = 0xFFFFFFFF,
  /*!
    some section transport data to this
    */
  SEC_STREAM_IN = 0xFFFFFFFA,
  /*!
    block writing ..
    */
  SEC_WRITE = 0xFFFFFFAA,
  /*!
    section is full, and have a lot valid data
    */
  SEC_USED = 0xFFFFFAAA,
  /*!
    it transprot data out
    */
  SEC_STREAM_OUT = 0xFFFFAAAA,
  /*!
    all of data is older
    */
  SEC_STREAM_OVERDUE = 0xFFFAAAAA,
  /*!
    section bad
    */
  SEC_STREAM_BAD,
}sec_status_t;

/*!
  section header , size 16BYTES
  */
typedef struct tag_section_header_t
{
  /*!
    status
    */
  sec_status_t status;
  /*!
    block_id
    */
  u8 block_id;
  /*!
    rev
    */
  u8 rev_2;
  /*!
    section index
    */
  u8 index;
  /*!
    rev
    */
  u8 rev_3;
  /*!
    the id flag the order
    */
  u32 sec_id;
  /*!
    rev
    */
  u32 rev_4;
}section_header_t;

/*!
  section head size
  */
#define SEC_HEAD_SIZE (sizeof(section_header_t))

/*!
  Data manager section information
  */
typedef struct tag_section_info_t
{
  /*!
    Data manager section header
    */
  section_header_t header;
  /*!
    address offset the section, the value is real addr
    */
  u32 cur_sec_addr;
  /*!
    valid length
    */
  u32 valid_len;
}section_info_t;

/*!
  Data manager section information
  */
typedef struct tag_section_mgr_t
{
  /*!
    Data manager section header
    */
  section_info_t *p_sec_arry;
  /*!
    iw section base address, the value is offset to flash base addr
    */
  u32 sec_base_addr;
  /*!
    how math section
    */
  u32 total_section_num;
  /*!
    how math compress section, only for piece mode
    */
  u32 total_compress_section_num;
  /*!
    dispatch counter
    */
  u32 dispatch_num;
  /*!
    sec data changed, for piece only
    */
  BOOL sec_data_changed;

}section_mgr_t;

/*!
  Data manager block information
  */
typedef struct tag_block_mgr_t
{
  u32 dm_header_addr;
  /*!
    Block id
    */
  u8 id;
  /*!
    refer to block_type_t
    */
  u8 type;
  /*!
    node number
    */
  u16 node_num;
  /*!
    Data manager base address
    */
  u32 base_addr;
  /*!
    Data manager block size
    */
  u32 size;
  /*!
    below only for type = BLOCK_TYPE_IW
    */
  node_logic_info_t *p_node_logic_info;
  /*!
    first unused node id
    */
  u16 free_id;
  /*!
    form [0:fix_num] is reserved. can't auto dispatch
    */
//  u16 fix_num;
  /*!
    size for each node
    */
  u16 cache_size;
  /*!
    this block used max section number
    */
  u16 sec_num;
  /*!
    current section index
    */
  u16 sec_index;
  /*!
    cache buffer
    */
  u8 *p_cache;
}block_mgr_t;

/*!
  base info
  */
typedef struct tag_dm_base_info_t
{
  /*!
    sdram size
    */
  u32 sdram_size;
  /*!
    flash size
    */
  u32 flash_size;
  /*!
    block number
    */
  u16 block_num;
  /*!
    block header size, use to check
    */
  u16 bh_size;
  //------------->>above 12B matchs the struct in flash
  /*!
    Data manager information
    */
  u32 flash_base_addr;
  /*!
    Data manager information
    */
  u32 iw_base_addr; //in this version, all of iw block must be together
  /*!
    block total
    */
  u16 max_blk_num;
  /*!
    iw_sec total
    */
  u16 total_iw_sec_num;
  /*!
    iw section size total
    */
  u32 total_iw_sec_max_size;
  /*!
    records previous block id, usually this index as same as the previous index.
    */
  u32 prev_blk_id;
  /*!
    Data manager information
    */
  u32 piece_base_addr; //in this version, all of iw block must be together
  /*!
    data copy in sdram for piece block .only support one block now
    */
  u8 *p_ghost;
  /*!
    ghost size
    */
  u32 ghost_size;
  /*!
    buffer for decompress temp buffer
    */
  u8 *p_decmp_buf;
  /*!
    buffer for decompress temp buffer
    */
  u32 decmp_buf_size;
  /*!
    buffer for decompress cache
    */
  u8 *p_decmp_cache;
  /*!
    decompress cache size
    */
  u32 decmp_cache_size;
  /*!
    block manager
    */
  block_mgr_t *p_blk_mgr;
  /*!
    section manager
    */
  section_mgr_t sec_mgr;
  /*!
    section manager on ghost
    */
  section_mgr_t sec_mgr_ghost;
  /*!
    section manager on fixed address
    */
  section_mgr_t sec_mgr_fixed;
  /*!
    turn section
    */
  u32 monitor_turn;
  /*!
    mutex sem
    */
  void *p_dm_mutex;
  /*!
    direct erase uses buffer start address.(or sdram to sec buffer)
    */
  u8 *p_erase_buf;
  /*!
    max node length
    */
  u32 max_node_length;
  /*!
    node rw_buffer list. 
    */
  dm_node_buf_t buf_list[DM_MAX_NODE_BUFFER];
}dm_global_info_t;

/*!
  Data manager private data
  */
typedef struct tag_dm_var
{
  /*!
    Data manager information
    */
  dm_global_info_t dm_info;
  /*!
    Char storage device
    */
  charsto_device_t *p_charsto_dev;
  /*!
    create the task_proc or not
    */
  BOOL is_open_monitor;
  /*!
    use mutex or not
    */
  BOOL is_use_mutex;
  /*!
    is test mode : if your project in the test mode, please set TRUE.
    Because sometimes is hardware error, but we must assure system can work well in normal mode.
    */
  BOOL is_test_mode;
  /*!
    Device operation protect mutex
    */
  void *p_dm_mutex;
}dm_var_t;

typedef struct tag_compress_block_head
{
  u32 zip_data_size;
  u32 match_flag;
  u32 rev_1;
  u32 rev_2;
}compress_block_head_t;

/********************************************************
                    prefix define
 ********************************************************/
static s32 find_block_index(dm_var_t *p_data, u8 block_id);
static void sec_cleanup_overdue(dm_var_t *p_data, u32 sec_indx, block_type_t type);
static void write_del_node(dm_var_t *p_data, node_logic_info_t *p_node,
              u16 node_id, block_type_t block_type);

static void dm_lock_v3(dm_var_t *p_data)
{
  if(p_data->is_use_mutex)
  {
    mtos_mutex_take(p_data->p_dm_mutex);
  }
  else
  {
    mtos_task_lock();
  }
}

static void dm_unlock_v3(dm_var_t *p_data)
{
  if(p_data->is_use_mutex)
  {
    mtos_mutex_give(p_data->p_dm_mutex);
  }
  else
  {
    mtos_task_unlock();
  }
}


/*!
 dm alloc node buffer.

 \param[in] p_data : dm_var_t
 \param[in] p_name : who call this function?
 */
static u8 *dm_alloc_node_buffer(dm_var_t *p_data, char *p_name)
{
  u8 i = 0;
  
  DM_DETAIL("%s\n", p_name);
  for(i = 0; i < DM_MAX_NODE_BUFFER; i++)
  {
    if(!p_data->dm_info.buf_list[i].is_used)
    {
      p_data->dm_info.buf_list[i].is_used = TRUE;

      memset(p_data->dm_info.buf_list[i].p_node_buf, 0x0, p_data->dm_info.max_node_length);  
      return p_data->dm_info.buf_list[i].p_node_buf;
    }
  }
  MT_ASSERT(0);
  return NULL;
}

static void dm_free_node_buffer(dm_var_t *p_data, u8 *p_buf, char *p_name)
{
  u8 i = 0;
  
  DM_DETAIL("%s\n", p_name);
  for(i = 0; i < DM_MAX_NODE_BUFFER; i++)
  {
    if(p_data->dm_info.buf_list[i].p_node_buf == p_buf)
    {
      p_data->dm_info.buf_list[i].is_used = FALSE;
      return;
    }
  }
  MT_ASSERT(0);
}

/********************************************************
                    section manager functions
 ********************************************************/

static u8 * ghost_addr_transf(dm_var_t *p_data, u32 addr)
{
  u8 *p_addr = NULL;
  u32 offset = addr - p_data->dm_info.sec_mgr_ghost.sec_base_addr;

  MT_ASSERT(offset < p_data->dm_info.ghost_size);

  p_addr = p_data->dm_info.p_ghost + offset;

  return p_addr;
}


static RET_CODE data_read(dm_var_t *p_data, u32 addr,
                   u8 *p_buf, u32 len, block_type_t type)
{
  dm_global_info_t *p_dm_info = NULL;
  RET_CODE ret = SUCCESS;
  u8 *p_addr = NULL;

  MT_ASSERT(p_data != NULL);

  p_dm_info = &p_data->dm_info;

  if(BLOCK_TYPE_PIECE == type)
  {
    p_addr = ghost_addr_transf(p_data, addr);

    //read length must't bigger than actual length.
    if((p_addr + len) >
      (p_data->dm_info.p_ghost + p_data->dm_info.ghost_size))
    {
      OS_PRINTF("addr 0x%x, p_addr 0x%x, len 0x%x, p_ghost 0x%x, g_size 0x%x, base_addr 0x%x\n",
        addr,  p_addr, len,
        p_data->dm_info.p_ghost, p_data->dm_info.ghost_size,
        p_data->dm_info.sec_mgr_ghost.sec_base_addr);
      //ret = ERR_FAILURE;
      //the requirement over flow the flash space, drop the end

      len = (u32)(p_data->dm_info.p_ghost + p_data->dm_info.ghost_size) - (u32)p_addr;
    }
    //read
    memcpy(p_buf, p_addr, len);
  }
  else if(BLOCK_TYPE_IW == type)
  {
    //have a case :  read_len + node_addr > flash_size
    if((len + addr) > p_dm_info->total_iw_sec_max_size)
    {
      len = p_dm_info->total_iw_sec_max_size - addr;
    }
    ret = charsto_read(p_data->p_charsto_dev, addr, p_buf, len);
  }
  else
  {
    ret = charsto_read(p_data->p_charsto_dev, addr, p_buf, len);
  }

  return ret;
}

static RET_CODE data_writeonly(dm_var_t *p_data, u32 addr,
                  u8 *p_buf, u32 len, block_type_t type)
{
  RET_CODE ret = SUCCESS;
  u8 *p_addr = NULL;
  u32 i = 0;

  if(BLOCK_TYPE_PIECE == type)
  {
    p_addr = ghost_addr_transf(p_data, addr);
    if((p_addr + len) >
      (p_data->dm_info.p_ghost + p_data->dm_info.ghost_size))
    {
      ret = ERR_FAILURE;
    }
    else
    {
      //write
      for(i = 0; i < len; i++)
      {
        p_addr[i] &= p_buf[i];
      }
      if(!p_data->dm_info.sec_mgr_ghost.sec_data_changed)
      {
        p_data->dm_info.sec_mgr_ghost.sec_data_changed = TRUE;
      }
    }
  }
  else if((BLOCK_TYPE_IW == type) || (BLOCK_TYPE_DIRECT_RW == type) 
    || (BLOCK_TYPE_FIXED_ADDR == type))
  {
    ret = charsto_writeonly(p_data->p_charsto_dev, addr, p_buf, len);
  }
  else
  {
    MT_ASSERT(0);
  }

  return ret;
}

static RET_CODE data_erase(dm_var_t *p_data, u32 addr,
                  u32 sec_cnt, block_type_t type)
{
  RET_CODE ret = SUCCESS;
  u8 *p_addr = NULL;

  if(BLOCK_TYPE_PIECE == type)
  {
    p_addr = ghost_addr_transf(p_data, addr);
    if((p_addr + FLASH_SECTION_SIZE * sec_cnt) >
      (p_data->dm_info.p_ghost + p_data->dm_info.ghost_size))
    {
      ret = ERR_FAILURE;
    }
    else
    {
      memset(p_addr, 0xFF, FLASH_SECTION_SIZE * sec_cnt);
    }
  }
  else if((BLOCK_TYPE_IW == type) || (BLOCK_TYPE_DIRECT_RW == type)
    || (BLOCK_TYPE_FIXED_ADDR == type))
  {
    ret = charsto_erase(p_data->p_charsto_dev, addr, sec_cnt);
  }
  else
  {
    MT_ASSERT(0);
  }

  return ret;
}

static RET_CODE data_cleanup(dm_var_t *p_data, u32 addr,
                  u32 len, block_type_t type)
{
  RET_CODE ret = SUCCESS;
  u8 *p_addr = NULL;
  u32 i = 0;

  if(BLOCK_TYPE_PIECE == type)
  {
    p_addr = ghost_addr_transf(p_data, addr);
    if((p_addr + len) >
      (p_data->dm_info.p_ghost + p_data->dm_info.ghost_size))
    {
      ret = ERR_FAILURE;
    }
    else
    {
      //clear
      for(i = 0; i < len; i++)
      {
        p_addr[i] = 0xFF;
      }
    }
  }
  else
  {
    MT_ASSERT(0);
  }

  return ret;
}

/********************************************************
                    section manager functions
 ********************************************************/
static void section_actual_show(dm_var_t *p_data, char *p_tag)
{
#ifdef DM_VERIFY_OPEN
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  u32 addr = 0;
  u8 i = 0;
  RET_CODE ret = SUCCESS;
  section_header_t sec_header = {0};

  DM_VERIFY("\n\n[section_actual_show]sec base addr 0x%x\n",p_sec_mgr->sec_base_addr);

  for(i = 0; i < p_sec_mgr->total_section_num; i++)
  {
    addr = p_sec_mgr->sec_base_addr + i * FLASH_SECTION_SIZE;
    ret = data_read(p_data, addr, (u8 *)&sec_header, SEC_HEAD_SIZE, BLOCK_TYPE_IW);
    MT_ASSERT(ret == SUCCESS);
    DM_VERIFY("[actual]addr %x, sec_idx %d\n"
              "read sec : status[0x%x], block_id[0x%x], index[%d], sec_id[%d].\n\n",
              addr, i, sec_header.status, sec_header.block_id, sec_header.index, sec_header.sec_id);
  }
#endif
}

static void section_show(dm_var_t *p_data, char *p_tag)
{
#ifdef DM_DEBUG_TRACE
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = NULL;
  u32 i = 0;

  DM_PRINT("-------->section_show: %s, base_iw_addr[0x%x]\n",
    p_tag, p_sec_mgr->sec_base_addr);

  //show flash sec
  for(i = 0; i < p_sec_mgr->total_section_num; i++)
  {
    p_sec = p_sec_mgr->p_sec_arry + i;
    DM_PRINT("\tsec[%.2d]: \tindex[%.3d], block[0x%.2x], "
      "statue[0x%.8x], cur[0x%.5x], valid[0x%.5x], sec_id[%d]\n",
      i, p_sec->header.index, p_sec->header.block_id, p_sec->header.status,
      p_sec->cur_sec_addr, p_sec->valid_len, p_sec->header.sec_id);
  }


  //show ghost sec
  p_sec_mgr = &p_data->dm_info.sec_mgr_ghost;
  for(i = 0; i < p_sec_mgr->total_section_num; i++)
  {
    p_sec = p_sec_mgr->p_sec_arry + i;
    DM_PRINT("\tghost sec[%.2d]: \tindex[%.3d], block[0x%.2x], "
      "statue[0x%.8x], cur[0x%.5x], valid[0x%.5x], sec_id[%d]\n",
      i, p_sec->header.index, p_sec->header.block_id, p_sec->header.status,
      p_sec->cur_sec_addr, p_sec->valid_len, p_sec->header.sec_id);
  }
  
  DM_PRINT("-------->section_show end.\n");

#endif
}

static section_mgr_t * get_sec_mgr(dm_var_t *p_data, block_type_t type)
{
  section_mgr_t *p_sec_mgr = NULL;

  if(BLOCK_TYPE_PIECE == type)
  {
    p_sec_mgr = &p_data->dm_info.sec_mgr_ghost;
  }
  else if(BLOCK_TYPE_IW == type)
  {
    p_sec_mgr = &p_data->dm_info.sec_mgr;
  }
  else if(BLOCK_TYPE_FIXED_ADDR == type)
  {
    p_sec_mgr = &p_data->dm_info.sec_mgr_fixed;
  }
  else
  {
    MT_ASSERT(0);
  }
  return p_sec_mgr;
}

static void sec_hardware_error(dm_var_t *p_data)
{
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, BLOCK_TYPE_IW);
  u16 sec_total_num = p_sec_mgr->total_section_num;
  u32 sec_base_addr = p_sec_mgr->sec_base_addr;
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_data != NULL);

  if(!p_data->is_test_mode)
  {
    //first clear iw section
    MT_ASSERT(sec_total_num > 0);
    ret = data_erase(p_data, sec_base_addr, sec_total_num, BLOCK_TYPE_IW);
    MT_ASSERT(ret == SUCCESS);


    //second clear piece section(flash data, not sdram data), fix me?
    p_sec_mgr = get_sec_mgr(p_data, BLOCK_TYPE_PIECE);
    sec_total_num = p_sec_mgr->total_compress_section_num;
    sec_base_addr = p_sec_mgr->sec_base_addr;

    //sometimes, don't have piece block.
    if(sec_total_num > 0)
    {
      ret = data_erase(p_data, sec_base_addr, sec_total_num, BLOCK_TYPE_IW);
      MT_ASSERT(ret == SUCCESS);
    }

    #ifndef WIN32
      hal_pm_reset();
    #else
      //herdware error, please restart simulator...
      MT_ASSERT(0);
    #endif // End for WIN32
  }
  else
  {
    MT_ASSERT(0);
  }
}


static void sec_init(dm_var_t *p_data, u32 base_addr,
   u16 sec_num, u16 comp_sec_num, block_type_t type, u8 block_id)
{
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  section_info_t *p_sec = NULL;
  u16 _cur = 0;

  MT_ASSERT(NULL == p_sec_mgr->p_sec_arry); //fix me. only init once
  p_sec_mgr->sec_base_addr = base_addr;
  p_sec_mgr->total_section_num = sec_num;
  p_sec_mgr->total_compress_section_num = comp_sec_num;
  p_sec_mgr->dispatch_num = 0;
  p_sec_mgr->p_sec_arry = mtos_malloc(sec_num * sizeof(section_info_t));
  MT_ASSERT(p_sec_mgr->p_sec_arry != NULL);
  memset(p_sec_mgr->p_sec_arry, 0x0, sec_num * sizeof(section_info_t));

  //init sec header
  for(_cur = 0; _cur < sec_num; _cur++)
  {
    p_sec = p_sec_mgr->p_sec_arry + _cur;
    p_sec->cur_sec_addr = SEC_HEAD_SIZE;
    p_sec->valid_len = 0;
    if(BLOCK_TYPE_FIXED_ADDR == type)
    {
      p_sec->header.block_id = block_id;
      p_sec->header.status = SEC_WRITE;
    }
    else
    {
      data_read(p_data, base_addr + FLASH_SECTION_SIZE * _cur,
        (u8 *) &p_sec->header, SEC_HEAD_SIZE, type);
      
      if(p_sec->header.status != SEC_CLEAN)
      {
        if(_cur != p_sec->header.index)
        {
          DM_PRINT("fail header. addr 0x%x, sec[%d]: status 0x%x, index %d\n",
            base_addr + FLASH_SECTION_SIZE * _cur, _cur,
            p_sec->header.status, p_sec->header.index);
          sec_hardware_error(p_data);
        }

        if(p_sec_mgr->dispatch_num <= p_sec->header.sec_id)
        {
          //always equal next of max id.
          p_sec_mgr->dispatch_num = p_sec->header.sec_id + 1;
        }
      }
    }
  }
  
  section_show(p_data, (char *)"Init done");
}

static BOOL sec_using_out(dm_var_t *p_data, u8 block_id)
{
  s32               block_index = ERR_FAILURE;
  block_mgr_t      *p_blk_mgr   = NULL;
  section_mgr_t    *p_sec_mgr   = NULL;
  section_header_t *p_header    = NULL;
  u32               i           = 0;
  u32               total       = 0;

  block_index = find_block_index(p_data, block_id);
  MT_ASSERT(block_index != ERR_FAILURE);
  
  p_blk_mgr   = p_data->dm_info.p_blk_mgr + block_index;
  p_sec_mgr   = &p_data->dm_info.sec_mgr;

  //calc the block using sec,
  for(i = 0; i < p_sec_mgr->total_section_num; i++)
  {
    p_header = &p_sec_mgr->p_sec_arry[i].header;
    if(p_header->block_id == block_id)
    {
      total++;
    }
  }

  return (total >= p_blk_mgr->sec_num);
}

static BOOL sec_repair_check(dm_var_t *p_data, u32 sec_idx, block_type_t type)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  u32 sec_flash_addr = 0;
  u32 read_addr = 0;
  u32 offset = 0;
  u16 buf_len = 256;
  u8 p_buf[256] = {0};
  u16 i = 0;

  MT_ASSERT(p_data != NULL);

  sec_flash_addr = p_sec_mgr->sec_base_addr + (sec_idx * FLASH_SECTION_SIZE);

  for(offset = 0; offset < FLASH_SECTION_SIZE;)
  {
    read_addr = sec_flash_addr + offset;
    data_read(p_data, read_addr, p_buf, buf_len, type);

    for(i = 0; i < buf_len; i++)
    {
      if(p_buf[i] != 0xFF)
      {
        //if find one byte not equal 0xff, erase this section again.
        data_erase(p_data, sec_flash_addr, 1, type);
        return FALSE;
      }
    }

    offset += buf_len;
  }

  return TRUE;
}


static BOOL sec_find_new(dm_var_t *p_data, u32 *p_new_sec_idx,
  u8 block_id, sec_status_t prio_sel)
{
  s32               block_index = ERR_FAILURE;
  block_type_t      block_type  = 0;
  section_mgr_t    *p_sec_mgr   = NULL;
  section_header_t *p_header    = NULL;
  u32 i = 0;
  
  block_index = find_block_index(p_data, block_id);
  MT_ASSERT(block_index != ERR_FAILURE);
  
  block_type = p_data->dm_info.p_blk_mgr[block_index].type;
  p_sec_mgr = get_sec_mgr(p_data, block_type);

  //first find the prio sel
  for(i = 0; i < p_sec_mgr->total_section_num; i++)
  {
    p_header = &p_sec_mgr->p_sec_arry[i].header;
    if((prio_sel == p_header->status)
      && (p_header->block_id == block_id))
    {
      DM_PRINT("find sec first: sec[%d], block_id[0x%x], prio[0x%x]\n",
        i, block_id, prio_sel);
      *p_new_sec_idx = i;
      return TRUE;
    }
  }

  if(sec_using_out(p_data, block_id))
  {
    return FALSE;
  }

  //second find the SEC_CLEAN
  //this status means: the block is idle
  for(i = 0; i < p_sec_mgr->total_section_num; i++)
  {
    p_header = &p_sec_mgr->p_sec_arry[i].header;
    if(SEC_CLEAN == p_header->status)
    {
      if(0xFF != p_header->block_id)
      {
        DM_ERROR("find sec second: status[0x%x], blk_id[0x%x], index[0x%x], sec_id[0x%x],"
            "rev_2[0x%x], rev_3[0x%x], rev_4[0x%x]\n",
        p_header->status, p_header->block_id, p_header->index, p_header->sec_id,
        p_header->rev_2, p_header->rev_3, p_header->rev_4);
        MT_ASSERT(0);
      }
      p_header->index = i;
      p_header->sec_id = p_sec_mgr->dispatch_num;
      p_sec_mgr->dispatch_num++;
      DM_PRINT("find sec second: sec[%d], block_id[0x%x], prio[0x%x]\n",
        i, block_id, prio_sel);
      *p_new_sec_idx = i;
      sec_repair_check(p_data, i, block_type);
      return TRUE;
    }
  }

  DM_ERROR("ERROR: can't find new sec\n");
  section_show(p_data,(char *) "can't find new sec");
  return FALSE;
}

static void sec_set_status(dm_var_t *p_data, u32 sec_indx,
  sec_status_t sec_st, u8 block_id)
{
  s32             block_index = ERR_FAILURE;
  block_type_t    block_type  = 0;
  section_mgr_t  *p_sec_mgr   = NULL;
  section_info_t *p_sec       = NULL;
  u32             addr        = 0;

  block_index = find_block_index(p_data, block_id);
  MT_ASSERT(block_index != ERR_FAILURE);
  
  block_type  = p_data->dm_info.p_blk_mgr[block_index].type;
  p_sec_mgr   = get_sec_mgr(p_data, block_type);
  p_sec       = p_sec_mgr->p_sec_arry + sec_indx;
  addr        = p_sec_mgr->sec_base_addr + sec_indx * FLASH_SECTION_SIZE;

  DM_PRINT("sec_set_status: sec[%d], block_id[0x%x], status[0x%x]\n",
    sec_indx, block_id, sec_st);

  switch(sec_st)
  {
    case SEC_CLEAN:
      p_sec->cur_sec_addr = SEC_HEAD_SIZE;
      p_sec->valid_len = 0;
      memset(&p_sec->header, 0xFF, SEC_HEAD_SIZE);
      break;
    case SEC_STREAM_IN:
    case SEC_WRITE:
    case SEC_USED:
    case SEC_STREAM_OUT:
    case SEC_STREAM_OVERDUE:
      p_sec->header.status = sec_st;
      p_sec->header.block_id = block_id;
      MT_ASSERT(p_sec->header.index == sec_indx);
      MT_ASSERT(p_sec->header.sec_id < p_sec_mgr->dispatch_num);
      data_writeonly(p_data, addr,
        (u8 *)&p_sec->header, SEC_HEAD_SIZE, block_type);
#ifdef DM_VERIFY_OPEN
      {
        section_header_t sec_header;
        data_read(p_data, addr,
          (u8 *)&sec_header, SEC_HEAD_SIZE, block_type);
        if(sec_header.status != sec_st || sec_header.block_id != block_id ||
          sec_header.index != sec_indx ||
          sec_header.sec_id != p_sec->header.sec_id)
        {
          DM_VERIFY("sec : status[0x%x], block_id[0x%x], index[%d], id[%d], "
              "read sec : status[0x%x], block_id[0x%x], index[%d], id[%d].\n",
            p_sec->header.status, p_sec->header.block_id, p_sec->header.index,
            p_sec->header.sec_id, sec_header.status, sec_header.block_id,
            sec_header.index, sec_header.sec_id);          
          MT_ASSERT(0);
        }
      }
#endif
      break;
    default:
      break;
  }
  section_show(p_data, (char *)"sec_set_status end");
}

static u32 sec_write_node(dm_var_t *p_data, u32 sec_indx,
  u8 *p_buffer, u32 len, block_type_t type)
{
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  //iw addr : base for iw first address
  u32 iw_addr = 0;

  MT_ASSERT(p_sec->cur_sec_addr + len <= FLASH_SECTION_SIZE);
  //need protect by task lock
  iw_addr = sec_indx * FLASH_SECTION_SIZE + p_sec->cur_sec_addr;

  data_writeonly(p_data,
        p_sec_mgr->sec_base_addr + iw_addr, p_buffer, len, type);

  p_sec->cur_sec_addr += len;
  p_sec->valid_len += len;

  DM_DETAIL("sec_wr : iw_addr[0x%x], sec_indx[%d], sec_addr[0x%x],"
    " valid_len[0x%x], node_id[%d], block_id[0x%x], w_len[0x%x]\n",
    iw_addr, sec_indx, p_sec->cur_sec_addr, p_sec->valid_len,
    ((node_header_t *)p_buffer)->id, p_sec->header.block_id, len);

#ifdef DM_VERIFY_OPEN
  {
    node_header_t head;
    node_header_t *p_src_head = (node_header_t *)p_buffer;
    data_read(p_data, p_sec_mgr->sec_base_addr + iw_addr,
      (u8 *)&head, NODE_HEADER_SIZE, type);
    if(0 == head.length || head.length != p_src_head->length
      || 0 == head.active || head.id != p_src_head->id)
    {
      DM_ERROR("src header: id[%d], len[%d], act[%d],"
        " read header[0x%x]: id[%d], len[%d], act[%d]\n",
        p_src_head->id, p_src_head->length, p_src_head->active,
        iw_addr, head.id, head.length, head.active);
      MT_ASSERT(0);
    }
  }
#endif

  return iw_addr;
}

static void sec_del_node(dm_var_t *p_data, u32 sec_indx, u32 sec_offset,
  u8 *p_buffer, u32 w_len, u32 node_len, block_type_t type)
{
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  sec_status_t status = p_sec->header.status;
  u32 iw_addr = sec_indx * FLASH_SECTION_SIZE + sec_offset;
  u32 flash_addr = p_sec_mgr->sec_base_addr + iw_addr;
#ifdef DM_VERIFY_OPEN
  node_header_t head;
#endif

  MT_ASSERT((SEC_STREAM_IN == status) || (SEC_STREAM_OUT == status)
    || (SEC_WRITE == status) || (SEC_USED == status));

  if(p_sec->valid_len < node_len)
  {
    section_show(p_data,  (char *)"sec_del_node error\n");
    DM_ERROR("del node error. flash addr 0x%x, valid_len %d, node_len %d\n",
      flash_addr, p_sec->valid_len, node_len);
  }
  MT_ASSERT(p_sec->valid_len >= node_len);

#ifdef DM_VERIFY_OPEN
  memset(&head, 0, sizeof(node_header_t));
  data_read(p_data, flash_addr, (u8 *)&head, NODE_HEADER_SIZE, type);
  DM_VERIFY("read_s header : id[%d], length[%d], active[%d]\n",
    head.id, head.length, head.active);
#endif

  data_writeonly(p_data, flash_addr, p_buffer,
    w_len, type);
  p_sec->valid_len -= node_len;

  DM_DETAIL("sec_del: iw_addr[0x%x], sec_indx[%d], sec_addr[0x%x],"
    " valid_len[0x%x], node_id[%d], block_id[0x%x]\n",
    iw_addr, sec_indx, p_sec->cur_sec_addr, p_sec->valid_len,
    ((node_header_t *)p_buffer)->id, p_sec->header.block_id);

#ifdef DM_VERIFY_OPEN
  {
    node_header_t *p_src_head = (node_header_t *)p_buffer;
    memset(&head, 0, sizeof(node_header_t));
    data_read(p_data, flash_addr, (u8 *)&head, NODE_HEADER_SIZE, type);
    if(0 == head.length || head.length != p_src_head->length
      || 0 != head.active || head.id != p_src_head->id)
    {
      DM_VERIFY("src header: id[%d], len[%d], act[%d],"
        " read_e header: id[%d], len[%d], act[%d]\n",
        p_src_head->id, p_src_head->length, p_src_head->active,
        head.id, head.length, head.active);
      MT_ASSERT(0);
    }
  }
#endif
}

static u8 * sec_alloc_buf(dm_global_info_t *p_dm_info)
{
  u8 *p_buf = NULL;

  if(p_dm_info->p_erase_buf != NULL)
  {
    DM_PRINT("sec alloc buf has erase buffer . addr 0x%x\n", p_dm_info->p_erase_buf);
    p_buf = p_dm_info->p_erase_buf;
  }
  else
  {
    p_buf = mtos_malloc(FLASH_SECTION_SIZE);
  }
  MT_ASSERT(p_buf != NULL);
  memset(p_buf, 0x0, FLASH_SECTION_SIZE);    
  return p_buf;
}


static void sec_free_buf(dm_global_info_t *p_dm_info, u8 *p_buf)
{
  if(p_dm_info->p_erase_buf != NULL)
  {
    DM_PRINT("sec free buf has erase buffer . addr 0x%x\n", p_dm_info->p_erase_buf);
    return;
  }
  
  mtos_free(p_buf); //fix me: it come from mtos_malloc???
}

//this function will be ventured
static void sec_to_sdram(dm_var_t *p_data, u32 sec_indx, block_type_t type)
{
  block_mgr_t *p_blk_mgr = p_data->dm_info.p_blk_mgr;
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  s32 block_idx = 0;
  u32 flash_addr = p_sec_mgr->sec_base_addr + sec_indx * FLASH_SECTION_SIZE;
  u32 offset = SEC_HEAD_SIZE; //current sec
  u32 cur_node_size = 0; //node step
  u32 write_size = 0;
  node_logic_info_t *p_logic_node = NULL;
  u32 node_iw_addr = 0;
  BOOL ret = FALSE;
  u8 *p_sec_buf = sec_alloc_buf(&p_data->dm_info);
  node_header_t *p_header = NULL;
  u8 *p_rw_buf = NULL;
  u8 block_id = p_sec->header.block_id;
  u8 *p_node_buf = NULL;

  DM_ERROR("ERROR!! Enter sec_to_sdram: sce[%d], block_id %d\n", sec_indx, block_id);
  section_show(p_data,  (char *)"enter sec_to_sdram");

  p_node_buf = dm_alloc_node_buffer(p_data, "sec_to_sdram");
  MT_ASSERT(p_node_buf != NULL);

  if(BLOCK_TYPE_FIXED_ADDR == type)
  {
    data_read(p_data, flash_addr, p_sec_buf, FLASH_SECTION_SIZE, type);
    data_erase(p_data, flash_addr, 1, type);
    p_sec->cur_sec_addr = SEC_HEAD_SIZE;
    p_sec->valid_len = 0;
    p_sec->header.status = SEC_WRITE;
    p_sec->header.block_id = block_id;
  }
  else 
  {
    block_idx = find_block_index(p_data, block_id);
    MT_ASSERT(block_idx != ERR_FAILURE);

    //read all data out
    data_read(p_data, flash_addr,
      p_sec_buf, FLASH_SECTION_SIZE, type);

    //reset it, NOTE !!! the p_sec info was changed
    sec_cleanup_overdue(p_data, sec_indx, type);

    //get it back.
    ret = sec_find_new(p_data, &sec_indx, block_id, SEC_CLEAN);
    MT_ASSERT(ret);

    //init the sec
    sec_set_status(p_data, sec_indx, SEC_STREAM_IN, block_id);
  }
  
  while(offset < FLASH_SECTION_SIZE)
  {
    //read a node header
    p_header = (node_header_t *)(p_sec_buf + offset);

    if(ERASE_DATA == p_header->value)  //find invalid data, means read finish
    {
      DM_DETAIL("s2r: finished on sec_addr[0x%x]\n", offset);
      break;
    }

    cur_node_size = (p_header->length * DM_ATOM_SIZE);

    //DO NOT need read the intact_flag!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //because if the node is unintace, the maping point the older
    //data_read(p_data, addr + offset + step,
    //  &intact_flag, 1);

    p_logic_node = p_blk_mgr[block_idx].p_node_logic_info + p_header->id;
    node_iw_addr = p_logic_node->node_iw_addr;

    DM_DETAIL("s2r:iw_addr[0x%x], node_id[%d], len[%d], act[%d], link[%d]\n",
      sec_indx * FLASH_SECTION_SIZE + offset, p_header->id, cur_node_size,
      p_header->active,
      (0 != p_logic_node->node_iw_addr && 0 != p_logic_node->active));

    //unexpected data, skip it
    if(p_header->id >= p_blk_mgr[block_idx].node_num)
    {
    }
    //the size is invalid, skip it
    else if(cur_node_size > p_data->dm_info.max_node_length)
    {
    }
    //the size is invalid, skip it
    else if(0 == cur_node_size)
    {
      DM_ERROR(" ZERO size: header_id[%d], block[0x%x], addr[0x%x]\n",
        p_header->id, block_id, flash_addr + offset);
      MT_ASSERT(0);
      break;  //fix me. how to found the next right node????
    }
    //the node is deleted, skip it
    else if(0 == p_header->active)
    {
    }
     //the node was deleted on linker, skip it
    else if(0 == p_logic_node->node_iw_addr
      || 0 == p_logic_node->active)
    {
      p_logic_node->active = 0;
      p_logic_node->node_iw_addr = 0;
    }
    //good node
    else
    {
      //the read node data
      if((node_iw_addr + p_sec_mgr->sec_base_addr) == (flash_addr + offset))
      {
        //mapping this
        p_rw_buf = p_sec_buf + offset;
        write_size = cur_node_size;
      }
      else
      {
        p_header = (node_header_t *)p_node_buf;
        //get the data size
        data_read(p_data, p_sec_mgr->sec_base_addr +
          node_iw_addr, p_node_buf, NODE_HEADER_SIZE, type);
        write_size = p_header->length * DM_ATOM_SIZE;

        //read whole node
        data_read(p_data, p_sec_mgr->sec_base_addr +
          node_iw_addr, p_node_buf, write_size, type);
        p_header->active = 1; //must set it. the mapping may point older
        p_rw_buf = p_node_buf;
        //don't need it. because node is older data, it should be deactived
      }
      p_blk_mgr[block_idx].p_node_logic_info[p_header->id].node_iw_addr =
        sec_write_node(p_data, sec_indx, p_rw_buf, write_size, type);
    }
    offset += cur_node_size;
  }

  dm_free_node_buffer(p_data, p_node_buf, "sec_to_sdram");
  sec_free_buf(&p_data->dm_info, p_sec_buf);
}

static void sec_to_sec(dm_var_t *p_data, u32 sec_out_indx, block_type_t type)
{
  block_mgr_t *p_blk_mgr = p_data->dm_info.p_blk_mgr;
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  section_info_t *p_out_sec = p_sec_mgr->p_sec_arry + sec_out_indx;
  section_info_t *p_in_sec = NULL;
  node_logic_info_t *p_node_logic = NULL;
  u32 transfer_sec_indx = 0;
  u32 new_iw_addr = 0;
  s32 block_index = 0;
  u32 out_sec_flash_addr = 0;
  u32 offset = SEC_HEAD_SIZE;
  u32 cur_node_size = 0; //node step
  BOOL find_idle_sec = FALSE;
  u16 node_id = 0;
  u8 *p_node_buf = NULL;
  node_header_t *p_header = NULL;

  p_node_buf = dm_alloc_node_buffer(p_data, "sec_to_sec");
  MT_ASSERT(p_node_buf != NULL);
  
  p_header = (node_header_t *)p_node_buf;

  DM_PRINT("Enter sec_to_sec: sce[%d]\n", sec_out_indx);
  section_show(p_data, (char *) "sec_to_sec");
  //clean up block
  block_index = find_block_index(p_data, p_out_sec->header.block_id);
  MT_ASSERT(block_index != ERR_FAILURE);
  
  out_sec_flash_addr = p_sec_mgr->sec_base_addr +
    sec_out_indx * FLASH_SECTION_SIZE;

  while(offset < FLASH_SECTION_SIZE)
  {
    //protect every cycle
    //DM_LOCK(p_data);
    //read a node header
    data_read(p_data, out_sec_flash_addr + offset,
      p_node_buf, NODE_HEADER_SIZE, type);

    if(ERASE_DATA == p_header->value)  //find invalid data, means read finish
    {
      DM_DETAIL("s2s: finished on sec_addr[0x%x]\n", offset);
      break;
    }

    cur_node_size = (p_header->length * DM_ATOM_SIZE);

    //check the stream in section. no section or the section is full
    if((NULL == p_in_sec) ||
        (p_in_sec->cur_sec_addr + cur_node_size > FLASH_SECTION_SIZE))
    {
      if(NULL != p_in_sec)  //the section is full
      {
        sec_set_status(p_data, transfer_sec_indx,
          SEC_USED, p_out_sec->header.block_id);
      }

      //find an idle section to transfer
      find_idle_sec = sec_find_new(p_data, &transfer_sec_indx,
        p_out_sec->header.block_id, SEC_STREAM_IN);
      if(find_idle_sec) //found the section
      {
        p_in_sec = p_sec_mgr->p_sec_arry + transfer_sec_indx;
        //set the new sec status to SEC_STREAM_IN.
        //means the block is transfering in, and the
        //block id is the out section's
        sec_set_status(p_data, transfer_sec_indx,
          SEC_STREAM_IN, p_out_sec->header.block_id);
      }
      else
      {
        sec_to_sdram(p_data, sec_out_indx, type);
        return;
      }
    }

    //DO NOT need read the intact_flag!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //because if the node is unintace, the maping point the older
    //data_read(p_data, addr + offset + step,
    //  &intact_flag, 1);

    node_id = p_header->id;
    p_node_logic = p_blk_mgr[block_index].p_node_logic_info + node_id;

    DM_DETAIL("s2s:iw_addr[0x%x], node_id[%d], len[%d], act[%d], link[%d]\n",
      sec_out_indx * FLASH_SECTION_SIZE + offset, node_id, cur_node_size,
      p_header->active,
      (0 != p_node_logic->node_iw_addr && 0 != p_node_logic->active));

    //unexpected data, skip it
    if(p_header->id >= p_blk_mgr[block_index].node_num)
    {
    }
    //the size is invalid, skip it
    else if(cur_node_size > p_data->dm_info.max_node_length)
    {
    }
    //the size is invalid, skip it
    else if(0 == cur_node_size)
    {
      DM_ERROR(" ZERO size: header_id[%d], block[0x%x], addr[0x%x]\n",
        p_header->id, p_out_sec->header.block_id, out_sec_flash_addr + offset);
      MT_ASSERT(0);
      break;  //fix me. how to found the next right node????
    }
    //the node is deleted, skip it
    else if(0 == p_header->active)
    {
    }
     //the node was deleted on linker, skip it
    else if(0 == p_node_logic->node_iw_addr
      || 0 == p_node_logic->active)
    {
      //don't need delete it. because the section will be erase, or later
      //jump to sec_to_sdram(), the node still be droped
      //but the sec->valid_len can't go back to ZERO.
      p_node_logic->active = 0;
      p_node_logic->node_iw_addr = 0;
    }
    //good node
    else
    {
      //the read addr is mapping addr, but current
      //read the data enough
      data_read(p_data, p_sec_mgr->sec_base_addr
        + p_node_logic->node_iw_addr, p_node_buf, p_data->dm_info.max_node_length, type);

      //the buffer reload
      MT_ASSERT(node_id == p_header->id);

      p_header->active = 1; //must set it. because the mapping may point older

      //NOTE!! must recalc the node size, because the two time size is unequal
      //new_iw_addr = sec_write_node(p_data, transfer_sec_indx, rw_buf, step);
      new_iw_addr = sec_write_node(p_data, transfer_sec_indx, p_node_buf,
        p_header->length * DM_ATOM_SIZE, type);

      //delete cur node
      write_del_node(p_data, p_node_logic, p_header->id, type);

      //NOTE!!!NOTE!!!NOTE!!!NOTE!!!
      // "p_node_logic->node_iw_addr" will be set to
      // NULL in the function write_del_node()
      p_node_logic->node_iw_addr = new_iw_addr;
    }
    offset += cur_node_size;
  }

  dm_free_node_buffer(p_data, p_node_buf, "sec_to_sec");

  //data was transfer out DONE,set status to overdru
  sec_set_status(p_data, sec_out_indx,
    SEC_STREAM_OVERDUE, p_in_sec->header.block_id);
}

static void sec_cleanup_out(dm_var_t *p_data, u32 sec_indx, block_type_t type)
{
  u32 enter_ticks = mtos_ticks_get();
  u32 exit_ticks = 0;

  DM_PRINT("Enter sec_cleanup_out: sce[%d]\n", sec_indx);
  sec_to_sec(p_data, sec_indx, type);

  exit_ticks = mtos_ticks_get();
  if(exit_ticks - enter_ticks > 100)
  {
    DM_ERROR("sec_cleanup_out spend long time %d ticks\n",
      exit_ticks - enter_ticks);
  }

  section_show(p_data, (char *) "Exit sec_cleanup_out");
}

static void sec_cleanup_overdue(dm_var_t *p_data, u32 sec_indx, block_type_t type)
{
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  u32 addr = p_sec_mgr->sec_base_addr + sec_indx * FLASH_SECTION_SIZE;
  u32 enter_ticks = mtos_ticks_get();
  u32 exit_ticks = 0;

  DM_PRINT("Enter sec_cleanup_overdue:sce[%d], addr[0x%x]\n", sec_indx, addr);
  //erase flash
  data_erase(p_data, addr, 1, type);

  //set status
  sec_set_status(p_data, sec_indx, SEC_CLEAN, p_sec->header.block_id);

  exit_ticks = mtos_ticks_get();
  if(exit_ticks - enter_ticks > 100)
  {
    DM_ERROR("sec_cleanup_overdue spend long time %d ticks\n",
      exit_ticks - enter_ticks);
  }

  section_actual_show(p_data, "exit sec_cleanup_overdue");
  section_show(p_data, "Exit sec_cleanup_overdue");
}

static void sec_cleanup_full(dm_var_t *p_data, u32 sec_indx, block_type_t type)
{
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;

  DM_PRINT("Enter sec_cleanup_full: sce[%d]\n", sec_indx);
  //set the head info
  sec_set_status(p_data, sec_indx, SEC_STREAM_OUT, p_sec->header.block_id);

  //do clean
  sec_cleanup_out(p_data, sec_indx, type);
  section_show(p_data, "Exit sec_cleanup_full");
}

static void sec_check_clean(dm_var_t *p_data)
{
  section_mgr_t *p_sec_mgr = NULL;
  sec_status_t sec_st = SEC_CLEAN;
  u32 total_sec = 0;
  u32 i = 0;
  block_type_t type = BLOCK_TYPE_IW;

  MT_ASSERT(p_data != NULL);
  type = (p_data->dm_info.monitor_turn % 2) ? BLOCK_TYPE_IW : BLOCK_TYPE_PIECE;
  p_data->dm_info.monitor_turn++;
  p_sec_mgr = get_sec_mgr(p_data, type);
  total_sec = p_sec_mgr->total_section_num;

  //first find the SEC_STREAM_OVERDUE
  //this status means: the block is overdue
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    if(SEC_STREAM_OVERDUE == sec_st)
    {
      DM_PRINT("check_block: SEC_STREAM_OVERDUE sec[%d]\n", i);      
      dm_lock_v3(p_data);
      sec_cleanup_overdue(p_data, i, type);      
      dm_unlock_v3(p_data);
      return;
    }
  }

  //second find the SEC_STREAM_OUT
  //this status means: the block is doing cleanup, unexpected break
  //eg. shut down;
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    if(SEC_STREAM_OUT == sec_st)
    {
      DM_PRINT("check_block: SEC_STREAM_OUT sec[%d]\nd", i);      
      dm_lock_v3(p_data);
      sec_cleanup_out(p_data, i, type);      
      dm_unlock_v3(p_data);
      return;
    }
  }

  //third find the SEC_STREAM_IN or SEC_USED
  //this status means: the block is valid, but don't write current
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    if((SEC_STREAM_IN == sec_st) || (SEC_USED == sec_st))
    {
      if((p_sec_mgr->p_sec_arry[i].cur_sec_addr >= CRITICAL_SIZE)
        && (p_sec_mgr->p_sec_arry[i].valid_len <= MIN_VALID_DATA))
      {
        DM_PRINT("check_block: SEC_STREAM_IN or USED sec[%d]\n", i);
        dm_lock_v3(p_data);
        sec_cleanup_full(p_data, i, type);        
        dm_unlock_v3(p_data);
        return;
      }
    }
  }

  //need check the write node?  //fix me???
}

//quick find a idle sec
static BOOL sec_check_clean_quick(dm_var_t *p_data, u8 block_id, block_type_t type)
{
  section_mgr_t *p_sec_mgr = NULL;
  sec_status_t sec_st = SEC_CLEAN;
  u32 use_len = 0;
  u32 valid_len = 0;
  u32 total_sec = 0;
  u32 i = 0;
  u8 sec_block_id = 0;
  BOOL is_using_out = sec_using_out(p_data, block_id);

  MT_ASSERT(p_data != NULL);
  p_sec_mgr = get_sec_mgr(p_data, type);
  total_sec = p_sec_mgr->total_section_num;


  //first find the SEC_STREAM_OVERDUE
  //this status means: the block is overdue
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    sec_block_id = p_sec_mgr->p_sec_arry[i].header.block_id;
    //case 1: current section status is SEC_STREAM_OVERDUE
    //        and current block not used all sections. now,
    //        whoever this section belongs to,
    //        can cleanup this section.
    //case 2: curretn section status is SEC_STREAM_OVERDUE
    //        and current section belongs to this block,
    //        can cleanup this section.
    if((SEC_STREAM_OVERDUE == sec_st) &&
          ((!is_using_out) || (sec_block_id == block_id)))
    {
      DM_PRINT("check_block_q: SEC_STREAM_OVERDUE sec[%d]\n", i);
      sec_cleanup_overdue(p_data, i, type);
      return TRUE;
    }
  }

  //second find the SEC_USED
  //this status means: all of nodes in the block is invalid, erase directly
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    valid_len = p_sec_mgr->p_sec_arry[i].valid_len;
    sec_block_id = p_sec_mgr->p_sec_arry[i].header.block_id;
    //case 1: current section status is SEC_USED and valid_len is zero.
    //        and current block not used all sections. now,
    //        whoever this section belongs to,
    //        can cleanup this section.
    //case 2: current section status is SEC_USED and valid_len is zero.
    //        and current section belongs to this block,
    //        can cleanup this section.
    if(((SEC_USED == sec_st) && (0 == valid_len)) &&
      ((!is_using_out) || (sec_block_id == block_id)))
    {
      DM_PRINT("check_block_q: SEC_USED sec[%d], zero valid data\n", i);
      sec_cleanup_overdue(p_data, i, type);
      return TRUE;
    }
  }

  //NOTE!!!!!!!, can't check the SEC_STREAM_OUT, because the sec used by
  //sec_check_clean in lowest prio-task

  //third find the SEC_USED
  //this status means: this section has some invalid node. so clean up it.
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    sec_block_id = p_sec_mgr->p_sec_arry[i].header.block_id;
    valid_len = p_sec_mgr->p_sec_arry[i].valid_len;
    use_len = p_sec_mgr->p_sec_arry[i].cur_sec_addr - SEC_HEAD_SIZE;
    if((sec_block_id == block_id) && (SEC_USED == sec_st)
      && (use_len > valid_len))
    {
      DM_PRINT("check_block_q: SEC_USED sec[%d]\n", i);
      //don't need task lock, because the abnormity process is locked
      sec_to_sdram(p_data, i, type);
      return TRUE;
    }
  }

  return FALSE;
}

static BOOL sec_abnormity_process(dm_var_t *p_data, u32 *p_new_sec_idx,
  u8 block_id)
{
  BOOL ret          = FALSE;
  BOOL found_ret    = FALSE;
  s32 block_indx    = ERR_FAILURE;
  block_type_t type = BLOCK_TYPE_UNKNOWN;

  block_indx = find_block_index(p_data, block_id);
  MT_ASSERT(block_indx != ERR_FAILURE);

  type = p_data->dm_info.p_blk_mgr[block_indx].type;

  DM_ERROR("enter sec_abnormity_process\n");
  section_show(p_data, "enter sec_abnormity_process");

  ret = sec_check_clean_quick(p_data, block_id, type);
  if(ret)
  {
    found_ret = sec_find_new(p_data, p_new_sec_idx, block_id, SEC_STREAM_IN);
    if(!found_ret)
    {
      section_show(p_data, "exit sec_abnormity_process");
      DM_ERROR("exit clean_quick, but can't find an idle section\n");
      MT_ASSERT(0);
    }
  }

  section_show(p_data, "exit sec_abnormity_process");
  DM_ERROR("exit sec_abnormity_process. ret %d\n", ret);
  return ret;
}

static void sec_sort(section_header_t *p_header_list, u8 depth)
{
  u8 i = 0;
  u8 j = 0;
  section_header_t temp;

  for(i = 0; i < depth; i++)
  {
    for(j = i + 1; j < depth; j++)
    {
      MT_ASSERT(p_header_list[i].sec_id != p_header_list[j].sec_id);
      if(p_header_list[i].sec_id > p_header_list[j].sec_id)
      {
        temp = p_header_list[i];
        p_header_list[i] = p_header_list[j];
        p_header_list[j] = temp;
      }
    }
  }
}

/*!
  reconstruct IW block node info
  \return FALSE if fail, or TRUE if success
  */
static BOOL sec_restore(dm_var_t *p_data, block_type_t type)
{
  dm_global_info_t *p_dm_info = &p_data->dm_info;
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  block_mgr_t *p_blk_mgr = p_dm_info->p_blk_mgr;
  node_logic_info_t *p_node = NULL;
  section_info_t *p_sec = NULL; //sec info
  node_header_t header = {{0}};
  u32 total_sec = p_sec_mgr->total_section_num;
  u32 step = 0; //node step
  u32 i = 0;
  u32 _cur = 0; //section point
  u32 sec_iw_addr = 0; //section addr base for ie base
  u32 sec_flash_addr = 0; //section addr base for flash zero addr
  u32 sec_index = 0; //section index uses dispose uninact or repeat node
  u32 sec_offset = 0; //ditto
  u32 offset = 0; //section offset base current sec.
  s32 block_index = 0;  //iw block index
  sec_status_t sec_st = SEC_CLEAN;
  u8 intact_flag = 0;
  u8 block_id = 0;
  u8 normal_depth = 0;
  u8 write_depth = 0;
  section_header_t *p_sort_normal = NULL;
  section_header_t *p_sort_write = NULL;

  //sort sec by dispath order
  p_sort_normal = mtos_malloc(SEC_HEAD_SIZE * total_sec);
  MT_ASSERT(NULL != p_sort_normal);
  memset(p_sort_normal, 0, SEC_HEAD_SIZE * total_sec);
  
  p_sort_write = mtos_malloc(SEC_HEAD_SIZE * total_sec);
  MT_ASSERT(NULL != p_sort_write);
  memset(p_sort_write, 0, SEC_HEAD_SIZE * total_sec);

  if(BLOCK_TYPE_FIXED_ADDR == type)
  {
    for(i = 0; i < p_sec_mgr->total_section_num; i++)
    {
      sec_iw_addr = i * FLASH_SECTION_SIZE;
      sec_flash_addr = p_sec_mgr->sec_base_addr + sec_iw_addr;
      offset = SEC_HEAD_SIZE; //section offset. skip header
      p_sec = p_sec_mgr->p_sec_arry + i;
      block_id = p_sec->header.block_id;
      p_sec->valid_len = 0;
      block_index = find_block_index(p_data, block_id);
      MT_ASSERT(block_index != ERR_FAILURE);
      sec_st = p_sec->header.status;
      intact_flag = 0;

      p_node = p_blk_mgr[block_index].p_node_logic_info;
        
      OS_PRINTF("sec_restore sec_iw_addr %d, sec_flash_addr %X\n", sec_iw_addr, sec_flash_addr);
        
      while(offset < FLASH_SECTION_SIZE)
      {
        //read a node header
        data_read(p_data, sec_flash_addr + offset,
          (u8 *)(&header), NODE_HEADER_SIZE, type);

        if(ERASE_DATA == header.value)  //find invalid data, means read finish
        {
          DM_PRINT("restore block[0x%x] section[%d], load[0x%x], end 0x%x\n",
            block_id, _cur, p_sec->valid_len, offset);
          p_sec->cur_sec_addr = offset;
          break;
        }
        step = (header.length * DM_ATOM_SIZE);

        //read a node header
        data_read(p_data, sec_flash_addr + offset + step - 1,
          &intact_flag, 1, type);

        //unexpected data, skip it
        if(header.id >= p_blk_mgr[block_index].node_num)
        {
          DM_ERROR("ERROR !!! bigger id: header_id[%d], block[0x%x],"
            "node_num[%d], addr[0x%x]\n",
            header.id, block_id, p_blk_mgr[block_index].node_num,
            sec_flash_addr + offset);
        }
        //the size is invalid, skip it,
        //fix me ,it's trouble. the step is error, can jump to the next??
        else if(step > p_data->dm_info.max_node_length)
        {
          DM_ERROR("ERROR !!! bigger size: header_id[%d], block[0x%x],"
            "size[%d], addr[0x%x]\n",
            header.id, block_id, step, sec_flash_addr + offset);
        }
        else if(0 == step)
        {
          DM_ERROR("ERROR !!! ZERO size: header_id[%d], block[0x%x],"
            "addr[0x%x]\n",
            header.id, block_id, sec_flash_addr + offset);
          break;  //fix me. how to found the next right node????
        }
        //the node is deleted, skip it
        else if(0 == header.active)
        {
      #if 0 //for debug
          if(type == BLOCK_TYPE_PIECE)
          {
            u8 data[32] = {0};
            data_read(p_data, sec_flash_addr + offset + NODE_HEADER_SIZE,
              data, 32, type);

            data[31] = 0;
            MT_ASSERT(data[0] == 0xFF);
          }
      #endif
          DM_DETAIL("find old node: id[%d], block[0x%x], addr[0x%x],"
            " active[%d]\n", header.id, block_id, sec_iw_addr + offset,
            p_node[header.id].active);
        }
        //the node is unintact
        else if(NODE_INTACT_FLAG != intact_flag)
        {
          DM_ERROR("ERROR found a unintact[impaired] node!!! header_id[%d], "
            "flag[0x%x], block[0x%x], node_num[%d], addr[0x%x]\n",
            header.id, intact_flag, block_id, p_blk_mgr[block_index].node_num,
            sec_flash_addr + offset + step - 1);

          //delete current bad node.
          header.active = 0;
          sec_del_node(p_data, _cur, offset, (u8 *)&header, NODE_HEADER_SIZE,
            header.length * DM_ATOM_SIZE, type);

          DM_ERROR("ERROR!!! dispose impaired node\n ");

          //use the old node, active it
          //don't need do anything !!!!!
          //because delete operation is after write (see write_node_to_flash)
          //so if the write operation was break, the last node isn't deleted

          //if(p_node[header.id].node_iw_addr != 0)
          //{
          //  p_node[header.id].active = 1;
          //}
          //p_sec->valid_len += step; //valid node
        }
        //good node
        else
        {
          if(1 == p_node[header.id].active) //the active node was found
          {
            DM_ERROR("ERROR!! found repeat node: id[%d], block[0x%x], "
              "old addr[0x%x], now [0x%x]\n",
              header.id, block_id, p_node[header.id].node_iw_addr,
              sec_iw_addr + offset);

            sec_index = p_node[header.id].node_iw_addr / FLASH_SECTION_SIZE;
            sec_offset = p_node[header.id].node_iw_addr % FLASH_SECTION_SIZE;

            //delete old node.
            header.active = 0;
            sec_del_node(p_data, sec_index, sec_offset, (u8 *)&header, NODE_HEADER_SIZE,
              header.length * DM_ATOM_SIZE, type);

            DM_ERROR("ERROR!! dispose repeat node\n");
          }

          p_node[header.id].node_iw_addr = sec_iw_addr + offset;
          p_node[header.id].active = 1;
          p_sec->valid_len += step; //valid node
          DM_DETAIL("restore good node: id[%d], block[0x%x], addr[0x%x]\n",
            header.id, block_id, sec_iw_addr + offset);
        }
        offset += step;
      }
      p_sec->cur_sec_addr = offset;
    }
  }
  else
  {
    for(i = 0; i < total_sec; i++)
    {
      p_sec = p_sec_mgr->p_sec_arry + i;
      MT_ASSERT(p_sec != NULL);
      sec_st = p_sec->header.status;

      if((SEC_STREAM_IN == sec_st) || (SEC_USED == sec_st)
        || (SEC_STREAM_OUT == sec_st))
      {
        memcpy(p_sort_normal + normal_depth, p_sec, SEC_HEAD_SIZE);
        normal_depth++;
      }
      else if(SEC_WRITE == sec_st)
      {
        memcpy(p_sort_write + write_depth, p_sec, SEC_HEAD_SIZE);
        write_depth++;
      }
      else
      {
        //skip it
      }
    }

    sec_sort(p_sort_normal, normal_depth);
    //cat list. the write sec must be end
    memcpy(p_sort_normal + normal_depth, p_sort_write,
      SEC_HEAD_SIZE * write_depth);
    normal_depth += write_depth;
    //sort end
    
    for(i = 0; i < normal_depth; i++)
    {
      _cur = p_sort_normal[i].index;
      sec_iw_addr = _cur * FLASH_SECTION_SIZE;
      sec_flash_addr = p_sec_mgr->sec_base_addr + sec_iw_addr;
      offset = SEC_HEAD_SIZE; //section offset. skip header
      p_sec = p_sec_mgr->p_sec_arry + _cur;
      block_id = p_sec->header.block_id;
      block_index = find_block_index(p_data, block_id);
      MT_ASSERT(block_index != ERR_FAILURE);
      
      sec_st = p_sec->header.status;
      intact_flag = 0;

      DM_PRINT("sec_restore: sce[%d], block_id[0x%x], status[0x%x]\n",
        _cur, block_id, p_sec->header.status);

      p_node = p_blk_mgr[block_index].p_node_logic_info;

      //this data is valid
      if((SEC_STREAM_IN == sec_st) || (SEC_WRITE == sec_st)
        || (SEC_USED == sec_st) || (SEC_STREAM_OUT == sec_st))
      {
        p_sec->valid_len = 0;
        while(offset < FLASH_SECTION_SIZE)
        {
          //read a node header
          data_read(p_data, sec_flash_addr + offset,
            (u8 *)(&header), NODE_HEADER_SIZE, type);

          if(ERASE_DATA == header.value)  //find invalid data, means read finish
          {
            DM_PRINT("restore block[0x%x] section[%d], load[0x%x], end 0x%x\n",
              block_id, _cur, p_sec->valid_len, offset);
            p_sec->cur_sec_addr = offset;
            break;
          }
          step = (header.length * DM_ATOM_SIZE);

          //read a node header
          data_read(p_data, sec_flash_addr + offset + step - 1,
            &intact_flag, 1, type);

          //unexpected data, skip it
          if(header.id >= p_blk_mgr[block_index].node_num)
          {
            DM_ERROR("ERROR !!! bigger id: header_id[%d], block[0x%x],"
              "node_num[%d], addr[0x%x]\n",
              header.id, block_id, p_blk_mgr[block_index].node_num,
              sec_flash_addr + offset);
          }
          //the size is invalid, skip it,
          //fix me ,it's trouble. the step is error, can jump to the next??
          else if(step > p_data->dm_info.max_node_length)
          {
            DM_ERROR("ERROR !!! bigger size: header_id[%d], block[0x%x],"
              "size[%d], addr[0x%x]\n",
              header.id, block_id, step, sec_flash_addr + offset);
          }
          else if(0 == step)
          {
            DM_ERROR("ERROR !!! ZERO size: header_id[%d], block[0x%x],"
              "addr[0x%x]\n",
              header.id, block_id, sec_flash_addr + offset);
            break;  //fix me. how to found the next right node????
          }
          //the node is deleted, skip it
          else if(0 == header.active)
          {
            #if 0 //for debug
            if(type == BLOCK_TYPE_PIECE)
            {
              u8 data[32] = {0};
              data_read(p_data, sec_flash_addr + offset + NODE_HEADER_SIZE,
                data, 32, type);

              data[31] = 0;
              MT_ASSERT(data[0] == 0xFF);
            }
            #endif
            DM_DETAIL("find old node: id[%d], block[0x%x], addr[0x%x],"
              " active[%d]\n", header.id, block_id, sec_iw_addr + offset,
              p_node[header.id].active);
          }
          //the node is unintact
          else if(NODE_INTACT_FLAG != intact_flag)
          {
            DM_ERROR("ERROR found a unintact[impaired] node!!! header_id[%d], "
              "flag[0x%x], block[0x%x], node_num[%d], addr[0x%x]\n",
              header.id, intact_flag, block_id, p_blk_mgr[block_index].node_num,
              sec_flash_addr + offset + step - 1);

            //delete current bad node.
            header.active = 0;
            sec_del_node(p_data, _cur, offset, (u8 *)&header, NODE_HEADER_SIZE,
              header.length * DM_ATOM_SIZE, type);

            DM_ERROR("ERROR!!! dispose impaired node\n ");

            //use the old node, active it
            //don't need do anything !!!!!
            //because delete operation is after write (see write_node_to_flash)
            //so if the write operation was break, the last node isn't deleted

            //if(p_node[header.id].node_iw_addr != 0)
            //{
            //  p_node[header.id].active = 1;
            //}
            //p_sec->valid_len += step; //valid node
          }
          //good node
          else
          {
            if(1 == p_node[header.id].active) //the active node was found
            {
              DM_ERROR("ERROR!! found repeat node: id[%d], block[0x%x], "
                "old addr[0x%x], now [0x%x]\n",
                header.id, block_id, p_node[header.id].node_iw_addr,
                sec_iw_addr + offset);

              sec_index = p_node[header.id].node_iw_addr / FLASH_SECTION_SIZE;
              sec_offset = p_node[header.id].node_iw_addr % FLASH_SECTION_SIZE;

              //delete old node.
              header.active = 0;
              sec_del_node(p_data, sec_index, sec_offset, (u8 *)&header, NODE_HEADER_SIZE,
                header.length * DM_ATOM_SIZE, type);

              DM_ERROR("ERROR!! dispose repeat node\n");
            }

            p_node[header.id].node_iw_addr = sec_iw_addr + offset;
            p_node[header.id].active = 1;
            p_sec->valid_len += step; //valid node
            DM_DETAIL("restore good node: id[%d], block[0x%x], addr[0x%x]\n",
              header.id, block_id, sec_iw_addr + offset);
          }
          offset += step;
        }
        p_sec->cur_sec_addr = offset;
      }
    }

    //free buffer
    mtos_free(p_sort_normal);
    mtos_free(p_sort_write);

    section_show(p_data, "sec restore done");
  }
  return TRUE;
}

/*!
  reconstruct IW block node info
  \return FALSE if fail, or TRUE if success
  */
static BOOL sec_clear_redun(dm_var_t *p_data)
{
  block_type_t type = BLOCK_TYPE_PIECE;
  dm_global_info_t *p_dm_info = &p_data->dm_info;
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  block_mgr_t *p_blk_mgr = p_dm_info->p_blk_mgr;
  section_info_t *p_sec = NULL; //sec info
  node_header_t header = {{0}};
  u32 total_sec = p_sec_mgr->total_section_num;
  u32 step = 0; //node step
  u32 i = 0;
  u32 sec_iw_addr = 0; //section addr base for ie base
  u32 sec_flash_addr = 0; //section addr base for flash zero addr
  u32 offset = 0; //section offset base current sec.
  s32 block_index = 0;  //iw block index
  sec_status_t sec_st = SEC_CLEAN;
  u8 intact_flag = 0;
  u8 block_id = 0;

  for(i = 0; i < total_sec; i++)
  {
    sec_iw_addr = i * FLASH_SECTION_SIZE;
    sec_flash_addr = p_sec_mgr->sec_base_addr + sec_iw_addr;
    offset = SEC_HEAD_SIZE; //section offset. skip header
    p_sec = p_sec_mgr->p_sec_arry + i;
    block_id = p_sec->header.block_id;
    block_index = find_block_index(p_data, block_id);
    MT_ASSERT(block_index != ERR_FAILURE);

    sec_st = p_sec->header.status;
    intact_flag = 0;

    //this data is valid
    if(SEC_STREAM_OVERDUE == sec_st)
    {
      sec_cleanup_overdue(p_data, i, type);
    }
    else if((SEC_STREAM_IN == sec_st) || (SEC_WRITE == sec_st)
      || (SEC_USED == sec_st) || (SEC_STREAM_OUT == sec_st))
    {
      while(offset < FLASH_SECTION_SIZE)
      {
        //read a node header
        data_read(p_data, sec_flash_addr + offset,
          (u8 *)(&header), NODE_HEADER_SIZE, type);

        if(ERASE_DATA == header.value)  //find invalid data, means read finish
        {
          break;
        }
        step = (header.length * DM_ATOM_SIZE);

        //read a node header
        data_read(p_data, sec_flash_addr + offset + step - 1,
          &intact_flag, 1, type);

        //unexpected data, skip it
        if(header.id >= p_blk_mgr[block_index].node_num)
        {
        }
        //the size is invalid, skip it,
        //fix me ,it's trouble. the step is error, can jump to the next??
        else if(step > p_data->dm_info.max_node_length)
        {
        }
        else if(0 == step)
        {
          break;  //fix me. how to found the next right node????
        }
        //the node is deleted, reset it
        else if(0 == header.active)
        {
          data_cleanup(p_data, sec_flash_addr + offset + NODE_HEADER_SIZE,
              step - 1 - NODE_HEADER_SIZE, type);
        }
        //the node is unintact
        else if(NODE_INTACT_FLAG != intact_flag)
        {
        }
        //good node
        else
        {
        }
        offset += step;
      }
    }
  }

  section_show(p_data, "sec_clear_redun");
  return TRUE;
}


/********************************************************
                    block functions
 ********************************************************/

/*!
  find block index
  \return ERR_FAILURE if fail, or index if success
  */
static s32 find_block_index(dm_var_t *p_data, u8 block_id)
{
  s32 i = 0;
  dm_global_info_t *p_dm_info = &p_data->dm_info;

  //we are really fast!!!
  if(block_id == p_dm_info->p_blk_mgr[p_dm_info->prev_blk_id].id)
  {
    return p_dm_info->prev_blk_id;
  }

  for(i = 0; i < p_dm_info->block_num; i++)
  {
    if(block_id == p_dm_info->p_blk_mgr[i].id)
    {
      p_dm_info->prev_blk_id = i;
      return i;
    }
  }

  DM_ERROR("%s. can't find block id[0x%x]\n", __FUNCTION__, block_id);
  return ERR_FAILURE;
}

/*!
  find first unused id
  \return None
  */
static void find_free_id(dm_var_t *p_data, u8 index)
{
  dm_global_info_t *p_dm_info = &p_data->dm_info;
  block_mgr_t *p_block_info = p_dm_info->p_blk_mgr + index;

  while(p_block_info->free_id < p_block_info->node_num)
  {
    if(0 == p_block_info->p_node_logic_info[p_block_info->free_id].active)
    {
      break;
    }
    p_block_info->free_id ++;
  }
}

/*!
  Write a del operation to flash.
  This operation can't change the data len in flash
  \return ERR_FAILURE if fail, or index if success
  */
static void write_del_node(dm_var_t *p_data, node_logic_info_t *p_node,
              u16 node_id, block_type_t type)
{
  section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, type);
  node_header_t header = {{0}};
  u32 flash_addr = 0;
  u32 iw_addr = 0;
  u32 sec_indx = 0;
  u32 sec_offset = 0;

  if(0 == p_node->node_iw_addr)
  {
    DM_PRINT("Task reenter same node. do nothing.\n");
    return;
  }

  flash_addr = p_sec_mgr->sec_base_addr + p_node->node_iw_addr;
  iw_addr = p_node->node_iw_addr;
  sec_indx = iw_addr / FLASH_SECTION_SIZE;
  sec_offset = iw_addr % FLASH_SECTION_SIZE;

  //del the link
  p_node->node_iw_addr = 0;

  data_read(p_data,
    flash_addr, (u8 *)&header, NODE_HEADER_SIZE, type);
  if(header.id != node_id)
  {
    DM_ERROR("error addr[0x%x], node_id[%d], len[%d], act[%d], input_id[%d]\n",
      iw_addr, header.id, header.length * DM_ATOM_SIZE,
      header.active, node_id);
  }
  MT_ASSERT(header.id == node_id);

  if(0 == header.active)  //it's deleted, link to old data
  {
    DM_ERROR("This node is deleted: node_id %d, addr 0x%x\n",
      node_id, flash_addr);
  }
  else
  {
    header.active = 0;  //delete it

    sec_del_node(p_data, sec_indx, sec_offset, (u8 *)&header,
      NODE_HEADER_SIZE, header.length * DM_ATOM_SIZE, type);
  }
}

static dm_ret_t write_node_to_flash(dm_var_t *p_data, u8 block_id, u16 node_id,
                  u8 *p_buffer, u16 len)
{
  s32                block_index  = ERR_FAILURE;
  block_mgr_t       *p_block      = NULL;
  block_type_t       type         = 0;
  section_mgr_t     *p_sec_mgr    = NULL;
  section_info_t    *p_sec        = NULL;
  node_logic_info_t *p_node_logic = NULL;
  u32 sec_addr = 0; //base of cur section
  u32 iw_addr = 0;  //base of iw block base
  u16 write_len = ALIGN16(len + NODE_HEADER_SIZE + NODE_FLAG_SIZE);
  u32 new_section_idx = 0;
  BOOL found_next = FALSE;
  u32 enter_ticks = mtos_ticks_get();
  u32 exit_ticks = 0;
  u8 *p_node_buf = NULL;
  node_header_t *p_header = NULL;

  block_index = find_block_index(p_data, block_id);
  MT_ASSERT(block_index != ERR_FAILURE);

  p_block = p_data->dm_info.p_blk_mgr + block_index;
  
  type = p_block->type;
  p_sec_mgr = get_sec_mgr(p_data, type);
  p_sec = p_sec_mgr->p_sec_arry;
  
  MT_ASSERT(NULL != p_buffer);
  MT_ASSERT(len > 0);
  MT_ASSERT((BLOCK_TYPE_IW == type) || (BLOCK_TYPE_PIECE == type)
          || (BLOCK_TYPE_FIXED_ADDR == type));
  MT_ASSERT(write_len <= p_data->dm_info.max_node_length);

  if(BLOCK_TYPE_FIXED_ADDR == type)
  {
    if(p_sec[0].cur_sec_addr + write_len > FLASH_SECTION_SIZE) 
    {
      OS_PRINTF("fixed addr p_sec[0].cur_sec_addr %d, write_len %d, block_id %d\n", 
        p_sec[0].cur_sec_addr, write_len, block_id);
      sec_to_sdram(p_data, 0, type);
    }    
    p_block->sec_index = 0;
  }
  else
  {
    //invalid sec. must be first enter
    if(p_block->sec_index > p_sec_mgr->total_section_num)
    {
      found_next = sec_find_new(p_data, &new_section_idx,
              block_id, SEC_WRITE);
      if(!found_next)
      {
        DM_ERROR("Can't find the writing block, block_id 0x%x\n", block_id);
        //find the stream in sec
        found_next = sec_find_new(p_data, &new_section_idx,
                    block_id, SEC_STREAM_IN);
        if(!found_next)
        {
          DM_ERROR("a. Can't find the s_in block, block_id 0x%x\n", block_id);
          found_next = sec_abnormity_process(p_data,
          &new_section_idx, block_id);
          if(!found_next)
          {
            return DM_FULL;
          }
        }
      }
      p_block->sec_index = new_section_idx;
      sec_set_status(p_data, new_section_idx, SEC_WRITE, block_id);
    }

    while(TRUE)
    {
      sec_addr = p_sec[p_block->sec_index].cur_sec_addr;
      if(sec_addr + write_len > FLASH_SECTION_SIZE) //the current sec is full
      {
        //set the old section
        sec_set_status(p_data, p_block->sec_index, SEC_USED, block_id);

        //find the next sec
        found_next = sec_find_new(p_data, &new_section_idx,
                    block_id, SEC_STREAM_IN);

        if(!found_next)
        {
          DM_ERROR("b. Can't find the s_in block, block_id 0x%x\n", block_id);
          found_next = sec_abnormity_process(p_data,
                          &new_section_idx, block_id);
          if(!found_next)
          {
            DM_PRINT("DM_FULL write node 2.\n");
            return DM_FULL;
          }
        }

        sec_set_status(p_data, new_section_idx, SEC_WRITE, block_id);
        p_block->sec_index = new_section_idx;
      }
      else
      {
        break;  //found right section
      }
    }
  }
  
  p_node_buf = dm_alloc_node_buffer(p_data, "write_node_to_flash");
  MT_ASSERT(p_node_buf != NULL);

  p_header = (node_header_t *)p_node_buf;
  p_header->id = node_id;
  p_header->length = write_len / DM_ATOM_SIZE;

  memcpy(p_node_buf + NODE_HEADER_SIZE, p_buffer, len);
  p_node_buf[write_len - 1] = NODE_INTACT_FLAG;
  p_node_logic = p_block->p_node_logic_info + p_header->id;
  
  //sync cache
  if(p_block->cache_size > 0)
  {
    u16 cpy_size = (len > p_block->cache_size) ? p_block->cache_size : len;
    memcpy(p_block->p_cache + p_block->cache_size * p_header->id,
      p_buffer, cpy_size);
    p_node_logic->cache_state = CACHE_SYNC;
    p_node_logic->unsync_cache_size = 0;   //no cache need flush
  }

  //write data
  p_header->active = 1;
  iw_addr = sec_write_node(p_data, p_block->sec_index, p_node_buf,
    write_len, type);

  dm_free_node_buffer(p_data, p_node_buf, "write_node_to_flash");
  
  //del the old data
  //why to check the addr but active flag?? because the "active"
  // flag the logic status, so we must del the last node.
  // But something need care, the last node maybe deactived in the section.
  // So we maybe delete a deactived node, it's not a trouble.
  if(p_node_logic->node_iw_addr != 0)
  {
    write_del_node(p_data, p_node_logic, node_id, type);
  }

  //update logic info
  p_node_logic->active = 1;
  p_node_logic->node_iw_addr = iw_addr;
  exit_ticks = mtos_ticks_get();

  if(exit_ticks - enter_ticks > 100)
  {
    DM_ERROR("write node spend long time %d ticks\n",
      exit_ticks - enter_ticks);
  }
  return DM_SUC;
}

static void task_process(void *p_data)
{
  //dm_var_t *p_priv = (dm_var_t *)p_data;
  //u8 is_protected = 0;

  while(1)
  {
    sec_check_clean((dm_var_t *)p_data);

    mtos_task_sleep(100);
  }
}

/********************************************************
                    Internal functions
 ********************************************************/

static void dm_restore_v3(void *p_data)
{
}

static BOOL dm_load_piece_data(dm_var_t *p_data, u32 base_addr,
                          u16 sec_num)
{
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;
  u8 *p_zip_buf = NULL;
  u32 zip_data_size = 0, inflate_len = 0;
  int ret = 0;
  compress_block_head_t comp_head = {0};

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  MT_ASSERT(p_dm_info->p_ghost != NULL);

  //to default
  memset(p_dm_info->p_ghost, 0xFF, p_dm_info->ghost_size);

  //check is new flash

  //load data to temp buffer
  //zip_data_size = sec_num * FLASH_SECTION_SIZE;
  //fix me
  p_zip_buf = p_dm_info->p_decmp_buf;
  MT_ASSERT(p_zip_buf != NULL);
 // MT_ASSERT(p_dm_info->decmp_buf_size >= zip_data_size);
  data_read(p_data, base_addr, (u8 *) &comp_head,
                  sizeof(compress_block_head_t), BLOCK_TYPE_RO);
  zip_data_size = comp_head.zip_data_size;
  //zip_data_size = *((u32 *)p_zip_buf);

  //unzip
  inflate_len = p_dm_info->ghost_size;
  if(zip_data_size > 0 && zip_data_size != ERASE_DATA)
  {
    data_read(p_data, base_addr + sizeof(compress_block_head_t),
                    p_zip_buf, zip_data_size, BLOCK_TYPE_RO);
    ret = gzip_uncompress(p_dm_info->p_ghost, &inflate_len,
      (u8 *)(p_zip_buf), zip_data_size,
      p_dm_info->p_decmp_cache, p_dm_info->decmp_cache_size);
      p_dm_info->sec_mgr_ghost.sec_data_changed = FALSE;
    if(ret !=0)
    {
      return FALSE;
    }
  }
  return TRUE;
}

static BOOL dm_store_piece_data_v3(void *p_data, u8 *p_zip, u32 zip_size,
          u8 *p_cache,u32 cache_size)
{
    dm_var_t *p_dm_var = NULL;
    dm_global_info_t *p_dm_info = NULL;
    u32 dest_size = 0;
    int ret = 0;
    compress_block_head_t comp_head = {0};

    p_dm_var = (dm_var_t *)p_data;
    p_dm_info = &p_dm_var->dm_info;

    //cleanup
    sec_clear_redun(p_dm_var);

    dest_size = p_dm_info->ghost_size;
    ret = gzip_compress(p_zip, (u32 *)(&dest_size), p_dm_info->p_ghost,
        p_dm_info->ghost_size,p_cache,cache_size);

    if(ret != 0)
    {
      MT_ASSERT(0);
      return FALSE;
    }

    comp_head.zip_data_size = dest_size;
    comp_head.match_flag = ERASE_DATA;

    //*((u32*)p_zip) = dest_size;// 4 byte to store the data size
    if(dest_size <= (p_dm_info->iw_base_addr - p_dm_info->piece_base_addr))
    {      
      dm_lock_v3(p_dm_var);
      charsto_erase(p_dm_var->p_charsto_dev,
                p_dm_info->piece_base_addr,
                p_dm_info->sec_mgr_ghost.total_compress_section_num);
      charsto_writeonly(p_dm_var->p_charsto_dev, p_dm_info->piece_base_addr,
        (u8 *)&comp_head, sizeof(compress_block_head_t));
      charsto_writeonly(p_dm_var->p_charsto_dev,
        p_dm_info->piece_base_addr + sizeof(compress_block_head_t),
                p_zip, dest_size);
      p_dm_var->dm_info.sec_mgr_ghost.sec_data_changed = FALSE;
      dm_unlock_v3(p_dm_var);
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

static BOOL dm_check_compress_match_flag_v3(void *p_data)
{
  dm_var_t *p_dm_var = (dm_var_t *)p_data;
  dm_global_info_t *p_dm_info = &p_dm_var->dm_info;
  compress_block_head_t comp_head = {0};

  data_read(p_data, p_dm_var->dm_info.piece_base_addr, (u8 *)&comp_head,
                  sizeof(compress_block_head_t), BLOCK_TYPE_RO);

  if(ERASE_DATA == comp_head.match_flag)
  {
    return TRUE;
  }
  else
  {    
    dm_lock_v3(p_dm_var);
    charsto_erase(p_dm_var->p_charsto_dev,
      p_dm_info->piece_base_addr,
      p_dm_info->sec_mgr_ghost.total_compress_section_num);

    charsto_erase(p_dm_var->p_charsto_dev,
      p_dm_info->iw_base_addr, p_dm_info->sec_mgr.total_section_num);

    dm_unlock_v3(p_dm_var);

#ifndef WIN32
    hal_pm_reset();
#else
    MT_ASSERT(0);
#endif
    return FALSE;
  }
}

static void dm_set_compress_unmatch_flag_v3(void *p_data)
{
  dm_var_t *p_dm_var = (dm_var_t *)p_data;
  compress_block_head_t comp_head = {0};

  if(!p_dm_var->dm_info.sec_mgr_ghost.sec_data_changed)
  {
    return;
  }

  data_read(p_data, p_dm_var->dm_info.piece_base_addr, (u8 *)&comp_head,
                  sizeof(compress_block_head_t), BLOCK_TYPE_RO);
  dm_lock_v3(p_dm_var);

  if(ERASE_DATA == comp_head.match_flag)
  {
    comp_head.match_flag = 0x475A475A;
    charsto_writeonly(p_dm_var->p_charsto_dev, p_dm_var->dm_info.piece_base_addr,
      (u8 *)&comp_head, sizeof(compress_block_head_t));
  }
  
  dm_unlock_v3(p_dm_var);
}

static void dm_set_decompress_buf_v3(void *p_data, u8 *p_buf,
                          u32 buf_size,u8 *p_cache, u32 cache_size)
{
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  p_dm_info->p_decmp_buf = p_buf;
  p_dm_info->decmp_buf_size = buf_size;
  p_dm_info->p_decmp_cache = p_cache;
  p_dm_info->decmp_cache_size = cache_size;
}

/*!
  read data from flash block to ram buffer

  \param[in] block_id Block ID
  \param[in] node_id node id for IW block , meaningless for RO block
  \param[in] offset Offset in block for RO block , offset in node for IW block
  \param[in] length read length
  \param[in] pbuffer Pointer to store data
  \return 0 if fail, actual length if success
  */
static u32 dm_read_v3(void *p_data, u8 block_id, u16 node_id, u16 offset,
  u32 length, u8 *p_buffer)
{
  u32 read_len = 0;
  u32 node_addr = 0;
  s32 b_index = 0;
  u8 *p_node_buf = NULL;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  u8 block_type = BLOCK_TYPE_UNKNOWN;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  MT_ASSERT(p_buffer != NULL);
  MT_ASSERT(length > 0);

  memset(p_buffer, 0x0, length);

  b_index = find_block_index(p_dm_var, block_id);

  if(ERR_FAILURE == b_index)
  {
    return 0;
  }

  dm_lock_v3(p_data);
  
  block_type = p_dm_info->p_blk_mgr[b_index].type;

  if((BLOCK_TYPE_RO == block_type) || (BLOCK_TYPE_IDENTITY == block_type)) // read only block
  {
    node_addr = p_dm_info->p_blk_mgr[b_index].base_addr + offset;
    if(SUCCESS == data_read(p_dm_var, node_addr, p_buffer, length, BLOCK_TYPE_RO))
    {
      read_len = length;
    }
  }
  else if((BLOCK_TYPE_IW == block_type) || (BLOCK_TYPE_PIECE == block_type) 
    || (BLOCK_TYPE_FIXED_ADDR == block_type))
  {
    block_mgr_t *p_block = p_dm_info->p_blk_mgr + b_index;
    node_logic_info_t *p_node = p_block->p_node_logic_info + node_id;
    section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, block_type);

    if((node_id >= p_block->node_num) || (0 == p_node->active))
    {
      dm_unlock_v3(p_data);
      return 0; //invalid node id
    }
    MT_ASSERT((offset + length) <= p_dm_info->max_node_length);
    read_len = offset + length;
    //calculate read addr,ignore head and locate to offset start
    node_addr = p_sec_mgr->sec_base_addr
        + p_node->node_iw_addr + NODE_HEADER_SIZE;

    if((p_block->cache_size > 0) && (CACHE_EMPTY == p_node->cache_state))
    {
      //read the cache
      if(SUCCESS == data_read(p_dm_var, node_addr,
        p_block->p_cache + p_block->cache_size * node_id,
        p_block->cache_size, block_type))
      {
        p_node->cache_state = CACHE_SYNC;
        p_node->unsync_cache_size = 0;
      }
      else
      {
        MT_ASSERT(0);
      }
    }

    if(read_len <= p_block->cache_size)  //all the data be in cache
    {
      memcpy(p_buffer,
        p_block->p_cache + p_block->cache_size * node_id + offset, length);
      read_len = length;
    }
    else
    {
      p_node_buf = dm_alloc_node_buffer(p_data, "dm_read_v3");
      MT_ASSERT(p_node_buf != NULL);

      if(p_block->cache_size >0)
      {
        memcpy(p_node_buf, p_block->p_cache + p_block->cache_size * node_id,
          p_block->cache_size);
        read_len -= p_block->cache_size;
        node_addr += p_block->cache_size;
      }

      if(SUCCESS == data_read(p_dm_var, node_addr,
        p_node_buf + p_block->cache_size, read_len, block_type))
      {
        memcpy(p_buffer, p_node_buf + offset, length);
        read_len = length;
      }
      else
      {
        OS_PRINTF("error read!! block_id 0x%x, node_id %d, length %d,"
          " sec_base_addr 0x%x, node_iw_addr 0x%x \n",
          block_id, node_id, length, p_sec_mgr->sec_base_addr, p_node->node_iw_addr);
        MT_ASSERT(0);
      }

      dm_free_node_buffer(p_data, p_node_buf, "dm_read_v3");
    }
  }
  else
  {
    DM_ERROR("block_id %d, block_type %d\n", block_id, block_type);
    MT_ASSERT(0);
  }

  dm_unlock_v3(p_data);
  return read_len;
}
/*!
  write data to a node

  \param[in] block_id Block ID
  \param[in,out] p_node_id UNKNOWN_ID to write a new node, else modify a node
  \param[in] length Data length to write
  \param[in] pbuffer Data pointer
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_write_node_v3(void *p_data, u8 block_id,
  u16 *p_node_id, u8 *p_buffer, u16 len)
{
  block_mgr_t *p_block = NULL;
  s32 index = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  u8 block_type = 0;
  
  MT_ASSERT(p_data != NULL);
  MT_ASSERT(p_buffer != NULL);
  MT_ASSERT(len > 0);

  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  p_block = p_dm_info->p_blk_mgr + index;
  block_type = p_block->type;

  if((ERR_FAILURE == index) || (p_block->type == BLOCK_TYPE_RO))
  {
    return DM_FAIL;
  }

  if(UNKNOWN_ID == *p_node_id)  // add new node
  {
    find_free_id(p_dm_var, index);
    if(p_block->free_id >= p_block->node_num)
    {
      DM_PRINT("%s:dm if full!\n", __FUNCTION__);
      DM_PRINT("DM_FULL write node 3.\n");
      return DM_FULL;
    }
    *p_node_id = p_block->free_id;
  }
  else  //edit a node
  {
    if(*p_node_id >= p_block->node_num)
    {
      DM_PRINT("%s: invalid node id[0x%x]!\n", __FUNCTION__, *p_node_id);
      return DM_FAIL;
    }
  }

  dm_lock_v3(p_dm_var);

  if((BLOCK_TYPE_IW == block_type) || (block_type == BLOCK_TYPE_PIECE))
  { 
    if(len <= p_block->cache_size) //write cache
    {
      memcpy(p_block->p_cache + p_block->cache_size * (*p_node_id),
          p_buffer, len);
      p_block->p_node_logic_info[*p_node_id].cache_state = CACHE_ASYNC;

      if(p_block->p_node_logic_info[*p_node_id].unsync_cache_size < len)  //record the max
      {
        p_block->p_node_logic_info[*p_node_id].unsync_cache_size = len;
      }
    }
    else  //write flash
    {
      write_node_to_flash(p_data, block_id, *p_node_id, p_buffer, len);
    }
  }
  else if(BLOCK_TYPE_FIXED_ADDR == block_type)
  {
    write_node_to_flash(p_data, block_id, *p_node_id, p_buffer, len);
  }
  else
  {
    MT_ASSERT(0);
  }
  
  dm_unlock_v3(p_dm_var);

  return DM_SUC;
}

/*!
  Delete a node, only for IW block

  \param[in] block_id Block ID
  \param[in] node_id Node ID
  \param[in] b_sync TRUE to write flash at once
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_del_node_v3(void *p_data, u8 block_id, u16 node_id,
    BOOL b_sync)
{
  u32 index = 0;
  block_mgr_t *p_block = NULL;
  node_logic_info_t *p_node = NULL;
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  p_block = p_dm_info->p_blk_mgr + index;
  p_node = p_block->p_node_logic_info + node_id;

  if((ERR_FAILURE == index)
    || (node_id >= p_block->node_num)
    || (0 == p_node->active))
  {
    return DM_FAIL;
  }

  p_node->active = 0;
  //reserve calc free space
  if(b_sync)
  {
    dm_lock_v3(p_dm_var);
    write_del_node(p_dm_var, p_node, node_id, p_block->type);
    dm_unlock_v3(p_dm_var);
  }

  if(node_id < p_block->free_id)
  {
    p_block->free_id = node_id;
  }

  return DM_SUC;
}

/*!
  Active a node, only for IW block

  \param[in] block_id Block ID
  \param[in] node_id Node ID
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_active_node_v3(void *p_data, u8 block_id, u16 node_id)
{
  u32 index = 0;
  block_mgr_t *p_block = NULL;
  node_logic_info_t *p_node = NULL;
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *) p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  p_block = p_dm_info->p_blk_mgr + index;
  p_node = p_block->p_node_logic_info + node_id;

  if((ERR_FAILURE == index)
    || (node_id >= p_block->node_num)
    || (1 == p_node->active)
    || (0 == p_node->node_iw_addr))
  {
    return DM_FAIL;
  }
  p_node->active = 1;
  return DM_SUC;
}

/*!
  Check block status, clean up blocks if needed

  \return None
  */
static void dm_check_block_v3(void *p_data)
{
  dm_var_t *p_para = (dm_var_t *)p_data;

  if(!p_para->is_open_monitor)
  {
    sec_check_clean(p_para);
  }
}

/*!
  Declare all IW node ID are invalid

  \param[in] block_id Block ID
  \return None
  */
static dm_ret_t dm_reset_iw_block_v3(void *p_data, u8 block_id, BOOL is_sync)
{
  u32 index = 0;
  u32 i = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  block_mgr_t *p_block = NULL;
  node_logic_info_t *p_node = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  if(ERR_FAILURE == index)
  {
    return DM_FAIL;
  }

  p_block = p_dm_info->p_blk_mgr + index;

  dm_lock_v3(p_dm_var);

  for(i = 0; i < p_dm_info->p_blk_mgr[index].node_num; i ++)
  {
    p_node = p_dm_info->p_blk_mgr[index].p_node_logic_info + i;

    p_node->active = 0;

    if(is_sync)
    {
      write_del_node(p_data, p_node, i, p_block->type);
    }
  }

  p_dm_info->p_blk_mgr[index].free_id = 0;
  dm_unlock_v3(p_dm_var);
  //p_dm_info->p_blk_mgr[index].free_id = p_dm_info->p_blk_mgr[index].fix_num;
  return DM_SUC;
}

static dm_ret_t dm_restore_cache_v3(void *p_data, u8 block_id, u16 node_id)
{
  s32 block_indx = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  block_indx = find_block_index(p_dm_var, block_id);
  p_dm_info->p_blk_mgr[block_indx].p_node_logic_info[node_id].cache_state = CACHE_EMPTY;
  
  return DM_SUC;
}

static dm_ret_t dm_flush_cache_v3(void *p_data, u8 block_id, u16 node_id)
{
  s32 block_indx = 0;
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;
  block_mgr_t *p_block = NULL;
  node_logic_info_t *p_node = NULL;
  u8 *p_node_buf = NULL;
  u8 *p_cache_buf = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  block_indx = find_block_index(p_dm_var, block_id);
  if(block_indx == ERR_FAILURE)
  {
    return DM_FAIL;
  }
  
  p_block = p_dm_info->p_blk_mgr + block_indx;

  if((p_block->type != BLOCK_TYPE_IW) &&
      (p_block->type != BLOCK_TYPE_PIECE))
  {
    return DM_FAIL;
  }

  dm_lock_v3(p_dm_var);

  p_node = p_block->p_node_logic_info + node_id;
  p_cache_buf = p_block->p_cache + p_block->cache_size * node_id;

  //the current node isn't exsit, add it
  if((0 == p_node->active) && (CACHE_ASYNC == p_node->cache_state))
  {
    write_node_to_flash(p_data, block_id, node_id, p_cache_buf,
      p_node->unsync_cache_size);
  }
  else
  {
    if(CACHE_ASYNC == p_node->cache_state)
    {
      section_mgr_t *p_sec_mgr = get_sec_mgr(p_data, p_block->type);
      u32 node_addr = p_sec_mgr->sec_base_addr + p_node->node_iw_addr;
      s32 cmp_rslt = 0;

      p_node_buf = dm_alloc_node_buffer(p_dm_var, "dm_flush_cache_v3");
      MT_ASSERT(p_node_buf != NULL);
      MT_ASSERT(p_node->unsync_cache_size > 0);
      MT_ASSERT(p_node->unsync_cache_size <= p_block->cache_size);

      data_read(p_dm_var, node_addr,p_node_buf, p_dm_info->max_node_length, p_block->type);

      cmp_rslt = memcmp(p_node_buf + NODE_HEADER_SIZE, p_cache_buf, p_node->unsync_cache_size);
      if(cmp_rslt != 0)
      {
        node_header_t *p_head = (node_header_t *)p_node_buf;
        memcpy(p_node_buf + NODE_HEADER_SIZE, p_cache_buf,
          p_node->unsync_cache_size);
        write_node_to_flash(p_data, block_id, node_id,
          p_node_buf + NODE_HEADER_SIZE,
          p_head->length * DM_ATOM_SIZE - NODE_HEADER_SIZE - NODE_FLAG_SIZE);
      }
      else
      {
        p_node->cache_state = CACHE_SYNC;
        p_node->unsync_cache_size = 0;
      }
      dm_free_node_buffer(p_data, p_node_buf, "dm_flush_cache_v3");
    }
  }

  dm_unlock_v3(p_dm_var); 

  //if the cache more than the data????
  return DM_SUC;
}

static dm_ret_t dm_set_cache_v3(void *p_data, u8 block_id, u16 cache_size)
{
  s32 block_indx = 0;
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;
  block_mgr_t *p_block_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *) p_data;
  p_dm_info = &p_dm_var->dm_info;

  block_indx = find_block_index(p_dm_var, block_id);
  if(block_indx == ERR_FAILURE)
  {
    return DM_FAIL;
  }

  p_block_info = p_dm_info->p_blk_mgr + block_indx;

  //Cache protection machanism
  //Avoid allocate cache for one table block for more times
  if(p_block_info->type == BLOCK_TYPE_RO || cache_size == 0)
  {
    return DM_FAIL;
  }

  p_block_info->p_cache = mtos_malloc(p_block_info->node_num * cache_size);

  MT_ASSERT(NULL != p_block_info->p_cache);
  p_block_info->cache_size = cache_size;

  return DM_SUC;
}

static void dm_set_header_v3(void *p_data, u32 addr)
{
  dm_var_t *p_dm_var = NULL;
  dmh_block_info_t dmh = { 0 };
  dm_global_info_t temp_info = {0};
  dm_global_info_t  *p_dm_info = NULL;
  u32 start_addr = addr;
  u32 i = 0;
  u32 old_num = 0;
  u32 total_iw_sec_num = 0;
  u32 min_iw_addr = ~0;
  u32 total_piece_sec_num = 0;
  u32 min_piece_addr = ~0;
  u8 temp[100];

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &temp_info;

  dm_lock_v3(p_data);

  //read dmh basic info(12B): sdram&flash size, block num, each block head size
  data_read(p_dm_var, start_addr + DM_TAG_SIZE,
                        (u8 *)temp, DM_BASE_INFO_HEAD_LEN, BLOCK_TYPE_RO);

  p_dm_info->sdram_size = MT_MAKE_DWORD(MAKE_WORD(temp[0], temp[1]), \
                                MAKE_WORD(temp[2], temp[3]));

  p_dm_info->flash_size = MT_MAKE_DWORD(MAKE_WORD(temp[4], temp[5]), \
                                MAKE_WORD(temp[6], temp[7]));
  p_dm_info->block_num = MAKE_WORD(temp[8], temp[9]);
  p_dm_info->bh_size = MAKE_WORD(temp[10], temp[11]);

  ////check invalid data
  if(p_dm_var->dm_info.block_num + p_dm_info->block_num >
    p_dm_var->dm_info.max_blk_num)
  {
    DM_ERROR("ERROR!! %s:too much block num! addr :0x%x, total : %d blocks\n",
      __FUNCTION__, start_addr, p_dm_info->block_num);
    MT_ASSERT(0);
  }

  if(p_dm_info->bh_size != sizeof(dmh_block_info_t))
  {
    DM_ERROR("ERROR!!  %s: struct size dismatch!\n",__FUNCTION__);
    MT_ASSERT(0);
  }

  DM_PRINT("The DMH at 0x%x,blocks total %d\n",
    start_addr, p_dm_info->block_num);
  //start_addr += DM_BASE_INFO_HEAD_LEN;
  old_num = p_dm_var->dm_info.block_num;
  for(i = 0;  i < p_dm_info->block_num; i++)
  {
    u32 index = i + old_num;
    u32 header_addr = start_addr + DM_TAG_SIZE + DM_BASE_INFO_HEAD_LEN +
      i * sizeof(dmh_block_info_t);

    p_dm_var->dm_info.block_num++;
    data_read(p_dm_var, header_addr
      , (u8 *)temp, sizeof(dmh_block_info_t), BLOCK_TYPE_RO);

    dmh.id = temp[0];
    dmh.type = temp[1];
    dmh.node_num = MAKE_WORD(temp[2], temp[3]);
    dmh.base_addr = MT_MAKE_DWORD(MAKE_WORD(temp[4], temp[5]), \
                                MAKE_WORD(temp[6], temp[7]));
    dmh.size = MT_MAKE_DWORD(MAKE_WORD(temp[8], temp[9]), \
                                MAKE_WORD(temp[10], temp[11]));
    dmh.crc = MT_MAKE_DWORD(MAKE_WORD(temp[12], temp[13]), \
                                MAKE_WORD(temp[14], temp[15]));
    //memcpy(&dmh.version,&temp[16],32);
    memcpy(&dmh.version[0],&temp[16],8);
    memcpy(&dmh.name[0],&temp[24],8);
    memcpy(&dmh.time[0],&temp[32],12);    
    DM_PRINT("name %s version 0x%8x time 0x%8x,", dmh.name, dmh.version, dmh.time);

    DM_PRINT("block id=0x%x,type=0x%x,node=0x%x,addr=0x%x,size=0x%x,"
      "crc=0x%x\n", dmh.id, dmh.type, dmh.node_num,
      dmh.base_addr + start_addr, dmh.size, dmh.crc);

    p_dm_var->dm_info.p_blk_mgr[index].dm_header_addr = header_addr;
    p_dm_var->dm_info.p_blk_mgr[index].id = dmh.id;
    p_dm_var->dm_info.p_blk_mgr[index].type = dmh.type;
    p_dm_var->dm_info.p_blk_mgr[index].node_num = dmh.node_num;
    p_dm_var->dm_info.p_blk_mgr[index].base_addr = dmh.base_addr + start_addr;
    p_dm_var->dm_info.p_blk_mgr[index].size = dmh.size;

    if(BLOCK_TYPE_IW == dmh.type)//init iw info
    {
      block_mgr_t *p_iw_blk = p_dm_var->dm_info.p_blk_mgr + index;
      u8 sec_num = p_iw_blk->size / FLASH_SECTION_SIZE;

      MT_ASSERT(0 == (p_iw_blk->size % FLASH_SECTION_SIZE));
      total_iw_sec_num += sec_num;

      if(min_iw_addr > p_dm_var->dm_info.p_blk_mgr[index].base_addr)
      {
        min_iw_addr = p_dm_var->dm_info.p_blk_mgr[index].base_addr;
      }

      p_iw_blk->sec_num = sec_num;
      p_iw_blk->sec_index = 0xFFFF;
      p_iw_blk->p_node_logic_info = mtos_malloc(
        dmh.node_num * sizeof(node_logic_info_t));

      MT_ASSERT(NULL != p_iw_blk->p_node_logic_info);
      //set default first
      //set node to 0 means: the node is deactived, the cache is empty
      memset(p_iw_blk->p_node_logic_info, 0,
        dmh.node_num * sizeof(node_logic_info_t));
    }
    else if(BLOCK_TYPE_PIECE == dmh.type)
    {
      block_mgr_t *p_iw_blk = p_dm_var->dm_info.p_blk_mgr + index;
      u8 sec_num = p_iw_blk->size / FLASH_SECTION_SIZE;

      MT_ASSERT(0 == (p_iw_blk->size % FLASH_SECTION_SIZE));
      total_piece_sec_num += sec_num;

      if(min_piece_addr > p_dm_var->dm_info.p_blk_mgr[index].base_addr)
      {
        min_piece_addr = p_dm_var->dm_info.p_blk_mgr[index].base_addr;
      }

      p_iw_blk->sec_num = sec_num;
      p_iw_blk->sec_index = 0xFFFF;
      p_iw_blk->p_node_logic_info = mtos_malloc(
        dmh.node_num * sizeof(node_logic_info_t));

      MT_ASSERT(NULL != p_iw_blk->p_node_logic_info);
      //set default first
      //set node to 0 means: the node is deactived, the cache is empty
      memset(p_iw_blk->p_node_logic_info, 0,
        dmh.node_num * sizeof(node_logic_info_t));
    }
    else if(BLOCK_TYPE_DIRECT_RW == dmh.type)
    {
      block_mgr_t *p_direct_blk = p_dm_var->dm_info.p_blk_mgr + index;
      MT_ASSERT(0 == (p_direct_blk->size % FLASH_SECTION_SIZE));

      if(p_dm_var->dm_info.p_erase_buf == NULL)
      {
        DM_PRINT("only alloc once. block id=0x%x,type=0x%x\n", dmh.id, dmh.type);
        p_dm_var->dm_info.p_erase_buf = (u8 *)mtos_malloc(FLASH_SECTION_SIZE);
        MT_ASSERT(p_dm_var->dm_info.p_erase_buf != NULL);
        memset(p_dm_var->dm_info.p_erase_buf, 0x0, FLASH_SECTION_SIZE);
      }
    }
    else if(BLOCK_TYPE_FIXED_ADDR == dmh.type)
    {
      block_mgr_t *p_iw_blk = p_dm_var->dm_info.p_blk_mgr + index;
      u8 sec_num = p_iw_blk->size / FLASH_SECTION_SIZE;

      MT_ASSERT(0 == (p_iw_blk->size % FLASH_SECTION_SIZE));

      OS_PRINTF("p_iw_blk->size %d, sec_num %d, base_addr %x\n",
        p_iw_blk->size, sec_num, p_iw_blk->base_addr);
    
      p_iw_blk->sec_num = sec_num;
      p_iw_blk->sec_index = 0xFFFF;

      p_iw_blk->p_node_logic_info = mtos_malloc(
                                      dmh.node_num * sizeof(node_logic_info_t));
      MT_ASSERT(NULL != p_iw_blk->p_node_logic_info);
      //set default first
      //set node to 0 means: the node is deactived, the cache is empty
      memset(p_iw_blk->p_node_logic_info, 0,
        dmh.node_num * sizeof(node_logic_info_t));
      sec_init(p_dm_var, p_iw_blk->base_addr,
                            sec_num, 0, BLOCK_TYPE_FIXED_ADDR, dmh.id);
      sec_restore(p_dm_var, BLOCK_TYPE_FIXED_ADDR);
    }
  }

  if(total_iw_sec_num > 0) //has the iw block
  {
    p_dm_var->dm_info.iw_base_addr = min_iw_addr;
    p_dm_var->dm_info.total_iw_sec_num = total_iw_sec_num;
    p_dm_var->dm_info.total_iw_sec_max_size = min_iw_addr +  \
                                        total_iw_sec_num * FLASH_SECTION_SIZE;

    sec_init(p_dm_var, min_iw_addr, total_iw_sec_num, 0, BLOCK_TYPE_IW, 0);
    sec_restore(p_dm_var, BLOCK_TYPE_IW);
  }

  if(total_piece_sec_num > 0) //has the piece block
  {
    p_dm_var->dm_info.piece_base_addr = min_piece_addr;

    if(TRUE == dm_load_piece_data(p_data, min_piece_addr, total_piece_sec_num))
    {
      sec_init(p_dm_var, min_piece_addr,
        p_dm_var->dm_info.ghost_size / FLASH_SECTION_SIZE,
                  total_piece_sec_num, BLOCK_TYPE_PIECE, 0);
      sec_restore(p_dm_var, BLOCK_TYPE_PIECE);
    }
  }
  
  dm_unlock_v3(p_data);
}

///////////////////////////////fix me--->

/*!
  get block node num

  \param[in] p_data data manager handle
  \param[in] block_id

  \return current block node num
  */
static u16 dm_get_blk_node_num_v3(void * p_data, u8 block_id)
{
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;
  block_mgr_t *p_block = NULL;
  s32 index = 0;

  MT_ASSERT(p_data != NULL);

  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  if(index == ERR_FAILURE)
  {
    return 0;
  }
  
  p_block = p_dm_info->p_blk_mgr + index;

  return p_block->node_num;
}


/*!
  get block start offset in flash

  \param[in] block_id Block ID
  \return 0 if fail, offset if success,
  */
static u32 dm_get_block_addr_32(void *p_data, u8 block_id)
{
  s32 i = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *) p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  i = find_block_index(p_data, block_id);

  if(ERR_FAILURE == i)
  {
    return 0;
  }
  else
  {
    return p_dm_info->flash_base_addr + p_dm_info->p_blk_mgr[i].base_addr;
  }
}

static u32 dm_get_block_size_32(void *p_data, u8 block_id)
{
  s32 i = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  i = find_block_index(p_data, block_id);
  if(ERR_FAILURE == i)
  {
    return 0;
  }
  else
  {
    return p_dm_info->p_blk_mgr[i].size;
  }
}

/*!
  get block head info for a specific block id

  \param[in] block_id Block ID
  \param[out] head return header info
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_get_block_header_32(void *p_data, u8 block_id,
dmh_block_info_t *p_dm_head)
{
  s32 i = 0;
  u32 start = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(p_data != NULL);
  MT_ASSERT(&p_dm_var->dm_info != NULL);

  p_dm_info = &p_dm_var->dm_info;

  i = find_block_index(p_data, block_id);

  if(ERR_FAILURE == i)
  {
    return DM_FAIL;
  }

  dm_lock_v3(p_data);
  start = p_dm_info->p_blk_mgr[i].dm_header_addr;

  DM_PRINT("dm_get_block_header_32 addr: 0x%x\n", start);
  
  if(SUCCESS == data_read(p_dm_var, start,
                   (u8 *)p_dm_head, sizeof(dmh_block_info_t), BLOCK_TYPE_RO))
  {
    dm_unlock_v3(p_data);
    return DM_SUC;
  }
  else
  {
    dm_unlock_v3(p_data);
    return DM_FAIL;
  }
}


static u16 dm_get_max_blk_num_32(void * p_data)
{
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  return p_dm_info->block_num;
}

static u16  dm_get_max_node_len_32(void * p_data)
{
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  return p_dm_info->max_node_length;
}

/*!
  get block total number and each block ID

  \param[out] block_num Total block ID
  \param[out] id_list Block ID list
  \return None
  */
static dm_ret_t dm_get_block_info_32(void *p_data, u8 *p_block_num,
u8 *p_id_list)
{
  u32 i = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  MT_ASSERT(p_block_num != 0 && p_id_list != NULL);

  *p_block_num = p_dm_info->block_num;
  for(i = 0; i < p_dm_info->block_num; i ++)
  {
    p_id_list[i] = p_dm_info->p_blk_mgr[i].id;
  }
  return DM_SUC;
}

/*!
  display flash map info
  \return None
  */
static void dm_show_flash_map_32(void * p_data)
{
  DM_PRINT("don't supported dm_show_flash_map\n");
}


/*!
  Special function: read bootloader block data from bootloader section
  It is not a part of data manager header, just put here for use
  \param[in] bid bootloader bin file ID, see sys_define.h
  \param[in] offset Offset from the bin file start address
  \param[in] length read length
  \param[in] pbuffer Pointer to store data
  \return actual length if success, else ERR_FAILURE
  */
static s32 dm_read_bl_block_32(void *p_data, u32 bid, u32 offset,
u32 len, u8 *p_buff)
{
  u32 base_addr = 0;
  u32 start = 12;
  u32 FLASH_HEAD_MGR_UNIT_LENT = 12;
  u32 FLAHS_HEAD_MGR_UNIT_NUM = 4;
  u32 size = 0;
  u32 i = 0;
  u32 head[4] = {0,};
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_charsto_dev != NULL);
  
  dm_lock_v3(p_data);

  memset(p_buff, 0x0, len);

  for(i = 0; i < FLAHS_HEAD_MGR_UNIT_NUM; i ++)
  {
    if(SUCCESS != data_read(p_data,
        start + i * FLASH_HEAD_MGR_UNIT_LENT,
          (u8 *)head, FLASH_HEAD_MGR_UNIT_LENT, BLOCK_TYPE_RO))
    {
      dm_unlock_v3(p_data);
      return ERR_FAILURE;
    }

    if(bid == head[0])
    {
      base_addr = head[1];
      size = head[2];
      break;
    }
  }

  if((i >= FLAHS_HEAD_MGR_UNIT_NUM) ||(offset > size))
  {
    dm_unlock_v3(p_data);
    return ERR_FAILURE;
  }

  len = (size - offset > len) ? len : size - offset;
  if(SUCCESS != data_read(p_data, base_addr + offset,
          p_buff, len, BLOCK_TYPE_RO))
  {
    dm_unlock_v3(p_data);
    return ERR_FAILURE;
  }
  else
  {
    dm_unlock_v3(p_data);
    return len;
  }
}

/********************************************************************
                    data manager direct functions
 *******************************************************************/

/*!
  direct read data from flash
  */
static u32 dm_direct_read_v3(void *p_data, u8 block_id, u32 offset, u32 len, u8 *p_buf)
{
  u32 read_len = 0;
  u32 node_addr = 0;
  s32 b_index = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  block_mgr_t *p_block = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  MT_ASSERT(p_buf != NULL);
  MT_ASSERT(len > 0);
  
  memset(p_buf, 0x0, len);
  
  b_index = find_block_index(p_dm_var, block_id);

  if(ERR_FAILURE == b_index)
  {
    return 0;
  }

  p_block = p_dm_info->p_blk_mgr + b_index;
  MT_ASSERT(BLOCK_TYPE_DIRECT_RW == p_block->type);

  node_addr = p_block->base_addr + offset;

  //some times,  read data cross-border
  if((offset + len) >  p_block->size)
  {
    len = p_block->size - offset;
  }
  
  dm_lock_v3(p_data);
  
  if(SUCCESS == data_read(p_dm_var, node_addr, p_buf, len, BLOCK_TYPE_DIRECT_RW))
  {
    read_len = len;
  }

  dm_unlock_v3(p_data);
  
  return read_len;
}

/*!
  direct Write data into flash
  */
static dm_ret_t dm_direct_write_v3(void *p_data, u8 block_id, u32 offset,
u32 len, u8 *p_buf)
{
  block_mgr_t *p_block = NULL;
  u32 write_addr = 0;
  s32 index = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  MT_ASSERT(p_buf != NULL);
  MT_ASSERT(len > 0);

  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  if(index == ERR_FAILURE)
  {
    return DM_FAIL;
  }
  
  dm_lock_v3(p_data);

  p_block = p_dm_info->p_blk_mgr + index;
  MT_ASSERT(BLOCK_TYPE_DIRECT_RW == p_block->type);

  if((offset + len) > p_block->size)
  {
    DM_ERROR("write len bigger than block surplus size, write_addr %d,"
        "len %d, bl_size %d, bl_id %d\n", write_addr, len, p_block->size, block_id);
    return DM_FULL;
  }

  write_addr = p_block->base_addr + offset;
  if(SUCCESS == data_writeonly(p_data, write_addr, p_buf, len, p_block->type))
  {
    dm_unlock_v3(p_data);
    return DM_SUC;
  }
  else
  {
    dm_unlock_v3(p_data);
    return DM_FAIL;
  }
}

/*!
  direct erase data into flash
  */
static dm_ret_t dm_direct_erase_v3(void *p_data, u8 block_id, u32 offset,
  u32 len)
{
  block_mgr_t *p_block = NULL;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  u32 addr = 0;
  s32 index = 0;
  u8 sec_off_num = 0;
  u32 sec_off_addr = 0;
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_data != NULL);
  MT_ASSERT(len > 0);

  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;
  MT_ASSERT(p_dm_info->p_erase_buf != NULL);
  
  index = find_block_index(p_dm_var, block_id);
  if(index == ERR_FAILURE)
  {
    return DM_FAIL;
  }
  
  dm_lock_v3(p_data);

  p_block = p_dm_info->p_blk_mgr + index;
  MT_ASSERT(BLOCK_TYPE_DIRECT_RW == p_block->type);

  if((offset + len) > p_block->size)
  {
    DM_ERROR("why erase size bigger than block size, offset %d, len %d, size %d,"
      "block_id %d\n",offset, len, p_block->size, block_id);
    MT_ASSERT(0);
  }

  sec_off_num = offset / FLASH_SECTION_SIZE;
  sec_off_addr = offset - sec_off_num * FLASH_SECTION_SIZE;

  addr = p_block->base_addr + sec_off_num * FLASH_SECTION_SIZE;

  if(sec_off_addr + len <= FLASH_SECTION_SIZE)
  {
    ret = data_read(p_data, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE, p_block->type);
    MT_ASSERT(SUCCESS == ret);
    memset(p_dm_info->p_erase_buf + sec_off_addr, 0xFF, len);

    ret = data_erase(p_data, addr, 1, p_block->type);
    MT_ASSERT(SUCCESS == ret);

    ret = data_writeonly(p_data, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE, p_block->type);
    MT_ASSERT(SUCCESS == ret);
  }
  else if(sec_off_addr + len > FLASH_SECTION_SIZE)
  {
    //head
    data_read(p_data, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE, p_block->type);
    memset(p_dm_info->p_erase_buf + sec_off_addr, 0xFF, FLASH_SECTION_SIZE - sec_off_addr);

    ret = data_erase(p_data, addr, 1, p_block->type);
    MT_ASSERT(SUCCESS == ret);

    ret = data_writeonly(p_data, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE, p_block->type);
    MT_ASSERT(SUCCESS == ret);

    //middle
    addr += FLASH_SECTION_SIZE;
    len -= (FLASH_SECTION_SIZE - sec_off_addr);
    while(len >= FLASH_SECTION_SIZE)
    {
      ret = data_erase(p_data, addr, 1, p_block->type);
      MT_ASSERT(SUCCESS == ret);

      len -= FLASH_SECTION_SIZE;
      addr += FLASH_SECTION_SIZE;
    }

    //tail
    data_read(p_data, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE, p_block->type);
    memset(p_dm_info->p_erase_buf, 0xFF, len);
    ret = data_erase(p_data, addr, 1, p_block->type);
    MT_ASSERT(SUCCESS == ret);
    ret = data_writeonly(p_data, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE, p_block->type);
    MT_ASSERT(SUCCESS == ret);
  }

  dm_unlock_v3(p_data);
  
  return DM_SUC;
}

/*!
  get all node id of block.
  */
static u32 dm_get_node_id_list_v3(void *p_data, u8 block_id,
  u16 *p_node_id_list, u32 total_list)
{
  dm_global_info_t *p_dm_info    = NULL;
  block_mgr_t      *p_block_info = NULL;
  dm_var_t         *p_dm_var     = NULL;
  u32               node_id      = 0;
  s32               index        = 0;
  u32               id_idx       = 0;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  MT_ASSERT(total_list > 0 && p_node_id_list != NULL);

  index = find_block_index(p_dm_var, block_id);
  if(index == ERR_FAILURE)
  {
    return DM_FAIL;
  }

  p_block_info = p_dm_info->p_blk_mgr + index;

  for(node_id = 0; node_id < p_block_info->node_num; node_id++)
  {
    if(1 == p_block_info->p_node_logic_info[node_id].active)
    {
      p_node_id_list[id_idx++] = node_id;

      if(id_idx >= total_list)
      {
        break;
      }
    }
  }

  return id_idx;
}

/*!
  Init dm: get dm header info and restore IW block info
  \return None
  */
void dm_init_v3(dm_v3_init_para_t *p_para)
{
  dm_global_info_t *p_dm_info = NULL;
  charsto_device_t *p_charsto_dev = NULL;
  dm_var_t *p_dm_priv_var = NULL;
  dm_proc_t *p_dm_proc = NULL;
  u8         i = 0;

  MT_ASSERT(p_para != NULL);

  p_dm_proc = class_get_handle_by_id(DM_CLASS_ID);
  if(p_dm_proc != NULL) //only init once
  {
    return;
  }

  //Malloc data manager process handle
  p_dm_proc = mtos_malloc(sizeof(dm_proc_t));
  MT_ASSERT(p_dm_proc != NULL);
  memset(p_dm_proc, 0, sizeof(dm_proc_t));

  //Malloc private data for data manager32
  p_dm_proc->p_priv_data = mtos_malloc(sizeof(dm_var_t));
  MT_ASSERT(p_dm_proc->p_priv_data != NULL);
  memset(p_dm_proc->p_priv_data, 0, sizeof(dm_var_t));

  //Attach process function
  p_dm_proc->active_node      = dm_active_node_v3;
  p_dm_proc->check_block      = dm_check_block_v3;
  p_dm_proc->del_node         = dm_del_node_v3;
  p_dm_proc->flush_cache      = dm_flush_cache_v3;
  p_dm_proc->reset_iw_block   = dm_reset_iw_block_v3;
  p_dm_proc->restore_cache    = dm_restore_cache_v3;
  p_dm_proc->set_cache        = dm_set_cache_v3;
//  p_dm_proc->set_fix_id       = dm_set_fix_id_v3;
  p_dm_proc->write_node       = dm_write_node_v3;
  p_dm_proc->read_node        = dm_read_v3;
  p_dm_proc->set_header       = dm_set_header_v3;
  p_dm_proc->set_header_ex    = NULL;
  p_dm_proc->data_restore     = dm_restore_v3;
  p_dm_proc->direct_read      = dm_direct_read_v3;
  p_dm_proc->direct_write     = dm_direct_write_v3;
  p_dm_proc->direct_erase     = dm_direct_erase_v3;
  p_dm_proc->get_node_id_list = dm_get_node_id_list_v3;
  p_dm_proc->get_blk_node_num = dm_get_blk_node_num_v3;

  ////fix me
  p_dm_proc->get_block_addr   = dm_get_block_addr_32;
  p_dm_proc->get_block_header = dm_get_block_header_32;
  p_dm_proc->get_block_info   = dm_get_block_info_32;
  p_dm_proc->get_block_size   = dm_get_block_size_32;
  p_dm_proc->read_bl_block    = dm_read_bl_block_32;
  p_dm_proc->get_max_node_len = dm_get_max_node_len_32;
  p_dm_proc->get_max_blk_num  = dm_get_max_blk_num_32;
  p_dm_proc->show_flash_map   = dm_show_flash_map_32;
  p_dm_proc->set_decompress_buf = dm_set_decompress_buf_v3;
  p_dm_proc->store_piece_data   = dm_store_piece_data_v3;
  p_dm_proc->check_compress_match_flag = dm_check_compress_match_flag_v3;
  p_dm_proc->set_compress_unmatch_flag = dm_set_compress_unmatch_flag_v3;

  //Initialize field members of private data information
  p_dm_priv_var = (dm_var_t *)p_dm_proc->p_priv_data;

  //Initialize data manager base information
  p_dm_info = &p_dm_priv_var->dm_info;
  memset(p_dm_info, 0, sizeof(dm_global_info_t));

  p_dm_info->max_blk_num = p_para->max_blk_num;
  p_dm_info->flash_base_addr = p_para->flash_base_addr;
  p_dm_info->monitor_turn = 0;

  p_dm_info->p_ghost =(u8 *)p_para->ghost_addr;
  p_dm_info->ghost_size = p_para->piece_size;
  memset((void *)p_dm_info->p_ghost, 0xFF, p_dm_info->ghost_size);

  p_dm_info->p_blk_mgr = mtos_malloc(p_para->max_blk_num
    * sizeof(block_mgr_t));
  MT_ASSERT(p_dm_info->p_blk_mgr != NULL);
  memset(p_dm_info->p_blk_mgr, 0, p_para->max_blk_num * sizeof(block_mgr_t));

  if(p_para->using_max_node_len <= DM_MAX_NODE_LOWER_LIMIT_LEN)
  {
    p_dm_info->max_node_length =  DM_MAX_NODE_LOWER_LIMIT_LEN;
  }
  else if(p_para->using_max_node_len < DM_MAX_NODE_HIGH_LIMIT_LEN)
  {
    p_dm_info->max_node_length = p_para->using_max_node_len;
  }
  else
  {
    DM_ERROR("using max node len %d\n", p_para->using_max_node_len);
    MT_ASSERT(0);
  }
  
  if(p_para->use_mutex)
  {
    p_dm_priv_var->is_use_mutex = TRUE;
    p_dm_priv_var->p_dm_mutex = mtos_mutex_create(p_para->mutex_prio);
    MT_ASSERT(p_dm_priv_var->p_dm_mutex != NULL);
  }
  else
  {
    p_dm_priv_var->is_use_mutex = FALSE;
    p_dm_priv_var->p_dm_mutex = NULL;
  }

  for(i = 0; i < DM_MAX_NODE_BUFFER;  i++)
  {
    p_dm_info->buf_list[i].is_used = FALSE;
    p_dm_info->buf_list[i].p_node_buf = (u8 *)mtos_malloc(p_dm_info->max_node_length);
    MT_ASSERT(p_dm_info->buf_list[i].p_node_buf != NULL);
  }
  
  p_dm_info->p_erase_buf = NULL;

  p_dm_info->prev_blk_id = 0;

  //Charsto device init
  p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_charsto_dev != NULL);
  p_dm_priv_var->p_charsto_dev = p_charsto_dev;

  //create task
  p_dm_priv_var->is_open_monitor = p_para->open_monitor;
  
  if(p_para->open_monitor)
  {
    u32 *p_stk = (u32 *)mtos_malloc(p_para->task_stack_size);
    MT_ASSERT(p_stk != NULL);

    if(!mtos_task_create((u8 *)"dm_v3", task_process, p_dm_proc->p_priv_data,
      p_para->task_prio, p_stk, p_para->task_stack_size))
    {
      DM_ERROR("create task monitor fail!\n");
      MT_ASSERT(0);
    }
  }

  p_dm_priv_var->is_test_mode = p_para->test_mode;

  //Register handle
  class_register(DM_CLASS_ID, p_dm_proc);

  //DM_PRINT("using data_manager_v2\n");
}

