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
#include "video.h"
#include "pti.h"
#include "audio.h"
#include "mdl.h"

#include "data_manager.h"
#include "data_manager_priv.h"
#include "data_manager_v2.h"
//log for performance
#include "log.h"
#include "log_mgr.h"
#include "log_interface.h"
#include "log_dm_imp.h"
#include "log_flash_protect_imp.h"

//#define DM_DETAIL_TRACE
#define DM_DEBUG_TRACE
//#define DM_ERROR_TRACE
//#define DM_VERIFY_OPEN
//#define DM_DEBUG_MUTEX

#ifdef DM_DETAIL_TRACE
#define DM_DETAIL OS_PRINTF
#else
#define DM_DETAIL DUMMY_PRINTF
#endif

#ifdef DM_DEBUG_TRACE
#define DM_PRINT OS_PRINTF
#else
#define DM_PRINT DUMMY_PRINTF
#endif

#ifdef DM_ERROR_TRACE
#define DM_ERROR OS_PRINTF("DM_ERROR!! function %s, line %d!!!!",   \
                            __FUNCTION__, __LINE__);OS_PRINTF
#else
#define DM_ERROR DUMMY_PRINTF
#endif

#ifdef DM_DEBUG_MUTEX
#define DM_DBG_MUTEX OS_PRINTF("DM_MUTEX %s, %d line ", __FUNCTION__, __LINE__);OS_PRINTF
#else
#define DM_DBG_MUTEX DUMMY_PRINTF
#endif


#define BUNDLE_RESTORE
//#define FLASH_PROTECT_DEBUG

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
    u16 id;
    /*!
      active flag
      */
    u8 active;
    /*!
      Node length
      */
    u8 length;
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
  align to 4 bytes or16 bytes
  */
#define ALIGN(x, y) (((y) + (x) - 1) / (x) * (x))

/*!
  Data manager atom size
  */
//#define DM_ATOM_SIZE (16)

#define DM_ATOM_SIZE(x) ((x) ? (4) : (16))


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
  Max node length in data manager 32
  */
#define DM_MAX_NODE_LEN (MAX_NODE_LEN + NODE_HEADER_SIZE)

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
  u32 node_iw_addr :21;   //base for iw addr
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
    dispatch counter
    */
  u32 dispatch_num;
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
  //u16 fix_num;
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
    iw_sec total
    */
  u16 total_iw_sec_num;
  /*!
    block total
    */
  u16 max_blk_num;
   /*!
    iw section size total
    */
  u32 total_iw_sec_max_size;
  /*!
    records previous block id, usually this index as same as the previous index.
    */
  u32 prev_blk_id;
  /*!
    block manager
    */
  block_mgr_t *p_blk_mgr;
  /*!
    section manager
    */
  section_mgr_t sec_mgr;
  /*!
    mutex sem
    */
  void *p_dm_mutex;
  /*!
    direct erase uses buffer start address.
    */
  u8 *p_erase_buf;
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
    is test mode : if your project is test mode, please set TRUE.
    Because sometimes is hardware error, but we must assure system can work well in normal mode.
    */
  BOOL is_test_mode;
  /*!
    use mutex or not
    */
  BOOL is_use_mutex;
  /*!
    monitor task stack address.
    */
  u32 *p_stack;
  /*!

    task priority.
    */
   u32 task_prio;
  /*!
    align to  bytes
    */
  u32 align_bytes;
 /*!
   operation protect mutex
   */
  void *p_dm_mutex;
}dm_var_t;



/********************************************************
                    prefix define
 ********************************************************/
static s32 find_block_index(dm_var_t *p_data, u8 block_id);
static void sec_cleanup_overdru(dm_var_t *p_data, u32 sec_indx);
static void write_del_node(dm_var_t *p_data, node_logic_info_t *p_node,
               u16 node_id);
/********************************************************
                    section manager functions
 ********************************************************/
static void section_show(dm_var_t *p_data, char *p_tag)
{
#ifdef DM_DEBUG_TRACE
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = NULL;
  u32 i = 0;

  DM_PRINT("-------->section_show: %s\n", p_tag);

  for(i = 0; i < p_sec_mgr->total_section_num; i++)
  {
    p_sec = p_sec_mgr->p_sec_arry + i;
    DM_PRINT("\tsec[%.2d]: \tindex[%.3d], block[0x%.2x], "
      "statue[0x%.8x], cur[0x%.5x], valid[0x%.5x], sec_id[%d]\n",
      i, p_sec->header.index, p_sec->header.block_id, p_sec->header.status,
      p_sec->cur_sec_addr, p_sec->valid_len, p_sec->header.sec_id);
  }
  DM_PRINT("-------->section_show end: base_iw_addr[0x%x]\n",
    p_sec_mgr->sec_base_addr);

#endif
}

static void dm_lock_v2(dm_var_t *p_data)
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

static void dm_unlock_v2(dm_var_t *p_data)
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

static RET_CODE data_writeonly_v2(charsto_device_t *p_dev, u32 addr, u8 *p_buf, u32 len)
{
  RET_CODE ret = SUCCESS;

  log_perf(LOG_FLASH_PROTECT,DM_WRITE_ONLY_V2,0,0);
  ret = charsto_writeonly(p_dev, addr, p_buf, len);

  return ret;
}

static RET_CODE data_erase_v2(charsto_device_t *p_dev, u32 addr, u32 sec_cnt)
{
  RET_CODE ret = SUCCESS;

  log_perf(LOG_FLASH_PROTECT,DM_DATA_ERASE_V2,0,0);
  ret = charsto_erase(p_dev, addr, sec_cnt);

  return ret;
}


static void sec_hardware_error(dm_var_t *p_data, BOOL is_assert)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  u16 sec_total_num = p_sec_mgr->total_section_num;
  u32 sec_base_addr = p_sec_mgr->sec_base_addr;
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_data != NULL);

  if(is_assert)
    return;

  if(!p_data->is_test_mode)
  {
    //first clear iw section
    ret = data_erase_v2(p_data->p_charsto_dev, sec_base_addr, sec_total_num);
    MT_ASSERT(ret == SUCCESS);

    #ifndef WIN32
      hal_pm_reset();
    #else
      //hardware flash error, please restart simulator
      MT_ASSERT(0);
    #endif //End for WIN32
  }
  else
  {
    MT_ASSERT(0);
  }
}

//read section actual header information to buffer.
static void sec_init(dm_var_t *p_data, u32 base_addr, u16 sec_num)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = NULL;
  u16 _cur = 0;

  MT_ASSERT(NULL == p_sec_mgr->p_sec_arry); //fix me. only init once
  p_sec_mgr->sec_base_addr = base_addr;
  p_sec_mgr->total_section_num = sec_num;
  p_sec_mgr->dispatch_num = 0;
  p_sec_mgr->p_sec_arry = mtos_malloc(sec_num * sizeof(section_info_t));

  //init sec header
  for(_cur = 0; _cur < sec_num; _cur++)
  {
    p_sec = p_sec_mgr->p_sec_arry + _cur;
    p_sec->cur_sec_addr = SEC_HEAD_SIZE;
    p_sec->valid_len = 0;
    charsto_read(p_data->p_charsto_dev, base_addr + FLASH_SECTION_SIZE * _cur,
      (u8 *)&p_sec->header, SEC_HEAD_SIZE);
    DM_PRINT("read addr 0x%x base addr 0x%x\n",base_addr + FLASH_SECTION_SIZE * _cur, base_addr);
    if(p_sec->header.status != SEC_CLEAN)
    {
      if(_cur != p_sec->header.index)
      {
        DM_PRINT("fail header. addr 0x%x, sec[%d]: status 0x%x, index %d\n",
          base_addr + FLASH_SECTION_SIZE * _cur, _cur,
          p_sec->header.status, p_sec->header.index);
        sec_hardware_error(p_data, 0);
      }
      if(p_sec_mgr->dispatch_num <= p_sec->header.sec_id)
      {
        //always equal next of max id.
        p_sec_mgr->dispatch_num = p_sec->header.sec_id + 1;
      }
    }
  }
  section_show(p_data, "Init done");
}

static BOOL sec_using_out(dm_var_t *p_data, u8 block_id)
{
  s32               block_index = ERR_FAILURE;
  block_mgr_t      *p_blk_mgr   = NULL;
  section_mgr_t    *p_sec_mgr   = NULL;
  section_header_t *p_header    = NULL;
  u32 i = 0;
  u32 total = 0;

  block_index = find_block_index(p_data, block_id);
  MT_ASSERT(block_index != ERR_FAILURE);

  p_blk_mgr = p_data->dm_info.p_blk_mgr + block_index;
  p_sec_mgr = &p_data->dm_info.sec_mgr;


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

static BOOL sec_repair_check(dm_var_t *p_data, u32 sec_idx)
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
    charsto_read(p_data->p_charsto_dev, read_addr, p_buf, buf_len);

    for(i = 0; i < buf_len; i++)
    {
      if(p_buf[i] != 0xFF)
      {
        //if find one byte not equal 0xff, erase this section again.
        data_erase_v2(p_data->p_charsto_dev, sec_flash_addr, 1);
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
  //s32 block_index = find_block_index(p_data, block_id);
  //block_mgr_t *p_blk_mgr = p_data->dm_info.p_blk_mgr + block_index;
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_header_t *p_header = NULL;
  u32 i = 0;

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
        sec_hardware_error(p_data, 0);
      }
      p_header->index = i;
      p_header->sec_id = p_sec_mgr->dispatch_num;
      p_sec_mgr->dispatch_num++;
      DM_PRINT("find sec second: sec[%d], block_id[0x%x], prio[0x%x]\n",
        i, block_id, prio_sel);
      *p_new_sec_idx = i;
      sec_repair_check(p_data, i);
      return TRUE;
    }
  }

  DM_ERROR("ERROR: can't find new sec\n");
  section_show(p_data, "can't find new sec");
  return FALSE;
}

