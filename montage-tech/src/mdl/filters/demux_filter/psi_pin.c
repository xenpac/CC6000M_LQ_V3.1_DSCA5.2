/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

// sys
#include "hal_misc.h"
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_sem.h"

//driver
#include "drv_dev.h"
#include "dmx.h"

//util
#include "class_factory.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "ipin.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "input_pin.h"
#include "output_pin.h"
#include "source_pin.h"
#include "ifilter.h"
#include "chain.h"
#include "iqueue.h"
#include "demux_filter.h"
#include "demux_interface.h"
#include "demux_pin_intra.h"

#include "mdl.h"
#include "nim.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "dvb_svc.h"
#include "err_check_def.h"

#define PSI_PIN_DEBUG

#ifdef  PSI_PIN_DEBUG
#define PSI_PRINT  OS_PRINTF("%s, %d line : ", __FUNCTION__, __LINE__); \
                                 OS_PRINTF
#else
#define PSI_PRINT  DUMMY_PRINTF
#endif


/*!
 how many psi open si_chan success? 
 */
#define PSI_CHANID_NUM                  (96) // DMX_FILTER_NUM


/*!
  defines psi invalid section id.
  */
#define PSI_INVALID_SEC_ID              (0xFFFF)


/*!
  defines psi data size
  */
#define PSI_DATA_SIZE                   (4*(KBYTES) + 188)


/*!
  signle mode and tspkt(for example video_packet_eli.c)
  */
#define PSI_TSPKT_SIZE                  (188)

/*!
  psi memory pool
  */
typedef struct tag_psi_mem_pool
{
 /*!
  buffer address
  */ 
  u8 *p_buf;
 /*!
  is used this buffer.
  */
  BOOL is_used;
}psi_mem_pool_t;

/*!
  psi element private information
  */
typedef struct tag_psi_section
{
 /*!
  psi psi request table parameter.
  */
  psi_request_data_t req_data;
 /*!
  start ticks
  */
  u32 start_ticks;
 /*!
  dmx channel id
  */
  dmx_chanid_t chan_id;
 /*!
  section id
  */
  u32 sec_id;
 /*!
  dmx buffer address
  */
  u8 *p_sec_buf;
}psi_section_t;

/*!
  designing idea :
  use four queue manage psi_section_t array.

  signal mode :
                --> wait_queue
               |        |
  free_queue --         |
       ^       |        V
       |        --> using_queue ---> filled_queue
       |                                   |
        -----------------------------------

  multi mode:
        -----------------
       |                |
       |        --> wait_queue
       V       |        |
  free_queue --         |
       ^       |        V
       |        --> using_queue ---> filled_queue
       |                |                  |
       |                V                  |
        -----------------------------------
  */

/*!
  the psi pin private data define
  */
typedef struct tag_psi_pin_private
{
 /*!
  fill sample success.
  */
  BOOL fill_sample_suc;
 /*!
  dmx device handle
  */
  dmx_device_t *p_dmx_dev;
 /*!
  mem alloc interface
  */
  imem_allocator_t *p_alloc;
 /*!
  total section : this value decided p_sec_list, free_queue, wait_queue.
  And external assign this value.
  */
  u32 total_sec   : 11;
 /*!
  total section : this value decided p_mem_list, using_queue, filled_queue.
  And this value must less than or equal PSI_CHANID_NUM(DMX_FILTER_NUM).
  So if total_sec big than PSI_CHANID_NUM, total_mem = PSI_CHANID_NUM.
  */
  u32 total_mem   : 11;
 /*!
  count malloc, use for debug memory leak.
  */
  u32 count_malloc  : 10;
 /*!
  psi chart list
  */
  psi_section_t *p_sec_list;
 /*!
  free section list queue.
  */
  iqueue_t *p_free_queue;
 /*!
  wait section list queue.
  */
  iqueue_t *p_wait_queue;
 /*!
  mem pool list.
  */
  psi_mem_pool_t *p_mem_list; 
 /*!
  using section list queue.
  */
  iqueue_t *p_using_queue;
 /*!
  filled sample list queue.
  */
  iqueue_t *p_filled_queue;
  /*!
  pin sem.
  */
  os_sem_t pin_sem;
  /*!
  ts in;
  */
  u8 demux_index;
}psi_pin_priv_t;

static psi_pin_priv_t *_psi_pin_get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return (psi_pin_priv_t *)(((demux_pin_t *)_this)->p_child_priv);
}

static void _psi_mem_pool_print(psi_pin_priv_t *p_priv, char *p_name)
{
  u16 index = 0;

  PSI_PRINT("%s :  total mem %d\n", p_name, p_priv->total_mem);
  for(index = 0; index < p_priv->total_mem; index++)
  {
    PSI_PRINT("index %d, is_used %d, p_buf %x\n", 
              index, p_priv->p_mem_list[index].is_used, 
              p_priv->p_mem_list[index].p_buf);
  }
}

