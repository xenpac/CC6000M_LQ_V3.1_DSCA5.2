/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "userdb/userdb_common.h"
#include "userdb/project_userdb.h"
#include "interface/userdb_native_interface.h"
#include "stdlib.h"
#include "wchar.h"

#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"
// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_event.h"
#include "mtos_printk.h"
//#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mtos_int.h"
// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"
// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal.h"
#include "ipc.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"
#include "../../../src/drv/drvbase/drv_svc.h"
#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"
#include "nim.h"
#include "dmx.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "mtos_event.h"
#include "list.h"
#include "block.h"

#include "fsioctl.h"
#include "ufs.h"

#include "vfs.h"
#include "hdmi.h"
#include "region.h"
#include "display.h"
#include "pdec.h"
// mdl
#include "mdl.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"
#include "data_base16v2.h"
#include "service.h"
#include "smc_ctrl.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"
#include "epg_data4.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"
#include "vfs.h"
#include "avctrl1.h"
#include "vbi_vsb.h"
#include "surface.h"
#include "db_dvbs.h"
#include "mem_cfg.h"
#include "pnp_service.h"
#include "media_data.h"
#include "cas_ware.h"
#include "dsmcc.h"
//eva
#include "interface.h"
#include "eva.h"
#include "media_format_define.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"
#include "eva_filter_factory.h"
#include "common_filter.h"
#include "file_source_filter.h"
#include "ts_player_filter.h"
#include "av_render_filter.h"
#include "demux_filter.h"
#include "record_filter.h"
#include "file_sink_filter.h"
#include "pic_filter.h"
#include "pic_render_filter.h"
#include "mp3_player_filter.h"
#include "lrc_filter.h"
#include "char_filter.h"
#include "str_filter.h"
#include "file_list.h"
#include "dmx_manager.h"
// ap
#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_signal_monitor.h"
#include "ap_satcodx.h"
#include "ap_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
#include "ap_cas.h"
#include "ap_epg4.h"
#include "media_data.h"
#include "ap_usb_upgrade.h"
#include "pvr_api.h"
//customer define
#include "customer_config.h"
#include "lpower.h"
#include "ui_util_api.h"
#include "ui_usb_api.h"
#include "sys_status.h"
#include "ethernet.h"
#include "subt_station_filter.h"

static u8 g_sys_partition[10 * 1024 * 1024] = {0};
static struct NativePropertyTable *p_SATtable = NULL;
static struct NativePropertyTable *p_TPtable = NULL;
static struct NativePropertyTable *p_PGtable = NULL;
static char parameter_open[300] = {0};

static u8 flash_for_userdb[300] = "D://flash.bin";
static usb_upg_start_params_t usb_param = {0};
u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE] = "*^_^*DM(^o^)";

static void *mt_mem_malloc(u32 size)
{
  void *p_addr = NULL;
  mem_mgr_alloc_param_t param = {0};

  param.id = MEM_SYS_PARTITION;
  param.size = size;
  param.user_id = SYS_MODULE_SYSTEM;
  p_addr = mem_mgr_alloc(&param);
  //MT_ASSERT(p_addr != NULL);
  memset(p_addr, 0, size);

  return p_addr;
}


static void mt_mem_free(void *p_addr)
{
  mem_mgr_free_param_t param = {0};

  param.id = MEM_SYS_PARTITION;
  param.p_addr = p_addr;
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}


extern void no_os_init(mtos_cfg_t *p_cfg);

BOOL usrdb_init(char *parameter)
{
  void *p_dev = NULL;
  dm_v2_init_para_t dm_para = {0};
  mem_mgr_partition_param_t partition_param = {0};
  RET_CODE ret = SUCCESS;
  charsto_cfg_t charsto_cfg = {0};

  memset(g_sys_partition, 0x0, 10 * 1024 * 1024);
  mem_mgr_init((u8 *)g_sys_partition, 10 * 1024 * 1024);

  //create system partition
  partition_param.id = MEM_SYS_PARTITION;
  partition_param.size = 9 * 1024 * 1024;
  partition_param.p_addr = (u8 *)g_sys_partition;
  partition_param.atom_size = 64;
  partition_param.user_id = SYS_MODULE_SYSTEM;
  partition_param.method_id = MEM_METHOD_NORMAL;
  ret = MEM_CREATE_PARTITION(&partition_param);
  if(FALSE == ret)
  {
    return FALSE;
  }
  OS_PRINTF("create SYS_PARTITION1 ok!\n");
  mtos_mem_init(mt_mem_malloc, mt_mem_free);
  OS_PRINTF("init mem11 ok!\n");

  mtos_ticks_init(SYS_CPU_CLOCK);

  //init message queue
  ret = mtos_message_init();
  if(FALSE == ret)
  {
   return FALSE;
  }

  mtos_task_sleep(250);

  //init char storage....
  ret = ATTACH_DRIVER(CHARSTO, warriors, default, default);
  if(ret != SUCCESS)
  {
      return FALSE;
  }

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  if(NULL == p_dev)
  {
   return FALSE;
  }
  charsto_cfg.size = 8 * MBYTES;
  charsto_cfg.spi_clk = FLASH_C_CLK_P6;
  charsto_cfg.rd_mode = SPI_FR_MODE;
  charsto_cfg.lock_mode = OS_MUTEX_LOCK;
  charsto_cfg.path = parameter;
  ret = dev_open(p_dev, &charsto_cfg);
  if(ret != SUCCESS)
  {
   return FALSE;
  }
  //init data manager....
  dm_para.flash_base_addr = (u32)NORF_BASE_ADDR;
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
  dm_para.open_monitor = TRUE;
  dm_para.para_size = sizeof(dm_v2_init_para_t);
  dm_para.use_mutex = TRUE;
  dm_para.mutex_prio = 1;
  dm_para.test_mode = FALSE;
  dm_init_v2(&dm_para);
  OS_PRINTF("set header [0xC]\n");
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), 0xC);
  OS_PRINTF("set header [0x%x]\n", DM_HDR_START_ADDR_BAK);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_HDR_START_ADDR_BAK);
  OS_PRINTF("set header [0x%x]\n", DM_HDR_START_ADDR);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_HDR_START_ADDR);

  db_dvbs_init();
  return  TRUE;
}


BOOL Creatfile(char *parameter)
{
  int c;
  FILE *p_file_open = NULL;
  FILE *p_file_save = NULL;

  if(strcmp(parameter_open, parameter) == 0)
  {
    return FALSE;
  }

  p_file_open = fopen(parameter_open, "rb");
  p_file_save = fopen(parameter, "wb");

  if(p_file_open == NULL
    || p_file_save == NULL)
  {
    return FALSE;
  }
  while((c = fgetc(p_file_open)) != EOF) //从源文件中读取数据知道结尾
  {
    fputc(c, p_file_save);
  }

  fclose(p_file_open);    //关闭文件指针，释放内存
  fclose(p_file_save);
  return TRUE;
}

