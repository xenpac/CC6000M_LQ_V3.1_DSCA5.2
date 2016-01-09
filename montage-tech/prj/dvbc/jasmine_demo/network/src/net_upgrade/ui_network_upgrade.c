/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_network_upgrade.h"
#include "ap_usb_upgrade.h"
#include "ui_net_upg_api.h"

/************local variable define*****************************/
/*!
  header offset in flash.bin
!*/
#define HEADER_OFFSET 16
/*!
  header size
!*/
#define HEADER_SIZE 12

typedef enum
{
  IDC_INVALID = 0,
  IDC_MODE,
  IDC_FILE,
  IDC_START,
  IDC_PBAR,
  IDC_STS,
  IDC_VERSION,
  IDC_ITEM_MAX
}upgrade_by_net_control_id_t;

static list_xstyle_t version_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_xstyle_t version_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t version_list_field_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};
static list_field_attr_t version_list_attr[VERSION_LIST_FIELD] =
{
  { LISTFIELD_TYPE_STRID| STL_LEFT | STL_VCENTER,
    195, 5, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
    300,200, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    300, 500, 0, &version_list_field_rstyle,  &version_list_field_fstyle},
};
/*
static rect_t g_upgrade_dlg_rc =
{
  UPGRADE_DLG_X,  UPGRADE_DLG_Y,
  UPGRADE_DLG_X + UPGRADE_DLG_W,
  UPGRADE_DLG_Y + UPGRADE_DLG_H,
};*/
//static flist_dir_t flist_dir = NULL;
//static partition_t *p_partition = NULL;
//static u32 g_partition_cnt = 0;
//static file_list_t g_list = {0};
//static u16 g_ffilter_all[32] = {0};
//static usb_upg_start_params_t net_param = {0};

extern u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE];

static net_upg_api_info_t *p_net_upg_info = NULL;

static u16 upgrade_by_net_cont_keymap(u16 key);
static RET_CODE upgrade_by_net_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_start_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_mode_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_file_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE on_upg_by_net_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE on_upg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
//static void _pre_burn(void);

static u32 ui_net_upg_sts = UI_NET_UPG_IDEL;

u16 ui_net_upg_evtmap(u32 event);

u32 ui_net_upgade_sts(void)
{
  return ui_net_upg_sts;
}

void ui_upgrade_by_net_sts_show(control_t *p_cont, u16 *p_str)
{
  text_set_content_by_unistr(p_cont, p_str);
  ctrl_paint_ctrl(p_cont, TRUE);
}

static void ui_refresh_items(control_t *p_cont)
{
  control_t *p_start, *p_pbar, *p_mode, *p_file;

  p_start = ctrl_get_child_by_id(p_cont, IDC_START);
  ui_comm_ctrl_update_attr(p_start, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_start, TRUE);

  p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
  ui_comm_ctrl_update_attr(p_mode, TRUE);
  ctrl_paint_ctrl(p_mode, TRUE);

  p_file = ctrl_get_child_by_id(p_cont, IDC_FILE);
  ui_comm_ctrl_update_attr(p_file, TRUE);
  ctrl_paint_ctrl(p_file, TRUE);

  p_pbar = ctrl_get_child_by_id(p_cont, IDC_PBAR);
  ui_comm_bar_update(p_pbar, 0, TRUE);
  ui_comm_bar_paint(p_pbar, TRUE);
}
#if 0
static void ui_upgrade_giveup(void)
{
  control_t *p_ctrl_sts;
  control_t *p_root;

  ui_net_upg_sts = UI_NET_UPG_IDEL;
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_STS);
  p_root = ctrl_get_parent(p_ctrl_sts);
  ui_refresh_items(p_root);
}

