/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "mem_manager.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"
#include "err_check_def.h"
#include "lib_util.h"
#include "lzmaif.h"
#include "hal_misc.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbc_util.h"
#include "dvbt_util.h"
#include "drv_dev.h"
#include "ss_ctrl.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "data_manager.h"
#include "dvb_protocol.h"

#include "charsto.h"
#include "pti.h"

#include "ap_framework.h"
#include "ap_ota.h"
#include "mosaic.h"
#include "pmt.h"
#include "dsmcc.h"
#include "fcrc.h"

#include "dmx.h"

/*!
   the DSI message queue depth
  */
#define DSI_MSGQ_DEPTH  5
/*!
   the DII message queue depth
  */
#define DII_MSGQ_DEPTH  1
/*!
   the DDM message queue depth
  */
#define DDM_MSGQ_DEPTH 10


#define MAX_SECTIONS_PER_MOD 256
typedef struct
{
  BOOL is_received;
  u32 start_addr;
  u32 section_len;
}section_info_t;
typedef struct
{
  BOOL invalid_mod;
  section_info_t section_info[MAX_SECTIONS_PER_MOD];
}mod_info_t;

/*!
   monitor private data
  */
typedef struct
{
  u32 pre_monitor_ticks;
  BOOL is_restart;
}ota_monitor_priv_t;

/*!
   playback private data
  */
typedef struct
{
  /*!
     ota state machine state
    */
  ota_sm_t sm;
  /*!
     ota info
    */
  ota_info_t otai;
  /*!
    terminate infor
    */
  ota_tdi_t tdi;
  /*!
    DVB service handle
    */
  service_t *p_dvb_svc;
  /*!
    DVB service handle
    */
  service_t *p_nim_svc;
  /*!
    module info
    */
  dsmcc_dii_info_t dii_info;
  /*!
    module info, for insert sections.
    */
  //mod_info_t mod_info[MAX_MODULE_NUM_PER_TYPE];
  mod_info_t *p_mod_info;
  /*!
    ota cfg t
    */
  ota_cfg_t ota_cfg;
  /*!
    upg data total length(include head data)
    */
  u32 total_len;
  /*!
    received data length
    */
  u32 received_len;
  /*!
    valid data cnt
    */
  u32 buf_cnt;
  /*!
    nc channel info
    */
  nc_channel_info_t nc_info;
  /*!
    block id
    */
  u16 block_id;
  /*!
    crc
    */
  u32 crc32;
  /*!
    is upgrade all.
    */
  BOOL is_upgrade_all;
  /*!
    is zipped.
    */
  BOOL is_zipped;
  /*!
    is split
    */
  BOOL is_split;
  /*!
    type length
    */
  u32 type_len[MODULE_TYPE_NUM];
  /*!
    type index
    */
  u32 type_idx;
  /*!
    type number
    */
  u32 type_num;
  /*!
    group id
    */
  u32 group_id;
  /*!
    ota monitor info
    */
  ota_monitor_priv_t monitor_info;
  /*!
    is user force upg
    */
  BOOL is_user_force;
  /*!
    ts in
  */
  u8 ts_in;
  /*!
    tuner id
    */
  u8 tuner_id;
} ota_priv_data_t;

/*!
   dvbs ota handle
  */
typedef struct
{
  /*!
     ota instance
    */
  app_t instance;
  /*!
     policy
    */
  ota_policy_t *p_policy;
  /*!
     private data
    */
  ota_priv_data_t data;
} ota_handle_t;

/*!
  Data manager setcion information
  */
typedef struct tag_setcion_mgr_t
{
  /*!
    Data manager setcion base address
    */
  u32 cur_addr;
}setcion_mgr_bl_t;

typedef struct tag_node_info_t
{
  //malloc node_num space to store each node offset in flash
  u32 node_addr :21;
  // used cache size in current node
  u32 use_cache_size:8;
  //00b: is sync, 01b is async, 11b is empty
  u32 cache_state :2;
  //0: active 1: deleted
  u32 deleted :1;
}node_info_bl_t;

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
  //refer to enum tag_block_type
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
  //below only for type = BLOCK_TYPE_IW
  node_info_bl_t *p_node_info;
  //current used bytes in total block
  u32 total_used;
  //first unused node id
  u16 free_id;
  //form [0:fix_id] is reserved. can't auto dispatch
  u16 fix_id;
  u16 cache_size;
  //current section index
  u16 sec_index;
  //section info
  setcion_mgr_bl_t *p_sec_info;
  u8 *p_cache;
}block_mgr_bl_t;

typedef struct tag_dm_base_info_t
{
  u32 sdram_size;
  u32 flash_size;
  u16 block_num;
  u16 bh_size;
  //above 12B matchs the struct in flash
  u32 flash_base_addr;
  u32 dmh_start_addr[3];
  u16 max_blk_num;
  s32 block_num_off;
  //block_mgr_t block[DM_MAX_BLOCK_NUM];
  block_mgr_bl_t *p_blk_mgr;
}dm_base_info_bl_t;


#define BURN_DEF_FLAG (0xFFFF)
#define BURNING_FLAG (0xFF5A)
#define BURN_DONE_FLAG (0x5A5A)
#define DMH_INDC_SIZE 12
#define FLASH_SECTION_SIZE ((64) * (KBYTES))

static u32 find_dmh_in_mem(u8 *p_mem);
static u8 *get_block_head_addr(u8 *p_start, u32 offset, u8 block_id);
static u8 ota_progress_bak = 0xFF;

