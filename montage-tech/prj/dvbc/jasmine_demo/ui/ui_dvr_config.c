/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
 #ifdef ENABLE_TIMESHIFT_CONFIG
#include "ui_common.h"

#include "ui_dvr_config.h"
#include "file_list.h"
#include "ui_timeshift_bar.h"
#include "vfs.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_TIMESHIFT,
  IDC_DVR_PARTITION,
  IDC_DVR_FORMAT_CONFIRM,
};

enum storage_format_confirm_cont_id
{
  IDC_DVR_FORMAT_CONFIRM_CONTENT = 1,
  IDC_DVR_FORMAT_CONFIRM_FAT,
  IDC_DVR_FORMAT_CONFIRM_NTFS,
  IDC_DVR_FORMAT_CONFIRM_CANCEL,
};

#define TIMESHIFT_MIN_SIZE (10*1024*1024)

#ifdef MIN_AV_64M
#define TIMESHIFT_MAN_SIZE (1000*1024*1024)
#endif
u16 dvr_config_cont_keymap(u16 key);

static u16 dvr_config_format_confirm_keymap(u16 key);
//RET_CODE dvr_config_format_confirm_proc(control_t * p_ctrl,
//  u16 msg, u32 para1, u32 para2);

static u16 dvr_config_format_fat_keymap(u16 key);
RET_CODE dvr_config_format_fat_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

//static u16 dvr_config_format_ntfs_keymap(u16 key);
//RET_CODE dvr_config_format_ntfs_proc(control_t * p_ctrl,
 // u16 msg, u32 para1, u32 para2);

static u16 dvr_config_format_cancel_keymap(u16 key);
RET_CODE dvr_config_format_cancel_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

static u16 timeshift_name[MAX_FILE_PATH] = {0};
static partition_t *p_partition = NULL;
static u16 g_format_msg = 0;
//lint -e438 -e550 -e830
u16 *ui_usb_get_timeshift_file_name(void)
{
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;

  partition_cnt = vfs_get_partitions(partition_letter, 10);

  if(partition_cnt == 0)
  {
    return NULL;
  }
  
  timeshift_name[0] = partition_letter[sys_status_get_usb_work_partition()];

  //sprintf(timeshift_name, "%c:\\timeshift.dat", letter);
  
  str_asc2uni((u8 *)":\\timeshift.dat", &timeshift_name[1]);
  return timeshift_name;
}
//lint +e438 +e550 +e830


RET_CODE dvr_config_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE dvr_config_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);


RET_CODE open_dvr_config(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[DVR_CONFIG_ITEM_CNT];
  control_t *p_dlg_cont, *p_content,*p_fat,*p_cancel;
  u8 i, j;
#if 1//#ifndef SPT_SUPPORT
  u16 stxt[DVR_CONFIG_ITEM_CNT] =
  {
    IDS_TIME_SHIFT, IDS_DVR_PATITION,
  };
  u8 on_off_str[2] = {IDS_OFF, IDS_ON};
  u16 y;
#endif
  u32 partition_cnt = 0;
  g_format_msg = 0;
  if(!(ui_get_usb_status()))
  {
    return SUCCESS;
  }

  partition_cnt = file_list_get_partition(&p_partition);
#if 1//#ifndef SPT_SUPPORT
  p_cont = ui_comm_root_create(ROOT_ID_DVR_CONFIG,
    0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, IM_TITLEICON_TV, IDS_DVR_CONFIG);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, dvr_config_cont_keymap);
  ctrl_set_proc(p_cont, dvr_config_cont_proc);

  y = DVR_CONFIG_ITEM_Y;
  for(i = 0; i < DVR_CONFIG_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_TIMESHIFT + i),
                                      DVR_CONFIG_ITEM_X, y,
                                      DVR_CONFIG_ITEM_LW,
                                      DVR_CONFIG_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], dvr_config_select_proc);

    if(i == 0)
    {
      ui_comm_select_set_param(p_ctrl[i], TRUE,
                               CBOX_WORKMODE_STATIC, (sizeof(on_off_str)/sizeof(u8)),
                               CBOX_ITEM_STRTYPE_STRID, NULL);
      for(j = 0; j < (sizeof(on_off_str)/sizeof(u8)); j++)
      {
        ui_comm_select_set_content(p_ctrl[i], j, on_off_str[j]);
      }
    }
    else if(i == 1)
    {
      ui_comm_select_set_param(p_ctrl[i], TRUE,
                               CBOX_WORKMODE_STATIC, (u16)partition_cnt,
                               CBOX_ITEM_STRTYPE_UNICODE, NULL);
      for(j = 0; j < partition_cnt; j++)
      {
        ui_comm_select_set_content_by_unistr(p_ctrl[i], j, p_partition[j].name);
      }
    }

    //ui_comm_select_create_droplist(p_ctrl[i], COMM_SELECT_DROPLIST_PAGE);
    ctrl_set_related_id(p_ctrl[i],
                        0, /* left */
                        (u8)((i - 1 + DVR_CONFIG_ITEM_CNT) % DVR_CONFIG_ITEM_CNT + 1), /* up */
                        0, /* right */
                        (u8)((i + 1) % DVR_CONFIG_ITEM_CNT + 1)); /* down */

    y += DVR_CONFIG_ITEM_H + DVR_CONFIG_ITEM_V_GAP;
  }
