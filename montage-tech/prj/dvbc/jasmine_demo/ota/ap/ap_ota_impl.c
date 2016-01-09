/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_devs.h"


#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "mtos_mem.h"

#include "mem_manager.h"
#include "fcrc.h"
#include "lib_util.h"

#include "drv_dev.h"
#include "charsto.h"
#include "data_manager.h"
#include "nim.h"
#include "pti.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "class_factory.h"
#include "mdl.h"
#include "dsmcc.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"

#include "ap_framework.h"
#include "ap_ota.h"

#include "uio.h"

#include "sys_status.h"

#include "sys_cfg.h"

/*!
   the buffer number of DSMCC
  */
#define DSMCC_BUF_NUM      20
#define DSMCC_OTA_TIMEOUT    3000
#define DMH_MAX_SIZE  1024

/*the struct must consistent with flash head*/
typedef  struct
{
  u8 dmh_indicator[12];// = "*^_^*DM(^o^)";
  u32 end_addr;
  u32 sdram_size;
  u32 flash_size;
  u16 block_cnt;
  u16 bisize;// = sizeof(dmh_block_info_t);
}dmh_info_t;

#define OTA_HEAD    1
#define MAIN_HEAD   2
#define BL_OFFSET 8
#define HEAD_OFFSET 0x0c
#define BLK_HEAD_OFFSET 0x1c

BOOL is_flash_error = FALSE;
BOOL is_bakup_mem_ota = TRUE;
static u8 *p_BakLoaderBuffer = NULL;
static u32 bak_loader_size = 0;
static u16 new_sw_ver = 0;

typedef void (*FUNC_NOTIFY_PROGRESS)(u32 cur_size, u32 full_size);

void ota_request_dsmcc_db_multi_section(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  //dsmcc_db_req_t *p_ddm_param = (dsmcc_db_req_t *)para1;
  filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t *)para2;
  u16 dsmcc_pid = (u16)((para1 >> 16) & 0xFFFF);
  u8 group_id = (u8)((para1 >> 8) & 0xFF);
  u8 type = (u8)(para1 & 0xFF);

  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  OS_PRINTF("\r\n[OTA] %s ", __FUNCTION__);

  p_sec->direct_data = TRUE;

  //Set PID information
  p_sec->pid = dsmcc_pid;
  p_sec->table_id = DVB_TABLE_ID_DSMCC_DDM;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_DDM;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = group_id ;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = type;
  p_sec->filter_mask[2] = 0xFF;

  p_sec->filter_code[8] = (u8)(DSMCC_MSG_DDB >> 8);
  p_sec->filter_mask[8] = 0xFF;
  p_sec->filter_code[9] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_sec->filter_mask[9] = 0xFF;

  p_sec->filter_mask_size = 8;
#else
  p_sec->filter_code[3] = (u8)group_id ;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = (u8)type;
  p_sec->filter_mask[4] = 0xFF;


  p_sec->filter_code[10] = (u8)(DSMCC_MSG_DDB >> 8);
  p_sec->filter_mask[10] = 0xFF;
  p_sec->filter_code[11] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_sec->filter_mask[11] = 0xFF;

  p_sec->filter_mask_size = 10;
#endif
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->timeout = (u32)DSMCC_OTA_TIMEOUT;

  //Attach buffer list
  p_sec->p_ext_buf_list = p_ext_buf_list;
  p_sec->crc_enable = TRUE;

  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