static void sec_set_status(dm_var_t *p_data, u32 sec_indx,
  sec_status_t sec_st, u8 block_id)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  u32 addr = p_sec_mgr->sec_base_addr + sec_indx * FLASH_SECTION_SIZE;

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
      sec_hardware_error(p_data, p_sec->header.index == sec_indx);
      sec_hardware_error(p_data, p_sec->header.sec_id < p_sec_mgr->dispatch_num);
      data_writeonly_v2(p_data->p_charsto_dev, addr,
        (u8 *)&p_sec->header, SEC_HEAD_SIZE);
#ifdef DM_VERIFY_OPEN
      {
        section_header_t sec_header;
        charsto_read(p_data->p_charsto_dev, addr,
          (u8 *)&sec_header, SEC_HEAD_SIZE);
        if(sec_header.status != sec_st || sec_header.block_id != block_id ||
          sec_header.index != sec_indx ||
          sec_header.sec_id != p_sec->header.sec_id)
        {
          DM_ERROR("sec : status[0x%x], block_id[0x%x], index[%d], id[%d], "
              "read sec : status[0x%x], block_id[0x%x], index[%d], id[%d].\n",
            p_sec->header.status, p_sec->header.block_id, p_sec->header.index,
            p_sec->header.sec_id, sec_header.status, sec_header.block_id,
            sec_header.index, sec_header.sec_id);
          sec_hardware_error(p_data, 0);
        }
      }
#endif
      break;
    default:
      break;
  }
  section_show(p_data, "sec_set_status end");
}

static u32 sec_write_node(dm_var_t *p_data, u32 sec_indx,
  u8 *p_buffer, u32 len)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  //iw addr : base for iw first address
  u32 iw_addr = 0;

  DM_DBG_MUTEX("sec index %d\n", sec_indx);
  MT_ASSERT(p_sec->cur_sec_addr + len <= FLASH_SECTION_SIZE);
  //need protect by task lock
  iw_addr = sec_indx * FLASH_SECTION_SIZE + p_sec->cur_sec_addr;

  data_writeonly_v2(p_data->p_charsto_dev,
        p_sec_mgr->sec_base_addr + iw_addr, p_buffer, len);

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
    charsto_read(p_data->p_charsto_dev, p_sec_mgr->sec_base_addr + iw_addr,
      (u8 *)&head, NODE_HEADER_SIZE);
    if(0 == head.length || head.length != p_src_head->length
      || 0 == head.active || head.id != p_src_head->id)
    {
      DM_ERROR("src header: id[%d], len[%d], act[%d],"
        " read header[0x%x]: id[%d], len[%d], act[%d]\n",
        p_src_head->id, p_src_head->length, p_src_head->active,
        iw_addr, head.id, head.length, head.active);
      sec_hardware_error(p_data, 0);
    }
  }
#endif

  return iw_addr;
}


static void sec_del_node(dm_var_t *p_data, u32 sec_indx, u32 sec_offset,
  u8 *p_buffer, u32 w_len, u32 node_len)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  sec_status_t status = p_sec->header.status;
  u32 iw_addr = sec_indx * FLASH_SECTION_SIZE + sec_offset;
  u32 flash_addr = p_data->dm_info.iw_base_addr + iw_addr;

  if((SEC_STREAM_IN != status) && (SEC_STREAM_OUT != status)
    && (SEC_WRITE != status) && (SEC_USED != status))
  {
    sec_hardware_error(p_data, 0);
  }

  if(p_sec->valid_len < node_len)
  {
    section_show(p_data, "sec_del_node error\n");
    DM_ERROR("del node error. flash addr 0x%x, valid_len %d, node_len %d\n",
      flash_addr, p_sec->valid_len, node_len);
  }
  MT_ASSERT(p_sec->valid_len >= node_len);

  data_writeonly_v2(p_data->p_charsto_dev, flash_addr, p_buffer, w_len);
  p_sec->valid_len -= node_len;


  DM_DETAIL("sec_del: iw_addr[0x%x], sec_indx[%d], sec_addr[0x%x],"
    " valid_len[0x%x], node_id[%d], block_id[0x%x]\n",
    iw_addr, sec_indx, p_sec->cur_sec_addr, p_sec->valid_len,
    ((node_header_t *)p_buffer)->id, p_sec->header.block_id);

#ifdef DM_VERIFY_OPEN
  {
    node_header_t head;
    node_header_t *p_src_head = (node_header_t *)p_buffer;
    charsto_read(p_data->p_charsto_dev,
      flash_addr, (u8 *)&head, NODE_HEADER_SIZE);
    if(0 == head.length || head.length != p_src_head->length
      || 0 != head.active || head.id != p_src_head->id)
    {
      DM_ERROR("src header: id[%d], len[%d], act[%d],"
        " read header: id[%d], len[%d], act[%d]\n",
        p_src_head->id, p_src_head->length, p_src_head->active,
        head.id, head.length, head.active);
      sec_hardware_error(p_data, 0);
    }
  }
#endif
}

static void sec_alloc_buf(dm_global_info_t *p_dm_info, u8 **p_buf)
{
  if(p_dm_info->p_erase_buf != NULL)
  {
    DM_PRINT("sec alloc buf has erase buffer . addr 0x%x\n", p_dm_info->p_erase_buf);
    *p_buf = p_dm_info->p_erase_buf;
  }
  else
  {
    *p_buf = mtos_malloc(FLASH_SECTION_SIZE);
    MT_ASSERT(*p_buf != NULL);
  }
}

static void sec_free_buf(dm_global_info_t *p_dm_info, u8 *p_buf)
{
  if(p_dm_info->p_erase_buf != NULL)
  {
    DM_PRINT("sec free buf has erase buffer . addr 0x%x\n", p_dm_info->p_erase_buf);
  }
  else
  {
    mtos_free(p_buf); //fix me: it come from mtos_malloc???
  }
}

//this function will be ventured
static void sec_to_sdram(dm_var_t *p_data, u32 sec_indx)
{
  block_mgr_t *p_blk_mgr = p_data->dm_info.p_blk_mgr;
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  s32 block_idx = 0;
  u32 flash_addr = p_sec_mgr->sec_base_addr + sec_indx * FLASH_SECTION_SIZE;
  u32 new_sec_idx = 0;
  u32 offset = SEC_HEAD_SIZE; //current sec
  u32 cur_node_size = 0; //node step
  u32 write_size = 0;
  node_logic_info_t *p_logic_node = NULL;
  u32 node_iw_addr = 0;
  BOOL ret = FALSE;
  u8 *p_sec_buf = NULL;
  node_header_t *p_header = NULL;
  u8 *p_rw_buf = NULL;
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  u8 block_id = p_sec->header.block_id;

  sec_alloc_buf(&p_data->dm_info, &p_sec_buf);
  MT_ASSERT(p_sec_buf != NULL);
  memset(p_sec_buf, 0x0, FLASH_SECTION_SIZE);
  DM_ERROR("ERROR!! Enter sec_to_sdram: sce[%d]\n", sec_indx);
  section_show(p_data, "enter sec_to_sdram");

  block_idx = find_block_index(p_data, block_id);
  MT_ASSERT(block_idx != ERR_FAILURE);

  //read all data out
  charsto_read(p_data->p_charsto_dev, flash_addr,
    p_sec_buf, FLASH_SECTION_SIZE);

  //reset it, NOTE !!! the p_sec info was changed
  sec_cleanup_overdru(p_data, sec_indx);
  DM_DBG_MUTEX("sec index %d\n", sec_indx);

  //get it back.
  ret = sec_find_new(p_data, &new_sec_idx, block_id, SEC_CLEAN);
  MT_ASSERT(ret == TRUE);
  DM_DBG_MUTEX("new sec index %d\n", new_sec_idx);

  //must get it
  section_show(p_data, "sec_to_sdram get back sec");

  //init the sec
  sec_set_status(p_data, new_sec_idx, SEC_STREAM_IN, block_id);

  while(offset < FLASH_SECTION_SIZE)
  {
    //read a node header
    p_header = (node_header_t *)(p_sec_buf + offset);

    if(ERASE_DATA == p_header->value)  //find invalid data, means read finish
    {
      DM_DETAIL("s2r: finished on sec_addr[0x%x]\n", offset);
      break;
    }

    cur_node_size = (p_header->length * p_data->align_bytes);

    //DO NOT need read the intact_flag!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //because if the node is unintace, the maping point the older
    //charsto_read(p_data->p_charsto_dev, addr + offset + step,
    //  &intact_flag, 1);

    p_logic_node = p_blk_mgr[block_idx].p_node_logic_info + p_header->id;
    node_iw_addr = p_logic_node->node_iw_addr;

    DM_DETAIL("s2r:iw_addr[0x%x], node_id[%d], len[%d], act[%d], link[%d]\n",
      new_sec_idx * FLASH_SECTION_SIZE + offset, p_header->id, cur_node_size,
      p_header->active,
      (0 != p_logic_node->node_iw_addr && 0 != p_logic_node->active));

    //unexpected data, skip it
    if(p_header->id >= p_blk_mgr[block_idx].node_num)
    {
    }
    //the size is invalid, skip it
    else if(cur_node_size > DM_MAX_NODE_LEN)
    {
    }
    //the size is invalid, skip it
    else if(0 == cur_node_size)
    {
      DM_ERROR(" ZERO size: header_id[%d], block[0x%x], addr[0x%x]\n",
        p_header->id, block_id, flash_addr + offset);
      sec_hardware_error(p_data, 0);
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
        p_header = (node_header_t *)rw_buf;
        //get the data size
        charsto_read(p_data->p_charsto_dev, p_data->dm_info.iw_base_addr +
          node_iw_addr, rw_buf, NODE_HEADER_SIZE);
        write_size = p_header->length * p_data->align_bytes;

        //read whole node
        charsto_read(p_data->p_charsto_dev, p_data->dm_info.iw_base_addr +
          node_iw_addr, rw_buf, write_size);
        p_header->active = 1; //must set it. the mapping may point older
        p_rw_buf = rw_buf;
        //don't need it. because node is older data, it should be deactived
      }
      p_blk_mgr[block_idx].p_node_logic_info[p_header->id].node_iw_addr =
      sec_write_node(p_data, new_sec_idx, p_rw_buf, write_size);
    }
    offset += cur_node_size;
  }
  sec_free_buf(&p_data->dm_info, p_sec_buf);
}