static u32 ota_merge_blocks(ota_priv_data_t *p_data)
{
  charsto_device_t *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 loader_size = 0, dmh1_size = 0;
  u32 dmh_offset = 0, block_offset = 0;
  u32 sdram_size = 0, flash_size = 0;
  u16 block_num = 0, bh_size = 0;
  u8 dmh_indc[DMH_INDC_SIZE + 1] = "*^_^*DM(^o^)";
  dmh_block_info_t blocks[64];
  u32 tmp1 = 0, tmp2 = 0;
  BOOL is_match = FALSE;
  BOOL is_found = FALSE;
  u32 i = 0;

  if (SUCCESS != charsto_read(p_dev, 0, p_data->ota_cfg.p_burn_buf, KBYTES))
  {
    return (0);
  }

  for(i = 0; i < KBYTES; i++)
  {
    if(0 == memcmp(p_data->ota_cfg.p_burn_buf + i, dmh_indc, DMH_INDC_SIZE))
    {
      loader_size = MT_MAKE_DWORD(
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[i + DMH_INDC_SIZE],
          p_data->ota_cfg.p_burn_buf[i + DMH_INDC_SIZE + 1]),
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[i + DMH_INDC_SIZE + 2],
          p_data->ota_cfg.p_burn_buf[i + DMH_INDC_SIZE + 3]));

      break;
    }
  }

  tmp1 = (loader_size + KBYTES) / KBYTES * KBYTES;

  for(i = 0; i < 0xFF; i++)
  {
    if (SUCCESS != charsto_read(p_dev,(i * 1024) + tmp1, p_data->ota_cfg.p_burn_buf, KBYTES))
    {
        return (0);
    }

    if(0 == memcmp(p_data->ota_cfg.p_burn_buf, dmh_indc, DMH_INDC_SIZE))
    {
      dmh_offset   = (i * 1024) + tmp1;

      dmh1_size = MT_MAKE_DWORD(
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[12], p_data->ota_cfg.p_burn_buf[13]),
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[14], p_data->ota_cfg.p_burn_buf[15]));

      break;
    }
  }

  if(i == 0xFF)
  {
    return 0;
  }

  if (SUCCESS != charsto_read(p_dev, dmh_offset + 16, p_data->ota_cfg.p_burn_buf, 12))
  {
    return (0);
  }

  sdram_size = MT_MAKE_DWORD(
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[0], p_data->ota_cfg.p_burn_buf[1]),
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[2], p_data->ota_cfg.p_burn_buf[3]));

  flash_size = MT_MAKE_DWORD(
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[4], p_data->ota_cfg.p_burn_buf[5]),
        MAKE_WORD(p_data->ota_cfg.p_burn_buf[6], p_data->ota_cfg.p_burn_buf[7]));

  block_num = MAKE_WORD(p_data->ota_cfg.p_burn_buf[8], p_data->ota_cfg.p_burn_buf[9]);

  bh_size = MAKE_WORD(p_data->ota_cfg.p_burn_buf[10], p_data->ota_cfg.p_burn_buf[11]);

  block_offset = dmh_offset + 28;//skip indicator, blocks size & system info.

  if(SUCCESS != charsto_read(p_dev, block_offset, (u8 *)blocks, block_num * bh_size))
  {
      return (0);
  }

  block_offset += (block_num * bh_size);

  block_offset = blocks[0].base_addr;

  //to save memory, just burn dmh2, skip bootloader.
  if(SUCCESS != charsto_read(p_dev,
    dmh_offset, p_data->ota_cfg.p_burn_buf, flash_size - dmh_offset))
  {
      return (0);
  }

  for (i = 0; i < block_num; i++)
  {
    if(p_data->block_id == blocks[i].id)
    {
      blocks[i].size = p_data->buf_cnt;

      if(blocks[i].crc != 0x4352434e)
      {
        blocks[i].crc = p_data->crc32;
      }
      is_match = TRUE;
      is_found = TRUE;
    }
    else
    {
      tmp1 = blocks[i].base_addr + dmh_offset;
      is_match = FALSE;
    }

    if((BLOCK_TYPE_IW == blocks[i].type) || (BLOCK_TYPE_PIECE == blocks[i].type))
    {
      tmp2 =(block_offset + (FLASH_SECTION_SIZE - 1)) / FLASH_SECTION_SIZE * FLASH_SECTION_SIZE;

      if(tmp2 > block_offset)
      {
        memset(p_data->ota_cfg.p_burn_buf + block_offset, 0xFF, tmp2 - block_offset);
      }

      block_offset = tmp2;
    }

    blocks[i].base_addr = block_offset;

    if(!is_match)
    {
      if (SUCCESS != charsto_read(p_dev,
        tmp1, p_data->ota_cfg.p_burn_buf + blocks[i].base_addr, blocks[i].size))
      {
        return (0);
      }

      block_offset += blocks[i].size;
    }
    else
    {
      blocks[i].ota_ver = p_data->otai.new_software_version;

      if(p_data->is_zipped)
      {
        memcpy(p_data->ota_cfg.p_burn_buf + blocks[i].base_addr,
          p_data->ota_cfg.p_unzip_buf, p_data->buf_cnt);

        block_offset += p_data->buf_cnt;
      }
      else
      {
        memcpy(p_data->ota_cfg.p_burn_buf + blocks[i].base_addr,
          p_data->ota_cfg.p_save_buf, p_data->buf_cnt);

        block_offset += p_data->buf_cnt;
      }
    }
  }

  //reset block head.
  memcpy(p_data->ota_cfg.p_burn_buf + 28, (u8 *)blocks, block_num * bh_size);

  if(is_found)
  {
    return (p_data->ota_cfg.flash_size - dmh_offset);
  }
  else
  {
    return 0;
  }
}

static u32 find_bh_addr(u8 block_id)
{
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 i = 0;
  u32 dm_head = 0;
  u32 init_start = 0;
  dmh_block_info_t dmh = { 0 };
  dm_base_info_bl_t temp_info = {0};
  u32 start_data1 = 0;
  u32 start_data2 = 0;
  u8 k = 0;
  //scan DMH
  for(k = 0; k < 3; k++)  //can support 4 head
  {
    temp_info.block_num = 0;
    for (i = 0; i < 1000; i++)
    {
      charsto_read(p_charsto_dev, init_start, (u8 *)&start_data1, 4);
      charsto_read(p_charsto_dev, init_start + 4, (u8 *)&start_data2, 4);

      if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
      {
        //found
          dm_head = init_start;
          charsto_read(p_charsto_dev, dm_head + 16, (u8 *)&temp_info, 12);
          init_start += 0x400; //for find next head
          break;
      }
      init_start += 0x400;
    }
    OS_PRINTF("\r\n[OTA] %s:block_num[0x%x] ",__FUNCTION__, temp_info.block_num);

    //scan block header

    for(i = 0;  i < temp_info.block_num; i++)
    {
      u32 bh = dm_head + 16 + 12 + i * sizeof(dmh_block_info_t);
      charsto_read(p_charsto_dev, bh, (u8 *)&dmh, sizeof(dmh_block_info_t));

      if(block_id == dmh.id)
      {
        return bh;
      }
    }
  }

  return 0;
}

BOOL is_burn_finish(void)
{
  dmh_block_info_t dmh = { 0 };
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 addr = find_bh_addr(MAINCODE_BLOCK_ID);

  charsto_read(p_charsto_dev, addr, (u8 *)&dmh, sizeof(dmh_block_info_t));

  return (BURN_DEF_FLAG == dmh.reserve || BURN_DONE_FLAG == dmh.reserve
    || 0 == dmh.reserve);
}

static void set_burning_flag(u8 *p_buf)
{
  dmh_block_info_t *p_dmh = (dmh_block_info_t *)get_block_head_addr(p_buf,
    find_dmh_in_mem(p_buf), MAINCODE_BLOCK_ID);
  p_dmh->reserve = BURNING_FLAG;
}

static void set_burn_done_flag(void)
{
  u16 flag = 0;
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 addr = find_bh_addr(MAINCODE_BLOCK_ID);
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  addr += 46;

  charsto_read(p_charsto_dev, addr, (u8 *)&flag, 2);
  OS_PRINTF("flag 0x%x\n", flag);
  flag = BURN_DONE_FLAG;
  OS_PRINTF("addr: 0x%x\n", addr);


  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);

  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  charsto_writeonly(p_charsto_dev, addr, (u8 *)&flag, 2);

  //restore
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

  //verify
  OS_PRINTF("addr: 0x%x\n", addr);

  charsto_read(p_charsto_dev, addr, (u8 *)&flag, 2);
  OS_PRINTF("0x%x\n", flag);
}