static void ui_upgrade_start_burn(void)
{
  //cmd_t cmd = {0};
  control_t *p_start,*p_bar,*p_sts = NULL;
  u16 uni_str[STS_STR_MAX_LEN] = {0};

  ui_net_upg_sts = UI_NET_UPG_BURNING;
  if(UPG_MAIN_CODE == net_param.mode)
  {
    _pre_burn();
  }
  //cmd.id = NET_UPG_CMD_START_BURN;
  //cmd.data1 = (u32)(&net_param);
  //ap_frm_do_command(APP_NET_UPG, &cmd);


  //burn flash, tbd
  gui_get_string(IDS_BURN_FLASH, uni_str, STS_STR_MAX_LEN);
  p_sts = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_STS);
  p_start = ctrl_get_child_by_id(ctrl_get_parent(p_sts), IDC_START);
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  ui_comm_ctrl_update_attr(p_start,FALSE);
  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  ctrl_paint_ctrl(p_bar, TRUE);

  ui_upgrade_by_net_sts_show(p_sts, uni_str);
}

static void _ui_upgrade_start_download( void)
{
  u32 total_size = 0;
  u32 size = 0;
  u16 percent = 0;
  s64 off_set = 0;
  u8 *p_buf = NULL;
  hfile_t file = NULL;
  u32 downloaded_size = 0;
  control_t *p_pbar = NULL;

  file = vfs_open(net_param.file, VFS_READ);
  if(NULL == file)
  {
    on_upg_fail(NULL, 0, 0, 0);
    return;
  }
  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  total_size = net_param.ext_buf_size;
  off_set = (net_param.mode == UPG_MAIN_CODE) ? net_param.flash_offset_addr : 0;
  vfs_seek(file, off_set, VFS_SEEK_SET);

  while(downloaded_size < total_size)
  {
    p_buf = (u8 *)(net_param.ext_buf_addr + downloaded_size);
    size = ((downloaded_size + USB_UPG_TRANS_PACKET_SIZE) <= total_size)? USB_UPG_TRANS_PACKET_SIZE\
              : (total_size - downloaded_size);
    if(size != vfs_read(p_buf, size, 1, file))
    {
      on_upg_fail(NULL, 0, 0, 0);
      return;
    }
    downloaded_size += size;
    percent = (downloaded_size * 100) / total_size;

    //update ui
    ui_comm_bar_update(p_pbar, percent, TRUE);
    ui_set_front_panel_by_str_with_upg("D", (u8)percent);
    ui_comm_bar_paint(p_pbar, TRUE);
  }
  vfs_close(file);
  ui_comm_ask_for_dodlg_open(&g_upgrade_dlg_rc, IDS_SURE_BURN_FLASH,\
                                                      ui_upgrade_start_burn, ui_upgrade_giveup, 0);
  return ;
}
#endif

static void u_itoa(u32 value, u8 *p_str, u8 radix)
{
#ifndef WIN32
  u32 temp = value;
  u8 loopi = 0;
  u8 radix_str[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  u8 temp_str[32] = {0};

  while (temp)
  {
    sprintf(temp_str + loopi, "%c", radix_str[temp % radix]);

    loopi ++;
    temp /= radix;
  }
  temp = strlen(temp_str);

  for (loopi = 0; loopi < temp; loopi ++)
  {
    p_str[temp - 1 - loopi] = temp_str[loopi];
  }
#else
  itoa(value, p_str, radix);
#endif
}

static RET_CODE version_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u8 local_version[MAX_CHANGESET_LEN] = {0};
  u16 i, focus = 0, str_id[VERSION_LIST_CNT -1] = {IDS_MAINCODE};
  upg_file_ver_t upg_file_version = {{0},{0},{0}};
  control_t *p_ctrl = NULL;

  for(i = 0; i < (VERSION_LIST_CNT -1); i++)
  {
    list_set_field_content_by_strid(p_list, i +1, 0, str_id[i]);
  }

  if(NULL != p_list->p_parent->p_parent)
  {
    p_ctrl = ctrl_get_child_by_id( p_list->p_parent->p_parent, IDC_FILE);
    if(NULL != p_ctrl)
    {
      focus = ui_comm_select_get_focus(p_ctrl);
    }
  }

  if(p_net_upg_info && p_net_upg_info->item_cnt)
  {
    u_itoa(p_net_upg_info->item[focus].sw, upg_file_version.changeset, 10);
  }
  upg_file_version.changeset[19] = '\0';

  dm_read(class_get_handle_by_id(DM_CLASS_ID), SS_DATA_BLOCK_ID, 0, 0, MAX_CHANGESET_LEN, local_version);

  list_set_field_content_by_ascstr(p_list, 0, 1, "Local Version");
  list_set_field_content_by_ascstr(p_list, 0, 2, "Bin Version");

  list_set_field_content_by_ascstr(p_list, 1, 1, local_version);
  list_set_field_content_by_ascstr(p_list, 1, 2, upg_file_version.changeset);

  return SUCCESS;
}

