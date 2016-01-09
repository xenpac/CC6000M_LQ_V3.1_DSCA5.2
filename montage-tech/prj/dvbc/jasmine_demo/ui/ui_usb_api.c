/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "vfs.h"

#define DMH_HEAD_SIZE  (1*KBYTES)
static  u8 g_read_data[DMH_HEAD_SIZE] = {0};
u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE] = "*^_^*DM(^o^)";
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


static BOOL file_info_exchange(media_file_t * p_file1, media_file_t *p_file2)
{
    media_file_t tmp_file = {DIRECTORY,};
    u16 *p_tmp_addr = NULL;

    if((NULL == p_file1) || (NULL == p_file2))
    {
      return FALSE;
    }
    memcpy(&tmp_file, p_file1, sizeof(media_file_t));
    memcpy(p_file1, p_file2, sizeof(media_file_t));
    memcpy(p_file2, &tmp_file, sizeof(media_file_t));

    p_tmp_addr = p_file1->p_name;
    p_file1->p_name = p_file2->p_name;
    p_file2->p_name = p_tmp_addr;

    return TRUE;
}

//lint -e784
static BOOL find_dmh_indicator(hfile_t file)
{
  u32 offset = 0;
  u32 read_size = 0;
  const u32 sec_size = 64*1024;
  u8 dmh_indicator[USB_UPG_DMH_INDC_SIZE] = "*^_^*DM(^o^)";
  vfs_file_info_t file_info = {0};
  
  vfs_get_file_info(file, &file_info);

  memset(g_read_data, 0, DMH_HEAD_SIZE);
  
  while(offset <= (file_info.file_size - sec_size))
  {
    vfs_seek(file, (s64)offset, VFS_SEEK_SET);
    read_size = vfs_read(g_read_data, USB_UPG_DMH_INDC_SIZE, 1, file);
    if(USB_UPG_DMH_INDC_SIZE == read_size)
    { 
        if(memcmp(g_read_data, dmh_indicator, USB_UPG_DMH_INDC_SIZE) == 0)
        {
          OS_PRINTF("get dmh indicator at offset 0x%x\n", offset);
          return TRUE;
        }
    }

    offset += sec_size;
  }

  OS_PRINTF("Cannot find dmh indicator!!!\n");
  return FALSE;
}

static BOOL is_flie_data_valid(hfile_t file, u8 data_type)
{
  u32 read_size = 0;
  BOOL is_valid = FALSE;
  u8 dmh_indicator[12] = "*^_^*DM(^o^)";
  memset(g_read_data, 0, DMH_HEAD_SIZE);
  
  if(data_type == UPG_CA_KEY)
  {
    is_valid = TRUE;
  }
  else
  {
        vfs_seek(file, (s64)((UPG_USER_DATA == data_type)? 0 : 0xc), VFS_SEEK_SET);
        read_size = vfs_read(g_read_data, USB_UPG_DMH_INDC_SIZE, 1, file);
        if(USB_UPG_DMH_INDC_SIZE == read_size)
        { 
            if(memcmp(g_read_data, dmh_indicator, USB_UPG_DMH_INDC_SIZE) == 0)
            {
                is_valid = TRUE;
            }
            else
            {
                is_valid = find_dmh_indicator(file);
            }
        }
  }

  return is_valid;
}

static BOOL is_flie_valid(u16 *p_path, u8 data_type)
{
    hfile_t file = NULL;
    vfs_file_info_t file_info = {0};
    BOOL is_valid = FALSE;

    file = vfs_open(p_path, VFS_READ);
    if(file == NULL)
    {
      OS_PRINTF("open file failed\n");
      return FALSE;
    }
    vfs_get_file_info(file, &file_info);

    switch(data_type)
    {
      case UPG_ALL_CODE:
      case UPG_MAIN_CODE:
      case UPG_APP_ALL:
        if(file_info.file_size != CHARSTO_SIZE)
        {
          break;
        }
        is_valid = is_flie_data_valid(file, data_type);
        break;
        case UPG_USER_DATA:
          if(file_info.file_size != (dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), IW_TABLE_BLOCK_ID) + \
                                               dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), IW_VIEW_BLOCK_ID) + \
                                               USB_UPG_DMH_INDC_SIZE))
          {
              break;
          }
          is_valid = is_flie_data_valid(file, data_type);
          break;
        default:
          MT_ASSERT(0);
          break;

    }

    vfs_close(file);
    return is_valid;
}