BOOL is_userdb_data_valid(hfile_t file)
{
  u32 read_size = 0;
  u8 head_data[USB_UPG_MAX_HEAD_SIZE] = {0};
  vfs_file_info_t file_info = {0};

  vfs_get_file_info(file, &file_info);

  if(file_info.file_size != (dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), IW_TABLE_BLOCK_ID) + \
                                       dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), IW_VIEW_BLOCK_ID) + \
                                       USB_UPG_MAX_HEAD_SIZE))
  {
    return FALSE;
  }
  vfs_seek(file, 0, VFS_SEEK_SET);
  read_size = vfs_read(head_data, USB_UPG_MAX_HEAD_SIZE, 1, file);
  if(USB_UPG_MAX_HEAD_SIZE != read_size)
  { 
    return FALSE;
  }
  else
  {
    if(memcmp(head_data, g_userdb_head_data, USB_UPG_MAX_HEAD_SIZE) != 0)
    {
        return FALSE;
    } 
  }
  return TRUE;
}

BOOL userdb_to_flash(char *parameter, char *flash_file_parameter)
{
  hfile_t file = NULL;
  FILE* flash_file = NULL;
  u16 dump_file_name[100] = {0};
  handle_t dm_handle = NULL;
  u32 total_size = 0;
  u32 size = 0;
  s64 off_set = 0;
  u32 downloaded_size = 0;
  u8 *p_buf = NULL;
  BOOL is_valid = FALSE;
  struct stat fileData;

 if( !((0 == stat(flash_file_parameter, &fileData))
    &&(fileData.st_size == FLASH_SIZE)))
    {
      return FALSE;
    }
 
  memset(dump_file_name, 0, 100 * sizeof(u16));
  str_asc2uni(parameter, dump_file_name);

  nos_mem_init(malloc,free);
  vfs_win32_init();
  if(NULL == parameter || NULL == flash_file_parameter)
  {
    return FALSE;
  }

  usrdb_init(flash_file_parameter);
  dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  memset(&usb_param, 0x0, sizeof(usb_upg_start_params_t));

  usb_param.ext_buf_addr = malloc(1024 * 1024);
  usb_param.head_size = USB_UPG_MAX_HEAD_SIZE;
  usb_param.ext_buf_size = dm_get_block_size(dm_handle, IW_TABLE_BLOCK_ID)+\
  dm_get_block_size(dm_handle, IW_VIEW_BLOCK_ID) + usb_param.head_size;
  usb_param.flash_offset_addr = dm_get_block_addr(dm_handle, IW_TABLE_BLOCK_ID) \
  - NORF_BASE_ADDR;
  memcpy(usb_param.file, dump_file_name, MAX_FILE_PATH * sizeof(u16));

  file = vfs_open(usb_param.file, VFS_READ);
  if(file == NULL)
  {
    return FALSE;
  }
  is_valid = is_userdb_data_valid(file);
  if(!is_valid)
  {
     return FALSE;
  }
  
  vfs_seek(file, 0, VFS_SEEK_SET);
  total_size = usb_param.ext_buf_size;
  while(downloaded_size < total_size)
  {

    p_buf = (u8 *)(usb_param.ext_buf_addr + downloaded_size);
    size = ((downloaded_size + USB_UPG_TRANS_PACKET_SIZE) <= total_size)? USB_UPG_TRANS_PACKET_SIZE\
          : (total_size - downloaded_size);
    if(size != vfs_read(p_buf, size, 1, file))
    {
       return FALSE;
    }
    downloaded_size += size;
  }
  vfs_close(file);
  flash_file = fopen(flash_file_parameter, "rb+");

  fseek(flash_file, usb_param.flash_offset_addr, SEEK_SET);

  fwrite((void *)(usb_param.ext_buf_addr + USB_UPG_MAX_HEAD_SIZE),total_size - 12,1,flash_file);
  fclose(flash_file);
  return TRUE;

}

BOOL flash_to_userdb(char *buff)
{
  FILE* file = NULL;
  FILE* flash_file = NULL;
  handle_t dm_handle = NULL;
  u32 total_size = 0, read_size = 0;
  u32 size = 0;
  s64 off_set = 0;
  u32 downloaded_size = 0;
  u8 *p_buf = NULL;
  u16 flash_file_name[100] = {0};

  str_asc2uni(flash_for_userdb, flash_file_name);
  
  nos_mem_init(malloc,free);
  vfs_win32_init();
  if(NULL == flash_for_userdb || NULL == buff)
  {
    return FALSE;
  }

  usrdb_init(flash_for_userdb);
  dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  memset(&usb_param, 0x0, sizeof(usb_upg_start_params_t));

  usb_param.ext_buf_addr = malloc(1024 * 1024);
  usb_param.head_size = USB_UPG_MAX_HEAD_SIZE;
  memcpy(usb_param.head_data, g_userdb_head_data, usb_param.head_size);
  usb_param.ext_buf_size = dm_get_block_size(dm_handle, IW_TABLE_BLOCK_ID)+\
  dm_get_block_size(dm_handle, IW_VIEW_BLOCK_ID) + usb_param.head_size;
  usb_param.flash_offset_addr = dm_get_block_addr(dm_handle, IW_TABLE_BLOCK_ID) \
  - NORF_BASE_ADDR;

  file = vfs_open(flash_file_name, VFS_READ);
  vfs_seek(file, usb_param.flash_offset_addr, VFS_SEEK_SET);
  total_size = usb_param.ext_buf_size -  USB_UPG_MAX_HEAD_SIZE;
  
  while(downloaded_size < total_size )
  {

    p_buf = (u8 *)(usb_param.ext_buf_addr + downloaded_size);
    size = ((downloaded_size + USB_UPG_TRANS_PACKET_SIZE) <= total_size)? USB_UPG_TRANS_PACKET_SIZE\
          : (total_size - downloaded_size);
    if(size != vfs_read(p_buf, size, 1, file))
    {
       return FALSE;
    }
    downloaded_size += size;
  }
  vfs_close(file);
  
  flash_file = fopen(buff, "wb+");
  
  fwrite(g_userdb_head_data,USB_UPG_MAX_HEAD_SIZE,1,flash_file);
  fseek(flash_file, USB_UPG_MAX_HEAD_SIZE, SEEK_SET);

  fwrite((void *)(usb_param.ext_buf_addr ),total_size,1,flash_file);
  fclose(flash_file);

  //free dm db
  dm_destory_v2();
  db_destory_database_16v2();
  db_dvbs_release();
  return TRUE;

}