static void sec_to_sec(dm_var_t *p_data, u32 sec_out_indx)
{
  block_mgr_t *p_blk_mgr = p_data->dm_info.p_blk_mgr;
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_out_sec = p_sec_mgr->p_sec_arry + sec_out_indx;
  section_info_t *p_in_sec = NULL;
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  u16 rw_len = DM_MAX_NODE_LEN;
  node_header_t *p_header = (node_header_t *)rw_buf;
  node_logic_info_t *p_node_logic = NULL;
  u32 transfer_sec_indx = 0;
  u32 new_iw_addr = 0;
  s32 block_index = 0;
  u32 out_sec_flash_addr = 0;
  u32 offset = SEC_HEAD_SIZE;
  u32 cur_node_size = 0; //node step
  BOOL find_idle_sec = FALSE;
  u16 node_id = 0;

  DM_PRINT("Enter sec_to_sec: sce[%d]\n", sec_out_indx);
  section_show(p_data, "sec_to_sec");
  //clean up block
  block_index = find_block_index(p_data, p_out_sec->header.block_id);
  MT_ASSERT(block_index != ERR_FAILURE);

  out_sec_flash_addr = p_sec_mgr->sec_base_addr +
    sec_out_indx * FLASH_SECTION_SIZE;

  while(offset < FLASH_SECTION_SIZE)
  {
    //read a node header
    charsto_read(p_data->p_charsto_dev, out_sec_flash_addr + offset,
      rw_buf, NODE_HEADER_SIZE);

    if(ERASE_DATA == p_header->value)  //find invalid data, means read finish
    {
      DM_DETAIL("s2s: finished on sec_addr[0x%x]\n", offset);
      break;
    }

    cur_node_size = (p_header->length * p_data->align_bytes);

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
        DM_DBG_MUTEX("set transfer sec index %d\n", transfer_sec_indx);
        sec_set_status(p_data, transfer_sec_indx,
          SEC_STREAM_IN, p_out_sec->header.block_id);
      }
      else
      {
        sec_to_sdram(p_data, sec_out_indx);
        return;
      }
    }

    //DO NOT need read the intact_flag!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //because if the node is unintace, the maping point the older
    //charsto_read(p_data->p_charsto_dev, addr + offset + step,
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
    else if(cur_node_size > DM_MAX_NODE_LEN)
    {
    }
    //the size is invalid, skip it
    else if(0 == cur_node_size)
    {
      DM_ERROR(" ZERO size: header_id[%d], block[0x%x], addr[0x%x]\n",
        p_header->id, p_out_sec->header.block_id, out_sec_flash_addr + offset);
      sec_hardware_error(p_data, 0);
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
      if((rw_len + p_data->dm_info.iw_base_addr + p_node_logic->node_iw_addr) >
        p_data->dm_info.total_iw_sec_max_size)
      {
        rw_len = p_data->dm_info.total_iw_sec_max_size - p_data->dm_info.iw_base_addr - \
                  p_node_logic->node_iw_addr;
      }

      charsto_read(p_data->p_charsto_dev, p_data->dm_info.iw_base_addr
        + p_node_logic->node_iw_addr, rw_buf, rw_len);

      //the buffer reload
      sec_hardware_error(p_data, node_id == p_header->id);

      p_header->active = 1; //must set it. because the mapping may point older

      //NOTE!! must recalc the node size, because the two time size is unequal
      //new_iw_addr = sec_write_node(p_data, transfer_sec_indx, rw_buf, step);
      new_iw_addr = sec_write_node(p_data, transfer_sec_indx, rw_buf,
        p_header->length * p_data->align_bytes);

      //delete cur node
      write_del_node(p_data, p_node_logic, p_header->id);

      //NOTE!!!NOTE!!!NOTE!!!NOTE!!!
      // "p_node_logic->node_iw_addr" will be set to
      // NULL in the function write_del_node()
      p_node_logic->node_iw_addr = new_iw_addr;
    }
    offset += cur_node_size;
  }

  //data was transfer out DONE,set status to overdru
  sec_set_status(p_data, sec_out_indx,
    SEC_STREAM_OVERDUE, p_in_sec->header.block_id);
}

static void sec_cleanup_out(dm_var_t *p_data, u32 sec_indx)
{
  u32 enter_ticks = mtos_ticks_get();
  u32 exit_ticks = 0;

  DM_PRINT("Enter sec_cleanup_out: sce[%d]\n", sec_indx);
  log_perf(LOG_DATA_MANAGER, PERF_DM_ARRANGE_FLASH_BEGIN,0,0);
  sec_to_sec(p_data, sec_indx);
  log_perf(LOG_DATA_MANAGER, PERF_DM_ARRANGE_FLASH_END,2,sec_indx);
  exit_ticks = mtos_ticks_get();
  if(exit_ticks - enter_ticks > 100)
  {
    DM_PRINT("sec_cleanup_out spend long time %d ticks\n",
      exit_ticks - enter_ticks);
  }

  section_show(p_data, "Exit sec_cleanup_out");
}

static void sec_cleanup_overdru(dm_var_t *p_data, u32 sec_indx)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;
  u32 addr = p_sec_mgr->sec_base_addr + sec_indx * FLASH_SECTION_SIZE;
  u32 enter_ticks = mtos_ticks_get();
  u32 exit_ticks = 0;

  DM_PRINT("Enter sec_cleanup_overdru:sce[%d], addr[0x%x]\n", sec_indx, addr);

  data_erase_v2(p_data->p_charsto_dev, addr, 1);

  //set status
  sec_set_status(p_data, sec_indx, SEC_CLEAN, p_sec->header.block_id);

  exit_ticks = mtos_ticks_get();
  if(exit_ticks - enter_ticks > 500)
  {
    DM_ERROR("sec_cleanup_overdru spend long time %d ticks\n",
      exit_ticks - enter_ticks);
  }

  section_show(p_data, "Exit sec_cleanup_overdru");
}

static void sec_cleanup_full(dm_var_t *p_data, u32 sec_indx)
{
  section_mgr_t *p_sec_mgr = &p_data->dm_info.sec_mgr;
  section_info_t *p_sec = p_sec_mgr->p_sec_arry + sec_indx;

  DM_PRINT("Enter sec_cleanup_full: sce[%d]\n", sec_indx);
  //set the head info
  sec_set_status(p_data, sec_indx, SEC_STREAM_OUT, p_sec->header.block_id);

  //do clean
  sec_cleanup_out(p_data, sec_indx);
  section_show(p_data, "Exit sec_cleanup_full");
}

static void sec_check_clean(dm_var_t *p_data)
{
  section_mgr_t *p_sec_mgr = NULL;
  sec_status_t sec_st = SEC_CLEAN;
  u32 total_sec = 0;
  u32 i = 0;

  MT_ASSERT(p_data != NULL);
  p_sec_mgr = &p_data->dm_info.sec_mgr;
  total_sec = p_sec_mgr->total_section_num;


  //first find the SEC_STREAM_OVERDUE
  //this status means: the block is overdue
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    if(SEC_STREAM_OVERDUE == sec_st)
    {
      log_perf(LOG_DATA_MANAGER, PERF_DM_ARRANGE_FLASH_BEGIN,0,0);

      dm_lock_v2(p_data);
      DM_DBG_MUTEX("start check_block: SEC_STREAM_OVERDUE sec[%d]\n", i);
      sec_cleanup_overdru(p_data, i);
      DM_DBG_MUTEX("end check_block: SEC_STREAM_OVERDUE sec[%d]\n", i);
      dm_unlock_v2(p_data);

      log_perf(LOG_DATA_MANAGER, PERF_DM_ARRANGE_FLASH_END,1,i);
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
      dm_lock_v2(p_data);
      DM_DBG_MUTEX("start check_block: SEC_STREAM_OUT sec[%d]\n", i);
      sec_cleanup_out(p_data, i);
      DM_DBG_MUTEX("end check_block: SEC_STREAM_OUT sec[%d]\n", i);
      dm_unlock_v2(p_data);
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
        dm_lock_v2(p_data);
        DM_DBG_MUTEX("start check_block: SEC_STREAM_IN or USED sec_idx[%d]\n", i);
        sec_cleanup_full(p_data, i);
        DM_DBG_MUTEX("end check_block: SEC_STREAM_IN or USED sec[%d]\n", i);
        dm_unlock_v2(p_data);
        return;
      }
    }
  }

  //need check the write node?  //fix me???
}

//quick find a idle sec
static BOOL sec_check_clean_quick(dm_var_t *p_data, u8 block_id)
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
  p_sec_mgr = &p_data->dm_info.sec_mgr;
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
      sec_cleanup_overdru(p_data, i);
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
      sec_cleanup_overdru(p_data, i);
      return TRUE;
    }
  }

  //NOTE!!!!!!!, can't check the SEC_STREAM_OUT, because the sec used by
  //sec_check_clean in lowest prio-task

  //third find the SEC_USED
  //this status means: all of nodes in the block is invalid, erase directly
  for(i = 0; i < total_sec; i++)
  {
    sec_st = p_sec_mgr->p_sec_arry[i].header.status;
    sec_block_id = p_sec_mgr->p_sec_arry[i].header.block_id;
    valid_len = p_sec_mgr->p_sec_arry[i].valid_len;
    use_len = p_sec_mgr->p_sec_arry[i].cur_sec_addr - SEC_HEAD_SIZE;
    if((sec_block_id == block_id) && (SEC_USED == sec_st) && (use_len > valid_len))
    {
      DM_PRINT("check_block_q: SEC_USED sec[%d]\n", i);
      //don't need task lock, because the abnormity process is locked
      sec_to_sdram(p_data, i);
      return TRUE;
    }
  }

  return FALSE;
}