BOOL ui_is_file_exist_in_usb(u16 *p_path)
{
    hfile_t file = NULL;

    if(NULL == p_path)
    {
        return FALSE;
    }

    file = vfs_open(p_path, VFS_READ);
    if (file == NULL)
    {
      return FALSE;
    }
    vfs_close(file);

    return TRUE;
}

RET_CODE ui_get_upg_file_version(u16 *p_path, upg_file_ver_t *p_upg_file_ver)
{
    hfile_t file = NULL;
    vfs_file_info_t file_info = {0};
    dmh_block_info_t *p_block_info = NULL;
    dmh_info_t *p_dmh_info = NULL;
    u32 read_size = 0;
    u32 mem_offset = 0, flash_offset = 0;
    u8 dmh_indicator[12] = "*^_^*DM(^o^)";
    BOOL is_find = FALSE;
    u16 i = 0;

    if((NULL == p_path) || (NULL == p_upg_file_ver))
    {
        return ERR_FAILURE;
    }

    file = vfs_open(p_path, VFS_READ);
    if(file == NULL)
    {
      OS_PRINTF("open file failed\n");
      return ERR_FAILURE;
    }

    vfs_get_file_info(file, &file_info);
    if(file_info.file_size != CHARSTO_SIZE)
    {
      vfs_close(file);
        return ERR_FAILURE;
    }

    memset(g_read_data, 0, DMH_HEAD_SIZE);

    flash_offset = DM_HDR_START_ADDR;
    vfs_seek(file, (s64)flash_offset, VFS_SEEK_SET);
    read_size = vfs_read(g_read_data, DMH_HEAD_SIZE, 1, file);

    if(DMH_HEAD_SIZE == read_size)
    {
        p_dmh_info = (dmh_info_t *)g_read_data;
        mem_offset += sizeof(dmh_info_t);

        if(0 != memcmp(dmh_indicator,p_dmh_info->dmh_indicator,12))
        {
            vfs_close(file);
            return ERR_FAILURE;
        }

        for(i = 0; i < p_dmh_info->block_cnt; i++)
        {
            p_block_info = (dmh_block_info_t *)(g_read_data + mem_offset);

            if(DM_MAINCODE_BLOCK_ID == p_block_info->id)
            {
                is_find = TRUE;
                flash_offset += p_block_info->base_addr;
                break;
            }
            mem_offset += sizeof(dmh_block_info_t);
        }
    }

    if(is_find)
    {
      is_find = FALSE;
      mem_offset = 0;
      
      vfs_seek(file, (s64)flash_offset, VFS_SEEK_SET);
      memset(g_read_data, 0, DMH_HEAD_SIZE);
      read_size = vfs_read(g_read_data, DMH_HEAD_SIZE, 1, file);

      if(DMH_HEAD_SIZE == read_size)
      {
        p_dmh_info = (dmh_info_t *)g_read_data;
        mem_offset += sizeof(dmh_info_t);

        if(0 != memcmp(dmh_indicator,p_dmh_info->dmh_indicator,12))
        {
            vfs_close(file);
            return ERR_FAILURE;
        }

        for(i = 0; i < p_dmh_info->block_cnt; i++)
        {
            p_block_info = (dmh_block_info_t *)(g_read_data + mem_offset);
            if(OTA_BLOCK_ID == p_block_info->id)
            {
                  memcpy(p_upg_file_ver->ota_version, p_block_info->version, 8);
            }
            if(SS_DATA_BLOCK_ID == p_block_info->id)
            {
                is_find = TRUE;
                flash_offset += p_block_info->base_addr;
                break;
            }
            mem_offset += sizeof(dmh_block_info_t);
        }
      }
    }
    
    if(is_find)
    {
      vfs_seek(file, (s64)flash_offset, VFS_SEEK_SET);
      memset(g_read_data, 0, DMH_HEAD_SIZE);
      read_size = vfs_read(g_read_data, 24, 1, file);
      if(24 == read_size)
      {
          memcpy(p_upg_file_ver->changeset, g_read_data, 20);
          p_upg_file_ver->changeset[19] = '\0';
          p_upg_file_ver->sw_version = (g_read_data[20])*16*16*16
                                     + (g_read_data[21])*16*16
                                     + (g_read_data[22])*16
                                     + (g_read_data[23]);
      }
      else
      {
          is_find = FALSE;
      }
    }

    vfs_close(file);

    if(is_find)
    {
        return SUCCESS;
    }

    return ERR_FAILURE;
}
//lint +e784

