/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "unzip.h"
#include "LzmaIf.h"
#include "ui_upgrade_by_usb.h"
#include "ap_usb_upgrade.h"
#include "data_manager.h"
#include "fcrc.h"

/************local variable define*****************************/
/*!
  header offset in flash.bin
!*/
#define HEADER_OFFSET 16
/*!
  header size
!*/
#define HEADER_SIZE 12
/*!
  main code block id, not only the main app, maybe include preset ssdata etc.
!*/
#define MAIN_CODE_BLOCK_ID 0x86
/*!
  ca data 1 block id
!*/
#define CADATA1_BLOCK_ID 0xAC

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
}upgrade_by_usb_control_id_t;

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
    165, 5, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
    250,170, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    250, 420, 0, &version_list_field_rstyle,  &version_list_field_fstyle},
};
/*
static comm_help_data_t usb_upgrade_help_data =
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
static rect_t g_upgrade_dlg_rc =
{
  UPGRADE_DLG_X,  UPGRADE_DLG_Y,
  UPGRADE_DLG_X + UPGRADE_DLG_W,
  UPGRADE_DLG_Y + UPGRADE_DLG_H,
};
static flist_dir_t flist_dir = NULL;
static partition_t *p_partition = NULL;
static u32 g_partition_cnt = 0;
static file_list_t g_list = {0};
static u16 g_ffilter_all[32] = {0};
static usb_upg_start_params_t usb_param = {0};
static ui_usb_upg_groups_t usb_grps = {0};

extern u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE];

static u16 upgrade_by_usb_cont_keymap(u16 key);
static RET_CODE upgrade_by_usb_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_usb_start_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_usb_mode_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_usb_file_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE on_upg_by_usb_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE on_upg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static void _pre_burn(u8 mode);
static u8 _get_selected_upg_mode(void);

static u32 ui_usb_upg_sts = UI_USB_UPG_IDEL;

BOOL is_finished_DS = TRUE;

u16 ui_usb_upg_evtmap(u32 event);

u32 ui_usb_upgade_sts(void)
{
  return ui_usb_upg_sts;
}
BOOL usb_upg_is_finish(void)
{
  return is_finished_DS;
}
void ui_upgrade_by_usb_sts_show(control_t *p_cont, u16 *p_str)
{
  text_set_content_by_unistr(p_cont, p_str);
  ctrl_paint_ctrl(p_cont, TRUE);
}

void ui_refresh_items(control_t *p_cont)
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

static void ui_upgrade_giveup(void)
{
  control_t *p_ctrl_sts;
  control_t *p_root;

  ui_usb_upg_sts = UI_USB_UPG_IDEL;
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_STS);
  if (p_ctrl_sts != NULL)
  {
    p_root = ctrl_get_parent(p_ctrl_sts);
    ui_refresh_items(p_root);
  }
}

static void ui_upgrade_start_burn_curn_grp(void)
{
  cmd_t cmd = {0};
  static usb_upg_start_params_t start_param;
  
  //impossible
  if (usb_grps.curn_burn_grp >= usb_grps.grp_cnt)
    return;

  memcpy(&start_param, &usb_param, sizeof(start_param));

  start_param.ext_buf_addr      = usb_grps.grps[usb_grps.curn_burn_grp].grp_addr;
  start_param.ext_buf_size      = usb_grps.grps[usb_grps.curn_burn_grp].grp_size;
  start_param.flash_offset_addr = usb_param.flash_offset_addr + 
                                  usb_grps.grps[usb_grps.curn_burn_grp].grp_addr -
                                  usb_param.ext_buf_addr;
  start_param.burn_flash_size   = start_param.ext_buf_size;

  if (usb_grps.curn_burn_grp != 0)
  {
    //the remain groups needn't to check head.
    start_param.head_size = 0;
    start_param.mode = USB_UPGRD_CONSTANT_CW; //just let it go
  }

  cmd.id = USB_UPG_CMD_START_BURN;
  cmd.data1 = (u32)(&start_param);
  ap_frm_do_command(APP_USB_UPG, &cmd);
}

#ifdef RESOURCE_ON_FLASH
#if 0
BOOL dump_rsc_to_usb(u8* buffer, u32 size)
{
  u16 file_name[50] = {0};
  u32 ret = 0,partition_cnt = 0;
  hfile_t file = NULL;
  partition_t *p_partition = NULL;

  partition_cnt = file_list_get_partition(&p_partition);
	if(partition_cnt > 0)
	{
    str_asc2uni("C:\\rsc_dump.bin",(u16 *)file_name);
    file = vfs_open(file_name,  VFS_NEW);
		if(file == NULL)
		{
		   return FALSE;
		}
		vfs_seek(file, 0, VFS_SEEK_SET);
		ret = vfs_write(buffer, size, 1, file);
		if(ret == 0)
		{
		   return FALSE;
		}
		vfs_close(file);
    hal_dcache_flush_all();
		return TRUE;
	}
  return FALSE;
}
#endif
//lint -e438 -e550 -e830
extern handle_t rsc_handle;
static void reconfigrsc()
{
  if(rsc_handle != NULL)
  {
    vf_ft_detach(rsc_handle); 
    gui_paint_release();
    rsc_release(rsc_handle);
    rsc_handle = NULL;
  }

  {
    u32 p_addr;
    u32 addr = 0;
    u32 av_addr = 0;
    u32 av_size = 0;
    u32 file_size = 0;
    u32 out_size = 0;
    u8 ret = 0;
    u8 *p_zip_sys = NULL;
    u8 *p_data_buffer = NULL;
    u32 data_buffer_size = 0;
    u32 rsc_buf_size = 0;
    sys_status_t *p_systatus = NULL;
    osd_set_t osd_set = {0};
    handle_t h_pango = NULL;
    paint_param_t paint_param = {0};
    vf_ft_cfg_t vf_ft_cfg = {0};
    
    addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
     RS_BLOCK_ID);
    file_size = get_dm_block_real_file_size(RS_BLOCK_ID);

    av_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
    MT_ASSERT(av_addr != 0);
    av_size = mem_mgr_get_block_size(BLOCK_AV_BUFFER);
    mem_mgr_release_block(BLOCK_AV_BUFFER);

    data_buffer_size = (file_size + 64)/64*64;
    p_data_buffer = (u8*)(av_addr + av_size - data_buffer_size);
    MT_ASSERT(p_data_buffer != 0);
    memset(p_data_buffer, 0, data_buffer_size);

    p_zip_sys = p_data_buffer - 320 * KBYTES;
    MT_ASSERT(p_zip_sys != NULL);
    memset(p_zip_sys, 0, 320 * KBYTES);

    rsc_buf_size = 3072*KBYTES;
    p_addr = (u32)(p_zip_sys - rsc_buf_size);
    MT_ASSERT(p_addr != 0);
    MT_ASSERT((p_addr % 8) == 0);
    
    dm_read(class_get_handle_by_id(DM_CLASS_ID), RS_BLOCK_ID, 0, 0, file_size, p_data_buffer);
    OS_PRINTF("file_size %d p_data_buffer %x \n",file_size, p_data_buffer);

    if((p_data_buffer[0] == 0x5d) && (p_data_buffer[1] == 0x00) &&
     (p_data_buffer[2] == 0x00)  && (p_data_buffer[3] == 0x80))
    {
      init_fake_mem_lzma(p_zip_sys, 320 * KBYTES);
      out_size = rsc_buf_size;
#ifdef WIN32
      p_addr = malloc(out_size);
#endif
      ret = (u8)lzma_decompress((void *)p_addr, &out_size, p_data_buffer, file_size);
      MT_ASSERT(ret == 0);
    }
    else
    {
      out_size = rsc_buf_size;
      memcpy((void *)p_addr,(void *)p_data_buffer,file_size);
      OS_PRINTF("res not compress , copy to gui address size :0x%x\n",file_size);
    }

#ifndef WIN32
    hal_dcache_flush((void *)p_addr, out_size);
#endif
#if 0
    dump_rsc_to_usb((void *)p_addr, file_size);
#endif
    g_rsc_config.rsc_data_addr = p_addr;
    g_rsc_config.flash_base = get_flash_addr();
    
    OS_PRINTF("before rsc_init\n");
    rsc_handle = rsc_init(&g_rsc_config);
    OS_PRINTF("rsc_init is ok\n");

    // get sys status
    p_systatus = sys_status_get();
    sys_status_get_osd_set(&osd_set);
    rsc_set_curn_language(rsc_handle, p_systatus->lang_set.osd_text);

    OS_PRINTF("paint init.\n");
    paint_param.max_str_len = MAX_PAINT_STRING_LENGTH;
    paint_param.max_str_lines = MAX_PAINT_STRING_LINES;
    paint_param.rsc_handle = rsc_handle;
    paint_param.h_pango = h_pango;
  
    gui_paint_init(&paint_param);

    #ifdef ALL_64MHD
    vf_ft_cfg.max_cnt = 1; //a~z, 0~9
    #else
    vf_ft_cfg.max_cnt = 36; //a~z, 0~9
    #endif
    
    #ifdef ENABLE_NETWORK  
    vf_ft_cfg.max_height = 72;
    vf_ft_cfg.max_width = 72;
    #else 
    vf_ft_cfg.max_height = 56;
    vf_ft_cfg.max_width = 56;
    #endif
    vf_ft_cfg.is_alpha_spt = TRUE;
    vf_ft_cfg.bpp = 32;

    OS_PRINTF("vf init.\n");
    vf_ft_attach(rsc_handle, &vf_ft_cfg);
  }
  
}
//lint +e438 +e550 +e830
#endif

static void ui_upgrade_start_burn(void)
{
  cmd_t cmd = {0};
  control_t *p_start,*p_bar,*p_sts = NULL;
  u16 uni_str[STS_STR_MAX_LEN] = {0};

  ui_usb_upg_sts = UI_USB_UPG_BURNING;

#ifdef RESOURCE_ON_FLASH
  reconfigrsc();
#endif

  _pre_burn(_get_selected_upg_mode());

  if (_get_selected_upg_mode() == UPG_APP_ALL)
  {
    ui_upgrade_start_burn_curn_grp();
  }
  else
  {
    cmd.id = USB_UPG_CMD_START_BURN;
    cmd.data1 = (u32)(&usb_param);
    ap_frm_do_command(APP_USB_UPG, &cmd);
  }
  
  gui_get_string(IDS_BURN_FLASH, uni_str, STS_STR_MAX_LEN);
  p_sts = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_STS);
  p_start = ctrl_get_child_by_id(ctrl_get_parent(p_sts), IDC_START);
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  ui_comm_ctrl_update_attr(p_start,FALSE);
  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  ctrl_paint_ctrl(p_bar, TRUE);
  ui_upgrade_by_usb_sts_show(p_sts, uni_str);
}

static BOOL _get_dm_block_head_info_from_upg_buf(u8 block_id, u32 dm_head_start, dmh_block_info_t *dest, u32 *block_head_offset)
{
  u32 blk_num = 0;
  u32 block_addr = 0;
  u32 p_upg_buf = usb_param.ext_buf_addr;
  u32 i = 0;
  u8 buf_block_id = 0;

  block_addr = p_upg_buf + dm_head_start + USB_UPG_DMH_INDC_SIZE + 16;
  blk_num = *(u16*)(p_upg_buf + dm_head_start + USB_UPG_DMH_INDC_SIZE + 12);
  for (i = 0; i < blk_num; i++) 
  {
    buf_block_id = *((u8 *)(block_addr));
    if (buf_block_id == block_id)
    {
      memcpy(dest, (u8 *)(block_addr), sizeof(dmh_block_info_t));
      *block_head_offset = block_addr - p_upg_buf;
      return TRUE;
    }
    
    block_addr += sizeof(dmh_block_info_t);
  }
  return FALSE;
}

static BOOL _get_dm_block_offset_from_upg_buf(u8 block_id, u32 *block_offset, u32 *block_head_offset)
{
  u32 i = 0;
  u32 dm_head_start = 0;
  const u32 sec_size = 64*1024;
  u32 p_upg_buf = usb_param.ext_buf_addr;
  dmh_block_info_t dest;
  u32 head_offset;

  for (i = 0; i < usb_param.ext_buf_size; i += sec_size)
  {
    if(memcmp((u8*)p_upg_buf, g_userdb_head_data, USB_UPG_DMH_INDC_SIZE) == 0)
    {
      dm_head_start = p_upg_buf - usb_param.ext_buf_addr;

      if(_get_dm_block_head_info_from_upg_buf(block_id, dm_head_start, &dest, &head_offset))
      {
        *block_offset = dest.base_addr + dm_head_start;
        *block_head_offset = head_offset;

        return TRUE;
      }
    }
    p_upg_buf += sec_size;
  }

  return FALSE;
}

static void _keep_serial_num()
{
  u8 sn[64];
  u32 indentity_block_offset;
  u32 indentity_block_head_offset;
  dmh_block_info_t *p_block_info;

  memset(sn, 0, sizeof(sn));
  if(sys_get_serial_num(sn, sizeof(sn)))
  {
    if(_get_dm_block_offset_from_upg_buf(IDENTITY_BLOCK_ID, &indentity_block_offset, &indentity_block_head_offset))
    {
      p_block_info = (dmh_block_info_t *)(usb_param.ext_buf_addr + indentity_block_head_offset);
      if(strlen((char*)sn) > p_block_info->size)
      {
		return;
      }
      memcpy((u8 *)(usb_param.ext_buf_addr + indentity_block_offset), sn, strlen((char*)sn));
      memset((u8 *)(usb_param.ext_buf_addr + indentity_block_offset + strlen((char*)sn)), 0xFF, p_block_info->size - strlen((char*)sn));
    }
  }
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

  file = vfs_open(usb_param.file, VFS_READ);
  if(NULL == file)
  {
    on_upg_fail(NULL, 0, 0, 0);
    return;
  }
  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  total_size = usb_param.ext_buf_size;
  off_set = (usb_param.mode == USB_UPGRD_MAINCODE) ? usb_param.flash_offset_addr : 0;
  vfs_seek(file, off_set, VFS_SEEK_SET);

  while(downloaded_size < total_size)
  {
    p_buf = (u8 *)(usb_param.ext_buf_addr + downloaded_size);
    size = ((downloaded_size + USB_UPG_TRANS_PACKET_SIZE) <= total_size)? USB_UPG_TRANS_PACKET_SIZE\
              : (total_size - downloaded_size);
    if(size != vfs_read(p_buf, size, 1, file))
    {
      on_upg_fail(NULL, 0, 0, 0);
      return;
    }
    downloaded_size += size;
    percent = (u16)((downloaded_size * 100) / total_size);

    //update ui
    ui_comm_bar_update(p_pbar, percent, TRUE);
    ui_set_front_panel_by_str_with_upg("D", (u8)percent);
    ui_comm_bar_paint(p_pbar, TRUE);
  }
  vfs_close(file);

  if(usb_param.mode == USB_UPGRD_ALL)
  {
    _keep_serial_num();
  }

  ui_comm_ask_for_dodlg_open(&g_upgrade_dlg_rc, IDS_SURE_BURN_FLASH,\
                                                      ui_upgrade_start_burn, ui_upgrade_giveup, 0);
  return ;
}

static RET_CODE version_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u8 local_changeset[MAX_CHANGESET_LEN];
  u8 asc_buf[64];
  u16 i, focus = 0, str_id[VERSION_LIST_CNT -1] = {IDS_MAINCODE};
  upg_file_ver_t upg_file_version = {{0},{0},{0}, 0};
  control_t *p_ctrl = NULL;
  u16 content[32];

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

  if(0 != g_list.file_count)
  {
    ui_get_upg_file_version(g_list.p_file[focus].name, &upg_file_version);
    upg_file_version.changeset[19] = '\0';
  }

  memset(local_changeset, 0, sizeof(local_changeset));
  dm_read(class_get_handle_by_id(DM_CLASS_ID), SS_DATA_BLOCK_ID, 0, 0, MAX_CHANGESET_LEN, local_changeset);
  
  memset(content, 0, sizeof(content));
  gui_get_string(IDS_USB_LOCAL_VERSION, content, 32);
  list_set_field_content_by_unistr(p_list, 0, 1, content);
  memset(content, 0, sizeof(content));
  gui_get_string(IDS_USB_BIN_VERSION, content, 32);
  list_set_field_content_by_unistr(p_list, 0, 2, content);

  memset(asc_buf, 0, sizeof(asc_buf));
  sprintf((char *)asc_buf, "%lx%s", ui_ota_api_get_upg_check_version(), local_changeset);
  list_set_field_content_by_ascstr(p_list, 1, 1, asc_buf);

  memset(asc_buf, 0, sizeof(asc_buf));
  sprintf((char *)asc_buf, "%lx%s", upg_file_version.sw_version, upg_file_version.changeset);
  list_set_field_content_by_ascstr(p_list, 1, 2, asc_buf);

  return SUCCESS;
}

static RET_CODE upgrade_file_update(control_t *p_ctrl, u16 focus,
                                       u16 *p_str, u16 max_length)
{
  u16 uni_str[32] = {0};

  if(0 == g_list.file_count)
  {
    str_nasc2uni((u8 *)"No file", uni_str, DB_DVBS_MAX_NAME_LENGTH);
  }
  else
  {
      if(focus >= g_list.file_count)
      {
          return ERR_FAILURE;
      }

      uni_strncpy(uni_str, g_list.p_file[focus].p_name, max_length - 1);
    }

  uni_strncpy(p_str, uni_str, max_length-1);

  return SUCCESS;
}

static u16 preopen_upgrade_by_usb(u32 para1, u32 para2)
{
  u16 FileCount = 0;

  p_partition = NULL;
  g_partition_cnt = 0;
  g_partition_cnt = file_list_get_partition(&p_partition);
  if(g_partition_cnt > 0)
  {
    flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, p_partition[0].letter);
    if(flist_dir != NULL)
    {
      ui_upg_file_list_get(flist_dir, FLIST_UNIT_FIRST, &g_list, (u8)para1);
      FileCount = (u16)g_list.file_count;
    }
    else
    {
       OS_PRINTF("\n preopen_upgrade_by_usb: enter dir failure \n");
    }
  }
  else
  {
    memset(&g_list, 0, sizeof(file_list_t));
  }
  /* set flag */
  is_finished_DS= FALSE;
  return FileCount;
}