static RET_CODE upgrade_file_update(control_t *p_ctrl, u16 focus,
                                       u16 *p_str, u16 max_length)
{
  u16 uni_str[256] = {0};

  if(p_net_upg_info == NULL || p_net_upg_info->item_cnt == 0)
  {
    str_nasc2uni("No file", uni_str, DB_DVBS_MAX_NAME_LENGTH);
  }
  else
  {
      if(focus >= p_net_upg_info->item_cnt)
      {
          return ERR_FAILURE;
      }
      str_nasc2uni(p_net_upg_info->item[focus].url, uni_str, max_length - 1);
      //uni_strncpy(uni_str, p_net_upg_info->item[focus].url, max_length - 1);
    }

  uni_strncpy(p_str, uni_str, max_length-1);

  return SUCCESS;
}

RET_CODE open_network_upgrade(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl[IDC_ITEM_MAX],*p_version;
  u8 i, j;
  u16 file_conut = 0;
  u16 stxt [IDC_ITEM_MAX] = {0,IDS_UPGRADE_MODE, IDS_UPGRADE_FILE, IDS_START };
  u16 smode[UPGRADE_BY_NET_MODE_CNT] =
    {
      IDS_UPGRADE_ALL_CODE, IDS_MAINCODE
    };
  u16 y;

  if(ui_recorder_isrecording())
  {
    return SUCCESS;
  }

  p_net_upg_info = (net_upg_api_info_t *)para1;

  file_conut = p_net_upg_info->item_cnt;
  p_cont =
    ui_comm_root_create(ROOT_ID_NETWORK_UPGRADE, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, 0, IDS_UPGRADE_BY_NET);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, upgrade_by_net_cont_keymap);
  ctrl_set_proc(p_cont, upgrade_by_net_cont_proc);

  y = UPGRADE_BY_NET_ITEM_Y;
  for (i = 1; i < IDC_ITEM_MAX; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case IDC_MODE://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_NET_ITEM_X, y,
                          UPGRADE_BY_NET_ITEM_LW, UPGRADE_BY_NET_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           UPGRADE_BY_NET_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < UPGRADE_BY_NET_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_mode_proc);
        break;
      case IDC_FILE://file
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_NET_ITEM_X, y,
                      UPGRADE_BY_NET_ITEM_LW, UPGRADE_BY_NET_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,
                    file_conut, CBOX_ITEM_STRTYPE_UNICODE, upgrade_file_update);
        ui_comm_select_right_set_mrect(p_ctrl[i], UPGRADE_BY_NET_SELECT_MIDX, 0, UPGRADE_BY_NET_SELECT_MIDY, COMM_CTRL_H);
        ui_comm_select_set_focus(p_ctrl[i], 0);
        if(0 == file_conut)
        {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_file_proc);
        break;

      case IDC_START://start
        p_ctrl[i] = ui_comm_static_create(p_cont, i, UPGRADE_BY_NET_ITEM_X, y,
                          UPGRADE_BY_NET_ITEM_LW, UPGRADE_BY_NET_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_start_text_proc);
        if(0 == file_conut)
        {
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        y += UPGRADE_BY_NET_ITEM_H + UPGRADE_BY_NET_ITEM_V_GAP;
        break;
      case IDC_PBAR://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, i, UPGRADE_BY_NET_PBAR_X,
                         (y - 15),
                          UPGRADE_BY_NET_PBAR_W, UPGRADE_BY_NET_PBAR_H,
                          UPGRADE_BY_NET_TXT_X, (y - 18), UPGRADE_BY_NET_TXT_W, UPGRADE_BY_NET_TXT_H,
                          UPGRADE_BY_NET_PER_X, (y - 18), UPGRADE_BY_NET_PER_W, UPGRADE_BY_NET_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_UPGRADE_BY_NET_PBAR_BG, RSI_UPGRADE_BY_NET_PBAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case IDC_STS://upgrade_by_net status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT,IDC_STS, UPGRADE_BY_NET_ITEM_X, (y - 20),
                          (UPGRADE_BY_NET_ITEM_LW + UPGRADE_BY_NET_ITEM_RW),
                          UPGRADE_BY_NET_ITEM_H,
                          p_cont, 0);

        ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
        text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
        text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], IDS_DOWNLOAD);
        break;
    case IDC_VERSION :
      p_ctrl[i] =  ctrl_create_ctrl(CTRL_CONT,IDC_VERSION,
                              COMM_ITEM_OX_IN_ROOT,y,
                                COMM_ITEM_MAX_WIDTH,80,
                                p_cont, 0);
      ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);

      p_version = ctrl_create_ctrl(CTRL_LIST, 1,0, 0,
                                                      COMM_ITEM_MAX_WIDTH, 80,
                                                      p_ctrl[i], 0);
      ctrl_set_rstyle(p_version, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      ctrl_set_mrect(p_version, 0, 0, 800, 80);
      list_set_item_interval(p_version, 0);
      list_set_item_rstyle(p_version, &version_list_item_rstyle);
      list_enable_select_mode(p_version, FALSE);
      list_set_focus_pos(p_version, 0);
      list_set_count(p_version, VERSION_LIST_CNT, VERSION_LIST_CNT);
      list_set_field_count(p_version, VERSION_LIST_FIELD, VERSION_LIST_CNT);
      list_set_update(p_version, version_list_update, 0);

    for (j = 0; j < VERSION_LIST_FIELD; j++)
      {
        list_set_field_attr(p_version, (u8)j, (u32)(version_list_attr[j].attr), (u16)(version_list_attr[j].width),
                            (u16)(version_list_attr[j].left), (u8)(version_list_attr[j].top));
        list_set_field_rect_style(p_version, (u8)j, version_list_attr[j].rstyle);
        list_set_field_font_style(p_version, (u8)j, version_list_attr[j].fstyle);
      }
      version_list_update(p_version, list_get_valid_pos(p_version), VERSION_LIST_CNT, 0);
        break;
      default:
        break;
    }

    y += UPGRADE_BY_NET_ITEM_H + UPGRADE_BY_NET_ITEM_V_GAP;
  }

  ctrl_set_related_id(p_ctrl[IDC_MODE], 0, IDC_START, 0, IDC_FILE);
  ctrl_set_related_id(p_ctrl[IDC_FILE], 0, IDC_MODE, 0, IDC_START);
  ctrl_set_related_id(p_ctrl[IDC_START], 0, IDC_FILE, 0, IDC_MODE);

  ctrl_default_proc(p_ctrl[IDC_MODE], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  //ui_init_upg(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE, ui_net_upg_evtmap);
  return SUCCESS;
}