BOOL ui_upg_file_list_get(flist_dir_t dir, flist_option_t option,
                                                file_list_t *p_list, u8 data_mode)
{
  u16 i = 0;
  file_list_t file_list = {0};
  u32 valid_file_cnt = 0;
  BOOL is_valid = TRUE;

  if((NULL == dir) || (NULL == p_list))
  {
    return FALSE;
  }
  file_list_get(dir, option, &file_list);
  OS_PRINTF("[USB]%s:file count[0x%x]\n", __FUNCTION__, file_list.file_count);
  valid_file_cnt = file_list.file_count;

  for(i = 0; i < file_list.file_count; i++)
  {
    if(!is_valid)
    {
        i -= 1;
    }
    if(i >= valid_file_cnt)
    {
        break;
    }

    is_valid = is_flie_valid(file_list.p_file[i].name, data_mode);

    if(!is_valid)
    {
       file_info_exchange(&(file_list.p_file[i]), &(file_list.p_file[valid_file_cnt - 1]));
       valid_file_cnt--;
       continue;
    }
  }

  file_list.file_count = valid_file_cnt;
  memcpy(p_list, &file_list, sizeof(file_list_t));

  OS_PRINTF("[USB]%s:valid_file_cnt[0x%x]\n", __FUNCTION__, p_list->file_count);

  return TRUE;
}

//lint -e438 -e830
BOOL ui_find_usb_upg_file(void)
{
  u16 FileCount = 0;
  partition_t *p_partition = NULL;
  u32 partition_cnt = 0;
  flist_dir_t flist_dir = NULL;
  u16 ffilter_all[32] = {0};
  file_list_t file_list = {0};
  u16 i = 0;
  upg_file_ver_t upg_file_version = {{0},{0},{0},0};
  BOOL ret = FALSE;

  str_asc2uni((u8 *)"|bin|BIN|nodir", ffilter_all);

  p_partition = NULL;
  partition_cnt = 0;
  partition_cnt = file_list_get_partition(&p_partition);
  if(partition_cnt > 0)
  {
    flist_dir = file_list_enter_dir(ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, p_partition[0].letter);
    //MT_ASSERT(flist_dir != NULL); //by sliu
    ui_upg_file_list_get(flist_dir, FLIST_UNIT_FIRST, &file_list, UPG_ALL_CODE);
    FileCount = (u16)file_list.file_count;
     
    if(FileCount > 0)
    {
      for(i=0; i<FileCount; i++)
      {
          ui_get_upg_file_version(file_list.p_file[i].name, &upg_file_version);
          if(0 != memcmp(upg_file_version.changeset, sys_status_get()->changeset, 19))
          {
              ret = TRUE;
              break;
          }
       }
    }
    
    if(flist_dir)
    {
      file_list_leave_dir(flist_dir);
      flist_dir = NULL;
    }
  }
  return ret;
}
//lint +e438 +e830

void ui_release_usb_upgrade(u8 root_id)
{
    cmd_t cmd = {0};

    cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
    cmd.data1 = APP_USB_UPG;
    ap_frm_do_command(APP_FRAMEWORK, &cmd);

    fw_unregister_ap_evtmap(APP_USB_UPG);
    fw_unregister_ap_msghost(APP_USB_UPG, root_id);
}