int ProjectInitial(char *parameter, Proinit_Type type)
{
  mtos_cfg_t os_cfg = {0};
 
  int file_type = 0;
  char file_path[300] = {0};
  struct stat fileData;
  BOOL ret = FALSE;

  file_type = find_file_type(file_path,parameter);
  if(!file_type)
  {
   return 0; 
  }
   
  if(file_type == FLASH && type == SAVE)
  {
    if( !((0 == stat(file_path, &fileData))
    &&(fileData.st_size == FLASH_SIZE)))
    {
       Creatfile(file_path);
    } 
  }
  else if(file_type == FLASH && type == OPEN)
  {
     if( !((0 == stat(file_path, &fileData))
    &&(fileData.st_size == FLASH_SIZE)))
    {
      return 2;
    }
    memset(parameter_open, 0, sizeof(parameter_open));
    memcpy(parameter_open, file_path, strlen(file_path));
  }

  os_cfg.enable_bhr = FALSE;
  no_os_init(&os_cfg);

  if(file_type == FLASH)
  {
    //free dm db
    usrdb_init(file_path);
  }
  else  if(type == OPEN && file_type == USER_DB)
  {  
    ret =  userdb_to_flash(file_path,flash_for_userdb);
    if(!ret)
    {
      return 2;
    }
	//stop flash and start flash
    dm_destory_v2();
    db_destory_database_16v2();
    db_dvbs_release();
    usrdb_init(flash_for_userdb);
  }
  else if(type == SAVE  && file_type == USER_DB)
  {
	usrdb_init(flash_for_userdb);
  }
  return 1;
}


void ui_dbase_pos2freq(u16 pos, u32 *p_lnbtype, u32 *p_freq1, u32 *p_freq2,
                       u8 *p_is_unicable, u8 *p_unicable_type)
{
  {
    *p_is_unicable = 0;
    *p_unicable_type = 0;
    *p_freq1 = 0;
    *p_freq2 = 0;
    *p_lnbtype = 0;

    if(pos < LNB_FREQ_CNT)
    {
      if(pos < FREQ_VAL_CNT - 1)
      {
        *p_lnbtype = 0; // standard
        *p_freq1 = *p_freq2 = wcharToint(Sat_LNB_freq[pos]);
      }
      else if(pos < FREQ_VAL_CNT + 1)
      {
        *p_lnbtype = 1; // user
        if(pos == FREQ_VAL_CNT - 1)
        {
          *p_freq1 = 5150;
          *p_freq2 = 5750;
        }
        else
        {
          *p_freq1 = 5750;
          *p_freq2 = 5150;
        }
      }
      else if(pos < (FREQ_VAL_CNT + 2))
      {
        *p_lnbtype = 2; // universal
        *p_freq1 = 9750;
        *p_freq2 = 10600;
      }
      else
      {
        *p_lnbtype = 2; //unicable_universal
        *p_is_unicable = TRUE;
        *p_unicable_type = (pos - FREQ_VAL_CNT - 2);
        *p_freq1 = 9750;
        *p_freq2 = 10600;
      }
    }
    else
    {
      MT_ASSERT(0);
      *p_lnbtype = *p_freq1 = *p_freq2 = 0;
    }
  }
}


static u16 get_freq_index(u32 freq)
{
  u16 i;

  for(i = 0; i < LNB_FREQ_CNT; i++)
  {
    if((u32)wcharToint(Sat_LNB_freq[i]) == freq)
    {
      return i;
    }
  }

  return 0;
}


u16 ui_dbase_freq2pos(u32 lnbtype, u32 freq1, u32 freq2, u8 p_is_unicable, u8 p_unicable_type)
{
  u16 pos;

  {
    switch(lnbtype)
    {
      case 0: // standard
        pos = get_freq_index(freq1);
        break;
      case 1: // user
        pos = freq1 < freq2 ? FREQ_VAL_CNT - 1 : FREQ_VAL_CNT;
        break;
      case 2: // universal
        if(p_is_unicable == 0)
        {
          pos = FREQ_VAL_CNT + 1;
        }
        else
        {
          pos = FREQ_VAL_CNT + 2 + p_unicable_type;
        }
        break;
      default:
        // MT_ASSERT(0);
        pos = 0;
    }
  }
  return pos;
}


int Sat_NativeProperty_validation(struct NativeProperty *NativeProperty,
                                  struct NativePropertyArray *NativePropertyArray)
{
  int i;
  int ret = 1;
  int namelen;
  int value;
  wchar_t L_value[256] = {'\0'};

  for(i = 0; i < SAT_PROPERTY_NUM; i++)
  {
    if(wcscmp(NativeProperty->key, Sat_Property_key[i]) == 0)
    {
      switch(i)
      {
        case SAT_NAME:
          namelen = wcslen(NativeProperty->value);
          if(namelen > SAT_NAME_MAX)
          {
            ret = 0;
          }
          else
          {
            ret = 1;
          }
          break;
        case sat_select:
          break;
        case longitude:
          checklongitude(L_value, NativeProperty->value);
          value = wcharToint(L_value);
          if(value < SAT_LONGITUDE_MIN
            || value > SAT_LONGITUDE_MAX)
          {
            ret = 0;
          }
          else
          {
            ret = 1;
          }
          break;
        case longitude_type:
          break;
        case LNB_freq:
          break;
        case user_band:
          break;
        case band_freq:
          value = wcharToint(NativeProperty->value);
          if(value < SAT_BAND_FREQ_MIN
            || value > SAT_BAND_FREQ_MAX)
          {
            ret = 0;
          }
          else
          {
            ret = 1;
          }
          break;
        case diseqc_port:
          break;
        case diseqc11_port:
          break;
        case K22:
          break;
        case Polarity:
          break;
        case position:
          break;
        case motor_type:
          break;

        default:
          ret = 0;
          break;
      }
      break;
    }
  }
  return ret;
};


int TP_NativeProperty_validation(struct NativeProperty *NativeProperty,
                                 struct NativePropertyArray *NativePropertyArray)
{
  int i;
  int ret = 1;
  int value;

  for(i = 0; i < TP_PROPERTY_NUM; i++)
  {
    if(wcscmp(NativeProperty->key, Tp_Property_key[freq + i]) == 0)
    {
      switch(i)
      {
        case freq:

          break;
        case sym:
          value = wcharToint(NativeProperty->value);
          if(value < TP_SYM_MIN
            || value > TP_SYM_MAX)
          {
            ret = 0;
          }
          else
          {
            ret = 1;
          }
          break;
        case polarity:
          break;
        case tp_select:
          break;
        case nit_pid:
          break;
        case pat_version:
          break;
        case tp_is_scramble:
          break;
        case is_on22k:
          break;
        case nim_type:
          break;
        case emm_num:
          break;
        default:
          break;
      }
      break;
    }
  }
  return 1;
};


int PG_NativeProperty_validation(struct NativeProperty *NativeProperty,
                                 struct NativePropertyArray *NativePropertyArray)
{
  int i;

  for(i = 0; i < PG_PROPERTY_NUM; i++)
  {
    if(wcscmp(NativeProperty->key, Pg_Property_key[i]) == 0)
    {
      switch(i)
      {
        case pg_name:
          break;
        case fav_grp_flag:
          break;
        case tv_flag:
          break;
        case lck_flag:
          break;
        case skp_flag:
          break;
        case volume:
          break;
        case audio_channel:
          break;
        case pg_is_scrambled:
          break;
        case audio_ch_num:
          break;
        case video_pid:
          break;
        case pcr_pid:
          break;
        case audio_track:
          break;
        case service_type:
          break;
        case video_type:
          break;
        case s_id:
          break;
        case orig_net_id:
          break;
        case ts_id:
          break;
        case default_order:
          break;
        case ca_system_id:
          break;
        case pmt_pid:
          break;
        case ecm_num:
          break;
        case cas_ecm:
          break;
        case p_id:
          break;
        case type:
          break;
        case language_index:
          break;
        default:
          break;
      }
      break;
    }
  }
  return 1;
};