static void _psi_mem_pool_assert(psi_pin_priv_t *p_priv, BOOL b_flag, char *p_name)
{
  if(b_flag == TRUE)
    return;

  _psi_mem_pool_print(p_priv, p_name);
  CHECK_FAIL_RET_VOID(0);
}

static u8 *_psi_mem_pool_alloc(psi_pin_priv_t *p_priv)
{
  u16 index = 0;
  psi_mem_pool_t *p_mem_list = p_priv->p_mem_list;
  
  /*!
    found mem pool, if not found, create new.
    */
  for(index = 0; index < p_priv->total_mem; index++)
  {
    if(p_mem_list[index].is_used == FALSE)
    {
     /*!
      if buf == null, means found to top.  
      */
      if(p_mem_list[index].p_buf != NULL)
      {
        p_mem_list[index].is_used = TRUE;
        return p_mem_list[index].p_buf;
      }
      else
      {
        break;
      }
    }
    _psi_mem_pool_assert(p_priv, (p_mem_list[index].p_buf != NULL), "buf null");
  }
  _psi_mem_pool_assert(p_priv, (index < p_priv->total_mem), "index invalid");
  
  p_mem_list[index].p_buf = (u8 *)mtos_malloc(PSI_DATA_SIZE + 15);
  CHECK_FAIL_RET_NULL(p_mem_list[index].p_buf != NULL);
  memset(p_mem_list[index].p_buf, 0x0, PSI_DATA_SIZE);
  
  p_mem_list[index].is_used = TRUE;
  
  p_priv->count_malloc++;    

  return p_mem_list[index].p_buf;
}


static void _psi_mem_pool_free(psi_pin_priv_t *p_priv, u8 *p_buf)
{
  u16 index = 0;
  psi_mem_pool_t *p_mem_list = p_priv->p_mem_list;
    
  /*!
    found mem pool, if not found, create new.
    */
  for(index = 0; index < p_priv->total_mem; index++)
  {
    if(p_mem_list[index].p_buf == p_buf)
    {
      p_mem_list[index].is_used = FALSE;
      return;
    }
  }
  
  PSI_PRINT("free buff : %x\n", p_buf);
  _psi_mem_pool_assert(p_priv, 0, "free abnormal");
}


static void _psi_mem_pool_init(psi_pin_priv_t *p_priv)
{
  u16 index = 0;

  p_priv->p_mem_list = (psi_mem_pool_t *)
      mtos_malloc(sizeof(psi_mem_pool_t) * p_priv->total_mem);
  CHECK_FAIL_RET_VOID(p_priv->p_mem_list != NULL);
  
  for(index = 0; index < p_priv->total_mem; index++)
  {
    p_priv->p_mem_list[index].is_used = FALSE;
    p_priv->p_mem_list[index].p_buf   = NULL;
  }
  //use for debug memory leak.
  p_priv->count_malloc = 0;
}

static void _psi_mem_pool_destory(psi_pin_priv_t *p_priv)
{
  u16 index = 0;
  u16 ref   = 0;
  psi_mem_pool_t *p_mem_list = p_priv->p_mem_list;
  
  for(index = 0; index < p_priv->total_mem; index++)
  {
    if(p_mem_list[index].p_buf != NULL)
    {
      mtos_free(p_mem_list[index].p_buf);
      p_mem_list[index].p_buf = NULL;
      ref ++;
    }
    else
    {
      break;
    }
  }
  _psi_mem_pool_assert(p_priv, ref == p_priv->count_malloc, "mem destory");

  mtos_free(p_priv->p_mem_list);
  p_priv->p_mem_list = NULL;
}

/*!
  psi pin report.
  */
static void _psi_pin_report(psi_pin_priv_t *p_priv)
{
#if 0
  PSI_PRINT("psi pin total section : %d\n", p_priv->total_sec);
  p_priv->p_free_queue->print_iqueue(p_priv->p_free_queue, NULL, NULL);
  p_priv->p_filled_queue->print_iqueue(p_priv->p_filled_queue, NULL, NULL);

  PSI_PRINT("psi pin total mem : %d\n", p_priv->total_mem);
  p_priv->p_using_queue->print_iqueue(p_priv->p_using_queue, NULL, NULL);
  p_priv->p_using_queue->print_iqueue(p_priv->p_wait_queue, NULL, NULL);
  _psi_mem_pool_print(p_priv, "report");  
  #endif
}