RET_CODE open_upgrade_by_usb(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl[IDC_ITEM_MAX],*p_version;
  u8 i, j;
  u16 file_conut = 0;
  u16 stxt [IDC_ITEM_MAX] = {0,IDS_UPGRADE_MODE, IDS_UPGRADE_FILE, IDS_START };
  u16 smode[UPGRADE_BY_USB_MODE_CNT] ={IDS_UPGRADE_ALL_CODE, IDS_APP_ALL, IDS_MAINCODE};
  u16 y;
 #ifdef ENABLE_PVR_REC_CONFIG
  if(ui_recorder_isrecording())
  {
    return SUCCESS;
  }
 #endif
  str_asc2uni((u8 *)"|bin|BIN|nodir", g_ffilter_all);

  file_conut = preopen_upgrade_by_usb(UPG_APP_ALL, 0);

  p_cont =
    ui_comm_root_create(ROOT_ID_UPGRADE_BY_USB, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, IM_TITLEICON_TV, IDS_UPGRADE_BY_USB);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, upgrade_by_usb_cont_keymap);
  ctrl_set_proc(p_cont, upgrade_by_usb_cont_proc);

  y = UPGRADE_BY_USB_ITEM_Y;
  for (i = 1; i < IDC_ITEM_MAX; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case IDC_MODE://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_USB_ITEM_X, y,
                          UPGRADE_BY_USB_ITEM_LW, UPGRADE_BY_USB_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           UPGRADE_BY_USB_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < UPGRADE_BY_USB_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 1);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_usb_mode_proc);
        break;
      case IDC_FILE://file
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_USB_ITEM_X, y,
                      UPGRADE_BY_USB_ITEM_LW, UPGRADE_BY_USB_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,
                    file_conut, CBOX_ITEM_STRTYPE_UNICODE, upgrade_file_update);
        //ui_comm_select_right_set_mrect(p_ctrl[i], UPGRADE_BY_USB_SELECT_MIDX, 0, UPGRADE_BY_USB_SELECT_MIDY, COMM_CTRL_H);
        ui_comm_select_set_focus(p_ctrl[i], 0);
        if(0 == file_conut)
        {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_usb_file_proc);
        break;

      case IDC_START://start
        p_ctrl[i] = ui_comm_static_create(p_cont, i, UPGRADE_BY_USB_ITEM_X, y,
                          UPGRADE_BY_USB_ITEM_LW, UPGRADE_BY_USB_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_usb_start_text_proc);
        if(0 == file_conut)
        {
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        y += UPGRADE_BY_USB_ITEM_H + UPGRADE_BY_USB_ITEM_V_GAP;
        break;
      case IDC_PBAR://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, i, UPGRADE_BY_USB_PBAR_X,
                         (y - 15),
                          UPGRADE_BY_USB_PBAR_W, UPGRADE_BY_USB_PBAR_H,
                          UPGRADE_BY_USB_TXT_X, (y - 18), UPGRADE_BY_USB_TXT_W, UPGRADE_BY_USB_TXT_H,
                          UPGRADE_BY_USB_PER_X, (y - 18), UPGRADE_BY_USB_PER_W, UPGRADE_BY_USB_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_UPGRADE_BY_USB_PBAR_BG, RSI_UPGRADE_BY_USB_PBAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case IDC_STS://upgrade_by_usb status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT,IDC_STS, UPGRADE_BY_USB_ITEM_X, (y - 20),
                          (UPGRADE_BY_USB_ITEM_LW + UPGRADE_BY_USB_ITEM_RW),
                          UPGRADE_BY_USB_ITEM_H,
                          p_cont, 0);

    ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
    text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
    text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_ctrl[i], (u8 *)" ");
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
      ctrl_set_mrect(p_version, 0, 0, 700, 80);
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

    y += UPGRADE_BY_USB_ITEM_H + UPGRADE_BY_USB_ITEM_V_GAP;
  }

  ctrl_set_related_id(p_ctrl[IDC_MODE], 0, IDC_START, 0, IDC_FILE);
  ctrl_set_related_id(p_ctrl[IDC_FILE], 0, IDC_MODE, 0, IDC_START);
  ctrl_set_related_id(p_ctrl[IDC_START], 0, IDC_FILE, 0, IDC_MODE);

  //ui_comm_help_create(&usb_upgrade_help_data, p_cont);

  ctrl_default_proc(p_ctrl[IDC_MODE], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  ui_init_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB, ui_usb_upg_evtmap);
  return SUCCESS;
}