int Str_NativeProperty_compare(struct NativeProperty *NativeProperty1,
                               struct NativeProperty *NativeProperty2)
{
  int ret = -2;

  if(wcscmp(NativeProperty1->key, NativeProperty2->key) == 0)
  {
    ret = wcscmp(NativeProperty1->value, NativeProperty2->value);
    if(ret < 0)
    {
      ret = -1;
    }
    else if(ret > 0)
    {
      ret = 1;
    }
    else
    {
      ret = 0;
    }
  }
  return ret;
}


int Int_NativeProperty_compare(struct NativeProperty *NativeProperty1,
                               struct NativeProperty *NativeProperty2)
{
  int ret = -2;
  int value1, value2;

  if(wcscmp(NativeProperty1->key, NativeProperty2->key) == 0)
  {
    value1 = wcharToint(NativeProperty1->value);
    value2 = wcharToint(NativeProperty2->value);
    if(value1 == value2)
    {
      ret = 0;
    }
    else if(value1 > value2)
    {
      ret = 1;
    }
    else
    {
      ret = -1;
    }
  }
  return ret;
};


NativePropertyArray *SatTemplate()
{
  struct  NativePropertyArray *PropertyArray;
  int i, key_len, i_compare;

  PropertyArray = (struct NativePropertyArray *)malloc(sizeof(struct NativePropertyArray));
  memset(PropertyArray, 0, sizeof(struct NativePropertyArray));
  PropertyArray->first = (struct NativeProperty *)malloc(
    sizeof(struct NativeProperty) * SAT_PROPERTY_NUM);
  memset(PropertyArray->first, 0, sizeof(struct NativeProperty) * SAT_PROPERTY_NUM);

  //Propert num
  PropertyArray->size = SAT_PROPERTY_NUM;

  for(i = 0; i < SAT_PROPERTY_NUM; i++)
  {
    //key
    key_len = wcslen(Sat_Property_key[i]);
    memcpy(PropertyArray->first[i].key, Sat_Property_key[i], key_len * 2);
    //value
    key_len = wcslen(Sat_Property_value[i]);
    memcpy(PropertyArray->first[i].value, Sat_Property_value[i], key_len * 2);
    //group
    key_len = wcslen(Sat_Property_group[i]);
    memcpy(PropertyArray->first[i].group, Sat_Property_group[i], key_len * 2);
    //editable
    PropertyArray->first[i].editable = Sat_Property_editable[i];
    //options
    key_len = wcslen(Sat_Property_options[i]);
    memcpy(PropertyArray->first[i].options, Sat_Property_options[i], key_len * 2);
    //m_validation
    PropertyArray->first[i].m_validation = &Sat_NativeProperty_validation;
    //m_compare
    if(i < SAT_COMPARE_NUM)
    {
      for(i_compare = 0; i_compare < STR_SAT_COMPARE_NUM; i_compare++)
      {
        if(wcscmp(PropertyArray->first[i].key, Str_Sat_compare[i_compare]) == 0)
        {
          PropertyArray->first[i].m_compare = &Str_NativeProperty_compare;
          break;
        }
      }
      if(i_compare == STR_SAT_COMPARE_NUM)
      {
        PropertyArray->first[i].m_compare = &Int_NativeProperty_compare;
      }
    }
    else
    {
      PropertyArray->first[i].m_compare = NULL;
    }
  }

  return PropertyArray;
}


NativePropertyArray *TPTemplate()
{
  struct  NativePropertyArray *PropertyArray;
  int i, key_len, i_compare;

  PropertyArray = (struct NativePropertyArray *)malloc(sizeof(struct NativePropertyArray));
  memset(PropertyArray, 0, sizeof(struct NativePropertyArray));

  PropertyArray->first = (struct NativeProperty *)malloc(
    sizeof(struct NativeProperty) * TP_PROPERTY_NUM);
  memset(PropertyArray->first, 0, sizeof(struct NativeProperty) * TP_PROPERTY_NUM);

  //Propert num
  PropertyArray->size = TP_PROPERTY_NUM;

  for(i = 0; i < TP_PROPERTY_NUM; i++)
  {
    //key
    key_len = wcslen(Tp_Property_key[i]);
    memcpy(PropertyArray->first[i].key, Tp_Property_key[i], key_len * 2);
    //value
    key_len = wcslen(Tp_Property_value[i]);
    memcpy(PropertyArray->first[i].value, Tp_Property_value[i], key_len * 2);
    //group
    key_len = wcslen(Tp_Property_group[i]);
    memcpy(PropertyArray->first[i].group, Tp_Property_group[i], key_len * 2);
    //editable
    PropertyArray->first[i].editable = Tp_Property_editable[i];
    //options
    key_len = wcslen(Tp_Property_options[i]);
    memcpy(PropertyArray->first[i].options, Tp_Property_options[i], key_len * 2);

    //m_validation
    PropertyArray->first[i].m_validation = &TP_NativeProperty_validation;
    //m_compare
    if(i < TP_COMPARE_NUM)
    {
      for(i_compare = 0; i_compare < STR_TP_COMPARE_NUM; i_compare++)
      {
        if(wcscmp(PropertyArray->first[i].key, Str_TP_compare[i_compare]) == 0)
        {
          PropertyArray->first[i].m_compare = &Str_NativeProperty_compare;
          break;
        }
      }
      if(i_compare == STR_TP_COMPARE_NUM)
      {
        PropertyArray->first[i].m_compare = &Int_NativeProperty_compare;
      }
    }
    else
    {
      PropertyArray->first[i].m_compare = NULL;
    }
  }

  return PropertyArray;
}


NativePropertyArray *PGTemplate()
{
  struct  NativePropertyArray *PropertyArray;
  int i, key_len, i_compare;

  PropertyArray = (struct NativePropertyArray *)malloc(sizeof(struct NativePropertyArray));
  memset(PropertyArray, 0, sizeof(struct NativePropertyArray));
  PropertyArray->first = (struct NativeProperty *)malloc(
    sizeof(struct NativeProperty) * PG_PROPERTY_NUM);

  memset(PropertyArray->first, 0, sizeof(struct NativeProperty) * PG_PROPERTY_NUM);

  //Propert num
  PropertyArray->size = PG_PROPERTY_NUM;

  for(i = 0; i < PG_PROPERTY_NUM; i++)
  {
    //key
    key_len = wcslen(Pg_Property_key[i]);
    memcpy(PropertyArray->first[i].key, Pg_Property_key[i], key_len * 2);
    //value
    key_len = wcslen(Pg_Property_value[i]);
    memcpy(PropertyArray->first[i].value, Pg_Property_value[i], key_len * 2);
    //group
    key_len = wcslen(Pg_Property_group[i]);
    memcpy(PropertyArray->first[i].group, Pg_Property_group[i], key_len * 2);
    //editable
    PropertyArray->first[i].editable = Pg_Property_editable[i];
    //options
    key_len = wcslen(Pg_Property_options[i]);
    memcpy(PropertyArray->first[i].options, Pg_Property_options[i], key_len * 2);
    //m_validation
    PropertyArray->first[i].m_validation = &PG_NativeProperty_validation;
    //m_compare
    if(i < PG_COMPARE_NUM)
    {
      for(i_compare = 0; i_compare < STR_PG_COMPARE_NUM; i_compare++)
      {
        if(wcscmp(PropertyArray->first[i].key, Str_PG_compare[i_compare]) == 0)
        {
          PropertyArray->first[i].m_compare = &Str_NativeProperty_compare;
          break;
        }
      }
      if(i_compare == STR_PG_COMPARE_NUM)
      {
        PropertyArray->first[i].m_compare = &Int_NativeProperty_compare;
      }
    }
    else
    {
      PropertyArray->first[i].m_compare = NULL;
    }
  }
  return PropertyArray;
}


