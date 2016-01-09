/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_dvr_config.h"
#include "ui_hdd_info.h"
#include "vfs.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_HDD_INFO_TOTAL, 
  IDC_HDD_INFO_FREE,
  IDC_HDD_INFO_USED,
  IDC_HDD_INFO_TIMESHIFT,
  IDC_HDD_INFO_RECFREE,
  IDC_HDD_INFO_FILESYS,
  IDC_HDD_INFO_HEP,
};

static u16 hdd_info_cont_keymap(u16 key);

RET_CODE hdd_info_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

/*
  all use k bytes
*/
u32 hdd_info_time_shift_free(void)
{
#ifdef ENABLE_TIMESHIFT_CONFIG
  if(sys_status_get_timeshift_switch())
  {
    return (u32)file_list_get_file_size(ui_usb_get_timeshift_file_name())/1024;
  }
#endif  
  return 0;
}

RET_CODE open_hdd_info(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[HDD_INFO_ITEM_CNT];
  u8 i = 0;
  u16 y = 0;
  u16 stxt [HDD_INFO_ITEM_CNT] =
  { IDS_TOTAL, IDS_REST_SPACE, IDS_USED,
    IDS_TIME_SHIFT, IDS_REC_FREE, IDS_FILE_SYSTEM};

  u8 asc_str[32];
  vfs_dev_info_t dev_info = {0};
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;
  u8 fs_type = 0;
  u16 partion_content = 0;
  u32 total_size = 0, free_size = 0, used_size = 0;

  if(!(ui_get_usb_status()))
  {
    return SUCCESS;
  }
  
  partition_cnt = vfs_get_partitions(partition_letter, 10);
  
  //OS_PRINTF("partition_cnt = %d\n",partition_cnt);
  partion_content = sys_status_get_usb_work_partition();//default letter in dvr configuration menu
  //OS_PRINTF("partion_content = %d\n",partion_content);
  for (i = 0; i < partition_cnt; i++)
  {
    vfs_get_dev_info(partition_letter[i],&dev_info);
    total_size += dev_info.total_size;
    free_size += dev_info.free_size;
    used_size += dev_info.used_size;
    OS_PRINTF("free_size = %d\n",dev_info.free_size);
    OS_PRINTF("dev_info.fs_type = %d\n",dev_info.fs_type);
    OS_PRINTF("total_size = %d\n",dev_info.total_size);
    OS_PRINTF("used_size = %d\n",dev_info.used_size);
    if(i == partion_content)
    {
      fs_type = dev_info.fs_type;//default letter in dvr configuration menu
    }
  }
  
#if 1//#ifndef SPT_SUPPORT
  p_cont =
    ui_comm_root_create(ROOT_ID_HDD_INFO, 0, COMM_BG_X, COMM_BG_Y,
    COMM_BG_W, COMM_BG_H, RSC_INVALID_ID, IDS_HDD_INFO);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, hdd_info_cont_keymap);
  ctrl_set_proc(p_cont, hdd_info_cont_proc);
  
  y = HDD_INFO_ITEM_Y;
  for (i = 0; i < HDD_INFO_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_HDD_INFO_TOTAL + i),
    HDD_INFO_ITEM_X, y, HDD_INFO_ITEM_LW, HDD_INFO_ITEM_RW);
    ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
    
    y += HDD_INFO_ITEM_H + HDD_INFO_ITEM_V_GAP;
  }

  //ui_comm_static_set_rstyle(p_ctrl[0], RSI_COMM_CONT_SH, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH);
  //ui_comm_static_set_rstyle(p_ctrl[1], RSI_COMM_CONT_SH, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH);


  //total
  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(total_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[0], asc_str);

  // free
  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(free_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[1], asc_str);

  // used
  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(used_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[2], asc_str);

  //timeshift free and rec free
  {
    ui_conver_file_size_unit(hdd_info_time_shift_free(), asc_str);
    ui_comm_static_set_content_by_ascstr(p_ctrl[3], asc_str);
    ui_conver_file_size_unit(free_size, asc_str);
    ui_comm_static_set_content_by_ascstr(p_ctrl[4], asc_str);
  }

  if(fs_type == VFS_FAT16)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], (u8 *)"FAT16");
  }
  else if(fs_type == VFS_FAT32)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], (u8 *)"FAT32");
  }
  else if(fs_type == VFS_NTFS)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], (u8 *)"NTFS");
  }
  else
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], (u8 *)"UNKNOW");
  }
  
#else

#endif
 
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

static RET_CODE on_hdd_info_select(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  //control_t * p_active;
  //u8 active_id;

  //p_active = ctrl_get_active_ctrl(p_cont);
  //active_id = ctrl_get_ctrl_id(p_active);
  
  return SUCCESS;
}

static RET_CODE on_hdd_info_init_ok(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  //control_t *p_name, *p_id;  
  //p_name = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_SMC);
  //p_id = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_CNM);

  //ui_comm_static_set_content_by_ascstr(p_name, "dfgv");

  //ui_comm_static_set_content_by_ascstr(p_id, "sdgf");

  //ctrl_paint_ctrl(p_name, TRUE);
  //ctrl_paint_ctrl(p_id, TRUE);

  return SUCCESS;
}

static RET_CODE on_hdd_info_init_failed(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  //control_t * p_name, *p_id;

  //p_name = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_SMC);
  //p_id = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_CNM);

  //ui_comm_static_set_content_by_ascstr(p_name, " ");
  //ui_comm_static_set_content_by_ascstr(p_id, " ");
  //
  //ctrl_paint_ctrl(p_name, TRUE);
  //ctrl_paint_ctrl(p_id, TRUE);
  return SUCCESS;
}

static RET_CODE on_hdd_info_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_HDD_INFO, 0, 0);
  
  return SUCCESS;
}

BEGIN_KEYMAP(hdd_info_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(hdd_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(hdd_info_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_hdd_info_select)
  ON_COMMAND(MSG_CA_INIT_OK, on_hdd_info_init_ok)
  ON_COMMAND(MSG_CA_INIT_FAILED, on_hdd_info_init_failed)
  ON_COMMAND(MSG_CA_CARD_OUT, on_hdd_info_init_failed)  
  ON_COMMAND(MSG_PLUG_OUT, on_hdd_info_plug_out)  
END_MSGPROC(hdd_info_cont_proc, ui_comm_root_proc)