static RET_CODE on_upg_by_net_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  /*if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }*/
  ui_net_upg_stop();
  if(UI_NET_UPG_IDEL != ui_net_upg_sts)
  {
    return SUCCESS;
  }

 // ui_release_upg(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);
  return ERR_NOFEATURE;

}
static RET_CODE on_upg_by_net_power_off(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  /*if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }*/
  ui_net_upg_stop();
  if(UI_NET_UPG_IDEL != ui_net_upg_sts)
  {
    return SUCCESS;
  }

  //ui_release_upg(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);

  return ERR_NOFEATURE;
}
static RET_CODE on_upg_by_net_quit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  ui_net_upg_sts = UI_NET_UPG_IDEL;
  //ui_release_upg(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);

  /*if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }*/
  ui_net_upg_stop();
  if (para1 == TRUE)
  {
    ui_close_all_mennus();
  }
  else
  {
    manage_close_menu(ROOT_ID_NETWORK_UPGRADE, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE on_upg_by_net_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_sts;
  u16 uni_str[STS_STR_MAX_LEN] = {0};

  ui_net_upg_sts = UI_NET_UPG_IDEL;

  p_sts = ctrl_get_child_by_id(p_ctrl, IDC_STS);
  str_nasc2uni("upgrade ok!", uni_str, STS_STR_MAX_LEN);
  ui_upgrade_by_net_sts_show(p_sts, uni_str);

  ui_refresh_items(p_ctrl);
  return SUCCESS;
}
static RET_CODE on_upg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t p_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W + 40, COMM_DLG_H,
    IDS_UPG_FAIL,
    0,
  };
  control_t *p_bar, *p_start;

  ui_net_upg_sts = UI_NET_UPG_IDEL;
  ui_set_front_panel_by_str_with_upg("ERR", 0);
  ui_comm_dlg_open(&p_data);
  mtos_task_delay_ms(1000);
  ui_comm_dlg_close();

  if(!ui_get_usb_status())
  {
    ui_release_upg(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);
    manage_close_menu(ROOT_ID_NETWORK_UPGRADE, 0, 0);
    ui_set_front_panel_by_str("----");
    return SUCCESS;
  }
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  p_start = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_START);

  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_bar, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  return SUCCESS;
}
static RET_CODE on_upg_by_net_update_sts(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 process = para2;
  control_t *p_pbar, *p_ctrl_sts;
  comm_dlg_data_t p_data =
  {
      ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W+ 40,COMM_DLG_H,
    IDS_USB_UPG_RESTART,
    0 ,
  };
OS_PRINTF("\n##on_upg_by_net_update_sts [%lu]\n", process);
  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_STS);
  //burn process
  if (msg == MSG_NET_UPG_EVT_BURN_PROGRESS)
  {
    ui_comm_bar_update(p_pbar, (u16)process, TRUE);
    ui_set_front_panel_by_str_with_upg("B",(u8)process);
    ui_comm_bar_paint(p_pbar, TRUE);
    if(100 == process)
    {
        ui_comm_dlg_open(&p_data);
        mtos_task_delay_ms(2000);
    #ifndef WIN32
        Api_Sys_Reset();
#endif
    }
  }
  else//download process
  {
    ui_comm_bar_update(p_pbar, (u16)process, TRUE);
    ui_set_front_panel_by_str_with_upg("D",(u8)process);
    ui_comm_bar_paint(p_pbar, TRUE);
    if(100 == process)
    {
      //fix here
      p_data.style   = DLG_FOR_ASK | DLG_STR_MODE_STATIC,
      p_data.content = IDS_SURE_BURN_FLASH;
      if(DLG_RET_YES == ui_comm_dlg_open(&p_data))
      {
        text_set_content_by_strid(p_ctrl_sts, IDS_BURN_FLASH);
        ctrl_paint_ctrl(p_ctrl_sts, TRUE);
        ui_net_upg_burn();
      }
      else
      {
        ui_net_upg_sts = UI_NET_UPG_IDEL;
        ui_refresh_items(p_ctrl);
      }
    }
  }

  return SUCCESS;
}