static void check_queue_number(psi_pin_priv_t *p_priv,char *function)
{
    u32 using_num = p_priv->p_using_queue->get_iqueue_len(p_priv->p_using_queue);
    u32 filled_num = p_priv->p_filled_queue->get_iqueue_len(p_priv->p_filled_queue);
    u32 wait_num = p_priv->p_wait_queue->get_iqueue_len(p_priv->p_wait_queue);
    u32 free_num = p_priv->p_free_queue->get_iqueue_len(p_priv->p_free_queue);
    if((using_num + filled_num + wait_num + free_num) < p_priv->total_sec)
    {
      OS_PRINTF("!!![%s]:wait_que:0x%x len:%d,free_que:0x%x len:%d\n",
      function,
      p_priv->p_wait_queue,wait_num,
      p_priv->p_free_queue,free_num);
            OS_PRINTF("!!![%s]:use_qu:0x%x len:%d,fill_qu:0x%x len:%d\n",
      function,
      p_priv->p_using_queue,using_num,
      p_priv->p_filled_queue,filled_num);
      CHECK_FAIL_RET_VOID(0);
    }
}
/*!
  pop free psi_section_t.
  */
static psi_section_t *_psi_pin_pop_free_sec(psi_pin_priv_t *p_priv)
{
  u32            sec_id = 0;
  psi_section_t *p_sec  = NULL;

  if(!p_priv->p_free_queue->pop_iqueue(p_priv->p_free_queue, &sec_id))
  {
    PSI_PRINT("free queue empty\n");
    _psi_pin_report(p_priv);
    CHECK_FAIL_RET_NULL(0);
  }

  p_sec = p_priv->p_sec_list + sec_id;
  p_sec->sec_id  = sec_id;
  p_sec->chan_id = DMX_INVALID_CHANNEL_ID;

  return p_sec;
}

/*!
 pop one section and return psi_section_t pointer.
 (not supports free queue.)

 \param[in] p_sec_list : section list.
 \param[in] list_depth : list depth
 \param[in] p_queue    : queue handle
 */
static psi_section_t *_psi_pin_pop_sec(psi_pin_priv_t *p_priv,
                                       iqueue_t       *p_queue)
{
  BOOL b_pop  = FALSE;
  u32  sec_id = 0;

  b_pop = p_queue->pop_iqueue(p_queue, &sec_id);
  if(b_pop != TRUE)
  {
    _psi_pin_report(p_priv);
    CHECK_FAIL_RET_NULL(0);
  }
  CHECK_FAIL_RET_NULL(sec_id < p_priv->total_sec);
  CHECK_FAIL_RET_NULL(p_priv->p_sec_list[sec_id].sec_id == sec_id);

  return p_priv->p_sec_list + sec_id;
}

/*!
 pop one section and return psi_section_t pointer.

 \param[in] p_queue : queue handle
 \param[in] sec_id  : section id.
 */
static void _psi_pin_push_sec(psi_pin_priv_t *p_priv, iqueue_t *p_queue, u32 sec_id)
{
  BOOL b_push = FALSE;

  b_push = p_queue->push_iqueue(p_queue, sec_id);
  if(b_push != TRUE)
  {
    _psi_pin_report(p_priv);
    CHECK_FAIL_RET_VOID(0);
  }
}

/*!
  psi pin alloc section

  \param[in] p_priv : psi pin private
  \param[in] p_sec  : psi section pointer
  */