NativePropertyArray *Parse_sat(BOOL is_mark,sat_node_t sat)
{
  struct NativePropertyArray *PropertyArray;
  int value_len;
  u16 pos;
  u16 longitude;
  int l_type;
  int i;
  wchar_t ar_value[256] = {'\0'}, Wlong[256] = {'\0'};
  int opt = 0;

  //creat sat Template
  PropertyArray = SatTemplate();
  //value set 0
  for(i = 0; i < SAT_PROPERTY_NUM; i++)
  {
    memset(PropertyArray->first[i].value, 0, sizeof(PropertyArray->first[i].value));
  }

  PropertyArray->id = sat.id;

  //fill sat value
  value_len = wcslen(sat.name);
  memcpy(PropertyArray->first[0].value, sat.name, value_len * 2);

  value_len = intTowchar(ar_value, is_mark);

  memcpy(PropertyArray->first[1].value, ar_value, value_len * 2);
  
  longitude = sat.longitude;
  LONGITUDE_NORMALIZE(longitude);

  intTowchar(Wlong, longitude);
  value_len = curelongitude(ar_value, Wlong);
  memcpy(PropertyArray->first[2].value, ar_value, value_len * 2);

  l_type = IS_W_LONGITUDE(sat.longitude) ? 0 : 1;
  value_len = wcslen(Sat_longitude_type[l_type]);
  memcpy(PropertyArray->first[3].value, Sat_longitude_type[l_type], value_len * 2);

  pos = ui_dbase_freq2pos(sat.lnb_type,
                          sat.lnb_low,
                          sat.lnb_high,
                          (u8)sat.p_is_unicable,
                          (u8)sat.unicable_type
                          );
  value_len = wcslen(Sat_LNB_freq[pos]);
  memcpy(PropertyArray->first[4].value, Sat_LNB_freq[pos], value_len * 2);


  value_len = intTowchar(ar_value, sat.user_band);

  memcpy(PropertyArray->first[5].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, sat.band_freq);
  memcpy(PropertyArray->first[6].value, ar_value, value_len * 2);

  if(sat.diseqc_type)
  {
    opt = 1;
  }
  else
  {
    opt = 0;
  }

  value_len = wcslen(Sat_diseqc_port[sat.diseqc_port + opt]);
  memcpy(PropertyArray->first[7].value, Sat_diseqc_port[sat.diseqc_port + opt], value_len * 2);

  if(sat.diseqc11_type)
  {
    opt = 1;
  }
  else
  {
    opt = 0;
  }

  value_len = wcslen(Sat_diseqc11_port[sat.diseqc11_port + opt]);
  memcpy(PropertyArray->first[8].value, Sat_diseqc11_port[sat.diseqc11_port + opt], value_len * 2);

  value_len = wcslen(Sat_K22[sat.k22]);
  memcpy(PropertyArray->first[9].value, Sat_K22[sat.k22], value_len * 2);

  value_len = wcslen(Sat_Polarity[sat.lnb_power]);
  memcpy(PropertyArray->first[10].value, Sat_Polarity[sat.lnb_power], value_len * 2);

  value_len = intTowchar(ar_value, sat.position);
  memcpy(PropertyArray->first[11].value, ar_value, value_len * 2);

  value_len = wcslen(Sat_motor_type[sat.motor_type]);
  memcpy(PropertyArray->first[12].value, Sat_motor_type[sat.motor_type], value_len * 2);

  return PropertyArray;
}


NativePropertyArray *Parse_tp(BOOL is_mark, dvbs_tp_node_t tp)
{
  struct NativePropertyArray *PropertyArray;
  int value_len;
  wchar_t ar_value[256] = {'\0'};
  int i;

  //creat tp Template
  PropertyArray = TPTemplate();

  //value set 0
  for(i = 0; i < TP_PROPERTY_NUM; i++)
  {
    memset(PropertyArray->first[i].value, 0, sizeof(PropertyArray->first[i].value));
  }

  //fill sat value
  PropertyArray->id = tp.id;
  PropertyArray->parent_id = tp.sat_id;

  value_len = intTowchar(ar_value, tp.freq);
  memcpy(PropertyArray->first[0].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, tp.sym);
  memcpy(PropertyArray->first[1].value, ar_value, value_len * 2);

  value_len = wcslen(TP_polarity[tp.polarity]);
  memcpy(PropertyArray->first[2].value, TP_polarity[tp.polarity], value_len * 2);

  value_len = intTowchar(ar_value, is_mark);
  memcpy(PropertyArray->first[3].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, tp.nit_pid);
  memcpy(PropertyArray->first[4].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, tp.pat_version);
  memcpy(PropertyArray->first[5].value, ar_value, value_len * 2);

  value_len = wcslen(TP_is_scramble[tp.is_scramble]);
  memcpy(PropertyArray->first[6].value, TP_is_scramble[tp.is_scramble], value_len * 2);

  value_len = wcslen(TP_is_on22k[tp.is_on22k]);
  memcpy(PropertyArray->first[7].value, TP_is_on22k[tp.is_on22k], value_len * 2);

  value_len = intTowchar(ar_value, tp.nim_type);
  memcpy(PropertyArray->first[8].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, tp.emm_num);
  memcpy(PropertyArray->first[9].value, ar_value, value_len * 2);

  return PropertyArray;
}


int ui_dbase_pg_fav(wchar_t *dest, u32 fav_grp_flag)
{
  u8 dig;
  u8 mask_shift = 0;  // Parameter for view operation
  BOOL ret = FALSE;

  memset(dest, '\0', 256);
  if(!fav_grp_flag)
  {
    wcscat(dest, L"0;");
  }
  for(fav_grp_flag, dig = 0; dig < MAX_FAV_GRP; dig++)
  {
    mask_shift = dig & 0x7FFF;
    ret = (fav_grp_flag & (1 << mask_shift)) ? TRUE : FALSE;
    if(ret)
    {
      wcscat(dest, PG_Fav_grp_flag[dig]);
      wcscat(dest, L";");
    }
  }
  memset(dest + wcslen(dest) - 1, '\0', 1);
  return wcslen(dest);
}