static RET_CODE on_upg_by_net_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mode, *p_cont, *p_start, *p_file;
  u16 mode_focus;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  dmh_block_info_t dm_head;
  net_upg_api_up_cfg_t cfg = {0};

  if(UI_NET_UPG_IDEL == ui_net_upg_sts)
  {
    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
    p_file = ctrl_get_child_by_id(p_cont, IDC_FILE);

    mode_focus = ui_comm_select_get_focus(p_mode);

    switch(mode_focus)
    {
    case 0://all code
      cfg.flash_addr_offset = 0;
      cfg.flash_burn_size = 0;
      break;

    case 1://main code
      dm_get_block_header(dm_handle, IW_TABLE_BLOCK_ID, &dm_head);
      cfg.flash_addr_offset = get_maincode_off_addr();
      cfg.flash_burn_size = dm_head.base_addr;
      cfg.main_code_offset_addr = get_maincode_off_addr();
      OS_PRINTF("\n\n## base_addr and size [0x%x, 0x%x]\n", dm_head.base_addr, cfg.flash_burn_size);
      break;

    default:
      MT_ASSERT(0);
      break;
    }

    ui_comm_ctrl_update_attr(p_mode, FALSE);

    ui_comm_ctrl_update_attr(p_file, FALSE);

    p_start = ctrl_get_child_by_id(p_cont, IDC_START);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ctrl_paint_ctrl(p_cont, TRUE);
    cfg.sw = p_net_upg_info->item[ui_comm_select_get_focus(p_file)].sw;
    cfg.p_flash_buf = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
    cfg.flash_buf_size = p_net_upg_info->item[ui_comm_select_get_focus(p_file)].size * KBYTES;
    mem_mgr_release_block(BLOCK_AV_BUFFER);

    ui_net_upg_upgrade(&cfg);

  }
  return SUCCESS;
}

