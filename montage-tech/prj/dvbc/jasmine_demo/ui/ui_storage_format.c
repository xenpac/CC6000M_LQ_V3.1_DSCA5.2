/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_storage_format.h"
#include "file_list.h"
#include "vfs.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_STORAGE_FORMAT_VOLUME, 
  IDC_STORAGE_FORMAT_TOTAL,
  IDC_STORAGE_FORMAT_FREE,
  IDC_STORAGE_FORMAT_USED,
  IDC_STORAGE_FORMAT_FILESYS,
  IDC_STORAGE_FORMAT_STORAGE_PATITION,
  IDC_STORAGE_FORMAT_HDD_FORMAT,
  IDC_STORAGE_FORMAT_CONFIRM,
};

enum storage_format_confirm_cont_id
{
  IDC_STORAGE_FORMAT_CONFIRM_CONTENT = 1,
  IDC_STORAGE_FORMAT_CONFIRM_FAT,
  IDC_STORAGE_FORMAT_CONFIRM_NTFS,
  IDC_STORAGE_FORMAT_CONFIRM_CANCEL,
};
static partition_t *p_partition = NULL;
static u32 g_partition_cnt = 0;

static u16 storage_format_cont_keymap(u16 key);
RET_CODE storage_format_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

static u16 storage_format_hdd_keymap(u16 key);
RET_CODE storage_format_hdd_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

static u16 storage_format_confirm_keymap(u16 key);
RET_CODE storage_format_confirm_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

static u16 storage_format_fat_keymap(u16 key);
RET_CODE storage_format_fat_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

//static u16 storage_format_ntfs_keymap(u16 key);

static u16 storage_format_cancel_keymap(u16 key);
RET_CODE storage_format_cancel_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

RET_CODE storage_format_partition_select_changed(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);
static RET_CODE on_storage_format_cancel_select(control_t * p_ctrl, 
  u16 msg, u32 para1, u32 para2);


static void storage_format_set_detail(control_t * p_cont)
{
  control_t *p_ctrl[STORAGE_FORMAT_ITEM_CNT], *p_storage_partition;
  int i = 0;
  u16 focus = 0;
  u8 asc_str[32];
  u16 partition_letter[10] = {0};
  vfs_dev_info_t p_dev_info;

  vfs_get_partitions(partition_letter, 10);
  p_storage_partition = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_STORAGE_PATITION);
  focus = ui_comm_select_get_focus(p_storage_partition);
  
  vfs_get_dev_info(partition_letter[focus],&p_dev_info);
  for(i=0; i<STORAGE_FORMAT_ITEM_CNT; i++)
  {
    p_ctrl[i] = ctrl_get_child_by_id(p_cont, (u16)(IDC_STORAGE_FORMAT_VOLUME+i));
  }

  ////OS_PRINTF("p_dev_info.free_size = %d\n",p_dev_info.free_size);
  ////OS_PRINTF("p_dev_info.fs_type = %d\n",p_dev_info.fs_type);
  ////OS_PRINTF("p_dev_info.total_size = %d\n",p_dev_info.total_size);
  ////OS_PRINTF("p_dev_info.used_size = %d\n",p_dev_info.used_size);

  memset(asc_str, 0, sizeof(asc_str));
  //sprintf(asc_str,"%f MBytes", (int)p_dev_info.used_size/1.0/1024/1024);
 // ui_comm_static_set_content_by_ascstr(p_ctrl[0], "xxxx");
 ui_comm_static_set_content_by_unistr(p_ctrl[0],p_partition[focus].name); 
  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(p_dev_info.total_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[1], asc_str);

  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(p_dev_info.free_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[2], asc_str);

  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(p_dev_info.used_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[3], asc_str);
  if(p_dev_info.fs_type == VFS_FAT16)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[4], (u8 *)"FAT16");
  }
  else if(p_dev_info.fs_type == VFS_FAT32)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[4], (u8 *)"FAT32");
  }
  else if(p_dev_info.fs_type == VFS_NTFS)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[4], (u8 *)"NTFS");
  }
  else
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[4], (u8 *)"UNKNOW");
  }
  //ui_comm_static_set_content_by_ascstr(p_ctrl[5], "USB Partition1");
  ctrl_paint_ctrl(p_cont,TRUE);
}