void ota_request_dsmcc_dsi_dii_section(dvb_section_t *p_sec,u32 para1, u32 para2)
{
  u16 dsmcc_pid = (para1 & 0xFFFF0000) >> 16;
  /* msg type could be DSI or DII */
  u16 dsmcc_msg_type = (u16)(para1 & 0xFFFF);
  u16 group_id = 0;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  OS_PRINTF("\r\n[OTA] %s ", __FUNCTION__);
  switch(dsmcc_msg_type)
  {
    case DSMCC_MSG_DII:
      group_id = (u16)(para2 & 0xFFFF);
      break;
    default:
      group_id = 0x00;
      break;

  }
  p_sec->pid = dsmcc_pid;
  p_sec->timeout = (u32)DSMCC_OTA_TIMEOUT; //DSMCC_MSG_TIMEOUT
  p_sec->table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->direct_data = FALSE;
  p_sec->crc_enable = TRUE;
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,
         sizeof(u8) * MAX_FILTER_MASK_BYTES);
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_MSG;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = (u8)(group_id >> 8);
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = (u8)(group_id & 0xFF);
  p_sec->filter_mask[2] = 0xFF;
  p_sec->filter_code[8] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[8] = 0xFF;
  p_sec->filter_code[9] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[9] = 0xFF;
  p_sec->filter_mask_size = 10;
#else
  p_sec->filter_code[3] = (u8)(group_id >> 8);
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = (u8)(group_id & 0xFF);
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_code[10] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[10] = 0xFF;
  p_sec->filter_code[11] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[11] = 0xFF;
  p_sec->filter_mask_size = 12;
#endif

  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);

}

void set_flash_error(BOOL b_error)
{
    is_flash_error = b_error;
}

static void ota_crc_init()
{
  RET_CODE ret;

   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret);
}

static u32 ota_crc32_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
}

static void ota_init_tdi(void *p_tdi)
{
  s32 ret;
  char misc_info[100];
  misc_options_t *misc;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)misc_info);
  MT_ASSERT(ret != ERR_FAILURE);
  misc = (misc_options_t *)misc_info;

  memcpy((ota_tdi_t *)p_tdi, (u8 *)&misc->ota_tdi, sizeof(ota_tdi_t));
}

void ota_read_otai(ota_info_t *p_otai)
{
  memcpy((u8 *)p_otai, (u8 *)sys_status_get_ota_info(), sizeof(ota_info_t));
}

void ota_write_otai(ota_info_t *p_otai)
{
  sys_status_set_ota_info(p_otai);
  sys_status_save();
}

/*!
  all & unzip : save buffer need only.
  mod & unzip : save buffer & burn buffer needed.
  all & zip   : save buffer ,burn buffer, fake buffetr  needed.
  mod & zip   : save buffer, unzip buffer ,burn buffer & fake bufer needed.
  */