#else
#endif
  /* set focus according to current info */

  //sys_status_get_status(BS_DVR_ON, &is_dvr_on);
  ui_comm_select_set_focus(p_ctrl[0], (u16)sys_status_get_timeshift_switch());
  ui_comm_select_set_focus(p_ctrl[1], (u16)sys_status_get_usb_work_partition());


//hdd format confirm container
  p_dlg_cont = ctrl_create_ctrl(CTRL_CONT, IDC_DVR_FORMAT_CONFIRM, DVR_FORMAT_CONFIRM_CONT_X,
                                 DVR_FORMAT_CONFIRM_CONT_Y, DVR_FORMAT_CONFIRM_CONT_W,
                                 DVR_FORMAT_CONFIRM_CONT_H, p_cont, 0);
  ctrl_set_rstyle(p_dlg_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_dlg_cont, dvr_config_format_confirm_keymap);
  //ctrl_set_proc(p_dlg_cont, dvr_config_format_confirm_proc);

  ctrl_set_sts(p_dlg_cont, OBJ_STS_HIDE);

  //content
  p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_DVR_FORMAT_CONFIRM_CONTENT,
                           DVR_FORMAT_CONFIRM_CONTENT_X, DVR_FORMAT_CONFIRM_CONTENT_Y,
                           DVR_FORMAT_CONFIRM_CONTENT_W, DVR_FORMAT_CONFIRM_CONTENT_H,
                           p_dlg_cont, 0);
  text_set_font_style(p_content, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_content, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_content, IDS_TIMESHIFT_FORMAT_USB);

  //fat btn
  p_fat = ctrl_create_ctrl(CTRL_TEXT, IDC_DVR_FORMAT_CONFIRM_FAT,
                           DVR_FORMAT_CONFIRM_FAT_X,DVR_FORMAT_CONFIRM_FAT_Y,
                           DVR_FORMAT_CONFIRM_FAT_W, DVR_FORMAT_CONFIRM_FAT_H,
                           p_dlg_cont, 0);
  ctrl_set_rstyle(p_fat, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_fat, dvr_config_format_fat_keymap);
  ctrl_set_proc(p_fat, dvr_config_format_fat_proc);
  text_set_font_style(p_fat, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_content_type(p_fat, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_fat, IDS_FAT);

  //cancel btn
  p_cancel= ctrl_create_ctrl(CTRL_TEXT, IDC_DVR_FORMAT_CONFIRM_CANCEL,
                             DVR_FORMAT_CONFIRM_CANCEL_X, DVR_FORMAT_CONFIRM_CANCEL_Y,
                             DVR_FORMAT_CONFIRM_CANCEL_W + 10, DVR_FORMAT_CONFIRM_CANCEL_H,
                             p_dlg_cont, 0);
  ctrl_set_rstyle(p_cancel, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_cancel, dvr_config_format_cancel_keymap);
  ctrl_set_proc(p_cancel, dvr_config_format_cancel_proc);
  text_set_font_style(p_cancel, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_content_type(p_cancel, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_cancel, IDS_CANCEL);

  ctrl_set_related_id(p_fat, IDC_DVR_FORMAT_CONFIRM_CANCEL, 0, IDC_DVR_FORMAT_CONFIRM_FAT, 0);
  ctrl_set_related_id(p_cancel, IDC_DVR_FORMAT_CONFIRM_FAT, 0, IDC_DVR_FORMAT_CONFIRM_CANCEL, 0);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}
//lint -e685 -e716
RET_CODE ui_timeshift_total_size(void)
{
  vfs_dev_info_t p_dev_info = {0};
  u64 timeshift_size = 0, i = 0;
  BOOL is_open = FALSE;
  ui_pvr_private_info_t * p_ts_data;
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;
  u8 work_partition = 0;
  u16 *p_timeshift_name = NULL;
  hfile_t file = NULL;
  vfs_file_info_t f_info = {0};

  partition_cnt = vfs_get_partitions(partition_letter, 10);
  if(partition_cnt == 0)
  {
    return ERR_FAILURE;
  }

  work_partition = sys_status_get_usb_work_partition();
  if(work_partition >= partition_cnt)
  {
    work_partition = 0;
    sys_status_set_usb_work_partition(work_partition);
  }
  
  vfs_get_dev_info(partition_letter[work_partition], &p_dev_info);
  p_ts_data = ui_timeshift_para_get();
  p_timeshift_name = ui_usb_get_timeshift_file_name();
  p_ts_data->file_name_len = uni_strlen(p_timeshift_name);
  memcpy(p_ts_data->file_name, p_timeshift_name, p_ts_data->file_name_len * sizeof(u16));

  file = vfs_open(p_timeshift_name, VFS_READ);
  if (file)
  {
    vfs_get_file_info(file, &f_info);
    vfs_close(file);
    OS_PRINTF("\n## timeshift exist is [%lld]\n", f_info.file_size);
#ifdef MIN_AV_64M
    if (f_info.file_size > TIMESHIFT_MAN_SIZE)
    {
      OS_PRINTF("\n## timeshift is too large\n");
      vfs_del(p_timeshift_name);
      mtos_task_sleep(100);
    }
    else
#endif
    if (f_info.file_size > TIMESHIFT_MIN_SIZE)
    {
      if (vfs_is_block_size_available(p_ts_data->file_name, (u32)f_info.file_size))
      {
        p_ts_data->data_size = (u32)f_info.file_size;
        return SUCCESS;
      }
    }
  } 
  if(p_dev_info.free_size  < TIMESHIFT_MIN_SIZE / 1024)
  {
    //reset timeshift data
    p_ts_data->file_name_len = 0;
    p_ts_data->data_size = 0;
    return ERR_FAILURE;
  }
  
  if(p_dev_info.free_size < (260 *KBYTES) && p_dev_info.free_size > (130 *KBYTES))
  {
    timeshift_size = (u64)(p_dev_info.free_size) * KBYTES;
  }
  else
  {
    timeshift_size =  (u64)(p_dev_info.free_size / 2) * KBYTES;
  }
#ifdef MIN_AV_64M
  if (timeshift_size >= TIMESHIFT_MAN_SIZE)
  {
    timeshift_size = (u64)1000 * MBYTES;
  }
#else
  if (timeshift_size >= 0xFFFFFFFF)
  {
    timeshift_size = (u64)4000 * MBYTES;
  }
#endif
  
  while(1)
  {
    i++;
    MT_ASSERT(i != 0);
    
    //temp solution fix q full
    if(timeshift_size < 130 * MBYTES) // > 130M
    {
      //reset timeshift data
      p_ts_data->file_name_len = 0;
      p_ts_data->data_size = 0;
      return ERR_FAILURE;
    }
    OS_PRINTF("\n##try timeshift [%lld]byte\n", timeshift_size);
    is_open =  vfs_is_block_size_available(p_ts_data->file_name, (u32)timeshift_size);

    if (is_open)
    {
      OS_PRINTF("\n## set timeshift ues[%lld]Mbyte\n", timeshift_size >> 20);
      p_ts_data->data_size = (u32)timeshift_size;
      break;
    }
    else
    {
      if(i == 2)
      {
        //reset timeshift data
        p_ts_data->file_name_len = 0;
        p_ts_data->data_size = 0;
        return ERR_FAILURE;
      }
      else
      {
        timeshift_size /= 2;
      }
    }
  }

  return SUCCESS;
}
//lint +e685 +e716
static RET_CODE dvr_config_format(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  control_t * p_cont, *p_confirm_cont, *p_cancel;
  p_cont = ctrl_get_parent(p_ctrl);
  p_confirm_cont = ctrl_get_child_by_id(p_cont, IDC_DVR_FORMAT_CONFIRM);
  p_cancel = ctrl_get_child_by_id(p_confirm_cont, IDC_DVR_FORMAT_CONFIRM_CANCEL);
  g_format_msg = msg;
  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);

  ctrl_set_attr(p_confirm_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_confirm_cont, OBJ_STS_SHOW);

  ctrl_set_active_ctrl(p_confirm_cont, p_cancel);
  ctrl_process_msg(p_cancel, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_confirm_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_dvr_config_format_cancel_select(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  control_t * p_cont, *p_confirm_cont, *p_hdd;

  p_confirm_cont = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_confirm_cont);
  p_hdd = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_confirm_cont, OBJ_STS_HIDE);

  ctrl_process_msg(p_hdd, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, TRUE);
  switch(g_format_msg)
  {
    case MSG_EXIT:
      manage_close_menu(ROOT_ID_DVR_CONFIG, 0, 0);
      break;
    case MSG_EXIT_ALL:
      ui_close_all_mennus();
      break;
    default:
      break;
  }
  g_format_msg = 0;
  sys_status_set_timeshift_switch(FALSE);
  sys_status_save();
  return SUCCESS;
}