NativePropertyArray *Parse_pg(dvbs_prog_node_t pg)
{
  struct NativePropertyArray *PropertyArray;
  int value_len;
  wchar_t ar_value[256] = {'\0'};
  int i;


  PropertyArray = PGTemplate();

  //value set 0
  for(i = 0; i < PG_PROPERTY_NUM; i++)
  {
    memset(PropertyArray->first[i].value, 0, sizeof(PropertyArray->first[i].value));
  }
  //fill sat value
  PropertyArray->id = pg.id;
  PropertyArray->parent_id = pg.tp_id;

  value_len = wcslen(pg.name);
  memcpy(PropertyArray->first[0].value, pg.name, value_len * 2);

  value_len = ui_dbase_pg_fav(ar_value, pg.fav_grp_flag);

  memcpy(PropertyArray->first[1].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.tv_flag);
  memcpy(PropertyArray->first[2].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.lck_flag);
  memcpy(PropertyArray->first[3].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.skp_flag);
  memcpy(PropertyArray->first[4].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.volume);
  memcpy(PropertyArray->first[5].value, ar_value, value_len * 2);

   if(pg.audio_channel > 2)
  {
    pg.audio_channel = 2;
  }
  value_len = wcslen(PG_audio_channel[pg.audio_channel]);
  memcpy(PropertyArray->first[6].value, PG_audio_channel[pg.audio_channel], value_len * 2);

  value_len = wcslen(PG_is_scrambled[pg.is_scrambled]);
  memcpy(PropertyArray->first[7].value, PG_is_scrambled[pg.is_scrambled], value_len * 2);

  value_len = intTowchar(ar_value, pg.audio_ch_num);
  memcpy(PropertyArray->first[8].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.video_pid);
  memcpy(PropertyArray->first[9].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.pcr_pid);
  memcpy(PropertyArray->first[10].value, ar_value, value_len * 2);

  value_len = wcslen(PG_audio_track[pg.audio_track]);
  memcpy(PropertyArray->first[11].value, PG_audio_track[pg.audio_track], value_len * 2);

  value_len = intTowchar(ar_value, pg.service_type);
  memcpy(PropertyArray->first[12].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.video_type);
  memcpy(PropertyArray->first[13].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.s_id);
  memcpy(PropertyArray->first[14].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.orig_net_id);
  memcpy(PropertyArray->first[15].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.ts_id);
  memcpy(PropertyArray->first[16].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.default_order);
  memcpy(PropertyArray->first[17].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.ca_system_id);
  memcpy(PropertyArray->first[18].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.pmt_pid);
  memcpy(PropertyArray->first[19].value, ar_value, value_len * 2);

  value_len = intTowchar(ar_value, pg.ecm_num);
  memcpy(PropertyArray->first[20].value, ar_value, value_len * 2);

  if(pg.ecm_num)
  {
    memcpy(PropertyArray->first[21].value, pg.cas_ecm, pg.ecm_num * sizeof(cas_desc_t));
  }

  value_len = intTowchar(ar_value, pg.audio->p_id);
  memcpy(PropertyArray->first[22].value, ar_value, value_len * 2);

  if(pg.audio->type > 2)
  {
    pg.audio->type = 2;
  }
  value_len = wcslen(PG_audio_type[pg.audio->type]);
  memcpy(PropertyArray->first[23].value, PG_audio_type[pg.audio->type], value_len * 2);

  value_len = intTowchar(ar_value, pg.audio->language_index);
  memcpy(PropertyArray->first[24].value, ar_value, value_len * 2);

  return PropertyArray;
}


int Parse(unsigned char *buff,
          long size,
          Type type,
          struct NativePropertyTable **SATtable,
          struct NativePropertyTable **TPtable,
          struct NativePropertyTable **PGtable)
{
  int cur_sat_no = 0, cur_tp_no = 0, cur_pg_no = 0;
  u8 sat_vv_id = 0;
  u8 tp_vv_id = 0;
  u8 pg_vv_id = 0;
  u16 sat_all_count = 0;
  u16 tp_all_count = 0;
  u16 pg_all_count = 0;
  u16 sat_id = 0xFFFF, tp_id = 0xFFFF, pg_id = 0xFFFF;
  u16 longitude = 0;
  BOOL is_mark;

  sat_node_t sat;
  dvbs_tp_node_t tp;
  dvbs_prog_node_t pg;

  sat_vv_id = db_dvbs_create_view(DB_DVBS_SAT, 0, NULL);
  sat_all_count = db_dvbs_get_count(sat_vv_id);
  tp_vv_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  tp_all_count = db_dvbs_get_count(tp_vv_id);
  pg_vv_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  pg_all_count = db_dvbs_get_count(pg_vv_id);

  //create table
  *SATtable = (struct NativePropertyTable *)malloc(sizeof(struct NativePropertyTable));
  *TPtable = (struct NativePropertyTable *)malloc(sizeof(struct NativePropertyTable));
  *PGtable = (struct NativePropertyTable *)malloc(sizeof(struct NativePropertyTable));

  memset((*SATtable), 0, sizeof(struct NativePropertyTable));
  memset((*TPtable), 0, sizeof(struct NativePropertyTable));
  memset((*PGtable), 0, sizeof(struct NativePropertyTable));

  (*SATtable)->first = (struct NativePropertyArray **)malloc(
    sizeof(struct NativePropertyArray *) * sat_all_count);
  (*TPtable)->first = (struct NativePropertyArray **)malloc(sizeof(struct NativePropertyArray *) *
                                                            tp_all_count);
  (*PGtable)->first = (struct NativePropertyArray **)malloc(sizeof(struct NativePropertyArray *) *
                                                            pg_all_count);
  memset((*SATtable)->first, 0, sizeof(struct NativePropertyArray *) * sat_all_count);
  memset((*TPtable)->first, 0, sizeof(struct NativePropertyArray *) * tp_all_count);
  memset((*PGtable)->first, 0, sizeof(struct NativePropertyArray *) * pg_all_count);

  //sat
  (*SATtable)->size = sat_all_count;
  for(cur_sat_no = 0; cur_sat_no < sat_all_count; cur_sat_no++)
  {
    sat_id = db_dvbs_get_id_by_view_pos(sat_vv_id, cur_sat_no);
    db_dvbs_get_sat_by_id(sat_id, &sat);
    //Select 
    is_mark =
      db_dvbs_get_mark_status(sat_vv_id, cur_sat_no , DB_DVBS_SEL_FLAG, 0);
    //Parse sat
    (*SATtable)->first[cur_sat_no] = Parse_sat(is_mark,sat);
  }
  //tp
  (*TPtable)->size = tp_all_count;
  for(cur_tp_no = 0; cur_tp_no < tp_all_count; cur_tp_no++)
  {
    tp_id = db_dvbs_get_id_by_view_pos(tp_vv_id, cur_tp_no);
    db_dvbs_get_tp_by_id(tp_id, &tp);
    //Select 
    is_mark =
      db_dvbs_get_mark_status(tp_vv_id, cur_tp_no, DB_DVBS_SEL_FLAG, 0);   
    //Parse tp
    (*TPtable)->first[cur_tp_no] = Parse_tp(is_mark,tp);
  }
  //pg
  (*PGtable)->size = pg_all_count;
  for(cur_pg_no = 0; cur_pg_no < pg_all_count; cur_pg_no++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(pg_vv_id, cur_pg_no);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    //Parse pg
    (*PGtable)->first[cur_pg_no] = Parse_pg(pg);
  }