static void ota_cfg(ota_cfg_t *p_ota_cfg, BOOL is_upgrade_all, BOOL is_zipped)
{
  filter_ext_buf_t *p_temp = NULL;

  MT_ASSERT(p_ota_cfg != NULL);

  memset((void *)p_ota_cfg, 0, sizeof(ota_cfg_t));

  //set flash size
  p_ota_cfg->flash_size = CHARSTO_SIZE;

  //dsmcc buffer init, alloc buffer ring for dvb svc works.
  p_ota_cfg->p_dsmcc_extern_buf = mtos_malloc(sizeof(filter_ext_buf_t));
  MT_ASSERT(p_ota_cfg->p_dsmcc_extern_buf != NULL);

  p_temp =  p_ota_cfg->p_dsmcc_extern_buf;

  p_temp->p_buf = mtos_malloc(80*KBYTES);//1024
  MT_ASSERT(p_temp->p_buf != NULL);

  p_temp->size = 80*KBYTES;//1024*KBYTES;

  //alloc section buffer for parse ddm.
  p_ota_cfg->p_sec_buf = (dvb_section_t *)mtos_malloc(sizeof(dvb_section_t));
  MT_ASSERT(p_ota_cfg->p_sec_buf != NULL);

  p_ota_cfg->p_sec_buf->p_buffer = (u8 *)mtos_malloc(4 * KBYTES);
  MT_ASSERT(NULL != p_ota_cfg->p_sec_buf->p_buffer);

  memset(p_ota_cfg->p_sec_buf->p_buffer, 0, 4 * KBYTES);

  //alloc buffer for download data or unzip data.
  if(is_upgrade_all)
  {
	OS_PRINTF("UPG ALL, and ");
    if(is_zipped)
    {
      OS_PRINTF("ZIPPED\n");
      p_ota_cfg->p_save_buf = mtos_malloc(3072 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);

      p_ota_cfg->save_size = 3072 * KBYTES;

      p_ota_cfg->p_burn_buf = mtos_malloc(4150 * KBYTES);

      p_ota_cfg->burn_size = 4150 * KBYTES;

      p_ota_cfg->p_unzip_buf = NULL;

      p_ota_cfg->unzip_size = 0;

      p_ota_cfg->p_fake_buf = mtos_malloc(400 * KBYTES);

      p_ota_cfg->fake_size = 400 * KBYTES;
      //MT_ASSERT(0);
    }
    else
    {
      OS_PRINTF("NOT ZIPPED\n");
      p_ota_cfg->p_save_buf = mtos_malloc(2050 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);

      p_ota_cfg->save_size = 2050 * KBYTES;

      p_ota_cfg->p_burn_buf = NULL;

      p_ota_cfg->burn_size = 0;

      p_ota_cfg->p_unzip_buf = NULL;

      p_ota_cfg->unzip_size = 0;

      p_ota_cfg->p_fake_buf = NULL;

      p_ota_cfg->fake_size = 0;
    }
  }
  else
  {
    //MT_ASSERT(0);
    OS_PRINTF("UPG MOD, and ");
    if(is_zipped)
    {
      OS_PRINTF("ZIPPED\n");
      p_ota_cfg->p_save_buf = mtos_malloc(80 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);

      p_ota_cfg->save_size = 80 * KBYTES;

      p_ota_cfg->p_burn_buf = mtos_malloc(2048 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_burn_buf != NULL);

      p_ota_cfg->burn_size = 2048 * KBYTES;

      p_ota_cfg->p_unzip_buf = mtos_malloc(128 * KBYTES);

      p_ota_cfg->unzip_size = 128 * KBYTES;

      p_ota_cfg->p_fake_buf = mtos_malloc(300 * KBYTES);

      p_ota_cfg->fake_size = 300 * KBYTES;
    }
    else
    {
      OS_PRINTF("NOT ZIPPED\n");
      p_ota_cfg->p_save_buf = mtos_malloc(128 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_save_buf != NULL);

      p_ota_cfg->save_size = 128 * KBYTES;

      p_ota_cfg->p_burn_buf = mtos_malloc(2048 * KBYTES);

      MT_ASSERT(p_ota_cfg->p_burn_buf != NULL);

      p_ota_cfg->burn_size = 2048 * KBYTES;

      p_ota_cfg->p_unzip_buf = NULL;

      p_ota_cfg->unzip_size = 0;

      p_ota_cfg->p_fake_buf = NULL;

      p_ota_cfg->fake_size = 0;
    }
  }

  return;
}


/*******************************
find head offset in memory
********************************/
static u32 find_dmh_offset_in_mem(u8 head_idx,u8 *p_mem)
{
  u32 i = 0;
  u32 dm_head_start = 0;
  u32 init_start = (u32)p_mem;
  u32 start_data1 = 0;
  u32 start_data2 = 0;
  u8 k = 0;
  if(head_idx >4)
  {
      OS_PRINTF("\r\n[OTA]error,FUNC[%s],head_idx[0x%x]#",__FUNCTION__,head_idx);
      MT_ASSERT(0);
  }
  for(i = 0; i < 32; i++)
  {
    start_data1 = *((unsigned int *)init_start);
    start_data2 = *((unsigned int *)(init_start + 4));

    if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
    {
      dm_head_start = init_start;
      k++;
      if(k == head_idx)
      {
          return (dm_head_start - (u32)p_mem);
      }
      else
      {
          init_start += 0x10000;
          continue;
      }
    }
    init_start += 0x10000;
  }

  MT_ASSERT(0);

  return 0;
}