static RET_CODE on_dvr_config_format_fat_select(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  control_t * p_cont, *p_confirm_cont, *p_hdd, *p_storage_partition;
  u16 focus;

  p_confirm_cont = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_confirm_cont);
  p_storage_partition = ctrl_get_child_by_id(p_cont, IDC_DVR_PARTITION);
  focus = ui_comm_select_get_focus(p_storage_partition);
  vfs_format(p_partition[focus].letter[0], VFS_FAT32);
  on_dvr_config_format_cancel_select(p_ctrl, msg, para1, para2);

  p_hdd = ctrl_get_child_by_id(p_cont, IDC_TIMESHIFT);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_confirm_cont, OBJ_STS_HIDE);

  ctrl_process_msg(p_hdd, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, TRUE);
  switch(g_format_msg)
  {
    case MSG_EXIT:
      manage_close_menu(ROOT_ID_DVR_CONFIG, 0, 0);
      break;
    case MSG_EXIT_ALL:
      ui_close_all_mennus();
      break;
    default:
      break;
  }
  g_format_msg = 0;
  sys_status_set_timeshift_switch(TRUE);
  sys_status_save();
  return SUCCESS;
}
#if 0

static RET_CODE on_select_change_focus(control_t *p_ctrl,
                                       u16 msg,
                                       u32 para1,
                                       u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 content = 0;
  control_t * p_cont, *p_parent;

  p_cont = ctrl_get_parent(p_ctrl);

  if(!ctrl_is_onfocus(p_ctrl))
  {
    return ERR_FAILURE;
  }

  switch(ctrl_get_ctrl_id(p_cont))
  {
    case IDC_TIMESHIFT:
      content = cbox_static_get_focus(p_ctrl);
      if(content == 0)
      {
        sys_status_set_timeshift_switch(FALSE);
        sys_status_save();
      }
      else
      {
        if(SUCCESS != ui_timeshift_total_size())
        {
          p_parent = ctrl_get_parent(p_ctrl);
          dvr_config_format(p_cont, msg, para1, para2);

          cbox_static_set_focus(p_ctrl, 0);
          ctrl_paint_ctrl(p_ctrl, FALSE);

          sys_status_set_timeshift_switch(FALSE);
          sys_status_save();
          return ERR_FAILURE;
        }

        sys_status_set_timeshift_switch(TRUE);
        sys_status_save();
      }

      break;
    case IDC_DVR_PARTITION:
       content = cbox_static_get_focus(p_ctrl);
       sys_status_set_usb_work_partition(content);
       ui_timeshift_total_size();
       sys_status_save();
      break;

    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  return ret;
}

#endif
static RET_CODE on_dvr_config_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_DVR_CONFIG, 0, 0);
  return SUCCESS;
}
//lint -e438 -e585 -e830
static RET_CODE on_dvr_config_exit(control_t *p_ctrl, u16 msg,       //timeshift
  u32 para1, u32 para2)
{
  u16 timeshift_content = 0,  partion_content = 0;
  control_t * p_timeshift, *p_partion, *p_cont;
  u16 *p_timeshift_name = NULL;
  hfile_t file = NULL;
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;
  u16 len=0;
  vfs_dev_info_t dev_info = {0};
  u16 dst_file[50] = {0};
  u16  tdst_file[50] = {0},letter[3] = {0};
  u8  tmp[5120] = {0};
  int i;

  p_timeshift = ui_comm_root_get_ctrl(ROOT_ID_DVR_CONFIG, IDC_TIMESHIFT);	
  p_partion =  ui_comm_root_get_ctrl(ROOT_ID_DVR_CONFIG, IDC_DVR_PARTITION);	
  p_cont = ctrl_get_parent(p_timeshift);

  partion_content = ui_comm_select_get_focus(p_partion);
  sys_status_set_usb_work_partition((u8)partion_content);

  partition_cnt = vfs_get_partitions(partition_letter, 10);
  
  OS_PRINTF("partition_cnt = %d\n",partition_cnt);
  partion_content = sys_status_get_usb_work_partition();//default letter in dvr configuration menu
  OS_PRINTF("on_dvr_config_exit partion_content = %d\n",partion_content);
  vfs_get_dev_info(partition_letter[partion_content],&dev_info);
  if(dev_info.fs_type == VFS_NTFS && dev_info.total_size > 0 && dev_info.free_size > 10240)
  {
    memset(tdst_file, 0, sizeof(u16) * 50);
    memset(dst_file, 0, sizeof(u16) * 50);
    memset(letter, 0, sizeof(u16) * 3);
    letter[0]= partition_letter[partion_content];
    uni_strcat(tdst_file, letter, 50);
    str_asc2uni((u8 *)":\\desktop.ini", dst_file);
    uni_strcat(tdst_file, dst_file, 50);
    file = vfs_open(tdst_file,  VFS_NEW);
    for(i=0;i<5120;i++)
      tmp[i]='a';
    if(file)
    {
      OS_PRINTF("on_dvr_config_exit write tick = %d\n",mtos_ticks_get());
      len=(u16)vfs_write(tmp,5120,1,file);
      OS_PRINTF("on_dvr_config_exit write len = %d,ticks=%d\n",len,mtos_ticks_get());
      vfs_close(file);
      vfs_del(tdst_file);
    }
  }
  
  timeshift_content = ui_comm_select_get_focus(p_timeshift);
  if(timeshift_content == 0)
  {
    sys_status_set_timeshift_switch(FALSE);
    sys_status_save();
    if(ui_get_usb_status())
    {
      p_timeshift_name = ui_usb_get_timeshift_file_name();
      if(p_timeshift_name)
      {
        file = vfs_open(p_timeshift_name, VFS_READ);
        if(file)
        {
          vfs_close(file);
	   file = NULL;
	   vfs_del(p_timeshift_name);
        }
      }
    }
  }
  else
  {
    if(SUCCESS != ui_timeshift_total_size())
    {
      dvr_config_format(ctrl_get_active_ctrl(p_cont), msg, para1, para2);

      //ui_comm_select_set_focus(p_timeshift, timeshift_content);
      //ctrl_paint_ctrl(p_timeshift, FALSE);

      return  SUCCESS;
    }

    sys_status_set_timeshift_switch(TRUE);
    sys_status_save();
  }

 return ERR_NOFEATURE;
}
//lint +e438 +e585 +e830
BEGIN_MSGPROC(dvr_config_select_proc, cbox_class_proc)
 // ON_COMMAND(MSG_CHANGED, on_select_change_focus)
END_MSGPROC(dvr_config_select_proc, cbox_class_proc)

BEGIN_KEYMAP(dvr_config_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(dvr_config_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(dvr_config_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PLUG_OUT, on_dvr_config_plug_out)
  ON_COMMAND(MSG_EXIT, on_dvr_config_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_dvr_config_exit)
END_MSGPROC(dvr_config_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(dvr_config_format_confirm_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT,MSG_FOCUS_LEFT)
END_KEYMAP(dvr_config_format_confirm_keymap, ui_comm_root_keymap)

BEGIN_KEYMAP(dvr_config_format_fat_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(dvr_config_format_fat_keymap, NULL)

BEGIN_MSGPROC(dvr_config_format_fat_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_dvr_config_format_fat_select)
END_MSGPROC(dvr_config_format_fat_proc, text_class_proc)

BEGIN_KEYMAP(dvr_config_format_cancel_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(dvr_config_format_cancel_keymap, NULL)

BEGIN_MSGPROC(dvr_config_format_cancel_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_dvr_config_format_cancel_select)
END_MSGPROC(dvr_config_format_cancel_proc, text_class_proc)

#endif