static BOOL sec_abnormity_process(dm_var_t *p_data, u32 *p_new_sec_idx,
  u8 block_id)
{
  BOOL ret = FALSE;
  BOOL found_ret = FALSE;

  DM_ERROR("enter sec_abnormity_process\n");
  section_show(p_data, "enter sec_abnormity_process");

  ret = sec_check_clean_quick(p_data, block_id);
  if(ret)
  {
    found_ret = sec_find_new(p_data, p_new_sec_idx, block_id, SEC_STREAM_IN);
    DM_DBG_MUTEX("new_sec_idx %d\n", *p_new_sec_idx);
    if(!found_ret)
    {
      section_show(p_data, "exit sec_abnormity_process");
      DM_ERROR("exit clean_quick, but can't find an idle section\n");
      sec_hardware_error(p_data, 0);
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
static BOOL sec_restore(dm_var_t *p_data)
{
  dm_global_info_t *p_dm_info = &p_data->dm_info;
  section_mgr_t *p_sec_mgr = &p_dm_info->sec_mgr;
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

  for(i = 0; i < total_sec; i++)
  {
    p_sec = p_sec_mgr->p_sec_arry + i;
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
        charsto_read(p_data->p_charsto_dev, sec_flash_addr + offset,
          (u8 *)(&header), NODE_HEADER_SIZE);

        if(ERASE_DATA == header.value)  //find invalid data, means read finish
        {
          DM_PRINT("restore block[0x%x] section[%d], load[0x%x], end 0x%x\n",
            block_id, _cur, p_sec->valid_len, offset);
          p_sec->cur_sec_addr = offset;
          break;
        }
        step = (header.length * p_data->align_bytes);

        //read a node header
        charsto_read(p_data->p_charsto_dev, sec_flash_addr + offset + step - 1,
          &intact_flag, 1);

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
        else if(step > DM_MAX_NODE_LEN)
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
          DM_DETAIL("find old node: id[%d], block[0x%x], addr[0x%x],"
            " active[%d]\n", header.id, block_id, sec_iw_addr + offset,
            p_node[header.id].active);
        }
        //the node is unintact
        else if(NODE_INTACT_FLAG != intact_flag)
        {
          DM_ERROR("ERROR found a unintact node!!! header_id[%d], "
            "flag[0x%x], block[0x%x], node_num[%d], addr[0x%x]\n",
            header.id, intact_flag, block_id, p_blk_mgr[block_index].node_num,
            sec_flash_addr + offset + step - 1);

          //delete current bad node.
          header.active = 0;
          sec_del_node(p_data, _cur, offset, (u8 *)&header, NODE_HEADER_SIZE,
            header.length * p_data->align_bytes);

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
            DM_ERROR("ERROR!! found reappear node: id[%d], block[0x%x], "
              "old addr[0x%x], now [0x%x]\n",
              header.id, block_id, p_node[header.id].node_iw_addr,
              sec_iw_addr + offset);

            //delete old node.
            sec_index = p_node[header.id].node_iw_addr / FLASH_SECTION_SIZE;
            sec_offset = p_node[header.id].node_iw_addr % FLASH_SECTION_SIZE;

            header.active = 0;
            sec_del_node(p_data, sec_index, sec_offset, (u8 *)&header, NODE_HEADER_SIZE,
              header.length * p_data->align_bytes);

            DM_ERROR("ERROR!! dispose repeat node\n");
          }

          p_node[header.id].node_iw_addr = sec_iw_addr + offset;
          p_node[header.id].active = 1;
          p_sec->valid_len += step; //valid node
          DM_DETAIL("restore good node: id[%d], block[0x%x], addr[0x%x], size[%d]\n",
            header.id, block_id, sec_iw_addr + offset, step);
        }
        offset += step;
      }
      p_sec->cur_sec_addr = offset;
      DM_DETAIL("sec info : id %d,b_id %d,  cur %d, vaild %d\n", p_sec->header.sec_id,
        p_sec->header.block_id, p_sec->cur_sec_addr, p_sec->valid_len);
    }
  }

  //free buffer
  mtos_free(p_sort_normal);
  mtos_free(p_sort_write);

  section_show(p_data, "sec restore done");
  return TRUE;
}

/*!
  reconstruct IW block node info
  \return FALSE if fail, or TRUE if success
  */
static BOOL sec_restore_ex(dm_var_t *p_data, u8 block_id)
{
  dm_global_info_t *p_dm_info = &p_data->dm_info;
  section_mgr_t *p_sec_mgr = &p_dm_info->sec_mgr;
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

  for(i = 0; i < total_sec; i++)
  {
    p_sec = p_sec_mgr->p_sec_arry + i;
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
    p_sec = p_sec_mgr->p_sec_arry + _cur;

    //only restore this block id data.
    if(block_id != p_sec->header.block_id)
      continue;

    offset = SEC_HEAD_SIZE; //section offset. skip header
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
        charsto_read(p_data->p_charsto_dev, sec_flash_addr + offset,
          (u8 *)(&header), NODE_HEADER_SIZE);

        if(ERASE_DATA == header.value)  //find invalid data, means read finish
        {
          DM_PRINT("restore block[0x%x] section[%d], load[0x%x], end 0x%x\n",
            block_id, _cur, p_sec->valid_len, offset);
          p_sec->cur_sec_addr = offset;
          break;
        }

        step = (header.length * p_data->align_bytes);

        //read a node header
        charsto_read(p_data->p_charsto_dev, sec_flash_addr + offset + step - 1,
          &intact_flag, 1);

        //unexpected data, skip it
        if(header.id >= p_blk_mgr[block_index].node_num)
        {
          DM_ERROR("ERROR !!! bigger id: header_id[%d], block[0x%x],"
            "node_num[%d], addr[0x%x]\n",
            header.id, block_id, p_blk_mgr[block_index].node_num,
            sec_flash_addr + offset);
        }
        else if(step > DM_MAX_NODE_LEN)
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
          DM_DETAIL("find old node: id[%d], block[0x%x], addr[0x%x],"
            " active[%d]\n", header.id, block_id, sec_iw_addr + offset,
            p_node[header.id].active);
        }
        //the node is unintact
        else if(NODE_INTACT_FLAG != intact_flag)
        {
          DM_ERROR("ERROR found a unintact node!!! header_id[%d], "
            "flag[0x%x], block[0x%x], node_num[%d], addr[0x%x]\n",
            header.id, intact_flag, block_id, p_blk_mgr[block_index].node_num,
            sec_flash_addr + offset + step - 1);

          //delete current bad node.
          header.active = 0;
          sec_del_node(p_data, _cur, offset, (u8 *)&header, NODE_HEADER_SIZE,
            header.length * p_data->align_bytes);
        }
        //good node
        else
        {
          if(1 == p_node[header.id].active) //the active node was found
          {
            DM_ERROR("ERROR!! found reappear node: id[%d], block[0x%x], "
              "old addr[0x%x], now [0x%x]\n",
              header.id, block_id, p_node[header.id].node_iw_addr,
              sec_iw_addr + offset);

            //delete old node.
            sec_index = p_node[header.id].node_iw_addr / FLASH_SECTION_SIZE;
            sec_offset = p_node[header.id].node_iw_addr % FLASH_SECTION_SIZE;

            header.active = 0;
            sec_del_node(p_data, sec_index, sec_offset, (u8 *)&header, NODE_HEADER_SIZE,
              header.length * p_data->align_bytes);
          }

          p_node[header.id].node_iw_addr = sec_iw_addr + offset;
          p_node[header.id].active = 1;
          p_sec->valid_len += step; //valid node
          DM_DETAIL("restore good node: id[%d], block[0x%x], addr[0x%x], size[%d]\n",
            header.id, block_id, sec_iw_addr + offset, step);
        }
        offset += step;
      }
      p_sec->cur_sec_addr = offset;
      DM_DETAIL("sec info : id %d,b_id %d,  cur %d, vaild %d\n", p_sec->header.sec_id,
        p_sec->header.block_id, p_sec->cur_sec_addr, p_sec->valid_len);
    }
  }

  //free buffer
  mtos_free(p_sort_normal);
  mtos_free(p_sort_write);
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
              u16 node_id)
{
  node_header_t header = {{0}};
  u32 flash_addr = 0;
  u32 iw_addr = 0;
  u32 sec_indx = 0;
  u32 sec_offset = 0;

  log_perf(LOG_FLASH_PROTECT,DM_WRITE_DELETE_NODE,0,0);
  //del the link
  if(p_node->node_iw_addr != 0)
  {
    flash_addr = p_data->dm_info.iw_base_addr + p_node->node_iw_addr;
    iw_addr = p_node->node_iw_addr;
    sec_indx = iw_addr / FLASH_SECTION_SIZE;
    sec_offset = iw_addr % FLASH_SECTION_SIZE;
    p_node->node_iw_addr = 0;
  }
  else
  {
    //DM_PRINT("Task reenter same node. do nothing.\n");
    return;
  }

  charsto_read(p_data->p_charsto_dev,
    flash_addr, (u8 *)&header, NODE_HEADER_SIZE);

  if(header.id != node_id)
  {
    DM_ERROR("error addr[0x%x], node_id[%d], len[%d], act[%d], input_id[%d]\n",
      iw_addr, header.id, header.length * p_data->align_bytes,
      header.active, node_id);
  }
  sec_hardware_error(p_data, header.id == node_id);

  if(0 == header.active)  //it's deleted, link to old data
  {
    DM_PRINT("This node is deleted: node_id %d, addr 0x%x\n",
      node_id, flash_addr);
  }
  else
  {
    header.active = 0;  //delete it

    sec_del_node(p_data, sec_indx, sec_offset, (u8 *)&header,
      NODE_HEADER_SIZE, header.length * p_data->align_bytes);
  }
}