RET_CODE open_storage_format(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[STORAGE_FORMAT_ITEM_CNT];
  control_t *p_hdd_btn, *p_dlg_cont;
  control_t *p_content, *p_fat, /**p_ntfs,*/ *p_cancel;
  u8 i = 0, j = 0;
  u16 y = 0;
  u16 stxt[STORAGE_FORMAT_ITEM_CNT] =
  { IDS_STORAGE_PARTITION, IDS_TOTAL, IDS_REST_SPACE, IDS_USED,
    IDS_FILE_SYSTEM, IDS_STORAGE_PARTITION,
  };

  if(!(ui_get_usb_status()))
  {
    return SUCCESS;
  }

  g_partition_cnt = file_list_get_partition(&p_partition);

  p_cont =
    ui_comm_root_create(ROOT_ID_STORAGE_FORMAT, 0, COMM_BG_X, COMM_BG_Y,
    COMM_BG_W, COMM_BG_H, RSC_INVALID_ID, IDS_STORAGE_FORMAT);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, storage_format_cont_keymap);
  ctrl_set_proc(p_cont, storage_format_cont_proc);
  
  y = STORAGE_FORMAT_ITEM_Y;
  for (i = 0; i < STORAGE_FORMAT_ITEM_CNT; i++)
  {
    if(i == STORAGE_FORMAT_ITEM_CNT -1)
    {
      p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_STORAGE_FORMAT_VOLUME + i),
      STORAGE_FORMAT_ITEM_X, y, STORAGE_FORMAT_ITEM_LW, STORAGE_FORMAT_ITEM_RW);
      ui_comm_ctrl_set_proc(p_ctrl[i], storage_format_partition_select_changed);
      ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
      ui_comm_select_set_param(p_ctrl[i], TRUE,
                                 CBOX_WORKMODE_STATIC, (u16)g_partition_cnt,
                                 CBOX_ITEM_STRTYPE_UNICODE, NULL);
      for(j = 0; j < g_partition_cnt; j++)
      {
        ui_comm_select_set_content_by_unistr(p_ctrl[i], j, p_partition[j].name);
      }
    }
    else
    {
      p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_STORAGE_FORMAT_VOLUME + i),
      STORAGE_FORMAT_ITEM_X, y, STORAGE_FORMAT_ITEM_LW, STORAGE_FORMAT_ITEM_RW);
      ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
      ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
    }
    
    y += STORAGE_FORMAT_ITEM_H + STORAGE_FORMAT_ITEM_V_GAP;
  }

  p_hdd_btn =
    ctrl_create_ctrl(CTRL_TEXT, IDC_STORAGE_FORMAT_HDD_FORMAT, STORAGE_FORMAT_TEXT_X, STORAGE_FORMAT_TEXT_Y,
                     STORAGE_FORMAT_TEXT_W, STORAGE_FORMAT_TEXT_H, p_cont, 0);
  ctrl_set_rstyle(p_hdd_btn, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_hdd_btn, storage_format_hdd_keymap);
  ctrl_set_proc(p_hdd_btn, storage_format_hdd_proc);
  text_set_align_type(p_hdd_btn, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_hdd_btn, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_hdd_btn, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_hdd_btn, IDS_HDD_FORMAT);

  ctrl_set_related_id(p_ctrl[5], 0, IDC_STORAGE_FORMAT_HDD_FORMAT, 0, IDC_STORAGE_FORMAT_HDD_FORMAT);
  ctrl_set_related_id(p_hdd_btn, 0, IDC_STORAGE_FORMAT_STORAGE_PATITION, 0, IDC_STORAGE_FORMAT_STORAGE_PATITION);

  //hdd format confirm container
  p_dlg_cont = ctrl_create_ctrl(CTRL_CONT, IDC_STORAGE_FORMAT_CONFIRM, STORAGE_FORMAT_CONFIRM_CONT_X,
                                 STORAGE_FORMAT_CONFIRM_CONT_Y, STORAGE_FORMAT_CONFIRM_CONT_W,
                                 STORAGE_FORMAT_CONFIRM_CONT_H, p_cont, 0);
  ctrl_set_rstyle(p_dlg_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_dlg_cont, storage_format_confirm_keymap);
  ctrl_set_proc(p_dlg_cont, storage_format_confirm_proc);

  ctrl_set_sts(p_dlg_cont, OBJ_STS_HIDE);
  //content
  p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_STORAGE_FORMAT_CONFIRM_CONTENT,
                           STORAGE_FORMAT_CONFIRM_CONTENT_X, STORAGE_FORMAT_CONFIRM_CONTENT_Y,
                           STORAGE_FORMAT_CONFIRM_CONTENT_W, STORAGE_FORMAT_CONFIRM_CONTENT_H,
                           p_dlg_cont, 0);
  text_set_font_style(p_content, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_content, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_content, IDS_SURE_FORMAT_USB);
  //fat btn
  p_fat = ctrl_create_ctrl(CTRL_TEXT, IDC_STORAGE_FORMAT_CONFIRM_FAT,
                           STORAGE_FORMAT_CONFIRM_FAT_X,STORAGE_FORMAT_CONFIRM_FAT_Y,
                           STORAGE_FORMAT_CONFIRM_FAT_W, STORAGE_FORMAT_CONFIRM_FAT_H,
                           p_dlg_cont, 0);
  ctrl_set_rstyle(p_fat, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_fat, storage_format_fat_keymap);
  ctrl_set_proc(p_fat, storage_format_fat_proc);
  text_set_font_style(p_fat, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_content_type(p_fat, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_fat, IDS_FAT);
 /* //ntfs btn //cancel ntfs
  p_ntfs = ctrl_create_ctrl(CTRL_TEXT, IDC_STORAGE_FORMAT_CONFIRM_NTFS,
                             STORAGE_FORMAT_CONFIRM_NTFS_X,STORAGE_FORMAT_CONFIRM_NTFS_Y,
                             STORAGE_FORMAT_CONFIRM_NTFS_W, STORAGE_FORMAT_CONFIRM_NTFS_H,
                             p_dlg_cont, 0);
  ctrl_set_rstyle(p_ntfs, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_ntfs, storage_format_ntfs_keymap);
  ctrl_set_proc(p_ntfs, storage_format_ntfs_proc);
  text_set_font_style(p_ntfs, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_content_type(p_ntfs, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ntfs, IDS_NTFS);*/
  //cancel btn
  p_cancel= ctrl_create_ctrl(CTRL_TEXT, IDC_STORAGE_FORMAT_CONFIRM_CANCEL,
                             STORAGE_FORMAT_CONFIRM_CANCEL_X, STORAGE_FORMAT_CONFIRM_CANCEL_Y,
                             STORAGE_FORMAT_CONFIRM_CANCEL_W + 10, STORAGE_FORMAT_CONFIRM_CANCEL_H,
                             p_dlg_cont, 0);
  ctrl_set_rstyle(p_cancel, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_cancel, storage_format_cancel_keymap);
  ctrl_set_proc(p_cancel, storage_format_cancel_proc);
  text_set_font_style(p_cancel, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_content_type(p_cancel, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_cancel, IDS_CANCEL);

 // ctrl_set_related_id(p_fat, IDC_STORAGE_FORMAT_CONFIRM_CANCEL, 0, IDC_STORAGE_FORMAT_CONFIRM_NTFS, 0);
//  ctrl_set_related_id(p_ntfs, IDC_STORAGE_FORMAT_CONFIRM_FAT, 0, IDC_STORAGE_FORMAT_CONFIRM_CANCEL, 0);
 // ctrl_set_related_id(p_cancel, IDC_STORAGE_FORMAT_CONFIRM_NTFS, 0, IDC_STORAGE_FORMAT_CONFIRM_FAT, 0);
  ctrl_set_related_id(p_fat, IDC_STORAGE_FORMAT_CONFIRM_CANCEL, 0, IDC_STORAGE_FORMAT_CONFIRM_FAT, 0);
  ctrl_set_related_id(p_cancel, IDC_STORAGE_FORMAT_CONFIRM_FAT, 0, IDC_STORAGE_FORMAT_CONFIRM_CANCEL, 0);

  storage_format_set_detail(p_cont);
  ctrl_default_proc(p_ctrl[5], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

static RET_CODE on_storage_format_select(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_storage_format_init_ok(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
#if 0
  u8 buf[MAX_OSD_SMARTCARD_INFO_BUF_LEN] = {0};
  u16 length = MAX_OSD_SMARTCARD_INFO_BUF_LEN;  
  p_name = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_SMC);
  p_id = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_CNM);

  mcard_get_smartcard_name(buf, &length);
  ui_comm_static_set_content_by_ascstr(p_name, buf);
  mcard_get_smartcard_ID(buf, &length);
  ui_comm_static_set_content_by_ascstr(p_id, buf);

  ctrl_paint_ctrl(p_name, TRUE);
  ctrl_paint_ctrl(p_id, TRUE);
#endif  
  return SUCCESS;
}

static RET_CODE on_storage_format_init_failed(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  //control_t * p_name, *p_id;

  //p_name = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_SMC);
  //p_id = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_CNM);

  //ui_comm_static_set_content_by_ascstr(p_name, " ");
  //ui_comm_static_set_content_by_ascstr(p_id, " ");
  //
  //ctrl_paint_ctrl(p_name, TRUE);
  //ctrl_paint_ctrl(p_id, TRUE);
  return SUCCESS;
}

static RET_CODE on_storage_format_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_STORAGE_FORMAT, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_storage_format_hdd_select(control_t * p_ctrl, 
  u16 msg, u32 para1, u32 para2)
{
  control_t * p_cont, *p_confirm_cont, *p_cancel;
  p_cont = ctrl_get_parent(p_ctrl);
  p_confirm_cont = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_CONFIRM);
  p_cancel = ctrl_get_child_by_id(p_confirm_cont, IDC_STORAGE_FORMAT_CONFIRM_CANCEL);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  
  ctrl_set_attr(p_confirm_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_confirm_cont, OBJ_STS_SHOW);

  ctrl_set_active_ctrl(p_confirm_cont, p_cancel);
  ctrl_process_msg(p_cancel, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_confirm_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_storage_format_fat_select(control_t * p_ctrl, 
  u16 msg, u32 para1, u32 para2)
{
  control_t * p_cont, *p_storage_partition;
  u16 focus;

  p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
  p_storage_partition = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_STORAGE_PATITION);
  focus = ui_comm_select_get_focus(p_storage_partition);
  //mtos_task_lock();
  vfs_format(p_partition[focus].letter[0], VFS_FAT32);
  //mtos_task_unlock();
  on_storage_format_cancel_select(p_ctrl, msg, para1, para2);

  storage_format_set_detail(p_cont);

  ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}
/*
static RET_CODE on_storage_format_ntfs_select(control_t * p_ctrl, 
  u16 msg, u32 para1, u32 para2)
{
  control_t * p_cont, *p_storage_partition;
  u16 focus;

  p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
  p_storage_partition = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_STORAGE_PATITION);
  focus = ui_comm_select_get_focus(p_storage_partition);
  
  vfs_format(p_partition[focus].letter[0], VFS_NTFS);

  on_storage_format_cancel_select(p_ctrl, msg, para1, para2);
  return SUCCESS;
}
*/
static RET_CODE on_storage_format_cancel_select(control_t * p_ctrl, 
  u16 msg, u32 para1, u32 para2)
{
  control_t * p_cont, *p_confirm_cont, *p_hdd;

  p_confirm_cont = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_confirm_cont);
  p_hdd = ctrl_get_child_by_id(p_cont, IDC_STORAGE_FORMAT_HDD_FORMAT);

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, para1, para2);
  ctrl_set_sts(p_confirm_cont, OBJ_STS_HIDE);

  ctrl_process_msg(p_hdd, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_storage_format_select_change_focus(control_t *ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
  //u16 focus;
  RET_CODE ret;
  
  ret = cbox_class_proc(ctrl, msg, para1, para2);

  /* after switch */
  switch (ctrl_get_ctrl_id(ctrl_get_parent(ctrl)))
  {
    case IDC_STORAGE_FORMAT_STORAGE_PATITION:
      //focus = cbox_static_get_focus(ctrl);
      storage_format_set_detail(ctrl_get_parent(ctrl_get_parent(ctrl)));
      
      break;
      
    default:
      break;
  }

  return ret;
}


BEGIN_KEYMAP(storage_format_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(storage_format_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(storage_format_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_storage_format_select)
  ON_COMMAND(MSG_CA_INIT_OK, on_storage_format_init_ok)
  ON_COMMAND(MSG_CA_INIT_FAILED, on_storage_format_init_failed)
  ON_COMMAND(MSG_CA_CARD_OUT, on_storage_format_init_failed)  
  ON_COMMAND(MSG_PLUG_OUT, on_storage_format_plug_out)  
END_MSGPROC(storage_format_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(storage_format_partition_select_changed, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_storage_format_select_change_focus)
END_MSGPROC(storage_format_partition_select_changed, cbox_class_proc)


BEGIN_KEYMAP(storage_format_hdd_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(storage_format_hdd_keymap, NULL)

BEGIN_MSGPROC(storage_format_hdd_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_storage_format_hdd_select)
END_MSGPROC(storage_format_hdd_proc, text_class_proc)

BEGIN_KEYMAP(storage_format_confirm_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT,MSG_FOCUS_LEFT)
END_KEYMAP(storage_format_confirm_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(storage_format_confirm_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PLUG_OUT, on_storage_format_plug_out)  
END_MSGPROC(storage_format_confirm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(storage_format_fat_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(storage_format_fat_keymap, NULL)

BEGIN_MSGPROC(storage_format_fat_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_storage_format_fat_select)
END_MSGPROC(storage_format_fat_proc, text_class_proc)

/*BEGIN_KEYMAP(storage_format_ntfs_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(storage_format_ntfs_keymap, NULL)

BEGIN_MSGPROC(storage_format_ntfs_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_storage_format_ntfs_select)
END_MSGPROC(storage_format_ntfs_proc, text_class_proc)
*/
BEGIN_KEYMAP(storage_format_cancel_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(storage_format_cancel_keymap, NULL)

BEGIN_MSGPROC(storage_format_cancel_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_storage_format_cancel_select)
END_MSGPROC(storage_format_cancel_proc, text_class_proc)