static RET_CODE erase_backup_loader(u32 *p_offset_in_flash)
{
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                    SYS_DEV_TYPE_CHARSTO);
  u32 i = 0;
  u32 dm_head1 = 0;
  u32 dm_head2 = 0;
  u32 init_start = 0;
  u16 sec_cnt = 0;
  u32 start_data1 = 0;
  u32 start_data2 = 0;
  u32 k = 0;
  RET_CODE ret = SUCCESS;

  OS_PRINTF("\r\n[OTA] %s:start...", __FUNCTION__);

  //scan DMH
  for (i = 0; i < 32; i++)
  {
    charsto_read(p_charsto_dev, init_start, (u8 *)&start_data1, 4);
    charsto_read(p_charsto_dev, init_start + 4, (u8 *)&start_data2, 4);

    if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
    {
      //found
        k++;
        if(1 == k)
        {
          dm_head1 = init_start;
        }
        else if(2 == k)
        {
          dm_head2 = init_start;
          break;
        }
    }
    init_start += 0x10000;
  }

  if(dm_head2 <= dm_head1)
  {
      OS_PRINTF("\r\n[OTA]FUNC[%s],head1[0x%x],head2[0x%x] ", __FUNCTION__, dm_head1,dm_head2);
      MT_ASSERT(0);
  }
  if(bak_loader_size > (dm_head2 - dm_head1))
  {
      OS_PRINTF("\r\n[OTA]FUNC[%s],bak_loader_size[0x%x],head1[0x%x],head2[0x%x] ",
                          __FUNCTION__, bak_loader_size,dm_head1,dm_head2);
      MT_ASSERT(0);
  }
  *p_offset_in_flash = dm_head1;
  sec_cnt = (dm_head2 - dm_head1)/CHARSTO_SECTOR_SIZE;
  