static dm_ret_t write_node_to_flash(dm_var_t *p_data, u8 block_id, u16 node_id,
                  u8 *p_buffer, u16 len)
{
  s32 b_index = ERR_FAILURE;
  block_mgr_t *p_block = NULL;
  section_info_t *p_sec = NULL;
  node_logic_info_t *p_node_logic = NULL;
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  node_header_t *p_header = (node_header_t *)rw_buf;
  u32 sec_addr = 0; //base of cur section
  u32 iw_addr = 0;  //base of iw block base
  u16 write_len = ALIGN(p_data->align_bytes, (len + NODE_HEADER_SIZE + NODE_FLAG_SIZE));
  u32 new_section_idx = 0;
  BOOL found_next = FALSE;
  u32 enter_ticks = mtos_ticks_get();
  u32 exit_ticks = 0;

  log_perf(LOG_FLASH_PROTECT,DM_WRITE_NODE_TO_FLASH,0,0);
  b_index = find_block_index(p_data, block_id);
  MT_ASSERT(b_index != ERR_FAILURE); //found block index.

  p_block = p_data->dm_info.p_blk_mgr + b_index;

  p_sec = p_data->dm_info.sec_mgr.p_sec_arry;

  MT_ASSERT(NULL != p_buffer);
  MT_ASSERT(len > 0);
  MT_ASSERT(BLOCK_TYPE_IW == p_block->type);
  MT_ASSERT(write_len <= DM_MAX_NODE_LEN);

  p_header->id = node_id;
  p_header->length = write_len / p_data->align_bytes;

  memcpy(rw_buf + NODE_HEADER_SIZE, p_buffer, len);
  rw_buf[write_len - 1] = NODE_INTACT_FLAG;

  p_node_logic = p_block->p_node_logic_info + p_header->id;

  //invalid sec. must be first enter
  if(p_block->sec_index > p_data->dm_info.sec_mgr.total_section_num)
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
          DM_PRINT("DM_FULL write node 1.\n");
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
        if((!found_next) || (p_sec[new_section_idx].cur_sec_addr + write_len > FLASH_SECTION_SIZE))
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
  iw_addr = sec_write_node(p_data, p_block->sec_index, rw_buf, write_len);


  //del the old data
  //why to check the addr but active flag?? because the "active"
  // flag the logic status, so we must del the last node.
  // But something need care, the last node maybe deactived in the section.
  // So we maybe delete a deactived node, it's not a trouble.
  if(p_node_logic->node_iw_addr != 0)
  {
    write_del_node(p_data, p_node_logic, node_id);
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
  dm_var_t *p_priv = (dm_var_t *)p_data;

  while(1)
  {
    sec_check_clean(p_priv);

    mtos_task_sleep(100);

#ifdef FLASH_PROTECT_DEBUG
    {
      u8 read_buf[1] = {0};
      u8 write_buf[1] = {0};
      u8 read_write_buf[1] = {0};
      u8 cmp_buf[1] = {0};

      memset(read_buf,0x00,1);
      memset(write_buf,0x00,1);
      memset(read_write_buf,0x00,1);

      charsto_read(p_priv->p_charsto_dev, 0xC,read_buf, 1);
      //OS_PRINTF("===#debug: read_buf = [%s]===\n",read_buf);

      //charsto_unprotect_all(p_priv->p_charsto_dev);
      write_buf[0] = read_buf[0] & 0x00;
      charsto_writeonly(p_priv->p_charsto_dev, 0xC,write_buf, 1);
      //OS_PRINTF("===#debug: write_buf = [%s]===\n",write_buf);

      cmp_buf[0] = read_buf[0] & write_buf[0];
      //OS_PRINTF("===#debug: cmp_buf = [%s]===\n",cmp_buf);

      charsto_read(p_priv->p_charsto_dev, 0xC,read_write_buf, 1);
      //OS_PRINTF("===#debug: read_write_buf = [%s]===\n",read_write_buf);

      if(memcmp(read_buf,read_write_buf,1) == 0)
      {
        OS_PRINTF("===#debug: flash is protected, success!===\n");
      }
      else if(memcmp(cmp_buf,read_write_buf,1) == 0)
      {
        OS_PRINTF("===#debug: flash is unprotected, fail!===\n");
      }
      else
      {
        MT_ASSERT(0);
      }
    }
#endif
  }
}

static void dm_restore_v2(void *p_data)
{
#ifndef BUNDLE_RESTORE
  dm_var_t *p_dm_var = NULL;

  log_perf(LOG_FLASH_PROTECT,DM_RESTORE_V2,0,0);
  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  sec_init(p_dm_var, p_dm_var->dm_info.iw_base_addr,
    p_dm_var->dm_info.total_iw_sec_num);
  sec_restore(p_dm_var);
#endif
}

/********************************************************
                    Internal functions
 ********************************************************/

/*!
  read data from flash block to ram buffer

  \param[in] block_id Block ID
  \param[in] node_id node id for IW block , meaningless for RO block
  \param[in] offset Offset in block for RO block , offset in node for IW block
  \param[in] length read length
  \param[in] pbuffer Pointer to store data
  \return 0 if fail, actual length if success
  */
static u32 dm_read_v2(void *p_data, u8 block_id, u16 node_id, u16 offset,
  u32 length, u8 *p_buffer)
{
  u32 read_len = 0;
  u32 node_addr = 0;
  s32 b_index = 0;
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  u8 block_type = BLOCK_TYPE_UNKNOWN;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  MT_ASSERT(p_buffer != NULL);
  MT_ASSERT(length > 0);

  b_index = find_block_index(p_dm_var, block_id);

  if(ERR_FAILURE == b_index)
  {
    return 0;
  }

  block_type = p_dm_info->p_blk_mgr[b_index].type;

  if((BLOCK_TYPE_RO == block_type) ||(BLOCK_TYPE_IDENTITY == block_type))
  {
    node_addr = p_dm_info->p_blk_mgr[b_index].base_addr + offset;
    if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, node_addr,
          p_buffer, length))
    {
      read_len = length;
    }
  }
  else if(BLOCK_TYPE_IW == block_type) //increasing write block
  {
    block_mgr_t *p_block = p_dm_info->p_blk_mgr + b_index;
    node_logic_info_t *p_node = p_block->p_node_logic_info + node_id;

    if((node_id >= p_block->node_num) || (0 == p_node->active))
    {
      return 0; //invalid node id
    }
    MT_ASSERT((offset + length) <= MAX_NODE_LEN);
    read_len = offset + length;
    //calculate read addr,ignore head and locate to offset start
    node_addr = p_dm_info->iw_base_addr
        + p_node->node_iw_addr + NODE_HEADER_SIZE;

    memset(p_buffer, 0x0, length);
    //only increasing write block,
    //have a case :  read_len + node_addr > flash_size
    if((read_len + node_addr) > p_dm_info->total_iw_sec_max_size)
    {
      read_len = p_dm_info->total_iw_sec_max_size - node_addr;
    }

    if((p_block->cache_size > 0) && (CACHE_EMPTY == p_node->cache_state))
    {
      //read the cache
      if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, node_addr,
        p_block->p_cache + p_block->cache_size * node_id, p_block->cache_size))
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
      if(p_block->cache_size >0)
      {
        memcpy(rw_buf, p_block->p_cache + p_block->cache_size * node_id,
          p_block->cache_size);
        read_len -= p_block->cache_size;
        node_addr += p_block->cache_size;
      }

      if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, node_addr,
        rw_buf + p_block->cache_size, read_len))
      {
        memcpy(p_buffer, rw_buf + offset, length);
        read_len = length;
      }
      else
      {
        MT_ASSERT(0);
      }
    }
  }
  else
  {
    DM_ERROR("block_id %d, block_type %d\n", block_id, block_type);
    MT_ASSERT(0);
  }

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
static dm_ret_t dm_write_node_v2(void *p_data, u8 block_id,
u16 *p_node_id, u8 *p_buffer, u16 len)
{
  block_mgr_t *p_block = NULL;
  s32 index = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;
  dm_ret_t  dm_ret = DM_FAIL;

  log_perf(LOG_FLASH_PROTECT,DM_WRITE_NODE_V2,0,0);
  MT_ASSERT(p_data != NULL);
  MT_ASSERT(p_buffer != NULL);
  MT_ASSERT(len > 0);

  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  p_block = p_dm_info->p_blk_mgr + index;
  if((ERR_FAILURE == index) || (p_block->type == BLOCK_TYPE_RO))
  {
    return DM_FAIL;
  }

  if(UNKNOWN_ID == *p_node_id)  // add new node
  {
    dm_lock_v2(p_data);
    DM_DBG_MUTEX("start find free id, block id %d\n", block_id);
    find_free_id(p_dm_var, index);
    DM_DBG_MUTEX("end find free id, block id %d\n", block_id);
    dm_unlock_v2(p_data);
    if(p_block->free_id >= p_block->node_num)
    {
      DM_PRINT("%s:dm if full! DM_FULL write node\n", __FUNCTION__);
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

  if(len <= p_block->cache_size) //write cache
  {
    dm_lock_v2(p_data);
    DM_DBG_MUTEX("start write cache, block id %d\n", block_id);
    memcpy(p_block->p_cache + p_block->cache_size * (*p_node_id),
        p_buffer, len);
    p_block->p_node_logic_info[*p_node_id].cache_state = CACHE_ASYNC;

    if(p_block->p_node_logic_info[*p_node_id].unsync_cache_size < len)  //record the max
    {
      p_block->p_node_logic_info[*p_node_id].unsync_cache_size = len;
    }
    DM_DBG_MUTEX("end write cache, block id %d\n", block_id);
    dm_unlock_v2(p_data);
    return DM_SUC;
  }
  else  //write flash
  {
    dm_lock_v2(p_data);
    DM_DBG_MUTEX("start write flash, block id %d, node_id %d\n", block_id, *p_node_id);
    dm_ret = write_node_to_flash(p_data, block_id, *p_node_id, p_buffer, len);
    DM_DBG_MUTEX("start write flash, block id %d, node_id %d\n", block_id, *p_node_id);
    dm_unlock_v2(p_data);
    return dm_ret;
  }
}

/*!
  Delete a node, only for IW block

  \param[in] block_id Block ID
  \param[in] node_id Node ID
  \param[in] b_sync TRUE to write flash at once
  \return DM_SUC if success, else fail
  */
static dm_ret_t dm_del_node_v2(void *p_data, u8 block_id, u16 node_id,
    BOOL b_sync)
{
  u32 index = 0;
  block_mgr_t *p_block = NULL;
  node_logic_info_t *p_node = NULL;
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;

  log_perf(LOG_FLASH_PROTECT,DM_DELETE_NODE_V2,0,0);
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
    dm_lock_v2(p_data);
    DM_DBG_MUTEX("start reset iw block id %x\n", block_id, node_id);
    write_del_node(p_dm_var, p_node, node_id);
    dm_unlock_v2(p_data);
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
static dm_ret_t dm_active_node_v2(void *p_data, u8 block_id, u16 node_id)
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
static void dm_check_block_v2(void *p_data)
{
  dm_var_t *p_para = (dm_var_t *)p_data;

  if(!p_para->is_open_monitor)
  {
    sec_check_clean(p_para);
  }
}

/*!
  Declare all IW node ID are invalid

  \param[in] p_data   : dm_var_t.
  \param[in] block_id : Block ID
  \param[in] is_sync  : sync delete flash data.
  \return None
  */
static dm_ret_t dm_reset_iw_block_v2(void *p_data, u8 block_id, BOOL is_sync)
{
  u32 index = 0;
  u32 i = 0;
  dm_global_info_t  *p_dm_info = NULL;
  node_logic_info_t *p_node    = NULL;
  dm_var_t          *p_dm_var  = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = p_data;
  p_dm_info = &p_dm_var->dm_info;

  index = find_block_index(p_dm_var, block_id);
  if(ERR_FAILURE == index)
  {
    return DM_FAIL;
  }

  dm_lock_v2(p_data);
  DM_DBG_MUTEX("start reset iw block id %x\n", block_id);

  for(i = 0; i < p_dm_info->p_blk_mgr[index].node_num; i ++)
  {
    p_node = p_dm_info->p_blk_mgr[index].p_node_logic_info + i;

    p_node->active = 0;

    if(is_sync)
    {
      write_del_node(p_dm_var, p_node, i);
    }
  }

  p_dm_info->p_blk_mgr[index].free_id = 0;

  DM_DBG_MUTEX("end reset iw block id %x\n", block_id);
  dm_unlock_v2(p_data);
  return DM_SUC;
}

static dm_ret_t dm_restore_cache_v2(void *p_data, u8 block_id, u16 node_id)
{
  s32 block_indx = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  log_perf(LOG_FLASH_PROTECT,DM_RESTORE_CACHE_V2,0,0);
  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  block_indx = find_block_index(p_dm_var, block_id);
  MT_ASSERT(block_indx != ERR_FAILURE);
  p_dm_info->p_blk_mgr[block_indx].p_node_logic_info[node_id].cache_state = CACHE_EMPTY;
  return DM_SUC;
}

static dm_ret_t dm_flush_cache_v2(void *p_data, u8 block_id, u16 node_id)
{
  s32 block_indx = 0;
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;
  block_mgr_t *p_block = NULL;
  node_logic_info_t *p_node = NULL;
  u8 rw_buf[DM_MAX_NODE_LEN] = {0};
  u16 rw_len = DM_MAX_NODE_LEN;
  u8 *p_cache_buf = NULL;

  log_perf(LOG_FLASH_PROTECT,DM_FLUSH_CACHE_V2,0,0);
  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  block_indx = find_block_index(p_dm_var, block_id);
  MT_ASSERT(block_indx != ERR_FAILURE);

  p_block = p_dm_info->p_blk_mgr + block_indx;
  p_node = p_block->p_node_logic_info + node_id;

  if(0 == p_block->cache_size)
  {
    return DM_SUC;
  }

  p_cache_buf = p_block->p_cache + p_block->cache_size * node_id;

  dm_lock_v2(p_data);
  DM_DBG_MUTEX("start flush cache\n");

  //the current node isn't exsit, add it
  if(0 == p_node->active)
  {
    write_node_to_flash(p_data, block_id, node_id, p_cache_buf,
      p_node->unsync_cache_size);
  }
  else
  {
    if(CACHE_ASYNC == p_node->cache_state)
    {
      u32 node_addr = p_dm_info->iw_base_addr + p_node->node_iw_addr;
      s32 cmp_rslt  = 0;

      MT_ASSERT(p_node->unsync_cache_size > 0);
      MT_ASSERT(p_node->unsync_cache_size <= p_block->cache_size);

      if(node_addr + rw_len > p_dm_info->total_iw_sec_max_size)
      {
        rw_len = p_dm_info->total_iw_sec_max_size - node_addr;
      }

      charsto_read(p_dm_var->p_charsto_dev, node_addr, rw_buf, rw_len);

      cmp_rslt = memcmp(rw_buf + NODE_HEADER_SIZE,
      p_cache_buf, p_node->unsync_cache_size);
      if(cmp_rslt != 0)
      {
        node_header_t *p_head = (node_header_t *)rw_buf;
        memcpy(rw_buf + NODE_HEADER_SIZE, p_cache_buf,
          p_node->unsync_cache_size);
        write_node_to_flash(p_data, block_id, node_id,
          rw_buf + NODE_HEADER_SIZE,
          p_head->length * p_dm_var->align_bytes - NODE_HEADER_SIZE - NODE_FLAG_SIZE);
      }
      else //not have different data.
      {
        p_node->cache_state = CACHE_SYNC;
        p_node->unsync_cache_size = 0;
      }
    }
  }

  DM_DBG_MUTEX("end flush cache\n");
  dm_unlock_v2(p_data);

  return DM_SUC;
}

static dm_ret_t dm_set_cache_v2(void *p_data, u8 block_id, u16 cache_size)
{
  s32               block_indx   = 0;
  u32               index        = 0;
  dm_var_t         *p_dm_var     = NULL;
  dm_global_info_t *p_dm_info    = NULL;
  block_mgr_t      *p_block_info = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *) p_data;
  p_dm_info = &p_dm_var->dm_info;

  block_indx = find_block_index(p_dm_var, block_id);
  MT_ASSERT(block_indx != ERR_FAILURE);

  p_block_info = p_dm_info->p_blk_mgr + block_indx;

  //Cache protection machanism
  //Avoid allocate cache for one table block for more times
  if(p_block_info->type == BLOCK_TYPE_RO || cache_size == 0)
  {
    return DM_FAIL;
  }

  //reset cache. so must free prev cache buffer.
  if(p_block_info->p_cache != NULL)
  {
    mtos_free(p_block_info->p_cache);
  }

  p_block_info->p_cache = mtos_malloc(p_block_info->node_num * cache_size);
  MT_ASSERT(NULL != p_block_info->p_cache);
  memset(p_block_info->p_cache, 0x0, p_block_info->node_num * cache_size);
  p_block_info->cache_size = cache_size;

  //init cache state.
  for(index = 0; index < p_block_info->node_num; index++)
  {
    p_block_info->p_node_logic_info[index].cache_state = CACHE_EMPTY;
  }

  return DM_SUC;
}

static void dm_set_header_v2(void *p_data, u32 addr)
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
  u8 temp[100];

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &temp_info;
  log_perf(LOG_DATA_MANAGER, PERF_DM_SETHEADER_BEGIN,0,0);
  //read dmh basic info(12B): sdram&flash size, block num, each block head size
  charsto_read(p_dm_var->p_charsto_dev, start_addr + DM_TAG_SIZE,
                        (u8 *)temp, DM_BASE_INFO_HEAD_LEN);

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
    DM_ERROR("much block num! addr :0x%x, total : %d blocks\n", start_addr, p_dm_info->block_num);
    MT_ASSERT(0);
  }

  if(p_dm_info->bh_size != sizeof(dmh_block_info_t))
  {
    DM_ERROR("struct size dismatch!\n");
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
    charsto_read(p_dm_var->p_charsto_dev, header_addr
      , (u8 *)temp, sizeof(dmh_block_info_t));

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
    
    DM_PRINT("name %s version 0x%8x time 0x%8x,block id=0x%x,type=0x%x,node=0x%x,"
      "addr=0x%x,size=0x%x, crc=0x%x\n", dmh.name, dmh.version, dmh.time, dmh.id,
      dmh.type, dmh.node_num, dmh.base_addr + start_addr, dmh.size, dmh.crc);

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
    else if(BLOCK_TYPE_DIRECT_RW == dmh.type)
    {
      block_mgr_t *p_direct_blk = p_dm_var->dm_info.p_blk_mgr + index;

      MT_ASSERT(0 == (p_direct_blk->size % FLASH_SECTION_SIZE));

      if(p_dm_var->dm_info.p_erase_buf == NULL)
      {
        DM_PRINT("only alloc once. block id=0x%x,type=0x%x\n", dmh.id, dmh.type);
        p_dm_var->dm_info.p_erase_buf = (u8 *)mtos_malloc(64*KBYTES);
        MT_ASSERT(p_dm_var->dm_info.p_erase_buf != NULL);
        memset(p_dm_var->dm_info.p_erase_buf, 0x0, 64*KBYTES);
      }
    }
  }

  if(total_iw_sec_num > 0) //has the iw block
  {
    p_dm_var->dm_info.iw_base_addr = min_iw_addr;
    p_dm_var->dm_info.total_iw_sec_num = total_iw_sec_num;
    p_dm_var->dm_info.total_iw_sec_max_size = min_iw_addr +  \
                                        total_iw_sec_num * FLASH_SECTION_SIZE;
#ifdef BUNDLE_RESTORE
    sec_init(p_dm_var, p_dm_var->dm_info.iw_base_addr,
      p_dm_var->dm_info.total_iw_sec_num);
    sec_restore(p_dm_var);
#endif
  }
  log_perf(LOG_DATA_MANAGER, PERF_DM_SETHEADER_BEGIN,0,0);
}