static RET_CODE on_upg_by_net_file_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_version;

  cbox_class_proc(p_ctrl, msg, 0, 0);
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
  version_list_update(ctrl_get_child_by_id(p_version, 1), 1, VERSION_LIST_CNT, 0);
  ctrl_paint_ctrl(p_version, TRUE);
  return SUCCESS;
}

static RET_CODE on_upg_by_net_mode_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_version = NULL;
//  control_t *p_file = NULL;
//  control_t *p_start = NULL;

  u16 focus;

  cbox_class_proc(p_ctrl, msg, 0, 0);

  focus = cbox_static_get_focus(p_ctrl);
/*
  if(g_partition_cnt > 0)
  {
    //update file list
    ui_upg_file_list_get(flist_dir, FLIST_UNIT_FIRST, &g_list, (u8)focus);
    p_file = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_FILE);
    ui_comm_select_set_count(p_file, CBOX_WORKMODE_DYNAMIC, (u16)g_list.file_count);
    ui_comm_select_set_focus(p_file, 0);

    //update start icon
    p_start = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_START);
    if(0 == g_list.file_count)
    {
        ui_comm_ctrl_update_attr(p_file, FALSE);
        ui_comm_ctrl_update_attr(p_start, FALSE);
    }
    else
    {
        ui_comm_ctrl_update_attr(p_file, TRUE);
        ui_comm_ctrl_update_attr(p_start, TRUE);
    }
    ctrl_paint_ctrl(p_file, TRUE);
    ctrl_paint_ctrl(p_start, TRUE);
  }
*/
  //update version
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
  ctrl_set_sts(ctrl_get_child_by_id(p_version,1), focus? OBJ_STS_HIDE : OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_version, TRUE);

  return SUCCESS;
}

static RET_CODE on_upg_by_net_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  /*if(flist_dir != NULL)
   {
      file_list_leave_dir(flist_dir);
      flist_dir = NULL;
   }*/
   ui_net_upg_stop();
  return ERR_NOFEATURE;
}
BEGIN_KEYMAP(upgrade_by_net_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(upgrade_by_net_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(upgrade_by_net_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_upg_by_net_exit)
  ON_COMMAND(MSG_EXIT, on_upg_by_net_exit)
  ON_COMMAND(MSG_POWER_OFF, on_upg_by_net_power_off)
  ON_COMMAND(MSG_UPG_QUIT, on_upg_by_net_quit)
  ON_COMMAND(MSG_UPG_OK, on_upg_by_net_ok)
  //ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_upg_by_net_update_sts)
  ON_COMMAND(MSG_NET_UPG_EVT_DOWN_PROGRESS, on_upg_by_net_update_sts)
  ON_COMMAND(MSG_NET_UPG_EVT_BURN_PROGRESS, on_upg_by_net_update_sts)
  ON_COMMAND(MSG_NET_UPG_EVT_FAIL, on_upg_fail)
  ON_COMMAND(MSG_DESTROY, on_upg_by_net_destory)
END_MSGPROC(upgrade_by_net_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(upgrade_by_net_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_upg_by_net_mode_change_focus)
  ON_COMMAND(MSG_DECREASE, on_upg_by_net_mode_change_focus)
END_MSGPROC(upgrade_by_net_mode_proc, cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_net_file_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_upg_by_net_file_change_focus)
  ON_COMMAND(MSG_DECREASE, on_upg_by_net_file_change_focus)
END_MSGPROC(upgrade_by_net_file_proc,cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_net_start_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_upg_by_net_start)
END_MSGPROC(upgrade_by_net_start_text_proc, text_class_proc)