  //help to free
  p_SATtable = *SATtable;
  p_TPtable = *TPtable;
  p_PGtable = *PGtable;
  //free dm db
  dm_destory_v2();
  db_destory_database_16v2();
  db_dvbs_release();

  return 1;
}


int Template_Clean(struct NativePropertyArray *PropertyArray)
{
  free(PropertyArray->first);
  free(PropertyArray);
  return 1;
}


int Clean()
{
  int cur_sat_no = 0, cur_tp_no = 0, cur_pg_no = 0;

  //free sat malloc
  for(cur_sat_no = 0; cur_sat_no < p_SATtable->size; cur_sat_no++)
  {
    Template_Clean(p_SATtable->first[cur_sat_no]);
  }
  free(p_SATtable->first);
  free(p_SATtable);
  //free TP malloc
  for(cur_tp_no = 0; cur_tp_no < p_TPtable->size; cur_tp_no++)
  {
    Template_Clean(p_TPtable->first[cur_tp_no]);
  }
  free(p_TPtable->first);
  free(p_TPtable);
  //free PG malloc
  for(cur_pg_no = 0; cur_pg_no < p_PGtable->size; cur_pg_no++)
  {
    Template_Clean(p_PGtable->first[cur_pg_no]);
  }
  free(p_PGtable->first);
  free(p_PGtable);
  return 1;
}


void ReOrder_sat(NativePropertyArray *PropertyArray, sat_node_t *sat)
{
  int value_len;
  int i = 0;
  u32 lnbtype, freq1, freq2;
  u8 is_unicable, unicable_type;
  u16 lnb_pos = 0, dis_opt = 0;
  u32 longitude;
  wchar_t L_value[256] = {'\0'};

  sat->id = PropertyArray->id;

  value_len = wcslen(PropertyArray->first[0].value);
  memcpy(sat->name, PropertyArray->first[0].value, value_len * 2);
    
  checklongitude(L_value, PropertyArray->first[2].value);
  longitude = (u32)wcharToint(L_value);
  if(longitude > SAT_LONGITUDE_MAX)
  {
    longitude = SAT_LONGITUDE_MAX;
  }
  else if(longitude < SAT_LONGITUDE_MIN)
  {
    longitude = SAT_LONGITUDE_MIN;
  }

  if(wcscmp(PropertyArray->first[3].value, Sat_longitude_type[0]) == 0)
  {
    SET_LONGITUDE(longitude, 1);
  }
  else
  {
    SET_LONGITUDE(longitude, 0);
  }
  sat->longitude = longitude;

  //PropertyArray->first[1].m_validation(&PropertyArray->first[1],PropertyArray);

  for(i = 0; i < 17; i++)
  {
    if(wcscmp(PropertyArray->first[4].value, Sat_LNB_freq[i]) == 0)
    {
      lnb_pos = i;
      break;
    }
  }
  ui_dbase_pos2freq((u16)lnb_pos, &lnbtype, &freq1, &freq2, &is_unicable, &unicable_type);
  sat->lnb_type = (u32)lnbtype;
  sat->lnb_low = (u32)freq1;
  sat->lnb_high = (u32)freq2;
  sat->p_is_unicable = is_unicable;
  sat->unicable_type = unicable_type;

  sat->user_band = (u32)wcharToint(PropertyArray->first[5].value);

  sat->band_freq = (u32)wcharToint(PropertyArray->first[6].value);

  for(i = 0; i < 5; i++)
  {
    if(wcscmp(PropertyArray->first[7].value, Sat_diseqc_port[i]) == 0)
    {
      dis_opt = i;
      break;
    }
  }
  if(dis_opt > 0)
  {
    sat->diseqc_type = 0x1;          /* 1/4 */
    sat->diseqc_port = dis_opt - 1;
  }
  else
  {
    sat->diseqc_type = sat->diseqc_port = 0;          /* OFF */
  }

  for(i = 0; i < 17; i++)
  {
    if(wcscmp(PropertyArray->first[8].value, Sat_diseqc11_port[i]) == 0)
    {
      dis_opt = i;
      break;
    }
  }
  if(dis_opt > 0)
  {
    sat->diseqc11_type = 0x2;          /* 2cascade */
    sat->diseqc11_port = dis_opt - 1;
  }
  else
  {
    sat->diseqc11_type = sat->diseqc11_port = 0;          /* OFF */
  }

  for(i = 0; i < 2; i++)
  {
    if(wcscmp(PropertyArray->first[9].value, Sat_K22[i]) == 0)
    {
      sat->k22 = i;
      break;
    }
  }
  for(i = 0; i < 3; i++)
  {
    if(wcscmp(PropertyArray->first[10].value, Sat_Polarity[i]) == 0)
    {
      sat->lnb_power = i;
      break;
    }
  }

  sat->position = (u32)wcharToint(PropertyArray->first[11].value);

  sat->positioner_type = sat->position != 0 ? 1 : 0; // DiSEqC1.2;

  for(i = 0; i < 3; i++)
  {
    if(wcscmp(PropertyArray->first[12].value, Sat_motor_type[i]) == 0)
    {
      sat->motor_type = i;
      break;
    }
  }
}


void ReOrder_tp(NativePropertyArray *PropertyArray, dvbs_tp_node_t *tp)
{
  int i = 0;

  tp->id = PropertyArray->id;

  tp->freq = (u32)wcharToint(PropertyArray->first[0].value);
  tp->sym = (u32)wcharToint(PropertyArray->first[1].value);

  for(i = 0; i < 2; i++)
  {
    if(wcscmp(PropertyArray->first[2].value, TP_polarity[i]) == 0)
    {
      tp->polarity = i;
      break;
    }
  }
  tp->nit_pid = (u32)wcharToint(PropertyArray->first[4].value);
  tp->pat_version = (u32)wcharToint(PropertyArray->first[5].value);

  for(i = 0; i < 2; i++)
  {
    if(wcscmp(PropertyArray->first[6].value, TP_is_scramble[i]) == 0)
    {
      tp->is_scramble = i;
      break;
    }
  }
  for(i = 0; i < 2; i++)
  {
    if(wcscmp(PropertyArray->first[7].value, TP_is_on22k[i]) == 0)
    {
      tp->is_on22k = i;
      break;
    }
  }
  tp->nim_type = (u32)wcharToint(PropertyArray->first[8].value);
  tp->emm_num = (u32)wcharToint(PropertyArray->first[9].value);
}