{
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  ret |= charsto_erase(p_charsto_dev, dm_head1, sec_cnt);

  //restore
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
}

  OS_PRINTF("\r\n[OTA]FUNC[%s],sec_cnt[0x%x],head1[0x%x],head2[0x%x],ret[0x%x] ",
                      __FUNCTION__,sec_cnt,dm_head1,dm_head2,ret);
  return ret;
}
static RET_CODE backup_user_data(u8 *p_buffer, u8 block_id)
{
  dmh_block_info_t *p_mem_block_info = NULL;
  dmh_block_info_t flash_block_info = {0};
  dmh_info_t *p_dmh_info = NULL;
  u32 offset_in_mem = 0;
  u32 i;
  dm_ret_t dm_ret = DM_SUC;
  BOOL is_in_mem = FALSE;
  u8 *p_block_buffer = NULL;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                    SYS_DEV_TYPE_CHARSTO);
  if((NULL == p_buffer)||(0xFF == block_id))
  {
      return ERR_FAILURE;
  }
  /***find block head in flash***/
  dm_ret = dm_get_block_header(p_dm_handle, block_id, &flash_block_info);
  if(DM_SUC != dm_ret)
  {
      OS_PRINTF("\r\n[OTA]%s: block_id[0x%x] not in flash! ",__FUNCTION__, block_id);
      return ERR_FAILURE;
  }
  /***find block head in buffer***/
  offset_in_mem = find_dmh_offset_in_mem(MAIN_HEAD, p_buffer);
  p_dmh_info = (dmh_info_t *)(p_buffer + offset_in_mem);
  for(i = 0;  i < p_dmh_info->block_cnt; i++)
  {
    p_mem_block_info = (dmh_block_info_t *)(p_buffer + offset_in_mem
                                  + BLK_HEAD_OFFSET + i * sizeof(dmh_block_info_t));
    if(block_id == p_mem_block_info->id)
    {
      is_in_mem = TRUE;
      break;
    }
  }

  if(!is_in_mem)
  {
      OS_PRINTF("\r\n[OTA]%s: block_id[0x%x] not in memory! ",__FUNCTION__, block_id);
      return ERR_FAILURE;
  }

  if(p_mem_block_info->size != flash_block_info.size)
  {
      OS_PRINTF("\r\n[OTA] the block size if different mem[0x%x],flash[0x%x]",
                        p_mem_block_info->size, flash_block_info.size);
      return ERR_FAILURE;
  }
  p_block_buffer = mtos_malloc(flash_block_info.size);
  if(NULL == p_block_buffer)
  {
      return ERR_FAILURE;
  }
  memset(p_block_buffer, 0xFF, flash_block_info.size);

  charsto_read(p_charsto_dev,
                      flash_block_info.base_addr + DM_HDR_START_ADDR,
                      p_block_buffer,
                      flash_block_info.size);

  offset_in_mem += p_mem_block_info->base_addr;
  memcpy(p_buffer+offset_in_mem, p_block_buffer, flash_block_info.size);
  
  mtos_free(p_block_buffer);

  OS_PRINTF("\r\n[OTA]FUNC[%s]:block_id[0x%x],offset_in_mem[0x%x] size[0x%x] ",
                      __FUNCTION__, block_id, offset_in_mem, flash_block_info.size);

  /***add for test***/
  offset_in_mem = find_dmh_offset_in_mem(MAIN_HEAD, p_buffer);
  p_dmh_info = (dmh_info_t *)(p_buffer + offset_in_mem);
  for(i = 0;  i < p_dmh_info->block_cnt; i++)
  {
    p_mem_block_info = (dmh_block_info_t *)(p_buffer + offset_in_mem
                                  + BLK_HEAD_OFFSET + i * sizeof(dmh_block_info_t));
    if(SS_DATA_BLOCK_ID == p_mem_block_info->id)
    {
        memcpy(&new_sw_ver,(p_buffer + p_mem_block_info->base_addr + offset_in_mem + 20),2);
        OS_PRINTF("\r\n[OTA]###########ver[0x%x] ",new_sw_ver);
        break;
    }
  }
  /***test end***/

  return SUCCESS;
}
static RET_CODE create_new_ota_loader(u8 *p_buffer)
{
  u8 write_addr = 0;
  u8 i = 0;
  RET_CODE ret = SUCCESS;
  u32 offset_in_mem = 0;
  dmh_block_info_t *p_mem_block_info = NULL;
  dmh_block_info_t flash_block_info = {0};
  u32 ota0_sec_in_flash = 0;
  u32 ota0_sec_in_mem = 0;
  u32 read_len = 0;
  BOOL is_in_mem = FALSE;
  dm_ret_t dm_ret = DM_SUC;
  dmh_info_t *p_dmh_info = NULL;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  /***find ota0 in flash***/
  dm_ret = dm_get_block_header(p_dm_handle, OTA_BLOCK_ID, &flash_block_info);

  /***find ota0 in buffer***/
  offset_in_mem = find_dmh_offset_in_mem(MAIN_HEAD, p_buffer);
  p_dmh_info = (dmh_info_t *)(p_buffer + offset_in_mem);
  for(i = 0;  i < p_dmh_info->block_cnt; i++)
  {
    p_mem_block_info = (dmh_block_info_t *)(p_buffer + offset_in_mem
                                  + BLK_HEAD_OFFSET + i * sizeof(dmh_block_info_t));
    if(OTA_BLOCK_ID == p_mem_block_info->id)
    {
      is_in_mem = TRUE;
      break;
    }
  }

  if((is_in_mem)&&(DM_SUC == dm_ret))
  {
      ota0_sec_in_flash = flash_block_info.size/CHARSTO_SECTOR_SIZE +1;
      ota0_sec_in_mem = p_mem_block_info->size/CHARSTO_SECTOR_SIZE +1;
      if(ota0_sec_in_mem > ota0_sec_in_flash)
      {
        is_bakup_mem_ota = FALSE;
      }
      else
      {
        is_bakup_mem_ota = TRUE;;
      }
  }
  else if((DM_SUC != dm_ret)&&(!is_in_mem))
  {
      MT_ASSERT(0);
  }
  else
  {
      MT_ASSERT(0);
  }
  OS_PRINTF("\r\n[OTA]FUNC[%s]:ota0_sec count in mem[0x%x],flash[0x%x] ",
                      __FUNCTION__, ota0_sec_in_mem, ota0_sec_in_flash);

  if(is_bakup_mem_ota)
  {
      offset_in_mem = find_dmh_offset_in_mem(OTA_HEAD, p_buffer);
      p_dmh_info = (dmh_info_t *)(p_buffer + offset_in_mem);
      if(p_dmh_info->end_addr != (ota0_sec_in_mem*CHARSTO_SECTOR_SIZE))
      {
          OS_PRINTF("\r\n[OTA]error!!!!ota1 end addr[0x%x],ota0 sec[0x%x] \r\n",
                            p_dmh_info->end_addr,ota0_sec_in_mem);
          MT_ASSERT(0);
      }
      p_BakLoaderBuffer = p_buffer + offset_in_mem;
      bak_loader_size = p_dmh_info->end_addr;
  }
  else
  {
      bak_loader_size = ota0_sec_in_flash * CHARSTO_SECTOR_SIZE;
      p_BakLoaderBuffer = mtos_malloc(ota0_sec_in_flash * CHARSTO_SECTOR_SIZE);
      MT_ASSERT(p_BakLoaderBuffer != NULL);
      memset(p_BakLoaderBuffer,0xFF,ota0_sec_in_flash * CHARSTO_SECTOR_SIZE);
      offset_in_mem = find_dmh_offset_in_mem(OTA_HEAD, p_buffer);
      p_dmh_info = (dmh_info_t *)(p_buffer + offset_in_mem);
      p_dmh_info->block_cnt = 1;
      p_dmh_info->end_addr = ota0_sec_in_flash * CHARSTO_SECTOR_SIZE;
      write_addr = 0;
      memcpy(p_BakLoaderBuffer, p_dmh_info, sizeof(dmh_info_t));
      write_addr += sizeof(dmh_info_t);
      memcpy(p_BakLoaderBuffer+write_addr, &flash_block_info, sizeof(dmh_block_info_t));

      read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                                  OTA_BLOCK_ID, 0, 0,
                                  flash_block_info.size,
                                  (u8 *)(p_BakLoaderBuffer+DMH_MAX_SIZE));
  }
  return ret;
}