static void dm_set_header_v2_ex(void *p_data, u32 addr, dm_set_block_t block_id)
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
  u8 temp[100];

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &temp_info;

  //read dmh basic info(12B): sdram&flash size, block num, each block head size
  charsto_read(p_dm_var->p_charsto_dev, start_addr + DM_TAG_SIZE,
                        (u8 *)temp, DM_BASE_INFO_HEAD_LEN);

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

  DM_PRINT("The DMH at 0x%x,blocks total %d\n", start_addr, p_dm_info->block_num);
  //start_addr += DM_BASE_INFO_HEAD_LEN;
  old_num = p_dm_var->dm_info.block_num;
  for(i = 0;  i < p_dm_info->block_num; i++)
  {
    u32 index = i + old_num;
    u32 header_addr = start_addr + DM_TAG_SIZE + DM_BASE_INFO_HEAD_LEN +
      i * sizeof(dmh_block_info_t);

    p_dm_var->dm_info.block_num++;
    charsto_read(p_dm_var->p_charsto_dev, header_addr
      , (u8 *)temp, sizeof(dmh_block_info_t));

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

    if((BLOCK_TYPE_IW == dmh.type))//init iw info
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

      if(dmh.id == block_id)
      {
        p_iw_blk->p_node_logic_info = mtos_malloc(
          dmh.node_num * sizeof(node_logic_info_t));
        MT_ASSERT(NULL != p_iw_blk->p_node_logic_info);
        //set default first
        //set node to 0 means: the node is deactived, the cache is empty
        memset(p_iw_blk->p_node_logic_info, 0,
            dmh.node_num * sizeof(node_logic_info_t));
      }
      else
      {
        p_iw_blk->p_node_logic_info = NULL;
      }
    }
  }

  if(total_iw_sec_num > 0) //has the iw block
  {
    p_dm_var->dm_info.iw_base_addr = min_iw_addr;
    p_dm_var->dm_info.total_iw_sec_num = total_iw_sec_num;
    p_dm_var->dm_info.total_iw_sec_max_size = min_iw_addr +  \
                                        total_iw_sec_num * FLASH_SECTION_SIZE;
    sec_init(p_dm_var, p_dm_var->dm_info.iw_base_addr,
      p_dm_var->dm_info.total_iw_sec_num);
    sec_restore_ex(p_dm_var, block_id);
  }
}