static RET_CODE _psi_pin_alloc_section(psi_pin_priv_t *p_priv,
                                       psi_section_t *p_sec)
{
  dmx_device_t *p_dmx_dev = p_priv->p_dmx_dev;
  dmx_slot_setting_t slot_set = { 0 };
  RET_CODE ret = ERR_FAILURE;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif

  CHECK_FAIL_RET_CODE(p_priv != NULL);
  CHECK_FAIL_RET_CODE(p_sec  != NULL);

  slot_set.pid = p_sec->req_data.pid;
  slot_set.type = p_sec->req_data.slot_type;
  //hack for warriors bug 22712,37060, all use soft filter.
  slot_set.soft_filter_flag = dvb_svc_is_use_soft();
  slot_set.in = p_priv->demux_index;
  if(chip_ic_id == IC_CONCERTO)
  {
    slot_set.muldisp = DMX_SLOT_MUL_SEC_DIS_DISABLE;//for bug 63129
  }
  if(SUCCESS == dmx_si_chan_open(p_dmx_dev, &slot_set, &p_sec->chan_id))
  {
    dmx_filter_setting_t filter_set = { 0 };
    u8 *p_data = NULL;
    u32 data_size = 0;
    CHECK_FAIL_RET_CODE(p_sec->chan_id != DMX_INVALID_CHANNEL_ID);

    if(p_sec->req_data.req_mode == DMX_DATA_SINGLE)
    {
      filter_set.continuous = FALSE;
      //default sec_buf = PSI_DATA_SIZE, so alloc buf_size = PSI_DATA_SIZE.
      p_sec->p_sec_buf = _psi_mem_pool_alloc(p_priv);      
      CHECK_FAIL_RET_CODE(p_sec->p_sec_buf != NULL);
      if(DMX_CH_TYPE_TSPKT == p_sec->req_data.slot_type)//fox example video_packet
      {
        data_size = PSI_TSPKT_SIZE;
      }
      else
      {
        data_size = PSI_DATA_SIZE;
      }
      p_sec->req_data.p_ext_data = NULL; //use for free flag.
    }
    else
    {
      filter_set.continuous = TRUE;
      CHECK_FAIL_RET_CODE(p_sec->req_data.p_ext_data != NULL);
      CHECK_FAIL_RET_CODE(p_sec->req_data.ext_data_size > 0);
      p_sec->p_sec_buf = p_sec->req_data.p_ext_data;
      data_size = p_sec->req_data.ext_data_size;
    }

    if(p_sec->req_data.filter_value != NULL)
    {
      memcpy(filter_set.value, p_sec->req_data.filter_value, DMX_SECTION_FILTER_SIZE);
    }

    if(p_sec->req_data.filter_mask != NULL)
    {
      memcpy(filter_set.mask, p_sec->req_data.filter_mask, DMX_SECTION_FILTER_SIZE);
    }

    filter_set.en_crc = p_sec->req_data.enable_crc;
    if(DMX_CH_TYPE_TSPKT == p_sec->req_data.slot_type)
      filter_set.ts_packet_mode = DMX_ONE_MODE;

   p_data = (u8 *)(((u32)p_sec->p_sec_buf + 15) & (~15)); //align 16
   data_size -= ((u32) p_data - (u32)p_sec->p_sec_buf);

   data_size = ((data_size / 8) * 8); //allign 8
    
   if(chip_ic_id == IC_WARRIORS)
    {
      p_data = (u8 *)(((u32)p_data) | 0xa0000000);
    }

    ret = dmx_si_chan_set_buffer(p_priv->p_dmx_dev, p_sec->chan_id, p_data, data_size);
    CHECK_FAIL_RET_CODE(ret == SUCCESS);  
    ret = dmx_si_chan_set_filter(p_priv->p_dmx_dev, p_sec->chan_id, &filter_set);
    CHECK_FAIL_RET_CODE(ret == SUCCESS);

    ret = dmx_chan_start(p_priv->p_dmx_dev, p_sec->chan_id);
    CHECK_FAIL_RET_CODE(ret == SUCCESS);

    p_sec->start_ticks = mtos_ticks_get();

    // alloc filter suc, push it to using queue
    _psi_pin_push_sec(p_priv, p_priv->p_using_queue, p_sec->sec_id);
    return SUCCESS;
  }

  //push it to wait queue.
  _psi_pin_push_sec(p_priv, p_priv->p_wait_queue, p_sec->sec_id);
  return ERR_FAILURE;
}

/*!
  psi psi free table

  \param[in] p_priv : psi pin private
  \param[in] p_sec  : psi section pointer.
  */
static RET_CODE _psi_pin_free_section(psi_pin_priv_t *p_priv,
                                      psi_section_t  *p_sec)
{
  dmx_device_t *p_dmx_dev = p_priv->p_dmx_dev;

  CHECK_FAIL_RET_CODE(p_dmx_dev      != NULL);
  CHECK_FAIL_RET_CODE(p_sec          != NULL);
  CHECK_FAIL_RET_CODE(p_sec->chan_id != DMX_INVALID_CHANNEL_ID);
  dmx_chan_stop(p_dmx_dev, p_sec->chan_id);
  dmx_chan_close(p_dmx_dev, p_sec->chan_id);

 /*!
  This section use intra buffer(mtos_malloc, so must mtos_free it)
  */
  if(p_sec->req_data.p_ext_data == NULL)
  {
    CHECK_FAIL_RET_CODE(p_sec->p_sec_buf != NULL);
    _psi_mem_pool_free(p_priv, p_sec->p_sec_buf);
  }

  _psi_pin_push_sec(p_priv, p_priv->p_free_queue, p_sec->sec_id);

  p_sec->chan_id = DMX_INVALID_CHANNEL_ID;
  p_sec->sec_id  = PSI_INVALID_SEC_ID;

  return SUCCESS;
}

/*!
  psi pin found section

  \param[in] p_sec_list : section list.
  \param[in] list_depth : section list depth.
  \param[in] p_queue    : queue handle
  \param[in] media_type : free data
  \param[in] psi_pid    : psi pid.
  */