u32 on_fav_set(wchar_t *src)
{
  RET_CODE ret = SUCCESS;
  u16 param;
  u8 dig, j, focus = NULL;
  u8 mask_shift = 0;  // Parameter for view operation
  u32 fav_grp_flag = 0;
  wchar_t pg_fav[8][20] = {'\0'};
  int fav_num;
  BOOL is_set;

  fav_num = findfavflag(pg_fav, src);
  for(j = 0; j < fav_num; j++)
  {
    for(dig = 0; dig < MAX_FAV_GRP; dig++)
    {
      if(wcscmp(pg_fav[j], PG_Fav_grp_flag[dig]) == 0)
      {
        focus = dig;
        break;
      }
	  if(dig == MAX_FAV_GRP - 1)
	  {
	   return fav_grp_flag;
	  }
    }
    param = (focus | DB_DVBS_PARAM_ACTIVE_FLAG);
    is_set = ((param & 0x8000) != 0) ? TRUE : FALSE;
    mask_shift = param & 0x7FFF;

    if(is_set == TRUE)
    {
      fav_grp_flag |= 1 << mask_shift;
    }
    else
    {
      fav_grp_flag &= ~(1 << mask_shift);
    }
  }

  return fav_grp_flag;
}


void ReOrder_pg(NativePropertyArray *PropertyArray, dvbs_prog_node_t *pg)
{
  int value_len;
  int i = 0;

  pg->id = PropertyArray->id;

  value_len = wcslen(PropertyArray->first[0].value);
  memcpy(pg->name, PropertyArray->first[0].value, value_len * 2);

  pg->fav_grp_flag = on_fav_set(PropertyArray->first[1].value);

  pg->tv_flag = (u32)wcharToint(PropertyArray->first[2].value);

  pg->lck_flag = (u32)wcharToint(PropertyArray->first[3].value);

  pg->skp_flag = (u32)wcharToint(PropertyArray->first[4].value);

  pg->volume = (u32)wcharToint(PropertyArray->first[5].value);

  for(i = 0; i < 3; i++)
  {
    if(wcscmp(PropertyArray->first[6].value, PG_audio_channel[i]) == 0)
    {
      pg->audio_channel = i;
      break;
    }
  }

  for(i = 0; i < 2; i++)
  {
    if(wcscmp(PropertyArray->first[7].value, PG_is_scrambled[i]) == 0)
    {
      pg->is_scrambled = i;
      break;
    }
  }

  pg->audio_ch_num = (u32)wcharToint(PropertyArray->first[8].value);

  pg->video_pid = (u32)wcharToint(PropertyArray->first[9].value);

  pg->pcr_pid = (u32)wcharToint(PropertyArray->first[10].value);

  for(i = 0; i < 3; i++)
  {
    if(wcscmp(PropertyArray->first[11].value, PG_audio_track[i]) == 0)
    {
      pg->audio_track = i;
      break;
    }
  }
  pg->service_type = (u32)wcharToint(PropertyArray->first[12].value);

  pg->video_type = (u32)wcharToint(PropertyArray->first[13].value);

  pg->s_id = (u32)wcharToint(PropertyArray->first[14].value);

  pg->orig_net_id = (u32)wcharToint(PropertyArray->first[15].value);

  pg->ts_id = (u32)wcharToint(PropertyArray->first[16].value);

  pg->default_order = (u32)wcharToint(PropertyArray->first[17].value);

  pg->ca_system_id = (u32)wcharToint(PropertyArray->first[18].value);

  pg->pmt_pid = (u32)wcharToint(PropertyArray->first[19].value);

  pg->ecm_num = (u32)wcharToint(PropertyArray->first[20].value);

  value_len = wcslen(PropertyArray->first[21].value);
  memcpy(pg->cas_ecm, PropertyArray->first[21].value, value_len * 2);

  pg->audio->p_id = (u32)wcharToint(PropertyArray->first[22].value);

  for(i = 0; i < 3; i++)
  {
    if(wcscmp(PropertyArray->first[23].value, PG_audio_type[i]) == 0)
    {
      pg->audio->type = i;
      break;
    }
  }

  pg->audio->language_index = (u32)wcharToint(PropertyArray->first[24].value);
}


void db_clear(void)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_dvbs_init();
  db_clean(p_db_handle);
}


int ReOrder(struct NativePropertyTable *SATtable,
            struct NativePropertyTable *TPtable,
            struct NativePropertyTable *PGtable,
            Type type, 
            unsigned char *buff,
            long *size)
{
  int cur_sat_no = 0, cur_tp_no = 0, cur_pg_no = 0;
  u8 sat_vv_id = 0;
  u8 tp_vv_id = 0;
  u8 pg_vv_id = 0;
  u16 param = 0;

  sat_node_t sat = {0};
  dvbs_tp_node_t tp = {0};
  dvbs_prog_node_t pg = {0};

  //clear flash.bin
  db_clear();

  sat_vv_id = db_dvbs_create_view(DB_DVBS_SAT, 0, NULL);
  tp_vv_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  pg_vv_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);

  //sat
  for(cur_sat_no = 0; cur_sat_no < SATtable->size; cur_sat_no++)
  {
    //ReOrder sat
    u32 sat_id = SATtable->first[cur_sat_no]->id;

    memset(&sat, 0, sizeof(sat));
    ReOrder_sat(SATtable->first[cur_sat_no], &sat);
    //Select
    if(wcscmp(SATtable->first[cur_sat_no]->first[1].value, L"0") == 0)
    {
     param = 0;
    }
    else
    {
     param = DB_DVBS_PARAM_ACTIVE_FLAG;
    }
    db_dvbs_add_satellite(sat_vv_id, &sat);
    db_dvbs_change_mark_status(sat_vv_id, cur_sat_no, DB_DVBS_SEL_FLAG, param);
    for(cur_tp_no = 0; cur_tp_no < TPtable->size; cur_tp_no++)
    {
      u32 tp_id = TPtable->first[cur_tp_no]->id;
      if(sat_id == TPtable->first[cur_tp_no]->parent_id)
      {
        memset(&tp, 0, sizeof(tp));
        tp.sat_id = sat.id;
        ReOrder_tp(TPtable->first[cur_tp_no], &tp);
        //Select
        if(wcscmp(TPtable->first[cur_tp_no]->first[3].value,L"0") == 0)
        {
          param = 0;
        }
        else
        {
           param = DB_DVBS_PARAM_ACTIVE_FLAG;
        }
        db_dvbs_add_tp(tp_vv_id, &tp);
		  db_dvbs_change_mark_status(tp_vv_id, cur_tp_no, DB_DVBS_SEL_FLAG, param);
        for(cur_pg_no = 0; cur_pg_no < PGtable->size; cur_pg_no++)
        {
          if(tp_id == PGtable->first[cur_pg_no]->parent_id)
          {
            memset(&pg, 0, sizeof(pg));
            pg.tp_id = tp.id;
            pg.sat_id = tp.sat_id;
            ReOrder_pg(PGtable->first[cur_pg_no], &pg);
            db_dvbs_add_program(pg_vv_id, &pg);
          }
        }
      }
    }
  }

  db_dvbs_save(pg_vv_id);
  db_dvbs_save(tp_vv_id);
  db_dvbs_save(sat_vv_id);

    //free dm db
   dm_destory_v2();
   db_destory_database_16v2();
   db_dvbs_release();
   
  if(type == USER_DB)
  {
   flash_to_userdb(buff);
  }

  return 1;
}