static RET_CODE on_upg_by_usb_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }

  if(UI_USB_UPG_IDEL != ui_usb_upg_sts)
  {
    return SUCCESS;
  }
  ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);
  return ERR_NOFEATURE;

}
static RET_CODE on_upg_by_usb_power_off(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }

  if(UI_USB_UPG_IDEL != ui_usb_upg_sts)
  {
    return SUCCESS;
  }
  ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);

  return ERR_NOFEATURE;
}
static RET_CODE on_upg_by_usb_quit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  ui_usb_upg_sts = UI_USB_UPG_IDEL;
  ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);

  if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }

  if (para1 == TRUE)
  {
    ui_close_all_mennus();
  }
  else
  {
    manage_close_menu(ROOT_ID_UPGRADE_BY_USB, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE on_upg_by_usb_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_sts;
  u16 uni_str[STS_STR_MAX_LEN] = {0};

  ui_usb_upg_sts = UI_USB_UPG_IDEL;

  p_sts = ctrl_get_child_by_id(p_ctrl, IDC_STS);
  str_nasc2uni((u8 *)"upgrade ok!", uni_str, STS_STR_MAX_LEN);
  ui_upgrade_by_usb_sts_show(p_sts, uni_str);

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

  ui_usb_upg_sts = UI_USB_UPG_IDEL;
  ui_set_front_panel_by_str_with_upg("ERR", 0);
  ui_comm_dlg_open(&p_data);
  mtos_task_delay_ms(1000);
  ui_comm_dlg_close();

  if(!ui_get_usb_status())
  {
    ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);
    manage_close_menu(ROOT_ID_UPGRADE_BY_USB, 0, 0);
    ui_set_front_panel_by_str("----");
    return SUCCESS;
  }
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  p_start = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_START);

  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_bar, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  return SUCCESS;
}
static RET_CODE on_upg_by_usb_update_sts(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  upg_status_t *sts = (upg_status_t *)para1;
  control_t *p_pbar;
  comm_dlg_data_t p_data =
    {
        ROOT_ID_INVALID,
      DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
      COMM_DLG_X, COMM_DLG_Y,
      COMM_DLG_W+ 40,COMM_DLG_H,
      IDS_USB_UPG_RESTART,
      0 ,
    };

  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_STS);

  if(UI_USB_UPG_IDEL != ui_usb_upg_sts)
  {
    switch((usb_upg_sm_t)(sts->sm))
    {
      case USB_SM_REMOTE_ERROR:
        break;
      case USB_UPG_SM_BURNING:
        if (_get_selected_upg_mode() == UPG_APP_ALL)
        {
          u16 progress = (u16)(sts->progress*(usb_grps.curn_burn_grp+1)/usb_grps.grp_cnt);

          ui_comm_bar_update(p_pbar, progress, TRUE);
          ui_set_front_panel_by_str_with_upg("B",(u8)progress);
          ui_comm_bar_paint(p_pbar, TRUE);
          if(100 == sts->progress)
          {
              //start burn next grp
              usb_grps.curn_burn_grp++;
              if (usb_grps.curn_burn_grp < usb_grps.grp_cnt)
              {
                ui_upgrade_start_burn_curn_grp();
                break;
              }
              else
              {
                OS_PRINTF("Burning all groups done!\n");
              }
          }
          else
          {
            break;
          }
        }
        ui_comm_bar_update(p_pbar, (u16)sts->progress, TRUE);
        ui_set_front_panel_by_str_with_upg("B",(u8)sts->progress);
        ui_comm_bar_paint(p_pbar, TRUE);
        if(100 == sts->progress)
        {
            ui_comm_dlg_open(&p_data);
            mtos_task_delay_ms(2000);
            #ifndef WIN32
            hal_pm_reset();
            #endif
        }
        break;
      default:
        return ERR_FAILURE;
    }
  }

  return SUCCESS;
}