static u8 *get_block(u8 *p_start, u32 offset,u8 block_id)
{
  dmh_block_info_t dmh = { 0 };
  dm_base_info_bl_t temp_info = {0};
  dm_base_info_bl_t  *p_dm_info = NULL;
  u32 i = 0;

  p_dm_info = &temp_info;

  memcpy(p_dm_info, p_start + offset + 16, 12);

  if(p_dm_info->bh_size != sizeof(dmh_block_info_t))
  {
    return NULL;
  }

  for(i = 0;  i < p_dm_info->block_num; i++)
  {
    memcpy((u8 *)&dmh, p_start + offset + 16 + 12 +
      i * sizeof(dmh_block_info_t), sizeof(dmh_block_info_t));

    if(block_id == dmh.id)
    {
      return (u8 *)(p_start + offset + dmh.base_addr);
    }
  }

  return NULL;
}

static u8 *get_block_head_addr(u8 *p_start, u32 offset,u8 block_id)
{
  dmh_block_info_t dmh = { 0 };
  dm_base_info_bl_t temp_info = {0};
  dm_base_info_bl_t  *p_dm_info = NULL;
  u32 i = 0;

  p_dm_info = &temp_info;

  memcpy(p_dm_info, p_start + offset + 16, 12);

  if(p_dm_info->bh_size != sizeof(dmh_block_info_t))
  {
    return NULL;
  }

  for(i = 0;  i < p_dm_info->block_num; i++)
  {
    memcpy((u8 *)&dmh, p_start + offset + 16 + 12 +
      i * sizeof(dmh_block_info_t), sizeof(dmh_block_info_t));

    if(block_id == dmh.id)
    {
      return (u8 *)(p_start + offset + 16 + 12 +
      i * sizeof(dmh_block_info_t));
    }
  }

  return NULL;
}

static u32 find_dmh_in_mem(u8 *p_mem)
{
  u32 i = 0;
  u32 dm_head_start = 0;
  u32 init_start = (u32)p_mem;
  u32 start_data1 = 0;
  u32 start_data2 = 0;
  dm_base_info_bl_t temp_info = {0};

  for(i = 0; i < 32; i++)
  {
    start_data1 = *((unsigned int *)init_start);
    start_data2 = *((unsigned int *)(init_start + 4));

    if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
    {
      temp_info.block_num = 0;
      dm_head_start = init_start;
      temp_info = *((dm_base_info_bl_t *)(init_start + 16));
      if(temp_info.block_num <= 2)  //the backup head
      {
          init_start += 0x10000;
          continue;
      }
      return (dm_head_start - (u32)p_mem);
    }

    init_start += 0x10000;
  }

  return 0;
}

static void notify_progress(u32 cur_size, u32 full_size)
{
  event_t evt;
  u8 progress = 0;

  progress = (u8)(100 * cur_size / full_size);
  OS_PRINTF("[OTA] %s(Line: %d): ota_progress_bak = %d, progress = %d.\n",
    __FUNCTION__, __LINE__, ota_progress_bak, progress);
  if(ota_progress_bak != progress)
  {
    ota_progress_bak = progress;

    evt.id = OTA_EVT_PROGRESS;
    evt.data1 = progress;
    ap_frm_send_evt_to_ui(APP_OTA, &evt);
  }

  return;
}

/*!
   send ota's evt out.

   \param[in] id: sent event id
  */
static void send_msg_to_ui(u32 id, u32 para1, u32 para2)
{
  event_t evt = {0};

  evt.id = id;
  evt.data1 = para1;
  evt.data2 = para2;
  ap_frm_send_evt_to_ui(APP_OTA, &evt);
}

static BOOL check_sec_crc(void *p_handler, dvb_section_t *p_sec)
{
  u16 section_len = 0;
  u32 crc = 0;
  ota_handle_t *p_handle = (ota_handle_t *)p_handler;
  ota_policy_t *p_policy = p_handle->p_policy;

  section_len = MAKE_WORD(p_sec->p_buffer[2], (p_sec->p_buffer[1] & 0x0f));
  section_len += 3;

  crc = p_policy->ota_crc32_generate((u8 *)p_sec->p_buffer, section_len - 4);
  return (crc == make32(p_sec->p_buffer + section_len - 4));
}


static u32 ota_get_type_size(dsmcc_dii_type_info_t *p_dii)
{
  u32 i = 0;
  u32 size = 0;

  for(i = 0; i < p_dii->module_num; i++)
  {
    size += p_dii->mod_info[i].mod_size;
  }

  return size;
}

static void ota_request_dsi_single(service_t *p_dvb_svc, u16 pid, u8 ts_in)
{
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_DSMCC_MSG;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = (u32)((pid << 16) | DSMCC_MSG_DSI);
  dvb_req.para2 = 0;
  dvb_req.ts_in = ts_in;
  dvb_req.context = (u32)((pid << 16) | DSMCC_MSG_DSI);
  p_dvb_svc->do_cmd(p_dvb_svc,
    DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));
}

static void ota_request_dii_single(service_t *p_dvb_svc, u16 pid, u8 group_id, u32 ts_in)
{
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_DSMCC_MSG;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.period = 0;
  dvb_req.para1 = (u32)((pid << 16) | DSMCC_MSG_DII);
  dvb_req.para2 = (u32)group_id;
  dvb_req.context = (u32)((pid << 16) | DSMCC_MSG_DII);
  dvb_req.ts_in = ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));
}

static void ota_request_ddm_multi(service_t *p_dvb_svc,
  u16 pid, filter_ext_buf_t *p_dsmcc_extern_buf, u8 group_id, u8 type_idx, u8 ts_in)
{
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_DSMCC_DDM;
  dvb_req.req_mode = DATA_MULTI;
  dvb_req.para1 = (u32)((pid << 16) | (group_id << 8) | type_idx);
  dvb_req.para2 = (u32)p_dsmcc_extern_buf;
  dvb_req.ts_in = ts_in;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_REQUEST, (u32)&dvb_req, sizeof(dvb_request_t));
}

static void ota_free_ddm_multi(service_t *p_dvb_svc,
  u16 pid, filter_ext_buf_t *p_dsmcc_extern_buf)
{
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_DSMCC_DDM;
  dvb_req.req_mode = DATA_MULTI;
  dvb_req.period = 0;
  dvb_req.para1 = (u32)pid;
  dvb_req.para2 = (u32)p_dsmcc_extern_buf;
  p_dvb_svc->do_cmd(p_dvb_svc, DVB_FREE, (u32)&dvb_req, sizeof(dvb_request_t));
}

/*!
   process the message methdd, it will be called by ota task.

   \param[in] handle ota handle
   \param[in] p_msg new message
  */