///////////////////////////////fix me--->

/*!
  get block node num

  \param[in] p_data data manager handle
  \param[in] block_id

  \return current block node num
  */
static u16 dm_get_blk_node_num_v2(void * p_data, u8 block_id)
{
  dm_var_t *p_dm_var = NULL;
  dm_global_info_t *p_dm_info = NULL;
  block_mgr_t *p_block = NULL;
  s32 block_indx = 0;

  MT_ASSERT(p_data != NULL);

  p_dm_var = (dm_var_t *)p_data;
  p_dm_info = &p_dm_var->dm_info;

  block_indx = find_block_index(p_dm_var, block_id);
  MT_ASSERT(block_indx != ERR_FAILURE);

  p_block = p_dm_info->p_blk_mgr + block_indx;

  return p_block->node_num;
}

/*!
  get all node id of block.
  */
static u32 dm_get_node_id_list_v2(void *p_data, u8 block_id,
  u16 *p_node_id_list, u32 total_list)
{
  u32 node_id = 0;
  u32 id_idx = 0;
  s32 index = 0;
  block_mgr_t *p_block_info = NULL;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  MT_ASSERT(p_data != NULL);
  p_dm_var = (dm_var_t *)p_data;

  MT_ASSERT(&p_dm_var->dm_info != NULL);
  p_dm_info = &p_dm_var->dm_info;

  MT_ASSERT(total_list > 0 && p_node_id_list != NULL);

  index = find_block_index(p_dm_var, block_id);
  MT_ASSERT(index != ERR_FAILURE);

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

  start = p_dm_info->p_blk_mgr[i].dm_header_addr;
  DM_PRINT("dm_get_block_header_32 addr: 0x%x\n", start);
  if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, start,
                              (u8 *)p_dm_head, sizeof(dmh_block_info_t)))
  {
    return DM_SUC;
  }
  else
  {
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

static u16 dm_get_max_node_len_32(void * p_data)
{
  return MAX_NODE_LEN;
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
static s32 dm_read_bl_block_32(void *p_handle, u32 bid, u32 offset,
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

  memset(p_buff, 0x0, len);

  for(i = 0; i < FLAHS_HEAD_MGR_UNIT_NUM; i ++)
  {
    if(SUCCESS != charsto_read(p_charsto_dev,
        start + i * FLASH_HEAD_MGR_UNIT_LENT,
          (u8 *)head, FLASH_HEAD_MGR_UNIT_LENT))
    {
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
    return ERR_FAILURE;

  len = (size - offset > len)? len : size - offset;
  if(SUCCESS != charsto_read(p_charsto_dev, base_addr + offset,
          p_buff, len))
  {
    return ERR_FAILURE;
  }
  else
  {
    return len;
  }
}

/********************************************************************
                    data manager direct functions
 *******************************************************************/

/*!
  direct read data from flash
  */
static u32 dm_direct_read_v2(void *p_data, u8 block_id, u32 offset, u32 len, u8 *p_buf)
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

  dm_lock_v2(p_data);
  DM_DBG_MUTEX("start direct read\n");
  if(SUCCESS == charsto_read(p_dm_var->p_charsto_dev, node_addr,
        p_buf, len))
  {
    read_len = len;
  }
  DM_DBG_MUTEX("start direct read\n");
  dm_unlock_v2(p_data);

  return read_len;
 }

/*!
  direct Write data into flash
  */
static dm_ret_t dm_direct_write_v2(void *p_data, u8 block_id, u32 offset,
u32 len, u8 *p_buf)
{
  block_mgr_t *p_block = NULL;
  u32 write_addr = 0;
  s32 index = 0;
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_var = NULL;

  log_perf(LOG_FLASH_PROTECT,DM_DIRECT_WRITE_V2,0,0);
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

  p_block = p_dm_info->p_blk_mgr + index;
  MT_ASSERT(BLOCK_TYPE_DIRECT_RW == p_block->type);

  if((offset + len) > p_block->size)
  {
    DM_ERROR("write len bigger than block surplus size, write_addr %d,"
        "len %d, bl_size %d, bl_id %d\n", write_addr, len, p_block->size, block_id);
    return DM_FULL;
  }

  dm_lock_v2(p_dm_var);
  DM_DBG_MUTEX("start direct write\n");
  write_addr = p_block->base_addr + offset;
  if(SUCCESS == data_writeonly_v2(p_dm_var->p_charsto_dev, write_addr, p_buf, len))
  {
    DM_DBG_MUTEX("end direct write\n");
    dm_unlock_v2(p_dm_var);
    return DM_SUC;
  }
  DM_DBG_MUTEX("end direct write\n");
  dm_unlock_v2(p_dm_var);
  return DM_FAIL;
}

/*!
  direct erase data into flash
  */
static dm_ret_t dm_direct_erase_v2(void *p_data, u8 block_id, u32 offset,
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

  log_perf(LOG_FLASH_PROTECT,DM_DIRECT_ERASER_V2,0,0);
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

  p_block = p_dm_info->p_blk_mgr + index;
  MT_ASSERT(BLOCK_TYPE_DIRECT_RW == p_block->type);

  if((offset + len) > p_block->size)
  {
    DM_ERROR("why erase size bigger than block size, offset %d, len %d, size %d,"
      "block_id %d\n",offset, len, p_block->size, block_id);
    MT_ASSERT(0);
  }

  dm_lock_v2(p_dm_var);
  DM_DBG_MUTEX("start direct earse\n");
  sec_off_num = offset / FLASH_SECTION_SIZE;
  sec_off_addr = offset - sec_off_num * FLASH_SECTION_SIZE;

  addr = p_block->base_addr + sec_off_num * FLASH_SECTION_SIZE;

  if(sec_off_addr + len <= FLASH_SECTION_SIZE)
  {
    charsto_read(p_dm_var->p_charsto_dev, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE);
    memset(p_dm_info->p_erase_buf + sec_off_addr, 0xFF, len);
    ret = data_erase_v2(p_dm_var->p_charsto_dev, addr, 1);
    MT_ASSERT(SUCCESS == ret);
    ret = data_writeonly_v2(p_dm_var->p_charsto_dev, addr, p_dm_info->p_erase_buf,
              FLASH_SECTION_SIZE);
    MT_ASSERT(SUCCESS == ret);
  }
  else if(sec_off_addr + len > FLASH_SECTION_SIZE)
  {
    //head
    charsto_read(p_dm_var->p_charsto_dev, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE);
    memset(p_dm_info->p_erase_buf + sec_off_addr, 0xFF, FLASH_SECTION_SIZE - sec_off_addr);
    ret = data_erase_v2(p_dm_var->p_charsto_dev, addr, 1);
    MT_ASSERT(SUCCESS == ret);
    ret = data_writeonly_v2(p_dm_var->p_charsto_dev, addr, p_dm_info->p_erase_buf,
              FLASH_SECTION_SIZE);
    MT_ASSERT(SUCCESS == ret);

    //middle
    addr += FLASH_SECTION_SIZE;
    len -= (FLASH_SECTION_SIZE - sec_off_addr);
    while(len >= FLASH_SECTION_SIZE)
    {
      ret = data_erase_v2(p_dm_var->p_charsto_dev, addr, 1);
      MT_ASSERT(SUCCESS == ret);

      len -= FLASH_SECTION_SIZE;
      addr += FLASH_SECTION_SIZE;
    }

    //tail
    charsto_read(p_dm_var->p_charsto_dev, addr, p_dm_info->p_erase_buf, FLASH_SECTION_SIZE);
    memset(p_dm_info->p_erase_buf, 0xFF, len);
    ret = data_erase_v2(p_dm_var->p_charsto_dev, addr, 1);
    MT_ASSERT(SUCCESS == ret);
    ret = data_writeonly_v2(p_dm_var->p_charsto_dev, addr, p_dm_info->p_erase_buf,
              FLASH_SECTION_SIZE);
    MT_ASSERT(SUCCESS == ret);
  }

  DM_DBG_MUTEX("end direct earse\n");
  dm_unlock_v2(p_dm_var);

  return DM_SUC;
}

/*!
  Init dm: get dm header info and restore IW block info
  \return None
  */
void dm_init_v2(dm_v2_init_para_t *p_para)
{
  dm_global_info_t *p_dm_info = NULL;
  charsto_device_t *p_charsto_dev = NULL;
  dm_var_t *p_dm_priv_var = NULL;
  dm_proc_t *p_dm_proc = NULL;
  MT_ASSERT(p_para != NULL);
  MT_ASSERT(p_para->para_size == sizeof(dm_v2_init_para_t));
  log_perf(LOG_FLASH_PROTECT,DM_INIT_V2,0,0);
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
  p_dm_proc->active_node = dm_active_node_v2;
  p_dm_proc->check_block = dm_check_block_v2;
  p_dm_proc->del_node = dm_del_node_v2;
  p_dm_proc->flush_cache = dm_flush_cache_v2;
  p_dm_proc->reset_iw_block = dm_reset_iw_block_v2;
  p_dm_proc->restore_cache = dm_restore_cache_v2;
  p_dm_proc->set_cache = dm_set_cache_v2;
//  p_dm_proc->set_fix_id = dm_set_fix_id_v2;
  p_dm_proc->write_node = dm_write_node_v2;
  p_dm_proc->read_node = dm_read_v2;
  p_dm_proc->set_header = dm_set_header_v2;
  p_dm_proc->data_restore = dm_restore_v2;
  p_dm_proc->direct_write = dm_direct_write_v2;
  p_dm_proc->direct_erase = dm_direct_erase_v2;
  p_dm_proc->direct_read = dm_direct_read_v2;
  p_dm_proc->get_node_id_list = dm_get_node_id_list_v2;
  p_dm_proc->get_blk_node_num = dm_get_blk_node_num_v2;

  ////fix me
  p_dm_proc->get_block_addr = dm_get_block_addr_32;
  p_dm_proc->get_block_header = dm_get_block_header_32;
  p_dm_proc->get_block_info = dm_get_block_info_32;
  p_dm_proc->get_block_size = dm_get_block_size_32;
  p_dm_proc->read_bl_block = dm_read_bl_block_32;
  p_dm_proc->get_max_node_len = dm_get_max_node_len_32;
  p_dm_proc->get_max_blk_num = dm_get_max_blk_num_32;
  p_dm_proc->show_flash_map = dm_show_flash_map_32;

  //Initialize field members of private data information
  p_dm_priv_var = (dm_var_t *)p_dm_proc->p_priv_data;

  //Initialize data manager base information
  p_dm_info = &p_dm_priv_var->dm_info;
  memset(p_dm_info, 0, sizeof(dm_global_info_t));

  p_dm_info->max_blk_num = p_para->max_blk_num;
  p_dm_info->flash_base_addr = p_para->flash_base_addr;
  p_dm_info->p_blk_mgr = mtos_malloc(p_para->max_blk_num
    * sizeof(block_mgr_t));

  MT_ASSERT(p_dm_info->p_blk_mgr != NULL);
  memset(p_dm_info->p_blk_mgr, 0, p_para->max_blk_num * sizeof(block_mgr_t));

  p_dm_priv_var->align_bytes = DM_ATOM_SIZE(p_para->align_4bytes);

  //Charsto device init
  p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_charsto_dev != NULL);
  p_dm_priv_var->p_charsto_dev = p_charsto_dev;

  if(p_para->use_mutex)
  {
    p_dm_priv_var->is_use_mutex = TRUE;
    p_dm_priv_var->p_dm_mutex = mtos_mutex_create(p_para->mutex_prio);
    MT_ASSERT(p_dm_priv_var->p_dm_mutex != NULL);
    DM_DBG_MUTEX("open mutex\n");
  }
  else
  {
    p_dm_priv_var->is_use_mutex = FALSE;
    p_dm_priv_var->p_dm_mutex = NULL;
    DM_DBG_MUTEX("not open mutex\n");
  }

  //create task
  p_dm_priv_var->is_open_monitor = p_para->open_monitor;
  /*
  if(p_dm_priv_var->is_open_monitor == FALSE)
  {
    DM_ERROR("plase open task monitor\n");
    MT_ASSERT(0);
  }
  */
  
  if(p_para->open_monitor)
  {
    u32 *p_stk = (u32 *)mtos_malloc(p_para->task_stack_size);
    MT_ASSERT(p_stk != NULL);
    memset(p_stk, 0x0, p_para->task_stack_size);
  
    if(!mtos_task_create((u8 *)"dm_v2", task_process, p_dm_proc->p_priv_data,
      p_para->task_prio, p_stk, p_para->task_stack_size))
    {
      DM_ERROR("create task monitor fail!\n");
      MT_ASSERT(0);
    }

    p_dm_priv_var->p_stack = p_stk;
    p_dm_priv_var->task_prio = p_para->task_prio;
  }

  p_dm_priv_var->is_test_mode = p_para->test_mode;

  //Register handle
  class_register(DM_CLASS_ID, p_dm_proc);

  //DM_PRINT("using data_manager_v2\n");
}


void dm_init_v2_ex(dm_v2_init_para_t *p_para)
{
  dm_global_info_t *p_dm_info = NULL;
  dm_var_t *p_dm_priv_var = NULL;
  dm_proc_t *p_dm_proc = NULL;

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

  //Malloc private data for data manager
  p_dm_proc->p_priv_data = mtos_malloc(sizeof(dm_var_t));
  MT_ASSERT(p_dm_proc->p_priv_data != NULL);
  memset(p_dm_proc->p_priv_data, 0, sizeof(dm_var_t));

  //Attach process function
  //p_dm_proc->active_node      = NULL;
  //p_dm_proc->check_block      = NULL;
  //p_dm_proc->del_node         = NULL;
  //p_dm_proc->flush_cache      = NULL;
  //p_dm_proc->reset_iw_block   = NULL;
  //p_dm_proc->restore_cache    = NULL;
  //p_dm_proc->set_cache        = NULL;
  //p_dm_proc->set_fix_id       = NULL;
  p_dm_proc->write_node       = dm_write_node_v2;
  p_dm_proc->read_node        = dm_read_v2;
  //p_dm_proc->set_header       = NULL;
  p_dm_proc->set_header_ex    = dm_set_header_v2_ex;
  //p_dm_proc->data_restore     = NULL;
  //p_dm_proc->direct_write     = NULL;
  //p_dm_proc->direct_erase     = NULL;
  //p_dm_proc->direct_read      = NULL;
  //p_dm_proc->get_node_id_list = NULL;
  p_dm_proc->get_blk_node_num = dm_get_blk_node_num_v2;

  ////fix me
  p_dm_proc->get_block_addr   = dm_get_block_addr_32;
  //p_dm_proc->get_block_header = NULL;
  //p_dm_proc->get_block_info   = NULL;
  //p_dm_proc->get_block_size   = NULL;
  //p_dm_proc->read_bl_block    = NULL;
  //p_dm_proc->get_max_node_len = NULL;
  //p_dm_proc->get_max_blk_num  = NULL;
  //p_dm_proc->show_flash_map   = NULL;

  //Initialize field members of private data information
  p_dm_priv_var = (dm_var_t *)p_dm_proc->p_priv_data;

  //Initialize data manager base information
  p_dm_info = &p_dm_priv_var->dm_info;
  memset(p_dm_info, 0, sizeof(dm_global_info_t));

  p_dm_info->max_blk_num = p_para->max_blk_num;
  p_dm_info->flash_base_addr = p_para->flash_base_addr;
  p_dm_info->p_blk_mgr = mtos_malloc(p_para->max_blk_num
                                  * sizeof(block_mgr_t));
  p_dm_info->prev_blk_id = 0;

  MT_ASSERT(p_dm_info->p_blk_mgr != NULL);
  memset(p_dm_info->p_blk_mgr, 0, p_para->max_blk_num * sizeof(block_mgr_t));

  p_dm_priv_var->align_bytes = DM_ATOM_SIZE(p_para->align_4bytes);

  //Charsto device init
  p_dm_priv_var->p_charsto_dev = dev_find_identifier(NULL,
                                                     DEV_IDT_TYPE,
                                                     SYS_DEV_TYPE_CHARSTO);

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

  MT_ASSERT(p_dm_priv_var->p_charsto_dev != NULL);

  //Register handle
  class_register(DM_CLASS_ID, p_dm_proc);
}

void dm_destory_v2(void)
{
  dm_var_t *p_dm_priv = NULL;
  dm_proc_t *p_dm_proc = NULL;
  dm_global_info_t *p_dm_info = NULL;

  p_dm_proc = class_get_handle_by_id(DM_CLASS_ID);
  if(p_dm_proc == NULL) //only init once
  {
    return;
  }

  p_dm_priv = (dm_var_t *)p_dm_proc->p_priv_data;

  p_dm_info = &p_dm_priv->dm_info;

  mtos_free(p_dm_info->p_blk_mgr);

  if(p_dm_priv->is_use_mutex)
  {
    mtos_mutex_delete(p_dm_priv->p_dm_mutex);
  }

  if(p_dm_priv->is_open_monitor)
  {
    mtos_free(p_dm_priv->p_stack);
    mtos_task_delete(p_dm_priv->task_prio);
  }

  mtos_free(p_dm_proc->p_priv_data);

  mtos_free(p_dm_proc);

  //unregister handle
  class_register(DM_CLASS_ID, NULL);
}