static u8 _get_selected_upg_mode(void)
{
  control_t *p_mode = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_MODE);

  return (u8)ui_comm_select_get_focus(p_mode);
}

static void _update_block_crc(u8 *p_buf, u8 blk_id)
{
  u8 i;
  u32 bh;
  ui_usb_dm_base_info_t *base_info;
  dmh_block_info_t *dmh;
  
  base_info = (ui_usb_dm_base_info_t*)(p_buf + HEADER_OFFSET);
  for(i = 0;  i < base_info->block_num; i++)
  {
    bh = HEADER_OFFSET + HEADER_SIZE + i * sizeof(dmh_block_info_t);
    dmh = (dmh_block_info_t*)(p_buf + bh);

    if(dmh->id == blk_id && dmh->crc != 0x4352434e)
    {
      //update crc
      dmh->crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF, 
                                   p_buf + dmh->base_addr, (int)(dmh->size));
    }
  }
}

static void _pre_burn(u8 mode)
{
  if(UPG_MAIN_CODE == mode)
  {
    //update crc in dmh for maincode block
    _update_block_crc((u8*)usb_param.ext_buf_addr, MAIN_CODE_BLOCK_ID);
  }
  else if (UPG_APP_ALL == mode)
  {
    u8 i;
    u8 jump_blk_ids[] = {CADATA1_BLOCK_ID, CADATA_BLOCK_ID};//NOTE: keep the order in flash.cfg
    u8 jump_blk_cnt = (u8)(sizeof(jump_blk_ids)/sizeof(jump_blk_ids[0]));
    handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    dmh_block_info_t dmh;
    
    //seperate blocks to several groups to jump certain block
    usb_grps.curn_burn_grp = 0;
    usb_grps.grp_cnt = 1;
    usb_grps.grps[usb_grps.grp_cnt-1].grp_addr = usb_param.ext_buf_addr;
    for (i=0; i<jump_blk_cnt; i++)
    {
      if (dm_get_block_header(dm_handle, jump_blk_ids[i], &dmh) == DM_SUC)
      {
        usb_grps.grps[usb_grps.grp_cnt-1].grp_size = dmh.base_addr - 
                                                     (usb_grps.grps[usb_grps.grp_cnt-1].grp_addr -
                                                     usb_param.ext_buf_addr);
        if (usb_grps.grps[usb_grps.grp_cnt-1].grp_size == 0)
        {
          //maybe two consecutive blocks jumped .
          usb_grps.grp_cnt--;
        }
        
        if (usb_grps.grp_cnt >= USB_UPG_MAX_GRP_CNT)
        {
          OS_PRINTF("Too many blocks to jump!!\n");
          //the grp_size will be re-calculated.
          break;
        }

        if ((dmh.base_addr%USB_UPG_BURN_PACKET_SIZE) != 0 ||
            (dmh.size%USB_UPG_BURN_PACKET_SIZE) != 0)
        {
          OS_PRINTF("USB UPG: cannot jump block '%s', addr or size not 64K aligned!\n", dmh.name);
          continue;
        }

        OS_PRINTF("USB UPG: jump block '%s'\n", dmh.name);

        //update crc in dmh for this block
        if (dmh.crc != 0x4352434e)
          _update_block_crc((u8*)usb_param.ext_buf_addr, jump_blk_ids[i]);
        
        //start next group
        usb_grps.grp_cnt++;
        usb_grps.grps[usb_grps.grp_cnt-1].grp_addr = usb_param.ext_buf_addr + 
                                                     dmh.base_addr + 
                                                     dmh.size;
      }
    }
    //the last grp size, may be also the first(no block jumped)
    usb_grps.grps[usb_grps.grp_cnt-1].grp_size = usb_param.ext_buf_addr +
                                                 usb_param.ext_buf_size - 
                                                 usb_grps.grps[usb_grps.grp_cnt-1].grp_addr;

    if (usb_grps.grps[usb_grps.grp_cnt-1].grp_size == 0)
    {
      //this only happens when the last block jumped and it reaches the flash end.
      usb_grps.grp_cnt--;
    }
  }
  else if (USB_UPGRD_CONSTANT_CW == mode)
  {
    return;
  }
  return;
}