static void ota_single_step(class_handle_t p_handler, os_msg_t *p_msg)
{
  //u32 i = 0;
  ota_handle_t *p_handle = (ota_handle_t *)p_handler;
  dvb_t *p_dvb = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  ota_policy_t *p_policy = p_handle->p_policy;
  ota_priv_data_t *p_data = &p_handle->data;
  dsmcc_dsi_info_t *p_dsi = NULL;
  u16 table_id = 0;
  u16 data_len = 0;
  dvb_section_t *p_sec = NULL;
  u8 *p_buf = NULL;
  dsmcc_ddb_t *p_ddb = NULL;
  u8 byte[4] = {0};
  u32 unzip_size = 0, burn_size = 0;
  u32 off_in_mem = 0;
  u8 *p_ssdata_start = NULL;
  u16 new_ver = 0, old_ver = 0;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  dmh_block_info_t dm_head = {0};
  u8 unzip_ret = 0;
  BOOL burn_ret = FALSE;
  u8 mod_id = 0;
  u16 sec_num = 0;
  u16 i = 0;
  u32 ticks_now = 0;
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  u8 tuner_id = 0, ts_in = 0;

  nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);

  if(p_msg != NULL)
  {
    switch(p_msg->content)
    {
      case OTA_CMD_START_SEARCHING:
        p_handle->data.p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_OTA);
        if(0 != p_msg->para1) //user set info
        {
          memcpy(&p_data->otai, (ota_info_t *)p_msg->para1, sizeof(ota_info_t));
        }
        else  //default info
        {
            p_policy->ota_read_otai(&p_data->otai);
        }
        p_data->tuner_id = p_data->otai.locks.sat_rcv.tuner_index;
        nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
        if(0 != p_msg->para2)
        {
            p_data->is_user_force = TRUE;
        }
        p_data->sm = OTA_SM_SEARCHING;
        break;

      case OTA_CMD_STOP_SEARCHING:
        p_data->sm = OTA_SM_IDLE;

        if(p_msg->para1 != 0)
        {
          p_data->otai.ota_tri = OTA_TRI_AUTO;
          p_policy->ota_write_otai(&(p_data->otai));

          send_msg_to_ui(OTA_EVT_SEARCH_STOPPED, 0, 0);
          send_msg_to_ui(OTA_EVT_TRIGGER_RESET, 0, 0);
        }
        else
        {

          send_msg_to_ui(OTA_EVT_SEARCH_STOPPED, 0, 0);
        }
        break;

      case OTA_CMD_START:
        if(0 != p_msg->para2)
        {
            p_data->is_user_force = TRUE;
        }
        p_policy->ota_read_otai(&p_data->otai);
        p_data->tuner_id = p_data->otai.locks.sat_rcv.tuner_index;
        nc_get_ts_by_tuner(nc_handle, p_data->tuner_id, &p_data->ts_in);
        OS_PRINTF("\r\n[OTA] %s:ota_tri[0x%x],pid[0x%x] ",
                            __FUNCTION__,p_data->otai.ota_tri,p_data->otai.download_data_pid);
        if((OTA_TRI_AUTO == p_data->otai.ota_tri)
            ||(OTA_TRI_MON_IN_LOADER == p_data->otai.ota_tri)) /*monitor in ota loader*/
        {
          p_data->sm = OTA_SM_LOCKING;
        }
        else if(OTA_TRI_FORC == p_data->otai.ota_tri)
        {
          send_msg_to_ui(OTA_EVT_UNLOCKED, 0, 0);
          p_data->sm = OTA_SM_IDLE;
        }
        else
        {
          OS_PRINTF("\r\n[OTA]error! FUNC[%s] LINE[%d],ota_tri[0x%x] ",
                              __FUNCTION__, __LINE__, p_data->otai.ota_tri);
        }
        break;

      case OTA_CMD_STOP:
        if(p_data->sm == OTA_SM_BURNNING)
        {
          break;
        }
        if((OTA_TRI_MON_IN_LOADER != p_data->otai.ota_tri)
            &&(OTA_TRI_MONITOR != p_data->otai.ota_tri))
        {
            p_data->otai.ota_tri = OTA_TRI_NONE;
            p_policy->ota_write_otai(&p_data->otai);
            #ifndef WIN32
            mtos_task_delay_ms(1000);
			hal_watchdog_init(0);
  			hal_watchdog_enable();
            //hal_pm_reset();
            #endif
            OS_PRINTF("\r\n[OTA][%s] ota_tri[0x%x] OTA_CMD_STOP ",
                              __FUNCTION__, p_data->otai.ota_tri);
        }
        p_data->sm = OTA_SM_IDLE;
        break;

      case OTA_CMD_RELOCK:
        OS_PRINTF("relock\n");
        memset(&p_data->otai, 0, sizeof(ota_info_t));

        memcpy(&p_data->otai, (void *)p_msg->para1, sizeof(ota_info_t));

        p_data->otai.ota_tri = OTA_TRI_FORC;

        p_data->sm = OTA_SM_LOCKING;
        break;

      case OTA_CMD_CHECK_MAIN:
        OS_PRINTF("lock main\n");
        memset(&p_data->otai, 0, sizeof(ota_info_t));

        memcpy(&p_data->otai, (void *)p_msg->para1, sizeof(ota_info_t));

        p_data->otai.ota_tri = (u32)p_msg->para2;

        if((p_data->otai.ota_tri != OTA_TRI_FORC) &&
          (p_data->otai.ota_tri != OTA_TRI_MON_IN_LOADER) &&
          (p_data->otai.ota_tri != OTA_TRI_AUTO) &&
          (p_data->otai.ota_tri != OTA_TRI_MONITOR))
        {
          return;
        }

        p_data->sm = OTA_SM_LOCKING;
        break;

      case NC_EVT_LOCKED:
        tuner_id = p_msg->context;
        OS_PRINTF("\r\n[OTA] locked");
        if(p_data->sm == OTA_SM_WAIT_LOCK)
        {
          if(OTA_TRI_MON_IN_LOADER != p_data->otai.ota_tri)
          {
            send_msg_to_ui(OTA_EVT_LOCKED, 0, 0);
          }
          p_data->sm = OTA_SM_SEARCHING;
        }
        break;

      case NC_EVT_UNLOCKED:
        tuner_id = p_msg->context;
        if(p_data->sm == OTA_SM_WAIT_LOCK)
        {
          OS_PRINTF("\r\n[OTA] %s:ota_tri[%d],NC_EVT_UNLOCKED ",__FUNCTION__, p_data->otai.ota_tri);
          if(OTA_TRI_MON_IN_LOADER != p_data->otai.ota_tri)
          {
            send_msg_to_ui(OTA_EVT_UNLOCKED, 0, 0);
          }
          p_data->sm = OTA_SM_IDLE;
        }
        break;

      case OTA_CMD_BURN_START:
        OS_PRINTF("\r\n recv msg OTA_SM_BURNNING ");
        if(p_data->sm == OTA_SM_BURN_CHECK)
        {
          if(p_msg->para1)
          {
            p_data->sm = OTA_SM_BURNNING;
          }
          else
          {
            #ifndef WIN32
            mtos_task_delay_ms(1000);
            //hal_pm_reset();
            hal_watchdog_init(0);
  			hal_watchdog_enable();
            #endif
            p_data->sm = OTA_SM_IDLE;
          }
        }
        break;


      case DVB_DSMCC_DSI_FOUND:
        OS_PRINTF("\r\n[OTA] dsi found");
        if(p_data->sm == OTA_SM_WAIT_DSI)
        {
          p_dsi = (dsmcc_dsi_info_t *)p_msg->para1;
          for(i = 0; i < p_dsi->group_num; i ++)
          {
            if(p_dsi->group_info[i].oui == p_data->tdi.oui
              && p_dsi->group_info[i].hw_mod_id == p_data->tdi.hw_mod_id
              && p_dsi->group_info[i].hw_version == p_data->tdi.hw_version
              && p_dsi->group_info[i].sw_mod_id == p_data->tdi.sw_mod_id)
            {
              break;
            }

          }

          if(i >= p_dsi->group_num)
          {
            p_data->sm = OTA_SM_IDLE;
            break;
          }

          p_data->block_id = p_dsi->group_info[i].sw_block_id;
          p_data->total_len = p_dsi->group_info[i].file_total_size;
          p_data->is_zipped = p_dsi->group_info[i].sw_is_zipped;
          p_data->otai.new_software_version = p_dsi->group_info[i].sw_version;
          p_data->group_id = p_dsi->group_info[i].group_id;

          if(p_data->block_id == 0xFF)
          {
            p_data->is_upgrade_all = TRUE;
            p_data->is_split = p_dsi->group_info[i].sw_is_spilt;
            old_ver = p_data->otai.orig_software_version;
          }
          else
          {
            p_data->is_upgrade_all = FALSE;
            p_data->is_split = FALSE;
            dm_get_block_header(p_dm_handle, p_data->block_id, &dm_head);
            old_ver = dm_head.ota_ver;
          }
          OS_PRINTF("\r\n[OTA] %s:old_ver[%d],new_ver[%d],block_id[0x%x] ",__FUNCTION__,
                  old_ver,p_data->otai.new_software_version,p_data->block_id);

          if((old_ver >= p_data->otai.new_software_version)
              &&(OTA_TRI_FORC != p_data->otai.ota_tri)
              &&(!p_data->is_user_force))
          {
              if(p_data->otai.ota_tri == OTA_TRI_MONITOR)
              {
                p_data->monitor_info.is_restart = TRUE;
                p_data->monitor_info.pre_monitor_ticks = mtos_ticks_get();
              }
              p_data->sm = OTA_SM_IDLE;
              break;
          }

          if((OTA_TRI_NONE == p_data->otai.ota_tri)
            ||(OTA_TRI_MONITOR == p_data->otai.ota_tri))
          {
            send_msg_to_ui(OTA_EVT_UPG_INFO_FOUND,
                      (p_data->otai.new_software_version << 16 | old_ver), p_data->block_id);
            p_data->sm = OTA_SM_IDLE;
          }
          else
          {
            if(OTA_TRI_MON_IN_LOADER == p_data->otai.ota_tri)
            {
              send_msg_to_ui(OTA_EVT_UPG_INFO_FOUND, 0, 0);
            }
            p_policy->ota_cfg(&(p_data->ota_cfg), p_data->is_upgrade_all, p_data->is_zipped);

            ota_request_dii_single(
              p_data->p_dvb_svc, p_data->otai.download_data_pid, p_data->group_id, p_data->ts_in);

            p_data->sm = OTA_SM_WAIT_DII;
          }
        }
        break;

      case DVB_DSMCC_DII_FOUND:
        OS_PRINTF("\r\n[OTA] dii found ");
        if(p_data->sm == OTA_SM_WAIT_DII)
        {
          memcpy(&p_data->dii_info, (dsmcc_dii_info_t *)p_msg->para1, sizeof(dsmcc_dii_info_t));

          p_data->received_len = 0;
          p_data->buf_cnt = 0;

          p_data->type_num = p_data->dii_info.type_num;

          OS_PRINTF("type num %d\n", p_data->type_num);

          for(i = 0; i < MODULE_TYPE_NUM; i++)
          {
            p_data->type_len[i] = ota_get_type_size(&p_data->dii_info.dii_type_info[i]);

            OS_PRINTF("%d mod type %d, length %d\n",
              i, p_data->dii_info.dii_type_info[i].mod_type, p_data->type_len[i]);
          }

          if(p_data->is_upgrade_all)
          {
            if(p_data->is_split)
            {
              p_data->type_idx = 1;
            }
            else
            {
              p_data->type_idx = 0;
            }
          }
          else
          {
            p_data->type_idx = 0;
          }

          p_data->sm = OTA_SM_DOWNLOADING;
          p_data->p_mod_info = mtos_malloc(sizeof(mod_info_t) * MAX_MODULE_NUM_PER_TYPE);
          CHECK_FAIL_RET_VOID(p_data->p_mod_info != NULL);
        }
        break;

      case DVB_ONE_SEC_FOUND:
        //OS_PRINTF("section found\n");
        if(p_data->sm == OTA_SM_WAIT_DDM)
        {
          p_sec = (dvb_section_t *)p_msg->para1;
          p_buf = p_sec->p_buffer;
          table_id = p_buf[0];

          if(DVB_TABLE_ID_DSMCC_DDM == table_id)
          {
            mod_id = (p_buf[20] << 8 | p_buf[21]);

            sec_num = p_buf[6];

            if(p_data->p_mod_info[mod_id].section_info[sec_num].is_received == TRUE)
            {
              //OS_PRINTF("dp\n");//, id[%d], sec[%d]\n", mod_id, sec_num);
              break;
            }

            if (p_sec->crc_enable && (!check_sec_crc(p_handler, p_sec)))
            {
              //OS_PRINTF("crc error\n");
              break;
            }

            data_len = (u16)(p_msg->para2 & 0xFFFF);

            memcpy(p_data->ota_cfg.p_sec_buf->p_buffer, p_buf, data_len);

            parse_dsmcc_db_section(p_data->p_dvb_svc, p_data->ota_cfg.p_sec_buf);
          }
        }
        break;


      case DVB_DSMCC_DDM_FOUND:
        //OS_PRINTF("ddm found\n");
        if(p_data->sm == OTA_SM_WAIT_DDM)
        {
          u32 offset = 0;

          p_ddb = (dsmcc_ddb_t *)p_msg->para1;
          mod_id = p_ddb->ddb_hdr.mod_id;

          //OS_PRINTF("sv[%d][%d]\n", mod_id, p_ddb->sec_num);
          if(TRUE == p_data->p_mod_info[mod_id].section_info[p_ddb->sec_num].is_received)
            break;
          offset = mod_id * 256 * 4066 + p_ddb->sec_num * 4066;
          memcpy(p_data->ota_cfg.p_save_buf + offset, p_ddb->content, p_ddb->content_len);
          p_data->p_mod_info[mod_id].section_info[p_ddb->sec_num].is_received = TRUE;
          p_data->buf_cnt += p_ddb->content_len;
          notify_progress(p_data->buf_cnt, p_data->type_len[p_data->type_idx]);

          //to judge if the upgrade file is totally download.
          //OS_PRINTF("buf_cnt = %d, type_len[%d]=%d\n", p_data->buf_cnt, p_data->typ
          //e_idx, p_data->type_len[p_data->type_idx]);
          if(p_data->buf_cnt == p_data->type_len[p_data->type_idx])
          {
            ota_free_ddm_multi(p_data->p_dvb_svc,
              p_data->otai.download_data_pid, p_data->ota_cfg.p_dsmcc_extern_buf);

            OS_PRINTF("set sm to OTA_SM_UPDATE\n");
            p_data->sm = OTA_SM_UPDATE;
          }
        }
        break;

      case DVB_TABLE_TIMED_OUT:
        switch(p_msg->para1)
        {
          case DVB_TABLE_ID_DSMCC_MSG:
            if((p_msg->context & 0xFFFF) == DSMCC_MSG_DII)
            {
              ota_request_dii_single(
                p_data->p_dvb_svc, p_data->otai.download_data_pid, p_data->group_id, p_data->ts_in);
            }
            else if((p_msg->context & 0xFFFF) == DSMCC_MSG_DSI)
            {
              if(p_data->otai.ota_tri == OTA_TRI_MON_IN_LOADER)
              {
                  send_msg_to_ui(OTA_EVT_TABLE_TIMEOUT, 0, 0);
                  p_data->sm = OTA_SM_IDLE;
              }
              else
              {
                ota_request_dsi_single(
                    p_data->p_dvb_svc, p_data->otai.download_data_pid, p_data->ts_in);
              }
            }
            break;

          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  else
  {
    switch(p_data->sm)
    {
      case OTA_SM_IDLE:
        p_data->is_user_force = FALSE;
        if(OTA_TRI_MON_IN_LOADER == p_data->otai.ota_tri)
        {
            OS_PRINTF("\r\n[OTA] %s:OTA_EVT_LOADER_MON_END ",__FUNCTION__);
            send_msg_to_ui(OTA_EVT_LOADER_MON_END, 0, 0);
        }
        else if((OTA_TRI_MONITOR == p_data->otai.ota_tri)
                  &&(TRUE == p_data->monitor_info.is_restart))
        {
              ticks_now = mtos_ticks_get();
              if((ticks_now - p_data->monitor_info.pre_monitor_ticks) > 1000)// 10s (10ms/ticks)
              {
                  OS_PRINTF("\r\n[OTA] %s:request dsi again ",__FUNCTION__);
                  p_data->monitor_info.is_restart = FALSE;
                  ota_request_dsi_single(p_data->p_dvb_svc,
                    p_data->otai.download_data_pid, p_data->ts_in);
                  p_data->sm = OTA_SM_WAIT_DSI;
              }
        }
        break;

      case OTA_SM_LOCKING:
        nc_handle = class_get_handle_by_id(NC_CLASS_ID);

        switch(p_data->otai.sys_mode)
        {
          case SYS_DVBS:
            nc_set_diseqc(nc_handle, p_data->tuner_id, &(p_data->otai.locks.disepc_rcv));

            nc_set_polarization(nc_handle, p_data->tuner_id,
              p_data->otai.locks.tp_rcv.polarity);

            nc_set_22k(nc_handle, p_data->tuner_id, p_data->otai.locks.tp_rcv.is_on22k);

            p_data->nc_info.polarization = p_data->otai.locks.tp_rcv.polarity;

            p_data->nc_info.channel_info.frequency =
              dvbs_calc_mid_freq(&(p_data->otai.locks.tp_rcv), &(p_data->otai.locks.sat_rcv));

            p_data->nc_info.channel_info.param.dvbs.symbol_rate
              = p_data->otai.locks.tp_rcv.sym;

            p_data->nc_info.channel_info.param.dvbs.fec_inner
              = NIM_CR_AUTO;

            p_data->nc_info.onoff22k = p_data->otai.locks.tp_rcv.is_on22k;

            nc_set_tp(nc_handle, p_data->tuner_id, p_data->p_nim_svc, &(p_data->nc_info));
            break;

          case SYS_DVBC:
            nc_set_blind_scan_mode(nc_handle, p_data->tuner_id, FALSE);

            p_data->nc_info.channel_info.frequency
              = p_data->otai.lockc.tp_freq * 1000;

            p_data->nc_info.channel_info.param.dvbc.symbol_rate
              = p_data->otai.lockc.tp_sym;

            p_data->nc_info.channel_info.param.dvbc.modulation
              = p_data->otai.lockc.nim_modulate;

            p_data->nc_info.polarization = NIM_PORLAR_HORIZONTAL;
            p_data->nc_info.channel_info.spectral_polar = NIM_IQ_AUTO;
            p_data->nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK;

            nc_set_tp(nc_handle, p_data->tuner_id, p_data->p_nim_svc, &(p_data->nc_info));
            break;

          case SYS_DTMB:
            nc_set_blind_scan_mode(nc_handle, p_data->tuner_id, FALSE);

            p_data->nc_info.channel_info.frequency
              = p_data->otai.lockt.tp_freq * 1000;
            p_data->nc_info.channel_info.param.dvbt.band_width
              = p_data->otai.lockt.band_width;
            p_data->nc_info.channel_info.param.dvbt.nim_type
              = p_data->otai.lockt.nim_type;
            nc_set_tp(nc_handle, p_data->tuner_id, p_data->p_nim_svc, &(p_data->nc_info));
            break;

          case SYS_DVBT2:
            nc_set_blind_scan_mode(nc_handle, p_data->tuner_id, FALSE);

            p_data->nc_info.channel_info.frequency
              = p_data->otai.lockt.tp_freq * 1000;
            p_data->nc_info.channel_info.param.dvbt.band_width
              = p_data->otai.lockt.band_width;
            p_data->nc_info.channel_info.param.dvbt.nim_type
              = p_data->otai.lockt.nim_type;

            nc_set_tp(nc_handle, 0, p_data->p_nim_svc, &(p_data->nc_info));
            break;

          default:
            return;
        }

        p_data->sm = OTA_SM_WAIT_LOCK;
        break;

      case OTA_SM_WAIT_LOCK:
        break;

      case OTA_SM_USER_INPUT:
        break;

      case OTA_SM_SEARCHING:
        p_data->p_dvb_svc->do_cmd(p_data->p_dvb_svc, DVB_TABLE_TS_UPDATE, 0,  p_data->ts_in);
        ota_request_dsi_single(p_data->p_dvb_svc, p_data->otai.download_data_pid,
            p_data->ts_in);
        p_data->sm = OTA_SM_WAIT_DSI;
        break;

      case OTA_SM_DOWNLOADING:
        send_msg_to_ui(OTA_EVT_START_DL, 0, 0);
        memset(p_data->p_mod_info, 0, sizeof(mod_info_t) * MAX_MODULE_NUM_PER_TYPE);
        if(p_data->is_upgrade_all)
        {
          OS_PRINTF("mod_type = %d\n", p_data->dii_info.dii_type_info[p_data->type_idx].mod_type);
          ota_request_ddm_multi(p_data->p_dvb_svc,
            p_data->otai.download_data_pid, p_data->ota_cfg.p_dsmcc_extern_buf,
            p_data->group_id, p_data->dii_info.dii_type_info[p_data->type_idx].mod_type,
            p_data->ts_in);
        }
        else
        {
          OS_PRINTF("group_id=%d, pid=%d\n", p_data->group_id, p_data->otai.download_data_pid);
          //ota_request_ddm_multi(p_data->p_dvb_svc,
            //p_data->otai.download_data_pid, p_data->ota_cfg.p_dsmcc_extern_buf,
            //p_data->group_id, 0xFF);
          ota_request_ddm_multi(p_data->p_dvb_svc,
            p_data->otai.download_data_pid, p_data->ota_cfg.p_dsmcc_extern_buf,
            p_data->group_id, p_data->dii_info.dii_type_info[p_data->type_idx].mod_type,
            p_data->ts_in);
        }

        OS_PRINTF("set sm to OTA_SM_WAIT_DDM\n");
        p_data->sm = OTA_SM_WAIT_DDM;
        break;

      case OTA_SM_UPDATE:
        notify_progress(100, 100);

        //check the upg file's crc.
        byte[0] = *((u8 *)(p_data->ota_cfg.p_save_buf + p_data->buf_cnt - 4));
        byte[1] = *((u8 *)(p_data->ota_cfg.p_save_buf + p_data->buf_cnt - 3));
        byte[2] = *((u8 *)(p_data->ota_cfg.p_save_buf + p_data->buf_cnt - 2));
        byte[3] = *((u8 *)(p_data->ota_cfg.p_save_buf + p_data->buf_cnt - 1));
        p_data->crc32 = ((byte[0]) << 24 | (byte[1] << 16) | (byte[2] << 8) | (byte[3]));
        OS_PRINTF("buf_cnt = %d\n");
        OS_PRINTF("crc32=0x%x\n", p_data->crc32);
        p_data->buf_cnt -= 4;
        OS_PRINTF("policy crc=0x%x\n",
            p_policy->ota_crc32_generate(p_data->ota_cfg.p_save_buf, p_data->buf_cnt));

        if(p_data->crc32 ==
          p_policy->ota_crc32_generate(p_data->ota_cfg.p_save_buf, p_data->buf_cnt))
        {
          //To recaculate the crc by different mode according to bootloader & anchor.
          crc_setup_fast_lut(CRC32_ARITHMETIC);

          p_data->crc32 = crc_fast_calculate(
              CRC32_ARITHMETIC, 0xFFFFFFFF, p_data->ota_cfg.p_save_buf, p_data->buf_cnt);

          //To init ota crc again.
          p_policy->ota_crc_init();
          OS_PRINTF("OTA_SM_UPDATE: CRC OK\n");
          send_msg_to_ui(OTA_EVT_CRC_OK, 0, 0);

          if(p_data->is_zipped)
          {
            CHECK_FAIL_RET_VOID(NULL != p_data->ota_cfg.p_fake_buf);

            init_fake_mem_lzma((void *)p_data->ota_cfg.p_fake_buf,
              p_data->ota_cfg.fake_size);

            if(p_data->is_upgrade_all)
            {
              unzip_size = p_data->ota_cfg.burn_size;
              OS_PRINTF("burn_size = %d\n", p_data->ota_cfg.burn_size);
              unzip_ret = lzma_decompress((void *)(p_data->ota_cfg.p_burn_buf),
                &unzip_size, (void *)p_data->ota_cfg.p_save_buf,
                p_data->buf_cnt);
              OS_PRINTF("OTA_SM_UPDATE: unzip_size = %d, buf_cnt=%d\n",
                unzip_size, p_data->buf_cnt);
            }
            else
            {
              unzip_size = p_data->ota_cfg.unzip_size;

              unzip_ret = lzma_decompress((void *)(p_data->ota_cfg.p_unzip_buf),
                &unzip_size, (void *)p_data->ota_cfg.p_save_buf,
                p_data->buf_cnt);
              OS_PRINTF("OTA_SM_UPDATE: unzip_size = %d\n", unzip_size);
            }

            p_data->buf_cnt = unzip_size;


            if(unzip_ret == 0)
            {
              send_msg_to_ui(OTA_EVT_UNZIP_OK, 0, 0);

              p_data->sm = OTA_SM_BURN_CHECK;
              //p_data->sm = OTA_SM_BURNNING;
            }
            else
            {
              OS_PRINTF("OTA_SM_UPDATE: UPZIP FAILED\n");
              send_msg_to_ui(OTA_EVT_UNZIP_FAILED, 0, 0);

              p_data->sm = OTA_SM_IDLE;
            }
          }
          else
          {
            send_msg_to_ui(OTA_EVT_UNZIP_OK, 0, 0);

            p_data->sm = OTA_SM_BURN_CHECK;
            //p_data->sm = OTA_SM_BURNNING;
          }
        }
        else
        {
          OS_PRINTF("OTA_SM_UPDATE:CRC failed\n");
          send_msg_to_ui(OTA_EVT_CRC_FAILED, 0, 0);


          p_data->sm = OTA_SM_IDLE;
        }
        break;

      case OTA_SM_BURN_CHECK:

        break;

      case OTA_SM_BURNNING:

        send_msg_to_ui(OTA_EVT_BURNING, 0, 0);

        mtos_task_sleep(10);
        notify_progress(0, 100);
        mtos_task_sleep(10);


        OS_PRINTF("a[%d], z[%d], b[%d], s[%d]\n", p_data->is_upgrade_all, p_data->is_zipped,
                          p_data->block_id, p_data->buf_cnt);

        if(OTA_TRI_MON_IN_LOADER != p_data->otai.ota_tri)
        {
            p_data->otai.ota_tri = OTA_TRI_NONE;
        }
        p_policy->ota_write_otai(&p_data->otai);

        if(p_data->is_upgrade_all)
        {

          if(p_data->is_zipped)
          {
            OS_PRINTF("\r\n[OTA]p_data->type_idx=%d, start_addr = 0x%x, buf_cnt = 0x%d ",
               p_data->type_idx, p_data->dii_info.dii_type_info[p_data->type_idx].start_addr, \
               p_data->buf_cnt);

            if(p_data->dii_info.dii_type_info[p_data->type_idx].mod_type == MODULE_TYPE_RD)
              { //Read Only area
                OS_PRINTF("all,split,zip,... Read Only area\n");
                set_burning_flag(p_data->ota_cfg.p_burn_buf);

                off_in_mem = find_dmh_in_mem(p_data->ota_cfg.p_burn_buf);
                p_ssdata_start = get_block(p_data->ota_cfg.p_burn_buf,
                                                        off_in_mem, SS_DATA_BLOCK_ID);

                CHECK_FAIL_RET_VOID(p_ssdata_start != NULL);

                p_ssdata_start += 20;
                new_ver = p_data->otai.new_software_version;
                memcpy(p_ssdata_start, &new_ver, 2);
            }
            else if(p_data->dii_info.dii_type_info[p_data->type_idx].mod_type == MODULE_TYPE_ALL)
            { //all
              OS_PRINTF("\r\n[OTA] all,zip,... ");
              set_burning_flag(p_data->ota_cfg.p_burn_buf);
              off_in_mem = find_dmh_in_mem(p_data->ota_cfg.p_burn_buf);
              p_ssdata_start = get_block(p_data->ota_cfg.p_burn_buf, off_in_mem, SS_DATA_BLOCK_ID);

              CHECK_FAIL_RET_VOID(p_ssdata_start != NULL);

              p_ssdata_start += 20;
              new_ver = p_data->otai.new_software_version;
              memcpy(p_ssdata_start, &new_ver, 2);
            }

            burn_ret = p_policy->ota_burn_file_group(
              p_data->ota_cfg.p_burn_buf,
              p_data->dii_info.dii_type_info[p_data->type_idx].start_addr,
              p_data->buf_cnt, notify_progress);
          }
          else
          {
            if(p_data->dii_info.dii_type_info[p_data->type_idx].mod_type == MODULE_TYPE_RD)
            {
              OS_PRINTF("MODULE TYPE: READ ONLY\n");
              off_in_mem = find_dmh_in_mem(p_data->ota_cfg.p_save_buf);
              p_ssdata_start = get_block(p_data->ota_cfg.p_save_buf, off_in_mem, SS_DATA_BLOCK_ID);

              CHECK_FAIL_RET_VOID(p_ssdata_start != NULL);

              p_ssdata_start += 20;
              new_ver = p_data->otai.new_software_version;
              memcpy(p_ssdata_start, &new_ver, 2);

              set_burning_flag(p_data->ota_cfg.p_save_buf);
            }
            else if(p_data->dii_info.dii_type_info[p_data->type_idx].mod_type == MODULE_TYPE_ALL)
            {
              off_in_mem = find_dmh_in_mem(p_data->ota_cfg.p_save_buf);
              p_ssdata_start = get_block(p_data->ota_cfg.p_save_buf, off_in_mem, SS_DATA_BLOCK_ID);

              CHECK_FAIL_RET_VOID(p_ssdata_start != NULL);

              p_ssdata_start += 20;
              new_ver = p_data->otai.new_software_version;
              memcpy(p_ssdata_start, &new_ver, 2);
              set_burning_flag(p_data->ota_cfg.p_save_buf);
            }

            burn_ret = p_policy->ota_burn_file_group(
              p_data->ota_cfg.p_save_buf,
              p_data->dii_info.dii_type_info[p_data->type_idx].start_addr,
              p_data->buf_cnt, notify_progress);
          }
        }
        else
        {
          burn_size = ota_merge_blocks(p_data);

          if(burn_size == 0)
          {
            burn_ret = FALSE;
          }
          else
          {
            set_burning_flag(p_data->ota_cfg.p_burn_buf);

            burn_ret = p_policy->ota_burn_file_group(
              p_data->ota_cfg.p_burn_buf,
              0xFFFFFFFF,
              burn_size, notify_progress);
          }
        }

        if(burn_ret)
        {
          OS_PRINTF("\r\n[OTA] burn flash success! ");
          p_data->sm = OTA_SM_BURN_FINISHED;
        }
        else
        {
          OS_PRINTF("\r\n[OTA] burn flash failed! ");
          send_msg_to_ui(OTA_EVT_BURN_FAILED, 0, 0);
          p_data->sm = OTA_SM_IDLE;
        }
        break;

      case OTA_SM_BURN_FINISHED:
        if((p_data->dii_info.dii_type_info[p_data->type_idx].mod_type == MODULE_TYPE_RD)
          || (p_data->is_upgrade_all == FALSE))
        {
            OS_PRINTF("set burn done flag 1\n");
            set_burn_done_flag();
        }
        else if(p_data->dii_info.dii_type_info[p_data->type_idx].mod_type == MODULE_TYPE_ALL)
        {
            OS_PRINTF("set burn done flag 2\n");
            set_burn_done_flag();
        }

        p_data->type_idx++;

        OS_PRINTF("type_idx = %d, type_num = %d\n", p_data->type_idx, p_data->type_num);
        if(p_data->type_idx < p_data->type_num)
        {
          p_data->buf_cnt = 0;

          p_data->sm = OTA_SM_DOWNLOADING;

          memset(p_data->ota_cfg.p_save_buf, 0, p_data->ota_cfg.save_size);
        }
        else
        {
          send_msg_to_ui(OTA_EVT_UPG_COMPLETE, 0, 0);

          p_data->sm = OTA_SM_IDLE;

          mtos_task_sleep(2000);
        }
        break;

      default:
        break;
    }
  }
}

/*!
   Initialization function.

   \param[in] handle ota handle
  */
static void init(class_handle_t p_handler)
{
  ota_handle_t *p_handle = (ota_handle_t *)p_handler;
  ota_policy_t *p_policy = p_handle->p_policy;
  dvb_t *p_dvb = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  nim_ctrl_t *p_nc = class_get_handle_by_id(NC_CLASS_ID);
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  u8 tuner_id = 0, ts_in = 0;

  p_policy->ota_crc_init();

  p_policy->ota_init_tdi((void *)&p_handle->data.tdi);

  memset(&p_handle->data.dii_info, 0, sizeof(dsmcc_dii_info_t));

  p_handle->data.p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_OTA);

  p_handle->data.p_nim_svc = nc_get_svc_instance(p_nc, APP_OTA);
  nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);
  p_handle->data.tuner_id = tuner_id;
  p_handle->data.ts_in = ts_in;
}


/*!
   Call this method, load ota instance by the policy.

   \param[in] p_policy The policy of application ota
   \return Return app_t instance address
  */
app_t *construct_ap_ota(ota_policy_t *p_policy)
{
  ota_handle_t *p_handle = mtos_malloc(sizeof(ota_handle_t));

  CHECK_FAIL_RET_NULL((p_handle != NULL) && (p_policy != NULL));
  p_handle->p_policy = p_policy;
  memset(&(p_handle->data), 0, sizeof(ota_priv_data_t));
  memset(&(p_handle->instance), 0, sizeof(app_t));
  p_handle->instance.init = init;
  p_handle->instance.task_single_step = ota_single_step;
  p_handle->instance.get_msgq_timeout = NULL;
  p_handle->instance.p_data = (void *)p_handle;

  return &(p_handle->instance);
}