static psi_section_t *_psi_pin_find_section(psi_pin_priv_t *p_priv,
                                            iqueue_t       *p_queue,
                                            media_type_t    media_type,
                                            u32             psi_pid,
                                            u16 table_id)
 {
  u32            index   = 0;
  u32            len_que = 0;
  psi_section_t *p_sec   = NULL;

  CHECK_FAIL_RET_NULL(p_priv  != NULL);
  CHECK_FAIL_RET_NULL(p_queue != NULL);

  len_que = p_queue->get_iqueue_len(p_queue);
  for(index = 0; index < len_que; index++)
  {
    p_sec = _psi_pin_pop_sec(p_priv, p_queue);

    if((media_type == p_sec->req_data.media_type) &&
       (psi_pid == p_sec->req_data.pid) && 
       (table_id == p_sec->req_data.table_id))
    {
      return p_sec;
    }
    else
    {
      OS_PRINTF("_psi_pin_find_section err:type=%d,pid=%d table_id = 0x%x\n",
       p_sec->req_data.media_type,p_sec->req_data.pid,p_sec->req_data.table_id);
    }
    _psi_pin_push_sec(p_priv, p_queue, p_sec->sec_id);
  }

  return NULL;
}

/*!
  psi pin dispose wait queue

  \param[in] p_priv : psi pin private
  \param[in] p_sec  : psi section pointer
  */
static void _psi_pin_process_wait_q(psi_pin_priv_t *p_priv)
{
  psi_section_t *p_sec     = NULL;
  u32            len_queue = 0;
  u32            index     = 0;
  RET_CODE       ret_alloc = ERR_FAILURE;

  CHECK_FAIL_RET_VOID(p_priv != NULL);
check_queue_number(p_priv,(char *)(char *)"_psi_pin_process_wait_q_start");
  //wait queue ==> using queue.
  len_queue = p_priv->p_wait_queue->get_iqueue_len(p_priv->p_wait_queue);
  for(index = 0; index < len_queue; index++)
  {
    p_sec = _psi_pin_pop_sec(p_priv, p_priv->p_wait_queue);
    ret_alloc = _psi_pin_alloc_section(p_priv, p_sec);

    if(SUCCESS != ret_alloc)
      break;     /***why break?***/
  }
 check_queue_number(p_priv,(char *)(char *)"_psi_pin_process_wait_q_end");
}

/*!
  psi pin request table

  \param[in] _this psi pin handle
  \param[in] p_req pointer psi pin request parameter
  */
static RET_CODE psi_pin_request_table(handle_t _this, void *p_data)
{
  psi_pin_priv_t     *p_priv     = _psi_pin_get_priv(_this);
  psi_section_t      *p_sec      = NULL;
  psi_request_data_t *p_req_data = (psi_request_data_t *)p_data;
  RET_CODE            ret_alloc  = ERR_FAILURE;

  CHECK_FAIL_RET_CODE(p_req_data != NULL);
  mtos_sem_take(&p_priv->pin_sem, 0);
  p_sec = _psi_pin_pop_free_sec(p_priv);
  CHECK_FAIL_RET_CODE(p_sec != NULL);
  memcpy(&p_sec->req_data, p_req_data, sizeof(psi_request_data_t));
  //OS_PRINTF("@@@debug psi pin request table[0x%x] pid[0x%x]\n",
  //p_sec->req_data.table_id,p_sec->req_data.pid);
  ret_alloc = _psi_pin_alloc_section(p_priv, p_sec);
  check_queue_number(p_priv,(char *)__FUNCTION__);
  mtos_sem_give(&p_priv->pin_sem);
  return ret_alloc;
}

/*!
  psi pin free table

  \param[in] _this psi pin handle
  \param[in] p_free free table info
  */