static RET_CODE on_upg_by_usb_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mode, *p_cont, *p_start, *p_file;
  u16 mode_focus;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
#ifdef CAS_CONFIG_CDCAS
  extern void check_watch_limit();
  check_watch_limit();
  ui_release_ca();
#endif


  memset(&usb_param, 0x0, sizeof(usb_upg_start_params_t));

  if(UI_USB_UPG_IDEL == ui_usb_upg_sts)
  {
    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
    p_file = ctrl_get_child_by_id(p_cont, IDC_FILE);

    mode_focus = ui_comm_select_get_focus(p_mode);

    switch(mode_focus)
    {
      case UPG_ALL_CODE:
      usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
      usb_param.ext_buf_size = CHARSTO_SIZE;
      usb_param.head_size = 0;
      usb_param.burn_flash_size = CHARSTO_SIZE;
      memset(usb_param.head_data, 0 ,sizeof(usb_param.head_data));
      break;
      case UPG_USER_DATA:
      usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
      usb_param.head_size = USB_UPG_MAX_HEAD_SIZE;
      memcpy(usb_param.head_data, g_userdb_head_data, usb_param.head_size);
      usb_param.ext_buf_size = dm_get_block_size(dm_handle, IW_TABLE_BLOCK_ID)+\
      dm_get_block_size(dm_handle, IW_VIEW_BLOCK_ID) + usb_param.head_size;
      usb_param.flash_offset_addr = dm_get_block_addr(dm_handle, IW_TABLE_BLOCK_ID) \
      - get_flash_addr();
      usb_param.burn_flash_size = usb_param.ext_buf_size - usb_param.head_size;
      break;
      case UPG_MAIN_CODE:
      usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
      usb_param.ext_buf_size = dm_get_block_addr(dm_handle, MAIN_CODE_BLOCK_ID)- 
                               get_flash_addr() -
                               get_maincode_off_addr();
      usb_param.ext_buf_size += dm_get_block_size(dm_handle, MAIN_CODE_BLOCK_ID);
      usb_param.head_size = 0;
      memset(usb_param.head_data, 0 ,sizeof(usb_param.head_data));
      usb_param.flash_offset_addr = get_maincode_off_addr();
      usb_param.burn_flash_size = usb_param.ext_buf_size;
      break;
      case UPG_APP_ALL:
      usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
      usb_param.ext_buf_size = CHARSTO_SIZE - get_maincode_off_addr();
      usb_param.head_size = 0;
      memset(usb_param.head_data, 0 ,sizeof(usb_param.head_data));
      usb_param.flash_offset_addr = get_maincode_off_addr();
      usb_param.burn_flash_size = usb_param.ext_buf_size;
      break;
      default:
      MT_ASSERT(0);
      break;
    }
    mem_mgr_release_block(BLOCK_AV_BUFFER);
    memcpy(usb_param.file, g_list.p_file[ui_comm_select_get_focus(p_file)].name, MAX_FILE_PATH * sizeof(u16));

    usb_param.mode = (u8)ui_comm_select_get_focus(p_mode);
    switch (usb_param.mode)
    {
      case UPG_ALL_CODE:
        usb_param.mode = USB_UPGRD_ALL;
        break;
      case UPG_APP_ALL:
        usb_param.mode = USB_UPGRD_MAINCODE; //no use
        break;
      case UPG_MAIN_CODE:
        usb_param.mode = USB_UPGRD_MAINCODE;
        break;
      case UPG_USER_DATA:
        usb_param.mode = USB_UPGRD_USER_DB;
        break;
      case UPG_CA_KEY:
        usb_param.mode = USB_UPGRD_CONSTANT_CW;
        break;
      default:
        break;
    }
    ui_usb_upg_sts = UI_USB_UPG_LOADING;

    ui_comm_ctrl_update_attr(p_mode, FALSE);

    ui_comm_ctrl_update_attr(p_file, FALSE);

    p_start = ctrl_get_child_by_id(p_cont, IDC_START);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ctrl_paint_ctrl(p_cont, TRUE);
    _ui_upgrade_start_download();
  }
  return SUCCESS;
}