static RET_CODE write_new_ota_loader(u32 off_in_flash)
{
  RET_CODE ret = SUCCESS;
  u32 i = 0;
  u32 sec_cnt = 0;
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
            SYS_DEV_TYPE_CHARSTO);
  if(NULL == p_BakLoaderBuffer)
  {
      MT_ASSERT(0);
  }
  sec_cnt = bak_loader_size/CHARSTO_SECTOR_SIZE;

  OS_PRINTF("\r\n[OTA]FUNC[%s]:off_in_flash[0x%x],size[0x%x],sec_cnt[0x%x] ",
                      __FUNCTION__, off_in_flash, bak_loader_size,sec_cnt);

  {
    charsto_prot_status_t st_old = {0};
    charsto_prot_status_t st_set = {0};

    dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
    //unprotect
    st_set.prt_t = PRT_UNPROT_ALL;
    dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

    for(i = 0; i<sec_cnt; i++)
    {
      ret |= charsto_writeonly(p_charsto_dev, off_in_flash + i * CHARSTO_SECTOR_SIZE,
                    p_BakLoaderBuffer + i * CHARSTO_SECTOR_SIZE, CHARSTO_SECTOR_SIZE);
    }

    //restore
    dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
  }

  return ret;
}

/************************************
FUNCTION NAME:backup_ota_loader
INPUT PARAM:u8 *buf (the download buffer of new code)
COMMENT:备份新的OTA
              如果新的OTA占用空间比现有的OTA大
              则备份flash中的OTA0
**************************************/
static RET_CODE backup_ota_loader(u8 *buf)
{
  RET_CODE ret = SUCCESS;
  u32 off_in_flash = 0;

  ret = create_new_ota_loader(buf);
  ret |= erase_backup_loader(&off_in_flash);
  ret |= write_new_ota_loader(off_in_flash);

  if(!is_bakup_mem_ota)
  {
      OS_PRINTF("\r\n[OTA]FUNC[%s]:free ota loader buffer ", __FUNCTION__);
      mtos_free(p_BakLoaderBuffer);
  }
  OS_PRINTF("\r\n[OTA] [%s done]:ret[0x%x] ", __FUNCTION__, ret);
  return ret;
}