static RET_CODE psi_pin_free_table(handle_t _this, void *p_data)
{
  psi_pin_priv_t  *p_priv = _psi_pin_get_priv(_this);
  psi_section_t   *p_sec  = NULL;
  psi_free_data_t *p_free = (psi_free_data_t *)p_data;

  CHECK_FAIL_RET_CODE(p_free != NULL);
  CHECK_FAIL_RET_CODE(p_free->req_mode == DMX_DATA_MULTI); 
  //only supports multi mode manul free.
 //OS_PRINTF("psi_pin_free_table:media_type %d, 
 //req_mode %d psi_id:%d;table_id:%d\n", 
  //p_free->media_type, p_free->req_mode,p_free->psi_pid,p_free->table_id);
  /*!
   first found from using queue.
   */
  mtos_sem_take(&p_priv->pin_sem, 0);
  check_queue_number(p_priv,(char *)"psi_pin_free_table_start");
  p_sec = _psi_pin_find_section(p_priv,
                                p_priv->p_using_queue,
                                p_free->media_type,
                                p_free->psi_pid,
                                p_free->table_id);
  if(p_sec != NULL)
  {
    _psi_pin_free_section(p_priv, p_sec);
    check_queue_number(p_priv,(char *)"psi_pin_free_table_using");
    mtos_sem_give(&p_priv->pin_sem);
    return SUCCESS;
  }

  /*!
   second found from filled iqueue.
   */
  p_sec = _psi_pin_find_section(p_priv,
                                p_priv->p_filled_queue,
                                p_free->media_type,
                                p_free->psi_pid,
                                p_free->table_id);
  if(p_sec != NULL)
  {
    _psi_pin_free_section(p_priv, p_sec);
    check_queue_number(p_priv,(char *)"psi_pin_free_table_fill");
    mtos_sem_give(&p_priv->pin_sem);
    return SUCCESS;
  }

  /*!
   third found from wait queue.
   only dispose sec_id, beacuse this section not dmx chan_id.
   */
  p_sec = _psi_pin_find_section(p_priv,
                                p_priv->p_wait_queue,
                                p_free->media_type,
                                p_free->psi_pid,
                                p_free->table_id);
  if(p_sec != NULL)
  {
    _psi_pin_push_sec(p_priv, p_priv->p_free_queue, p_sec->sec_id);
    check_queue_number(p_priv,(char *)"psi_pin_free_table_wait");
    mtos_sem_give(&p_priv->pin_sem);
    return SUCCESS;
  }
   _psi_pin_report(p_priv);
  check_queue_number(p_priv,(char *)"psi_pin_free_table_end");
  mtos_sem_give(&p_priv->pin_sem);
  return ERR_FAILURE;
}

static RET_CODE psi_pin_config(handle_t _this, void *p_data)
{
  psi_pin_priv_t *p_priv = _psi_pin_get_priv(_this);
  psi_pin_cfg_t  *p_cfg  = (psi_pin_cfg_t *)p_data;

  CHECK_FAIL_RET_CODE(p_cfg->max_table_num > 0);

  p_priv->total_sec = p_cfg->max_table_num;
  //memory pool depth.
  p_priv->total_mem = p_priv->total_sec > PSI_CHANID_NUM ? 
                        PSI_CHANID_NUM : p_priv->total_sec;
  p_priv->demux_index = p_cfg->demux_index;
  return SUCCESS;
}

static RET_CODE psi_pin_on_open(handle_t _this)
{
  u16             index  = 0;
  psi_pin_priv_t *p_priv = _psi_pin_get_priv(_this);

  if(p_priv->total_sec <= 0)
  {
    PSI_PRINT("please config psi_pin before do open");
    CHECK_FAIL_RET_CODE(0);
  }

  /*!
   alloc and init section list. and create free_queue, wait_queue.
   */
  p_priv->p_sec_list = (psi_section_t *)mtos_malloc(p_priv->total_sec * sizeof(psi_section_t));
  CHECK_FAIL_RET_CODE(p_priv->p_sec_list != NULL);
  memset(p_priv->p_sec_list, 0x0, p_priv->total_sec * sizeof(psi_section_t));

  for(index = 0; index < p_priv->total_sec; index++)
  {
    p_priv->p_sec_list[index].sec_id  = PSI_INVALID_SEC_ID;
    p_priv->p_sec_list[index].chan_id = DMX_INVALID_CHANNEL_ID;
  }

  p_priv->p_free_queue = create_iqueue(p_priv->total_sec, "psi_free_queue");
  CHECK_FAIL_RET_CODE(p_priv->p_free_queue != NULL);

  p_priv->p_wait_queue = create_iqueue_ext(p_priv->total_sec, NULL, "psi_wait_queue");
  CHECK_FAIL_RET_CODE(p_priv->p_wait_queue != NULL);

 /*!
  init mem pool, and create using_queue, filled_queue.
  */
  _psi_mem_pool_init(p_priv);
  
  p_priv->p_using_queue = create_iqueue_ext(p_priv->total_mem, NULL, "psi_using_queue");
  CHECK_FAIL_RET_CODE(p_priv->p_using_queue != NULL);

  p_priv->p_filled_queue = create_iqueue_ext(p_priv->total_mem, NULL, "psi_filled_queue");
  CHECK_FAIL_RET_CODE(p_priv->p_filled_queue != NULL);

  return SUCCESS;
}

static RET_CODE psi_pin_on_close(handle_t _this)
{
  psi_pin_priv_t *p_priv = _psi_pin_get_priv(_this);
  _psi_mem_pool_destory(p_priv);
  
  mtos_free(p_priv->p_sec_list);
  p_priv->p_sec_list = NULL;
  p_priv->p_free_queue->destory_iqueue(p_priv->p_free_queue);
  p_priv->p_using_queue->destory_iqueue(p_priv->p_using_queue);
  p_priv->p_wait_queue->destory_iqueue(p_priv->p_wait_queue);
  p_priv->p_filled_queue->destory_iqueue(p_priv->p_filled_queue);
  return SUCCESS;
}