static RET_CODE on_upg_by_usb_file_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_version;

  cbox_class_proc(p_ctrl, msg, 0, 0);
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
  version_list_update(ctrl_get_child_by_id(p_version, 1), 1, VERSION_LIST_CNT, 0);
  ctrl_paint_ctrl(p_version, TRUE);
  return SUCCESS;
}

static RET_CODE on_upg_by_usb_mode_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_version = NULL;
  control_t *p_file = NULL;
  control_t *p_start = NULL;

  u16 focus;

  cbox_class_proc(p_ctrl, msg, 0, 0);

  focus = cbox_static_get_focus(p_ctrl);

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

  //update version
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
  ctrl_set_sts(ctrl_get_child_by_id(p_version,1), 
              (focus == UPG_ALL_CODE || focus == UPG_APP_ALL) ? 
              OBJ_STS_SHOW : OBJ_STS_HIDE);
  ctrl_paint_ctrl(p_version, TRUE);

  return SUCCESS;
}

static RET_CODE on_upg_by_usb_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  if(flist_dir != NULL)
   {
      file_list_leave_dir(flist_dir);
      flist_dir = NULL;
   }
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(upgrade_by_usb_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(upgrade_by_usb_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(upgrade_by_usb_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_upg_by_usb_exit)
  ON_COMMAND(MSG_EXIT, on_upg_by_usb_exit)
  ON_COMMAND(MSG_POWER_OFF, on_upg_by_usb_power_off)
  ON_COMMAND(MSG_UPG_QUIT, on_upg_by_usb_quit)
  ON_COMMAND(MSG_UPG_OK, on_upg_by_usb_ok)
  ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_upg_by_usb_update_sts)
  ON_COMMAND(MSG_UPG_FAIL, on_upg_fail)
  ON_COMMAND(MSG_DESTROY, on_upg_by_usb_destory)
END_MSGPROC(upgrade_by_usb_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(upgrade_by_usb_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_upg_by_usb_mode_change_focus)
  ON_COMMAND(MSG_DECREASE, on_upg_by_usb_mode_change_focus)
END_MSGPROC(upgrade_by_usb_mode_proc, cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_usb_file_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_upg_by_usb_file_change_focus)
  ON_COMMAND(MSG_DECREASE, on_upg_by_usb_file_change_focus)
END_MSGPROC(upgrade_by_usb_file_proc,cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_usb_start_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_upg_by_usb_start)
END_MSGPROC(upgrade_by_usb_start_text_proc, text_class_proc)

BEGIN_AP_EVTMAP(ui_usb_upg_evtmap)
  CONVERT_EVENT(USB_UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(USB_UPG_EVT_QUIT_UPG, MSG_UPG_QUIT)
  CONVERT_EVENT(USB_UPG_EVT_SUCCESS, MSG_UPG_OK)
  CONVERT_EVENT(USB_DOWNLOAD_FILE_FAIL, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_OEPN_UPG_FILE_FAILED, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_MALLOC_MEMORY_FAILED, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_INVLID_UPGRADE_FILE, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_UPG_BURN_FLASH_ERROR, MSG_UPG_FAIL)
END_AP_EVTMAP(ui_usb_upg_evtmap)