static BOOL ota_burn_file_group(u8 *buf, u32 start, u32 size, void *notify_progress)
{
  struct charsto_device *norf_dev;
  u32 read_addr = BL_OFFSET;
  u32 bl_size;
  u32 backup_load_size = 0;
  u32 offset_in_mem = 0;
  u32 offset_in_flash = 0;
  u32 burn_size = 0;
  RET_CODE ret = SUCCESS;
  u32 charsto_size = CHARSTO_SIZE;
  u32 i, sec_cnt;
  FUNC_NOTIFY_PROGRESS func;
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  if(!is_flash_error)/*the prev ota version is ok ?*/
  {
      ret = backup_ota_loader(buf);
        /*****back up user data*****/
      backup_user_data(buf, IW_TABLE_BLOCK_ID);
      backup_user_data(buf, IW_VIEW_BLOCK_ID);
  }
  if(SUCCESS != ret)
  {
      return FALSE;
  }

  norf_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  /***read bootload head***/
  if(SUCCESS != charsto_read(norf_dev, BL_OFFSET, (u8 *)&bl_size, 4))
  {
    OS_PRINTF("charsto read[0x%x] error\n",BL_OFFSET);
    return FALSE;
  }
  if(bl_size % CHARSTO_SECTOR_SIZE != 0)
  {
    bl_size += CHARSTO_SECTOR_SIZE - (bl_size % CHARSTO_SECTOR_SIZE);
  }

  /***read backup ota head***/
  read_addr = bl_size + HEAD_OFFSET;
  if(SUCCESS != charsto_read(norf_dev, read_addr, (u8 *)&backup_load_size, 4))
  {
    OS_PRINTF("charsto read[0x%x] error\n",read_addr);
    return FALSE;
  }
  if(backup_load_size % CHARSTO_SECTOR_SIZE != 0)
  {
    backup_load_size += CHARSTO_SECTOR_SIZE - (backup_load_size % CHARSTO_SECTOR_SIZE);
  }

  /***cal the offset in flash/memory***/
  offset_in_flash = bl_size + backup_load_size;
  offset_in_mem = find_dmh_offset_in_mem(MAIN_HEAD, buf);

  if(start < bl_size && 0x0 != start )
  {
    //bootloader shouldn't be modified.
    return FALSE;
  }

  if(start == 0xFFFFFFFF)
  {
    //ota by module
    if(size != charsto_size && size != (charsto_size - bl_size))
    {
      return FALSE;
    }

    if(size == charsto_size)
    {
      buf += offset_in_mem;//bl_size;
      burn_size = charsto_size - offset_in_mem;//bl_size;
    }
    else
    {
      burn_size = size;
    }
    sec_cnt = burn_size / CHARSTO_SECTOR_SIZE;

    OS_PRINTF("\r\n[OTA]FUNC[%s]LINE[%d],offset_in_flash[0x%x],burn_size[0x%x] ",
                        __FUNCTION__, __LINE__, offset_in_flash, burn_size);
  }
  else if(0x0== start)
  {
    //upg all
    OS_PRINTF("\r\n[OTA] abc");
    buf+=offset_in_mem;//bl_size;
    burn_size = charsto_size - offset_in_mem;//bl_size;
    sec_cnt = burn_size / CHARSTO_SECTOR_SIZE;
    OS_PRINTF("\r\n[OTA]FUNC[%s]LINE[%d],offset_in_flash[0x%x],burn_size[0x%x] ",
                        __FUNCTION__, __LINE__, offset_in_flash, burn_size);
  }
  else
  {
    if(size % CHARSTO_SECTOR_SIZE != 0)
    {

      return FALSE;
    }
    sec_cnt = size / CHARSTO_SECTOR_SIZE;
    offset_in_flash = start;
    OS_PRINTF("\r\n[OTA]FUNC[%s]LINE[%d],offset_in_flash[0x%x],sec_cnt[0x%x] ",
                        __FUNCTION__, __LINE__, offset_in_flash, sec_cnt);
  }


  dev_io_ctrl(norf_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(norf_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);
  /**new ota img size is different with the backup ota img size**/
  if(offset_in_flash < offset_in_mem)
  {
    i = (offset_in_mem - offset_in_flash)/CHARSTO_SECTOR_SIZE;
    charsto_erase(norf_dev, offset_in_flash, i);
    offset_in_flash = offset_in_mem;
  }
  else if(offset_in_flash > offset_in_mem)
  {
    burn_size = charsto_size - offset_in_flash;//bl_size;
    sec_cnt = burn_size / CHARSTO_SECTOR_SIZE;
  }
    OS_PRINTF("\r\n[OTA] burn offset_in_flash[0x%x], offset_in_mem[0x%x] ",
                  offset_in_flash, offset_in_mem);

  func = (FUNC_NOTIFY_PROGRESS)notify_progress;
  for(i = 0; i < sec_cnt; i++)
  {
    #ifndef WIN32
    charsto_erase(norf_dev, offset_in_flash + i * CHARSTO_SECTOR_SIZE, 1);
    ret = charsto_writeonly(norf_dev, offset_in_flash + i * CHARSTO_SECTOR_SIZE,
                  buf + i * CHARSTO_SECTOR_SIZE, CHARSTO_SECTOR_SIZE);
    #endif

    if(ret != SUCCESS)
    {
      //restore
      dev_io_ctrl(norf_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

      return FALSE;
    }

    func((i + 1) * 100 / sec_cnt, 100);

    #ifdef WIN32
    mtos_task_sleep(500);
    #else
    #endif
  }
  if(!is_flash_error)
  {
    ota_info_t ota_info = {0};
    ota_read_otai(&ota_info);
    OS_PRINTF("\r\n[OTA]****SW VER[0x%x]",new_sw_ver);
    sys_status_set_sw_version(new_sw_ver);
    ota_info.orig_software_version = new_sw_ver;
    ota_write_otai(&ota_info);
  }

  //restore
  dev_io_ctrl(norf_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

  return TRUE;
}

ota_policy_t *construct_ota_policy(void)
{
  ota_policy_t *p_ota_impl = mtos_malloc(sizeof(ota_policy_t));
  MT_ASSERT(p_ota_impl != NULL);

  p_ota_impl->ota_crc_init = ota_crc_init;
  p_ota_impl->ota_crc32_generate = ota_crc32_generate;
  p_ota_impl->ota_init_tdi = ota_init_tdi;
  p_ota_impl->ota_read_otai = ota_read_otai;
  p_ota_impl->ota_write_otai = ota_write_otai;
  p_ota_impl->ota_cfg = ota_cfg;
  p_ota_impl->ota_burn_file_group = ota_burn_file_group;
  p_ota_impl->p_data = NULL;

  return p_ota_impl;
}

void destruct_ota_policy(ota_policy_t *p_ota_impl)
{
  //Free private data
  mtos_free(p_ota_impl->p_data);

  //Free implement policy
  mtos_free(p_ota_impl);
}