static void psi_pin_on_destroy(handle_t _this)
{
  psi_pin_priv_t *p_priv = _psi_pin_get_priv(_this);
  if(p_priv != NULL)
  {
    mtos_free((void *)p_priv);
  }
}

static void psi_pin_on_report(handle_t _this, void *p_state)
{
  psi_pin_priv_t *p_priv = _psi_pin_get_priv(_this);
  _psi_pin_report(p_priv);
}

static RET_CODE psi_pin_on_fill_sample(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, s64 position)
{
  psi_pin_priv_t *p_priv    = _psi_pin_get_priv(_this);
  psi_section_t  *p_sec     = NULL;
  dmx_device_t   *p_dmx_dev = p_priv->p_dmx_dev;
  u8             *p_buf     = NULL;
  u32             buf_size  = 0;
  u32             len_queue = 0;
  RET_CODE        dmx_ret   = ERR_FAILURE;
  u32             index     = 0;

  CHECK_FAIL_RET_CODE(p_priv->p_dmx_dev != NULL);

  p_priv->fill_sample_suc = FALSE;
  p_sample->state = SAMP_STATE_FAIL;
  
  /*!
    first : fill sample from using iqueue.
    using queue ==> filled queue.
   */
  mtos_sem_take(&p_priv->pin_sem, 0);
  check_queue_number(p_priv,(char *)"psi_pin_on_fill_sample_start");
  len_queue = p_priv->p_using_queue->get_iqueue_len(p_priv->p_using_queue);
  for(index = 0; index < len_queue; index++)
  {
    p_sec = _psi_pin_pop_sec(p_priv, p_priv->p_using_queue);
    CHECK_FAIL_RET_CODE(p_sec != NULL);

    CHECK_FAIL_RET_CODE(p_sec->chan_id != DMX_INVALID_CHANNEL_ID);
    dmx_ret = dmx_si_chan_get_data(p_dmx_dev, p_sec->chan_id, &p_buf, &buf_size);
    if(dmx_ret == SUCCESS)
    {
      //fill sample success.
      p_sample->p_data            = p_buf;
      p_sample->payload           = buf_size;
      //p_sample->context           = p_sec->req_data.pid;
      p_sample->context           = ((p_sec->req_data.pid << 16) \
                                                       & 0xffff0000) \
                                                       | (p_sec->req_data.table_id);
      p_sample->format.media_type = p_sec->req_data.media_type;
      p_sample->state             = SAMP_STATE_GOOD;
      _psi_pin_push_sec(p_priv, p_priv->p_filled_queue, p_sec->sec_id);
      p_priv->fill_sample_suc = TRUE;
      p_sec->start_ticks = mtos_ticks_get();
      break;
    }
    else if((mtos_ticks_get() - p_sec->start_ticks) * 10 > p_sec->req_data.timeout)
    {
      //fill sample timeout
      p_sample->p_data            = NULL;
      p_sample->payload           = 0;
      //p_sample->context           = p_sec->req_data.pid;
      p_sample->context           = ((p_sec->req_data.pid << 16) \
                                                     & 0xffff0000) \
                                                     | (p_sec->req_data.table_id);
      p_sample->format.media_type = p_sec->req_data.media_type;
      p_sample->state             = SAMP_STATE_TIMEOUT;
      _psi_pin_push_sec(p_priv, p_priv->p_filled_queue, p_sec->sec_id);
      p_priv->fill_sample_suc = TRUE;
      p_sec->start_ticks = mtos_ticks_get();
      break;
    }
    else
    {
      //back to using_queue.
      _psi_pin_push_sec(p_priv, p_priv->p_using_queue, p_sec->sec_id);
    }
  }

 check_queue_number(p_priv,(char *)(char *)"psi_pin_on_fill_sample_end");
  /*!
    second. dispose wait iqueue, because some times other user maybe releases resource.
    */
  _psi_pin_process_wait_q(p_priv);
  mtos_sem_give(&p_priv->pin_sem);
 return (p_priv->fill_sample_suc ? SUCCESS : ERR_FAILURE);
}

/*!
 this function only process DMX_DATA_SINGLE mode.
 */
static RET_CODE psi_pin_process_evt(handle_t _this,
                                    sample_evt_t evt,
                                    media_sample_t *p_sample)
{
  psi_pin_priv_t *p_priv     = _psi_pin_get_priv(_this);
  psi_section_t  *p_sec      = NULL;
  media_type_t    media_type = MF_UNKNOWN;
  u16             psi_pid    = 0;
  u16 table_id = 0;

  if((evt != EVT_SAMPLE_DESTROY) || (p_priv->fill_sample_suc == FALSE))
  {
    return ERR_FAILURE;
  }

  CHECK_FAIL_RET_CODE(p_sample != NULL);
  mtos_sem_take(&p_priv->pin_sem, 0);
  media_type = p_sample->format.media_type;
  psi_pid    = (u16)((p_sample->context >> 16) & 0xffff);
  table_id = (u16)(p_sample->context & 0xffff);
  check_queue_number(p_priv,(char *)(char *)"psi_pin_process_evt_start");
 /*!
  first free resource : filled queue ==>  free queue or using queue.
  */
  p_sec = _psi_pin_find_section(p_priv,
                                p_priv->p_filled_queue,
                                media_type,
                                psi_pid,
                                table_id);
  //CHECK_FAIL_RET_CODE(p_sec != NULL);
  if(p_sec != NULL)
  {
    if(p_sec->req_data.req_mode == DMX_DATA_SINGLE)
    {
      _psi_pin_free_section(p_priv, p_sec);
    }
    else if(p_sec->req_data.req_mode == DMX_DATA_MULTI)
    {
      CHECK_FAIL_RET_CODE(p_sec->chan_id != DMX_INVALID_CHANNEL_ID);
      _psi_pin_push_sec(p_priv, p_priv->p_using_queue, p_sec->sec_id);
    }
  }
  check_queue_number(p_priv,(char *)(char *)"psi_pin_process_evt_end");
 /*!
  second dispose wait iqueue.
  */
  _psi_pin_process_wait_q(p_priv);
  mtos_sem_give(&p_priv->pin_sem);
  return SUCCESS;
}

static RET_CODE psi_pin_decide_buffer_size(handle_t _this)
{
  psi_pin_priv_t        *p_priv     = _psi_pin_get_priv(_this);
  imem_allocator_t      *p_alloc    = p_priv->p_alloc;
  allocator_properties_t properties = {0,};

  //config mem_alloc
  p_alloc->get_properties(p_alloc, &properties);
  properties.buffers           = 1;
  properties.buffer_size       = PSI_DATA_SIZE;
  properties.use_virtual_space = TRUE;
  p_alloc->set_properties(p_alloc, &properties, NULL);
  return SUCCESS;
}

demux_pin_t * attach_psi_pin_instance(handle_t _this)
{
  psi_pin_priv_t    *p_priv       = NULL;
  demux_pin_t       *p_psi_pin    = (demux_pin_t *)_this;
  base_output_pin_t *p_output_pin = (base_output_pin_t *)_this;
  ipin_t            *p_ipin       = (ipin_t *)_this;
  interface_t       *p_interface  = (interface_t *)_this;
  media_format_t     media_format = { MT_PSI, };

  //check input parameter
  CHECK_FAIL_RET_NULL(_this != NULL);

  //create base class
  p_interface->_rename(p_interface, "psi_pin");

  //init private date
  p_psi_pin->p_child_priv = mtos_malloc(sizeof(psi_pin_priv_t));
  CHECK_FAIL_RET_NULL(p_psi_pin->p_child_priv != NULL);
  memset(p_psi_pin->p_child_priv, 0x0, sizeof(psi_pin_priv_t));

  p_priv = (psi_pin_priv_t *)(p_psi_pin->p_child_priv);

  p_priv->total_sec = 0;
  p_priv->total_mem = 0;
  p_priv->count_malloc = 0;
  
  mtos_sem_create (&p_priv->pin_sem, TRUE);

  p_priv->p_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_NULL(p_priv->p_dmx_dev != NULL);

  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);

  p_ipin->add_supported_media_format(p_ipin, &media_format);

  //init member function
  p_psi_pin->config  = psi_pin_config;
  p_psi_pin->request = psi_pin_request_table;
  p_psi_pin->free    = psi_pin_free_table;
  p_psi_pin->reset_pid = NULL;
  p_psi_pin->pause_resume = NULL;

  //overload source pin virtual function
  //...

  //overload output pin virtual function
  p_output_pin->decide_buffer_size = psi_pin_decide_buffer_size;
  p_output_pin->on_sample_evt      = psi_pin_process_evt;
  p_output_pin->on_fill_sample     = psi_pin_on_fill_sample;

  //overload ipin virtual function
  p_ipin->on_open  = psi_pin_on_open;
  p_ipin->on_close = psi_pin_on_close;

  //overload interface virtual function.
  p_interface->on_destroy = psi_pin_on_destroy;
  p_interface->report     = psi_pin_on_report;

  return p_psi_pin;
}

//END for psi pin
